/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont demo
// Author:      Vadim Zeitlin
// Modified by:
// Created:     30.09.99
// Copyright:   (c) 1999 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all standard wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"

    #include "wx/log.h"
#endif

#include "wx/checkbox.h"
#include "wx/choicdlg.h"
#include "wx/fontdlg.h"
#include "wx/fontenum.h"
#include "wx/fontmap.h"
#include "wx/encconv.h"
#include "wx/sizer.h"
#include "wx/spinctrl.h"
#include "wx/splitter.h"
#include "wx/statline.h"
#include "wx/stdpaths.h"
#include "wx/textfile.h"
#include "wx/settings.h"

#include "../sample.xpm"

#ifdef __WXMAC__
    #undef wxFontDialog
    #include "wx/osx/fontdlg.h"
#endif

// used as title for several dialog boxes
static wxString GetSampleTitle()
{
    return "wxWidgets Font Sample";
}

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
    virtual bool OnInit() override;
};

// FontPanel contains controls allowing to specify the font properties
class FontPanel : public wxPanel
{
public:
    explicit FontPanel(wxWindow* parent);

    void ShowFont(const wxFont& font) { m_font = font; DoUpdate(); }

    wxFontInfo GetFontInfo() const;

private:
    // Update m_useXXX flags depending on which control was changed last.
    void OnFacename(wxCommandEvent& e) { m_useFamily = false; e.Skip(); }
    void OnFamily(wxCommandEvent& e) { m_useFamily = true; e.Skip(); }

    void OnWeightChoice(wxCommandEvent& e) { m_useNumericWeight = false; e.Skip(); }
    void OnWeightSpin(wxCommandEvent& e) { m_useNumericWeight = true; e.Skip(); }


    // Unlike wxFontXXX, the elements of these enum are consecutive, which is
    // more convenient here.
    enum Family
    {
        Family_Default,
        Family_Decorative,
        Family_Roman,
        Family_Script,
        Family_Swiss,
        Family_Modern,
        Family_Teletype
    };

    enum Style
    {
        Style_Normal,
        Style_Italic,
        Style_Slant
    };

    enum Weight
    {
        Weight_Thin,
        Weight_Extralight,
        Weight_Light,
        Weight_Normal,
        Weight_Medium,
        Weight_Semibold,
        Weight_Bold,
        Weight_Extrabold,
        Weight_Heavy,
        Weight_Extraheavy
    };

    void DoUpdate();

    wxFont m_font;

    wxTextCtrl* m_textFaceName;
    wxChoice* m_choiceFamily;
    wxSpinCtrlDouble* m_spinPointSize;
    wxChoice* m_choiceStyle;
    wxChoice* m_choiceWeight;
    wxSpinCtrl* m_spinWeight;
    wxCheckBox* m_checkUnderlined;
    wxCheckBox* m_checkStrikethrough;
    wxCheckBox* m_checkFixedWidth;
    wxStaticText* m_labelInfo;

    bool m_useFamily;
    bool m_useNumericWeight;
};

// FontCanvas shows the font characters.
class FontCanvas : public wxWindow
{
public:
    explicit FontCanvas( wxWindow *parent );

    // accessors for FontWindow
    const wxFont& GetTextFont() const { return m_font; }
    const wxColour& GetColour() const { return m_colour; }
    void SetTextFont(const wxFont& font) { m_font = font; }
    void SetColour(const wxColour& colour) { m_colour = colour; }

    // event handlers
    void OnPaint( wxPaintEvent &event );

protected:
    virtual wxSize DoGetBestClientSize() const override
    {
        return wxSize(80*GetCharWidth(), 15*GetCharHeight());
    }

private:
    wxColour m_colour;
    wxFont   m_font;

    wxDECLARE_EVENT_TABLE();
};

// FontWindow contains both FontPanel and FontCanvas
class FontWindow : public wxWindow
{
public:
    explicit FontWindow(wxWindow *parent);

    const wxFont& GetTextFont() const { return m_canvas->GetTextFont(); }
    const wxColour& GetColour() const { return m_canvas->GetColour(); }

    wxFont MakeNewFont() const { return m_panel->GetFontInfo(); }

    void UpdateFont(const wxFont& font, const wxColour& colour);

private:
    FontPanel* const m_panel;
    FontCanvas* const m_canvas;
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnGetBaseFont(wxCommandEvent& WXUNUSED(event))
        { DoChangeFont(m_fontWindow->GetTextFont().GetBaseFont()); }
    void OnIncFont(wxCommandEvent& WXUNUSED(event)) { DoResizeFont(+2); }
    void OnDecFont(wxCommandEvent& WXUNUSED(event)) { DoResizeFont(-2); }

    void OnBold(wxCommandEvent& event);
    void OnLight(wxCommandEvent& event);

    void OnItalic(wxCommandEvent& event);
    void OnSlant(wxCommandEvent& event);

    void OnUnderline(wxCommandEvent& event);
    void OnStrikethrough(wxCommandEvent& event);

    void OnwxPointerFont(wxCommandEvent& event);
    void OnFontDefault(wxCommandEvent& event);
    void OnwxSystemSettingsFont(wxCommandEvent& event);

    void OnTestTextValue(wxCommandEvent& event);
    void OnViewMsg(wxCommandEvent& event);
    void OnSelectFont(wxCommandEvent& event);
    void OnEnumerateFamiliesForEncoding(wxCommandEvent& event);
    void OnEnumerateFamilies(wxCommandEvent& WXUNUSED(event))
        { DoEnumerateFamilies(false); }
    void OnEnumerateFixedFamilies(wxCommandEvent& WXUNUSED(event))
        { DoEnumerateFamilies(true); }
    void OnEnumerateEncodings(wxCommandEvent& event);

