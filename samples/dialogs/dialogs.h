/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.h
// Purpose:     Common dialogs demo
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __DIALOGSH__
#define __DIALOGSH__

// Define a new application type
class MyApp: public wxApp
{
public:
    bool OnInit();

    wxFont       m_canvasFont;
    wxColour     m_canvasTextColour;
};

// A custom modeless dialog
class MyModelessDialog : public wxDialog
{
public:
    MyModelessDialog(wxWindow *parent);

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
    wxButton *m_btnFocused;
    wxButton *m_btnDelete;

    DECLARE_EVENT_TABLE()
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyFrame(wxWindow *parent, const wxString& title,
            const wxPoint& pos, const wxSize& size);

    void ChooseColour(wxCommandEvent& event);
    void ChooseFont(wxCommandEvent& event);
    void LogDialog(wxCommandEvent& event);
    void MessageBox(wxCommandEvent& event);
    void SingleChoice(wxCommandEvent& event);
    void TextEntry(wxCommandEvent& event);
    void PasswordEntry(wxCommandEvent& event);
    void NumericEntry(wxCommandEvent& event);
    void FileOpen(wxCommandEvent& event);
    void FileOpen2(wxCommandEvent& event);
    void FilesOpen(wxCommandEvent& event);
    void FileSave(wxCommandEvent& event);
    void DirChoose(wxCommandEvent& event);
    void GenericDirChoose(wxCommandEvent& event);
    void ShowTip(wxCommandEvent& event);
    void ModalDlg(wxCommandEvent& event);
    void ModelessDlg(wxCommandEvent& event);
    void ShowProgress(wxCommandEvent& event);

#if !defined(__WXMSW__) || wxTEST_GENERIC_DIALOGS_IN_MSW
    void ChooseColourGeneric(wxCommandEvent& event);
    void ChooseFontGeneric(wxCommandEvent& event);
#endif

    void OnExit(wxCommandEvent& event);

    void OnButton(wxCommandEvent& event);

private:
    MyModelessDialog *m_dialog;

    DECLARE_EVENT_TABLE()
};

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxWindow *parent) : wxScrolledWindow(parent) { }

    void OnPaint(wxPaintEvent& event);

    DECLARE_EVENT_TABLE()
};


// Menu IDs
enum
{
    DIALOGS_CHOOSE_COLOUR = 1,
    DIALOGS_CHOOSE_COLOUR_GENERIC,
    DIALOGS_CHOOSE_FONT,
    DIALOGS_CHOOSE_FONT_GENERIC,
    DIALOGS_MESSAGE_BOX,
    DIALOGS_SINGLE_CHOICE,
    DIALOGS_TEXT_ENTRY,
    DIALOGS_PASSWORD_ENTRY,
    DIALOGS_FILE_OPEN,
    DIALOGS_FILE_OPEN2,
    DIALOGS_FILES_OPEN,
    DIALOGS_FILE_SAVE,
    DIALOGS_DIR_CHOOSE,
    DIALOGS_GENERIC_DIR_CHOOSE,
    DIALOGS_TIP,
    DIALOGS_NUM_ENTRY,
    DIALOGS_LOG_DIALOG,
    DIALOGS_MODAL,
    DIALOGS_MODELESS,
    DIALOGS_MODELESS_BTN,
    DIALOGS_PROGRESS
};

#endif

