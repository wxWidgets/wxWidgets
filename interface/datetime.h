/////////////////////////////////////////////////////////////////////////////
// Name:        datetime.h
// Purpose:     documentation for wxDateTime class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxDateTime
    @wxheader{datetime.h}

    wxDateTime class represents an absolute moment in the time.

    @library{wxbase}
    @category{data}

    @seealso
    @ref overview_wxdatetimeoverview "Date classes overview", wxTimeSpan,
    wxDateSpan, wxCalendarCtrl
*/
class wxDateTime
{
public:
    /**
        Same as @ref setdate() Set
    */
    wxDateTime(wxDateTime_t day, Month month = Inv_Month,
               int Inv_Year, wxDateTime_t hour = 0,
               wxDateTime_t minute = 0,
               wxDateTime_t second = 0,
               wxDateTime_t millisec = 0);

    /**
        Here are the trivial accessors. Other functions, which might have to perform
        some more complicated calculations to find the answer are under the
        @ref overview_datetimecalculations "Calendar calculations" section.
        
        IsValid()
        
        GetTicks()
        
        GetCentury()
        
        GetYear()
        
        GetMonth()
        
        GetDay()
        
        GetWeekDay()
        
        GetHour()
        
        GetMinute()
        
        GetSecond()
        
        GetMillisecond()
        
        GetDayOfYear()
        
        GetWeekOfYear()
        
        GetWeekOfMonth()
        
        GetYearDay()
        
        IsWorkDay()
        
        IsGregorianDate()
        
        GetAsDOS()
    */


    //@{
    /**
        Adds the given date span to this object.
    */
    wxDateTime Add(const wxDateSpan& diff);
    wxDateTime Add(const wxDateSpan& diff);
    wxDateTime operator+=(const wxDateSpan& diff);
    //@}

    /**
        Some degree of support for the date units used in astronomy and/or history is
        provided. You can construct a wxDateTime object from a
        @ref setjdn() JDN and you may also get its JDN,
        @ref getmodifiedjuliandaynumber() MJD or
        @ref getratadie() "Rata Die number" from it.
        
        @ref wxdatetimejdn() "wxDateTime(double jdn)"
        
        @ref setjdn() "Set(double jdn)"
        
        GetJulianDayNumber()
        
        GetJDN()
        
        GetModifiedJulianDayNumber()
        
        GetMJD()
        
        GetRataDie()
    */


    /**
        The functions in this section perform the basic calendar calculations, mostly
        related to the week days. They allow to find the given week day in the
        week with given number (either in the month or in the year) and so on.
        
        All (non-const) functions in this section don't modify the time part of the
        wxDateTime -- they only work with the date part of it.
        
        SetToWeekDayInSameWeek()
        
        GetWeekDayInSameWeek()
        
        SetToNextWeekDay()
        
        GetNextWeekDay()
        
        SetToPrevWeekDay()
        
        GetPrevWeekDay()
        
        SetToWeekDay()
        
        @ref wxDateTime::getweekday2 GetWeekDay
        
        SetToLastWeekDay()
        
        GetLastWeekDay()
        
        SetToWeekOfYear()
        
        SetToLastMonthDay()
        
        GetLastMonthDay()
        
        SetToYearDay()
        
        GetYearDay()
    */


    /**
        Constructors and various @c Set() methods are collected here. If you
        construct a date object from separate values for day, month and year, you
        should use IsValid() method to check that the
        values were correct as constructors can not return an error code.
        
        @ref wxdatetimedef() wxDateTime
        
        @ref wxdatetimetimet() wxDateTime(time_t)
        
        @ref wxdatetimetm() "wxDateTime(struct tm)"
        
        @ref wxdatetimejdn() "wxDateTime(double jdn)"
        
        @ref wxdatetimetime() "wxDateTime(h, m, s, ms)"
        
        @ref wxdatetimedate() "wxDateTime(day, mon, year, h, m, s, ms)"
        
        SetToCurrent()
        
        @ref settimet() Set(time_t)
        
        @ref settm() "Set(struct tm)"
        
        @ref setjdn() "Set(double jdn)"
        
        @ref settime() "Set(h, m, s, ms)"
        
        @ref setdate() "Set(day, mon, year, h, m, s, ms)"
        
        @ref setfromdos() "SetFromDOS(unsigned long ddt)"
        
        ResetTime()
        
        SetYear()
        
        SetMonth()
        
        @ref setdate() SetDay
        
        SetHour()
        
        SetMinute()
        
        SetSecond()
        
        SetMillisecond()
        
        @ref operatoreqtimet() operator=(time_t)
        
        @ref operatoreqtm() "operator=(struct tm)"
    */


    /**
        Converts the year in absolute notation (i.e. a number which can be negative,
        positive or zero) to the year in BC/AD notation. For the positive years,
        nothing is done, but the year 0 is year 1 BC and so for other years there is a
        difference of 1.
        
        This function should be used like this:
    */
    static int ConvertYearToBC(int year);

    /**
        These functions carry out arithmetics on the wxDateTime
        objects. As explained in the overview, either wxTimeSpan or wxDateSpan may be
        added to wxDateTime, hence all functions are overloaded to accept both
        arguments.
        
        Also, both @c Add() and @c Subtract() have both const and non-const
        version. The first one returns a new object which represents the
        sum/difference of the original one with the argument while the second form
        modifies the object to which it is applied. The operators -= and += are
        defined to be equivalent to the second forms of these functions.
        
        @ref addts() Add(wxTimeSpan)
        
        @ref addds() Add(wxDateSpan)
        
        @ref subtractts() Subtract(wxTimeSpan)
        
        @ref subtractds() Subtract(wxDateSpan)
        
        @ref subtractdt() Subtract(wxDateTime)
        
        @ref addts() oparator+=(wxTimeSpan)
        
        @ref addds() oparator+=(wxDateSpan)
        
        @ref subtractts() oparator-=(wxTimeSpan)
        
        @ref subtractds() oparator-=(wxDateSpan)
    */


