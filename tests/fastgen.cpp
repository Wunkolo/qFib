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

int main()
{
	std::cout << std::fixed << std::setprecision(2);

	glm::u64vec4 FibState(2,1,1,0);

	glm::mat<4,4,glm::u64,glm::aligned_highp> NextState(
		4, 1, 2, 1,
		4, 4, 1, 1,
		1, 2, 0, 0,
		0, 0, 0, 0
	);

	for( std::size_t i = 0; i < 300; i += 4 )
	{
		std::cout
			<< std::setw(8) << (i + 0) << ':' << std::setw(32) << FibState.w << '\n'
			<< std::setw(8) << (i + 1) << ':' << std::setw(32) << FibState.z << '\n' 
			<< std::setw(8) << (i + 2) << ':' << std::setw(32) << FibState.y << '\n' 
			<< std::setw(8) << (i + 3) << ':' << std::setw(32) << FibState.x << '\n'; 

		// This matrix multiplication uses nice and clean powers of two
		// allowing the compiler to hopefully optimize the matrix multiplication
		// into a pipelined sequence of 'lea' instructions:
		// Such as ' lea r__, qword [r__ + r__ * {0,2,4}] '
		FibState = NextState * FibState;
	}

	return EXIT_SUCCESS;
}
