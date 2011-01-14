/////////////////////////////////////////////////////////////////////////////
// Name:        wx/richtext/richtextbuffer.h
// Purpose:     Buffer for wxRichTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     2005-09-30
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RICHTEXTBUFFER_H_
#define _WX_RICHTEXTBUFFER_H_

/*

  Data structures
  ===============

  Data is represented by a hierarchy of objects, all derived from
  wxRichTextObject.

  The top of the hierarchy is the buffer, a kind of wxRichTextParagraphLayoutBox.
  These boxes will allow flexible placement of text boxes on a page, but
  for now there is a single box representing the document, and this box is
  a wxRichTextParagraphLayoutBox which contains further wxRichTextParagraph
  objects, each of which can include text and images.

  Each object maintains a range (start and end position) measured
  from the start of the main parent box.
  A paragraph object knows its range, and a text fragment knows its range
  too. So, a character or image in a page has a position relative to the
  start of the document, and a character in an embedded text box has
  a position relative to that text box. For now, we will not be dealing with
  embedded objects but it's something to bear in mind for later.

  Note that internally, a range (5,5) represents a range of one character.
  In the public wx[Rich]TextCtrl API, this would be passed to e.g. SetSelection
  as (5,6). A paragraph with one character might have an internal range of (0, 1)
  since the end of the paragraph takes up one position.

  Layout
  ======

  When Layout is called on an object, it is given a size which the object
  must limit itself to, or one or more flexible directions (vertical
  or horizontal). So for example a centered paragraph is given the page
  width to play with (minus any margins), but can extend indefinitely
  in the vertical direction. The implementation of Layout can then
  cache the calculated size and position within the parent.

 */

/*!
 * Includes
 */

#include "wx/defs.h"

#if wxUSE_RICHTEXT

#include "wx/list.h"
#include "wx/textctrl.h"
#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/cmdproc.h"
#include "wx/txtstrm.h"
#include "wx/variant.h"

#if wxUSE_DATAOBJ
#include "wx/dataobj.h"
#endif

// Compatibility
//#define wxRichTextAttr wxTextAttr
#define wxTextAttrEx wxTextAttr

// Setting wxRICHTEXT_USE_OWN_CARET to 1 implements a
// caret reliably without using wxClientDC in case there
// are platform-specific problems with the generic caret.
#if defined(__WXGTK__) || defined(__WXMAC__)
#define wxRICHTEXT_USE_OWN_CARET 1
#else
#define wxRICHTEXT_USE_OWN_CARET 0
#endif

// Switch off for binary compatibility, on for faster drawing
// Note: this seems to be buggy (overzealous use of extents) so
// don't use for now
#define wxRICHTEXT_USE_OPTIMIZED_LINE_DRAWING 0

// The following two symbols determine whether an output implementation
// is present. To switch the relevant one on, set wxRICHTEXT_USE_XMLDOCUMENT_OUTPUT in
// richtextxml.cpp. By default, the faster direct output implementation is used.

// Include the wxXmlDocument implementation for output
#define wxRICHTEXT_HAVE_XMLDOCUMENT_OUTPUT 1

// Include the faster, direct implementation for output
#define wxRICHTEXT_HAVE_DIRECT_OUTPUT 1

/*
 * Special characters
 */

extern WXDLLIMPEXP_RICHTEXT const wxChar wxRichTextLineBreakChar;

/*!
 * File types in wxRichText context.
 */
enum wxRichTextFileType
{
    wxRICHTEXT_TYPE_ANY = 0,
    wxRICHTEXT_TYPE_TEXT,
    wxRICHTEXT_TYPE_XML,
    wxRICHTEXT_TYPE_HTML,
    wxRICHTEXT_TYPE_RTF,
    wxRICHTEXT_TYPE_PDF
};

/*
 * Forward declarations
 */

class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextCtrl;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextObject;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextImage;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextCacheObject;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextObjectList;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextLine;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextParagraph;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextFileHandler;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextStyleSheet;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextListStyleDefinition;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextEvent;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextRenderer;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextBuffer;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextXMLHandler;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextParagraphLayoutBox;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextImageBlock;
class WXDLLIMPEXP_FWD_XML      wxXmlNode;
class                          wxRichTextFloatCollector;

/*!
 * Flags determining the available space, passed to Layout
 */

#define wxRICHTEXT_FIXED_WIDTH      0x01
#define wxRICHTEXT_FIXED_HEIGHT     0x02
#define wxRICHTEXT_VARIABLE_WIDTH   0x04
#define wxRICHTEXT_VARIABLE_HEIGHT  0x08

// Only lay out the part of the buffer that lies within
// the rect passed to Layout.
#define wxRICHTEXT_LAYOUT_SPECIFIED_RECT 0x10

/*
 * Flags to pass to Draw
 */

// Ignore paragraph cache optimization, e.g. for printing purposes
// where one line may be drawn higher (on the next page) compared
// with the previous line
#define wxRICHTEXT_DRAW_IGNORE_CACHE    0x01
#define wxRICHTEXT_DRAW_SELECTED        0x02
#define wxRICHTEXT_DRAW_PRINT           0x04
#define wxRICHTEXT_DRAW_GUIDELINES      0x08

/*
 * Flags returned from hit-testing, or passed to hit-test function.
 */
enum wxRichTextHitTestFlags
{
    // The point was not on this object
    wxRICHTEXT_HITTEST_NONE =    0x01,

    // The point was before the position returned from HitTest
    wxRICHTEXT_HITTEST_BEFORE =  0x02,

    // The point was after the position returned from HitTest
    wxRICHTEXT_HITTEST_AFTER =   0x04,

    // The point was on the position returned from HitTest
    wxRICHTEXT_HITTEST_ON =      0x08,

    // The point was on space outside content
    wxRICHTEXT_HITTEST_OUTSIDE = 0x10,

    // Only do hit-testing at the current level (don't traverse into top-level objects)
    wxRICHTEXT_HITTEST_NO_NESTED_OBJECTS = 0x20
};

/*!
 * Flags for GetRangeSize
 */

#define wxRICHTEXT_FORMATTED        0x01
#define wxRICHTEXT_UNFORMATTED      0x02
#define wxRICHTEXT_CACHE_SIZE       0x04
#define wxRICHTEXT_HEIGHT_ONLY      0x08

/*!
 * Flags for SetStyle/SetListStyle
 */

#define wxRICHTEXT_SETSTYLE_NONE            0x00

// Specifies that this operation should be undoable
#define wxRICHTEXT_SETSTYLE_WITH_UNDO       0x01

// Specifies that the style should not be applied if the
// combined style at this point is already the style in question.
#define wxRICHTEXT_SETSTYLE_OPTIMIZE        0x02

// Specifies that the style should only be applied to paragraphs,
// and not the content. This allows content styling to be
// preserved independently from that of e.g. a named paragraph style.
#define wxRICHTEXT_SETSTYLE_PARAGRAPHS_ONLY 0x04

// Specifies that the style should only be applied to characters,
// and not the paragraph. This allows content styling to be
// preserved independently from that of e.g. a named paragraph style.
#define wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY 0x08

// For SetListStyle only: specifies starting from the given number, otherwise
// deduces number from existing attributes
#define wxRICHTEXT_SETSTYLE_RENUMBER        0x10

// For SetListStyle only: specifies the list level for all paragraphs, otherwise
// the current indentation will be used
#define wxRICHTEXT_SETSTYLE_SPECIFY_LEVEL   0x20

// Resets the existing style before applying the new style
#define wxRICHTEXT_SETSTYLE_RESET           0x40

// Removes the given style instead of applying it
#define wxRICHTEXT_SETSTYLE_REMOVE          0x80

/*!
 * Flags for object insertion
 */

#define wxRICHTEXT_INSERT_NONE                              0x00
#define wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE     0x01
#define wxRICHTEXT_INSERT_INTERACTIVE                       0x02

// A special flag telling the buffer to keep the first paragraph style
// as-is, when deleting a paragraph marker. In future we might pass a
// flag to InsertFragment and DeleteRange to indicate the appropriate mode.
#define wxTEXT_ATTR_KEEP_FIRST_PARA_STYLE   0x10000000

/*!
 * Default superscript/subscript font multiplication factor
 */

#define wxSCRIPT_MUL_FACTOR             1.5

typedef unsigned short wxTextAttrDimensionFlags;

// Miscellaneous text box flags
enum wxTextBoxAttrFlags
{
    wxTEXT_BOX_ATTR_FLOAT                   = 0x00000001,
    wxTEXT_BOX_ATTR_CLEAR                   = 0x00000002,
    wxTEXT_BOX_ATTR_COLLAPSE_BORDERS        = 0x00000004,
    wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT      = 0x00000004
};

// Whether a value is present, used in dimension flags
enum wxTextAttrValueFlags
{
    wxTEXT_ATTR_VALUE_VALID               = 0x1000,
    wxTEXT_ATTR_VALUE_VALID_MASK          = 0x1000
};

// Units - included in the dimension value
enum wxTextAttrUnits
{
    wxTEXT_ATTR_UNITS_TENTHS_MM             = 0x0001,
    wxTEXT_ATTR_UNITS_PIXELS                = 0x0002,
    wxTEXT_ATTR_UNITS_PERCENTAGE            = 0x0004,
    wxTEXT_ATTR_UNITS_POINTS                = 0x0008,

    wxTEXT_ATTR_UNITS_MASK                  = 0x000F
};

// Position - included in the dimension flags
enum wxTextBoxAttrPosition
{
    wxTEXT_BOX_ATTR_POSITION_STATIC         = 0x0000, // Default is static, i.e. as per normal layout
    wxTEXT_BOX_ATTR_POSITION_RELATIVE       = 0x0010, // Relative to the relevant edge
    wxTEXT_BOX_ATTR_POSITION_ABSOLUTE       = 0x0020,

    wxTEXT_BOX_ATTR_POSITION_MASK           = 0x00F0
};

// Dimension, including units and position
class WXDLLIMPEXP_RICHTEXT wxTextAttrDimension
{
public:
    wxTextAttrDimension() { Reset(); }
    wxTextAttrDimension(int value, wxTextAttrUnits units = wxTEXT_ATTR_UNITS_TENTHS_MM) { m_value = value; m_flags = units|wxTEXT_ATTR_VALUE_VALID; }

    void Reset() { m_value = 0; m_flags = 0; }

    // Partial equality test
    bool EqPartial(const wxTextAttrDimension& dim) const;

    // Apply
    bool Apply(const wxTextAttrDimension& dim, const wxTextAttrDimension* compareWith = NULL);

    // Collects the attributes that are common to a range of content, building up a note of
    // which attributes are absent in some objects and which clash in some objects.
    void CollectCommonAttributes(const wxTextAttrDimension& attr, wxTextAttrDimension& clashingAttr, wxTextAttrDimension& absentAttr);

    bool operator==(const wxTextAttrDimension& dim) const { return m_value == dim.m_value && m_flags == dim.m_flags; }

    int GetValue() const { return m_value; }
    float GetValueMM() const { return float(m_value) / 10.0; }
    void SetValueMM(float value) { m_value = (int) ((value * 10.0) + 0.5); m_flags |= wxTEXT_ATTR_VALUE_VALID; }
    void SetValue(int value) { m_value = value; m_flags |= wxTEXT_ATTR_VALUE_VALID; }
    void SetValue(int value, wxTextAttrDimensionFlags flags) { SetValue(value); m_flags = flags; }
    void SetValue(int value, wxTextAttrUnits units) { m_value = value; m_flags = units | wxTEXT_ATTR_VALUE_VALID; }
    void SetValue(const wxTextAttrDimension& dim) { (*this) = dim; }

    wxTextAttrUnits GetUnits() const { return (wxTextAttrUnits) (m_flags & wxTEXT_ATTR_UNITS_MASK); }
    void SetUnits(wxTextAttrUnits units) { m_flags &= ~wxTEXT_ATTR_UNITS_MASK; m_flags |= units; }

    wxTextBoxAttrPosition GetPosition() const { return (wxTextBoxAttrPosition) (m_flags & wxTEXT_BOX_ATTR_POSITION_MASK); }
    void SetPosition(wxTextBoxAttrPosition pos) { m_flags &= ~wxTEXT_BOX_ATTR_POSITION_MASK; m_flags |= pos; }

    bool IsValid() const { return (m_flags & wxTEXT_ATTR_VALUE_VALID) != 0; }
    void SetValid(bool b) { m_flags &= ~wxTEXT_ATTR_VALUE_VALID_MASK; m_flags |= (b ? wxTEXT_ATTR_VALUE_VALID : 0); }

    wxTextAttrDimensionFlags GetFlags() const { return m_flags; }
    void SetFlags(wxTextAttrDimensionFlags flags) { m_flags = flags; }

    int                         m_value;
    wxTextAttrDimensionFlags    m_flags;
};

// A class for left, right, top and bottom dimensions
class WXDLLIMPEXP_RICHTEXT wxTextAttrDimensions
{
public:
    wxTextAttrDimensions() {}

    void Reset() { m_left.Reset(); m_top.Reset(); m_right.Reset(); m_bottom.Reset(); }

    bool operator==(const wxTextAttrDimensions& dims) const { return m_left == dims.m_left && m_top == dims.m_top && m_right == dims.m_right && m_bottom == dims.m_bottom; }

    // Partial equality test
    bool EqPartial(const wxTextAttrDimensions& dims) const;

    // Apply border to 'this', but not if the same as compareWith
    bool Apply(const wxTextAttrDimensions& dims, const wxTextAttrDimensions* compareWith = NULL);

    // Collects the attributes that are common to a range of content, building up a note of
    // which attributes are absent in some objects and which clash in some objects.
    void CollectCommonAttributes(const wxTextAttrDimensions& attr, wxTextAttrDimensions& clashingAttr, wxTextAttrDimensions& absentAttr);

    // Remove specified attributes from this object
    bool RemoveStyle(const wxTextAttrDimensions& attr);

    const wxTextAttrDimension& GetLeft() const { return m_left; }
    wxTextAttrDimension& GetLeft() { return m_left; }

    const wxTextAttrDimension& GetRight() const { return m_right; }
    wxTextAttrDimension& GetRight() { return m_right; }

    const wxTextAttrDimension& GetTop() const { return m_top; }
    wxTextAttrDimension& GetTop() { return m_top; }

    const wxTextAttrDimension& GetBottom() const { return m_bottom; }
    wxTextAttrDimension& GetBottom() { return m_bottom; }

    wxTextAttrDimension         m_left;
    wxTextAttrDimension         m_top;
    wxTextAttrDimension         m_right;
    wxTextAttrDimension         m_bottom;
};

// A class for width and height
class WXDLLIMPEXP_RICHTEXT wxTextAttrSize
{
public:
    wxTextAttrSize() {}

    void Reset() { m_width.Reset(); m_height.Reset(); }

    bool operator==(const wxTextAttrSize& size) const { return m_width == size.m_width && m_height == size.m_height ; }

    // Partial equality test
    bool EqPartial(const wxTextAttrSize& dims) const;

    // Apply border to 'this', but not if the same as compareWith
    bool Apply(const wxTextAttrSize& dims, const wxTextAttrSize* compareWith = NULL);

    // Collects the attributes that are common to a range of content, building up a note of
    // which attributes are absent in some objects and which clash in some objects.
    void CollectCommonAttributes(const wxTextAttrSize& attr, wxTextAttrSize& clashingAttr, wxTextAttrSize& absentAttr);

    // Remove specified attributes from this object
    bool RemoveStyle(const wxTextAttrSize& attr);

    // Width and height

    wxTextAttrDimension& GetWidth() { return m_width; }
    const wxTextAttrDimension& GetWidth() const { return m_width; }

    void SetWidth(int value, wxTextAttrDimensionFlags flags) { m_width.SetValue(value, flags); }
    void SetWidth(int value, wxTextAttrUnits units) { m_width.SetValue(value, units); }
    void SetWidth(const wxTextAttrDimension& dim) { m_width.SetValue(dim); }

    wxTextAttrDimension& GetHeight() { return m_height; }
    const wxTextAttrDimension& GetHeight() const { return m_height; }

