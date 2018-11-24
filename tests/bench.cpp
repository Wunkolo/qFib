#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <iomanip>

#include <memory>
#include <limits>
#include <type_traits>

#include <immintrin.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include "TestTools.hpp"

struct FibMethod
{
	~FibMethod(){}
	// Mostly used to protect against how bad the 
	// recursive algorithm performs
	virtual std::size_t Limit() const
	{
		return std::numeric_limits<std::size_t>::max();
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
// Similar to the last one, but without the stack-based array
// allowing this to be register-only.
// Causes "cmove,cmovne" to be emitted in gcc
struct Stack2Reg : FibMethod
{
	const char* GetName() const override
	{
		return "2-Stack-Register";
	}
	std::uint64_t operator()(std::uint64_t n) override
	{
		std::uint64_t Val1, Val2;
		Val1 = Val2 = 1U;

		while( n-- > 2 )
		{
			(n & 1 ? Val2:Val1) = Val1 + Val2;
		}
		return Val1;
	}
};

struct MatrixExp : FibMethod
{
	const char* GetName() const override
	{
		return "Matrix Exponent";
	}
	std::uint64_t operator()(std::uint64_t n) override
	{
		const glm::mat<2,2,glm::u64,glm::aligned> Q(1,1,1,0);
		glm::mat<2,2,glm::u64,glm::aligned> P = Q;

		for( std::size_t i = 2; i < n; ++i )
		{
			P *= Q;
		}

		return P[0][0];
	}
};

struct ChunMin : FibMethod
{
	const char* GetName() const override
	{
		return "Chun-Min Chang";
		//https://chunminchang.github.io/blog/post/calculating-fibonacci-numbers-by-fast-doubling
	}

	std::uint64_t operator()(std::uint64_t n) override
	{
		// The position of the highest bit of n.
		// So we need to loop `h` times to get the answer.
		// Example: n = (Dec)50 = (Bin)00110010, then h = 6.
		//                               ^ 6th bit from right side
		#ifdef _MSC_VER
		const std::uint64_t h = 64 - __lzcnt64(n);
		#else
		const std::uint64_t h = 64 - __builtin_clzll(n);
		#endif
		// for( unsigned int i = n ; i ; ++h, i >>= 1 );

		std::uint64_t a = 0; // F(0) = 0
		std::uint64_t b = 1; // F(1) = 1
		// There is only one `1` in the bits of `mask`. The `1`'s position is same as
		// the highest bit of n(mask = 2^(h-1) at first), and it will be shifted right
		// iteratively to do `AND` operation with `n` to check `n_j` is odd or even,
		// where n_j is defined below.
		for( std::uint64_t mask = 1 << (h - 1); mask; mask >>= 1 )
		{ // Run h times!
		// Let j = h-i (looping from i = 1 to i = h), n_j = floor(n / 2^j) = n >> j
		// (n_j = n when j = 0), k = floor(n_j / 2), then a = F(k), b = F(k+1) now.
			std::uint64_t c = a * (2 * b - a); // F(2k) = F(k) * [ 2 * F(k+1) – F(k) ]
			std::uint64_t d = a * a + b * b;   // F(2k+1) = F(k)^2 + F(k+1)^2

			if( mask & n )
			{ // n_j is odd: k = (n_j-1)/2 => n_j = 2k + 1
				a = d;        //   F(n_j) = F(2k + 1)
				b = c + d;    //   F(n_j + 1) = F(2k + 2) = F(2k) + F(2k + 1)
			}
			else
			{ // n_j is even: k = n_j/2 => n_j = 2k
				a = c;        //   F(n_j) = F(2k)
				b = d;        //   F(n_j + 1) = F(2k + 1)
			}
		}

		return a;
	}
};

#if defined(__AVX512F__) || defined(_MSC_VER)
struct ChunMinAVX512 : FibMethod
{
	const char* GetName() const override
	{
		return "Chun-Min - AVX512";
	}

	std::uint64_t operator()(std::uint64_t n) override
	{
		#ifdef _MSC_VER
		const std::uint64_t h = 64 - __lzcnt64(n);
		#else
		const std::uint64_t h = 64 - __builtin_clzll(n);
		#endif

		__m128i ab = _mm_set_epi64x(1,0);
		for( std::uint64_t mask = 1 << (h - 1); mask; mask >>= 1 )
		{
			const __m128i ab_sq = _mm_mullo_epi64( ab, ab );
			const __m128i cd = _mm_add_epi64(
				_mm_mullo_epi64(
					_mm_mullo_epi64(
						ab,
						_mm_shuffle_epi32(ab,0b11'10'11'10)
					),
					_mm_set_epi64x( 1, 2 )
				),
				_mm_mullo_epi64(
					_mm_broadcastq_epi64(ab_sq),
					_mm_set_epi64x(1,-1)
				)
			);
			const __m128i cd_sum = _mm_add_epi64(
				cd,
				_mm_alignr_epi64(
					cd,
					cd,
					1
				)
			);
			if( mask & n )
			{
				ab = _mm_permutex2var_epi64(
					cd,
					_mm_set_epi64x(
						2,1
					),
					cd_sum
				);
			}
			else
			{
				ab = cd;
			}
		}

		return _mm_extract_epi64(ab,0);
	}
};
#endif
}

const static std::unique_ptr<FibMethod> FibMethods[] = {
	std::make_unique<Methods::Recursive>(),
	std::make_unique<Methods::Stack2>(),
	std::make_unique<Methods::Stack2Reg>(),
	std::make_unique<Methods::MatrixExp>(),
	std::make_unique<Methods::ChunMin>(),
#if defined(__AVX512F__) || defined(_MSC_VER)
	std::make_unique<Methods::ChunMinAVX512>(),
#endif
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
	return EXIT_SUCCESS;
}
