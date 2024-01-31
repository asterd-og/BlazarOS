#include <dev/timer/rtc/rtc.h>
#include <arch/io.h>

bool rtc_leap_year(u64 year, u64 month) {
    if(year % 4 == 0 && (month == 1 || month == 2)) return true;
    return false;
}

u64 rtc_get(u8 type) {
    outb(0x70, type);
    u64 ret = inb(0x71);
    return (ret >> 4) * 10 + (ret & 15);
}

// Got these next 2 functions from ilobilo (https://github.com/ilobilo)
u64 jdn(u8 days, u8 months, u16 years) {
    return (1461 * (years + 4800 + (months - 14) / 12)) / 4 + (367 * (months - 2 - 12 * ((months - 14) / 12))) / 12 - (3 * ((years + 4900 + (months - 14) / 12) / 100)) / 4 + days - 32075;
}

u64 rtc_get_unix() {
    u64 second = rtc_get(RTC_SECOND);
    u64 minute = rtc_get(RTC_MINUTE);
    u64 hour = rtc_get(RTC_HOUR);
    u64 day = rtc_get(RTC_DAY);
    u64 year = rtc_get(RTC_YEAR);
    u64 month = rtc_get(RTC_MONTH);
    u64 century = rtc_get(RTC_CENTURY);

    u64 jdn_current = jdn(day, month, century * 100 + year);
    u64 jdn_1970 = jdn(1, 1, 1970);

    return ((jdn_current - jdn_1970) * (60 * 60 * 24)) + hour * 3600 + minute * 60 + second;
}