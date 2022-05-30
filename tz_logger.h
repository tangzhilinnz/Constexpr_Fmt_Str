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

#include "constexpr_fmt.h"


#define MAX_SIZE_OF_FORMAT_ARGS  64 * 1024

/**
 * The levels of verbosity for messages logged with #TZ_LOG.
 */
enum class LogLevel {
	// Keep this in sync with logLevelNames
	SILENT_LOG_LEVEL = 0,

	// Bad stuff that shouldn't happen. The system broke its contract to
	// users in some way or some major assumption was violated.
	ERROR,

	// Messages at the WARNING level indicate that, although something went
	// wrong or something unexpected happened, it was transient and
	// recoverable.
	WARNING,

	// Somewhere in between WARNING and DEBUG...
	INFORMATION,

	// Messages at the DEBUG level don't necessarily indicate that anything
	// went wrong, but they could be useful in diagnosing problems.
	DEBUG,

	// must be the last element in the enum
	NUM_LOG_LEVELS
};


/**
 * Stores the static format information associated with a log invocation site.
 */
struct StaticFmtInfo {
	// Function signature of the convert function used in the log sink thread
	using ConvertFn =  void (*)(OutbufArg&, const char*/*char***/);

	// With constexpr constructors, objects of user-defined types can be
	// included in valid constant expressions.
	// Definitions of constexpr constructors must satisfy the following 
	// requirements:
	// - The containing class must not have any virtual base classes.
	// - Each of the parameter types is a literal type. 
	//   (all reference types are literal types)
	// - Its function body is = delete or = default; otherwise, it must satisfy
	//   the following constraints:
	//   1) It is not a function try block.
	//   2) The compound statement in it must contain only the following statements:
	//      null statements, static_assert declarations,
	//      typedef declarations that do not define classes or enumerations,
	//      using directives, using declarations
	// - Each nonstatic data member and base class subobject is initialized.
	// - Each constructor that is used for initializing nonstatic data members
	//   and base class subobjects is a constexpr constructor.
	// - Initializers for all nonstatic data members that are not named by a 
	//   member initializer identifier are constant expressions.
	// - When initializing data members, all implicit conversions that are 
	//   involved in the following context must be valid in a constant expression:
	//   Calling any constructors, Converting any expressions to data member types
	constexpr StaticFmtInfo(ConvertFn convertFN,
		                    const char* filename,
		                    const uint32_t lineNum,
		                    const uint8_t severity,
		                    const char* fmtString, 
		                    const int numVarArgs)
		: convertFN_(convertFN)
		, filename_(filename)
		, lineNum_(lineNum)
		, severity_(severity)
		, formatString_(fmtString)
		, numVarArgs_(numVarArgs)
	{ }

	// Stores the convert function to be used on the log's runtime arguments
	ConvertFn convertFN_;

	// File where the log invocation is invoked
	const char* filename_;

	// Line number in the file for the invocation
	const uint32_t lineNum_;

	// LogLevel severity associated with the log invocation
	const uint8_t severity_;

	// format string associated with the log invocation
	const char* formatString_;

	// Number of variadic arguments required for format string
	const int numVarArgs_;
};


struct OneLogEntry {
    // Uniquely identifies a log message by its static format information
	StaticFmtInfo* fmtId;

    // Number of bytes for this header and the various runtime log arguments
	// after it
    uint32_t entrySize;

    // Stores the rdtsc() value at the time of the log function invocation
    uint64_t timestamp;

    // After this header are the runtime arguments required by the original 
	// format string
    char argData[0];
};


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

	template <typename... Ts>
	constexpr size_t argsSize(std::tuple<Ts...>&&) const {
		constexpr auto N = countArgsRequired<SIs...>();
		static_assert(static_cast<size_t>(N) <= sizeof...(Ts),
			"The minimum number of arguments required by SpecInfo pack must be"
			" less than or equal to the number of elements in the parameter"
			" pack in TZ_LOG");

		return getNArgsSize<N, Ts...>();
	}

	template <typename... Ts>
	auto strSizeArray(Ts ...args) const {
		constexpr auto N = countArgsRequired<SIs...>();
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
		constexpr auto N = countArgsRequired<SIs...>();
		static_assert(static_cast<size_t>(N) <= sizeof...(Ts),
			"The minimum number of arguments required by SpecInfo pack must be"
			" less than or equal to the number of elements in the parameter"
			" pack in TZ_LOG");

		char* pStrStorage = *storage + ARGS_SIZE;
		storeArgs<0, SIs...>(storage, &pStrStorage, arr.data(), args...);
	}

	template <const char* const* FmtStr, typename... Ts>
	constexpr decltype(auto) formatFuncGen(std::tuple<Ts...>&&) const {
		constexpr auto N = countArgsRequired<SIs...>();
		static_assert(static_cast<size_t>(N) <= sizeof...(Ts),
			"The minimum number of arguments required by SpecInfo pack must be"
			" less than or equal to the number of elements in the parameter"
			" pack in TZ_LOG");

		return &(formator<0, FmtStr, std::tuple<Ts...>, SIs...>);
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
using this_tupe_t = decltype(std::make_tuple(##__VA_ARGS__));                  \
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
constexpr auto kArgsSize = kHandler.argsSize(this_tupe_t());                   \
auto strSizeArr = kHandler.strSizeArray(##__VA_ARGS__);                        \
auto bufSize = kArgsSize;                                                      \
if (!strSizeArr.empty()) {                                                     \
    for (auto e : strSizeArr)                                                  \
		bufSize += e;                                                          \
}                                                                              \
} while (0)


#endif /* TZ_LOGGER_H__ */