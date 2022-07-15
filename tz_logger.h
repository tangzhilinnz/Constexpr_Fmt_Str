#ifndef TZ_LOGGER_H__
#define TZ_LOGGER_H__

#if ! defined(MARKUP_SIZEOFWCHAR)
#if __SIZEOF_WCHAR_T__ == 4 || __WCHAR_MAX__ > 0x10000
#define MARKUP_SIZEOFWCHAR 4
#else
#define MARKUP_SIZEOFWCHAR 2
#endif
#endif

// A workaround of __LINE__ that is not constexpr in MSVC
#if (defined(_MSC_VER))
#define CAT(X, Y)     CAT2(X, Y)
#define CAT2(X, Y)    X##Y
#define __USABLE_LINE__     int(CAT(__LINE__, U)) 
#else
#define __USABLE_LINE__     __LINE__
#endif

#include <cwchar>
#include <cstring>
#include <ctime>


#include "constexpr_fmt.h"
#include "runtime_logger.h"
#include "tz_type.h"


#define MAX_SIZE_OF_FORMAT_ARGS  64 * 1024


// forward declaration of template getNArgsSize
template<int N, typename... Ts>
constexpr inline size_t getNArgsSize();

template <typename T>
constexpr inline size_t getArgSize() {
	return sizeof(T);
}

template<int N, typename T, typename... Ts>
constexpr inline size_t getNArgsSize_helper() {
	return getArgSize<T>() + getNArgsSize<N - 1, Ts...>();
}

template<int N, typename... Ts>
constexpr inline size_t getNArgsSize() {
	if constexpr (N > 0)
		return getNArgsSize_helper<N, Ts...>();
	else
		return 0;
}


/* Count the number of 's' terminator in specifiers template pack */
template<SpecInfo... SIs>
constexpr inline int getStrsNum() {
	int count = 0;
	for (auto& SI : { SIs... }) {
		if ('s' == SI.terminal_) count++;
	}
	return count;
}

template<size_t IDX, SpecInfo SI, typename T>
inline void getSizeForTerminal_s(size_t* arr, T arg) {
	if constexpr ((SI.flags_ & __FLAG_LONGINT) == __FLAG_LONGINT) { // '%ls'
		if constexpr (std::is_convertible_v<T, const wchar_t*>) {
			// wchar_t string is 2 bytes UTF-16 on Windows, 
			// 4 bytes UTF-32 (gcc/g++ and XCode) on Linux and OS,
			// and 2 bytes UTF-16 on Cygwin (cygwin uses Windows APIs)
			const wchar_t* wcp = static_cast<const wchar_t*>(arg);
			if (nullptr == wcp)
				arr[IDX] = 0;
			else
				arr[IDX] = (std::wcslen(wcp) + 1) * MARKUP_SIZEOFWCHAR;
		}
		else // T is not const wchar_t* type
			arr[IDX] = 0;
	}
	else { // '%s'
		if constexpr (std::is_convertible_v<T, const char*>) {
			const char* cp = static_cast<const char*>(arg);
			if (nullptr == cp)
				arr[IDX] = 0;
			else
				arr[IDX] = std::strlen(cp) + 1;
		}
		else // T is not const char* type
			arr[IDX] = 0;
	}
}

// forward declaration of template getStrSizeArray
template<size_t IDX, SpecInfo... SIs, typename... Ts>
inline void getStrSizeArray(size_t* arr, Ts...args);

template<size_t IDX, SpecInfo SI, SpecInfo... SIs, typename T, typename... Ts>
inline void getStrSize(size_t* arr, T arg, Ts... rest) {
	if constexpr (SI.terminal_ == 's') {		 
		getSizeForTerminal_s<IDX, SI>(arr, arg);
		getStrSizeArray<IDX + 1, SIs...>(arr, rest...);
	}
	else
		getStrSizeArray<IDX, SIs...>(arr, rest...);
}