    void SetHeight(int value, wxTextAttrDimensionFlags flags) { m_height.SetValue(value, flags); }
    void SetHeight(int value, wxTextAttrUnits units) { m_height.SetValue(value, units); }
    void SetHeight(const wxTextAttrDimension& dim) { m_height.SetValue(dim); }

    wxTextAttrDimension         m_width;
    wxTextAttrDimension         m_height;
};

// A class to make it easier to convert dimensions
class WXDLLIMPEXP_RICHTEXT wxTextAttrDimensionConverter
{
public:
    wxTextAttrDimensionConverter(wxDC& dc, double scale = 1.0, const wxSize& parentSize = wxDefaultSize);
    wxTextAttrDimensionConverter(int ppi, double scale = 1.0, const wxSize& parentSize = wxDefaultSize);

    int GetPixels(const wxTextAttrDimension& dim, int direction = wxHORIZONTAL) const;
    int GetTenthsMM(const wxTextAttrDimension& dim) const;

    int ConvertTenthsMMToPixels(int units) const;
    int ConvertPixelsToTenthsMM(int pixels) const;

    int     m_ppi;
    double  m_scale;
    wxSize  m_parentSize;
};

// Border styles
enum wxTextAttrBorderStyle
{
    wxTEXT_BOX_ATTR_BORDER_NONE             = 0,
    wxTEXT_BOX_ATTR_BORDER_SOLID            = 1,
    wxTEXT_BOX_ATTR_BORDER_DOTTED           = 2,
    wxTEXT_BOX_ATTR_BORDER_DASHED           = 3,
    wxTEXT_BOX_ATTR_BORDER_DOUBLE           = 4,
    wxTEXT_BOX_ATTR_BORDER_GROOVE           = 5,
    wxTEXT_BOX_ATTR_BORDER_RIDGE            = 6,
    wxTEXT_BOX_ATTR_BORDER_INSET            = 7,
    wxTEXT_BOX_ATTR_BORDER_OUTSET           = 8
};

// Border style presence flags
enum wxTextAttrBorderFlags
{
    wxTEXT_BOX_ATTR_BORDER_STYLE            = 0x0001,
    wxTEXT_BOX_ATTR_BORDER_COLOUR           = 0x0002
};

// Border width symbols for qualitative widths
enum wxTextAttrBorderWidth
{
    wxTEXT_BOX_ATTR_BORDER_THIN             = -1,
    wxTEXT_BOX_ATTR_BORDER_MEDIUM           = -2,
    wxTEXT_BOX_ATTR_BORDER_THICK            = -3
};

// Float styles
enum wxTextBoxAttrFloatStyle
{
    wxTEXT_BOX_ATTR_FLOAT_NONE              = 0,
    wxTEXT_BOX_ATTR_FLOAT_LEFT              = 1,
    wxTEXT_BOX_ATTR_FLOAT_RIGHT             = 2
};

// Clear styles
enum wxTextBoxAttrClearStyle
{
    wxTEXT_BOX_ATTR_CLEAR_NONE              = 0,
    wxTEXT_BOX_ATTR_CLEAR_LEFT              = 1,
    wxTEXT_BOX_ATTR_CLEAR_RIGHT             = 2,
    wxTEXT_BOX_ATTR_CLEAR_BOTH              = 3
};

// Collapse mode styles. TODO: can they be switched on per side?
enum wxTextBoxAttrCollapseMode
{
    wxTEXT_BOX_ATTR_COLLAPSE_NONE           = 0,
    wxTEXT_BOX_ATTR_COLLAPSE_FULL           = 1
};

// Vertical alignment values
enum wxTextBoxAttrVerticalAlignment
{
    wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_NONE =       0,
    wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_TOP  =       1,
    wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_CENTRE =     2,
    wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_BOTTOM  =    3
};

// Border
class WXDLLIMPEXP_RICHTEXT wxTextAttrBorder
{
public:
    wxTextAttrBorder() { Reset(); }

    bool operator==(const wxTextAttrBorder& border) const
    {
        return m_flags == border.m_flags && m_borderStyle == border.m_borderStyle &&
               m_borderColour == border.m_borderColour && m_borderWidth == border.m_borderWidth;
    }

    void Reset() { m_borderStyle = 0; m_borderColour = 0; m_flags = 0; m_borderWidth.Reset(); }

    // Partial equality test
    bool EqPartial(const wxTextAttrBorder& border) const;

    // Apply border to 'this', but not if the same as compareWith
    bool Apply(const wxTextAttrBorder& border, const wxTextAttrBorder* compareWith = NULL);

    // Remove specified attributes from this object
    bool RemoveStyle(const wxTextAttrBorder& attr);

    // Collects the attributes that are common to a range of content, building up a note of
    // which attributes are absent in some objects and which clash in some objects.
    void CollectCommonAttributes(const wxTextAttrBorder& attr, wxTextAttrBorder& clashingAttr, wxTextAttrBorder& absentAttr);

    void SetStyle(int style) { m_borderStyle = style; m_flags |= wxTEXT_BOX_ATTR_BORDER_STYLE; }
    int GetStyle() const { return m_borderStyle; }

    void SetColour(unsigned long colour) { m_borderColour = colour; m_flags |= wxTEXT_BOX_ATTR_BORDER_COLOUR; }
    void SetColour(const wxColour& colour) { m_borderColour = colour.GetRGB(); m_flags |= wxTEXT_BOX_ATTR_BORDER_COLOUR; }
    unsigned long GetColourLong() const { return m_borderColour; }
    wxColour GetColour() const { return wxColour(m_borderColour); }

    wxTextAttrDimension& GetWidth() { return m_borderWidth; }
    const wxTextAttrDimension& GetWidth() const { return m_borderWidth; }
    void SetWidth(const wxTextAttrDimension& width) { m_borderWidth = width; }
    void SetWidth(int value, wxTextAttrUnits units = wxTEXT_ATTR_UNITS_TENTHS_MM) { SetWidth(wxTextAttrDimension(value, units)); }

    bool HasStyle() const { return (m_flags & wxTEXT_BOX_ATTR_BORDER_STYLE) != 0; }
    bool HasColour() const { return (m_flags & wxTEXT_BOX_ATTR_BORDER_COLOUR) != 0; }
    bool HasWidth() const { return m_borderWidth.IsValid(); }

    bool IsValid() const { return HasWidth(); }
    void MakeValid() { m_borderWidth.SetValid(true); }

    int GetFlags() const { return m_flags; }
    void SetFlags(int flags) { m_flags = flags; }
    void AddFlag(int flag) { m_flags |= flag; }
    void RemoveFlag(int flag) { m_flags &= ~flag; }

    int                         m_borderStyle;
    unsigned long               m_borderColour;
    wxTextAttrDimension         m_borderWidth;
    int                         m_flags;
};

// Borders
class WXDLLIMPEXP_RICHTEXT wxTextAttrBorders
{
public:
    wxTextAttrBorders() { }

    bool operator==(const wxTextAttrBorders& borders) const
    {
        return m_left == borders.m_left && m_right == borders.m_right &&
               m_top == borders.m_top && m_bottom == borders.m_bottom;
    }

    // Set style of all borders
    void SetStyle(int style);

    // Set colour of all borders
    void SetColour(unsigned long colour);
    void SetColour(const wxColour& colour);

    // Set width of all borders
    void SetWidth(const wxTextAttrDimension& width);
    void SetWidth(int value, wxTextAttrUnits units = wxTEXT_ATTR_UNITS_TENTHS_MM) { SetWidth(wxTextAttrDimension(value, units)); }

    // Reset
    void Reset() { m_left.Reset(); m_right.Reset(); m_top.Reset(); m_bottom.Reset(); }

    // Partial equality test
    bool EqPartial(const wxTextAttrBorders& borders) const;

    // Apply border to 'this', but not if the same as compareWith
    bool Apply(const wxTextAttrBorders& borders, const wxTextAttrBorders* compareWith = NULL);

    // Remove specified attributes from this object
    bool RemoveStyle(const wxTextAttrBorders& attr);

    // Collects the attributes that are common to a range of content, building up a note of
    // which attributes are absent in some objects and which clash in some objects.
    void CollectCommonAttributes(const wxTextAttrBorders& attr, wxTextAttrBorders& clashingAttr, wxTextAttrBorders& absentAttr);

    bool IsValid() const { return m_left.IsValid() || m_right.IsValid() || m_top.IsValid() || m_bottom.IsValid(); }

    const wxTextAttrBorder& GetLeft() const { return m_left; }
    wxTextAttrBorder& GetLeft() { return m_left; }

    const wxTextAttrBorder& GetRight() const { return m_right; }
    wxTextAttrBorder& GetRight() { return m_right; }

    const wxTextAttrBorder& GetTop() const { return m_top; }
    wxTextAttrBorder& GetTop() { return m_top; }

    const wxTextAttrBorder& GetBottom() const { return m_bottom; }
    wxTextAttrBorder& GetBottom() { return m_bottom; }

    wxTextAttrBorder m_left, m_right, m_top, m_bottom;

};

// ----------------------------------------------------------------------------
// wxTextBoxAttr: a structure containing box attributes
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_RICHTEXT wxTextBoxAttr
{
public:
    // ctors
    wxTextBoxAttr() { Init(); }
    wxTextBoxAttr(const wxTextBoxAttr& attr) { Init(); (*this) = attr; }

    /// Initialise this object.
    void Init() { Reset(); }

    /// Reset this object.
    void Reset();

    // Copy. Unnecessary since we let it do a binary copy
    //void Copy(const wxTextBoxAttr& attr);

    // Assignment
    //void operator= (const wxTextBoxAttr& attr);

    /// Equality test
    bool operator== (const wxTextBoxAttr& attr) const;

    /// Partial equality test, ignoring unset attributes.
    bool EqPartial(const wxTextBoxAttr& attr) const;

    /// Merges the given attributes. If compareWith
    /// is non-NULL, then it will be used to mask out those attributes that are the same in style
    /// and compareWith, for situations where we don't want to explicitly set inherited attributes.
    bool Apply(const wxTextBoxAttr& style, const wxTextBoxAttr* compareWith = NULL);

    /// Collects the attributes that are common to a range of content, building up a note of
    /// which attributes are absent in some objects and which clash in some objects.
    void CollectCommonAttributes(const wxTextBoxAttr& attr, wxTextBoxAttr& clashingAttr, wxTextBoxAttr& absentAttr);

    /// Removes the specified attributes from this object
    bool RemoveStyle(const wxTextBoxAttr& attr);

    /// Sets the flags.
    void SetFlags(int flags) { m_flags = flags; }

    /// Returns the flags.
    int GetFlags() const { return m_flags; }

    /// Is this flag present?
    bool HasFlag(wxTextBoxAttrFlags flag) const { return (m_flags & flag) != 0; }

    /// Remove this flag
    void RemoveFlag(wxTextBoxAttrFlags flag) { m_flags &= ~flag; }

    /// Add this flag
    void AddFlag(wxTextBoxAttrFlags flag) { m_flags |= flag; }

    /// Is this default? I.e. no attributes set.
    bool IsDefault() const;

    /// Get the float mode.
    wxTextBoxAttrFloatStyle GetFloatMode() const { return m_floatMode; }

    /// Set the float mode.
    void SetFloatMode(wxTextBoxAttrFloatStyle mode) { m_floatMode = mode; m_flags |= wxTEXT_BOX_ATTR_FLOAT; }

    /// Do we have a float mode?
    bool HasFloatMode() const { return HasFlag(wxTEXT_BOX_ATTR_FLOAT); }

    /// Is this object floating?
    bool IsFloating() const { return HasFloatMode() && GetFloatMode() != wxTEXT_BOX_ATTR_FLOAT_NONE; }

    /// Returns the clear mode - whether to wrap text after object. Currently unimplemented.
    wxTextBoxAttrClearStyle GetClearMode() const { return m_clearMode; }

    /// Set the clear mode. Currently unimplemented.
    void SetClearMode(wxTextBoxAttrClearStyle mode) { m_clearMode = mode; m_flags |= wxTEXT_BOX_ATTR_CLEAR; }

    /// Do we have a clear mode?
    bool HasClearMode() const { return HasFlag(wxTEXT_BOX_ATTR_CLEAR); }

    /// Returns the collapse mode - whether to collapse borders. Currently unimplemented.
    wxTextBoxAttrCollapseMode GetCollapseBorders() const { return m_collapseMode; }

    /// Sets the collapse mode - whether to collapse borders. Currently unimplemented.
    void SetCollapseBorders(wxTextBoxAttrCollapseMode collapse) { m_collapseMode = collapse; m_flags |= wxTEXT_BOX_ATTR_COLLAPSE_BORDERS; }

    /// Do we have a collapse borders flag?
    bool HasCollapseBorders() const { return HasFlag(wxTEXT_BOX_ATTR_COLLAPSE_BORDERS); }

    /// Returns the vertical alignment.
    wxTextBoxAttrVerticalAlignment GetVerticalAlignment() const { return m_verticalAlignment; }

    /// Set the vertical alignment.
    void SetVerticalAlignment(wxTextBoxAttrVerticalAlignment verticalAlignment) { m_verticalAlignment = verticalAlignment; m_flags |= wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT; }

    /// Do we have a vertical alignment flag?
    bool HasVerticalAlignment() const { return HasFlag(wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT); }

    /// Gets the margin values.
    wxTextAttrDimensions& GetMargins() { return m_margins; }
    const wxTextAttrDimensions& GetMargins() const { return m_margins; }

    /// Gets the left margin.
    wxTextAttrDimension& GetLeftMargin() { return m_margins.m_left; }
    const wxTextAttrDimension& GetLeftMargin() const { return m_margins.m_left; }

    /// Gets the right margin.
    wxTextAttrDimension& GetRightMargin() { return m_margins.m_right; }
    const wxTextAttrDimension& GetRightMargin() const { return m_margins.m_right; }

    /// Gets the top margin.
    wxTextAttrDimension& GetTopMargin() { return m_margins.m_top; }
    const wxTextAttrDimension& GetTopMargin() const { return m_margins.m_top; }

    /// Gets the bottom margin.
    wxTextAttrDimension& GetBottomMargin() { return m_margins.m_bottom; }
    const wxTextAttrDimension& GetBottomMargin() const { return m_margins.m_bottom; }

    /// Returns the position.
    wxTextAttrDimensions& GetPosition() { return m_position; }
    const wxTextAttrDimensions& GetPosition() const { return m_position; }

    /// Returns the left position.
    wxTextAttrDimension& GetLeft() { return m_position.m_left; }
    const wxTextAttrDimension& GetLeft() const { return m_position.m_left; }

    /// Returns the right position.
    wxTextAttrDimension& GetRight() { return m_position.m_right; }
    const wxTextAttrDimension& GetRight() const { return m_position.m_right; }

    /// Returns the top position.
    wxTextAttrDimension& GetTop() { return m_position.m_top; }
    const wxTextAttrDimension& GetTop() const { return m_position.m_top; }

    /// Returns the bottom position.
    wxTextAttrDimension& GetBottom() { return m_position.m_bottom; }
    const wxTextAttrDimension& GetBottom() const { return m_position.m_bottom; }

    /// Returns the padding values.
    wxTextAttrDimensions& GetPadding() { return m_padding; }
    const wxTextAttrDimensions& GetPadding() const { return m_padding; }

    /// Returns the left padding value.
    wxTextAttrDimension& GetLeftPadding() { return m_padding.m_left; }
    const wxTextAttrDimension& GetLeftPadding() const { return m_padding.m_left; }

    /// Returns the right padding value.
    wxTextAttrDimension& GetRightPadding() { return m_padding.m_right; }
    const wxTextAttrDimension& GetRightPadding() const { return m_padding.m_right; }

    /// Returns the top padding value.
    wxTextAttrDimension& GetTopPadding() { return m_padding.m_top; }
    const wxTextAttrDimension& GetTopPadding() const { return m_padding.m_top; }

    /// Returns the bottom padding value.
    wxTextAttrDimension& GetBottomPadding() { return m_padding.m_bottom; }
    const wxTextAttrDimension& GetBottomPadding() const { return m_padding.m_bottom; }

    /// Returns the borders.
    wxTextAttrBorders& GetBorder() { return m_border; }
    const wxTextAttrBorders& GetBorder() const { return m_border; }

    /// Returns the left border.
    wxTextAttrBorder& GetLeftBorder() { return m_border.m_left; }
    const wxTextAttrBorder& GetLeftBorder() const { return m_border.m_left; }

    /// Returns the top border.
    wxTextAttrBorder& GetTopBorder() { return m_border.m_top; }
    const wxTextAttrBorder& GetTopBorder() const { return m_border.m_top; }

    /// Returns the right border.
    wxTextAttrBorder& GetRightBorder() { return m_border.m_right; }
    const wxTextAttrBorder& GetRightBorder() const { return m_border.m_right; }

    /// Returns the bottom border.
    wxTextAttrBorder& GetBottomBorder() { return m_border.m_bottom; }
    const wxTextAttrBorder& GetBottomBorder() const { return m_border.m_bottom; }

    /// Returns the outline.
    wxTextAttrBorders& GetOutline() { return m_outline; }
    const wxTextAttrBorders& GetOutline() const { return m_outline; }

    /// Returns the left outline.
    wxTextAttrBorder& GetLeftOutline() { return m_outline.m_left; }
    const wxTextAttrBorder& GetLeftOutline() const { return m_outline.m_left; }

    /// Returns the top outline.
    wxTextAttrBorder& GetTopOutline() { return m_outline.m_top; }
    const wxTextAttrBorder& GetTopOutline() const { return m_outline.m_top; }

    /// Returns the right outline.
    wxTextAttrBorder& GetRightOutline() { return m_outline.m_right; }
    const wxTextAttrBorder& GetRightOutline() const { return m_outline.m_right; }

    /// Returns the bottom outline.
    wxTextAttrBorder& GetBottomOutline() { return m_outline.m_bottom; }
    const wxTextAttrBorder& GetBottomOutline() const { return m_outline.m_bottom; }

    /// Returns the object size.
    wxTextAttrSize& GetSize() { return m_size; }
    const wxTextAttrSize& GetSize() const { return m_size; }

    /// Sets the object size.
    void SetSize(const wxTextAttrSize& sz) { m_size = sz; }

    /// Returns the object width.
    wxTextAttrDimension& GetWidth() { return m_size.m_width; }
    const wxTextAttrDimension& GetWidth() const { return m_size.m_width; }

    /// Returns the object height.
    wxTextAttrDimension& GetHeight() { return m_size.m_height; }
    const wxTextAttrDimension& GetHeight() const { return m_size.m_height; }

public:

    int                             m_flags;

    wxTextAttrDimensions            m_margins;
    wxTextAttrDimensions            m_padding;
    wxTextAttrDimensions            m_position;

    wxTextAttrSize                  m_size;

    wxTextAttrBorders               m_border;
    wxTextAttrBorders               m_outline;

    wxTextBoxAttrFloatStyle         m_floatMode;
    wxTextBoxAttrClearStyle         m_clearMode;
    wxTextBoxAttrCollapseMode       m_collapseMode;
    wxTextBoxAttrVerticalAlignment  m_verticalAlignment;
};

