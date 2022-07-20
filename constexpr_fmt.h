#ifndef CONSTEXPR_FMT_H__
#define CONSTEXPR_FMT_H__

#if (defined(_MSC_VER))
#pragma warning(disable:4996)
#endif

#define STACK_MEMORY_FOR_WIDE_STRING_FORMAT

#include <string>
#include <iostream>
#include <array>
#include <cstring>
#include <cassert>
#include <charconv>
#include <cwchar>
#include <tuple>
#include <climits>
#include <cuchar>
#include <cmath>
#include <system_error>

#include "Portability.h"

//#undef _MSC_VER
//#define __GNUC__

#define	PADSIZE	     16
#define	DEFPREC		 6
#define MAXFRACT_F   60
#define MAXFRACT_LF  60
#define MAXFRACT_E   309

#if (defined(_MSC_VER))
    #define MAXFRACT_LE  309
#elif defined(__GNUC__) || defined(__GNUG__)
    #define MAXFRACT_LE  4933
#endif

#define MAXFRACT_A   20  // a A La LA

#if defined(STACK_MEMORY_FOR_WIDE_STRING_FORMAT)
    #define STACK_MEMORY_SIZE  1000
#endif

#define BUFSIZE_INT  32 // [ll l h hh j t z]iduoxX p c lc
#define BUFSIZE_S    1  // s

#if defined(STACK_MEMORY_FOR_WIDE_STRING_FORMAT)
    #define BUFSIZE_lS   STACK_MEMORY_SIZE // ls
#else
    #define BUFSIZE_lS   1 // ls
#endif

#define BUFSIZE_F    309 + MAXFRACT_F + 4 // f F a A g G

#if (defined(_MSC_VER))
    #define BUFSIZE_LF  309 + MAXFRACT_LF + 4 // Lf LF La LA Lg LG
#elif defined(__GNUC__) || defined(__GNUG__)
    #define BUFSIZE_LF  4933 + MAXFRACT_LF + 4
#endif 

#define BUFSIZE_E    MAXFRACT_E + 16 // e E
#define BUFSIZE_LE    MAXFRACT_LE + 16 // Le LE

#define BUFSIZE_A    MAXFRACT_A + 16 // a A La LA

/*
 * Flags used during conversion.
 */

#define	__FLAG_ALT         0x008  /* show prefixes 0x or 0 and show the decimal point even if no fractional digits present */
#define __FLAG_ZEROPAD     0x002  /* zero (as opposed to blank) pad */
#define	__FLAG_LADJUST     0x004  /* left adjustment */

#define	__FLAG_LONGDBL     0x010  /* long double */
#define	__FLAG_LONGINT     0x020  /* long integer */
#define	__FLAG_LLONGINT    0x040  /* long long integer */
#define	__FLAG_SHORTINT    0x080  /* short integer */
 /* C99 additional size modifiers: */
#define	__FLAG_SIZET       0x100  /* size_t */
#define	__FLAG_PTRDIFFT	   0x200  /* ptrdiff_t */
#define	__FLAG_INTMAXT     0x400  /* intmax_t */
#define	__FLAG_CHARINT     0x800  /* print char using int format */

//#define	__FLAG_INTEGER_SIZE (__FLAG_LONGINT|__FLAG_LLONGINT|__FLAG_SHORTINT|__FLAG_SIZET|__FLAG_PTRDIFFT|__FLAG_INTMAXT|__FLAG_CHARINT)

 // #define	FPT		    0x100     /* Floating point number */
 // #define	GROUPING	0x200     /* use grouping ("'" flag) */

#define DYNAMIC_WIDTH      0x80000000
#define DYNAMIC_PRECISION  0x80000000

// A macro to disallow the copy constructor and operator= functions
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(const TypeName&) = delete;

using flags_t = int;
using width_t = int;
using precision_t = int;
using sign_t = char;
using terminal_t = char;
using begin_t = unsigned;
using end_t = unsigned;
using width_first_t = bool;


/** used by CFMT_STR */
struct OutbufArg {

	//static const char* BLANKS;
	//static const char* ZEROS;

	template <size_t N>
	OutbufArg(char(&buffer)[N])
		: pBuf_(buffer)
		, pBufBegind_(buffer)
		, pBufEnd_(buffer + N)
		, written_(0) {
	}

	OutbufArg(char* buffer, size_t size)
		: pBuf_(buffer)
		, pBufBegind_(buffer)
		, pBufEnd_(buffer + size)
		, written_(0) {
	}

	size_t getWrittenNum() {
		return written_;
	}

	void setWrittenNum(size_t n) {
		written_ += n;
	}

	size_t getAvailableSize() {
		size_t remaining = static_cast<size_t>(pBufEnd_ - pBuf_ - 1);

		// fail if at the end of buffer, recall need a single byte for null
		if (static_cast<std::make_signed<size_t>::type>(remaining) <= 0)
			return 0;
		else
			return remaining;
	}

	char* getBufPtr() {
		return pBuf_;
	}

	char* bufBegin() {
		return pBufBegind_;
	}

	void setBufPtr(size_t n) {
		// check if sufficient free space remains in the buffer
		// the last byte position is reserved for the terminating '\0' 
		size_t remaining = static_cast<size_t>(pBufEnd_ - (pBuf_ + n) - 1);

		// fail if at the end of buffer, recall need a single byte for null
		if (static_cast<std::make_signed<size_t>::type>(remaining) >= 0)
			pBuf_ += n;
	}

	void reset() {
		pBuf_ = pBufBegind_;
		written_ = 0;
	}

	void write(char ch) noexcept {
		++written_;

		// check if sufficient free space remains in the buffer
		// the last byte position is reserved for the terminating '\0' 
		size_t remaining = static_cast<size_t>(pBufEnd_ - pBuf_ - 1);

		// fail if at the end of buffer, recall need a single byte for null
		if (static_cast<std::make_signed<size_t>::type>(remaining) <= 0)
			return;
		else
			*pBuf_++ = ch;
	}

	/**
	 * Copies [n] bytes from source p to destination pBuf_, leaving pBuf_
	 * pointing at byte following block copied.
	 * If [n] exceeds the number of bytes available in pBuf_, only the number
	 * of bytes that fit within the buffer are copied. In this case [n] is
	 * still added to written_ (although no further copying will be performed)
	 * so that written_ can record the number of characters that would have
	 * been copied if the supplied buffer was of sufficient size.
	 */
	void write(const char* p, size_t n) {
		written_ += n;

		// check if sufficient free space remains in the buffer
		// the last byte position is reserved for the terminating '\0' 
		size_t remaining = static_cast<size_t>(pBufEnd_ - pBuf_ - 1);

		// fail if at the end of buffer, recall need a single byte for null
		if (static_cast<std::make_signed<size_t>::type>(remaining) <= 0)
			return;
		else if (n > remaining)
			n = remaining;

		std::memcpy(pBuf_, p, n);
		pBuf_ += n;
	}

	template <const char* const* padding/*char padding*/>
	void writePaddings(int n/*, const char padding = ' '*/) {
		if (n <= 0) return;

		written_ += static_cast<size_t>(n);

		// check if sufficient free space remains in the buffer
		// the last byte position is reserved for the terminating '\0' 
		size_t remaining = static_cast<size_t>(pBufEnd_ - pBuf_ - 1);

		// fail if at the end of buffer, recall need a single byte for null
		if (static_cast<std::make_signed<size_t>::type>(remaining) <= 0)
			return;
		else if (n > remaining)
			n = static_cast<int>(remaining);

		while (n > PADSIZE) {
			std::memcpy(pBuf_, *padding, PADSIZE);
			n -= PADSIZE;
			pBuf_ += PADSIZE;
		}
		std::memcpy(pBuf_, *padding, n);
		pBuf_ += n;

		//while (n-- > 0) {
		//	*pBuf_++ = padding;
		//}
	}

	void done() noexcept {
		//if (static_cast<size_t>(pBufEnd_ - pBuf_) > 0) 
		*pBuf_ = '\0';
	}

private:

	// running pointer to the next char
	char* pBuf_/*{ nullptr }*/;
	// const pointer to buffer end
	char* const pBufEnd_;
	// const pointer to buffer begin
	char* const pBufBegind_;

	// pointer to buffer end
	// char* pBufEnd_{ nullptr };

	// record the number of characters that would have been written if writing
	// buffer had been sufficiently large, not counting the terminating null 
	// character. 
	size_t written_/*{ 0 }*/;

	DISALLOW_COPY_AND_ASSIGN(OutbufArg);
};

static constexpr const char* BLANKS = "                ";
static constexpr const char* ZEROS = "0000000000000000";

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
static constexpr const char*  digit_3[256] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", 
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", 
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", 
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59", 
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", 
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79", 
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", 
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99", 
	"100", "101", "102", "103", "104", "105", "106", "107", "108", "109", 
	"110", "111", "112", "113", "114", "115", "116", "117", "118", "119", 
	"120", "121", "122", "123", "124", "125", "126", "127", "128", "129", 
	"130", "131", "132", "133", "134", "135", "136", "137", "138", "139", 
	"140", "141", "142", "143", "144", "145", "146", "147", "148", "149", 
	"150", "151", "152", "153", "154", "155", "156", "157", "158", "159", 
	"160", "161", "162", "163", "164", "165", "166", "167", "168", "169", 
	"170", "171", "172", "173", "174", "175", "176", "177", "178", "179", 
	"180", "181", "182", "183", "184", "185", "186", "187", "188", "189", 
	"190", "191", "192", "193", "194", "195", "196", "197", "198", "199", 
	"200", "201", "202", "203", "204", "205", "206", "207", "208", "209", 
	"210", "211", "212", "213", "214", "215", "216", "217", "218", "219", 
	"220", "221", "222", "223", "224", "225", "226", "227", "228", "229", 
	"230", "231", "232", "233", "234", "235", "236", "237", "238", "239", 
	"240", "241", "242", "243", "244", "245", "246", "247", "248", "249", 
	"250", "251", "252", "253", "254", "255"
};
#endif