    void OnSetNativeDesc(wxCommandEvent& event);
    void OnSetNativeUserDesc(wxCommandEvent& event);

    void OnSetFamily(wxCommandEvent& event);
    void OnSetFaceName(wxCommandEvent& event);
    void OnSetEncoding(wxCommandEvent& event);
    void OnPrivateFont(wxCommandEvent& event);

    void OnFontPanelApply(wxCommandEvent& WXUNUSED(event))
        { DoChangeFont(m_fontWindow->MakeNewFont()); }

protected:
    bool DoEnumerateFamilies(bool fixedWidthOnly,
                             wxFontEncoding encoding = wxFONTENCODING_SYSTEM,
                             bool silent = false);

    void DoResizeFont(int diff);
    void DoChangeFont(const wxFont& font, const wxColour& col = wxNullColour);

    // ask the user to choose an encoding and return it or
    // wxFONTENCODING_SYSTEM if the dialog was cancelled
    wxFontEncoding GetEncodingFromUser();

    // ask the user to choose a font family and return it or
    // wxFONTFAMILY_DEFAULT if the dialog was cancelled
    wxFontFamily GetFamilyFromUser();

    wxTextCtrl *m_textctrl;
    FontWindow *m_fontWindow;

private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Font_Quit = wxID_EXIT,
    Font_About = wxID_ABOUT,

    Font_ViewMsg = wxID_HIGHEST,
    Font_TestTextValue,

    Font_IncSize,
    Font_DecSize,

    Font_GetBaseFont,

    Font_Bold,
    Font_Light,

    Font_Italic,
    Font_Slant,

    Font_Underlined,
    Font_Strikethrough,

    // standard global wxFont objects:
    Font_wxNORMAL_FONT,
    Font_wxSMALL_FONT,
    Font_wxITALIC_FONT,
    Font_wxSWISS_FONT,
    Font_wxFont_Default,
    Font_Standard,

    // wxSystemSettings::GetFont possible objects:
    Font_wxSYS_OEM_FIXED_FONT,
    Font_wxSYS_ANSI_FIXED_FONT,
    Font_wxSYS_ANSI_VAR_FONT,
    Font_wxSYS_SYSTEM_FONT,
    Font_wxSYS_DEVICE_DEFAULT_FONT,
    Font_wxSYS_DEFAULT_GUI_FONT,
    Font_SystemSettings,

    Font_Choose = 100,
    Font_EnumFamiliesForEncoding,
    Font_EnumFamilies,
    Font_EnumFixedFamilies,
    Font_EnumEncodings,
    Font_SetNativeDesc,
    Font_SetNativeUserDesc,
    Font_SetFamily,
    Font_SetFaceName,
    Font_SetEncoding,

