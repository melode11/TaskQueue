#ifndef H_TASK_POOL_H
#define H_TASK_POOL_H
#include <list>
#include "threadpp/threadpp.h"
#include <cstdlib>
#include <cstdio>
namespace tq
{
    template <typename TaskType>
    class TaskPool
    {
    public:
        TaskPool(unsigned capacity = 10);
        ~TaskPool();
        TaskType* GetTask(TaskType const& taskPrototype);
        void RecycleTask(TaskType* task);
        void Purge();
    private:
        threadpp::lock _mutex;
        std::list<TaskType*> _tasks;
        unsigned _capacity;
    };

    template <typename TaskType>
    TaskType* TaskPool<TaskType>::GetTask(TaskType const& taskPrototype)
    {
        _mutex.lock();
        if(!_tasks.empty())
        {

            TaskType* taskptr = _tasks.front();
            _tasks.pop_front();
            _mutex.unlock();
            new (taskptr)TaskType(taskPrototype);
            return taskptr;
        }
        else
        {
            _mutex.unlock();
            static int newcount = 0;
            TaskType* taskptr = new TaskType(taskPrototype);
            return taskptr;
        }
    }

    template <typename TaskType>
    void TaskPool<TaskType>::RecycleTask(TaskType* task)
    {
        (*task).~TaskType();
        _mutex.lock();
        if(_tasks.size()<_capacity)
        {
            _tasks.push_back(task);
        }
        else
        {
            free(task);
        }
        _mutex.unlock();
    }
    
    template <typename TaskType>
    void TaskPool<TaskType>::Purge()
    {
        _mutex.lock();
        if(!_tasks.empty())
        {
            for(typename std::list<TaskType*>::const_iterator it = _tasks.begin();it!=_tasks.end();++it)
            {
                free(*it);
            }
            _tasks.clear();
        }
        _mutex.unlock();
    }

    template <typename TaskType>
    TaskPool<TaskType>::TaskPool(unsigned capacity):
    _capacity(capacity)
    {
    }

    template <typename TaskType>
    TaskPool<TaskType>::~TaskPool()
    {
        this->Purge();
    }
}
#endif