// ----------------------------------------------------------------------------
// wxRichTextAttr: an enhanced attribute
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_RICHTEXT wxRichTextAttr: public wxTextAttr
{
public:
    wxRichTextAttr(const wxTextAttr& attr) { wxTextAttr::Copy(attr); }
    wxRichTextAttr(const wxRichTextAttr& attr): wxTextAttr() { Copy(attr); }
    wxRichTextAttr() {}

    // Copy
    void Copy(const wxRichTextAttr& attr);

    // Assignment
    void operator=(const wxRichTextAttr& attr) { Copy(attr); }
    void operator=(const wxTextAttr& attr) { wxTextAttr::Copy(attr); }

    // Equality test
    bool operator==(const wxRichTextAttr& attr) const;

    // Partial equality test taking comparison object into account
    bool EqPartial(const wxRichTextAttr& attr) const;

    // Merges the given attributes. If compareWith
    // is non-NULL, then it will be used to mask out those attributes that are the same in style
    // and compareWith, for situations where we don't want to explicitly set inherited attributes.
    bool Apply(const wxRichTextAttr& style, const wxRichTextAttr* compareWith = NULL);

    // Collects the attributes that are common to a range of content, building up a note of
    // which attributes are absent in some objects and which clash in some objects.
    void CollectCommonAttributes(const wxRichTextAttr& attr, wxRichTextAttr& clashingAttr, wxRichTextAttr& absentAttr);

    // Remove specified attributes from this object
    bool RemoveStyle(const wxRichTextAttr& attr);

    wxTextBoxAttr& GetTextBoxAttr() { return m_textBoxAttr; }
    const wxTextBoxAttr& GetTextBoxAttr() const { return m_textBoxAttr; }
    void SetTextBoxAttr(const wxTextBoxAttr& attr) { m_textBoxAttr = attr; }

    wxTextBoxAttr    m_textBoxAttr;
};

WX_DECLARE_USER_EXPORTED_OBJARRAY(wxVariant, wxRichTextVariantArray, WXDLLIMPEXP_RICHTEXT);

// ----------------------------------------------------------------------------
// wxRichTextProperties - A simple property class using wxVariants
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_RICHTEXT wxRichTextProperties: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxRichTextProperties)
public:
    wxRichTextProperties() {}
    wxRichTextProperties(const wxRichTextProperties& props): wxObject() { Copy(props); }

    void operator=(const wxRichTextProperties& props) { Copy(props); }
    bool operator==(const wxRichTextProperties& props) const;
    void Copy(const wxRichTextProperties& props) { m_properties = props.m_properties; }
    const wxVariant& operator[](size_t idx) const { return m_properties[idx]; }
    wxVariant& operator[](size_t idx) { return m_properties[idx]; }
    void Clear() { m_properties.Clear(); }

    const wxRichTextVariantArray& GetProperties() const { return m_properties; }
    wxRichTextVariantArray& GetProperties() { return m_properties; }
    void SetProperties(const wxRichTextVariantArray& props) { m_properties = props; }

    wxArrayString GetPropertyNames() const;

    size_t GetCount() const { return m_properties.GetCount(); }

    int HasProperty(const wxString& name) const { return Find(name) != -1; }

    int Find(const wxString& name) const;
    const wxVariant& GetProperty(const wxString& name) const;
    wxVariant* FindOrCreateProperty(const wxString& name);

    wxString GetPropertyString(const wxString& name) const;
    long GetPropertyLong(const wxString& name) const;
    bool GetPropertyBool(const wxString& name) const;
    double GetPropertyDouble(const wxString& name) const;

    void SetProperty(const wxVariant& variant);
    void SetProperty(const wxString& name, const wxVariant& variant);
    void SetProperty(const wxString& name, const wxString& value);
    void SetProperty(const wxString& name, long value);
    void SetProperty(const wxString& name, double value);
    void SetProperty(const wxString& name, bool value);

protected:
    wxRichTextVariantArray  m_properties;
};


/*!
 * wxRichTextFontTable
 * Manages quick access to a pool of fonts for rendering rich text
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextFontTable: public wxObject
{
public:
    wxRichTextFontTable();

    wxRichTextFontTable(const wxRichTextFontTable& table);
    virtual ~wxRichTextFontTable();

    bool IsOk() const { return m_refData != NULL; }

    wxFont FindFont(const wxRichTextAttr& fontSpec);
    void Clear();

    void operator= (const wxRichTextFontTable& table);
    bool operator == (const wxRichTextFontTable& table) const;
    bool operator != (const wxRichTextFontTable& table) const { return !(*this == table); }

protected:

    DECLARE_DYNAMIC_CLASS(wxRichTextFontTable)
};

/*!
 * wxRichTextRange class declaration
 * This stores beginning and end positions for a range of data.
 * TODO: consider renaming wxTextRange and using for all text controls.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextRange
{
public:
// Constructors

    wxRichTextRange() { m_start = 0; m_end = 0; }
    wxRichTextRange(long start, long end) { m_start = start; m_end = end; }
    wxRichTextRange(const wxRichTextRange& range) { m_start = range.m_start; m_end = range.m_end; }
    ~wxRichTextRange() {}

    void operator =(const wxRichTextRange& range) { m_start = range.m_start; m_end = range.m_end; }
    bool operator ==(const wxRichTextRange& range) const { return (m_start == range.m_start && m_end == range.m_end); }
    bool operator !=(const wxRichTextRange& range) const { return (m_start != range.m_start || m_end != range.m_end); }
    wxRichTextRange operator -(const wxRichTextRange& range) const { return wxRichTextRange(m_start - range.m_start, m_end - range.m_end); }
    wxRichTextRange operator +(const wxRichTextRange& range) const { return wxRichTextRange(m_start + range.m_start, m_end + range.m_end); }

    void SetRange(long start, long end) { m_start = start; m_end = end; }

    void SetStart(long start) { m_start = start; }
    long GetStart() const { return m_start; }

    void SetEnd(long end) { m_end = end; }
    long GetEnd() const { return m_end; }

    /// Returns true if this range is completely outside 'range'
    bool IsOutside(const wxRichTextRange& range) const { return range.m_start > m_end || range.m_end < m_start; }

    /// Returns true if this range is completely within 'range'
    bool IsWithin(const wxRichTextRange& range) const { return m_start >= range.m_start && m_end <= range.m_end; }

    /// Returns true if the given position is within this range. Allow
    /// for the possibility of an empty range - assume the position
    /// is within this empty range. NO, I think we should not match with an empty range.
    // bool Contains(long pos) const { return pos >= m_start && (pos <= m_end || GetLength() == 0); }
    bool Contains(long pos) const { return pos >= m_start && pos <= m_end ; }

    /// Limit this range to be within 'range'
    bool LimitTo(const wxRichTextRange& range) ;

    /// Gets the length of the range
    long GetLength() const { return m_end - m_start + 1; }

    /// Swaps the start and end
    void Swap() { long tmp = m_start; m_start = m_end; m_end = tmp; }

    /// Convert to internal form: (n, n) is the range of a single character.
    wxRichTextRange ToInternal() const { return wxRichTextRange(m_start, m_end-1); }

    /// Convert from internal to public API form: (n, n+1) is the range of a single character.
    wxRichTextRange FromInternal() const { return wxRichTextRange(m_start, m_end+1); }

protected:
    long m_start;
    long m_end;
};

WX_DECLARE_USER_EXPORTED_OBJARRAY(wxRichTextRange, wxRichTextRangeArray, WXDLLIMPEXP_RICHTEXT);

#define wxRICHTEXT_ALL  wxRichTextRange(-2, -2)
#define wxRICHTEXT_NONE  wxRichTextRange(-1, -1)

// ----------------------------------------------------------------------------
// wxRichTextSelection: stores selection information
// ----------------------------------------------------------------------------

#define wxRICHTEXT_NO_SELECTION wxRichTextRange(-2, -2)

class WXDLLIMPEXP_RICHTEXT wxRichTextSelection
{
public:
    wxRichTextSelection(const wxRichTextSelection& sel) { Copy(sel); }
    wxRichTextSelection(const wxRichTextRange& range, wxRichTextParagraphLayoutBox* container) { m_ranges.Add(range); m_container = container; }
    wxRichTextSelection() { Reset(); }

    /// Reset the selection
    void Reset() { m_ranges.Clear(); m_container = NULL; }

    /// Set the selection
    void Set(const wxRichTextRange& range, wxRichTextParagraphLayoutBox* container)
    { m_ranges.Clear(); m_ranges.Add(range); m_container = container; }

    /// Add a selection
    void Add(const wxRichTextRange& range)
    { m_ranges.Add(range); }

    /// Set the selections
    void Set(const wxRichTextRangeArray& ranges, wxRichTextParagraphLayoutBox* container)
    { m_ranges = ranges; m_container = container; }

    /// Copy
    void Copy(const wxRichTextSelection& sel)
    { m_ranges = sel.m_ranges; m_container = sel.m_container; }

    /// Assignment
    void operator=(const wxRichTextSelection& sel) { Copy(sel); }

    /// Equality test
    bool operator==(const wxRichTextSelection& sel) const;

    /// Index operator
    wxRichTextRange operator[](size_t i) const { return GetRange(i); }

    /// Get the selection ranges
    wxRichTextRangeArray& GetRanges() { return m_ranges; }
    const wxRichTextRangeArray& GetRanges() const { return m_ranges; }

    /// Set the selection ranges
    void SetRanges(const wxRichTextRangeArray& ranges) { m_ranges = ranges; }

    /// Get the number of ranges in the selection
    size_t GetCount() const { return m_ranges.GetCount(); }

    /// Get the given range
    wxRichTextRange GetRange(size_t i) const { return m_ranges[i]; }

    /// Get the first range if there is one, otherwise wxRICHTEXT_NO_SELECTION.
    wxRichTextRange GetRange() const { return (m_ranges.GetCount() > 0) ? (m_ranges[0]) : wxRICHTEXT_NO_SELECTION; }

    /// Set a single range.
    void SetRange(const wxRichTextRange& range) { m_ranges.Clear(); m_ranges.Add(range); }

    /// Get the container for which the selection is valid
    wxRichTextParagraphLayoutBox* GetContainer() const { return m_container; }

    /// Set the container for which the selection is valid
    void SetContainer(wxRichTextParagraphLayoutBox* container) { m_container = container; }

    /// Is the selection valid?
    bool IsValid() const { return m_ranges.GetCount() > 0 && GetContainer(); }

    /// Get the selection appropriate to the specified object, if any; returns an empty array if none
    /// at the level of the object's container.
    wxRichTextRangeArray GetSelectionForObject(wxRichTextObject* obj) const;

    /// Is the given position within the selection?
    bool WithinSelection(long pos, wxRichTextObject* obj) const;

    /// Is the given position within the selection?
    bool WithinSelection(long pos) const { return WithinSelection(pos, m_ranges); }

    /// Is the given position within the selection range?
    static bool WithinSelection(long pos, const wxRichTextRangeArray& ranges);

    /// Is the given range within the selection range?
    static bool WithinSelection(const wxRichTextRange& range, const wxRichTextRangeArray& ranges);

    wxRichTextRangeArray            m_ranges;
    wxRichTextParagraphLayoutBox*   m_container;
};

/*!
 * wxRichTextObject class declaration
 * This is the base for drawable objects.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextObject: public wxObject
{
    DECLARE_CLASS(wxRichTextObject)
public:
// Constructors

    wxRichTextObject(wxRichTextObject* parent = NULL);
    virtual ~wxRichTextObject();

// Overrideables

    /// Draw the item, within the given range. Some objects may ignore the range (for
    /// example paragraphs) while others must obey it (lines, to implement wrapping)
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style) = 0;

    /// Lay the item out at the specified position with the given size constraint.
    /// Layout must set the cached size.
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style) = 0;

    /// Hit-testing: returns a flag indicating hit test details, plus
    /// information about position. contextObj is returned to specify what object
    /// position is relevant to, since otherwise there's an ambiguity.
    /// obj may not a child of contextObj, since we may be referring to the container itself
    /// if we have no hit on a child - for example if we click outside an object.
    virtual int HitTest(wxDC& dc, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags = 0);

    /// Finds the absolute position and row height for the given character position
    virtual bool FindPosition(wxDC& WXUNUSED(dc), long WXUNUSED(index), wxPoint& WXUNUSED(pt), int* WXUNUSED(height), bool WXUNUSED(forceLineStart)) { return false; }

    /// Get the best size, i.e. the ideal starting size for this object irrespective
    /// of available space. For a short text string, it will be the size that exactly encloses
    /// the text. For a longer string, it might use the parent width for example.
    virtual wxSize GetBestSize() const { return m_size; }

    /**
        Gets the object size for the given range. Returns false if the range
        is invalid for this object.
    */

    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position = wxPoint(0,0), wxArrayInt* partialExtents = NULL) const  = 0;

    /// Do a split, returning an object containing the second part, and setting
    /// the first part in 'this'.
    virtual wxRichTextObject* DoSplit(long WXUNUSED(pos)) { return NULL; }

    /// Calculate range. By default, guess that the object is 1 unit long.
    virtual void CalculateRange(long start, long& end) { end = start ; m_range.SetRange(start, end); }

    /// Delete range
    virtual bool DeleteRange(const wxRichTextRange& WXUNUSED(range)) { return false; }

    /// Returns true if the object is empty
    virtual bool IsEmpty() const { return false; }

    /// Whether this object floatable
    virtual bool IsFloatable() const { return false; }

    /// Whether this object is currently floating
    virtual bool IsFloating() const { return GetAttributes().GetTextBoxAttr().IsFloating(); }

    /// Whether this object is a place holding one
    // virtual bool IsPlaceHolding() const { return false; }

    /// The floating direction
    virtual int GetFloatDirection() const { return GetAttributes().GetTextBoxAttr().GetFloatMode(); }

    /// Get any text in this object for the given range
    virtual wxString GetTextForRange(const wxRichTextRange& WXUNUSED(range)) const { return wxEmptyString; }

    /// Returns true if this object can merge itself with the given one.
    virtual bool CanMerge(wxRichTextObject* WXUNUSED(object)) const { return false; }

    /// Returns true if this object merged itself with the given one.
    /// The calling code will then delete the given object.
    virtual bool Merge(wxRichTextObject* WXUNUSED(object)) { return false; }

    /// Dump to output stream for debugging
    virtual void Dump(wxTextOutputStream& stream);

    /// Can we edit properties via a GUI?
    virtual bool CanEditProperties() const { return false; }

    /// Edit properties via a GUI
    virtual bool EditProperties(wxWindow* WXUNUSED(parent), wxRichTextBuffer* WXUNUSED(buffer)) { return false; }

    /// Return the label to be used for the properties context menu item.
    virtual wxString GetPropertiesMenuLabel() const { return wxEmptyString; }

    /// Returns true if objects of this class can accept the focus, i.e. a call to SetFocusObject
    /// is possible. For example, containers supporting text, such as a text box object, can accept the focus,
    /// but a table can't (set the focus to individual cells instead).
    virtual bool AcceptsFocus() const { return false; }

