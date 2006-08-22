/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont demo
// Author:      Vadim Zeitlin
// Modified by:
// Created:     30.09.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all standard wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"

    #include "wx/log.h"
#endif

#include "wx/choicdlg.h"
#include "wx/fontdlg.h"
#include "wx/fontenum.h"
#include "wx/fontmap.h"
#include "wx/encconv.h"
#include "wx/splitter.h"
#include "wx/textfile.h"

#include "../sample.xpm"

#ifdef __WXMAC__
#undef wxFontDialog
#include "wx/mac/fontdlg.h"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// MyCanvas is a canvas on which we show the font sample
class MyCanvas: public wxWindow
{
public:
    MyCanvas( wxWindow *parent );
    virtual ~MyCanvas(){};

    // accessors for the frame
    const wxFont& GetTextFont() const { return m_font; }
    const wxColour& GetColour() const { return m_colour; }
    void SetTextFont(const wxFont& font) { m_font = font; }
    void SetColour(const wxColour& colour) { m_colour = colour; }

    // event handlers
    void OnPaint( wxPaintEvent &event );

private:
    wxColour m_colour;
    wxFont   m_font;

    DECLARE_EVENT_TABLE()
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // accessors
    MyCanvas *GetCanvas() const { return m_canvas; }

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnIncFont(wxCommandEvent& WXUNUSED(event)) { DoResizeFont(+2); }
    void OnDecFont(wxCommandEvent& WXUNUSED(event)) { DoResizeFont(-2); }

    void OnBold(wxCommandEvent& event);
    void OnItalic(wxCommandEvent& event);
    void OnUnderline(wxCommandEvent& event);

    void OnwxPointerFont(wxCommandEvent& event);

    void OnViewMsg(wxCommandEvent& event);
    void OnSelectFont(wxCommandEvent& event);
    void OnEnumerateFamiliesForEncoding(wxCommandEvent& event);
    void OnEnumerateFamilies(wxCommandEvent& WXUNUSED(event))
        { DoEnumerateFamilies(false); }
    void OnEnumerateFixedFamilies(wxCommandEvent& WXUNUSED(event))
        { DoEnumerateFamilies(true); }
    void OnEnumerateEncodings(wxCommandEvent& event);

    void OnCheckNativeToFromString(wxCommandEvent& event);
    void OnCheckNativeToFromUserString(wxCommandEvent& event);
    void OnCheckFaceName(wxCommandEvent& event);

protected:
    bool DoEnumerateFamilies(bool fixedWidthOnly,
                             wxFontEncoding encoding = wxFONTENCODING_SYSTEM,
                             bool silent = false);

    void DoResizeFont(int diff);
    void DoChangeFont(const wxFont& font, const wxColour& col = wxNullColour);

    size_t      m_fontSize; // in points

    wxTextCtrl *m_textctrl;
    MyCanvas   *m_canvas;

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Font_Quit = 1,
    Font_About,
    Font_ViewMsg,
    Font_IncSize,
    Font_DecSize,
    Font_Bold,
    Font_Italic,
    Font_Underlined,
    Font_wxNORMAL_FONT,
    Font_wxSMALL_FONT,
    Font_wxITALIC_FONT,
    Font_wxSWISS_FONT,
    Font_Standard,

