#include <gtest/gtest.h>

#include <worker/worker.h>
#include <chrono>

TEST(aWorker, WorkerMissingStart)
{
  ASSERT_NO_THROW({
    auto func = [] (worker*) {return;};
    worker w(func);
  });
}

TEST(aWorker, WorkerStartInitialization)
{
  auto func = [] (worker*) {return;};
  worker w(func);
  w.start(1);
  ASSERT_EQ(w.id(), 1);
  ASSERT_EQ(w.status(), worker::threadStatus::running);
}

TEST(aWorker, WorkerStartWhenNotReady)
{
  ASSERT_NO_THROW({
    auto func = [] (worker*) {return;};
    worker w(func);
    w.start(1);
  });
}

TEST(aWorker, WorkerPauseWithoutStart)
{
  ASSERT_ANY_THROW({
    auto func = [] (worker*) {return;};
    worker w(func);
    w.pause();
  });
}

TEST(aWorker, WorkerPauseWhenNotRunning)
{
  ASSERT_ANY_THROW({
    auto func = [] (worker*) {return;};
    worker w(func);
    w.start(1);
    w.stop();
    w.pause();
  });
}

worker* gworker = 0;
void gfunc(worker* w) {gworker = w;};
TEST(aWorker, WorkerTaskFunction)
{
  worker w(gfunc);
  w.start(1);
  w.join();
  ASSERT_TRUE(gworker);
}

int addval1 = 0;
void addfunc1(worker* w) {addval1 = 1;};
int addval2 = 0;
void addfunc2(worker* w) {addval2 = 2;};

TEST(aWorker, SingleWorkerTask)
{
  worker w(addfunc1);
  w.start(1);
  w.join();
  ASSERT_EQ(addval1,1);
}

TEST(aWorker, TwoWorkersTask)
{
  worker w1(addfunc1);
  w1.start(1);
  worker w2(addfunc2);
  w2.start(1);
  w1.join();
  w2.join();
  ASSERT_EQ(addval1,1);
  ASSERT_EQ(addval2,2);
}
