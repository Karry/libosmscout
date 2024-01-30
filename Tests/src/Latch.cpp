/*
  Latch - a test program for libosmscout
  Copyright (C) 2024  Jean-Luc Barriere

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

#include <osmscout/async/ReadWriteLock.h>
#include <osmscout/async/ProcessingQueue.h>
#include <osmscout/util/StopClock.h>

using namespace std::chrono_literals;

static size_t iterationCount=1000;
static auto   taskDuration=1ms;

static long refCounter = 0;
static osmscout::Latch latch;

class ReaderWorker
{
private:
  osmscout::ProcessingQueue<int>& queue;
  std::thread                     thread;

public:
  size_t                          processedCount;

private:
  void ProcessorLoop()
  {
    std::cout << "Processing..." << std::endl;

    while (true) {
      std::optional<int> value=queue.PopTask();

      if (!value) {
        std::cout << "Queue empty!" << std::endl;
        break;
      }

      if (value<0) {
        std::cout << "Stop signal fetched!" << std::endl;
        break;
      }

      {
        osmscout::ReadLock locker(latch);
        [[maybe_unused]] long c = refCounter;
        std::this_thread::sleep_for(taskDuration);
      }

      processedCount++;
    }

    std::cout << "Processing...done" << std::endl;
  }

public:
  explicit ReaderWorker(osmscout::ProcessingQueue<int>& queue)
  : queue(queue),
    thread(&ReaderWorker::ProcessorLoop,this),
    processedCount(0)
  {
  }

  void Wait() {
    thread.join();
  }
};

class WriterWorker
{
private:
  osmscout::ProcessingQueue<int>& queue;
  std::thread                     thread;

public:
  size_t                          processedCount;

private:
  void ProcessorLoop()
  {
    std::cout << "Processing..." << std::endl;

    while (true) {
      std::optional<int> value=queue.PopTask();

      if (!value) {
        std::cout << "Queue empty!" << std::endl;
        break;
      }

      if (value<0) {
        std::cout << "Stop signal fetched!" << std::endl;
        break;
      }

      {
        osmscout::WriteLock locker(latch);
        ++refCounter;
        std::this_thread::sleep_for(taskDuration);
      }

      processedCount++;
    }

    std::cout << "Processing...done" << std::endl;
  }

public:
  explicit WriterWorker(osmscout::ProcessingQueue<int>& queue)
      : queue(queue),
      thread(&WriterWorker::ProcessorLoop,this),
      processedCount(0)
  {
  }

  void Wait() {
    thread.join();
  }
};

class ReaderReaderWorker
{
private:
  osmscout::ProcessingQueue<int>& queue;
  std::thread                     thread;

public:
  size_t                          processedCount;

private:
  void ProcessorLoop()
  {
    std::cout << "Processing..." << std::endl;

    while (true) {
      std::optional<int> value=queue.PopTask();

      if (!value) {
        std::cout << "Queue empty!" << std::endl;
        break;
      }

      if (value<0) {
        std::cout << "Stop signal fetched!" << std::endl;
        break;
      }

      {
        osmscout::ReadLock rl1(latch);
        {
          osmscout::ReadLock rl2(latch);
          {
            osmscout::ReadLock rl3(latch);
            [[maybe_unused]] long c = refCounter;
            std::this_thread::sleep_for(taskDuration);
          }
        }
      }

      processedCount++;
    }

    std::cout << "Processing...done" << std::endl;
  }

public:
  explicit ReaderReaderWorker(osmscout::ProcessingQueue<int>& queue)
      : queue(queue),
      thread(&ReaderReaderWorker::ProcessorLoop,this),
      processedCount(0)
  {
  }

  void Wait() {
    thread.join();
  }
};

class WriterReaderWorker
{
private:
  osmscout::ProcessingQueue<int>& queue;
  std::thread                     thread;

public:
  size_t                          processedCount;

private:
  void ProcessorLoop()
  {
    std::cout << "Processing..." << std::endl;

    while (true) {
      std::optional<int> value=queue.PopTask();

      if (!value) {
        std::cout << "Queue empty!" << std::endl;
        break;
      }

      if (value<0) {
        std::cout << "Stop signal fetched!" << std::endl;
        break;
      }

      {
        osmscout::WriteLock wr1(latch);
        {
          osmscout::WriteLock wr2(latch);
          {
            osmscout::ReadLock rl1(latch);
            {
              osmscout::ReadLock rl2(latch);
              [[maybe_unused]] long c = refCounter;
            }
          }
          ++refCounter;
        }
        std::this_thread::sleep_for(taskDuration);
      }

      processedCount++;
    }

    std::cout << "Processing...done" << std::endl;
  }

public:
  explicit WriterReaderWorker(osmscout::ProcessingQueue<int>& queue)
      : queue(queue),
      thread(&WriterReaderWorker::ProcessorLoop,this),
      processedCount(0)
  {
  }

  void Wait() {
    thread.join();
  }
};

int main(int /*argc*/, char* /*argv*/[])
{
  std::cout << "Main thread id: #" << std::this_thread::get_id() << std::endl;
  std::cout << iterationCount << " iterations, every task takes " << taskDuration.count() << "ms" << std::endl;
  std::cout << std::endl;

  {
    std::cout << ">>> MultiReaderWorker..." << std::endl;

    osmscout::StopClock            stopClock;
    osmscout::ProcessingQueue<int> queue(1000);
    ReaderWorker                   worker1(queue);
    ReaderWorker                   worker2(queue);
    ReaderWorker                   worker3(queue);
    ReaderWorker                   worker4(queue);

    std::cout << "Pushing tasks..." << std::endl;

    for (size_t i=1; i<=iterationCount; i++) {
      queue.PushTask(i);
    }

    queue.PushTask(-1);

    queue.Stop();

    std::cout << "Pushing tasks...done, waiting..." << std::endl;

    worker1.Wait();
    worker2.Wait();
    worker3.Wait();
    worker4.Wait();

    stopClock.Stop();

    long pc = worker1.processedCount+worker2.processedCount+worker3.processedCount+
              worker4.processedCount;

    std::cout << "#processed: "
              << pc << " ["
              << worker1.processedCount << ","
              << worker2.processedCount << ","
              << worker3.processedCount << ","
              << worker4.processedCount << "]"
              << std::endl;
    std::cout << "<<< MultiReaderWorker...done: " << stopClock.ResultString() << std::endl;
    std::cout << std::endl;
  }

  {
    std::cout << ">>> MultiWriterWorker..." << std::endl;
    osmscout::StopClock                   stopClock;
    osmscout::ProcessingQueue<int>        queue(1000);
    WriterWorker                          worker1(queue);
    WriterWorker                          worker2(queue);
    WriterWorker                          worker3(queue);
    WriterWorker                          worker4(queue);

    std::cout << "Pushing tasks..." << std::endl;

    for (size_t i=1; i<=iterationCount; i++) {
      queue.PushTask(i);
    }

    queue.PushTask(-1);

    queue.Stop();

    worker1.Wait();
    worker2.Wait();
    worker3.Wait();
    worker4.Wait();

    std::cout << "Pushing tasks...done, waiting..." << std::endl;


    stopClock.Stop();

    long pc = worker1.processedCount+worker2.processedCount+worker3.processedCount+
              worker4.processedCount;

    std::cout << "#processed: (" << refCounter << ") "
              << pc << " ["
              << worker1.processedCount << ","
              << worker2.processedCount << ","
              << worker3.processedCount << ","
              << worker4.processedCount << "]"
              << std::endl;
    std::cout << "<<< MultiWriterWorker...done: " << stopClock.ResultString() << std::endl;
    std::cout << std::endl;

    if (refCounter != pc)
      return -1;
  }

  {
    std::cout << ">>> MultiReaderOneWriterWorker..." << std::endl;
    osmscout::StopClock                   stopClock;
    osmscout::ProcessingQueue<int>        queue(1000);
    WriterWorker                          worker1(queue);
    ReaderWorker                          worker2(queue);
    ReaderWorker                          worker3(queue);
    ReaderWorker                          worker4(queue);

    refCounter = 0; // reset counter

    std::cout << "Pushing tasks..." << std::endl;

    for (size_t i=1; i<=iterationCount; i++) {
      queue.PushTask(i);
    }

    queue.PushTask(-1);

    queue.Stop();

    worker1.Wait();
    worker2.Wait();
    worker3.Wait();
    worker4.Wait();

    std::cout << "Pushing tasks...done, waiting..." << std::endl;


    stopClock.Stop();

    long pc = worker1.processedCount+worker2.processedCount+worker3.processedCount+
              worker4.processedCount;

    std::cout << "#processed: (" << refCounter << ") "
              << pc << " ["
              << worker1.processedCount << ","
              << worker2.processedCount << ","
              << worker3.processedCount << ","
              << worker4.processedCount << "]"
              << std::endl;
    std::cout << "<<< MultiReaderOneWriterWorker...done: " << stopClock.ResultString() << std::endl;
    std::cout << std::endl;
  }

  {
    std::cout << ">>> MultiReentrantReaderWorker..." << std::endl;
    osmscout::StopClock                   stopClock;
    osmscout::ProcessingQueue<int>        queue(1000);
    ReaderReaderWorker                    worker1(queue);
    ReaderReaderWorker                    worker2(queue);
    ReaderReaderWorker                    worker3(queue);
    ReaderReaderWorker                    worker4(queue);

    std::cout << "Pushing tasks..." << std::endl;

    for (size_t i=1; i<=iterationCount; i++) {
      queue.PushTask(i);
    }

    queue.PushTask(-1);

    queue.Stop();

    worker1.Wait();
    worker2.Wait();
    worker3.Wait();
    worker4.Wait();

    std::cout << "Pushing tasks...done, waiting..." << std::endl;


    stopClock.Stop();

    long pc = worker1.processedCount+worker2.processedCount+worker3.processedCount+
              worker4.processedCount;

    std::cout << "#processed: "
              << pc << " ["
              << worker1.processedCount << ","
              << worker2.processedCount << ","
              << worker3.processedCount << ","
              << worker4.processedCount << "]"
              << std::endl;
    std::cout << "<<< MultiReentrantReaderWorker...done: " << stopClock.ResultString() << std::endl;
    std::cout << std::endl;
  }

  {
    std::cout << ">>> MultiReentrantWriterWorker..." << std::endl;
    osmscout::StopClock                   stopClock;
    osmscout::ProcessingQueue<int>        queue(1000);
    WriterReaderWorker                    worker1(queue);
    WriterReaderWorker                    worker2(queue);
    WriterReaderWorker                    worker3(queue);
    WriterReaderWorker                    worker4(queue);

    refCounter = 0; // reset counter

    std::cout << "Pushing tasks..." << std::endl;

    for (size_t i=1; i<=iterationCount; i++) {
      queue.PushTask(i);
    }

    queue.PushTask(-1);

    queue.Stop();

    worker1.Wait();
    worker2.Wait();
    worker3.Wait();
    worker4.Wait();

    std::cout << "Pushing tasks...done, waiting..." << std::endl;


    stopClock.Stop();

    long pc = worker1.processedCount+worker2.processedCount+worker3.processedCount+
              worker4.processedCount;

    std::cout << "#processed: (" << refCounter << ") "
              << pc << " ["
              << worker1.processedCount << ","
              << worker2.processedCount << ","
              << worker3.processedCount << ","
              << worker4.processedCount << "]"
              << std::endl;
    std::cout << "<<< MultiReentrantWriterWorker...done: " << stopClock.ResultString() << std::endl;
    std::cout << std::endl;

    if (refCounter != pc)
      return -1;
  }

  return 0;
}