#if wxUSE_XML
    /// Import this object from XML
    virtual bool ImportFromXML(wxRichTextBuffer* buffer, wxXmlNode* node, wxRichTextXMLHandler* handler, bool* recurse);
#endif

#if wxRICHTEXT_HAVE_DIRECT_OUTPUT
    /// Export this object directly to the given stream.
    virtual bool ExportXML(wxOutputStream& stream, int indent, wxRichTextXMLHandler* handler);
#endif

#if wxRICHTEXT_HAVE_XMLDOCUMENT_OUTPUT
    /// Export this object to the given parent node, usually creating at least one child node.
    virtual bool ExportXML(wxXmlNode* parent, wxRichTextXMLHandler* handler);
#endif

    /// Does this object take note of paragraph attributes? Text and image objects don't.
    virtual bool UsesParagraphAttributes() const { return true; }

    /// What is the XML node name of this object?
    virtual wxString GetXMLNodeName() const { return wxT("unknown"); }

    /// Invalidate the buffer. With no argument, invalidates whole buffer.
    virtual void Invalidate(const wxRichTextRange& invalidRange = wxRICHTEXT_ALL);

    /// Can this object handle the selections of its children? FOr example, a table.
    virtual bool HandlesChildSelections() const { return false; }

    /// Returns a selection object specifying the selections between start and end character positions.
    /// For example, a table would deduce what cells (of range length 1) are selected when dragging across the table.
    virtual wxRichTextSelection GetSelection(long WXUNUSED(start), long WXUNUSED(end)) const { return wxRichTextSelection(); }

// Accessors

    /// Get/set the cached object size as calculated by Layout.
    virtual wxSize GetCachedSize() const { return m_size; }
    virtual void SetCachedSize(const wxSize& sz) { m_size = sz; }

    /// Get/set the maximum object size as calculated by Layout. This allows
    /// us to fit an object to its contents or allocate extra space if required.
    virtual wxSize GetMaxSize() const { return m_maxSize; }
    virtual void SetMaxSize(const wxSize& sz) { m_maxSize = sz; }

    /// Get/set the minimum object size as calculated by Layout. This allows
    /// us to constrain an object to its absolute minimum size if necessary.
    virtual wxSize GetMinSize() const { return m_minSize; }
    virtual void SetMinSize(const wxSize& sz) { m_minSize = sz; }

    /// Get the 'natural' size for an object. For an image, it would be the
    /// image size.
    virtual wxTextAttrSize GetNaturalSize() const { return wxTextAttrSize(); }

    /// Get/set the object position
    virtual wxPoint GetPosition() const { return m_pos; }
    virtual void SetPosition(const wxPoint& pos) { m_pos = pos; }

    /// Get the absolute object position, by traversing up the child/parent hierarchy
    /// TODO: may not be needed, if all object positions are in fact relative to the
    /// top of the coordinate space.
    virtual wxPoint GetAbsolutePosition() const;

    /// Get the rectangle enclosing the object
    virtual wxRect GetRect() const { return wxRect(GetPosition(), GetCachedSize()); }

    /// Set the range
    void SetRange(const wxRichTextRange& range) { m_range = range; }

    /// Get the range
    const wxRichTextRange& GetRange() const { return m_range; }
    wxRichTextRange& GetRange() { return m_range; }

    /// Set the 'own' range, for a top-level object with its own position space
    void SetOwnRange(const wxRichTextRange& range) { m_ownRange = range; }

    /// Get own range (valid if top-level)
    const wxRichTextRange& GetOwnRange() const { return m_ownRange; }
    wxRichTextRange& GetOwnRange() { return m_ownRange; }

    /// Get own range only if a top-level object
    wxRichTextRange GetOwnRangeIfTopLevel() const { return IsTopLevel() ? m_ownRange : m_range; }

    /// Is this composite?
    virtual bool IsComposite() const { return false; }

    /// Get/set the parent.
    virtual wxRichTextObject* GetParent() const { return m_parent; }
    virtual void SetParent(wxRichTextObject* parent) { m_parent = parent; }

    /// Get/set the top-level container of this object.
    /// May return itself if it's a container; use GetParentContainer to return
    /// a different container.
    virtual wxRichTextParagraphLayoutBox* GetContainer() const;

    /// Get/set the top-level container of this object.
    /// Returns a different container than itself, unless there's no parent, in which case it will return NULL.
    virtual wxRichTextParagraphLayoutBox* GetParentContainer() const { return GetParent() ? GetParent()->GetContainer() : GetContainer(); }

    /// Set the margin around the object, in pixels
    virtual void SetMargins(int margin);
    virtual void SetMargins(int leftMargin, int rightMargin, int topMargin, int bottomMargin);
    virtual int GetLeftMargin() const;
    virtual int GetRightMargin() const;
    virtual int GetTopMargin() const;
    virtual int GetBottomMargin() const;

    /// Calculate the available content space in the given rectangle, given the
    /// margins, border and padding specified in the object's attributes.
    virtual wxRect GetAvailableContentArea(wxDC& dc, const wxRect& outerRect) const;

    /// Lays out the object first with a given amount of space, and then if no width was specified in attr,
    /// lays out the object again using the minimum size
    virtual bool LayoutToBestSize(wxDC& dc, wxRichTextBuffer* buffer,
                    const wxRichTextAttr& parentAttr, const wxRichTextAttr& attr, const wxRect& availableParentSpace, int style);

    /// Set/get attributes object
    void SetAttributes(const wxRichTextAttr& attr) { m_attributes = attr; }
    const wxRichTextAttr& GetAttributes() const { return m_attributes; }
    wxRichTextAttr& GetAttributes() { return m_attributes; }

    /// Set/get properties
    wxRichTextProperties& GetProperties() { return m_properties; }
    const wxRichTextProperties& GetProperties() const { return m_properties; }
    void SetProperties(const wxRichTextProperties& props) { m_properties = props; }

    /// Set/get stored descent
    void SetDescent(int descent) { m_descent = descent; }
    int GetDescent() const { return m_descent; }

    /// Gets the containing buffer
    wxRichTextBuffer* GetBuffer() const;

    /// Sets the identifying name for this object, as a property.
    void SetName(const wxString& name) { m_properties.SetProperty(wxT("name"), name); }

    /// Gets the identifying name for this object.
    wxString GetName() const { return m_properties.GetPropertyString(wxT("name")); }

    /// Is this object top-level, i.e. with its own paragraphs, such as a text box?
    virtual bool IsTopLevel() const { return false; }

    /// Returns @true if the object will be shown, @false otherwise.
    bool IsShown() const { return m_show; }

// Operations

    /// Call to show or hide this object. This function does not cause the content to be
    /// laid out or redrawn.
    virtual void Show(bool show) { m_show = show; }

    /// Clone the object
    virtual wxRichTextObject* Clone() const { return NULL; }

    /// Copy
    void Copy(const wxRichTextObject& obj);

    /// Reference-counting allows us to use the same object in multiple
    /// lists (not yet used)
    void Reference() { m_refCount ++; }
    void Dereference();

    /// Move the object recursively, by adding the offset from old to new
    virtual void Move(const wxPoint& pt);

    /// Convert units in tenths of a millimetre to device units
    int ConvertTenthsMMToPixels(wxDC& dc, int units) const;
    static int ConvertTenthsMMToPixels(int ppi, int units, double scale = 1.0);

    /// Convert units in pixels to tenths of a millimetre
    int ConvertPixelsToTenthsMM(wxDC& dc, int pixels) const;
    static int ConvertPixelsToTenthsMM(int ppi, int pixels, double scale = 1.0);

    /// Draw the borders and background for the given rectangle and attributes.
    /// Width and height are taken to be the outer margin size, not the content.
    static bool DrawBoxAttributes(wxDC& dc, wxRichTextBuffer* buffer, const wxRichTextAttr& attr, const wxRect& boxRect, int flags = 0);

    /// Draw a border
    static bool DrawBorder(wxDC& dc, wxRichTextBuffer* buffer, const wxTextAttrBorders& attr, const wxRect& rect, int flags = 0);

    /// Get the various rectangles of the box model in pixels. You can either specify contentRect (inner)
    /// or marginRect (outer), and the other must be the default rectangle (no width or height).
    /// Note that the outline doesn't affect the position of the rectangle, it's drawn in whatever space
    /// is available.
    static bool GetBoxRects(wxDC& dc, wxRichTextBuffer* buffer, const wxRichTextAttr& attr, wxRect& marginRect, wxRect& borderRect, wxRect& contentRect, wxRect& paddingRect, wxRect& outlineRect);

    /// Get the total margin for the object in pixels, taking into account margin, padding and border size
    static bool GetTotalMargin(wxDC& dc, wxRichTextBuffer* buffer, const wxRichTextAttr& attr, int& leftMargin, int& rightMargin,
        int& topMargin, int& bottomMargin);

    /// Returns the rectangle which the child has available to it given restrictions specified in the
    /// child attribute, e.g. 50% width of the parent, 400 pixels, x position 20% of the parent, etc.
    static wxRect AdjustAvailableSpace(wxDC& dc, wxRichTextBuffer* buffer, const wxRichTextAttr& parentAttr, const wxRichTextAttr& childAttr, const wxRect& availableParentSpace);

protected:
    wxSize                  m_size;
    wxSize                  m_maxSize;
    wxSize                  m_minSize;
    wxPoint                 m_pos;
    int                     m_descent; // Descent for this object (if any)
    int                     m_refCount;
    bool                    m_show;
    wxRichTextObject*       m_parent;

    /// The range of this object (start position to end position)
    wxRichTextRange         m_range;

    /// The internal range of this object, if it's a top-level object with its own range space
    wxRichTextRange         m_ownRange;

    /// Attributes
    wxRichTextAttr          m_attributes;

    /// Properties
    wxRichTextProperties    m_properties;
};

WX_DECLARE_LIST_WITH_DECL( wxRichTextObject, wxRichTextObjectList, class WXDLLIMPEXP_RICHTEXT );