//static constexpr const char* odigit_3[256] = {
//	"0", "1", "2", "3", "4", "5", "6", "7", "10", "11", "12", "13", "14", "15", "16", "17", "20", "21", "22", "23", "24", "25", "26", "27", "30", "31", "32", "33", "34", "35", "36", "37", "40", "41", "42", "43", "44", "45", "46", "47", "50", "51", "52", "53", "54", "55", "56", "57", "60", "61", "62", "63", "64", "65", "66", "67", "70", "71", "72", "73", "74", "75", "76", "77", "100", "101", "102", "103", "104", "105", "106", "107", "110", "111", "112", "113", "114", "115", "116", "117", "120", "121", "122", "123", "124", "125", "126", "127", "130", "131", "132", "133", "134", "135", "136", "137", "140", "141", "142", "143", "144", "145", "146", "147", "150", "151", "152", "153", "154", "155", "156", "157", "160", "161", "162", "163", "164", "165", "166", "167", "170", "171", "172", "173", "174", "175", "176", "177", "200", "201", "202", "203", "204", "205", "206", "207", "210", "211", "212", "213", "214", "215", "216", "217", "220", "221", "222", "223", "224", "225", "226", "227", "230", "231", "232", "233", "234", "235", "236", "237", "240", "241", "242", "243", "244", "245", "246", "247", "250", "251", "252", "253", "254", "255", "256", "257", "260", "261", "262", "263", "264", "265", "266", "267", "270", "271", "272", "273", "274", "275", "276", "277", "300", "301", "302", "303", "304", "305", "306", "307", "310", "311", "312", "313", "314", "315", "316", "317", "320", "321", "322", "323", "324", "325", "326", "327", "330", "331", "332", "333", "334", "335", "336", "337", "340", "341", "342", "343", "344", "345", "346", "347", "350", "351", "352", "353", "354", "355", "356", "357", "360", "361", "362", "363", "364", "365", "366", "367", "370", "371", "372", "373", "374", "375", "376", "377"
//};
//
//static constexpr const char* Xdigit_2[256] = {
//	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F", "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F", "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF", "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF", "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF", "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF", "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF", "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
//};
//
//static constexpr const char* xdigit_2[256] = {
//	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2a", "2b", "2c", "2d", "2e", "2f", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3a", "3b", "3c", "3d", "3e", "3f", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4a", "4b", "4c", "4d", "4e", "4f", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5a", "5b", "5c", "5d", "5e", "5f", "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6a", "6b", "6c", "6d", "6e", "6f", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7a", "7b", "7c", "7d", "7e", "7f", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8a", "8b", "8c", "8d", "8e", "8f", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9a", "9b", "9c", "9d", "9e", "9f", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af", "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf", "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da", "db", "dc", "dd", "de", "df", "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef", "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff"
//};

using u_short = unsigned short;

//static constexpr const char digits3[3001] = { "000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037038039040041042043044045046047048049050051052053054055056057058059060061062063064065066067068069070071072073074075076077078079080081082083084085086087088089090091092093094095096097098099100101102103104105106107108109110111112113114115116117118119120121122123124125126127128129130131132133134135136137138139140141142143144145146147148149150151152153154155156157158159160161162163164165166167168169170171172173174175176177178179180181182183184185186187188189190191192193194195196197198199200201202203204205206207208209210211212213214215216217218219220221222223224225226227228229230231232233234235236237238239240241242243244245246247248249250251252253254255256257258259260261262263264265266267268269270271272273274275276277278279280281282283284285286287288289290291292293294295296297298299300301302303304305306307308309310311312313314315316317318319320321322323324325326327328329330331332333334335336337338339340341342343344345346347348349350351352353354355356357358359360361362363364365366367368369370371372373374375376377378379380381382383384385386387388389390391392393394395396397398399400401402403404405406407408409410411412413414415416417418419420421422423424425426427428429430431432433434435436437438439440441442443444445446447448449450451452453454455456457458459460461462463464465466467468469470471472473474475476477478479480481482483484485486487488489490491492493494495496497498499500501502503504505506507508509510511512513514515516517518519520521522523524525526527528529530531532533534535536537538539540541542543544545546547548549550551552553554555556557558559560561562563564565566567568569570571572573574575576577578579580581582583584585586587588589590591592593594595596597598599600601602603604605606607608609610611612613614615616617618619620621622623624625626627628629630631632633634635636637638639640641642643644645646647648649650651652653654655656657658659660661662663664665666667668669670671672673674675676677678679680681682683684685686687688689690691692693694695696697698699700701702703704705706707708709710711712713714715716717718719720721722723724725726727728729730731732733734735736737738739740741742743744745746747748749750751752753754755756757758759760761762763764765766767768769770771772773774775776777778779780781782783784785786787788789790791792793794795796797798799800801802803804805806807808809810811812813814815816817818819820821822823824825826827828829830831832833834835836837838839840841842843844845846847848849850851852853854855856857858859860861862863864865866867868869870871872873874875876877878879880881882883884885886887888889890891892893894895896897898899900901902903904905906907908909910911912913914915916917918919920921922923924925926927928929930931932933934935936937938939940941942943944945946947948949950951952953954955956957958959960961962963964965966967968969970971972973974975976977978979980981982983984985986987988989990991992993994995996997998999" };
//template <size_t N>
//std::tuple<const char*, size_t> formatDec(char(&buf)[N], uintmax_t d) {
//	char* it = &buf[N - 3];
//	uintmax_t div = d / 1000;
//
//	while (div) {
//		std::memcpy(it, &digits3[3 * (d - div * 1000)], 3);
//		d = div;
//		it -= 3;
//		div = d / 1000;
//	}
//
//	std::memcpy(it, &digits3[3 * d], 3);
//
//	if (d < 10) {
//		it += 2;
//	}
//	else if (d < 100) {
//		it++;
//	}
//
//	return std::make_tuple(it, &buf[N] - it);
//}

template <size_t N>
std::tuple<const char*, size_t> formatDec(char(&buf)[N]/*char* &it*/, uintmax_t d) {

	static constexpr const u_short digit_pairs[100] = {
	    0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830, 0x3930,
	    0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731, 0x3831, 0x3931,
	    0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632, 0x3732, 0x3832, 0x3932,
	    0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533, 0x3633, 0x3733, 0x3833, 0x3933,
	    0x3034, 0x3134, 0x3234, 0x3334, 0x3434, 0x3534, 0x3634, 0x3734, 0x3834, 0x3934,
	    0x3035, 0x3135, 0x3235, 0x3335, 0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935,
	    0x3036, 0x3136, 0x3236, 0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936,
	    0x3037, 0x3137, 0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937,
	    0x3038, 0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938,
	    0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839, 0x3939
	};

	char* it = &buf[N - 2];
	uintmax_t div = d / 100;

	while (div) {
		*reinterpret_cast<u_short*>(it) = digit_pairs[d - div * 100];

		d = div;
		it -= 2;
		div = d / 100;
		// d > 0
	}
	// d < 100

	*reinterpret_cast<u_short*>(it) = digit_pairs[d];

	if (d < 10) {
		it++;
	}

	return std::make_tuple(it, &buf[N] - it);
}

template<size_t N>
std::tuple<const char*, size_t> formatOct(char(&buf)[N], uintmax_t d) {

	static constexpr const u_short digit_pairs[64] = {
		0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730,
		0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731,
		0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632, 0x3732,
		0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533, 0x3633, 0x3733,
		0x3034, 0x3134, 0x3234, 0x3334, 0x3434, 0x3534, 0x3634, 0x3734,
		0x3035, 0x3135, 0x3235, 0x3335, 0x3435, 0x3535, 0x3635, 0x3735,
		0x3036, 0x3136, 0x3236, 0x3336, 0x3436, 0x3536, 0x3636, 0x3736,
		0x3037, 0x3137, 0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737
	};

	char* p = &buf[N - 2];
	uintmax_t div = d >> 6;

	while (div) {
		//std::memcpy(p, &digit_pairs[2 * (d & 077)], 2);
		*reinterpret_cast<u_short*>(p) = digit_pairs[d & 077];
		d = div;
		p -= 2;
		div = d >> 6;
		// d > 0
	}
	// d < 64

	*reinterpret_cast<u_short*>(p) = digit_pairs[/*d & 077*/d];

	//std::memcpy(p, &digit_pairs[2 * (d & 077)], 2);

	if (d < 8) {
		p++;
	}

	return std::make_tuple(p, &buf[N] - p);
}


template <char T, size_t N>
std::tuple<const char*, size_t> formatHex(char(&buf)[N], uintmax_t d) {

	static constexpr const u_short Xdigit_pairs[256] = {
		0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830, 0x3930, 0x4130, 0x4230, 0x4330, 0x4430, 0x4530, 0x4630,
		0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731, 0x3831, 0x3931, 0x4131, 0x4231, 0x4331, 0x4431, 0x4531, 0x4631,
		0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632, 0x3732, 0x3832, 0x3932, 0x4132, 0x4232, 0x4332, 0x4432, 0x4532, 0x4632,
		0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533, 0x3633, 0x3733, 0x3833, 0x3933, 0x4133, 0x4233, 0x4333, 0x4433, 0x4533, 0x4633,
		0x3034, 0x3134, 0x3234, 0x3334, 0x3434, 0x3534, 0x3634, 0x3734, 0x3834, 0x3934, 0x4134, 0x4234, 0x4334, 0x4434, 0x4534, 0x4634,
		0x3035, 0x3135, 0x3235, 0x3335, 0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935, 0x4135, 0x4235, 0x4335, 0x4435, 0x4535, 0x4635,
		0x3036, 0x3136, 0x3236, 0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936, 0x4136, 0x4236, 0x4336, 0x4436, 0x4536, 0x4636,
		0x3037, 0x3137, 0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937, 0x4137, 0x4237, 0x4337, 0x4437, 0x4537, 0x4637,
		0x3038, 0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938, 0x4138, 0x4238, 0x4338, 0x4438, 0x4538, 0x4638,
		0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839, 0x3939, 0x4139, 0x4239, 0x4339, 0x4439, 0x4539, 0x4639,

		0x3041, 0x3141, 0x3241, 0x3341, 0x3441, 0x3541, 0x3641, 0x3741, 0x3841, 0x3941, 0x4141, 0x4241, 0x4341, 0x4441, 0x4541, 0x4641,
		0x3042, 0x3142, 0x3242, 0x3342, 0x3442, 0x3542, 0x3642, 0x3742, 0x3842, 0x3942, 0x4142, 0x4242, 0x4342, 0x4442, 0x4542, 0x4642,
		0x3043, 0x3143, 0x3243, 0x3343, 0x3443, 0x3543, 0x3643, 0x3743, 0x3843, 0x3943, 0x4143, 0x4243, 0x4343, 0x4443, 0x4543, 0x4643,
		0x3044, 0x3144, 0x3244, 0x3344, 0x3444, 0x3544, 0x3644, 0x3744, 0x3844, 0x3944, 0x4144, 0x4244, 0x4344, 0x4444, 0x4544, 0x4644,
		0x3045, 0x3145, 0x3245, 0x3345, 0x3445, 0x3545, 0x3645, 0x3745, 0x3845, 0x3945, 0x4145, 0x4245, 0x4345, 0x4445, 0x4545, 0x4645,
		0x3046, 0x3146, 0x3246, 0x3346, 0x3446, 0x3546, 0x3646, 0x3746, 0x3846, 0x3946, 0x4146, 0x4246, 0x4346, 0x4446, 0x4546, 0x4646
	};

	static constexpr const u_short xdigit_pairs[256] = {
		0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830, 0x3930, 0x6130, 0x6230, 0x6330, 0x6430, 0x6530, 0x6630,
		0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731, 0x3831, 0x3931, 0x6131, 0x6231, 0x6331, 0x6431, 0x6531, 0x6631,
		0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632, 0x3732, 0x3832, 0x3932, 0x6132, 0x6232, 0x6332, 0x6432, 0x6532, 0x6632,
		0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533, 0x3633, 0x3733, 0x3833, 0x3933, 0x6133, 0x6233, 0x6333, 0x6433, 0x6533, 0x6633,
		0x3034, 0x3134, 0x3234, 0x3334, 0x3434, 0x3534, 0x3634, 0x3734, 0x3834, 0x3934, 0x6134, 0x6234, 0x6334, 0x6434, 0x6534, 0x6634,
		0x3035, 0x3135, 0x3235, 0x3335, 0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935, 0x6135, 0x6235, 0x6335, 0x6435, 0x6535, 0x6635,
		0x3036, 0x3136, 0x3236, 0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936, 0x6136, 0x6236, 0x6336, 0x6436, 0x6536, 0x6636,
		0x3037, 0x3137, 0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937, 0x6137, 0x6237, 0x6337, 0x6437, 0x6537, 0x6637,
		0x3038, 0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938, 0x6138, 0x6238, 0x6338, 0x6438, 0x6538, 0x6638,
		0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839, 0x3939, 0x6139, 0x6239, 0x6339, 0x6439, 0x6539, 0x6639,

		0x3061, 0x3161, 0x3261, 0x3361, 0x3461, 0x3561, 0x3661, 0x3761, 0x3861, 0x3961, 0x6161, 0x6261, 0x6361, 0x6461, 0x6561, 0x6661,
		0x3062, 0x3162, 0x3262, 0x3362, 0x3462, 0x3562, 0x3662, 0x3762, 0x3862, 0x3962, 0x6162, 0x6262, 0x6362, 0x6462, 0x6562, 0x6662,
		0x3063, 0x3163, 0x3263, 0x3363, 0x3463, 0x3563, 0x3663, 0x3763, 0x3863, 0x3963, 0x6163, 0x6263, 0x6363, 0x6463, 0x6563, 0x6663,
		0x3064, 0x3164, 0x3264, 0x3364, 0x3464, 0x3564, 0x3664, 0x3764, 0x3864, 0x3964, 0x6164, 0x6264, 0x6364, 0x6464, 0x6564, 0x6664,
		0x3065, 0x3165, 0x3265, 0x3365, 0x3465, 0x3565, 0x3665, 0x3765, 0x3865, 0x3965, 0x6165, 0x6265, 0x6365, 0x6465, 0x6565, 0x6665,
		0x3066, 0x3166, 0x3266, 0x3366, 0x3466, 0x3566, 0x3666, 0x3766, 0x3866, 0x3966, 0x6166, 0x6266, 0x6366, 0x6466, 0x6566, 0x6666
	};

	static constexpr const u_short* digit_pairs = (T == 'X') ? Xdigit_pairs : xdigit_pairs;

	char* it = &buf[N - 2];
	uintmax_t div = d >> 8;

	while (div) {
		*reinterpret_cast<u_short*>(it) = digit_pairs[d & 0xff];

		d = div;
		it -= 2;
		div = d >> 8;
		// d > 0
	}
	// d < 256

	*reinterpret_cast<u_short*>(it) = digit_pairs[d];

	if (d < 16) {
		it++;
	}

	return std::make_tuple(it, &buf[N] - it);
}


