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

	thread_pool(int thread_count, std::size_t queue_capacity, bool need_output = true)
		: is_termination_requested_(false)
		, task_queue_(queue_capacity)
		, output_queue_(queue_capacity)
		, need_output_(need_output){
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

	bool add(const std::function<OutputType()>& func) {
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
		assert(need_output_);
		return output_queue_.try_dequeue(ret);
	}

	OutputType pull() {
		assert(need_output_);
		OutputType ret;
		output_queue_.dequeue(ret);
		return ret;
	}

private:

	std::function<void()> main_ = [this]() {
		while (true) {
			std::function<OutputType()> func;
			{
				std::unique_lock<std::mutex> lock(mutex_);
				while (task_queue_.empty()) {
					if (is_termination_requested_.load()) {
						return;
					}
					condition_.wait(lock, [&]() { return !task_queue_.empty() || is_termination_requested_.load(); });
				}
				const auto dequeue_result = task_queue_.dequeue(func);
				assert(dequeue_result);
			}
			if (need_output_) output_queue_.enqueue(func());
			else func();
		}
	};

	bool need_output_;
	std::atomic<bool> is_termination_requested_;
	locked_queue<std::function<OutputType()>> task_queue_;
	locked_queue<OutputType> output_queue_;
	std::mutex mutex_;
	std::condition_variable condition_;
	std::vector<std::thread> threads_;
};



#endif // !__THREAD_POOL_HPP__