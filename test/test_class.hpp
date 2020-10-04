#ifndef __TEST_CLASS_HPP__
#define __TEST_CLASS_HPP__

#include <iostream>
#include "../core/thread_pool.hpp"
#include "../utilities/stop_watch.hpp"

#define CALL_NOTICE() (std::cout << __FUNCTION__ << " called at thread " << std::this_thread::get_id() << std::endl)

class test_class {
public:

	test_class() : producer(1, 10, true), consumer(2, 10, false) {
	}

	void execute() {
		CALL_NOTICE();
		producer.add(std::bind(&test_class::runner, this));
		consumer.add(std::bind(&test_class::getter, this));
	}

	int runner() {
		return test_func(1, 2, 100);
	}

	int getter() {
		auto out = producer.pull();
		std::printf("out = %d\n", out);
		return 0;
	}

private:

	int test_func(int a, int b, int sleep_time) {
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
		return a + b;
	}

	thread_pool<int> producer;
	thread_pool<int> consumer;
};

#endif // !__TEST_CLASS_HPP__