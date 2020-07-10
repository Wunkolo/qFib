#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <iomanip>

#include <memory>
#include <limits>
#include <type_traits>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <immintrin.h>

#include "Bench.hpp"

using VectorT = glm::vec<4, glm::u32,glm::qualifier::packed_highp>;
using MatrixT = glm::mat<4, 4, glm::u32,glm::qualifier::packed_highp>;

void Matrix()
{
	VectorT FibState(2, 1, 1, 0);

	const MatrixT NextState(
		4, 1, 2, 1,
		4, 4, 1, 1,
		1, 2, 0, 0,
		0, 0, 0, 0
	);

	std::cout
		<< std::setw(8) << 0 << ':' << std::setw(32) << FibState.w << '\n'
		<< std::setw(8) << 1 << ':' << std::setw(32) << FibState.z << '\n'
		<< std::setw(8) << 2 << ':' << std::setw(32) << FibState.y << '\n'
		<< std::setw(8) << 3 << ':' << std::setw(32) << FibState.x << '\n';

	for( std::size_t i = 0; i < 300; i += 4 )
	{
		auto BenchResult = Bench<>::BenchResult(
			std::multiplies<>(),
			NextState,
			FibState
		);
		FibState = std::get<1>(BenchResult);
		std::cout
			<< std::get<0>(BenchResult).count() << "ns | \n"
			<< std::setw(8) << (i + 0) << ':' << std::setw(32) << FibState.w << '\n'
			<< std::setw(8) << (i + 1) << ':' << std::setw(32) << FibState.z << '\n'
			<< std::setw(8) << (i + 2) << ':' << std::setw(32) << FibState.y << '\n'
			<< std::setw(8) << (i + 3) << ':' << std::setw(32) << FibState.x << '\n';
	}
}

void MatrixSIMD()
{
	__m128i FibState = _mm_set_epi32(2, 1, 1, 0);


	std::cout
		<< std::setw(8) << 0 << ':' << std::setw(32) << _mm_extract_epi32(FibState,0) << '\n'
		<< std::setw(8) << 1 << ':' << std::setw(32) << _mm_extract_epi32(FibState,1) << '\n'
		<< std::setw(8) << 2 << ':' << std::setw(32) << _mm_extract_epi32(FibState,2) << '\n'
		<< std::setw(8) << 3 << ':' << std::setw(32) << _mm_extract_epi32(FibState,3) << '\n';

	const __m128i NextState[3] = {
		//_mm_set_epi32(~0, ~0, ~0, ~0), same as multiplying vector by <0>
		_mm_set_epi32( 0,  1, ~0, ~0),
		_mm_set_epi32( 2,  2,  0,  0),
		_mm_set_epi32( 2,  0,  1,  0)
	};

	for( std::size_t i = 0; i < 300; i += 4 )
	{
		const auto Start = std::chrono::high_resolution_clock::now();
		__m128i Result = _mm_setzero_si128();
		FibState = _mm_alignr_epi8(FibState, FibState, 4);
		for( std::size_t j = 0; j < 3; ++j )
		{
			const __m128i Product = _mm_sllv_epi32(
				_mm_broadcastd_epi32(FibState), NextState[j]
			);
			FibState = _mm_alignr_epi8(_mm_setzero_si128(), FibState, 4);
			Result = _mm_add_epi32(Result, Product);
		}
		const auto Stop = std::chrono::high_resolution_clock::now();
		FibState = Result;
		std::cout
			<< (Stop - Start).count() << "ns |\n"
			<< std::setw(8) << (i + 0) << ':' << std::setw(32) << (std::uint32_t)_mm_extract_epi32(FibState,0) << '\n'
			<< std::setw(8) << (i + 1) << ':' << std::setw(32) << (std::uint32_t)_mm_extract_epi32(FibState,1) << '\n'
			<< std::setw(8) << (i + 2) << ':' << std::setw(32) << (std::uint32_t)_mm_extract_epi32(FibState,2) << '\n'
			<< std::setw(8) << (i + 3) << ':' << std::setw(32) << (std::uint32_t)_mm_extract_epi32(FibState,3) << '\n';
	}
}

