/////////////////////////////////////////////////////////////////////////////
// Name:        typetest.cpp
// Purpose:     Types wxWindows sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "typetest.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/time.h"
#include "wx/date.h"
#include "wx/variant.h"

#include "typetest.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "mondrian.xpm"
#endif

#include "wx/ioswrap.h"

#if wxUSE_IOSTREAMH
    #include <fstream.h>
#else
    #include <fstream>
#endif

#include "wx/wfstream.h"


// Create a new application object
IMPLEMENT_APP	(MyApp)

IMPLEMENT_DYNAMIC_CLASS	(MyApp, wxApp)

BEGIN_EVENT_TABLE(MyApp, wxApp)
	EVT_MENU(TYPES_DATE,      MyApp::DoDateDemo)
	EVT_MENU(TYPES_TIME,      MyApp::DoTimeDemo)
	EVT_MENU(TYPES_VARIANT,   MyApp::DoVariantDemo)
	EVT_MENU(TYPES_BYTEORDER, MyApp::DoByteOrderDemo)
#if wxUSE_UNICODE
	EVT_MENU(TYPES_UNICODE,   MyApp::DoUnicodeDemo)
#endif
	EVT_MENU(TYPES_STREAM, MyApp::DoStreamDemo)
END_EVENT_TABLE()

bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame((wxFrame *) NULL, "wxWindows Types Demo",
      wxPoint(50, 50), wxSize(450, 340));

  // Give it an icon
  frame->SetIcon(wxICON(mondrian));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(TYPES_ABOUT, "&About");
  file_menu->AppendSeparator();
  file_menu->Append(TYPES_DATE, "&Date test");
  file_menu->Append(TYPES_TIME, "&Time test");
  file_menu->Append(TYPES_VARIANT, "&Variant test");
  file_menu->Append(TYPES_BYTEORDER, "&Byteorder test");
#if wxUSE_UNICODE
  file_menu->Append(TYPES_UNICODE, "&Unicode test");
#endif
  file_menu->Append(TYPES_STREAM, "&Stream test");
  file_menu->AppendSeparator();
  file_menu->Append(TYPES_QUIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  frame->SetMenuBar(menu_bar);

  m_textCtrl = new wxTextCtrl(frame, -1, "", wxPoint(0, 0), wxDefaultSize, wxTE_MULTILINE);

  // Show the frame
  frame->Show(TRUE);
  
  SetTopWindow(frame);

  return TRUE;
}

void MyApp::DoStreamDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();
    
    textCtrl.Clear();
    textCtrl << "\nTest fstream vs. wxFileStream:\n\n";

    textCtrl.WriteText( "Writing to ofstream and wxFileOutputStream:\n" );
    
    ofstream std_file_output( "test_std.dat" );
    wxFileOutputStream file_output( "test_wx.dat" );

    wxString tmp;
    signed int si = 0xFFFFFFFF;
    tmp.Printf( "Signed int: %d\n", si );
    textCtrl.WriteText( tmp );
    file_output << si << "\n";
    std_file_output << si << "\n";
    
    unsigned int ui = 0xFFFFFFFF;
    tmp.Printf( "Unsigned int: %u\n", ui );
    textCtrl.WriteText( tmp );
    file_output << ui << "\n";
    std_file_output << ui << "\n";
    
    double d = 2.01234567890123456789;
    tmp.Printf( "Double: %f\n", d );
    textCtrl.WriteText( tmp );
    file_output << d << "\n";
    std_file_output << d << "\n";
    
    float f = 0.00001;
    tmp.Printf( "Float: %f\n", f );
    textCtrl.WriteText( tmp );
    file_output << f << "\n";
    std_file_output << f << "\n";
    
    wxString str( "Hello!" );
    tmp.Printf( "String: %s\n", str.c_str() );
    textCtrl.WriteText( tmp );
    file_output << str << "\n";
    std_file_output << str.c_str() << "\n";
    
    textCtrl.WriteText( "\nReading from ifstream:\n" );
    
    ifstream std_file_input( "test_std.dat" );

    std_file_input >> si;
    tmp.Printf( "Signed int: %d\n", si );
    textCtrl.WriteText( tmp );
    
    std_file_input >> ui;
    tmp.Printf( "Unsigned int: %u\n", ui );
    textCtrl.WriteText( tmp );
    
    std_file_input >> d;
    tmp.Printf( "Double: %f\n", d );
    textCtrl.WriteText( tmp );
    
    std_file_input >> f;
    tmp.Printf( "Float: %f\n", f );
    textCtrl.WriteText( tmp );
    
    std_file_input >> str;
    tmp.Printf( "String: %s\n", str.c_str() );
    textCtrl.WriteText( tmp );
    
    textCtrl.WriteText( "\nReading from wxFileInputStream:\n" );

    file_output.OutputStreamBuffer()->FlushBuffer();
    
    wxFileInputStream file_input( "test_wx.dat" );
    
    file_input >> si;
    tmp.Printf( "Signed int: %d\n", si );
    textCtrl.WriteText( tmp );
    
    file_input >> ui;
    tmp.Printf( "Unsigned int: %u\n", ui );
    textCtrl.WriteText( tmp );
    
    file_input >> d;
    tmp.Printf( "Double: %f\n", d );
    textCtrl.WriteText( tmp );
    
    file_input >> f;
    tmp.Printf( "Float: %f\n", f );
    textCtrl.WriteText( tmp );
    
    file_input >> str;
    tmp.Printf( "String: %s\n", str.c_str() );
    textCtrl.WriteText( tmp );
}

