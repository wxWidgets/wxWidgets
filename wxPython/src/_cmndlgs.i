/////////////////////////////////////////////////////////////////////////////
// Name:        _cmndlgs.i
// Purpose:     SWIG interface for the "Common Dialog" classes
//
// Author:      Robin Dunn
//
// Created:     25-July-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{

    DECLARE_DEF_STRING(FileSelectorPromptStr);
    DECLARE_DEF_STRING(DirSelectorPromptStr);
    DECLARE_DEF_STRING(DirDialogNameStr);
    DECLARE_DEF_STRING(FileSelectorDefaultWildcardStr);
    DECLARE_DEF_STRING(GetTextFromUserPromptStr);
    DECLARE_DEF_STRING(MessageBoxCaptionStr);

%}

//---------------------------------------------------------------------------


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
    %addtofunc wxColourDialog   "self._setOORInfo(self)"

    wxColourDialog(wxWindow* parent, wxColourData* data = NULL);

    wxColourData& GetColourData();
    int ShowModal();
};


//--------------------------------------------------------------------------------

class wxDirDialog : public wxDialog {
public:
    %addtofunc wxDirDialog   "self._setOORInfo(self)"
    
    wxDirDialog(wxWindow* parent,
                const wxString& message = wxPyDirSelectorPromptStr,
                const wxString& defaultPath = wxPyEmptyString,
                long style = 0,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxPyDirDialogNameStr);


    wxString GetPath();
    wxString GetMessage();
    long GetStyle();
    void SetMessage(const wxString& message);
    void SetPath(const wxString& path);
    int ShowModal();
};


//---------------------------------------------------------------------------

class wxFileDialog : public wxDialog {
public:
    %addtofunc wxFileDialog   "self._setOORInfo(self)"
    
    wxFileDialog(wxWindow* parent,
                 const wxString& message = wxPyFileSelectorPromptStr,
                 const wxString& defaultDir = wxPyEmptyString,
                 const wxString& defaultFile = wxPyEmptyString,
                 const wxString& wildcard = wxPyFileSelectorDefaultWildcardStr,
                 long style = 0,
                 const wxPoint& pos = wxDefaultPosition);


    void SetMessage(const wxString& message);
    void SetPath(const wxString& path);
    void SetDirectory(const wxString& dir);
    void SetFilename(const wxString& name);
    void SetWildcard(const wxString& wildCard);
    void SetStyle(long style);
    void SetFilterIndex(int filterIndex);

    wxString GetMessage() const;
    wxString GetPath() const;
    wxString GetDirectory() const;
    wxString GetFilename() const;
    wxString GetWildcard() const;
    long GetStyle() const;
    int GetFilterIndex() const;

    %extend {
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

// TODO
//     // Utility functions

//     // Parses the wildCard, returning the number of filters.
//     // Returns 0 if none or if there's a problem,
//     // The arrays will contain an equal number of items found before the error.
//     // wildCard is in the form:
//     // "All files (*)|*|Image Files (*.jpeg *.png)|*.jpg;*.png"
//     static int ParseWildcard(const wxString& wildCard,
//                              wxArrayString& descriptions,
//                              wxArrayString& filters);

//     // Append first extension to filePath from a ';' separated extensionList
//     // if filePath = "path/foo.bar" just return it as is
//     // if filePath = "foo[.]" and extensionList = "*.jpg;*.png" return "foo.jpg"
//     // if the extension is "*.j?g" (has wildcards) or "jpg" then return filePath
//     static wxString AppendExtension(const wxString &filePath,
//                                     const wxString &extensionList);


};


//---------------------------------------------------------------------------

enum { wxCHOICEDLG_STYLE };

class wxMultiChoiceDialog : public wxDialog
{
public:
    %addtofunc wxMultiChoiceDialog   "self._setOORInfo(self)"
    
    wxMultiChoiceDialog(wxWindow *parent,
                        const wxString& message,
                        const wxString& caption,
                        int LCOUNT, wxString *choices,
                        long style = wxCHOICEDLG_STYLE,
                        const wxPoint& pos = wxDefaultPosition);

    void SetSelections(const wxArrayInt& selections);