template <terminal_t TM, flags_t FGS, size_t N, typename T>
//inline
std::tuple<const char*, size_t> formatDuble(char(&buf)[N], T arg, int prec) {

	std::to_chars_result ret;
	size_t size = 0;

	if constexpr (TM == 'f' || TM == 'F') {

		if constexpr ((FGS & __FLAG_LONGDBL) == __FLAG_LONGDBL) {
			long double ldbl = static_cast<long double>(arg);
			ret = std::to_chars(buf, buf + N, ldbl, std::chars_format::fixed,
				                prec);
		}
		else {
			double dbl = static_cast<double>(arg);
			ret = std::to_chars(buf, buf + N, dbl, std::chars_format::fixed, 
				                prec);
		}

		size = static_cast<size_t>(ret.ptr - buf);

		if constexpr ((FGS & __FLAG_ALT) == __FLAG_ALT) {
			//char* it = (char*)std::memchr(buf, '.', size);
			if (/*it == nullptr*/0 == prec) {
				*ret.ptr = '.';
				size++;
			}
		}
		//return std::make_tuple(buf, size);
	}
	else if constexpr (TM == 'e' || TM == 'E') {

		if constexpr ((FGS & __FLAG_LONGDBL) == __FLAG_LONGDBL) {
			long double ldbl = static_cast<long double>(arg);
			ret = std::to_chars(buf, buf + N, ldbl, 
				std::chars_format::scientific, prec);
		}
		else {
			double dbl = static_cast<double>(arg);
			ret = std::to_chars(buf, buf + N, dbl, 
				std::chars_format::scientific, prec);
		}

		size = static_cast<size_t>(ret.ptr - buf);

		if constexpr ((FGS & __FLAG_ALT) == __FLAG_ALT) {
			if (0 == prec) {
				char* p = ret.ptr - 2; // x.xxxxe+xx
				while (*(--p) != 'e');
				std::memmove(p + 1, p, static_cast<size_t>(ret.ptr - p));
				*p = '.';
				size++;
			}
		}
		//return std::make_tuple(buf, size);
	}
	else if constexpr (TM == 'a' || TM == 'A') {
		//buf[0] = '0';
		//buf[1] = (TM == 'a') ? 'x' : 'X';
		//size += 2;
		if constexpr ((FGS & __FLAG_LONGDBL) == __FLAG_LONGDBL) {
			long double ldbl = static_cast<long double>(arg);
			ret = std::to_chars(buf/* + 2*/, buf + N, ldbl,
				std::chars_format::hex, prec);
		}
		else {
			double dbl = static_cast<double>(arg);
			ret = std::to_chars(buf/* + 2*/, buf + N, dbl,
				std::chars_format::hex, prec);
		}

		size = static_cast<size_t>(ret.ptr - buf);

		if constexpr ((FGS & __FLAG_ALT) == __FLAG_ALT) {
			if (buf[/*3*/1] != '.') { // 0x1.xxxxxxxp+xxx
				std::memmove(buf + 2/*4*/, buf + /*3*/1, 
					static_cast<size_t>(ret.ptr - (buf + /*3*/1)));
				buf[/*3*/1] = '.';
				size++;
			}
		}
		//return std::make_tuple(buf, size);
	}
	else if constexpr (TM == 'g' || TM == 'G') {
		if constexpr ((FGS & __FLAG_LONGDBL) == __FLAG_LONGDBL) {
			long double ldbl = static_cast<long double>(arg);
			ret = std::to_chars(buf, buf + N, ldbl, std::chars_format::general,
				                prec);
		}
		else {
			double dbl = static_cast<double>(arg);
			ret = std::to_chars(buf, buf + N, dbl, std::chars_format::general,
				                prec);
		}

		size = static_cast<size_t>(ret.ptr - buf);
	}
	else { // invalid termnial specifier
		return std::make_tuple(nullptr, 0);
	}

	if (ret.ec == std::errc{}) // no error
		return std::make_tuple(buf, size);
	else // probably std::errc::value_too_large
		return std::make_tuple(nullptr, 0);
}


// A non-type template - parameter shall have one of the following 
// (optionally cv-qualified) types:
// integral or enumeration type,
// pointer to object or pointer to function,
// lvalue reference to object or lvalue reference to function,
// pointer to member,
// std::nullptr_t

/**
 * Stores the static format information associated with a specifier in a format
 * string (i.e. %<flags><width>.<precision><length><terminal>).
 */
struct SpecInfo {
	begin_t begin_ = 0;
	end_t end_ = 0;
	flags_t flags_ = 0;
	width_t width_ = 0;
	precision_t prec_ = -1;
	sign_t sign_ = '\0';
	terminal_t terminal_ = '\0';
	width_first_t wFirst_ = true; // indicate which one was extracted first in the 
								  // argument list when width and precision are both
								  // dynamically resolved
};
constexpr int LEN = sizeof(SpecInfo);


/**
 * Checks whether a character is with the terminal set of format specifier
 * characters according to the printf specification:
 * http://www.cplusplus.com/reference/cstdio/printf/
 *
 * \param c
 *      character to check
 * \return
 *      true if the character is in the set, indicating the end of the specifier
 */
constexpr inline bool
isTerminal(char c) {
	return c == 'd' || c == 'i'
		|| c == 'u' || c == 'o'
		|| c == 'x' || c == 'X'
		|| c == 'f' || c == 'F'
		|| c == 'e' || c == 'E'
		|| c == 'g' || c == 'G'
		|| c == 'a' || c == 'A'
		|| c == 'c' || c == 'p'
		|| c == 's' || c == 'n';
}

/**
 * Checks whether a character is in the set of characters that specifies
 * a flag according to the printf specification:
 * http://www.cplusplus.com/reference/cstdio/printf/
 *
 * \param c
 *      character to check
 * \return
 *      true if the character is in the set
 */
constexpr inline bool
isFlag(char c) {
	return c == '-' || c == '+' || c == ' ' || c == '#' || c == '0';
}

/**
 * Checks whether a character is in the set of characters that specifies
 * a length field according to the printf specification:
 * http://www.cplusplus.com/reference/cstdio/printf/
 *
 * \param c
 *      character to check
 * \return
 *      true if the character is in the set
 */
constexpr inline bool
isLength(char c) {
	return c == 'h' || c == 'l' || c == 'j'
		|| c == 'z' || c == 't' || c == 'L';
}

/**
 * Checks whether a character is a digit (0-9) or not.
 *
 * \param c
 *      character to check
 * \return
 *      true if the character is a digit
 */
constexpr inline bool
isDigit(char c) {
	return (c >= '0' && c <= '9');
}

constexpr inline int
toDigit(char c) {
	return static_cast<int>(c - '0');
}


/**
 * Analyzes a static printf style format string and extracts info of the num-th format specifier
 * would be used in a corresponding CFMT_STR() invocation.
 *
 * \tparam N
 *      Length of the static format string (automatically deduced)
 * \param fmt
 *      Format c style string to parse
 * \param num
 *      select the num-th format specifier to analyze (starts from zero)
 * \return
 *      Returns an SpecInfo object describing the detailed information of num-th format specifier
 */