#if wxUSE_UNICODE
void MyApp::DoUnicodeDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();
    
    textCtrl.Clear();
    textCtrl << "\nTest wchar_t to char (Unicode to ANSI/Multibyte) converions:";

    wxString str;
    str = _T("Robert Röbling\n");
    
    printf( "\n\nConversion with wxConvLocal:\n" );
    wxConvCurrent = &wxConvLocal;
    printf( (const char*) str.mbc_str() );
    
    printf( "\n\nConversion with wxConvGdk:\n" );
    wxConvCurrent = &wxConvGdk;
    printf( (const char*) str.mbc_str() );
    
    printf( "\n\nConversion with wxConvLibc:\n" );
    wxConvCurrent = &wxConvLibc;
    printf( (const char*) str.mbc_str() );
    
}
#endif

void MyApp::DoByteOrderDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << "\nTest byte order macros:\n\n";
    
    if (wxBYTE_ORDER == wxLITTLE_ENDIAN)
        textCtrl << "This is a little endian system.\n\n";
    else    
        textCtrl << "This is a big endian system.\n\n";
	
    wxString text;
    
    wxInt32 var = 0xF1F2F3F4;
    text = "";
    text.Printf( _T("Value of wxInt32 is now: %#x.\n\n"), var );
    textCtrl.WriteText( text );
    
    text = "";
    text.Printf( _T("Value of swapped wxInt32 is: %#x.\n\n"), wxINT32_SWAP_ALWAYS( var ) );
    textCtrl.WriteText( text );
    
    text = "";
    text.Printf( _T("Value of wxInt32 swapped on little endian is: %#x.\n\n"), wxINT32_SWAP_ON_LE( var ) );
    textCtrl.WriteText( text );
    
    text = "";
    text.Printf( _T("Value of wxInt32 swapped on big endian is: %#x.\n\n"), wxINT32_SWAP_ON_BE( var ) );
    textCtrl.WriteText( text );
}

void MyApp::DoTimeDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << "\nTest class wxTime:\n";
    wxTime now;
    textCtrl << "It is now " << (wxString) now << "\n";
}