    /**
        There are several function to allow date comparison. To supplement them, a few
        global operators ,  etc taking wxDateTime are defined.
        
        IsEqualTo()
        
        IsEarlierThan()
        
        IsLaterThan()
        
        IsStrictlyBetween()
        
        IsBetween()
        
        IsSameDate()
        
        IsSameTime()
        
        IsEqualUpTo()
    */


    /**
        This function does the same as the standard ANSI C @c strftime(3) function.
        Please see its description for the meaning of @e format parameter.
        
        It also accepts a few wxWidgets-specific extensions: you can optionally specify
        the width of the field to follow using @c printf(3)-like syntax and the
        format specification @c %l can be used to get the number of milliseconds.
        
        @sa ParseFormat()
    */
    wxString Format(const wxChar * format = wxDefaultDateTimeFormat,
                    const TimeZone& tz = Local);

    /**
        Identical to calling Format() with @c "%x"
        argument (which means 'preferred date representation for the current locale').
    */
    wxString FormatDate();

    /**
        Returns the combined date-time representation in the ISO 8601 format
        (YYYY-MM-DDTHH:MM:SS). The @e sep parameter default value produces the
        result exactly corresponding to the ISO standard, but it can also be useful to
        use a space as seprator if a more human-readable combined date-time
        representation is needed.
        
        @sa FormatISODate(), FormatISOTime(),
              ParseISOCombined()
    */
    wxString FormatISOCombined(char sep = 'T');

    /**
        This function returns the date representation in the ISO 8601 format
        (YYYY-MM-DD).
    */
    wxString FormatISODate();

    /**
        This function returns the time representation in the ISO 8601 format
        (HH:MM:SS).
    */
    wxString FormatISOTime();

    /**
        Identical to calling Format() with @c "%X"
        argument (which means 'preferred time representation for the current locale').
    */
    wxString FormatTime();

    /**
        Transform the date from the given time zone to the local one. If @e noDST is
        @true, no DST adjustments will be made.
        
        Returns the date in the local time zone.
    */
    wxDateTime FromTimezone(const TimeZone& tz,
                            bool noDST = @false);

    /**
        Returns the translations of the strings @c AM and @c PM used for time
        formatting for the current locale. Either of the pointers may be @NULL if
        the corresponding value is not needed.
    */
    static void GetAmPmStrings(wxString * am, wxString * pm);

    /**
        Returns the date and time in
        DOS
        format.
    */
    unsigned long GetAsDOS();

    /**
        Get the beginning of DST for the given country in the given year (current one
        by default). This function suffers from limitations described in
        @ref overview_tdatedst "DST overview".
        
        @sa GetEndDST()
    */
    static wxDateTime GetBeginDST(int year = Inv_Year,
                                  Country country = Country_Default);

    /**
        Returns the century of this date.
    */
    int GetCentury(const TimeZone& tz = Local);

    /**
        Returns the current default country. The default country is used for DST
        calculations, for example.
        
        @sa SetCountry()
    */
    static Country GetCountry();

    /**
        Get the current month in given calendar (only Gregorian is currently supported).
    */
    static Month GetCurrentMonth(Calendar cal = Gregorian);

    /**
        Get the current year in given calendar (only Gregorian is currently supported).
    */
    static int GetCurrentYear(Calendar cal = Gregorian);

    /**
        Returns the object having the same date component as this one but time of
        00:00:00.
        
        This function is new since wxWidgets version 2.8.2
        
        @sa ResetTime()
    */
    wxDateTime GetDateOnly();

    /**
        Returns the day in the given timezone (local one by default).
    */
    wxDateTime_t GetDay(const TimeZone& tz = Local);

    /**
        Returns the day of the year (in 1...366 range) in the given timezone
        (local one by default).
    */
    wxDateTime_t GetDayOfYear(const TimeZone& tz = Local);

    /**
        Returns the end of DST for the given country in the given year (current one by
        default).
        
        @sa GetBeginDST()
    */
    static wxDateTime GetEndDST(int year = Inv_Year,
                                Country country = Country_Default);

    /**
        Returns the hour in the given timezone (local one by default).
    */
    wxDateTime_t GetHour(const TimeZone& tz = Local);

    /**
        Synonym for GetJulianDayNumber().
    */
#define double GetJDN()     /* implementation is private */

    /**
        Returns the @ref setjdn() JDN corresponding to this date. Beware
        of rounding errors!
        
        @sa GetModifiedJulianDayNumber()
    */
    double GetJulianDayNumber();

    /**
        Returns the copy of this object to which
        SetToLastMonthDay() was applied.
    */
    wxDateTime GetLastMonthDay(Month month = Inv_Month,
                               int year = Inv_Year);

    /**
        Returns the copy of this object to which
        SetToLastWeekDay() was applied.
    */
    wxDateTime GetLastWeekDay(WeekDay weekday,
                              Month month = Inv_Month,
                              int year = Inv_Year);

    /**
        Synonym for GetModifiedJulianDayNumber().
    */
#define double GetMJD()     /* implementation is private */

    /**
        Returns the milliseconds in the given timezone (local one by default).
    */
    wxDateTime_t GetMillisecond(const TimeZone& tz = Local);

    /**
        Returns the minute in the given timezone (local one by default).
    */
    wxDateTime_t GetMinute(const TimeZone& tz = Local);

