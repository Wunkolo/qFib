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

	MatrixT NextState(
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

	const __m128i NextState[4] = {
		_mm_set_epi32( 2,  0,  1,  0),
		_mm_set_epi32( 2,  2,  0,  0),
		_mm_set_epi32( 0,  1, ~0, ~0),
		_mm_set_epi32(~0, ~0, ~0, ~0)
	};

	std::cout
	   << std::setw(8) << 0 << ':' << std::setw(32) << _mm_extract_epi32(FibState,0) << '\n'
	   << std::setw(8) << 1 << ':' << std::setw(32) << _mm_extract_epi32(FibState,1) << '\n'
	   << std::setw(8) << 2 << ':' << std::setw(32) << _mm_extract_epi32(FibState,2) << '\n'
	   << std::setw(8) << 3 << ':' << std::setw(32) << _mm_extract_epi32(FibState,3) << '\n';

	// Integer matrix multiply
	const auto Multiply4SIMD = [](const __m128i Matrix[4], const  __m128i Vector) -> __m128i
	{
		__m128i Result = _mm_setzero_si128();
		for( std::size_t i = 0; i < 4; ++i )
		{
			// Dot Product
			const __m128i Product = _mm_sllv_epi32(
				_mm_shuffle_epi32(
					Vector,
					0b00'00'00'00'01'01'01'01'10'10'10'10'11'11'11'11 >> i * 8
				),
				Matrix[i]
			);
			Result = _mm_add_epi32(Result, Product);
		}
		return Result;
	};

	for( std::size_t i = 0; i < 300; i += 4 )
	{
		auto BenchResult = Bench<>::BenchResult(
			Multiply4SIMD,
			NextState,
			FibState
		);
		FibState = std::get<1>(BenchResult);
		std::cout
			<< std::get<0>(BenchResult).count() << "ns | \n"
			<< std::setw(8) << (i + 0) << ':' << std::setw(32) << (unsigned)_mm_extract_epi32(FibState,0) << '\n'
	   		<< std::setw(8) << (i + 1) << ':' << std::setw(32) << (unsigned)_mm_extract_epi32(FibState,1) << '\n'
	   		<< std::setw(8) << (i + 2) << ':' << std::setw(32) << (unsigned)_mm_extract_epi32(FibState,2) << '\n'
	   		<< std::setw(8) << (i + 3) << ':' << std::setw(32) << (unsigned)_mm_extract_epi32(FibState,3) << '\n';
	}
}

int main()
{
	std::cout << std::fixed << std::setprecision(2);
	Matrix();
	std::puts("---------");
	MatrixSIMD();

	return EXIT_SUCCESS;
}
