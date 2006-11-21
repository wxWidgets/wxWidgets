/////////////////////////////////////////////////////////////////////////////
// Name:        _textctrl.i
// Purpose:     SWIG interface defs for wxTextCtrl and related classes
//
// Author:      Robin Dunn
//
// Created:     10-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(TextCtrlNameStr);

//---------------------------------------------------------------------------
%newgroup

enum {
    // Style flags
    wxTE_NO_VSCROLL,
    wxTE_AUTO_SCROLL,

    wxTE_READONLY,
    wxTE_MULTILINE,
    wxTE_PROCESS_TAB,

    // alignment flags
    wxTE_LEFT,
    wxTE_CENTER,
    wxTE_RIGHT,
    wxTE_CENTRE,

    // this style means to use RICHEDIT control and does something only under
    // wxMSW and Win32 and is silently ignored under all other platforms
    wxTE_RICH,

    wxTE_PROCESS_ENTER,
    wxTE_PASSWORD,

    // automatically detect the URLs and generate the events when mouse is
    // moved/clicked over an URL
    //
    // this is for Win32 richedit controls only so far
    wxTE_AUTO_URL,

    // by default, the Windows text control doesn't show the selection when it
    // doesn't have focus - use this style to force it to always show it
    wxTE_NOHIDESEL,

    // use wxHSCROLL (wxTE_DONTWRAP) to not wrap text at all, wxTE_CHARWRAP to
    // wrap it at any position and wxTE_WORDWRAP to wrap at words boundary
    //
    // if no wrapping style is given at all, the control wraps at word boundary
    wxTE_DONTWRAP,
    wxTE_CHARWRAP,
    wxTE_WORDWRAP,
    wxTE_BESTWRAP,

    // force using RichEdit version 2.0 or 3.0 instead of 1.0 (default) for
    // wxTE_RICH controls - can be used together with or instead of wxTE_RICH
    wxTE_RICH2,

    wxTE_CAPITALIZE,
};

%pythoncode { TE_LINEWRAP = TE_CHARWRAP }



enum wxTextAttrAlignment
{
    wxTEXT_ALIGNMENT_DEFAULT,
    wxTEXT_ALIGNMENT_LEFT,
    wxTEXT_ALIGNMENT_CENTRE,
    wxTEXT_ALIGNMENT_CENTER = wxTEXT_ALIGNMENT_CENTRE,
    wxTEXT_ALIGNMENT_RIGHT,
    wxTEXT_ALIGNMENT_JUSTIFIED
};

enum {
    // Flags to indicate which attributes are being applied
    wxTEXT_ATTR_TEXT_COLOUR,
    wxTEXT_ATTR_BACKGROUND_COLOUR,
    wxTEXT_ATTR_FONT_FACE,
    wxTEXT_ATTR_FONT_SIZE,
    wxTEXT_ATTR_FONT_WEIGHT,
    wxTEXT_ATTR_FONT_ITALIC,
    wxTEXT_ATTR_FONT_UNDERLINE,
    wxTEXT_ATTR_FONT,
    wxTEXT_ATTR_ALIGNMENT,
    wxTEXT_ATTR_LEFT_INDENT,
    wxTEXT_ATTR_RIGHT_INDENT,
    wxTEXT_ATTR_TABS
};

enum wxTextCtrlHitTestResult
{
    wxTE_HT_UNKNOWN = -2,   // this means HitTest() is simply not implemented
    wxTE_HT_BEFORE,         // either to the left or upper
    wxTE_HT_ON_TEXT,        // directly on
    wxTE_HT_BELOW,          // below [the last line]
    wxTE_HT_BEYOND          // after [the end of line]
};


enum {
    wxOutOfRangeTextCoord,
    wxInvalidTextCoord,

    wxTEXT_TYPE_ANY
};

//---------------------------------------------------------------------------

// wxTextAttr: a structure containing the visual attributes of a text
class wxTextAttr
{
public:    
    wxTextAttr(const wxColour& colText = wxNullColour,
               const wxColour& colBack = wxNullColour,
               const wxFont& font = wxNullFont,
               wxTextAttrAlignment alignment = wxTEXT_ALIGNMENT_DEFAULT);
    ~wxTextAttr();
    
    // operations
    void Init();

    // merges the attributes of the base and the overlay objects and returns
    // the result; the parameter attributes take precedence
    //
    // WARNING: the order of arguments is the opposite of Combine()
    static wxTextAttr Merge(const wxTextAttr& base, const wxTextAttr& overlay);
    
//     // merges the attributes of this object and overlay
//     void Merge(const wxTextAttr& overlay);

    
    // setters
    void SetTextColour(const wxColour& colText);
    void SetBackgroundColour(const wxColour& colBack);
    void SetFont(const wxFont& font, long flags = wxTEXT_ATTR_FONT);
    void SetAlignment(wxTextAttrAlignment alignment);
    void SetTabs(const wxArrayInt& tabs);
    void SetLeftIndent(int indent, int subIndent=0);
    void SetRightIndent(int indent);
    void SetFlags(long flags);

