//
//  TQFactory.h
//  LibDriveRating-CXX
//
//  Created by Melo Yao on 6/10/14.
//  Copyright (c) 2014 AutoNavi. All rights reserved.
//

#ifndef __LibDriveRating_CXX__TQFactory__
#define __LibDriveRating_CXX__TQFactory__

#include "TQInterface.h"

namespace tq {
    class TQFactory
    {
    public:
        static IQueue* CreateDefaultQueue();

        static void ReleaseQueue(IQueue* queue);
    };
}

#endif /* defined(__LibDriveRating_CXX__TQFactory__) */
