/////////////////////////////////////////////////////////////////////////////
// Name:        internat.cpp
// Purpose:     Demonstrates internationalisation (i18n) support
// Author:      Vadim Zeitlin/Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// NOTE: don't miss the "readme.txt" file which comes with this sample!



// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/intl.h"
#include "wx/file.h"
#include "wx/log.h"
#include "wx/cmdline.h"
#include "wx/platinfo.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type
class MyApp: public wxApp
{
public:
    MyApp() { m_lang = wxLANGUAGE_UNKNOWN; }

    virtual void OnInitCmdLine(wxCmdLineParser& parser) wxOVERRIDE;
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) wxOVERRIDE;
    virtual bool OnInit() wxOVERRIDE;

protected:
    wxLanguage m_lang;  // language specified by user
    wxLocale m_locale;  // locale we'll be using
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyFrame(wxLocale& m_locale);

public:
    void OnTestLocaleAvail(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

    void OnPlay(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnTest1(wxCommandEvent& event);
    void OnTest2(wxCommandEvent& event);
    void OnTest3(wxCommandEvent& event);
    void OnTestMsgBox(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();

    wxLocale& m_locale;
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ID for the menu commands
enum
{
    INTERNAT_TEST = wxID_HIGHEST + 1,
    INTERNAT_PLAY,
    INTERNAT_TEST_1,
    INTERNAT_TEST_2,
    INTERNAT_TEST_3,
    INTERNAT_TEST_MSGBOX,
    INTERNAT_MACRO_1,
    INTERNAT_MACRO_2,
    INTERNAT_MACRO_3,
    INTERNAT_MACRO_4,
    INTERNAT_MACRO_5,
    INTERNAT_MACRO_6,
    INTERNAT_MACRO_7,
    INTERNAT_MACRO_8,
    INTERNAT_MACRO_9
};

// language data
static const wxLanguage langIds[] =
{
    wxLANGUAGE_DEFAULT,
    wxLANGUAGE_FRENCH,
    wxLANGUAGE_ITALIAN,
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
    "System default",
    "French",
    "Italian",
    "German",
    "Russian",
    "Bulgarian",
    "Czech",
    "Polish",
    "Swedish",
#if wxUSE_UNICODE || defined(__WXMOTIF__)
    "Japanese",
#endif
#if wxUSE_UNICODE
    "Georgian",
    "English",
    "English (U.S.)",
    "Arabic",
    "Arabic (Egypt)"
#endif
};

// the arrays must be in sync
wxCOMPILE_TIME_ASSERT( WXSIZEOF(langNames) == WXSIZEOF(langIds),
                       LangArraysMismatch );

// ----------------------------------------------------------------------------
// wxWidgets macros
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(INTERNAT_TEST, MyFrame::OnTestLocaleAvail)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)

    EVT_MENU(INTERNAT_PLAY, MyFrame::OnPlay)
    EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
    EVT_MENU(INTERNAT_TEST_1, MyFrame::OnTest1)
    EVT_MENU(INTERNAT_TEST_2, MyFrame::OnTest2)
    EVT_MENU(INTERNAT_TEST_3, MyFrame::OnTest3)
    EVT_MENU(INTERNAT_TEST_MSGBOX, MyFrame::OnTestMsgBox)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

// command line arguments handling
void MyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.AddParam(_("locale"),
                    wxCMD_LINE_VAL_STRING,
                    wxCMD_LINE_PARAM_OPTIONAL);

    wxApp::OnInitCmdLine(parser);
}

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if ( !wxApp::OnCmdLineParsed(parser) )
        return false;

    if ( parser.GetParamCount() )
    {
        const wxString loc = parser.GetParam();
        const wxLanguageInfo * const lang = wxLocale::FindLanguageInfo(loc);
        if ( !lang )
        {
            wxLogError(_("Locale \"%s\" is unknown."), loc);
            return false;
        }

        m_lang = static_cast<wxLanguage>(lang->Language);
    }

    return true;
}

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    if ( m_lang == wxLANGUAGE_UNKNOWN )
    {
        int lng = wxGetSingleChoiceIndex
                  (
                    _("Please choose language:"),
                    _("Language"),
                    WXSIZEOF(langNames),
                    langNames
                  );
        m_lang = lng == -1 ? wxLANGUAGE_DEFAULT : langIds[lng];
    }

    // don't use wxLOCALE_LOAD_DEFAULT flag so that Init() doesn't return
    // false just because it failed to load wxstd catalog
    if ( !m_locale.Init(m_lang, wxLOCALE_DONT_LOAD_DEFAULT) )
    {
        wxLogWarning(_("This language is not supported by the system."));

        // continue nevertheless
    }

    // normally this wouldn't be necessary as the catalog files would be found
    // in the default locations, but when the program is not installed the
    // catalogs are in the build directory where we wouldn't find them by
    // default
    wxLocale::AddCatalogLookupPathPrefix(".");

    // Initialize the catalogs we'll be using
    const wxLanguageInfo* pInfo = wxLocale::GetLanguageInfo(m_lang);
    if (!m_locale.AddCatalog("internat"))
    {
        wxLogError(_("Couldn't find/load the 'internat' catalog for locale '%s'."),
                   pInfo ? pInfo->GetLocaleName() : _("unknown"));
    }

    // Now try to add wxstd.mo so that loading "NOTEXIST.ING" file will produce
    // a localized error message:
    m_locale.AddCatalog("wxstd");
        // NOTE: it's not an error if we couldn't find it!

    // this catalog is installed in standard location on Linux systems and
    // shows that you may make use of the standard message catalogs as well
    //
    // if it's not installed on your system, it is just silently ignored
