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

MAKE_CONST_WXSTRING(FileSelectorPromptStr);
MAKE_CONST_WXSTRING(DirSelectorPromptStr);
MAKE_CONST_WXSTRING(DirDialogNameStr);
MAKE_CONST_WXSTRING(FileSelectorDefaultWildcardStr);
MAKE_CONST_WXSTRING(GetTextFromUserPromptStr);
MAKE_CONST_WXSTRING(MessageBoxCaptionStr);

//---------------------------------------------------------------------------


DocStr(wxColourData,
       "This class holds a variety of information related to colour dialogs.");
    
class wxColourData : public wxObject {
public:
    DocCtorStr(
        wxColourData(),
        "Constructor, sets default values.");
    
    ~wxColourData();

    
    DocDeclStr(
        bool , GetChooseFull(),
        "Under Windows, determines whether the Windows colour dialog will display\n"
        "the full dialog with custom colour selection controls. Has no meaning\n"
        "under other platforms.  The default value is true.");
    
    DocDeclStr(
        wxColour , GetColour(),
        "Gets the colour (pre)selected by the dialog.");
    
    DocDeclStr(
        wxColour , GetCustomColour(int i),
        "Gets the i'th custom colour associated with the colour dialog. i should\n"
        "be an integer between 0 and 15. The default custom colours are all white.");
    
    DocDeclStr(
        void , SetChooseFull(int flag),
        "Under Windows, tells the Windows colour dialog to display the full dialog\n"
        "with custom colour selection controls. Under other platforms, has no effect.\n"
        "The default value is true.");
    
    DocDeclStr(
        void , SetColour(const wxColour& colour),
        "Sets the default colour for the colour dialog.  The default colour is black.");
    
    DocDeclStr(
        void , SetCustomColour(int i, const wxColour& colour),
        "Sets the i'th custom colour for the colour dialog. i should be an integer\n"
        "between 0 and 15. The default custom colours are all white.");
    
};


DocStr(wxColourDialog,
       "This class represents the colour chooser dialog.");

class wxColourDialog : public wxDialog {
public:
    %pythonAppend wxColourDialog   "self._setOORInfo(self)"

   DocCtorStr(
       wxColourDialog(wxWindow* parent, wxColourData* data = NULL),
       "Constructor. Pass a parent window, and optionally a ColourData, which\n"
       "will be copied to the colour dialog's internal ColourData instance.");

    DocDeclStr(
        wxColourData& , GetColourData(),
        "Returns a reference to the ColourData used by the dialog.");
};


//--------------------------------------------------------------------------------


DocStr(wxDirDialog,
       "This class represents the directory chooser dialog.");

