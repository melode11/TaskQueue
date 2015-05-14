#include "../src/TaskPool.h"
#include "../src/TQFactory.h"
#include "../src/BaseTask.h"
#include <cstdio>
#include <chrono>
#include <thread>
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

    TaskCategory GetCategory() const
    {
        return 1;
    }

    private:
    int _count;
};

void TaskRecycle(ITask* task,void* context)
{
    ((TaskPool<PrintTask> *)context)->RecycleTask((PrintTask*)task);
}

int main(int argc, const char* argv[])
{
    printf("main\n");
    TaskPool<PrintTask> pool;
    IQueue* q = TQFactory::CreateDefaultQueue();
    q->SetTaskRecycler(1,TaskRecycle,&pool);
    q->Start(1);
    for(int i =0;i<10000;++i)
    {
       q->AddTask(pool.GetTask(PrintTask(i)));
       std::this_thread::sleep_for(std::chrono::microseconds(10));
    } 

    q->WaitForFinish();
    TQFactory::ReleaseQueue(q);
    return 0;
}