    Font_Choose = 100,
    Font_EnumFamiliesForEncoding,
    Font_EnumFamilies,
    Font_EnumFixedFamilies,
    Font_EnumEncodings,
    Font_CheckNativeToFromString,
    Font_CheckNativeToFromUserString,
    Font_CheckFaceName,
    Font_Max
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Font_Quit,  MyFrame::OnQuit)
    EVT_MENU(Font_ViewMsg, MyFrame::OnViewMsg)
    EVT_MENU(Font_About, MyFrame::OnAbout)

    EVT_MENU(Font_IncSize, MyFrame::OnIncFont)
    EVT_MENU(Font_DecSize, MyFrame::OnDecFont)
    EVT_MENU(Font_Bold, MyFrame::OnBold)
    EVT_MENU(Font_Italic, MyFrame::OnItalic)
    EVT_MENU(Font_Underlined, MyFrame::OnUnderline)

    EVT_MENU(Font_wxNORMAL_FONT, MyFrame::OnwxPointerFont)
    EVT_MENU(Font_wxSMALL_FONT, MyFrame::OnwxPointerFont)
    EVT_MENU(Font_wxITALIC_FONT, MyFrame::OnwxPointerFont)
    EVT_MENU(Font_wxSWISS_FONT, MyFrame::OnwxPointerFont)


    EVT_MENU(Font_CheckNativeToFromString, MyFrame::OnCheckNativeToFromString)
    EVT_MENU(Font_CheckNativeToFromUserString, MyFrame::OnCheckNativeToFromUserString)
    EVT_MENU(Font_CheckFaceName, MyFrame::OnCheckFaceName)

    EVT_MENU(Font_Choose, MyFrame::OnSelectFont)
    EVT_MENU(Font_EnumFamiliesForEncoding, MyFrame::OnEnumerateFamiliesForEncoding)
    EVT_MENU(Font_EnumFamilies, MyFrame::OnEnumerateFamilies)
    EVT_MENU(Font_EnumFixedFamilies, MyFrame::OnEnumerateFixedFamilies)
    EVT_MENU(Font_EnumEncodings, MyFrame::OnEnumerateEncodings)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // Create the main application window
    MyFrame *frame = new MyFrame(wxT("Font wxWidgets demo"),
                                 wxPoint(50, 50), wxSize(600, 400));

    // Show it and tell the application that it's our main window
    frame->Show(true);
    SetTopWindow(frame);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned 'false' here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size), m_textctrl(NULL)
{
    m_fontSize = 12;

    SetIcon(wxIcon(sample_xpm));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(Font_ViewMsg, wxT("&View...\tCtrl-V"),
                     wxT("View an email message file"));
    menuFile->AppendSeparator();
    menuFile->Append(Font_About, wxT("&About...\tCtrl-A"), wxT("Show about dialog"));
    menuFile->AppendSeparator();
    menuFile->Append(Font_Quit, wxT("E&xit\tAlt-X"), wxT("Quit this program"));

    wxMenu *menuFont = new wxMenu;
    menuFont->Append(Font_IncSize, wxT("&Increase font size by 2 points\tCtrl-I"));
    menuFont->Append(Font_DecSize, wxT("&Decrease font size by 2 points\tCtrl-D"));
    menuFont->AppendSeparator();
    menuFont->AppendCheckItem(Font_Bold, wxT("&Bold\tCtrl-B"), wxT("Toggle bold state"));
    menuFont->AppendCheckItem(Font_Italic, wxT("&Oblique\tCtrl-O"), wxT("Toggle italic state"));
    menuFont->AppendCheckItem(Font_Underlined, wxT("&Underlined\tCtrl-U"),
                     wxT("Toggle underlined state"));

    menuFont->AppendSeparator();
    menuFont->Append(Font_CheckNativeToFromString,
                     wxT("Set native font &description\tShift-Ctrl-D"));
    menuFont->Append(Font_CheckNativeToFromUserString,
                     wxT("Set &user font description\tShift-Ctrl-U"));
    menuFont->Append(Font_CheckFaceName,
                     wxT("Check font face name"));                     

    wxMenu *menuSelect = new wxMenu;
    menuSelect->Append(Font_Choose, wxT("&Select font...\tCtrl-S"),
                     wxT("Select a standard font"));

    wxMenu *menuStdFonts = new wxMenu;
    menuStdFonts->Append(Font_wxNORMAL_FONT, wxT("wxNORMAL_FONT"), wxT("Normal font used by wxWidgets"));
    menuStdFonts->Append(Font_wxSMALL_FONT,  wxT("wxSMALL_FONT"),  wxT("Small font used by wxWidgets"));
    menuStdFonts->Append(Font_wxITALIC_FONT, wxT("wxITALIC_FONT"), wxT("Italic font used by wxWidgets"));
    menuStdFonts->Append(Font_wxSWISS_FONT,  wxT("wxSWISS_FONT"),  wxT("Swiss font used by wxWidgets"));
    menuSelect->Append(Font_Standard, wxT("Standar&d fonts"), menuStdFonts);

    menuSelect->AppendSeparator();
    menuSelect->Append(Font_EnumFamilies, wxT("Enumerate font &families\tCtrl-F"));
    menuSelect->Append(Font_EnumFixedFamilies,
                     wxT("Enumerate fi&xed font families\tCtrl-X"));
    menuSelect->Append(Font_EnumEncodings,
                     wxT("Enumerate &encodings\tCtrl-E"));
    menuSelect->Append(Font_EnumFamiliesForEncoding,
                     wxT("Find font for en&coding...\tCtrl-C"),
                     wxT("Find font families for given encoding"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, wxT("&File"));
    menuBar->Append(menuFont, wxT("F&ont"));
    menuBar->Append(menuSelect, wxT("&Select"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    wxSplitterWindow *splitter = new wxSplitterWindow(this);

    m_textctrl = new wxTextCtrl(splitter, wxID_ANY,
                                wxT("Paste text here to see how it looks\nlike in the given font"),
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE);

    m_canvas = new MyCanvas(splitter);

    splitter->SplitHorizontally(m_textctrl, m_canvas, 100);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar();
    SetStatusText(wxT("Welcome to wxWidgets font demo!"));
#endif // wxUSE_STATUSBAR
}

// --------------------------------------------------------

class MyEncodingEnumerator : public wxFontEnumerator
{
public:
    MyEncodingEnumerator()
        { m_n = 0; }

    const wxString& GetText() const
        { return m_text; }

protected:
    virtual bool OnFontEncoding(const wxString& facename,
                                const wxString& encoding)
    {
        wxString text;
        text.Printf(wxT("Encoding %u: %s (available in facename '%s')\n"),
                    (unsigned int) ++m_n, encoding.c_str(), facename.c_str());
        m_text += text;
        return true;
    }

private:
    size_t m_n;
    wxString m_text;
};

void MyFrame::OnEnumerateEncodings(wxCommandEvent& WXUNUSED(event))
{
    MyEncodingEnumerator fontEnumerator;

    fontEnumerator.EnumerateEncodings();

    wxLogMessage(wxT("Enumerating all available encodings:\n%s"),
                 fontEnumerator.GetText().c_str());
}

// -------------------------------------------------------------

class MyFontEnumerator : public wxFontEnumerator
{
public:
    bool GotAny() const
        { return !m_facenames.IsEmpty(); }

    const wxArrayString& GetFacenames() const
        { return m_facenames; }

protected:
    virtual bool OnFacename(const wxString& facename)
    {
        m_facenames.Add(facename);
        return true;
    }

    private:
        wxArrayString m_facenames;
} fontEnumerator;

bool MyFrame::DoEnumerateFamilies(bool fixedWidthOnly,
                                  wxFontEncoding encoding,
                                  bool silent)
{
    MyFontEnumerator fontEnumerator;

    fontEnumerator.EnumerateFacenames(encoding, fixedWidthOnly);

    if ( fontEnumerator.GotAny() )
    {
        int nFacenames = fontEnumerator.GetFacenames().GetCount();
        if ( !silent )
        {
            wxLogStatus(this, wxT("Found %d %sfonts"),
                        nFacenames, fixedWidthOnly ? wxT("fixed width ") : wxT(""));
        }

        wxString facename;

        if ( silent )
        {
            // choose the first
            facename = fontEnumerator.GetFacenames().Item(0);
        }
        else
        {
            // let the user choose
            wxString *facenames = new wxString[nFacenames];
            int n;
            for ( n = 0; n < nFacenames; n++ )
                facenames[n] = fontEnumerator.GetFacenames().Item(n);

            n = wxGetSingleChoiceIndex(wxT("Choose a facename"), wxT("Font demo"),
                                       nFacenames, facenames, this);

            if ( n != -1 )
                facename = facenames[n];

            delete [] facenames;
        }

        if ( !facename.empty() )
        {
            wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL, false, facename, encoding);

            DoChangeFont(font);
        }

        return true;
    }
    else if ( !silent )
    {
        wxLogWarning(wxT("No such fonts found."));
    }

    return false;
}

void MyFrame::OnEnumerateFamiliesForEncoding(wxCommandEvent& WXUNUSED(event))
{
    static wxFontEncoding encodings[] =
    {
        wxFONTENCODING_ISO8859_1,
        wxFONTENCODING_ISO8859_2,
        wxFONTENCODING_ISO8859_5,
        wxFONTENCODING_ISO8859_7,
        wxFONTENCODING_ISO8859_15,
        wxFONTENCODING_KOI8,
        wxFONTENCODING_KOI8_U,
        wxFONTENCODING_CP1250,
        wxFONTENCODING_CP1251,
        wxFONTENCODING_CP1252,
    };

    static const wxString encodingNames[] =
    {
        wxT("Western European (ISO-8859-1)"),
        wxT("Central European (ISO-8859-2)"),
        wxT("Cyrillic (ISO-8859-5)"),
        wxT("Greek (ISO-8859-7)"),
        wxT("Western European with Euro (ISO-8859-15)"),
        wxT("KOI8-R"),
        wxT("KOI8-U"),
        wxT("Windows Central European (CP 1250)"),
        wxT("Windows Cyrillic (CP 1251)"),
        wxT("Windows Western European (CP 1252)"),
    };

    int n = wxGetSingleChoiceIndex(wxT("Choose an encoding"), wxT("Font demo"),
                                   WXSIZEOF(encodingNames),
                                   encodingNames,
                                   this);

    if ( n != -1 )
    {
        DoEnumerateFamilies(false, encodings[n]);
    }
}

void MyFrame::OnCheckNativeToFromString(wxCommandEvent& WXUNUSED(event))
{
    wxString fontInfo = wxGetTextFromUser
                        (
                            wxT("Enter native font string"),
                            wxT("Input font description"),
                            m_canvas->GetTextFont().GetNativeFontInfoDesc(),
                            this
                        );
    if ( fontInfo.empty() )
        return;     // user clicked "Cancel" - do nothing

    wxFont font;
    font.SetNativeFontInfo(fontInfo);
    if ( !font.Ok() )
    {
        wxLogError(wxT("Font info string \"%s\" is invalid."),
                   fontInfo.c_str());
        return;
    }

    DoChangeFont(font);
}

void MyFrame::OnCheckFaceName(wxCommandEvent& WXUNUSED(event))
{
    wxString facename = GetCanvas()->GetTextFont().GetFaceName();
    wxString newFaceName = wxGetTextFromUser(
            wxT("Here you can edit current font face name."),
            wxT("Input font facename"), facename,
            this);
    if (newFaceName.IsEmpty())
        return;     // user clicked "Cancel" - do nothing

    wxFont font(GetCanvas()->GetTextFont());
    if (font.SetFaceName(newFaceName))      // change facename only
    {
        wxASSERT_MSG(font.Ok(), wxT("The font should now be valid"));
        DoChangeFont(font);
    }
    else
    {
        wxASSERT_MSG(!font.Ok(), wxT("The font should now be invalid"));
        wxMessageBox(wxT("There is no font with such face name..."),
                     wxT("Invalid face name"), wxOK|wxICON_ERROR, this);
    }    
}

void MyFrame::OnCheckNativeToFromUserString(wxCommandEvent& WXUNUSED(event))
{
    wxString fontdesc = GetCanvas()->GetTextFont().GetNativeFontInfoUserDesc();
    wxString fontUserInfo = wxGetTextFromUser(
            wxT("Here you can edit current font description"),
            wxT("Input font description"), fontdesc,
            this);
    if (fontUserInfo.IsEmpty())
        return;     // user clicked "Cancel" - do nothing

    wxFont font;
    if (font.SetNativeFontInfoUserDesc(fontUserInfo))
    {
        wxASSERT_MSG(font.Ok(), wxT("The font should now be valid"));
        DoChangeFont(font);
    }
    else
    {
        wxASSERT_MSG(!font.Ok(), wxT("The font should now be invalid"));
        wxMessageBox(wxT("Error trying to create a font with such description..."));
    }
}

void MyFrame::DoResizeFont(int diff)
{
    wxFont font = m_canvas->GetTextFont();

    font.SetPointSize(font.GetPointSize() + diff);
    DoChangeFont(font);
}

void MyFrame::OnBold(wxCommandEvent& event)
{
    wxFont font = m_canvas->GetTextFont();

    font.SetWeight(event.IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    DoChangeFont(font);
}

void MyFrame::OnItalic(wxCommandEvent& event)
{
    wxFont font = m_canvas->GetTextFont();

    font.SetStyle(event.IsChecked() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
    DoChangeFont(font);
}

void MyFrame::OnUnderline(wxCommandEvent& event)
{
    wxFont font = m_canvas->GetTextFont();

    font.SetUnderlined(event.IsChecked());
    DoChangeFont(font);
}

void MyFrame::OnwxPointerFont(wxCommandEvent& event)
{
    wxFont font;

    switch (event.GetId())
    {
        case Font_wxNORMAL_FONT : font = wxFont(*wxNORMAL_FONT); break;
        case Font_wxSMALL_FONT  : font = wxFont(*wxSMALL_FONT); break;
        case Font_wxITALIC_FONT : font = wxFont(*wxITALIC_FONT); break;
        case Font_wxSWISS_FONT  : font = wxFont(*wxSWISS_FONT); break;
        default                 : font = wxFont(*wxNORMAL_FONT); break;
    }

    DoChangeFont(font);
}

void MyFrame::DoChangeFont(const wxFont& font, const wxColour& col)
{
    m_canvas->SetTextFont(font);
    if ( col.Ok() )
        m_canvas->SetColour(col);
    m_canvas->Refresh();

    m_textctrl->SetFont(font);
    if ( col.Ok() )
        m_textctrl->SetForegroundColour(col);

    // update the state of the bold/italic/underlined menu items
    wxMenuBar *mbar = GetMenuBar();
    if ( mbar )
    {
        mbar->Check(Font_Bold, font.GetWeight() == wxFONTWEIGHT_BOLD);
        mbar->Check(Font_Italic, font.GetStyle() == wxFONTSTYLE_ITALIC);
        mbar->Check(Font_Underlined, font.GetUnderlined());
    }
}

void MyFrame::OnSelectFont(wxCommandEvent& WXUNUSED(event))
{
    wxFontData data;
    data.SetInitialFont(m_canvas->GetTextFont());
    data.SetColour(m_canvas->GetColour());

    wxFontDialog dialog(this, data);
    if ( dialog.ShowModal() == wxID_OK )
    {
        wxFontData retData = dialog.GetFontData();
        wxFont font = retData.GetChosenFont();
        wxColour colour = retData.GetColour();

        DoChangeFont(font, colour);
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnViewMsg(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_FILEDLG
    // first, choose the file
    static wxString s_dir, s_file;
    wxFileDialog dialog(this, wxT("Open an email message file"),
                        s_dir, s_file);
    if ( dialog.ShowModal() != wxID_OK )
        return;

    // save for the next time
    s_dir = dialog.GetDirectory();
    s_file = dialog.GetFilename();

    wxString filename = dialog.GetPath();

    // load it and search for Content-Type header
    wxTextFile file(filename);
    if ( !file.Open() )
        return;

    wxString charset;

    static const wxChar *prefix = wxT("Content-Type: text/plain; charset=");
    const size_t len = wxStrlen(prefix);

    size_t n, count = file.GetLineCount();
    for ( n = 0; n < count; n++ )
    {
        wxString line = file[n];

        if ( !line )
        {
            // if it is an email message, headers are over, no need to parse
            // all the file
            break;
        }

        if ( line.Left(len) == prefix )
        {
            // found!
            const wxChar *pc = line.c_str() + len;
            if ( *pc == wxT('"') )
                pc++;

            while ( *pc && *pc != wxT('"') )
            {
                charset += *pc++;
            }

            break;
        }
    }

    if ( !charset )
    {
        wxLogError(wxT("The file '%s' doesn't contain charset information."),
                   filename.c_str());

        return;
    }

    // ok, now get the corresponding encoding
    wxFontEncoding fontenc = wxFontMapper::Get()->CharsetToEncoding(charset);
    if ( fontenc == wxFONTENCODING_SYSTEM )
    {
        wxLogError(wxT("Charset '%s' is unsupported."), charset.c_str());
        return;
    }

    m_textctrl->LoadFile(filename);

    if ( fontenc == wxFONTENCODING_UTF8 ||
            !wxFontMapper::Get()->IsEncodingAvailable(fontenc) )
    {
        // try to find some similar encoding:
        wxFontEncoding encAlt;
        if ( wxFontMapper::Get()->GetAltForEncoding(fontenc, &encAlt) )
        {
            wxEncodingConverter conv;

            if (conv.Init(fontenc, encAlt))
            {
                fontenc = encAlt;
                m_textctrl -> SetValue(conv.Convert(m_textctrl -> GetValue()));
            }
            else
            {
                wxLogWarning(wxT("Cannot convert from '%s' to '%s'."),
                             wxFontMapper::GetEncodingDescription(fontenc).c_str(),
                             wxFontMapper::GetEncodingDescription(encAlt).c_str());
            }
        }
        else
            wxLogWarning(wxT("No fonts for encoding '%s' on this system."),
                         wxFontMapper::GetEncodingDescription(fontenc).c_str());
    }

    // and now create the correct font
    if ( !DoEnumerateFamilies(false, fontenc, true /* silent */) )
    {
        wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                    wxFONTWEIGHT_NORMAL, false /* !underlined */,
                    wxEmptyString /* facename */, fontenc);
        if ( font.Ok() )
        {
            DoChangeFont(font);
        }
        else
        {
            wxLogWarning(wxT("No fonts for encoding '%s' on this system."),
                         wxFontMapper::GetEncodingDescription(fontenc).c_str());
        }
    }
#endif // wxUSE_FILEDLG
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("wxWidgets font demo\n")
                 wxT("(c) 1999 Vadim Zeitlin"),
                 wxT("About Font"),
                 wxOK | wxICON_INFORMATION, this);
}

// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyCanvas, wxWindow)
    EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

MyCanvas::MyCanvas( wxWindow *parent )
        : wxWindow( parent, wxID_ANY ),
          m_colour(*wxRED), m_font(*wxNORMAL_FONT)
{
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc(this);
    PrepareDC(dc);

    // set background
    dc.SetBackground(wxBrush(wxT("white"), wxSOLID));
    dc.Clear();

    // one text line height
    wxCoord hLine = dc.GetCharHeight();

    // the current text origin
    wxCoord x = 5,
            y = 5;

    // output the font name/info
    wxString fontInfo;
    fontInfo.Printf(wxT("Font size is %d points, family: %s, encoding: %s"),
                    m_font.GetPointSize(),
                    m_font.GetFamilyString().c_str(),
                    wxFontMapper::
                        GetEncodingDescription(m_font.GetEncoding()).c_str());

    dc.DrawText(fontInfo, x, y);
    y += hLine;

    fontInfo.Printf(wxT("Style: %s, weight: %s, fixed width: %s"),
                    m_font.GetStyleString().c_str(),
                    m_font.GetWeightString().c_str(),
                    m_font.IsFixedWidth() ? _T("yes") : _T("no"));

    dc.DrawText(fontInfo, x, y);
    y += hLine;

    if ( m_font.Ok() )
    {
        const wxNativeFontInfo *info = m_font.GetNativeFontInfo();
        if ( info )
        {
            wxString fontDesc = m_font.GetNativeFontInfoUserDesc();
            fontInfo.Printf(wxT("Native font info: %s"), fontDesc.c_str());

            dc.DrawText(fontInfo, x, y);
            y += hLine;
        }
    }

    y += hLine;

    // prepare to draw the font
    dc.SetFont(m_font);
    dc.SetTextForeground(m_colour);

    // the size of one cell (Normally biggest char + small margin)
    long maxCharWidth, maxCharHeight;
    dc.GetTextExtent(wxT("W"), &maxCharWidth, &maxCharHeight);
    int w = maxCharWidth + 5,
        h = maxCharHeight + 4;


    // print all font symbols from 32 to 256 in 7 rows of 32 chars each
    for ( int i = 0; i < 7; i++ )
    {
        for ( int j = 0; j < 32; j++ )
        {
            wxChar c = (wxChar)(32 * (i + 1) + j);

            long charWidth, charHeight;
            dc.GetTextExtent(c, &charWidth, &charHeight);
            dc.DrawText
            (
                c,
                x + w*j + (maxCharWidth - charWidth) / 2 + 1,
                y + h*i + (maxCharHeight - charHeight) / 2
            );
        }
    }

    // draw the lines between them
    dc.SetPen(wxPen(wxColour(_T("blue")), 1, wxSOLID));
    int l;

    // horizontal
    for ( l = 0; l < 8; l++ )
    {
        int yl = y + h*l - 2;
        dc.DrawLine(x - 2, yl, x + 32*w - 1, yl);
    }

    // and vertical
    for ( l = 0; l < 33; l++ )
    {
        int xl = x + w*l - 2;
        dc.DrawLine(xl, y - 2, xl, y + 7*h - 1);
    }
}
