/**
 * Thread helpers
 * @author Tobias Weber <tobias.weber@tum.de>
 * @date aug-2015
 * @license GPLv2 or GPLv3
 * @desc see, e.g, (Williams 2012), pp. 273-299
 */

#ifndef __TLIBS_THREAD_H__
#define __TLIBS_THREAD_H__

#include <future>
#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include <algorithm>
#include <type_traits>
#include <memory>

#ifdef USE_BOOST_THREADPOOL
	#include <boost/asio.hpp>
#endif


namespace tl {


/**
	* thread pool interface
	*/
template<class t_func>
class IThreadPool
{
public:
	using t_ret = typename std::result_of<t_func&()>::type;
	using t_fut = std::list<std::future<t_ret>>;
	using t_task = std::list<std::packaged_task<t_ret()>>;

public:
	virtual ~IThreadPool() {}

	/**
	 * add a function to be executed, giving a packaged task and a future.
	 */
	virtual void AddTask(const std::function<t_ret()>& fkt) = 0;

	/**
	 * issue start signal
	 */
	virtual void StartTasks() = 0;

	/**
	 * get (remaining) tasks and their results
	 */
	virtual t_task& GetTasks() = 0;
	virtual t_fut& GetFutures() = 0;

	/**
	 * wait for all tasks to be finished
	 */
	virtual void JoinAll() = 0;
};



/**
 * minimalistic direct thread pool implementation
 */
template<class t_func> class ThreadPool1 : public IThreadPool<t_func>
{
	public:
		using t_ret = typename IThreadPool<t_func>::t_ret;
		using t_fut = typename IThreadPool<t_func>::t_fut;
		using t_task = typename IThreadPool<t_func>::t_task;


	protected:
		std::list<std::unique_ptr<std::thread>> m_lstThreads;
		std::mutex m_mtx;


		// list of wrapped function to be executed
		t_task m_lstTasks;
		// futures with function return values
		t_fut m_lstFutures;


		// signal to start jobs
		std::promise<void> m_signalStartIn;
		std::future<void> m_signalStartOut = std::move(m_signalStartIn.get_future());


	public:
		ThreadPool1(unsigned int iNumThreads = std::thread::hardware_concurrency(),
			void (*pThStartFunc)() = nullptr)
		{
			// start 'iNumThreads' threads
			for(unsigned int iThread=0; iThread<iNumThreads; ++iThread)
			{
				m_lstThreads.emplace_back(
				std::unique_ptr<std::thread>(new std::thread([this, pThStartFunc, iThread]()
				{
					// callback to invoke before starting job thread
					if(pThStartFunc) (*pThStartFunc)();
					m_signalStartOut.wait();

					while(1)
					{
						std::unique_lock<std::mutex> lock0(m_mtx);

						// is a task available
						if(m_lstTasks.size() > 0)
						{
							// pop task from list
							std::packaged_task<t_ret()> task =
								std::move(m_lstTasks.front());
							m_lstTasks.pop_front();

							lock0.unlock();

							// run task
							//tl::log_debug("Thread ", iThread, ": running task.");
							task();
						}
						else
							break;
					}
				})));
			}
		}


		virtual ~ThreadPool1()
		{
			JoinAll();
			m_lstThreads.clear();
		}


		/**
		 * add a function to be executed, giving a packaged task and a future.
		 */
		virtual void AddTask(const std::function<t_ret()>& fkt) override
		{
			std::packaged_task<t_ret()> task(fkt);
			std::future<t_ret> fut = task.get_future();

			std::lock_guard<std::mutex> lock(m_mtx);
			m_lstTasks.emplace_back(std::move(task));
			m_lstFutures.emplace_back(std::move(fut));
		}


		/**
		 * issue start signal
		 */
		virtual void StartTasks() override
		{
			m_signalStartIn.set_value();
		}


		virtual t_fut& GetFutures() override { return m_lstFutures; }
		virtual t_task& GetTasks() override { return m_lstTasks; }


		/**
		 * wait for all tasks to be finished
		 */
		virtual void JoinAll() override
		{
			std::for_each(m_lstThreads.begin(), m_lstThreads.end(),
				[](std::unique_ptr<std::thread>& pThread)
				{
					if(pThread)
						pThread->join();
				});
		}
};




#ifdef USE_BOOST_THREADPOOL
/**
 * wrapper for boost thread pool
 */
template<class t_func>
class ThreadPool2 : public IThreadPool<t_func>
{
public:
	using t_ret = typename IThreadPool<t_func>::t_ret;
	using t_fut = typename IThreadPool<t_func>::t_fut;
	using t_task = typename IThreadPool<t_func>::t_task;

protected:
	std::shared_ptr<boost::asio::thread_pool> m_tp;
	std::mutex m_mtx, m_mtxStart;

	// list of wrapped function to be executed
	t_task m_lstTasks;
	// futures with function return values
	t_fut m_lstFutures;

	// function to run before each thread (not task)
	void (*m_pThStartFunc)() = nullptr;


public:
	ThreadPool2(unsigned int iNumThreads = std::thread::hardware_concurrency(),
		void (*pThStartFunc)() = nullptr)
		: m_tp{std::make_shared<boost::asio::thread_pool>(iNumThreads)},
			m_pThStartFunc{pThStartFunc}
	{}


	virtual ~ThreadPool2()
	{
		JoinAll();
	}


	/**
	 * add a function to be executed, giving a packaged task and a future.
	 */
	virtual void AddTask(const std::function<t_ret()>& fkt) override
	{
		std::packaged_task<t_ret()> task(fkt);
		std::future<t_ret> fut = task.get_future();

		std::lock_guard<std::mutex> lock(m_mtx);
		m_lstTasks.emplace_back(std::move(task));
		m_lstFutures.emplace_back(std::move(fut));

		std::packaged_task<t_ret()>* thetask = &m_lstTasks.back();;

		boost::asio::post(*m_tp, [this, thetask]() -> void
		{
			{
				// ensure that this is only called per-thread, not per-task
				std::lock_guard<std::mutex> lockStart(m_mtxStart);

				thread_local bool bThreadAlreadySeen{0};
				if(m_pThStartFunc && !bThreadAlreadySeen)
				{
					bThreadAlreadySeen = 1;
					(*m_pThStartFunc)();
				}
			}

			// run task
			(*thetask)();
		});
	}


	/**
	 * issue start signal
	 */
	virtual void StartTasks() override
	{
		// not supported, starts immediately
	}


	virtual t_fut& GetFutures() override { return m_lstFutures; }
	virtual t_task& GetTasks() override { return m_lstTasks; }


	/**
	 * wait for all tasks to be finished
	 */
	virtual void JoinAll() override
	{
		if(!m_tp) return;
		m_tp->join();
	}
};
#endif



#ifdef USE_BOOST_THREADPOOL
	template<class t_func> using ThreadPool = ThreadPool2<t_func>;
#else
	template<class t_func> using ThreadPool = ThreadPool1<t_func>;
#endif


}
#endif
