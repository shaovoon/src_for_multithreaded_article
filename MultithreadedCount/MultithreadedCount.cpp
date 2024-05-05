#include <iostream>
#include <numeric>
#include <execution>
#include <vector>
#include <list>
#include <mutex>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <cstring>
#include "timer.h"
#include "parallel_for_each.h"

void init_vector();
bool is_valid(int n);
void inc_mutex();
void inc_atomic();
#ifdef _WIN32
void inc_winInterlocked();
#endif
void inc_no_lock();
void inc_single_thread();
void inc_less_mutex_lock();

struct CountStruct
{
	CountStruct() : Count(0)
	{
		memset((void*)buf, 0, sizeof(buf));
	}
	int Count;
	char buf[128]; // to prevent false sharing
};

int main()
{
	std::cout << "Running Benchmark. Please wait...\n\n";

	init_vector();
	inc_mutex();
	inc_atomic();
#ifdef _WIN32
	inc_winInterlocked();
#endif
	inc_no_lock();
	inc_single_thread();
	inc_less_mutex_lock();

	std::cout << "\nPlease ignore total count output!\nIt is to prevent optimizer from doing away with\nfor loops if count is not used afterwards.\n";
	std::cout << "Done!\n";
	return 0;
}

const size_t VEC_SIZE = 100000000U;
std::vector<size_t> Vec;

void init_vector()
{
	srand(time(NULL));
	for (size_t i = 0; i < VEC_SIZE; ++i)
		Vec.push_back(rand() % 20);
}

bool is_valid(int n)
{
	return (n % 2 == 0);
}

void inc_atomic()
{
	timer watch;
	watch.start("inc atomic");

	std::atomic<int> count = 0;

	std::for_each(std::execution::par, Vec.begin(), Vec.end(),
		[&count](size_t num) -> void
		{
			if (is_valid((int)(num)))
				++count;
		});

	watch.stop();

	std::cout << "total count:" << count << std::endl;
}

void inc_mutex()
{
	timer watch;
	watch.start("inc mutex");

	int count = 0;
	std::mutex mut;

	std::for_each(std::execution::par, Vec.begin(), Vec.end(),
		[&mut, &count](size_t num) -> void
		{
			if (is_valid((int)(num)))
			{
				std::lock_guard<std::mutex> guard(mut);
				++count;
			}
		});

	watch.stop();

	std::cout << "total count:" << count << std::endl;
}

void inc_less_mutex_lock()
{
	timer watch;
	watch.start("inc less mutex lock");

	const size_t threads = std::thread::hardware_concurrency();
	std::vector<size_t> vecIndex;
	for (size_t i = 0; i < threads; ++i)
		vecIndex.push_back(i);

	int count = 0;
	std::mutex mut;

	std::for_each(std::execution::par, vecIndex.begin(), vecIndex.end(),
		[&mut, &count, threads](size_t index) -> void
		{
			size_t thunk = Vec.size() / threads;
			size_t start = thunk * index;
			size_t end = start + thunk;
			if (index == (threads - 1))
			{
				size_t remainder = Vec.size() % threads;
				end += remainder;
			}
			int temp_count = 0;
			for (int i = start; i < end; ++i)
			{
				if (is_valid((int)(Vec[i])))
				{
					++temp_count;
				}
			}
			{
				std::lock_guard<std::mutex> guard(mut);
				count += temp_count;
			}
		});

	watch.stop();

	std::cout << "total count:" << count << std::endl;
}

#ifdef _WIN32
void inc_winInterlocked()
{
	timer watch;
	watch.start("inc win Interlocked");

	LONG count = 0;

	std::for_each(std::execution::par, Vec.begin(), Vec.end(),
		[&count](size_t num) -> void
		{
			if (is_valid((int)(num)))
				::InterlockedIncrement(&count);
		});

	watch.stop();

	std::cout << "total count:" << count << std::endl;
}
#endif

void inc_no_lock()
{
	int threads = std::thread::hardware_concurrency();
	std::vector<CountStruct> count(threads);

	timer watch;
	watch.start("inc no lock");

	loop::parallel_for(threads, (size_t)(0), (size_t)(VEC_SIZE),
		[&count](int threadIndex, int index) -> void
		{
			if (is_valid(Vec[index]))
				++(count[threadIndex].Count);
		});

	int total = 0;
	for (auto& st : count)
		total += st.Count;

	watch.stop();

	std::cout << "total count:" << total << std::endl;
}

void inc_single_thread()
{
	timer watch;
	watch.start("inc single_thread");

	int count = 0;

	for (size_t i = 0; i < Vec.size(); ++i)
	{
		if (is_valid(Vec[i]))
			++count;
	}

	watch.stop();

	std::cout << "total count:" << count << std::endl;
}
