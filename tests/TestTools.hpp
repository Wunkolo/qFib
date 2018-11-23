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

const static std::uint64_t FibMod64[] = {
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
7540113804746346429,
12200160415121876738,
1293530146158671551,
13493690561280548289,
14787220707439219840,
9834167195010216513,
6174643828739884737,
16008811023750101250,
3736710778780434371,
1298777728820984005,
5035488507601418376,
6334266236422402381,
11369754744023820757,
17704020980446223138,
10627031650760492279,
9884308557497163801,
2064596134548104464,
11948904692045268265,
14013500826593372729,
7515661444929089378,
3082418197812910491,
10598079642741999869,
13680497840554910360,
5831833409587358613,
1065587176432717357,
6897420586020075970,
7963007762452793327,
14860428348472869297,
4376692037216111008,
790376311979428689,
5167068349195539697,
5957444661174968386,
11124513010370508083,
17081957671545476469,
9759726608206432936,
8394940206042357789,
18154666814248790725,
8102862946581596898,
7810785687120836007,
15913648633702432905,
5277690247113717296,
2744594807106598585,
8022285054220315881,
10766879861326914466,
342420841837678731,
11109300703164593197,
11451721545002271928,
4114278174457313509,
15565999719459585437,
1233533820207347330,
16799533539666932767,
18033067359874280097,
16385856825831661248,
15972180111996389729,
13911292864118499361,
11436728902405337474,
6901277692814285219,
18338006595219622693,
6792540214324356296,
6683802735834427373,
13476342950158783669,
1713401612283659426,
15189744562442443095,
16903146174726102521,
13646146663458994000,
12102548764475544905,
7301951354224987289,
957756044990980578,
8259707399215967867,
9217463444206948445,
17477170843422916312,
8247890213920313141,
7278316983633677837,
15526207197553990978,
4357780107478117199,
1437243231322556561,
5795023338800673760,
7232266570123230321,
13027289908923904081,
1812812405337582786,
14840102314261486867,
16652914719599069653,
13046272960151004904,
11252443606040522941,
5851972492481976229,
17104416098522499170,
4509644517294923783,
3167316542107871337,
7676961059402795120,
10844277601510666457,
74494587203909961,
10918772188714576418,
10993266775918486379,
3465294890923511181,
14458561666841997560,
17923856557765508741,
13935674150897954685,
13412786634953911810,
8901716712142314879,
3867759273386675073,
12769475985528989952,
16637235258915665025,
10959967170735103361,
9150458355941216770,
1663681452966768515,
10814139808907985285,
12477821261874753800,
4845216997073187469,
17323038258947941269,
3721511182311577122,
2597805367549966775,
6319316549861543897,
8917121917411510672,
15236438467273054569,
5706816310975013625,
2496510704538516578,
8203327015513530203,
10699837720052046781,
456420661856025368,
11156258381908072149,
11612679043764097517,
4322193351962618050,
15934872395726715567,
1810321673979782001,
17745194069706497568,
1108771669976727953,
407221665973673905,
1515993335950401858,
1923215001924075763,
3439208337874477621,
5362423339798553384,
8801631677673031005,
14164055017471584389,
4518942621435063778,
236253565197096551,
4755196186632160329,
4991449751829256880,
9746645938461417209,
14738095690290674089,
6037997555042539682,
2329349171623662155,
8367346726666201837,
10696695898289863992,
617298551246514213,
11313994449536378205,
11931293000782892418,
4798543376609719007,
16729836377392611425,
3081635680292778816,
1364727983975838625,
4446363664268617441,
5811091648244456066,
10257455312513073507,
16068546960757529573,
7879258199561051464,
5501061086609029421,
13380319286170080885,
434636299069558690,
13814955585239639575,
14249591884309198265,
9617803395839286224,
5420651206438932873,
15038454602278219097,
2012361735007600354,
17050816337285819451,
616433998583868189,
17667250335869687640,
18283684334453555829,
17504190596613691853,
17341130857357696066,
16398577380261836303,
15292964163909980753,
13244797470462265440,
10091017560662694577,
4889070957415408401,
14980088518078102978,
1422415401783959763,
16402503919862062741,
17824919321646022504,
15780679167798533629,
15158854415735004517,
12492789509823986530,
9204899851849439431,
3250945287963874345,
12455845139813313776,
15706790427777188121,
9715891493880950281,
6975937847948586786,
16691829341829537067,
5221023116068572237,
3466108384188557688,
8687131500257129925,
12153239884445687613,
2393627310993265922,
14546867195438953535,
16940494506432219457,
13040617628161621376,
11534368060884289217,
6128241615336358977,
17662609676220648194,
5344107217847455555,
4559972820358552133,
9904080038206007688,
14464052858564559821,
5921388823061015893,
1938697607916024098,
7860086430977039991,
9798784038893064089
};