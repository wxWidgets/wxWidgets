/////////////////////////////////////////////////////////////////////////////
// Name:        typetest.cpp
// Purpose:     Types wxWindows sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:       wxWindows license
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
#include "wx/mimetype.h"

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
#include "wx/datstrm.h"
#include "wx/txtstrm.h"

// Create a new application object
IMPLEMENT_APP    (MyApp)

IMPLEMENT_DYNAMIC_CLASS    (MyApp, wxApp)

BEGIN_EVENT_TABLE(MyApp, wxApp)
    EVT_MENU(TYPES_DATE,      MyApp::DoDateDemo)
    EVT_MENU(TYPES_TIME,      MyApp::DoTimeDemo)
    EVT_MENU(TYPES_VARIANT,   MyApp::DoVariantDemo)
    EVT_MENU(TYPES_BYTEORDER, MyApp::DoByteOrderDemo)
#if wxUSE_UNICODE
    EVT_MENU(TYPES_UNICODE,   MyApp::DoUnicodeDemo)
#endif
    EVT_MENU(TYPES_STREAM, MyApp::DoStreamDemo)
    EVT_MENU(TYPES_STREAM2, MyApp::DoStreamDemo2)
    EVT_MENU(TYPES_STREAM3, MyApp::DoStreamDemo3)
    EVT_MENU(TYPES_STREAM4, MyApp::DoStreamDemo4)
    EVT_MENU(TYPES_MIME, MyApp::DoMIMEDemo)
END_EVENT_TABLE()

bool MyApp::OnInit()
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
    file_menu->Append(TYPES_QUIT, "E&xit\tAlt-X");

    wxMenu *test_menu = new wxMenu;
    test_menu->Append(TYPES_DATE, "&Date test");
    test_menu->Append(TYPES_TIME, "&Time test");
    test_menu->Append(TYPES_VARIANT, "&Variant test");
    test_menu->Append(TYPES_BYTEORDER, "&Byteorder test");
#if wxUSE_UNICODE
    test_menu->Append(TYPES_UNICODE, "&Unicode test");