    Font_Private,
    Font_Max
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Font_Quit,  MyFrame::OnQuit)
    EVT_MENU(Font_TestTextValue, MyFrame::OnTestTextValue)
    EVT_MENU(Font_ViewMsg, MyFrame::OnViewMsg)
    EVT_MENU(Font_About, MyFrame::OnAbout)

    EVT_MENU(Font_GetBaseFont, MyFrame::OnGetBaseFont)
    EVT_MENU(Font_IncSize, MyFrame::OnIncFont)
    EVT_MENU(Font_DecSize, MyFrame::OnDecFont)

    EVT_MENU(Font_Bold, MyFrame::OnBold)
    EVT_MENU(Font_Light, MyFrame::OnLight)

    EVT_MENU(Font_Italic, MyFrame::OnItalic)
    EVT_MENU(Font_Slant, MyFrame::OnSlant)

    EVT_MENU(Font_Underlined, MyFrame::OnUnderline)
    EVT_MENU(Font_Strikethrough, MyFrame::OnStrikethrough)

    EVT_MENU(Font_wxNORMAL_FONT, MyFrame::OnwxPointerFont)
    EVT_MENU(Font_wxSMALL_FONT, MyFrame::OnwxPointerFont)
    EVT_MENU(Font_wxITALIC_FONT, MyFrame::OnwxPointerFont)
    EVT_MENU(Font_wxSWISS_FONT, MyFrame::OnwxPointerFont)
    EVT_MENU(Font_wxFont_Default, MyFrame::OnFontDefault)

    EVT_MENU(Font_wxSYS_OEM_FIXED_FONT, MyFrame::OnwxSystemSettingsFont)
    EVT_MENU(Font_wxSYS_ANSI_FIXED_FONT, MyFrame::OnwxSystemSettingsFont)
    EVT_MENU(Font_wxSYS_ANSI_VAR_FONT, MyFrame::OnwxSystemSettingsFont)
    EVT_MENU(Font_wxSYS_SYSTEM_FONT, MyFrame::OnwxSystemSettingsFont)
    EVT_MENU(Font_wxSYS_DEVICE_DEFAULT_FONT, MyFrame::OnwxSystemSettingsFont)
    EVT_MENU(Font_wxSYS_DEFAULT_GUI_FONT, MyFrame::OnwxSystemSettingsFont)

    EVT_MENU(Font_SetNativeDesc, MyFrame::OnSetNativeDesc)
    EVT_MENU(Font_SetNativeUserDesc, MyFrame::OnSetNativeUserDesc)
    EVT_MENU(Font_SetFamily, MyFrame::OnSetFamily)
    EVT_MENU(Font_SetFaceName, MyFrame::OnSetFaceName)
    EVT_MENU(Font_SetEncoding, MyFrame::OnSetEncoding)

    EVT_MENU(Font_Choose, MyFrame::OnSelectFont)
    EVT_MENU(Font_EnumFamiliesForEncoding, MyFrame::OnEnumerateFamiliesForEncoding)
    EVT_MENU(Font_EnumFamilies, MyFrame::OnEnumerateFamilies)
    EVT_MENU(Font_EnumFixedFamilies, MyFrame::OnEnumerateFixedFamilies)
    EVT_MENU(Font_EnumEncodings, MyFrame::OnEnumerateEncodings)
    EVT_MENU(Font_Private, MyFrame::OnPrivateFont)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main application window
    MyFrame *frame = new MyFrame();

    // Show it
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned 'false' here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame()
       : wxFrame(nullptr, wxID_ANY, "wxWidgets font sample")
{
    SetIcon(wxICON(sample));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(Font_TestTextValue, "&Test text value",
                     "Verify that getting and setting text value doesn't change it");
    menuFile->Append(Font_ViewMsg, "&View...\tCtrl-V",
                     "View an email message file");
    menuFile->AppendSeparator();
    menuFile->Append(Font_About, "&About\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(Font_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *menuFont = new wxMenu;
    menuFont->Append(Font_IncSize, "&Increase font size by 2 points\tCtrl-I");
    menuFont->Append(Font_DecSize, "&Decrease font size by 2 points\tCtrl-D");
    menuFont->Append(Font_GetBaseFont, "Use &base version of the font\tCtrl-0");
    menuFont->AppendSeparator();
    menuFont->AppendCheckItem(Font_Bold, "&Bold\tCtrl-B", "Toggle bold state");
    menuFont->AppendCheckItem(Font_Light, "&Light\tCtrl-L", "Toggle light state");
    menuFont->AppendSeparator();
    menuFont->AppendCheckItem(Font_Italic, "&Oblique\tCtrl-O", "Toggle italic state");
#ifndef __WXMSW__
    // under wxMSW slant == italic so there's no reason to provide another menu item for the same thing
    menuFont->AppendCheckItem(Font_Slant, "&Slant\tCtrl-S", "Toggle slant state");
#endif
    menuFont->AppendSeparator();
    menuFont->AppendCheckItem(Font_Underlined, "&Underlined\tCtrl-U",
                              "Toggle underlined state");
    menuFont->AppendCheckItem(Font_Strikethrough, "&Strikethrough",
                              "Toggle strikethrough state");

    menuFont->AppendSeparator();
    menuFont->Append(Font_SetNativeDesc,
                     "Set native font &description\tShift-Ctrl-D");
    menuFont->Append(Font_SetNativeUserDesc,
                     "Set &user font description\tShift-Ctrl-U");
    menuFont->AppendSeparator();
    menuFont->Append(Font_SetFamily, "Set font family");
    menuFont->Append(Font_SetFaceName, "Set font face name");
    menuFont->Append(Font_SetEncoding, "Set font &encoding\tShift-Ctrl-E");

    wxMenu *menuSelect = new wxMenu;
    menuSelect->Append(Font_Choose, "&Select font...\tCtrl-S",
                       "Select a standard font");

    wxMenu *menuStdFonts = new wxMenu;
    menuStdFonts->Append(Font_wxNORMAL_FONT, "wxNORMAL_FONT", "Normal font used by wxWidgets");
    menuStdFonts->Append(Font_wxSMALL_FONT,  "wxSMALL_FONT",  "Small font used by wxWidgets");
    menuStdFonts->Append(Font_wxITALIC_FONT, "wxITALIC_FONT", "Italic font used by wxWidgets");
    menuStdFonts->Append(Font_wxSWISS_FONT,  "wxSWISS_FONT",  "Swiss font used by wxWidgets");
    menuStdFonts->Append(Font_wxFont_Default,  "wxFont()",  "wxFont constructed from default wxFontInfo");
    menuSelect->Append(Font_Standard, "Standar&d fonts", menuStdFonts);

    wxMenu *menuSettingFonts = new wxMenu;
    menuSettingFonts->Append(Font_wxSYS_OEM_FIXED_FONT, "wxSYS_OEM_FIXED_FONT",
                         "Original equipment manufacturer dependent fixed-pitch font.");
    menuSettingFonts->Append(Font_wxSYS_ANSI_FIXED_FONT,  "wxSYS_ANSI_FIXED_FONT",
                         "Windows fixed-pitch (monospaced) font. ");
    menuSettingFonts->Append(Font_wxSYS_ANSI_VAR_FONT, "wxSYS_ANSI_VAR_FONT",
                         "Windows variable-pitch (proportional) font.");
    menuSettingFonts->Append(Font_wxSYS_SYSTEM_FONT,  "wxSYS_SYSTEM_FONT",
                         "System font.");
    menuSettingFonts->Append(Font_wxSYS_DEVICE_DEFAULT_FONT,  "wxSYS_DEVICE_DEFAULT_FONT",
                         "Device-dependent font.");
    menuSettingFonts->Append(Font_wxSYS_DEFAULT_GUI_FONT,  "wxSYS_DEFAULT_GUI_FONT",
                         "Default font for user interface objects such as menus and dialog boxes. ");
    menuSelect->Append(Font_SystemSettings, "System fonts", menuSettingFonts);

    menuSelect->AppendSeparator();
    menuSelect->Append(Font_EnumFamilies, "Enumerate font &families\tCtrl-F");
    menuSelect->Append(Font_EnumFixedFamilies,
                     "Enumerate fi&xed font families\tCtrl-X");
    menuSelect->Append(Font_EnumEncodings,
                     "Enumerate &encodings\tCtrl-E");
    menuSelect->Append(Font_EnumFamiliesForEncoding,
                     "Find font for en&coding...\tCtrl-C",
                     "Find font families for given encoding");

#if wxUSE_PRIVATE_FONTS
    // Try to use a private font, under most platforms we just look for it in
    // the current directory but under OS X it must be in a specific location
    // so look for it there.
    //
    // For OS X you also need to ensure that you actually do put wxprivate.ttf
    // in font.app/Contents/Resources/Fonts and add the following snippet
    //
    //     <plist version="0.9">
    //       <dict>
    //         ...
    //         <key>ATSApplicationFontsPath</key>
    //         <string>Fonts</string>
    //         ...
    //       </dict>
    //     </plist>
    //
    // to your font.app/Contents/Info.plist.

    wxString privfont;
#ifdef __WXOSX__
    privfont << wxStandardPaths::Get().GetResourcesDir() << "/Fonts/";
#endif
    privfont << "wxprivate.ttf";

    if ( !wxFont::AddPrivateFont(privfont) )
    {
        wxLogWarning("Failed to add private font from \"%s\"", privfont);
    }
    else
    {
        menuSelect->AppendSeparator();
        menuSelect->Append(Font_Private,
                           "Select private font",
                           "Select a font available only in this application");
    }
#endif // wxUSE_PRIVATE_FONTS


    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuFont, "F&ont");
    menuBar->Append(menuSelect, "&Select");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    wxSplitterWindow *splitter = new wxSplitterWindow(this);

    m_fontWindow = new FontWindow(splitter);

    m_fontWindow->Bind(wxEVT_BUTTON, &MyFrame::OnFontPanelApply, this);

    m_textctrl = new wxTextCtrl(splitter, wxID_ANY,
                                "Paste text here to see how it looks\nlike in the given font",
                                wxDefaultPosition,
                                wxSize(-1, 6*GetCharHeight()),
                                wxTE_MULTILINE);

    splitter->SplitHorizontally(m_fontWindow, m_textctrl, 0);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets font demo!");
#endif // wxUSE_STATUSBAR

    SetClientSize(splitter->GetBestSize());
    splitter->SetSashPosition(m_fontWindow->GetBestSize().y);
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
                                const wxString& encoding) override
    {
        wxString text;
        text.Printf("Encoding %u: %s (available in facename '%s')\n",
                    (unsigned int) ++m_n, encoding, facename);
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

    wxLogMessage("Enumerating all available encodings:\n%s",
                 fontEnumerator.GetText());
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
    virtual bool OnFacename(const wxString& facename) override
    {
        m_facenames.Add(facename);
        return true;
    }

    private:
        wxArrayString m_facenames;
};

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
            wxLogStatus(this, "Found %d %sfonts",
                        nFacenames, fixedWidthOnly ? "fixed width " : "");
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
            wxArrayString facenames;
            facenames.Alloc(nFacenames);
            int n;
            for ( n = 0; n < nFacenames; n++ )
                facenames.Add(fontEnumerator.GetFacenames().Item(n));

            // it's more convenient to see the fonts in alphabetical order
            facenames.Sort();

            n = wxGetSingleChoiceIndex
                (
                    "Choose a facename",
                    GetSampleTitle(),
                    facenames,
                    this
                );

            if ( n != -1 )
                facename = facenames[n];
        }

        if ( !facename.empty() )
        {
            wxFont font(wxFontInfo().FaceName(facename).Encoding(encoding));

            DoChangeFont(font);
        }

        return true;
    }
    else if ( !silent )
    {
        wxLogWarning("No such fonts found.");
    }

    return false;
}

