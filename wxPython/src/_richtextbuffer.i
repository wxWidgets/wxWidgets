/////////////////////////////////////////////////////////////////////////////
// Name:        _richtextbuffer.i
// Purpose:     wxRichTextAttr
//
// Author:      Robin Dunn
//
// Created:     11-April-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include <wx/sstream.h>
#include "wx/wxPython/pyistream.h"
%}

//---------------------------------------------------------------------------
%newgroup

enum {
/*!
 * File types
 */
    wxRICHTEXT_TYPE_ANY,
    wxRICHTEXT_TYPE_TEXT,
    wxRICHTEXT_TYPE_XML,
    wxRICHTEXT_TYPE_HTML,
    wxRICHTEXT_TYPE_RTF,
    wxRICHTEXT_TYPE_PDF,

/*!
 * Flags determining the available space, passed to Layout
 */
    wxRICHTEXT_FIXED_WIDTH,
    wxRICHTEXT_FIXED_HEIGHT,
    wxRICHTEXT_VARIABLE_WIDTH,
    wxRICHTEXT_VARIABLE_HEIGHT,


    wxRICHTEXT_LAYOUT_SPECIFIED_RECT,
    wxRICHTEXT_DRAW_IGNORE_CACHE,


/*!
 * Flags returned from hit-testing
 */
    wxRICHTEXT_HITTEST_NONE,
    wxRICHTEXT_HITTEST_BEFORE,
    wxRICHTEXT_HITTEST_AFTER,
    wxRICHTEXT_HITTEST_ON,
    wxRICHTEXT_HITTEST_OUTSIDE,

/*!
 * Flags for GetRangeSize
 */
    wxRICHTEXT_FORMATTED,
    wxRICHTEXT_UNFORMATTED,

/*!
 * Flags for SetStyle/SetListStyle
 */
    wxRICHTEXT_SETSTYLE_NONE,
    wxRICHTEXT_SETSTYLE_WITH_UNDO,
    wxRICHTEXT_SETSTYLE_OPTIMIZE,
    wxRICHTEXT_SETSTYLE_PARAGRAPHS_ONLY,
    wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY,
    wxRICHTEXT_SETSTYLE_RENUMBER,
    wxRICHTEXT_SETSTYLE_SPECIFY_LEVEL,
    wxRICHTEXT_SETSTYLE_RESET,
    wxRICHTEXT_SETSTYLE_REMOVE,

/*!
 * Flags for text insertion
 */
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
    wxTEXT_ATTR_BULLET_TEXT,
    wxTEXT_ATTR_BULLET_NAME,
    wxTEXT_ATTR_URL,
    wxTEXT_ATTR_PAGE_BREAK,
    wxTEXT_ATTR_EFFECTS,
    wxTEXT_ATTR_OUTLINE_LEVEL,

/*!
 * Styles for wxTextAttrEx::SetBulletStyle
 */
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
    wxTEXT_ATTR_BULLET_STYLE_STANDARD,
    wxTEXT_ATTR_BULLET_STYLE_RIGHT_PARENTHESIS,
    wxTEXT_ATTR_BULLET_STYLE_OUTLINE,

    wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT,
    wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT,
    wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE,

/*!
 * Styles for wxTextAttrEx::SetTextEffects
 */
    wxTEXT_ATTR_EFFECT_NONE,
    wxTEXT_ATTR_EFFECT_CAPITALS,
    wxTEXT_ATTR_EFFECT_SMALL_CAPITALS,
    wxTEXT_ATTR_EFFECT_STRIKETHROUGH,
    wxTEXT_ATTR_EFFECT_DOUBLE_STRIKETHROUGH,
    wxTEXT_ATTR_EFFECT_SHADOW,
    wxTEXT_ATTR_EFFECT_EMBOSS,
    wxTEXT_ATTR_EFFECT_OUTLINE,
    wxTEXT_ATTR_EFFECT_ENGRAVE,
    wxTEXT_ATTR_EFFECT_SUPERSCRIPT,
    wxTEXT_ATTR_EFFECT_SUBSCRIPT,

/*!
 * Line spacing values
 */
    wxTEXT_ATTR_LINE_SPACING_NORMAL,
    wxTEXT_ATTR_LINE_SPACING_HALF,
    wxTEXT_ATTR_LINE_SPACING_TWICE,

/*!
 * Character and paragraph combined styles
 */
    wxTEXT_ATTR_CHARACTER,
    wxTEXT_ATTR_PARAGRAPH,
    wxTEXT_ATTR_ALL,

 };


//---------------------------------------------------------------------------
%newgroup



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

// Turn off the generation of code that aquires the Global Interpreter Lock
%threadWrapperOff

class wxRichTextRange
{
public:
    DocCtorStr(
        wxRichTextRange(long start=0, long end=0),
        "Creates a new range object.", "");

    ~wxRichTextRange();

