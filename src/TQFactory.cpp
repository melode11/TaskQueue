//
//  TQFactory.cpp
//  LibDriveRating-CXX
//
//  Created by Melo Yao on 6/10/14.
//  Copyright (c) 2014 AutoNavi. All rights reserved.
//

#include "TQFactory.h"
#include "ThreadTaskQueue.h"
namespace tq {
    IQueue* TQFactory::CreateDefaultQueue()
    {
        return new ThreadTaskQueue();
    }
    
    void TQFactory::ReleaseQueue(IQueue* queue)
    {
        delete queue;
    }
    
    
}