#endif
    test_menu->Append(TYPES_STREAM, "&Stream test");
    test_menu->Append(TYPES_STREAM2, "&Stream seek test");
    test_menu->Append(TYPES_STREAM3, "&Stream error test");
    test_menu->Append(TYPES_STREAM4, "&Stream buffer test");
    test_menu->AppendSeparator();
    test_menu->Append(TYPES_MIME, "&MIME database test");

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(test_menu, "&Tests");
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
    textCtrl << _T("\nTest fstream vs. wxFileStream:\n\n");

    textCtrl.WriteText( "Writing to ofstream and wxFileOutputStream:\n" );

    ofstream std_file_output( "test_std.dat" );
    wxFileOutputStream file_output( "test_wx.dat" );
    wxBufferedOutputStream buf_output( file_output );
    wxTextOutputStream text_output( buf_output );

    wxString tmp;
    signed int si = 0xFFFFFFFF;
    tmp.Printf( _T("Signed int: %d\n"), si );
    textCtrl.WriteText( tmp );
    text_output << si << "\n";
    std_file_output << si << "\n";

    unsigned int ui = 0xFFFFFFFF;
    tmp.Printf( _T("Unsigned int: %u\n"), ui );
    textCtrl.WriteText( tmp );
    text_output << ui << "\n";
    std_file_output << ui << "\n";

    double d = 2.01234567890123456789;
    tmp.Printf( _T("Double: %f\n"), d );
    textCtrl.WriteText( tmp );
    text_output << d << "\n";
    std_file_output << d << "\n";

    float f = (float)0.00001;
    tmp.Printf( _T("Float: %f\n"), f );
    textCtrl.WriteText( tmp );
    text_output << f << "\n";
    std_file_output << f << "\n";

    wxString str( _T("Hello!") );
    tmp.Printf( _T("String: %s\n"), str.c_str() );
    textCtrl.WriteText( tmp );
    text_output << str << "\n";
    std_file_output << str.c_str() << "\n";

    textCtrl.WriteText( "\nReading from ifstream:\n" );

    ifstream std_file_input( "test_std.dat" );

    std_file_input >> si;
    tmp.Printf( _T("Signed int: %d\n"), si );
    textCtrl.WriteText( tmp );

    std_file_input >> ui;
    tmp.Printf( _T("Unsigned int: %u\n"), ui );
    textCtrl.WriteText( tmp );

    std_file_input >> d;
    tmp.Printf( _T("Double: %f\n"), d );
    textCtrl.WriteText( tmp );

    std_file_input >> f;
    tmp.Printf( _T("Float: %f\n"), f );
    textCtrl.WriteText( tmp );

    std_file_input >> str;
    tmp.Printf( _T("String: %s\n"), str.c_str() );
    textCtrl.WriteText( tmp );

    textCtrl.WriteText( "\nReading from wxFileInputStream:\n" );

    buf_output.Sync();

    wxFileInputStream file_input( "test_wx.dat" );
    wxBufferedInputStream buf_input( file_input );
    wxTextInputStream text_input( file_input );

    text_input >> si;
    tmp.Printf( _T("Signed int: %d\n"), si );
    textCtrl.WriteText( tmp );

    text_input >> ui;
    tmp.Printf( _T("Unsigned int: %u\n"), ui );
    textCtrl.WriteText( tmp );

    text_input >> d;
    tmp.Printf( _T("Double: %f\n"), d );
    textCtrl.WriteText( tmp );

    text_input >> f;
    tmp.Printf( _T("Float: %f\n"), f );
    textCtrl.WriteText( tmp );

    text_input >> str;
    tmp.Printf( _T("String: %s\n"), str.c_str() );
    textCtrl.WriteText( tmp );


    textCtrl << "\nTest for wxDataStream:\n\n";

    textCtrl.WriteText( "Writing to wxDataOutputStream:\n" );

    file_output.SeekO( 0 );
    wxDataOutputStream data_output( buf_output );

    wxInt16 i16 = (short)0xFFFF;
    tmp.Printf( _T("Signed int16: %d\n"), (int)i16 );
    textCtrl.WriteText( tmp );
    data_output.Write16( i16 );

    wxUint16 ui16 = 0xFFFF;
    tmp.Printf( _T("Unsigned int16: %u\n"), (unsigned int) ui16 );
    textCtrl.WriteText( tmp );
    data_output.Write16( ui16 );

    d = 2.01234567890123456789;
    tmp.Printf( _T("Double: %f\n"), d );
    textCtrl.WriteText( tmp );
    data_output.WriteDouble( d );

    str = "Hello!";
    tmp.Printf( _T("String: %s\n"), str.c_str() );
    textCtrl.WriteText( tmp );
    data_output.WriteString( str );

    buf_output.Sync();

    textCtrl.WriteText( "\nReading from wxDataInputStream:\n" );

    file_input.SeekI( 0 );
    wxDataInputStream data_input( buf_input );

    i16 = data_input.Read16();
    tmp.Printf( _T("Signed int16: %d\n"), (int)i16 );
    textCtrl.WriteText( tmp );

    ui16 = data_input.Read16();
    tmp.Printf( _T("Unsigned int16: %u\n"), (unsigned int) ui16 );
    textCtrl.WriteText( tmp );

    d = data_input.ReadDouble();
    tmp.Printf( _T("Double: %f\n"), d );
    textCtrl.WriteText( tmp );

    str = data_input.ReadString();
    tmp.Printf( _T("String: %s\n"), str.c_str() );
    textCtrl.WriteText( tmp );
}

