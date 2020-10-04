#ifndef __LOCKED_QUEUE_HPP__
#define __LOCKED_QUEUE_HPP__

#include <cstdint>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

template <class T, class UnderlyingContainer = std::deque<T>>
class locked_queue {
public:

	typedef T value_type;
	typedef std::queue<T, UnderlyingContainer> container;

	locked_queue() : capacity_(std::numeric_limits<std::size_t>::max()) {}

	locked_queue(std::size_t capacity) : capacity_(capacity) {}

	void enqueue(T val) {
		std::unique_lock<std::mutex> lock(mutex_);
		condition_enqueue.wait(lock, [&] { return queue_.size() < capacity_; });
		queue_.push(std::move(val));
		condition_dequeue.notify_one();
	}

	bool try_dequeue(T& ret) {
		std::unique_lock<std::mutex> lock(mutex_);
		if (!queue_.empty()) {
			ret = std::move(queue_.front());
			queue_.pop();
			condition_enqueue.notify_one();
			return true;
		}
		else {
			return false;
		}
	}

	bool dequeue(T& ret) {
		std::unique_lock<std::mutex> lock(mutex_);
		condition_dequeue.wait(lock, [this] { return !queue_.empty(); });
		ret = std::move(queue_.front());
		queue_.pop();
		condition_enqueue.notify_one();
		return true;
	}

	bool empty() {
		return queue_.empty();
	}

private:

	std::size_t capacity_;
	container queue_;

	std::mutex mutex_;
	std::condition_variable condition_enqueue;
	std::condition_variable condition_dequeue;
};

#endif // !__LOCKED_QUEUE_HPP__