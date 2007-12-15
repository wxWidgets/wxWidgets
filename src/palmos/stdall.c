
#include <Preferences.h> // Core/System/
#include <DateTime.h> //Core/System/, DateToAscii() etc.
#include <TimeMgr.h> //Core/System/, TimGetTicks()

#include "wx/defs.h"

#if __WXPALMOS6__
void
exit (int exitno)
{
}

#else

#define SEC_1904_1970 2082844800LU//0x7C25B080 // 0x7C25B080 == 2082844800LU

time_t
time (time_t *T)
{
    UInt32 sec;
    // UInt32 TimGetSeconds(void); // seconds since 1/1/1904
    // void TimSetSeconds (UInt32 seconds); // seconds since 1/1/1904
    // UInt32 TimGetTicks(void); // ticks since power on
    sec = TimGetSeconds ();
    if (T) {
        *T = sec - SEC_1904_1970;
    }
    return (sec - SEC_1904_1970);
}

static struct tm *
palmtimet2unixtm (const time_t t, struct tm *ptm)
{
    DateTimeType dt;
    memset (&dt, 0, sizeof (dt));
    memset (ptm, 0, sizeof (*ptm));
    TimSecondsToDateTime (t, &dt);
    ptm->tm_sec  = dt.second;
    ptm->tm_min  = dt.minute;
    ptm->tm_hour = dt.hour;
    ptm->tm_mday = dt.day;
    ptm->tm_mon  = dt.month;
    ptm->tm_year = dt.year;
    ptm->tm_wday = dt.weekDay;

    //ptm->tm_wday = DayOfWeek (dt.month, dt.day, dt.year);
    //ptm->tm_mday = DayOfMonth (dt.month, dt.day, dt.year);
    return ptm;
}

struct tm *
gmtime_r (const time_t *pt, struct tm *ptm)
{
    UInt32 utcTime;
    Int16 timeZone = PrefGetPreference(prefTimeZone);
    Int16 daylightSavingAdjustment = PrefGetPreference(prefDaylightSavingAdjustment);

    if ((NULL == pt) || (NULL == ptm)) {
        return NULL;
    }
    utcTime = TimTimeZoneToUTC (*pt + SEC_1904_1970, timeZone, daylightSavingAdjustment);
    palmtimet2unixtm (utcTime, ptm);
    return ptm;
}

struct tm *
localtime_r (const time_t *pt, struct tm *ptm)
{
    UInt32 utcTime;
    Int16 timeZone = PrefGetPreference(prefTimeZone);
    Int16 daylightSavingAdjustment = PrefGetPreference(prefDaylightSavingAdjustment);
    if ((NULL == pt) || (NULL == ptm)) {
        return NULL;
    }
    utcTime = TimUTCToTimeZone (*pt + SEC_1904_1970, timeZone, daylightSavingAdjustment);
    palmtimet2unixtm (utcTime, ptm);
    return ptm;
}
/*
static struct tm g_gmtime_info;

struct tm *
gmtime (const time_t *CLOCK)
{
    if (NULL == CLOCK) {
        return NULL;
    }
    if (NULL == gmtime_r (CLOCK, &g_gmtime_info)) {
        return NULL;
    }
    return &g_gmtime_info;
}
*/
static struct tm g_localtime_info;

struct tm *
localtime (const time_t *CLOCK)
{
    if (NULL == CLOCK) {
        return NULL;
    }
    if (NULL == localtime_r (CLOCK, &g_localtime_info)) {
        return NULL;
    }
    return &g_localtime_info;
}

static char * g_strftime_wdayconv[][2] = {
    {"Sun", "Sunday"},
    {"Mon", "Monday"},
    {"Tue", "Tuesday"},
    {"Wed", "Wednesday"},
    {"Thu", "Thursday"},
    {"Fri", "Friday"},
    {"Sat", "Saturday"},
};

static char * g_strftime_monconv[][2] = {
    {"Jan", "January"},
    {"Feb", "Febuary"},
    {"Mar", "March"},
    {"Apr", "April"},
    {"May", "May"},
    {"Jun", "June"},
    {"Jul", "July"},
    {"Aug", "August"},
    {"Sep", "September"},
    {"Oct", "October"},
    {"Nov", "November"},
    {"Dec", "December"},
};
#define SFCONV_ABBR 0
#define SFCONV_FULL 1

