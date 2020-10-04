#include "test_class.hpp"

int main(int argc, char** argv)
{
	test_class test;

	while (std::getchar()) {
		test.execute();
	}

	return 0;
}