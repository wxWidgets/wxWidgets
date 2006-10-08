/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.h
// Purpose:     Common dialogs demo
// Author:      Julian Smart
// Modified by: ABX (2004) - adjustementd for conditional building
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*
This sample shows how to use the common dialogs available from wxWidgets.
It also shows that generic implementations of common dialogs can be exchanged
with native dialogs and can coexist in one application. The need for generic
dialogs addition is recognized thanks to setup of below USE_*** setting. Their
combinations reflects conditions of makefiles and project files to avoid unresolved
references during linking. For now some generic dialogs are added in static builds
of MSW, MAC and OS2
*/

#ifndef __DIALOGSH__
#define __DIALOGSH__

#ifdef __WXUNIVERSAL__
    #define USE_WXUNIVERSAL 1
#else
    #define USE_WXUNIVERSAL 0
#endif

#ifdef WXUSINGDLL
    #define USE_DLL 1
#else
    #define USE_DLL 0
#endif

#if defined(__WXWINCE__)
    #define USE_WXWINCE 1
#else
    #define USE_WXWINCE 0
#endif

#if defined(__WXMSW__) && !USE_WXWINCE
    #define USE_WXMSW 1
#else
    #define USE_WXMSW 0
#endif

#ifdef __WXMAC__
    #define USE_WXMAC 1
#else
    #define USE_WXMAC 0
#endif

#if defined(__WXMAC_OSX__) && ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_2) && USE_NATIVE_FONT_DIALOG_FOR_MACOSX
    #define USE_WXMACFONTDLG 1
#else
    #define USE_WXMACFONTDLG 0
#endif

#ifdef __WXGTK__
    #define USE_WXGTK 1
#else
    #define USE_WXGTK 0
#endif

#ifdef __WXPM__
    #define USE_WXPM 1
#else
    #define USE_WXPM 0
#endif

#define USE_GENERIC_DIALOGS (!USE_WXUNIVERSAL && !USE_DLL)

#define USE_COLOURDLG_GENERIC \
    ((USE_WXMSW || USE_WXMAC) && USE_GENERIC_DIALOGS && wxUSE_COLOURDLG)
#define USE_DIRDLG_GENERIC \
    ((USE_WXMSW || USE_WXMAC) && USE_GENERIC_DIALOGS && wxUSE_DIRDLG)
#define USE_FILEDLG_GENERIC \
    ((((USE_WXMSW || USE_WXMAC || USE_WXPM) && USE_GENERIC_DIALOGS) || USE_WXWINCE) && wxUSE_FILEDLG)
#define USE_FONTDLG_GENERIC \
    ((USE_WXMSW || USE_WXMACFONTDLG || USE_WXPM) && USE_GENERIC_DIALOGS && wxUSE_FONTDLG)

// Turn USE_MODAL_PRESENTATION to 0 if there is any reason for not presenting difference
// between modal and modeless dialogs (ie. not implemented it in your port yet)
#if defined(__SMARTPHONE__) || !wxUSE_BOOKCTRL
    #define USE_MODAL_PRESENTATION 0
#else
    #define USE_MODAL_PRESENTATION 1
#endif


// Turn USE_SETTINGS_DIALOG to 0 if supported
#if wxUSE_BOOKCTRL
    #define USE_SETTINGS_DIALOG 1
#else
    #define USE_SETTINGS_DIALOG 0
#endif


// Define a new application type
class MyApp: public wxApp
{
public:
    bool OnInit();

    wxFont       m_canvasFont;
    wxColour     m_canvasTextColour;
};

#if USE_MODAL_PRESENTATION

// A custom modeless dialog
class MyModelessDialog : public wxDialog
{
public:
    MyModelessDialog(wxWindow *parent);

