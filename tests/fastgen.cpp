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

#include "Bench.hpp"

using VectorT = glm::vec<4, glm::u64,glm::qualifier::packed_highp>;
using MatrixT = glm::mat<4, 4, glm::u64,glm::qualifier::packed_highp>;

void Matrix()
{
	VectorT FibState(2,1,1,0);

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

int main()
{
	std::cout << std::fixed << std::setprecision(2);
	Matrix();

	return EXIT_SUCCESS;
}