/*!
 * wxRichTextCompositeObject class declaration
 * Objects of this class can contain other objects.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextCompositeObject: public wxRichTextObject
{
    DECLARE_CLASS(wxRichTextCompositeObject)
public:
// Constructors

    wxRichTextCompositeObject(wxRichTextObject* parent = NULL);
    virtual ~wxRichTextCompositeObject();

// Overrideables

    /// Hit-testing: returns a flag indicating hit test details, plus
    /// information about position
    virtual int HitTest(wxDC& dc, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags = 0);

    /// Finds the absolute position and row height for the given character position
    virtual bool FindPosition(wxDC& dc, long index, wxPoint& pt, int* height, bool forceLineStart);

    /// Calculate range
    virtual void CalculateRange(long start, long& end);

    /// Delete range
    virtual bool DeleteRange(const wxRichTextRange& range);

    /// Get any text in this object for the given range
    virtual wxString GetTextForRange(const wxRichTextRange& range) const;

    /// Gets the object size for the given range. Returns false if the range
    /// is invalid for this object.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position = wxPoint(0,0), wxArrayInt* partialExtents = NULL) const;

    /// Dump to output stream for debugging
    virtual void Dump(wxTextOutputStream& stream);

    /// Invalidate the buffer. With no argument, invalidates whole buffer.
    virtual void Invalidate(const wxRichTextRange& invalidRange = wxRICHTEXT_ALL);

// Accessors

    /// Get the children
    wxRichTextObjectList& GetChildren() { return m_children; }
    const wxRichTextObjectList& GetChildren() const { return m_children; }

    /// Get the child count
    size_t GetChildCount() const ;

    /// Get the nth child
    wxRichTextObject* GetChild(size_t n) const ;

    /// Is this composite?
    virtual bool IsComposite() const { return true; }

    /// Returns true if the buffer is empty
    virtual bool IsEmpty() const { return GetChildCount() == 0; }

    /// Get the child object at the given character position
    virtual wxRichTextObject* GetChildAtPosition(long pos) const;

// Operations

    /// Copy
    void Copy(const wxRichTextCompositeObject& obj);

    /// Assignment
    void operator= (const wxRichTextCompositeObject& obj) { Copy(obj); }

    /// Append a child, returning the position
    size_t AppendChild(wxRichTextObject* child) ;

    /// Insert the child in front of the given object, or at the beginning
    bool InsertChild(wxRichTextObject* child, wxRichTextObject* inFrontOf) ;

    /// Delete the child
    bool RemoveChild(wxRichTextObject* child, bool deleteChild = false) ;

    /// Delete all children
    bool DeleteChildren() ;

    /// Recursively merge all pieces that can be merged.
    bool Defragment(const wxRichTextRange& range = wxRICHTEXT_ALL);

    /// Move the object recursively, by adding the offset from old to new
    virtual void Move(const wxPoint& pt);

protected:
    wxRichTextObjectList    m_children;
};

/*!
 * wxRichTextParagraphBox class declaration
 * This box knows how to lay out paragraphs.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextParagraphLayoutBox: public wxRichTextCompositeObject
{
    DECLARE_DYNAMIC_CLASS(wxRichTextParagraphLayoutBox)
public:
// Constructors

    wxRichTextParagraphLayoutBox(wxRichTextObject* parent = NULL);
    wxRichTextParagraphLayoutBox(const wxRichTextParagraphLayoutBox& obj): wxRichTextCompositeObject() { Init(); Copy(obj); }
    ~wxRichTextParagraphLayoutBox();

// Overrideables

    /// Hit-testing: returns a flag indicating hit test details, plus
    /// information about position
    virtual int HitTest(wxDC& dc, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags = 0);

    /// Draw the item
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style);

    /// Lay the item out
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style);

    /// Gets the object size for the given range. Returns false if the range
    /// is invalid for this object.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position = wxPoint(0,0), wxArrayInt* partialExtents = NULL) const;

    /// Delete range
    virtual bool DeleteRange(const wxRichTextRange& range);

    /// Get any text in this object for the given range
    virtual wxString GetTextForRange(const wxRichTextRange& range) const;

#if wxUSE_XML
    /// Import this object from XML
    virtual bool ImportFromXML(wxRichTextBuffer* buffer, wxXmlNode* node, wxRichTextXMLHandler* handler, bool* recurse);
#endif

#if wxRICHTEXT_HAVE_DIRECT_OUTPUT
    /// Export this object directly to the given stream.
    virtual bool ExportXML(wxOutputStream& stream, int indent, wxRichTextXMLHandler* handler);
#endif

#if wxRICHTEXT_HAVE_XMLDOCUMENT_OUTPUT
    /// Export this object to the given parent node, usually creating at least one child node.
    virtual bool ExportXML(wxXmlNode* parent, wxRichTextXMLHandler* handler);
#endif

    /// What is the XML node name of this object?
    virtual wxString GetXMLNodeName() const { return wxT("paragraphlayout"); }

    /// Returns true if objects of this class can accept the focus, i.e. a call to SetFocusObject
    /// is possible. For example, containers supporting text, such as a text box object, can accept the focus,
    /// but a table can't (set the focus to individual cells instead).
    virtual bool AcceptsFocus() const { return true; }

// Accessors

    /// Associate a control with the buffer, for operations that for example require refreshing the window.
    void SetRichTextCtrl(wxRichTextCtrl* ctrl) { m_ctrl = ctrl; }

    /// Get the associated control.
    wxRichTextCtrl* GetRichTextCtrl() const { return m_ctrl; }

    /// Get/set whether the last paragraph is partial or complete
    void SetPartialParagraph(bool partialPara) { m_partialParagraph = partialPara; }
    bool GetPartialParagraph() const { return m_partialParagraph; }

    /// Returns the style sheet associated with the overall buffer.
    virtual wxRichTextStyleSheet* GetStyleSheet() const;

    /// Is this object top-level, i.e. with its own paragraphs, such as a text box?
    virtual bool IsTopLevel() const { return true; }

// Operations

    /// Submit command to insert paragraphs
    bool InsertParagraphsWithUndo(long pos, const wxRichTextParagraphLayoutBox& paragraphs, wxRichTextCtrl* ctrl, wxRichTextBuffer* buffer, int flags = 0);

    /// Submit command to insert the given text
    bool InsertTextWithUndo(long pos, const wxString& text, wxRichTextCtrl* ctrl, wxRichTextBuffer* buffer, int flags = 0);

    /// Submit command to insert the given text
    bool InsertNewlineWithUndo(long pos, wxRichTextCtrl* ctrl, wxRichTextBuffer* buffer, int flags = 0);

    /// Submit command to insert the given image
    bool InsertImageWithUndo(long pos, const wxRichTextImageBlock& imageBlock,
                                                        wxRichTextCtrl* ctrl, wxRichTextBuffer* buffer, int flags,
                                                        const wxRichTextAttr& textAttr);

    /// Get the style that is appropriate for a new paragraph at this position.
    /// If the previous paragraph has a paragraph style name, look up the next-paragraph
    /// style.
    wxRichTextAttr GetStyleForNewParagraph(wxRichTextBuffer* buffer, long pos, bool caretPosition = false, bool lookUpNewParaStyle=false) const;

    /// Insert an object.
    wxRichTextObject* InsertObjectWithUndo(long pos, wxRichTextObject *object, wxRichTextCtrl* ctrl, wxRichTextBuffer* buffer, int flags = 0);

    /// Submit command to delete this range
    bool DeleteRangeWithUndo(const wxRichTextRange& range, wxRichTextCtrl* ctrl, wxRichTextBuffer* buffer);

    /// Draw the floats of this buffer
    void DrawFloats(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style);

    /// Move an anchored object to another paragraph
    void MoveAnchoredObjectToParagraph(wxRichTextParagraph* from, wxRichTextParagraph* to, wxRichTextObject* obj);

    /// Initialize the object.
    void Init();

    /// Clear all children
    virtual void Clear();

    /// Clear and initialize with one blank paragraph
    virtual void Reset();

    /// Convenience function to add a paragraph of text
    virtual wxRichTextRange AddParagraph(const wxString& text, wxRichTextAttr* paraStyle = NULL);

    /// Convenience function to add an image
    virtual wxRichTextRange AddImage(const wxImage& image, wxRichTextAttr* paraStyle = NULL);

    /// Adds multiple paragraphs, based on newlines.
    virtual wxRichTextRange AddParagraphs(const wxString& text, wxRichTextAttr* paraStyle = NULL);

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
    /// Given a position, get the corresponding wxRichTextLine object.
    virtual wxRichTextObject* GetLeafObjectAtPosition(long position) const;

    /// Get the paragraph by number
    virtual wxRichTextParagraph* GetParagraphAtLine(long paragraphNumber) const;

    /// Get the paragraph for a given line
    virtual wxRichTextParagraph* GetParagraphForLine(wxRichTextLine* line) const;

    /// Get the length of the paragraph
    virtual int GetParagraphLength(long paragraphNumber) const;

    /// Get the number of paragraphs
    virtual int GetParagraphCount() const { return static_cast<int>(GetChildCount()); }

    /// Get the number of visible lines
    virtual int GetLineCount() const;

    /// Get the text of the paragraph
    virtual wxString GetParagraphText(long paragraphNumber) const;

    /// Convert zero-based line column and paragraph number to a position.
    virtual long XYToPosition(long x, long y) const;

    /// Convert zero-based position to line column and paragraph number
    virtual bool PositionToXY(long pos, long* x, long* y) const;

    /// Set text attributes: character and/or paragraph styles.
    virtual bool SetStyle(const wxRichTextRange& range, const wxRichTextAttr& style, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO);

    /// Set the attributes for the given object only, for example the box attributes for a text box.
    virtual void SetStyle(wxRichTextObject *obj, const wxRichTextAttr& textAttr, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO);

    /// Get the combined text attributes for this position.
    virtual bool GetStyle(long position, wxRichTextAttr& style);

    /// Get the content (uncombined) attributes for this position.
    virtual bool GetUncombinedStyle(long position, wxRichTextAttr& style);

    /// Implementation helper for GetStyle. If combineStyles is true, combine base, paragraph and
    /// context attributes.
    virtual bool DoGetStyle(long position, wxRichTextAttr& style, bool combineStyles = true);

    /// Get the combined style for a range - if any attribute is different within the range,
    /// that attribute is not present within the flags
    virtual bool GetStyleForRange(const wxRichTextRange& range, wxRichTextAttr& style);

    /// Combines 'style' with 'currentStyle' for the purpose of summarising the attributes of a range of
    /// content.
    bool CollectStyle(wxRichTextAttr& currentStyle, const wxRichTextAttr& style, wxRichTextAttr& clashingAttr, wxRichTextAttr& absentAttr);

    /// Set list style
    virtual bool SetListStyle(const wxRichTextRange& range, wxRichTextListStyleDefinition* def, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO, int startFrom = 1, int specifiedLevel = -1);
    virtual bool SetListStyle(const wxRichTextRange& range, const wxString& defName, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO, int startFrom = 1, int specifiedLevel = -1);

    /// Clear list for given range
    virtual bool ClearListStyle(const wxRichTextRange& range, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO);

    /// Number/renumber any list elements in the given range.
    /// def/defName can be NULL/empty to indicate that the existing list style should be used.
    virtual bool NumberList(const wxRichTextRange& range, wxRichTextListStyleDefinition* def = NULL, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO, int startFrom = 1, int specifiedLevel = -1);
    virtual bool NumberList(const wxRichTextRange& range, const wxString& defName, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO, int startFrom = 1, int specifiedLevel = -1);

    /// Promote the list items within the given range. promoteBy can be a positive or negative number, e.g. 1 or -1
    /// def/defName can be NULL/empty to indicate that the existing list style should be used.
    virtual bool PromoteList(int promoteBy, const wxRichTextRange& range, wxRichTextListStyleDefinition* def = NULL, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO, int specifiedLevel = -1);
    virtual bool PromoteList(int promoteBy, const wxRichTextRange& range, const wxString& defName, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO, int specifiedLevel = -1);

    /// Helper for NumberList and PromoteList, that does renumbering and promotion simultaneously
    /// def/defName can be NULL/empty to indicate that the existing list style should be used.
    virtual bool DoNumberList(const wxRichTextRange& range, const wxRichTextRange& promotionRange, int promoteBy, wxRichTextListStyleDefinition* def, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO, int startFrom = 1, int specifiedLevel = -1);

    /// Fills in the attributes for numbering a paragraph after previousParagraph.
    virtual bool FindNextParagraphNumber(wxRichTextParagraph* previousParagraph, wxRichTextAttr& attr) const;

    /// Test if this whole range has character attributes of the specified kind. If any
    /// of the attributes are different within the range, the test fails. You
    /// can use this to implement, for example, bold button updating. style must have
    /// flags indicating which attributes are of interest.
    virtual bool HasCharacterAttributes(const wxRichTextRange& range, const wxRichTextAttr& style) const;

    /// Test if this whole range has paragraph attributes of the specified kind. If any
    /// of the attributes are different within the range, the test fails. You
    /// can use this to implement, for example, centering button updating. style must have
    /// flags indicating which attributes are of interest.
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

    /// Assignment
    void operator= (const wxRichTextParagraphLayoutBox& obj) { Copy(obj); }

    /// Calculate ranges
    virtual void UpdateRanges();

    /// Get all the text
    virtual wxString GetText() const;

    /// Set default style for new content. Setting it to a default attribute
    /// makes new content take on the 'basic' style.
    virtual bool SetDefaultStyle(const wxRichTextAttr& style);

    /// Get default style
    virtual const wxRichTextAttr& GetDefaultStyle() const { return m_defaultAttributes; }

    /// Set basic (overall) style
    virtual void SetBasicStyle(const wxRichTextAttr& style) { m_attributes = style; }

    /// Get basic (overall) style
    virtual const wxRichTextAttr& GetBasicStyle() const { return m_attributes; }

    /// Invalidate the buffer. With no argument, invalidates whole buffer.
    virtual void Invalidate(const wxRichTextRange& invalidRange = wxRICHTEXT_ALL);

    /// Do the (in)validation for this object only
    virtual void DoInvalidate(const wxRichTextRange& invalidRange);

    /// Do the (in)validation both up and down the hierarchy
    virtual void InvalidateHierarchy(const wxRichTextRange& invalidRange = wxRICHTEXT_ALL);

    /// Gather information about floating objects. If untilObj is non-NULL,
    /// will stop getting information if the current object is this, since we
    /// will collect the rest later.
    virtual bool UpdateFloatingObjects(const wxRect& availableRect, wxRichTextObject* untilObj = NULL);

    /// Get invalid range, rounding to entire paragraphs if argument is true.
    wxRichTextRange GetInvalidRange(bool wholeParagraphs = false) const;

    /// Does this object need layout?
    bool IsDirty() const { return m_invalidRange != wxRICHTEXT_NONE; }

    /// Get the wxRichTextFloatCollector of this object
    wxRichTextFloatCollector* GetFloatCollector() { return m_floatCollector; }

    /// Get the number of floating objects at this level
    int GetFloatingObjectCount() const;

    /// Get a list of floating objects
    bool GetFloatingObjects(wxRichTextObjectList& objects) const;

protected:
    wxRichTextCtrl* m_ctrl;
    wxRichTextAttr  m_defaultAttributes;

    /// The invalidated range that will need full layout
    wxRichTextRange m_invalidRange;

    // Is the last paragraph partial or complete?
    bool            m_partialParagraph;

    // The floating layout state
    wxRichTextFloatCollector* m_floatCollector;
};

/**
    @class wxRichTextBox

    wxRichTextBox is a floating or inline text box, containing paragraphs.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextBox: public wxRichTextParagraphLayoutBox
{
    DECLARE_DYNAMIC_CLASS(wxRichTextBox)
public:
// Constructors

    /**
        Default constructor; optionally pass the parent object.
    */

    wxRichTextBox(wxRichTextObject* parent = NULL);

    /**
        Copy constructor.
    */

    wxRichTextBox(const wxRichTextBox& obj): wxRichTextParagraphLayoutBox() { Copy(obj); }

// Overrideables

    /**
        Draws the item.
    */

    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style);

    /**
        Returns the XML node name of this object.
    */

    virtual wxString GetXMLNodeName() const { return wxT("textbox"); }

    /// Can we edit properties via a GUI?
    virtual bool CanEditProperties() const { return true; }

    /// Edit properties via a GUI
    virtual bool EditProperties(wxWindow* parent, wxRichTextBuffer* buffer);

    /// Return the label to be used for the properties context menu item.
    virtual wxString GetPropertiesMenuLabel() const { return _("&Box"); }

// Accessors

// Operations

    /**
        Makes a clone of this object.
    */
    virtual wxRichTextObject* Clone() const { return new wxRichTextBox(*this); }

    /**
        Copies this object.
    */
    void Copy(const wxRichTextBox& obj);

protected:
};