void MyApp::DoDateDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << "\nTest class wxDate" << "\n";

    // Various versions of the constructors
    // and various output

    wxDate x(10,20,1962);

    textCtrl << x.FormatDate(wxFULL) << "  (full)\n";

    // constuctor with a string, just printing the day of the week
    wxDate y("8/8/1988");

    textCtrl << y.FormatDate(wxDAY) << "  (just day)\n";

    // constructor with a julian
    wxDate z( 2450000L );
    textCtrl << z.FormatDate(wxFULL) << "  (full)\n";

    // using date addition and subtraction
    wxDate a = x + 10;
    textCtrl << a.FormatDate(wxFULL) << "  (full)\n";
    a = a - 25;
    textCtrl << a.FormatDate(wxEUROPEAN) << "  (European)\n";

    
    // Using subtraction of two date objects
    wxDate a1 = wxString("7/13/1991");
    wxDate a2 = a1 + 14;
    textCtrl << (a1-a2) << "\n";
    textCtrl << (a2+=10) << "\n";

    a1++;
    textCtrl << "Tomorrow= " << a1.FormatDate(wxFULL) << "\n";

    wxDate tmpDate1("08/01/1991");
    wxDate tmpDate2("07/14/1991");
    textCtrl << "a1 (7-14-91) < 8-01-91 ? ==> " << ((a1 < tmpDate1) ? "TRUE" : "FALSE") << "\n";
    textCtrl << "a1 (7-14-91) > 8-01-91 ? ==> " << ((a1 > tmpDate1) ? "TRUE" : "FALSE") << "\n";
    textCtrl << "a1 (7-14-91)== 7-14-91 ? ==> " << ((a1==tmpDate2) ? "TRUE" : "FALSE") << "\n";

    wxDate a3 = a1;
    textCtrl << "a1 (7-14-91)== a3 (7-14-91) ? ==> " << ((a1==a3) ? "TRUE" : "FALSE") << "\n";
    wxDate a4 = a1;
    textCtrl << "a1 (7-14-91)== a4 (7-15-91) ? ==> " << ((a1==++a4) ? "TRUE" : "FALSE") << "\n";

    wxDate a5 = wxString("today");
    textCtrl << "Today is: " << a5 << "\n";
    a4 = "TODAY";
    textCtrl << "Today (a4) is: " << a4 << "\n";

    textCtrl << "Today + 4 is: " << (a4+=4) << "\n";
    a4 = "TODAY";
    textCtrl << "Today - 4 is: " << (a4-=4) << "\n";

    textCtrl << "=========== Leap Year Test ===========\n";
    a1 = "1/15/1992";
    textCtrl << a1.FormatDate(wxFULL) << "  " << ((a1.IsLeapYear()) ? "Leap" : "non-Leap");
    textCtrl << "  " << "day of year:  " << a1.GetDayOfYear() << "\n";

    a1 = "2/16/1993";
    textCtrl << a1.FormatDate(wxFULL) << "  " << ((a1.IsLeapYear()) ? "Leap" : "non-Leap");
    textCtrl << "  " << "day of year:  " << a1.GetDayOfYear() << "\n";

    textCtrl << "================== string assignment test ====================\n";
    wxString date_string=a1;
    textCtrl << "a1 as a string (s/b 2/16/1993) ==> " << date_string << "\n";

    textCtrl << "================== SetFormat test ============================\n";
    a1.SetFormat(wxFULL);
    textCtrl << "a1 (s/b FULL format) ==> " << a1 << "\n";
    a1.SetFormat(wxEUROPEAN);
    textCtrl << "a1 (s/b EUROPEAN format) ==> " << a1 << "\n";

    textCtrl << "================== SetOption test ============================\n";
    textCtrl << "Date abbreviation ON\n";

    a1.SetOption(wxDATE_ABBR);
    a1.SetFormat(wxMONTH);
    textCtrl << "a1 (s/b MONTH format) ==> " << a1 << "\n";
    a1.SetFormat(wxDAY);
    textCtrl << "a1 (s/b DAY format) ==> " << a1 << "\n";
    a1.SetFormat(wxFULL);
    textCtrl << "a1 (s/b FULL format) ==> " << a1 << "\n";
    a1.SetFormat(wxEUROPEAN);
    textCtrl << "a1 (s/b EUROPEAN format) ==> " << a1 << "\n";
    textCtrl << "Century suppression ON\n";
    a1.SetOption(wxNO_CENTURY);
    a1.SetFormat(wxMDY);
    textCtrl << "a1 (s/b MDY format) ==> " << a1 << "\n";
    textCtrl << "Century suppression OFF\n";
    a1.SetOption(wxNO_CENTURY,FALSE);
    textCtrl << "a1 (s/b MDY format) ==> " << a1 << "\n";
    textCtrl << "Century suppression ON\n";
    a1.SetOption(wxNO_CENTURY);
    textCtrl << "a1 (s/b MDY format) ==> " << a1 << "\n";
    a1.SetFormat(wxFULL);
    textCtrl << "a1 (s/b FULL format) ==> " << a1 << "\n";

    textCtrl << "\n=============== Version 4.0 Enhancement Test =================\n";

    wxDate v4("11/26/1966");
    textCtrl << "\n---------- Set Stuff -----------\n";
    textCtrl << "First, 'Set' to today..." << "\n";
    textCtrl << "Before 'Set' => " << v4 << "\n";
    textCtrl << "After  'Set' => " << v4.Set() << "\n\n";

    textCtrl << "Set to 11/26/66 => " << v4.Set(11,26,1966) << "\n";
    textCtrl << "Current Julian  => " << v4.GetJulianDate() << "\n";
    textCtrl << "Set to Julian 2450000L => " << v4.Set(2450000L) << "\n";
    textCtrl << "See! => " << v4.GetJulianDate() << "\n";

    textCtrl << "---------- Add Stuff -----------\n";
    textCtrl << "Start => " << v4 << "\n";
    textCtrl << "Add 4 Weeks => " << v4.AddWeeks(4) << "\n";
    textCtrl << "Sub 1 Month => " << v4.AddMonths(-1) << "\n";
    textCtrl << "Add 2 Years => " << v4.AddYears(2) << "\n";

    textCtrl << "---------- Misc Stuff -----------\n";
    textCtrl << "The date aboves' day of the month is => " << v4.GetDay() << "\n";
    textCtrl << "There are " << v4.GetDaysInMonth() << " days in this month.\n";
    textCtrl << "The first day of this month lands on " << v4.GetFirstDayOfMonth() << "\n";
    textCtrl << "This day happens to be " << v4.GetDayOfWeekName() << "\n";
    textCtrl << "the " << v4.GetDayOfWeek() << " day of the week," << "\n";
    textCtrl << "on the " << v4.GetWeekOfYear() << " week of the year," << "\n";
    textCtrl << "on the " << v4.GetWeekOfMonth() << " week of the month, " << "\n";
    textCtrl << "(which is " << v4.GetMonthName() << ")\n";
    textCtrl << "the "<< v4.GetMonth() << "th month in the year.\n";
    textCtrl << "The year alone is " << v4.GetYear() << "\n";

    textCtrl << "---------- First and Last Stuff -----------\n";
    v4.Set();
    textCtrl << "The first date of this month is " << v4.GetMonthStart() << "\n";
    textCtrl << "The last date of this month is " << v4.GetMonthEnd() << "\n";
    textCtrl << "The first date of this year is " << v4.GetYearStart() << "\n";
    textCtrl << "The last date of this year is " << v4.GetYearEnd() << "\n";
}