//template<int N>
constexpr inline SpecInfo
getOneSpec(/*const char(&fmt)[N]*/const char* fmt, int num = 0) {
	begin_t begin = 0;
	end_t end = 0;
	flags_t flags = 0;
	width_t width = 0;
	precision_t prec = -1;
	sign_t sign = '\0';
	terminal_t terminal = '\0';
	width_first_t widthFirst = true;

	int intWidth = 0;
	int pos = 0;
	int specStartPos = 0;
	int sizeOfInvalidSpecs = 0;
	bool exit = false;

	//const char* fmt = *pFmt;
	//int num = N;

	while (num > 0) {
		// consume the current non-format string until reaching the next '%'
		// if num is greater than 0, fmt[pos] can never be '\0'
		for (; /*(fmt[pos] != '\0') && */(fmt[pos] != '%'); pos++);

		specStartPos = pos; // record the position of '%' as it appears
		pos++;   // fmt[pos] == '%', so pos++ skips over '%'

		while (isFlag(fmt[pos]) || isDigit(fmt[pos]) || isLength(fmt[pos])
			|| fmt[pos] == '.' || fmt[pos] == '*') {
			pos++;
		}

		if (isTerminal(fmt[pos])) {
			if (--num <= 0) begin = pos + 1 - sizeOfInvalidSpecs;
		}
		else { // isTerminal(fmt[pos]) == false
			// pos - specStartPos is the size of this invlaid specifier
			sizeOfInvalidSpecs += pos - specStartPos;
		}

		pos++;   // skips over '%', terminal, or non-terminal
	}

	while (true) {
		// consume the current non-format string until reaching the next '%' or the final '\0'
		for (; (fmt[pos] != '\0') && (fmt[pos] != '%'); pos++);

		if (fmt[pos] == '\0') {
			end = pos - sizeOfInvalidSpecs;
			exit = true;
			break;
		}

		specStartPos = pos; // record the position of '%' as it appears
		pos++;   // fmt[pos] == '%', so pos++ skips over '%'

		while (isFlag(fmt[pos]) || isDigit(fmt[pos]) || isLength(fmt[pos])
			|| fmt[pos] == '.' || fmt[pos] == '*') {
			pos++;
		}

		if (isTerminal(fmt[pos])) {
			end = specStartPos - sizeOfInvalidSpecs;
			break;
		}
		else { // isTerminal(fmt[pos]) == false
			// pos - specStartPos is the size of this invlaid specifier
			sizeOfInvalidSpecs += pos - specStartPos;
		}

		if (fmt[pos] == '\0') {
			end = pos - sizeOfInvalidSpecs;
			exit = true;
			break;
		}

		pos++;   // skips over '%' or non-terminal
	}

	pos = specStartPos + 1;  // fmt[specStartPos] == '%', so specStartPos + 1 over '%'

	while (!exit) {

		switch (fmt[pos]) {
		case ' ':
			/*-
			 * ``If the space and + flags both appear, the space
			 * flag will be ignored.''
			 * If the printed number is positive and "+" flag is not set, 
			 * print space in place of sign.
			 *	-- ANSI X3J11
			 */
			if (!sign)
				sign = ' ';
			break;
		case '#':
			flags |= __FLAG_ALT;
			break;
		case '*':
			/*-
			 * ``A negative field width argument is taken as a
			 * - flag followed by a positive field width.''
			 *	-- ANSI X3J11
			 * They don't exclude field widths read from args.
			 */
			width = DYNAMIC_WIDTH;
			if (prec == DYNAMIC_PRECISION)
				widthFirst = false;
			break;
		case '-':
			flags |= __FLAG_LADJUST;
			break;
		case '+':
			sign = '+';
			break;
		case '.':
			pos++;
			if (fmt[pos] == '*') {
				prec = DYNAMIC_PRECISION;
				if (width == DYNAMIC_WIDTH)
					widthFirst = true;
				break;
			}
			prec = 0;
			while (isDigit(fmt[pos])) {
				prec = 10 * prec + toDigit(fmt[pos]);
				pos++;
			}
			pos--;
			break;
		case '0':
			/*-
			 * ``Note that 0 is taken as a flag, not as the
			 * beginning of a field width.''
			 *	-- ANSI X3J11
			 */
			flags |= __FLAG_ZEROPAD;
			break;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			width = 0;
			do {
				width = 10 * width + toDigit(fmt[pos]);
				pos++;
			} while (isDigit(fmt[pos]));

			pos--;
			break;

		case 'L':
			flags |= __FLAG_LONGDBL;
			break;

		case 'h':
			pos++;
			if (fmt[pos] == 'h') {
				intWidth = __FLAG_CHARINT;
				break;
			}
			else {
				intWidth = __FLAG_SHORTINT;
				pos--;
				break;
			}
		case 'l':
			pos++;
			if (fmt[pos] == 'l') {
				intWidth = __FLAG_LLONGINT;
				break;
			}
			else {
				intWidth = __FLAG_LONGINT;
				pos--;
				break;
			}
		case 'j':
			intWidth = __FLAG_INTMAXT;
			break;
		case 't':
			intWidth = __FLAG_PTRDIFFT;
			break;
		case 'z':
			intWidth = __FLAG_SIZET;
			break;

		case 'c': case 'd': case 'i': case 'a': case 'A': case 'e': case 'E':
		case 'f': case 'F': case 'g': case 'G': case 'n': case 'o': case 'p':
		case 's': case 'u': case 'X': case 'x':
			terminal = fmt[pos];
			// end = pos + 1;
			exit = true;
			break;

		default: /* should never happen */
			abort();
			break;
		}

		pos++;
	}

	flags |= intWidth;
	//begin -= sizeOfInvalidSpecs;
	//end -= sizeOfInvalidSpecs;
	return { begin, end, flags, width, prec, sign, terminal, widthFirst };
}


/**
 * Helper to analyzeFormatString. This level of indirection is needed to
 * unpack the index_sequence generated in analyzeFormatString and
 * use the sequence as indices for calling getParamInfo.
 *
 * \tparam N
 *      Length of the format string (automatically deduced)
 * \tparam Indices
 *      An index sequence from [0, Num) where Num is the number of fmt
 *      specifiers in the format string (automatically deduced)
 *
 * \param fmt
 *      printf format string to analyze
 *
 * \return
 *      An std::array of format specifiers that describes the detailed
 *      info (position of the head '%', position of the end of specifier,
 *      width, precision, sign and terminal) of each specifier.
 */
 // std::integer_sequence Defined in header <utility>
 // template< class T, T... Ints >
 // class integer_sequence;
 // T: The type of the values; must be an integral type : bool, char, char16_t, 
 //    char32_t, wchar_t, or signed or unsigned integer types.
 // Ints: A non-type parameter pack that represents a sequence of values of
 //       integral type T.
 // The class template std::integer_sequence represents a compile-time sequence
 // of integers. When used as an argument to a function template, the parameter 
 // pack Ints can be deduced and used in pack expansion.
 // std::integer_sequence::size
 // static constexpr std::size_t size() noexcept;
 // Returns the number of elements in Ints. Equivalent to sizeof...(Ints)
 // A helper alias template std::index_sequence is defined for the common case 
 // where T is std::size_t:
 // template<std::size_t... Ints>
 // using index_sequence = std::integer_sequence<std::size_t, Ints...>;
 // Helper alias templates std::make_integer_sequence and std::make_index_sequence
 // are defined to simplify creation of std::integer_sequence and 
 // std::index_sequence types, respectively, with 0, 1, 2, ..., N-1 as Ints:
 // template<class T, T N>
 // using make_integer_sequence = std::integer_sequence<T, /* a sequence 0, 1, 2, ..., N-1 */ >;
 // template<std::size_t N>
 // using make_index_sequence = std::make_integer_sequence<std::size_t, N>;
 // The program is ill-formed if N is negative. If N is zero, the indicated type is integer_sequence<T>.
template<int N, std::size_t... Indices>
constexpr /*auto*/std::array<SpecInfo, sizeof...(Indices)> // Returns the number of elements in pack Indices
analyzeFormatStringHelper(const char(&fmt)[N]/*const char* fmt*/, std::index_sequence<Indices...>) {
	return { { getOneSpec(fmt, Indices)... } };
	//return std::make_tuple(getOneSpec(fmt, Indices)...);
}


/**
 * Computes a fmt_info_t array describing format specifiers (subsequences
 * beginning with %) that would be used with the provided printf style format
 * string. The indices of the array correspond with one of the specifiers in
 * format string.
 *
 * \template NParams
 *      The number of the format specifiers that follow the format
 *      string in a printf-like function.
 *      For example printf("%*.*ddsdf%%, %ll-+ K", 9, 8, 7)
 *      would have NParams = 3
 * \template N
 *      length of the printf style format string (automatically deduced)
 *
 * \param fmt
 *      Format string to generate the array for
 *
 * \return
 *      An std::array of format specifiers that describes the detailed
 *      info (position of the head '%', position of the end of specifier,
 *      width, precision, sign and terminal) of each specifier.
 */
template<int NParams, size_t N>
constexpr std::array<SpecInfo, NParams>/*auto*/
analyzeFormatString(const char(&fmt)[N]/*const char* fmt*/) {
	return analyzeFormatStringHelper(fmt, std::make_index_sequence<NParams>{});
}


template<int N>
constexpr inline char
storeOneChar(const char(&fmt)[N], int& offset, int num = 0) {
	int saveOffset = offset;

	// "sd%%sf%%%fes%h-+ 01233lzhhjt *.***k12.dsdsss%h-+ 01233lzhhjt *.***k23.\n"
	//  num    0 1 2 3 4 5 6 7 8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25  26 ...                
	//  offset 0 0 1 1 1 2 2 2 2  2  24 24 24 24 24 24 24 24 24 24 46 46 46 46 46 46  46 ...        
	//  index  0 1 3 4 5 7 8 9 10 11 34 35 36 37 38 39 40 41 42 43 66 67 68 69 70 71  72 ...
	//  char   s d % s f % % f e  s  k  1  2  .  d  s  d  s  s  s  k  2  3  .  \n \0  \0 ...            
	if (num + offset < N) {
		if (fmt[num + offset] == '%') {
			offset++;   // fmt[num + offset] == '%', so offset++ skips over '%'

			while (isFlag(fmt[num + offset]) || isDigit(fmt[num + offset])
				|| isLength(fmt[num + offset]) || fmt[num + offset] == '.'
				|| fmt[num + offset] == '*') {
				offset++;
			}

			if (isTerminal(fmt[num + offset])) {
				offset = saveOffset;
			}
		}

		return fmt[num + offset];
	}
	else
		return '\0';
}

template<int N, std::size_t... Indices>
constexpr std::array<char, sizeof...(Indices)> // Returns the number of elements in pack Indices
preprocessInvalidSpecsHelper(const char(&fmt)[N], std::index_sequence<Indices...>) {
	int offset = 0;
	return { { storeOneChar(fmt, offset, Indices)... } };
}

template<int Len, size_t N>
constexpr std::enable_if_t < Len < N, std::array<char, Len>>
	preprocessInvalidSpecs(const char(&fmt)[N]) {
	return preprocessInvalidSpecsHelper(fmt, std::make_index_sequence<Len>{});
}

template<int Len, size_t N>
constexpr std::enable_if_t< Len == N, std::array<char, 0>>
preprocessInvalidSpecs(const char(&fmt)[N]) {
	return std::array<char, 0>();
}

/*
 * Count the number of valid specifiers in the format string at compile time
 */
template<int N>
constexpr inline int
countValidSpecInfos(const char(&fmt)[N]) {
	int pos = 0;
	int count = 0;
	// A C string is usually declared as an array of char. However, an array 
	// of char is NOT by itself a C string. A valid C string requires the 
	// presence of a terminating "null character" (a character with ASCII
	// value 0, usually represented by the character literal '\0').
	for (;;) {
		// consume the current non-format string until reaching the next '%' 
		for (; (fmt[pos] != '\0') && (fmt[pos] != '%'); pos++);

		if (fmt[pos] == '\0')
			return count;

		pos++;   // fmt[pos] == '%', so pos++ skips over '%'

		while (isFlag(fmt[pos]) || isDigit(fmt[pos]) || isLength(fmt[pos])
			|| fmt[pos] == '.' || fmt[pos] == '*') {
			pos++;
		}

		if (isTerminal(fmt[pos])) {
			count++;
		}

		if (fmt[pos] == '\0')
			return count;

		pos++;   // skips over '%', terminal, or non-terminal
	}
}


/*
 * Count the minimum number of arguments required by valid specifiers in the format string
 */
template<SpecInfo... SIs>
constexpr inline int
countArgsRequired() {
	int count = 0;
	for (auto& SI : { SIs... }) {
		if (isTerminal(SI.terminal_)) count++;
		if (SI.width_ == DYNAMIC_WIDTH) count++;
		if (SI.prec_ == DYNAMIC_PRECISION) count++;
	}

	return count;
}


/*
 * return the size of the format string without fault specififers or format
 * escape sequence (e.g., "%-+ #0ltz%", "%-+ K", "%%")
 */
