#ifndef __THREAD_POOL_HPP__
#define __THREAD_POOL_HPP__

#include <cstdint>
#include <functional>
#include <atomic>
#include <cassert>

#include "locked_queue.hpp"

template <class OutputType>
class thread_pool {
public:

	thread_pool(int thread_count, std::size_t queue_capacity)
		: is_termination_requested_(false)
		, task_queue_(queue_capacity)
		, output_queue_(queue_capacity) {
		for (int i = 0; i < thread_count; i++) {
			threads_.emplace_back(std::thread(main_));
		}
	}

	~thread_pool() {
		is_termination_requested_.store(true);
		condition_.notify_all();
		for (auto& thread : threads_) {
			thread.join();
		}
	}

	bool add(std::function<OutputType()>& func) {
		task_queue_.enqueue(func);
		condition_.notify_all();
		return true;
	}

	bool add(std::function<OutputType()>&& func) {
		task_queue_.enqueue(std::move(func));
		condition_.notify_all();
		return true;
	}

	bool try_pull(OutputType& ret) {
		return output_queue_.try_dequeue(ret);
	}

	OutputType pull() {
		OutputType ret;
		output_queue_.dequeue(ret);
		return ret;
	}

private:

	std::function<void()> main_ = [this]() {
		while (1) {
			std::function<OutputType()> func;
			{
				std::unique_lock<std::mutex> lock(mutex_);
				while (task_queue_.empty()) {
					if (is_termination_requested_.load()) {
						return;
					}
					condition_.wait(lock);
				}
				const auto dequeue_result = task_queue_.try_dequeue(func);
				assert(dequeue_result);
			}
			output_queue_.enqueue(func());
		}
	};


	std::atomic<bool> is_termination_requested_;
	locked_queue<std::function<OutputType()>> task_queue_;
	locked_queue<OutputType> output_queue_;
	std::mutex mutex_;
	std::condition_variable condition_;
	std::vector<std::thread> threads_;
};



#endif // !__THREAD_POOL_HPP__