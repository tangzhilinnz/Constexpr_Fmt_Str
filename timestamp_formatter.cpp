#include <array>                      // for array
#include <cassert>                    // for assert
#include <cstring>                    // for memcpy, memset
#include <ctime>                      // for size_t, strftime, time_t
#include <utility>                    // for addressof

#include "timestamp_formatter.h"
#include "tz_error.h"                // for TZ_THROW, TzError
#include "constexpr_fmt.h"           // for formatDec
#include "portability.h"

namespace {
// Contains the additional specifier name, at the same index as the enum
std::array<char const*, 4> specifier_name{"", "%Qms", "%Qus", "%Qns"};

// All special specifiers have same length at the moment
constexpr size_t specifier_length = 4;
} // namespace

namespace tz {
namespace detail {


/***/
TimestampFormatter::TimestampFormatter(
  std::string const& timestamp_format_string,
  Timezone timezone_type /* = Timezone::LocalTime */)
  : _format_part_1()
  , _format_part_2() 
  , _pre_formatted_ts_1()
  , _pre_formatted_ts_2()
  , _cached_timestamp(0)
  , _timezone_type(timezone_type) {
  assert((_timezone_type == Timezone::LocalTime || 
          _timezone_type == Timezone::GmtTime) &&
         "Invalid timezone type");

  // store the beginning of the found specifier
  size_t specifier_begin{std::string::npos};

  // look for all three special specifiers
  size_t search_qms = timestamp_format_string.find(specifier_name[AdditionalSpecifier::Qms]);
  if (search_qms != std::string::npos) {
    _additional_format_specifier = AdditionalSpecifier::Qms;
    specifier_begin = search_qms;

    search_qms = timestamp_format_string.find(specifier_name[AdditionalSpecifier::Qms], 
                                              search_qms + specifier_length);
    if (search_qms != std::string::npos)
      TZ_THROW(TzError{ "only one additional specifier can be included" });
  }

  size_t search_qus = timestamp_format_string.find(specifier_name[AdditionalSpecifier::Qus]);
  if (search_qus != std::string::npos) {
    if (_additional_format_specifier != AdditionalSpecifier::None) {
      TZ_THROW(TzError{"format specifiers %Qms, %Qus and %Qns are mutually exclusive"});
    }
    _additional_format_specifier = AdditionalSpecifier::Qus;
    specifier_begin = search_qus;

    search_qus = timestamp_format_string.find(specifier_name[AdditionalSpecifier::Qus],
                                              search_qus + specifier_length);
    if (search_qus != std::string::npos)
      TZ_THROW(TzError{ "only one additional specifier can be included" });
  }

  size_t search_qns = timestamp_format_string.find(specifier_name[AdditionalSpecifier::Qns]);
  if (search_qns != std::string::npos) {
    if (_additional_format_specifier != AdditionalSpecifier::None) {
      TZ_THROW(TzError{"format specifiers %Qms, %Qus and %Qns are mutually exclusive"});
    }
    _additional_format_specifier = AdditionalSpecifier::Qns;
    specifier_begin = search_qns;

    search_qns = timestamp_format_string.find(specifier_name[AdditionalSpecifier::Qns],
                                              search_qns + specifier_length);
    if (search_qns != std::string::npos)
      TZ_THROW(TzError{ "only one additional specifier can be included" });
  }

  if (_additional_format_specifier == AdditionalSpecifier::None) {
    // If no additional specifier was found then we can simply store the whole format string
    _format_part_1 = timestamp_format_string;
  }
  else {
    // We now the index where the specifier begins so copy everything until there from beginning
    _format_part_1 = timestamp_format_string.substr(0, specifier_begin);

    // Now copy he remaining format string, ignoring the specifier
    size_t const specifier_end = specifier_begin + specifier_length;

    _format_part_2 =
      timestamp_format_string.substr(specifier_end, timestamp_format_string.length() - specifier_end);
  }

  // Get the current timestamp now
  time_t timestamp;
  std::time(&timestamp);

  _cached_timestamp = timestamp;

  tm time_info{};

  if (_timezone_type == Timezone::LocalTime) {
    detail::localtime_rs(reinterpret_cast<time_t const*>(std::addressof(_cached_timestamp)),
                         std::addressof(time_info));
  }
  else if (_timezone_type == Timezone::GmtTime) {
    detail::gmtime_rs(reinterpret_cast<time_t const*>(std::addressof(_cached_timestamp)),
                      std::addressof(time_info));
  }

  _pre_formatted_ts_1 = safe_strftime(_format_part_1.data(), std::addressof(time_info)).data();
  if (!_format_part_2.empty()) {
    _pre_formatted_ts_2 = safe_strftime(_format_part_2.data(), std::addressof(time_info)).data();
  }

  // Now init two custom string from time classes with pre-formatted strings
  //_strftime_part_1.init(_format_part_1, _timezone_type);
  //_strftime_part_2.init(_format_part_2, _timezone_type);
}

/***/
/*std::string_view*/std::array<char, 100>& TimestampFormatter::format_timestamp(const int64_t timestamp_ns/*std::chrono::nanoseconds time_since_epoch*/) {
  //int64_t const timestamp_ns = time_since_epoch.count();

  // convert timestamp to seconds
  int64_t const timestamp_secs = timestamp_ns / 1'000'000'000;

  if (static_cast<time_t>(timestamp_secs) != _cached_timestamp) {
    _cached_timestamp = static_cast<time_t>(timestamp_secs);

    tm time_info{};

    if (_timezone_type == Timezone::LocalTime) {
      detail::localtime_rs(reinterpret_cast<time_t const*>(std::addressof(_cached_timestamp)),
                           std::addressof(time_info));
    }
    else if (_timezone_type == Timezone::GmtTime) {
      detail::gmtime_rs(reinterpret_cast<time_t const*>(std::addressof(_cached_timestamp)),
                        std::addressof(time_info));
    }

    _pre_formatted_ts_1 = safe_strftime(_format_part_1.data(), std::addressof(time_info)).data();
    if (!_format_part_2.empty()) {
      _pre_formatted_ts_2 = safe_strftime(_format_part_2.data(), std::addressof(time_info)).data();
    }
  };

  // add any special ms/us/ns specifier if any
  auto const extracted_ns = static_cast<uint32_t>(timestamp_ns - (timestamp_secs * 1'000'000'000));

  int result = 0;
  if (_additional_format_specifier == AdditionalSpecifier::Qms) {
    uint32_t const extracted_ms = extracted_ns / 1'000'000;

    if (_format_part_2.empty()) [[likely]] {
      CFMT_STR(result, _formatted_buffer.data(), 100, "%s%03u", _pre_formatted_ts_1.c_str(), extracted_ms);
    }
    else [[unlikely]] { // format part 2 after fractional seconds - if any
      CFMT_STR(result, _formatted_buffer.data(), 100, "%s%03u%s", _pre_formatted_ts_1.c_str(), extracted_ms, _pre_formatted_ts_2.c_str());
    }
  }
  else if (_additional_format_specifier == AdditionalSpecifier::Qus) {
    uint32_t const extracted_us = extracted_ns / 1'000;

    if (_format_part_2.empty()) [[likely]] {
      CFMT_STR(result, _formatted_buffer.data(), 100, "%s%06u", _pre_formatted_ts_1.data(), extracted_us);
    }
    else [[unlikely]] { // format part 2 after fractional seconds - if any
      CFMT_STR(result, _formatted_buffer.data(), 100, "%s%06u%s", _pre_formatted_ts_1.data(), extracted_us, _pre_formatted_ts_2.c_str());
    }
  }
  else if (_additional_format_specifier == AdditionalSpecifier::Qns) {
    if (_format_part_2.empty()) [[likely]] {
      CFMT_STR(result, _formatted_buffer.data(), 100, "%s%09u", _pre_formatted_ts_1.c_str(), extracted_ns);
    }
    else [[unlikely]] { // format part 2 after fractional seconds - if any
      CFMT_STR(result, _formatted_buffer.data(), 100, "%s%09u%s", _pre_formatted_ts_1.c_str(), extracted_ns, _pre_formatted_ts_2.c_str());
    }
  }

  //return std::string_view(_formatted_buffer);
  return _formatted_buffer;
}

} // namespace detail
} // namespace tz