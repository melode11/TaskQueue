//
//  BaseTaskQueue.h
//  LibDriveRating-CXX
//
//  Created by Melo Yao on 6/9/14.
//  Copyright (c) 2014 AutoNavi. All rights reserved.
//

#ifndef __LibDriveRating_CXX__BaseTaskQueue__
#define __LibDriveRating_CXX__BaseTaskQueue__

#include "TQInterface.h"
namespace tq
{
    class BaseTask : public ITask
    {
    private:
        volatile bool _cancelled;
    public:
        BaseTask();
        void Run();
        void Cancel();
        bool IsCancelled() const;
        
        virtual void Main() = 0;
    };
}
#endif /* defined(__LibDriveRating_CXX__BaseTaskQueue__) */
