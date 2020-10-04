#ifndef __TEST_CLASS_HPP__
#define __TEST_CLASS_HPP__

#include <iostream>
#include "../core/thread_pool.hpp"
#include "../utilities/stop_watch.hpp"

#define CALL_NOTICE() (std::printf("%s called at thread %d\n", __FUNCTION__, std::this_thread::get_id()));

class test_class {
public:

	test_class() : core(10, 10), tp(5, 1000) {
	}

	~test_class() {
		thread1.join();
		thread2.join();
	}

	void execute() {
		CALL_NOTICE();
		core.add(std::bind(&test_class::runner, this));
		core.add(std::bind(&test_class::getter, this));
	}

	void run(int a, int b) {
		std::printf("Thread %d ran\n", std::this_thread::get_id());
		tp.add(std::bind(&test_class::test_func, this, a, b, 1));
	}

	int get() {
		return tp.pull();
	}

	int runner() {
		for (int i = 0; i < 2; i++) {
			run(i, i + 1);
		}
		return 0;
	}

	int getter() {
		for (int i = 0; i < 2; i++) {
			auto out = tp.pull();
			std::printf("out%d = %d\n", i, out);
		}
		return 0;
	}


private:

	int test_func(int a, int b, int sleep_time) {
		std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
		return a + b;
	}

	std::thread thread1, thread2;
	thread_pool<int> core;
	thread_pool<int> tp;
};

#endif // !__TEST_CLASS_HPP__