/////////////////////////////////////////////////////////////////////////////
// Name:        time.h
// Purpose:     wxTime class, from NIHCL
// Author:      Julian Smart, after K. E. Gorlen
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMEH__
#define _WX_TIMEH__

#include "wx/object.h"

#if wxUSE_TIMEDATE

#ifdef __GNUG__
#pragma interface "time.h"
#endif

class WXDLLEXPORT wxDate;

typedef unsigned short hourTy;
typedef unsigned short minuteTy;
typedef unsigned short secondTy;
typedef unsigned long clockTy;

class WXDLLEXPORT wxTime: public wxObject
{
        DECLARE_DYNAMIC_CLASS(wxTime)
        
public:                 // type definitions
    enum tFormat { wx12h, wx24h };
    enum tPrecision { wxStdMinSec, wxStdMin };
private:
    static tFormat      Format;
    static tPrecision   Precision;

    clockTy    sec;        /* seconds since 1/1/1901 */

    bool IsDST() const;
    wxTime GetLocalTime() const;
private:        // static member functions
    static wxTime GetLocalTime(const wxDate& date, hourTy h=0, minuteTy m=0, secondTy s=0);
    static wxTime GetBeginDST(unsigned year);
    static wxTime GetEndDST(unsigned year);
public:
    wxTime();             // current time 
    wxTime(clockTy s)         { sec = s; }
    void operator=(const wxTime& t)     { sec = t.sec; } // Ordering required for some compilers
    wxTime(const wxTime& t)         { (*this) = t ; }
    wxTime(hourTy h, minuteTy m, secondTy s =0, bool dst =FALSE);
    wxTime(const wxDate&, hourTy h =0, minuteTy m =0, secondTy s=0, bool dst =FALSE);

    // Convert to string
#ifndef __SALFORDC__
    operator wxChar * (void);
    operator wxDate() const;
#endif

    bool operator<(const wxTime& t) const     { return sec < t.sec; }
    bool operator<=(const wxTime& t) const    { return sec <= t.sec; }
    bool operator>(const wxTime& t) const     { return sec > t.sec; }
    bool operator>=(const wxTime& t) const    { return sec >= t.sec; }
    bool operator==(const wxTime& t) const    { return sec == t.sec; }
    bool operator!=(const wxTime& t) const    { return sec != t.sec; }
    friend wxTime operator+(const wxTime& t, long s)    { return wxTime(t.sec+s); }
    friend wxTime operator+(long s, const wxTime& t)    { return wxTime(t.sec+s); }
    long operator-(const wxTime& t) const     { return sec - t.sec; }
    wxTime operator-(long s) const    { return wxTime(sec-s); }
    void operator+=(long s)     { sec += s; }
    void operator-=(long s)     { sec -= s; }
    bool IsBetween(const wxTime& a, const wxTime& b) const;

    /// Get day
    int GetDay() const;
    /// Get month
    int GetMonth() const;
    /// Get year
    int GetYear() const;
    /// Get day of week (0=Sunday 6=Saturday)
    int GetDayOfWeek() const;

    hourTy GetHour() const;        // hour in local time
    hourTy GetHourGMT() const;     // hour in GMT
    minuteTy GetMinute() const;    // minute in local time
    minuteTy GetMinuteGMT() const;     // minute in GMT
    secondTy GetSecond() const;    // second in local time or GMT
    clockTy GetSeconds() const     { return sec; }
    secondTy GetSecondGMT() const ;
    wxTime Max(const wxTime&) const;
    wxTime Min(const wxTime&) const;
    static void SetFormat(const tFormat lFormat = wx12h,
                  const tPrecision lPrecision = wxStdMinSec);
    wxChar *FormatTime() const;
/*
    virtual int compare(const Object&) const;
    virtual void deepenShallowCopy();       // {}
    virtual unsigned hash() const;
    virtual bool isEqual(const Object&) const;
    virtual void printOn(ostream& strm =cout) const;
    virtual const Class* species() const;
*/
};

#endif
  // wxUSE_TIMEDATE
#endif
    // _WX_TIMEH__

