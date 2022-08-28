#ifndef TIMESTAMP_FORMATTER_H__
#define TIMESTAMP_FORMATTER_H__


#include <chrono>
#include <vector>
#include <chrono>                         // for nanoseconds
#include <cstddef>                        // for size_t
#include <cstdint>                        // for uint32_t, uint8_t
#include <string>                         // for string
#include <array>
#include <string_view>                    // for string_view

#include "tz_type.h"                      // for Timezone, Timezone::LocalTime


/** forward declarations **/
struct tm;

namespace tz {
namespace detail {

/**
 * Formats a timestamp given a format string as a pattern. The format pattern
 * uses the same format specifiers as strftime() but with the following
 * additional specifiers :
 * 1) %Qms - Milliseconds
 * 2) %Qus - Microseconds
 * 3) %Qns - Nanoseconds
 * @note %Qms, %Qus, %Qns specifiers are mutually exclusive and there can only
 * be one in timestamp_format_string
 * e.g given : "%I:%M.%Qms%p" the output would be "03:21.343PM"
 */
class TimestampFormatter {
private:
  enum AdditionalSpecifier : uint8_t {
    None,
    Qms,
    Qus,
    Qns
  };

public:
  /**
   * Constructor
   * @param timestamp_format_string  format string
   * @param timezone_type local time or gmttime, defaults to local time
   * @throws on invalid format string
   */
  explicit TimestampFormatter(std::string const& timestamp_format_string,
                              Timezone timezone_type = Timezone::LocalTime);

  /**
   * Formats a strings given a timestamp
   * @param time_since_epoch the timestamp from epoch
   * @return formatted string
   */
  /*std::string_view*/std::array<char, 100>& format_timestamp(const int64_t timestamp_ns/*std::chrono::nanoseconds time_since_epoch*/);

private:
  //char _formatted_buffer[100];
  std::array<char, 100> _formatted_buffer;

  /** The format string is broken down to two parts. Before and after our additional specifiers */
  std::string _format_part_1;
  std::string _format_part_2;

  /** The pre-formatted timestamp string */
  std::string _pre_formatted_ts_1;
  std::string _pre_formatted_ts_2;

  /** The timestamp of value of our pre-formated string */
  time_t _cached_timestamp;

  /** Strftime cache for both parts of the string */
  //StringFromTime _strftime_part_1;
  //StringFromTime _strftime_part_2;

  /** Timezone, Local time by default **/
  Timezone _timezone_type;

  /** fractional seconds */
  AdditionalSpecifier _additional_format_specifier{AdditionalSpecifier::None};
};

} // namespace detail
} // namespace tz


#endif /* TIMESTAMP_FORMATTER_H__ */