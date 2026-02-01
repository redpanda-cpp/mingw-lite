#pragma once

#include <corecrt.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>

#include <windows.h>

namespace mingw_thunk::internal
{
  inline FILETIME filetime_from_c_time64(__time64_t ct)
  {
    ULARGE_INTEGER ftv{
        .QuadPart = ULONGLONG((ct * 10000000LL) + 116444736000000000LL),
    };
    return {
        .dwLowDateTime = ftv.LowPart,
        .dwHighDateTime = ftv.HighPart,
    };
  }

  inline __time64_t c_time64_from_filetime(const FILETIME &ft)
  {
    ULARGE_INTEGER ftv{
        .LowPart = ft.dwLowDateTime,
        .HighPart = ft.dwHighDateTime,
    };
    return int64_t(ftv.QuadPart - 116444736000000000LL) / 10000000LL;
  }

  constexpr int64_t LEAPOCH = 946684800LL + 86400 * (31 + 29);

  constexpr int64_t DAYS_PER_400Y = 365 * 400 + 97;
  constexpr int64_t DAYS_PER_100Y = 365 * 100 + 24;
  constexpr int64_t DAYS_PER_4Y = 365 * 4 + 1;

  // musl 1.2.5
  inline int __secs_to_tm(long long t, struct tm *tm)
  {
    long long days, secs, years;
    int remdays, remsecs, remyears;
    int qc_cycles, c_cycles, q_cycles;
    int months;
    int wday, yday, leap;
    static const char days_in_month[] = {
        31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29};

    /* Reject time_t values whose year would overflow int */
    if (t < INT_MIN * 31622400LL || t > INT_MAX * 31622400LL)
      return -1;

    secs = t - LEAPOCH;
    days = secs / 86400;
    remsecs = secs % 86400;
    if (remsecs < 0) {
      remsecs += 86400;
      days--;
    }

    wday = (3 + days) % 7;
    if (wday < 0)
      wday += 7;

    qc_cycles = days / DAYS_PER_400Y;
    remdays = days % DAYS_PER_400Y;
    if (remdays < 0) {
      remdays += DAYS_PER_400Y;
      qc_cycles--;
    }

    c_cycles = remdays / DAYS_PER_100Y;
    if (c_cycles == 4)
      c_cycles--;
    remdays -= c_cycles * DAYS_PER_100Y;

    q_cycles = remdays / DAYS_PER_4Y;
    if (q_cycles == 25)
      q_cycles--;
    remdays -= q_cycles * DAYS_PER_4Y;

    remyears = remdays / 365;
    if (remyears == 4)
      remyears--;
    remdays -= remyears * 365;

    leap = !remyears && (q_cycles || !c_cycles);
    yday = remdays + 31 + 28 + leap;
    if (yday >= 365 + leap)
      yday -= 365 + leap;

    years = remyears + 4 * q_cycles + 100 * c_cycles + 400LL * qc_cycles;

    for (months = 0; days_in_month[months] <= remdays; months++)
      remdays -= days_in_month[months];

    if (months >= 10) {
      months -= 12;
      years++;
    }

    if (years + 100 > INT_MAX || years + 100 < INT_MIN)
      return -1;

    tm->tm_year = years + 100;
    tm->tm_mon = months + 2;
    tm->tm_mday = remdays + 1;
    tm->tm_wday = wday;
    tm->tm_yday = yday;

    tm->tm_hour = remsecs / 3600;
    tm->tm_min = remsecs / 60 % 60;
    tm->tm_sec = remsecs % 60;

    return 0;
  }

} // namespace mingw_thunk::internal