void MyFrame::OnEnumerateFamiliesForEncoding(wxCommandEvent& WXUNUSED(event))
{
    wxFontEncoding enc = GetEncodingFromUser();
    if ( enc != wxFONTENCODING_SYSTEM )
    {
        DoEnumerateFamilies(false, enc);
    }
}

void MyFrame::OnSetNativeDesc(wxCommandEvent& WXUNUSED(event))
{
    wxString fontInfo = wxGetTextFromUser
                        (
                            "Enter native font string",
                            "Input font description",
                            m_fontWindow->GetTextFont().GetNativeFontInfoDesc(),
                            this
                        );
    if ( fontInfo.empty() )
        return;     // user clicked "Cancel" - do nothing

    wxFont font;
    font.SetNativeFontInfo(fontInfo);
    if ( !font.IsOk() )
    {
        wxLogError("Font info string \"%s\" is invalid.",
                   fontInfo);
        return;
    }

    DoChangeFont(font);
}

void MyFrame::OnSetNativeUserDesc(wxCommandEvent& WXUNUSED(event))
{
    wxString fontdesc = m_fontWindow->GetTextFont().GetNativeFontInfoUserDesc();
    wxString fontUserInfo = wxGetTextFromUser(
            "Here you can edit current font description",
            "Input font description", fontdesc,
            this);
    if (fontUserInfo.IsEmpty())
        return;     // user clicked "Cancel" - do nothing

    wxFont font;
    if (font.SetNativeFontInfoUserDesc(fontUserInfo))
    {
        wxASSERT_MSG(font.IsOk(), "The font should now be valid");
        DoChangeFont(font);
    }
    else
    {
        wxASSERT_MSG(!font.IsOk(), "The font should now be invalid");
        wxMessageBox("Error trying to create a font with such description...");
    }
}

void MyFrame::OnSetFamily(wxCommandEvent& WXUNUSED(event))
{
    wxFontFamily f = GetFamilyFromUser();

    wxFont font = m_fontWindow->GetTextFont();
    font.SetFamily(f);
    DoChangeFont(font);
}