    /**
        Returns the @e Modified Julian Day Number (MJD) which is, by definition,
        equal to JDN - 2400000.5. The MJDs are simpler to work with as the integral
        MJDs correspond to midnights of the dates in the Gregorian calendar and not th
        noons like JDN. The MJD 0 is Nov 17, 1858.
    */
    double GetModifiedJulianDayNumber();

    /**
        Returns the month in the given timezone (local one by default).
    */
    Month GetMonth(const TimeZone& tz = Local);

    /**
        Gets the full (default) or abbreviated (specify @c Name_Abbr name of the
        given month.
        
        @sa GetWeekDayName()
    */
    static wxString GetMonthName(Month month,
                                 NameFlags flags = Name_Full);

    /**
        Returns the copy of this object to which
        SetToNextWeekDay() was applied.
    */
    wxDateTime GetNextWeekDay(WeekDay weekday);

    //@{
    /**
        Returns the number of days in the given year or in the given month of the
        year.
        
        The only supported value for @e cal parameter is currently @c Gregorian.
    */
    static wxDateTime_t GetNumberOfDays(int year,
                                        Calendar cal = Gregorian);
    static wxDateTime_t GetNumberOfDays(Month month,
                                        int year = Inv_Year,
                                        Calendar cal = Gregorian);
    //@}

    /**
        Returns the copy of this object to which
        SetToPrevWeekDay() was applied.
    */
    wxDateTime GetPrevWeekDay(WeekDay weekday);

    /**
        Return the @e Rata Die number of this date.
        
        By definition, the Rata Die number is a date specified as the number of days
        relative to a base date of December 31 of the year 0. Thus January 1 of the
        year 1 is Rata Die day 1.
    */
    double GetRataDie();

    /**
        Returns the seconds in the given timezone (local one by default).
    */
    wxDateTime_t GetSecond(const TimeZone& tz = Local);

    /**
        Returns the number of seconds since Jan 1, 1970. An assert failure will occur
        if the date is not in the range covered by @c time_t type.
    */
    time_t GetTicks();

    /**
        Returns the current time.
    */
    static time_t GetTimeNow();

    /**
        Returns broken down representation of the date and time.
    */
    Tm GetTm(const TimeZone& tz = Local);

    /**
        Returns the current time broken down. Note that this function returns a
        pointer to a static buffer that's reused by calls to this function and
        certain C library functions (e.g. localtime). If there is any chance your
        code might be used in a multi-threaded application, you really should use
        the flavour of function GetTmNow()
        taking a parameter.
    */
    static struct tm * GetTmNow();

    /**
        Returns the copy of this object to which
        SetToWeekDay() was applied.
    */
    wxDateTime GetWeekDay(WeekDay weekday, int n = 1,
                          Month month = Inv_Month,
                          int year = Inv_Year);

    /**
        Returns the copy of this object to which
        SetToWeekDayInSameWeek() was
        applied.
    */
    wxDateTime GetWeekDayInSameWeek(WeekDay weekday,
                                    WeekFlags flags = Monday_First);

    /**
        Gets the full (default) or abbreviated (specify @c Name_Abbr name of the
        given week day.
        
        @sa GetMonthName()
    */
    static wxString GetWeekDayName(WeekDay weekday,
                                   NameFlags flags = Name_Full);

    /**
        Returns the ordinal number of the week in the month (in 1...5  range).
        
        As GetWeekOfYear(), this function supports
        both conventions for the week start. See the description of these
        @ref overview_wxdatetime "week start" conventions.
    */
    wxDateTime_t GetWeekOfMonth(WeekFlags flags = Monday_First,
                                const TimeZone& tz = Local);

    /**
        Returns the number of the week of the year this date is in. The first week of
        the year is, according to international standards, the one containing Jan 4 or,
        equivalently, the first week which has Thursday in this year. Both of these
        definitions are the same as saying that the first week of the year must contain
        more than half of its days in this year. Accordingly, the week number will
        always be in 1...53 range (52 for non-leap years).
        
        The function depends on the @ref overview_wxdatetime "week start" convention
        specified by the @e flags argument but its results for
        @c Sunday_First are not well-defined as the ISO definition quoted above
        applies to the weeks starting on Monday only.
    */
    wxDateTime_t GetWeekOfYear(WeekFlags flags = Monday_First,
                               const TimeZone& tz = Local);

    /**
        Returns the year in the given timezone (local one by default).
    */
    int GetYear(const TimeZone& tz = Local);

    /**
        Returns the copy of this object to which
        SetToYearDay() was applied.
    */
    wxDateTime GetYearDay(wxDateTime_t yday);

    /**
        Returns @true if IsStrictlyBetween()
        is @true or if the date is equal to one of the limit values.
        
        @sa IsStrictlyBetween()
    */
    bool IsBetween(const wxDateTime& t1, const wxDateTime& t2);

    /**
        Returns @true if the DST is applied for this date in the given country.
    */
#define int IsDST(Country country = Country_Default)     /* implementation is private */

    /**
        Returns @true if DST was used n the given year (the current one by
        default) in the given country.
    */
    static bool IsDSTApplicable(int year = Inv_Year,
                                Country country = Country_Default);

    /**
        Returns @true if this date precedes the given one.
    */
    bool IsEarlierThan(const wxDateTime& datetime);

    /**
        Returns @true if the two dates are strictly identical.
    */
    bool IsEqualTo(const wxDateTime& datetime);

    /**
        Returns @true if the date is equal to another one up to the given time
        interval, i.e. if the absolute difference between the two dates is less than
        this interval.
    */
    bool IsEqualUpTo(const wxDateTime& dt, const wxTimeSpan& ts);

    /**
        Returns @true if the given date is later than the date of adoption of the
        Gregorian calendar in the given country (and hence the Gregorian calendar
        calculations make sense for it).
    */
    bool IsGregorianDate(GregorianAdoption country = Gr_Standard);

