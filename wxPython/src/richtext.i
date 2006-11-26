/////////////////////////////////////////////////////////////////////////////
// Name:        richtext.i
// Purpose:     Classes for wxRichTExtCtrl and support classes
//
// Author:      Robin Dunn
//
// Created:     11-April-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
""
%enddef

%module(package="wx", docstring=DOCSTRING) richtext

%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include "wx/wxPython/printfw.h"
#include "wx/wxPython/twoitem.h"

#include <wx/richtext/richtextctrl.h>


class wxEffects;
class wxBufferedDC;
class wxBufferedPaintDC;
class wxMetaFile;
class wxMetaFileDC;
class wxPrinterDC;

%}

#define USE_TEXTATTREX 0

//----------------------------------------------------------------------

%import windows.i
// ??  %import gdi.i

%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }

MAKE_CONST_WXSTRING_NOSWIG(EmptyString);

//----------------------------------------------------------------------

enum {
    wxRE_READONLY,
    wxRE_MULTILINE,

    wxRICHTEXT_SHIFT_DOWN,
    wxRICHTEXT_CTRL_DOWN,
    wxRICHTEXT_ALT_DOWN,

    wxRICHTEXT_SELECTED,
    wxRICHTEXT_TAGGED,
    wxRICHTEXT_FOCUSSED,
    wxRICHTEXT_IS_FOCUS,

    wxRICHTEXT_TYPE_ANY,
    wxRICHTEXT_TYPE_TEXT,
    wxRICHTEXT_TYPE_XML,
    wxRICHTEXT_TYPE_HTML,
    wxRICHTEXT_TYPE_RTF,
    wxRICHTEXT_TYPE_PDF,

    wxRICHTEXT_FIXED_WIDTH,
    wxRICHTEXT_FIXED_HEIGHT,
    wxRICHTEXT_VARIABLE_WIDTH,
    wxRICHTEXT_VARIABLE_HEIGHT,


    wxRICHTEXT_HITTEST_NONE,
    wxRICHTEXT_HITTEST_BEFORE,
    wxRICHTEXT_HITTEST_AFTER,
    wxRICHTEXT_HITTEST_ON,

    wxRICHTEXT_FORMATTED,
    wxRICHTEXT_UNFORMATTED,

    wxRICHTEXT_SETSTYLE_NONE,
    wxRICHTEXT_SETSTYLE_WITH_UNDO,
    wxRICHTEXT_SETSTYLE_OPTIMIZE,
    wxRICHTEXT_SETSTYLE_PARAGRAPHS_ONLY,
    wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY,

    wxRICHTEXT_INSERT_NONE,
    wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE,



    // TODO:  Rename these to be wxRICHTEXT_* ??

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
    wxTEXT_ATTR_TABS,

    wxTEXT_ATTR_PARA_SPACING_AFTER,
    wxTEXT_ATTR_PARA_SPACING_BEFORE,
    wxTEXT_ATTR_LINE_SPACING,
    wxTEXT_ATTR_CHARACTER_STYLE_NAME,
    wxTEXT_ATTR_PARAGRAPH_STYLE_NAME,
    wxTEXT_ATTR_BULLET_STYLE,
    wxTEXT_ATTR_BULLET_NUMBER,

    wxTEXT_ATTR_BULLET_STYLE_NONE,
    wxTEXT_ATTR_BULLET_STYLE_ARABIC,
    wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER,
    wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER,
    wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER,
    wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER,
    wxTEXT_ATTR_BULLET_STYLE_SYMBOL,
    wxTEXT_ATTR_BULLET_STYLE_BITMAP,
    wxTEXT_ATTR_BULLET_STYLE_PARENTHESES,
    wxTEXT_ATTR_BULLET_STYLE_PERIOD,

    wxTEXT_ATTR_LINE_SPACING_NORMAL,
    wxTEXT_ATTR_LINE_SPACING_HALF,
    wxTEXT_ATTR_LINE_SPACING_TWICE,

};

enum wxTextAttrAlignment
{
    wxTEXT_ALIGNMENT_DEFAULT,
    wxTEXT_ALIGNMENT_LEFT,
    wxTEXT_ALIGNMENT_CENTRE,
    wxTEXT_ALIGNMENT_CENTER = wxTEXT_ALIGNMENT_CENTRE,
    wxTEXT_ALIGNMENT_RIGHT,
    wxTEXT_ALIGNMENT_JUSTIFIED
};

//----------------------------------------------------------------------

%typemap(in) wxRichTextRange& (wxRichTextRange temp) {
    $1 = &temp;
    if ( ! wxRichTextRange_helper($input, &$1)) SWIG_fail;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) wxRichTextRange& {
    $1 = wxPySimple_typecheck($input, wxT("wxRichTextRange"), 2);
}


%{

bool wxRichTextRange_helper(PyObject* source, wxRichTextRange** obj)
{
    if (source == Py_None) {
        **obj = wxRICHTEXT_NONE;
        return true;
    }
    return wxPyTwoIntItem_helper(source, obj, wxT("wxRichTextRange"));
}
%}



