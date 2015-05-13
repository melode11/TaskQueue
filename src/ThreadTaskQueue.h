//
//  ThreadTaskQueue.h
//  LibDriveRating-CXX
//
//  Created by Melo Yao on 6/9/14.
//  Copyright (c) 2014 AutoNavi. All rights reserved.
//

#ifndef __LibDriveRating_CXX__ThreadTaskQueue__
#define __LibDriveRating_CXX__ThreadTaskQueue__

#include "TQInterface.h"
#include "BaseTask.h"
#include <list>
#include <map>
#include <vector>
#include "threadpp/threadpp.h"

namespace tq
{
    class QueueRunnable;
    
    class ThreadTaskQueue:public IQueue
    {
    public:
        ThreadTaskQueue();
        
        void Start(unsigned int nThreads = 1);
        
        void Stop();
        
        bool IsStarted() const;
        
        void AddTask(ITask* task);//Will transfer the ownership of task.
        
        void GetTasks(ITask** tasksBuf, unsigned int taskBufSize) const;
        
        unsigned int TaskCount() const;
        
        void CancelAll();
        
        void WaitForFinish() ;
        
        void Suspend();
        
        void Resume();

        void SetTaskRecycler(TaskCategory cat, TaskRecycler recycler,void *context);

        void ClearTaskRecycler(TaskCategory cat);
        
        ~ThreadTaskQueue();
        
    protected:
        
        void LockQueue();
        
        void UnlockQueue();
        
        void NotifyQueue();
        
        ITask* NextTask();

        void FinishTask(ITask* task);
        
        friend class QueueRunnable;

    private:
        struct RecyclerPair
        {
            TaskRecycler recycler;
            void *context;
        };

        std::map<TaskCategory,RecyclerPair> _recyclers;
        std::list<ITask*> _tasklist;
        std::vector<QueueRunnable*> _threads;
        threadpp::recursivelock _mutex;
        threadpp::lock _recyclerMutex;
        bool _started;
        bool _suspended;
    };
}

#endif /* defined(__LibDriveRating_CXX__ThreadTaskQueue__) */
