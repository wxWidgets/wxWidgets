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

    DECLARE_EVENT_TABLE()

    wxLocale& m_locale;
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ID for the menu commands
enum
{
    INTERNAT_QUIT = 1,
    INTERNAT_TEXT,
    INTERNAT_TEST,
    INTERNAT_OPEN
};

// ----------------------------------------------------------------------------
// wxWindows macros
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(INTERNAT_QUIT, MyFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(INTERNAT_TEST, MyFrame::OnPlay)
    EVT_MENU(INTERNAT_OPEN, MyFrame::OnOpen)
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

    static const wxLanguage langIds[] =
    {
        wxLANGUAGE_DEFAULT,
        wxLANGUAGE_FRENCH,
        wxLANGUAGE_GERMAN,
        wxLANGUAGE_RUSSIAN,
        wxLANGUAGE_JAPANESE,
        wxLANGUAGE_ENGLISH,
        wxLANGUAGE_ENGLISH_US,
    };

    if ( lng == -1 )
    {
        // note that it makes no sense to translate these strings, they are
        // shown before we set the locale anyhow
        const wxString langNames[] =
        {
            _T("System default"),
            _T("French"),
            _T("German"),
            _T("Russian"),
            _T("Japanese"),         // this will only work in Unicode build
            _T("English"),
            _T("English (U.S.)")
        };

        // the arrays should be in sync
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(langNames) == WXSIZEOF(langIds),
                               LangArraysMismatch );

        lng = wxGetSingleChoiceIndex
              (
                _T("Please choose language:"),
                _T("Language"), 
                WXSIZEOF(langNames),
                langNames
              );
    }

    if ( lng != -1 )
        m_locale.Init(langIds[lng]);


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
    file_menu->Append(INTERNAT_QUIT, _("E&xit"));

    wxMenu *test_menu = new wxMenu;
    test_menu->Append(INTERNAT_OPEN, _("&Open bogus file"));
    test_menu->Append(INTERNAT_TEST, _("&Play a game"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _("&File"));
    menu_bar->Append(test_menu, _("&Test"));
    frame->SetMenuBar(menu_bar);

    // Show the frame
    frame->Show(TRUE);
    SetTopWindow(frame);

    return TRUE;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// main frame constructor
MyFrame::MyFrame(wxLocale& locale)
       : wxFrame(NULL,
                 -1,
                 _("International wxWindows App"),
                 wxPoint(50, 50),
                 wxSize(350, 60)),
         m_locale(locale)
{
    // Empty
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString localeInfo;
    wxString locale = m_locale.GetLocale();
    wxString sysname = m_locale.GetSysName();
    wxString canname = m_locale.GetCanonicalName();

    localeInfo.Printf(_("Language: %s\nSystem locale name: %s\nCanonical locale name: %s\n"),
        locale.c_str(), sysname.c_str(), canname.c_str() );

    wxMessageDialog
    (
        this,
        wxString(_("I18n sample\n(c) 1998, 1999 Vadim Zeitlin and Julian Smart"))
            + wxT("\n\n")
            + localeInfo,
        _("About Internat"), 
        wxOK | wxICON_INFORMATION
    ).ShowModal();
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
    // open a bogus file -- the error message should be also translated if you've
    // got wxstd.mo somewhere in the search path
    wxFile file(wxT("NOTEXIST.ING"));
}