template<size_t IDX, SpecInfo SI, SpecInfo... SIs, typename D, typename T, typename... Ts>
inline void getStrSize_D(size_t* arr, D d, T arg, Ts... rest) {
	if constexpr (SI.terminal_ == 's') {
		getSizeForTerminal_s<IDX, SI>(arr, arg);
		getStrSizeArray<IDX + 1, SIs...>(arr, rest...);
	}
	else
		getStrSizeArray<IDX, SIs...>(arr, rest...);
}

template<size_t IDX, SpecInfo SI, SpecInfo... SIs, typename D1, typename D2, typename T, typename... Ts>
inline void getStrSize_D_D(size_t* arr, D1 d1, D2 d2, T arg, Ts... rest) {
	if constexpr (SI.terminal_ == 's') {
		getSizeForTerminal_s<IDX, SI>(arr, arg);
		getStrSizeArray<IDX + 1, SIs...>(arr, rest...);
	}
	else
		getStrSizeArray<IDX, SIs...>(arr, rest...);
}

template<size_t IDX, SpecInfo... SIs, typename... Ts>
inline void getStrSizeArray(size_t* arr, Ts...args) {
	// At least one argument exists in the template parameter pack
	// SpecInfo... SIs (tailed SI holding no valid terminal).
	constexpr auto& SI = std::get<0>(std::forward_as_tuple(SIs...));

	if constexpr (sizeof ...(SIs) > 1) {
		if constexpr (SI.width_ == DYNAMIC_WIDTH
			&& SI.prec_ == DYNAMIC_PRECISION) {
			getStrSize_D_D<IDX, SIs...>(arr, args...);
		}
		else if constexpr (SI.width_ == DYNAMIC_WIDTH
			|| SI.prec_ == DYNAMIC_PRECISION) {
			getStrSize_D<IDX, SIs...>(arr, args...);
		}
		else {
			getStrSize<IDX, SIs...>(arr, args...);
		}
	}
}


template<size_t IDX, SpecInfo SI, typename T>
inline void storeArg(char** storage, char** storage2, size_t* arr, T arg) {
	if constexpr (SI.terminal_ == 's') {
		if constexpr ((SI.flags_ & __FLAG_LONGINT) == __FLAG_LONGINT) {
			if constexpr (std::is_convertible_v<T, const wchar_t*>) {
				// wchar_t string is 2 bytes UTF-16 on Windows, 
				// 4 bytes UTF-32 (gcc/g++ and XCode) on Linux and OS,
				// and 2 bytes UTF-16 on Cygwin (cygwin uses Windows APIs)
				const wchar_t* wcp = static_cast<const wchar_t*>(arg);
				if (nullptr == wcp) {
					*reinterpret_cast<wchar_t**>(*storage) = nullptr;
					*storage += sizeof(wchar_t*);
				}
				else {
					size_t size = arr[IDX] - MARKUP_SIZEOFWCHAR;
					const char* cp = reinterpret_cast<const char*>(wcp);
					std::memcpy(*storage2, cp, size);
					*reinterpret_cast<wchar_t*>(*storage2 + size) = L'\0';
					*reinterpret_cast<wchar_t**>(*storage) =
						reinterpret_cast<wchar_t*>(*storage2);
					*storage += sizeof(wchar_t*);
					*storage2 += arr[IDX];
				}
			}
			else {
				*reinterpret_cast<T*>(*storage) = arg;
				*storage += sizeof(T);
			}
		}
		else {
			if constexpr (std::is_convertible_v<T, const char*>) {
				const char* cp = static_cast<const char*>(arg);
				if (nullptr == cp) {
					*reinterpret_cast<char**>(*storage) = nullptr;
					*storage += sizeof(char*);
				}
				else {
					size_t size = arr[IDX] - 1;
					std::memcpy(*storage2, cp, size);
					*(*storage2 + size) = '\0';
					*reinterpret_cast<char**>(*storage) =  *storage2;
					*storage += sizeof(char*);
					*storage2 += arr[IDX];
				}
			}
			else {
				*reinterpret_cast<T*>(*storage) = arg;
				*storage += sizeof(T);
			}
		}
	}
	else {
		// std::memcpy(*storage, &arg, sizeof(T));
		*reinterpret_cast<T*>(*storage) = arg;
		*storage += sizeof(T);
	}
}

