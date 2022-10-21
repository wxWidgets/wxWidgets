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

#include "wx/calctrl.h"
#include "wx/intl.h"
#include "wx/file.h"
#include "wx/grid.h"
#include "wx/log.h"
#include "wx/cmdline.h"
#include "wx/numformatter.h"
#include "wx/platinfo.h"
#include "wx/spinctrl.h"
#include "wx/translation.h"
#include "wx/uilocale.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// Under Linux we demonstrate loading an existing message catalog using
// coreutils package (which is always installed) as an example.
#ifdef __LINUX__
    #define USE_COREUTILS_MO
    static bool g_loadedCoreutilsMO = false;
#endif // __LINUX__

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type
class MyApp: public wxApp
{
public:
    MyApp() { m_setLocale = Locale_Ask; }

    virtual void OnInitCmdLine(wxCmdLineParser& parser) override;
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) override;
    virtual bool OnInit() override;

protected:
    // Specifies whether we should use the current locale or not. By default we
    // ask the user about it, but it's possible to override this using the
    // command line options.
    enum
    {
        Locale_Ask,
        Locale_Set,
        Locale_Skip
    } m_setLocale;
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyFrame();

public:
    void OnTestLocaleAvail(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
#ifdef USE_COREUTILS_MO
    void OnCoreutilsHelp(wxCommandEvent& event);
#endif // USE_COREUTILS_MO
    void OnQuit(wxCommandEvent& event);

    void OnPlay(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnTest1(wxCommandEvent& event);
    void OnTest2(wxCommandEvent& event);
    void OnTest3(wxCommandEvent& event);
    void OnTestMsgBox(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
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

// ----------------------------------------------------------------------------
// wxWidgets macros
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(INTERNAT_TEST, MyFrame::OnTestLocaleAvail)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
#ifdef USE_COREUTILS_MO
    EVT_MENU(wxID_HELP, MyFrame::OnCoreutilsHelp)
#endif // USE_COREUTILS_MO
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)

    EVT_MENU(INTERNAT_PLAY, MyFrame::OnPlay)
    EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
    EVT_MENU(wxID_SAVE, MyFrame::OnSave)
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

static const char* OPTION_NO_LOCALE = "no-locale";
static const char* OPTION_SET_LOCALE = "set-locale";

void MyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.AddSwitch("n", OPTION_NO_LOCALE,
                     _("skip setting locale on startup"));
    parser.AddSwitch("y", OPTION_SET_LOCALE,
                     _("do set locale on startup without asking"));

    wxApp::OnInitCmdLine(parser);
}

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if ( !wxApp::OnCmdLineParsed(parser) )
        return false;

    if ( parser.Found(OPTION_NO_LOCALE) )
    {
        m_setLocale = Locale_Skip;
    }

    if ( parser.Found(OPTION_SET_LOCALE) )
    {
        if ( m_setLocale == Locale_Skip )
        {
            wxLogWarning("--%s option overrides --%s",
                         OPTION_SET_LOCALE, OPTION_NO_LOCALE);
        }

        m_setLocale = Locale_Set;
    }

    return true;
}

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // For demonstration purposes only, ask the user if they want to run the
    // program using the current system language. In real programs, we would do
    // it unconditionally for localized programs -- or never do it at all for
    // the other ones.
    const wxLanguageInfo* const
        langInfo = wxUILocale::GetLanguageInfo(wxLANGUAGE_DEFAULT);
    const wxString
        langDesc = langInfo ? langInfo->Description
                            : wxString("the default system language");

    if ( m_setLocale == Locale_Ask )
    {
        m_setLocale = wxMessageBox
                      (
                        wxString::Format
                        (
                            "Would you like to use the program in %s?",
                            langDesc
                        ),
                        "wxWidgets i18n (internat) sample",
                        wxYES_NO
                      ) == wxYES ? Locale_Set : Locale_Skip;
    }

    if ( m_setLocale == Locale_Set )
    {
        if ( !wxUILocale::UseDefault() )
        {
            wxLogWarning("Failed to initialize the default system locale.");
        }


        // Independently of whether we succeeded to set the locale or not, try
        // to load the translations (for the default system language) here.

        // normally this wouldn't be necessary as the catalog files would be found
        // in the default locations, but when the program is not installed the
        // catalogs are in the build directory where we wouldn't find them by
        // default
        wxFileTranslationsLoader::AddCatalogLookupPathPrefix(".");

        // Create the object for message translation and set it up for global use.
        wxTranslations* const trans = new wxTranslations();
        wxTranslations::Set(trans);

        // Initialize the catalogs we'll be using.
        if ( !trans->AddCatalog("internat") )
        {
            wxLogError(_("Couldn't find/load 'internat' catalog for %s."),
                       langDesc);
        }

        // Now try to add wxstd.mo so that loading "NOTEXIST.ING" file will produce
        // a localized error message:
        trans->AddCatalog("wxstd");
            // NOTE: it's not an error if we couldn't find it!

        // this catalog is installed in standard location on Linux systems and
        // shows that you may make use of the standard message catalogs as well
        //
        // if it's not installed on your system, it is just silently ignored
#ifdef USE_COREUTILS_MO
        wxFileTranslationsLoader::AddCatalogLookupPathPrefix("/usr/share/locale");
        g_loadedCoreutilsMO = trans->AddCatalog("coreutils");
#endif // USE_COREUTILS_MO
    }

    // Create the main frame window
    MyFrame *frame = new MyFrame();

    // Show the frame
    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// main frame constructor