template<int N>
constexpr inline int
squeezeSoundSize(const char(&fmt)[N]) {
	int pos = 0;
	int specStartPos = 0;
	int size = N;

	for (;;) {
		// consume the current non-format string until reaching the next '%' 
		for (; (fmt[pos] != '\0') && (fmt[pos] != '%'); pos++);

		if (fmt[pos] == '\0')
			return size;

		specStartPos = pos; // record the position of '%' as it appears
		pos++;   // fmt[pos] == '%', so pos++ skips over '%'

		while (isFlag(fmt[pos]) || isDigit(fmt[pos]) || isLength(fmt[pos])
			|| fmt[pos] == '.' || fmt[pos] == '*') {
			pos++;
		}

		if (!isTerminal(fmt[pos])) {
			// pos - specStartPos is the size of this invlaid specifier
			size -= pos - specStartPos;
		}

		if (fmt[pos] == '\0')
			return size;

		pos++;   // skips over '%', terminal, or non-terminal
	}
}


/**
 * No-Op function that triggers the preprocessor's format checker for
 * printf format strings and argument parameters.
 *
 * \param format
 *      printf format string
 * \param ...
 *      format parameters
 */
//static void
//CFMT_PRINTF_FORMAT_ATTR(1, 2)
//checkFormat(CFMT_PRINTF_FORMAT const char*, ...) {}


// ============================================================================
// ============================================================================

template <class T>
inline constexpr int formattedPrec([[maybe_unused]] T/*&&*/ n) {
	if constexpr (std::is_integral_v<std::remove_reference_t<T>>)
		return static_cast<int>(n);
	else
		return static_cast<int>(-1);
}

template <class T>
inline constexpr int formattedWidth([[maybe_unused]] T/*&&*/ n) {
	if constexpr (std::is_integral_v<std::remove_reference_t<T>>)
		return static_cast<int>(n);
	else
		return static_cast<int>(0);
}

template<flags_t flag, typename T>
inline uintmax_t SARG(T/*&&*/ arg) {
	if constexpr ((flag & __FLAG_LONGINT) != 0) {
		return static_cast<intmax_t>(static_cast<long int>(arg));
	}
	else if constexpr ((flag & __FLAG_SHORTINT) != 0) {
		return static_cast<intmax_t>(static_cast<short int>(arg));
	}
	else if constexpr ((flag & __FLAG_CHARINT) != 0) {
		return static_cast<intmax_t>(static_cast<signed char>(arg));
	}
	else if constexpr ((flag & __FLAG_LLONGINT) != 0) {
		return static_cast<intmax_t>(static_cast<long long int>(arg));
	}
	else if constexpr ((flag & __FLAG_SIZET) != 0) {
		return static_cast<intmax_t>(static_cast<std::make_signed_t<size_t>>(arg));
	}
	else if constexpr ((flag & __FLAG_PTRDIFFT) != 0) {
		return static_cast<intmax_t>(static_cast<ptrdiff_t>(arg));
	}
	else if constexpr ((flag & __FLAG_INTMAXT) != 0) {
		return static_cast<intmax_t>(arg);
	}
	else {
		return static_cast<intmax_t>(static_cast<int>(arg));
	}
}

template<flags_t flag, typename T>
inline uintmax_t UARG(T/*&&*/ arg) {
	if constexpr ((flag & __FLAG_LONGINT) != 0) {
		return static_cast<uintmax_t>(static_cast<unsigned long int>(arg));
	}
	else if constexpr ((flag & __FLAG_SHORTINT) != 0) {
		return static_cast<uintmax_t>(static_cast<unsigned short int>(arg));
	}
	else if constexpr ((flag & __FLAG_CHARINT) != 0) {
		return static_cast<uintmax_t>(static_cast<unsigned char>(arg));
	}
	else if constexpr ((flag & __FLAG_LLONGINT) != 0) {
		return static_cast<uintmax_t>(static_cast<unsigned long long int>(arg));
	}
	else if constexpr ((flag & __FLAG_SIZET) != 0) {
		return static_cast<uintmax_t>(static_cast<size_t>(arg));
	}
	else if constexpr ((flag & __FLAG_PTRDIFFT) != 0) {
		return static_cast<uintmax_t>(static_cast<std::make_unsigned_t<ptrdiff_t>>(arg));
	}
	else if constexpr ((flag & __FLAG_INTMAXT) != 0) {
		return static_cast<uintmax_t>(arg);
	}
	else {
		return static_cast<uintmax_t>(static_cast<unsigned int>(arg));
	}
}

template<flags_t flags, terminal_t term>
inline constexpr int maxFractSize() { // for f F a A e E
	if (term == 'F' || term == 'f') {
		if ((flags & __FLAG_LONGDBL) == __FLAG_LONGDBL) // Lf LF
			return MAXFRACT_LF;
		else // f F
			return MAXFRACT_F;
	}
	else if (term == 'e' || term == 'E') { // Le LE
		if ((flags & __FLAG_LONGDBL) == __FLAG_LONGDBL)
			return MAXFRACT_LE;
		else // e E
			return MAXFRACT_E;
	}
	else if (term == 'a' || term == 'A') // a A La LA
		return MAXFRACT_A;
}

template<flags_t flags, terminal_t term>
inline constexpr size_t fitBufferSize() {
	if (term == 'F' || term == 'f') {
		if ((flags & __FLAG_LONGDBL) == __FLAG_LONGDBL) // Lf LF
			return static_cast<size_t>(BUFSIZE_LF);
		else // f F
			return static_cast<size_t>(BUFSIZE_F);
	}
	else if (term == 'e' || term == 'E') {
		if ((flags & __FLAG_LONGDBL) == __FLAG_LONGDBL) // Le LE
			return static_cast<size_t>(BUFSIZE_LE);
		else // e E
			return static_cast<size_t>(BUFSIZE_E);
	}
	else if (term == 'g' || term == 'G') {
		if ((flags & __FLAG_LONGDBL) == __FLAG_LONGDBL) {// Lg LG
			int buf = (BUFSIZE_LF > BUFSIZE_LE) ? BUFSIZE_LF : BUFSIZE_LE;
			return static_cast<size_t>(buf);
		}
		else { // g G
			int buf = (BUFSIZE_F > BUFSIZE_E) ? BUFSIZE_F : BUFSIZE_E;
			return static_cast<size_t>(buf);
		}
	}
	else if (term == 'a' || term == 'A') { // a A La LA
		return static_cast<size_t>(BUFSIZE_A);
	}
	else if (term == 's') {
		if ((flags & __FLAG_LONGINT) == __FLAG_LONGINT) //ls
			return static_cast<size_t>(BUFSIZE_lS);
		else // s
			return static_cast<size_t>(BUFSIZE_S); // useless
	}
	else // [ll l h hh j t z]iduoxX p c lc
		return static_cast<size_t>(BUFSIZE_INT);
}

/* template converter_impl declaration */
template<const char* const* fmt, SpecInfo... SIs, typename... Ts>
inline void converter_impl(OutbufArg& outbuf, Ts/*&&*/...args);

