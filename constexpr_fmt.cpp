#include <string>
#include <iostream>
#include <array>
#include <chrono>
#include <cstring>
#include <cassert>
#include <charconv>
#include <tuple>

#include "ConstExprIF.h"

using namespace std;
using namespace chrono;


// sd%%sf%%%fes%h-+ 01233lzhhjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n
constexpr int kSize = squeezeSoundSize("sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n");
constexpr int kNVS = countValidSpecInfos("sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n");
//constexpr static std::array<char, SIZE> myStr = preprocessInvalidSpecs<SIZE>("sd%%sf%%%fes%h-+ 01233lzhhjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***f23.\n");
constexpr static std::array<char, kSize> myStr = preprocessInvalidSpecs<kSize>("sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n");
constexpr int kSize1 = squeezeSoundSize("sdsssssssss%-+ *.*... *llzthhlh#-054d=ssssssadfadfasfsasadasdasdsa.\n");
constexpr static auto myStr1 = preprocessInvalidSpecs<kSize1>("sdsssssssss%-+ *.*... *llzthhlh#-054d=ssssssadfadfasfsasadasdasdsa.\n");
constexpr static auto myStr2 = preprocessInvalidSpecs<2>("%ks");
// "sd%sf%%fes%sds%%%%gjt *.***k12.dsd%%s%%d%f%f%dsss%h-+ 01233lzhhjt *.***d23.\n"
constexpr std::array<SpecInfo, kNVS + 1> specInfos = analyzeFormatString<kNVS + 1>("sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n");
constexpr int kNVS1 = countValidSpecInfos("dsdsdsds%k%kds%h.h.hlcdsd.\n");
constexpr std::array<SpecInfo, kNVS1 + 1> specInfos1 = analyzeFormatString<kNVS1 + 1>("dsdsdsds%k%kds%h.h.hlcdsd.\n");



/** std::tuple_size_v provides access to the number of elements in a tuple as a
 * compile-time constant expression */
#define COUNT(fmt, ...) std::tuple_size_v<decltype(std::make_tuple(__VA_ARGS__))>;

int foo(int& i) { return i; }
int tz_snprintf(char* buffer, size_t  count, const char* fmt, ...);
int test_CFMT_STR(int& i);