// forward declaration of template storeArgs
template<size_t IDX, SpecInfo... SIs, typename... Ts>
inline void storeArgs(char** storage, char** storage2, size_t* arr, Ts ...args);

template<size_t IDX, SpecInfo SI, SpecInfo... SIs, typename T, typename... Ts>
inline void store_A_Args(char** storage, char** storage2, size_t* arr,
	                     T arg, Ts ... rest) {
	storeArg<IDX, SI>(storage, storage2, arr, arg);
	if constexpr(SI.terminal_ == 's') 
	    storeArgs<IDX + 1, SIs...>(storage, storage2, arr, (rest)...);
	else
		storeArgs<IDX, SIs...>(storage, storage2, arr, (rest)...);
}

template<size_t IDX, SpecInfo SI, SpecInfo... SIs, typename D, typename T, typename... Ts>
inline void store_D_A_Args(char** storage, char** storage2, size_t* arr,
	                       D d, T arg, Ts ... rest) {
	// std::memcpy(*storage, &d, sizeof(D));
	*reinterpret_cast<D*>(*storage) = d;
	*storage += sizeof(D);

	storeArg<IDX, SI>(storage, storage2, arr, arg);
	if constexpr (SI.terminal_ == 's')
		storeArgs<IDX + 1, SIs...>(storage, storage2, arr, (rest)...);
	else
		storeArgs<IDX, SIs...>(storage, storage2, arr, (rest)...);
}

template<size_t IDX, SpecInfo SI, SpecInfo... SIs, typename D1, typename D2, typename T, typename... Ts>
inline void store_D_D_A_Args(char** storage, char** storage2, size_t* arr,
	                         D1 d1, D2 d2, T arg, Ts... rest) {
	// std::memcpy(*storage, &d1, sizeof(D1));
	*reinterpret_cast<D1*>(*storage) = d1;
	*storage += sizeof(D1);
	// std::memcpy(*storage, &d2, sizeof(D2));
	*reinterpret_cast<D2*>(*storage) = d2;
	*storage += sizeof(D2);

	storeArg<IDX, SI>(storage, storage2, arr, arg);
	if constexpr (SI.terminal_ == 's')
		storeArgs<IDX + 1, SIs...>(storage, storage2, arr, (rest)...);
	else
		storeArgs<IDX, SIs...>(storage, arr, storage2, (rest)...);
}

/**
 * Stores entire printf arguments into a buffer and bumps the buffer pointer.
 *
 * Non-string types are stored (full-width) and string types are stored
 * with its string pointer at storage and the string itself with NULL
 * terminator is stored at storage2
 * ('\0' for char* and L'\0' for wchar_t*).
 *
 * \param[in/out] storage
 *      Buffer to store all arguments into
 * \param[in/out] storage2
 *      Buffer to store all strings corresponding to format specifier 's' into
 * \param arr
 *      The pointer to the array of string size
 * \param ...args
 *      Arguments to store
 */
template<size_t IDX, SpecInfo... SIs, typename... Ts>
inline void storeArgs(char** storage, char** storage2, size_t* arr, Ts ...args) {
	// At least one argument exists in the template parameter pack
	// SpecInfo... SIs (tailed SI holding no valid terminal).
	constexpr auto& SI = std::get<0>(std::forward_as_tuple(SIs...));

	if constexpr (sizeof ...(SIs) > 1) {
		if constexpr (SI.width_ == DYNAMIC_WIDTH
			&& SI.prec_ == DYNAMIC_PRECISION) {
			store_D_D_A_Args<IDX, SIs...>(storage, storage2, arr, (args)...);
		}
		else if constexpr (SI.width_ == DYNAMIC_WIDTH
			|| SI.prec_ == DYNAMIC_PRECISION) {
			store_D_A_Args<IDX, SIs...>(storage, storage2, arr, (args)...);
		}
		else {
			store_A_Args<IDX, SIs...>(storage, storage2, arr, (args)...);
		}
	}
}