#ifdef __LINUX__
    {
        wxLogNull noLog;
        m_locale.AddCatalog("fileutils");
    }
#endif

    // Create the main frame window
    MyFrame *frame = new MyFrame(m_locale);

    // Show the frame
    frame->Show(true);

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
    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(INTERNAT_TEST, _("&Test locale availability...\tCtrl-T"));
    file_menu->AppendSeparator();

    // since wxID_EXIT is a stock ID it will automatically get a translated help
    // string; nice isn't it?
    file_menu->Append(wxID_EXIT, _("E&xit"));

    wxMenu *test_menu = new wxMenu;
    test_menu->Append(wxID_OPEN, _("&Open bogus file"), _("Shows a wxWidgets localized error message"));
    test_menu->Append(INTERNAT_PLAY, _("&Play a game"), _("A little game; hint: 17 is a lucky number for many"));
    test_menu->AppendSeparator();
    test_menu->Append(INTERNAT_TEST_1, _("&1 _() (gettext)"), _("Tests the _() macro"));
    test_menu->Append(INTERNAT_TEST_2, _("&2 _N() (ngettext)"), _("Tests the _N() macro"));
    test_menu->Append(INTERNAT_TEST_3, _("&3 wxTRANSLATE() (gettext_noop)"), _("Tests the wxTRANSLATE() macro"));
    test_menu->Append(INTERNAT_TEST_MSGBOX, _("&Message box test"),
                      _("Tests message box buttons labels translation"));

    // Note that all these strings are currently "translated" only in French
    // catalog, so you need to use French locale to see them in action.
    wxMenu *macro_menu = new wxMenu;
    macro_menu->Append(INTERNAT_MACRO_1, _("item"));
    macro_menu->Append(INTERNAT_MACRO_2, wxGETTEXT_IN_CONTEXT("context_1", "item"));
    macro_menu->Append(INTERNAT_MACRO_3, wxGETTEXT_IN_CONTEXT("context_2", "item"));
    macro_menu->Append(INTERNAT_MACRO_4, wxPLURAL("sing", "plur", 1));
    macro_menu->Append(INTERNAT_MACRO_5, wxPLURAL("sing", "plur", 2));
    macro_menu->Append(INTERNAT_MACRO_6, wxGETTEXT_IN_CONTEXT_PLURAL("context_1", "sing", "plur", 1));
    macro_menu->Append(INTERNAT_MACRO_7, wxGETTEXT_IN_CONTEXT_PLURAL("context_1", "sing", "plur", 2));
    macro_menu->Append(INTERNAT_MACRO_8, wxGETTEXT_IN_CONTEXT_PLURAL("context_2", "sing", "plur", 1));
    macro_menu->Append(INTERNAT_MACRO_9, wxGETTEXT_IN_CONTEXT_PLURAL("context_2", "sing", "plur", 2));

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, _("&About"));

    wxMenuBar *menu_bar = new wxMenuBar;
    // Using stock label here means that it will be automatically translated.
    menu_bar->Append(file_menu, wxGetStockLabel(wxID_FILE));
    menu_bar->Append(test_menu, _("&Test"));
    menu_bar->Append(macro_menu, _("&Macro"));
    // We could have used wxGetStockLabel(wxID_HELP) here too, but show the
    // special case of "Help" menu: it has a special, Windows-specific
    // translation for some languages. Note that normally we would actually use
    // it only under MSW, we're doing it here unconditionally just for
    // demonstration purposes.
    menu_bar->Append(help_menu, wxGETTEXT_IN_CONTEXT("standard Windows menu", "&Help"));
    SetMenuBar(menu_bar);

    // this demonstrates RTL support in wxStatusBar:
    CreateStatusBar(1);

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

    localeInfo.Printf(_("Language: %s\nSystem locale name: %s\nCanonical locale name: %s\n"),
                      locale, sysname, canname );

    wxMessageDialog dlg(
                        this,
                        wxString(_("I18n sample\n(c) 1998, 1999 Vadim Zeitlin and Julian Smart"))
                                 + "\n\n"
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
        // this message is not translated (not in catalog) because we
        // did not put _() around it
        str = "You've found a bug in this program!";
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
        // catalog as by default xgettext won't do it; it only knows of _(),
        // not of wxTRANSLATE(). As internat's readme.txt says you should thus
        // call xgettext with -kwxTRANSLATE.
        str = wxGetTranslation(wxTRANSLATE("Bad luck! try again..."));

        // note also that if we want 'str' to contain a localized string
        // we need to use wxGetTranslation explicitly as wxTRANSLATE just
        // tells xgettext to extract the string but has no effect on the
        // runtime of the program!
    }

    wxMessageBox(str, _("Result"), wxOK | wxICON_INFORMATION);
}