    /**
        Returns @true if this date is later than the given one.
    */
    bool IsLaterThan(const wxDateTime& datetime);

    /**
        Returns @true if the @e year is a leap one in the specified calendar.
        
        This functions supports Gregorian and Julian calendars.
    */
    static bool IsLeapYear(int year = Inv_Year,
                           Calendar cal = Gregorian);

    /**
        Returns @true if the date is the same without comparing the time parts.
    */
    bool IsSameDate(const wxDateTime& dt);

    /**
        Returns @true if the time is the same (although dates may differ).
    */
    bool IsSameTime(const wxDateTime& dt);

    /**
        Returns @true if this date lies strictly between the two others,
        
        @sa IsBetween()
    */
    bool IsStrictlyBetween(const wxDateTime& t1,
                           const wxDateTime& t2);

    /**
        Returns @true if the object represents a valid time moment.
    */
    bool IsValid();

    /**
        This function returns @true if the specified (or default) country is one
        of Western European ones. It is used internally by wxDateTime to determine the
        DST convention and date and time formatting rules.
    */
    static bool IsWestEuropeanCountry(Country country = Country_Default);

    /**
        Returns @true is this day is not a holiday in the given country.
    */
    bool IsWorkDay(Country country = Country_Default);

    /**
        Same as FromTimezone() but modifies the object
        in place.
    */
    wxDateTime MakeFromTimezone(const TimeZone& tz,
                                bool noDST = @false);

    /**
        Modifies the object in place to represent the date in another time zone. If
        @e noDST is @true, no DST adjustments will be made.
    */
    wxDateTime MakeTimezone(const TimeZone& tz,
                            bool noDST = @false);

    /**
        This is the same as calling MakeTimezone() with
        the argument @c GMT0.
    */
    wxDateTime MakeUTC(bool noDST = @false);

    /**
        Returns the object corresponding to the current time.
        
        Example:
        Note that this function is accurate up to second:
        UNow() should be used for better precision
        (but it is less efficient and might not be available on all platforms).
        
        @sa Today()
    */
#define static wxDateTime Now()     /* implementation is private */

    //@{
    /**
        This function is like ParseDateTime(), but it
        only allows the date to be specified. It is thus less flexible then
        ParseDateTime(), but also has less chances to
        misinterpret the user input.
        
        Returns @NULL if the conversion failed, otherwise return the pointer to
        the character which stopped the scan.
    */
    const char * ParseDate(const wxString& date,
                           wxString::const_iterator * end = @NULL);
    const char * ParseDate(const char * date);
    const wchar_t * ParseDate(const wchar_t * date);
    //@}

    //@{
    /**
        Parses the string @e datetime containing the date and time in free format.
        This function tries as hard as it can to interpret the given string as date
        and time. Unlike wxDateTime::ParseRfc822Date, it
        will accept anything that may be accepted and will only reject strings which
        can not be parsed in any way at all.
        
        Returns @NULL if the conversion failed, otherwise return the pointer to
        the character which stopped the scan.
    */
    const char * ParseDateTime(const wxString& datetime,
                               wxString::const_iterator * end = @NULL);
    const char * ParseDateTime(const char * datetime);
    const wchar_t * ParseDateTime(const wchar_t * datetime);
    //@}

    //@{
    /**
        This function parses the string @e date according to the given
        @e format. The system @c strptime(3) function is used whenever available,
        but even if it is not, this function is still implemented, although support
        for locale-dependent format specifiers such as @c "%c", @c "%x" or @c "%X" may
        not be perfect and GNU extensions such as @c "%z" and @c "%Z" are
        not implemented. This function does handle the month and weekday
        names in the current locale on all platforms, however.
        
        Please see the description of the ANSI C function @c strftime(3) for the syntax
        of the format string.
        
        The @e dateDef parameter is used to fill in the fields which could not be
        determined from the format string. For example, if the format is @c "%d" (the
        ay of the month), the month and the year are taken from @e dateDef. If
        it is not specified, Today() is used as the
        default date.
        
        Returns @NULL if the conversion failed, otherwise return the pointer to
        the character which stopped the scan.
    */
    const char * ParseFormat(const wxString& date,
                             const wxString& format = wxDefaultDateTimeFormat,
                             const wxDateTime& dateDef = wxDefaultDateTime,
                             wxString::const_iterator * end = @NULL);
    const char * ParseFormat(const char * date,
                             const wxString& format = wxDefaultDateTimeFormat,
                             const wxDateTime& dateDef = wxDefaultDateTime);
    const wchar_t * ParseFormat(const wchar_t * date,
                                const wxString& format = wxDefaultDateTimeFormat,
                                const wxDateTime& dateDef = wxDefaultDateTime);
    //@}

    /**
        This function parses the string containing the date and time in ISO 8601
        combined format (YYYY-MM-DDTHH:MM:SS). The separator between the date and time
        parts must be equal to @e sep for the function to succeed.
        
        Returns @true if the entire string was parsed successfully, @false
        otherwise.
    */
    bool ParseISOCombined(const wxString& date, char sep = 'T');

    /**
        This function parses the date in ISO 8601 format (YYYY-MM-DD).
        
        Returns @true if the entire string was parsed successfully, @false
        otherwise.
    */
    bool ParseISODate(const wxString& date);

    /**
        This function parses the time in ISO 8601 format (HH:MM:SS).
        
        Returns @true if the entire string was parsed successfully, @false
        otherwise.
    */
    bool ParseISOTime(const wxString& date);