/*!
 * wxRichTextLine class declaration
 * This object represents a line in a paragraph, and stores
 * offsets from the start of the paragraph representing the
 * start and end positions of the line.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextLine
{
public:
// Constructors

    wxRichTextLine(wxRichTextParagraph* parent);
    wxRichTextLine(const wxRichTextLine& obj) { Init( NULL); Copy(obj); }
    virtual ~wxRichTextLine() {}

// Overrideables

// Accessors

    /// Set the range
    void SetRange(const wxRichTextRange& range) { m_range = range; }
    void SetRange(long from, long to) { m_range = wxRichTextRange(from, to); }

    /// Get the parent paragraph
    wxRichTextParagraph* GetParent() { return m_parent; }

    /// Get the range
    const wxRichTextRange& GetRange() const { return m_range; }
    wxRichTextRange& GetRange() { return m_range; }

    /// Get the absolute range
    wxRichTextRange GetAbsoluteRange() const;

    /// Get/set the line size as calculated by Layout.
    virtual wxSize GetSize() const { return m_size; }
    virtual void SetSize(const wxSize& sz) { m_size = sz; }

    /// Get/set the object position relative to the parent
    virtual wxPoint GetPosition() const { return m_pos; }
    virtual void SetPosition(const wxPoint& pos) { m_pos = pos; }

    /// Get the absolute object position
    virtual wxPoint GetAbsolutePosition() const;

    /// Get the rectangle enclosing the line
    virtual wxRect GetRect() const { return wxRect(GetAbsolutePosition(), GetSize()); }

    /// Set/get stored descent
    void SetDescent(int descent) { m_descent = descent; }
    int GetDescent() const { return m_descent; }

#if wxRICHTEXT_USE_OPTIMIZED_LINE_DRAWING
    wxArrayInt& GetObjectSizes() { return m_objectSizes; }
    const wxArrayInt& GetObjectSizes() const { return m_objectSizes; }
#endif

// Operations

    /// Initialisation
    void Init(wxRichTextParagraph* parent);

    /// Copy
    void Copy(const wxRichTextLine& obj);

    /// Clone
    virtual wxRichTextLine* Clone() const { return new wxRichTextLine(*this); }

protected:

    /// The range of the line (start position to end position)
    /// This is relative to the parent paragraph.
    wxRichTextRange     m_range;

    /// Size and position measured relative to top of paragraph
    wxPoint             m_pos;
    wxSize              m_size;

    /// Maximum descent for this line (location of text baseline)
    int                 m_descent;

    // The parent object
    wxRichTextParagraph* m_parent;

#if wxRICHTEXT_USE_OPTIMIZED_LINE_DRAWING
    wxArrayInt          m_objectSizes;
#endif
};

WX_DECLARE_LIST_WITH_DECL( wxRichTextLine, wxRichTextLineList , class WXDLLIMPEXP_RICHTEXT );

/*!
 * wxRichTextParagraph class declaration
 * This object represents a single paragraph (or in a straight text editor, a line).
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextParagraph: public wxRichTextCompositeObject
{
    DECLARE_DYNAMIC_CLASS(wxRichTextParagraph)
public:
// Constructors

    wxRichTextParagraph(wxRichTextObject* parent = NULL, wxRichTextAttr* style = NULL);
    wxRichTextParagraph(const wxString& text, wxRichTextObject* parent = NULL, wxRichTextAttr* paraStyle = NULL, wxRichTextAttr* charStyle = NULL);
    virtual ~wxRichTextParagraph();
    wxRichTextParagraph(const wxRichTextParagraph& obj): wxRichTextCompositeObject() { Copy(obj); }

// Overrideables

    /// Draw the item
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style);

    /// Lay the item out
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style);

    /// Gets the object size for the given range. Returns false if the range
    /// is invalid for this object.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position = wxPoint(0,0), wxArrayInt* partialExtents = NULL) const;

    /// Finds the absolute position and row height for the given character position
    virtual bool FindPosition(wxDC& dc, long index, wxPoint& pt, int* height, bool forceLineStart);

    /// Hit-testing: returns a flag indicating hit test details, plus
    /// information about position and the object that was found.
    virtual int HitTest(wxDC& dc, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags = 0);

    /// Calculate range
    virtual void CalculateRange(long start, long& end);

    /// What is the XML node name of this object?
    virtual wxString GetXMLNodeName() const { return wxT("paragraph"); }

// Accessors

    /// Get the cached lines
    wxRichTextLineList& GetLines() { return m_cachedLines; }

// Operations

    /// Copy
    void Copy(const wxRichTextParagraph& obj);

    /// Clone
    virtual wxRichTextObject* Clone() const { return new wxRichTextParagraph(*this); }

    /// Clear the cached lines
    void ClearLines();

// Implementation

    /// Apply paragraph styles such as centering to the wrapped lines
    virtual void ApplyParagraphStyle(wxRichTextLine* line, const wxRichTextAttr& attr, const wxRect& rect, wxDC& dc);

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
    bool FindWrapPosition(const wxRichTextRange& range, wxDC& dc, int availableSpace, long& wrapPosition, wxArrayInt* partialExtents);

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
    wxRichTextAttr GetCombinedAttributes(const wxRichTextAttr& contentStyle, bool includingBoxAttr = false) const;

    /// Get combined attributes of the base style and paragraph style.
    wxRichTextAttr GetCombinedAttributes(bool includingBoxAttr = false) const;

    /// Get the first position from pos that has a line break character.
    long GetFirstLineBreakPosition(long pos);

    /// Create default tabstop array
    static void InitDefaultTabs();

    /// Clear default tabstop array
    static void ClearDefaultTabs();

    /// Get default tabstop array
    static const wxArrayInt& GetDefaultTabs() { return sm_defaultTabs; }

    /// Layout the floats object
    void LayoutFloat(wxDC& dc, const wxRect& rect, int style, wxRichTextFloatCollector* floatCollector);

protected:
    /// The lines that make up the wrapped paragraph
    wxRichTextLineList m_cachedLines;

    /// Default tabstops
    static wxArrayInt  sm_defaultTabs;

friend class wxRichTextFloatCollector;
};

/*!
 * wxRichTextPlainText class declaration
 * This object represents a single piece of text.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextPlainText: public wxRichTextObject
{
    DECLARE_DYNAMIC_CLASS(wxRichTextPlainText)
public:
// Constructors

    wxRichTextPlainText(const wxString& text = wxEmptyString, wxRichTextObject* parent = NULL, wxRichTextAttr* style = NULL);
    wxRichTextPlainText(const wxRichTextPlainText& obj): wxRichTextObject() { Copy(obj); }

// Overrideables

    /// Draw the item
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style);

    /// Lay the item out
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style);

    /// Gets the object size for the given range. Returns false if the range
    /// is invalid for this object.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position = wxPoint(0,0), wxArrayInt* partialExtents = NULL) const;

    /// Get any text in this object for the given range
    virtual wxString GetTextForRange(const wxRichTextRange& range) const;

    /// Do a split, returning an object containing the second part, and setting
    /// the first part in 'this'.
    virtual wxRichTextObject* DoSplit(long pos);

    /// Calculate range
    virtual void CalculateRange(long start, long& end);

    /// Delete range
    virtual bool DeleteRange(const wxRichTextRange& range);

    /// Returns true if the object is empty
    virtual bool IsEmpty() const { return m_text.empty(); }

    /// Returns true if this object can merge itself with the given one.
    virtual bool CanMerge(wxRichTextObject* object) const;

    /// Returns true if this object merged itself with the given one.
    /// The calling code will then delete the given object.
    virtual bool Merge(wxRichTextObject* object);

    /// Dump to output stream for debugging
    virtual void Dump(wxTextOutputStream& stream);

    /// Get the first position from pos that has a line break character.
    long GetFirstLineBreakPosition(long pos);

    /// Does this object take note of paragraph attributes? Text and image objects don't.
    virtual bool UsesParagraphAttributes() const { return false; }

#if wxUSE_XML
    /// Import this object from XML
    virtual bool ImportFromXML(wxRichTextBuffer* buffer, wxXmlNode* node, wxRichTextXMLHandler* handler, bool* recurse);
#endif

#if wxRICHTEXT_HAVE_DIRECT_OUTPUT
    /// Export this object directly to the given stream.
    virtual bool ExportXML(wxOutputStream& stream, int indent, wxRichTextXMLHandler* handler);
#endif

#if wxRICHTEXT_HAVE_XMLDOCUMENT_OUTPUT
    /// Export this object to the given parent node, usually creating at least one child node.
    virtual bool ExportXML(wxXmlNode* parent, wxRichTextXMLHandler* handler);
#endif

    /// What is the XML node name of this object?
    virtual wxString GetXMLNodeName() const { return wxT("text"); }

// Accessors

    /// Get the text
    const wxString& GetText() const { return m_text; }

    /// Set the text
    void SetText(const wxString& text) { m_text = text; }

// Operations

    /// Copy
    void Copy(const wxRichTextPlainText& obj);

    /// Clone
    virtual wxRichTextObject* Clone() const { return new wxRichTextPlainText(*this); }
private:
    bool DrawTabbedString(wxDC& dc, const wxRichTextAttr& attr, const wxRect& rect, wxString& str, wxCoord& x, wxCoord& y, bool selected);

protected:
    wxString    m_text;
};

/*!
 * wxRichTextImageBlock stores information about an image, in binary in-memory form
 */

class WXDLLIMPEXP_FWD_BASE wxDataInputStream;
class WXDLLIMPEXP_FWD_BASE wxDataOutputStream;

class WXDLLIMPEXP_RICHTEXT wxRichTextImageBlock: public wxObject
{
public:
    wxRichTextImageBlock();
    wxRichTextImageBlock(const wxRichTextImageBlock& block);
    virtual ~wxRichTextImageBlock();

    void Init();
    void Clear();

    // Load the original image into a memory block.
    // If the image is not a JPEG, we must convert it into a JPEG
    // to conserve space.
    // If it's not a JPEG we can make use of 'image', already scaled, so we don't have to
    // load the image a 2nd time.
    virtual bool MakeImageBlock(const wxString& filename, wxBitmapType imageType,
                                wxImage& image, bool convertToJPEG = true);

    // Make an image block from the wxImage in the given
    // format.
    virtual bool MakeImageBlock(wxImage& image, wxBitmapType imageType, int quality = 80);

    // Uses a const wxImage for efficiency, but can't set quality (only relevant for JPEG)
    virtual bool MakeImageBlockDefaultQuality(const wxImage& image, wxBitmapType imageType);

    // Makes the image block
    virtual bool DoMakeImageBlock(const wxImage& image, wxBitmapType imageType);

    // Write to a file
    bool Write(const wxString& filename);

    // Write data in hex to a stream
    bool WriteHex(wxOutputStream& stream);

    // Read data in hex from a stream
    bool ReadHex(wxInputStream& stream, int length, wxBitmapType imageType);

    // Copy from 'block'
    void Copy(const wxRichTextImageBlock& block);

    // Load a wxImage from the block
    bool Load(wxImage& image);

//// Operators
    void operator=(const wxRichTextImageBlock& block);

//// Accessors

    unsigned char* GetData() const { return m_data; }
    size_t GetDataSize() const { return m_dataSize; }
    wxBitmapType GetImageType() const { return m_imageType; }

    void SetData(unsigned char* image) { m_data = image; }
    void SetDataSize(size_t size) { m_dataSize = size; }
    void SetImageType(wxBitmapType imageType) { m_imageType = imageType; }

    bool Ok() const { return IsOk(); }
    bool IsOk() const { return GetData() != NULL; }

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

protected:
    // Size in bytes of the image stored.
    // This is in the raw, original form such as a JPEG file.
    unsigned char*      m_data;
    size_t              m_dataSize;
    wxBitmapType        m_imageType;
};

/*!
 * wxRichTextImage class declaration
 * This object represents an image.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextImage: public wxRichTextObject
{
    DECLARE_DYNAMIC_CLASS(wxRichTextImage)
public:
// Constructors

    wxRichTextImage(wxRichTextObject* parent = NULL): wxRichTextObject(parent) { }
    wxRichTextImage(const wxImage& image, wxRichTextObject* parent = NULL, wxRichTextAttr* charStyle = NULL);
    wxRichTextImage(const wxRichTextImageBlock& imageBlock, wxRichTextObject* parent = NULL, wxRichTextAttr* charStyle = NULL);
    wxRichTextImage(const wxRichTextImage& obj): wxRichTextObject(obj) { Copy(obj); }

// Overrideables

    /// Draw the item
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style);

    /// Lay the item out
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style);

    /// Get the object size for the given range. Returns false if the range
    /// is invalid for this object.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position = wxPoint(0,0), wxArrayInt* partialExtents = NULL) const;

    /// Get the 'natural' size for an object. For an image, it would be the
    /// image size.
    virtual wxTextAttrSize GetNaturalSize() const;

    /// Returns true if the object is empty. An image is never empty; if the image is broken, that's not the same thing as empty.
    virtual bool IsEmpty() const { return false; /* !m_imageBlock.Ok(); */ }

    /// Can we edit properties via a GUI?
    virtual bool CanEditProperties() const { return true; }

    /// Edit properties via a GUI
    virtual bool EditProperties(wxWindow* parent, wxRichTextBuffer* buffer);

    /// Return the label to be used for the properties context menu item.
    virtual wxString GetPropertiesMenuLabel() const { return _("&Picture"); }

    /// Does this object take note of paragraph attributes? Text and image objects don't.
    virtual bool UsesParagraphAttributes() const { return false; }

#if wxUSE_XML
    /// Import this object from XML
    virtual bool ImportFromXML(wxRichTextBuffer* buffer, wxXmlNode* node, wxRichTextXMLHandler* handler, bool* recurse);
#endif

#if wxRICHTEXT_HAVE_DIRECT_OUTPUT
    /// Export this object directly to the given stream.
    virtual bool ExportXML(wxOutputStream& stream, int indent, wxRichTextXMLHandler* handler);
#endif

#if wxRICHTEXT_HAVE_XMLDOCUMENT_OUTPUT
    /// Export this object to the given parent node, usually creating at least one child node.
    virtual bool ExportXML(wxXmlNode* parent, wxRichTextXMLHandler* handler);
#endif

    // Images can be floatable (optionally).
    virtual bool IsFloatable() const { return true; }

    /// What is the XML node name of this object?
    virtual wxString GetXMLNodeName() const { return wxT("image"); }

// Accessors

    /// Get the image cache (scaled bitmap)
    const wxBitmap& GetImageCache() const { return m_imageCache; }

    /// Set the image cache
    void SetImageCache(const wxBitmap& bitmap) { m_imageCache = bitmap; }

    /// Reset the image cache
    void ResetImageCache() { m_imageCache = wxNullBitmap; }

    /// Get the image block containing the raw data
    wxRichTextImageBlock& GetImageBlock() { return m_imageBlock; }

// Operations

    /// Copy
    void Copy(const wxRichTextImage& obj);

    /// Clone
    virtual wxRichTextObject* Clone() const { return new wxRichTextImage(*this); }

    /// Create a cached image at the required size
    virtual bool LoadImageCache(wxDC& dc, bool resetCache = false);

protected:
    wxRichTextImageBlock    m_imageBlock;
    wxBitmap                m_imageCache;
};


/*!
 * wxRichTextBuffer class declaration
 * This is a kind of box, used to represent the whole buffer
 */

class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextCommand;
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextAction;

class WXDLLIMPEXP_RICHTEXT wxRichTextBuffer: public wxRichTextParagraphLayoutBox
{
    DECLARE_DYNAMIC_CLASS(wxRichTextBuffer)
public:
// Constructors

    wxRichTextBuffer() { Init(); }
    wxRichTextBuffer(const wxRichTextBuffer& obj): wxRichTextParagraphLayoutBox() { Init(); Copy(obj); }
    virtual ~wxRichTextBuffer() ;

// Accessors

    /// Gets the command processor
    wxCommandProcessor* GetCommandProcessor() const { return m_commandProcessor; }

    /// Set style sheet, if any.
    void SetStyleSheet(wxRichTextStyleSheet* styleSheet) { m_styleSheet = styleSheet; }
    virtual wxRichTextStyleSheet* GetStyleSheet() const { return m_styleSheet; }

    /// Set style sheet and notify of the change
    bool SetStyleSheetAndNotify(wxRichTextStyleSheet* sheet);

    /// Push style sheet to top of stack
    bool PushStyleSheet(wxRichTextStyleSheet* styleSheet);

    /// Pop style sheet from top of stack
    wxRichTextStyleSheet* PopStyleSheet();

    /// Set/get table storing fonts
    wxRichTextFontTable& GetFontTable() { return m_fontTable; }
    const wxRichTextFontTable& GetFontTable() const { return m_fontTable; }
    void SetFontTable(const wxRichTextFontTable& table) { m_fontTable = table; }

// Operations

    /// Initialisation
    void Init();

    /// Clears the buffer, adds an empty paragraph, and clears the command processor.
    virtual void ResetAndClearCommands();

    /// Load a file
    virtual bool LoadFile(const wxString& filename, wxRichTextFileType type = wxRICHTEXT_TYPE_ANY);

    /// Save a file
    virtual bool SaveFile(const wxString& filename, wxRichTextFileType type = wxRICHTEXT_TYPE_ANY);

    /// Load from a stream
    virtual bool LoadFile(wxInputStream& stream, wxRichTextFileType type = wxRICHTEXT_TYPE_ANY);

    /// Save to a stream
    virtual bool SaveFile(wxOutputStream& stream, wxRichTextFileType type = wxRICHTEXT_TYPE_ANY);

    /// Set the handler flags, controlling loading and saving
    void SetHandlerFlags(int flags) { m_handlerFlags = flags; }

    /// Get the handler flags, controlling loading and saving
    int GetHandlerFlags() const { return m_handlerFlags; }

    /// Convenience function to add a paragraph of text
    virtual wxRichTextRange AddParagraph(const wxString& text, wxRichTextAttr* paraStyle = NULL) { Modify(); return wxRichTextParagraphLayoutBox::AddParagraph(text, paraStyle); }

    /// Begin collapsing undo/redo commands. Note that this may not work properly
    /// if combining commands that delete or insert content, changing ranges for
    /// subsequent actions.
    virtual bool BeginBatchUndo(const wxString& cmdName);

    /// End collapsing undo/redo commands
    virtual bool EndBatchUndo();

    /// Collapsing commands?
    virtual bool BatchingUndo() const { return m_batchedCommandDepth > 0; }

    /// Submit immediately, or delay according to whether collapsing is on
    virtual bool SubmitAction(wxRichTextAction* action);

    /// Get collapsed command
    virtual wxRichTextCommand* GetBatchedCommand() const { return m_batchedCommand; }

    /// Begin suppressing undo/redo commands. The way undo is suppressed may be implemented
    /// differently by each command. If not dealt with by a command implementation, then
    /// it will be implemented automatically by not storing the command in the undo history
    /// when the action is submitted to the command processor.
    virtual bool BeginSuppressUndo();

    /// End suppressing undo/redo commands.
    virtual bool EndSuppressUndo();

    /// Collapsing commands?
    virtual bool SuppressingUndo() const { return m_suppressUndo > 0; }

    /// Copy the range to the clipboard
    virtual bool CopyToClipboard(const wxRichTextRange& range);