void MyFrame::OnSetFaceName(wxCommandEvent& WXUNUSED(event))
{
    wxString facename = m_fontWindow->GetTextFont().GetFaceName();
    wxString newFaceName = wxGetTextFromUser(
            "Here you can edit current font face name.",
            "Input font facename", facename,
            this);
    if (newFaceName.IsEmpty())
        return;     // user clicked "Cancel" - do nothing

    wxFont font(m_fontWindow->GetTextFont());
    if (font.SetFaceName(newFaceName))      // change facename only
    {
        wxASSERT_MSG(font.IsOk(), "The font should now be valid");
        DoChangeFont(font);
    }
    else
    {
        wxASSERT_MSG(!font.IsOk(), "The font should now be invalid");
        wxMessageBox("There is no font with such face name...",
                     "Invalid face name", wxOK|wxICON_ERROR, this);
    }
}

void MyFrame::OnSetEncoding(wxCommandEvent& WXUNUSED(event))
{
    wxFontEncoding enc = GetEncodingFromUser();
    if ( enc == wxFONTENCODING_SYSTEM )
        return;

    wxFont font = m_fontWindow->GetTextFont();
    font.SetEncoding(enc);
    DoChangeFont(font);
}

wxFontEncoding MyFrame::GetEncodingFromUser()
{
    wxArrayString names;
    wxArrayInt encodings;

    const size_t count = wxFontMapper::GetSupportedEncodingsCount();
    names.reserve(count);
    encodings.reserve(count);

    for ( size_t n = 0; n < count; n++ )
    {
        wxFontEncoding enc = wxFontMapper::GetEncoding(n);
        encodings.push_back(enc);
        names.push_back(wxFontMapper::GetEncodingName(enc));
    }

    int i = wxGetSingleChoiceIndex
            (
                "Choose the encoding",
                GetSampleTitle(),
                names,
                this
            );

    return i == -1 ? wxFONTENCODING_SYSTEM : (wxFontEncoding)encodings[i];
}

wxFontFamily MyFrame::GetFamilyFromUser()
{
    wxArrayString names;
    wxArrayInt families;

    families.push_back(wxFONTFAMILY_DECORATIVE);
    families.push_back(wxFONTFAMILY_ROMAN);
    families.push_back(wxFONTFAMILY_SCRIPT);
    families.push_back(wxFONTFAMILY_SWISS);
    families.push_back(wxFONTFAMILY_MODERN);
    families.push_back(wxFONTFAMILY_TELETYPE);

    names.push_back("DECORATIVE");
    names.push_back("ROMAN");
    names.push_back("SCRIPT");
    names.push_back("SWISS");
    names.push_back("MODERN");
    names.push_back("TELETYPE");

    int i = wxGetSingleChoiceIndex
            (
                "Choose the family",
                GetSampleTitle(),
                names,
                this
            );

    return i == -1 ? wxFONTFAMILY_DEFAULT : (wxFontFamily)families[i];
}

void MyFrame::DoResizeFont(int diff)
{
    wxFont font = m_fontWindow->GetTextFont();

    font.SetPointSize(font.GetPointSize() + diff);
    DoChangeFont(font);
}

