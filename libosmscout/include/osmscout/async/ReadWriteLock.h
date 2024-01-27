#ifndef LIBOSMSCOUT_READWRITELOCK_H
#define LIBOSMSCOUT_READWRITELOCK_H

/*
 This source is part of the libosmscout library
 Copyright (C) 2024  Jean-Luc Barriere

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#include <osmscout/lib/CoreImportExport.h>

#include <osmscout/private/Config.h>

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace osmscout {

/**
 * This implements a pure C++ Latch providing lock-S (shared) and lock-X (exclusive).
 * The concept used here allows X requests to be prioritized faster and more smoothly
 * than standard implementations. It uses a no-lock strategy whenever possible and
 * reverts to lock and wait in race condition.
 */

class OSMSCOUT_API Latch {
private:
  mutable std::atomic_flag s_spin = ATOMIC_FLAG_INIT;

  volatile int x_wait = 0;              /* counts requests in wait for X  */
  volatile int s_count = 0;             /* counts held S locks            */
  std::atomic<int> x_flag = 0;          /* X status: 0, 1, 2, or 3        */
  std::thread::id x_owner;              /* X owner (thread id)            */

  std::mutex x_gate_lock;
  std::condition_variable x_gate;       /* wait for release of X          */
  std::mutex s_gate_lock;
  std::condition_variable s_gate;       /* wait for release of S          */

  bool px = true;                       /* prioritize requests for X      */

  void spin_lock() {
    while (s_spin.test_and_set(std::memory_order_acquire)) {
      std::this_thread::yield();
    }
  }
  void spin_unlock() {
    s_spin.clear(std::memory_order_release);
  }

public:
  Latch() = default;
  Latch(const Latch&) = delete;
  Latch(Latch&&) = delete;
  Latch& operator=(const Latch&) = delete;
  Latch& operator=(Latch&&) = delete;
  ~Latch() = default;

  void prefer_exclusive(bool yesno) { px = yesno; }

  void lock();
  void unlock();

  void lock_shared();
  void unlock_shared();
};

/*
 * Cannot use the template std::shared_lock as Latch does not implement all the
 * requirements of the standard
 */
class OSMSCOUT_API ReadLock
{
private:
  Latch *p = nullptr;
  bool owns = false;

  void swap(ReadLock& rl) noexcept {
    std::swap(p, rl.p);
    std::swap(owns, rl.owns);
  }

public:

  ReadLock() = default;

  explicit ReadLock(Latch& latch) : p(&latch), owns(true) { latch.lock_shared(); }

  ~ReadLock() {
    if (owns) {
      p->unlock_shared();
    }
  }

  ReadLock(ReadLock const&) = delete;
  ReadLock& operator=(ReadLock const&) = delete;

  ReadLock(ReadLock&& rl) noexcept { swap(rl); }

  ReadLock& operator=(ReadLock&& rl) noexcept {
    swap(rl);
    return *this;
  }

  bool owns_lock() const noexcept {
    return owns;
  }

  void lock() {
    if (!owns && p != nullptr) {
      p->lock_shared();
      owns = true;
    }
  }

  void unlock() {
    if (owns) {
      owns = false;
      p->unlock_shared();
    }
  }
};

/*
 * Cannot use the template std::unique_lock as Latch does not implement all the
 * requirements of the standard
 */
class OSMSCOUT_API WriteLock
{
private:
  Latch *p = nullptr;
  bool owns = false;

  void swap(WriteLock& wl) noexcept {
    std::swap(p, wl.p);
    std::swap(owns, wl.owns);
  }

public:

  WriteLock() = default;

  explicit WriteLock(Latch& latch) : p(&latch), owns(true) { latch.lock(); }

  ~WriteLock() {
    if (owns) {
      p->unlock();
    }
  }

  WriteLock(WriteLock const&) = delete;
  WriteLock& operator=(WriteLock const&) = delete;

  WriteLock(WriteLock&& wl) noexcept { swap(wl); }

  WriteLock& operator=(WriteLock&& wl) noexcept {
    swap(wl);
    return *this;
  }

  bool owns_lock() const noexcept {
    return owns;
  }

  void lock() {
    if (!owns && p != nullptr) {
      p->lock();
      owns = true;
    }
  }

  void unlock() {
    if (owns) {
      owns = false;
      p->unlock();
    }
  }
};

}

#endif //LIBOSMSCOUT_READWRITELOCK_H