template<const char* const* fmt, SpecInfo SI, typename T>
inline void converter_single(OutbufArg& outbuf, T/*&&*/ arg, width_t W = 0,
	precision_t P = -1) {

	constexpr size_t BUF = fitBufferSize<SI.flags_, SI.terminal_>();

	//std::cout << BUF << std::endl; // for test

	char buf[BUF];	// space for %ls, %c, %lc, %[pdiouxX], %[aAeEfgG]
	const char* cp = nullptr;
	const char* pExpstr = nullptr;	// char pointer to exponent string: e+ZZ

#ifndef STACK_MEMORY_FOR_WIDE_STRING_FORMAT
	char* convbuf = nullptr; // wide to multibyte conversion result
#endif

	//int dprec = 0;	// a copy of prec if [diouxX], 0 otherwise
	int	fpprec = 0;	    // `extra' floating precision in [eEfgG]
	int	realsz = 0;	    // field size expanded by dprec
	size_t size = 0;
	size_t expsize = 0; // character count for expstr
	bool gtoe = false;  // is conversion for g/G with style e/E

	flags_t flags = SI.flags_;
	sign_t sign = SI.sign_;

	// output the characters between the previous specifier and this specifier
	// in fmt
	if constexpr (SI.end_ - SI.begin_ > 0) {
		outbuf.write(*fmt + SI.begin_, static_cast<size_t>(SI.end_ - SI.begin_));
	}

	if constexpr (SI.width_ != DYNAMIC_WIDTH
		&& SI.prec_ == DYNAMIC_PRECISION) {
		W = SI.width_;
		P = P < 0 ? -1 : P;
	}
	else if constexpr (SI.width_ == DYNAMIC_WIDTH
		&& SI.prec_ != DYNAMIC_PRECISION) {
		if (W < 0) {
			W = -W;
			flags |= __FLAG_LADJUST;
		}
		P = SI.prec_;
	}
	else if constexpr (SI.width_ != DYNAMIC_WIDTH
		&& SI.prec_ != DYNAMIC_PRECISION) {
		W = SI.width_;
		P = SI.prec_;
	}
	else {
		P = P < 0 ? -1 : P;
		if (W < 0) {
			W = -W;
			flags |= __FLAG_LADJUST;
		}
	}

	if constexpr (SI.terminal_ == 'c') {
		if constexpr ((SI.flags_ & __FLAG_LONGINT) == __FLAG_LONGINT) {
			if constexpr (std::is_convertible_v<T, /*wint_t*/char32_t>) {
				static const std::mbstate_t initial{};
				std::mbstate_t mbs{ initial };
				size_t mbseqlen;

				// T = char, T = char8_t, T = char16_t, T = char32_t, T= wchar_t,
				// or T = other types that can be converted to char32_t 
				mbseqlen = std::c32rtomb(buf,
					static_cast<char32_t>(arg), &mbs);
				if (mbseqlen == static_cast<size_t>(-1)) {
					outbuf.write("(ER)", sizeof("(ER)") - 1);
					return;
				}
				cp = buf;
				size = mbseqlen;
			}
			else {
				outbuf.write("(ER)", sizeof("(ER)") - 1);
				return;
			}
		}
		else {
			if constexpr (std::is_convertible_v<T, int>) {
				// T = char, T = char8_t, or 
				// T = other types that can be converted to int 
				buf[0] = static_cast<char>(static_cast<int>(arg));
				cp = buf;
				size = 1;
			}
			else {
				outbuf.write("(ER)", sizeof("(ER)") - 1);
				return;
			}
		}

		//P = 0;
		//sign = '\0';
	}

	if constexpr (SI.terminal_ == 's') {
		if constexpr (std::is_same_v<T, std::nullptr_t>) {
			cp = "(null)";
		}
		else if constexpr ((SI.flags_ & __FLAG_LONGINT) == __FLAG_LONGINT) {
			if constexpr (std::is_convertible_v<T, const wchar_t*>) {
				// wchar_t string is 2 bytes UTF-16 on Windows, 
				// 4 bytes UTF-32 (gcc/g++ and XCode) on Linux and OS,
				// and 2 bytes UTF-16 on Cygwin (cygwin uses Windows APIs)
				const wchar_t* wcp = static_cast<const wchar_t*>(arg);

				//if (nullptr == wcp)
				//	cp = "(null)";
				//else {
				static const mbstate_t initial{};
				std::mbstate_t mbs{ initial };

#if defined(STACK_MEMORY_FOR_WIDE_STRING_FORMAT)
				/* Allocate space for the maximum number of bytes we could output. */
				if (P < 0)
					size = static_cast<size_t>(STACK_MEMORY_SIZE);
				else
					size = P > static_cast<size_t>(STACK_MEMORY_SIZE)
					? static_cast<size_t>(STACK_MEMORY_SIZE) : P;

				/* Fill the output buffer. */
				if ((size = std::wcsrtombs(buf, &wcp, size, &mbs))
					== static_cast<size_t>(-1)) {
					outbuf.write("(ER)", sizeof("(ER)") - 1);
					return;
				}
				buf[size] = '\0';
				cp = buf;
#else
				/* Allocate space for the maximum number of bytes we could output. */
				if (P < 0)
					size = wcslen(wcp) * MB_CUR_MAX;
				else
					size = P;

				//std::cout << size << std::endl;
				convbuf = (char*)malloc(size + 1);

				if (convbuf == nullptr) {
					outbuf.write("(ER)", sizeof("(ER)") - 1);
					return;
				}

				/* Fill the output buffer. */
				if ((size = std::wcsrtombs(convbuf, &wcp, size, &mbs))
					== static_cast<size_t>(-1)) {
					free(convbuf);
					outbuf.write("(ER)", sizeof("(ER)") - 1);
					return;
				}
				convbuf[size] = '\0';
				cp = convbuf;
#endif
				//}
			}
			else {
				outbuf.write("(ER)", sizeof("(ER)") - 1);
				return;
			}
		}
		else {
			if constexpr (std::is_convertible_v<T, const char*>) {
				cp = static_cast<const char*>(arg);

				//if (nullptr == cp)
				//	cp = "(null)";
			}
			else {
				outbuf.write("(ER)", sizeof("(ER)") - 1);
				return;
			}
		}

		// get string size(<=P) excluding the tail '\0'
		if (P >= 0) {
			// can't use strlen; can only look for the
			// NULL in the first `prec' characters, and
			// strlen() will go further.
			// The C library function
			// void* memchr(const void* str, int c, size_t n) 
			// searches for the first occurrence of the character 
			// c(an unsigned char) in the first n bytes of the string 
			// pointed to by the argument str.
			// This function returns a pointer to the matching byte or NULL 
			// if the character does not occur in the given memory area.
			char* it = (char*)memchr(cp, 0, static_cast<size_t>(P));

			if (it != NULL) {
				size = static_cast<size_t>(it - cp);
				//if (size > static_cast<size_t>(P))
				//	size = static_cast<size_t>(P);
			}
			else
				size = static_cast<size_t>(P);
		}
		else
			size = strlen(cp);
	}

	if constexpr (SI.terminal_ == 'i' || SI.terminal_ == 'd' ||
		SI.terminal_ == 'x' || SI.terminal_ == 'X' ||
		SI.terminal_ == 'o' || SI.terminal_ == 'u' ||
		SI.terminal_ == 'p') {
		uintmax_t ujval;

		if constexpr (SI.terminal_ == 'p' && std::is_convertible_v<T, const void*>) {
			ujval = static_cast<uintmax_t>(reinterpret_cast<uintptr_t>(arg));
			//sign = '\0';
			P = 0;
			// NOTE: GNU printf prints "(nil)" for nullptr pointers, 
			// we print 0000000000000000
			cp = buf + BUF - 16;
			size = 16;
			std::memcpy(buf + BUF - 16, ZEROS, 16);
			/*std::tie(cp, size) = */formatHex<'X'>(buf, ujval);
		}

		else if constexpr ((SI.terminal_ == 'i' || SI.terminal_ == 'd' || SI.terminal_ == 'u') &&
			std::is_integral_v<std::remove_reference_t<T>>) {

			if constexpr (SI.terminal_ == 'i' || SI.terminal_ == 'd') {
				ujval = SARG<SI.flags_>(/*std::forward<T>*/(arg));
				if (static_cast<intmax_t/*std::make_signed<uintmax_t>::type*/>(ujval) < 0) {
					ujval = 0 - ujval;
					sign = '-';
				}
			}
			else if constexpr (SI.terminal_ == 'u') {
				ujval = UARG<SI.flags_>(/*std::forward<T>*/(arg));
				//sign = '\0';
			}

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
			if constexpr ((SI.flags_ & __FLAG_CHARINT) == __FLAG_CHARINT) {
				cp = const_cast<char*>(digit_3[ujval]);
				size = ujval < 10 ? 1 : (ujval < 100 ? 2 : 3);
			}
			else {
				if (ujval < 100) {
					cp = const_cast<char*>(digit_3[ujval]);
					size = ujval < 10 ? 1 : 2;
				}
				else
					std::tie(cp, size) = formatDec(buf, ujval);
			}
#else
			std::tie(cp, size) = formatDec(buf, ujval);
#endif
		}

		else if constexpr (SI.terminal_ == 'o' &&
			std::is_integral_v<std::remove_reference_t<T>>) {
			ujval = UARG<SI.flags_>(/*std::forward<T>*/(arg));
			//sign = '\0';

			std::tie(cp, size) = formatOct(buf, ujval);

			// handle octal leading 0
			if constexpr ((SI.flags_ & __FLAG_ALT) == __FLAG_ALT) {
				if (*cp != '0') {
					*const_cast<char*>(--cp) = '0';
					size++;
				}
			}
		}

		else if constexpr ((SI.terminal_ == 'x' || SI.terminal_ == 'X') &&
			std::is_integral_v<std::remove_reference_t<T>>) {
			ujval = UARG<SI.flags_>(/*std::forward<T>*/(arg));
			//sign = '\0';

			std::tie(cp, size) = formatHex<SI.terminal_>(buf, ujval);

			// handle octal leading prefix 0x or 0X
			if constexpr ((SI.flags_ & __FLAG_ALT) == __FLAG_ALT) {
				if (*cp != '0') {
					*const_cast<char*>(--cp) = SI.terminal_;
					*const_cast<char*>(--cp) = '0';
					size += 2;
				}
			}
		}

		else {
			outbuf.write("(ER)", sizeof("(ER)") - 1);
			return;
		}

		/*-
		 * ``... diouXx conversions ... if a precision is
		 * specified (P >= 0), the 0 flag will be ignored.''
		 *	-- ANSI X3J11 */
		if /*((dprec = P) >= 0)*/ (P >= 0)
			flags &= ~__FLAG_ZEROPAD;
	}

	if constexpr (SI.terminal_ == 'n') {
		if constexpr (std::is_convertible_v<T, const void*>) {
			if constexpr ((SI.flags_ & __FLAG_LONGINT) == __FLAG_LONGINT)
				*reinterpret_cast<long int*>(arg) = outbuf.getWrittenNum();
			else if constexpr (
				(SI.flags_ & __FLAG_SHORTINT) == __FLAG_SHORTINT)
				*reinterpret_cast<short int*>(arg) = outbuf.getWrittenNum();
			else if constexpr ((SI.flags_ & __FLAG_CHARINT) == __FLAG_CHARINT)
				*reinterpret_cast<signed char*>(arg) = outbuf.getWrittenNum();
			else if constexpr (
				(SI.flags_ & __FLAG_LLONGINT) == __FLAG_LLONGINT)
				*reinterpret_cast<long long int*>(arg) = 
				    outbuf.getWrittenNum();
			else if constexpr ((SI.flags_ & __FLAG_INTMAXT) == __FLAG_INTMAXT)
				*reinterpret_cast<intmax_t*>(arg) = outbuf.getWrittenNum();
			else if constexpr ((SI.flags_ & __FLAG_SIZET) == __FLAG_SIZET)
				*reinterpret_cast<std::make_signed_t<size_t>*>(arg) = 
				    outbuf.getWrittenNum();
			else if constexpr (
				(SI.flags_ & __FLAG_PTRDIFFT) == __FLAG_PTRDIFFT)
				*reinterpret_cast<ptrdiff_t*>(arg) = outbuf.getWrittenNum();
			else
				*reinterpret_cast<int*>(arg) = outbuf.getWrittenNum();
		}
		else
			outbuf.write("(ER)", sizeof("(ER)") - 1);

		return;
	}

	if constexpr (SI.terminal_ == 'f' || SI.terminal_ == 'F'
		|| SI.terminal_ == 'a' || SI.terminal_ == 'A'
		|| SI.terminal_ == 'e' || SI.terminal_ == 'E'
		|| SI.terminal_ == 'g' || SI.terminal_ == 'G') {

		if constexpr (std::is_floating_point_v<std::remove_reference_t<T>>) {
			auto stat = std::fpclassify(arg);
			switch (/*auto stat = std::fpclassify(arg)*/stat) {
			case FP_NORMAL:
			case FP_SUBNORMAL:
			case FP_ZERO:
				if (arg < 0.0) {
					arg = std::abs(arg);
					sign = '-';
				}

				if constexpr (SI.terminal_ != 'g' && SI.terminal_ != 'G') {
					constexpr int MAXFRACT = 
						maxFractSize<SI.flags_, SI.terminal_>();
					if (P > MAXFRACT) { // do realistic precision
						/*if constexpr ((SI.terminal_ != 'g' && SI.terminal_ != 'G')
							|| (SI.flags_ & __FLAG_ALT) == __FLAG_ALT)*/
						fpprec = P - MAXFRACT;
						P = MAXFRACT;	// they asked for it!
					}
					std::tie(cp, size) =
						formatDuble<SI.terminal_, SI.flags_>(buf, arg, P);
				}
				else { // SI.terminal_ == 'g' || SI.terminal_ == 'G'
					/** 
					 * g/G format: floating-point number is converted in style
					 * f or e(or in style F or E in the case of a G conversion
					 * specifier), depending on the value convertedand the
					 * precision. Let P equal the precision if nonzero, 6 if
					 * the precision is omitted(-1), or 1 if the precision is
					 * zero. 
					 * Then, if a conversion with style E would have an exponent
					 * of X: 
					 * if P > X >= -4, the conversion is with style f(or F) and
					 * precision P - (X + 1).
					 * otherwise, the conversion is with style e(or E) and 
					 * precision P - 1.
					 * Finally, unless the # flag is used, any trailing zeros
					 * are removed from the fractional portion of the result and
					 * the decimal-point character is removed if there is no 
					 * fractional portion remaining. */
					if constexpr ((SI.flags_ & __FLAG_ALT) == __FLAG_ALT) {
						P = (-1 == P) ? 6 : ((0 == P) ? 1 : P);
						int exp = 0;
						if (FP_ZERO != stat)
							exp = static_cast<int>(std::floor(std::log10(arg)));
						if (P > exp && exp >= -4) {
							P = P - (exp + 1);
							constexpr int MAXFRACT =
								maxFractSize<SI.flags_, 'f'>();
							if (P > MAXFRACT) {
								fpprec = P - MAXFRACT;
								P = MAXFRACT;
							}
							std::tie(cp, size) = (SI.terminal_ == 'g') 
								? formatDuble<'f', SI.flags_>(buf, arg, P) 
								: formatDuble<'F', SI.flags_>(buf, arg, P);
						}
						else {
							--P;
							constexpr int MAXFRACT =
								maxFractSize<SI.flags_, 'e'>();
							if (P > MAXFRACT) {
								fpprec = P - MAXFRACT;
								P = MAXFRACT;
							}
							std::tie(cp, size) = (SI.terminal_ == 'g')
								? formatDuble<'e', SI.flags_>(buf, arg, P)
								: formatDuble<'E', SI.flags_>(buf, arg, P);
							gtoe = true;
						}
					}
					else {
						//if (P > exp && exp >= -4) {
						//	P = P - (exp + 1);
						//	constexpr int MAXFRACT = 
						//		maxFractSize<SI.flags_, 'f'>();
						//	if (P > MAXFRACT) P = MAXFRACT;
						//}
						//else {
						//	--P;
						//	constexpr int MAXFRACT =
						//		maxFractSize<SI.flags_, 'e'>();
						//	if (P > MAXFRACT) P = MAXFRACT;
						//}
						std::tie(cp, size) =
							formatDuble<SI.terminal_, SI.flags_>(buf, arg, P);
					}
				}
				break;

			case FP_INFINITE:
				// fill in the string
				if constexpr (SI.terminal_ == 'E' || SI.terminal_ == 'G'
					|| SI.terminal_ == 'F' || SI.terminal_ == 'A')
					std::memcpy(buf, "INF", 3);
				else
					std::memcpy(buf, "inf", 3);

				if (arg < 0.0)  // is negative ?
					sign = '-'; // need a sign

				cp = buf;
				size = 3;  // length will be three, excluding tail '\0'
				flags &= ~__FLAG_ZEROPAD;
				break;
			case FP_NAN:
				// fill in the string
				if constexpr (SI.terminal_ == 'E' || SI.terminal_ == 'G'
					|| SI.terminal_ == 'F' || SI.terminal_ == 'A')
					std::memcpy(buf, "NAN", 3);
				else
					std::memcpy(buf, "nan", 3);

				cp = buf;
				size = 3;  // length will be three
				flags &= ~__FLAG_ZEROPAD;
				break;
			default: // unknown floating-Point representation
				outbuf.write("(ER)", sizeof("(ER)") - 1);
				return;
			}
		}
		else {
			outbuf.write("(ER)", sizeof("(ER)") - 1);
			return;
		}
	}

	if constexpr (SI.terminal_ != 'n') {
		// All reasonable formats wind up here. At this point, `cp' points to a
		// string which (if not flags&__FLAG_LADJUST) should be padded out to
		// `width' places. If flags&__FLAG_ZEROPAD, it should first be prefixed
		// by any sign or other prefix; otherwise, it should be blank padded
		// before the prefix is emitted. After any left-hand padding and 
		// prefixing, emit zeroes required by a decimal [diouxX] precision, 
		// then print the string properly, then emit zeroes required by any
		// leftover floating  precision; finally, if flags&_FLAG_LADJUST, pad
		// with blanks.

		// compute actual size, so we know how much to pad.
		// fieldsz excludes decimal prec; realsz includes it

		if constexpr (SI.terminal_ == 'i' || SI.terminal_ == 'd' ||
			SI.terminal_ == 'x' || SI.terminal_ == 'X' ||
			SI.terminal_ == 'o' || SI.terminal_ == 'u') {

			//fieldsz = static_cast<int>(size);
			realsz = (P > static_cast<int>(size)) ? P : static_cast<int>(size);
		}
		else if constexpr (SI.terminal_ == 'F' || SI.terminal_ == 'f'
			|| SI.terminal_ == 'a' || SI.terminal_ == 'A'
			|| SI.terminal_ == 'e' || SI.terminal_ == 'E'
			|| SI.terminal_ == 'g' || SI.terminal_ == 'G') {
			realsz = static_cast<int>(size + fpprec); // normally fpprec is 0
		}
		else // SI.terminal_: 'c' 's' 'p'
			realsz = static_cast<int>(size);

		if constexpr (SI.terminal_ == 'F' || SI.terminal_ == 'f'
			|| SI.terminal_ == 'a' || SI.terminal_ == 'A'
			|| SI.terminal_ == 'e' || SI.terminal_ == 'E'
			|| SI.terminal_ == 'g' || SI.terminal_ == 'G'
			|| SI.terminal_ == 'i' || SI.terminal_ == 'd') {
			if (sign)
				realsz++;
		}
		//if (ox[1])
		//	realsz += 2;

		// handle hex prefix 0x or 0X
		//if constexpr ((SI.terminal_ == 'x' || SI.terminal_ == 'X')
		//	&& (SI.flags_ & __FLAG_ALT) == __FLAG_ALT) {
		//	if (*cp != '0') {
		//		realsz += 2;
		//	}
		//}

		///////////////////////////// Right Adjust ////////////////////////////
		/* right-adjusting blank padding */
		if ((flags & (__FLAG_ZEROPAD | __FLAG_LADJUST)) == 0) {
			outbuf.writePaddings<&BLANKS>(W - realsz);
		}
		///////////////////////////// Right Adjust ////////////////////////////

		/* prefix */
		if constexpr (SI.terminal_ == 'F' || SI.terminal_ == 'f'
			|| SI.terminal_ == 'a' || SI.terminal_ == 'A'
			|| SI.terminal_ == 'e' || SI.terminal_ == 'E'
			|| SI.terminal_ == 'g' || SI.terminal_ == 'G'
			|| SI.terminal_ == 'i' || SI.terminal_ == 'd') {
			if (sign)
				outbuf.write(sign);
		}
		//if constexpr (SI.terminal_ == 'x' && (SI.flags_ & __FLAG_ALT) == __FLAG_ALT) {
		//	if (*cp != '0') outbuf.write("0x", 2);
		//}
		//if constexpr (SI.terminal_ == 'X' && (SI.flags_ & __FLAG_ALT) == __FLAG_ALT) {
		//	if (*cp != '0') outbuf.write("0X", 2);
		//}
		//if constexpr ((SI.terminal_ == 'x' || SI.terminal_ == 'X') 
		//	&& (SI.flags_ & __FLAG_ALT) == __FLAG_ALT) {
		//	if (*cp != '0') {
		//		outbuf.write('0');
		//		outbuf.write(SI.terminal_);
		//	}
		//}

		///////////////////////////// Right Adjust ////////////////////////////
		/* right-adjusting zero padding */
		if ((flags & (__FLAG_ZEROPAD | __FLAG_LADJUST)) == __FLAG_ZEROPAD) {
			outbuf.writePaddings<&ZEROS>(W - realsz);
		}
		///////////////////////////// Right Adjust ////////////////////////////

		// [diouXx] leading zeroes from decimal precision
		// when P > fieldsz, realsz == P and zero padding size is P - fieldsz;
		// when P =< fieldsz, realsz == fieldsz and zero padding is skipped.
		if constexpr (SI.terminal_ == 'i' || SI.terminal_ == 'd' ||
			SI.terminal_ == 'x' || SI.terminal_ == 'X' ||
			SI.terminal_ == 'o' || SI.terminal_ == 'u') {
			outbuf.writePaddings<&ZEROS>(/*dprec*/P - /*fieldsz*/size);
		}

		// the string or number proper
		if constexpr (SI.terminal_ == 'e' || SI.terminal_ == 'E'
			|| SI.terminal_ == 'a' || SI.terminal_ == 'A') {
			if (fpprec > 0) {
				char exp = (SI.terminal_ == 'e' || SI.terminal_ == 'E') ? 
					'e' : 'p';

				pExpstr = cp + size - 2; // x.xxxxe+xx  1.xxxxp+xxx
				while (*(--pExpstr) != /*'e'*/exp);
				expsize = static_cast<size_t>(cp + size - pExpstr);
				size = static_cast<size_t>(pExpstr - cp);
			}
		}

		if constexpr (SI.terminal_ == 'g' || SI.terminal_ == 'G') {
			if (fpprec > 0 && gtoe) {
				pExpstr = cp + size - 2; // x.xxxxe+xx  1.xxxxp+xxx
				while (*(--pExpstr) != 'e');
				expsize = static_cast<size_t>(cp + size - pExpstr);
				size = static_cast<size_t>(pExpstr - cp);
			}
		}
		    
		outbuf.write(cp, size);

		// trailing floating point zeroes
		if constexpr (SI.terminal_ == 'F' || SI.terminal_ == 'f'
			|| SI.terminal_ == 'a' || SI.terminal_ == 'A'
			|| SI.terminal_ == 'e' || SI.terminal_ == 'E'
			|| SI.terminal_ == 'g' || SI.terminal_ == 'G') {
			outbuf.writePaddings<&ZEROS>(fpprec);
		}

		if constexpr (SI.terminal_ == 'e' || SI.terminal_ == 'E'
			|| SI.terminal_ == 'a' || SI.terminal_ == 'A'
			|| SI.terminal_ == 'g' || SI.terminal_ == 'G') {
			if (fpprec > 0) {
				outbuf.write(pExpstr, expsize);
			}
		}

		////////////////////////////// Left Adjust ////////////////////////////
		/* left-adjusting padding (always blank) */
		if ((flags & __FLAG_LADJUST) == __FLAG_LADJUST)
			outbuf.writePaddings<&BLANKS>(W - realsz);
		////////////////////////////// Left Adjust ////////////////////////////

#ifndef STACK_MEMORY_FOR_WIDE_STRING_FORMAT
		if constexpr (SI.terminal_ == 's' &&
			(SI.flags_ & __FLAG_LONGINT) == __FLAG_LONGINT)
			/*delete*/
			free(convbuf);
#endif 
	}
}