    /// Paste the clipboard content to the buffer
    virtual bool PasteFromClipboard(long position);

    /// Can we paste from the clipboard?
    virtual bool CanPasteFromClipboard() const;

    /// Begin using a style
    virtual bool BeginStyle(const wxRichTextAttr& style);

    /// End the style
    virtual bool EndStyle();

    /// End all styles
    virtual bool EndAllStyles();

    /// Clear the style stack
    virtual void ClearStyleStack();

    /// Get the size of the style stack, for example to check correct nesting
    virtual size_t GetStyleStackSize() const { return m_attributeStack.GetCount(); }

    /// Begin using bold
    bool BeginBold();

    /// End using bold
    bool EndBold() { return EndStyle(); }

    /// Begin using italic
    bool BeginItalic();

    /// End using italic
    bool EndItalic() { return EndStyle(); }

    /// Begin using underline
    bool BeginUnderline();

    /// End using underline
    bool EndUnderline() { return EndStyle(); }

    /// Begin using point size
    bool BeginFontSize(int pointSize);

    /// End using point size
    bool EndFontSize() { return EndStyle(); }

    /// Begin using this font
    bool BeginFont(const wxFont& font);

    /// End using a font
    bool EndFont() { return EndStyle(); }

    /// Begin using this colour
    bool BeginTextColour(const wxColour& colour);

    /// End using a colour
    bool EndTextColour() { return EndStyle(); }

    /// Begin using alignment
    bool BeginAlignment(wxTextAttrAlignment alignment);

    /// End alignment
    bool EndAlignment() { return EndStyle(); }

    /// Begin left indent
    bool BeginLeftIndent(int leftIndent, int leftSubIndent = 0);

    /// End left indent
    bool EndLeftIndent() { return EndStyle(); }

    /// Begin right indent
    bool BeginRightIndent(int rightIndent);

    /// End right indent
    bool EndRightIndent() { return EndStyle(); }

    /// Begin paragraph spacing
    bool BeginParagraphSpacing(int before, int after);

    /// End paragraph spacing
    bool EndParagraphSpacing() { return EndStyle(); }

    /// Begin line spacing
    bool BeginLineSpacing(int lineSpacing);

    /// End line spacing
    bool EndLineSpacing() { return EndStyle(); }

    /// Begin numbered bullet
    bool BeginNumberedBullet(int bulletNumber, int leftIndent, int leftSubIndent, int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD);

    /// End numbered bullet
    bool EndNumberedBullet() { return EndStyle(); }

    /// Begin symbol bullet
    bool BeginSymbolBullet(const wxString& symbol, int leftIndent, int leftSubIndent, int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_SYMBOL);

    /// End symbol bullet
    bool EndSymbolBullet() { return EndStyle(); }

    /// Begin standard bullet
    bool BeginStandardBullet(const wxString& bulletName, int leftIndent, int leftSubIndent, int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_STANDARD);

    /// End standard bullet
    bool EndStandardBullet() { return EndStyle(); }

    /// Begin named character style
    bool BeginCharacterStyle(const wxString& characterStyle);

    /// End named character style
    bool EndCharacterStyle() { return EndStyle(); }

    /// Begin named paragraph style
    bool BeginParagraphStyle(const wxString& paragraphStyle);

    /// End named character style
    bool EndParagraphStyle() { return EndStyle(); }

    /// Begin named list style
    bool BeginListStyle(const wxString& listStyle, int level = 1, int number = 1);

    /// End named character style
    bool EndListStyle() { return EndStyle(); }

    /// Begin URL
    bool BeginURL(const wxString& url, const wxString& characterStyle = wxEmptyString);

    /// End URL
    bool EndURL() { return EndStyle(); }

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

    /// Hit-testing: returns a flag indicating hit test details, plus
    /// information about position
    virtual int HitTest(wxDC& dc, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags = 0);

    /// Copy
    void Copy(const wxRichTextBuffer& obj);

    /// Assignment
    void operator= (const wxRichTextBuffer& obj) { Copy(obj); }

    /// Clone
    virtual wxRichTextObject* Clone() const { return new wxRichTextBuffer(*this); }

    /// Submit command to insert paragraphs
    bool InsertParagraphsWithUndo(long pos, const wxRichTextParagraphLayoutBox& paragraphs, wxRichTextCtrl* ctrl, int flags = 0);

    /// Submit command to insert the given text
    bool InsertTextWithUndo(long pos, const wxString& text, wxRichTextCtrl* ctrl, int flags = 0);

    /// Submit command to insert a newline
    bool InsertNewlineWithUndo(long pos, wxRichTextCtrl* ctrl, int flags = 0);

    /// Submit command to insert the given image
    bool InsertImageWithUndo(long pos, const wxRichTextImageBlock& imageBlock, wxRichTextCtrl* ctrl, int flags = 0,
            const wxRichTextAttr& textAttr = wxRichTextAttr());

    /// Submit command to insert an object
    wxRichTextObject* InsertObjectWithUndo(long pos, wxRichTextObject *object, wxRichTextCtrl* ctrl, int flags);

    /// Submit command to delete this range
    bool DeleteRangeWithUndo(const wxRichTextRange& range, wxRichTextCtrl* ctrl);

    /// Mark modified
    void Modify(bool modify = true) { m_modified = modify; }
    bool IsModified() const { return m_modified; }

    /// Dumps contents of buffer for debugging purposes
    virtual void Dump();
    virtual void Dump(wxTextOutputStream& stream) { wxRichTextParagraphLayoutBox::Dump(stream); }

    /// Returns the file handlers
    static wxList& GetHandlers() { return sm_handlers; }

    /// Adds a handler to the end
    static void AddHandler(wxRichTextFileHandler *handler);

    /// Inserts a handler at the front
    static void InsertHandler(wxRichTextFileHandler *handler);

    /// Removes a handler
    static bool RemoveHandler(const wxString& name);

    /// Finds a handler by name
    static wxRichTextFileHandler *FindHandler(const wxString& name);

    /// Finds a handler by extension and type
    static wxRichTextFileHandler *FindHandler(const wxString& extension, wxRichTextFileType imageType);

    /// Finds a handler by filename or, if supplied, type
    static wxRichTextFileHandler *FindHandlerFilenameOrType(const wxString& filename,
                                                            wxRichTextFileType imageType);

    /// Finds a handler by type
    static wxRichTextFileHandler *FindHandler(wxRichTextFileType imageType);

    /// Gets a wildcard incorporating all visible handlers. If 'types' is present,
    /// will be filled with the file type corresponding to each filter. This can be
    /// used to determine the type to pass to LoadFile given a selected filter.
    static wxString GetExtWildcard(bool combine = false, bool save = false, wxArrayInt* types = NULL);

    /// Clean up handlers
    static void CleanUpHandlers();

    /// Initialise the standard handlers
    static void InitStandardHandlers();

    /// Get renderer
    static wxRichTextRenderer* GetRenderer() { return sm_renderer; }

    /// Set renderer, deleting old one
    static void SetRenderer(wxRichTextRenderer* renderer);

    /// Minimum margin between bullet and paragraph in 10ths of a mm
    static int GetBulletRightMargin() { return sm_bulletRightMargin; }
    static void SetBulletRightMargin(int margin) { sm_bulletRightMargin = margin; }

    /// Factor to multiply by character height to get a reasonable bullet size
    static float GetBulletProportion() { return sm_bulletProportion; }
    static void SetBulletProportion(float prop) { sm_bulletProportion = prop; }

    /// Scale factor for calculating dimensions
    double GetScale() const { return m_scale; }
    void SetScale(double scale) { m_scale = scale; }

protected:

    /// Command processor
    wxCommandProcessor*     m_commandProcessor;

    /// Table storing fonts
    wxRichTextFontTable     m_fontTable;

    /// Has been modified?
    bool                    m_modified;

    /// Collapsed command stack
    int                     m_batchedCommandDepth;

    /// Name for collapsed command
    wxString                m_batchedCommandsName;

    /// Current collapsed command accumulating actions
    wxRichTextCommand*      m_batchedCommand;

    /// Whether to suppress undo
    int                     m_suppressUndo;

    /// Style sheet, if any
    wxRichTextStyleSheet*   m_styleSheet;

    /// List of event handlers that will be notified of events
    wxList                  m_eventHandlers;

    /// Stack of attributes for convenience functions
    wxList                  m_attributeStack;

    /// Flags to be passed to handlers
    int                     m_handlerFlags;

    /// File handlers
    static wxList           sm_handlers;

    /// Renderer
    static wxRichTextRenderer* sm_renderer;

    /// Minimum margin between bullet and paragraph in 10ths of a mm
    static int              sm_bulletRightMargin;

    /// Factor to multiply by character height to get a reasonable bullet size
    static float            sm_bulletProportion;

    /// Scaling factor in use: needed to calculate correct dimensions when printing
    double                  m_scale;
};

/**
    @class wxRichTextCell

    wxRichTextCell is the cell in a table.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextCell: public wxRichTextBox
{
    DECLARE_DYNAMIC_CLASS(wxRichTextCell)
public:
// Constructors

    /**
        Default constructor; optionally pass the parent object.
    */

    wxRichTextCell(wxRichTextObject* parent = NULL);

    /**
        Copy constructor.
    */

    wxRichTextCell(const wxRichTextCell& obj): wxRichTextBox() { Copy(obj); }

// Overrideables

    /**
        Draws the item.
    */

    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style);

    /**
        Returns the XML node name of this object.
    */

    virtual wxString GetXMLNodeName() const { return wxT("cell"); }

    /// Can we edit properties via a GUI?
    virtual bool CanEditProperties() const { return true; }

    /// Edit properties via a GUI
    virtual bool EditProperties(wxWindow* parent, wxRichTextBuffer* buffer);

    /// Return the label to be used for the properties context menu item.
    virtual wxString GetPropertiesMenuLabel() const { return _("&Cell"); }

// Accessors

// Operations

    /**
        Makes a clone of this object.
    */
    virtual wxRichTextObject* Clone() const { return new wxRichTextCell(*this); }

    /**
        Copies this object.
    */
    void Copy(const wxRichTextCell& obj);

protected:
};

/**
    @class wxRichTextTable

    wxRichTextTable represents a table with arbitrary columns and rows.
 */

WX_DEFINE_ARRAY_PTR(wxRichTextObject*, wxRichTextObjectPtrArray);
WX_DECLARE_OBJARRAY(wxRichTextObjectPtrArray, wxRichTextObjectPtrArrayArray);

class WXDLLIMPEXP_RICHTEXT wxRichTextTable: public wxRichTextBox
{
    DECLARE_DYNAMIC_CLASS(wxRichTextTable)
public:

// Constructors

    /**
        Default constructor; optionally pass the parent object.
    */

    wxRichTextTable(wxRichTextObject* parent = NULL);

    /**
        Copy constructor.
    */

    wxRichTextTable(const wxRichTextTable& obj): wxRichTextBox() { Copy(obj); }

// Overrideables

    // Draws the object.
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style);

    // Returns the XML node name of this object.
    virtual wxString GetXMLNodeName() const { return wxT("table"); }

    // Lays the object out.
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style);

    // Gets the range size.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position = wxPoint(0,0), wxArrayInt* partialExtents = NULL) const;

    // Deletes content in the given range.
    virtual bool DeleteRange(const wxRichTextRange& range);

    // Gets any text in this object for the given range.
    virtual wxString GetTextForRange(const wxRichTextRange& range) const;

#if wxUSE_XML
    // Import this object from XML
    virtual bool ImportFromXML(wxRichTextBuffer* buffer, wxXmlNode* node, wxRichTextXMLHandler* handler, bool* recurse);
#endif

#if wxRICHTEXT_HAVE_DIRECT_OUTPUT
    // Export this object directly to the given stream.
    virtual bool ExportXML(wxOutputStream& stream, int indent, wxRichTextXMLHandler* handler);
#endif

#if wxRICHTEXT_HAVE_XMLDOCUMENT_OUTPUT
    // Export this object to the given parent node, usually creating at least one child node.
    virtual bool ExportXML(wxXmlNode* parent, wxRichTextXMLHandler* handler);
#endif

    /// Finds the absolute position and row height for the given character position
    virtual bool FindPosition(wxDC& dc, long index, wxPoint& pt, int* height, bool forceLineStart);

    /// Calculate range
    virtual void CalculateRange(long start, long& end);

    /// Can this object handle the selections of its children? FOr example, a table.
    virtual bool HandlesChildSelections() const { return true; }

    /// Returns a selection object specifying the selections between start and end character positions.
    /// For example, a table would deduce what cells (of range length 1) are selected when dragging across the table.
    virtual wxRichTextSelection GetSelection(long start, long end) const;

    /// Can we edit properties via a GUI?
    virtual bool CanEditProperties() const { return true; }

    /// Edit properties via a GUI
    virtual bool EditProperties(wxWindow* parent, wxRichTextBuffer* buffer);

    /// Return the label to be used for the properties context menu item.
    virtual wxString GetPropertiesMenuLabel() const { return _("&Table"); }

    /// Returns true if objects of this class can accept the focus, i.e. a call to SetFocusObject
    /// is possible. For example, containers supporting text, such as a text box object, can accept the focus,
    /// but a table can't (set the focus to individual cells instead).
    virtual bool AcceptsFocus() const { return false; }

// Accessors

    const wxRichTextObjectPtrArrayArray& GetCells() const { return m_cells; }
    wxRichTextObjectPtrArrayArray& GetCells() { return m_cells; }

    int GetRowCount() const { return m_rowCount; }
    int GetColumnCount() const { return m_colCount; }

    /// Get the cell at the given row/column position
    virtual wxRichTextCell* GetCell(int row, int col) const;

    /// Get the cell at the given character position (in the range of the table).
    virtual wxRichTextCell* GetCell(long pos) const;

    /// Get the row/column for a given character position
    virtual bool GetCellRowColumnPosition(long pos, int& row, int& col) const;

// Operations

    /**
        Clears the table.
    */

    virtual void ClearTable();

    /**
        Creates a table of the given dimensions.
    */

    virtual bool CreateTable(int rows, int cols);

    /**
        Sets the attributes for the cells specified by the selection.
    */

    virtual bool SetCellStyle(const wxRichTextSelection& selection, const wxRichTextAttr& style, int flags = wxRICHTEXT_SETSTYLE_WITH_UNDO);

    /**
        Deletes rows from the given row position.
    */

    virtual bool DeleteRows(int startRow, int noRows = 1);

    /**
        Deletes columns from the given column position.
    */

    virtual bool DeleteColumns(int startCol, int noCols = 1);

    /**
        Adds rows from the given row position.
    */

    virtual bool AddRows(int startRow, int noRows = 1, const wxRichTextAttr& attr = wxRichTextAttr());

    /**
        Adds columns from the given column position.
    */

    virtual bool AddColumns(int startCol, int noCols = 1, const wxRichTextAttr& attr = wxRichTextAttr());

    // Makes a clone of this object.
    virtual wxRichTextObject* Clone() const { return new wxRichTextTable(*this); }

    // Copies this object.
    void Copy(const wxRichTextTable& obj);

protected:

    int m_rowCount;
    int m_colCount;

    // An array of rows, each of which is a wxRichTextObjectPtrArray containing
    // the cell objects. The cell objects are also children of this object.
    // Problem: if boxes are immediate children of a box, this will cause problems
    // with wxRichTextParagraphLayoutBox functions (and functions elsewhere) that
    // expect to find just paragraphs. May have to adjust the way we handle the
    // hierarchy to accept non-paragraph objects in a a paragraph layout box.
    // We'll be overriding much wxRichTextParagraphLayoutBox functionality so this
    // may not be such a problem. Perhaps the table should derive from a different
    // class?
    wxRichTextObjectPtrArrayArray   m_cells;
};


/*!
 * The command identifiers
 *
 */

enum wxRichTextCommandId
{
    wxRICHTEXT_INSERT,
    wxRICHTEXT_DELETE,
    wxRICHTEXT_CHANGE_ATTRIBUTES,
    wxRICHTEXT_CHANGE_STYLE,
    wxRICHTEXT_CHANGE_OBJECT
};

