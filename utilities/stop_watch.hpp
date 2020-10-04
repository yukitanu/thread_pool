#ifndef __STOP_WATCH_HPP__
#define __STOP_WATCH_HPP__

#include <chrono>
#include <stack>

namespace time_unit {
	using milliseconds = std::chrono::milliseconds;
	using microseconds = std::chrono::microseconds;
}

template <typename DurationUnit = time_unit::microseconds>
class stop_watch {
public:

	using milliseconds = std::chrono::milliseconds;
	using microseconds = std::chrono::microseconds;
	using time_point = std::chrono::high_resolution_clock::time_point;

	void tick() {
		time_points_.push(get());
	}

	std::uint64_t tock() {
		const auto finish = get();
		const auto start = time_points_.top();
		time_points_.pop();
		return duration_cast(start, finish);
	}

private:

	time_point get() {
		return std::chrono::high_resolution_clock::now();
	}

	auto duration_cast(const time_point& start, const time_point& finish) {
		return std::chrono::duration_cast<DurationUnit>(finish - start).count();
	}

	std::stack<time_point> time_points_;
};

#endif // !__STOP_WATCH_HPP__