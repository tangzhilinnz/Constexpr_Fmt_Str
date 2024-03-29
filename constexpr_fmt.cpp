﻿#include <string>
#include <iostream>
#include <array>
#include <chrono>
#include <cstring>
#include <cassert>
#include <charconv>
#include <tuple>
#include <random>
#include <cuchar>
#include <thread>
#include <stdlib.h>

#include <clocale>
#include <string>
#include <cwchar>
#include <time.h>
#include <iomanip>

#include "constexpr_fmt.h"
#include "tz_logger.h"
#include "timestamp_formatter.h"

//#include "mimalloc-new-delete.h"
//#include "mimalloc-override.h"
//#include "mimalloc.h"

using namespace std;
using namespace chrono;

// Protects the condition variables below
std::mutex m_mutex;

// Signal for when the poll thread should wakeup
std::condition_variable condi;

const int thdnum = 1;

const int kSize = 1024;

char* pLargeStr = new char[kSize];

void thdFunc() {
	for (int i = 0; i < 1000000 / thdnum; i++) {		
		//TZ_LOG(LogLevel::INFORMATION, "%s", pLargeStr);
		TZ_LOG(LogLevel::INFORMATION, "%c,%lc, %*.*ls %#+ 0*.*20.*d %*.*f", 'a', U'🍌', 20, 14, L"zß水🍌", 10, i, 20, 10, i / 117.);
		//TZ_LOG(LogLevel::INFORMATION, "%d", i);
	}
}

std::thread thds[thdnum];

int main() {

	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	for (int i = 0; i < kSize - 1; i++) {
		//if (i % 2 == 0)
		//    pLargeStr[i] = 'a';
		//else 
		//	pLargeStr[i] = ' ';
		pLargeStr[i] = 'a';
	}
	pLargeStr[kSize - 1] = '\0';

	std::to_chars_result ret;
	char buf[5000];
	char buf_out[1000];

	OutbufArg outBuf(buf_out);

	size_t size = 0;
	char* cp = nullptr;
	int exponent = 0;

	int result = 0;
	int64_t ran = 0;
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_int_distribution<int> distr(0, (std::numeric_limits<int>::max)()/*1000*/);
	int* pdata = new int[10000000];
	for (int i = 0; i < 1000000; i++) {
		pdata[i] = distr(eng);
	}

	//std::cout << "11-ll-11-ll" << std::endl;

	int& rpdata = pdata[100];

	std::setlocale(LC_ALL, /*"en_US.utf8"*/"ja_JP.UTF-8");
	//setlocale(LC_ALL, "");

	char pc[5] = "asdf";

	char arr[] = {'t', 'a', 'n', 'g', ' ', 'z', 'h', 'i', 'l', 'i', 'n', '\0'};

	auto tu = std::make_tuple("zoe", 2, 3., 4, 5.);

	char pstr[] = "tang zhilin";
	char* pBuf = buf;
	wchar_t pwstr[] = L"tang zhilin";

	char name[20] = { 0 };
	//unsigned long tid = static_cast<unsigned long>(GetCurrentThreadId());
	//snprintf(name, sizeof(name), "%lu", tid);
	RuntimeLogger::setThreadName("zoe snail");

	const std::chrono::nanoseconds timestamp{ 1587161887987654321 };
	tz::detail::TimestampFormatter tf(std::string("%Y-%m-%d %H:%M:%S,%Qus"));

	

	auto start = system_clock::now();

	//std::string_view sv;


	//std::thread t1(thdFunc);
	//std::thread t2(thdFunc);
	//t1.join();
	//t2.join();

	//time_t lastTime;
	for (int i = 0; i < 100000000; i++) {
		//TZ_LOG(LogLevel::INFORMATION, "test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n", 'a', 100, pstr, 12, 10, L"asd", "asdf", pwstr);
		//TZ_LOG(LogLevel::INFORMATION, "test %d", i);
		//Sleep(1);
		
		//TZ_LOG(LogLevel::INFORMATION, "%s %d %*.*f", "tangzhilin test", i, 20, 10, i / 117.);
		TZ_LOG(LogLevel::INFORMATION, "%d", i);
		//TZ_LOG(LogLevel::INFORMATION, "%c,%lc, %*.*ls %#+ 0*.*20.*d %*.*f", 'a', U'🍌', 20, 14, L"zß水🍌", 10, i, 20, 10, i / 117.);

		//std::this_thread::sleep_for(std::chrono::seconds(1));
		//condi.notify_all();
		//CFMT_STR(result, buf, 800, "%s", "");
		//std::strftime(buffer, 80, /*"%Y-%m-%d %H:%M:%S"*/"sdsdsdsd", timeinfo);
		//sv = tf.format_timestamp(timestamp);
		//if (rawtime != lastTime) {
		//	timeinfo = localtime(&rawtime);
		//	lastTime = rawtime;
		//}
	}

	//for (int i = 0; i < thdnum; i++) {
	//	thds[i] = std::thread(thdFunc);
	//}

	//for (int i = 0; i < thdnum; i++) {
	//	thds[i].join();
	//}

	auto end = system_clock::now();
	auto duration = duration_cast<microseconds>(end - start);
	std::cout << "cost: "
		<< double(duration.count()) * microseconds::period::num / microseconds::period::den << "seconds" << std::endl;
	//puts(buffer);

	std::this_thread::sleep_for(std::chrono::seconds(1));
	//abort();

	//printf("%+020.*d", 10, 999999);

	//puts(buffer);
	//printf("%+020.*d", 10, 999999);result
	printf("%d", result);
	//std::cout << sv << std::endl;

	delete[] pLargeStr;
}





