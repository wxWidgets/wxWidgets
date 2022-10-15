/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.h
// Purpose:     Common dialogs demo
// Author:      Julian Smart, Vadim Zeitlin, ABX
// Created:     04/01/98
// Copyright:   (c) Julian Smart
//              (c) 2004 ABX
//              (c) Vadim Zeitlin
// Licence:     wxWindows licence
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

#if defined(__WXMSW__)
    #define USE_WXMSW 1
#else
    #define USE_WXMSW 0
#endif

#ifdef __WXMAC__
    #define USE_WXMAC 1
#else
    #define USE_WXMAC 0
#endif

#if USE_NATIVE_FONT_DIALOG_FOR_MACOSX
    #define USE_WXMACFONTDLG 1
#else
    #define USE_WXMACFONTDLG 0
#endif

#ifdef __WXGTK__
    #define USE_WXGTK 1
#else
    #define USE_WXGTK 0
#endif

#define USE_GENERIC_DIALOGS (!USE_WXUNIVERSAL && !USE_DLL)

#define USE_COLOURDLG_GENERIC \
    ((USE_WXMSW || USE_WXMAC) && USE_GENERIC_DIALOGS && wxUSE_COLOURDLG)
#define USE_DIRDLG_GENERIC \
    ((USE_WXMSW || USE_WXMAC) && USE_GENERIC_DIALOGS && wxUSE_DIRDLG)
#define USE_FILEDLG_GENERIC \
    ((USE_WXMSW || USE_WXMAC) && USE_GENERIC_DIALOGS  && wxUSE_FILEDLG)
#define USE_FONTDLG_GENERIC \
    ((USE_WXMSW || USE_WXMACFONTDLG) && USE_GENERIC_DIALOGS && wxUSE_FONTDLG)

// Turn USE_MODAL_PRESENTATION to 0 if there is any reason for not presenting difference
// between modal and modeless dialogs (ie. not implemented it in your port yet)
#if !wxUSE_BOOKCTRL
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

#if wxUSE_LOG

// Custom application traits class which we use to override the default log
// target creation
class MyAppTraits : public wxGUIAppTraits
{
public:
    virtual wxLog *CreateLogTarget() override;
};

#endif // wxUSE_LOG

// Define a new application type
class MyApp: public wxApp
{
public:
    MyApp() { m_startupProgressStyle = -1; }

    virtual bool OnInit() override;

#if wxUSE_CMDLINE_PARSER
    virtual void OnInitCmdLine(wxCmdLineParser& parser) override;
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) override;
#endif // wxUSE_CMDLINE_PARSER

protected:
#if wxUSE_LOG
    virtual wxAppTraits *CreateTraits() override { return new MyAppTraits; }
#endif // wxUSE_LOG

private:
    // Flag set to a valid value if command line option "progress" is used,
    // this allows testing of wxProgressDialog before the main event loop is
    // started. If this option is not specified it is set to -1 by default
    // meaning that progress dialog shouldn't be shown at all.
    long m_startupProgressStyle;
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
    wxDECLARE_EVENT_TABLE();
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

    wxDECLARE_EVENT_TABLE();
};

#endif // USE_MODAL_PRESENTATION

// A class demonstrating CreateStdDialogButtonSizer()
class StdButtonSizerDialog : public wxDialog
{
public:
    StdButtonSizerDialog(wxWindow *parent);

    void OnEvent(wxCommandEvent& event);

private:
    void EnableDisableControls();

    wxCheckBox *m_chkboxAffirmativeButton;
    wxRadioButton *m_radiobtnOk,
                  *m_radiobtnYes;

    wxCheckBox *m_chkboxDismissButton;
    wxRadioButton *m_radiobtnClose,
                  *m_radiobtnCancel;

    wxCheckBox *m_chkboxApply,
               *m_chkboxNo,
               *m_chkboxHelp,
               *m_chkboxNoDefault;

    wxSizer *m_buttonsSizer;

    wxDECLARE_EVENT_TABLE();
};

