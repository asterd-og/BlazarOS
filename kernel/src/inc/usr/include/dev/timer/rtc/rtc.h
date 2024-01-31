#pragma once

#include <types.h>

#define RTC_SECOND 0
#define RTC_MINUTE 0x02
#define RTC_HOUR 0x04
#define RTC_DAY 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09
#define RTC_CENTURY 0x32

u64 rtc_get(u8 type);
u64 rtc_get_unix();