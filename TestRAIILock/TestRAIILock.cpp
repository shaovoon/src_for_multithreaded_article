// TestRAIILock.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <mutex>
#include <thread>
#include "CriticalSection.h"
#include "RAII2CSLock.h"

std::mutex mutA;
std::mutex mutB;

CriticalSection sectionA;
CriticalSection sectionB;

void scoped_lock_example()
{
	std::thread th1([] {
			std::scoped_lock lock(mutA, mutB);
		});

	std::thread th2([] {
			std::scoped_lock lock(mutB, mutA);
		});
	
	th1.join();
	th2.join();
}

void raii_2_lock_example()
{
	std::thread th1([] {
		RAII2CSLock lock(sectionA, sectionB);
		});

	std::thread th2([] {
		RAII2CSLock lock(sectionB, sectionA);
		});

	th1.join();
	th2.join();
}

int main()
{
	//scoped_lock_example();
	raii_2_lock_example();

	std::cout << "Done!\n";

	return 0;
}

