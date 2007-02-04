/////////////////////////////////////////////////////////////////////////////
// Name:        typetest.cpp
// Purpose:     Types wxWidgets sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:       wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/variant.h"
#include "wx/mimetype.h"

#include "typetest.h"

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
#include "mondrian.xpm"
#endif

#ifdef new
#undef new
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
#include "wx/mstream.h"

// Create a new application object
IMPLEMENT_APP    (MyApp)

IMPLEMENT_DYNAMIC_CLASS    (MyApp, wxApp)

BEGIN_EVENT_TABLE(MyApp, wxApp)
    EVT_MENU(TYPES_VARIANT,   MyApp::DoVariantDemo)
    EVT_MENU(TYPES_BYTEORDER, MyApp::DoByteOrderDemo)
#if wxUSE_UNICODE
    EVT_MENU(TYPES_UNICODE,   MyApp::DoUnicodeDemo)
#endif // wxUSE_UNICODE
    EVT_MENU(TYPES_STREAM, MyApp::DoStreamDemo)
    EVT_MENU(TYPES_STREAM2, MyApp::DoStreamDemo2)
    EVT_MENU(TYPES_STREAM3, MyApp::DoStreamDemo3)
    EVT_MENU(TYPES_STREAM4, MyApp::DoStreamDemo4)
    EVT_MENU(TYPES_STREAM5, MyApp::DoStreamDemo5)
    EVT_MENU(TYPES_STREAM6, MyApp::DoStreamDemo6)
    EVT_MENU(TYPES_STREAM7, MyApp::DoStreamDemo7)
    EVT_MENU(TYPES_MIME, MyApp::DoMIMEDemo)
END_EVENT_TABLE()

wxString file_name = _T("test_wx.dat");
wxString file_name2 = wxString(_T("test_wx2.dat"));

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window
    MyFrame *frame = new MyFrame((wxFrame *) NULL, _T("wxWidgets Types Demo"),
                                 wxPoint(50, 50), wxSize(450, 340));

    // Give it an icon
    frame->SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(TYPES_ABOUT, _T("&About"));
    file_menu->AppendSeparator();
    file_menu->Append(TYPES_QUIT, _T("E&xit\tAlt-X"));

    wxMenu *test_menu = new wxMenu;
    test_menu->Append(TYPES_VARIANT, _T("&Variant test"));
    test_menu->Append(TYPES_BYTEORDER, _T("&Byteorder test"));
#if wxUSE_UNICODE
    test_menu->Append(TYPES_UNICODE, _T("&Unicode test"));