    %extend {
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


    DocDeclStr(
        wxRichTextRange , ToInternal() const,
        "Convert to internal form: (n, n) is the range of a single character.", "");


    DocDeclStr(
        wxRichTextRange , FromInternal() const,
        "Convert from internal to public API form: (n, n+1) is the range of a
single character.", "");


    %extend {
        DocAStr(Get,
               "Get() -> (start,end)",
               "Returns the start and end properties as a tuple.", "");
        PyObject* Get() {
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->GetStart()));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->GetEnd()));
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

// Turn back on the generation of code that aquires the Global Interpreter Lock
%threadWrapperOn




//---------------------------------------------------------------------------
%newgroup


DocStr(wxRichTextAttr,
"The RichTextAttr class stores information about the various attributes
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


    DocDeclStr(
        void , Init(),
        "Initialise this object.", "");


    DocDeclStr(
        void , Copy(const wxRichTextAttr& attr),
        "Copy from attr to self.", "");


    // Equality test
    bool operator== (const wxRichTextAttr& attr) const;



    DocDeclStr(
        wxFont , CreateFont() const,
        "Create font from the font attributes in this attr object.", "");


    DocDeclStr(
        bool , GetFontAttributes(const wxFont& font),
        "Set our font attributes from the font.", "");


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
    void SetListStyleName(const wxString& name);
    void SetParagraphSpacingAfter(int spacing);
    void SetParagraphSpacingBefore(int spacing);
    void SetLineSpacing(int spacing);
    void SetBulletStyle(int style);
    void SetBulletNumber(int n);
    void SetBulletText(wxChar symbol);
    void SetBulletFont(const wxString& bulletFont);
    void SetBulletName(const wxString& name);
    void SetURL(const wxString& url);
    void SetPageBreak(bool pageBreak = true);
    void SetTextEffects(int effects);
    void SetTextEffectFlags(int effects);
    void SetOutlineLevel(int level);

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
    const wxString& GetListStyleName() const;
    int GetParagraphSpacingAfter() const;
    int GetParagraphSpacingBefore() const;
    int GetLineSpacing() const;
    int GetBulletStyle() const;
    int GetBulletNumber() const;
    const wxString& GetBulletText() const;
    const wxString& GetBulletFont() const;
    const wxString& GetBulletName() const;
    const wxString& GetURL() const;
    int GetTextEffects() const;
    int GetTextEffectFlags() const;
    int GetOutlineLevel() const;

    // accessors
    bool HasTextColour() const;
    bool HasBackgroundColour() const;
    bool HasAlignment() const;
    bool HasTabs() const;
    bool HasLeftIndent() const;
    bool HasRightIndent() const;
    bool HasFontWeight() const;
    bool HasFontSize() const;
    bool HasFontItalic() const;
    bool HasFontUnderlined() const;
    bool HasFontFaceName() const;
    bool HasFont() const;

    bool HasParagraphSpacingAfter() const;
    bool HasParagraphSpacingBefore() const;
    bool HasLineSpacing() const;
    bool HasCharacterStyleName() const;
    bool HasParagraphStyleName() const;
    bool HasListStyleName() const;
    bool HasBulletStyle() const;
    bool HasBulletNumber() const;
    bool HasBulletText() const;
    bool HasBulletName() const;
    bool HasURL() const;
    bool HasPageBreak() const;
    bool HasTextEffects() const;
    bool HasTextEffect(int effect) const;
    bool HasOutlineLevel() const;

    bool HasFlag(long flag) const;

    bool IsCharacterStyle() const;
    bool IsParagraphStyle() const;


    DocDeclStr(
        bool , IsDefault() const,
        "Returns false if we have any attributes set, true otherwise", "");


    DocDeclStr(
        bool , Apply(const wxRichTextAttr& style, const wxRichTextAttr* compareWith = NULL),
        "Merges the given attributes. Does not affect self. If compareWith is
not None, then it will be used to mask out those attributes that are
the same in style and compareWith, for situations where we don't want
to explicitly set inherited attributes.
", "");


    DocDeclStr(
        wxRichTextAttr , Combine(const wxRichTextAttr& style, const wxRichTextAttr* compareWith = NULL) const,
        "Merges the given attributes and returns the result. Does not affect
self. If compareWith is not None, then it will be used to mask out
those attributes that are the same in style and compareWith, for
situations where we don't want to explicitly set inherited attributes.
", "");



    %property(Alignment, GetAlignment, SetAlignment);
    %property(BackgroundColour, GetBackgroundColour, SetBackgroundColour);
    %property(BulletFont, GetBulletFont, SetBulletFont);
    %property(BulletNumber, GetBulletNumber, SetBulletNumber);
    %property(BulletStyle, GetBulletStyle, SetBulletStyle);
    %property(BulletText, GetBulletText, SetBulletText);
    %property(CharacterStyleName, GetCharacterStyleName, SetCharacterStyleName);
    %property(Flags, GetFlags, SetFlags);
    %property(Font, GetFont, SetFont);
    %property(FontAttributes, GetFontAttributes);
    %property(FontFaceName, GetFontFaceName, SetFontFaceName);
    %property(FontSize, GetFontSize, SetFontSize);
    %property(FontStyle, GetFontStyle, SetFontStyle);
    %property(FontUnderlined, GetFontUnderlined, SetFontUnderlined);
    %property(FontWeight, GetFontWeight, SetFontWeight);
    %property(LeftIndent, GetLeftIndent, SetLeftIndent);
    %property(LeftSubIndent, GetLeftSubIndent);
    %property(LineSpacing, GetLineSpacing, SetLineSpacing);
    %property(ParagraphSpacingAfter, GetParagraphSpacingAfter, SetParagraphSpacingAfter);
    %property(ParagraphSpacingBefore, GetParagraphSpacingBefore, SetParagraphSpacingBefore);
    %property(ParagraphStyleName, GetParagraphStyleName, SetParagraphStyleName);
    %property(RightIndent, GetRightIndent, SetRightIndent);
    %property(Tabs, GetTabs, SetTabs);
    %property(TextColour, GetTextColour, SetTextColour);

    %property(ListStyleName, GetListStyleName, SetListStyleName);
    %property(BulletName, GetBulletName, SetBulletName);
    %property(URL, GetURL, SetURL);
    %property(TextEffects, GetTextEffects, SetTextEffects);
    %property(TextEffectFlags, GetTextEffectFlags, SetTextEffectFlags);
    %property(OutlineLevel, GetOutlineLevel, SetOutlineLevel);
};



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


// TODO TODO TODO
// There's still lots to do for these classes...
//
// 1. Decide how to coalesce overloaded methods and ctors
// 2. correctly set disown flags and re-ownership as needed
// 3. Implement PyRichTextObject
// 4. Decide how to typemap and tweak the virtuals that pass back
//    values in their parameters (yuck! damn C++)
// 5. better handling of streams
// 6. list-like wrapper for C++ wxLists
// 7. wrappers for wxTextAttrEx --> wxRichTextAttr (as needed)
// 8. Add more properties



DocStr(wxRichTextObject,
"This is the base class for all drawable objects in a `RichTextCtrl`.

The data displayed in a `RichTextCtrl` is handled by `RichTextBuffer`,
and a `RichTextCtrl` always has one such buffer.

The content is represented by a hierarchy of objects, all derived from
`RichTextObject`. An object might be an image, a fragment of text, a
paragraph, or a whole buffer. Objects store a an attribute object
containing style information; a paragraph object can contain both
paragraph and character information, but content objects such as text
can only store character information. The final style displayed in the
control or in a printout is a combination of base style, paragraph
style and content (character) style.

The top of the hierarchy is the buffer, a kind of
`RichTextParagraphLayoutBox`. containing further `RichTextParagraph`
objects, each of which can include text, images and potentially other
types of objects.

Each object maintains a range (start and end position) measured from
the start of the main parent object.

When Layout is called on an object, it is given a size which the
object must limit itself to, or one or more flexible directions
(vertical or horizontal). So, for example, a centred paragraph is
given the page width to play with (minus any margins), but can extend
indefinitely in the vertical direction. The implementation of Layout
caches the calculated size and position.

When the buffer is modified, a range is invalidated (marked as
requiring layout), so that only the minimum amount of layout is
performed.

A paragraph of pure text with the same style contains just one further
object, a `RichTextPlainText` object. When styling is applied to part
of this object, the object is decomposed into separate objects, one
object for each different character style. So each object within a
paragraph always has just one attribute object to denote its character
style. Of course, this can lead to fragmentation after a lot of edit
operations, potentially leading to several objects with the same style
where just one would do. So a Defragment function is called when
updating the control's display, to ensure that the minimum number of
objects is used.

To implement your own RichTextObjects in Python you must derive a
class from `PyRichTextObject`, which has been instrumented to forward
the virtual C++ method calls to the Python methods in the derived
class. (This class hasn't been implemented yet!)", "");

// argout typemap for wxPoint
%typemap(in, numinputs=0, noblock=1) wxPoint& OUTPUT (wxPoint temp) {
    $1 = &temp;
}
%typemap(argout, noblock=1) wxPoint& OUTPUT {
    %append_output(SWIG_NewPointerObj((void*)new wxPoint(*$1), $1_descriptor, SWIG_POINTER_OWN));
}

// argout typemap for wxSize
%typemap(in, numinputs=0, noblock=1) wxSize& OUTPUT (wxSize temp) {
    $1 = &temp;
}
%typemap(argout, noblock=1) wxSize& OUTPUT {
    %append_output(SWIG_NewPointerObj((void*)new wxSize(*$1), $1_descriptor, SWIG_POINTER_OWN));
}


class wxRichTextObject: public wxObject
{
public:
    // wxRichTextObject(wxRichTextObject* parent = NULL);  // **** This is an ABC
    virtual ~wxRichTextObject();


// Overrideables

    /// Draw the item, within the given range. Some objects may ignore the range (for
    /// example paragraphs) while others must obey it (lines, to implement wrapping)
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range,
                      const wxRichTextRange& selectionRange, const wxRect& rect,
                      int descent, int style);

    /// Lay the item out at the specified position with the given size constraint.
    /// Layout must set the cached size.
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style);

    /// Hit-testing: returns a flag indicating hit test details, plus
    /// information about position
    virtual int HitTest(wxDC& dc, const wxPoint& pt, long& OUTPUT /*textPosition*/);

    /// Finds the absolute position and row height for the given character position
    virtual bool FindPosition(wxDC& dc, long index, wxPoint& OUTPUT /*pt*/,
                              int* OUTPUT /*height*/, bool forceLineStart);
    

    /// Get the best size, i.e. the ideal starting size for this object irrespective
    /// of available space. For a short text string, it will be the size that exactly encloses
    /// the text. For a longer string, it might use the parent width for example.
    virtual wxSize GetBestSize() const;

    /// Get the object size for the given range. Returns false if the range
    /// is invalid for this object.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& OUTPUT /*size*/,
                              int& OUTPUT /*descent*/, wxDC& dc, int flags,
                              wxPoint position = wxPoint(0,0)) const;

    /// Do a split, returning an object containing the second part, and setting
    /// the first part in 'this'.
    virtual wxRichTextObject* DoSplit(long pos);

    /// Calculate range. By default, guess that the object is 1 unit long.
    virtual void CalculateRange(long start, long& OUTPUT /*end*/);

    /// Delete range
    virtual bool DeleteRange(const wxRichTextRange& range);

    /// Returns true if the object is empty
    virtual bool IsEmpty() const;

    /// Get any text in this object for the given range
    virtual wxString GetTextForRange(const wxRichTextRange& range) const;

    /// Returns true if this object can merge itself with the given one.
    virtual bool CanMerge(wxRichTextObject* object) const;

    /// Returns true if this object merged itself with the given one.
    /// The calling code will then delete the given object.
    virtual bool Merge(wxRichTextObject* object);

    /// Dump to output stream for debugging
    //virtual void Dump(wxTextOutputStream& stream);
    %extend {
        wxString Dump() {
            wxStringOutputStream strstream;
            wxTextOutputStream txtstream(strstream);
            self->Dump(txtstream);
            return strstream.GetString();
        }
    }

// Accessors

    /// Get/set the cached object size as calculated by Layout.
    virtual wxSize GetCachedSize() const;
    virtual void SetCachedSize(const wxSize& sz);
    %property(CachedSize, GetCachedSize, SetCachedSize);

    /// Get/set the object position
    virtual wxPoint GetPosition() const;
    virtual void SetPosition(const wxPoint& pos);
    %property(Position, GetPosition, SetPosition);

    /// Get the rectangle enclosing the object
    virtual wxRect GetRect() const;
    %property(Rect, GetRect);


    /// Set the range
    void SetRange(const wxRichTextRange& range);
    /// Get the range
    wxRichTextRange GetRange();
    %property(Range, GetRange, SetRange);


    /// Get/set dirty flag (whether the object needs Layout to be called)
    virtual bool GetDirty() const;
    virtual void SetDirty(bool dirty);
    %property(Dirty, GetDirty, SetDirty);


    /// Is this composite?
    virtual bool IsComposite() const;

    /// Get/set the parent.
    virtual wxRichTextObject* GetParent() const;
    virtual void SetParent(wxRichTextObject* parent);
    %property(Parent, GetParent, SetParent);


    // TODO: Morph these into SetLeftMargin and etc. for wxPython so
    // there can be proper properties

    /// Set the margin around the object
    %Rename(SetSameMargins,
            virtual void , SetMargins(int margin));
    virtual void SetMargins(int leftMargin, int rightMargin, int topMargin, int bottomMargin);
    virtual int GetLeftMargin() const;
    virtual int GetRightMargin() const;
    virtual int GetTopMargin() const;
    virtual int GetBottomMargin() const;


    /// Set attributes object
    void SetAttributes(const wxRichTextAttr& attr);
    wxRichTextAttr GetAttributes();
    %property(Attributes, GetAttributes, SetAttributes);

    /// Set/get stored descent
    void SetDescent(int descent);
    int GetDescent() const;
    %property(Descent, GetDescent, SetDescent);

    /// Gets the containing buffer
    wxRichTextBuffer* GetBuffer() const;

// Operations

    /// Clone the object
    virtual wxRichTextObject* Clone() const;

    /// Copy
    void Copy(const wxRichTextObject& obj);

    /// Reference-counting allows us to use the same object in multiple
    /// lists (not yet used)
    void Reference();
    void Dereference();

    /// Convert units in tenths of a millimetre to device units
    %Rename(ConvertTenthsMMToPixelsDC,
            int, ConvertTenthsMMToPixels(wxDC& dc, int units));
    static int ConvertTenthsMMToPixels(int ppi, int units);

};




wxLIST_WRAPPER(wxRichTextObjectList, wxRichTextObject);

    
DocStr(wxRichTextCompositeObject,
       "Objects of this class can contain other rich text objects.", "");


class wxRichTextCompositeObject: public wxRichTextObject
{
public:
    // wxRichTextCompositeObject(wxRichTextObject* parent = NULL);   **** This is an ABC
    virtual ~wxRichTextCompositeObject();

// Accessors

    /// Get the children
    wxRichTextObjectList& GetChildren();

    /// Get the child count
    size_t GetChildCount() const ;

    /// Get the nth child
    wxRichTextObject* GetChild(size_t n) const ;

// Operations

    /// Copy
    void Copy(const wxRichTextCompositeObject& obj);

    %disownarg(wxRichTextObject* child);
    
    /// Append a child, returning the position
    size_t AppendChild(wxRichTextObject* child) ;

    /// Insert the child in front of the given object, or at the beginning
    bool InsertChild(wxRichTextObject* child, wxRichTextObject* inFrontOf) ;

    %cleardisown(wxRichTextObject* child);

    
    /// Delete the child
    %feature("shadow") RemoveChild %{
        def RemoveChild(self, child, deleteChild=False):
            val = _richtext.RichTextCompositeObject_RemoveChild(self, child, deleteChild)
            self.this.own(not deleteChild)
            return val
    %}
    bool RemoveChild(wxRichTextObject* child, bool deleteChild = false) ;

    /// Delete all children
    bool DeleteChildren();

    /// Recursively merge all pieces that can be merged.
    bool Defragment();
};






DocStr(wxRichTextBox,
       "This defines a 2D space to lay out objects.", "");

class wxRichTextBox: public wxRichTextCompositeObject
{
public:
    wxRichTextBox(wxRichTextObject* parent = NULL);

    virtual wxRichTextObject* Clone() const;
    void Copy(const wxRichTextBox& obj);
};






DocStr(wxRichTextParagraphBox,
       "This box knows how to lay out paragraphs.", "");

class wxRichTextParagraphLayoutBox: public wxRichTextBox
{
public:

    wxRichTextParagraphLayoutBox(wxRichTextObject* parent = NULL);

// Accessors

    /// Associate a control with the buffer, for operations that for example require refreshing the window.
    void SetRichTextCtrl(wxRichTextCtrl* ctrl);

    /// Get the associated control.
    wxRichTextCtrl* GetRichTextCtrl() const;

    /// Get/set whether the last paragraph is partial or complete
    void SetPartialParagraph(bool partialPara);
    bool GetPartialParagraph() const;

    /// If this is a buffer, returns the current style sheet. The base layout box
    /// class doesn't have an associated style sheet.
    virtual wxRichTextStyleSheet* GetStyleSheet() const;

// Operations

    /// Initialize the object.
    void Init();

    /// Clear all children
    virtual void Clear();

    /// Clear and initialize with one blank paragraph
    virtual void Reset();

    /// Convenience function to add a paragraph of text
    virtual wxRichTextRange AddParagraph(const wxString& text, wxTextAttrEx* paraStyle = NULL);

    /// Convenience function to add an image
    virtual wxRichTextRange AddImage(const wxImage& image, wxTextAttrEx* paraStyle = NULL);

    /// Adds multiple paragraphs, based on newlines.
    virtual wxRichTextRange AddParagraphs(const wxString& text, wxTextAttrEx* paraStyle = NULL);

    /// Get the line at the given position. If caretPosition is true, the position is
    /// a caret position, which is normally a smaller number.
    virtual wxRichTextLine* GetLineAtPosition(long pos, bool caretPosition = false) const;

    /// Get the line at the given y pixel position, or the last line.
    virtual wxRichTextLine* GetLineAtYPosition(int y) const;

    /// Get the paragraph at the given character or caret position
    virtual wxRichTextParagraph* GetParagraphAtPosition(long pos, bool caretPosition = false) const;

    /// Get the line size at the given position
    virtual wxSize GetLineSizeAtPosition(long pos, bool caretPosition = false) const;

    /// Given a position, get the number of the visible line (potentially many to a paragraph),
    /// starting from zero at the start of the buffer. We also have to pass a bool (startOfLine)
    /// that indicates whether the caret is being shown at the end of the previous line or at the start
    /// of the next, since the caret can be shown at 2 visible positions for the same underlying
    /// position.
    virtual long GetVisibleLineNumber(long pos, bool caretPosition = false, bool startOfLine = false) const;

    /// Given a line number, get the corresponding wxRichTextLine object.
    virtual wxRichTextLine* GetLineForVisibleLineNumber(long lineNumber) const;

    /// Get the leaf object in a paragraph at this position.
    /// Given a line number, get the corresponding wxRichTextLine object.
    virtual wxRichTextObject* GetLeafObjectAtPosition(long position) const;

    /// Get the paragraph by number
    virtual wxRichTextParagraph* GetParagraphAtLine(long paragraphNumber) const;

    /// Get the paragraph for a given line
    virtual wxRichTextParagraph* GetParagraphForLine(wxRichTextLine* line) const;

    /// Get the length of the paragraph
    virtual int GetParagraphLength(long paragraphNumber) const;

    /// Get the number of paragraphs
    virtual int GetParagraphCount() const { return GetChildCount(); }

    /// Get the number of visible lines
    virtual int GetLineCount() const;

    /// Get the text of the paragraph
    virtual wxString GetParagraphText(long paragraphNumber) const;

    /// Convert zero-based line column and paragraph number to a position.
    virtual long XYToPosition(long x, long y) const;

    /// Convert zero-based position to line column and paragraph number
    virtual bool PositionToXY(long pos, long* x, long* y) const;

    /// Set text attributes: character and/or paragraph styles.
    virtual bool SetStyle(const wxRichTextRange& range,
                          const wxRichTextAttr& style,
                          int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO);
//     virtual bool SetStyle(const wxRichTextRange& range,
//                           const wxTextAttrEx& style,
//                           int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO);

    /// Get the conbined text attributes for this position.
//     virtual bool GetStyle(long position, wxTextAttrEx& style);
    virtual bool GetStyle(long position, wxRichTextAttr& style);

    /// Get the content (uncombined) attributes for this position.
//     virtual bool GetUncombinedStyle(long position, wxTextAttrEx& style);
    virtual bool GetUncombinedStyle(long position, wxRichTextAttr& style);

//     /// Implementation helper for GetStyle. If combineStyles is true, combine base, paragraph and
//     /// context attributes.
//     virtual bool DoGetStyle(long position, wxTextAttrEx& style, bool combineStyles = true);

    /// Get the combined style for a range - if any attribute is different within the range,
    /// that attribute is not present within the flags
    virtual bool GetStyleForRange(const wxRichTextRange& range, wxTextAttrEx& style);

    /// Combines 'style' with 'currentStyle' for the purpose of summarising the attributes of a range of
    /// content.
    bool CollectStyle(wxTextAttrEx& currentStyle, const wxTextAttrEx& style, long& multipleStyleAttributes, int& multipleTextEffectAttributes);

    /// Set list style
    //virtual bool SetListStyle(const wxRichTextRange& range, wxRichTextListStyleDefinition* def, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO, int startFrom = 1, int specifiedLevel = -1);
    virtual bool SetListStyle(const wxRichTextRange& range, const wxString& defName,
                              int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
                              int startFrom = 1, int specifiedLevel = -1);

    /// Clear list for given range
    virtual bool ClearListStyle(const wxRichTextRange& range,
                                int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO);

    /// Number/renumber any list elements in the given range.
    /// def/defName can be NULL/empty to indicate that the existing list style should be used.
    //virtual bool NumberList(const wxRichTextRange& range, wxRichTextListStyleDefinition* def = NULL, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO, int startFrom = 1, int specifiedLevel = -1);
    virtual bool NumberList(const wxRichTextRange& range, const wxString& defName,
                            int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
                            int startFrom = 1, int specifiedLevel = -1);

    /// Promote the list items within the given range. promoteBy can be a positive or negative number, e.g. 1 or -1
    /// def/defName can be NULL/empty to indicate that the existing list style should be used.
    //virtual bool PromoteList(int promoteBy, const wxRichTextRange& range, wxRichTextListStyleDefinition* def = NULL, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO, int specifiedLevel = -1);
    virtual bool PromoteList(int promoteBy, const wxRichTextRange& range,
                             const wxString& defName,
                             int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO,
                             int specifiedLevel = -1);

    /// Helper for NumberList and PromoteList, that does renumbering and promotion simultaneously
    /// def/defName can be NULL/empty to indicate that the existing list style should be used.
    virtual bool DoNumberList(const wxRichTextRange& range, const wxRichTextRange& promotionRange, int promoteBy, wxRichTextListStyleDefinition* def, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO, int startFrom = 1, int specifiedLevel = -1);

    /// Fills in the attributes for numbering a paragraph after previousParagraph.
    virtual bool FindNextParagraphNumber(wxRichTextParagraph* previousParagraph, wxRichTextAttr& attr) const;

    /// Test if this whole range has character attributes of the specified kind. If any
    /// of the attributes are different within the range, the test fails. You
    /// can use this to implement, for example, bold button updating. style must have
    /// flags indicating which attributes are of interest.
    //virtual bool HasCharacterAttributes(const wxRichTextRange& range, const wxTextAttrEx& style) const;
    virtual bool HasCharacterAttributes(const wxRichTextRange& range, const wxRichTextAttr& style) const;

    /// Test if this whole range has paragraph attributes of the specified kind. If any
    /// of the attributes are different within the range, the test fails. You
    /// can use this to implement, for example, centering button updating. style must have
    /// flags indicating which attributes are of interest.
    //virtual bool HasParagraphAttributes(const wxRichTextRange& range, const wxTextAttrEx& style) const;
    virtual bool HasParagraphAttributes(const wxRichTextRange& range, const wxRichTextAttr& style) const;

    /// Clone
    virtual wxRichTextObject* Clone() const { return new wxRichTextParagraphLayoutBox(*this); }

    /// Insert fragment into this box at the given position. If partialParagraph is true,
    /// it is assumed that the last (or only) paragraph is just a piece of data with no paragraph
    /// marker.
    virtual bool InsertFragment(long position, wxRichTextParagraphLayoutBox& fragment);

    /// Make a copy of the fragment corresponding to the given range, putting it in 'fragment'.
    virtual bool CopyFragment(const wxRichTextRange& range, wxRichTextParagraphLayoutBox& fragment);

    /// Apply the style sheet to the buffer, for example if the styles have changed.
    virtual bool ApplyStyleSheet(wxRichTextStyleSheet* styleSheet);

    /// Copy
    void Copy(const wxRichTextParagraphLayoutBox& obj);

    /// Calculate ranges
    virtual void UpdateRanges();

    /// Get all the text
    virtual wxString GetText() const;

    /// Set default style for new content. Setting it to a default attribute
    /// makes new content take on the 'basic' style.
    virtual bool SetDefaultStyle(const wxTextAttrEx& style);

    /// Get default style
    virtual const wxTextAttrEx& GetDefaultStyle() const;

    /// Set basic (overall) style
    //virtual void SetBasicStyle(const wxTextAttrEx& style);
    virtual void SetBasicStyle(const wxRichTextAttr& style);

    /// Get basic (overall) style
    virtual const wxTextAttrEx& GetBasicStyle() const;

    /// Invalidate the buffer. With no argument, invalidates whole buffer.
    void Invalidate(const wxRichTextRange& invalidRange = wxRICHTEXT_ALL);

    /// Get invalid range, rounding to entire paragraphs if argument is true.
    wxRichTextRange GetInvalidRange(bool wholeParagraphs = false) const;

};





DocStr(wxRichTextLine,
       "This object represents a line in a paragraph, and stores offsets from
the start of the paragraph representing the start and end positions of
the line.", "");

class wxRichTextLine
{
public:
    wxRichTextLine(wxRichTextParagraph* parent);
    virtual ~wxRichTextLine();

    /// Set the range
    void SetRange(const wxRichTextRange& range);

    /// Get the parent paragraph
    wxRichTextParagraph* GetParent();

    /// Get the range
    wxRichTextRange GetRange();

    /// Get the absolute range
    wxRichTextRange GetAbsoluteRange() const;

    /// Get/set the line size as calculated by Layout.
    virtual wxSize GetSize() const;
    virtual void SetSize(const wxSize& sz);

    /// Get/set the object position relative to the parent
    virtual wxPoint GetPosition() const;
    virtual void SetPosition(const wxPoint& pos);

    /// Get the absolute object position
    virtual wxPoint GetAbsolutePosition() const;

    /// Get the rectangle enclosing the line
    virtual wxRect GetRect() const;

    /// Set/get stored descent
    void SetDescent(int descent);
    int GetDescent() const;

// Operations

    /// Initialisation
    void Init(wxRichTextParagraph* parent);

    /// Copy
    void Copy(const wxRichTextLine& obj);

    /// Clone
    virtual wxRichTextLine* Clone() const;
};






DocStr(wxRichTextParagraph,
       "This object represents a single paragraph (or in a straight text
editor, a line).", "");

class wxRichTextParagraph: public wxRichTextBox
{
public:
    %extend {
        wxRichTextParagraph(const wxString& text = wxPyEmptyString,
                            wxRichTextObject* parent = NULL,
                            wxRichTextAttr* paraStyle = NULL,
                            wxRichTextAttr* charStyle = NULL)
        {
            wxTextAttrEx* psAttr = NULL;
            wxTextAttrEx* csAttr = NULL;
            wxTextAttrEx  psAttr_v;
            wxTextAttrEx  csAttr_v;
            if (paraStyle) {
                psAttr_v = *paraStyle;
                psAttr = &psAttr_v;                
            }
            if (charStyle) {
                csAttr_v = *charStyle;
                csAttr = &csAttr_v;                
            }
            return new wxRichTextParagraph(text, parent, psAttr, csAttr);
        }
    }
    virtual ~wxRichTextParagraph();


// Accessors

    /// Get the cached lines
    wxRichTextLineList& GetLines();

// Operations

    /// Copy
    void Copy(const wxRichTextParagraph& obj);

    /// Clone
    virtual wxRichTextObject* Clone() const;

    /// Clear the cached lines
    void ClearLines();

// Implementation

    /// Apply paragraph styles such as centering to the wrapped lines
    virtual void ApplyParagraphStyle(const wxTextAttrEx& attr, const wxRect& rect);

    /// Insert text at the given position
    virtual bool InsertText(long pos, const wxString& text);

    /// Split an object at this position if necessary, and return
    /// the previous object, or NULL if inserting at beginning.
    virtual wxRichTextObject* SplitAt(long pos, wxRichTextObject** previousObject = NULL);

    /// Move content to a list from this point
    virtual void MoveToList(wxRichTextObject* obj, wxList& list);

    /// Add content back from list
    virtual void MoveFromList(wxList& list);

    /// Get the plain text searching from the start or end of the range.
    /// The resulting string may be shorter than the range given.
    bool GetContiguousPlainText(wxString& text, const wxRichTextRange& range, bool fromStart = true);

    /// Find a suitable wrap position. wrapPosition is the last position in the line to the left
    /// of the split.
    bool FindWrapPosition(const wxRichTextRange& range, wxDC& dc, int availableSpace, long& wrapPosition);

    /// Find the object at the given position
    wxRichTextObject* FindObjectAtPosition(long position);

    /// Get the bullet text for this paragraph.
    wxString GetBulletText();

    /// Allocate or reuse a line object
    wxRichTextLine* AllocateLine(int pos);

    /// Clear remaining unused line objects, if any
    bool ClearUnusedLines(int lineCount);

    /// Get combined attributes of the base style, paragraph style and character style. We use this to dynamically
    /// retrieve the actual style.
//    wxTextAttrEx GetCombinedAttributes(const wxTextAttrEx& contentStyle) const;

    /// Get combined attributes of the base style and paragraph style.
    wxTextAttrEx GetCombinedAttributes() const;

    /// Get the first position from pos that has a line break character.
    long GetFirstLineBreakPosition(long pos);

    /// Create default tabstop array
    static void InitDefaultTabs();

    /// Clear default tabstop array
    static void ClearDefaultTabs();

    /// Get default tabstop array
    static const wxArrayInt& GetDefaultTabs() { return sm_defaultTabs; }
};







DocStr(wxRichTextPlainText,
       "This object represents a single piece of text.", "");

class wxRichTextPlainText: public wxRichTextObject
{
public:
    wxRichTextPlainText(const wxString& text = wxEmptyString, wxRichTextObject* parent = NULL, wxTextAttrEx* style = NULL);

    /// Get the first position from pos that has a line break character.
    long GetFirstLineBreakPosition(long pos);

    /// Get the text
    const wxString& GetText() const;

    /// Set the text
    void SetText(const wxString& text);

// Operations

    /// Copy
    void Copy(const wxRichTextPlainText& obj);

    /// Clone
    virtual wxRichTextObject* Clone() const;

};






#if 0
// TODO: we may not even need wrappers for this class.  It looks to me
// like wxRichTextImage might be enough...
DocStr(wxRichTextImageBlock,
       "Stores information about an image, in binary in-memory form.", "");

class wxRichTextImageBlock: public wxObject
{
public:
    wxRichTextImageBlock();
    virtual ~wxRichTextImageBlock();

    void Init();
    void Clear();

    // Load the original image into a memory block.
    // If the image is not a JPEG, we must convert it into a JPEG
    // to conserve space.
    // If it's not a JPEG we can make use of 'image', already scaled, so we don't have to
    // load the image a 2nd time.
    virtual bool MakeImageBlock(const wxString& filename, int imageType, wxImage& image, bool convertToJPEG = true);

    // Make an image block from the wxImage in the given
    // format.
    virtual bool MakeImageBlock(wxImage& image, int imageType, int quality = 80);

    // Write to a file
    bool Write(const wxString& filename);

    // Write data in hex to a stream
    bool WriteHex(wxOutputStream& stream);

    // Read data in hex from a stream
    bool ReadHex(wxInputStream& stream, int length, int imageType);

    // Copy from 'block'
    void Copy(const wxRichTextImageBlock& block);

    // Load a wxImage from the block
    bool Load(wxImage& image);


//// Accessors

    unsigned char* GetData() const;
    size_t GetDataSize() const;
    int GetImageType() const;

    void SetData(unsigned char* image);
    void SetDataSize(size_t size);
    void SetImageType(int imageType);

    bool IsOk() const;

    // Gets the extension for the block's type
    wxString GetExtension() const;

/// Implementation

    // Allocate and read from stream as a block of memory
    static unsigned char* ReadBlock(wxInputStream& stream, size_t size);
    static unsigned char* ReadBlock(const wxString& filename, size_t size);

    // Write memory block to stream
    static bool WriteBlock(wxOutputStream& stream, unsigned char* block, size_t size);

    // Write memory block to file
    static bool WriteBlock(const wxString& filename, unsigned char* block, size_t size);
};
#endif






DocStr(wxRichTextImage,
       "This object represents an image.", "");

class wxRichTextImage: public wxRichTextObject
{
public:
    // TODO: Which of these constructors?
    wxRichTextImage(wxRichTextObject* parent = NULL);
    //wxRichTextImage(const wxImage& image, wxRichTextObject* parent = NULL, wxTextAttrEx* charStyle = NULL);
    //wxRichTextImage(const wxRichTextImageBlock& imageBlock, wxRichTextObject* parent = NULL, wxTextAttrEx* charStyle = NULL);


// Accessors

    /// Get the image
    const wxImage& GetImage() const;

    /// Set the image
    void SetImage(const wxImage& image);

    /// Get the image block containing the raw data
    wxRichTextImageBlock& GetImageBlock();

// Operations

    /// Copy
    void Copy(const wxRichTextImage& obj);

    /// Clone
    virtual wxRichTextObject* Clone() const;

    /// Load wxImage from the block
    virtual bool LoadFromBlock();

    /// Make block from the wxImage
    virtual bool MakeBlock();
};






%typemap(out) wxRichTextFileHandler*  { $result = wxPyMake_wxObject($1, (bool)$owner); }
wxUNTYPED_LIST_WRAPPER(wxRichTextFileHandlerList, wxRichTextFileHandler);

DocStr(wxRichTextBuffer,
       "This is a kind of box, used to represent the whole buffer.", "");

class wxRichTextCommand;
class wxRichTextAction;

class wxRichTextBuffer: public wxRichTextParagraphLayoutBox
{
public:
    wxRichTextBuffer();
    virtual ~wxRichTextBuffer() ;

// Accessors

    /// Gets the command processor
    wxCommandProcessor* GetCommandProcessor() const;

    /// Set style sheet, if any.
    void SetStyleSheet(wxRichTextStyleSheet* styleSheet);
    virtual wxRichTextStyleSheet* GetStyleSheet() const;

    /// Set style sheet and notify of the change
    bool SetStyleSheetAndNotify(wxRichTextStyleSheet* sheet);

    /// Push style sheet to top of stack
    bool PushStyleSheet(wxRichTextStyleSheet* styleSheet);

    /// Pop style sheet from top of stack
    wxRichTextStyleSheet* PopStyleSheet();

// Operations

    /// Initialisation
    void Init();

    /// Clears the buffer, adds an empty paragraph, and clears the command processor.
    virtual void ResetAndClearCommands();

    /// Load a file
    virtual bool LoadFile(const wxString& filename, int type = wxRICHTEXT_TYPE_ANY);

    /// Save a file
    virtual bool SaveFile(const wxString& filename, int type = wxRICHTEXT_TYPE_ANY);

    /// Load from a stream
    %Rename(LoadStream,
            virtual bool , LoadFile(wxInputStream& stream, int type = wxRICHTEXT_TYPE_ANY));

    /// Save to a stream
    %Rename(SaveStream,
            virtual bool , SaveFile(wxOutputStream& stream, int type = wxRICHTEXT_TYPE_ANY));

    /// Set the handler flags, controlling loading and saving
    void SetHandlerFlags(int flags) { m_handlerFlags = flags; }

    /// Get the handler flags, controlling loading and saving
    int GetHandlerFlags() const { return m_handlerFlags; }

    /// Convenience function to add a paragraph of text
    virtual wxRichTextRange AddParagraph(const wxString& text, wxTextAttrEx* paraStyle = NULL) { Modify(); return wxRichTextParagraphLayoutBox::AddParagraph(text, paraStyle); }

    /// Begin collapsing undo/redo commands. Note that this may not work properly
    /// if combining commands that delete or insert content, changing ranges for
    /// subsequent actions.
    virtual bool BeginBatchUndo(const wxString& cmdName);

    /// End collapsing undo/redo commands
    virtual bool EndBatchUndo();

    /// Collapsing commands?
    virtual bool BatchingUndo() const;

    /// Submit immediately, or delay according to whether collapsing is on
    virtual bool SubmitAction(wxRichTextAction* action);

    /// Get collapsed command
    virtual wxRichTextCommand* GetBatchedCommand() const;

    /// Begin suppressing undo/redo commands. The way undo is suppressed may be implemented
    /// differently by each command. If not dealt with by a command implementation, then
    /// it will be implemented automatically by not storing the command in the undo history
    /// when the action is submitted to the command processor.
    virtual bool BeginSuppressUndo();

    /// End suppressing undo/redo commands.
    virtual bool EndSuppressUndo();

    /// Collapsing commands?
    virtual bool SuppressingUndo() const;

    /// Copy the range to the clipboard
    virtual bool CopyToClipboard(const wxRichTextRange& range);

    /// Paste the clipboard content to the buffer
    virtual bool PasteFromClipboard(long position);

    /// Can we paste from the clipboard?
    virtual bool CanPasteFromClipboard() const;

    /// Begin using a style
    virtual bool BeginStyle(const wxTextAttrEx& style);

    /// End the style
    virtual bool EndStyle();

    /// End all styles
    virtual bool EndAllStyles();

    /// Clear the style stack
    virtual void ClearStyleStack();

    /// Get the size of the style stack, for example to check correct nesting
    virtual size_t GetStyleStackSize() const;

    /// Begin using bold
    bool BeginBold();

    /// End using bold
    bool EndBold();

    /// Begin using italic
    bool BeginItalic();

    /// End using italic
    bool EndItalic();

    /// Begin using underline
    bool BeginUnderline();

    /// End using underline
    bool EndUnderline();

    /// Begin using point size
    bool BeginFontSize(int pointSize);

    /// End using point size
    bool EndFontSize();

    /// Begin using this font
    bool BeginFont(const wxFont& font);

    /// End using a font
    bool EndFont();

    /// Begin using this colour
    bool BeginTextColour(const wxColour& colour);

    /// End using a colour
    bool EndTextColour();

    /// Begin using alignment
    bool BeginAlignment(wxTextAttrAlignment alignment);

    /// End alignment
    bool EndAlignment();

    /// Begin left indent
    bool BeginLeftIndent(int leftIndent, int leftSubIndent = 0);

    /// End left indent
    bool EndLeftIndent();

    /// Begin right indent
    bool BeginRightIndent(int rightIndent);

    /// End right indent
    bool EndRightIndent();

    /// Begin paragraph spacing
    bool BeginParagraphSpacing(int before, int after);

    /// End paragraph spacing
    bool EndParagraphSpacing();

    /// Begin line spacing
    bool BeginLineSpacing(int lineSpacing);

    /// End line spacing
    bool EndLineSpacing();

    /// Begin numbered bullet
    bool BeginNumberedBullet(int bulletNumber, int leftIndent, int leftSubIndent, int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD);

    /// End numbered bullet
    bool EndNumberedBullet();

    /// Begin symbol bullet
    bool BeginSymbolBullet(const wxString& symbol, int leftIndent, int leftSubIndent, int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_SYMBOL);

    /// End symbol bullet
    bool EndSymbolBullet();

    /// Begin standard bullet
    bool BeginStandardBullet(const wxString& bulletName, int leftIndent, int leftSubIndent, int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_STANDARD);

    /// End standard bullet
    bool EndStandardBullet();

    /// Begin named character style
    bool BeginCharacterStyle(const wxString& characterStyle);

    /// End named character style
    bool EndCharacterStyle();

    /// Begin named paragraph style
    bool BeginParagraphStyle(const wxString& paragraphStyle);

    /// End named character style
    bool EndParagraphStyle();

    /// Begin named list style
    bool BeginListStyle(const wxString& listStyle, int level = 1, int number = 1);

    /// End named character style
    bool EndListStyle();

    /// Begin URL
    bool BeginURL(const wxString& url, const wxString& characterStyle = wxEmptyString);

    /// End URL
    bool EndURL();

// Event handling

    /// Add an event handler
    bool AddEventHandler(wxEvtHandler* handler);

    /// Remove an event handler
    bool RemoveEventHandler(wxEvtHandler* handler, bool deleteHandler = false);

    /// Clear event handlers
    void ClearEventHandlers();

    /// Send event to event handlers. If sendToAll is true, will send to all event handlers,
    /// otherwise will stop at the first successful one.
    bool SendEvent(wxEvent& event, bool sendToAll = true);

// Implementation

    /// Copy
    void Copy(const wxRichTextBuffer& obj);

    /// Clone
    virtual wxRichTextObject* Clone() const;

    /// Submit command to insert paragraphs
    bool InsertParagraphsWithUndo(long pos, const wxRichTextParagraphLayoutBox& paragraphs, wxRichTextCtrl* ctrl, int flags = 0);

    /// Submit command to insert the given text
    bool InsertTextWithUndo(long pos, const wxString& text, wxRichTextCtrl* ctrl, int flags = 0);

    /// Submit command to insert a newline
    bool InsertNewlineWithUndo(long pos, wxRichTextCtrl* ctrl, int flags = 0);

    /// Submit command to insert the given image
    bool InsertImageWithUndo(long pos, const wxRichTextImageBlock& imageBlock, wxRichTextCtrl* ctrl, int flags = 0);

    /// Submit command to delete this range
    bool DeleteRangeWithUndo(const wxRichTextRange& range, wxRichTextCtrl* ctrl);

    /// Mark modified
    void Modify(bool modify = true);
    bool IsModified() const;

    /// Get the style that is appropriate for a new paragraph at this position.
    /// If the previous paragraph has a paragraph style name, look up the next-paragraph
    /// style.
    wxRichTextAttr GetStyleForNewParagraph(long pos, bool caretPosition = false, bool lookUpNewParaStyle=false) const;


    /// Returns the file handlers
    static wxRichTextFileHandlerList_t& GetHandlers();

    %disownarg(wxRichTextFileHandler *handler);
    
    /// Adds a handler to the end
    static void AddHandler(wxRichTextFileHandler *handler);

    /// Inserts a handler at the front
    static void InsertHandler(wxRichTextFileHandler *handler);

    %cleardisown(wxRichTextFileHandler *handler);
    
    
    /// Removes a handler
    static bool RemoveHandler(const wxString& name);

    /// Finds a handler by name
    %Rename(FindHandlerByName,
            static wxRichTextFileHandler* , FindHandler(const wxString& name));

    /// Finds a handler by extension and type
    %Rename(FindHandlerByExtension,
            static wxRichTextFileHandler*, FindHandler(const wxString& extension, int imageType));

    /// Finds a handler by filename or, if supplied, type
    %Rename(FindHandlerByFilename,
            static wxRichTextFileHandler* , FindHandlerFilenameOrType(const wxString& filename, int imageType));

    /// Finds a handler by type
    %Rename(FindHandlerByType,
            static wxRichTextFileHandler* , FindHandler(int imageType));


    // TODO: Handle returning the types array?
    
    /// Gets a wildcard incorporating all visible handlers. If 'types' is present,
    /// will be filled with the file type corresponding to each filter. This can be
    /// used to determine the type to pass to LoadFile given a selected filter.
    //static wxString GetExtWildcard(bool combine = false, bool save = false,
    //                               wxArrayInt* types = NULL);
    %extend {
        KeepGIL(GetExtWildcard);
        DocAStr(GetExtWildcard,
                "GetExtWildcard(self, bool combine=False, bool save=False) --> (wildcards, types)",
                "Gets a wildcard string for the file dialog based on all the currently
loaded richtext file handlers, and a list that can be used to map
those filter types to the file handler type.", "");
        static PyObject* GetExtWildcard(bool combine = false, bool save = false) {
            wxString wildcards;
            wxArrayInt types;
            wildcards = wxRichTextBuffer::GetExtWildcard(combine, save, &types);
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, wx2PyString(wildcards));
            PyTuple_SET_ITEM(tup, 1, wxArrayInt2PyList_helper(types));
            return tup;
        }
    }

    /// Clean up handlers
    static void CleanUpHandlers();

    /// Initialise the standard handlers
    static void InitStandardHandlers();

    /// Get renderer
    static wxRichTextRenderer* GetRenderer();

    /// Set renderer, deleting old one
    static void SetRenderer(wxRichTextRenderer* renderer);

    /// Minimum margin between bullet and paragraph in 10ths of a mm
    static int GetBulletRightMargin();
    static void SetBulletRightMargin(int margin);

    /// Factor to multiply by character height to get a reasonable bullet size
    static float GetBulletProportion();
    static void SetBulletProportion(float prop);

    /// Scale factor for calculating dimensions
    double GetScale() const;
    void SetScale(double scale);
};






// TODO:  Do we need wrappers for the command processor, undo/redo, etc.?
//
// enum wxRichTextCommandId
// {
//     wxRICHTEXT_INSERT,
//     wxRICHTEXT_DELETE,
//     wxRICHTEXT_CHANGE_STYLE
// };
// class WXDLLIMPEXP_RICHTEXT wxRichTextCommand: public wxCommand
// class WXDLLIMPEXP_RICHTEXT wxRichTextAction: public wxObject




//---------------------------------------------------------------------------
%newgroup


/*!
 * Handler flags
 */

enum {
    // Include style sheet when loading and saving
    wxRICHTEXT_HANDLER_INCLUDE_STYLESHEET,

    // Save images to memory file system in HTML handler
    wxRICHTEXT_HANDLER_SAVE_IMAGES_TO_MEMORY,

    // Save images to files in HTML handler
    wxRICHTEXT_HANDLER_SAVE_IMAGES_TO_FILES,

    // Save images as inline base64 data in HTML handler
    wxRICHTEXT_HANDLER_SAVE_IMAGES_TO_BASE64,

    // Don't write header and footer (or BODY), so we can include the
    // fragment in a larger document
    wxRICHTEXT_HANDLER_NO_HEADER_FOOTER,
};




DocStr(wxRichTextFileHandler,
       "Base class for file handlers", "");

class wxRichTextFileHandler: public wxObject
{
public:
    //wxRichTextFileHandler(const wxString& name = wxEmptyString,   ****  This is an ABC
    //                      const wxString& ext = wxEmptyString,
    //                      int type = 0);

    ~wxRichTextFileHandler();
    
    %Rename(LoadStream,
            bool, LoadFile(wxRichTextBuffer *buffer, wxInputStream& stream));
    %Rename(SaveStream,
            bool,  SaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream));
    
    bool LoadFile(wxRichTextBuffer *buffer, const wxString& filename);
    bool SaveFile(wxRichTextBuffer *buffer, const wxString& filename);

    /// Can we handle this filename (if using files)? By default, checks the extension.
    virtual bool CanHandle(const wxString& filename) const;

    /// Can we save using this handler?
    virtual bool CanSave() const;

    /// Can we load using this handler?
    virtual bool CanLoad() const;

    /// Should this handler be visible to the user?
    virtual bool IsVisible() const;
    virtual void SetVisible(bool visible);

    /// The name of the nandler
    void SetName(const wxString& name);
    wxString GetName() const;
    %property(Name, GetName, SetName)
    
    /// The default extension to recognise
    void SetExtension(const wxString& ext);
    wxString GetExtension() const;
    %property(Extension, GetExtension, SetExtension)

    /// The handler type
    void SetType(int type);
    int GetType() const;
    %property(Type, GetType, SetType)

    /// Flags controlling how loading and saving is done
    void SetFlags(int flags);
    int GetFlags() const;
    %property(Flags, GetFlags, SetFlags)

    /// Encoding to use when saving a file. If empty, a suitable encoding is chosen
    void SetEncoding(const wxString& encoding);
    const wxString& GetEncoding() const;
    %property(Encoding, GetEncoding, SetEncoding)   
};



MAKE_CONST_WXSTRING2(TextName, wxT("Text"));
MAKE_CONST_WXSTRING2(TextExt,  wxT("txt"));

class wxRichTextPlainTextHandler: public wxRichTextFileHandler
{
public:
    wxRichTextPlainTextHandler(const wxString& name = wxPyTextName,
                               const wxString& ext =  wxPyTextExt,
                               int type = wxRICHTEXT_TYPE_TEXT);
};





//---------------------------------------------------------------------------
%newgroup


// TODO: Make a PyRichTextRenderer class


/*!
 * wxRichTextRenderer isolates common drawing functionality
 */

class wxRichTextRenderer: public wxObject
{
public:
    wxRichTextRenderer() {}
    virtual ~wxRichTextRenderer() {}

    /// Draw a standard bullet, as specified by the value of GetBulletName
    virtual bool DrawStandardBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxTextAttrEx& attr, const wxRect& rect) = 0;

    /// Draw a bullet that can be described by text, such as numbered or symbol bullets
    virtual bool DrawTextBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxTextAttrEx& attr, const wxRect& rect, const wxString& text) = 0;

    /// Draw a bitmap bullet, where the bullet bitmap is specified by the value of GetBulletName
    virtual bool DrawBitmapBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxTextAttrEx& attr, const wxRect& rect) = 0;

    /// Enumerate the standard bullet names currently supported
    virtual bool EnumerateStandardBulletNames(wxArrayString& bulletNames) = 0;
};

/*!
 * wxRichTextStdRenderer: standard renderer
 */

class wxRichTextStdRenderer: public wxRichTextRenderer
{
public:
    wxRichTextStdRenderer() {}

    /// Draw a standard bullet, as specified by the value of GetBulletName
    virtual bool DrawStandardBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxTextAttrEx& attr, const wxRect& rect);

    /// Draw a bullet that can be described by text, such as numbered or symbol bullets
    virtual bool DrawTextBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxTextAttrEx& attr, const wxRect& rect, const wxString& text);

    /// Draw a bitmap bullet, where the bullet bitmap is specified by the value of GetBulletName
    virtual bool DrawBitmapBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxTextAttrEx& attr, const wxRect& rect);

    /// Enumerate the standard bullet names currently supported
    virtual bool EnumerateStandardBulletNames(wxArrayString& bulletNames);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