// Test harness for wxMessageDialog.
class TestMessageBoxDialog : public wxDialog
{
public:
    TestMessageBoxDialog(wxWindow *parent);

    bool Create();

protected:
    wxString GetBoxTitle() { return m_textTitle->GetValue(); }
    wxString GetMessage() { return m_textMsg->GetValue(); }
    long GetStyle();

    void PrepareMessageDialog(wxMessageDialogBase &dlg);

    virtual void AddAdditionalTextOptions(wxSizer *WXUNUSED(sizer)) { }
    virtual void AddAdditionalFlags(wxSizer *WXUNUSED(sizer)) { }

    void ShowResult(int res);

    void OnApply(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnUpdateLabelUI(wxUpdateUIEvent& event);
    void OnUpdateNoDefaultUI(wxUpdateUIEvent& event);

private:
    enum
    {
        Btn_Yes,
        Btn_No,
        Btn_Ok,
        Btn_Cancel,
        Btn_Help,
        Btn_Max
    };

    struct BtnInfo
    {
        int flag;
        const char *name;
    };

    static const BtnInfo ms_btnInfo[Btn_Max];

    enum
    {
        MsgDlgIcon_No,
        MsgDlgIcon_None,
        MsgDlgIcon_Info,
        MsgDlgIcon_Question,
        MsgDlgIcon_Warning,
        MsgDlgIcon_Error,
        MsgDlgIcon_AuthNeeded,
        MsgDlgIcon_Max
    };

    wxTextCtrl *m_textTitle,
               *m_textMsg,
               *m_textExtMsg;

    wxCheckBox *m_buttons[Btn_Max];
    wxTextCtrl *m_labels[Btn_Max];

    wxRadioBox *m_icons;

    wxCheckBox *m_chkNoDefault,
               *m_chkCentre;

    wxStaticText *m_labelResult;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(TestMessageBoxDialog);
};

#if wxUSE_RICHMSGDLG
class TestRichMessageDialog : public TestMessageBoxDialog
{
public:
    TestRichMessageDialog(wxWindow *parent);

protected:
    // overrides method in base class
    virtual void AddAdditionalTextOptions(wxSizer *sizer) override;
    virtual void AddAdditionalFlags(wxSizer *sizer) override;

    void OnApply(wxCommandEvent& event);

private:
    wxTextCtrl *m_textCheckBox;
    wxCheckBox *m_initialValueCheckBox;
    wxTextCtrl *m_textDetailed;
    wxTextCtrl *m_textFooter;
    wxChoice *m_iconsFooter;

    wxDECLARE_EVENT_TABLE();
};
#endif // wxUSE_RICHMSGDLG

class TestDefaultActionDialog: public wxDialog
{
public:
    TestDefaultActionDialog( wxWindow *parent );

    void OnListBoxDClick(wxCommandEvent& event);
    void OnDisableOK(wxCommandEvent& event);
    void OnDisableCancel(wxCommandEvent& event);
    void OnCatchListBoxDClick(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);

private:
    bool   m_catchListBoxDClick;

private:
    wxDECLARE_EVENT_TABLE();
};


#if USE_SETTINGS_DIALOG

// Struct containing properties edited by SettingsDialog.
struct SettingsData
{
    SettingsData() :
        m_loadLastOnStartup(false),
        m_autoSaveInterval(1),
        m_showToolTips(false),
        m_applyTo(0),
        m_bgStyle(0),
        m_titleFontSize(10)
    {
    }

    bool m_loadLastOnStartup;
    int m_autoSaveInterval;
    bool m_showToolTips;
    int m_applyTo;
    int m_bgStyle;
    int m_titleFontSize;
};

// Property sheet dialog
class SettingsDialog: public wxPropertySheetDialog
{
    wxDECLARE_CLASS(SettingsDialog);
public:
    SettingsDialog(wxWindow* parent, SettingsData& settingsData, int dialogType);
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

    SettingsData& m_settingsData;

