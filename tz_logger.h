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


template <typename T>
inline constexpr size_t getArgSize() {
	return sizeof(T);
}

template<int bound, typename T, typename... Ts>
inline constexpr size_t getArgSizes() {
	if constexpr (bound > 0)
		return getArgSize<T>() + getArgSizes<bound - 1, Ts...>();
	else
		return 0;
}

template<int bound>
inline constexpr size_t getArgSizes() {
	return 0;
}


template<int bound, typename... Ts>
inline constexpr size_t requiredSizes(Ts ...args) {
	return getArgSizes<bound, Ts...>();
}

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


template<SpecInfo SI, typename T>
inline void store_arg(char** storage, T arg) {
	if constexpr (SI.terminal_ != 's') {
		//std::memcpy(*storage, &arg, sizeof(T));
		*reinterpret_cast<T*>(*storage) = arg;
		*storage += sizeof(T);
	}
	else {
		uint32_t* pSize = reinterpret_cast<uint32_t*>(*storage);
		*storage += sizeof(uint32_t);

		if constexpr ((SI.flags_ & __FLAG_LONGINT) == __FLAG_LONGINT) {
			if constexpr (std::is_convertible_v<T, const wchar_t*>) {
				// wchar_t string is 2 bytes UTF-16 on Windows, 
				// 4 bytes UTF-32 (gcc/g++ and XCode) on Linux and OS,
				// and 2 bytes UTF-16 on Cygwin (cygwin uses Windows APIs)
				const wchar_t* wcp = static_cast<const wchar_t*>(arg);

				if (nullptr == wcp)
					cp = "(null)";
				else {

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

				if (nullptr == cp)
					cp = "(null)";
			}
			else {
				*reinterpret_cast<T*>(*storage) = arg;
				*storage += sizeof(T);
			}
		}

		//// get string size(<=P) excluding the tail '\0'
		//if (P >= 0) {
		//	// can't use strlen; can only look for the
		//	// NULL in the first `prec' characters, and
		//	// strlen() will go further.
		//	// The C library function
		//	// void* memchr(const void* str, int c, size_t n) 
		//	// searches for the first occurrence of the character 
		//	// c(an unsigned char) in the first n bytes of the string 
		//	// pointed to by the argument str.
		//	// This function returns a pointer to the matching byte or NULL 
		//	// if the character does not occur in the given memory area.
		//	char* it = (char*)memchr(cp, 0, static_cast<size_t>(P));

		//	if (it != NULL) {
		//		size = static_cast<size_t>(it - cp);
		//		//if (size > static_cast<size_t>(P))
		//		//	size = static_cast<size_t>(P);
		//	}
		//	else
		//		size = static_cast<size_t>(P);
		//}
		//else
		//	size = strlen(cp);
	}
}

template<SpecInfo SI, SpecInfo... SIs, typename T, typename... Ts>
inline void store_args(char** storage, T arg, Ts ... rest) {
	store_arg<SI>(storage, arg);
	store_arguments_impl<SIs...>(storage, (rest)...);
}

template<SpecInfo SI, SpecInfo... SIs, typename D, typename T, typename... Ts>
inline void converter_D_args(char** storage, D d, T arg, Ts ... rest) {
	// std::memcpy(*storage, &d, sizeof(D));
	*reinterpret_cast<D*>(*storage) = d;
	*storage += sizeof(D);

	store_arg<SI>(storage, arg);
	store_arguments_impl<SIs...>(storage, (rest)...);
}

template<SpecInfo SI, SpecInfo... SIs, typename D1, typename D2, typename T, 
	     typename... Ts>
inline void store_D_D_args(char** storage, D1 d1, D2 d2, T arg, Ts... rest) {
	// std::memcpy(*storage, &d1, sizeof(D1));
	*reinterpret_cast<D1*>(*storage) = d1;
	*storage += sizeof(D1);
	// std::memcpy(*storage, &d2, sizeof(D2));
	*reinterpret_cast<D2*>(*storage) = d2;
	*storage += sizeof(D2);

	store_arg<SI>(storage, arg);
	store_arguments_impl<SIs...>(storage, (rest)...);
}

template<SpecInfo... SIs, typename... Ts>
inline void store_arguments_impl(char** storage, Ts ...args) {
	// At least one argument exists in the template parameter pack
	// SpecInfo... SIs (tailed SI holding no valid terminal).
	constexpr auto& SI = std::get<0>(std::forward_as_tuple(SIs...));

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





struct StaticFmtInfo {
	using ConvertFn = void (*)(char**);

	constexpr StaticFmtInfo(ConvertFn convertFN,
		const char* filename,
		const uint32_t lineNum,
		const char* fmtString,
		const int numVarArgs)
		: convertFN_(convertFN)
		, filename_(filename)
		, lineNum_(lineNum)
		, formatString_(fmtString)
		, numVarArgs_(numVarArgs)
	{ }

	// Stores the convert function to be used on the log's runtime arguments
	ConvertFn convertFN_;

	// File where the log invocation is invoked
	const char* filename_;

	// Line number in the file for the invocation
	const uint32_t lineNum_;

	// format string associated with the log invocation
	const char* formatString_;

	// Number of variadic arguments required for format string
	const int numVarArgs_;
};

//=============================================================================
template<int bound, typename... Ts>
constexpr auto sum_size_impl();

template <typename T>
constexpr size_t getArgSize() {
	return sizeof(T);
}

template<int bound, typename T, typename... Ts>
constexpr size_t getArgSizes() {
	return getArgSize<T>() + sum_size_impl<bound - 1, Ts...>();
}

template<int bound, typename... Ts>
constexpr auto sum_size_impl() {
	if constexpr (bound > 0)
		return getArgSizes<bound, Ts...>();
	else
		return 0;
}

template <int bound, typename ... Ts>
constexpr std::size_t sum_size(std::tuple<Ts...> const&) {
	static_assert(static_cast<size_t>(bound) <= sizeof...(Ts),
		"bound must be less than or equal to the number of elements in the"
		" parameter pack");

	return sum_size_impl<bound, Ts...>();
}

//=============================================================================
template <typename T>
void convertSingle(char** input) {

	std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	std::cout << "This arg Type: " << typeid(T).name() << std::endl;
	std::cout << "This arg size: " << sizeof(T) << std::endl;
	std::cout << "This arg value: " << *reinterpret_cast<T*>(*input) << std::endl;

	std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	*input += sizeof(T);
}

template<typename T = void, typename... Ts>
void convertImpl(char** input) {
	convertSingle<T>(input);
	convertImpl<Ts...>(input);
}

template<>
void convertImpl<>(char** input) {}


template <typename... Ts>
constexpr decltype(auto) convertGen(std::tuple<Ts...> const&) {
	return &convertImpl<Ts...>;
}
//=============================================================================

#define TEST do { \
    using tuple_like_type = decltype(std::make_tuple(4, 6, 8.8, p)); \
    constexpr size_t bytes = sum_size<4>(tuple_like_type()); \
    static constexpr auto pFunc = convertGen(tuple_like_type()); \
    static constexpr auto fmtInfo = StaticFmtInfo(pFunc, __FILE__, __LINE__, "test", 4); \
    std::cout << fmtInfo.filename_ << std::endl; \
    std::cout << fmtInfo.lineNum_ << std::endl; \
    std::cout << fmtInfo.numVarArgs_ << std::endl; \
    std::cout << fmtInfo.formatString_ << std::endl; \
    pFunc(&begin); \
} while (0)

int main() {

	const char* p = "asd";
	const int& cri = 3;
	int i = 3;
	int& ri = i;

	char input[100];
	char* begin = input;
	char* p1 = input;
	*reinterpret_cast<int*>(p1) = 4;
	p1 += sizeof(int);
	*reinterpret_cast<int*>(p1) = 6;
	p1 += sizeof(int);
	*reinterpret_cast<double*>(p1) = 8.8;
	p1 += sizeof(double);
	*reinterpret_cast<decltype(p)*>(p1) = p;
	p1 += sizeof(decltype(p));

	const StaticFmtInfo* pFmtInfo = nullptr;

	{
		using tuple_like_type = decltype(std::make_tuple(4, 6, 8.8, p));
		constexpr size_t bytes = sum_size<4>(tuple_like_type());
		static constexpr auto pFunc = convertGen(tuple_like_type());
		static constexpr auto fmtInfo = StaticFmtInfo(pFunc, __FILE__, __LINE__, "test", 4);

		pFmtInfo = &fmtInfo;

		pFunc(&begin);
	}
	std::cout << pFmtInfo->filename_ << std::endl;
	std::cout << pFmtInfo->lineNum_ << std::endl;
	std::cout << pFmtInfo->numVarArgs_ << std::endl;
	std::cout << pFmtInfo->formatString_ << std::endl;

	{
		using tuple_like_type = decltype(std::make_tuple(ri, p, p, p));
		constexpr size_t bytes = sum_size<4>(tuple_like_type());
	}

}





/**
 * TZ_LOG macro used for logging.
 *
 * \param severity
 *      The LogLevel of the log invocation (must be constant)
 * \param format
 *      printf-like format string (must be literal)
 * \param ...UNASSIGNED_LOGID
 *      Log arguments associated with the printf-like string.
 */
 // nParams can be zero, the specialization of the std::array with N equal to
 // zero defines an empty container
#define TZ_LOG(severity, format, ...) do { \
constexpr int kNVSIs = countValidSpecInfos(format); \
constexpr int kSS = squeezeSoundSize(format); \
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
 * cannot be accessed by name from another translation unit. */ \
static constexpr auto fmtRawStr = format; \
static constexpr auto kfmtArr = preprocessInvalidSpecs<kSS>(format); \
static constexpr auto kRTStr = kSS < sizeof(format) ? kfmtArr.data() : format; \
} while (0)


#endif /* TZ_LOGGER_H__ */