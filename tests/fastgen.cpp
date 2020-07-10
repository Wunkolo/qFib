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

int main()
{
	std::cout << std::fixed << std::setprecision(2);
	Matrix();
	std::puts("---------");
	MatrixSIMD();
	std::puts("---------");
	TermSIMD4(29108);

	return EXIT_SUCCESS;
}