/*!
 * A class for specifying an object anywhere in an object hierarchy,
 * without using a pointer, necessary since wxRTC commands may delete
 * and recreate sub-objects so physical object addresses change. An array
 * of positions (one per hierarchy level) is used.
 *
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextObjectAddress
{
public:
    // Creates the address given container and object.
    wxRichTextObjectAddress(wxRichTextParagraphLayoutBox* topLevelContainer, wxRichTextObject* obj) { Create(topLevelContainer, obj); }
    wxRichTextObjectAddress() { Init(); }
    wxRichTextObjectAddress(const wxRichTextObjectAddress& address) { Copy(address); }

    void Init() {}
    void Copy(const wxRichTextObjectAddress& address) { m_address = address.m_address; }
    void operator=(const wxRichTextObjectAddress& address) { Copy(address); }

    wxRichTextObject* GetObject(wxRichTextParagraphLayoutBox* topLevelContainer) const;
    bool Create(wxRichTextParagraphLayoutBox* topLevelContainer, wxRichTextObject* obj);

    wxArrayInt& GetAddress() { return m_address; }
    const wxArrayInt& GetAddress() const { return m_address; }
    void SetAddress(const wxArrayInt& address) { m_address = address; }

protected:

    wxArrayInt  m_address;
};

/*!
 * Command classes for undo/redo
 *
 */

class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextAction;
class WXDLLIMPEXP_RICHTEXT wxRichTextCommand: public wxCommand
{
public:
    // Ctor for one action
    wxRichTextCommand(const wxString& name, wxRichTextCommandId id, wxRichTextBuffer* buffer,
        wxRichTextParagraphLayoutBox* container, wxRichTextCtrl* ctrl, bool ignoreFirstTime = false);

    // Ctor for multiple actions
    wxRichTextCommand(const wxString& name);

    virtual ~wxRichTextCommand();

    bool Do();
    bool Undo();

    void AddAction(wxRichTextAction* action);
    void ClearActions();

    wxList& GetActions() { return m_actions; }

protected:

    wxList  m_actions;
};

/*!
 * wxRichTextAction class declaration
 * There can be more than one action in a command.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextAction: public wxObject
{
public:
    /// Constructor. 'buffer' is the top-level buffer, while 'container' is the object within
    /// which the action is taking place. In the simplest case, they are the same.
    wxRichTextAction(wxRichTextCommand* cmd, const wxString& name, wxRichTextCommandId id,
        wxRichTextBuffer* buffer, wxRichTextParagraphLayoutBox* container,
        wxRichTextCtrl* ctrl, bool ignoreFirstTime = false);

    virtual ~wxRichTextAction();

    bool Do();
    bool Undo();

    /// Update the control appearance
    void UpdateAppearance(long caretPosition, bool sendUpdateEvent = false,
                            wxArrayInt* optimizationLineCharPositions = NULL, wxArrayInt* optimizationLineYPositions = NULL, bool isDoCmd = true);

    /// Replace the buffer paragraphs with the given fragment.
    void ApplyParagraphs(const wxRichTextParagraphLayoutBox& fragment);

    /// Get the fragments
    wxRichTextParagraphLayoutBox& GetNewParagraphs() { return m_newParagraphs; }
    wxRichTextParagraphLayoutBox& GetOldParagraphs() { return m_oldParagraphs; }

    /// Get the attributes
    wxRichTextAttr& GetAttributes() { return m_attributes; }

    /// An object to replace the one at the position
    /// defined by the container address and the action's range start position.
    wxRichTextObject* GetObject() const { return m_object; }
    void SetObject(wxRichTextObject* obj) { m_object = obj; m_objectAddress.Create(m_buffer, m_object); }
    void MakeObject(wxRichTextObject* obj) { m_objectAddress.Create(m_buffer, obj); }

    /// Calculate arrays for refresh optimization
    void CalculateRefreshOptimizations(wxArrayInt& optimizationLineCharPositions, wxArrayInt& optimizationLineYPositions);

    /// Set/get the position used for e.g. insertion
    void SetPosition(long pos) { m_position = pos; }
    long GetPosition() const { return m_position; }

    /// Set/get the range for e.g. deletion
    void SetRange(const wxRichTextRange& range) { m_range = range; }
    const wxRichTextRange& GetRange() const { return m_range; }

    /// The address (nested position) of the container within the buffer being manipulated
    wxRichTextObjectAddress& GetContainerAddress() { return m_containerAddress; }
    const wxRichTextObjectAddress& GetContainerAddress() const { return m_containerAddress; }
    void SetContainerAddress(const wxRichTextObjectAddress& address) { m_containerAddress = address; }
    void SetContainerAddress(wxRichTextParagraphLayoutBox* container, wxRichTextObject* obj) { m_containerAddress.Create(container, obj); }

    /// Returns the container that this action refers to, using the container address and top-level buffer.
    wxRichTextParagraphLayoutBox* GetContainer() const;
    /// Get name
    const wxString& GetName() const { return m_name; }

protected:
    // Action name
    wxString                        m_name;

    // Buffer
    wxRichTextBuffer*               m_buffer;

    // The address (nested position) of the container being manipulated.
    // This is necessary because objects are deleted, and we can't
    // therefore store actual pointers.
    wxRichTextObjectAddress         m_containerAddress;

    // Control
    wxRichTextCtrl*                 m_ctrl;

    // Stores the new paragraphs
    wxRichTextParagraphLayoutBox    m_newParagraphs;

    // Stores the old paragraphs
    wxRichTextParagraphLayoutBox    m_oldParagraphs;

    // Stores an object to replace the one at the position
    // defined by the container address and the action's range start position.
    wxRichTextObject*               m_object;

    // Stores the attributes
    wxRichTextAttr                  m_attributes;

    // The address of the object being manipulated (used for changing an individual object or its attributes)
    wxRichTextObjectAddress         m_objectAddress;

    // Stores the old attributes
    // wxRichTextAttr                  m_oldAttributes;

    // The affected range
    wxRichTextRange                 m_range;

    // The insertion point for this command
    long                            m_position;

    // Ignore 1st 'Do' operation because we already did it
    bool                            m_ignoreThis;

    // The command identifier
    wxRichTextCommandId             m_cmdId;
};

/*!
 * Handler flags
 */

// Include style sheet when loading and saving
#define wxRICHTEXT_HANDLER_INCLUDE_STYLESHEET       0x0001

// Save images to memory file system in HTML handler
#define wxRICHTEXT_HANDLER_SAVE_IMAGES_TO_MEMORY    0x0010

// Save images to files in HTML handler
#define wxRICHTEXT_HANDLER_SAVE_IMAGES_TO_FILES     0x0020

// Save images as inline base64 data in HTML handler
#define wxRICHTEXT_HANDLER_SAVE_IMAGES_TO_BASE64    0x0040

// Don't write header and footer (or BODY), so we can include the fragment
// in a larger document
#define wxRICHTEXT_HANDLER_NO_HEADER_FOOTER         0x0080

// Convert the more common face names to names that will work on the current platform
// in a larger document
#define wxRICHTEXT_HANDLER_CONVERT_FACENAMES        0x0100

/*!
 * wxRichTextFileHandler
 * Base class for file handlers
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextFileHandler: public wxObject
{
    DECLARE_CLASS(wxRichTextFileHandler)
public:
    wxRichTextFileHandler(const wxString& name = wxEmptyString, const wxString& ext = wxEmptyString, int type = 0)
        : m_name(name), m_extension(ext), m_type(type), m_flags(0), m_visible(true)
        { }

#if wxUSE_STREAMS
    bool LoadFile(wxRichTextBuffer *buffer, wxInputStream& stream)
    { return DoLoadFile(buffer, stream); }
    bool SaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream)
    { return DoSaveFile(buffer, stream); }
#endif

#if wxUSE_FFILE && wxUSE_STREAMS
    virtual bool LoadFile(wxRichTextBuffer *buffer, const wxString& filename);
    virtual bool SaveFile(wxRichTextBuffer *buffer, const wxString& filename);
#endif // wxUSE_STREAMS && wxUSE_STREAMS

    /// Can we handle this filename (if using files)? By default, checks the extension.
    virtual bool CanHandle(const wxString& filename) const;

    /// Can we save using this handler?
    virtual bool CanSave() const { return false; }

    /// Can we load using this handler?
    virtual bool CanLoad() const { return false; }

    /// Should this handler be visible to the user?
    virtual bool IsVisible() const { return m_visible; }
    virtual void SetVisible(bool visible) { m_visible = visible; }

    /// The name of the nandler
    void SetName(const wxString& name) { m_name = name; }
    wxString GetName() const { return m_name; }

    /// The default extension to recognise
    void SetExtension(const wxString& ext) { m_extension = ext; }
    wxString GetExtension() const { return m_extension; }

    /// The handler type
    void SetType(int type) { m_type = type; }
    int GetType() const { return m_type; }

    /// Flags controlling how loading and saving is done
    void SetFlags(int flags) { m_flags = flags; }
    int GetFlags() const { return m_flags; }

    /// Encoding to use when saving a file. If empty, a suitable encoding is chosen
    void SetEncoding(const wxString& encoding) { m_encoding = encoding; }
    const wxString& GetEncoding() const { return m_encoding; }

protected:

#if wxUSE_STREAMS
    virtual bool DoLoadFile(wxRichTextBuffer *buffer, wxInputStream& stream) = 0;
    virtual bool DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream) = 0;
#endif

    wxString  m_name;
    wxString  m_encoding;
    wxString  m_extension;
    int       m_type;
    int       m_flags;
    bool      m_visible;
};

/*!
 * wxRichTextPlainTextHandler
 * Plain text handler
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextPlainTextHandler: public wxRichTextFileHandler
{
    DECLARE_CLASS(wxRichTextPlainTextHandler)
public:
    wxRichTextPlainTextHandler(const wxString& name = wxT("Text"),
                               const wxString& ext = wxT("txt"),
                               wxRichTextFileType type = wxRICHTEXT_TYPE_TEXT)
        : wxRichTextFileHandler(name, ext, type)
        { }

    /// Can we save using this handler?
    virtual bool CanSave() const { return true; }

    /// Can we load using this handler?
    virtual bool CanLoad() const { return true; }

protected:

#if wxUSE_STREAMS
    virtual bool DoLoadFile(wxRichTextBuffer *buffer, wxInputStream& stream);
    virtual bool DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream);
#endif

};

#if wxUSE_DATAOBJ

/*!
 * The data object for a wxRichTextBuffer
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextBufferDataObject: public wxDataObjectSimple
{
public:
    // ctor doesn't copy the pointer, so it shouldn't go away while this object
    // is alive
    wxRichTextBufferDataObject(wxRichTextBuffer* richTextBuffer = NULL);
    virtual ~wxRichTextBufferDataObject();

    // after a call to this function, the buffer is owned by the caller and it
    // is responsible for deleting it
    wxRichTextBuffer* GetRichTextBuffer();

    // Returns the id for the new data format
    static const wxChar* GetRichTextBufferFormatId() { return ms_richTextBufferFormatId; }

    // base class pure virtuals

    virtual wxDataFormat GetPreferredFormat(Direction dir) const;
    virtual size_t GetDataSize() const;
    virtual bool GetDataHere(void *pBuf) const;
    virtual bool SetData(size_t len, const void *buf);

    // prevent warnings

    virtual size_t GetDataSize(const wxDataFormat&) const { return GetDataSize(); }
    virtual bool GetDataHere(const wxDataFormat&, void *buf) const { return GetDataHere(buf); }
    virtual bool SetData(const wxDataFormat&, size_t len, const void *buf) { return SetData(len, buf); }

private:
    wxDataFormat            m_formatRichTextBuffer;     // our custom format
    wxRichTextBuffer*       m_richTextBuffer;           // our data
    static const wxChar*    ms_richTextBufferFormatId;  // our format id
};

#endif

/*!
 * wxRichTextRenderer isolates common drawing functionality
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextRenderer: public wxObject
{
public:
    wxRichTextRenderer() {}
    virtual ~wxRichTextRenderer() {}

    /// Draw a standard bullet, as specified by the value of GetBulletName
    virtual bool DrawStandardBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxRichTextAttr& attr, const wxRect& rect) = 0;

    /// Draw a bullet that can be described by text, such as numbered or symbol bullets
    virtual bool DrawTextBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxRichTextAttr& attr, const wxRect& rect, const wxString& text) = 0;

    /// Draw a bitmap bullet, where the bullet bitmap is specified by the value of GetBulletName
    virtual bool DrawBitmapBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxRichTextAttr& attr, const wxRect& rect) = 0;

    /// Enumerate the standard bullet names currently supported
    virtual bool EnumerateStandardBulletNames(wxArrayString& bulletNames) = 0;
};

/*!
 * wxRichTextStdRenderer: standard renderer
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextStdRenderer: public wxRichTextRenderer
{
public:
    wxRichTextStdRenderer() {}

    /// Draw a standard bullet, as specified by the value of GetBulletName
    virtual bool DrawStandardBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxRichTextAttr& attr, const wxRect& rect);

    /// Draw a bullet that can be described by text, such as numbered or symbol bullets
    virtual bool DrawTextBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxRichTextAttr& attr, const wxRect& rect, const wxString& text);

    /// Draw a bitmap bullet, where the bullet bitmap is specified by the value of GetBulletName
    virtual bool DrawBitmapBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxRichTextAttr& attr, const wxRect& rect);

    /// Enumerate the standard bullet names currently supported
    virtual bool EnumerateStandardBulletNames(wxArrayString& bulletNames);
};

/*!
 * Utilities
 *
 */

inline bool wxRichTextHasStyle(int flags, int style)
{
    return ((flags & style) == style);
}

/// Compare two attribute objects
WXDLLIMPEXP_RICHTEXT bool wxTextAttrEq(const wxRichTextAttr& attr1, const wxRichTextAttr& attr2);
WXDLLIMPEXP_RICHTEXT bool wxTextAttrEq(const wxRichTextAttr& attr1, const wxRichTextAttr& attr2);

/// Compare two attribute objects, but take into account the flags
/// specifying attributes of interest.
WXDLLIMPEXP_RICHTEXT bool wxTextAttrEqPartial(const wxRichTextAttr& attr1, const wxRichTextAttr& attr2);

/// Apply one style to another
WXDLLIMPEXP_RICHTEXT bool wxRichTextApplyStyle(wxRichTextAttr& destStyle, const wxRichTextAttr& style, wxRichTextAttr* compareWith = NULL);

// Remove attributes
WXDLLIMPEXP_RICHTEXT bool wxRichTextRemoveStyle(wxRichTextAttr& destStyle, const wxRichTextAttr& style);

/// Combine two bitlists
WXDLLIMPEXP_RICHTEXT bool wxRichTextCombineBitlists(int& valueA, int valueB, int& flagsA, int flagsB);

/// Compare two bitlists
WXDLLIMPEXP_RICHTEXT bool wxRichTextBitlistsEqPartial(int valueA, int valueB, int flags);

/// Split into paragraph and character styles
WXDLLIMPEXP_RICHTEXT bool wxRichTextSplitParaCharStyles(const wxRichTextAttr& style, wxRichTextAttr& parStyle, wxRichTextAttr& charStyle);

/// Compare tabs
WXDLLIMPEXP_RICHTEXT bool wxRichTextTabsEq(const wxArrayInt& tabs1, const wxArrayInt& tabs2);

/// Convert a decimal to Roman numerals
WXDLLIMPEXP_RICHTEXT wxString wxRichTextDecimalToRoman(long n);

// Collects the attributes that are common to a range of content, building up a note of
// which attributes are absent in some objects and which clash in some objects.
WXDLLIMPEXP_RICHTEXT void wxTextAttrCollectCommonAttributes(wxTextAttr& currentStyle, const wxTextAttr& attr, wxTextAttr& clashingAttr, wxTextAttr& absentAttr);

WXDLLIMPEXP_RICHTEXT void wxRichTextModuleInit();

#endif
    // wxUSE_RICHTEXT

#endif
    // _WX_RICHTEXTBUFFER_H_

