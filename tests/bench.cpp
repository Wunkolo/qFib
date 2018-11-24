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
	~FibMethod(){}
	// Mostly used to protect against how bad the 
	// recursive algorithm performs
	virtual std::size_t Limit() const
	{
		return -1U;
	}
	virtual const char* GetName() const = 0;
	virtual std::uint64_t operator()(std::uint64_t n) = 0;
};

namespace Methods
{
struct Recursive : FibMethod
{
	std::size_t Limit() const override
	{
		return 23;
	}
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


#define ColumnWidth 18
int main()
{
	std::cout << std::fixed << std::setprecision(2);
	std::cout << GetProcessorBrandString() << std::endl;

	// Print table headers
	std::cout << "n\t|";
	for( std::size_t i = 0; i < std::extent<decltype(FibMethods)>::value; ++i )
	{
		std::cout << std::setw(ColumnWidth) << FibMethods[i]->GetName();
	}
	std::cout << std::endl;


	for( std::uint64_t n = 0; n < std::extent<decltype(FibMod64)>::value; ++n )
	{
		std::cout << n << "\t|";
		for( std::size_t i = 0; i < std::extent<decltype(FibMethods)>::value; ++i )
		{
			// This is just here to protect against the massive runtime of the recursive method
			if( n >= FibMethods[i]->Limit() )
			{
				std::cout << std::setw(ColumnWidth) << "---|";
				continue;
			}
			const auto BenchResult = Bench<>::BenchResult( *FibMethods[i], n );
			const auto Time        = std::get<0>(BenchResult).count();
			const auto Value       = std::get<1>(BenchResult);

			std::cout 
				// Verify
				<< (Value == FibMod64[n] ? "\033[1;32m\u2714":"\033[1;31m\u2717")
				<< "\033[0m"
				<< ':'
				// Timing
				<< std::setw(ColumnWidth - 3) << Time << '|';
		}
		std::cout << std::endl;
	}

	std::cin.get();
	return EXIT_SUCCESS;
}