DocStr(wxRichTextRange,
"RichTextRange is a data structure that represents a range of text
within a `RichTextCtrl`.  It simply contains integer ``start`` and
``end`` properties and a few operations useful for dealing with
ranges.  In most places in wxPython where a RichTextRange is expected a
2-tuple containing (start, end) can be used instead.", "");

class wxRichTextRange
{
public:
    DocCtorStr(
        wxRichTextRange(long start=0, long end=0),
        "Creates a new range object.", "");

    ~wxRichTextRange();

    %extend {
        KeepGIL(__eq__);
        DocStr(__eq__, "Test for equality of RichTextRange objects.", "");
        bool __eq__(PyObject* other) {
            wxRichTextRange  temp, *obj = &temp;
            if ( other == Py_None ) return false;
            if ( ! wxRichTextRange_helper(other, &obj) ) {
                PyErr_Clear();
                return false;
            }
            return self->operator==(*obj);
        }
    }


    DocDeclStr(
        wxRichTextRange , operator -(const wxRichTextRange& range) const,
        "", "");

    DocDeclStr(
        wxRichTextRange , operator +(const wxRichTextRange& range) const,
        "", "");


    DocDeclStr(
        void , SetRange(long start, long end),
        "", "");


    DocDeclStr(
        void , SetStart(long start),
        "", "");

    DocDeclStr(
        long , GetStart() const,
        "", "");

    %pythoncode { start = property(GetStart, SetStart) }

    DocDeclStr(
        void , SetEnd(long end),
        "", "");

    DocDeclStr(
        long , GetEnd() const,
        "", "");

    %pythoncode { end = property(GetEnd, SetEnd) }


    DocDeclStr(
        bool , IsOutside(const wxRichTextRange& range) const,
        "Returns true if this range is completely outside 'range'", "");


    DocDeclStr(
        bool , IsWithin(const wxRichTextRange& range) const,
        "Returns true if this range is completely within 'range'", "");


    DocDeclStr(
        bool , Contains(long pos) const,
        "Returns true if the given position is within this range. Allow for the
possibility of an empty range - assume the position is within this
empty range.", "");


    DocDeclStr(
        bool , LimitTo(const wxRichTextRange& range) ,
        "Limit this range to be within 'range'", "");


    DocDeclStr(
        long , GetLength() const,
        "Gets the length of the range", "");


    DocDeclStr(
        void , Swap(),
        "Swaps the start and end", "");


    %extend {
        DocAStr(Get,
               "Get() -> (start,end)",
               "Returns the start and end properties as a tuple.", "");
        PyObject* Get() {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->GetStart()));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->GetEnd()));
            wxPyEndBlockThreads(blocked);
            return tup;
        }
    }
    %pythoncode {
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'RichTextRange'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.start = val
        elif index == 1: self.end = val
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0,0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (RichTextRange, self.Get())
    }

    %property(End, GetEnd, SetEnd, doc="See `GetEnd` and `SetEnd`");
    %property(Length, GetLength, doc="See `GetLength`");
    %property(Start, GetStart, SetStart, doc="See `GetStart` and `SetStart`");
};



%{
    wxRichTextRange wxPy_RTR_ALL(wxRICHTEXT_ALL);
    wxRichTextRange wxPy_RTR_NONE(wxRICHTEXT_NONE);
%}

%rename(RICHTEXT_ALL)   wxPy_RTR_ALL;
%rename(RICHTEXT_NONE)  wxPy_RTR_NONE;

%immutable;
wxRichTextRange wxPy_RTR_ALL;
wxRichTextRange wxPy_RTR_NONE;
%mutable;

//----------------------------------------------------------------------