MyFrame::MyFrame()
       : wxFrame(nullptr,
                 wxID_ANY,
                 _("International wxWidgets App"))
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
    test_menu->Append(wxID_SAVE, _("&Save dummy file"), _("Shows a localized standard dialog"));
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
#ifdef USE_COREUTILS_MO
    help_menu->Append(wxID_HELP, _("Show coreutils &help"));
    help_menu->AppendSeparator();
#endif // USE_COREUTILS_MO
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

    wxPanel* const panel = new wxPanel(this);

    wxSizer* const topSizer = new wxBoxSizer(wxVERTICAL);

    // create controls showing the locale being used
    topSizer->Add(new wxStaticText
                      (
                        panel,
                        wxID_ANY,
                        wxString::Format
                        (
                         _("Current UI locale: %s; C locale: %s"),
                         wxUILocale::GetCurrent().GetName(),
                         setlocale(LC_ALL, nullptr)
                        )
                      ),
                  wxSizerFlags().Center().Border());

    topSizer->Add(new wxStaticText
                      (
                        panel,
                        wxID_ANY,
                        wxString::Format
                        (
                          _("UI locale name in English: %s"),
                          wxUILocale::GetCurrent().GetLocalizedName(wxLOCALE_NAME_LOCALE, wxLOCALE_FORM_ENGLISH)
                        )
                      ),
                  wxSizerFlags().Center().Border());

    topSizer->Add(new wxStaticText
                      (
                        panel,
                        wxID_ANY,
                        wxString::Format
                        (
                          _("... and in the locale language: %s"),
                          wxUILocale::GetCurrent().GetLocalizedName(wxLOCALE_NAME_LOCALE, wxLOCALE_FORM_NATIVE)
                        )
                      ),
                  wxSizerFlags().Center().Border());

    // create some controls affected by the locale

    // this demonstrates RTL layout mirroring for Arabic locales and using
    // locale-specific decimal separator in wxSpinCtrlDouble.
    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(new wxStaticText(panel, wxID_ANY, _("Numeric input:")),
               wxSizerFlags().Center().Border());

    wxSpinCtrlDouble* const spin = new wxSpinCtrlDouble(panel, wxID_ANY);
    spin->SetDigits(2);
    spin->SetValue(12.34);
    sizer->Add(spin, wxSizerFlags().Center().Border());

    topSizer->Add(sizer, wxSizerFlags().Center());

    // show that week days and months names are translated too
    topSizer->Add(new wxCalendarCtrl(panel, wxID_ANY),
                  wxSizerFlags().Center().Border());

    // another control using locale-specific number and date format
    wxGrid* const grid = new wxGrid(panel, wxID_ANY,
                                    wxDefaultPosition, wxDefaultSize,
                                    wxBORDER_SIMPLE);
    grid->CreateGrid(2, 2);
    grid->HideRowLabels();

    grid->SetColLabelValue(0, _("Number"));
    grid->SetColFormatFloat(0);
    grid->SetCellValue(0, 0, wxNumberFormatter::ToString(3.14159265, -1));

    grid->SetColLabelValue(1, _("Date"));
    grid->SetColFormatDate(1);
    grid->SetCellValue(0, 1, "Today");

    topSizer->Add(grid, wxSizerFlags().Center().Border());

    // show the difference (in decimal and thousand separator, hence use a
    // floating point number > 1000) between wxString::Format() and
    // wxNumberFormatter: the former uses the current C locale, while the
    // latter uses the UI locale
    topSizer->Add(new wxStaticText
                      (
                        panel,
                        wxID_ANY,
                        wxString::Format
                        (
                            _("Number in UI locale: %s; in C locale: %.2f"),
                            wxNumberFormatter::ToString(1234567.89, 2),
                            1234567.89
                        )
                      ),
                  wxSizerFlags().Center().Border());

    panel->SetSizer(topSizer);
    topSizer->SetSizeHints(this);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageDialog dlg(
                        this,
                        _("I18n sample\n(c) 1998, 1999 Vadim Zeitlin and Julian Smart"),
                        _("About Internat"),
                        wxOK | wxICON_INFORMATION
                       );
    dlg.ShowModal();
}