void MyApp::DoVariantDemo(wxCommandEvent& WXUNUSED(event) )
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    wxVariant var1 = "String value";
    textCtrl << "var1 = " << var1.MakeString() << "\n";

    // Conversion
    wxString str = var1.MakeString();

    var1 = 123.456;
    textCtrl << "var1 = " << var1.GetReal() << "\n";

    // Implicit conversion
    double v = var1;

    var1 = 9876L;
    textCtrl << "var1 = " << var1.GetLong() << "\n";

    // Implicit conversion
    long l = var1;

    wxStringList stringList;
    stringList.Add(_T("one")); stringList.Add(_T("two")); stringList.Add(_T("three"));
    var1 = stringList;
    textCtrl << "var1 = " << var1.MakeString() << "\n";

    var1.ClearList();
    var1.Append(wxVariant(1.2345));
    var1.Append(wxVariant("hello"));
    var1.Append(wxVariant(54321L));

    textCtrl << "var1 = " << var1.MakeString() << "\n";

    size_t n = var1.GetCount();
    size_t i;
    for (i = (size_t) 0; i < n; i++)
    {
        textCtrl << "var1[" << (int) i << "] (type " << var1[i].GetType() << ") = " << var1[i].MakeString() << "\n";
    }
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(TYPES_QUIT, MyFrame::OnQuit)
	EVT_MENU(TYPES_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame *parent, const wxString& title,
       const wxPoint& pos, const wxSize& size):
  wxFrame(parent, -1, title, pos, size)
{}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
  Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
  wxMessageDialog dialog(this, "Tests various wxWindows types",
  	"About Types", wxYES_NO|wxCANCEL);

  dialog.ShowModal();
}