size_t
strftime (char *buf, size_t sz_buf, const char *fmt, const struct tm *ptm)
{
    char *p;
    char *pret;
    if ((timeStringLength > sz_buf) || (strlen (fmt) < 1)) {
        return 0;
    }
    memset (buf, 0, sz_buf);
    p = fmt;
    pret = buf;
    for (; *p != '\0';) {
        if ('%' == *p) {
            p ++;
            if (*p == '\0') {
                break;
            }
            // FIXME: ÅÐ¶Ï»º³åÊÇ·ñÒç³ö
            switch (*p) {
            case 'a':
                strcpy (pret, g_strftime_wdayconv[ptm->tm_wday % 7][SFCONV_ABBR]);
                pret += 3;
                break;
            case 'A':
                strcpy (pret, g_strftime_wdayconv[ptm->tm_wday % 7][SFCONV_FULL]);
                pret += strlen (g_strftime_wdayconv[ptm->tm_wday % 7][SFCONV_FULL]);
                break;
            case 'b':
                strcpy (pret, g_strftime_monconv[ptm->tm_mon % 12][SFCONV_ABBR]);
                pret += 3;
                break;
            case 'B':
                strcpy (pret, g_strftime_monconv[ptm->tm_mon % 12][SFCONV_FULL]);
                pret += strlen (g_strftime_monconv[ptm->tm_mon % 12][SFCONV_FULL]);
                break;
            case 'c':
                strcpy (pret, g_strftime_wdayconv[ptm->tm_wday % 7][SFCONV_ABBR]);
                pret += 3;
                *pret = ' ';
                pret ++;
                strcpy (pret, g_strftime_monconv[ptm->tm_mon % 12][SFCONV_ABBR]);
                pret += 3;
                *pret = ' ';
                pret ++;
                sprintf (pret, "%d %02d:%02d:%02d %d", ptm->tm_mday,
                    ptm->tm_hour, ptm->tm_min, ptm->tm_sec, ptm->tm_year);
                pret += strlen (pret);
                break;
            case 'd':
                sprintf (pret, "%02d", ptm->tm_mday % 31);
                pret += 2;
                break;
            case 'H':
                sprintf (pret, "%02d", ptm->tm_hour % 24);
                pret += 2;
                break;
            case 'I':
                sprintf (pret, "%02d", (ptm->tm_hour % 12) + 1);
                pret += 2;
                break;
            case 'j':
                sprintf (pret, "%03d", ptm->tm_year % 400);
                pret += 3;
                break;
            case 'm':
                sprintf (pret, "%02d", (ptm->tm_mon % 12) + 1);
                pret += 2;
                break;
            case 'M':
                sprintf (pret, "%02d", ptm->tm_min % 60);
                pret += 2;
                break;
            case 'p':
                if (ptm->tm_hour > 12) {
                    strcpy (pret, "PM");
                    break;
                }
                strcpy (pret, "AM");
                pret += 2;
                break;
            case 'S':
                sprintf (pret, "%02d", ptm->tm_sec % 61);
                pret += 2;
                break;
            case 'U':
                // FIXME: Week number with the first Sunday as the first day of week one (00-53)
                sprintf (pret, "%02d", ptm->tm_yday / 7);
                pret += 2;
                break;
            case 'w':
                sprintf (pret, "%d", ptm->tm_wday % 7);
                pret += 1;
                break;
            case 'W':
                // FIXME: Week number with the first Sunday as the first day of week one (00-53)
                sprintf (pret, "%02d", ptm->tm_yday / 7);
                pret += 2;
                break;
            case 'x':
                sprintf (pret, "%02d/%02d/%02d",
                    ptm->tm_mon, ptm->tm_mday, ptm->tm_year - 1900);
                pret += strlen (pret);
                break;
            case 'X':
                sprintf (pret, "%02d:%02d:%02d",
                    ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
                pret += strlen (pret);
                break;
            case 'y':
                sprintf (pret, "%02d", ptm->tm_year - 1900);
                pret += 2;
                break;
            case 'Y':
                sprintf (pret, "%02d", ptm->tm_year);
                pret += strlen (pret);
                break;
            case 'Z':
            {
                LmLocaleType ll;
                ll.language = lmAnyLanguage;
                ll.country = 0;
                TimeZoneToAscii (ptm->tm_gmtoff, &ll, pret);
            }
                pret += strlen (pret);
                break;
            case '%':
                *pret = *p;
                pret ++;
                break;
            }
        } else {
            *pret = *p;
            pret ++;
        }
        p ++;
    }
    return 0;
}

time_t
mktime (struct tm *ptm)
{
    DateTimeType dt;
    if (NULL == ptm) {
        return 0;
    }
    memset (&dt, 0, sizeof (dt));
    dt.second  = ptm->tm_sec;
    dt.minute  = ptm->tm_min;
    dt.hour    = ptm->tm_hour;
    dt.day     = ptm->tm_mday;
    dt.month   = ptm->tm_mon;
    dt.year    = ptm->tm_year;
    dt.weekDay = ptm->tm_wday;
    return TimDateTimeToSeconds (&dt);
}

int
vsscanf (const char *s, const char *format, va_list param)
{
    return -1;
}

FILE * fopen (const char *_name, const char *_type) {return NULL;}
int fclose (FILE *fp) {return 0;}
size_t fread (void *buf, size_t _size, size_t _n, FILE *fp) {return 0;}
size_t fwrite (const void *buf , size_t _size, size_t _n, FILE *fp) {return 0;}
int fseek (FILE *fp, long offset, int whence) {return -1;}
long ftell (FILE *fp) {return -1;}
int feof (FILE *fp) {return -1;}
int ferror (FILE *fp) {return -1;}
void clearerr (FILE *fp) {}

#endif // __WXPALMOS6__

#define _BIT_ALPHA  0X0001
#define _BIT_BLANK  0X0002
#define _BIT_CNTRL  0X0004
#define _BIT_DIGIT  0X0008
#define _BIT_GRAPH  0X0010
#define _BIT_LOWER  0X0020
#define _BIT_PRINT  0X0040
#define _BIT_PUNCT  0X0080
#define _BIT_SPACE  0X0100
#define _BIT_UPPER  0X0200
#define _BIT_XDIGIT 0X0400

int iswalnum(wint_t C) {return 0;}
int iswalpha(wint_t C) {return 0;}
int iswcntrl(wint_t C) {return 0;}
int iswdigit(wint_t C) {return 0;}
int iswgraph(wint_t C) {return 0;}
int iswlower(wint_t C) {return 0;}
int iswprint(wint_t C) {return 0;}
int iswpunct(wint_t C) {return 0;}
int iswspace(wint_t C) {return 0;}
int iswupper(wint_t C) {return 0;}
int iswxdigit(wint_t C) {return 0;}

wint_t towlower(wint_t C) {return 0;}
wint_t towupper(wint_t C) {return 0;}
size_t wcsftime(wchar_t *strDest, size_t maxsize, const wchar_t *format, const struct tm *timeptr) {return 0;}

size_t wcslen (const wchar_t * str) {return 0;}
wchar_t * wcscpy (wchar_t * dst, const wchar_t * src) {return NULL;}
wchar_t * wcsncpy (wchar_t * dst, const wchar_t * src, size_t n) {return NULL;}
wchar_t * wcscat (wchar_t * dst, const wchar_t * src) {return NULL;}
wchar_t * wcsncat (wchar_t * dst, const wchar_t * src, size_t n) {return NULL;}
int wcscmp (const wchar_t * str1, const wchar_t * str2) {return 0;}
int wcsncmp (const wchar_t * str1, const wchar_t * str2, size_t n) {return 0;}
wchar_t * wcschr (const wchar_t * str, const wchar_t chr) {return NULL;}
int wcscoll (const wchar_t *str1, const wchar_t * str2) {return 0;}
size_t wcsxfrm (wchar_t * str1, const wchar_t * str2, size_t n) {return 0;}
wchar_t * wcsrchr (const wchar_t * str, wchar_t chr) {return NULL;}
wchar_t * wcspbrk (const wchar_t * str, const wchar_t * set) {return NULL;}
size_t wcsspn (const wchar_t * str, const wchar_t * set) {return 0;}
size_t wcscspn (const wchar_t * str, const wchar_t * set) {return 0;}
wchar_t * wcsstr (const wchar_t * str, const wchar_t * pat) {return NULL;}
wchar_t * wcstok (wchar_t * str, const wchar_t * set, wchar_t ** a) {return NULL;}

unsigned long wcstoul (const wchar_t * a, wchar_t ** b, int c) {return 0;}
double wcstod (const wchar_t * a, wchar_t ** b) {return 0.0;}

long wcstol (const wchar_t * str, wchar_t ** end, int base) {return 0;}
char * setlocale (int category, const char *locale) {return NULL;}
