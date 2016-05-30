#include <stack.h>

#include <iostream>
#include <thread>


static const unsigned int nbMessage = 100000;
static const unsigned int nbThreads = 10;

void readerThread(lockFree::stack<int> *s)
{
	unsigned int j = 0;
	unsigned int i = 0;
	while (i < nbMessage) {
		try {
			j += s->pop();
			i++;
			if (j % 5 == 0)
				std::cout << "." << std::flush;
		}
		catch (std::exception &e) {
			std::cout << "*" << std::flush;
		}
	}
	std::cout << std::endl;
}

void writerThread(lockFree::stack<int> *s)
{
	for (unsigned int i = 0; i < nbMessage; i++) {
		s->push(1);
		if (i % 5 == 0)
			std::cout << "+" << std::flush;
	}
}

static void test_lock_free_stack(void)
{
	lockFree::stack<int> s;
	std::unique_ptr<std::thread> writer[nbThreads];
	std::unique_ptr<std::thread> reader[nbThreads];

	std::cout << "test lock free stack: ";

	for (unsigned int i = 0; i < nbThreads; i++) {
		writer[i].reset(new std::thread(writerThread, &s));
		reader[i].reset(new std::thread(readerThread, &s));
	}

	for (unsigned int i = 0; i < nbThreads; i++) {
		writer[i]->join();
		reader[i]->join();
	}

	if (s.isEmpty())
		std::cout << "OK" << std::endl;
	else
		std::cout << "KO" << std::endl;


}

int main() {
	test_lock_free_stack();
	return EXIT_SUCCESS;
}