void MyApp::DoStreamDemo2(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTesting wxBufferedStream:\n\n");

    char ch,ch2;

    textCtrl.WriteText( "Writing number 0 to 9 to buffered wxFileOutputStream:\n\n" );

    wxFileOutputStream file_output( "test_wx.dat" );
    wxBufferedOutputStream buf_output( file_output );
    for (ch = 0; ch < 10; ch++)
        buf_output.Write( &ch, 1 );
    buf_output.Sync();
    
    wxFileInputStream file_input( "test_wx.dat" );
    for (ch2 = 0; ch2 < 10; ch2++)
    {
        file_input.Read( &ch, 1 );
	textCtrl.WriteText( (char)(ch + '0') );
    }
    textCtrl.WriteText( "\n\n\n" );
    
    textCtrl.WriteText( "Writing number 0 to 9 to buffered wxFileOutputStream, then\n" );
    textCtrl.WriteText( "seeking back to #3 and writing 3:\n\n" );

    wxFileOutputStream file_output2( "test_wx2.dat" );
    wxBufferedOutputStream buf_output2( file_output2 );
    for (ch = 0; ch < 10; ch++)
        buf_output2.Write( &ch, 1 );
    buf_output2.SeekO( 3 );
    ch = 3;
    buf_output2.Write( &ch, 1 );
    buf_output2.Sync();
    
    wxFileInputStream file_input2( "test_wx2.dat" );
    for (ch2 = 0; ch2 < 10; ch2++)
    {
        file_input2.Read( &ch, 1 );
	textCtrl.WriteText( (char)(ch + '0') );
    }
    textCtrl.WriteText( "\n\n\n" );
    
    // now append 2000 bytes to file (bigger than buffer)
    buf_output2.SeekO( 0, wxFromEnd );
    ch = 1;
    for (int i = 0; i < 2000; i++)
       buf_output2.Write( &ch, 1 );
    buf_output2.Sync();
    
    textCtrl.WriteText( "Reading number 0 to 9 from buffered wxFileInputStream, then\n" );
    textCtrl.WriteText( "seeking back to #3 and reading 3:\n\n" );

    wxFileInputStream file_input3( "test_wx2.dat" );
    wxBufferedInputStream buf_input3( file_input3 );
    for (ch2 = 0; ch2 < 10; ch2++)
    {
        buf_input3.Read( &ch, 1 );
	textCtrl.WriteText( (char)(ch + '0') );
    }
    for (int j = 0; j < 2000; j++)
       buf_input3.Read( &ch, 1 );
    textCtrl.WriteText( "\n" );
    buf_input3.SeekI( 3 );
    buf_input3.Read( &ch, 1 );
    textCtrl.WriteText( (char)(ch + '0') );
    textCtrl.WriteText( "\n\n\n" );
    
}

void MyApp::DoStreamDemo3(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << "\nTesting wxFileInputStream's and wxFFileInputStream's error handling:\n\n";

    char ch,ch2;

    textCtrl.WriteText( "Writing number 0 to 9 to wxFileOutputStream:\n\n" );

    wxFileOutputStream file_output( "test_wx.dat" );
    for (ch = 0; ch < 10; ch++)
        file_output.Write( &ch, 1 );

    // Testing wxFileInputStream
    
    textCtrl.WriteText( "Reading 0 to 10 to wxFileInputStream:\n\n" );

    wxFileInputStream file_input( "test_wx.dat" );
    for (ch2 = 0; ch2 < 11; ch2++)
    {
        file_input.Read( &ch, 1 );
	textCtrl.WriteText( "Value read: " );
	textCtrl.WriteText( (char)(ch + '0') );
	textCtrl.WriteText( ";  stream.LastError() returns: " );
	switch (file_input.LastError())
	{
	    case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	    case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	    case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	    case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	    default: textCtrl.WriteText( "Huh?\n" ); break;
	}
    }
    textCtrl.WriteText( "\n" );
    
    textCtrl.WriteText( "Seeking to 0;  stream.LastError() returns: " );
    file_input.SeekI( 0 );
    switch (file_input.LastError())
    {
	case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	default: textCtrl.WriteText( "Huh?\n" ); break;
    }
    textCtrl.WriteText( "\n" );
    
    file_input.Read( &ch, 1 );
    textCtrl.WriteText( "Value read: " );
    textCtrl.WriteText( (char)(ch + '0') );
    textCtrl.WriteText( ";  stream.LastError() returns: " );
    switch (file_input.LastError())
    {
	case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	default: textCtrl.WriteText( "Huh?\n" ); break;
    }
    textCtrl.WriteText( "\n\n" );

    
    // Testing wxFFileInputStream
    
    textCtrl.WriteText( "Reading 0 to 10 to wxFFileInputStream:\n\n" );

    wxFFileInputStream ffile_input( "test_wx.dat" );
    for (ch2 = 0; ch2 < 11; ch2++)
    {
        ffile_input.Read( &ch, 1 );
	textCtrl.WriteText( "Value read: " );
	textCtrl.WriteText( (char)(ch + '0') );
	textCtrl.WriteText( ";  stream.LastError() returns: " );
	switch (ffile_input.LastError())
	{
	    case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	    case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	    case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	    case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	    default: textCtrl.WriteText( "Huh?\n" ); break;
	}
    }
    textCtrl.WriteText( "\n" );
    
    textCtrl.WriteText( "Seeking to 0;  stream.LastError() returns: " );
    ffile_input.SeekI( 0 );
    switch (ffile_input.LastError())
    {
	case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	default: textCtrl.WriteText( "Huh?\n" ); break;
    }
    textCtrl.WriteText( "\n" );
    
    ffile_input.Read( &ch, 1 );
    textCtrl.WriteText( "Value read: " );
    textCtrl.WriteText( (char)(ch + '0') );
    textCtrl.WriteText( ";  stream.LastError() returns: " );
    switch (ffile_input.LastError())
    {
	case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	default: textCtrl.WriteText( "Huh?\n" ); break;
    }
    textCtrl.WriteText( "\n\n" );

    // Testing wxFFileInputStream
    
    textCtrl.WriteText( "Reading 0 to 10 to buffered wxFFileInputStream:\n\n" );

    wxFFileInputStream ffile_input2( "test_wx.dat" );
    wxBufferedInputStream buf_input( ffile_input2 );
    for (ch2 = 0; ch2 < 11; ch2++)
    {
        buf_input.Read( &ch, 1 );
	textCtrl.WriteText( "Value read: " );
	textCtrl.WriteText( (char)(ch + '0') );
	textCtrl.WriteText( ";  stream.LastError() returns: " );
	switch (buf_input.LastError())
	{
	    case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	    case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	    case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	    case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	    default: textCtrl.WriteText( "Huh?\n" ); break;
	}
    }
    textCtrl.WriteText( "\n" );
    
    textCtrl.WriteText( "Seeking to 0;  stream.LastError() returns: " );
    buf_input.SeekI( 0 );
    switch (buf_input.LastError())
    {
	case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	default: textCtrl.WriteText( "Huh?\n" ); break;
    }
    textCtrl.WriteText( "\n" );
    
    buf_input.Read( &ch, 1 );
    textCtrl.WriteText( "Value read: " );
    textCtrl.WriteText( (char)(ch + '0') );
    textCtrl.WriteText( ";  stream.LastError() returns: " );
    switch (buf_input.LastError())
    {
	case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	default: textCtrl.WriteText( "Huh?\n" ); break;
    }
}

