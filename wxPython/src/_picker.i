/////////////////////////////////////////////////////////////////////////////
// Name:        _picker.i
// Purpose:     SWIG interface for Colour, Dir, File, Font picker controls
//
// Author:      Robin Dunn
//
// Created:     6-June-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

enum {
     wxPB_USE_TEXTCTRL,
 };


DocStr(wxPickerBase,
"Base abstract class for all pickers which support an auxiliary text
control. This class handles all positioning and sizing of the text
control like a an horizontal `wx.BoxSizer` would do, with the text
control on the left of the picker button and the proportion of the
picker fixed to value 1.", "");

class wxPickerBase : public wxControl
{
public:
    // This class is an ABC, can't be instantiated from Python.
    //wxPickerBase() : m_text(NULL), m_picker(NULL),
    //                 m_margin(5), m_textProportion(2) {}
    //virtual ~wxPickerBase();


    // if present, intercepts wxPB_USE_TEXTCTRL style and creates the text control
    // The 3rd argument is the initial wxString to display in the text control
    bool CreateBase(wxWindow *parent, wxWindowID id,
        const wxString& text = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxButtonNameStr);



    DocDeclStr(
        void , SetInternalMargin(int newmargin),
        "Sets the margin (in pixels) between the picker and the text control.", "");

    DocDeclStr(
        int , GetInternalMargin() const,
        "Returns the margin (in pixels) between the picker and the text
control.", "");


    DocDeclStr(
        void , SetTextCtrlProportion(int prop),
        "Sets the proportion between the text control and the picker button.
This is used to set relative sizes of the text contorl and the picker.
The value passed to this function must be >= 1.", "");

    DocDeclStr(
        int , GetTextCtrlProportion() const,
        "Returns the proportion between the text control and the picker.", "");


    DocDeclStr(
        void , SetPickerCtrlProportion(int prop),
        "Sets the proportion value of the picker.", "");
    
    DocDeclStr(
        int , GetPickerCtrlProportion() const,
        "Gets the proportion value of the picker.", "");
    
    
    DocDeclStr(
        bool , IsTextCtrlGrowable() const,
        "", "");

    DocDeclStr(
        void , SetTextCtrlGrowable(bool grow = true),
        "", "");
    

    DocDeclStr(
        bool , IsPickerCtrlGrowable() const,
        "", "");
    
    DocDeclStr(
        void , SetPickerCtrlGrowable(bool grow = true),
        "", "");
        

    DocDeclStr(
        bool , HasTextCtrl() const,
        "Returns true if this class has a valid text control (i.e. if the
wx.PB_USE_TEXTCTRL style was given when creating this control).", "");

    DocDeclStr(
        wxTextCtrl *, GetTextCtrl(),
        "Returns a pointer to the text control handled by this class or None if
the wx.PB_USE_TEXTCTRL style was not specified when this control was
created.

Very important: the contents of the text control could be containing
an invalid representation of the entity which can be chosen through
the picker (e.g. the user entered an invalid colour syntax because of
a typo). Thus you should never parse the content of the textctrl to
get the user's input; rather use the derived-class getter
(e.g. `wx.ColourPickerCtrl.GetColour`, `wx.FilePickerCtrl.GetPath`,
etc).", "");

    DocDeclStr(
        wxControl *, GetPickerCtrl(),
        "", "");


    %property(InternalMargin, GetInternalMargin, SetInternalMargin, doc="See `GetInternalMargin` and `SetInternalMargin`");
    %property(PickerCtrl, GetPickerCtrl, doc="See `GetPickerCtrl`");
    %property(PickerCtrlProportion, GetPickerCtrlProportion, SetPickerCtrlProportion, doc="See `GetPickerCtrlProportion` and `SetPickerCtrlProportion`");
    %property(TextCtrl, GetTextCtrl, doc="See `GetTextCtrl`");
    %property(TextCtrlProportion, GetTextCtrlProportion, SetTextCtrlProportion, doc="See `GetTextCtrlProportion` and `SetTextCtrlProportion`");

    %property(TextCtrlGrowable, IsTextCtrlGrowable, SetTextCtrlGrowable, doc="See `IsTextCtrlGrowable` and `SetTextCtrlGrowable`");
    %property(PickerCtrlGrowable, IsPickerCtrlGrowable, SetPickerCtrlGrowable, doc="See `IsPickerCtrlGrowable` and `SetPickerCtrlGrowable`");
    
};

//---------------------------------------------------------------------------
%newgroup

MAKE_CONST_WXSTRING(ColourPickerCtrlNameStr);

enum {
    wxCLRP_SHOW_LABEL,
    wxCLRP_USE_TEXTCTRL,
    wxCLRP_DEFAULT_STYLE,
};


MustHaveApp(wxColourPickerCtrl);
DocStr(wxColourPickerCtrl,
"This control allows the user to select a colour. The generic
implementation is a button which brings up a `wx.ColourDialog` when
clicked. Native implementations may differ but this is usually a
(small) widget which give access to the colour-chooser dialog.",

"
Window Styles
-------------

    ======================  ============================================
    wx.CLRP_DEFAULT         Default style.
    wx.CLRP_USE_TEXTCTRL    Creates a text control to the left of the
                            picker button which is completely managed
                            by the `wx.ColourPickerCtrl` and which can
                            be used by the user to specify a colour.
                            The text control is automatically synchronized
                            with the button's value. Use functions defined in
                            `wx.PickerBase` to modify the text control.
    wx.CLRP_SHOW_LABEL      Shows the colour in HTML form (AABBCC) as the
                            colour button label (instead of no label at all).
    ======================  ============================================

Events
------

    ========================  ==========================================
    EVT_COLOURPICKER_CHANGED  The user changed the colour selected in the
                              control either using the button or using the
                              text control (see wx.CLRP_USE_TEXTCTRL; note
                              that in this case the event is fired only if
                              the user's input is valid, i.e. recognizable).
    ========================  ==========================================
");

class wxColourPickerCtrl : public wxPickerBase
{
public:
    %pythonAppend wxColourPickerCtrl      "self._setOORInfo(self)"
    %pythonAppend wxColourPickerCtrl()    ""

    wxColourPickerCtrl(wxWindow *parent, wxWindowID id=-1,
                       const wxColour& col = *wxBLACK,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxCLRP_DEFAULT_STYLE,
                       const wxValidator& validator = wxDefaultValidator,
                       const wxString& name = wxPyColourPickerCtrlNameStr);
    %RenameCtor(PreColourPickerCtrl, wxColourPickerCtrl());

    bool Create(wxWindow *parent, wxWindowID id,
                const wxColour& col = *wxBLACK,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCLRP_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyColourPickerCtrlNameStr);


    DocDeclStr(
        wxColour , GetColour() const,
        "Returns the currently selected colour.", "");


    DocDeclStr(
        void , SetColour(const wxColour& col),
        "Set the displayed colour.", "");

    %property(Colour, GetColour, SetColour, doc="See `GetColour` and `SetColour`");
};



%constant wxEventType wxEVT_COMMAND_COLOURPICKER_CHANGED;
%pythoncode {
    EVT_COLOURPICKER_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_COLOURPICKER_CHANGED, 1 )
}

class wxColourPickerEvent : public wxCommandEvent
{
public:
    wxColourPickerEvent(wxObject *generator, int id, const wxColour &col);

    wxColour GetColour() const;
    void SetColour(const wxColour &c);

    %property(Colour, GetColour, SetColour, doc="See `GetColour` and `SetColour`");
};


//---------------------------------------------------------------------------
%newgroup

MAKE_CONST_WXSTRING(FilePickerCtrlNameStr);
MAKE_CONST_WXSTRING(FileSelectorPromptStr);
MAKE_CONST_WXSTRING(DirPickerCtrlNameStr);
MAKE_CONST_WXSTRING(DirSelectorPromptStr);
MAKE_CONST_WXSTRING(FileSelectorDefaultWildcardStr);


enum {
    wxFLP_OPEN,
    wxFLP_SAVE,
    wxFLP_OVERWRITE_PROMPT,
    wxFLP_FILE_MUST_EXIST,
    wxFLP_CHANGE_DIR,
    wxDIRP_DIR_MUST_EXIST,
    wxDIRP_CHANGE_DIR,

    wxFLP_USE_TEXTCTRL,
    wxFLP_DEFAULT_STYLE,
    
    wxDIRP_USE_TEXTCTRL,
    wxDIRP_DEFAULT_STYLE,
};



MustHaveApp(wxFilePickerCtrl);
DocStr(wxFilePickerCtrl,
    "", "");

class wxFilePickerCtrl : public wxPickerBase
{
public:
    %pythonAppend wxFilePickerCtrl      "self._setOORInfo(self)"
    %pythonAppend wxFilePickerCtrl()    ""

    wxFilePickerCtrl(wxWindow *parent,
                     wxWindowID id=-1,
                     const wxString& path = wxPyEmptyString,
                     const wxString& message = wxPyFileSelectorPromptStr,
                     const wxString& wildcard = wxPyFileSelectorDefaultWildcardStr,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxFLP_DEFAULT_STYLE,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxPyFilePickerCtrlNameStr);
    %RenameCtor(PreFilePickerCtrl, wxFilePickerCtrl());

    bool Create(wxWindow *parent,
                wxWindowID id=-1,
                const wxString& path = wxPyEmptyString,
                const wxString& message = wxPyFileSelectorPromptStr,
                const wxString& wildcard = wxPyFileSelectorDefaultWildcardStr,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxFLP_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyFilePickerCtrlNameStr);

    wxString GetPath() const;
    void SetPath(const wxString &str);

    // return true if the given path is valid for this control
    bool CheckPath(const wxString& path) const;

    // Returns the filtered value currently placed in the text control (if present).
    wxString GetTextCtrlValue() const;

    %property(Path, GetPath, SetPath, doc="See `GetPath` and `SetPath`");
    %property(TextCtrlValue, GetTextCtrlValue, doc="See `GetTextCtrlValue`");
};




MustHaveApp(wxDirPickerCtrl);
DocStr(wxDirPickerCtrl,
    "", "");

class wxDirPickerCtrl : public wxPickerBase
{
public:
    %pythonAppend wxDirPickerCtrl      "self._setOORInfo(self)"
    %pythonAppend wxDirPickerCtrl()    ""

    wxDirPickerCtrl(wxWindow *parent, wxWindowID id=-1,
                    const wxString& path = wxPyEmptyString,
                    const wxString& message = wxPyDirSelectorPromptStr,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDIRP_DEFAULT_STYLE,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxPyDirPickerCtrlNameStr);
    %RenameCtor(PreDirPickerCtrl, wxDirPickerCtrl());

    bool Create(wxWindow *parent, wxWindowID id=-1,
                const wxString& path = wxPyEmptyString,
                const wxString& message = wxPyDirSelectorPromptStr,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDIRP_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyDirPickerCtrlNameStr);

    wxString GetPath() const;
    void SetPath(const wxString &str);
    
    // return true if the given path is valid for this control
    bool CheckPath(const wxString& path) const;

    // Returns the filtered value currently placed in the text control (if present).
    wxString GetTextCtrlValue() const;

    %property(Path, GetPath, SetPath, doc="See `GetPath` and `SetPath`");
    %property(TextCtrlValue, GetTextCtrlValue, doc="See `GetTextCtrlValue`");
    
};


%constant wxEventType wxEVT_COMMAND_FILEPICKER_CHANGED;
%constant wxEventType wxEVT_COMMAND_DIRPICKER_CHANGED;

%pythoncode {
EVT_FILEPICKER_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_FILEPICKER_CHANGED, 1 )
EVT_DIRPICKER_CHANGED  = wx.PyEventBinder( wxEVT_COMMAND_DIRPICKER_CHANGED,  1 )
}

class wxFileDirPickerEvent : public wxCommandEvent
{
public:
    wxFileDirPickerEvent(wxEventType type, wxObject *generator, int id, const wxString &path);

    wxString GetPath() const { return m_path; }
    void SetPath(const wxString &p) { m_path = p; }

    %property(Path, GetPath, SetPath, doc="See `GetPath` and `SetPath`");
};


//---------------------------------------------------------------------------
%newgroup

MAKE_CONST_WXSTRING(FontPickerCtrlNameStr);

enum {
    wxFNTP_FONTDESC_AS_LABEL,
    wxFNTP_USEFONT_FOR_LABEL,
    wxFNTP_USE_TEXTCTRL,
    wxFNTP_DEFAULT_STYLE,
};


MustHaveApp(wxFontPickerCtrl);
DocStr(wxFontPickerCtrl,
    "", "");


class wxFontPickerCtrl : public wxPickerBase
{
public:
    %pythonAppend wxFontPickerCtrl      "self._setOORInfo(self)"
    %pythonAppend wxFontPickerCtrl()    ""


    wxFontPickerCtrl(wxWindow *parent,
                     wxWindowID id=-1,
                     const wxFont& initial = wxNullFont,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxFNTP_DEFAULT_STYLE,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxPyFontPickerCtrlNameStr);
    %RenameCtor(PreFontPickerCtrl, wxFontPickerCtrl());

    bool Create(wxWindow *parent,
                wxWindowID id=-1,
                const wxFont& initial = wxNullFont,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxFNTP_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyFontPickerCtrlNameStr);


    // get the font chosen
    wxFont GetSelectedFont() const;

    // sets currently displayed font
    void SetSelectedFont(const wxFont& f);

    // set/get the max pointsize
    void SetMaxPointSize(unsigned int max);
    unsigned int GetMaxPointSize() const;
    
    %property(MaxPointSize, GetMaxPointSize, SetMaxPointSize, doc="See `GetMaxPointSize` and `SetMaxPointSize`");
    %property(SelectedFont, GetSelectedFont, SetSelectedFont, doc="See `GetSelectedFont` and `SetSelectedFont`");
};

 
%constant wxEventType wxEVT_COMMAND_FONTPICKER_CHANGED;

%pythoncode {
EVT_FONTPICKER_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_FONTPICKER_CHANGED, 1 )
}


class wxFontPickerEvent : public wxCommandEvent
{
public:
    wxFontPickerEvent(wxObject *generator, int id, const wxFont &f);

    wxFont GetFont() const;
    void SetFont(const wxFont &c);
    
    %property(Font, GetFont, SetFont, doc="See `GetFont` and `SetFont`");
};

//---------------------------------------------------------------------------