    wxDECLARE_EVENT_TABLE();
};

#endif // USE_SETTINGS_DIALOG

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title);
    virtual ~MyFrame();

#if wxUSE_MSGDLG
    void MessageBox(wxCommandEvent& event);
    void MessageBoxDialog(wxCommandEvent& event);
    void MessageBoxDialogWindowModal(wxCommandEvent& event);
    void MessageBoxDialogWindowModalClosed(wxWindowModalDialogEvent& event);
    void MessageBoxInfo(wxCommandEvent& event);
    void MessageBoxWindowModal(wxCommandEvent& event);
    void MessageBoxWindowModalClosed(wxWindowModalDialogEvent& event);
#endif // wxUSE_MSGDLG
#if wxUSE_RICHMSGDLG
    void RichMessageDialog(wxCommandEvent& event);
#endif // wxUSE_RICHMSGDLG

#if wxUSE_COLOURDLG
    void ChooseColour(wxCommandEvent& event);
    void GetColour(wxCommandEvent& event);
#endif // wxUSE_COLOURDLG

#if wxUSE_FONTDLG
    void ChooseFont(wxCommandEvent& event);
#endif // wxUSE_FONTDLG

#if wxUSE_LOG_DIALOG
    void LogDialog(wxCommandEvent& event);
#endif // wxUSE_LOG_DIALOG

#if wxUSE_INFOBAR
    void InfoBarSimple(wxCommandEvent& event);
    void InfoBarSimpleWrapped(wxCommandEvent &event);
    void InfoBarAdvanced(wxCommandEvent& event);
#endif // wxUSE_INFOBAR

#if wxUSE_CHOICEDLG
    void SingleChoice(wxCommandEvent& event);
    void MultiChoice(wxCommandEvent& event);
#endif // wxUSE_CHOICEDLG

#if wxUSE_REARRANGECTRL
    void Rearrange(wxCommandEvent& event);
#endif // wxUSE_REARRANGECTRL

#if wxUSE_ADDREMOVECTRL
    void AddRemove(wxCommandEvent& event);
#endif // wxUSE_ADDREMOVECTRL

#if wxUSE_TEXTDLG
    void LineEntry(wxCommandEvent& event);
    void TextEntry(wxCommandEvent& event);
    void PasswordEntry(wxCommandEvent& event);
#endif // wxUSE_TEXTDLG

#ifdef wxUSE_CREDENTIALDLG
    void CredentialEntry(wxCommandEvent& event);
#endif // wxUSE_CREDENTIALDLG

#if wxUSE_NUMBERDLG
    void NumericEntry(wxCommandEvent& event);
#endif // wxUSE_NUMBERDLG

#if wxUSE_FILEDLG
    void FileOpen(wxCommandEvent& event);
    void FileOpen2(wxCommandEvent& event);
    void FilesOpen(wxCommandEvent& event);
    void FilesOpenWindowModal(wxCommandEvent& event);
    void FilesOpenWindowModalClosed(wxWindowModalDialogEvent& event);
    void FileSave(wxCommandEvent& event);
    void FileSaveWindowModal(wxCommandEvent& event);
    void FileSaveWindowModalClosed(wxWindowModalDialogEvent& event);
    void MacToggleAlwaysShowTypes(wxCommandEvent& event);
#endif // wxUSE_FILEDLG

#if USE_FILEDLG_GENERIC
    void FileOpenGeneric(wxCommandEvent& event);
    void FilesOpenGeneric(wxCommandEvent& event);
    void FileSaveGeneric(wxCommandEvent& event);
#endif // USE_FILEDLG_GENERIC

#if wxUSE_DIRDLG
    void DirChoose(wxCommandEvent& event);
    void DirChooseWindowModal(wxCommandEvent& event);
    void DirChooseWindowModalClosed(wxWindowModalDialogEvent& event);
    void DirChooseNew(wxCommandEvent& event);
    void DirChooseMultiple(wxCommandEvent& event);
#endif // wxUSE_DIRDLG