DocStr(wxRichTextAttr,
"The RichTextAttr class stored information about the various attributes
for a block of text, including font, colour, indents, alignments, and
etc.", "");

class wxRichTextAttr
{
public:

    wxRichTextAttr(const wxColour& colText = wxNullColour,
                   const wxColour& colBack = wxNullColour,
                   wxTextAttrAlignment alignment = wxTEXT_ALIGNMENT_DEFAULT);

    ~wxRichTextAttr();

//     // Making a wxTextAttrEx object.
//     operator wxTextAttrEx () const ;

//     // Copy to a wxTextAttr
//     void CopyTo(wxTextAttrEx& attr) const;



    // Create font from font attributes.
    DocDeclStr(
        wxFont , CreateFont() const,
        "", "");


    // Get attributes from font.
    bool GetFontAttributes(const wxFont& font);

    %pythoncode {
        def GetFont(self):
            return self.CreateFont()
        def SetFont(self, font):
            return self.GetFontAttributes(font)
    }

    // setters
    void SetTextColour(const wxColour& colText);
    void SetBackgroundColour(const wxColour& colBack);
    void SetAlignment(wxTextAttrAlignment alignment);
    void SetTabs(const wxArrayInt& tabs);
    void SetLeftIndent(int indent, int subIndent = 0);
    void SetRightIndent(int indent);

    void SetFontSize(int pointSize);
    void SetFontStyle(int fontStyle);
    void SetFontWeight(int fontWeight);
    void SetFontFaceName(const wxString& faceName);
    void SetFontUnderlined(bool underlined);

    void SetFlags(long flags);

    void SetCharacterStyleName(const wxString& name);
    void SetParagraphStyleName(const wxString& name);
    void SetParagraphSpacingAfter(int spacing);
    void SetParagraphSpacingBefore(int spacing);
    void SetLineSpacing(int spacing);
    void SetBulletStyle(int style);
    void SetBulletNumber(int n);
    void SetBulletText(wxChar symbol);
    void SetBulletFont(const wxString& bulletFont);
    
    const wxColour& GetTextColour() const;
    const wxColour& GetBackgroundColour() const;
    wxTextAttrAlignment GetAlignment() const;
    const wxArrayInt& GetTabs() const;
    long GetLeftIndent() const;
    long GetLeftSubIndent() const;
    long GetRightIndent() const;
    long GetFlags() const;

    int GetFontSize() const;
    int GetFontStyle() const;
    int GetFontWeight() const;
    bool GetFontUnderlined() const;
    const wxString& GetFontFaceName() const;

    const wxString& GetCharacterStyleName() const;
    const wxString& GetParagraphStyleName() const;
    int GetParagraphSpacingAfter() const;
    int GetParagraphSpacingBefore() const;
    int GetLineSpacing() const;
    int GetBulletStyle() const;
    int GetBulletNumber() const;
    const wxString& GetBulletText() const;
    const wxString& GetBulletFont() const;

    // accessors
    bool HasTextColour() const;
    bool HasBackgroundColour() const;
    bool HasAlignment() const;
    bool HasTabs() const;
    bool HasLeftIndent() const;
    bool HasRightIndent() const;
//     bool HasWeight() const;
//     bool HasSize() const;
//     bool HasItalic() const;
//     bool HasUnderlined() const;
//     bool HasFaceName() const;
    bool HasFont() const;

    bool HasParagraphSpacingAfter() const;
    bool HasParagraphSpacingBefore() const;
    bool HasLineSpacing() const;
    bool HasCharacterStyleName() const;
    bool HasParagraphStyleName() const;
    bool HasBulletStyle() const;
    bool HasBulletNumber() const;
    bool HasBulletText() const;

    bool HasFlag(long flag) const;

    // Is this a character style?
    bool IsCharacterStyle() const;
    bool IsParagraphStyle() const;

    // returns false if we have any attributes set, true otherwise
    bool IsDefault() const;


//     // return the attribute having the valid font and colours: it uses the
//     // attributes set in attr and falls back first to attrDefault and then to
//     // the text control font/colours for those attributes which are not set
//     static wxRichTextAttr Combine(const wxRichTextAttr& attr,
//                               const wxRichTextAttr& attrDef,
//                               const wxTextCtrlBase *text);


    %property(Alignment, GetAlignment, SetAlignment, doc="See `GetAlignment` and `SetAlignment`");
    %property(BackgroundColour, GetBackgroundColour, SetBackgroundColour, doc="See `GetBackgroundColour` and `SetBackgroundColour`");
    %property(BulletFont, GetBulletFont, SetBulletFont, doc="See `GetBulletFont` and `SetBulletFont`");
    %property(BulletNumber, GetBulletNumber, SetBulletNumber, doc="See `GetBulletNumber` and `SetBulletNumber`");
    %property(BulletStyle, GetBulletStyle, SetBulletStyle, doc="See `GetBulletStyle` and `SetBulletStyle`");
    %property(BulletText, GetBulletText, SetBulletText, doc="See `GetBulletText` and `SetBulletText`");
    %property(CharacterStyleName, GetCharacterStyleName, SetCharacterStyleName, doc="See `GetCharacterStyleName` and `SetCharacterStyleName`");
    %property(Flags, GetFlags, SetFlags, doc="See `GetFlags` and `SetFlags`");
    %property(Font, GetFont, SetFont, doc="See `GetFont` and `SetFont`");
    %property(FontAttributes, GetFontAttributes, doc="See `GetFontAttributes`");
    %property(FontFaceName, GetFontFaceName, SetFontFaceName, doc="See `GetFontFaceName` and `SetFontFaceName`");
    %property(FontSize, GetFontSize, SetFontSize, doc="See `GetFontSize` and `SetFontSize`");
    %property(FontStyle, GetFontStyle, SetFontStyle, doc="See `GetFontStyle` and `SetFontStyle`");
    %property(FontUnderlined, GetFontUnderlined, SetFontUnderlined, doc="See `GetFontUnderlined` and `SetFontUnderlined`");
    %property(FontWeight, GetFontWeight, SetFontWeight, doc="See `GetFontWeight` and `SetFontWeight`");
    %property(LeftIndent, GetLeftIndent, SetLeftIndent, doc="See `GetLeftIndent` and `SetLeftIndent`");
    %property(LeftSubIndent, GetLeftSubIndent, doc="See `GetLeftSubIndent`");
    %property(LineSpacing, GetLineSpacing, SetLineSpacing, doc="See `GetLineSpacing` and `SetLineSpacing`");
    %property(ParagraphSpacingAfter, GetParagraphSpacingAfter, SetParagraphSpacingAfter, doc="See `GetParagraphSpacingAfter` and `SetParagraphSpacingAfter`");
    %property(ParagraphSpacingBefore, GetParagraphSpacingBefore, SetParagraphSpacingBefore, doc="See `GetParagraphSpacingBefore` and `SetParagraphSpacingBefore`");
    %property(ParagraphStyleName, GetParagraphStyleName, SetParagraphStyleName, doc="See `GetParagraphStyleName` and `SetParagraphStyleName`");
    %property(RightIndent, GetRightIndent, SetRightIndent, doc="See `GetRightIndent` and `SetRightIndent`");
    %property(Tabs, GetTabs, SetTabs, doc="See `GetTabs` and `SetTabs`");
    %property(TextColour, GetTextColour, SetTextColour, doc="See `GetTextColour` and `SetTextColour`");
};


enum {
    wxTEXT_ATTR_CHARACTER,
    wxTEXT_ATTR_PARAGRAPH,
    wxTEXT_ATTR_ALL
};


//----------------------------------------------------------------------
//----------------------------------------------------------------------

MustHaveApp(wxRichTextCtrl);
DocStr(wxRichTextCtrl,
"", "");

class wxRichTextCtrl : public wxScrolledWindow
{
public:
    %pythonAppend wxRichTextCtrl         "self._setOORInfo(self)"
    %pythonAppend wxRichTextCtrl()       ""

    wxRichTextCtrl( wxWindow* parent,
                    wxWindowID id = -1,
                    const wxString& value = wxPyEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxRE_MULTILINE );
    %RenameCtor(PreRichTextCtrl, wxRichTextCtrl());


    bool Create( wxWindow* parent,
                 wxWindowID id = -1,
                 const wxString& value = wxPyEmptyString,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxRE_MULTILINE );


    DocDeclStr(
        virtual wxString , GetValue() const,
        "", "");

    DocDeclStr(
        virtual void , SetValue(const wxString& value),
        "", "");


    DocDeclStr(
        virtual wxString , GetRange(long from, long to) const,
        "", "");


    DocDeclStr(
        virtual int , GetLineLength(long lineNo) const ,
        "", "");

    DocDeclStr(
        virtual wxString , GetLineText(long lineNo) const ,
        "", "");

    DocDeclStr(
        virtual int , GetNumberOfLines() const ,
        "", "");


    DocDeclStr(
        virtual bool , IsModified() const ,
        "", "");

    DocDeclStr(
        virtual bool , IsEditable() const ,
        "", "");


    // more readable flag testing methods
    DocDeclStr(
        bool , IsSingleLine() const,
        "", "");

    DocDeclStr(
        bool , IsMultiLine() const,
        "", "");


    DocDeclAStr(
        virtual void , GetSelection(long* OUTPUT, long* OUTPUT) const,
        "GetSelection() --> (start, end)",
        "Returns the start and end positions of the current selection.  If the
values are the same then there is no selection.", "");


    DocDeclStr(
        virtual wxString , GetStringSelection() const,
        "", "");


    DocDeclStr(
        wxString , GetFilename() const,
        "", "");


    DocDeclStr(
        void , SetFilename(const wxString& filename),
        "", "");


    DocDeclStr(
        void , SetDelayedLayoutThreshold(long threshold),
        "Set the threshold in character positions for doing layout optimization
during sizing.", "");


    DocDeclStr(
        long , GetDelayedLayoutThreshold() const,
        "Get the threshold in character positions for doing layout optimization
during sizing.", "");



    DocDeclStr(
        virtual void , Clear(),
        "", "");

    DocDeclStr(
        virtual void , Replace(long from, long to, const wxString& value),
        "", "");

    DocDeclStr(
        virtual void , Remove(long from, long to),
        "", "");


    DocDeclStr(
        virtual bool , LoadFile(const wxString& file, int type = wxRICHTEXT_TYPE_ANY),
        "Load the contents of the document from the given filename.", "");

    DocDeclStr(
        virtual bool , SaveFile(const wxString& file = wxPyEmptyString,
                                int type = wxRICHTEXT_TYPE_ANY),
        "Save the contents of the document to the given filename, or if the
empty string is passed then to the filename set with `SetFilename`.", "");


    DocDeclStr(
        void , SetHandlerFlags(int flags), 
        "Set the handler flags, controlling loading and saving.", "");

    DocDeclStr(
        int , GetHandlerFlags() const, 
        "Get the handler flags, controlling loading and saving.", "");

    // sets/clears the dirty flag
    DocDeclStr(
        virtual void , MarkDirty(),
        "Sets the dirty flag, meaning that the contents of the control have
changed and need to be saved.", "");

    DocDeclStr(
        virtual void , DiscardEdits(),
        "Clears the dirty flag.
:see: `MarkDirty`", "");


    DocDeclStr(
        virtual void , SetMaxLength(unsigned long len),
        "Set the max number of characters which may be entered in a single line
text control.", "");


    DocDeclStr(
        virtual void , WriteText(const wxString& text),
        "Insert text at the current position.", "");

    DocDeclStr(
        virtual void , AppendText(const wxString& text),
        "Append text to the end of the document.", "");


    DocDeclStr(
        virtual bool , SetStyle(const wxRichTextRange& range, const wxRichTextAttr& style),
        "Set the style for the text in ``range`` to ``style``", "");

    DocDeclStr(
        virtual bool , SetStyleEx(const wxRichTextRange& range, const wxRichTextAttr& style,
                                  int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO),
        "Extended style setting operation with flags including:
RICHTEXT_SETSTYLE_WITH_UNDO, RICHTEXT_SETSTYLE_OPTIMIZE,
RICHTEXT_SETSTYLE_PARAGRAPHS_ONLY, RICHTEXT_SETSTYLE_CHARACTERS_ONLY", "");
    

    
    DocDeclStr(
        virtual bool , GetStyle(long position, wxRichTextAttr& style),
        "Retrieve the style used at the given position.  Copies the style
values at ``position`` into the ``style`` parameter and returns ``True``
if successful.  Returns ``False`` otherwise.", "");

    DocDeclStr(
        virtual bool , GetUncombinedStyle(long position, wxRichTextAttr& style),
        "Get the content (uncombined) attributes for this position.  Copies the
style values at ``position`` into the ``style`` parameter and returns
``True`` if successful.  Returns ``False`` otherwise.", "");


    DocDeclStr(
        virtual bool , SetDefaultStyle(const wxRichTextAttr& style),
        "Set the style used by default for the rich text document.", "");


    DocDeclStrName(
        virtual const wxRichTextAttr , GetDefaultStyleEx() const,
        "Retrieves a copy of the default style object.", "",
        GetDefaultStyle);


    DocDeclStr(
        virtual long , XYToPosition(long x, long y) const,
        "Translate a col,row coordinants into a document position.", "");

    DocDeclAStr(
        virtual void , PositionToXY(long pos, long *OUTPUT, long *OUTPUT) const,
        "PositionToXY(self, long pos) --> (x, y)",
        "Retrieves the col,row for the given position within the document", "");


    DocDeclStr(
        virtual void , ShowPosition(long position),
        "Ensure that the given position in the document is visible.", "");


    DocDeclAStr(
        virtual wxTextCtrlHitTestResult , HitTest(const wxPoint& pt, long *OUTPUT) const,
        "HitTest(self, Point pt) --> (result, pos)",
        "Returns the character position at the given point in pixels.  Note
that ``pt`` should be given in device coordinates, and not be adjusted
for the client area origin nor for scrolling.  The return value is a
tuple of the hit test result and the position.", "

Possible result values are a bitmask of these flags:

    =========================  ====================================
    RICHTEXT_HITTEST_NONE      The point was not on this object.
    RICHTEXT_HITTEST_BEFORE    The point was before the position
                                  returned from HitTest.
    RICHTEXT_HITTEST_AFTER     The point was after the position
                               returned from HitTest.
    RICHTEXT_HITTEST_ON        The point was on the position
                               returned from HitTest
    =========================  ====================================
");

    DocDeclAStrName(
        virtual wxTextCtrlHitTestResult , HitTest(const wxPoint& pt,
                                                  wxTextCoord *OUTPUT,
                                                  wxTextCoord *OUTPUT) const,
        "HitTestRC(self, Point pt) --> (result, col, row)",
        "Returns the column and row of the given point in pixels.  Note that
``pt`` should be given in device coordinates, and not be adjusted for
the client area origin nor for scrolling.  The return value is a tuple
of the hit test result and the column and row values.", "
:see: `HitTest`",
        HitTestXY);


    // Clipboard operations
    DocDeclStr(
        virtual void , Copy(),
        "Copies the selected text to the clipboard.", "");

    DocDeclStr(
        virtual void , Cut(),
        "Copies the selected text to the clipboard and removes the selection.", "");

    DocDeclStr(
        virtual void , Paste(),
        "Pastes text from the clipboard into the document at the current
insertion point.", "");

    DocDeclStr(
        virtual void , DeleteSelection(),
        "Remove the current selection.", "");


    DocDeclStr(
        virtual bool , CanCopy() const,
        "Returns ``True`` if the selection can be copied to the clipboard.", "");

    DocDeclStr(
        virtual bool , CanCut() const,
        "Returns ``True`` if the selection can be cut to the clipboard.", "");

    DocDeclStr(
        virtual bool , CanPaste() const,
        "Returns ``True`` if the current contents of the clipboard can be
pasted into the document.", "");

    DocDeclStr(
        virtual bool , CanDeleteSelection() const,
        "Returns ``True`` if the selection can be removed from the document.", "");


    // Undo/redo
    DocDeclStr(
        virtual void , Undo(),
        "If the last operation can be undone, undoes the last operation.", "");

    DocDeclStr(
        virtual void , Redo(),
        "If the last operation can be redone, redoes the last operation.", "");


    DocDeclStr(
        virtual bool , CanUndo() const,
        "Returns ``True`` if the last operation can be undone.", "");

    DocDeclStr(
        virtual bool , CanRedo() const,
        "Returns ``True`` if the last operation can be redone.", "");


    // Insertion point
    DocDeclStr(
        virtual void , SetInsertionPoint(long pos),
        "Sets the insertion point at the given position.", "");

    DocDeclStr(
        virtual void , SetInsertionPointEnd(),
        "Moves the insertion point to the end of the document.", "");

    DocDeclStr(
        virtual long , GetInsertionPoint() const,
        "Returns the insertion point. This is defined as the zero based index
of the character position to the right of the insertion point.", "");

    DocDeclStr(
        virtual long , GetLastPosition() const,
        "Returns the zero based index of the last position in the document.", "");


    DocDeclStr(
        virtual void , SetSelection(long from, long to),
        "Selects the text starting at the first position up to (but not
including) the character at the last position. If both parameters are
equal to -1 then all text in the control is selected.", "");

    DocDeclStr(
        virtual void , SelectAll(),
        "Select all text in the document.", "");

    DocDeclStr(
        virtual void , SetEditable(bool editable),
        "Makes the document editable or read-only, overriding the RE_READONLY
flag.", "");


//     /// Call Freeze to prevent refresh
//     virtual void Freeze();

//     /// Call Thaw to refresh
//     virtual void Thaw();

//     /// Call Thaw to refresh
//     DocDeclStr(
//         virtual bool , IsFrozen() const,
//         "", "");


    DocDeclStr(
        virtual bool , HasSelection() const,
        "", "");


///// Functionality specific to wxRichTextCtrl

    /// Write an image at the current insertion point. Supply optional type to use
    /// for internal and file storage of the raw data.
    DocDeclStr(
        virtual bool , WriteImage(const wxImage& image, int bitmapType = wxBITMAP_TYPE_PNG),
        "", "");


    /// Write a bitmap at the current insertion point. Supply optional type to use
    /// for internal and file storage of the raw data.
    DocDeclStrName(
        virtual bool , WriteImage(const wxBitmap& bitmap, int bitmapType = wxBITMAP_TYPE_PNG),
        "", "",
        WriteBitmap);


    /// Load an image from file and write at the current insertion point.
    DocDeclStrName(
        virtual bool , WriteImage(const wxString& filename, int bitmapType),
        "", "",
        WriteImageFile);


    /// Write an image block at the current insertion point.
    DocDeclStrName(
        virtual bool , WriteImage(const wxRichTextImageBlock& imageBlock),
        "", "",
        WriteImageBlock);


    /// Insert a newline (actually paragraph) at the current insertion point.
    DocDeclStr(
        virtual bool , Newline(),
        "", "");


#if USE_TEXTATTREX
/// Set basic (overall) style
    DocDeclStr(
        virtual void , SetBasicStyle(const wxTextAttrEx& style),
        "", "");
#endif

    DocDeclStr(
        virtual void , SetBasicStyle(const wxRichTextAttr& style),
        "", "");


#if USE_TEXTATTREX
    /// Get basic (overall) style
    DocDeclStr(
        virtual const wxTextAttrEx& , GetBasicStyle() const,
        "", "");


    /// Begin using a style
    DocDeclStr(
        virtual bool , BeginStyle(const wxTextAttrEx& style),
        "", "");
#endif

    /// End the style
    DocDeclStr(
        virtual bool , EndStyle(),
        "", "");


    /// End all styles
    DocDeclStr(
        virtual bool , EndAllStyles(),
        "", "");


    /// Begin using bold
    DocDeclStr(
        bool , BeginBold(),
        "", "");


    /// End using bold
    DocDeclStr(
        bool , EndBold(),
        "", "");


    /// Begin using italic
    DocDeclStr(
        bool , BeginItalic(),
        "", "");


    /// End using italic
    DocDeclStr(
        bool , EndItalic(),
        "", "");


    /// Begin using underline
    DocDeclStr(
        bool , BeginUnderline(),
        "", "");


    /// End using underline
    DocDeclStr(
        bool , EndUnderline(),
        "", "");


    /// Begin using point size
    DocDeclStr(
        bool , BeginFontSize(int pointSize),
        "", "");


    /// End using point size
    DocDeclStr(
        bool , EndFontSize(),
        "", "");


    /// Begin using this font
    DocDeclStr(
        bool , BeginFont(const wxFont& font),
        "", "");


    /// End using a font
    DocDeclStr(
        bool , EndFont(),
        "", "");


    /// Begin using this colour
    DocDeclStr(
        bool , BeginTextColour(const wxColour& colour),
        "", "");


    /// End using a colour
    DocDeclStr(
        bool , EndTextColour(),
        "", "");


    /// Begin using alignment
    DocDeclStr(
        bool , BeginAlignment(wxTextAttrAlignment alignment),
        "", "");


    /// End alignment
    DocDeclStr(
        bool , EndAlignment(),
        "", "");


    /// Begin left indent
    DocDeclStr(
        bool , BeginLeftIndent(int leftIndent, int leftSubIndent = 0),
        "", "");


    /// End left indent
    DocDeclStr(
        bool , EndLeftIndent(),
        "", "");


    /// Begin right indent
    DocDeclStr(
        bool , BeginRightIndent(int rightIndent),
        "", "");


    /// End right indent
    DocDeclStr(
        bool , EndRightIndent(),
        "", "");


    /// Begin paragraph spacing
    DocDeclStr(
        bool , BeginParagraphSpacing(int before, int after),
        "", "");


    /// End paragraph spacing
    DocDeclStr(
        bool , EndParagraphSpacing(),
        "", "");


    /// Begin line spacing
    DocDeclStr(
        bool , BeginLineSpacing(int lineSpacing),
        "", "");


    /// End line spacing
    DocDeclStr(
        bool , EndLineSpacing(),
        "", "");


    /// Begin numbered bullet
    DocDeclStr(
        bool , BeginNumberedBullet(int bulletNumber,
                                   int leftIndent,
                                   int leftSubIndent,
                                   int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD),
        "", "");


    /// End numbered bullet
    DocDeclStr(
        bool , EndNumberedBullet(),
        "", "");


    /// Begin symbol bullet
    DocDeclStr(
        bool , BeginSymbolBullet(const wxString& symbol,
                                 int leftIndent,
                                 int leftSubIndent,
                                 int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_SYMBOL),
        "", "");


    /// End symbol bullet
    DocDeclStr(
        bool , EndSymbolBullet(),
        "", "");


    /// Begin named character style
    DocDeclStr(
        bool , BeginCharacterStyle(const wxString& characterStyle),
        "", "");


    /// End named character style
    DocDeclStr(
        bool , EndCharacterStyle(),
        "", "");


    /// Begin named paragraph style
    DocDeclStr(
        bool , BeginParagraphStyle(const wxString& paragraphStyle),
        "", "");


    /// End named character style
    DocDeclStr(
        bool , EndParagraphStyle(),
        "", "");


    DocDeclStr(
        bool , BeginListStyle(const wxString& listStyle, int level = 1, int number = 1),
        "Begin named list style.", "");
        
    DocDeclStr(
        bool , EndListStyle(), "End named list style.", "");

    DocDeclStr(
        bool , BeginURL(const wxString& url, const wxString& characterStyle = wxEmptyString),
            "Begin URL.", "");
    
    DocDeclStr(
        bool , EndURL(), "End URL.", "");

    /// Sets the default style to the style under the cursor
    DocDeclStr(
        bool , SetDefaultStyleToCursorStyle(),
        "", "");


    /// Clear the selection
    DocDeclStr(
        virtual void , SelectNone(),
        "", "");

    /// Select the word at the given character position
    DocDeclStr(
        virtual bool , SelectWord(long position),
        "", "");


    /// Get/set the selection range in character positions. -1, -1 means no selection.
    DocDeclStr(
        wxRichTextRange , GetSelectionRange() const,
        "", "");

    DocDeclStr(
        void , SetSelectionRange(const wxRichTextRange& range),
        "", "");

    /// Get/set the selection range in character positions. -1, -1 means no selection.
    /// The range is in internal format, i.e. a single character selection is denoted
    /// by (n, n)
    DocDeclStr(
        const wxRichTextRange& , GetInternalSelectionRange() const,
        "", "");

    DocDeclStr(
        void , SetInternalSelectionRange(const wxRichTextRange& range),
        "", "");



    /// Add a new paragraph of text to the end of the buffer
    DocDeclStr(
        virtual wxRichTextRange , AddParagraph(const wxString& text),
        "", "");


    /// Add an image
    DocDeclStr(
        virtual wxRichTextRange , AddImage(const wxImage& image),
        "", "");


    /// Layout the buffer: which we must do before certain operations, such as
    /// setting the caret position.
    DocDeclStr(
        virtual bool , LayoutContent(bool onlyVisibleRect = false),
        "", "");


    /// Move the caret to the given character position
    DocDeclStr(
        virtual bool , MoveCaret(long pos, bool showAtLineStart = false),
        "", "");


    /// Move right
    DocDeclStr(
        virtual bool , MoveRight(int noPositions = 1, int flags = 0),
        "", "");


    /// Move left
    DocDeclStr(
        virtual bool , MoveLeft(int noPositions = 1, int flags = 0),
        "", "");


    /// Move up
    DocDeclStr(
        virtual bool , MoveUp(int noLines = 1, int flags = 0),
        "", "");


    /// Move up
    DocDeclStr(
        virtual bool , MoveDown(int noLines = 1, int flags = 0),
        "", "");


    /// Move to the end of the line
    DocDeclStr(
        virtual bool , MoveToLineEnd(int flags = 0),
        "", "");


    /// Move to the start of the line
    DocDeclStr(
        virtual bool , MoveToLineStart(int flags = 0),
        "", "");


    /// Move to the end of the paragraph
    DocDeclStr(
        virtual bool , MoveToParagraphEnd(int flags = 0),
        "", "");


    /// Move to the start of the paragraph
    DocDeclStr(
        virtual bool , MoveToParagraphStart(int flags = 0),
        "", "");


    /// Move to the start of the buffer
    DocDeclStr(
        virtual bool , MoveHome(int flags = 0),
        "", "");


    /// Move to the end of the buffer
    DocDeclStr(
        virtual bool , MoveEnd(int flags = 0),
        "", "");


    /// Move n pages up
    DocDeclStr(
        virtual bool , PageUp(int noPages = 1, int flags = 0),
        "", "");


    /// Move n pages down
    DocDeclStr(
        virtual bool , PageDown(int noPages = 1, int flags = 0),
        "", "");


    /// Move n words left
    DocDeclStr(
        virtual bool , WordLeft(int noPages = 1, int flags = 0),
        "", "");


    /// Move n words right
    DocDeclStr(
        virtual bool , WordRight(int noPages = 1, int flags = 0),
        "", "");


    /// Returns the buffer associated with the control.
//    wxRichTextBuffer& GetBuffer();
    DocDeclStr(
        const wxRichTextBuffer& , GetBuffer() const,
        "", "");


    /// Start batching undo history for commands.
    DocDeclStr(
        virtual bool , BeginBatchUndo(const wxString& cmdName),
        "", "");


    /// End batching undo history for commands.
    DocDeclStr(
        virtual bool , EndBatchUndo(),
        "", "");


    /// Are we batching undo history for commands?
    DocDeclStr(
        virtual bool , BatchingUndo() const,
        "", "");


    /// Start suppressing undo history for commands.
    DocDeclStr(
        virtual bool , BeginSuppressUndo(),
        "", "");


    /// End suppressing undo history for commands.
    DocDeclStr(
        virtual bool , EndSuppressUndo(),
        "", "");


    /// Are we suppressing undo history for commands?
    DocDeclStr(
        virtual bool , SuppressingUndo() const,
        "", "");


    /// Test if this whole range has character attributes of the specified kind. If any
    /// of the attributes are different within the range, the test fails. You
    /// can use this to implement, for example, bold button updating. style must have
    /// flags indicating which attributes are of interest.
#if USE_TEXTATTREX
    DocDeclStr(
        virtual bool , HasCharacterAttributes(const wxRichTextRange& range,
                                              const wxTextAttrEx& style) const,
        "", "");
#endif

    DocDeclStr(
        virtual bool , HasCharacterAttributes(const wxRichTextRange& range,
                                              const wxRichTextAttr& style) const,
        "", "");



    /// Test if this whole range has paragraph attributes of the specified kind. If any
    /// of the attributes are different within the range, the test fails. You
    /// can use this to implement, for example, centering button updating. style must have
    /// flags indicating which attributes are of interest.
#if USE_TEXTATTREX
    DocDeclStr(
        virtual bool , HasParagraphAttributes(const wxRichTextRange& range,
                                              const wxTextAttrEx& style) const,
        "", "");
#endif

    DocDeclStr(
        virtual bool , HasParagraphAttributes(const wxRichTextRange& range,
                                              const wxRichTextAttr& style) const,
        "", "");



    /// Is all of the selection bold?
    DocDeclStr(
        virtual bool , IsSelectionBold(),
        "", "");


    /// Is all of the selection italics?
    DocDeclStr(
        virtual bool , IsSelectionItalics(),
        "", "");


    /// Is all of the selection underlined?
    DocDeclStr(
        virtual bool , IsSelectionUnderlined(),
        "", "");


    /// Is all of the selection aligned according to the specified flag?
    DocDeclStr(
        virtual bool , IsSelectionAligned(wxTextAttrAlignment alignment),
        "", "");


    /// Apply bold to the selection
    DocDeclStr(
        virtual bool , ApplyBoldToSelection(),
        "", "");


    /// Apply italic to the selection
    DocDeclStr(
        virtual bool , ApplyItalicToSelection(),
        "", "");


    /// Apply underline to the selection
    DocDeclStr(
        virtual bool , ApplyUnderlineToSelection(),
        "", "");


    /// Apply alignment to the selection
    DocDeclStr(
        virtual bool , ApplyAlignmentToSelection(wxTextAttrAlignment alignment),
        "", "");


    /// Set style sheet, if any.
    DocDeclStr(
        void , SetStyleSheet(wxRichTextStyleSheet* styleSheet),
        "", "");

    DocDeclStr(
        wxRichTextStyleSheet* , GetStyleSheet() const,
        "", "");

    /// Apply the style sheet to the buffer, for example if the styles have changed.
    DocDeclStr(
        bool , ApplyStyleSheet(wxRichTextStyleSheet* styleSheet = NULL),
        "", "");



    %property(Buffer, GetBuffer, doc="See `GetBuffer`");
    %property(DefaultStyle, GetDefaultStyle, SetDefaultStyle, doc="See `GetDefaultStyle` and `SetDefaultStyle`");
    %property(DelayedLayoutThreshold, GetDelayedLayoutThreshold, SetDelayedLayoutThreshold, doc="See `GetDelayedLayoutThreshold` and `SetDelayedLayoutThreshold`");
    %property(Filename, GetFilename, SetFilename, doc="See `GetFilename` and `SetFilename`");
    %property(InsertionPoint, GetInsertionPoint, SetInsertionPoint, doc="See `GetInsertionPoint` and `SetInsertionPoint`");
    %property(InternalSelectionRange, GetInternalSelectionRange, SetInternalSelectionRange, doc="See `GetInternalSelectionRange` and `SetInternalSelectionRange`");
    %property(LastPosition, GetLastPosition, doc="See `GetLastPosition`");
    %property(NumberOfLines, GetNumberOfLines, doc="See `GetNumberOfLines`");
    %property(Selection, GetSelection, SetSelection, doc="See `GetSelection` and `SetSelection`");
    %property(SelectionRange, GetSelectionRange, SetSelectionRange, doc="See `GetSelectionRange` and `SetSelectionRange`");
    %property(StringSelection, GetStringSelection, doc="See `GetStringSelection`");
    %property(StyleSheet, GetStyleSheet, SetStyleSheet, doc="See `GetStyleSheet` and `SetStyleSheet`");
    %property(Value, GetValue, SetValue, doc="See `GetValue` and `SetValue`");

// Implementation
// TODO: Which of these should be exposed to Python?

//     /// Set font, and also default attributes
//     virtual bool SetFont(const wxFont& font);

//     /// Set up scrollbars, e.g. after a resize
//     virtual void SetupScrollbars(bool atTop = false);

//     /// Keyboard navigation
//     virtual bool KeyboardNavigate(int keyCode, int flags);

//     /// Paint the background
//     virtual void PaintBackground(wxDC& dc);

//     /// Recreate buffer bitmap if necessary
//     virtual bool RecreateBuffer(const wxSize& size = wxDefaultSize);

//     /// Set the selection
//     virtual void DoSetSelection(long from, long to, bool scrollCaret = true);

//     /// Write text
//     virtual void DoWriteText(const wxString& value, bool selectionOnly = true);

//     /// Send an update event
//     virtual bool SendUpdateEvent();

//     /// Init command event
//     void InitCommandEvent(wxCommandEvent& event) const;

//     /// do the window-specific processing after processing the update event
//     //  (duplicated code from wxTextCtrlBase)
// #if !wxRICHTEXT_DERIVES_FROM_TEXTCTRLBASE
//     virtual void DoUpdateWindowUI(wxUpdateUIEvent& event);
// #endif

//     /// Should we inherit colours?
//     virtual bool ShouldInheritColours() const;

//     /// Position the caret
//     virtual void PositionCaret();

//     /// Extend the selection, returning true if the selection was
//     /// changed. Selections are in caret positions.
//     virtual bool ExtendSelection(long oldPosition, long newPosition, int flags);

//     /// Scroll into view. This takes a _caret_ position.
//     virtual bool ScrollIntoView(long position, int keyCode);

//     /// The caret position is the character position just before the caret.
//     /// A value of -1 means the caret is at the start of the buffer.
//     void SetCaretPosition(long position, bool showAtLineStart = false) ;
//     long GetCaretPosition() const;

//     /// Move caret one visual step forward: this may mean setting a flag
//     /// and keeping the same position if we're going from the end of one line
//     /// to the start of the next, which may be the exact same caret position.
//     void MoveCaretForward(long oldPosition) ;

//     /// Move caret one visual step forward: this may mean setting a flag
//     /// and keeping the same position if we're going from the end of one line
//     /// to the start of the next, which may be the exact same caret position.
//     void MoveCaretBack(long oldPosition) ;

//     /// Get the caret height and position for the given character position
//     bool GetCaretPositionForIndex(long position, wxRect& rect);

//     /// Gets the line for the visible caret position. If the caret is
//     /// shown at the very end of the line, it means the next character is actually
//     /// on the following line. So let's get the line we're expecting to find
//     /// if this is the case.
//     wxRichTextLine* GetVisibleLineForCaretPosition(long caretPosition) const;

//     /// Gets the command processor
//     wxCommandProcessor* GetCommandProcessor() const;

//     /// Delete content if there is a selection, e.g. when pressing a key.
//     /// Returns the new caret position in newPos, or leaves it if there
//     /// was no action.
//     bool DeleteSelectedContent(long* newPos= NULL);

//     /// Transform logical to physical
//     wxPoint GetPhysicalPoint(const wxPoint& ptLogical) const;

//     /// Transform physical to logical
//     wxPoint GetLogicalPoint(const wxPoint& ptPhysical) const;

//     /// Finds the caret position for the next word. Direction
//     /// is 1 (forward) or -1 (backwards).
//     virtual long FindNextWordPosition(int direction = 1) const;

//     /// Is the given position visible on the screen?
//     bool IsPositionVisible(long pos) const;

//     /// Returns the first visible position in the current view
//     long GetFirstVisiblePosition() const;
};


//----------------------------------------------------------------------


%constant wxEventType wxEVT_COMMAND_RICHTEXT_LEFT_CLICK;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_RIGHT_CLICK;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_MIDDLE_CLICK;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_LEFT_DCLICK;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_RETURN;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGED;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_CHARACTER;
%constant wxEventType wxEVT_COMMAND_RICHTEXT_DELETE;

%pythoncode {
EVT_RICHTEXT_LEFT_CLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_LEFT_CLICK, 1)
EVT_RICHTEXT_RIGHT_CLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_RIGHT_CLICK, 1)
EVT_RICHTEXT_MIDDLE_CLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_MIDDLE_CLICK, 1)
EVT_RICHTEXT_LEFT_DCLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_LEFT_DCLICK, 1)
EVT_RICHTEXT_RETURN = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_RETURN, 1)
EVT_RICHTEXT_STYLESHEET_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING, 1)
EVT_RICHTEXT_STYLESHEET_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGED, 1)
EVT_RICHTEXT_STYLESHEET_REPLACING = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING, 1)
EVT_RICHTEXT_STYLESHEET_REPLACED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED, 1)
EVT_RICHTEXT_CHARACTER = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_CHARACTER, 1)
EVT_RICHTEXT_DELETE = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_DELETE, 1)
}


class wxRichTextEvent : public wxNotifyEvent
{
public:
    wxRichTextEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);

    int GetPosition() const;
    void SetPosition(int n);

    int GetFlags() const;
    void SetFlags(int flags);

    %property(Flags, GetFlags, SetFlags, doc="See `GetFlags` and `SetFlags`");
    %property(Index, GetPosition, SetPosition, doc="See `GetPosition` and `SetPosition`");
};

//----------------------------------------------------------------------

%init %{
    wxRichTextModuleInit();
%}

//----------------------------------------------------------------------
