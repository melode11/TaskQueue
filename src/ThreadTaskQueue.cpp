//
//  ThreadTaskQueue.cpp
//  LibDriveRating-CXX
//
//  Created by Melo Yao on 6/9/14.
//  Copyright (c) 2014 AutoNavi. All rights reserved.
//

#include "ThreadTaskQueue.h"
#include <algorithm>
#define WAIT_TIMEOUT 5000

using namespace threadpp;

namespace tq{
    
    class QueueRunnable
    {
        ThreadTaskQueue* queue;
        ITask* currentTask;
    protected:
        static void run_callback(void*);
        thread* th;
        
        QueueRunnable(ThreadTaskQueue* q):queue(q),currentTask(NULL){}
        void run();
        void CancelCurrentTask();
        bool TaskIsRunning() const;
        friend class ThreadTaskQueue;
    };
    
    void QueueRunnable::run_callback(void *ctx)
    {
        ((QueueRunnable*) ctx)->run();
    }
    
    void QueueRunnable::run()
    {
        while (queue->IsStarted()) {
            queue->LockQueue();
            ITask* task = queue->NextTask();
            if (task == NULL) {
                queue->UnlockQueue();
                continue;
            }
            currentTask = task;
            queue->UnlockQueue();
            task->Run();
            queue->LockQueue();
            currentTask = NULL;
            queue->FinishTask(task);
            queue->NotifyQueue();
            queue->UnlockQueue();
        }
    }
    
    void QueueRunnable::CancelCurrentTask()
    {
        queue->LockQueue();
        if(currentTask)
        {
            currentTask->Cancel();
        }
        queue->UnlockQueue();
    }
    
    bool QueueRunnable::TaskIsRunning() const
    {
        return currentTask != NULL;
    }
    
    ThreadTaskQueue::ThreadTaskQueue():_tasklist(),_started(false),_suspended(false)
    {
        
    }
    
    void ThreadTaskQueue::Start(unsigned int nThreads)
    {
        _mutex.lock();
        if (_started) {
            _mutex.unlock();
            return;
        }
        _started = true;
        _threads.reserve(nThreads);
        for (int i = 0; i<nThreads; ++i) {
            QueueRunnable* runnable = new QueueRunnable(this);
            runnable->th = new thread(QueueRunnable::run_callback, runnable);
            _threads.push_back(runnable);
        }
        _mutex.unlock();
    }
    
    void ThreadTaskQueue::Stop()
    {
        _mutex.lock();
        if (!_started) {
            _mutex.unlock();
            return;
        }
        _started = false;
        for (std::list<ITask*>::iterator it = _tasklist.begin(); it!= _tasklist.end(); ++it) {
            delete *it;
        }
        _tasklist.clear();
        _mutex.notify_all();
        std::vector<QueueRunnable*> copy(_threads);
        _threads.clear();
        _mutex.unlock();
        for (std::vector<QueueRunnable*>::iterator it = copy.begin(); it!=copy.end(); ++it) {
            (*it)->th->join();
            thread* t = (*it)->th;
            delete (*it);
            delete t;
        }
    }
    
    bool ThreadTaskQueue::IsStarted() const
    {
        return _started;
    }
    
    void ThreadTaskQueue::AddTask(ITask* task)
    {
        _mutex.lock();
        if (_started) {
            _tasklist.push_back(task);
            _mutex.notify_all();
        }
        _mutex.unlock();
    }
    
    void ThreadTaskQueue::GetTasks(ITask** tasksBuf, unsigned int taskBufSize) const
    {
        recursivelock* mutex = const_cast<recursivelock*>(&_mutex);
        mutex->lock();
        size_t count = 0;
        for (std::list<ITask*>::const_iterator it = _tasklist.begin(); it!=_tasklist.end(); ++it) {
            if (count<taskBufSize) {
                tasksBuf[count] = *it;
                count++;
            }
            else
            {
                break;
            }
        }
        mutex->unlock();
    }
    
    unsigned int ThreadTaskQueue::TaskCount() const
    {
        recursivelock* mutex = const_cast<recursivelock*>(&_mutex);
        mutex->lock();
        unsigned int count = (unsigned int)_tasklist.size();
        mutex->unlock();
        return count;
    }
    
    void ThreadTaskQueue::CancelAll()
    {
        _mutex.lock();
        for (std::vector<QueueRunnable*>::iterator it = _threads.begin(); it!=_threads.end(); ++it) {
            (*it)->CancelCurrentTask();
        }
        for (std::list<ITask*>::const_iterator it = _tasklist.begin(); it!=_tasklist.end(); ++it) {
            (*it)->Cancel();
        }
        _mutex.unlock();
        
    }
    
    void ThreadTaskQueue::WaitForFinish()
    {
        
        while (true) {
            _mutex.lock();
            bool isExecuting = false;
            for (std::vector<QueueRunnable*>::iterator it = _threads.begin(); it!=_threads.end(); ++it) {
                if ((*it)->TaskIsRunning()) {
                    isExecuting = true;
                    break;
                }
            }
            if (!isExecuting&&_tasklist.size() == 0) {
                _mutex.unlock();
                break;
            }
            _mutex.wait(1000);
            _mutex.unlock();
        }
        
    }
    
    void ThreadTaskQueue::Suspend()
    {
        _mutex.lock();
        _suspended = true;
        _mutex.unlock();
    }
    
    void ThreadTaskQueue::Resume()
    {
        _mutex.lock();
        _suspended = false;
        _mutex.notify_all();
        _mutex.unlock();

    }
    
    void ThreadTaskQueue::NotifyQueue()
    {
        _mutex.notify_all();
    }
    
    ITask* ThreadTaskQueue::NextTask()
    {
        while (_started && (_tasklist.empty()||_suspended)) {
            _mutex.wait(WAIT_TIMEOUT);//defensive waiting time limit.
        }
        ITask* task = NULL;
        if (_tasklist.size()>0) {
            task = _tasklist.front();
            _tasklist.pop_front();
        }
        return task;
    }
    
    inline
    void ThreadTaskQueue::LockQueue()
    {
        _mutex.lock();
    }
    
    inline
    void ThreadTaskQueue::UnlockQueue()
    {
        _mutex.unlock();
    }

    inline
    void ThreadTaskQueue::FinishTask(ITask* task)
    {
        if(task->GetCategory() != NoCategory)
        {
            _recyclerMutex.lock();
            std::map<TaskCategory,RecyclerPair>::iterator it = _recyclers.find(task->GetCategory());
            if(it!=_recyclers.end())
            {
                RecyclerPair pair = it->second;
                _recyclerMutex.unlock();
                pair.recycler(task,pair.context);
                return;
            }
            _recyclerMutex.unlock();
        }
        delete task;

    }
    
    void ThreadTaskQueue::SetTaskRecycler(TaskCategory cat, TaskRecycler recycler,void *context)
    {
        _recyclerMutex.lock();
        _recyclers[cat] = {recycler,context};
        _recyclerMutex.unlock();
    }

    void ThreadTaskQueue::ClearTaskRecycler(TaskCategory cat)
    {
        _recyclerMutex.lock();
        std::map<TaskCategory,RecyclerPair>::iterator it = _recyclers.find(cat);
        if(it!=_recyclers.end())
        {
            _recyclers.erase(it);
        }
        _recyclerMutex.unlock();
    }

    ThreadTaskQueue::~ThreadTaskQueue()
    {
        this->Stop();//Defensive stop.
    }
    
}