template<const char* const* fmt, SpecInfo SI, SpecInfo... SIs, typename T, typename... Ts>
inline void converter_args(OutbufArg& outbuf, T/*&&*/ arg, Ts/*&&*/... rest) {
	converter_single<fmt, SI>(outbuf, /*std::forward<T>*/(arg));
	converter_impl<fmt, SIs...>(outbuf, /*std::forward<Ts>*/(rest)...);
}


template<const char* const* fmt, SpecInfo SI, SpecInfo... SIs, typename D, typename T, typename... Ts>
inline void converter_D_args(OutbufArg& outbuf, D/*&&*/ d, T/*&&*/ arg, Ts/*&&*/... rest) {
	if constexpr (SI.width_ == DYNAMIC_WIDTH) {
		// test 
		//std::cout << "\nwidth: " << d << std::endl;
		//std::cout << "arg: " << arg << std::endl;
		//std::cout << std::endl;

		converter_single<fmt, SI>(outbuf, /*std::forward<T>*/(arg),
			formattedWidth(/*std::forward<D>*/(d)), -1);
	}
	else if constexpr (SI.prec_ == DYNAMIC_PRECISION) {
		// test 
		//std::cout << "\nprec: " << d << std::endl;
		//std::cout << "arg: " << arg << std::endl;
		//std::cout << std::endl;

		converter_single<fmt, SI>(outbuf, /*std::forward<T>*/(arg), 0,
			formattedPrec(/*std::forward<D>*/(d)));
	}
	else { /* should never happen */
		abort();
	}
	converter_impl<fmt, SIs...>(outbuf, /*std::forward<Ts>*/(rest)...);
}

