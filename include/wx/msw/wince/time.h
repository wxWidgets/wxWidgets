
/*
 * time.h
 * Missing time functions and structures for use under WinCE
 */

#ifndef __WINCE_TIME_
#define __WINCE_TIME_

struct tm {
        int tm_sec;     /* seconds after the minute - [0,59] */
        int tm_min;     /* minutes after the hour - [0,59] */
        int tm_hour;    /* hours since midnight - [0,23] */
        int tm_mday;    /* day of the month - [1,31] */
        int tm_mon;     /* months since January - [0,11] */
        int tm_year;    /* years since 1900 */
        int tm_wday;    /* days since Sunday - [0,6] */
        int tm_yday;    /* days since January 1 - [0,365] */
        int tm_isdst;   /* daylight savings time flag */
        };

struct tm * __cdecl localtime(const time_t *);

time_t __cdecl time(time_t *);

time_t __cdecl mktime(struct tm *);

struct tm * __cdecl gmtime(const time_t *);

#define _tcsftime   wcsftime

size_t __cdecl wcsftime(wchar_t *, size_t, const wchar_t *,
        const struct tm *);

extern long timezone;

#endif

