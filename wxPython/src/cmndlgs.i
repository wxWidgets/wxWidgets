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
#include <wx/fdrepdlg.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i
%import windows.i
%import events.i
%import frames.i

%pragma(python) code = "import wx"

//----------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    DECLARE_DEF_STRING(FileSelectorPromptStr);
    DECLARE_DEF_STRING(DirSelectorPromptStr);
    DECLARE_DEF_STRING(DirDialogNameStr);
    DECLARE_DEF_STRING(FileSelectorDefaultWildcardStr);
    DECLARE_DEF_STRING(GetTextFromUserPromptStr);
    DECLARE_DEF_STRING(MessageBoxCaptionStr);
    static const wxString wxPyEmptyString(wxT(""));

%}

//----------------------------------------------------------------------

class wxColourData : public wxObject {
public:
    wxColourData();
    ~wxColourData();

    bool GetChooseFull();
    wxColour GetColour();
    wxColour GetCustomColour(int i);
    void SetChooseFull(int flag);
    void SetColour(const wxColour& colour);
    void SetCustomColour(int i, const wxColour& colour);
};


class wxColourDialog : public wxDialog {
public:
    wxColourDialog(wxWindow* parent, wxColourData* data = NULL);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    wxColourData& GetColourData();
    int ShowModal();
};


//----------------------------------------------------------------------

class wxDirDialog : public wxDialog {
public:
    wxDirDialog(wxWindow* parent,
                const wxString& message = wxPyDirSelectorPromptStr,
                const wxString& defaultPath = wxPyEmptyString,
                long style = 0,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxPyDirDialogNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

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
                 const wxString& message = wxPyFileSelectorPromptStr,
                 const wxString& defaultDir = wxPyEmptyString,
                 const wxString& defaultFile = wxPyEmptyString,
                 const wxString& wildcard = wxPyFileSelectorDefaultWildcardStr,
                 long style = 0,
                 const wxPoint& pos = wxDefaultPosition);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

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

    %addmethods {
        PyObject* GetFilenames() {
            wxArrayString arr;
            self->GetFilenames(arr);
            return wxArrayString2PyList_helper(arr);
        }

        PyObject* GetPaths() {
            wxArrayString arr;
            self->GetPaths(arr);
            return wxArrayString2PyList_helper(arr);
        }
    }
};


//----------------------------------------------------------------------

enum { wxCHOICEDLG_STYLE };

class wxMultiChoiceDialog : public wxDialog
{
public:
    wxMultiChoiceDialog(wxWindow *parent,
                        const wxString& message,
                        const wxString& caption,
                        int LCOUNT, wxString *choices,
                        long style = wxCHOICEDLG_STYLE,
                        const wxPoint& pos = wxDefaultPosition);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    void SetSelections(const wxArrayInt& selections);

    // wxArrayInt GetSelections() const;
    %addmethods {
        PyObject* GetSelections() {
            return wxArrayInt2PyList_helper(self->GetSelections());
        }
    }
};


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
                             long style = wxCHOICEDLG_STYLE,
                             wxPoint* pos = &wxDefaultPosition) {
            return new wxSingleChoiceDialog(parent, *message, *caption,
                                            LCOUNT, choices, NULL, style, *pos);
        }
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    }

    int GetSelection();
    wxString GetStringSelection();
    void SetSelection(int sel);
    int ShowModal();
};


//----------------------------------------------------------------------

class wxTextEntryDialog : public wxDialog {
public:
    wxTextEntryDialog(wxWindow* parent,
                      const wxString& message,
                      const wxString& caption = wxPyGetTextFromUserPromptStr,
                      const wxString& defaultValue = wxPyEmptyString,
                      long style = wxOK | wxCANCEL | wxCENTRE,
                      const wxPoint& pos = wxDefaultPosition);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    wxString GetValue();
    void SetValue(const wxString& value);
    int ShowModal();
};

//----------------------------------------------------------------------

class wxFontData : public wxObject {
public:
    wxFontData();
    ~wxFontData();

    void EnableEffects(bool enable);
    bool GetAllowSymbols();
    wxColour GetColour();
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
    wxFontDialog(wxWindow* parent, const wxFontData& data);
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    wxFontData& GetFontData();
    int ShowModal();
};


//----------------------------------------------------------------------

class wxMessageDialog : public wxDialog {
public:
    wxMessageDialog(wxWindow* parent,
                    const wxString& message,
                    const wxString& caption = wxPyMessageBoxCaptionStr,
                    long style = wxOK | wxCANCEL | wxCENTRE,
                    const wxPoint& pos = wxDefaultPosition);
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

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
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    bool Update(int value, const wxString& newmsg = wxPyEmptyString);
    void Resume();
}

//----------------------------------------------------------------------

enum wxFindReplaceFlags
{
    // downward search/replace selected (otherwise - upwards)
    wxFR_DOWN       = 1,

    // whole word search/replace selected
    wxFR_WHOLEWORD  = 2,

    // case sensitive search/replace selected (otherwise - case insensitive)
    wxFR_MATCHCASE  = 4
};


enum wxFindReplaceDialogStyles
{
    // replace dialog (otherwise find dialog)
    wxFR_REPLACEDIALOG = 1,

    // don't allow changing the search direction
    wxFR_NOUPDOWN      = 2,

    // don't allow case sensitive searching
    wxFR_NOMATCHCASE   = 4,

    // don't allow whole word searching
    wxFR_NOWHOLEWORD   = 8
};

enum {
    wxEVT_COMMAND_FIND,
    wxEVT_COMMAND_FIND_NEXT,
    wxEVT_COMMAND_FIND_REPLACE,
    wxEVT_COMMAND_FIND_REPLACE_ALL,
    wxEVT_COMMAND_FIND_CLOSE,
};

%pragma(python) code = "

def EVT_COMMAND_FIND(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_FIND, func)

def EVT_COMMAND_FIND_NEXT(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_FIND_NEXT, func)

def EVT_COMMAND_FIND_REPLACE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_FIND_REPLACE, func)

def EVT_COMMAND_FIND_REPLACE_ALL(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_FIND_REPLACE_ALL, func)

def EVT_COMMAND_FIND_CLOSE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_FIND_CLOSE, func)

"

class wxFindDialogEvent : public wxCommandEvent
{
public:
    wxFindDialogEvent(wxEventType commandType = wxEVT_NULL, int id = 0);
    int GetFlags();
    wxString GetFindString();
    const wxString& GetReplaceString();
    wxFindReplaceDialog *GetDialog();
    void SetFlags(int flags);
    void SetFindString(const wxString& str);
    void SetReplaceString(const wxString& str);
};



class wxFindReplaceData : public wxObject
{
public:
    wxFindReplaceData(int flags=0);
    ~wxFindReplaceData();

    const wxString& GetFindString();
    const wxString& GetReplaceString();
    int GetFlags();
    void SetFlags(int flags);
    void SetFindString(const wxString& str);
    void SetReplaceString(const wxString& str);
};


class wxFindReplaceDialog : public wxDialog {
public:
    wxFindReplaceDialog(wxWindow *parent,
                        wxFindReplaceData *data,
                        const wxString &title,
                        int style = 0);
    %name(wxPreFindReplaceDialog)wxFindReplaceDialog();

     bool Create(wxWindow *parent,
                 wxFindReplaceData *data,
                 const wxString &title,
                 int style = 0);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreFindReplaceDialog:val._setOORInfo(val)"

    const wxFindReplaceData *GetData();
    void SetData(wxFindReplaceData *data);
};

//----------------------------------------------------------------------