#if USE_DIRDLG_GENERIC
    void GenericDirChoose(wxCommandEvent& event);
#endif // USE_DIRDLG_GENERIC

#if wxUSE_STARTUP_TIPS
    void ShowTip(wxCommandEvent& event);
#endif // wxUSE_STARTUP_TIPS

#if USE_MODAL_PRESENTATION
    void ModalDlg(wxCommandEvent& event);
#endif // USE_MODAL_PRESENTATION
    void ModelessDlg(wxCommandEvent& event);
    void DlgCenteredScreen(wxCommandEvent& event);
    void DlgCenteredParent(wxCommandEvent& event);
    void MiniFrame(wxCommandEvent& event);
    void DlgOnTop(wxCommandEvent& event);

#if wxUSE_PROGRESSDLG
    void ShowProgress(wxCommandEvent& event);
#ifdef wxHAS_NATIVE_PROGRESSDIALOG
    void ShowProgressGeneric(wxCommandEvent& event);
#endif // wxHAS_NATIVE_PROGRESSDIALOG
    void DoShowProgress(wxGenericProgressDialog& dialog);
#endif // wxUSE_PROGRESSDLG
    void ShowAppProgress(wxCommandEvent& event);

#if wxUSE_ABOUTDLG
    void ShowSimpleAboutDialog(wxCommandEvent& event);
    void ShowFancyAboutDialog(wxCommandEvent& event);
    void ShowFullAboutDialog(wxCommandEvent& event);
    void ShowCustomAboutDialog(wxCommandEvent& event);
#endif // wxUSE_ABOUTDLG

#if wxUSE_BUSYINFO
    void ShowBusyInfo(wxCommandEvent& event);
    void ShowRichBusyInfo(wxCommandEvent& event);
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
#if wxUSE_NOTIFICATION_MESSAGE
    void OnNotifMsg(wxCommandEvent& event);
#endif // wxUSE_NOTIFICATION_MESSAGE

#if wxUSE_RICHTOOLTIP
    void OnRichTipDialog(wxCommandEvent& event);
#endif // wxUSE_RICHTOOLTIP

    void OnStandardButtonsSizerDialog(wxCommandEvent& event);

    void OnTestDefaultActionDialog(wxCommandEvent& event);
    void OnModalHook(wxCommandEvent& event);
    void OnSimulatedUnsaved(wxCommandEvent& event);

    void OnExit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

private:
#if wxUSE_COLOURDLG
    void OnColourChanged(wxColourDialogEvent& event);
    void DoApplyColour(const wxColour& colour);
#endif // wxUSE_COLOURDLG

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

    // just a window which we use to show the effect of font/colours selection
    wxWindow *m_canvas;

#if wxUSE_INFOBAR
    void OnInfoBarRedo(wxCommandEvent& event);

    wxInfoBar *m_infoBarSimple,
              *m_infoBarAdvanced;
#endif // wxUSE_INFOBAR

#if USE_SETTINGS_DIALOG
    SettingsData m_settingsData;
#endif // USE_SETTINGS_DIALOG

#if wxUSE_TIPWINDOW
    void OnShowTip(wxCommandEvent& event);
    void OnUpdateShowTipUI(wxUpdateUIEvent& event);

    wxTipWindow *m_tipWindow;
#endif // wxUSE_TIPWINDOW

    bool m_confirmExit;
    wxDECLARE_EVENT_TABLE();
};

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxWindow *parent) : wxScrolledWindow(parent, wxID_ANY)
    {
        SetForegroundColour(*wxBLACK);
        SetBackgroundColour(*wxWHITE);
        SetFont(*wxNORMAL_FONT);
    }

private:
    void OnPaint(wxPaintEvent& event);

    wxDECLARE_EVENT_TABLE();
};


