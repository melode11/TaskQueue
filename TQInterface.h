//
//  TQInterface.h
//  LibDriveRating-CXX
//
//  Created by Melo Yao on 6/9/14.
//  Copyright (c) 2014 AutoNavi. All rights reserved.
//

#ifndef __LibDriveRating_CXX__TQInterface__
#define __LibDriveRating_CXX__TQInterface__

namespace tq {
    class ITask
    {
    public:
        virtual void Run() = 0;
        virtual void Cancel() = 0;
        virtual bool IsCancelled() const = 0;
        virtual ~ITask(){};
    };
    
    class IQueue
    {
    public:
        virtual void Start(unsigned int nThreads = 1) = 0;

        virtual void Stop() = 0;

        virtual void AddTask(ITask* task) = 0;
        
        virtual void GetTasks(ITask** tasksBuf, unsigned int taskBufSize) const= 0;
        
        virtual unsigned int TaskCount() const = 0;
        
        virtual void CancelAll() = 0;
        
        virtual void WaitForFinish() = 0;

        virtual void Suspend() = 0;
        
        virtual void Resume() = 0;
        
        virtual ~IQueue() {};
    };
}

#endif /* defined(__LibDriveRating_CXX__TQInterface__) */