    // accessors
    bool HasTextColour() const;
    bool HasBackgroundColour() const;
    bool HasFont() const;
    bool HasAlignment() const;
    bool HasTabs() const;
    bool HasLeftIndent() const;
    bool HasRightIndent() const;
    bool HasFlag(long flag) const;

    const wxColour& GetTextColour() const;
    const wxColour& GetBackgroundColour() const;
    const wxFont& GetFont() const;
    wxTextAttrAlignment GetAlignment() const;
    const wxArrayInt& GetTabs() const;
    long GetLeftIndent() const;
    long GetLeftSubIndent() const;
    long GetRightIndent() const;
    long GetFlags() const;

    // returns False if we have any attributes set, True otherwise
    bool IsDefault() const;

    // return the attribute having the valid font and colours: it uses the
    // attributes set in attr and falls back first to attrDefault and then to
    // the text control font/colours for those attributes which are not set
    static wxTextAttr Combine(const wxTextAttr& attr,
                              const wxTextAttr& attrDef,
                              const wxTextCtrl *text);

    %property(Alignment, GetAlignment, SetAlignment, doc="See `GetAlignment` and `SetAlignment`");
    %property(BackgroundColour, GetBackgroundColour, SetBackgroundColour, doc="See `GetBackgroundColour` and `SetBackgroundColour`");
    %property(Flags, GetFlags, SetFlags, doc="See `GetFlags` and `SetFlags`");
    %property(Font, GetFont, SetFont, doc="See `GetFont` and `SetFont`");
    %property(LeftIndent, GetLeftIndent, SetLeftIndent, doc="See `GetLeftIndent` and `SetLeftIndent`");
    %property(LeftSubIndent, GetLeftSubIndent, doc="See `GetLeftSubIndent`");
    %property(RightIndent, GetRightIndent, SetRightIndent, doc="See `GetRightIndent` and `SetRightIndent`");
    %property(Tabs, GetTabs, SetTabs, doc="See `GetTabs` and `SetTabs`");
    %property(TextColour, GetTextColour, SetTextColour, doc="See `GetTextColour` and `SetTextColour`");
};

//---------------------------------------------------------------------------

// wxTextCtrl: a single or multiple line text zone where user can enter and
// edit text
MustHaveApp(wxTextCtrl);
class wxTextCtrl : public wxControl
{
public:
    %pythonAppend wxTextCtrl         "self._setOORInfo(self)"
    %pythonAppend wxTextCtrl()       ""
    %typemap(out) wxTextCtrl*;    // turn off this typemap

    wxTextCtrl(wxWindow* parent, wxWindowID id=-1,
               const wxString& value = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyTextCtrlNameStr);
    %RenameCtor(PreTextCtrl, wxTextCtrl());

    // Turn it back on again
    %typemap(out) wxTextCtrl* { $result = wxPyMake_wxObject($1, $owner); }