//#include <string>
//#include <iostream>
//#include <array>
//#include <chrono>
//#include <cstring>
//#include <cassert>
//#include <charconv>
//#include <tuple>
//#include <random>
//#include <cuchar>
//
//#include <clocale>
//#include <string>
//#include <cwchar>
//
//#include "constexpr_fmt.h"
//#include "tz_logger.h"
//
//using namespace std;
//using namespace chrono;
//
////constexpr const char BLANKS[16] = { ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' };
////constexpr const char ZEROS[16] = { '0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0' };
//
////constexpr const char* const BLANKS = "                ";
////constexpr const char* const ZEROS = "0000000000000000";
////
////constexpr const short digit_pairs[100] = {
////	0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830, 0x3930,
////	0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731, 0x3831, 0x3931,
////	0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632, 0x3732, 0x3832, 0x3932,
////	0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533, 0x3633, 0x3733, 0x3833, 0x3933,
////	0x3034, 0x3134, 0x3234, 0x3334, 0x3434, 0x3534, 0x3634, 0x3734, 0x3834, 0x3934,
////	0x3035, 0x3135, 0x3235, 0x3335, 0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935,
////	0x3036, 0x3136, 0x3236, 0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936,
////	0x3037, 0x3137, 0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937,
////	0x3038, 0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938,
////	0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839, 0x3939
////};
//
//
////// sd%%sf%%%fes%h-+ 01233lzhhjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n
////constexpr int kSize = squeezeSoundSize("sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n");
////constexpr int kNVS = countValidSpecInfos("sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n");
//////constexpr static std::array<char, SIZE> myStr = preprocessInvalidSpecs<SIZE>("sd%%sf%%%fes%h-+ 01233lzhhjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***f23.\n");
////constexpr static std::array<char, kSize> myStr = preprocessInvalidSpecs<kSize>("sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n");
////constexpr int kSize1 = squeezeSoundSize("sdsssssssss%-+ *.*... *llzthhlh#-054d=ssssssadfadfasfsasadasdasdsa.\n");
////constexpr static auto myStr1 = preprocessInvalidSpecs<kSize1>("sdsssssssss%-+ *.*... *llzthhlh#-054d=ssssssadfadfasfsasadasdasdsa.\n");
////constexpr static auto myStr2 = preprocessInvalidSpecs<2>("%ks");
////// "sd%sf%%fes%sds%%%%gjt *.***k12.dsd%%s%%d%f%f%dsss%h-+ 01233lzhhjt *.***d23.\n"
////constexpr auto specInfos = analyzeFormatString<kNVS + 1>("sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n");
////constexpr int kNVS1 = countValidSpecInfos("dsdsdsds%k%kds%h.h.hlcdsd.\n");
////constexpr auto specInfos1 = analyzeFormatString<kNVS1 + 1>("dsdsdsds%k%kds%h.h.hlcdsd.\n");
//
////void print_wide(const std::wstring& wstr) {
////	std::mbstate_t state{};
////	for (wchar_t wc : wstr) {
////		std::string mb(MB_CUR_MAX, '\0');
////		std::size_t ret = std::wcrtomb(&mb[0], wc, &state);
////		std::cout << "multibyte char " << mb << " is " << ret << " bytes\n";
////	}
////}
//
///** std::tuple_size_v provides access to the number of elements in a tuple as a
// * compile-time constant expression */
//#define COUNT(fmt, ...) std::tuple_size_v<decltype(std::make_tuple(__VA_ARGS__))>;
//
//int foo(int& i) { return i; }
//int tz_snprintf(char* buffer, size_t  count, const char* fmt, ...);
//int test_CFMT_STR(int& i);
//
//template<size_t N>
//void OutputConstArgsSize() {
//	std::cout << "Args Size: " << N << std::endl;
//}
//
//template<size_t N>
//void OutputConstStrSize(std::array<size_t, N> arr) {
//	int i = 0;
//	for (auto e : arr)
//		std::cout << "string " << i++ << " size: " << e << std::endl;
//}
//
//int main() {
//
//	std::to_chars_result ret;
//	char buf[5000];
//	char buf_out[1000];
//
//	OutbufArg outBuf(buf_out);
//
//	size_t size = 0;
//	char* cp = nullptr;
//	int exponent = 0;
//
//	int result = 0;
//	int64_t ran = 0;
//	std::random_device rd;
//	std::default_random_engine eng(rd());
//	std::uniform_int_distribution<int> distr(0, (std::numeric_limits<int>::max)()/*1000*/);
//	int* pdata = new int[10000000];
//	for (int i = 0; i < 10000000; i++) {
//		pdata[i] = distr(eng);
//	}
//
//	//std::cout << "11-ll-11-ll" << std::endl;
//
//	int& rpdata = pdata[100];
//
//	std::setlocale(LC_ALL, /*"en_US.utf8"*/"ja_JP.UTF-8");
//	//setlocale(LC_ALL, "");
//
//	char pc[5] = "asdf";
//
//	char arr[] = { 't', 'a', 'n', 'g', ' ', 'z', 'h', 'i', 'l', 'i', 'n', '\0' };
//
//	auto tu = std::make_tuple("zoe", 2, 3., 4, 5.);
//
//	char pstr[] = "tang zhilin";
//	char* pBuf = buf;
//	wchar_t pwstr[] = L"tang zhilin";
//
//
//	// ==========================================================================================================================================
//	//using this_tupe_t = decltype(std::make_tuple(9999999, 1. /3/*, 1. / 3*//*pstr*//*'a', 100, pstr, 12, 10, L"asd", "asdf", pwstr*/));
//	//constexpr int kNVSIs = countValidSpecInfos("%#x %e"/*"test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n"*//*"test %hhl #-+0zjtM %c %x\n"*/);
//	//constexpr int kSS = squeezeSoundSize("%#x %e"/*"test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n"*//*"test %hhl #-+0zjtM %c %x\n"*/);
//	//static constexpr auto fmtRawStr = "%#x %e"/*"test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n"*//*"test %hhl #-+0zjtM %c %x\n"*/;
//	//static constexpr auto kfmtArr = preprocessInvalidSpecs<kSS>("%#x %e"/*"test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n"*//*"test %hhl #-+0zjtM %c %x\n"*/);
//	//static constexpr auto kRTStr = kSS < sizeof("%#x %e"/*"test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n"*//*"test %hhl #-+0zjtM %c %x\n"*/) ? kfmtArr.data() :
//	//	"%#x %e"/*"test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n"*//*"test %hhl #-+0zjtM %c %x\n"*/;
//	//static constexpr auto kHandler = unpack<kNVSIs + 1, LogEntryHandler, &fmtRawStr>();
//	//static constexpr auto pFormator = kHandler.instFormator<&kRTStr>(this_tupe_t());
//	//static constexpr StaticFmtInfo fmtInfo(pFormator, __FILE__, __USABLE_LINE__, 3, kRTStr, 3);
//	//constexpr auto kArgsSize = kHandler.argsSize(this_tupe_t());
//	//auto strSizeArr = kHandler.strSizeArray(9999999, 1. / 3/*pstr*//*'a', 100, pstr, 12, 10, L"asd", "asdf", pwstr*/);
//	//auto bufSize = kArgsSize;
//	//if (!strSizeArr.empty()) {
//	//	for (auto e : strSizeArr)
//	//		bufSize += e;
//	//}
//	//(void)kHandler.dump<kArgsSize>(&pBuf, strSizeArr, 9999999, 1. / 3/*pstr*//*'a', 100, pstr, 12, 10, L"asd", "asdf", pwstr*/);
//	//pBuf = buf;
//	// ==========================================================================================================================================
//	RuntimeLogger::setThreadName("test");
//	int j = 9999999;
//	auto start = system_clock::now();
//
//	for (int i = 0; i < 1000000; i++) {
//
//		//ret = std::to_chars(buf, buf + 100, pdata[i]);
//		//ret = std::to_chars(buf, buf + 100, pdata[(i + 1) % 10000000]);
//		//ret = std::to_chars(buf, buf + 100, pdata[(i + 2) % 10000000]);
//		//ret = std::to_chars(buf, buf + 100, pdata[(i + 3) % 10000000]);
//		//char buffer[12];
//		//std::tie(cp, size) = formatDec(buffer, pdata[i]);
//		//memcpy(buf, cp, size);
//		//std::tie(cp, size) = formatDec(buffer, pdata[(i + 1) % 10000000]);
//		//memcpy(buf, cp, size);
//		//std::tie(cp, size) = formatDec(buffer, pdata[(i + 2) % 10000000]);
//		//memcpy(buf, cp, size);
//		//std::tie(cp, size) = formatDec(buffer, pdata[(i + 3) % 10000000]);
//		//memcpy(buf, cp, size);
//		//result = /*tz_*/snprintf(buf, 600, "test%dtest%dtest%dtest%dtest%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%", pdata[i], pdata[(i + 1) % 10000000], pdata[(i + 2) % 10000000], pdata[(i + 3) % 10000000]);
//
//
//		//CFMT_STR(result, buf, 100, "sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n", 45, 's', L"adsds", 1, 1,'a');
//		//CFMT_STR(result, buf, 400, "=%f=%f=%f=%f=%f=%f=%f=%f=%f=%f==%f=%f=%f=%f=%f=%f=%f=%f=%f=%f=\n", 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
//		//CFMT_STR(result, buf, 100, "%3-+# *.***hjzll 00676 hK=%f=%f=%f=%f=%f=%f%f%3-+# *.***hjzll 00676 hkddd%h", 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1,1);
//		//CFMT_STR(result, buf, 100, "dadadsadadadadasssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssdad");
//		//CFMT_STR(result, buf, 100, "sd+ 01236%s66657==k31%s==lllllz%ahhjt *.***d23.\n", 45, 's', L"adsds", 1, 1, 34);
//		//CFMT_STR(result, buf, 100, "sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n", 2,2,2, 45, 's', L"adsds");
//		//CFMT_STR(result, buf, 100, "34342323%hls-+ 0#s...llks12.0#%**.***+-.**lls*.*20%%ahjhj",2, L"aad",2,2);
//		//CFMT_STR(result, buf, 100, "%%s%%s%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%ji", i);
//		//result = tz_snprintf/*CFMT_STR*/(/*result,*/ buf, 100, "h-+ 01233lzhhjt *.***hhhlll8.**s", 22,323,"adada");
//		///*result = tz_snprintf*/CFMT_STR(result, buf, 100, "my test for %d", i);
//		///*result = snprintf*/CFMT_STR(result, buf, 400, "test snprintf %d%hhdtest%%%%%%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhdtest%hhd", (int)i, static_cast<long>(i), (3 - 2 < 3 ? i : 3), foo(i), i, (i), (i, 3), i, i, i);
//		//result = test_CFMT_STR(i);
//		///*result = tz_snprintf*/CFMT_STR(result, buf, 400, 
//		//	"test snprintf %hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhd\n"
//		//	"test snprintf %hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhd\n"
//		//	"test snprintf %hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhd\n"
//		//	//"test snprintf %hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhd\n"
//		//	, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i
//		//	, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i);
//		//CFMT_STR(result, buf, 400, "test snprintf %hhdtest%hhdtest%hhdtest", i, i, i, i, i, i, i, i, i, i);
//		//CFMT_STR(result, buf, 400, "test snprintf %hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhd", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s");
//		//result = tz_snprintf(/*result,*/ buf, 400, "test snprintf %hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhd", i,i,i,i,i, i, i, i, i, i);
//		//result = tz_snprintf(buf, 400, "test snprintf %dtest%dtest%dtest%dtest%dtest%dtest%dtest%dtest%dtest%d", i, i, i, i, i, i, i, i, i, i);
//		//result = snprintf(buf, 100, "342324233hlk-+ 0#...saerereshdkGshjz..-+sf,dsdsffs+- #..*hdgfgf");
//		//result = snprintf(buf, 100, "h-+ 01233lzhhjt *.***hhhlll8.**s", 22, 323, "adada");
//		//result = snprintf(buf, 100, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
//		//result = tz_snprintf(buf, 100, "342324233hlk-+ 0#...%fkerere%hkG%hjz..-+%f,dsdsff%+- #..*hdgfgf", 8.6, 4, 2);
//		//result = tz_snprintf(buf, 10, "34342323%hlk-+ 0#%...llks12.0#%**.***+-.**llk*.*20%%ahjhj");
//		//result = tz_snprintf(buf, 100, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%d%d%d%d%d");
//		//result = tz_snprintf(buf, 100, "%hhd%hhd%hhd%hhd%hhd", i, i, i, i, i);
//		//result = tz_snprintf(buf, 100, "%lld%lld%lld%lld%lld", 1ll, 1ll, 1ll, 1ll, 1ll/*(long long)i, (long long)i, (long long)i, (long long)i, (long long)i*/);
//
//
//		//CFMT_STR(result, buf, 800, "%#hho", i);
//
//		/* %hho x 80 */
//		/*CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);
//		CFMT_STR(result, buf, 800, "%hho", i);*/
//
//		/* %o x 80 */
//		/*CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);
//		CFMT_STR(result, buf, 800, "%o", i);*/
//
//		/* %ho x 80 */
//		/*CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);
//		CFMT_STR(result, buf, 800, "%ho", i);*/
//
//		/* %llo x 80 */
//		/*CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);
//		CFMT_STR(result, buf, 800, "%llo", i);*/
//
//
//		/* %ld x 80 */
//		/*CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);
//		CFMT_STR(result, buf, 800, "%ld", i);*/
//
//		/* %d x 80 */
//		/*CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);
//		CFMT_STR(result, buf, 800, "%d", i);*/
//
//		/* %llu x 80 */
//		/*CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);
//		CFMT_STR(result, buf, 800, "%llu", i);*/
//
//		/* %hho x 80 -> snprintf */
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//		//result = snprintf(buf, 800, "%hho", i);
//
//		/* %hhd x 80 */
//		/*CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);
//		CFMT_STR(result, buf, 800, "%hhd", i);*/
//
//		/* %hhu x 80 */
//		/*CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);
//		CFMT_STR(result, buf, 800, "%hhu", i);*/
//
//		/* %hd x 80 */
//		/*CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);
//		CFMT_STR(result, buf, 800, "%hd", i);*/
//
//		/* %hu x 80 */
//		/*CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);
//		CFMT_STR(result, buf, 800, "%hu", i);*/
//
//
//		/* %#ho x 80 */
//		/*CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);
//		CFMT_STR(result, buf, 800, "%#ho", i);*/
//
//		/* %#o x 80 */
//		/*CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);
//		CFMT_STR(result, buf, 800, "%#o", i);*/
//
//		/* %#hho x 80 */
//		/*CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);
//		CFMT_STR(result, buf, 800, "%#hho", i);*/
//
//		/* %llX x 80 */
//		/*CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);
//		CFMT_STR(result, buf, 800, "%llX", i);*/
//
//		/* %hX x 80 */
//		/*CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);
//		CFMT_STR(result, buf, 800, "%hX", i);*/
//
//		/* %hhX x 80 */
//	   /* CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);
//		CFMT_STR(result, buf, 800, "%hhX", i);*/
//
//		/* %hx x 80 */
//		/*CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);
//		CFMT_STR(result, buf, 800, "%hx", i);*/
//
//		/* %hhx x 80 */
//		/*CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);
//		CFMT_STR(result, buf, 800, "%hhx", i);*/
//
//		/* %#hhx x 80 */
//		/*CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);
//		CFMT_STR(result, buf, 800, "%#hhx", i);*/
//
//		/* %#hx x 160 */
//		/*CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);
//		CFMT_STR(result, buf, 800, "%#hx", i);*/
//
//		/* %#llx x 160 */
//		/*CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);
//		CFMT_STR(result, buf, 800, "%#llx", i);*/
//
//		/* %#jzthlhhllp x 80 */
//		/*CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);
//		CFMT_STR(result, buf, 800, "%#jzthlhhllp", &i);*/
//		///*result = tz_snprintf*/CFMT_STR(result, buf, 800, "%+0 *.5#jzthlhhllp%lln|", -30, &i, &ran);
//
//
//		/* %#jzthlhhllp x 80 -> tz_snprintf*/
//		/*result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);
//		result = tz_snprintf(buf, 800, "%#jzthlhhllp", &i);*/
//
//		//result = snprintf(buf, 800, "%#10.4o", 012345);
//
//		//CFMT_STR(result, buf, 800, "%10hhd", i);
//
/////*result = tz_snprintf*/CFMT_STR(result, buf, 800, "%+ 0- #+30.010*.**X%lln|", 9, 30, i, &ran);
//		//result = snprintf(buf, 800, "%#-10.8o|", 012345);
//
////result = tz_snprintf(/*result,*/ buf, 800, "%u", pdata[i]);
////result = tz_snprintf(/*result,*/ buf, 800, "%+0*.*u", 20, 10, i);
////result = tz_snprintf(/*result,*/ buf, 800, "%+0*.*u", 20, 10, i);
////result = tz_snprintf(/*result,*/ buf, 800, "%+0*.*u", 20, 10, i);
////result = tz_snprintf(/*result,*/ buf, 800, "%+0*.*u", 20, 10, i);
////result = tz_snprintf(/*result,*/ buf, 800, "%+0*.*u", 20, 10, i);
////result = tz_snprintf(/*result,*/ buf, 800, "%+0*.*u", 20, 10, i);
////result = tz_snprintf(/*result,*/ buf, 800, "%+0*.*u", 20, 10, i);
////result = tz_snprintf(/*result,*/ buf, 800, "%+0*.*u", 20, 10, i);
////result = tz_snprintf(/*result,*/ buf, 800, "%+0*.*u", 20, 10, i);
////int& j = i;
//
////CFMT_STR/*_TUPLE*/(result, buf, 2000, "%#xsdsdsdsdsdsdsdsdsdsd", /*"test zhilin tang %s %d %lld %hx %x"*//*tu*//*std::make_tuple*/(pdata[i]));
//		///*result = tz_snprintf*/CFMT_STR(result, buf, 2000,
//		//	//"%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd"
//		//	//"%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd"
//		//	//"%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd"
//		//	//"%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd%10hhd",
//		//	////"%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld"
//		//	//"%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld"
//		//	//"%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld",
//		//	//"%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld %lld |||||%lld%lld%lld%"
//		//	//"%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld",
//		//	//"%lld%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx"
//		//	//"%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx",
//		//	//"%llu", i,
//		//	//"%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld",
//		//	//"%+030hhd%+030hhd%+030.10hhd%+030.10hhd%+030.10hhd",
//		//	//"%0*.*u|", -20, 10, i,
//		//	//"%#xsdsdsdsdsdsdsdsdsdsd", /*-99.9999*/ /*sqrt(2.)*//*1./0*/ /*30,*/ /*0.0l*//*(double)i*/ /*std::numeric_limits<double>::max()*//*1./ 3*//*i*//*(double)*/pdata[i]/*i*//*2.365*//*nullptr*//*nullptr*//*&i*/
//		//	//"test  %d %x %c %s", /*(double)i,*/ i, i, (char)i, "zhilin tang"
//		//	"%#x %e", i, 1. /3
//		//	//"%lld",
//		//	//"%s", arr
//		//	//"test%dtest%dtest%dtest%dtest%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%", pdata[i], pdata[(i+1)% 10000000], pdata[(i + 2) % 10000000], pdata[(i + 3) % 10000000],
//		//	//"%+0*.*u", 20, 10, i,
//		//	//i, i,i,i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,i,
//		//	//i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,
//		//	//"s", "s", "s", "s", "s", "s", "s", "s", "s", "s" "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s",
//		//	//"s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s",
//		//	//"s", "s", "s", "s", "s", "s", "s", "s", "s", "s" "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s",
//		//	//"s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s",
//		//	//922337203685477, 9223372036854771, 92233720368547712, 9223372036477123, 922337203, 9223, 92237, 92, 92233720368, 92237,
//		//	//922337203685477, 9223372036854771, 92233720368547712, 922337203685477123, 922337203, 9223, 92237, 92, 92233720368, 92237,
//		//	//922337203685477, 92233726854771, 92220368547712, 92233785477123, 9337203, 9223, 92237, 9, 92233720368, 92237,
//		//	//92237, 92233720771, 9223372037712, 92233785477123, 92233203, 9223, 92237, 92, 92233720368, 92237,
//		//	//92237, 9223372036854771, 92233720368547712, 9223372036477123, 922337203, 9223, 92237, 92, 92233720368, 92237,
//		//	//922337203685477, 92233720368, 92233720368, 92233720368, 922337203, 9223, 92237, 92233720368, 92233720368, 92237,
//		//	//92237, 92233726854771, 92237, 92233785477123, 9337203, 9223, 92237, 9, 92233720368, 92237,
//		//	//922337685477, 92233720771, 9223372037712, 92233785477123, 92233203, 9223, 92237, 92, 92233720368, 92237
//		//	/*rpdata, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//		//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i*/);
//			//static constexpr const char fmt[] = "%llx%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld";
//			//static constexpr const char fmt[] = "%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd";
//			//static constexpr const size_t size = sizeof(fmt);
//			//CFMT_STR<size, fmt>(result, buf, 400,
//			//	i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i
//			//	/*"s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s",
//			//	"s", "s", "s", "s", "s", "s", "s", "s", "s", "s","s"*/
//			//	/*(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//			//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//			//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
//			//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i*/);
//				// result = tz_snprintf(buf, 400, "+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld%+lld",
//				// 	"%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd%+hhd");
//				// "%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd"
//				// "%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu%hu"
//				//result = tz_snprintf(buf, 400, "%hhd%", i)
//
//		//CFMT_STR(result, buf, 2000, "%c", u8'a');
//		//CFMT_STR(result, buf, 2000, "%c", u'ß');
//		//CFMT_STR(result, buf, 2000, "%c", U'水');
//		//CFMT_STR(result, buf, 2000, "%c", U'🍌');
//		//CFMT_STR(result, buf, 2000, "%lc", L'a');
//		//result = snprintf(buf, 2000, "%lc", U'水');
//		//char8_t w = u8'b';
//		//char16_t w = u'水';
//		//char w = 'c';
//		//wchar_t w = L'水';
//		//char32_t w = U'🍌';
//		//CFMT_STR(result, buf, 2000, "%lc", /*w*/0x0001f34c);
//		//CFMT_STR(result, buf, 2000, "%lc", U'水');
//		//CFMT_STR(result, buf, 2000, "%lc", U'🍌');
//
//		//result = snprintf(buf, 2000, "%E", 0. / -0);
//		//CFMT_STR(result, buf, 2000, "%f", 0.02);
//
/////*result = tz_snprintf*/CFMT_STR(result, buf, 2000, "%#32LG", (long double)(-2 / 0.));
//
/////*result = tz_snprintf*/CFMT_STR(result, buf, 2000, "%.f", /*(long double)*//*i*/2.345);
//
//		//result = snprintf(buf, 2000, "%A", 23.8);
//
//		//result = snprintf/*CFMT_STR*/(/*result,*/ buf, 2000, "%.1000g", /*0.0000256*//*1234567.0*//*123456.0*//*0. / 0*//*std::numeric_limits<double>::max()*/1. /3/*1e+200*//*(double)i*//*-2.365*//*(double)i*/);
//		//ret = std::to_chars(buf, buf + 2000, /*1234567.0*//*123456.0*//*std::numeric_limits<double>::max()*/(double)i/*(double)pdata[i]*/, std::chars_format::general, -1);
//	   //exponent = static_cast<int>(std::floor(std::log10(/*1. / 3*/ 3434545/*std::numeric_limits<long double>::max()*/)));
//
//	   ///*result = snprintf*/  CFMT_STR(result, buf, 5000, "%.2000e", (double)std::numeric_limits<double>::max());
//
//		//result = snprintf(buf, 2000, "%lc", U'🍌');
//		//result = snprintf(buf, 2000, "%- +020.10lc||", L'\u6c34');
////result = snprintf/*CFMT_STR*/(/*result,*/ buf, 2000, "%-10lc%-10lc,%-10lc%-10lc", /*L'\u6c34'*//*L'a'*/U'\U0001f34c', L'你', L'好', L'世', L'界');
//		//result = snprintf(buf, 2000, "%lc", L'\u00df');
//		//result = snprintf(buf, 2000, "%c%c%c\n", '\xE2', '\x99', '\xA5');
//		//CFMT_STR(result, buf, 2000, "%c%c%c\n", '\xE2', '\x99', '\xA5');
//
//		//result = tz_snprintf(buf, 2000, "%+ 0100.16s||", /*"tangzhilin"*/"ss");
/////*result = tz_snprintf*/CFMT_STR(result, buf, 2000, "%+s||", /*"tangzhilin"*/"tangzhilin for s test");
//
//		//result = snprintf/*CFMT_STR*/(/*result,*/ buf, 2000, "% +010.4ls||", L"你好世界"/*nullptr*//*(const wchar_t*)NULL*/);
///////*result = snprintf*/CFMT_STR(result, buf, 2000, "%ls||" , /*L"hello, world"*/ /*L"你好世界"*//*nullptr*//*(const wchar_t*)NULL*//*L"水水水水水水",*/ L"zß水🍌" L"爆ぜろリアル！弾けろシナプス！パニッシュメントディス、ワールド！"
/////*	L"爆ぜろリアル！弾けろシナプス！パニッシュメントディス、ワールド！"
////	L"爆ぜろリアル！弾けろシナプス！パニッシュメントディス、ワールド！"
////	L"爆ぜろリアル！弾けろシナプス！パニッシュメントディス、ワールド！"
////	L"爆ぜろリアル！弾けろシナプス！パニッシュメントディス、ワールド！"
////	L"爆ぜろリアル！弾けろシナプス！パニッシュメントディス、ワールド！"
////	L"爆ぜろリアル！弾けろシナプス！パニッシュメントディス、ワールド！"
////	L"爆ぜろリアル！弾けろシナプス！パニッシュメントディス、ワールド！"
////	L"爆ぜろリアル！弾けろシナプス！パニッシュメントディス、ワールド！"
////	L"爆ぜろリアル！弾けろシナプス！パニッシュメントディス、ワールド！"
////	L"爆ぜろリアル！弾けろシナプス！パニッシュメントディス、ワールド！"*/);
//
//		//result = snprintf(buf, 2000, "%- +020.13ls||", /*L"你好世界"*//*nullptr*//*(const wchar_t*)NULL*/L"zß水🍌");
//
////result = /*tz_*/snprintf/*CFMT_STR*/(/*result,*/ buf, 2000, "LLONG_MAX = %lld\n", LLONG_MAX);
////result = /*tz_*/snprintf/*CFMT_STR*/(/*result,*/ buf, 2000, "LLONG_MAX = %lld\n", LLONG_MIN);
////result = /*tz_*/snprintf/*CFMT_STR*/(/*result,*/ buf, 2000, "ULLONG_MAX = %llu\n", ULLONG_MAX);
// //   pBuf = buf;
// //    
//	//{
//	//	using this_tupe_t = decltype(std::make_tuple(i, 1./3 /*"test"*//*pstr*//*'a', 100, pstr, 12, 10, L"asd", "asdf", pwstr*/));
//	//	constexpr int kNVSIs = countValidSpecInfos("%#x %e"/*"test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n"*//*"test %hhl #-+0zjtM %c %x\n"*/);
//	//	constexpr int kSS = squeezeSoundSize("%#x %e"/*"test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n"*//*"test %hhl #-+0zjtM %c %x\n"*/);
//	//	static constexpr auto fmtRawStr = "%#x %e"/*"test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n"*//*"test %hhl #-+0zjtM %c %x\n"*/;
//	//	static constexpr auto kfmtArr = preprocessInvalidSpecs<kSS>("%#x %e"/*"test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n"*//*"test %hhl #-+0zjtM %c %x\n"*/);
//	//	static constexpr auto kRTStr = kSS < sizeof("%#x %e"/*"test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n"*//*"test %hhl #-+0zjtM %c %x\n"*/) ? kfmtArr.data() :
//	//		"%#x %e"/*"test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n"*//*"test %hhl #-+0zjtM %c %x\n"*/;
//	//	static constexpr auto kHandler = unpack<kNVSIs + 1, LogEntryHandler, &fmtRawStr>();
//	//	static constexpr auto pFormator = kHandler.instFormator<&kRTStr>(this_tupe_t());
//	//	static constexpr StaticFmtInfo fmtInfo(pFormator, __FILE__, __USABLE_LINE__, 3, kRTStr, 3);
//	//	constexpr auto kArgsSize = kHandler.argsSize(this_tupe_t());
//	//	auto strSizeArr = kHandler.strSizeArray(i, 1. / 3 /*"test"*//*pstr*//*'a', 100, pstr, 12, 10, L"asd", "asdf", pwstr*/);
//	//	auto bufSize = kArgsSize;
//	//	if (!strSizeArr.empty()) {
//	//		for (auto e : strSizeArr)
//	//			bufSize += e;
//	//	}
//	//	(void)kHandler.dump<kArgsSize>(&pBuf, strSizeArr, i, 1. / 3 /*"test"*//*pstr*//*'a', 100, pstr, 12, 10, L"asd", "asdf", pwstr*/);
//	//	pBuf = buf;
//
//	//	outBuf.reset();
//
//	//	(*pFormator)(outBuf, pBuf);
//
//	//	outBuf.done();
//	//}
//
//	//TZ_LOG(LogLevel::INFORMATION, "test %hhl #-+0zjtM %.*p %s %*.*ls %s %ls\n", 'a', 100, pstr, 12, 10, L"asd", "asdf", pwstr);
//		TZ_LOG(LogLevel::INFORMATION, "test %d", i);
//		//std::this_thread::sleep_for(std::chrono::nanoseconds(10));
//		//pBuf = buf;
//
//	}
//
//	auto end = system_clock::now();
//	auto duration = duration_cast<microseconds>(end - start);
//
//
//
//
//	//static const std::mbstate_t initial{};
//	//std::mbstate_t mbs{ initial };
//
//	//size_t len = std::c32rtomb(buf, u'中', &mbs);
//
//	//if (len == static_cast<size_t>(-1)) {
//	//	std::cout << "convert fault" << std::endl;
//	//}
//
//	//std::setlocale(LC_ALL, "en_US.utf8");
//	//std::u16string_view strv = u"zß水🍌"; // or z\u00df\u6c34\U0001F34C
//	//std::cout << "Processing " << strv.size() << " UTF-16 code units: [ ";
//	//for (char16_t c : strv)
//	//	std::cout << std::showbase << std::hex << static_cast<int>(c) << ' ';
//	//std::cout << "]\n";
//
//	//std::mbstate_t state{};
//	//char out[MB_LEN_MAX]{};
//	//for (char16_t c : strv)
//	//{
//	//	std::size_t rc = std::c16rtomb(out, c, &state);
//	//	std::cout << static_cast<int>(c) << " converted to [ ";
//	//	if (rc != (std::size_t)-1)
//	//		for (unsigned char c8 : std::string_view{ out, rc })
//	//			std::cout << +c8 << ' ';
//	//	std::cout << "]\n";
//	//}
//
//
//	//std::cout << "outbuf: " << buf_out << std::endl;
//	//std::cout << j << std::endl;
//
//	//char buf1[5000];
//	//int result1;
//
//	//double dl = /*-99.9999*//*std::numeric_limits<double>::max()*//*0.000003232*//*3333333333.3433*/ 2. /7;
//
//	////result = snprintf(buf, 5000, "%#.153g", dl);
//	////CFMT_STR(result1, buf1, 5000, "%#.153g", dl);
//
//	//printf("%s\n", buf);
//	//printf("%s\n", buf1);
//	//std::cout << "len: " << len << std::endl;
//	//std::cout << "ret: " << (ret.ptr - buf) << std::endl;
//	//std::cout << "result: " << result << std::endl;
//	//std::cout << "result1: " << result1 << std::endl;
//	//std::cout << "ran: " << ran << std::endl;
//	//std::cout << "exponent: " << exponent << std::endl;
//
//	std::cout << "cost: "
//		<< double(duration.count()) * microseconds::period::num / microseconds::period::den << "seconds" << std::endl;
//
//	//printf("%- +020ls||", L"你好世界");
//
//	//size_t sizeW = 9;
//	//const wchar_t* wcp = L"你好世界";
//
//	std::this_thread::sleep_for(std::chrono::seconds(120));
//
//	//static const std::mbstate_t initial{};
//	//std::mbstate_t mbs{ initial };
//
//	//char bufForWCS[10] = {0};
//
//	//sizeW = wcsrtombs(bufForWCS, &wcp, sizeW, &mbs);
//
//	//std::cout << "sizeW: " << sizeW << std::endl;
//	//std::cout << bufForWCS << std::endl;
//
//	//wint_t c = L'd';
//	////Foo(2, 3, 4u, (int64_t)9, 'a', "s", 2.3, L'A', L"tangzhilin", c);
//	//constexpr int i = sizeof(wchar_t);
//	//constexpr int j = sizeof(wint_t);
//
//	//std::cout << "long to int: " << boolalpha << std::is_convertible_v<long, int> << std::endl;
//	//std::cout << "int to long: " << boolalpha << std::is_convertible_v<int, long> << std::endl;
//
//	//std::cout << "long to char: " << boolalpha << std::is_convertible_v<long, char> << std::endl;
//	//std::cout << "char to long: " << boolalpha << std::is_convertible_v<char, long> << std::endl;
//
//	//std::cout << "long to void*: " << boolalpha << std::is_convertible_v<long, void*> << std::endl;
//	//std::cout << "void* to long: " << boolalpha << std::is_convertible_v<void*, long> << std::endl;
//
//	//std::cout << "wint_t integer? " << is_integral_v<wint_t> << std::endl;
//	//std::cout << "char* integer? " << is_integral_v<char*> << std::endl;
//
//	//std::cout << "char* pointer? " << is_pointer_v<char*> << std::endl;
//
//	//constexpr bool compare2 = std::is_integral_v<const char&>;
//	//constexpr bool compare = std::is_integral_v<std::remove_reference_t<const char>>;
//	//constexpr bool compare1 = std::is_convertible_v<void(*)(int, int), const char*>;
//
//	//char str[5] = { 0 };
//	//wchar_t wc = L'c';
//	//wint_t wc1 = L'c';
//
//	//const wchar_t* wstr = L"sdsdasdasd";
//
//	//struct A { int i = 0; };
//	//A a;
//	//A* pa = &a;
//	//A& ra = a;
//
//	//string s = "aa";
//
//	//void* v = nullptr;
//
//	//int in = 100;
//
//	//const int& rin = in;
//
//	//double d = 10.2;
//
//	//std::to_chars_result re = to_chars(buf, buf + 100, static_cast<short>(rin), 10);
//	//re = to_chars(buf, buf + 100, rin, 10);
//	//re = to_chars(buf, buf + 100, reinterpret_cast<uintptr_t>(pa), 10);
//
//	//uintmax_t ux = (short)-100;
//	//re = to_chars(buf, buf + 100, ux, 10);
//	//*re.ptr = '\0';
//
//	//std::cout << buf << std::endl;
//
//	//float f = 2.32343f;
//	//double df = 2.32343;
//	//long double ldf = 2.32343;
//
//	//std::cout << static_cast<long double>(f) << std::endl;
//	//std::cout << static_cast<float>(df) << std::endl;
//	//std::cout << static_cast<float>(ldf) << std::endl;
//
//	//int b = 23;
//	//int& rb = b;
//	//std::cout << static_cast<long>(rb) << std::endl;
//
//	//for (int i = 0; i < 256; i++) {
//	//	printf("\"%x\", ", i);
//
//	////	/*if (i < 10) {
//	////		printf("{ 1, \"%d\" }, ", i);
//	////	}
//	////	else if (i < 100) {
//	////		printf("{ 2, \"%d\" }, ", i);
//	////	}
//	////	else {
//	////		printf("{ 3, \"%d\" }, ", i);
//	////	}*/
//	//}
//
//	//int i = -1;
//	//printf("%#30x", i);
//
//	//std::setlocale(LC_ALL, "en_US.utf8");
//	//std::wstring wstr = L"z\u00df\u6c34\U0001f34c"; // or L"zß水🍌"
//	//print_wide(wstr);
//
//
////char32_t wi = 3;
////wint_t wi2 = 4;
////
////const char8_t wi3 = 5;
////
////std::cout << std::is_convertible_v<char32_t, wint_t> << std::endl;
////std::cout << std::is_convertible_v<const wint_t, char32_t> << std::endl;
////std::cout << std::is_convertible_v<char16_t, char32_t> << std::endl;
////std::cout << std::is_convertible_v<char8_t, char32_t> << std::endl;
////std::cout << std::is_convertible_v<const unsigned char, char32_t> << std::endl;
////std::cout << std::is_convertible_v<wchar_t, char32_t> << std::endl;
//
////std::cout << static_cast<char32_t>(static_cast<wint_t>(wi)) << std::endl;
//
////std::cout << std::is_convertible_v<double, long double> << std::endl;
////std::cout << std::is_convertible_v<float, double> << std::endl;
////std::cout << std::is_convertible_v<float, long double> << std::endl;
////
////
////std::cout << std::is_convertible_v<double, float> << std::endl;
////std::cout << std::is_convertible_v<long double, double> << std::endl;
////std::cout << std::is_convertible_v<long double, float> << std::endl;
//
////result = snprintf  /*CFMT_STR*/(/*result,*/ buf, 2000, "%.320Le", std::numeric_limits<double>::max());
////printf("%s\n", buf);
////std::cout << "result: " << result << std::endl;
//
//}