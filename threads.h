#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>
using namespace std;

class aMutex {
public:
    aMutex() {
        int status = pthread_mutex_init(&mLock, NULL);
    }
    ~aMutex() {
        int status = pthread_mutex_destroy(&mLock);
    }

    void lock() {
        int status = pthread_mutex_lock(&mLock);
    }
    void unlock() {
        int status = pthread_mutex_unlock(&mLock);

    }

protected:
    pthread_mutex_t mLock;
};

class aCond : public aMutex {
public:
    aCond() {
        int status = pthread_cond_init(&mCond, NULL);
    }
    ~aCond() {
        int status = pthread_cond_destroy(&mCond);
    }

    void wait(long t = 0) {
        int status = pthread_cond_wait(&mCond, &mLock);
    }
    void signal() {
        int status = pthread_cond_signal(&mCond);
    }

protected:
    pthread_cond_t mCond;
};

class aSync {
public:
    aSync(const aMutex & mut) : mpMutex((aMutex*) &mut) {mpMutex->lock();}

    ~aSync() {
        mpMutex->unlock();
    }
private:
    aMutex * mpMutex;
};

class baseThread { //cannot be instantiated, as it contains pure virtual funtion.
public:
    baseThread(int Id) {
        thrdId = Id;
    }
    void start();
    void stop();
    virtual void run() = 0; //pure virtual function, it need to be overriden by every child class.
    int getThrdId() {
        return thrdId;
    }
    void join();
    
private:
    int thrdId;
    pthread_t thrd;

};

void* startCall(void* arg) {
    baseThread* thrd = (baseThread*) arg;
    thrd->run();
    return 0;
}

void baseThread::start() {
    int status = pthread_create(&thrd, NULL, startCall, this);
    //cout << "thread is created with id:" << endl;

    if(status != 0) {
        cout << "Thread creation with ID:" << thrdId << " failed!!" << endl;
    }
}

void baseThread::join() {
    void * status;
    int r = pthread_join(thrd, &status);
    if(r!=0) {
        cout << "unable to join" << endl;
    }
}

void baseThread::stop() {
    int status = pthread_cancel(thrd);
    if(status != 0) {
        cout << "Unable to cancel the thread!!" << endl;
    }
}

template <class taskType> 
class threadManager;


//---------------- Child class --------------------
template <class taskType>
class myThread : public baseThread {
private: 
    threadManager<taskType> * mgr;
    bool init;
    bool done;
public:
    myThread(int id, threadManager<taskType> * m) : init(false), baseThread(id), done(false), mgr(m) {}
    void initialize() {
        //cout << " init is true!" << endl;
        init = true;
    }

    void exit() {
        done = true;
    }

    void run() {
        while(!done) {
            if(!init) { 
                //cout << "waiting....." << endl;
                sleep(0.2);
                continue;
            }
            while (1) {
                taskType* item = mgr->get(getThrdId()); 
                if (item != NULL) {
                    item->work(getThrdId());
                    mgr->finish();
                } else {
                    break;
                }
            }
            init = false;
            // break;
        }
    }
};


// --- thread manager -------------------
template<class taskType> 
class threadManager {
private: 
    vector<myThread<taskType>* > threadList;
    vector<taskType*>* taskList;
    unsigned int finished_tasks;
    unsigned int total_tasks;
    unsigned int currIdx; // current index for new task in task list 
    aCond mCond;
    aMutex mLock;

public: 
    void start(int numThreads) {
        aSync sync(mCond);
        for(int i=0; i<numThreads; i++) {
            myThread<taskType>* thrd = new myThread<taskType>(i, this);
            threadList.push_back(thrd);
            thrd->start();
        }
    }

    void init(vector<taskType*> * lst) {
        taskList = lst;
        total_tasks = lst->size();
        finished_tasks = 0;
        currIdx = 0;
        for (unsigned int i=0; i < threadList.size(); i++) {
            threadList[i]->initialize();
        }
    }

    void exit() {
        // notify all threads to exit
        for (unsigned int i=0; i<threadList.size(); i++) {
            threadList[i]->exit();
        }
        for (unsigned int i=0; i<threadList.size(); i++) {
            threadList[i]->join();
        }
    }

    void wait() {
        while (1) {
            aSync sync(mCond);
            if (finished_tasks < total_tasks) {
                mCond.wait();
            } else {
                break;
            }
        }
    }

    taskType* get(int id) {
        aSync lock(mLock);
        if (currIdx >= taskList->size()) {
            return NULL;
        }
        taskType* task = (*taskList)[currIdx++];
        return task;
    }

    void finish() {
        aSync sync(mCond);
        finished_tasks++;
        mCond.signal();
    }
};



