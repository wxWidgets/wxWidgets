/////////////////////////////////////////////////////////////////////////////
// Name:        utils.i
// Purpose:     SWIG definitions of various utility classes
//
// Author:      Robin Dunn
//
// Created:     25-Nov-1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module utils

%{
#include "export.h"
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/datetime.h>
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i

%pragma(python) code = "import string"


//---------------------------------------------------------------------------

%{
    static wxString wxPyEmptyStr("");

    static PyObject* __EnumerationHelper(bool flag, wxString& str, long index) {
        PyObject* ret = PyTuple_New(3);
        if (ret) {
            PyTuple_SET_ITEM(ret, 0, PyInt_FromLong(flag));
            PyTuple_SET_ITEM(ret, 1, PyString_FromString(str));
            PyTuple_SET_ITEM(ret, 2, PyInt_FromLong(index));
        }
        return ret;
    }
%}

//---------------------------------------------------------------------------

enum
{
    wxCONFIG_USE_LOCAL_FILE = 1,
    wxCONFIG_USE_GLOBAL_FILE = 2,
    wxCONFIG_USE_RELATIVE_PATH = 4
};

//---------------------------------------------------------------------------

class wxConfigBase {
public:
//      wxConfigBase(const wxString& appName = wxPyEmptyStr,       **** An ABC
//                   const wxString& vendorName = wxPyEmptyStr,
//                   const wxString& localFilename = wxPyEmptyStr,
//                   const wxString& globalFilename = wxPyEmptyStr,
//                   long style = 0);
    ~wxConfigBase();

    enum EntryType
    {
        Type_Unknown,
        Type_String,
        Type_Boolean,
        Type_Integer,    // use Read(long *)
        Type_Float       // use Read(double *)
    };

  // static functions
    // sets the config object, returns the previous pointer
    static wxConfigBase *Set(wxConfigBase *pConfig);
    // get the config object, creates it on demand unless DontCreateOnDemand
    // was called
    static wxConfigBase *Get(bool createOnDemand = TRUE);
    // create a new config object: this function will create the "best"
    // implementation of wxConfig available for the current platform, see
    // comments near definition wxUSE_CONFIG_NATIVE for details. It returns
    // the created object and also sets it as ms_pConfig.
    static wxConfigBase *Create();
    // should Get() try to create a new log object if the current one is NULL?
    static void DontCreateOnDemand();



    bool DeleteAll();  // This is supposed to have been fixed...
    bool DeleteEntry(const wxString& key, bool bDeleteGroupIfEmpty = TRUE);
    bool DeleteGroup(const wxString& key);
    bool Exists(wxString& strName);
    bool Flush(bool bCurrentOnly = FALSE);
    wxString GetAppName();


    // Each of these enumeration methods return a 3-tuple consisting of
    // the continue flag, the value string, and the index for the next call.
    %addmethods {
        PyObject* GetFirstGroup() {
            bool     cont;
            long     index = 0;
            wxString value;

            cont = self->GetFirstGroup(value, index);
            return __EnumerationHelper(cont, value, index);
        }

        PyObject* GetFirstEntry() {
            bool     cont;
            long     index = 0;
            wxString value;

            cont = self->GetFirstEntry(value, index);
            return __EnumerationHelper(cont, value, index);
        }

        PyObject* GetNextGroup(long index) {
            bool     cont;
            wxString value;

            cont = self->GetNextGroup(value, index);
            return __EnumerationHelper(cont, value, index);
        }

        PyObject* GetNextEntry(long index) {
            bool     cont;
            wxString value;

            cont = self->GetNextEntry(value, index);
            return __EnumerationHelper(cont, value, index);
        }
    }


    int GetNumberOfEntries(bool bRecursive = FALSE);
    int GetNumberOfGroups(bool bRecursive = FALSE);
    wxString GetPath();
    wxString GetVendorName();
    bool HasEntry(wxString& strName);
    bool HasGroup(const wxString& strName);
    bool IsExpandingEnvVars();
    bool IsRecordingDefaults();

    wxString Read(const wxString& key, const wxString& defaultVal = wxPyEmptyStr);
    %name(ReadInt)long Read(const wxString& key, long defaultVal = 0);
    %name(ReadFloat)double Read(const wxString& key, double defaultVal = 0.0);

    void SetExpandEnvVars (bool bDoIt = TRUE);
    void SetPath(const wxString& strPath);
    void SetRecordDefaults(bool bDoIt = TRUE);
    void SetAppName(const wxString& appName);
    void SetVendorName(const wxString& vendorName);

    void SetStyle(long style);
    long GetStyle();

    bool Write(const wxString& key, const wxString& value);
    %name(WriteInt)bool Write(const wxString& key, long value);
    %name(WriteFloat)bool Write(const wxString& key, double value);

    EntryType GetEntryType(const wxString& name);
    bool RenameEntry(const wxString& oldName,
                     const wxString& newName);
    bool RenameGroup(const wxString& oldName,
                     const wxString& newName);
    wxString ExpandEnvVars(const wxString& str);


};

//---------------------------------------------------------------------------

class wxConfig : public wxConfigBase {
public:
    wxConfig(const wxString& appName = wxPyEmptyStr,
             const wxString& vendorName = wxPyEmptyStr,
             const wxString& localFilename = wxPyEmptyStr,
             const wxString& globalFilename = wxPyEmptyStr,
             long style = 0);
    ~wxConfig();
};

class wxFileConfig : public wxConfigBase {
public:
    wxFileConfig(const wxString& appName = wxPyEmptyStr,
                 const wxString& vendorName = wxPyEmptyStr,
                 const wxString& localFilename = wxPyEmptyStr,
                 const wxString& globalFilename = wxPyEmptyStr,
                 long style = 0);
    ~wxFileConfig();
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class wxDateTime;
class wxTimeSpan;
class wxDateSpan;


%typemap(python,in) wxDateTime::TimeZone& {
    $target = new wxDateTime::TimeZone((wxDateTime::TZ)PyInt_AsLong($source));
}
%typemap(python,freearg) wxDateTime::TimeZone& {
    if ($source) delete $source;
}

%{
#define LOCAL *(new wxDateTime::TimeZone(wxDateTime::Local))
%}


%typemap(python, out) wxLongLong {
    PyObject *hi, *lo, *shifter, *shifted;
    hi = PyLong_FromLong($source->GetHi());
    lo = PyLong_FromLong($source->GetLo());
    shifter = PyLong_FromLong(32);
    shifted = PyNumber_Lshift(hi, shifter);
    $target = PyNumber_Or(shifted, lo);
    Py_DECREF(hi);
    Py_DECREF(lo);
    Py_DECREF(shifter);
    Py_DECREF(shifted);
}




class wxDateTime {
public:
    typedef unsigned short wxDateTime_t;

    enum TZ
    {
        Local,

        GMT_12, GMT_11, GMT_10, GMT_9, GMT_8, GMT_7,
        GMT_6, GMT_5, GMT_4, GMT_3, GMT_2, GMT_1,
        GMT0,
        GMT1, GMT2, GMT3, GMT4, GMT5, GMT6,
        GMT7, GMT8, GMT9, GMT10, GMT11, GMT12,

        // Europe
        WET = GMT0,                         // Western Europe Time
        WEST = GMT1,                        // Western Europe Summer Time
        CET = GMT1,                         // Central Europe Time
        CEST = GMT2,                        // Central Europe Summer Time
        EET = GMT2,                         // Eastern Europe Time
        EEST = GMT3,                        // Eastern Europe Summer Time
        MSK = GMT3,                         // Moscow Time
        MSD = GMT4,                         // Moscow Summer Time

        // US and Canada
        AST = GMT_4,                        // Atlantic Standard Time
        ADT = GMT_3,                        // Atlantic Daylight Time
        EST = GMT_5,                        // Eastern Standard Time
        EDT = GMT_4,                        // Eastern Daylight Saving Time
        CST = GMT_6,                        // Central Standard Time
        CDT = GMT_5,                        // Central Daylight Saving Time
        MST = GMT_7,                        // Mountain Standard Time
        MDT = GMT_6,                        // Mountain Daylight Saving Time
        PST = GMT_8,                        // Pacific Standard Time
        PDT = GMT_7,                        // Pacific Daylight Saving Time
        HST = GMT_10,                       // Hawaiian Standard Time
        AKST = GMT_9,                       // Alaska Standard Time
        AKDT = GMT_8,                       // Alaska Daylight Saving Time

        // Australia

        A_WST = GMT8,                       // Western Standard Time
        A_CST = GMT12 + 1,                  // Central Standard Time (+9.5)
        A_EST = GMT10,                      // Eastern Standard Time
        A_ESST = GMT11,                     // Eastern Summer Time

        // Universal Coordinated Time = the new and politically correct name
        // for GMT
        UTC = GMT0
    };

    enum Calendar
    {
        Gregorian,  // current calendar
        Julian      // calendar in use since -45 until the 1582 (or later)
    };

    enum Country
    {
        Country_Unknown, // no special information for this country
        Country_Default, // set the default country with SetCountry() method
                         // or use the default country with any other

        // Western European countries: we assume that they all follow the same
        // DST rules (true or false?)
        Country_WesternEurope_Start,
        Country_EEC = Country_WesternEurope_Start,
        France,
        Germany,
        UK,
        Country_WesternEurope_End = UK,

        Russia,

        USA
    };

        // symbolic names for the months
    enum Month
    {
        Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec, Inv_Month
    };

        // symbolic names for the weekdays
    enum WeekDay
    {
        Sun, Mon, Tue, Wed, Thu, Fri, Sat, Inv_WeekDay
    };

        // invalid value for the year
    enum Year
    {
        Inv_Year = SHRT_MIN    // should hold in wxDateTime_t
    };

        // flags for GetWeekDayName and GetMonthName
    enum NameFlags
    {
        Name_Full = 0x01,       // return full name
        Name_Abbr = 0x02        // return abbreviated name
    };

        // flags for GetWeekOfYear and GetWeekOfMonth
    enum WeekFlags
    {
        Default_First,   // Sunday_First for US, Monday_First for the rest
        Monday_First,    // week starts with a Monday
        Sunday_First     // week starts with a Sunday
    };


    // static methods
    // ------------------------------------------------------------------------

        // set the current country
    static void SetCountry(Country country);
        // get the current country
    static Country GetCountry();

        // return TRUE if the country is a West European one (in practice,
        // this means that the same DST rules as for EEC apply)
    static bool IsWestEuropeanCountry(Country country = Country_Default);

        // return the current year
    static int GetCurrentYear(Calendar cal = Gregorian);

        // convert the year as returned by wxDateTime::GetYear() to a year
        // suitable for BC/AD notation. The difference is that BC year 1
        // corresponds to the year 0 (while BC year 0 didn't exist) and AD
        // year N is just year N.
    static int ConvertYearToBC(int year);

        // return the current month
    static Month GetCurrentMonth(Calendar cal = Gregorian);

        // returns TRUE if the given year is a leap year in the given calendar
    static bool IsLeapYear(int year = Inv_Year, Calendar cal = Gregorian);

        // get the century (19 for 1999, 20 for 2000 and -5 for 492 BC)
    static int GetCentury(int year = Inv_Year);

        // returns the number of days in this year (356 or 355 for Gregorian
        // calendar usually :-)
    %name(GetNumberOfDaysinYear)
        static wxDateTime_t GetNumberOfDays(int year, Calendar cal = Gregorian);

        // get the number of the days in the given month (default value for
        // the year means the current one)
    %name(GetNumberOfDaysInMonth)
    static wxDateTime_t GetNumberOfDays(Month month,
                                        int year = Inv_Year,
                                        Calendar cal = Gregorian);

        // get the full (default) or abbreviated month name in the current
        // locale, returns empty string on error
    static wxString GetMonthName(Month month,
                                 NameFlags flags = Name_Full);

        // get the full (default) or abbreviated weekday name in the current
        // locale, returns empty string on error
    static wxString GetWeekDayName(WeekDay weekday,
                                   NameFlags flags = Name_Full);

        // get the AM and PM strings in the current locale (may be empty)
    static void GetAmPmStrings(wxString *OUTPUT, wxString *OUTPUT);

        // return TRUE if the given country uses DST for this year
    static bool IsDSTApplicable(int year = Inv_Year,
                                Country country = Country_Default);

        // get the beginning of DST for this year, will return invalid object
        // if no DST applicable in this year. The default value of the
        // parameter means to take the current year.
    static wxDateTime GetBeginDST(int year = Inv_Year,
                                  Country country = Country_Default);
        // get the end of DST for this year, will return invalid object
        // if no DST applicable in this year. The default value of the
        // parameter means to take the current year.
    static wxDateTime GetEndDST(int year = Inv_Year,
                                Country country = Country_Default);

        // return the wxDateTime object for the current time
    static inline wxDateTime Now();

        // return the wxDateTime object for today midnight: i.e. as Now() but
        // with time set to 0
    static inline wxDateTime Today();



    // ------------------------------------------------------------------------
    // constructors

    wxDateTime();
    %name(wxDateTimeFromTimeT)wxDateTime(time_t timet);
    %name(wxDateTimeFromJDN)wxDateTime(double jdn);
    %name(wxDateTimeFromHMS)wxDateTime(wxDateTime_t hour,
                                       wxDateTime_t minute = 0,
                                       wxDateTime_t second = 0,
                                       wxDateTime_t millisec = 0);
    %name(wxDateTimeFromDMY)wxDateTime(wxDateTime_t day,
                                       Month        month = Inv_Month,
                                       int          year = Inv_Year,
                                       wxDateTime_t hour = 0,
                                       wxDateTime_t minute = 0,
                                       wxDateTime_t second = 0,
                                       wxDateTime_t millisec = 0);

    ~wxDateTime();

    // ------------------------------------------------------------------------
    // Set methods

    wxDateTime& SetToCurrent();

        // set to given time_t value
    %name(SetTimeT)wxDateTime& Set(time_t timet);

        // set to given JDN (beware of rounding errors)
    %name(SetJDN)wxDateTime& Set(double jdn);

        // set to given time, date = today
    %name(SetHMS)wxDateTime& Set(wxDateTime_t hour,
                    wxDateTime_t minute = 0,
                    wxDateTime_t second = 0,
                    wxDateTime_t millisec = 0);

        // from separate values for each component with explicit date
        // (defaults for month and year are the current values)
    wxDateTime& Set(wxDateTime_t day,
                    Month        month = Inv_Month,
                    int          year = Inv_Year, // 1999, not 99 please!
                    wxDateTime_t hour = 0,
                    wxDateTime_t minute = 0,
                    wxDateTime_t second = 0,
                    wxDateTime_t millisec = 0);

        // resets time to 00:00:00, doesn't change the date
    wxDateTime& ResetTime();

        // the following functions don't change the values of the other
        // fields, i.e. SetMinute() won't change either hour or seconds value

        // set the year
    wxDateTime& SetYear(int year);
        // set the month
    wxDateTime& SetMonth(Month month);
        // set the day of the month
    wxDateTime& SetDay(wxDateTime_t day);
        // set hour
    wxDateTime& SetHour(wxDateTime_t hour);
        // set minute
    wxDateTime& SetMinute(wxDateTime_t minute);
        // set second
    wxDateTime& SetSecond(wxDateTime_t second);
        // set millisecond
    wxDateTime& SetMillisecond(wxDateTime_t millisecond);


    // ------------------------------------------------------------------------
    // calendar calculations

        // set to the given week day in the same week as this one
    wxDateTime& SetToWeekDayInSameWeek(WeekDay weekday);
    wxDateTime GetWeekDayInSameWeek(WeekDay weekday);

        // set to the next week day following this one
    wxDateTime& SetToNextWeekDay(WeekDay weekday);
    wxDateTime GetNextWeekDay(WeekDay weekday);

        // set to the previous week day before this one
    wxDateTime& SetToPrevWeekDay(WeekDay weekday);
    wxDateTime GetPrevWeekDay(WeekDay weekday);

        // set to Nth occurence of given weekday in the given month of the
        // given year (time is set to 0), return TRUE on success and FALSE on
        // failure. n may be positive (1..5) or negative to count from the end
        // of the month (see helper function SetToLastWeekDay())
    bool SetToWeekDay(WeekDay weekday,
                      int n = 1,
                      Month month = Inv_Month,
                      int year = Inv_Year);
    wxDateTime GetWeekDay(WeekDay weekday,
                          int n = 1,
                          Month month = Inv_Month,
                          int year = Inv_Year);

        // sets to the last weekday in the given month, year
    bool SetToLastWeekDay(WeekDay weekday,
                          Month month = Inv_Month,
                          int year = Inv_Year);
    wxDateTime GetLastWeekDay(WeekDay weekday,
                              Month month = Inv_Month,
                              int year = Inv_Year);

        // sets the date to the given day of the given week in the year,
        // returns TRUE on success and FALSE if given date doesn't exist (e.g.
        // numWeek is > 53)
    bool SetToTheWeek(wxDateTime_t numWeek, WeekDay weekday = Mon);
    wxDateTime GetWeek(wxDateTime_t numWeek, WeekDay weekday = Mon);

        // sets the date to the last day of the given (or current) month or the
        // given (or current) year
    wxDateTime& SetToLastMonthDay(Month month = Inv_Month,
                                  int year = Inv_Year);
    wxDateTime GetLastMonthDay(Month month = Inv_Month,
                               int year = Inv_Year);

        // sets to the given year day (1..365 or 366)
    wxDateTime& SetToYearDay(wxDateTime_t yday);
    wxDateTime GetYearDay(wxDateTime_t yday);

        // The definitions below were taken verbatim from
        //
        //      http://www.capecod.net/~pbaum/date/date0.htm
        //
        // (Peter Baum's home page)
        //
        // definition: The Julian Day Number, Julian Day, or JD of a
        // particular instant of time is the number of days and fractions of a
        // day since 12 hours Universal Time (Greenwich mean noon) on January
        // 1 of the year -4712, where the year is given in the Julian
        // proleptic calendar. The idea of using this reference date was
        // originally proposed by Joseph Scalizer in 1582 to count years but
        // it was modified by 19th century astronomers to count days. One
        // could have equivalently defined the reference time to be noon of
        // November 24, -4713 if were understood that Gregorian calendar rules
        // were applied. Julian days are Julian Day Numbers and are not to be
        // confused with Julian dates.
        //
        // definition: The Rata Die number is a date specified as the number
        // of days relative to a base date of December 31 of the year 0. Thus
        // January 1 of the year 1 is Rata Die day 1.

        // get the Julian Day number (the fractional part specifies the time of
        // the day, related to noon - beware of rounding errors!)
    double GetJulianDayNumber();
    double GetJDN();

        // get the Modified Julian Day number: it is equal to JDN - 2400000.5
        // and so integral MJDs correspond to the midnights (and not noons).
        // MJD 0 is Nov 17, 1858
    double GetModifiedJulianDayNumber() const { return GetJDN() - 2400000.5; }
    double GetMJD();

        // get the Rata Die number
    double GetRataDie();


    // ------------------------------------------------------------------------
    // timezone stuff

        // transform to any given timezone
    wxDateTime ToTimezone(const wxDateTime::TimeZone& tz, bool noDST = FALSE);
    wxDateTime& MakeTimezone(const wxDateTime::TimeZone& tz, bool noDST = FALSE);

        // transform to GMT/UTC
    wxDateTime ToGMT(bool noDST = FALSE);
    wxDateTime& MakeGMT(bool noDST = FALSE);

        // is daylight savings time in effect at this moment according to the
        // rules of the specified country?
        //
        // Return value is > 0 if DST is in effect, 0 if it is not and -1 if
        // the information is not available (this is compatible with ANSI C)
    int IsDST(Country country = Country_Default);



    // ------------------------------------------------------------------------
    // accessors

        // is the date valid (TRUE even for non initialized objects)?
    inline bool IsValid() const;

        // get the number of seconds since the Unix epoch - returns (time_t)-1
        // if the value is out of range
    inline time_t GetTicks() const;

        // get the year (returns Inv_Year if date is invalid)
    int GetYear(const wxDateTime::TimeZone& tz = LOCAL) const;

        // get the month (Inv_Month if date is invalid)
    Month GetMonth(const wxDateTime::TimeZone& tz = LOCAL) const;

        // get the month day (in 1..31 range, 0 if date is invalid)
    wxDateTime_t GetDay(const wxDateTime::TimeZone& tz = LOCAL) const;

        // get the day of the week (Inv_WeekDay if date is invalid)
    WeekDay GetWeekDay(const wxDateTime::TimeZone& tz = LOCAL) const;

        // get the hour of the day
    wxDateTime_t GetHour(const wxDateTime::TimeZone& tz = LOCAL) const;

        // get the minute
    wxDateTime_t GetMinute(const wxDateTime::TimeZone& tz = LOCAL) const;

        // get the second
    wxDateTime_t GetSecond(const wxDateTime::TimeZone& tz = LOCAL) const;

        // get milliseconds
    wxDateTime_t GetMillisecond(const wxDateTime::TimeZone& tz = LOCAL) const;


        // get the day since the year start (1..366, 0 if date is invalid)
    wxDateTime_t GetDayOfYear(const wxDateTime::TimeZone& tz = LOCAL) const;
        // get the week number since the year start (1..52 or 53, 0 if date is
        // invalid)
    wxDateTime_t GetWeekOfYear(WeekFlags flags = Monday_First,
                               const wxDateTime::TimeZone& tz = LOCAL) const;
        // get the week number since the month start (1..5, 0 if date is
        // invalid)
    wxDateTime_t GetWeekOfMonth(WeekFlags flags = Monday_First,
                                const wxDateTime::TimeZone& tz = LOCAL) const;

        // is this date a work day? This depends on a country, of course,
        // because the holidays are different in different countries
    bool IsWorkDay(Country country = Country_Default) const;

        // is this date later than Gregorian calendar introduction for the
        // given country (see enum GregorianAdoption)?
        //
        // NB: this function shouldn't be considered as absolute authority in
        //     the matter. Besides, for some countries the exact date of
        //     adoption of the Gregorian calendar is simply unknown.
    //bool IsGregorianDate(GregorianAdoption country = Gr_Standard) const;


    // ------------------------------------------------------------------------
    // comparison (see also functions below for operator versions)

        // returns TRUE if the two moments are strictly identical
    inline bool IsEqualTo(const wxDateTime& datetime) const;

        // returns TRUE if the date is strictly earlier than the given one
    inline bool IsEarlierThan(const wxDateTime& datetime) const;

        // returns TRUE if the date is strictly later than the given one
    inline bool IsLaterThan(const wxDateTime& datetime) const;

        // returns TRUE if the date is strictly in the given range
    inline bool IsStrictlyBetween(const wxDateTime& t1,
                                  const wxDateTime& t2) const;

        // returns TRUE if the date is in the given range
    inline bool IsBetween(const wxDateTime& t1, const wxDateTime& t2) const;

        // do these two objects refer to the same date?
    inline bool IsSameDate(const wxDateTime& dt) const;

        // do these two objects have the same time?
    inline bool IsSameTime(const wxDateTime& dt) const;

        // are these two objects equal up to given timespan?
    inline bool IsEqualUpTo(const wxDateTime& dt, const wxTimeSpan& ts) const;


    // ------------------------------------------------------------------------
    // arithmetics with dates (see also below for more operators)

        // add a time span (positive or negative)
    %name(AddTS) wxDateTime& Add(const wxTimeSpan& diff);
        // add a date span (positive or negative)
    %name(AddDS) wxDateTime& Add(const wxDateSpan& diff);

        // subtract a time span (positive or negative)
    %name(SubtractTS) wxDateTime& Subtract(const wxTimeSpan& diff);

        // subtract a date span (positive or negative)
    %name(SubtractDS) wxDateTime& Subtract(const wxDateSpan& diff);

        // return the difference between two dates
    wxTimeSpan Subtract(const wxDateTime& dt) const;


    %addmethods {
        wxDateTime __add__TS(const wxTimeSpan& other) { return *self + other; }
        wxDateTime __add__DS(const wxDateSpan& other) { return *self + other; }

        wxTimeSpan __sub__DT(const wxDateTime& other) { return *self - other; }
        wxDateTime __sub__TS(const wxTimeSpan& other) { return *self - other; }
        wxDateTime __sub__DS(const wxDateSpan& other) { return *self - other; }

        int __cmp__(const wxDateTime& other) {
            if (*self <  other) return -1;
            if (*self == other) return 0;
            return 1;
        }
    }

    %pragma(python) addtoclass = "
    def __add__(self, other):
        if string.find(other.this, 'wxTimeSpan') != -1:
            return self.__add__TS(other)
        if string.find(other.this, 'wxDateSpan') != -1:
            return self.__add__DS(other)
        raise TypeError, 'Invalid r.h.s. type for __add__'
    def __sub__(self, other):
        if string.find(other.this, 'wxDateTime') != -1:
            return self.__sub__DT(other)
        if string.find(other.this, 'wxTimeSpan') != -1:
            return self.__sub__TS(other)
        if string.find(other.this, 'wxDateSpan') != -1:
            return self.__sub__DS(other)
        raise TypeError, 'Invalid r.h.s. type for __sub__'
"

    // ------------------------------------------------------------------------
    // conversion to/from text: all conversions from text return the pointer to
    // the next character following the date specification (i.e. the one where
    // the scan had to stop) or NULL on failure.

        // parse a string in RFC 822 format (found e.g. in mail headers and
        // having the form "Wed, 10 Feb 1999 19:07:07 +0100")
    const char *ParseRfc822Date(const char* date);

        // parse a date/time in the given format (see strptime(3)), fill in
        // the missing (in the string) fields with the values of dateDef (by
        // default, they will not change if they had valid values or will
        // default to Today() otherwise)
    const char *ParseFormat(const char *date,
                              const char *format = "%c",
                              const wxDateTime& dateDef = wxDefaultDateTime);

        // parse a string containing the date/time in "free" format, this
        // function will try to make an educated guess at the string contents
    const char *ParseDateTime(const char *datetime);

        // parse a string containing the date only in "free" format (less
        // flexible than ParseDateTime)
    const char *ParseDate(const char *date);

        // parse a string containing the time only in "free" format
    const char *ParseTime(const char *time);

        // this function accepts strftime()-like format string (default
        // argument corresponds to the preferred date and time representation
        // for the current locale) and returns the string containing the
        // resulting text representation
    wxString Format(const char *format = "%c",
                    const wxDateTime::TimeZone& tz = LOCAL) const;

        // preferred date representation for the current locale
    wxString FormatDate() const;

        // preferred time representation for the current locale
    wxString FormatTime() const;

        // returns the string representing the date in ISO 8601 format
        // (YYYY-MM-DD)
    wxString FormatISODate() const;

        // returns the string representing the time in ISO 8601 format
        // (HH:MM:SS)
    wxString FormatISOTime() const;

    %pragma(python) addtoclass = "
    def __repr__(self):
        return '<wxDateTime: \"%s\" at %s>' % ( self.Format(), self.this)
    def __str__(self):
        return self.Format()
"

};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


class wxTimeSpan
{
public:
        // return the timespan for the given number of seconds
    static wxTimeSpan Seconds(long sec);
    static wxTimeSpan Second();

        // return the timespan for the given number of minutes
    static wxTimeSpan Minutes(long min);
    static wxTimeSpan Minute();

        // return the timespan for the given number of hours
    static wxTimeSpan Hours(long hours);
    static wxTimeSpan Hour();

        // return the timespan for the given number of days
    static wxTimeSpan Days(long days);
    static wxTimeSpan Day();

        // return the timespan for the given number of weeks
    static wxTimeSpan Weeks(long days);
    static wxTimeSpan Week();

    // ------------------------------------------------------------------------
    // constructors

        // from separate values for each component, date set to 0 (hours are
        // not restricted to 0..24 range, neither are minutes, seconds or
        // milliseconds)
    wxTimeSpan(long hours = 0,
               long minutes = 0,
               long seconds = 0,
               long milliseconds = 0);

    ~wxTimeSpan();

    // ------------------------------------------------------------------------
    // arithmetics with time spans

        // add two timespans together
    inline wxTimeSpan& Add(const wxTimeSpan& diff);

        // subtract another timespan
    inline wxTimeSpan& Subtract(const wxTimeSpan& diff);

        // multiply timespan by a scalar
    inline wxTimeSpan& Multiply(int n);

        // negate the value of the timespan
    wxTimeSpan& Neg();

        // return the absolute value of the timespan: does _not_ modify the
        // object
    inline wxTimeSpan Abs() const;

    %addmethods {
        wxTimeSpan __add__(const wxTimeSpan& other) { return *self + other; }
        wxTimeSpan __sub__(const wxTimeSpan& other) { return *self - other; }
        wxTimeSpan __mul__(int n)                   { return *self * n; }
        wxTimeSpan __rmul__(int n)                  { return n * *self; }
        wxTimeSpan __neg__()                        { return self->Negate(); }
        int __cmp__(const wxTimeSpan& other) {
            if (*self <  other) return -1;
            if (*self == other) return 0;
            return 1;
        }
    }

    // comparaison (see also operator versions below)
    // ------------------------------------------------------------------------

        // is the timespan null?
    bool IsNull() const;

        // is the timespan positive?
    bool IsPositive() const;

        // is the timespan negative?
    bool IsNegative() const;

        // are two timespans equal?
    inline bool IsEqualTo(const wxTimeSpan& ts) const;

        // compare two timestamps: works with the absolute values, i.e. -2
        // hours is longer than 1 hour. Also, it will return FALSE if the
        // timespans are equal in absolute value.
    inline bool IsLongerThan(const wxTimeSpan& ts) const;

        // compare two timestamps: works with the absolute values, i.e. 1
        // hour is shorter than -2 hours. Also, it will return FALSE if the
        // timespans are equal in absolute value.
    bool IsShorterThan(const wxTimeSpan& t) const;

    // ------------------------------------------------------------------------
    // breaking into days, hours, minutes and seconds

        // get the max number of weeks in this timespan
    inline int GetWeeks() const;
        // get the max number of days in this timespan
    inline int GetDays() const;
        // get the max number of hours in this timespan
    inline int GetHours() const;
        // get the max number of minutes in this timespan
    inline int GetMinutes() const;


         // get the max number of seconds in this timespan
    inline wxLongLong GetSeconds() const;
         // get the number of milliseconds in this timespan
    wxLongLong GetMilliseconds() const;

    // ------------------------------------------------------------------------
    // conversion to text

        // this function accepts strftime()-like format string (default
        // argument corresponds to the preferred date and time representation
        // for the current locale) and returns the string containing the
        // resulting text representation. Notice that only some of format
        // specifiers valid for wxDateTime are valid for wxTimeSpan: hours,
        // minutes and seconds make sense, but not "PM/AM" string for example.
    wxString Format(const char *format = "%c") const;

//          // preferred date representation for the current locale
//      wxString FormatDate() const;

//          // preferred time representation for the current locale
//      wxString FormatTime() const;

//      %pragma(python) addtoclass = "
//      def __repr__(self):
//          return '<wxTimeSpan: \"%s\" at %s>' % ( self.Format(), self.this)
//      def __str__(self):
//          return self.Format()
//  "
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class wxDateSpan
{
public:
        // this many years/months/weeks/days
    wxDateSpan(int years = 0, int months = 0, int weeks = 0, int days = 0)
    {
        m_years = years;
        m_months = months;
        m_weeks = weeks;
        m_days = days;
    }

    ~wxDateSpan();

        // get an object for the given number of days
    static wxDateSpan Days(int days);
    static wxDateSpan Day();

        // get an object for the given number of weeks
    static wxDateSpan Weeks(int weeks);
    static wxDateSpan Week();

        // get an object for the given number of months
    static wxDateSpan Months(int mon);
    static wxDateSpan Month();

        // get an object for the given number of years
    static wxDateSpan Years(int years);
    static wxDateSpan Year();


    // ------------------------------------------------------------------------

        // set number of years
    wxDateSpan& SetYears(int n);
        // set number of months
    wxDateSpan& SetMonths(int n);
        // set number of weeks
    wxDateSpan& SetWeeks(int n);
        // set number of days
    wxDateSpan& SetDays(int n);

        // get number of years
    int GetYears() const;
        // get number of months
    int GetMonths() const;
        // get number of weeks
    int GetWeeks() const;
        // get number of days
    int GetDays() const;
        // returns 7*GetWeeks() + GetDays()
    int GetTotalDays() const;


    // ------------------------------------------------------------------------

        // add another wxDateSpan to us
    inline wxDateSpan& Add(const wxDateSpan& other);

        // subtract another wxDateSpan from us
    inline wxDateSpan& Subtract(const wxDateSpan& other);

        // inverse the sign of this timespan
    inline wxDateSpan& Neg();

        // multiply all components by a (signed) number
    inline wxDateSpan& Multiply(int factor);

    %addmethods {
        wxDateSpan __add__(const wxDateSpan& other) { return *self + other; }
        wxDateSpan __sub__(const wxDateSpan& other) { return *self - other; }
        wxDateSpan __mul__(int n)                   { return *self * n; }
        wxDateSpan __rmul__(int n)                  { return n * *self; }
        wxDateSpan __neg__()                        { return self->Negate(); }
    }
};


//---------------------------------------------------------------------------

long wxGetLocalTime();
long wxGetUTCTime();
long wxGetCurrentTime();
wxLongLong wxGetLocalTimeMillis();

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

%init %{

//    wxClassInfo::CleanUpClasses();
//    wxClassInfo::InitializeClasses();

%}

//---------------------------------------------------------------------------

