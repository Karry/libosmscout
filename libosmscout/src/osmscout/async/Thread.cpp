/*
 This source is part of the libosmscout library
 Copyright (C) 2023 Lukas Karas

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
#include <osmscout/async/Thread.h>

#ifdef OSMSCOUT_PTHREAD_NAME
#include <type_traits>

#include <pthread.h>

static_assert(std::is_same<std::thread::native_handle_type, pthread_t>::value, "std::thread::native_handle_type have to be pthread_t");
#endif

namespace osmscout {

bool SetThreadName([[maybe_unused]] const std::string &name)
{
#ifdef OSMSCOUT_PTHREAD_NAME
  return pthread_setname_np(pthread_self(), name.c_str()) == 0;
#else
  return false;
#endif
}

#ifdef OSMSCOUT_PTHREAD_NAME
bool SetThreadName(std::thread &thread, const std::string &name)
{
    return pthread_setname_np(thread.native_handle(), name.c_str()) == 0;
}
#else
bool SetThreadName(std::thread &/*thread*/, const std::string &/*name*/)
{
    return false;
}
#endif

namespace {
  struct ThreadFinalizer
  {
    Signal<std::thread::id> threadExit;

    ThreadFinalizer() = default;
    ThreadFinalizer(const ThreadFinalizer &) = delete;
    ThreadFinalizer(ThreadFinalizer &&) = delete;

    // Note: intentionally non-virtual. ThreadFinalizer is never used
    // polymorphically (it is held by a std::unique_ptr of its exact type),
    // and a vtable pointer would end up in the thread-local initial image
    // (see the comment on GetThreadFinalizer() below).
    ~ThreadFinalizer()
    {
      threadExit.Emit(std::this_thread::get_id());
    }

    ThreadFinalizer &operator=(const ThreadFinalizer &) = delete;
    ThreadFinalizer &operator=(ThreadFinalizer &&) = delete;
  };

  // The per-thread finalizer is kept behind a thread_local *pointer* that is
  // dynamically initialised, instead of being a thread_local object with a
  // non-trivial initial value. This is deliberate and important:
  //
  // A thread_local object whose initial image contains non-zero data (here the
  // vtable pointers of ThreadFinalizer/Signal) is emitted into the module's
  // static TLS block (.tdata) and is memcpy'd by the C runtime into the TLS
  // area of *every* newly created thread -- even threads that never touch this
  // variable. On Sailfish OS / aarch64 the process' static TLS block overlaps
  // TLS slots that the Android GPU driver (loaded via libhybris) reserves for
  // itself (TLS_SLOT_OPENGL / TLS_SLOT_OPENGL_API). A non-zero TLS init image
  // then clobbers the GPU driver's context pointer and crashes the Mali driver
  // on the Qt scene-graph render thread.
  //
  // Keeping the initial image all-zero (only a null pointer lives in .tbss)
  // avoids the collision, and matches the behaviour of older toolchains that
  // placed the whole object in .tbss and constructed it lazily.
  // See https://github.com/Karry/osmscout-sailfish/issues/348
  ThreadFinalizer &GetThreadFinalizer()
  {
    thread_local std::unique_ptr<ThreadFinalizer> finalizer = std::make_unique<ThreadFinalizer>();
    return *finalizer;
  }
}

Signal<std::thread::id>& ThreadExitSignal()
{
  return GetThreadFinalizer().threadExit;
}
}