template<const char* const* fmt, SpecInfo SI, SpecInfo... SIs, typename D1, typename D2, typename T, typename... Ts>
inline void converter_D_D_args(OutbufArg& outbuf, D1/*&&*/ d1, D2/*&&*/ d2, T/*&&*/ arg, Ts/*&&*/... rest) {
	if constexpr (SI.wFirst_) {
		// test 
		//std::cout << "\nwidth: " << d1 << std::endl;
		//std::cout << "prec: " << d2 << std::endl;
		//std::cout << "arg: " << arg << std::endl;
		//std::cout << std::endl;

		converter_single<fmt, SI>(outbuf, /*std::forward<T>*/(arg),
			formattedWidth(/*std::forward<D1>*/(d1)),
			formattedPrec(/*std::forward<D2>*/(d2)));
	}
	else {
		// test 
		//std::cout << "\nwidth: " << d2 << std::endl;
		//std::cout << "prec: " << d1 << std::endl;
		//std::cout << "arg: " << arg << std::endl;
		//std::cout << std::endl;

		converter_single<fmt, SI>(outbuf, /*std::forward<T>*/(arg),
			formattedWidth(/*std::forward<D2>*/(d2)),
			formattedPrec(/*std::forward<D1>*/(d1)));
	}

	converter_impl<fmt, SIs...>(outbuf, /*fmt,*/ /*std::forward<Ts>*/(rest)...);
}

template<const char* const* fmt, SpecInfo... SIs, typename... Ts>
inline void converter_impl(OutbufArg& outbuf, Ts/*&&*/...args) {
	// According to CFMT_STR implementation, at least one argument exists in the
	// template parameter pack SpecInfo... SIs (tailed SI).
	constexpr auto& SI = std::get<0>(std::forward_as_tuple(SIs...));

	if constexpr (sizeof ...(SIs) > 1) {
		if constexpr (SI.width_ == DYNAMIC_WIDTH
			&& SI.prec_ == DYNAMIC_PRECISION) {
			converter_D_D_args<fmt, SIs...>(outbuf, /* std::forward<Ts>*/(args)...);
		}
		else if constexpr (SI.width_ == DYNAMIC_WIDTH
			|| SI.prec_ == DYNAMIC_PRECISION) {
			converter_D_args<fmt, SIs...>(outbuf, /*std::forward<Ts>*/(args)...);
		}
		else {
			converter_args<fmt, SIs...>(outbuf, /*std::forward<Ts>*/(args)...);
		}
	}
	else {
		// Fetch tail SpecInfo to terminate the formatting process.
		outbuf.write(*fmt + SI.begin_, static_cast<size_t>(SI.end_ - SI.begin_));
	}
}

template</*const char* const* pRTStr,*/ SpecInfo... SIs>
struct Converter {
	constexpr Converter() {}

	template <const char* const* pRTStr, typename... Ts>
	void convert(OutbufArg& outbuf, Ts/*&&*/... args) const {
		constexpr auto numArgsReuqired = countArgsRequired<SIs...>();
		if constexpr (static_cast<uint32_t>(numArgsReuqired) > 
			static_cast<uint32_t>(sizeof...(Ts))) {
			std::cerr << "CFMT: forced abort due to illegal number of variadic"
				" arguments passed to CFMT_STR for converting\n"
				"(Required: " << numArgsReuqired << " ---- " <<
				"Passed: " << (sizeof...(Ts)) << ")";
			abort();
		}
		else {
			converter_impl</*fmt*/pRTStr, SIs...>(outbuf, /*std::forward<Ts>*/(args)...);
		}
	}

};

//template<const auto tuple_like, template<auto...> typename Template>
//constexpr decltype(auto) unpack() {
//	constexpr auto size = std::tuple_size_v<decltype(tuple_like)>;
//	return []<std::size_t... Is>(std::index_sequence<Is...>) {
//		return Template<std::get<Is>(tuple_like)...>{};
//	} (std::make_index_sequence<size>{});
//}
//}

template<int N, template</*auto*/SpecInfo...> typename Template,
	     /*auto*/const char* const* fmt>
	constexpr decltype(auto) unpack() {
	return[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		return Template <getOneSpec(*fmt, Is)... > {};
	} (std::make_index_sequence<N>{});
}

// ============================================================================
// ============================================================================

/**
 * CFMT_STR macro used for caching format specifier infos at compile time.
 *
 * \param result
 *      The variable to store the number of characters that would have been
 *      written to buffer if count had been sufficiently large, not counting
 *      the terminating null character.
 *      Notice that only when this returned value is non-negative and less
 *      than count, the string has been completely written.
 * \param buffer
 *      buffer to write to formatted string
 * \param count
 *      max number of characters to store in buffer
 * \param format
 *      printf-like format string (must be literal)
 * \param ... (additional arguments)
 *      Depending on the format string, the function may expect a sequence of
 *      additional arguments, each containing a value to be used to replace a
 *      format specifier in the format string (or a pointer to a storage
 *      location, for specifier n). There should be at least as many of these
 *      arguments as the number of valid format specifiers.
 */
#define CFMT_STR(result, buffer, count, format, ...) do {                      \
constexpr int kNVSIs = countValidSpecInfos(format);                            \
constexpr int kSS = squeezeSoundSize(format);                                  \
static constexpr auto fmtRawStr = format;                                      \
/**
 * A static variable inside a scope or function is something different than
 * a global static variable. Since there can be as many scoped statics with
 * the same name as you like (provided they are all in different scopes),
 * the compiler might have to change their names internally (incorporating
 * the function's name or the line number or whatever), so that the linker
 * can tell them apart.
 * Adding the static keyword to the file scope version of variables doesn't
 * change their extent, but it does change its visibility with respect to
 * other translation units; the name is not exported to the linker, so it
 * cannot be accessed by name from another translation unit. */               \
/*static constexpr std::array<SpecInfo, kNVSIs + 1> kSIs */                   \
	/*= analyzeFormatString<kNVSIs + 1>(format);*/                            \
static constexpr auto kfmtArr = preprocessInvalidSpecs<kSS>(format);          \
static constexpr auto kRTStr = kSS < sizeof(format)? kfmtArr.data() : format; \
/** 
 * use the address of the pointer to a string literal (&kRTStr) with static
 * storage duration and internal linkage instead of a raw string literal to
 * comply with c++ standard 14.3.2/1 */                                       \
static constexpr auto kConverter =                                            \
    unpack</*&kRTStr,*/ kNVSIs + 1, Converter, &fmtRawStr>();                 \
OutbufArg outbuf(buffer, count);                                              \
kConverter.convert<&kRTStr>(outbuf, ##__VA_ARGS__);                           \
result = outbuf.getWrittenNum();                                              \
outbuf.done(); /* null - terminate the string */                              \
} while (0);


/**
 * CFMT_STR_OUTBUFARG macro used for caching format specifier infos at compile
 * time.
 *
 * \param outBuf
 *      OutbufArg object to write to formatted string
 * \param format
 *      printf-like format string (must be literal)
 * \param ... (additional arguments)
 *      Depending on the format string, the function may expect a sequence of
 *      additional arguments, each containing a value to be used to replace a
 *      format specifier in the format string (or a pointer to a storage
 *      location, for specifier n). There should be at least as many of these
 *      arguments as the number of valid format specifiers.
 */
#define CFMT_STR_OUTBUFARG(outbuf, format, ...) do {                           \
constexpr int kNVSIs = countValidSpecInfos(format);                            \
constexpr int kSS = squeezeSoundSize(format);                                  \
static constexpr auto fmtRawStr = format;                                      \
/**
 * A static variable inside a scope or function is something different than
 * a global static variable. Since there can be as many scoped statics with
 * the same name as you like (provided they are all in different scopes),
 * the compiler might have to change their names internally (incorporating
 * the function's name or the line number or whatever), so that the linker
 * can tell them apart.
 * Adding the static keyword to the file scope version of variables doesn't
 * change their extent, but it does change its visibility with respect to
 * other translation units; the name is not exported to the linker, so it
 * cannot be accessed by name from another translation unit. */               \
/*static constexpr std::array<SpecInfo, kNVSIs + 1> kSIs */                   \
	/*= analyzeFormatString<kNVSIs + 1>(format);*/                            \
static constexpr auto kfmtArr = preprocessInvalidSpecs<kSS>(format);          \
static constexpr auto kRTStr = kSS < sizeof(format)? kfmtArr.data() : format; \
/** 
 * use the address of the pointer to a string literal (&kRTStr) with static
 * storage duration and internal linkage instead of a raw string literal to
 * comply with c++ standard 14.3.2/1 */                                       \
static constexpr auto kConverter =                                            \
    unpack</*&kRTStr,*/ kNVSIs + 1, Converter, &fmtRawStr>();                 \
kConverter.convert<&kRTStr>(outbuf, ##__VA_ARGS__);                           \
/*outbuf.done();*/ /* null - terminate the string */                          \
} while (0);

/**
 * CFMT_STR_TUPLE macro used for caching format specifier infos at compile
 * time with tuple or tuple-like types array and pair as arguments.
 *
 * \param result
 *      The variable to store the number of characters that would have been
 *      written to buffer if count had been sufficiently large, not counting
 *      the terminating null character.
 *      Notice that only when this returned value is non-negative and less
 *      than count, the string has been completely written.
 * \param buffer
 *      buffer to write to formatted string
 * \param count
 *      max number of characters to store in buffer
 * \param format
 *      printf-like format string (must be literal)
 * \param tuple
 *      A tuple or tuple-like types array and pair containing values to be used
 *      to replace format specifiers in the format string (or a pointer to a 
 *      storage location, for specifier n). There should be at least as many of
 *      values in tuple as the number of valid format specifiers.
 */
#define CFMT_STR_TUPLE(result, buffer, count, format, tuple) do {              \
constexpr int kNVSIs = countValidSpecInfos(format);                            \
constexpr int kSS = squeezeSoundSize(format);                                  \
static constexpr auto fmtRawStr = format;                                      \
static constexpr auto kfmtArr = preprocessInvalidSpecs<kSS>(format);           \
static constexpr auto kRTStr = kSS < sizeof(format) ? kfmtArr.data() : format; \
/**
* use the address of the pointer to a string literal (&kRTStr) with static
* storage duration and internal linkage instead of a raw string literal to
* comply with c++ standard 14.3.2/1 */                                         \
static constexpr auto kConverter =                                             \
    unpack<kNVSIs + 1, Converter, &fmtRawStr>();                               \
OutbufArg outbuf(buffer, count);                                               \
auto convert_lambda = [&](auto... args) {                                      \
	return kConverter.convert<&kRTStr>(outbuf, (args)...); };                  \
std::apply(convert_lambda, tuple);                                             \
result = outbuf.getWrittenNum();                                               \
outbuf.done(); /* null - terminate the string */                               \
} while (0);


/**
 * A template-argument for a non-type, non-template template-parameter shall
 * be one of:
 * 1) for a non-type template-parameter of integral or enumeration type, a
 *    converted constant expression(5.19) of the type of the
 *    template-parameter; or
 * 2) the name of a non-type template-parameter; or
 * 3) a constant expression(5.19) that designates the address of an object
 *    with static storage duration and external or internal linkage or a
 *    function with external or internal linkage, including function
 *    templates and function template-ids but excluding non-static class
 *    members, expressed(ignoring parentheses) as & id-expression, except
 *    that the & may be omitted if the name refers to a function or an array
 *    and shall be omitted if the corresponding template-parameter is a
 *    reference; or
 * 4) a constant expression that evaluates to a null pointer value(4.10); or
 * 5) a constant expression that evaluates to a null member pointer value
 *    (4.11); or
 * 6) a pointer to member expressed as described in 5.3.1; or
 * 7) an address constant expression of type std::nullptr_t.
 */


#endif  /* CONSTEXPR_FMT_H__ */