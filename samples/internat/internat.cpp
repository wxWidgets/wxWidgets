/////////////////////////////////////////////////////////////////////////////
// Name:        internat.cpp
// Purpose:     Demonstrates internationalisation (i18n) support
// Author:      Vadim Zeitlin/Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/intl.h"
#include "wx/file.h"
#include "wx/log.h"

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
#include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type
class MyApp: public wxApp
{
public:
    virtual bool OnInit();

protected:
    wxLocale m_locale; // locale we'll be using
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyFrame(wxLocale& m_locale);

public:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnPlay(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnTest1(wxCommandEvent& event);
    void OnTest2(wxCommandEvent& event);
    void OnTest3(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

    wxLocale& m_locale;
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ID for the menu commands
enum
{
    INTERNAT_TEXT = wxID_HIGHEST + 1,
    INTERNAT_TEST,
    INTERNAT_TEST_1,
    INTERNAT_TEST_2,
    INTERNAT_TEST_3,
    INTERNAT_OPEN
};

// language data
static const wxLanguage langIds[] =
{
    wxLANGUAGE_DEFAULT,
    wxLANGUAGE_FRENCH,
    wxLANGUAGE_GERMAN,
    wxLANGUAGE_RUSSIAN,
    wxLANGUAGE_BULGARIAN,
    wxLANGUAGE_CZECH,
    wxLANGUAGE_POLISH,
    wxLANGUAGE_SWEDISH,
#if wxUSE_UNICODE || defined(__WXMOTIF__)
    wxLANGUAGE_JAPANESE,
#endif
#if wxUSE_UNICODE
    wxLANGUAGE_GEORGIAN,
    wxLANGUAGE_ENGLISH,
    wxLANGUAGE_ENGLISH_US,
    wxLANGUAGE_ARABIC,
    wxLANGUAGE_ARABIC_EGYPT
#endif
};

// note that it makes no sense to translate these strings, they are
// shown before we set the locale anyhow
const wxString langNames[] =
{
    _T("System default"),
    _T("French"),
    _T("German"),
    _T("Russian"),
    _T("Bulgarian"),
    _T("Czech"),
    _T("Polish"),
    _T("Swedish"),
#if wxUSE_UNICODE || defined(__WXMOTIF__)
    _T("Japanese"),
#endif
#if wxUSE_UNICODE
    _T("Georgian"),
    _T("English"),
    _T("English (U.S.)"),
    _T("Arabic"),
    _T("Arabic (Egypt)")
#endif
};

// the arrays must be in sync
wxCOMPILE_TIME_ASSERT( WXSIZEOF(langNames) == WXSIZEOF(langIds),
                       LangArraysMismatch );

// ----------------------------------------------------------------------------
// wxWidgets macros
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(INTERNAT_TEST, MyFrame::OnPlay)
    EVT_MENU(INTERNAT_OPEN, MyFrame::OnOpen)
    EVT_MENU(INTERNAT_TEST_1, MyFrame::OnTest1)
    EVT_MENU(INTERNAT_TEST_2, MyFrame::OnTest2)
    EVT_MENU(INTERNAT_TEST_3, MyFrame::OnTest3)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
    long lng = -1;

    if ( argc == 2 )
    {
        // the parameter must be the lang index
        wxString tmp(argv[1]);
        tmp.ToLong(&lng);
    }

    if ( lng == -1 )
    {
        lng = wxGetSingleChoiceIndex
              (
                _T("Please choose language:"),
                _T("Language"),
                WXSIZEOF(langNames),
                langNames
              );
    }

    if ( lng != -1 )
    {
        // don't use wxLOCALE_LOAD_DEFAULT flag so that Init() doesn't return
        // false just because it failed to load wxstd catalog
        if ( !m_locale.Init(langIds[lng], wxLOCALE_CONV_ENCODING) )
        {
            wxLogError(_T("This language is not supported by the system."));
            return false;
        }
    }

    // normally this wouldn't be necessary as the catalog files would be found
    // in the default locations, but when the program is not installed the
    // catalogs are in the build directory where we wouldn't find them by
    // default
    wxLocale::AddCatalogLookupPathPrefix(wxT("."));

    // Initialize the catalogs we'll be using
    m_locale.AddCatalog(wxT("internat"));

    // this catalog is installed in standard location on Linux systems and
    // shows that you may make use of the standard message catalogs as well
    //
    // if it's not installed on your system, it is just silently ignored
#ifdef __LINUX__
    {
        wxLogNull noLog;
        m_locale.AddCatalog(_T("fileutils"));
    }
#endif

    // Create the main frame window
    MyFrame *frame = new MyFrame(m_locale);

    // Give it an icon
    frame->SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(wxID_ABOUT, _("&About..."));
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, _("E&xit"));

    wxMenu *test_menu = new wxMenu;
    test_menu->Append(INTERNAT_OPEN, _("&Open bogus file"));
    test_menu->Append(INTERNAT_TEST, _("&Play a game"));
    test_menu->AppendSeparator();
    test_menu->Append(INTERNAT_TEST_1, _("&1 _() (gettext)"));
    test_menu->Append(INTERNAT_TEST_2, _("&2 _N() (ngettext)"));
    test_menu->Append(INTERNAT_TEST_3, _("&3 wxTRANSLATE() (gettext_noop)"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _("&File"));
    menu_bar->Append(test_menu, _("&Test"));
    frame->SetMenuBar(menu_bar);

    // Show the frame
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// main frame constructor
MyFrame::MyFrame(wxLocale& locale)
       : wxFrame(NULL,
                 wxID_ANY,
                 _("International wxWidgets App")),
         m_locale(locale)
{
    // this demonstrates RTL layout mirroring for Arabic locales
    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(new wxStaticText(this, wxID_ANY, _("First")),
                wxSizerFlags().Border());
    sizer->Add(new wxStaticText(this, wxID_ANY, _("Second")),
                wxSizerFlags().Border());
    SetSizer(sizer);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString localeInfo;
    wxString locale = m_locale.GetLocale();
    wxString sysname = m_locale.GetSysName();
    wxString canname = m_locale.GetCanonicalName();

    localeInfo.Printf(_("Language: %s\nSystem locale name:\n%s\nCanonical locale name: %s\n"),
        locale.c_str(), sysname.c_str(), canname.c_str() );

    wxMessageDialog dlg(
                        this,
                        wxString(_("I18n sample\n(c) 1998, 1999 Vadim Zeitlin and Julian Smart"))
                                 + wxT("\n\n")
                                 + localeInfo,
                                 _("About Internat"),
                        wxOK | wxICON_INFORMATION
                       );
    dlg.ShowModal();
}

void MyFrame::OnPlay(wxCommandEvent& WXUNUSED(event))
{
    wxString str = wxGetTextFromUser
                   (
                    _("Enter your number:"),
                    _("Try to guess my number!"),
                    wxEmptyString,
                    this
                   );

    if ( str.empty() )
    {
        // cancelled
        return;
    }

    long num;
    if ( !str.ToLong(&num) || num < 0 )
    {
        str = _("You've probably entered an invalid number.");
    }
    else if ( num == 9 )
    {
        // this message is not translated (not in catalog) because we used _T()
        // and not _() around it
        str = _T("You've found a bug in this program!");
    }
    else if ( num == 17 )
    {
        str.clear();

        // string must be split in two -- otherwise the translation would't be
        // found
        str << _("Congratulations! you've won. Here is the magic phrase:")
            << _("cannot create fifo `%s'");
    }
    else
    {
        // this is a more implicit way to write _() but note that if you use it
        // you must ensure that the strings get extracted in the message
        // catalog as by default xgettext won't do it (it only knows of _(),
        // not wxGetTranslation())
        str = wxGetTranslation(_T("Bad luck! try again..."));
    }

    wxMessageBox(str, _("Result"), wxOK | wxICON_INFORMATION);
}

void MyFrame::OnOpen(wxCommandEvent&)
{
    // open a bogus file -- the error message should be also translated if
    // you've got wxstd.mo somewhere in the search path
    wxFile file(wxT("NOTEXIST.ING"));
}

void MyFrame::OnTest1(wxCommandEvent& WXUNUSED(event))
{
    const wxChar* title = _("Testing _() (gettext)");
    wxTextEntryDialog d(this, _("Please enter text to translate"),
        title, wxTRANSLATE("default value"));
    if (d.ShowModal() == wxID_OK)
    {
        wxString v = d.GetValue();
        wxString s(title);
        s << _T("\n") << v << _T(" -> ")
            << wxGetTranslation(v.c_str()) << _T("\n");
        wxMessageBox(s);
    }
}

void MyFrame::OnTest2(wxCommandEvent& WXUNUSED(event))
{
    const wxChar* title = _("Testing _N() (ngettext)");
    wxTextEntryDialog d(this,
        _("Please enter range for plural forms of \"n files deleted\" phrase"),
        title, _T("0-10"));
    if (d.ShowModal() == wxID_OK)
    {
        int first, last;
        wxSscanf(d.GetValue(), _T("%d-%d"), &first, &last);
        wxString s(title);
        s << _T("\n");
        for (int n = first; n <= last; ++n)
        {
            s << n << _T(" ") <<
                wxPLURAL("file deleted", "files deleted", n) <<
                _T("\n");
        }
        wxMessageBox(s);
    }
}

void MyFrame::OnTest3(wxCommandEvent& WXUNUSED(event))
{
    const wxChar* lines[] =
    {
        wxTRANSLATE("line 1"),
        wxTRANSLATE("line 2"),
        wxTRANSLATE("line 3"),
    };
    wxString s(_("Testing wxTRANSLATE() (gettext_noop)"));
    s << _T("\n");
    for (size_t i = 0; i < WXSIZEOF(lines); ++i)
    {
        s << lines[i] << _T(" -> ") << wxGetTranslation(lines[i]) << _T("\n");
    }
    wxMessageBox(s);
}


