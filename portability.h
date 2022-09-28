#ifndef PORTABILITY_H__
#define PORTABILITY_H__

#include <cstdint> // for uint32_t, uint16_t
#include <cstdio>  // for FILE
#include <ctime>   // for size_t, time_t
#include <string>  // for string
#include <utility> // for pair
#include <vector>

#ifdef _MSC_VER
#define CFMT_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__)
#define CFMT_ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
#define CFMT_ALWAYS_INLINE inline
#endif

#ifdef _MSC_VER
#define CFMT_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define CFMT_NOINLINE __attribute__((__noinline__))
#else
#define CFMT_NOINLINE
#endif


// difference between #pragma pack (push,1) and #pragma pack (1)
// This is the parameter setting for the compiler, for the structure byte alignment
// setting, #pragma pack is the alignment of the specified data in memory.
// 
// #pragma pack (n) Action: The C compiler will be aligned by N bytes.
// #pragma pack () Effect: cancels custom byte alignment.
//
// #pragma pack (push, 1) function: means to set the original alignment of the 
// stack, and set a new alignment to one byte alignment
// #pragma pack (POP) Action: Restore Alignment State
// 
// As a result, adding push and pop allows alignment to revert to its original 
// state, rather than the compiler default, which can be said to be better, but
// many times the difference is small
// Such as:
// #pragma pack (push): save alignment status
// #pragma pack (4): set to 4-byte alignment, equivalent to #pragma pack (push, 4)
// #pragma pack (1): adjust the boundary alignment of the structure so that it 
//                   aligns in one byte(to align the structure in 1-byte alignment)
// #pragma pack ()
// For example:
// #pragma pack (1)
// struct sample {
//   Char A;
//   Double b;
// };
// #pragma pack ()
// Note: If you do not enclose #pragma pack (1) and #pragma pack (), sample is 
// aligned by the compiler default (that is, the size of the largest member). 
// That is, by 8-byte (double) alignment, sizeof (sample) == 16. Member Char A 
// takes up 8 bytes (7 of which are empty bytes);
// If #pragma pack (1) is used, sizeof (sample) == 9 is aligned to by 1 bytes. 
// (No empty bytes), more space-saving.


#ifdef _MSC_VER
#define CFMT_PACK_PUSH __pragma(pack(push, 1))
#define CFMT_PACK_POP  __pragma(pack(pop))
#elif defined(__GNUC__)
#define CFMT_PACK_PUSH _Pragma("pack(push, 1)")
#define CFMT_PACK_POP _Pragma("pack(pop)")
#else
#define CFMT_PACK_PUSH
#define CFMT_PACK_POP
#endif

#if _MSC_VER

#ifdef _USE_ATTRIBUTES_FOR_SAL
#undef _USE_ATTRIBUTES_FOR_SAL
#endif

#define _USE_ATTRIBUTES_FOR_SAL 1
#include <sal.h>

#define CFMT_PRINTF_FORMAT _Printf_format_string_
#define CFMT_PRINTF_FORMAT_ATTR(string_index, first_to_check)

#elif defined(__GNUC__)
#define CFMT_PRINTF_FORMAT
#define CFMT_PRINTF_FORMAT_ATTR(string_index, first_to_check) \
  __attribute__((__format__(__printf__, string_index, first_to_check)))
#else
#define CFMT_PRINTF_FORMAT
#define CFMT_PRINTF_FORMAT_ATTR(string_index, first_to_check)
#endif

namespace tz {
namespace detail {
#if defined(_WIN32)
/**
 * Convert a string to wstring
 * @param str input string
 * @return the value of input string as wide string
 */
std::wstring s2ws(const std::string& str);

/**
 * wstring to string
 * @param wstr input wide string
 * @return the value of input wide string as string
 */
std::string ws2s(const std::wstring& wstr);
#endif

 /**
 * Portable gmtime_r or _s per operating system
 * @param timer to a time_t object to convert
 * @param buf to a struct tm object to store the result
 * @return copy of the buf pointer, or throws on error
 * @throws std::system_error
 */
std::tm* gmtime_rs(time_t const* timer, std::tm* buf);

/**
 * Portable localtime_r or _s per operating system
 * @param timer to a time_t object to convert
 * @param buf to a struct tm object to store the result
 * @return copy of the buf pointer, or throws on error
 * @throws std::system_error
 */
std::tm* localtime_rs(time_t const* timer, std::tm* buf);

/**
 * Sets the cpu affinity of the caller thread to the given cpu id
 * @param cpu_id the cpu_id to pin the caller thread
 * @note: cpu_id starts from zero
 * @throws if fails to set cpu affinity
 */
void set_cpu_affinity(uint16_t cpu_id);

/**
 * Sets the name of the caller thread
 * @param name the name of the thread
 * @throws std::runtime_error if it fails to set cpu affinity
 */
void set_thread_name(char const* name);

/**
 * Returns the name of the thread. By default, each thread is unnamed.
 * If set_thread_name has not been used to set the name of the specified thread,
 * a null string is retrieved into name.
 * @return the thread name
 */
std::string get_thread_name();

/**
 * Returns the os assigned ID of the thread
 * @return the thread ID of the calling thread
 */
uint32_t get_thread_id() noexcept;

/**
 * Returns the os assigned ID of the process
 * @return the process ID of the calling process
 */
uint32_t get_process_id() noexcept;

/**
 * Gets the page size
 * @return the size of the page
 */
size_t get_page_size() noexcept;

/**
 * Aligned alloc
 * @param alignment specifies the alignment. Must be a valid alignment supported by the implementation.
 * @param size number of bytes to allocate. An integral multiple of alignment
 * @return On success, returns the pointer to the beginning of newly allocated memory.
 * To avoid a memory leak, the returned pointer must be deallocated with aligned_free().
 * @throws  std::system_error on failure
 */
void* aligned_alloc(size_t alignment, size_t size);

/**
 * Free aligned memory allocated with aligned_alloc
 * @param ptr address to aligned memory
 */
void aligned_free(void* ptr) noexcept;

/**
 * inverses of gmtime
 * @param tm struct tm to convert
 * @throws on invalid input
 */
time_t timegm(struct tm* tm);


/**
 * Check if the terminal supports colours
 * @return true if the terminate supports colours
 */
bool is_colour_terminal() noexcept;

/**
 * Check if file descriptor is attached to terminal
 * @param file the file handler
 * @return true if the file is attached to terminal
 */
bool is_in_terminal(FILE* file) noexcept;

/**
 * Calls strftime and returns a null terminated vector of chars
 * @param format_string The format string to pass to strftime
 * @param time_info_ptr The pointer to tm struct
 * @return the formatted string as vector of characters
 */
std::vector<char> safe_strftime(char const* format_string, const std::tm* time_info_ptr);

} // namespace detail
} // namespace tz

#endif /* PORTABILITY_H__ */

// __attribute__((format(function, string-index, first-to-check)))
// This attribute causes the compiler to check that the supplied arguments are
// in the correct format for the specified function.
// Where function is a printf-style function, such as printf(), scanf(),
// strftime(), gnu_printf(), gnu_scanf(), gnu_strftime(), or strfmon().
// string-index: specifies the index of the string argument in your function 
//               (starting from one). 
// first-to-check: is the index of the first argument to check against the
//                 format string.