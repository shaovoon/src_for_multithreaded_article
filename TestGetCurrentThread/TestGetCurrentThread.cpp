#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

int main()
{
	std::cout << "GetCurrentProcess(): " << 
		(std::ptrdiff_t)::GetCurrentProcess() << "\n";

	std::cout << "GetCurrentThread(): " << 
		(std::ptrdiff_t)::GetCurrentThread() << "\n";
	std::cout << "Done!\n";
}