    //@{
    /**
        Parses the string @e date looking for a date formatted according to the RFC
        822 in it. The exact description of this format may, of course, be found in
        the RFC (section 5), but, briefly, this is the format used in the headers of
        Internet email messages and one of the most common strings expressing date in
        this format may be something like @c "Sat, 18 Dec 1999 00:48:30 +0100".
        
        Returns @NULL if the conversion failed, otherwise return the pointer to
        the character immediately following the part of the string which could be
        parsed. If the entire string contains only the date in RFC 822 format,
        the returned pointer will be pointing to a @c NUL character.
        
        This function is intentionally strict, it will return an error for any string
        which is not RFC 822 compliant. If you need to parse date formatted in more
        free ways, you should use ParseDateTime() or
        ParseDate() instead.
    */
    const char * ParseRfc822Date(const wxString& date,
                                 wxString::const_iterator * end = @NULL);
    const char * ParseRfc822Date(const char* date);
    const wchar_t * ParseRfc822Date(const wchar_t* date);
    //@}

    //@{
    /**
        This functions is like ParseDateTime(), but
        only allows the time to be specified in the input string.
        
        Returns @NULL if the conversion failed, otherwise return the pointer to
        the character which stopped the scan.
    */
    const char * ParseTime(const wxString& time,
                           wxString::const_iterator * end = @NULL);
    const char * ParseTime(const char * time);
    const wchar_t * ParseTime(const wchar_t * time);
    //@}

    /**
        These functions convert wxDateTime objects to and from text. The
        conversions to text are mostly trivial: you can either do it using the default
        date and time representations for the current locale (
        FormatDate() and
        wxDateTime::FormatTime), using the international standard
        representation defined by ISO 8601 (
        FormatISODate(),
        FormatISOTime() and
        wxDateTime::FormatISOCombined) or by specifying any
        format at all and using Format() directly.
        
        The conversions from text are more interesting, as there are much more
        possibilities to care about. The simplest cases can be taken care of with
        ParseFormat() which can parse any date in the
        given (rigid) format. wxDateTime::ParseRfc822Date is
        another function for parsing dates in predefined format -- the one of RFC 822
        which (still...) defines the format of email messages on the Internet. This
        format can not be described with @c strptime(3)-like format strings used by
        Format(), hence the need for a separate function.
        
        But the most interesting functions are
        ParseTime(),
        ParseDate() and
        ParseDateTime(). They try to parse the date
        ans time (or only one of them) in 'free' format, i.e. allow them to be
        specified in any of possible ways. These functions will usually be used to
        parse the (interactive) user input which is not bound to be in any predefined
        format. As an example, ParseDateTime() can
        parse the strings such as @c "tomorrow", @c "March first" and even
        @c "next Sunday".
        
        Finally notice that each of the parsing functions is available in several
        overloads: if the input string is a narrow (@c char *) string, then a
        narrow pointer is returned. If the input string is a wide string, a wide char
        pointer is returned. Finally, if the input parameter is a wxString, a narrow
        char pointer is also returned for backwards compatibility but there is also an
        additional argument of wxString::const_iterator type in which, if it is not
        @NULL, an iterator pointing to the end of the scanned string part is returned.
        
        ParseFormat()
        
        ParseDateTime()
        
        ParseDate()
        
        ParseTime()
        
        ParseISODate()
        
        ParseISOTime()
        
        ParseISOCombined()
        
        wxDateTime::ParseRfc822Date
        
        Format()
        
        FormatDate()
        
        FormatTime()
        
        FormatISOCombined()
        
        FormatISODate()
        
        FormatISOTime()
    */


    /**
        Reset time to midnight (00:00:00) without changing the date.
    */
    wxDateTime ResetTime();