    void OnButton(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

// A custom modal dialog
class MyModalDialog : public wxDialog
{
public:
    MyModalDialog(wxWindow *parent);

    void OnButton(wxCommandEvent& event);

private:
    wxButton *m_btnModal,
             *m_btnModeless,
             *m_btnDelete;

    DECLARE_EVENT_TABLE()
};

#endif // USE_MODAL_PRESENTATION

#if USE_SETTINGS_DIALOG
// Property sheet dialog
class SettingsDialog: public wxPropertySheetDialog
{
DECLARE_CLASS(SettingsDialog)
public:
    SettingsDialog(wxWindow* parent, int dialogType);
    ~SettingsDialog();

    wxPanel* CreateGeneralSettingsPage(wxWindow* parent);
    wxPanel* CreateAestheticSettingsPage(wxWindow* parent);

protected:

    enum {
        ID_SHOW_TOOLTIPS = 100,
        ID_AUTO_SAVE,
        ID_AUTO_SAVE_MINS,
        ID_LOAD_LAST_PROJECT,

        ID_APPLY_SETTINGS_TO,
        ID_BACKGROUND_STYLE,
        ID_FONT_SIZE
    };

    wxImageList*    m_imageList;

DECLARE_EVENT_TABLE()
};

#endif // USE_SETTINGS_DIALOG

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyFrame(wxWindow *parent, const wxString& title);

    void MessageBox(wxCommandEvent& event);

#if wxUSE_COLOURDLG
    void ChooseColour(wxCommandEvent& event);
#endif // wxUSE_COLOURDLG

#if wxUSE_FONTDLG
    void ChooseFont(wxCommandEvent& event);
#endif // wxUSE_FONTDLG

#if wxUSE_LOG_DIALOG
    void LogDialog(wxCommandEvent& event);
#endif // wxUSE_LOG_DIALOG

#if wxUSE_CHOICEDLG
    void SingleChoice(wxCommandEvent& event);
    void MultiChoice(wxCommandEvent& event);
#endif // wxUSE_CHOICEDLG

#if wxUSE_TEXTDLG
    void TextEntry(wxCommandEvent& event);
    void PasswordEntry(wxCommandEvent& event);
#endif // wxUSE_TEXTDLG

#if wxUSE_NUMBERDLG
    void NumericEntry(wxCommandEvent& event);
#endif // wxUSE_NUMBERDLG

#if wxUSE_FILEDLG
    void FileOpen(wxCommandEvent& event);
    void FileOpen2(wxCommandEvent& event);
    void FilesOpen(wxCommandEvent& event);
    void FileSave(wxCommandEvent& event);
#endif // wxUSE_FILEDLG

#if USE_FILEDLG_GENERIC
    void FileOpenGeneric(wxCommandEvent& event);
    void FilesOpenGeneric(wxCommandEvent& event);
    void FileSaveGeneric(wxCommandEvent& event);
#endif // USE_FILEDLG_GENERIC

#if wxUSE_DIRDLG
    void DirChoose(wxCommandEvent& event);
    void DirChooseNew(wxCommandEvent& event);
#endif // wxUSE_DIRDLG

#if USE_DIRDLG_GENERIC
    void GenericDirChoose(wxCommandEvent& event);
#endif // USE_DIRDLG_GENERIC

#if wxUSE_STARTUP_TIPS
    void ShowTip(wxCommandEvent& event);
#endif // wxUSE_STARTUP_TIPS

#if USE_MODAL_PRESENTATION
    void ModalDlg(wxCommandEvent& event);
    void ModelessDlg(wxCommandEvent& event);
    void DlgCenteredScreen(wxCommandEvent& event);
    void DlgCenteredParent(wxCommandEvent& event);
#endif // USE_MODAL_PRESENTATION

#if wxUSE_PROGRESSDLG
    void ShowProgress(wxCommandEvent& event);
#endif // wxUSE_PROGRESSDLG

#if wxUSE_ABOUTDLG
    void ShowSimpleAboutDialog(wxCommandEvent& event);
    void ShowFancyAboutDialog(wxCommandEvent& event);
#endif // wxUSE_ABOUTDLG

#if wxUSE_BUSYINFO
    void ShowBusyInfo(wxCommandEvent& event);
#endif // wxUSE_BUSYINFO

#if wxUSE_FINDREPLDLG
    void ShowFindDialog(wxCommandEvent& event);
    void ShowReplaceDialog(wxCommandEvent& event);
    void OnFindDialog(wxFindDialogEvent& event);
#endif // wxUSE_FINDREPLDLG

#if USE_COLOURDLG_GENERIC
    void ChooseColourGeneric(wxCommandEvent& event);
#endif // USE_COLOURDLG_GENERIC

#if USE_FONTDLG_GENERIC
    void ChooseFontGeneric(wxCommandEvent& event);
#endif // USE_FONTDLG_GENERIC

    void OnPropertySheet(wxCommandEvent& event);
    void OnRequestUserAttention(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

private:
#if wxUSE_DIRDLG
    void DoDirChoose(int style);
#endif // wxUSE_DIRDLG

#if USE_MODAL_PRESENTATION
    MyModelessDialog *m_dialog;
#endif // USE_MODAL_PRESENTATION

#if wxUSE_FINDREPLDLG
    wxFindReplaceData m_findData;

    wxFindReplaceDialog *m_dlgFind,
                        *m_dlgReplace;
#endif // wxUSE_FINDREPLDLG

    wxColourData m_clrData;

    DECLARE_EVENT_TABLE()
};

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxWindow *parent) :
       wxScrolledWindow(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxNO_FULL_REPAINT_ON_RESIZE) { }

    void OnPaint(wxPaintEvent& event);

    DECLARE_EVENT_TABLE()
};


// Menu IDs
enum
{
    DIALOGS_CHOOSE_COLOUR = wxID_HIGHEST,
    DIALOGS_CHOOSE_COLOUR_GENERIC,
    DIALOGS_CHOOSE_FONT,
    DIALOGS_CHOOSE_FONT_GENERIC,
    DIALOGS_MESSAGE_BOX,
    DIALOGS_SINGLE_CHOICE,
    DIALOGS_MULTI_CHOICE,
    DIALOGS_TEXT_ENTRY,
    DIALOGS_PASSWORD_ENTRY,
    DIALOGS_FILE_OPEN,
    DIALOGS_FILE_OPEN2,
    DIALOGS_FILES_OPEN,
    DIALOGS_FILE_SAVE,
    DIALOGS_FILE_OPEN_GENERIC,
    DIALOGS_FILES_OPEN_GENERIC,
    DIALOGS_FILE_SAVE_GENERIC,
    DIALOGS_DIR_CHOOSE,
    DIALOGS_DIRNEW_CHOOSE,
    DIALOGS_GENERIC_DIR_CHOOSE,
    DIALOGS_TIP,
    DIALOGS_NUM_ENTRY,
    DIALOGS_LOG_DIALOG,
    DIALOGS_MODAL,
    DIALOGS_MODELESS,
    DIALOGS_CENTRE_SCREEN,
    DIALOGS_CENTRE_PARENT,
    DIALOGS_MODELESS_BTN,
    DIALOGS_PROGRESS,
    DIALOGS_ABOUTDLG_SIMPLE,
    DIALOGS_ABOUTDLG_FANCY,
    DIALOGS_BUSYINFO,
    DIALOGS_FIND,
    DIALOGS_REPLACE,
    DIALOGS_REQUEST,
    DIALOGS_PROPERTY_SHEET,
    DIALOGS_PROPERTY_SHEET_TOOLBOOK,
    DIALOGS_PROPERTY_SHEET_BUTTONTOOLBOOK
};

#endif

