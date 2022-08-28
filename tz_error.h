#ifndef TZ_ERROR_H__
#define TZ_ERROR_H__

#include <exception>
#include <string>

/**
 * Require check
 */
#define TZ_REQUIRE(expression, error)                                          \
do {                                                                           \
    if (!(expression)) [[unlikely]] {                                          \
        printf("Tz fatal error: %s (%s:%d)\n", error, __FILE__, __LINE__);     \
        std::abort();                                                          \
    }                                                                          \
} while (0)

#if defined(TZ_NO_EXCEPTIONS)
  #define TZ_TRY if (true)
  #define TZ_THROW(ex) TZ_REQUIRE(false, ex.what())
  #define TZ_CATCH(x) if (false)
  #define TZ_CATCH_ALL() if (false)
#else
  #define TZ_TRY try
  #define TZ_THROW(ex) throw(ex)
  #define TZ_CATCH(x) catch (x)
  #define TZ_CATCH_ALL() catch (...)
#endif

namespace tz {
/**
 * custom exception
 */
class TzError : public std::exception {
public:
	explicit TzError(std::string s) : _error(std::move(s)) {}
	explicit TzError(char const* s) : _error(s) {}

	char const* what() const noexcept override { return _error.data(); }
private:
	std::string _error;
};

} // namespace tz

#endif /* TZ_ERROR_H__ */