void MyFrame::OnTestLocaleAvail(wxCommandEvent& WXUNUSED(event))
{
    static wxString s_locale;
    wxString locale = wxGetTextFromUser
                      (
                        _("Enter the locale to test"),
                        wxGetTextFromUserPromptStr,
                        s_locale,
                        this
                      );
    if ( locale.empty() )
        return;

    s_locale = locale;
    const wxLanguageInfo * const info = wxLocale::FindLanguageInfo(s_locale);
    if ( !info )
    {
        wxLogError(_("Locale \"%s\" is unknown."), s_locale);
        return;
    }

    if ( wxLocale::IsAvailable(info->Language) )
    {
        wxLogMessage(_("Locale \"%s\" is available."), s_locale);
    }
    else
    {
        wxLogWarning(_("Locale \"%s\" is not available."), s_locale);
    }
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    // open a bogus file -- the error message should be also translated if
    // you've got wxstd.mo somewhere in the search path (see MyApp::OnInit)
    wxFile file("NOTEXIST.ING");
}

void MyFrame::OnTest1(wxCommandEvent& WXUNUSED(event))
{
    const wxString& title = _("Testing _() (gettext)");

    // NOTE: using the wxTRANSLATE() macro here we won't show a localized
    //       string in the text entry dialog; we'll simply show the un-translated
    //       string; however if the user press "ok" without altering the text,
    //       since the "default value" string has been extracted by xgettext
    //       the wxGetTranslation call later will manage to return a localized
    //       string
    wxTextEntryDialog d(this, _("Please enter text to translate"),
                        title, wxTRANSLATE("default value"));

    if (d.ShowModal() == wxID_OK)
    {
        wxString v = d.GetValue();
        wxString s(title);
        s << "\n" << v << " -> "
            << wxGetTranslation(v.c_str()) << "\n";
        wxMessageBox(s);
    }
}

void MyFrame::OnTest2(wxCommandEvent& WXUNUSED(event))
{
    const wxString& title = _("Testing _N() (ngettext)");
    wxTextEntryDialog d(this,
        _("Please enter range for plural forms of \"n files deleted\" phrase"),
        title, "0-10");

    if (d.ShowModal() == wxID_OK)
    {
        int first, last;
        wxSscanf(d.GetValue(), "%d-%d", &first, &last);
        wxString s(title);
        s << "\n";
        for (int n = first; n <= last; ++n)
        {
            s << n << " " <<
                wxPLURAL("file deleted", "files deleted", n) <<
                "\n";
        }
        wxMessageBox(s);
    }
}

void MyFrame::OnTest3(wxCommandEvent& WXUNUSED(event))
{
    const char* lines[] =
    {
        wxTRANSLATE("line 1"),
        wxTRANSLATE("line 2"),
        wxTRANSLATE("line 3"),
    };

    wxString s(_("Testing wxTRANSLATE() (gettext_noop)"));
    s << "\n";
    for (size_t i = 0; i < WXSIZEOF(lines); ++i)
    {
        s << lines[i] << " -> " << wxGetTranslation(lines[i]) << "\n";
    }
    wxMessageBox(s);
}

void MyFrame::OnTestMsgBox(wxCommandEvent& WXUNUSED(event))
{
    if ( wxMessageBox
         (
            _("Are the labels of the buttons in this message box "
              "translated into the current locale language?"),
            _("wxWidgets i18n sample"),
            wxYES_NO,
            this
         ) != wxYES )
    {
        wxMessageBox(_("Please report the details of your platform to us."));
    }
}