    /**
        Sets the date and time from the parameters.
    */
#define wxDateTime Set(wxDateTime_t day, Month month = Inv_Month,
    int year = Inv_Year,
               wxDateTime_t hour = 0,
                                   wxDateTime_t minute = 0,
                                                         wxDateTime_t second = 0,
                                                                               wxDateTime_t millisec = 0)     /* implementation is private */

    /**
        Sets the country to use by default. This setting influences the DST
        calculations, date formatting and other things.
        
        The possible values for @e country parameter are enumerated in
        @ref overview_wxdatetime "wxDateTime constants section".
        
        @sa GetCountry()
    */
    static void SetCountry(Country country);

    /**
        Sets the day without changing other date components.
    */
    wxDateTime SetDay(wxDateTime_t day);

    /**
        Sets the date from the date and time in
        DOS
        format.
    */
    wxDateTime Set(unsigned long ddt);

    /**
        Sets the hour without changing other date components.
    */
    wxDateTime SetHour(wxDateTime_t hour);

    /**
        Sets the millisecond without changing other date components.
    */
    wxDateTime SetMillisecond(wxDateTime_t millisecond);

    /**
        Sets the minute without changing other date components.
    */
    wxDateTime SetMinute(wxDateTime_t minute);

    /**
        Sets the month without changing other date components.
    */
    wxDateTime SetMonth(Month month);

    /**
        Sets the second without changing other date components.
    */
    wxDateTime SetSecond(wxDateTime_t second);

    /**
        Sets the date and time of to the current values. Same as assigning the result
        of Now() to this object.
    */
    wxDateTime SetToCurrent();

    /**
        Sets the date to the last day in the specified month (the current one by
        default).
        
        Returns the reference to the modified object itself.
    */
    wxDateTime SetToLastMonthDay(Month month = Inv_Month,
                                 int year = Inv_Year);

    /**
        The effect of calling this function is the same as of calling
        @c SetToWeekDay(-1, weekday, month, year). The date will be set to the last
        @e weekday in the given month and year (the current ones by default).
        
        Always returns @true.
    */
    bool SetToLastWeekDay(WeekDay weekday, Month month = Inv_Month,
                          int year = Inv_Year);

    /**
        Sets the date so that it will be the first @e weekday following the current
        date.
        
        Returns the reference to the modified object itself.
    */
    wxDateTime SetToNextWeekDay(WeekDay weekday);

    /**
        Sets the date so that it will be the last @e weekday before the current
        date.
        
        Returns the reference to the modified object itself.
    */
    wxDateTime SetToPrevWeekDay(WeekDay weekday);

    /**
        Sets the date to the @e n-th @e weekday in the given month of the given
        year (the current month and year are used by default). The parameter @e n
        may be either positive (counting from the beginning of the month) or negative
        (counting from the end of it).
        
        For example, @c SetToWeekDay(2, wxDateTime::Wed) will set the date to the
        second Wednesday in the current month and
        @c SetToWeekDay(-1, wxDateTime::Sun) -- to the last Sunday in it.
        
        Returns @true if the date was modified successfully, @false
        otherwise meaning that the specified date doesn't exist.
    */
    bool SetToWeekDay(WeekDay weekday, int n = 1,
                      Month month = Inv_Month,
                                    int year = Inv_Year);

    /**
        Adjusts the date so that it will still lie in the same week as before, but its
        week day will be the given one.
        
        Returns the reference to the modified object itself.
    */
    wxDateTime SetToWeekDayInSameWeek(WeekDay weekday,
                                      WeekFlags flags = Monday_First);

    /**
        Set the date to the given @e weekday in the week number @e numWeek of the
        given @e year . The number should be in range 1...53.
        
        Note that the returned date may be in a different year than the one passed to
        this function because both the week 1 and week 52 or 53 (for leap years)
        contain days from different years. See
        GetWeekOfYear() for the explanation of how the
        year weeks are counted.
    */
    static wxDateTime SetToWeekOfYear(int year, wxDateTime_t numWeek,
                                      WeekDay weekday = Mon);

    /**
        Sets the date to the day number @e yday in the same year (i.e., unlike the
        other functions, this one does not use the current year). The day number
        should be in the range 1...366 for the leap years and 1...365 for
        the other ones.
        
        Returns the reference to the modified object itself.
    */
    wxDateTime SetToYearDay(wxDateTime_t yday);

    /**
        Sets the year without changing other date components.
    */
    wxDateTime SetYear(int year);

    /**
        For convenience, all static functions are collected here. These functions
        either set or return the static variables of wxDateSpan (the country), return
        the current moment, year, month or number of days in it, or do some general
        calendar-related actions.
        
        Please note that although several function accept an extra @e Calendar
        parameter, it is currently ignored as only the Gregorian calendar is
        supported. Future versions will support other calendars.
        SetCountry()
        
        GetCountry()
        
        IsWestEuropeanCountry()
        
        GetCurrentYear()
        
        ConvertYearToBC()
        
        GetCurrentMonth()
        
        IsLeapYear()
        
        @ref getcenturystatic() GetCentury
        
        GetNumberOfDays()
        
        GetNumberOfDays()
        
        GetMonthName()
        
        GetWeekDayName()
        
        GetAmPmStrings()
        
        IsDSTApplicable()
        
        GetBeginDST()
        
        GetEndDST()
        
        Now()
        
        UNow()
        
        Today()
    */


    /**
        Subtracts another date from this one and returns the difference between them
        as wxTimeSpan.
    */
    wxTimeSpan Subtract(const wxDateTime& dt);

    /**
        Please see the @ref overview_tdatetimezones "time zone overview" for more
        information about time zones. Normally, these functions should be rarely used.
        
        FromTimezone()
        
        ToTimezone()
        
        MakeTimezone()
        
        MakeFromTimezone()
        
        ToUTC()
        
        MakeUTC()
        
        GetBeginDST()
        
        GetEndDST()
        
        IsDST()
    */


    /**
        Transform the date to the given time zone. If @e noDST is @true, no
        DST adjustments will be made.
        
        Returns the date in the new time zone.
    */
    wxDateTime ToTimezone(const TimeZone& tz, bool noDST = @false);

    /**
        This is the same as calling ToTimezone() with
        the argument @c GMT0.
    */
#define wxDateTime ToUTC(bool noDST = @false)     /* implementation is private */

    /**
        Returns the object corresponding to the midnight of the current day (i.e. the
        same as Now(), but the time part is set to 0).
        
        @sa Now()
    */
    static wxDateTime Today();

    /**
        Returns the object corresponding to the current time including the
        milliseconds if a function to get time with such precision is available on the
        current platform (supported under most Unices and Win32).
        
        @sa Now()
    */
#define static wxDateTime UNow()     /* implementation is private */

    /**
        Same as @ref settm() Set.
    */
    wxDateTime operator(const struct tm& tm);
            };


/**
    @class wxDateTimeWorkDays
    @wxheader{datetime.h}


    @library{wxbase}
    @category{FIXME}
*/
class wxDateTimeWorkDays
{
public:

};


