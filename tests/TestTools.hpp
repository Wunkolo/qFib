#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <array>
#include <chrono>
#include <tuple>

#ifdef _WIN32
#include <intrin.h>
#define NOMINMAX
#include <Windows.h>
// Statically enables "ENABLE_VIRTUAL_TERMINAL_PROCESSING" for the terminal
// at runtime to allow for unix-style escape sequences. 
static const bool _WndV100Enabled = []() -> bool
{
	const auto Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD ConsoleMode;
	GetConsoleMode(
		Handle,
		&ConsoleMode
	);
	SetConsoleMode(
		Handle,
		ConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING
	);
	GetConsoleMode(
		Handle,
		&ConsoleMode
	);
	return ConsoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING;
}();


std::string GetProcessorBrandString()
{
	std::string Model;
	Model.reserve(48);

	for( std::uint32_t i = 0x80000002; i < 0x80000005; ++i )
	{
		std::array<std::uint32_t, 4> CPUData;
		__cpuid(
			reinterpret_cast<int*>(&CPUData[0]),
			i
		);
		for( const std::uint32_t& Word : CPUData )
		{
			Model.append(
				reinterpret_cast<const char*>(&Word),
				4
			);
		}
	}

	return Model;
}
#else
#include <cpuid.h>
std::string GetProcessorBrandString()
{
	std::string Model;
	Model.reserve(48);

	for( std::size_t i = 0x80000002; i < 0x80000005; ++i )
	{
		std::array<std::uint32_t, 4> CPUData;
		__get_cpuid(
			i,
			&CPUData[0], &CPUData[1],
			&CPUData[2], &CPUData[3]
		);
		for( const std::uint32_t& Word : CPUData )
		{
			Model.append(
				reinterpret_cast<const char*>(&Word),
				4
			);
		}
	}

	return Model;
}

#endif

template< typename TimeT = std::chrono::nanoseconds >
struct Bench
{

	// Returns a tuple of (Time, Function return value)
	template< typename FunctionT, typename ...ArgsT >
	static std::tuple<
		TimeT,
		typename std::result_of<FunctionT(ArgsT...)>::type
	> BenchResult(
		FunctionT&& Func,
		ArgsT&&... Arguments
	)
	{
		const auto Start = std::chrono::high_resolution_clock::now();

		typename std::result_of<FunctionT(ArgsT...)>::type ReturnValue = std::forward<FunctionT>(Func)(
			std::forward<ArgsT>(Arguments)...
		);
		const auto Stop = std::chrono::high_resolution_clock::now();

		return std::make_tuple<
			TimeT,
			typename std::result_of<FunctionT(ArgsT...)>::type
		>(
			std::chrono::duration_cast<TimeT>(
				Stop - Start
				),
			std::move(ReturnValue)
		);
	}
};

const static std::uint64_t FibMod64[93] = {
	0,
	1,
	1,
	2,
	3,
	5,
	8,
	13,
	21,
	34,
	55,
	89,
	144,
	233,
	377,
	610,
	987,
	1597,
	2584,
	4181,
	6765,
	10946,
	17711,
	28657,
	46368,
	75025,
	121393,
	196418,
	317811,
	514229,
	832040,
	1346269,
	2178309,
	3524578,
	5702887,
	9227465,
	14930352,
	24157817,
	39088169,
	63245986,
	102334155,
	165580141,
	267914296,
	433494437,
	701408733,
	1134903170,
	1836311903,
	2971215073,
	4807526976,
	7778742049,
	12586269025,
	20365011074,
	32951280099,
	53316291173,
	86267571272,
	139583862445,
	225851433717,
	365435296162,
	591286729879,
	956722026041,
	1548008755920,
	2504730781961,
	4052739537881,
	6557470319842,
	10610209857723,
	17167680177565,
	27777890035288,
	44945570212853,
	72723460248141,
	117669030460994,
	190392490709135,
	308061521170129,
	498454011879264,
	806515533049393,
	1304969544928657,
	2111485077978050,
	3416454622906707,
	5527939700884757,
	8944394323791464,
	14472334024676221,
	23416728348467685,
	37889062373143906,
	61305790721611591,
	99194853094755497,
	160500643816367088,
	259695496911122585,
	420196140727489673,
	679891637638612258,
	1100087778366101931,
	1779979416004714189,
	2880067194370816120,
	4660046610375530309,
	7540113804746346429
	// This is the cut-off for 64-bit numbers before values get truncated
};