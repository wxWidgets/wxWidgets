/////////////////////////////////////////////////////////////////////////////
// Name:        cmndlgs.i
// Purpose:     SWIG definitions for the Common Dialog Classes
//
// Author:      Robin Dunn
//
// Created:     7/25/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module cmndlgs

%{
#include "helpers.h"
#include <wx/colordlg.h>
#include <wx/dirdlg.h>
#include <wx/fontdlg.h>
#include <wx/progdlg.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i
%import windows.i
%import frames.i

%pragma(python) code = "import wx"

//----------------------------------------------------------------------

class wxColourData {
public:
    wxColourData();
    ~wxColourData();

    bool GetChooseFull();
    wxColour& GetColour();
    wxColour GetCustomColour(int i);
    void SetChooseFull(int flag);
    void SetColour(const wxColour& colour);
    void SetCustomColour(int i, const wxColour& colour);
};


class wxColourDialog : public wxDialog {
public:
    wxColourDialog(wxWindow* parent, wxColourData* data = NULL);

    %pragma(python) addtomethod = "__init__:wx._StdDialogCallbacks(self)"

    wxColourData& GetColourData();
    int ShowModal();
};


//----------------------------------------------------------------------

class wxDirDialog : public wxDialog {
public:
    wxDirDialog(wxWindow* parent,
                char* message = "Choose a directory",
                char* defaultPath = "",
                long style = 0,
                const wxPoint& pos = wxPyDefaultPosition);

    %pragma(python) addtomethod = "__init__:wx._StdDialogCallbacks(self)"

    wxString GetPath();
    wxString GetMessage();
    long GetStyle();
    void SetMessage(const wxString& message);
    void SetPath(const wxString& path);
    int ShowModal();
};

//----------------------------------------------------------------------

class wxFileDialog : public wxDialog {
public:
    wxFileDialog(wxWindow* parent,
                 char* message = "Choose a file",
                 char* defaultDir = "",
                 char* defaultFile = "",
                 char* wildcard = "*.*",
                 long style = 0,
                 const wxPoint& pos = wxPyDefaultPosition);

    %pragma(python) addtomethod = "__init__:wx._StdDialogCallbacks(self)"

    wxString GetDirectory();
    wxString GetFilename();
    int GetFilterIndex();
    wxString GetMessage();
    wxString GetPath();
    long GetStyle();
    wxString GetWildcard();
    void SetDirectory(const wxString& directory);
    void SetFilename(const wxString& setfilename);
    void SetFilterIndex(int filterIndex);
    void SetMessage(const wxString& message);
    void SetPath(const wxString& path);
    void SetStyle(long style);
    void SetWildcard(const wxString& wildCard);
    int ShowModal();
};


//----------------------------------------------------------------------

//TODO: wxMultipleChoiceDialog

//----------------------------------------------------------------------

class wxSingleChoiceDialog : public wxDialog {
public:
    %addmethods {
        // TODO: ignoring clientData for now...
        //       SWIG is messing up the &/*'s for some reason.
        wxSingleChoiceDialog(wxWindow* parent,
                             wxString* message,
                             wxString* caption,
                             int LCOUNT, wxString* choices,
                             //char** clientData = NULL,
                             long style = wxOK | wxCANCEL | wxCENTRE,
                             wxPoint* pos = &wxPyDefaultPosition) {
            return new wxSingleChoiceDialog(parent, *message, *caption,
                                            LCOUNT, choices, NULL, style, *pos);
        }
    }

    %pragma(python) addtomethod = "__init__:wx._StdDialogCallbacks(self)"

    int GetSelection();
    wxString GetStringSelection();
    void SetSelection(int sel);
    int ShowModal();
};


//----------------------------------------------------------------------

class wxTextEntryDialog : public wxDialog {
public:
    wxTextEntryDialog(wxWindow* parent,
                      char* message,
                      char* caption = "Input Text",
                      char* defaultValue = "",
                      long style = wxOK | wxCANCEL | wxCENTRE,
                      const wxPoint& pos = wxPyDefaultPosition);

    %pragma(python) addtomethod = "__init__:wx._StdDialogCallbacks(self)"

    wxString GetValue();
    void SetValue(const wxString& value);
    int ShowModal();
};

//----------------------------------------------------------------------

class wxFontData {
public:
    wxFontData();
    ~wxFontData();

    void EnableEffects(bool enable);
    bool GetAllowSymbols();
    wxColour& GetColour();
    wxFont GetChosenFont();
    bool GetEnableEffects();
    wxFont GetInitialFont();
    bool GetShowHelp();
    void SetAllowSymbols(bool allowSymbols);
    void SetChosenFont(const wxFont& font);
    void SetColour(const wxColour& colour);
    void SetInitialFont(const wxFont& font);
    void SetRange(int min, int max);
    void SetShowHelp(bool showHelp);
};


class wxFontDialog : public wxDialog {
public:
    wxFontDialog(wxWindow* parent, wxFontData* data = NULL);

    %pragma(python) addtomethod = "__init__:wx._StdDialogCallbacks(self)"

    wxFontData& GetFontData();
    int ShowModal();
};


//----------------------------------------------------------------------

class wxMessageDialog : public wxDialog {
public:
    wxMessageDialog(wxWindow* parent,
                    char* message,
                    char* caption = "Message box",
                    long style = wxOK | wxCANCEL | wxCENTRE,
                    const wxPoint& pos = wxPyDefaultPosition);

    %pragma(python) addtomethod = "__init__:wx._StdDialogCallbacks(self)"

    int ShowModal();
};

//----------------------------------------------------------------------

class wxProgressDialog : public wxFrame {
public:
    wxProgressDialog(const wxString& title,
                     const wxString& message,
                     int maximum = 100,
                     wxWindow* parent = NULL,
                     int style = wxPD_AUTO_HIDE | wxPD_APP_MODAL );


    bool Update(int value = -1, const char* newmsg = NULL);
    void Resume();
}

//----------------------------------------------------------------------
