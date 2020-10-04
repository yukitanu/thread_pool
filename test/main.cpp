#include "test_class.hpp"

int main(int argc, char** argv)
{
	test_class test;
	int counter = 0;

	while (std::getchar()) {
		test.execute();
		counter++;
		std::cout << counter << std::endl;
	}

	return 0;
}