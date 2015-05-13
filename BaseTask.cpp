//
//  BaseTaskQueue.cpp
//  LibDriveRating-CXX
//
//  Created by Melo Yao on 6/9/14.
//  Copyright (c) 2014 AutoNavi. All rights reserved.
//

#include "BaseTask.h"

namespace tq {
    BaseTask::BaseTask():ITask(),_cancelled(false)
    {
    }
    
    void BaseTask::Cancel()
    {
        _cancelled = true;
    }
    
    bool BaseTask::IsCancelled() const
    {
        return _cancelled;
    }
    
    void BaseTask::Run()
    {
        if (_cancelled) {
            return;
        }
        Main();
    }
}