void MyApp::DoStreamDemo4(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    wxString msg;
    
    textCtrl.Clear();
    textCtrl << "\nTesting wxStreamBuffer:\n\n";

    // bigger than buffer
    textCtrl.WriteText( "Writing 2000x 1 to wxFileOutputStream.\n\n" );

    wxFileOutputStream file_output( "test_wx.dat" );
    for (int i = 0; i < 2000; i++)
    {
        char ch = 1;
        file_output.Write( &ch, 1 );
    }

    textCtrl.WriteText( "Opening with a buffered wxFileInputStream:\n\n" );

    wxFileInputStream file_input( "test_wx.dat" );
    wxBufferedInputStream buf_input( file_input );
    
    textCtrl.WriteText( "wxBufferedInputStream.LastError() returns: " );
    switch (buf_input.LastError())
    {
        case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	default: textCtrl.WriteText( "Huh?\n" ); break;
    }
    msg.Printf( "wxBufferedInputStream.LastRead() returns: %d\n", (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( "wxBufferedInputStream.TellI() returns: %d\n", (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( "\n\n" );
    

    textCtrl.WriteText( "Seeking to position 300:\n\n" );

    buf_input.SeekI( 300 );
    
    textCtrl.WriteText( "wxBufferedInputStream.LastError() returns: " );
    switch (buf_input.LastError())
    {
        case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	default: textCtrl.WriteText( "Huh?\n" ); break;
    }
    msg.Printf( "wxBufferedInputStream.LastRead() returns: %d\n", (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( "wxBufferedInputStream.TellI() returns: %d\n", (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( "\n\n" );
    

    char buf[2000];

    textCtrl.WriteText( "Reading 500 bytes:\n\n" );

    buf_input.Read( buf, 500 );
    
    textCtrl.WriteText( "wxBufferedInputStream.LastError() returns: " );
    switch (buf_input.LastError())
    {
        case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	default: textCtrl.WriteText( "Huh?\n" ); break;
    }
    msg.Printf( "wxBufferedInputStream.LastRead() returns: %d\n", (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( "wxBufferedInputStream.TellI() returns: %d\n", (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( "\n\n" );
    
    textCtrl.WriteText( "Reading another 500 bytes:\n\n" );

    buf_input.Read( buf, 500 );
    
    textCtrl.WriteText( "wxBufferedInputStream.LastError() returns: " );
    switch (buf_input.LastError())
    {
        case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	default: textCtrl.WriteText( "Huh?\n" ); break;
    }
    msg.Printf( "wxBufferedInputStream.LastRead() returns: %d\n", (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( "wxBufferedInputStream.TellI() returns: %d\n", (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( "\n\n" );

    textCtrl.WriteText( "Reading another 500 bytes:\n\n" );

    buf_input.Read( buf, 500 );
    
    textCtrl.WriteText( "wxBufferedInputStream.LastError() returns: " );
    switch (buf_input.LastError())
    {
        case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	default: textCtrl.WriteText( "Huh?\n" ); break;
    }
    msg.Printf( "wxBufferedInputStream.LastRead() returns: %d\n", (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( "wxBufferedInputStream.TellI() returns: %d\n", (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( "\n\n" );

    textCtrl.WriteText( "Reading another 500 bytes:\n\n" );

    buf_input.Read( buf, 500 );
    
    textCtrl.WriteText( "wxBufferedInputStream.LastError() returns: " );
    switch (buf_input.LastError())
    {
        case wxSTREAM_NOERROR:      textCtrl.WriteText( "wxSTREAM_NOERROR\n" ); break;
	case wxSTREAM_EOF:          textCtrl.WriteText( "wxSTREAM_EOF\n" ); break;
	case wxSTREAM_READ_ERROR:   textCtrl.WriteText( "wxSTREAM_READ_ERROR\n" ); break;
	case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( "wxSTREAM_WRITE_ERROR\n" ); break;
	default: textCtrl.WriteText( "Huh?\n" ); break;
    }
    msg.Printf( "wxBufferedInputStream.LastRead() returns: %d\n", (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( "wxBufferedInputStream.TellI() returns: %d\n", (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( "\n\n" );
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

void MyApp::DoMIMEDemo(wxCommandEvent& WXUNUSED(event))
{
    static wxString s_defaultExt = "xyz";

    wxString ext = wxGetTextFromUser("Enter a file extension: ",
                                     "MIME database test",
                                     s_defaultExt);
    if ( !!ext )
    {
        s_defaultExt = ext;

        // init MIME database if not done yet
        if ( !m_mimeDatabase )
        {
            m_mimeDatabase = new wxMimeTypesManager;

            static const wxFileTypeInfo fallbacks[] =
            {
                wxFileTypeInfo("application/xyz",
                               "XyZ %s",
                               "XyZ -p %s",
                               "The one and only XYZ format file",
                               "xyz", "123", NULL),
                wxFileTypeInfo("text/html",
                               "lynx %s",
                               "lynx -dump %s | lpr",
                               "HTML document (from fallback)",
                               "htm", "html", NULL),

                // must terminate the table with this!
                wxFileTypeInfo()
            };

            m_mimeDatabase->AddFallbacks(fallbacks);
        }

        wxTextCtrl& textCtrl = * GetTextCtrl();

        wxFileType *filetype = m_mimeDatabase->GetFileTypeFromExtension(ext);
        if ( !filetype )
        {
            textCtrl << "Unknown extension '" << ext << "'\n";
        }
        else
        {
            wxString type, desc, open;
            filetype->GetMimeType(&type);
            filetype->GetDescription(&desc);

            wxString filename = "filename";
            filename << "." << ext;
            wxFileType::MessageParameters params(filename, type);
            filetype->GetOpenCommand(&open, params);

            textCtrl << "MIME information about extension '" << ext << "'\n"
                     << "\tMIME type: " << ( !type ? "unknown"
                                                   : type.c_str() ) << '\n'
                     << "\tDescription: " << ( !desc ? "" : desc.c_str() )
                        << '\n'
                     << "\tCommand to open: " << ( !open ? "no" : open.c_str() )
                        << '\n';

            delete filetype;
        }
    }
    //else: cancelled by user
}

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

    // suppress compile warnings about unused variables
    if ( l < v )
    {
        ;
    }

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