/**
    @class wxDateSpan
    @wxheader{datetime.h}

    This class is a "logical time span" and is useful for implementing program
    logic for such things as "add one month to the date" which, in general,
    doesn't mean to add 60*60*24*31 seconds to it, but to take the same date
    the next month (to understand that this is indeed different consider adding
    one month to Feb, 15 -- we want to get Mar, 15, of course).

    When adding a month to the date, all lesser components (days, hours, ...)
    won't be changed unless the resulting date would be invalid: for example,
    Jan 31 + 1 month will be Feb 28, not (non-existing) Feb 31.

    Because of this feature, adding and subtracting back again the same
    wxDateSpan will @b not, in general give back the original date: Feb 28 - 1
    month will be Jan 28, not Jan 31!

    wxDateSpan objects can be either positive or negative. They may be
    multiplied by scalars which multiply all deltas by the scalar: i.e.
    2*(1  month and  1  day) is 2 months and 2 days. They can
    be added together and with wxDateTime or
    wxTimeSpan, but the type of result is different for each
    case.

    Beware about weeks: if you specify both weeks and days, the total number of
    days added will be 7*weeks + days! See also GetTotalDays()
    function.

    Equality operators are defined for wxDateSpans. Two datespans are equal if
    and only if they both give the same target date when added to @b every
    source date. Thus wxDateSpan::Months(1) is not equal to wxDateSpan::Days(30),
    because they don't give the same date when added to 1 Feb. But
    wxDateSpan::Days(14) is equal to wxDateSpan::Weeks(2)

    Finally, notice that for adding hours, minutes and so on you don't need this
    class at all: wxTimeSpan will do the job because there
    are no subtleties associated with those (we don't support leap seconds).

    @library{wxbase}
    @category{data}

    @seealso
    @ref overview_wxdatetimeoverview "Date classes overview", wxDateTime
*/
class wxDateSpan
{
public:
    /**
        Constructs the date span object for the given number of years, months, weeks
        and days. Note that the weeks and days add together if both are given.
    */
    wxDateSpan(int years = 0, int months = 0, int weeks = 0,
               int days = 0);

    //@{
    /**
        Returns the sum of two date spans. The first version returns a new object, the
        second and third ones modify this object in place.
    */
    wxDateSpan Add(const wxDateSpan& other);
    wxDateSpan Add(const wxDateSpan& other);
    wxDateSpan operator+=(const wxDateSpan& other);
    //@}

    /**
        Returns a date span object corresponding to one day.
        
        @sa Days()
    */
#define static wxDateSpan Day()     /* implementation is private */

    /**
        Returns a date span object corresponding to the given number of days.
        
        @sa Day()
    */
    static wxDateSpan Days(int days);

    /**
        Returns the number of days (only, that it not counting the weeks component!)
        in this date span.
        
        @sa GetTotalDays()
    */
    int GetDays();

    /**
        Returns the number of the months (not counting the years) in this date span.
    */
    int GetMonths();

    /**
        Returns the combined number of days in this date span, counting both weeks and
        days. It still doesn't take neither months nor years into the account.
        
        @sa GetWeeks(), GetDays()
    */
    int GetTotalDays();

    /**
        Returns the number of weeks in this date span.
        
        @sa GetTotalDays()
    */
    int GetWeeks();

    /**
        Returns the number of years in this date span.
    */
    int GetYears();

    /**
        Returns a date span object corresponding to one month.
        
        @sa Months()
    */
    static wxDateSpan Month();

    /**
        Returns a date span object corresponding to the given number of months.
        
        @sa Month()
    */
    static wxDateSpan Months(int mon);

    //@{
    /**
        Returns the product of the date span by the specified @e factor. The
        product is computed by multiplying each of the components by the factor.
        
        The first version returns a new object, the second and third ones modify this
        object in place.
    */
    wxDateSpan Multiply(int factor);
    wxDateSpan Multiply(int factor);
    wxDateSpan operator*=(int factor);
    //@}

    //@{
    /**
        Changes the sign of this date span.
        
        @sa Negate()
    */
    wxDateSpan Neg();
    wxDateSpan operator-();
    //@}

    /**
        Returns the date span with the opposite sign.
        
        @sa Neg()
    */
    wxDateSpan Negate();

    /**
        Sets the number of days (without modifying any other components) in this date
        span.
    */
    wxDateSpan SetDays(int n);

    /**
        Sets the number of months (without modifying any other components) in this
        date span.
    */
    wxDateSpan SetMonths(int n);

    /**
        Sets the number of weeks (without modifying any other components) in this date
        span.
    */
    wxDateSpan SetWeeks(int n);

    /**
        Sets the number of years (without modifying any other components) in this date
        span.
    */
    wxDateSpan SetYears(int n);

    //@{
    /**
        Returns the difference of two date spans. The first version returns a new
        object, the second and third ones modify this object in place.
    */
    wxDateSpan Subtract(const wxDateSpan& other);
    wxDateSpan Subtract(const wxDateSpan& other);
    wxDateSpan operator+=(const wxDateSpan& other);
    //@}

    /**
        Returns a date span object corresponding to one week.
        
        @sa Weeks()
    */
    static wxDateSpan Week();

    /**
        Returns a date span object corresponding to the given number of weeks.
        
        @sa Week()
    */
    static wxDateSpan Weeks(int weeks);

    /**
        Returns a date span object corresponding to one year.
        
        @sa Years()
    */
    static wxDateSpan Year();

    /**
        Returns a date span object corresponding to the given number of years.
        
        @sa Year()
    */
    static wxDateSpan Years(int years);

    /**
        Returns @true if this date span is different from the other one.
    */
    bool operator!=(wxDateSpan& other);

    /**
        Returns @true if this date span is equal to the other one. Two date spans
        are considered equal if and only if they have the same number of years and
        months and the same total number of days (counting both days and weeks).
    */
    bool operator==(wxDateSpan& other);
};


/**
    @class wxTimeSpan
    @wxheader{datetime.h}

    wxTimeSpan class represents a time interval.

    @library{wxbase}
    @category{data}

    @seealso
    @ref overview_wxdatetimeoverview "Date classes overview", wxDateTime
*/
class wxTimeSpan
{
public:
    //@{
    /**
        Constructs timespan from separate values for each component, with the date
        set to 0. Hours are not restricted to 0..24 range, neither are
        minutes, seconds or milliseconds.
    */
    wxTimeSpan();
    wxTimeSpan(long hours, long min, long sec, long msec);
    //@}

    /**
        Returns the absolute value of the timespan: does not modify the
        object.
    */
#define wxTimeSpan Abs()     /* implementation is private */