#ifdef USE_COREUTILS_MO

void MyFrame::OnCoreutilsHelp(wxCommandEvent& WXUNUSED(event))
{
    if ( g_loadedCoreutilsMO )
    {
        // Try showing translation of a message used by coreutils: notice that
        // this string isn't inside _(), as its translation is supposed to be
        // already present in the coreutils catalog, we don't need to extract
        // it from here.
        const char* const msg = "      --help     display this help and exit\n";
        wxLogMessage("Translation of coreutils help option description is:\n%s",
                     wxGetTranslation(msg));
    }
    else
    {
        wxLogMessage("Loading coreutils message catalog failed, set "
                     "WXTRACE=i18n to get more information about it.");
    }
}

#endif // USE_COREUTILS_MO

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

    wxUILocale uiLocale = wxUILocale::FromTag(s_locale);
    if (uiLocale.IsSupported())
    {
        wxLayoutDirection layout = uiLocale.GetLayoutDirection();
        wxString strLayout = (layout == wxLayout_RightToLeft) ? "RTL" : "LTR";
        wxString strLocale = uiLocale.GetLocalizedName(wxLOCALE_NAME_LOCALE, wxLOCALE_FORM_NATIVE);
        wxLogMessage(_("Locale \"%s\" is available.\nIdentifier: %s; Layout: %s\nEnglish name: %s\nLocalized name: %s"),
                     s_locale, uiLocale.GetName(), strLayout,
                     uiLocale.GetLocalizedName(wxLOCALE_NAME_LOCALE, wxLOCALE_FORM_ENGLISH),
                     uiLocale.GetLocalizedName(wxLOCALE_NAME_LOCALE, wxLOCALE_FORM_NATIVE));
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

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    // show this file dialog just to check that the locale-specific elements in
    // it (such as dates) follow the current locale convnetions
    wxSaveFileSelector(_("Dummy file dialog"), ".ext", "dummy", this);
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