// forward declaration of template storeArgs
template<size_t IDX, const char* const* FmtStr, typename TUPLE, SpecInfo... SIs>
inline void formator(OutbufArg& outbuf, /*char***/const char* input);

template<size_t IDX, const char* const* FmtStr, typename TUPLE, SpecInfo SI, SpecInfo... SIs>
inline void formator_A_args(OutbufArg& outbuf, /*char***/const char* input) {
	using T = std::tuple_element_t<IDX, TUPLE>;

	T val = *reinterpret_cast<const T*>(input);
	// T val;
	// std::memcpy(&val, input, sizeof(T));
	input += sizeof(T);

	converter_single<FmtStr, SI, T>(outbuf, val);
	formator<IDX + 1, FmtStr, TUPLE, SIs...>(outbuf, input);
}

template<size_t IDX, const char* const* FmtStr, typename TUPLE, SpecInfo SI, SpecInfo... SIs>
inline void formator_D_A_args(OutbufArg& outbuf, /*char***/const char* input) {
	using D = std::tuple_element_t<IDX, TUPLE>;
	using T = std::tuple_element_t<IDX + 1, TUPLE>;

	if constexpr (SI.width_ == DYNAMIC_WIDTH) {
		int d = 0;
		if constexpr (std::is_integral_v<std::remove_reference_t<D>>)
			d = static_cast<int>(*reinterpret_cast<const D*>(input));
		input += sizeof(D);

		T val = *reinterpret_cast<const T*>(input);
		input += sizeof(T);

		converter_single<FmtStr, SI, T>(outbuf, val, d, -1);
	}
	else if constexpr (SI.prec_ == DYNAMIC_PRECISION) {
		int d = -1;
		if constexpr (std::is_integral_v<std::remove_reference_t<D>>)
			d = static_cast<int>(*reinterpret_cast<const D*>(input));
		input += sizeof(D);

		T val = *reinterpret_cast<const T*>(input);
		input += sizeof(T);

		converter_single<FmtStr, SI, T>(outbuf, val, 0, d);
	}
	else { /* should never happen */
		abort();
	}

	formator<IDX + 2, FmtStr, TUPLE, SIs...>(outbuf, input);
}

template<size_t IDX, const char* const* FmtStr, typename TUPLE, SpecInfo SI, SpecInfo... SIs>
inline void formator_D_D_A_args(OutbufArg& outbuf, /*char***/const char* input) {
	using D1 = std::tuple_element_t<IDX, TUPLE>;
	using D2 = std::tuple_element_t<IDX + 1, TUPLE>;
	using T = std::tuple_element_t<IDX + 2, TUPLE>;

	if constexpr (SI.wFirst_) {
		int d1 = 0, d2 = -1;
		if constexpr (std::is_integral_v<std::remove_reference_t<D1>>)
			d1 = static_cast<int>(*reinterpret_cast<const D1*>(input));
		input += sizeof(D1);
		if constexpr (std::is_integral_v<std::remove_reference_t<D2>>)
			d2 = static_cast<int>(*reinterpret_cast<const D2*>(input));
		input += sizeof(D2);

		T val = *reinterpret_cast<const T*>(input);
		input += sizeof(T);

		converter_single<FmtStr, SI, T>(outbuf, val, d1, d2);
	}
	else {
		int d1 = -1, d2 = 0;
		if constexpr (std::is_integral_v<std::remove_reference_t<D1>>)
			d1 = static_cast<int>(*reinterpret_cast<const D1*>(input));
		input += sizeof(D1);
		if constexpr (std::is_integral_v<std::remove_reference_t<D2>>)
			d2 = static_cast<int>(*reinterpret_cast<const D2*>(input));
		input += sizeof(D2);

		T val = *reinterpret_cast<const T*>(input);
		input += sizeof(T);

		converter_single<FmtStr, SI, T>(outbuf, val, d2, d1);
	}

	formator<IDX + 3, FmtStr, TUPLE, SIs...>(outbuf, input);
}