// Menu IDs
enum
{
    DIALOGS_CHOOSE_COLOUR = wxID_HIGHEST,
    DIALOGS_CHOOSE_COLOUR_ALPHA,
    DIALOGS_GET_COLOUR,
    DIALOGS_CHOOSE_COLOUR_GENERIC,
    DIALOGS_CHOOSE_COLOUR_GENERIC_ALPHA,
    DIALOGS_CHOOSE_FONT,
    DIALOGS_CHOOSE_FONT_GENERIC,
    DIALOGS_MESSAGE_BOX,
    DIALOGS_MESSAGE_BOX_WINDOW_MODAL,
    DIALOGS_MESSAGE_DIALOG,
    DIALOGS_MESSAGE_DIALOG_WINDOW_MODAL,
    DIALOGS_MESSAGE_BOX_WXINFO,
    DIALOGS_RICH_MESSAGE_DIALOG,
    DIALOGS_SINGLE_CHOICE,
    DIALOGS_MULTI_CHOICE,
    DIALOGS_REARRANGE,
    DIALOGS_ADDREMOVE,
    DIALOGS_LINE_ENTRY,
    DIALOGS_TEXT_ENTRY,
    DIALOGS_PASSWORD_ENTRY,
    DIALOGS_CREDENTIAL_ENTRY,
    DIALOGS_FILE_OPEN,
    DIALOGS_FILE_OPEN2,
    DIALOGS_FILES_OPEN,
    DIALOGS_FILES_OPEN_WINDOW_MODAL,
    DIALOGS_FILE_SAVE,
    DIALOGS_FILE_SAVE_WINDOW_MODAL,
    DIALOGS_FILE_OPEN_GENERIC,
    DIALOGS_FILES_OPEN_GENERIC,
    DIALOGS_FILE_SAVE_GENERIC,
    DIALOGS_FILE_USE_CUSTOMIZER,
    DIALOGS_FILE_USE_EXTRA_CONTROL_CREATOR,
    DIALOGS_MAC_TOGGLE_ALWAYS_SHOW_TYPES,
    DIALOGS_DIR_CHOOSE,
    DIALOGS_DIR_CHOOSE_WINDOW_MODAL,
    DIALOGS_DIRNEW_CHOOSE,
    DIALOGS_DIRMULTIPLE_CHOOSE,
    DIALOGS_GENERIC_DIR_CHOOSE,
    DIALOGS_TIP,
    DIALOGS_NUM_ENTRY,
    DIALOGS_LOG_DIALOG,
    DIALOGS_INFOBAR_SIMPLE,
    DIALOGS_INFOBAR_SIMPLE_WRAPPED,
    DIALOGS_INFOBAR_ADVANCED,
    DIALOGS_MODAL,
    DIALOGS_MODELESS,
    DIALOGS_CENTRE_SCREEN,
    DIALOGS_CENTRE_PARENT,
    DIALOGS_MINIFRAME,
    DIALOGS_ONTOP,
    DIALOGS_MODELESS_BTN,
    DIALOGS_PROGRESS,
    DIALOGS_PROGRESS_GENERIC,
    DIALOGS_APP_PROGRESS,
    DIALOGS_ABOUTDLG_SIMPLE,
    DIALOGS_ABOUTDLG_FANCY,
    DIALOGS_ABOUTDLG_FULL,
    DIALOGS_ABOUTDLG_CUSTOM,
    DIALOGS_BUSYINFO,
    DIALOGS_BUSYINFO_RICH,
    DIALOGS_FIND,
    DIALOGS_REPLACE,
    DIALOGS_REQUEST,
    DIALOGS_NOTIFY_MSG,
    DIALOGS_SHOW_TIP,
    DIALOGS_RICHTIP_DIALOG,
    DIALOGS_PROPERTY_SHEET,
    DIALOGS_PROPERTY_SHEET_TOOLBOOK,
    DIALOGS_PROPERTY_SHEET_BUTTONTOOLBOOK,
    DIALOGS_STANDARD_BUTTON_SIZER_DIALOG,
    DIALOGS_TEST_DEFAULT_ACTION,
    DIALOGS_MODAL_HOOK,
    DIALOGS_SIMULATE_UNSAVED
};

#endif