void MyFrame::OnBold(wxCommandEvent& event)
{
    wxFont font = m_fontWindow->GetTextFont();

    font.SetWeight(event.IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    DoChangeFont(font);
}

void MyFrame::OnLight(wxCommandEvent& event)
{
    wxFont font = m_fontWindow->GetTextFont();

    font.SetWeight(event.IsChecked() ? wxFONTWEIGHT_LIGHT : wxFONTWEIGHT_NORMAL);
    DoChangeFont(font);
}

void MyFrame::OnItalic(wxCommandEvent& event)
{
    wxFont font = m_fontWindow->GetTextFont();

    font.SetStyle(event.IsChecked() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
    DoChangeFont(font);
}

void MyFrame::OnSlant(wxCommandEvent& event)
{
    wxFont font = m_fontWindow->GetTextFont();

    font.SetStyle(event.IsChecked() ? wxFONTSTYLE_SLANT : wxFONTSTYLE_NORMAL);
    DoChangeFont(font);
}

void MyFrame::OnUnderline(wxCommandEvent& event)
{
    wxFont font = m_fontWindow->GetTextFont();

    font.SetUnderlined(event.IsChecked());
    DoChangeFont(font);
}

void MyFrame::OnStrikethrough(wxCommandEvent& event)
{
    wxFont font = m_fontWindow->GetTextFont();
    font.SetStrikethrough(event.IsChecked());
    DoChangeFont(font);
}

void MyFrame::OnwxPointerFont(wxCommandEvent& event)
{
    wxFont font;

    switch ( event.GetId() )
    {
        case Font_wxNORMAL_FONT:
            font = *wxNORMAL_FONT;
            break;

        case Font_wxSMALL_FONT:
            font = *wxSMALL_FONT;
            break;

        case Font_wxITALIC_FONT:
            font = *wxITALIC_FONT;
            break;

        case Font_wxSWISS_FONT:
            font = *wxSWISS_FONT;
            break;

        default:
            wxFAIL_MSG( "unknown standard font" );
            return;
    }

    DoChangeFont(font);
}

void MyFrame::OnFontDefault(wxCommandEvent& WXUNUSED(event))
{
    DoChangeFont(wxFont(wxFontInfo()));
}

void MyFrame::OnwxSystemSettingsFont(wxCommandEvent& event)
{
    wxFont font;

    switch ( event.GetId() )
    {
        case Font_wxSYS_OEM_FIXED_FONT:
            font = wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT);
            break;

        case Font_wxSYS_ANSI_FIXED_FONT:
            font = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
            break;

        case Font_wxSYS_ANSI_VAR_FONT:
            font = wxSystemSettings::GetFont(wxSYS_ANSI_VAR_FONT);
            break;

        case Font_wxSYS_SYSTEM_FONT:
            font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
            break;

        case Font_wxSYS_DEVICE_DEFAULT_FONT:
            font = wxSystemSettings::GetFont(wxSYS_DEVICE_DEFAULT_FONT);
            break;

        case Font_wxSYS_DEFAULT_GUI_FONT:
            font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
            break;

        default:
            wxFAIL_MSG( "unknown standard font" );
            return;
    }

    DoChangeFont(font);
}

void MyFrame::DoChangeFont(const wxFont& font, const wxColour& col)
{
    m_fontWindow->UpdateFont(font, col);

    m_textctrl->SetFont(font);
    if ( col.IsOk() )
        m_textctrl->SetForegroundColour(col);
    m_textctrl->Refresh();

    // update the state of the bold/italic/underlined menu items
    wxMenuBar *mbar = GetMenuBar();
    if ( mbar )
    {
        mbar->Check(Font_Light, font.GetWeight() == wxFONTWEIGHT_LIGHT);
        mbar->Check(Font_Bold, font.GetWeight() == wxFONTWEIGHT_BOLD);

        mbar->Check(Font_Italic, font.GetStyle() == wxFONTSTYLE_ITALIC);
#ifndef __WXMSW__
        mbar->Check(Font_Slant, font.GetStyle() == wxFONTSTYLE_SLANT);
#endif

        mbar->Check(Font_Underlined, font.GetUnderlined());
        mbar->Check(Font_Strikethrough, font.GetStrikethrough());
    }
}

void MyFrame::OnSelectFont(wxCommandEvent& WXUNUSED(event))
{
    wxFontData data;
    data.SetInitialFont(m_fontWindow->GetTextFont());
    data.SetColour(m_fontWindow->GetColour());

    wxFontDialog dialog(this, data);
    if ( dialog.ShowModal() == wxID_OK )
    {
        wxFontData retData = dialog.GetFontData();
        wxFont font = retData.GetChosenFont();
        wxColour colour = retData.GetColour();

        DoChangeFont(font, colour);
    }
}

void MyFrame::OnPrivateFont(wxCommandEvent& WXUNUSED(event))
{
    wxFont font(m_fontWindow->GetTextFont());
    if (font.SetFaceName("wxprivate"))
    {
        wxASSERT_MSG( font.IsOk(), "The font should now be valid") ;
        DoChangeFont(font);
    }
    else
    {
        wxLogError("Failed to use private font.");
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnTestTextValue(wxCommandEvent& WXUNUSED(event))
{
    wxString value = m_textctrl->GetValue();
    m_textctrl->SetValue(value);
    if ( m_textctrl->GetValue() != value )
    {
        wxLogError("Text value changed after getting and setting it");
    }
}

void MyFrame::OnViewMsg(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_FILEDLG
    // first, choose the file
    static wxString s_dir, s_file;
    wxFileDialog dialog(this, "Open an email message file",
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

    wxString prefix = "Content-Type: text/plain; charset=";
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
            if ( *pc == '"')
                pc++;

            while ( *pc && *pc != '"')
            {
                charset += *pc++;
            }

            break;
        }
    }

    if ( !charset )
    {
        wxLogError("The file '%s' doesn't contain charset information.",
                   filename);

        return;
    }

    // ok, now get the corresponding encoding
    wxFontEncoding fontenc = wxFontMapper::Get()->CharsetToEncoding(charset);
    if ( fontenc == wxFONTENCODING_SYSTEM )
    {
        wxLogError("Charset '%s' is unsupported.", charset);
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
                wxLogWarning("Cannot convert from '%s' to '%s'.",
                             wxFontMapper::GetEncodingDescription(fontenc),
                             wxFontMapper::GetEncodingDescription(encAlt));
            }
        }
        else
            wxLogWarning("No fonts for encoding '%s' on this system.",
                         wxFontMapper::GetEncodingDescription(fontenc));
    }

    // and now create the correct font
    if ( !DoEnumerateFamilies(false, fontenc, true /* silent */) )
    {
        wxFont font(wxFontInfo(wxNORMAL_FONT->GetPointSize()).Encoding(fontenc));
        if ( font.IsOk() )
        {
            DoChangeFont(font);
        }
        else
        {
            wxLogWarning("No fonts for encoding '%s' on this system.",
                         wxFontMapper::GetEncodingDescription(fontenc));
        }
    }
#endif // wxUSE_FILEDLG
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("wxWidgets font sample\n"
                 "(c) 1999-2006 Vadim Zeitlin",
                 wxString("About ") + GetSampleTitle(),
                 wxOK | wxICON_INFORMATION, this);
}

// ----------------------------------------------------------------------------
// FontWindow
// ----------------------------------------------------------------------------

FontWindow::FontWindow(wxWindow *parent)
          : wxWindow(parent, wxID_ANY),
            m_panel(new FontPanel(this)),
            m_canvas(new FontCanvas(this))
{
    wxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_panel, wxSizerFlags().Expand().Border());
    sizer->Add(new wxStaticLine(this), wxSizerFlags().Expand());
    sizer->Add(m_canvas, wxSizerFlags(1).Expand());
    SetSizer(sizer);
}

void FontWindow::UpdateFont(const wxFont& font, const wxColour& colour)
{
    m_panel->ShowFont(font);

    m_canvas->SetTextFont(font);
    if ( colour.IsOk() )
        m_canvas->SetColour(colour);
    m_canvas->Refresh();
}

// ----------------------------------------------------------------------------
// FontPanel
// ----------------------------------------------------------------------------

