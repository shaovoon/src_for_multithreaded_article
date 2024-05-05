// The MIT License (MIT)
// Parallel For Each with Index 1.0.0
// Copyright (C) 2023 by Shao Voon Wong (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT

#pragma once
#include <memory>
#include <thread>
#include <vector>
#include <iterator>

namespace loop
{
	template<typename ForwardIt, typename Func>
	void serial_for_each(ForwardIt begin, ForwardIt end, Func f)
	{
		for(;begin != end; ++begin)
			f(*begin);
	}

	template<typename ForwardIt, typename Func>
	void serial_for_each_index(ForwardIt begin, ForwardIt end, Func f)
	{
		auto count = std::distance(begin, end);
		for (int i = 0; i < count; ++i, ++begin)
		{
			f(*begin, 0, i);
		}
	}

	template<typename Func>
	void serial_for(size_t beginIndex, size_t endIndex, Func f)
	{
		for (size_t i = beginIndex; i < endIndex; ++i)
		{
			f(0, (int)(i));
		}
	}

	template<typename ForwardIt, typename Func>
	void parallel_for_each(int numOfThreads, ForwardIt begin, ForwardIt end, Func f)
	{
		if (numOfThreads == -1)
			numOfThreads = static_cast<int>(std::thread::hardware_concurrency());

		auto count = std::distance(begin, end);

		if ((int)(count) < numOfThreads)
			numOfThreads = (int)(count);

		if (numOfThreads == 0)
			return;

		if (numOfThreads == 1)
		{
			serial_for_each(begin, end, f);
			return;
		}

		const int elementPerThread = (int)(count) / numOfThreads;
		const int remainder = (int)(count) % numOfThreads;
		int beginIndex = 0;
		int elementCount = elementPerThread;

		std::vector<std::shared_ptr<std::thread>> threads;
		for (int j = 0; j < numOfThreads; ++j)
		{
			if (j == numOfThreads - 1)
				elementCount = elementPerThread + remainder;
			std::shared_ptr<std::thread> threadPtr =
				std::make_shared<std::thread>(
					[begin, beginIndex, elementCount, f]() -> void {
						ForwardIt it = begin;
						std::advance(it, beginIndex);
						for (int i = 0; i < elementCount; ++i, ++it)
						{
							f(*it);
						}
					});
			threads.emplace_back(threadPtr);
			beginIndex += elementPerThread;
		}

		for (auto& threadPtr : threads)
			threadPtr->join();
	}

	template<typename ForwardIt, typename Func>
	void parallel_for_each_index(int numOfThreads, ForwardIt begin, ForwardIt end, Func f)
	{
		if (numOfThreads == -1)
			numOfThreads = static_cast<int>(std::thread::hardware_concurrency());

		auto count = std::distance(begin, end);

		if ((int)(count) < numOfThreads)
			numOfThreads = (int)(count);

		if (numOfThreads == 0)
			return;

		if (numOfThreads == 1)
		{
			serial_for_each_index(begin, end, f);
			return;
		}

		const int elementPerThread = (int)(count) / numOfThreads;
		const int remainder = (int)(count) % numOfThreads;
		int beginIndex = 0;
		int elementCount = elementPerThread;

		std::vector<std::shared_ptr<std::thread>> threads;
		for (int j = 0; j < numOfThreads; ++j)
		{
			if (j == numOfThreads - 1)
				elementCount = elementPerThread + remainder;

			std::shared_ptr<std::thread> threadPtr =
				std::make_shared<std::thread>(
					[begin, beginIndex, elementCount, j, f]() -> void {
						ForwardIt it = begin;
						std::advance(it, beginIndex);
						for (int i = 0; i < elementCount; ++i, ++it)
						{
							f(*it, j, beginIndex + i);
						}
					});
			threads.emplace_back(threadPtr);
			beginIndex += elementPerThread;
		}

		for (auto& threadPtr : threads)
			threadPtr->join();
	}

	template<typename Func>
	void parallel_for(int numOfThreads, size_t beginIndex, size_t endIndex, Func f)
	{
		if (numOfThreads == -1)
			numOfThreads = static_cast<int>(std::thread::hardware_concurrency());

		auto count = endIndex - beginIndex;

		if ((int)(count) < numOfThreads)
			numOfThreads = (int)(count);

		if (numOfThreads == 0)
			return;

		if (numOfThreads == 1)
		{
			serial_for(beginIndex, endIndex, f);
			return;
		}

		const int elementPerThread = (int)(count) / numOfThreads;
		const int remainder = (int)(count) % numOfThreads;
		int elementCount = elementPerThread;

		std::vector<std::shared_ptr<std::thread>> threads;
		for (int j = 0; j < numOfThreads; ++j)
		{
			if (j == numOfThreads - 1)
				elementCount = elementPerThread + remainder;

			std::shared_ptr<std::thread> threadPtr =
				std::make_shared<std::thread>(
					[beginIndex, elementCount, j, f]() -> void {
						for (int i = (int)(beginIndex); i < (int)(beginIndex) + elementCount; ++i)
						{
							f(j, i);
						}
					});
			threads.emplace_back(threadPtr);
			beginIndex += elementPerThread;
		}

		for (auto& threadPtr : threads)
			threadPtr->join();
	}

} // ns loop