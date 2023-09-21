#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <condition_variable>
using namespace std;

template <class jobType>
class thrdMgr;

template <class jobType>
class myThread {
 public:
    myThread(int id, thrdMgr<jobType> * m) : mId(id), mInit(false), mMgr(m), done(false) {};

    void initThread() {
        mInit = true;
    }

    void exitThread() {
        done = true;
    }

    void start() {
        mThrd = thread(&myThread::run, this);
    }
 
    void join() {
        mThrd.join();
    }

    void run() {
        while (!done) {
            while(!mInit) {
                this_thread::sleep_for(5000ms);
                continue;
            }
            while(1) {
                jobType* job = mMgr->getJob();
                if (job) {
                    job->work(mId);
                    mMgr->finishJob();
                } else 
                    break;
            }
            mInit = false;
        }
    }

 private:
    int mId;
    bool mInit;
    bool done;
    thrdMgr<jobType> * mMgr;
    thread mThrd;
};

template <class jobType>
class thrdMgr {
 public:
    thrdMgr(vector<jobType*> & jobs) : jobList(jobs), mCurrJobIdx(0), finishedJobs(0) {};
    ~thrdMgr() {};

    void startThreads(int thrds) {
        for (int i=0; i<thrds; i++) {
            myThread<jobType>* mthrd = new myThread<jobType>(i, this);
            mthrd->start();
            thrdList.push_back(mthrd);
        }
    }

    void initThreads() {
        totalJobs = jobList.size();
        for (auto it = thrdList.begin(); it != thrdList.end(); it++)
            (*it)->initThread();
    }
    
    jobType* getJob() {
        lock_guard<mutex> lock(mJobMutex);
        if (mCurrJobIdx >= totalJobs)
            return NULL;
        return jobList[mCurrJobIdx++];
    }

    void addJob(jobType* job) {
        lock_guard<mutex> lock(mJobMutex);
        jobList.push_back(job);
        totalJobs++;
        for (auto it = thrdList.begin(); it!= thrdList.end(); it++) {
            (*it)->initThread();
        }
    }

    void finishJob() {
        lock_guard<mutex> lock(mWaitMutex);
        finishedJobs++;
        cv.notify_one();
    }

    void wait() {
        unique_lock<mutex> lock(mWaitMutex);
        cv.wait(lock, [this]{ return (finishedJobs == totalJobs); });
        cout << "All jobs assigned to the thread manager are finished" << endl;
    }

    void exit() {
        for (auto it = thrdList.begin(); it != thrdList.end(); it++)  
            (*it)->exitThread();

        for (auto it = thrdList.begin(); it != thrdList.end(); it++)
            (*it)->join();
    }

private:
    int mCurrJobIdx;
    int finishedJobs;
    int totalJobs;
    vector<jobType*> jobList;
    vector<myThread<jobType>*> thrdList;
    mutex mJobMutex;
    mutex mWaitMutex;
    condition_variable cv;
};