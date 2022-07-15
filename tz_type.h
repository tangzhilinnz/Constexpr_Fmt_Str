#ifndef TZ_TYPE_H__
#define TZ_TYPE_H__

#include <stdint.h>
#include "constexpr_fmt.h"

/**
 * The levels of verbosity for messages logged with #TZ_LOG.
 */
enum class LogLevel {
	// Keep this in sync with logLevelNames
	SILENT_LOG_LEVEL = 0,

	// Bad stuff that shouldn't happen. The system broke its contract to
	// users in some way or some major assumption was violated.
	ERR,

	// Messages at the WARNING level indicate that, although something went
	// wrong or something unexpected happened, it was transient and
	// recoverable.
	WARNING,

	// Somewhere in between WARNING and DEBUG...
	INFORMATION,

	// Messages at the DEBUG level don't necessarily indicate that anything
	// went wrong, but they could be useful in diagnosing problems.
	/*DEBUG*/DEB,

	// must be the last element in the enum
	NUM_LOG_LEVELS
};


/**
 * Stores the static format information associated with a log invocation site.
 */
struct StaticFmtInfo {
	// Function signature of the convert function used in the log sink thread
	using ConvertFn = void (*)(OutbufArg&, const char*/*char***/);

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
		const char* funcname,
		const uint32_t lineNum,
		const LogLevel severity,
		const char* fmtString,
		const int numVarArgs)
		: convertFN_(convertFN)
		, filename_(filename)
		, funcname_(funcname)
		, lineNum_(lineNum)
		, severity_(severity)
		, formatString_(fmtString)
		, numVarArgs_(numVarArgs)
	{ }

	// Stores the convert function to be used on the log's runtime arguments
	ConvertFn convertFN_;

	// File where the log invocation is invoked
	const char* filename_;

	// Function in the file where for the log invocation
	const char* funcname_;

	// Line number in the file for the invocation
	const uint32_t lineNum_;

	// LogLevel severity associated with the log invocation
	const LogLevel severity_;

	// format string associated with the log invocation
	const char* formatString_;

	// Number of variadic arguments required for format string
	const int numVarArgs_;
};


struct OneLogEntry {
	// Uniquely identifies a log message by its static format information
	const StaticFmtInfo* fmtId;

	// Number of bytes for this header and the various runtime log arguments
	// after it
	uint32_t entrySize;

	// Stores the rdtsc() value at the time of the log function invocation
	uint64_t timestamp;

	// After this header are the runtime arguments required by the original 
	// format string
	char argData[0];
};


struct ThreadCheckPoint {
	// thread name (provided by operating system or users)
	char name_[20];

	// Number of bytes for a chunk of non-formatted logs that belonged to the same
	// thread after this check point header
	int blockSize_;
};


#endif // TZ_TYPE_H__