template<size_t IDX, const char* const* FmtStr, typename TUPLE, SpecInfo... SIs>
inline void formator(OutbufArg& outbuf, /*char***/ const char* input) {
	// At least one argument exists in the template parameter pack
    // SpecInfo... SIs (tailed SI holding no valid terminal).
	constexpr auto& SI = std::get<0>(std::forward_as_tuple(SIs...));

	if constexpr (sizeof ...(SIs) > 1) {
		if constexpr (SI.width_ == DYNAMIC_WIDTH
			&& SI.prec_ == DYNAMIC_PRECISION) {
			formator_D_D_A_args<IDX, FmtStr, TUPLE, SIs...>(outbuf, input);
		}
		else if constexpr (SI.width_ == DYNAMIC_WIDTH
			|| SI.prec_ == DYNAMIC_PRECISION) {
			formator_D_A_args<IDX, FmtStr, TUPLE, SIs...>(outbuf, input);
		}
		else {
			formator_A_args<IDX, FmtStr, TUPLE, SIs...>(outbuf, input);
		}
	}
	else {
		// Fetch tail SpecInfo to terminate the formatting process.
		outbuf.write(*FmtStr + SI.begin_, 
			static_cast<size_t>(SI.end_ - SI.begin_));
	}
}


template<SpecInfo... SIs>
struct LogEntryHandler {
	constexpr LogEntryHandler() { }

	inline constexpr static size_t N = countArgsRequired<SIs...>();

	template <typename... Ts>
	constexpr size_t argsSize(std::tuple<Ts...>&&) const {
		static_assert(static_cast<size_t>(N) <= sizeof...(Ts),
			"The minimum number of arguments required by SpecInfo pack must be"
			" less than or equal to the number of elements in the parameter"
			" pack in TZ_LOG");

		return getNArgsSize<N, Ts...>();
	}

	template <const char* const* FmtStr, typename... Ts>
	constexpr decltype(auto) instFormator(std::tuple<Ts...>&&) const {
		static_assert(static_cast<size_t>(N) <= sizeof...(Ts),
			"The minimum number of arguments required by SpecInfo pack must be"
			" less than or equal to the number of elements in the parameter"
			" pack in TZ_LOG");

		return &(formator<0, FmtStr, std::tuple<Ts...>, SIs...>);
	}

	template <typename... Ts>
	auto strSizeArray(Ts ...args) const {
		static_assert(static_cast<size_t>(N) <= sizeof...(Ts),
			"The minimum number of arguments required by SpecInfo pack must be"
			" less than or equal to the number of elements in the parameter"
			" pack in TZ_LOG");

		constexpr auto NSTR = getStrsNum<SIs...>();
		std::array<size_t, NSTR> arr{};

		if constexpr (NSTR > 0)
			getStrSizeArray<0, SIs...>(arr.data(), args...);

		return std::move(arr);
	}

	template <size_t ARGS_SIZE, size_t M, typename... Ts>
	void dump(char** storage, std::array<size_t, M>& arr, Ts ...args) const {
		static_assert(static_cast<size_t>(N) <= sizeof...(Ts),
			"The minimum number of arguments required by SpecInfo pack must be"
			" less than or equal to the number of elements in the parameter"
			" pack in TZ_LOG");

		char* pStrStorage = *storage + ARGS_SIZE;
		storeArgs<0, SIs...>(storage, &pStrStorage, arr.data(), args...);

		*storage = pStrStorage;
	}

};


