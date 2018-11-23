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