#endif // wxUSE_UNICODE
    test_menu->Append(TYPES_STREAM, _T("&Stream test"));
    test_menu->Append(TYPES_STREAM2, _T("&Stream seek test"));
    test_menu->Append(TYPES_STREAM3, _T("&Stream error test"));
    test_menu->Append(TYPES_STREAM4, _T("&Stream buffer test"));
    test_menu->Append(TYPES_STREAM5, _T("&Stream peek test"));
    test_menu->Append(TYPES_STREAM6, _T("&Stream ungetch test"));
    test_menu->Append(TYPES_STREAM7, _T("&Stream ungetch test for a buffered stream"));
    test_menu->AppendSeparator();
    test_menu->Append(TYPES_MIME, _T("&MIME database test"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _T("&File"));
    menu_bar->Append(test_menu, _T("&Tests"));
    frame->SetMenuBar(menu_bar);

    m_textCtrl = new wxTextCtrl(frame, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

    // Show the frame
    frame->Show(true);

    SetTopWindow(frame);

    return true;
}

void MyApp::DoStreamDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTest fstream vs. wxFileStream:\n\n");

    textCtrl.WriteText( _T("Writing to ofstream and wxFileOutputStream:\n") );

    wxSTD ofstream std_file_output( "test_std.dat" );
    wxFileOutputStream file_output( file_name );
    wxBufferedOutputStream buf_output( file_output );
    wxTextOutputStream text_output( buf_output );

    wxString tmp;
    signed int si = 0xFFFFFFFF;
    tmp.Printf( _T("Signed int: %d\n"), si );
    textCtrl.WriteText( tmp );
    text_output << si << _T("\n");
    std_file_output << si << "\n";

    unsigned int ui = 0xFFFFFFFF;
    tmp.Printf( _T("Unsigned int: %u\n"), ui );
    textCtrl.WriteText( tmp );
    text_output << ui << _T("\n");
    std_file_output << ui << "\n";

    double d = 2.01234567890123456789;
    tmp.Printf( _T("Double: %f\n"), d );
    textCtrl.WriteText( tmp );
    text_output << d << _T("\n");
    std_file_output << d << "\n";

    float f = (float)0.00001;
    tmp.Printf( _T("Float: %f\n"), f );
    textCtrl.WriteText( tmp );
    text_output << f << _T("\n");
    std_file_output << f << "\n";

    wxString str( _T("Hello!") );
    tmp.Printf( _T("String: %s\n"), str.c_str() );
    textCtrl.WriteText( tmp );
    text_output << str << _T("\n");
    std_file_output << str.ToAscii() << "\n";

    textCtrl.WriteText( _T("\nReading from ifstream:\n") );

    wxSTD ifstream std_file_input( "test_std.dat" );

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

    char std_buf[200];
    std_file_input >> std_buf;
    str = wxString::FromAscii(std_buf);
    tmp.Printf( _T("String: %s\n"), str.c_str() );
    textCtrl.WriteText( tmp );

    textCtrl.WriteText( _T("\nReading from wxFileInputStream:\n") );

    buf_output.Sync();

    wxFileInputStream file_input( file_name );
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



    textCtrl << _T("\nTest for wxDataStream:\n\n");

    textCtrl.WriteText( _T("Writing to wxDataOutputStream:\n") );

    file_output.SeekO( 0 );
    wxDataOutputStream data_output( buf_output );

    wxInt16 i16 = (unsigned short)0xFFFF;
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

    str = _T("Hello!");
    tmp.Printf( _T("String: %s\n"), str.c_str() );
    textCtrl.WriteText( tmp );
    data_output.WriteString( str );

    buf_output.Sync();

    textCtrl.WriteText( _T("\nReading from wxDataInputStream:\n") );

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

    textCtrl.WriteText( _T("Writing number 0 to 9 to buffered wxFileOutputStream:\n\n") );

    wxFileOutputStream file_output( file_name );
    wxBufferedOutputStream buf_output( file_output );
    for (ch = 0; ch < 10; ch++)
        buf_output.Write( &ch, 1 );
    buf_output.Sync();

    wxFileInputStream file_input( file_name );
    for (ch2 = 0; ch2 < 10; ch2++)
    {
        file_input.Read( &ch, 1 );
        textCtrl.WriteText( (wxChar)(ch + _T('0')) );
    }
    textCtrl.WriteText( _T("\n\n\n") );

    textCtrl.WriteText( _T("Writing number 0 to 9 to buffered wxFileOutputStream, then\n") );
    textCtrl.WriteText( _T("seeking back to #3 and writing 0:\n\n") );

    wxFileOutputStream file_output2( file_name2 );
    wxBufferedOutputStream buf_output2( file_output2 );
    for (ch = 0; ch < 10; ch++)
        buf_output2.Write( &ch, 1 );
    buf_output2.SeekO( 3 );
    ch = 0;
    buf_output2.Write( &ch, 1 );
    buf_output2.Sync();

    wxFileInputStream file_input2( file_name2 );
    for (ch2 = 0; ch2 < 10; ch2++)
    {
        file_input2.Read( &ch, 1 );
        textCtrl.WriteText( (wxChar)(ch + _T('0')) );
    }
    textCtrl.WriteText( _T("\n\n\n") );

    // now append 2000 bytes to file (bigger than buffer)
    buf_output2.SeekO( 0, wxFromEnd );
    ch = 1;
    for (int i = 0; i < 2000; i++)
       buf_output2.Write( &ch, 1 );
    buf_output2.Sync();

    textCtrl.WriteText( _T("Reading number 0 to 9 from buffered wxFileInputStream, then\n") );
    textCtrl.WriteText( _T("seeking back to #3 and reading the 0:\n\n") );

    wxFileInputStream file_input3( file_name2 );
    wxBufferedInputStream buf_input3( file_input3 );
    for (ch2 = 0; ch2 < 10; ch2++)
    {
        buf_input3.Read( &ch, 1 );
        textCtrl.WriteText( (wxChar)(ch + _T('0')) );
    }
    for (int j = 0; j < 2000; j++)
       buf_input3.Read( &ch, 1 );
    textCtrl.WriteText( _T("\n") );
    buf_input3.SeekI( 3 );
    buf_input3.Read( &ch, 1 );
    textCtrl.WriteText( (wxChar)(ch + _T('0')) );
    textCtrl.WriteText( _T("\n\n\n") );

}