/**
 * TZ_LOG macro used for logging.
 *
 * \param severity
 *      The LogLevel of the log invocation (must be constant)
 * \param format
 *      printf-like format string (must be literal)
 * \param ...
 *      Log arguments associated with the printf-like string. */
#define TZ_LOG(severity, format, ...) do {                                     \
/* std::make_tuple: zero or more arguments to construct the tuple from */      \
using this_tupe_t = decltype(std::make_tuple(__VA_ARGS__));                    \
constexpr int kNVSIs = countValidSpecInfos(format);                            \
constexpr int kSS = squeezeSoundSize(format);                                  \
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
 * cannot be accessed by name from another translation unit. */                \
static constexpr auto fmtRawStr = format;                                      \
static constexpr auto kfmtArr = preprocessInvalidSpecs<kSS>(format);           \
static constexpr auto kRTStr = kSS < sizeof(format) ? kfmtArr.data() : format; \
/**
 * use the address of the pointer to a string literal (&kRTStr) with static
 * storage duration and internal linkage instead of a raw string literal to
 * comply with c++ standard 14.3.2/1 */                                        \
static constexpr auto kHandler =                                               \
    unpack<kNVSIs + 1, LogEntryHandler, &fmtRawStr>();                         \
static constexpr auto pFormator =                                              \
    kHandler.instFormator<&kRTStr>(this_tupe_t());                             \
static constexpr StaticFmtInfo fmtInfo(                                        \
	pFormator, __FILE__, __FUNCTION__, __USABLE_LINE__, severity, kRTStr,      \
	kHandler.N);                                                               \
constexpr auto kArgsSize = kHandler.argsSize(this_tupe_t());                   \
auto strSizeArr = kHandler.strSizeArray(__VA_ARGS__);                          \
auto bufSize = kArgsSize + sizeof(OneLogEntry);                                \
if (!strSizeArr.empty()) {                                                     \
    for (auto e : strSizeArr)                                                  \
		bufSize += e;                                                          \
}                                                                              \
uint64_t timestamp = tscns.rdtsc();                                            \
char* writePos = RuntimeLogger::reserveAlloc(bufSize);                         \
auto originalWritePos = writePos;                                              \
OneLogEntry* oe = new(writePos) OneLogEntry();                                 \
writePos += sizeof(OneLogEntry);                                               \
(void)kHandler.dump<kArgsSize>(&writePos, strSizeArr, ##__VA_ARGS__);          \
oe->fmtId = &fmtInfo;                                                          \
oe->entrySize = static_cast<uint32_t>(bufSize);                                \
oe->timestamp = timestamp;                                                     \
assert(bufSize == static_cast<uint32_t>(writePos - originalWritePos));         \
RuntimeLogger::finishAlloc(bufSize);                                           \
} while (0)


#endif /* TZ_LOGGER_H__ */