// Get the Nth fib term
void TermSIMD4(std::uint32_t N)
{
	__m128i FibState = _mm_set_epi32(2, 1, 1, 0);

	const __m128i NextState[3] = {
		//_mm_set_epi32(~0, ~0, ~0, ~0), same as multiplying vector by <0>
		_mm_set_epi32( 0,  1, ~0, ~0),
		_mm_set_epi32( 2,  2,  0,  0),
		_mm_set_epi32( 2,  0,  1,  0)
	};

	const auto Start = std::chrono::high_resolution_clock::now();
	for( std::size_t i = 0; i < N/4; ++i )
	{
		const __m128i Product0 = _mm_sllv_epi32(
			_mm_broadcastd_epi32(_mm_alignr_epi8(_mm_setzero_si128(), FibState, 4)), NextState[0]
		);
		const __m128i Product1 = _mm_sllv_epi32(
			_mm_broadcastd_epi32(_mm_alignr_epi8(_mm_setzero_si128(), FibState, 8)), NextState[1]
		);
		const __m128i Product2 = _mm_sllv_epi32(
			_mm_broadcastd_epi32(_mm_alignr_epi8(_mm_setzero_si128(), FibState,12)), NextState[2]
		);
		// Throughput _mm_add_epi32 is 0.33 since Skylake
		const __m128i Result = _mm_add_epi32(
			_mm_add_epi32(Product0, Product1),
			Product2
		);
		FibState = Result;
	}
	const auto Stop = std::chrono::high_resolution_clock::now();
	std::uint32_t Results[4];
	_mm_store_si128((__m128i*)Results, FibState);
	std::cout
		<< (Stop - Start).count() << "ns |\n"
		<< std::setw(8) << N << ':' << std::setw(32) << Results[N % 4] << '\n';
}

void cmchang(uint32_t n)
{
	const auto Start = std::chrono::high_resolution_clock::now();
	// The position of the highest bit of n.
	// So we need to loop `h` times to get the answer.
	// Example: n = (Dec)50 = (Bin)00110010, then h = 6.
	//                               ^ 6th bit from right side
	//uint32_t h = 0;
	//for (uint32_t i = n ; i ; ++h, i >>= 1);
	const uint32_t h = 32 - __builtin_clz(n);

	uint32_t a = 0; // F(0) = 0
	uint32_t b = 1; // F(1) = 1
	// There is only one `1` in the bits of `mask`. 
	// The `1`'s position is same as
	// the highest bit of n(mask = 2^(h-1) at first), 
	// and it will be shifted right iteratively to do 
	// `AND` operation with `n` to check `n / 2^j` is odd
	// or even.
	for (uint32_t mask = 1 << (h - 1) ; mask ; mask >>= 1) {
		// Let j = h-i (looping from i = 1 to i = h),
		// n_j = floor(n / 2^j) = n >> j (n_j = n when j = 0), 
		// k = floor(n_j / 2), then a = F(k), b = F(k+1) now.
		//const uint32_t c = a * (2 * b - a); // F(2k)=F(k) * [2*F(k+1)–F(k)]
		const uint32_t c = 2 * a * b - a * a; // F(2k)=F(k) * [2*F(k+1)–F(k)]
		const uint32_t d = a * a + b * b;   // F(2k+1) = F(k)^2 + F(k+1)^2

		const bool odd = mask & n;
		a = odd ? d : c;
		b = odd ? c + d : d;
	}

	const auto Stop = std::chrono::high_resolution_clock::now();
	std::cout
		<< (Stop - Start).count() << "ns |\n"
		<< std::setw(8) << n << ':' << std::setw(32) << a << '\n';
}

int main()
{
	std::cout << std::fixed << std::setprecision(2);
	Matrix();
	std::puts("---------");
	MatrixSIMD();
	std::puts("---------");
	TermSIMD4(29108);
	cmchang(29108);

	return EXIT_SUCCESS;
}
