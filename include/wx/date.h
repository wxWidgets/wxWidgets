/////////////////////////////////////////////////////////////////////////////
// Name:        date.h
// Purpose:     wxDate class
// Author:      Julian Smart, Steve Marcus, Eric Simon, Chris Hill,
//              Charles D. Price
// Modified by:	
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATE_H_
#define _WX_DATE_H_

#ifdef __GNUG__
#pragma interface "date.h"
#endif

#include "wx/object.h"
#include "wx/string.h"

#ifdef wxUSE_TIMEDATE

enum wxdate_format_type {wxMDY, wxDAY, wxMONTH, wxFULL, wxEUROPEAN};

#define wxNO_CENTURY  0x02
#define wxDATE_ABBR   0x04

class WXDLLEXPORT wxDate: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxDate)
 protected:
   unsigned long julian; // see julDate();  days since 1/1/4713 B.C.
   int month;            // see NMonth()
   int day;              // see Day()
   int year;             // see NYear4()
   int day_of_week;      // see NDOW();  1 = Sunday, ... 7 = Saturday

 private:
  int  		 DisplayFormat;
  unsigned char DisplayOptions;

  void julian_to_mdy ();         // convert julian day to mdy
  void julian_to_wday ();        // convert julian day to day_of_week
  void mdy_to_julian ();         // convert mdy to julian day

 public:
  wxDate ();
  wxDate (long j);
  wxDate (int m, int d, int y);
  wxDate (const wxString& dat);
  wxDate (const wxDate &dt);

  operator wxString  (void);
  void operator =   (const wxDate& date);
  void operator =   (const wxString& date);

  wxDate operator +  (long i);
  wxDate operator +  (int  i);

  wxDate operator -  (long i);
  wxDate operator -  (int  i);

  long operator -  (const wxDate &dt);

  wxDate &operator += (long i);
  wxDate &operator -= (long i);

  wxDate &operator ++ ();     // Prefix increment
  wxDate &operator ++ (int);  // Postfix increment
  wxDate &operator -- ();     // Prefix decrement
  wxDate &operator -- (int);  // Postfix decrement

  friend bool operator <  (const wxDate &dt1, const wxDate &dt2);
  friend bool operator <= (const wxDate &dt1, const wxDate &dt2);
  friend bool operator >  (const wxDate &dt1, const wxDate &dt2);
  friend bool operator >= (const wxDate &dt1, const wxDate &dt2);
  friend bool operator == (const wxDate &dt1, const wxDate &dt2);
  friend bool operator != (const wxDate &dt1, const wxDate &dt2);

  friend ostream &operator << (ostream &os, const wxDate &dt);

  wxString FormatDate 	   (int type=-1) const;
  void  SetFormat (int format);
  int   SetOption (int option, bool enable=TRUE);

  long  GetJulianDate() const;  // returns julian date
  int   GetDayOfYear()  const;  // returns relative date since Jan. 1
  bool  IsLeapYear()    const;  // returns TRUE if leap year, FALSE if not

  // Version 4.0 Extension to Public Interface - CDP
  
  // These 'Set's modify the date object and actually SET it
  // They all return a reference to self (*this)

  wxDate &Set();            // Sets to current system date
  wxDate &Set(long lJulian);
  wxDate &Set(int nMonth, int nDay, int nYear);

  wxDate &AddWeeks(int nCount = 1);  // 
  wxDate &AddMonths(int nCount = 1); // May also pass neg# to decrement
  wxDate &AddYears(int nCount = 1);  //

  int   GetDay() const;      // Numeric Day of date object
  int   GetDaysInMonth();    // Number of days in month (1..31)
  int   GetFirstDayOfMonth() const; // First Day Of Month  (1..7)

  wxString GetDayOfWeekName();       // Character Day Of Week ('Sunday'..'Saturday')
  int   GetDayOfWeek() const;     // (1..7)

  int   GetWeekOfMonth();            // Numeric Week Of Month  (1..6)
  int   GetWeekOfYear();            // Numeric Week Of Year   (1..52)

  wxString GetMonthName();            // Character Month name
  int   GetMonth() const;          // Month Number (1..12)
  wxDate  GetMonthStart();         // First Date Of Month
  wxDate  GetMonthEnd();           // Last Date Of Month

  int   GetYear() const;           // eg. 1992
  wxDate  GetYearStart();          // First Date Of Year
  wxDate  GetYearEnd();            // Last Date Of Year

  bool IsBetween(const wxDate& first, const wxDate& second) const;

  wxDate Previous(int dayOfWeek) const;
};

#endif
#endif
    // _WX_DATE_H_
