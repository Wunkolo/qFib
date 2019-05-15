#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <string>

// < Offset, Coefficient >
using TermMap = std::map<std::intmax_t,std::size_t>;

void ExpandTerm(
	TermMap& Terms,
	std::intmax_t Term
)
{
	// Split term into two lower terms
	// F(n) = F(n - 1) + F(n - 2)
	Terms[Term - 1] += Terms[Term];
	Terms[Term - 2] += Terms[Term];
	Terms.erase(Term);
	const auto TermFail = std::find_if_not(
		Terms.begin(),
		Terms.end(),
		[]( const auto& CurTerm ) -> bool
		{
			const bool IsPowerTwo = !(CurTerm.second & (CurTerm.second - 1U));
			const std::intmax_t Upper = -1;
			const std::intmax_t Lower = -3;
			const bool IsWithinRange = CurTerm.first < 0;
			return IsPowerTwo && IsWithinRange;
		}
	);
	if( TermFail != Terms.end() )
	{
		ExpandTerm(
			Terms,
			TermFail->first
		);
	}
}

int main()
{
	std::cout << std::showpos;
	TermMap Terms;

	// 1 * F(n+3)
	Terms[3] = 1;

	ExpandTerm(
		Terms,
		3
	);

	for( const auto& CurTerm : Terms )
	{
		std::cout << CurTerm.second << "*F(n" << CurTerm.first << "), ";
	}

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
