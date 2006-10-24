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
"This class holds a variety of information related to the colour
chooser dialog, used to transfer settings and results to and from the
`wx.ColourDialog`.", "");

class wxColourData : public wxObject {
public:
    DocCtorStr(
        wxColourData(),
        "Constructor, sets default values.", "");

    ~wxColourData();


    DocDeclStr(
        bool , GetChooseFull(),
        "Under Windows, determines whether the Windows colour dialog will
display the full dialog with custom colour selection controls. Has no
meaning under other platforms.  The default value is true.", "");

    DocDeclStr(
        wxColour , GetColour(),
        "Gets the colour (pre)selected by the dialog.", "");

    DocDeclStr(
        wxColour , GetCustomColour(int i),
        "Gets the i'th custom colour associated with the colour dialog. i
should be an integer between 0 and 15. The default custom colours are
all invalid colours.", "");

    DocDeclStr(
        void , SetChooseFull(int flag),
        "Under Windows, tells the Windows colour dialog to display the full
dialog with custom colour selection controls. Under other platforms,
has no effect.  The default value is true.", "");

    DocDeclStr(
        void , SetColour(const wxColour& colour),
        "Sets the default colour for the colour dialog.  The default colour is
black.", "");

    DocDeclStr(
        void , SetCustomColour(int i, const wxColour& colour),
        "Sets the i'th custom colour for the colour dialog. i should be an
integer between 0 and 15. The default custom colours are all invalid colours.", "");


    %property(ChooseFull, GetChooseFull, SetChooseFull, doc="See `GetChooseFull` and `SetChooseFull`");
    %property(Colour, GetColour, SetColour, doc="See `GetColour` and `SetColour`");
    %property(CustomColour, GetCustomColour, SetCustomColour, doc="See `GetCustomColour` and `SetCustomColour`");
   
};




DocStr(wxColourDialog,
       "This class represents the colour chooser dialog.", "");

MustHaveApp(wxColourDialog);

class wxColourDialog : public wxDialog {
public:
    %pythonAppend wxColourDialog   "self._setOORInfo(self)"

   DocCtorStr(
       wxColourDialog(wxWindow* parent, wxColourData* data = NULL),
       "Constructor. Pass a parent window, and optionally a `wx.ColourData`,
which will be copied to the colour dialog's internal ColourData
instance.", "");

    DocDeclStr(
        wxColourData& , GetColourData(),
        "Returns a reference to the `wx.ColourData` used by the dialog.", "");

    %property(ColourData, GetColourData, doc="See `GetColourData`");
};


wxColour wxGetColourFromUser(wxWindow *parent = (wxWindow *)NULL,
                             const wxColour& colInit = wxNullColour,
                             const wxString& caption = wxPyEmptyString);


//--------------------------------------------------------------------------------

enum {
    wxDD_NEW_DIR_BUTTON,
    wxDD_DEFAULT_STYLE,
    wxDD_CHANGE_DIR,
};

DocStr(wxDirDialog,
       "wx.DirDialog allows the user to select a directory by browising the
file system.", "

Window  Styles
--------------
    ====================  ==========================================
    wx.DD_DEFAULT_STYLE   Equivalent to a combination of
                          wx.DEFAULT_DIALOG_STYLE, wx.DD_NEW_DIR_BUTTON
                          and wx.RESIZE_BORDER.

    wx.DD_NEW_DIR_BUTTON  Add 'Create new directory' button and allow
                          directory names to be editable. On Windows
                          the new directory button is only available
                          with recent versions of the common dialogs.

    wx.DD_CHANGE_DIR      Change the current working directory to the
                          directory chosen by the user.
    ====================  ==========================================
");

MustHaveApp(wxDirDialog);

class wxDirDialog : public wxDialog {
public:
    %pythonAppend wxDirDialog   "self._setOORInfo(self)"
    %pythonAppend wxDirDialog() ""

    DocCtorStr(
        wxDirDialog(wxWindow* parent,
                    const wxString& message = wxPyDirSelectorPromptStr,
                    const wxString& defaultPath = wxPyEmptyString,
                    long style = wxDD_DEFAULT_STYLE,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    const wxString& name = wxPyDirDialogNameStr),
        "Constructor.  Use ShowModal method to show the dialog.", "");

//     %RenameCtor(PreDirDialog, wxDirDialog());

//     bool Create(wxWindow *parent,
//                 const wxString& title = wxDirSelectorPromptStr,
//                 const wxString& defaultPath = wxEmptyString,
//                 long style = wxDD_DEFAULT_STYLE,
//                 const wxPoint& pos = wxDefaultPosition,
//                 const wxSize& sz = wxDefaultSize,
//                 const wxString& name = wxPyDirDialogNameStr);

    DocDeclStr(
        wxString , GetPath(),
        "Returns the default or user-selected path.", "");

    DocDeclStr(
        wxString , GetMessage(),
        "Returns the message that will be displayed on the dialog.", "");

    DocDeclStr(
        void , SetMessage(const wxString& message),
        "Sets the message that will be displayed on the dialog.", "");

    DocDeclStr(
        void , SetPath(const wxString& path),
        "Sets the default path.", "");

    %property(Message, GetMessage, SetMessage, doc="See `GetMessage` and `SetMessage`");
    %property(Path, GetPath, SetPath, doc="See `GetPath` and `SetPath`");
};


//---------------------------------------------------------------------------

%{
// for compatibility only    
#define wxHIDE_READONLY 0
%}

enum {
    // These will dissappear in 2.8
    wxOPEN,
    wxSAVE,
    wxOVERWRITE_PROMPT,
    wxFILE_MUST_EXIST,
    wxMULTIPLE,
    wxCHANGE_DIR,
    wxHIDE_READONLY,

    wxFD_OPEN,
    wxFD_SAVE,
    wxFD_OVERWRITE_PROMPT,
    wxFD_FILE_MUST_EXIST,
    wxFD_MULTIPLE,
    wxFD_CHANGE_DIR,
    wxFD_PREVIEW,
    wxFD_DEFAULT_STYLE,
};

DocStr(wxFileDialog,
"wx.FileDialog allows the user to select one or more files from the
filesystem.", "

In Windows, this is the common file selector dialog. On X based
platforms a generic alternative is used.  The path and filename are
distinct elements of a full file pathname. If path is \"\", the
current directory will be used. If filename is \"\", no default
filename will be supplied. The wildcard determines what files are
displayed in the file selector, and file extension supplies a type
extension for the required filename.

Both the X and Windows versions implement a wildcard filter. Typing a
filename containing wildcards (\*, ?) in the filename text item, and
clicking on Ok, will result in only those files matching the pattern
being displayed. The wildcard may be a specification for multiple
types of file with a description for each, such as::

   \"BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif\"


Window  Styles
--------------
    ======================   ==========================================
    wx.FD_OPEN               This is an open dialog.

    wx.FD_SAVE               This is a save dialog.

    wx.FD_OVERWRITE_PROMPT   For save dialog only: prompt for a confirmation
                             if a file will be overwritten.

    wx.FD_MULTIPLE           For open dialog only: allows selecting multiple
                             files.

    wx.FD_CHANGE_DIR         Change the current working directory to the
                             directory  where the file(s) chosen by the user
                             are.
    ======================   ==========================================
");



MustHaveApp(wxFileDialog);

class wxFileDialog : public wxDialog {
public:
    %pythonAppend wxFileDialog   "self._setOORInfo(self)"

    DocCtorStr(
        wxFileDialog(wxWindow* parent,
                     const wxString& message = wxPyFileSelectorPromptStr,
                     const wxString& defaultDir = wxPyEmptyString,
                     const wxString& defaultFile = wxPyEmptyString,
                     const wxString& wildcard = wxPyFileSelectorDefaultWildcardStr,
                     long style = wxFD_DEFAULT_STYLE,
                     const wxPoint& pos = wxDefaultPosition),
        "Constructor.  Use ShowModal method to show the dialog.", "");


    DocDeclStr(
        void , SetMessage(const wxString& message),
        "Sets the message that will be displayed on the dialog.", "");

    DocDeclStr(
        void , SetPath(const wxString& path),
        "Sets the path (the combined directory and filename that will be
returned when the dialog is dismissed).", "");

    DocDeclStr(
        void , SetDirectory(const wxString& dir),
        "Sets the default directory.", "");

    DocDeclStr(
        void , SetFilename(const wxString& name),
        "Sets the default filename.", "");

    DocDeclStr(
        void , SetWildcard(const wxString& wildCard),
        "Sets the wildcard, which can contain multiple file types, for
example::

    \"BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif\"
", "");

    DocDeclStr(
        void , SetFilterIndex(int filterIndex),
        "Sets the default filter index, starting from zero.", "");

    DocDeclStr(
        wxString , GetMessage() const,
        "Returns the message that will be displayed on the dialog.", "");

    DocDeclStr(
        wxString , GetPath() const,
        "Returns the full path (directory and filename) of the selected file.", "");

    DocDeclStr(
        wxString , GetDirectory() const,
        "Returns the default directory.", "");

    DocDeclStr(
        wxString , GetFilename() const,
        "Returns the default filename.", "");

    DocDeclStr(
        wxString , GetWildcard() const,
        "Returns the file dialog wildcard.", "");

    DocDeclStr(
        int , GetFilterIndex() const,
        "Returns the index into the list of filters supplied, optionally, in
the wildcard parameter. Before the dialog is shown, this is the index
which will be used when the dialog is first displayed. After the
dialog is shown, this is the index selected by the user.", "");


    %extend {
        DocStr(GetFilenames,
               "Returns a list of filenames chosen in the dialog.  This function
should only be used with the dialogs which have wx.MULTIPLE style, use
GetFilename for the others.", "");
        PyObject* GetFilenames() {
            wxArrayString arr;
            self->GetFilenames(arr);
            return wxArrayString2PyList_helper(arr);
        }

        DocStr(GetPaths,
               "Fills the array paths with the full paths of the files chosen. This
function should only be used with the dialogs which have wx.MULTIPLE
style, use GetPath for the others.", "");

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


    %property(Directory, GetDirectory, SetDirectory, doc="See `GetDirectory` and `SetDirectory`");
    %property(Filename, GetFilename, SetFilename, doc="See `GetFilename` and `SetFilename`");
    %property(Filenames, GetFilenames, doc="See `GetFilenames`");
    %property(FilterIndex, GetFilterIndex, SetFilterIndex, doc="See `GetFilterIndex` and `SetFilterIndex`");
    %property(Message, GetMessage, SetMessage, doc="See `GetMessage` and `SetMessage`");
    %property(Path, GetPath, SetPath, doc="See `GetPath` and `SetPath`");
    %property(Paths, GetPaths, doc="See `GetPaths`");
    %property(Wildcard, GetWildcard, SetWildcard, doc="See `GetWildcard` and `SetWildcard`");
    
};


//---------------------------------------------------------------------------

enum { wxCHOICEDLG_STYLE };


DocStr(wxMultiChoiceDialog,
       "A simple dialog with a multi selection listbox.", "");

MustHaveApp(wxMultiChoiceDialog);

class wxMultiChoiceDialog : public wxDialog
{
public:
    %pythonAppend wxMultiChoiceDialog   "self._setOORInfo(self)"

    DocCtorAStr(
        wxMultiChoiceDialog(wxWindow *parent,
                            const wxString& message,
                            const wxString& caption,
                            int choices=0, wxString* choices_array=NULL,
                            long style = wxCHOICEDLG_STYLE,
                            const wxPoint& pos = wxDefaultPosition),
        "__init__(self, Window parent, String message, String caption,
    List choices=EmptyList, long style=CHOICEDLG_STYLE,
    Point pos=DefaultPosition) -> MultiChoiceDialog",
        "Constructor.  Use the `ShowModal` method to show the dialog.

    :param parent: The parent window.
    :param message: Text to display above the list of selections.
    :param caption: Text to use in the title bar of the dialog.
    :param choices: A list of strings or unicode objects that the
        user is allowed to choose from.
    :param style: Styles to apply to the dialog.  The default value is
        equivallent to wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.OK|wx.CANCEL|wx.CENTER.
    :param pos: Where to position the dialog (not used on Windows)

", "");


    DocDeclAStr(
        void, SetSelections(const wxArrayInt& selections),
        "SetSelections(List selections)",
        "Specify the items in the list that should be selected, using a list of
integers.  The list should specify the indexes of the items that
should be selected.", "");

    DocAStr(GetSelections,
            "GetSelections() -> [selections]",
            "Returns a list of integers representing the items that are selected.
If an item is selected then its index will appear in the list.", "");
    %extend {
        PyObject* GetSelections() {
            return wxArrayInt2PyList_helper(self->GetSelections());
        }
    }

    %property(Selections, GetSelections, SetSelections, doc="See `GetSelections` and `SetSelections`");
};


//---------------------------------------------------------------------------

DocStr(wxSingleChoiceDialog,
       "A simple dialog with a single selection listbox.", "");

MustHaveApp(wxSingleChoiceDialog);

class wxSingleChoiceDialog : public wxDialog {
public:
    %pythonAppend wxSingleChoiceDialog   "self._setOORInfo(self)"

    DocAStr(wxSingleChoiceDialog,
            "__init__(Window parent, String message, String caption,
    List choices=EmptyList, long style=CHOICEDLG_STYLE,
    Point pos=DefaultPosition) -> SingleChoiceDialog",
            "Constructor.  Use ShowModal method to show the dialog.", "");

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

    DocDeclStr(
        int , GetSelection(),
        "Get the index of the currently selected item.", "");

    DocDeclStr(
        wxString , GetStringSelection(),
        "Returns the string value of the currently selected item", "");

    DocDeclStr(
        void , SetSelection(int sel),
        "Set the current selected item to sel", "");

    %property(Selection, GetSelection, SetSelection, doc="See `GetSelection` and `SetSelection`");
    %property(StringSelection, GetStringSelection, doc="See `GetStringSelection`");
};


//---------------------------------------------------------------------------

DocStr(wxTextEntryDialog,
       "A dialog with text control, [ok] and [cancel] buttons", "");

MustHaveApp(wxTextEntryDialog);

enum { wxTextEntryDialogStyle };

class wxTextEntryDialog : public wxDialog {
public:
    %pythonAppend wxTextEntryDialog   "self._setOORInfo(self)"

    DocCtorStr(
        wxTextEntryDialog(wxWindow* parent,
                          const wxString& message,
                          const wxString& caption = wxPyGetTextFromUserPromptStr,
                          const wxString& defaultValue = wxPyEmptyString,
                          long style = wxTextEntryDialogStyle,
                          const wxPoint& pos = wxDefaultPosition),
        "Constructor.  Use ShowModal method to show the dialog.", "");

    DocDeclStr(
        wxString , GetValue(),
        "Returns the text that the user has entered if the user has pressed OK,
or the original value if the user has pressed Cancel.", "");

    DocDeclStr(
        void , SetValue(const wxString& value),
        "Sets the default text value.", "");
    
    %property(Value, GetValue, SetValue, doc="See `GetValue` and `SetValue`");
};

//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(GetPasswordFromUserPromptStr);

class wxPasswordEntryDialog : public wxTextEntryDialog
{
public:
    wxPasswordEntryDialog(wxWindow *parent,
                      const wxString& message,
                      const wxString& caption = wxPyGetPasswordFromUserPromptStr,
                      const wxString& value = wxPyEmptyString,
                      long style = wxTextEntryDialogStyle,
                      const wxPoint& pos = wxDefaultPosition);
};

//---------------------------------------------------------------------------

DocStr( wxNumberEntryDialog,
"A dialog with spin control, ok and cancel buttons.", "")

MustHaveApp(wxNumberEntryDialog);

class wxNumberEntryDialog : public wxDialog
{
public:
    %pythonAppend wxNumberEntryDialog   "self._setOORInfo(self)"
    
    DocCtorStr(
        wxNumberEntryDialog(wxWindow *parent,
                            const wxString& message,
                            const wxString& prompt,
                            const wxString& caption,
                            long value, long min, long max,
                            const wxPoint& pos = wxDefaultPosition),
        "Constructor.  Use ShowModal method to show the dialog.", "");

    long GetValue();

    %property(Value, GetValue, doc="See `GetValue`");
   
};

//---------------------------------------------------------------------------


DocStr(wxFontData,
       "This class holds a variety of information related to font dialogs and
is used to transfer settings to and results from a `wx.FontDialog`.", "");


class wxFontData : public wxObject {
public:
    wxFontData();
    ~wxFontData();

    DocDeclStr(
        void , EnableEffects(bool enable),
        "Enables or disables 'effects' under MS Windows only. This refers to
the controls for manipulating colour, strikeout and underline
properties.  The default value is true.", "");

    DocDeclStr(
        bool , GetAllowSymbols(),
        "Under MS Windows, returns a flag determining whether symbol fonts can
be selected. Has no effect on other platforms. The default value is
true.", "");

    DocDeclStr(
        wxColour , GetColour(),
        "Gets the colour associated with the font dialog. The default value is
black.", "");

    DocDeclStr(
        wxFont , GetChosenFont(),
        "Gets the font chosen by the user.", "");

    DocDeclStr(
        bool , GetEnableEffects(),
        "Determines whether 'effects' are enabled under Windows.", "");

    DocDeclStr(
        wxFont , GetInitialFont(),
        "Gets the font that will be initially used by the font dialog. This
should have previously been set by the application.", "");

    DocDeclStr(
        bool , GetShowHelp(),
        "Returns true if the Help button will be shown (Windows only).  The
default value is false.", "");

    DocDeclStr(
        void , SetAllowSymbols(bool allowSymbols),
        "Under MS Windows, determines whether symbol fonts can be selected. Has
no effect on other platforms.  The default value is true.", "");

    DocDeclStr(
        void , SetChosenFont(const wxFont& font),
        "Sets the font that will be returned to the user (normally for internal
use only).", "");

    DocDeclStr(
        void , SetColour(const wxColour& colour),
        "Sets the colour that will be used for the font foreground colour.  The
default colour is black.", "");

    DocDeclStr(
        void , SetInitialFont(const wxFont& font),
        "Sets the font that will be initially used by the font dialog.", "");

    DocDeclStr(
        void , SetRange(int min, int max),
        "Sets the valid range for the font point size (Windows only).  The
default is 0, 0 (unrestricted range).", "");

    DocDeclStr(
        void , SetShowHelp(bool showHelp),
        "Determines whether the Help button will be displayed in the font
dialog (Windows only).  The default value is false.", "");

    %property(AllowSymbols, GetAllowSymbols, SetAllowSymbols, doc="See `GetAllowSymbols` and `SetAllowSymbols`");
    %property(ChosenFont, GetChosenFont, SetChosenFont, doc="See `GetChosenFont` and `SetChosenFont`");
    %property(Colour, GetColour, SetColour, doc="See `GetColour` and `SetColour`");
//    %property(EnableEffects, GetEnableEffects, doc="See `GetEnableEffects`");
    %property(InitialFont, GetInitialFont, SetInitialFont, doc="See `GetInitialFont` and `SetInitialFont`");
    %property(ShowHelp, GetShowHelp, SetShowHelp, doc="See `GetShowHelp` and `SetShowHelp`");
    
};




DocStr(wxFontDialog,
       "wx.FontDialog allows the user to select a system font and its attributes.

:see: `wx.FontData`
", "");

MustHaveApp(wxFontDialog);

class wxFontDialog : public wxDialog {
public:
    %pythonAppend wxFontDialog   "self._setOORInfo(self)"

    DocStr(wxFontDialog,
           "Constructor. Pass a parent window and the `wx.FontData` object to be
used to initialize the dialog controls.  Call `ShowModal` to display
the dialog.  If ShowModal returns ``wx.ID_OK`` then you can fetch the
results with via the `wx.FontData` returned by `GetFontData`.", "");
    wxFontDialog(wxWindow* parent, const wxFontData& data);


    DocDeclStr(
        wxFontData& , GetFontData(),
        "Returns a reference to the internal `wx.FontData` used by the
wx.FontDialog.", "");

    %property(FontData, GetFontData, doc="See `GetFontData`");    
};


wxFont wxGetFontFromUser(wxWindow *parent = NULL,
                         const wxFont& fontInit = wxNullFont,
                         const wxString& caption = wxPyEmptyString);


//---------------------------------------------------------------------------


DocStr(wxMessageDialog,
"This class provides a simple dialog that shows a single or multi-line
message, with a choice of OK, Yes, No and/or Cancel buttons.", "

Window Styles
--------------
    ===================    =============================================
    wx.OK                  Show an OK button.
    wx.CANCEL              Show a Cancel button.
    wx.YES_NO              Show Yes and No buttons.
    wx.YES_DEFAULT         Used with wxYES_NO, makes Yes button the
                           default - which is the default behaviour.
    wx.NO_DEFAULT          Used with wxYES_NO, makes No button the default.
    wx.ICON_EXCLAMATION    Shows an exclamation mark icon.
    wx.ICON_HAND           Shows an error icon.
    wx.ICON_ERROR          Shows an error icon - the same as wxICON_HAND.
    wx.ICON_QUESTION       Shows a question mark icon.
    wx.ICON_INFORMATION    Shows an information (i) icon.
    wx.STAY_ON_TOP         The message box stays on top of all other
                           window, even those of the other applications
                           (Windows only).
    ===================    =============================================
");


MustHaveApp(wxMessageDialog);

class wxMessageDialog : public wxDialog {
public:
    %pythonAppend wxMessageDialog   "self._setOORInfo(self)"

    DocCtorStr(
        wxMessageDialog(wxWindow* parent,
                        const wxString& message,
                        const wxString& caption = wxPyMessageBoxCaptionStr,
                        long style = wxOK | wxCANCEL | wxCENTRE,
                        const wxPoint& pos = wxDefaultPosition),
        "Constructor, use `ShowModal` to display the dialog.", "");

};

//---------------------------------------------------------------------------

enum {
    wxPD_AUTO_HIDE,
    wxPD_APP_MODAL,
    wxPD_CAN_ABORT,
    wxPD_ELAPSED_TIME,
    wxPD_ESTIMATED_TIME,
    wxPD_REMAINING_TIME,
    wxPD_SMOOTH,
    wxPD_CAN_SKIP
};


DocStr(wxProgressDialog,
"A dialog that shows a short message and a progress bar. Optionally, it
can display an ABORT button.", "

Window Styles
--------------
    ====================     =============================================
    wx.PD_APP_MODAL          Make the progress dialog modal. If this flag is
                             not given, it is only \"locally\" modal -
                             that is the input to the parent window is
                             disabled, but not to the other ones.

    wx.PD_AUTO_HIDE          Causes the progress dialog to disappear from
                             screen as soon as the maximum value of the
                             progress meter has been reached.

    wx.PD_CAN_ABORT          This flag tells the dialog that it should have
                             a \"Cancel\" button which the user may press. If
                             this happens, the next call to Update() will
                             return False in the first component of its return
                             value.

    wx.PD_CAN_SKIP           This flag tells the dialog that it should have a
                             \"Skip\" button which the user may press. If this
                             happens, the next call to Update() will return
                             True in the second component of its return value.

    wx.PD_ELAPSED_TIME       This flag tells the dialog that it should show
                             elapsed time (since creating the dialog).

    wx.PD_ESTIMATED_TIME     This flag tells the dialog that it should show
                             estimated time.

    wx.PD_REMAINING_TIME     This flag tells the dialog that it should show
                             remaining time.

    wx.PD_SMOOTH             Uses the wx.GA_SMOOTH style on the embedded
                             wx.Gauge widget.
    ====================     =============================================
");


// TODO: wxPD_CAN_SKIP

MustHaveApp(wxProgressDialog);

class wxProgressDialog : public wxFrame {
public:
    %pythonAppend wxProgressDialog   "self._setOORInfo(self)"

    DocCtorStr(
        wxProgressDialog(const wxString& title,
                         const wxString& message,
                         int maximum = 100,
                         wxWindow* parent = NULL,
                         int style = wxPD_AUTO_HIDE | wxPD_APP_MODAL ),
        "Constructor. Creates the dialog, displays it and disables user input
for other windows, or, if wx.PD_APP_MODAL flag is not given, for its
parent window only.", "");

    // TODO: support getting the skipped value back in the return value, but
    // only if style is set.  This is so the API doesn't change for existing
    // users...
    DocDeclAStr(
        virtual bool , Update(int value,
                              const wxString& newmsg = wxPyEmptyString,
                              bool *OUTPUT),
        "Update(self, int value, String newmsg) --> (continue, skip)",
        "Updates the dialog, setting the progress bar to the new value and, if
given changes the message above it. The value given should be less
than or equal to the maximum value given to the constructor and the
dialog is closed if it is equal to the maximum.  Returns a tuple of
boolean values, ``(continue, skip)`` where ``continue`` is ``True``
unless the Cancel button has been pressed, and ``skip`` is ``False``
unless the Skip button (if any) has been pressed.

If the ``continue`` return value is ``False``, the application can either
immediately destroy the dialog or ask the user for confirmation, and if the
abort is not confirmed the dialog may be resumed with `Resume` function.
", "");


    DocDeclAStr(
        virtual bool , Pulse(const wxString& newmsg = wxPyEmptyString,
                                   bool *OUTPUT),
        "Pulse(self, String newmsg) --> (continue, skip)",
        "Just like `Update` but switches the dialog to use a gauge in
interminante mode and calls `wx.Gauge.Pulse` to show the user a bit of
progress.", "");
    %pythoncode { UpdatePulse =  Pulse }

    DocDeclStr(
        void , Resume(),
        "Can be used to continue with the dialog, after the user had chosen to
abort.", "");

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


DocStr(wxFindDialogEvent,
       "Events for the FindReplaceDialog", "");

class wxFindDialogEvent : public wxCommandEvent
{
public:
    wxFindDialogEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

    DocDeclStr(
        int , GetFlags(),
        "Get the currently selected flags: this is the combination of
wx.FR_DOWN, wx.FR_WHOLEWORD and wx.FR_MATCHCASE flags.", "");

    DocDeclStr(
        const wxString& , GetFindString(),
        "Return the string to find (never empty).", "");

    DocDeclStr(
        const wxString& , GetReplaceString(),
        "Return the string to replace the search string with (only for replace
and replace all events).", "");

    DocDeclStr(
        wxFindReplaceDialog *, GetDialog(),
        "Return the pointer to the dialog which generated this event.", "");

    DocDeclStr(
        void , SetFlags(int flags),
        "", "");

    DocDeclStr(
        void , SetFindString(const wxString& str),
        "", "");

    DocDeclStr(
        void , SetReplaceString(const wxString& str),
        "", "");
    
    %property(Dialog, GetDialog, doc="See `GetDialog`");
    %property(FindString, GetFindString, SetFindString, doc="See `GetFindString` and `SetFindString`");
    %property(Flags, GetFlags, SetFlags, doc="See `GetFlags` and `SetFlags`");
    %property(ReplaceString, GetReplaceString, SetReplaceString, doc="See `GetReplaceString` and `SetReplaceString`");
};



DocStr(wxFindReplaceData,
"wx.FindReplaceData holds the data for wx.FindReplaceDialog. It is used
to initialize the dialog with the default values and will keep the
last values from the dialog when it is closed. It is also updated each
time a `wx.FindDialogEvent` is generated so instead of using the
`wx.FindDialogEvent` methods you can also directly query this object.

Note that all SetXXX() methods may only be called before showing the
dialog and calling them has no effect later.", "

Flags
-----
    ================   ===============================================
    wx.FR_DOWN         Downward search/replace selected (otherwise,
                       upwards)

    wx.FR_WHOLEWORD    Whole word search/replace selected

    wx.FR_MATCHCASE    Case sensitive search/replace selected
                       (otherwise, case insensitive)
    ================   ===============================================
");




class wxFindReplaceData : public wxObject
{
public:
    DocCtorStr(
        wxFindReplaceData(int flags=0),
        "Constuctor initializes the flags to default value (0).", "");

    ~wxFindReplaceData();


    DocDeclStr(
        const wxString& , GetFindString(),
        "Get the string to find.", "");

    DocDeclStr(
        const wxString& , GetReplaceString(),
        "Get the replacement string.", "");

    DocDeclStr(
        int , GetFlags(),
        "Get the combination of flag values.", "");

    DocDeclStr(
        void , SetFlags(int flags),
        "Set the flags to use to initialize the controls of the dialog.", "");

    DocDeclStr(
        void , SetFindString(const wxString& str),
        "Set the string to find (used as initial value by the dialog).", "");

    DocDeclStr(
        void , SetReplaceString(const wxString& str),
        "Set the replacement string (used as initial value by the dialog).", "");

    %property(FindString, GetFindString, SetFindString, doc="See `GetFindString` and `SetFindString`");
    %property(Flags, GetFlags, SetFlags, doc="See `GetFlags` and `SetFlags`");
    %property(ReplaceString, GetReplaceString, SetReplaceString, doc="See `GetReplaceString` and `SetReplaceString`");
};




DocStr(wxFindReplaceDialog,
"wx.FindReplaceDialog is a standard modeless dialog which is used to
allow the user to search for some text (and possibly replace it with
something else). The actual searching is supposed to be done in the
owner window which is the parent of this dialog. Note that it means
that unlike for the other standard dialogs this one must have a parent
window. Also note that there is no way to use this dialog in a modal
way; it is always, by design and implementation, modeless.", "


Window Styles
-------------

    =====================  =========================================
    wx.FR_REPLACEDIALOG    replace dialog (otherwise find dialog)

    wx.FR_NOUPDOWN         don't allow changing the search direction

    wx.FR_NOMATCHCASE      don't allow case sensitive searching

    wx.FR_NOWHOLEWORD      don't allow whole word searching
    =====================  =========================================
");

MustHaveApp(wxFindReplaceDialog);

class wxFindReplaceDialog : public wxDialog {
public:
    %pythonAppend wxFindReplaceDialog     "self._setOORInfo(self)"
    %pythonAppend wxFindReplaceDialog()   ""

    DocCtorStr(
        wxFindReplaceDialog(wxWindow *parent,
                            wxFindReplaceData *data,
                            const wxString &title,
                            int style = 0),
        "Create a FindReplaceDialog.  The parent and data parameters must be
non-None.  Use Show to display the dialog.", "");

    DocCtorStrName(
        wxFindReplaceDialog(),
        "Precreate a FindReplaceDialog for 2-phase creation", "",
        PreFindReplaceDialog);


    DocDeclStr(
        bool , Create(wxWindow *parent, wxFindReplaceData *data,
                      const wxString &title, int style = 0),
        "Create the dialog, for 2-phase create.", "");


    DocDeclStr(
        const wxFindReplaceData *, GetData(),
        "Get the FindReplaceData object used by this dialog.", "");

    DocDeclStr(
        void , SetData(wxFindReplaceData *data),
        "Set the FindReplaceData object used by this dialog.", "");

    %property(Data, GetData, SetData, doc="See `GetData` and `SetData`");
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
