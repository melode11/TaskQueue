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
        TaskType* GetTask(TaskType const& jobPrototype);
        void RecycleTask(TaskType* job);
    private:
        threadpp::lock _mutex;
        std::list<TaskType*> _jobs;
        unsigned _capacity;
    };

    template <typename TaskType>
    TaskType* TaskPool<TaskType>::GetTask(TaskType const& jobPrototype)
    {
        _mutex.lock();
        if(!_jobs.empty())
        {

            TaskType* jobptr = _jobs.front();
            _jobs.pop_front();
            _mutex.unlock();
            new (jobptr)TaskType(jobPrototype);
            return jobptr;
        }
        else
        {
            _mutex.unlock();
            static int newcount = 0;
            TaskType* jobptr = new TaskType(jobPrototype);
            return jobptr;
        }
    }

    template <typename TaskType>
    void TaskPool<TaskType>::RecycleTask(TaskType* job)
    {
        (*job).~TaskType();
        _mutex.lock();
        if(_jobs.size()<_capacity)
        {
            _jobs.push_back(job);
        }
        else
        {
            free(job);
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
        _mutex.lock();
        if(!_jobs.empty())
        {
            for(typename std::list<TaskType*>::const_iterator it = _jobs.begin();it!=_jobs.end();++it)
            {
                free(*it);
            }
            _jobs.clear();
        }
        _mutex.unlock();
    }
}
#endif
