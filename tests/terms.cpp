#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <string>


// Attempt at automatically generating optimal matrix coefficient terms

// < Offset, Coefficient >
using TermMap = std::map<std::intmax_t,std::size_t>;

void PrintTerms (
	const TermMap& Terms
)
{
	for( const auto& Term : Terms)
	{
		std::printf("%4zu*F(n%+2i) ", Term.second,  Term.first);
	}
	std::putchar('\n');
}

const auto IsPow2 = [](std::size_t n) -> bool
{
	return (!(n & (n - 1u)));
};

void ExpandTerm(
	TermMap& Terms,
	std::intmax_t Offset,
	std::size_t TailLimit
)
{
	PrintTerms(Terms);
	const auto IsCoefPow2 = []( const auto& CurTerm ) -> bool
	{
		// Coefficient must be power of two, including zero
		return IsPow2(CurTerm.second);
	};
	const auto IsPrevTerm = []( const auto& CurTerm ) -> bool
	{
		// Offset must be samping from below 0
		return CurTerm.first < 0;
	};

	// First, remove all positive terms
	auto CurTerm = std::find_if_not(Terms.begin(), Terms.end(), IsPrevTerm);
	while(CurTerm != Terms.end())
	{
		const std::intmax_t CurOffset = CurTerm->first;
		const std::intmax_t CurCoef = CurTerm->second;
		Terms[CurOffset - 1] += CurCoef;
		Terms[CurOffset - 2] += CurCoef;
		Terms.erase(CurOffset);
		PrintTerms(Terms);
		CurTerm = std::find_if_not(Terms.begin(), Terms.end(), IsPrevTerm);
	}

	// Second, break it down to work towards power-of-two coefficients
	CurTerm = std::find_if_not(Terms.begin(), Terms.end(), IsCoefPow2);
	while(CurTerm != Terms.end())
	{
		const std::intmax_t CurOffset = CurTerm->first;
		std::intmax_t CurCoef = CurTerm->second;
		const std::intmax_t Direction =
		CurOffset > -static_cast<std::intmax_t>(TailLimit) ? 1: -1;
		while( CurOffset >= 0 || !IsPow2(CurCoef) )
		{
			Terms[CurOffset + (1 * -Direction)] += Direction;
			Terms[CurOffset + (2 * -Direction)] += Direction;
			CurCoef -= Direction;
		}
		Terms.erase(CurOffset);
		PrintTerms(Terms);
		CurTerm = std::find_if_not(Terms.begin(), Terms.end(), IsCoefPow2);
	}
}

int main()
{
	std::cout << std::showpos;
	TermMap Terms;

	// 1 * F(n+3)
	Terms[3] = 1u;

	ExpandTerm(Terms, 3, 4);

	return EXIT_SUCCESS;
}

std::size_t binomial( std::size_t n, std::size_t k )
{
	std::size_t res = 1;

	if( k > n - k )
	{
		k = n - k;
	}

	for( std::size_t i = 0; i < k; ++i )
	{
		res *= (n - i);
		res /= (i + 1);
	}

	return res;
}