    /**
        GetSeconds()
        
        GetMinutes()
        
        GetHours()
        
        GetDays()
        
        GetWeeks()
        
        GetValue()
    */


    //@{
    /**
        Returns the sum of two timespans.
    */
    wxTimeSpan Add(const wxTimeSpan& diff);
    wxTimeSpan Add(const wxTimeSpan& diff);
    wxTimeSpan operator+=(const wxTimeSpan& diff);
    //@}

    /**
        @ref ctor() wxTimeSpan
    */


    /**
        Returns the timespan for one day.
    */
#define static wxTimespan Day()     /* implementation is private */

    /**
        Returns the timespan for the given number of days.
    */
    static wxTimespan Days(long days);

    /**
        Returns the string containing the formatted representation of the time span.
        The following format specifiers are allowed after %:
        
        H
        
        
        number of @b Hours
        
        M
        
        
        number of @b Minutes
        
        S
        
        
        number of @b Seconds
        
        l
        
        
        number of mi@b lliseconds
        
        D
        
        
        number of @b Days
        
        E
        
        
        number of w@b Eeks
        
        %
        
        
        the percent character
        
        Note that, for example, the number of hours in the description above is not
        well defined: it can be either the total number of hours (for example, for a
        time span of 50 hours this would be 50) or just the hour part of the time
        span, which would be 2 in this case as 50 hours is equal to 2 days and
        2 hours.
        
        wxTimeSpan resolves this ambiguity in the following way: if there had been,
        indeed, the @c %D format specified preceding the @c %H, then it is
        interpreted as 2. Otherwise, it is 50.
        
        The same applies to all other format specifiers: if they follow a specifier of
        larger unit, only the rest part is taken, otherwise the full value is used.
    */
    wxString Format(const wxChar * format = wxDefaultTimeSpanFormat);

    /**
        Format()
    */


    /**
        Returns the difference in number of days.
    */
    int GetDays();

    /**
        Returns the difference in number of hours.
    */
    int GetHours();

    /**
        Returns the difference in number of milliseconds.
    */
    wxLongLong GetMilliseconds();

    /**
        Returns the difference in number of minutes.
    */
    int GetMinutes();

    /**
        Returns the difference in number of seconds.
    */
    wxLongLong GetSeconds();

    /**
        Returns the internal representation of timespan.
    */
    wxLongLong GetValue();

    /**
        Returns the difference in number of weeks.
    */
    int GetWeeks();

    /**
        Returns the timespan for one hour.
    */
    static wxTimespan Hour();

    /**
        Returns the timespan for the given number of hours.
    */
    static wxTimespan Hours(long hours);

    /**
        Returns @true if two timespans are equal.
    */
    bool IsEqualTo(const wxTimeSpan& ts);

    /**
        Compares two timespans: works with the absolute values, i.e. -2
        hours is longer than 1 hour. Also, it will return @false if
        the timespans are equal in absolute value.
    */
    bool IsLongerThan(const wxTimeSpan& ts);

    /**
        Returns @true if the timespan is negative.
    */
    bool IsNegative();

    /**
        Returns @true if the timespan is empty.
    */
    bool IsNull();

    /**
        Returns @true if the timespan is positive.
    */
    bool IsPositive();

    /**
        Compares two timespans: works with the absolute values, i.e. 1
        hour is shorter than -2 hours. Also, it will return @false if
        the timespans are equal in absolute value.
    */
    bool IsShorterThan(const wxTimeSpan& ts);

    /**
        Returns the timespan for one millisecond.
    */
    static wxTimespan Millisecond();

    /**
        Returns the timespan for the given number of milliseconds.
    */
    static wxTimespan Milliseconds(long ms);

    /**
        Returns the timespan for one minute.
    */
    static wxTimespan Minute();

    /**
        Returns the timespan for the given number of minutes.
    */
    static wxTimespan Minutes(long min);

    //@{
    /**
        Multiplies timespan by a scalar.
    */
    wxTimeSpan Multiply(int n);
    wxTimeSpan Multiply(int n);
    wxTimeSpan operator*=(int n);
    //@}

    //@{
    /**
        Negate the value of the timespan.
    */
    wxTimeSpan Neg();
    wxTimeSpan operator-();
    //@}

    /**
        Returns timespan with inverted sign.
    */
    wxTimeSpan Negate();

    /**
        Add()
        
        Subtract()
        
        Multiply()
        
        Negate()
        
        Neg()
        
        Abs()
    */


    /**
        Returns the timespan for one second.
    */
    static wxTimespan Second();

    /**
        Returns the timespan for the given number of seconds.
    */
    static wxTimespan Seconds(long sec);

    /**
        Milliseconds()
        
        Millisecond()
        
        Seconds()
        
        Second()
        
        Minutes()
        
        Minute()
        
        Hours()
        
        Hour()
        
        Days()
        
        Day()
        
        Weeks()
        
        Week()
    */


    //@{
    /**
        Returns the difference of two timespans.
    */
    wxTimeSpan Subtract(const wxTimeSpan& diff);
    wxTimeSpan Subtract(const wxTimeSpan& diff);
    wxTimeSpan operator-=(const wxTimeSpan& diff);
    //@}

    /**
        IsNull()
        
        IsPositive()
        
        IsNegative()
        
        IsEqualTo()
        
        IsLongerThan()
        
        IsShorterThan()
    */


    /**
        Returns the timespan for one week.
    */
    static wxTimespan Week();

    /**
        Returns the timespan for the given number of weeks.
    */
    static wxTimespan Weeks(long weeks);
};


/**
    @class wxDateTimeHolidayAuthority
    @wxheader{datetime.h}


    @library{wxbase}
    @category{FIXME}
*/
class wxDateTimeHolidayAuthority
{
public:

};