    // wxArrayInt GetSelections() const;
    %extend {
        PyObject* GetSelections() {
            return wxArrayInt2PyList_helper(self->GetSelections());
        }
    }
};


//---------------------------------------------------------------------------

class wxSingleChoiceDialog : public wxDialog {
public:
    %addtofunc wxSingleChoiceDialog   "self._setOORInfo(self)"
    
    %extend {
        // TODO: ignoring clientData for now...  FIX THIS
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
    }

    int GetSelection();
    wxString GetStringSelection();
    void SetSelection(int sel);
    int ShowModal();
};


//---------------------------------------------------------------------------

class wxTextEntryDialog : public wxDialog {
public:
    %addtofunc wxTextEntryDialog   "self._setOORInfo(self)"
    
    wxTextEntryDialog(wxWindow* parent,
                      const wxString& message,
                      const wxString& caption = wxPyGetTextFromUserPromptStr,
                      const wxString& defaultValue = wxPyEmptyString,
                      long style = wxOK | wxCANCEL | wxCENTRE,
                      const wxPoint& pos = wxDefaultPosition);

    wxString GetValue();
    void SetValue(const wxString& value);
    int ShowModal();
};

//---------------------------------------------------------------------------

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
    %addtofunc wxFontDialog   "self._setOORInfo(self)"
    
    wxFontDialog(wxWindow* parent, const wxFontData& data);

    wxFontData& GetFontData();
    int ShowModal();
};


//---------------------------------------------------------------------------

class wxMessageDialog : public wxDialog {
public:
    %addtofunc wxMessageDialog   "self._setOORInfo(self)"
    
    wxMessageDialog(wxWindow* parent,
                    const wxString& message,
                    const wxString& caption = wxPyMessageBoxCaptionStr,
                    long style = wxOK | wxCANCEL | wxCENTRE,
                    const wxPoint& pos = wxDefaultPosition);

    int ShowModal();
};

//---------------------------------------------------------------------------

class wxProgressDialog : public wxFrame {
public:
    %addtofunc wxProgressDialog   "self._setOORInfo(self)"
    
    wxProgressDialog(const wxString& title,
                     const wxString& message,
                     int maximum = 100,
                     wxWindow* parent = NULL,
                     int style = wxPD_AUTO_HIDE | wxPD_APP_MODAL );

    bool Update(int value, const wxString& newmsg = wxPyEmptyString);
    void Resume();
};

//---------------------------------------------------------------------------

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


%pythoncode {
EVT_FIND = wx.PyEventBinder( wxEVT_COMMAND_FIND, 1 )
EVT_FIND_NEXT = wx.PyEventBinder( wxEVT_COMMAND_FIND_NEXT, 1 )
EVT_FIND_REPLACE = wx.PyEventBinder( wxEVT_COMMAND_FIND_REPLACE, 1 )
EVT_FIND_REPLACE_ALL = wx.PyEventBinder( wxEVT_COMMAND_FIND_REPLACE_ALL, 1 )
EVT_FIND_CLOSE = wx.PyEventBinder( wxEVT_COMMAND_FIND_CLOSE, 1 )

%# For backwards compatibility.  Should they be removed?
EVT_COMMAND_FIND             = EVT_FIND 
EVT_COMMAND_FIND_NEXT        = EVT_FIND_NEXT
EVT_COMMAND_FIND_REPLACE     = EVT_FIND_REPLACE
EVT_COMMAND_FIND_REPLACE_ALL = EVT_FIND_REPLACE_ALL
EVT_COMMAND_FIND_CLOSE       = EVT_FIND_CLOSE        
}


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
    %addtofunc wxFindReplaceDialog     "self._setOORInfo(self)"
    %addtofunc wxFindReplaceDialog()   "val._setOORInfo(val)"

    wxFindReplaceDialog(wxWindow *parent,
                        wxFindReplaceData *data,
                        const wxString &title,
                        int style = 0);
    %name(PreFindReplaceDialog)wxFindReplaceDialog();

     bool Create(wxWindow *parent,
                 wxFindReplaceData *data,
                 const wxString &title,
                 int style = 0);

    const wxFindReplaceData *GetData();
    void SetData(wxFindReplaceData *data);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
