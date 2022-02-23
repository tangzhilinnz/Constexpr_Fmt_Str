#include <string>
#include <iostream>
#include <array>
#include <chrono>
#include <cstring>
#include <cassert>
#include <charconv>
#include <tuple>

#include "Portability.h"

using namespace std;
using namespace chrono;

// A non-type template - parameter shall have one of the following(optionally cv-qualified) types:
// integral or enumeration type,
// pointer to object or pointer to function,
// lvalue reference to object or lvalue reference to function,
// pointer to member,
// std::nullptr_t

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


/** used by CFMT_STR */
struct OutbufArg {
	char* pBuf_;            // running pointer to the next char
	char* pBufEnd_;         // pointer to buffer end 
};


using flags_t = int;
using width_t = int;
using precision_t = int;
using sign_t = char;
using terminal_t = char;
using begin_t = unsigned;
using end_t = unsigned;
using width_first_t = bool;
/**
 * Stores the static format information associated with a specifier in a format
 * string (i.e. %<flags><width>.<precision><length><terminal>).
 */
struct FmtInfos {
	begin_t begin_ = 0;
	end_t end_ = 0;
	flags_t flags_ = 0;
	width_t width_ = 0;
	precision_t prec_ = -1;
	sign_t sign_ = '\0';
	terminal_t terminal_ = '?';
	width_first_t wFirst_ = true; // indicate which one was extracted first in the 
								  // argument list when width and precision are both
								  // dynamically resolved
};
constexpr int LEN = sizeof(FmtInfos);

/**
 * Stores the static format information associated with a CFMT_STR invocation site.
 */
 //struct StaticFmtInfo {
 //	// With constexpr constructors, objects of user-defined types can be
 //	// included in valid constant expressions.
 //	// Definitions of constexpr constructors must satisfy the following 
 //	// requirements:
 //	// - The containing class must not have any virtual base classes.
 //	// - Each of the parameter types is a literal type. 
 //	//   (all reference types are literal types)
 //	// - Its function body is = delete or = default; otherwise, it must satisfy
 //	//   the following constraints:
 //	//   1) It is not a function try block.
 //	//   2) The compound statement in it must contain only the following statements:
 //	//      null statements, static_assert declarations,
 //	//      typedef declarations that do not define classes or enumerations,
 //	//      using directives, using declarations
 //	// - Each nonstatic data member and base class subobject is initialized.
 //	// - Each constructor that is used for initializing nonstatic data members
 //	//   and base class subobjects is a constexpr constructor.
 //	// - Initializers for all nonstatic data members that are not named by a 
 //	//   member initializer identifier are constant expressions.
 //	// - When initializing data members, all implicit conversions that are 
 //	//   involved in the following context must be valid in a constant expression:
 //	//   Calling any constructors, Converting any expressions to data member types
 //	constexpr StaticFmtInfo(const char* fmtString, const int numSpecs,
 //		const int numVarArgs, const FmtInfos* fmtInfos)
 //		: /*filename_(filename)
 //		, lineNum_(lineNum)
 //		, severity_(severity)
 //		,*/ formatString_(fmtString)
 //		, numSpecs_(numSpecs)
 //		, numVarArgs_(numVarArgs)
 //		, fmtInfos_(fmtInfos)
 //	{ }
 //
 //
 //	//// File where the log invocation is invoked
 //	//const char* filename_;
 //
 //	//// Line number in the file for the invocation
 //	//const uint32_t lineNum_;
 //
 //	//// LogLevel severity associated with the log invocation
 //	//const uint8_t severity_;
 //
 //	// printf format string associated with the log invocation
 //	const char* formatString_;
 //
 //	// Number of fmt specifiers fetched from format string
 //	const int numSpecs_;
 //
 //	// Number of variadic arguments required for CFMT_STR invocation
 //	const int numVarArgs_;
 //
 //	// Mapping of detailed infos of fmt specifiers as inferred from CFMT_STR invocation.
 //	const FmtInfos* fmtInfos_;
 //};


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
 *      Returns an FmtInfos object describing the detailed information of num-th format specifier
 */