RefDoc(wxDirDialog, "
 Styles
    wxDD_NEW_DIR_BUTTON     Add \"Create new directory\" button and allow
                            directory names to be editable. On Windows the new
                            directory button is only available with recent
                            versions of the common dialogs.");

class wxDirDialog : public wxDialog {
public:
    %pythonAppend wxDirDialog   "self._setOORInfo(self)"
    
    DocCtorStr(
        wxDirDialog(wxWindow* parent,
                    const wxString& message = wxPyDirSelectorPromptStr,
                    const wxString& defaultPath = wxPyEmptyString,
                    long style = 0,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    const wxString& name = wxPyDirDialogNameStr),
        "Constructor.  Use ShowModal method to show the dialog.");


    DocDeclStr(
        wxString , GetPath(),
        "Returns the default or user-selected path.");
    
    DocDeclStr(
        wxString , GetMessage(),
        "Returns the message that will be displayed on the dialog.");
    
    DocDeclStr(
        long , GetStyle(),
        "Returns the dialog style.");
    
    DocDeclStr(
        void , SetMessage(const wxString& message),
        "Sets the message that will be displayed on the dialog.");
    
    DocDeclStr(
        void , SetPath(const wxString& path),
        "Sets the default path.");
    
};


//---------------------------------------------------------------------------

DocStr(wxFileDialog,
       "This class represents the file chooser dialog.");

RefDoc(wxFileDialog, "
In Windows, this is the common file selector dialog. In X, this is a file
selector box with somewhat less functionality. The path and filename are
distinct elements of a full file pathname. If path is \"\", the current
directory will be used. If filename is \"\", no default filename will be
supplied. The wildcard determines what files are displayed in the file
selector, and file extension supplies a type extension for the required
filename.

Both the X and Windows versions implement a wildcard filter. Typing a filename
containing wildcards (*, ?) in the filename text item, and clicking on Ok,
will result in only those files matching the pattern being displayed. The
wildcard may be a specification for multiple types of file with a description
for each, such as:

   \"BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif\"

 Styles
    wx.OPEN              This is an open dialog.

    wx.SAVE              This is a save dialog.

    wx.HIDE_READONLY     For open dialog only: hide the checkbox allowing to
                         open the file in read-only mode.

    wx.OVERWRITE_PROMPT  For save dialog only: prompt for a confirmation if a
                         file will be overwritten.

    wx.MULTIPLE          For open dialog only: allows selecting multiple files.

    wx.CHANGE_DIR        Change the current working directory to the directory
                         where the file(s) chosen by the user are.
");



class wxFileDialog : public wxDialog {
public:
    %pythonAppend wxFileDialog   "self._setOORInfo(self)"

    DocCtorStr(
        wxFileDialog(wxWindow* parent,
                     const wxString& message = wxPyFileSelectorPromptStr,
                     const wxString& defaultDir = wxPyEmptyString,
                     const wxString& defaultFile = wxPyEmptyString,
                     const wxString& wildcard = wxPyFileSelectorDefaultWildcardStr,
                     long style = 0,
                     const wxPoint& pos = wxDefaultPosition),
        "Constructor.  Use ShowModal method to show the dialog.");


    DocDeclStr(
        void , SetMessage(const wxString& message),
        "Sets the message that will be displayed on the dialog.");
    
    DocDeclStr(
        void , SetPath(const wxString& path),
        "Sets the path (the combined directory and filename that will\n"
        "be returned when the dialog is dismissed).");
    
    DocDeclStr(
        void , SetDirectory(const wxString& dir),
        "Sets the default directory.");
    
    DocDeclStr(
        void , SetFilename(const wxString& name),
        "Sets the default filename.");
    
    DocDeclStr(
        void , SetWildcard(const wxString& wildCard),
        "Sets the wildcard, which can contain multiple file types, for example:\n"
        "    \"BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif\"");
    
    DocDeclStr(
        void , SetStyle(long style),
        "Sets the dialog style.");
    
    DocDeclStr(
        void , SetFilterIndex(int filterIndex),
        "Sets the default filter index, starting from zero.");
    

    DocDeclStr(
        wxString , GetMessage() const,
        "Returns the message that will be displayed on the dialog.");
    
    DocDeclStr(
        wxString , GetPath() const,
        "Returns the full path (directory and filename) of the selected file.");
    
    DocDeclStr(
        wxString , GetDirectory() const,
        "Returns the default directory.");
    
    DocDeclStr(
        wxString , GetFilename() const,
        "Returns the default filename.");
    
    DocDeclStr(
        wxString , GetWildcard() const,
        "Returns the file dialog wildcard.");
    
    DocDeclStr(
        long , GetStyle() const,
        "Returns the dialog style.");
    
    DocDeclStr(
        int , GetFilterIndex() const,
        "Returns the index into the list of filters supplied, optionally, in\n"
        "the wildcard parameter. Before the dialog is shown, this is the index\n"
        "which will be used when the dialog is first displayed. After the dialog\n"
        "is shown, this is the index selected by the user.");


    DocStr(GetFilenames,
        "Returns a list of filenames chosen in the dialog.  This function should\n"
        "only be used with the dialogs which have wx.MULTIPLE style, use\n"
        "GetFilename for the others.");

    DocStr(GetPaths,
        "Fills the array paths with the full paths of the files chosen. This\n"
        "function should only be used with the dialogs which have wx.MULTIPLE style,\n"
        "use GetPath for the others.");   
    
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
    %pythonAppend wxMultiChoiceDialog   "self._setOORInfo(self)"
    
    wxMultiChoiceDialog(wxWindow *parent,
                        const wxString& message,
                        const wxString& caption,
                        int choices=0, wxString* choices_array,
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
    %pythonAppend wxSingleChoiceDialog   "self._setOORInfo(self)"
    
    %extend {
        // TODO: ignoring clientData for now...  FIX THIS
        //       SWIG is messing up the &/*'s for some reason.
        wxSingleChoiceDialog(wxWindow* parent,
                             const wxString& message,
                             const wxString& caption,
                             int choices, wxString* choices_array,
                             //char** clientData = NULL,
                             long style = wxCHOICEDLG_STYLE,
                             const wxPoint& pos = wxDefaultPosition) {
            return new wxSingleChoiceDialog(parent, message, caption,
                                            choices, choices_array, NULL, style, pos);
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
    %pythonAppend wxTextEntryDialog   "self._setOORInfo(self)"
    
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
    %pythonAppend wxFontDialog   "self._setOORInfo(self)"
    
    wxFontDialog(wxWindow* parent, const wxFontData& data);

    wxFontData& GetFontData();
    int ShowModal();
};


//---------------------------------------------------------------------------

class wxMessageDialog : public wxDialog {
public:
    %pythonAppend wxMessageDialog   "self._setOORInfo(self)"
    
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
    %pythonAppend wxProgressDialog   "self._setOORInfo(self)"
    
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



%constant wxEventType wxEVT_COMMAND_FIND;
%constant wxEventType wxEVT_COMMAND_FIND_NEXT;
%constant wxEventType wxEVT_COMMAND_FIND_REPLACE;
%constant wxEventType wxEVT_COMMAND_FIND_REPLACE_ALL;
%constant wxEventType wxEVT_COMMAND_FIND_CLOSE;


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
    const wxString& GetFindString();
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
    %pythonAppend wxFindReplaceDialog     "self._setOORInfo(self)"
    %pythonAppend wxFindReplaceDialog()   ""

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
