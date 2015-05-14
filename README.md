## TaskQueue
A super simple producer-consumer implementation of multi-threaded task queue.

### You may use this because:
- Looking for a light-weight and easy to integrate multi-threaded task queue.
- You have no c++11 support.
- You want cross platform.

### Get it
```shell
git clone --recurse-submodules https://github.com/melode11/TaskQueue.git
```

### Example
```c++
#include "../src/TQFactory.h"
#include "../src/BaseTask.h"
#include <cstdio>
using namespace tq;

class PrintTask : public BaseTask
{
    public:
    PrintTask(int counter):_count(counter){
    }

    void Main()
    {
        printf("task number:%ld\n",_count);
    }

    private:
    int _count;
};

int main(int argc, const char* argv[])
{
    IQueue* q = TQFactory::CreateDefaultQueue();
    q->Start();
    for(int i =0;i<10000;++i)
    {
       q->AddTask(new PrintTask(i));
    } 

    q->WaitForFinish();
    TQFactory::ReleaseQueue(q);
    return 0;
}
```

### Task Reuse
Use class `TaskPool`

See example in [TestTaskPool.cpp](./test/TestTaskPool.cpp)
