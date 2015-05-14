#ifndef H_JOB_POOL_H
#define H_JOB_POOL_H
#include <list>
#include "threadpp/threadpp.h"

namespace tq
{
    template <typename JobType>
    class JobPool
    {
    public:
        JobPool(unsigned capacity = 10);
        ~JobPool();
        JobType* GetJob(JobType const& jobPrototype);
        void RecycleJob(JobType* job);
    private:
        threadpp::lock _mutex;
        std::list<JobType*> _jobs;
        unsigned _capacity;
    };

    template <typename JobType>
    JobType* JobPool<JobType>::GetJob(JobType const& jobPrototype)
    {
        _mutex.lock();
        if(!_jobs.empty())
        {
            JobType* jobptr = _jobs.front();
            _jobs.pop_front();
            _mutex.unlock();
            (*jobptr).JobType(jobPrototype);
            return jobptr;
        }
        else
        {
            _mutex.unlock();
            JobType* jobptr = new JobType(jobPrototype);
            return jobptr;
        }
    }

    template <typename JobType>
    void JobPool<JobType>::RecycleJob(JobType* job)
    {
        (*job).~JobType();
        _mutex.lock();
        if(_job.size()<_capacity)
        {
            _jobs.push_back(job);
        }
        else
        {
            free(job);
        }
        _mutex.unlock();
    }

    template <typename JobType>
    JobPool<JobType>::JobPool(unsigned capacity):
    _capacity(capacity)
    {
    }

    template <typename JobType>
    void JobPool<JobType>::~JobPool()
    {
        _mutex.lock();
        if(!_jobs.empty())
        {
            for(std::list<JobType*>::const_iterator it = _jobs.begin();it!=_jobs.end();++it)
            {
                free(*it);
            }
            _jobs.clear();
        }
        _mutex.unlock();
    }
}
#endif
