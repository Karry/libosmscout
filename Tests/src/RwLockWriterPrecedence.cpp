#include <osmscout/async/ReadWriteLock.h>
#include <osmscout/util/ScopeGuard.h>

#include <TestMain.h>

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

