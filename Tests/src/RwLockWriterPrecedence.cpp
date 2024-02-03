#include <osmscout/async/ReadWriteLock.h>
#include <osmscout/util/ScopeGuard.h>

#include <TestMain.h>

#include <chrono>

TEST_CASE("Check write precedence") {
  osmscout::Latch latch;
  volatile int i=0;
  osmscout::ReadLock rl(latch);

  std::thread t([&latch, &i](){
    osmscout::WriteLock wl(latch);
    i++;
  });

  osmscout::ScopeGuard finalizer([&t]() noexcept {
    if (t.joinable()) {
      t.join();
    }
  });

  // wait until writer lock is requested
  while (true) {
    if (!latch.try_lock_shared()) {
      // writer lock is requested already
      break;
    }
    latch.unlock_shared();
    std::this_thread::yield();
  }

  REQUIRE(i == 0);
  rl.unlock();

  osmscout::ReadLock rl2(latch); // we should not get shared lock until writer is done
  REQUIRE(i == 1);
}

TEST_CASE("Second shared lock should be blocked when exclusive is requested") {
  osmscout::Latch latch;
  volatile int i=0;
  volatile int j=0;
  osmscout::ReadLock rl(latch);

  std::thread t([&latch, &i](){
    osmscout::WriteLock wl(latch);
    i++;
  });

  // wait until writer lock is requested
  while (true) {
    if (!latch.try_lock_shared()) {
      // writer lock is requested already
      break;
    }
    latch.unlock_shared();
    std::this_thread::yield();
  }

  std::thread t2([&latch, &j](){
    osmscout::ReadLock rl(latch);
    j++;
  });

  osmscout::ScopeGuard finalizer([&t, &t2]() noexcept {
    if (t.joinable()) {
      t.join();
    }
    if (t2.joinable()) {
      t2.join();
    }
  });

  using namespace std::chrono_literals;

  REQUIRE(i == 0); // write lock is still waiting
  std::this_thread::sleep_for(2s); // not nice, but is there better way howto check that t2 is blocked?
  REQUIRE(j == 0); // second read lock is blocked because exclusive lock is requested
  rl.unlock();
}