FontPanel::FontPanel(wxWindow* parent)
         : wxPanel(parent)
{
    m_useFamily =
    m_useNumericWeight = false;

    m_textFaceName = new wxTextCtrl(this, wxID_ANY, wxString(),
                                    wxDefaultPosition, wxDefaultSize,
                                    wxTE_PROCESS_ENTER);
    m_textFaceName->Bind(wxEVT_TEXT, &FontPanel::OnFacename, this);

    // Must be in sync with the Family enum.
    const wxString familiesNames[] =
    {
        "Default",
        "Decorative",
        "Roman",
        "Script",
        "Swiss",
        "Modern",
        "Teletype",
    };
    m_choiceFamily = new wxChoice(this, wxID_ANY,
                                  wxDefaultPosition, wxDefaultSize,
                                  WXSIZEOF(familiesNames), familiesNames);
    m_choiceFamily->Bind(wxEVT_CHOICE, &FontPanel::OnFamily, this);

    m_spinPointSize = new wxSpinCtrlDouble(this, wxID_ANY, wxString(),
                                           wxDefaultPosition, wxDefaultSize,
                                           wxSP_ARROW_KEYS,
                                           1.0, 100.0, 10.0, 0.1);

    m_spinPointSize->SetInitialSize
        (
         m_spinPointSize->GetSizeFromTextSize(GetTextExtent("999.9").x)
        );

    // Must be in sync with the Style enum.
    const wxString stylesNames[] =
    {
        "Normal",
        "Italic",
        "Slant",
    };

    m_choiceStyle = new wxChoice(this, wxID_ANY,
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(stylesNames), stylesNames);

    // Must be in sync with the Weight enum.
    const wxString weightsNames[] =
    {
        "Thin",
        "Extra light",
        "Light",
        "Normal",
        "Medium",
        "Semi-bold",
        "Bold",
        "Extra bold",
        "Heavy",
        "Extra heavy",
    };

    m_choiceWeight = new wxChoice(this, wxID_ANY,
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(weightsNames), weightsNames);
    m_choiceWeight->Bind(wxEVT_CHOICE, &FontPanel::OnWeightChoice, this);

    m_spinWeight = new wxSpinCtrl(this, wxID_ANY, wxString(),
                                  wxDefaultPosition, wxDefaultSize,
                                  wxSP_ARROW_KEYS,
                                  1, wxFONTWEIGHT_MAX);
    m_spinWeight->SetInitialSize
        (
         m_spinWeight->GetSizeFromTextSize(GetTextExtent("9999").x)
        );
    m_spinWeight->Bind(wxEVT_SPINCTRL, &FontPanel::OnWeightSpin, this);

    m_checkUnderlined = new wxCheckBox(this, wxID_ANY, wxString());
    m_checkStrikethrough = new wxCheckBox(this, wxID_ANY, wxString());
    m_checkFixedWidth = new wxCheckBox(this, wxID_ANY, wxString());
    m_checkFixedWidth->Disable(); // Can't be changed by the user.

    m_labelInfo = new wxStaticText(this, wxID_ANY, "\n\n\n");


    const int border = wxSizerFlags::GetDefaultBorder();

    // Columns are: label, control, gap, label, control, label, control (there
    // is no second gap column because we don't want any gap in the weight row).
    wxFlexGridSizer* const sizer = new wxFlexGridSizer(7, wxSize(border, border));

    const wxSizerFlags flagsLabel = wxSizerFlags().CentreVertical();
    const wxSizerFlags flagsValue = wxSizerFlags().Expand().CentreVertical();

    sizer->Add(new wxStaticText(this, wxID_ANY, "Face &name:"), flagsLabel);
    sizer->Add(m_textFaceName, flagsValue);

    sizer->AddSpacer(2*border);

    sizer->Add(new wxStaticText(this, wxID_ANY, "&Family:"), flagsLabel);
    sizer->Add(m_choiceFamily, flagsValue);

    sizer->Add(new wxStaticText(this, wxID_ANY, "&Point size:"),
               wxSizerFlags().DoubleBorder(wxLEFT).CentreVertical());
    sizer->Add(m_spinPointSize, flagsValue);


    sizer->Add(new wxStaticText(this, wxID_ANY, "&Style:"), flagsLabel);
    sizer->Add(m_choiceStyle, flagsValue);

    sizer->AddSpacer(0);

    sizer->Add(new wxStaticText(this, wxID_ANY, "&Weight:"), flagsLabel);
    sizer->Add(m_choiceWeight, flagsValue);

    sizer->Add(new wxStaticText(this, wxID_ANY, "or &raw value:"), flagsLabel);
    sizer->Add(m_spinWeight, flagsValue);


    sizer->Add(new wxStaticText(this, wxID_ANY, "&Underlined:"), flagsLabel);
    sizer->Add(m_checkUnderlined, flagsValue);

    sizer->AddSpacer(0);

    sizer->Add(new wxStaticText(this, wxID_ANY, "&Strike through:"), flagsLabel);
    sizer->Add(m_checkStrikethrough, flagsValue);

    sizer->Add(new wxStaticText(this, wxID_ANY, "Fixed width:"), flagsLabel);
    sizer->Add(m_checkFixedWidth, flagsValue);

    wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(sizer, wxSizerFlags().Expand().Border(wxBOTTOM));
    sizerTop->Add(new wxButton(this, wxID_APPLY, "&Apply changes"),
                  wxSizerFlags().Border(wxBOTTOM).Centre());
    sizerTop->Add(m_labelInfo, wxSizerFlags().Expand().Border(wxTOP));
    SetSizer(sizerTop);

    ShowFont(*wxNORMAL_FONT);
}