int main() {
	auto start = system_clock::now();

	char buf[400];

	int result = 0;

	//std::cout << "11-ll-11-ll" << std::endl;

	for (int i = 0; i < 10000000; i++) {
		//CFMT_STR(result, buf, 100, "sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n", 45, 's', L"adsds", 1, 1,'a');
		//CFMT_STR(result, buf, 400, "=%f=%f=%f=%f=%f=%f=%f=%f=%f=%f==%f=%f=%f=%f=%f=%f=%f=%f=%f=%f=\n", 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
		//CFMT_STR(result, buf, 100, "%3-+# *.***hjzll 00676 hK=%f=%f=%f=%f=%f=%f%f%3-+# *.***hjzll 00676 hkddd%h", 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1,1);
		//CFMT_STR(result, buf, 100, "dadadsadadadadasssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssdad");
		//CFMT_STR(result, buf, 100, "sd+ 01236%s66657==k31%s==lllllz%ahhjt *.***d23.\n", 45, 's', L"adsds", 1, 1, 34);
		//CFMT_STR(result, buf, 100, "sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n", 2,2,2, 45, 's', L"adsds");
		//CFMT_STR(result, buf, 100, "34342323%hls-+ 0#s...llks12.0#%**.***+-.**lls*.*20%%ahjhj",2, L"aad",2,2);
		//CFMT_STR(result, buf, 100, "%%s%%s%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%i", i);
		//result = tz_snprintf/*CFMT_STR*/(/*result,*/ buf, 100, "h-+ 01233lzhhjt *.***hhhlll8.**s", 22,323,"adada");
		///*result = tz_snprintf*/CFMT_STR(result, buf, 100, "my test for %d", i);
		///*result = snprintf*/CFMT_STR(result, buf, 400, "test snprintf %d%hhdtest%%%%%%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhdtest%hhd", (int)i, static_cast<long>(i), (3 - 2 < 3 ? i : 3), foo(i), i, (i), (i, 3), i, i, i);
		//result = test_CFMT_STR(i);
		///*result = tz_snprintf*/CFMT_STR(result, buf, 400, 
		//	"test snprintf %hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhd\n"
		//	"test snprintf %hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhd\n"
		//	"test snprintf %hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhd\n"
		//	//"test snprintf %hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhd\n"
		//	, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i
		//	, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i);
		//CFMT_STR(result, buf, 400, "test snprintf %hhdtest%hhdtest%hhdtest", i, i, i, i, i, i, i, i, i, i);
		//CFMT_STR(result, buf, 400, "test snprintf %hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhd", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s");
		//result = tz_snprintf(/*result,*/ buf, 400, "test snprintf %hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhdtest%hhd", i,i,i,i,i, i, i, i, i, i);
		//result = tz_snprintf(buf, 400, "test snprintf %dtest%dtest%dtest%dtest%dtest%dtest%dtest%dtest%dtest%d", i, i, i, i, i, i, i, i, i, i);
		//result = snprintf(buf, 100, "342324233hlk-+ 0#...saerereshdkGshjz..-+sf,dsdsffs+- #..*hdgfgf");
		//result = snprintf(buf, 100, "h-+ 01233lzhhjt *.***hhhlll8.**s", 22, 323, "adada");
		//result = snprintf(buf, 100, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
		//result = tz_snprintf(buf, 100, "342324233hlk-+ 0#...%fkerere%hkG%hjz..-+%f,dsdsff%+- #..*hdgfgf", 8.6, 4, 2);
		//result = tz_snprintf(buf, 10, "34342323%hlk-+ 0#%...llks12.0#%**.***+-.**llk*.*20%%ahjhj");
		//result = tz_snprintf(buf, 100, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
		//result = tz_snprintf(buf, 100, "%hhd%hhd%hhd%hhd%hhd", i, i, i, i, i);
		//result = tz_snprintf(buf, 100, "%lld%lld%lld%lld%lld", 1ll, 1ll, 1ll, 1ll, 1ll/*(long long)i, (long long)i, (long long)i, (long long)i, (long long)i*/);
		 /*result = tz_snprintf*/CFMT_STR(result, buf, 400, "%hhd%hhd%hhd%hhd",
			 //i, i,i,i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,i
			 "s", "s", "s", "s", "s", "s", "s", "s", "s", "s" "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s",
			 "s", "s", "s", "s", "s", "s", "s", "s", "s", "s","s","s"
			/*(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
			(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
			(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
			(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i*/);
			//static constexpr const char fmt[] = "%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld";
			//static constexpr const char fmt[] = "%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd";
			//static constexpr const size_t size = sizeof(fmt);
			//CFMT_STR<size, fmt>(result, buf, 400,
			//	i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i
			//	/*"s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s", "s",
			//	"s", "s", "s", "s", "s", "s", "s", "s", "s", "s","s"*/
			//	/*(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
			//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
			//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i,
			//	(long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i, (long long)i*/);
				// result = tz_snprintf(buf, 400, "%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd",
				// 	i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i);

				//result = tz_snprintf(buf, 400, "%hhd%", i);

				//OutbufArg outbuf(buf);
				//std::to_chars_result ret;
				//char buffer[100];
				//ret = std::to_chars(buffer, buffer + 100, (long long)i);
				////outbuf.write(buffer, ret.ptr - buffer);
				//std::memcpy(outbuf.pBuf_, buffer, ret.ptr - buffer);
				////ret = std::to_chars(buf, buf + 100, (long long)i);
				////outbuf.write(buf, ret.ptr - buf);
				////ret = std::to_chars(buf, buf + 100, (long long)i);
				////outbuf.write(buf, ret.ptr - buf);
				////ret = std::to_chars(buf, buf + 100, (long long)i);
				////outbuf.write(buf, ret.ptr - buf);
				////ret = std::to_chars(buf, buf + 100, (long long)i);
				////outbuf.write(buf, ret.ptr - buf);
				//outbuf.done();

	}

	std::cout << buf << std::endl;
	std::cout << "result: " << result << std::endl;

	auto end = system_clock::now();
	auto duration = duration_cast<microseconds>(end - start);

	std::cout << "cost: "
		<< double(duration.count()) * microseconds::period::num / microseconds::period::den << "seconds" << std::endl;

	//wint_t c = L'd';
	////Foo(2, 3, 4u, (int64_t)9, 'a', "s", 2.3, L'A', L"tangzhilin", c);
	//constexpr int i = sizeof(wchar_t);
	//constexpr int j = sizeof(wint_t);

	//std::cout << "long to int: " << boolalpha << std::is_convertible_v<long, int> << std::endl;
	//std::cout << "int to long: " << boolalpha << std::is_convertible_v<int, long> << std::endl;

	//std::cout << "long to char: " << boolalpha << std::is_convertible_v<long, char> << std::endl;
	//std::cout << "char to long: " << boolalpha << std::is_convertible_v<char, long> << std::endl;

	//std::cout << "long to void*: " << boolalpha << std::is_convertible_v<long, void*> << std::endl;
	//std::cout << "void* to long: " << boolalpha << std::is_convertible_v<void*, long> << std::endl;

	//std::cout << "wint_t integer? " << is_integral_v<wint_t> << std::endl;
	//std::cout << "char* integer? " << is_integral_v<char*> << std::endl;

	//std::cout << "char* pointer? " << is_pointer_v<char*> << std::endl;

	//constexpr bool compare2 = std::is_integral_v<const char&>;
	//constexpr bool compare = std::is_integral_v<std::remove_reference_t<const char>>;
	//constexpr bool compare1 = std::is_convertible_v<void(*)(int, int), const char*>;

	//char str[5] = { 0 };
	//wchar_t wc = L'c';
	//wint_t wc1 = L'c';

	//const wchar_t* wstr = L"sdsdasdasd";

	//struct A { int i = 0; };
	//A a;
	//A* pa = &a;
	//A& ra = a;

	//string s = "aa";

	//void* v = nullptr;

	//int in = 100;

	//const int& rin = in;

	//double d = 10.2;

	//std::to_chars_result re = to_chars(buf, buf + 100, static_cast<short>(rin), 10);
	//re = to_chars(buf, buf + 100, rin, 10);
	//re = to_chars(buf, buf + 100, reinterpret_cast<uintptr_t>(pa), 10);

	//uintmax_t ux = (short)-100;
	//re = to_chars(buf, buf + 100, ux, 10);
	//*re.ptr = '\0';

	//std::cout << buf << std::endl;

	//float f = 2.32343f;
	//double df = 2.32343;
	//long double ldf = 2.32343;

	//std::cout << static_cast<long double>(f) << std::endl;
	//std::cout << static_cast<float>(df) << std::endl;
	//std::cout << static_cast<float>(ldf) << std::endl;

	//int b = 23;
	//int& rb = b;
	//std::cout << static_cast<long>(rb) << std::endl;

	//constexpr auto intResult = formattedInteger<intmax_t>(3);
	//re = to_chars(buf, buf + 100, std::get<0>(intResult), 10);
	//*re.ptr = '\0';
	//std::cout << buf << std::endl;




	//{
	//	static constexpr auto a = std::array<int, 4>{ 1, 2, 4, 6};
	//	static constexpr std::array<int, 0> a_zero = {};
	//	//constexpr std::array<char, 0> c = {};

	//	static constexpr std::array<SpecInfo, 4> infos = {
	//		SpecInfo{6U, 8U, 0, 0, -1, '\000', 'i', true},
	//		SpecInfo{35U, 37U, 0, 0, -1, '\000', 'd', true},
	//		SpecInfo{40U, 42U, 0, 0, -1, '\000', 'd', true},
	//		SpecInfo{49U, 72U, 518, -2147483648, -3, '+', 'd', true}
	//	};

	//	//constexpr auto converter = converterFromSTDArray<4, infos>();


	//	constexpr const auto s = unpack<a, S>();
	//	constexpr const auto s_zero = unpack<a_zero, S>();
	//	constexpr const auto s_zero = unpack<a_zero, S>();
	//	constexpr auto converter = unpack<infos, Converter>();

	//	//s('a','b', 'c', 'd');
	//	//s_zero();

	//	OutbufArg outbuf;
	//	outbuf.pBuf_ = buf;
	//	outbuf.pBufEnd_ = (char*)(buf + 100);

	//	converter(outbuf, "sadasdasd", 13, 34, 34, 34,34);
	//}
    int j;
    std::cin >> j;

	system("pause");
}