template<int N>
constexpr inline FmtInfos
getOneSepc(const char(&fmt)[N], int num = 0) {
	begin_t begin = 0;
	end_t end = 0;
	flags_t flags = 0;
	width_t width = 0;
	precision_t prec = -1;
	sign_t sign = '\0';
	terminal_t terminal = '\0';
	width_first_t widthFirst = true;

	int intLen = 0;
	int pos = 0;
	int specStar = 0;
	int sizeOfInvalidSpecs = 0;
	bool exit = false;

	while (num > 0) {
		// consume the current non-format string until reaching the next '%'
		// if num is greater than 0, fmt[pos] can never be '\0'
		for (; /*(fmt[pos] != '\0') && */(fmt[pos] != '%'); pos++);

		specStar = pos; // record the position of '%' as it appears
		pos++;   // fmt[pos] == '%', so pos++ skips over '%'

		while (isFlag(fmt[pos]) || isDigit(fmt[pos]) || isLength(fmt[pos])
			|| fmt[pos] == '.' || fmt[pos] == '*') {
			pos++;
		}

		if (isTerminal(fmt[pos])) {
			num--;
		}
		else { // isTerminal(fmt[pos]) == false
			// pos - specStar is the size of this invlaid specifier
			sizeOfInvalidSpecs += pos - specStar;
		}

		pos++;   // skips over '%', terminal, or non-terminal
	}

	while (!exit) {
		// consume the current non-format string until reaching the next '%'
		for (; (fmt[pos] != '%'); pos++);

		specStar = pos; // record the position of '%' as it appears
		pos++;   // fmt[pos] == '%', so pos++ skips over '%'

		while (isFlag(fmt[pos]) || isDigit(fmt[pos]) || isLength(fmt[pos])
			|| fmt[pos] == '.' || fmt[pos] == '*') {
			pos++;
		}

		if (isTerminal(fmt[pos])) {
			break;
		}
		else { // isTerminal(fmt[pos]) == false
			// pos - specStar is the size of this invlaid specifier
			sizeOfInvalidSpecs += pos - specStar;
		}

		pos++;   // skips over '%' or non-terminal
	}

	begin = specStar;
	pos = specStar + 1;  // fmt[specStar] == '%', so specStar + 1 over '%'

	while (!exit) {

		switch (fmt[pos]) {
		case ' ':
			/*-
			 * ``If the space and + flags both appear, the space
			 * flag will be ignored.''
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
				intLen = __FLAG_CHARINT;
				break;
			}
			else {
				intLen = __FLAG_SHORTINT;
				pos--;
				break;
			}
		case 'l':
			pos++;
			if (fmt[pos] == 'l') {
				intLen = __FLAG_LLONGINT;
				break;
			}
			else {
				intLen = __FLAG_LONGINT;
				pos--;
				break;
			}
		case 'j':
			intLen = __FLAG_INTMAXT;
			break;
		case 't':
			intLen = __FLAG_PTRDIFFT;
			break;
		case 'z':
			intLen = __FLAG_SIZET;
			break;

		case 'c': case 'd': case 'i': case 'a': case 'A': case 'e': case 'E':
		case 'f': case 'F': case 'g': case 'G': case 'n': case 'o': case 'p':
		case 's': case 'u': case 'X': case 'x':
			terminal = fmt[pos];
			end = pos + 1;
			exit = true;
			break;

		default: /* should never happen */
			abort();
			break;
		}

		pos++;
	}

	flags |= intLen;
	begin -= sizeOfInvalidSpecs;
	end -= sizeOfInvalidSpecs;

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
constexpr std::array<FmtInfos, sizeof...(Indices)> // Returns the number of elements in pack Indices
analyzeFormatStringHelper(const char(&fmt)[N], std::index_sequence<Indices...>) {
	return { { getOneSepc(fmt, Indices)... } };
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
constexpr std::array<FmtInfos, NParams>
analyzeFormatString(const char(&fmt)[N]) {
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
		}

		if (isTerminal(fmt[num + offset])) {
			offset = saveOffset;
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
countValidSpecs(const char(&fmt)[N]) {
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
 * return the size of the format string without invalid specififers
 * (e.g., "%-+ #0ltz%", "%%", "%-+ K")
 */
template<int N>
constexpr inline int
sizeWithoutInvalidSpecs(const char(&fmt)[N]) {
	int pos = 0;
	int specStar = 0;
	int size = N;

	for (;;) {
		// consume the current non-format string until reaching the next '%' 
		for (; (fmt[pos] != '\0') && (fmt[pos] != '%'); pos++);

		if (fmt[pos] == '\0')
			return size;

		specStar = pos; // record the position of '%' as it appears
		pos++;   // fmt[pos] == '%', so pos++ skips over '%'

		while (isFlag(fmt[pos]) || isDigit(fmt[pos]) || isLength(fmt[pos])
			|| fmt[pos] == '.' || fmt[pos] == '*') {
			pos++;
		}

		if (!isTerminal(fmt[pos])) {
			// pos - specStar is the size of this invlaid specifier
			size -= pos - specStar;
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
static void
CFMT_PRINTF_FORMAT_ATTR(1, 2)
checkFormat(CFMT_PRINTF_FORMAT const char*, ...) {}


/**
 * This is a support routine for fioFormat(). This routine copies [length]
 * bytes from source to destination, leaving the destination buffer pointer
 * (pArg->pBuf) pointing at byte following block copied.
 *
 * If [length] exceeds the number of bytes available in the buffer, only the
 * number of bytes that fit within the buffer are copied. In this case
 * OK is still returned (although no further copying will be performed) so
 * that fioFormat() can return the number of characters that would have been
 * copied if the supplied buffer was of sufficient size.
 *
 * \param pInBuf
 *      pointer to input buffer
 * \param length
 *      length of input buffer
 * \param pArg
 *      fioBufPut argument structure
 *
 * RETURNS: OK always
 */
inline void
fioBufPut(const char* pInBuf, size_t length, OutbufArg& outbuf) {
	size_t remaining;

	if (length == 0) return;

	// check if sufficient free space remains in the buffer
	remaining = (size_t)(outbuf.pBufEnd_ - outbuf.pBuf_ - 1);
	// the last byte position is reserved for the terminating '\0' 

	// fail if at the end of buffer, recall need a single byte for null
	if (std::make_signed<size_t>::type(remaining) <= 0)
		return;

	else if (length > remaining)
		length = remaining;

	memcpy(outbuf.pBuf_, pInBuf, length);

	outbuf.pBuf_ += length;

	return;
}


template<int nex, size_t M, typename T>
inline void
processSingle(OutbufArg& outbuf, int& ret, const char* fmt,
	const std::array<FmtInfos, M>& FIS, T&& arg) {
	if (nex >= M) return;

	if (nex < M - 1) {
		size_t length = static_cast<size_t>(FIS[nex + 1].begin_ - FIS[nex].end_);
		fioBufPut(fmt + FIS[nex].end_, length, outbuf);
		ret += length;
	}
}


template<int nex = 0, size_t M, typename T, typename... Ts>
inline void
processArgs(OutbufArg& outbuf, int& ret, const char* fmt,
	const std::array<FmtInfos, M>& FIS, T&& head, Ts&&... rest) {
	// Peel off one argument to convert, and then recursively process rest
	processSingle<nex>(outbuf, ret, fmt, FIS, std::forward<T>(head));
	processArgs<nex + 1>(outbuf, ret, fmt, FIS, std::forward<Ts>(rest)...);
}

/**
 * Specialization of processArgts that processes no arguments, i.e. this
 * is the end of the head/rest recursion. See above for full documentation.
 */
template<int nex = 0, size_t M>
inline void
processArgs(OutbufArg& outbuf, int& ret, const char* fmt,
	const std::array<FmtInfos, M>& FIS) {
	// No arguments, do nothing.
}

/**
 * Logs a log message in the NanoLog system given all the static and dynamic
 * information associated with the log message. This function is meant to work
 * in conjunction with the #define-d NANO_LOG() and expects the caller to
 * maintain a permanent mapping of logId to static information once it's
 * assigned by this function.
 *
 * \tparam NS
 *      number of valid specifiers used to format args and worked out
 *      at compile time
 * \tparam N
 *      length of the format string (automatically deduced)
 * \tparam M
 *      length of the paramTypes array (automatically deduced)
 * \tparam Ts
 *      Types of the arguments passed in for the log (automatically deduced)
 *
 * \param outbuf
 *      OUTBUF_ARG object used to store the formatted string to user-specified buffer
 * \param format
 *      Static printf format string associated with the CFMT_STR invocation
 * \param fmtInfos
 *      An array storing static format information associated with a specifier
 *      in the format string to be processed.
 *      *** THIS VARIABLE MUST HAVE A STATIC LIFETIME AS PTRS WILL BE SAVED ***
 * \param args
 *      Argument pack for all the arguments for the log invocation
 */
template<size_t M, typename... Ts>
inline int
fioFormat(OutbufArg& outbuf, const char* fmt, const size_t len,
	const std::array<FmtInfos, M>& FIS, Ts&&... args) {

	if constexpr (M > static_cast<uint32_t>(sizeof...(Ts))) {
		std::cerr << "CFMT: forced abort due to illegal number of variadic "
			"arguments passed to CFMT_STR for converting!!!";
		abort();
	}

	//const char* fmt = (L == 0) ? &format[0] : formatArr.data();
	//const size_t len = (L == 0) ? N : L;

	int ret = 0;
	size_t nex = 0;
	char ch = '\0';
	bool returning = false;

	if (0 == M) {
		fioBufPut(fmt, len - 1, outbuf);
		return len - 1;
	}

	auto processSingle = [&](auto&& input) {
		if (returning) return;

		if (nex < M - 1) {
			size_t length = static_cast<size_t>(FIS[nex + 1].begin_ - FIS[nex].end_);
			fioBufPut(fmt + FIS[nex].end_, length, outbuf);
			ret += length;
			nex++;
		}
		else { // nex == M - 1
			returning = true;
		}
	};


	fioBufPut(fmt, (size_t)FIS[0].begin_, outbuf);
	ret += FIS[0].begin_;

	// With MSVC, the performance of c++17 fold expressions on (variadic template) 
	// pack expansion is considerably better than that of recursive template.
	// It is also neat, compact, and understandable.
	(processSingle(std::forward<Ts>(args)), ...);

	// the method of recursive template
	//processArgs(outbuf, ret, fmt, FIS, std::forward<Ts>(args)...);

	fioBufPut(fmt + FIS[M - 1].end_, (size_t)(len - FIS[M - 1].end_ - 1), outbuf);
	ret += len - FIS[M - 1].end_ - 1;

	return ret;
}


template<int N, size_t L>
constexpr inline std::tuple<const char*, int>
getRTFmtStr(const char(&fmt)[N], const std::array<char, L>& fmtArr) {
	if (0 == L) return { &fmt[0], N };
	else return { fmtArr.data(), L };
}


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
 *      location, for n). There should be at least as many of these arguments
 *      as the number of values specified in the valid format specifiers.
 */

#define CFMT_STR(result, buffer, count, format, ...) do { \
	constexpr int kNVS = countValidSpecs(format); \
	constexpr int kLen = sizeWithoutInvalidSpecs(format); \
	/**
	 * Very Important*** These must be 'static' so that we can save pointers
	 * to these variables and have them persist beyond the invocation.
	 * The static logId is used to forever associate this local scope (tied
	 * to an expansion of #NANO_LOG) with an id and the paramTypes array is
	 * used by the compression function, which is invoked in another thread
	 * at a much later time.
	 * A static variable inside a scope or function is something different than
	 * a global static variable. Since there can be as many scoped statics with
	 * the same name as you like (provided they are all in different scopes),
	 * the compiler might have to change their names internally (incorporating
	 * the function's name or the line number or whatever), so that the linker
	 * can tell them apart.
	 * Adding the static keyword to the file scope version of variables doesn't
	 * change their extent, but it does change its visibility with respect to
	 * other translation units; the name is not exported to the linker, so it
	 * cannot be accessed by name from another translation unit. */ \
	static constexpr std::array<FmtInfos, kNVS> kFmtInfos = analyzeFormatString<kNVS>(format); \
	static constexpr auto kformatArr = preprocessInvalidSpecs<kLen>(format); \
	static constexpr auto kRTStr = getRTFmtStr(format, kformatArr); \
	OutbufArg outbuf; \
	outbuf.pBuf_ = buffer; \
	outbuf.pBufEnd_ = (char*)(buffer + count); \
	/**
	* Triggers the printf checker by passing it into a no-op function.
	* Trick: This call is surrounded by an if false so that the VA_ARGS don't
	* evaluate for cases like '++i'.*/ \
	if (false) { checkFormat(format, ##__VA_ARGS__); } \
	result = fioFormat(outbuf, std::get<const char*>(kRTStr), std::get<int>(kRTStr), kFmtInfos, ##__VA_ARGS__); \
	/* null - terminate the string */ \
	if (count != 0) { *outbuf.pBuf_ = '\0'; } \
} while (0)

int tz_snprintf(char* buffer, size_t  count, const char* fmt, ...);





template<int nex, size_t M, const std::array<FmtInfos, M>& FIS, typename T>
inline void
	convertSingle(char* outbuf, T && arg) {
	if constexpr (nex >= M) return;

	else {

		if constexpr (FIS[nex].terminal_ == 'i' || FIS[nex].terminal_ == 'd') {
			if constexpr ((FIS[nex].flags_ & __FLAG_LONGINT) != 0) {
				std::cout << "long int" << std::endl;
			}
			else if constexpr ((FIS[nex].flags_ & __FLAG_SHORTINT) != 0) {
				std::cout << "short int" << std::endl;
			}
			else if constexpr ((FIS[nex].flags_ & __FLAG_CHARINT) != 0) {
				std::cout << "signed char" << std::endl;
			}
			else if constexpr ((FIS[nex].flags_ & __FLAG_LLONGINT) != 0) {
				std::cout << "long long int" << std::endl;
			}
			else if constexpr ((FIS[nex].flags_ & __FLAG_SIZET) != 0) {
				std::cout << "size_t" << std::endl;
			}
			else if constexpr ((FIS[nex].flags_ & __FLAG_PTRDIFFT) != 0) {
				std::cout << "ptrdiff_t" << std::endl;
			}
			else if constexpr ((FIS[nex].flags_ & __FLAG_INTMAXT) != 0) {
				std::cout << "intmax_t" << std::endl;
			}
			else {
				std::cout << "int" << std::endl;
			}
		}
		else if constexpr (FIS[nex].terminal_ == 'x' || FIS[nex].terminal_ == 'X' ||
			FIS[nex].terminal_ == 'o' || FIS[nex].terminal_ == 'u') {
		}
		else {}
	}
}


/**
* Specialization of processArgts that processes no arguments, i.e. this
* is the end of the head/rest recursion. See above for full documentation.
*/
template<int nex, size_t M, const std::array<FmtInfos, M>& FIS>
inline void
	convertArgs(char* outbuf) {
	// No arguments, do nothing.
	return;
}

template<int nex, size_t M, const std::array<FmtInfos, M>& FIS, typename T, typename... Ts>
inline void
	convertArgs(char* outbuf, T && head, Ts&&... rest) {
	// Peel off one argument to convert, and then recursively process rest
	convertSingle<nex, M, FIS>(outbuf, std::forward<T>(head));
	convertArgs<nex + 1, M, FIS>(outbuf, std::forward<Ts>(rest)...);
}

template<int nex, size_t M, const std::array<FmtInfos, M>& FIS, typename... T>
inline void
	processArgs(char* buf, T&&... args) {
	convertArgs<nex, M, FIS>(buf, std::forward<T>(args)...);
}

template<int...>
struct S {};

template<FmtInfos...>
struct Converter {};

template<auto tuple_like, template<auto...> typename Template>
constexpr decltype(auto) unpack()
{
	constexpr auto size = std::tuple_size_v<decltype(tuple_like)>;
	return[]<std::size_t... Is>(std::index_sequence<Is...>) {
		return Template<std::get<Is>(tuple_like)...>{};
	}(std::make_index_sequence<size>{});
}





// sd%%sf%%%fes%h-+ 01233lzhhjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n
constexpr int kSize = sizeWithoutInvalidSpecs("sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n");
constexpr int kNVS = countValidSpecs("sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n");
//constexpr static std::array<char, SIZE> myStr = preprocessInvalidSpecs<SIZE>("sd%%sf%%%fes%h-+ 01233lzhhjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***f23.\n");
constexpr static std::array<char, kSize> myStr = preprocessInvalidSpecs<kSize>("sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n");
constexpr int kSize1 = sizeWithoutInvalidSpecs("sdsssssssss%-+ *.*... *llzthhlh#-054d=ssssssadfadfasfsasadasdasdsa.\n");
constexpr static auto myStr1 = preprocessInvalidSpecs<kSize1>("sdsssssssss%-+ *.*... *llzthhlh#-054d=ssssssadfadfasfsasadasdasdsa.\n");
constexpr static auto myStr2 = preprocessInvalidSpecs<2>("%ks");
// "sd%sf%%fes%sds%%%%gjt *.***k12.dsd%%s%%d%f%f%dsss%h-+ 01233lzhhjt *.***d23.\n"
constexpr std::array<FmtInfos, kNVS> fmtInfos = analyzeFormatString<kNVS>("sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n");

int main() {
	auto start = system_clock::now();

	char buf[100];

	int result = 0;

	std::cout << "11-ll-11-ll" << std::endl;

	for (int i = 0; i < 10000000; i++) {
		//CFMT_STR(result, buf, 100, "sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n", 45, 's', L"adsds", 1, 1);
		CFMT_STR(result, buf, 100, "=%f=%f=%f=%f=%f=%f=%f=%f=%f=%f=%f=%f\n", 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
		//CFMT_STR(result, buf, 100, "dadadsadadadadasssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssdad");
		//CFMT_STR(result, buf, 100, "sd+ 01236%s66657==k31%s==lllllz%ahhjt *.***d23.\n", 45, 's', L"adsds",1,1);
		//CFMT_STR(result, buf, 100, "sd%%sf%%%fes%h-+ 01233lzhh%sds%%%%%%%%gjt *.***k12.dsd%%%s%%%%d%f%%f%%dsss%h-+ 01233lzhhjt *.***d23.\n", 2, 45, 's', L"adsds");
		//CFMT_STR(result, buf, 100, "34342323%hls-+ 0#s...llks12.0#%**.***+-.**lls*.*20%%ahjhj",2, L"aad");
		//CFMT_STR(result, buf, 100, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
		//CFMT_STR(result, buf, 100, "h-+ 01233lzhhjt *.***hhhlll8.****s");
		//result = snprintf(buf, 100, "342324233hlk-+ 0#...saerereshdkGshjz..-+sf,dsdsffs+- #..*hdgfgf");
		//result = snprintf(buf, 100, "34342323%h0#%.llks12.0#%*.*llk*.*20%%ahjhj");
		//result = snprintf(buf, 100, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
		//result = tz_snprintf(buf, 100, "342324233hlk-+ 0#...%fkerere%hkG%hjz..-+%f,dsdsff%+- #..*hdgfgf", 8.6, 4, 2);
		//result = tz_snprintf(buf, 10, "34342323%hlk-+ 0#%...llks12.0#%**.***+-.**llk*.*20%%ahjhj");
		//result = tz_snprintf(buf, 100, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
	}

	std::cout << buf << std::endl;
	std::cout << "result: " << result << std::endl;

	auto end = system_clock::now();
	auto duration = duration_cast<microseconds>(end - start);

	std::cout << "cost: "
		<< double(duration.count()) * microseconds::period::num / microseconds::period::den << "seconds" << std::endl;



	wint_t c = L'd';
	//Foo(2, 3, 4u, (int64_t)9, 'a', "s", 2.3, L'A', L"tangzhilin", c);
	constexpr int i = sizeof(wchar_t);
	constexpr int j = sizeof(wint_t);

	std::cout << "long to int: " << boolalpha << std::is_convertible_v<long, int> << std::endl;
	std::cout << "int to long: " << boolalpha << std::is_convertible_v<int, long> << std::endl;

	std::cout << "long to char: " << boolalpha << std::is_convertible_v<long, char> << std::endl;
	std::cout << "char to long: " << boolalpha << std::is_convertible_v<char, long> << std::endl;

	std::cout << "long to void*: " << boolalpha << std::is_convertible_v<long, void*> << std::endl;
	std::cout << "void* to long: " << boolalpha << std::is_convertible_v<void*, long> << std::endl;

	std::cout << "wint_t integer? " << is_integral_v<wint_t> << std::endl;
	std::cout << "char* integer? " << is_integral_v<char*> << std::endl;

	std::cout << "char* pointer? " << is_pointer_v<char*> << std::endl;

	constexpr bool compare2 = std::is_integral_v<const char&>;
	constexpr bool compare = std::is_integral_v<std::remove_reference_t<const char&&>>;
	constexpr bool compare1 = std::is_convertible_v<void(*)(int, int), const char*>;

	char str[5] = { 0 };
	wchar_t wc = L'c';
	wint_t wc1 = L'c';

	const wchar_t* wstr = L"sdsdasdasd";

	struct A { int i = 0; };
	A a;
	A* pa = &a;
	A& ra = a;

	string s = "aa";

	void* v = nullptr;

	int in = 100;

	const int& rin = in;

	double d = 10.2;

	std::to_chars_result re = to_chars(buf, buf + 100, static_cast<short>(rin), 10);
	re = to_chars(buf, buf + 100, rin, 10);
	re = to_chars(buf, buf + 100, reinterpret_cast<uintptr_t>(pa), 10);

	uintmax_t ux = (short)-100;
	re = to_chars(buf, buf + 100, ux, 10);
	*re.ptr = '\0';

	std::cout << buf << std::endl;

	float f = 2.32343f;
	double df = 2.32343;
	long double ldf = 2.32343;

	std::cout << static_cast<long double>(f) << std::endl;
	std::cout << static_cast<float>(df) << std::endl;
	std::cout << static_cast<float>(ldf) << std::endl;

	int b = 23;
	int& rb = b;
	std::cout << static_cast<long>(rb) << std::endl;



	do {

		static constexpr auto a = std::array{ 0, 1, 2 };

		static constexpr const std::array<FmtInfos, 4> infos = {
			FmtInfos{6U, 8U, 0, 0, -1, '\000', 'i', true},
			FmtInfos{35U, 37U, 0, 0, -1, '\000', 'd', true},
			FmtInfos{40U, 42U, 0, 0, -1, '\000', 'd', true},
			FmtInfos{49U, 72U, 518, -2147483648, -2147483648, '+', 'd', false}
		};

		constexpr auto s = unpack<a, S>();
		constexpr auto converter = unpack<infos, Converter>();

		processArgs<0, 4, infos>(buf, 12, 12, 12, 12, 12);
	} while (0);

}