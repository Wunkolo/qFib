#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <iomanip>

#include <memory>
#include <type_traits>

#include <immintrin.h>

#include "TestTools.hpp"

struct FibMethod
{
	virtual const char* GetName() const = 0;
	virtual std::uint64_t operator()(std::uint64_t n) = 0;
};

namespace Methods
{
struct Recursive : FibMethod
{
	const char* GetName() const override
	{
		return "Recursive";
	}
	std::uint64_t operator()( std::uint64_t n ) override
	{
		return Fib(n);
	}

	static std::uint64_t Fib(std::uint64_t n)
	{
		if( n == 0 || n == 1 )
		{
			return n;
		}
		else
		{
			return (Fib(n - 1) + Fib(n - 2));
		}
	}
};

struct Stack2 : FibMethod
{
	const char* GetName() const override
	{
		return "2-Stack";
	}
	std::uint64_t operator()(std::uint64_t n) override
	{
		std::uint64_t Stack[2] = { 1, 1 };

		while( n-- > 2 )
		{
			Stack[n & 1] = Stack[0] + Stack[1];
		}
		return Stack[0];
	}
};
}

const static std::unique_ptr<FibMethod> FibMethods[] = {
	std::make_unique<Methods::Recursive>(),
	std::make_unique<Methods::Stack2>()
};

int main()
{
	std::cout << std::fixed << std::setprecision(2);
	std::cout << GetProcessorBrandString() << std::endl;

	// Print table headers
	std::cout << "n\t|";
	for( std::size_t i = 0; i < std::extent<decltype(FibMethods)>::value; ++i )
	{
		std::cout << std::setw(12) << FibMethods[i]->GetName();
	}
	std::cout << std::endl;


	for( std::uint64_t n = 0; n < 12; ++n )
	{
		std::cout << n << "\t|";
		for( std::size_t i = 0; i < std::extent<decltype(FibMethods)>::value; ++i )
		{
			const auto BenchMethod = [](std::size_t i, std::uint64_t n) -> std::uint64_t
			{
				return (*FibMethods[i])(n);
			};

			const auto BenchResult = Bench<>::BenchResult(BenchMethod, i, n);
			const auto Time        = std::get<0>(BenchResult).count();
			const auto Value       = std::get<1>(BenchResult);

			std::cout << std::setw(12) << Time;
		}
		std::cout << std::endl;
	}

	std::cin.get();
	return EXIT_SUCCESS;
}