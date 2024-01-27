/*
 This source is part of the libosmscout library
 Copyright (C) 2024 Lukas Karas

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

#include <osmscout/private/Config.h>

#include <osmscout/async/ReadWriteLock.h>

#include <osmscout/log/Logger.h>

#include <cassert>

namespace osmscout {

/**
 * X flag is defined as the following:
 * 0 : X is released
 * 1 : X is held, but waits for release of S
 * 2 : X is held
 * 3 : X was released and left available for one of request in wait
 */

void Latch::lock() {
  std::thread::id tid = std::this_thread::get_id();

  spin_lock();

  if (x_owner != tid) {
    /* increments the count of request in wait */
    ++x_wait;
    for (;;) {
      /* if flag is 0 or 3 then it hold X with no wait,
       * in other case it have to wait for X gate
       */
      if (x_flag == 0) {
        ++x_flag;
        --x_wait;
        break;
      } else if (x_flag == 3) {
        x_flag = 1;
        --x_wait;
        break;
      } else {
        /* !!! pop gate first then unlock spin (reverse order for S request) */
        std::unique_lock<std::mutex> lk(x_gate_lock);
        spin_unlock();
        x_gate.wait(lk);
      }
      spin_lock();
    }

    /* X = 1, check the releasing of S */
    for (;;) {
      /* if the count of S is zeroed then it finalize with no wait,
       * in other case it have to wait for S gate */
      if (s_count == 0) {
        ++x_flag;
        break;
      } else {
        /* !!! pop gate first then unlock spin (reverse order for notifier),
         * the lock can be held here by itself, or by the thread is releasing
         * the last S */
        std::unique_lock<std::mutex> lk(s_gate_lock);
        spin_unlock();
        s_gate.wait(lk);
      }
      spin_lock();
    }

    /* X = 2, set owner */
    x_owner = tid;
  }

  spin_unlock();
}

void Latch::unlock() {
  spin_lock();
  if (x_owner == std::this_thread::get_id()) {
    x_owner = std::thread::id();
    /* if X priority is enabled then hand-over to a request in wait for X */
    x_flag = (px && x_wait > 0 ? 3 : 0);
    spin_unlock();
    x_gate.notify_all();
  } else {
    spin_unlock();
  }
}

void Latch::lock_shared() {
  spin_lock();
  if (x_owner != std::this_thread::get_id()) {
    /* if flag is 0 or 1 then it hold S with no wait,
     * in other case it have to wait for X gate
     */
    for (;;) {
      if (x_flag.load() < 2) {
        break;
      } else {
        /* !!! unlock spin first then pop gate (reverse order for X request)*/
        spin_unlock();
        std::unique_lock<std::mutex> lk(x_gate_lock);
        x_gate.wait(lk);
      }
      spin_lock();
    }
  }
  ++s_count;
  spin_unlock();
}

void Latch::unlock_shared() {
  spin_lock();
  /* on last S, it notifies the thread in wait (X = 1) */
  if (--s_count == 0 && x_flag == 1 &&
      x_owner != std::this_thread::get_id()) {
    /* !!! unlock spin first then pop gate (reverse order for receiver) */
    spin_unlock();
    std::unique_lock<std::mutex> lk(s_gate_lock);
    s_gate.notify_one();
  } else {
    spin_unlock();
  }
}

}