    bool Create(wxWindow* parent, wxWindowID id=-1,
               const wxString& value = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyTextCtrlNameStr);

    
    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value);

    virtual bool IsEmpty() const;

    virtual void ChangeValue(const wxString &value);
    
    virtual wxString GetRange(long from, long to) const;

    virtual int GetLineLength(long lineNo) const;
    virtual wxString GetLineText(long lineNo) const;
    virtual int GetNumberOfLines() const;

    virtual bool IsModified() const;
    virtual bool IsEditable() const;

    // more readable flag testing methods
    bool IsSingleLine() const;
    bool IsMultiLine() const;


    DocDeclAStr(
        virtual void, GetSelection(long* OUTPUT, long* OUTPUT) const,
        "GetSelection() -> (from, to)",
        "If the return values from and to are the same, there is no selection.", "");

    virtual wxString GetStringSelection() const;


    // editing
    virtual void Clear();
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);

    // load/save the controls contents from/to the file
    virtual bool LoadFile(const wxString& file, int fileType = wxTEXT_TYPE_ANY);
    virtual bool SaveFile(const wxString& file = wxPyEmptyString, int fileType = wxTEXT_TYPE_ANY);

    // sets/clears the dirty flag
    virtual void MarkDirty();
    virtual void DiscardEdits();
    void SetModified(bool modified);
    
    // set the max number of characters which may be entered in a single line
    // text control
    virtual void SetMaxLength(unsigned long len);

    // writing text inserts it at the current position, appending always
    // inserts it at the end
    virtual void WriteText(const wxString& text);
    virtual void AppendText(const wxString& text);

    // insert the character which would have resulted from this key event,
    // return True if anything has been inserted
    virtual bool EmulateKeyPress(const wxKeyEvent& event);

    // text control under some platforms supports the text styles: these
    // methods allow to apply the given text style to the given selection or to
    // set/get the style which will be used for all appended text
    virtual bool SetStyle(long start, long end, const wxTextAttr& style);
    virtual bool GetStyle(long position, wxTextAttr& style);
    virtual bool SetDefaultStyle(const wxTextAttr& style);
    virtual const wxTextAttr& GetDefaultStyle() const;

    // translate between the position (which is just an index in the text ctrl
    // considering all its contents as a single strings) and (x, y) coordinates
    // which represent column and line.
    virtual long XYToPosition(long x, long y) const;
    DocDeclA(
        virtual /*bool*/ void, PositionToXY(long pos, long *OUTPUT, long *OUTPUT) const,
        "PositionToXY(long pos) -> (x, y)");

    virtual void ShowPosition(long pos);


    DocDeclAStr(
        virtual wxTextCtrlHitTestResult, HitTest(const wxPoint& pt,
                                                 long* OUTPUT, long* OUTPUT) const,
        "HitTest(Point pt) -> (result, col, row)",
        "Find the row, col coresponding to the character at the point given in
pixels. NB: pt is in device coords but is not adjusted for the client
area origin nor scrolling.", "");


    DocDeclAStrName(
        virtual wxTextCtrlHitTestResult , HitTest(const wxPoint& pt, long *OUTPUT) const,
        "HitTestPos(Point pt) -> (result, position)",
        "Find the character position in the text coresponding to the point
given in pixels. NB: pt is in device coords but is not adjusted for
the client area origin nor scrolling. ", "",
        HitTestPos);

    
    
    // Clipboard operations
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();

    virtual bool CanCopy() const;
    virtual bool CanCut() const;
    virtual bool CanPaste() const;

    // Undo/redo
    virtual void Undo();
    virtual void Redo();

    virtual bool CanUndo() const;
    virtual bool CanRedo() const;

    // Insertion point
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual long GetLastPosition() const;

    virtual void SetSelection(long from, long to);
    virtual void SelectAll();
    virtual void SetEditable(bool editable);
    
#ifdef __WXMAC__
    virtual void MacCheckSpelling(bool check);
#endif

    // generate the wxEVT_COMMAND_TEXT_UPDATED event, like SetValue() does
    void SendTextUpdatedEvent();

#ifdef __WXMSW__
    // Caret handling (Windows only)
    bool ShowNativeCaret(bool show = true);
    bool HideNativeCaret();
#endif

    %extend {
        // TODO: Add more file-like methods
        void write(const wxString& text) {
            self->AppendText(text);
        }
    }

    // TODO: replace this when the method is really added to wxTextCtrl
    %extend {
        wxString GetString(long from, long to) {
            return self->GetValue().Mid(from, to - from);
        }
    }

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    %property(DefaultStyle, GetDefaultStyle, SetDefaultStyle, doc="See `GetDefaultStyle` and `SetDefaultStyle`");
    %property(InsertionPoint, GetInsertionPoint, SetInsertionPoint, doc="See `GetInsertionPoint` and `SetInsertionPoint`");
    %property(LastPosition, GetLastPosition, doc="See `GetLastPosition`");
    %property(NumberOfLines, GetNumberOfLines, doc="See `GetNumberOfLines`");
    %property(Selection, GetSelection, SetSelection, doc="See `GetSelection` and `SetSelection`");
    %property(StringSelection, GetStringSelection, doc="See `GetStringSelection`");
    %property(Value, GetValue, SetValue, doc="See `GetValue` and `SetValue`");
};

//---------------------------------------------------------------------------


%constant wxEventType wxEVT_COMMAND_TEXT_UPDATED;
%constant wxEventType wxEVT_COMMAND_TEXT_ENTER;
%constant wxEventType wxEVT_COMMAND_TEXT_URL;
%constant wxEventType wxEVT_COMMAND_TEXT_MAXLEN;


class wxTextUrlEvent : public wxCommandEvent
{
public:
    wxTextUrlEvent(int winid, const wxMouseEvent& evtMouse,
                   long start, long end);

    // get the mouse event which happend over the URL
    const wxMouseEvent& GetMouseEvent();

    // get the start of the URL
    long GetURLStart() const;

    // get the end of the URL
    long GetURLEnd() const;
    
    %property(MouseEvent, GetMouseEvent, doc="See `GetMouseEvent`");
    %property(URLEnd, GetURLEnd, doc="See `GetURLEnd`");
    %property(URLStart, GetURLStart, doc="See `GetURLStart`");
};


%pythoncode {
EVT_TEXT        = wx.PyEventBinder( wxEVT_COMMAND_TEXT_UPDATED, 1)
EVT_TEXT_ENTER  = wx.PyEventBinder( wxEVT_COMMAND_TEXT_ENTER, 1)
EVT_TEXT_URL    = wx.PyEventBinder( wxEVT_COMMAND_TEXT_URL, 1) 
EVT_TEXT_MAXLEN = wx.PyEventBinder( wxEVT_COMMAND_TEXT_MAXLEN, 1)
}


    

//---------------------------------------------------------------------------