void FontPanel::DoUpdate()
{
    m_textFaceName->ChangeValue(m_font.GetFaceName());

    Family family = Family_Default;
    switch ( m_font.GetFamily() )
    {
        case wxFONTFAMILY_DECORATIVE: family = Family_Decorative; break;
        case wxFONTFAMILY_ROMAN:      family = Family_Roman;      break;
        case wxFONTFAMILY_SCRIPT:     family = Family_Script;     break;
        case wxFONTFAMILY_SWISS:      family = Family_Swiss;      break;
        case wxFONTFAMILY_MODERN:     family = Family_Modern;     break;
        case wxFONTFAMILY_TELETYPE:   family = Family_Teletype;   break;

        case wxFONTFAMILY_DEFAULT:
        case wxFONTFAMILY_UNKNOWN:
            // Leave family as Family_Default, what else can we do.
            break;
    }
    m_choiceFamily->SetSelection(family);

    m_spinPointSize->SetValue(m_font.GetFractionalPointSize());

    Style style = Style_Normal;
    switch ( m_font.GetStyle() )
    {
        case wxFONTSTYLE_ITALIC: style = Style_Italic; break;
        case wxFONTSTYLE_SLANT:  style = Style_Slant;  break;

        case wxFONTSTYLE_NORMAL:
        case wxFONTSTYLE_MAX:
            break;
    }
    m_choiceStyle->SetSelection(style);

    Weight weight = Weight_Normal;
    switch ( m_font.GetWeight() )
    {
        case wxFONTWEIGHT_THIN:         weight = Weight_Thin;       break;
        case wxFONTWEIGHT_EXTRALIGHT:   weight = Weight_Extralight; break;
        case wxFONTWEIGHT_LIGHT:        weight = Weight_Light;      break;
        case wxFONTWEIGHT_MEDIUM:       weight = Weight_Medium;     break;
        case wxFONTWEIGHT_SEMIBOLD:     weight = Weight_Semibold;   break;
        case wxFONTWEIGHT_BOLD:         weight = Weight_Bold;       break;
        case wxFONTWEIGHT_EXTRABOLD:    weight = Weight_Extrabold;  break;
        case wxFONTWEIGHT_HEAVY:        weight = Weight_Heavy;      break;
        case wxFONTWEIGHT_EXTRAHEAVY:   weight = Weight_Extraheavy; break;

        case wxFONTWEIGHT_NORMAL:
        case wxFONTWEIGHT_INVALID:
            break;
    }
    m_choiceWeight->SetSelection(weight);
    m_spinWeight->SetValue(m_font.GetNumericWeight());

    m_checkUnderlined->SetValue(m_font.GetUnderlined());
    m_checkStrikethrough->SetValue(m_font.GetStrikethrough());
    m_checkFixedWidth->SetValue(m_font.IsFixedWidth());

    const wxSize pixelSize = m_font.GetPixelSize();
    wxClientDC dc(this);
    dc.SetFont(m_font);

    m_labelInfo->SetLabelText
        (
            wxString::Format
            (
                "Font info string: %s\n"
                "Size in pixels: %d*%d, "
                "average char size: %d*%d",
                m_font.GetNativeFontInfoDesc(),
                pixelSize.x, pixelSize.y,
                dc.GetCharWidth(), dc.GetCharHeight()
            )
        );
}

wxFontInfo FontPanel::GetFontInfo() const
{
    wxFontInfo info(m_spinPointSize->GetValue());

    if ( m_useFamily )
    {
        const wxFontFamily families[] =
        {
            wxFONTFAMILY_DEFAULT,
            wxFONTFAMILY_DECORATIVE,
            wxFONTFAMILY_ROMAN,
            wxFONTFAMILY_SCRIPT,
            wxFONTFAMILY_SWISS,
            wxFONTFAMILY_MODERN,
            wxFONTFAMILY_TELETYPE,
        };
        info.Family(families[m_choiceFamily->GetSelection()]);
    }
    else
    {
        info.FaceName(m_textFaceName->GetValue());
    }

    switch ( m_choiceStyle->GetSelection() )
    {
        case Style_Normal:
            break;

        case Style_Italic:
            info.Italic();
            break;

        case Style_Slant:
            info.Slant();
            break;
    }

    info.Weight(m_useNumericWeight ? m_spinWeight->GetValue()
                                   : (m_choiceWeight->GetSelection() + 1)*100);

    if ( m_checkUnderlined->GetValue() )
        info.Underlined();
    if ( m_checkStrikethrough->GetValue() )
        info.Strikethrough();

    return info;
}

// ----------------------------------------------------------------------------
// FontCanvas
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(FontCanvas, wxWindow)
    EVT_PAINT(FontCanvas::OnPaint)
wxEND_EVENT_TABLE()

FontCanvas::FontCanvas( wxWindow *parent )
          : wxWindow( parent, wxID_ANY ),
            m_colour(*wxRED), m_font(*wxNORMAL_FONT)
{
}

void FontCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc(this);
    PrepareDC(dc);

    // set background
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    dc.SetFont(m_font);

    // the current text origin
    wxCoord x = 5,
            y = 5;

    // prepare to draw the font
    dc.SetTextForeground(m_colour);

    // the size of one cell (Normally biggest char + small margin)
    wxCoord maxCharWidth, maxCharHeight;
    dc.GetTextExtent("W", &maxCharWidth, &maxCharHeight);
    int w = maxCharWidth + 5,
        h = maxCharHeight + 4;


    // print all font symbols from 32 to 256 in 7 rows of 32 chars each
    for ( int i = 0; i < 7; i++ )
    {
        for ( int j = 0; j < 32; j++ )
        {
            wxChar c = (wxChar)(32 * (i + 1) + j);

            wxCoord charWidth, charHeight;
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
    dc.SetPen(*wxBLUE_PEN);
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
