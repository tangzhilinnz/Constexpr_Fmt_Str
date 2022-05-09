#ifndef TZ_LOGGER_H__
#define TZ_LOGGER_H__

#if ! defined(MARKUP_SIZEOFWCHAR)
#if __SIZEOF_WCHAR_T__ == 4 || __WCHAR_MAX__ > 0x10000
#define MARKUP_SIZEOFWCHAR 4
#else
#define MARKUP_SIZEOFWCHAR 2
#endif
#endif

#include "constexpr_fmt.h"


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
	using ConvertFn =  void (*)(OutbufArg&, char*);

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


/**
 * Stores a single printf argument into a buffer and bumps the buffer pointer.
 *
 * Non-string types are stored (full-width) and string types are stored
 * with a uint32_t header describing the string length in bytes followed
 * by the string itself with NULL terminator
 * ('\0' for char* and L'\0' for wchar_t*).
 *
 * Note: This is the non-string specialization of the function
 * (hence the std::enable_if below), so it contains extra
 * parameters that are unused.
 *
 * \tparam T
 *      Type to store (automatically deduced)
 *
 * \param[in/out] storage
 *      Buffer to store the argument into
 * \param arg
 *      Argument to store
 * \param paramType
 *      Type information deduced from the format string about this
 *      argument (unused here)
 * \param stringSize
 *      Stores the byte length of the argument, if it is a string (unused here)
 */
template<typename T>
inline
typename std::enable_if<!std::is_same<T, const wchar_t*>::value
	                    && !std::is_same<T, const char*>::value
	                    && !std::is_same<T, wchar_t*>::value
	                    && !std::is_same<T, char*>::value
	                    , void>::type
store_argument(char** storage, T arg, ParamType paramType, size_t stringSize) {
	std::memcpy(*storage, &arg, sizeof(T));
	*storage += sizeof(T);
}

// string specialization of the above
template<typename T>
inline
typename std::enable_if<std::is_same<T, const wchar_t*>::value
	|| std::is_same<T, const char*>::value
	|| std::is_same<T, wchar_t*>::value
	|| std::is_same<T, char*>::value
	, void>::type
	store_argument(char** storage,
		T arg,
		const ParamType paramType,
		const size_t stringSize)
{
	// If the printf style format string's specifier says the arg is not
	// a string, we save it as a pointer instead
	if (paramType <= ParamType::NON_STRING) {
		store_argument<const void*>(storage, static_cast<const void*>(arg),
			paramType, stringSize);
		return;
	}

	// Since we've already paid the cost to find the string length earlier,
	// might as well save it in the stream so that the compression function
	// can later avoid another strlen/wsclen invocation.
	if (stringSize > std::numeric_limits<uint32_t>::max())
	{
		throw std::invalid_argument("Strings larger than std::numeric_limits<uint32_t>::max() are unsupported");
	}
	auto size = static_cast<uint32_t>(stringSize);
	std::memcpy(*storage, &size, sizeof(uint32_t));
	*storage += sizeof(uint32_t);

#ifdef ENABLE_DEBUG_PRINTING
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"
#pragma GCC diagnostic ignored "-Wformat"
	if (sizeof(typename std::remove_pointer<T>::type) == 1) {
		printf("\tRString[%p-%u]= %s\r\n", *buffer, size, arg);
	}
	else {
		printf("\tRWString[%p-%u]= %ls\r\n", *buffer, size, arg);
	}
#pragma GCC diagnostic pop
#endif

	memcpy(*storage, arg, stringSize);
	*storage += stringSize;
	return;
}

/**
 * Given a variable number of arguments to a NANO_LOG (i.e. printf-like)
 * statement, recursively unpack the arguments, store them to a buffer, and
 * bump the buffer pointer.
 *
 * \tparam argNum
 *      Internal counter indicating which parameter we're storing
 *      (aka the recursion depth).
 * \tparam N
 *      Size of the isArgString array (automatically deduced)
 * \tparam M
 *      Size of the stringSizes array (automatically deduced)
 * \tparam T1
 *      Type of the Head of the remaining variable number of arguments (deduced)
 * \tparam Ts
 *      Type of the Rest of the remaining variable number of arguments (deduced)
 *
 * \param paramTypes
 *      Type information deduced from the printf format string about the
 *      n-th argument to be processed.
 * \param[in/out] stringBytes
 *      Stores the byte length of the n-th argument, if it is a string
 *      (if not, it is undefined).
 * \param[in/out] storage
 *      Buffer to store the arguments to
 * \param head
 *      Head of the remaining number of variable arguments
 * \param rest
 *      Rest of the remaining variable number of arguments
 */
template<int argNum = 0, unsigned long N, int M, typename T1, typename... Ts>
inline void
store_arguments(const std::array<ParamType, N>& paramTypes,
	size_t(&stringBytes)[M],
	char** storage,
	T1 head,
	Ts... rest)
{
	// Peel off one argument to store, and then recursively process rest
	store_argument(storage, head, paramTypes[argNum], stringBytes[argNum]);
	store_arguments<argNum + 1>(paramTypes, stringBytes, storage, rest...);
}

/**
 * Specialization of store_arguments that processes no arguments, i.e. this
 * is the end of the head/rest recursion. See above for full documentation.
 */
template<int argNum = 0, unsigned long N, int M>
inline void
store_arguments(const std::array<ParamType, N>&,
	size_t(&stringSizes)[M], char**)
{
	// No arguments, do nothing.
}


template<SpecInfo... SIs, typename... Ts>
inline void store_arguments_impl(char** storage, Ts ...args) {
	// At least one argument exists in the template parameter pack
	// SpecInfo... SIs (tailed SI holding no valid terminal).
	if constexpr (sizeof ...(SIs) > 1) {
		if constexpr (SI.width_ == DYNAMIC_WIDTH
			&& SI.prec_ == DYNAMIC_PRECISION) {
			store_D_D_args<SIs...>(storage, (args)...);
		}
		else if constexpr (SI.width_ == DYNAMIC_WIDTH
			|| SI.prec_ == DYNAMIC_PRECISION) {
			store_D_args<SIs...>(storage, (args)...);
		}
		else {
			store_args<SIs...>(storage, (args)...);
		}
	}
}


//InstantiateFN

template<SpecInfo... SIs>
struct LogGenerator {
	constexpr LogGenerator() { }

	template <typename... Ts>
	size_t requiredBytes(Ts ...args) const {
	}

	template <typename... Ts>
	void dump(char** storage, Ts ...args) const {
	}

	template <const char* const* pRTStr, typename... Ts>
	void convert(OutbufArg& outbuf, char** input) const {
	}

};


#endif /* TZ_LOGGER_H__ */