void MyApp::DoStreamDemo3(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTesting wxFileInputStream's and wxFFileInputStream's error handling:\n\n");

    char ch,ch2;

    textCtrl.WriteText( _T("Writing number 0 to 9 to wxFileOutputStream:\n\n") );

    wxFileOutputStream file_output( file_name );
    for (ch = 0; ch < 10; ch++)
        file_output.Write( &ch, 1 );

    // Testing wxFileInputStream

    textCtrl.WriteText( _T("Reading 0 to 10 to wxFileInputStream:\n\n") );

    wxFileInputStream file_input( file_name );
    for (ch2 = 0; ch2 < 11; ch2++)
    {
        file_input.Read( &ch, 1 );
        textCtrl.WriteText( _T("Value read: ") );
        textCtrl.WriteText( (wxChar)(ch + '0') );
        textCtrl.WriteText( _T(";  stream.GetLastError() returns: ") );
        switch (file_input.GetLastError())
        {
            case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
            case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
            case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
            case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
            default: textCtrl.WriteText( _T("Huh?\n") ); break;
        }
    }
    textCtrl.WriteText( _T("\n") );

    textCtrl.WriteText( _T("Seeking to 0;  stream.GetLastError() returns: ") );
    file_input.SeekI( 0 );
    switch (file_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    textCtrl.WriteText( _T("\n") );

    file_input.Read( &ch, 1 );
    textCtrl.WriteText( _T("Value read: ") );
    textCtrl.WriteText( (wxChar)(ch + _T('0')) );
    textCtrl.WriteText( _T(";  stream.GetLastError() returns: ") );
    switch (file_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    textCtrl.WriteText( _T("\n\n") );


    // Testing wxFFileInputStream

    textCtrl.WriteText( _T("Reading 0 to 10 to wxFFileInputStream:\n\n") );

    wxFFileInputStream ffile_input( file_name );
    for (ch2 = 0; ch2 < 11; ch2++)
    {
        ffile_input.Read( &ch, 1 );
        textCtrl.WriteText( _T("Value read: ") );
        textCtrl.WriteText( (wxChar)(ch + '0') );
        textCtrl.WriteText( _T(";  stream.GetLastError() returns: ") );
        switch (ffile_input.GetLastError())
        {
            case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
            case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
            case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
            case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
            default: textCtrl.WriteText( _T("Huh?\n") ); break;
        }
    }
    textCtrl.WriteText( _T("\n") );

    textCtrl.WriteText( _T("Seeking to 0;  stream.GetLastError() returns: ") );
    ffile_input.SeekI( 0 );
    switch (ffile_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    textCtrl.WriteText( _T("\n") );

    ffile_input.Read( &ch, 1 );
    textCtrl.WriteText( _T("Value read: ") );
    textCtrl.WriteText( (wxChar)(ch + '0') );
    textCtrl.WriteText( _T(";  stream.GetLastError() returns: ") );
    switch (ffile_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    textCtrl.WriteText( _T("\n\n") );

    // Testing wxFFileInputStream

    textCtrl.WriteText( _T("Reading 0 to 10 to buffered wxFFileInputStream:\n\n") );

    wxFFileInputStream ffile_input2( file_name );
    wxBufferedInputStream buf_input( ffile_input2 );
    for (ch2 = 0; ch2 < 11; ch2++)
    {
        buf_input.Read( &ch, 1 );
        textCtrl.WriteText( _T("Value read: ") );
        textCtrl.WriteText( (wxChar)(ch + '0') );
        textCtrl.WriteText( _T(";  stream.GetLastError() returns: ") );
        switch (buf_input.GetLastError())
        {
            case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
            case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
            case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
            case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
            default: textCtrl.WriteText( _T("Huh?\n") ); break;
        }
    }
    textCtrl.WriteText( _T("\n") );

    textCtrl.WriteText( _T("Seeking to 0;  stream.GetLastError() returns: ") );
    buf_input.SeekI( 0 );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    textCtrl.WriteText( _T("\n") );

    buf_input.Read( &ch, 1 );
    textCtrl.WriteText( _T("Value read: ") );
    textCtrl.WriteText( (wxChar)(ch + '0') );
    textCtrl.WriteText( _T(";  stream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
}

void MyApp::DoStreamDemo4(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    wxString msg;

    textCtrl.Clear();
    textCtrl << _T("\nTesting wxStreamBuffer:\n\n");

    // bigger than buffer
    textCtrl.WriteText( _T("Writing 2000x 1 to wxFileOutputStream.\n\n") );

    wxFileOutputStream file_output( file_name );
    for (int i = 0; i < 2000; i++)
    {
        char ch = 1;
        file_output.Write( &ch, 1 );
    }

    textCtrl.WriteText( _T("Opening with a buffered wxFileInputStream:\n\n") );

    wxFileInputStream file_input( file_name );
    wxBufferedInputStream buf_input( file_input );

    textCtrl.WriteText( _T("wxBufferedInputStream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    msg.Printf( wxT("wxBufferedInputStream.LastRead() returns: %d\n"), (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( wxT("wxBufferedInputStream.TellI() returns: %d\n"), (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( _T("\n\n") );


    textCtrl.WriteText( _T("Seeking to position 300:\n\n") );

    buf_input.SeekI( 300 );

    textCtrl.WriteText( _T("wxBufferedInputStream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    msg.Printf( wxT("wxBufferedInputStream.LastRead() returns: %d\n"), (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( wxT("wxBufferedInputStream.TellI() returns: %d\n"), (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( _T("\n\n") );


    char buf[2000];

    textCtrl.WriteText( _T("Reading 500 bytes:\n\n") );

    buf_input.Read( buf, 500 );

    textCtrl.WriteText( _T("wxBufferedInputStream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    msg.Printf( wxT("wxBufferedInputStream.LastRead() returns: %d\n"), (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( wxT("wxBufferedInputStream.TellI() returns: %d\n"), (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( _T("\n\n") );

    textCtrl.WriteText( _T("Reading another 500 bytes:\n\n") );

    buf_input.Read( buf, 500 );

    textCtrl.WriteText( _T("wxBufferedInputStream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    msg.Printf( wxT("wxBufferedInputStream.LastRead() returns: %d\n"), (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( wxT("wxBufferedInputStream.TellI() returns: %d\n"), (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( _T("\n\n") );

    textCtrl.WriteText( _T("Reading another 500 bytes:\n\n") );

    buf_input.Read( buf, 500 );

    textCtrl.WriteText( _T("wxBufferedInputStream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    msg.Printf( wxT("wxBufferedInputStream.LastRead() returns: %d\n"), (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( wxT("wxBufferedInputStream.TellI() returns: %d\n"), (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( _T("\n\n") );

    textCtrl.WriteText( _T("Reading another 500 bytes:\n\n") );

    buf_input.Read( buf, 500 );

    textCtrl.WriteText( _T("wxBufferedInputStream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    msg.Printf( wxT("wxBufferedInputStream.LastRead() returns: %d\n"), (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( wxT("wxBufferedInputStream.TellI() returns: %d\n"), (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( _T("\n\n") );
}

void MyApp::DoStreamDemo5(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTesting wxFileInputStream's Peek():\n\n");

    char ch;
    wxString str;

    textCtrl.WriteText( _T("Writing number 0 to 9 to wxFileOutputStream:\n\n") );

    wxFileOutputStream file_output( file_name );
    for (ch = 0; ch < 10; ch++)
        file_output.Write( &ch, 1 );

    file_output.Sync();

    wxFileInputStream file_input( file_name );

    ch = file_input.Peek();
    str.Printf( wxT("First char peeked: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = file_input.GetC();
    str.Printf( wxT("First char read: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = file_input.Peek();
    str.Printf( wxT("Second char peeked: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = file_input.GetC();
    str.Printf( wxT("Second char read: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = file_input.Peek();
    str.Printf( wxT("Third char peeked: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = file_input.GetC();
    str.Printf( wxT("Third char read: %d\n"), (int) ch );
    textCtrl.WriteText( str );


    textCtrl << _T("\n\n\nTesting wxMemoryInputStream's Peek():\n\n");

    char buf[] = { 0,1,2,3,4,5,6,7,8,9,10 };
    wxMemoryInputStream input( buf, 10 );

    ch = input.Peek();
    str.Printf( wxT("First char peeked: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = input.GetC();
    str.Printf( wxT("First char read: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = input.Peek();
    str.Printf( wxT("Second char peeked: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = input.GetC();
    str.Printf( wxT("Second char read: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = input.Peek();
    str.Printf( wxT("Third char peeked: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = input.GetC();
    str.Printf( wxT("Third char read: %d\n"), (int) ch );
    textCtrl.WriteText( str );
}

void MyApp::DoStreamDemo6(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl.WriteText( _T("\nTesting Ungetch():\n\n") );

    char ch = 0;
    wxString str;

    textCtrl.WriteText( _T("Writing number 0 to 9 to wxFileOutputStream...\n\n") );

    wxFileOutputStream file_output( file_name );
    for (ch = 0; ch < 10; ch++)
        file_output.Write( &ch, 1 );

    file_output.Sync();

    textCtrl.WriteText( _T("Reading char from wxFileInputStream:\n\n") );

    wxFileInputStream file_input( file_name );

    ch = file_input.GetC();
    size_t pos = (size_t)file_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading another char from wxFileInputStream:\n\n") );

    ch = file_input.GetC();
    pos = (size_t)file_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading yet another char from wxFileInputStream:\n\n") );

    ch = file_input.GetC();
    pos = (size_t)file_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Now calling Ungetch( 5 ) from wxFileInputStream...\n\n") );

    file_input.Ungetch( 5 );
    pos = (size_t)file_input.TellI();
    str.Printf( wxT("Now at position %d\n\n"), (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading char from wxFileInputStream:\n\n") );

    ch = file_input.GetC();
    pos = (size_t)file_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading another char from wxFileInputStream:\n\n") );

    ch = file_input.GetC();
    pos = (size_t)file_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Now calling Ungetch( 5 ) from wxFileInputStream again...\n\n") );

    file_input.Ungetch( 5 );
    pos = (size_t)file_input.TellI();
    str.Printf( wxT("Now at position %d\n\n"), (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Seeking to pos 3 in wxFileInputStream. This invalidates the writeback buffer.\n\n") );

    file_input.SeekI( 3 );

    ch = file_input.GetC();
    pos = (size_t)file_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );
}

void MyApp::DoStreamDemo7(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl.WriteText( _T("\nTesting Ungetch() in buffered input stream:\n\n") );

    char ch = 0;
    wxString str;

    textCtrl.WriteText( _T("Writing number 0 to 9 to wxFileOutputStream...\n\n") );

    wxFileOutputStream file_output( file_name );
    for (ch = 0; ch < 10; ch++)
        file_output.Write( &ch, 1 );

    file_output.Sync();

    textCtrl.WriteText( _T("Reading char from wxBufferedInputStream via wxFileInputStream:\n\n") );

    wxFileInputStream file_input( file_name );
    wxBufferedInputStream buf_input( file_input );

    ch = buf_input.GetC();
    size_t pos = (size_t)buf_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading another char from wxBufferedInputStream:\n\n") );

    ch = buf_input.GetC();
    pos = (size_t)buf_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading yet another char from wxBufferedInputStream:\n\n") );

    ch = buf_input.GetC();
    pos = (size_t)buf_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Now calling Ungetch( 5 ) from wxBufferedInputStream...\n\n") );

    buf_input.Ungetch( 5 );
    pos = (size_t)buf_input.TellI();
    str.Printf( wxT("Now at position %d\n\n"), (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading char from wxBufferedInputStream:\n\n") );

    ch = buf_input.GetC();
    pos = (size_t)buf_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading another char from wxBufferedInputStream:\n\n") );

    ch = buf_input.GetC();
    pos = (size_t)buf_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Now calling Ungetch( 5 ) from wxBufferedInputStream again...\n\n") );

    buf_input.Ungetch( 5 );
    pos = (size_t)buf_input.TellI();
    str.Printf( wxT("Now at position %d\n\n"), (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Seeking to pos 3 in wxBufferedInputStream. This invalidates the writeback buffer.\n\n") );

    buf_input.SeekI( 3 );

    ch = buf_input.GetC();
    pos = (size_t)buf_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );
}

#if wxUSE_UNICODE
void MyApp::DoUnicodeDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTest wchar_t to char (Unicode to ANSI/Multibyte) converions:");

    wxString str;
    str = _T("Robert R\366bling\n");

    printf( "\n\nConversion with wxConvLocal:\n" );
    wxConvCurrent = &wxConvLocal;
    printf( (const char*) str.mbc_str() );
    printf( "\n\nConversion with wxConvLibc:\n" );
    wxConvCurrent = &wxConvLibc;
    printf( (const char*) str.mbc_str() );

}
#endif

void MyApp::DoMIMEDemo(wxCommandEvent& WXUNUSED(event))
{
    static wxString s_defaultExt = _T("xyz");

    wxString ext = wxGetTextFromUser(_T("Enter a file extension: "),
                                     _T("MIME database test"),
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
                wxFileTypeInfo(_T("application/xyz"),
                               _T("XyZ %s"),
                               _T("XyZ -p %s"),
                               _T("The one and only XYZ format file"),
                               _T("xyz"), _T("123"), NULL),
                wxFileTypeInfo(_T("text/html"),
                               _T("lynx %s"),
                               _T("lynx -dump %s | lpr"),
                               _T("HTML document (from fallback)"),
                               _T("htm"), _T("html"), NULL),

                // must terminate the table with this!
                wxFileTypeInfo()
            };

            m_mimeDatabase->AddFallbacks(fallbacks);
        }

        wxTextCtrl& textCtrl = * GetTextCtrl();

        wxFileType *filetype = m_mimeDatabase->GetFileTypeFromExtension(ext);
        if ( !filetype )
        {
            textCtrl << _T("Unknown extension '") << ext << _T("'\n");
        }
        else
        {
            wxString type, desc, open;
            filetype->GetMimeType(&type);
            filetype->GetDescription(&desc);

            wxString filename = _T("filename");
            filename << _T(".") << ext;
            wxFileType::MessageParameters params(filename, type);
            filetype->GetOpenCommand(&open, params);

            textCtrl << _T("MIME information about extension '") << ext << _T('\n')
                     << _T("\tMIME type: ") << ( !type ? wxT("unknown")
                                                   : type.c_str() ) << _T('\n')
                     << _T("\tDescription: ") << ( !desc ? wxEmptyString : desc.c_str() )
                        << _T('\n')
                     << _T("\tCommand to open: ") << ( !open ? wxT("no") : open.c_str() )
                        << _T('\n');

            delete filetype;
        }
    }
    //else: cancelled by user
}

void MyApp::DoByteOrderDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTest byte order macros:\n\n");

    #if wxBYTE_ORDER == wxLITTLE_ENDIAN
        textCtrl << _T("This is a little endian system.\n\n");
    #else
        textCtrl << _T("This is a big endian system.\n\n");
    #endif

    wxString text;

    wxInt32 var = 0xF1F2F3F4;
    text = wxEmptyString;
    text.Printf( _T("Value of wxInt32 is now: %#x.\n\n"), var );
    textCtrl.WriteText( text );

    text = wxEmptyString;
    text.Printf( _T("Value of swapped wxInt32 is: %#x.\n\n"), wxINT32_SWAP_ALWAYS( var ) );
    textCtrl.WriteText( text );

    text = wxEmptyString;
    text.Printf( _T("Value of wxInt32 swapped on little endian is: %#x.\n\n"), wxINT32_SWAP_ON_LE( var ) );
    textCtrl.WriteText( text );

    text = wxEmptyString;
    text.Printf( _T("Value of wxInt32 swapped on big endian is: %#x.\n\n"), wxINT32_SWAP_ON_BE( var ) );
    textCtrl.WriteText( text );
}

void MyApp::DoVariantDemo(wxCommandEvent& WXUNUSED(event) )
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    wxVariant var1 = _T("String value");
    textCtrl << _T("var1 = ") << var1.MakeString() << _T("\n");

    // Conversion
    wxString str = var1.MakeString();

    var1 = 123.456;
    textCtrl << _T("var1 = ") << var1.GetReal() << _T("\n");

    // Implicit conversion
    double v = var1;

    var1 = 9876L;
    textCtrl << _T("var1 = ") << var1.GetLong() << _T("\n");

    // Implicit conversion
    long l = var1;

    // suppress compile warnings about unused variables
    wxUnusedVar(l);
    wxUnusedVar(v);

    wxArrayString stringArray;
    stringArray.Add(_T("one")); stringArray.Add(_T("two")); stringArray.Add(_T("three"));
    var1 = stringArray;
    textCtrl << _T("var1 = ") << var1.MakeString() << _T("\n");

    var1.ClearList();
    var1.Append(wxVariant(1.2345));
    var1.Append(wxVariant(_T("hello")));
    var1.Append(wxVariant(54321L));

    textCtrl << _T("var1 = ") << var1.MakeString() << _T("\n");

    size_t n = var1.GetCount();
    size_t i;
    for (i = (size_t) 0; i < n; i++)
    {
        textCtrl << _T("var1[") << (int) i << _T("] (type ") << var1[i].GetType() << _T(") = ") << var1[i].MakeString() << _T("\n");
    }

    var1 = wxVariant(new wxFont(wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT)));
    textCtrl << _T("var1 = (wxfont)\"");
    wxFont* font = wxGetVariantCast(var1,wxFont);
    if (font)
    {
        textCtrl << font->GetNativeFontInfoDesc() << _T("\"\n");
    }
    else
    {
        textCtrl << _T("(null)\"\n");
    }
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(TYPES_QUIT, MyFrame::OnQuit)
    EVT_MENU(TYPES_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame *parent, const wxString& title,
    const wxPoint& pos, const wxSize& size)
    : wxFrame(parent, wxID_ANY, title, pos, size)
{}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxMessageDialog dialog(this, _T("Tests various wxWidgets types"),
        _T("About Types"), wxYES_NO|wxCANCEL);

    dialog.ShowModal();
}