// Usual arithmetic conversions
// Many operators that expect operands of arithmetic type cause conversions
// and yield result types in a similar way. The purpose is to determine a
// common real type for the operands and result.
// The arguments of the following arithmetic operators undergo implicit 
// conversions for the purpose of obtaining the common real type, which is the
// type in which the calculation is performed:
//     binary arithmetic *, /, %, +, -
//     relational operators <, >, <=, >=, ==, !=
//     binary bitwise arithmetic &, ^, |,
//     the conditional operator ?:
// 
// 1) If one operand is long double, long double complex, or long double imaginary, 
//    the other operand is implicitly converted as follows:
//        integer or real floating type to long double
//        complex type to long double complex
//        imaginary type to long double imaginary
// 2) Otherwise, if one operand is double, double complex, or double imaginary, 
//    the other operand is implicitly converted as follows:
//        integer or real floating type to double
//        complex type to double complex
//        imaginary type to double imaginary
// 3) Otherwise, if one operand is float, float complex, or float imaginary, 
//    the other operand is implicitly converted as follows:
//        integer type to float (the only real type possible is float, which remains as-is)
//        complex type remains float complex
//        imaginary type remains float imaginary
// 4) Otherwise, both operands are integers. Both operands undergo integer promotions
//    (see below); then, after integer promotion, one of the following cases applies:
//        If the types are the same, that type is the common type.
//        Else, the types are different:
//            If the types have the same signedness (both signed or both unsigned), 
//            the operand whose type has the lesser conversion rank is implicitly 
//            converted to the other type.
//            Else, the operands have different signedness:
//                If the unsigned type has conversion rank greater than or equal to the
//                rank of the signed type, then the operand with the signed type is 
//                implicitly converted to the unsigned type.
//                Else, the unsigned type has conversion rank less than the signed type.
//                    If the signed type can represent all values of the unsigned type, 
//                    then the operand with the unsigned type is implicitly converted to 
//                    the signed type.
//                    Else, both operands undergo implicit conversion to the unsigned type
//                    counterpart of the signed operand's type.

// The result type is determined as follows:
//     if both operands are complex, the result type is complex
//     if both operands are imaginary, the result type is imaginary
//     if both operands are real, the result type is real
//     if the two floating - point operands have different type domains(complex vs real, 
//     complex vs imaginary, or imaginary vs real), the result type is complex
// As always, the result of a floating-point operator may have greater range and precision
// than is indicated by its type

// Note: real and imaginary operands are not implicitly converted to complex because doing 
// so would require extra computation, while producing undesirable results in certain cases
// involving infinities, NaNs and signed zeros.

// Integer promotions
// Integer promotion is the implicit conversion of a value of any integer type with rank 
// less or equal to rank of int or of a bit field of type _Bool, int, signed int, 
// unsigned int, to the value of type int or unsigned int
// If int can represent the entire range of values of the original type (or the range of
// values of the original bit field), the value is converted to type int. Otherwise the 
// value is converted to unsigned int.
// Integer promotions preserve the value, including the sign:

// rank above is a property of every integer type and is defined as follows:
//     1) the ranks of all signed integer types are different and increase with
//        their precision: rank of signed char < rank of short < rank of int < 
//        rank of long int < rank of long long int
//     2) the ranks of all signed integer types equal the ranks of the corresponding
//        unsigned integer types
//     3) the rank of any standard integer type is greater than the rank of any 
//        extended integer type of the same size(that is, 
//        rank of __int64 < rank of long long int, but 
//        rank of long long < rank of __int128 due to the rule(1))
//     4) rank of char equals rank of signed char and rank of unsigned char
//     5) the rank of _Bool is less than the rank of any other standard integer type
//     6) the rank of any enumerated type equals the rank of its compatible integer type
//     7) ranking is transitive : if rank of T1 < rank of T2and rank of T2 < rank of T3 
//        then rank of T1 < rank of T3
//     8) any aspects of relative ranking of extended integer types not covered above are
//        implementation defined

// Note: integer promotions are applied only
//     as part of usual arithmetic conversions (see above)
//     as part of default argument promotions (see above)
//     to the operand of the unary arithmetic operators + and -
//     to the operand of the unary bitwise operator ~
//     to both operands of the shift operators << and >>

// Notes:
// Although signed integer overflow in any arithmetic operator is undefined
// behavior, overflowing a signed integer type in an integer conversion is 
// merely unspecified behavior.
// On the other hand, although unsigned integer overflow in any arithmetic 
// operator (and in integer conversion) is a well-defined operation and 
// follows the rules of modulo arithmetic, overflowing an unsigned integer
// in a floating-to-integer conversion is undefined behavior: the values 
// of real floating type that can be converted to unsigned integer are the 
// values from the open interval (-1; Unnn_MAX+1).