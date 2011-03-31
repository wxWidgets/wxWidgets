/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextbuffer.cpp
// Purpose:     Buffer for wxRichTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     2005-09-30
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_RICHTEXT

#include "wx/richtext/richtextbuffer.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/dataobj.h"
    #include "wx/module.h"
#endif

#include "wx/settings.h"
#include "wx/filename.h"
#include "wx/clipbrd.h"
#include "wx/wfstream.h"
#include "wx/mstream.h"
#include "wx/sstream.h"
#include "wx/textfile.h"
#include "wx/hashmap.h"
#include "wx/dynarray.h"

#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"
#include "wx/richtext/richtextimagedlg.h"
#include "wx/richtext/richtextsizepage.h"

#include "wx/listimpl.cpp"
#include "wx/arrimpl.cpp"

WX_DEFINE_LIST(wxRichTextObjectList)
WX_DEFINE_LIST(wxRichTextLineList)

// Switch off if the platform doesn't like it for some reason
#define wxRICHTEXT_USE_OPTIMIZED_DRAWING 1

// Use GetPartialTextExtents for platforms that support it natively
#define wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS 1

const wxChar wxRichTextLineBreakChar = (wxChar) 29;

// Helper classes for floating layout
struct wxRichTextFloatRectMap
{
    wxRichTextFloatRectMap(int sY, int eY, int w, wxRichTextObject* obj)
    {
        startY = sY;
        endY = eY;
        width = w;
        anchor = obj;
    }

    int startY, endY;
    int width;
    wxRichTextObject* anchor;
};

WX_DEFINE_SORTED_ARRAY(wxRichTextFloatRectMap*, wxRichTextFloatRectMapArray);

int wxRichTextFloatRectMapCmp(wxRichTextFloatRectMap* r1, wxRichTextFloatRectMap* r2)
{
    return r1->startY - r2->startY;
}

class wxRichTextFloatCollector
{
public:
    wxRichTextFloatCollector(const wxRect& availableRect);
    ~wxRichTextFloatCollector();

    // Collect the floating objects info in the given paragraph
    void CollectFloat(wxRichTextParagraph* para);
    void CollectFloat(wxRichTextParagraph* para, wxRichTextObject* floating);

    // Return the last paragraph we collected
    wxRichTextParagraph* LastParagraph();

    // Given the start y position and the height of the line,
    // find out how wide the line can be
    wxRect GetAvailableRect(int startY, int endY);

    // Given a floating box, find its fit position
    int GetFitPosition(int direction, int start, int height) const;
    int GetFitPosition(const wxRichTextFloatRectMapArray& array, int start, int height) const;

    // Find the last y position
    int GetLastRectBottom();

    // Draw the floats inside a rect
    void Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style);

    // HitTest the floats
    int HitTest(wxDC& dc, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, int flags);

    // Get floating object count
    int GetFloatingObjectCount() const { return m_left.GetCount() + m_right.GetCount(); }

    // Get floating objects
    bool GetFloatingObjects(wxRichTextObjectList& objects) const;

    // Delete a float
    bool DeleteFloat(wxRichTextObject* obj);

    // Do we have this float already?
    bool HasFloat(wxRichTextObject* obj);

    bool HasFloats() const { return m_left.GetCount() >0 || m_right.GetCount() > 0; }

    static int SearchAdjacentRect(const wxRichTextFloatRectMapArray& array, int point);

    static int GetWidthFromFloatRect(const wxRichTextFloatRectMapArray& array, int index, int startY, int endY);

    static void FreeFloatRectMapArray(wxRichTextFloatRectMapArray& array);

    static void DrawFloat(const wxRichTextFloatRectMapArray& array, wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style);

    static int HitTestFloat(const wxRichTextFloatRectMapArray& array, wxDC& WXUNUSED(dc), const wxPoint& pt, long& textPosition, wxRichTextObject** obj, int flags);

private:
    wxRichTextFloatRectMapArray m_left;
    wxRichTextFloatRectMapArray m_right;
    //int m_width;
    wxRect               m_availableRect;
    wxRichTextParagraph* m_para;
};

// Delete a float
bool wxRichTextFloatCollector::DeleteFloat(wxRichTextObject* obj)
{
    size_t i;
    for (i = 0; i < m_left.GetCount(); i++)
    {
        if (m_left[i]->anchor == obj)
        {
            m_left.RemoveAt(i);
            return true;
        }
    }
    for (i = 0; i < m_right.GetCount(); i++)
    {
        if (m_right[i]->anchor == obj)
        {
            m_right.RemoveAt(i);
            return true;
        }
    }
    return false;
}

// Do we have this float already?
bool wxRichTextFloatCollector::HasFloat(wxRichTextObject* obj)
{
    size_t i;
    for (i = 0; i < m_left.GetCount(); i++)
    {
        if (m_left[i]->anchor == obj)
        {
            return true;
        }
    }
    for (i = 0; i < m_right.GetCount(); i++)
    {
        if (m_right[i]->anchor == obj)
        {
            return true;
        }
    }
    return false;
}

// Get floating objects
bool wxRichTextFloatCollector::GetFloatingObjects(wxRichTextObjectList& objects) const
{
    size_t i;
    for (i = 0; i < m_left.GetCount(); i++)
        objects.Append(m_left[i]->anchor);
    for (i = 0; i < m_right.GetCount(); i++)
        objects.Append(m_right[i]->anchor);
    return true;
}


/*
 * Binary search helper function
 * The argument point is the Y coordinate, and this fuction
 * always return the floating rect that contain this coordinate
 * or under this coordinate.
 */
int wxRichTextFloatCollector::SearchAdjacentRect(const wxRichTextFloatRectMapArray& array, int point)
{
    int end = array.GetCount() - 1;
    int start = 0;
    int ret = 0;

    wxASSERT(end >= 0);

    while (true)
    {
        if (start > end)
        {
            break;
        }

        int mid = (start + end) / 2;
        if (array[mid]->startY <= point && array[mid]->endY >= point)
            return mid;
        else if (array[mid]->startY > point)
        {
            end = mid - 1;
            ret = mid;
        }
        else if (array[mid]->endY < point)
        {
            start = mid + 1;
            ret = start;
        }
    }

    return ret;
}

int wxRichTextFloatCollector::GetWidthFromFloatRect(const wxRichTextFloatRectMapArray& array, int index, int startY, int endY)
{
    int ret = 0;
    int len = array.GetCount();

    wxASSERT(index >= 0 && index < len);

    if (array[index]->startY < startY && array[index]->endY > startY)
        ret = ret < array[index]->width ? array[index]->width : ret;
    while (index < len && array[index]->startY <= endY)
    {
        ret = ret < array[index]->width ? array[index]->width : ret;
        index++;
    }

    return ret;
}

wxRichTextFloatCollector::wxRichTextFloatCollector(const wxRect& rect) : m_left(wxRichTextFloatRectMapCmp), m_right(wxRichTextFloatRectMapCmp)
{
    m_availableRect = rect;
    m_para = NULL;
}

void wxRichTextFloatCollector::FreeFloatRectMapArray(wxRichTextFloatRectMapArray& array)
{
    int len = array.GetCount();
    for (int i = 0; i < len; i++)
        delete array[i];
}

wxRichTextFloatCollector::~wxRichTextFloatCollector()
{
    FreeFloatRectMapArray(m_left);
    FreeFloatRectMapArray(m_right);
}

int wxRichTextFloatCollector::GetFitPosition(const wxRichTextFloatRectMapArray& array, int start, int height) const
{
    if (array.GetCount() == 0)
        return start;

    int i = SearchAdjacentRect(array, start);
    int last = start;
    while (i < (int) array.GetCount())
    {
        if (array[i]->startY - last >= height)
            return last + 1;
        last = array[i]->endY;
        i++;
    }

    return last + 1;
}

int wxRichTextFloatCollector::GetFitPosition(int direction, int start, int height) const
{
    if (direction == wxTEXT_BOX_ATTR_FLOAT_LEFT)
        return GetFitPosition(m_left, start, height);
    else if (direction == wxTEXT_BOX_ATTR_FLOAT_RIGHT)
        return GetFitPosition(m_right, start, height);
    else
    {
        wxASSERT("Never should be here");
        return start;
    }
}

// Adds a floating image to the float collector.
// The actual positioning is done by wxRichTextParagraph::LayoutFloat.
void wxRichTextFloatCollector::CollectFloat(wxRichTextParagraph* para, wxRichTextObject* floating)
{
    int direction = floating->GetFloatDirection();

    wxPoint pos = floating->GetPosition();
    wxSize size = floating->GetCachedSize();
    wxRichTextFloatRectMap *map = new wxRichTextFloatRectMap(pos.y, pos.y + size.y, size.x, floating);
    switch (direction)
    {
        case wxTEXT_BOX_ATTR_FLOAT_NONE:
            delete map;
            break;
        case wxTEXT_BOX_ATTR_FLOAT_LEFT:
            // Just a not-enough simple assertion
            wxASSERT (m_left.Index(map) == wxNOT_FOUND);
            m_left.Add(map);
            break;
        case wxTEXT_BOX_ATTR_FLOAT_RIGHT:
            wxASSERT (m_right.Index(map) == wxNOT_FOUND);
            m_right.Add(map);
            break;
        default:
            delete map;
            wxASSERT("Unrecognised float attribute.");
    }

    m_para = para;
}

void wxRichTextFloatCollector::CollectFloat(wxRichTextParagraph* para)
{
    wxRichTextObjectList::compatibility_iterator node = para->GetChildren().GetFirst();
    while (node)
    {
        wxRichTextObject* floating = node->GetData();

        if (floating->IsFloating())
        {
            CollectFloat(para, floating);
        }

        node = node->GetNext();
    }

    m_para = para;
}

wxRichTextParagraph* wxRichTextFloatCollector::LastParagraph()
{
    return m_para;
}

wxRect wxRichTextFloatCollector::GetAvailableRect(int startY, int endY)
{
    int widthLeft = 0, widthRight = 0;
    if (m_left.GetCount() != 0)
    {
        int i = SearchAdjacentRect(m_left, startY);
        if (i < (int) m_left.GetCount())
            widthLeft = GetWidthFromFloatRect(m_left, i, startY, endY);
    }
    if (m_right.GetCount() != 0)
    {
        int j = SearchAdjacentRect(m_right, startY);
        if (j < (int) m_right.GetCount())
            widthRight = GetWidthFromFloatRect(m_right, j, startY, endY);
    }

    // TODO: actually we want to use the actual image positions to find the
    // available remaining space, since the image might not be right up against
    // the left or right edge of the container.
    return wxRect(widthLeft + m_availableRect.x, 0, m_availableRect.width - widthLeft - widthRight, 0);
}

int wxRichTextFloatCollector::GetLastRectBottom()
{
    int ret = 0;
    int len = m_left.GetCount();
    if (len) {
        ret = ret > m_left[len-1]->endY ? ret : m_left[len-1]->endY;
    }
    len = m_right.GetCount();
    if (len) {
        ret = ret > m_right[len-1]->endY ? ret : m_right[len-1]->endY;
    }

    return ret;
}

void wxRichTextFloatCollector::DrawFloat(const wxRichTextFloatRectMapArray& array, wxDC& dc, const wxRichTextRange& WXUNUSED(range), const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    int start = rect.y;
    int end = rect.y + rect.height;
    int i, j;
    i = SearchAdjacentRect(array, start);
    if (i < 0 || i >= (int) array.GetCount())
        return;
    j = SearchAdjacentRect(array, end);
    if (j < 0 || j >= (int) array.GetCount())
        j = array.GetCount() - 1;
    while (i <= j)
    {
        wxRichTextObject* obj = array[i]->anchor;
        wxRichTextRange r = obj->GetRange();
        obj->Draw(dc, r, selection, wxRect(obj->GetPosition(), obj->GetCachedSize()), descent, style);
        i++;
    }
}

void wxRichTextFloatCollector::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    if (m_left.GetCount() > 0)
        DrawFloat(m_left, dc, range, selection, rect, descent, style);
    if (m_right.GetCount() > 0)
        DrawFloat(m_right, dc, range, selection, rect, descent, style);
}

int wxRichTextFloatCollector::HitTestFloat(const wxRichTextFloatRectMapArray& array, wxDC& WXUNUSED(dc), const wxPoint& pt, long& textPosition, wxRichTextObject** obj, int WXUNUSED(flags))
{
    int i;
    if (array.GetCount() == 0)
        return wxRICHTEXT_HITTEST_NONE;
    i = SearchAdjacentRect(array, pt.y);
    if (i < 0 || i >= (int) array.GetCount())
        return wxRICHTEXT_HITTEST_NONE;
    if (!array[i]->anchor->IsShown())
        return wxRICHTEXT_HITTEST_NONE;

    wxPoint point = array[i]->anchor->GetPosition();
    wxSize size = array[i]->anchor->GetCachedSize();
    if (point.x <= pt.x && point.x + size.x >= pt.x
        && point.y <= pt.y && point.y + size.y >= pt.y)
    {
        textPosition = array[i]->anchor->GetRange().GetStart();
        * obj = array[i]->anchor;
        if (pt.x > (pt.x + pt.x + size.x) / 2)
            return wxRICHTEXT_HITTEST_BEFORE;
        else
            return wxRICHTEXT_HITTEST_AFTER;
    }

    return wxRICHTEXT_HITTEST_NONE;
}

int wxRichTextFloatCollector::HitTest(wxDC& dc, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, int flags)
{
    int ret = HitTestFloat(m_left, dc, pt, textPosition, obj, flags);
    if (ret == wxRICHTEXT_HITTEST_NONE)
    {
        ret = HitTestFloat(m_right, dc, pt, textPosition, obj, flags);
    }
    return ret;
}

// Helpers for efficiency
inline void wxCheckSetFont(wxDC& dc, const wxFont& font)
{
    // JACS: did I do this some time ago when testing? Should we re-enable it?
#if 0
    const wxFont& font1 = dc.GetFont();
    if (font1.IsOk() && font.IsOk())
    {
        if (font1.GetPointSize() == font.GetPointSize() &&
            font1.GetFamily() == font.GetFamily() &&
            font1.GetStyle() == font.GetStyle() &&
            font1.GetWeight() == font.GetWeight() &&
            font1.GetUnderlined() == font.GetUnderlined() &&
            font1.GetFamily() == font.GetFamily() &&
            font1.GetFaceName() == font.GetFaceName())
            return;
    }
#endif
    dc.SetFont(font);
}

inline void wxCheckSetPen(wxDC& dc, const wxPen& pen)
{
    const wxPen& pen1 = dc.GetPen();
    if (pen1.IsOk() && pen.IsOk())
    {
        if (pen1.GetWidth() == pen.GetWidth() &&
            pen1.GetStyle() == pen.GetStyle() &&
            pen1.GetColour() == pen.GetColour())
            return;
    }
    dc.SetPen(pen);
}

inline void wxCheckSetBrush(wxDC& dc, const wxBrush& brush)
{
    const wxBrush& brush1 = dc.GetBrush();
    if (brush1.IsOk() && brush.IsOk())
    {
        if (brush1.GetStyle() == brush.GetStyle() &&
            brush1.GetColour() == brush.GetColour())
            return;
    }
    dc.SetBrush(brush);
}

/*!
 * wxRichTextObject
 * This is the base for drawable objects.
 */

IMPLEMENT_CLASS(wxRichTextObject, wxObject)

wxRichTextObject::wxRichTextObject(wxRichTextObject* parent)
{
    m_refCount = 1;
    m_parent = parent;
    m_descent = 0;
    m_show = true;
}

wxRichTextObject::~wxRichTextObject()
{
}

void wxRichTextObject::Dereference()
{
    m_refCount --;
    if (m_refCount <= 0)
        delete this;
}

/// Copy
void wxRichTextObject::Copy(const wxRichTextObject& obj)
{
    m_size = obj.m_size;
    m_maxSize = obj.m_maxSize;
    m_minSize = obj.m_minSize;
    m_pos = obj.m_pos;
    m_range = obj.m_range;
    m_ownRange = obj.m_ownRange;
    m_attributes = obj.m_attributes;
    m_properties = obj.m_properties;
    m_descent = obj.m_descent;
    m_show = obj.m_show;
}

// Get/set the top-level container of this object.
wxRichTextParagraphLayoutBox* wxRichTextObject::GetContainer() const
{
    const wxRichTextObject* p = this;
    while (p)
    {
        if (p->IsTopLevel())
        {
            return wxDynamicCast(p, wxRichTextParagraphLayoutBox);
        }
        p = p->GetParent();
    }
    return NULL;
}

void wxRichTextObject::SetMargins(int margin)
{
    SetMargins(margin, margin, margin, margin);
}

void wxRichTextObject::SetMargins(int leftMargin, int rightMargin, int topMargin, int bottomMargin)
{
    GetAttributes().GetTextBoxAttr().GetMargins().GetLeft().SetValue(leftMargin, wxTEXT_ATTR_UNITS_PIXELS);
    GetAttributes().GetTextBoxAttr().GetMargins().GetRight().SetValue(rightMargin, wxTEXT_ATTR_UNITS_PIXELS);
    GetAttributes().GetTextBoxAttr().GetMargins().GetTop().SetValue(topMargin, wxTEXT_ATTR_UNITS_PIXELS);
    GetAttributes().GetTextBoxAttr().GetMargins().GetBottom().SetValue(bottomMargin, wxTEXT_ATTR_UNITS_PIXELS);
}

int wxRichTextObject::GetLeftMargin() const
{
    return GetAttributes().GetTextBoxAttr().GetMargins().GetLeft().GetValue();
}

int wxRichTextObject::GetRightMargin() const
{
    return GetAttributes().GetTextBoxAttr().GetMargins().GetRight().GetValue();
}

int wxRichTextObject::GetTopMargin() const
{
    return GetAttributes().GetTextBoxAttr().GetMargins().GetTop().GetValue();
}

int wxRichTextObject::GetBottomMargin() const
{
    return GetAttributes().GetTextBoxAttr().GetMargins().GetBottom().GetValue();
}

// Calculate the available content space in the given rectangle, given the
// margins, border and padding specified in the object's attributes.
wxRect wxRichTextObject::GetAvailableContentArea(wxDC& dc, const wxRect& outerRect) const
{
    wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
    marginRect = outerRect;
    GetBoxRects(dc, GetBuffer(), GetAttributes(), marginRect, borderRect, contentRect, paddingRect, outlineRect);
    return contentRect;
}

// Invalidate the buffer. With no argument, invalidates whole buffer.
void wxRichTextObject::Invalidate(const wxRichTextRange& invalidRange)
{
    if (invalidRange != wxRICHTEXT_NONE)
    {
        SetCachedSize(wxDefaultSize);
        SetMaxSize(wxDefaultSize);
        SetMinSize(wxDefaultSize);
    }
}

// Convert units in tenths of a millimetre to device units
int wxRichTextObject::ConvertTenthsMMToPixels(wxDC& dc, int units) const
{
    // Unscale
    double scale = 1.0;
    if (GetBuffer())
        scale = GetBuffer()->GetScale();
    int p = ConvertTenthsMMToPixels(dc.GetPPI().x, units, scale);

    return p;
}

// Convert units in tenths of a millimetre to device units
int wxRichTextObject::ConvertTenthsMMToPixels(int ppi, int units, double scale)
{
    // There are ppi pixels in 254.1 "1/10 mm"

    double pixels = ((double) units * (double)ppi) / 254.1;
    if (scale != 1.0)
        pixels /= scale;

    // If the result is very small, make it at least one pixel in size.
    if (pixels == 0 && units > 0)
        pixels = 1;

    return (int) pixels;
}

// Convert units in pixels to tenths of a millimetre
int wxRichTextObject::ConvertPixelsToTenthsMM(wxDC& dc, int pixels) const
{
    int p = pixels;
    double scale = 1.0;
    if (GetBuffer())
        scale = GetBuffer()->GetScale();

    return ConvertPixelsToTenthsMM(dc.GetPPI().x, p, scale);
}

int wxRichTextObject::ConvertPixelsToTenthsMM(int ppi, int pixels, double scale)
{
    // There are ppi pixels in 254.1 "1/10 mm"

    double p = double(pixels);

    if (scale != 1.0)
        p *= scale;

    int units = int( p * 254.1 / (double) ppi );
    return units;
}

// Draw the borders and background for the given rectangle and attributes.
// Width and height are taken to be the outer margin size, not the content.
bool wxRichTextObject::DrawBoxAttributes(wxDC& dc, wxRichTextBuffer* buffer, const wxRichTextAttr& attr, const wxRect& boxRect, int flags)
{
    // Assume boxRect is the area around the content
    wxRect marginRect = boxRect;
    wxRect contentRect, borderRect, paddingRect, outlineRect;

    GetBoxRects(dc, buffer, attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);

    // Margin is transparent. Draw background from margin.
    if (attr.HasBackgroundColour() || (flags & wxRICHTEXT_DRAW_SELECTED))
    {
        wxColour colour;
        if (flags & wxRICHTEXT_DRAW_SELECTED)
        {
            // TODO: get selection colour from control?
            colour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        }
        else
            colour = attr.GetBackgroundColour();

        wxPen pen(colour);
        wxBrush brush(colour);

        dc.SetPen(pen);
        dc.SetBrush(brush);
        dc.DrawRectangle(marginRect);
    }

    if (flags & wxRICHTEXT_DRAW_GUIDELINES)
    {
        wxRichTextAttr editBorderAttr = attr;
        // TODO: make guideline colour configurable
        editBorderAttr.GetTextBoxAttr().GetBorder().SetColour(*wxLIGHT_GREY);
        editBorderAttr.GetTextBoxAttr().GetBorder().SetWidth(1, wxTEXT_ATTR_UNITS_PIXELS);
        editBorderAttr.GetTextBoxAttr().GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);

        DrawBorder(dc, buffer, editBorderAttr.GetTextBoxAttr().GetBorder(), borderRect, flags);
    }

    if (attr.GetTextBoxAttr().GetBorder().IsValid())
        DrawBorder(dc, buffer, attr.GetTextBoxAttr().GetBorder(), borderRect);

    if (attr.GetTextBoxAttr().GetOutline().IsValid())
        DrawBorder(dc, buffer, attr.GetTextBoxAttr().GetOutline(), outlineRect);

    return true;
}

// Draw a border
bool wxRichTextObject::DrawBorder(wxDC& dc, wxRichTextBuffer* buffer, const wxTextAttrBorders& attr, const wxRect& rect, int WXUNUSED(flags))
{
    int borderLeft = 0, borderRight = 0, borderTop = 0, borderBottom = 0;
    wxTextAttrDimensionConverter converter(dc, buffer ? buffer->GetScale() : 1.0);

    if (attr.GetLeft().IsValid() && attr.GetLeft().GetStyle() != wxTEXT_BOX_ATTR_BORDER_NONE)
    {
        borderLeft = converter.GetPixels(attr.GetLeft().GetWidth());
        wxColour col(attr.GetLeft().GetColour());

        // If pen width is > 1, resorts to a solid rectangle.
        if (borderLeft == 1)
        {
            int penStyle = wxSOLID;
            if (attr.GetLeft().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DOTTED)
                penStyle = wxDOT;
            else if (attr.GetLeft().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DASHED)
                penStyle = wxLONG_DASH;
            wxPen pen(col, 1, penStyle);
            dc.SetPen(pen);
            dc.DrawLine(rect.x, rect.y, rect.x, rect.y + rect.height);

        }
        else if (borderLeft > 1)
        {
            wxPen pen(col);
            wxBrush brush(col);
            dc.SetPen(pen);
            dc.SetBrush(brush);
            dc.DrawRectangle(rect.x, rect.y, borderLeft, rect.height);
        }
    }

    if (attr.GetRight().IsValid() && attr.GetRight().GetStyle() != wxTEXT_BOX_ATTR_BORDER_NONE)
    {
        borderRight = converter.GetPixels(attr.GetRight().GetWidth());

        wxColour col(attr.GetRight().GetColour());

        // If pen width is > 1, resorts to a solid rectangle.
        if (borderRight == 1)
        {
            int penStyle = wxSOLID;
            if (attr.GetRight().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DOTTED)
                penStyle = wxDOT;
            else if (attr.GetRight().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DASHED)
                penStyle = wxLONG_DASH;
            wxPen pen(col, 1, penStyle);
            dc.SetPen(pen);
            dc.DrawLine(rect.x + rect.width, rect.y, rect.x + rect.width, rect.y + rect.height + 1);

        }
        else if (borderRight > 1)
        {
            wxPen pen(col);
            wxBrush brush(col);
            dc.SetPen(pen);
            dc.SetBrush(brush);
            dc.DrawRectangle(rect.x - borderRight, rect.y, borderRight, rect.height);
        }
    }

    if (attr.GetTop().IsValid() && attr.GetTop().GetStyle() != wxTEXT_BOX_ATTR_BORDER_NONE)
    {
        borderTop = converter.GetPixels(attr.GetTop().GetWidth());

        wxColour col(attr.GetTop().GetColour());

        // If pen width is > 1, resorts to a solid rectangle.
        if (borderTop == 1)
        {
            int penStyle = wxSOLID;
            if (attr.GetTop().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DOTTED)
                penStyle = wxDOT;
            else if (attr.GetTop().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DASHED)
                penStyle = wxLONG_DASH;
            wxPen pen(col, 1, penStyle);
            dc.SetPen(pen);
            dc.DrawLine(rect.x, rect.y, rect.x + rect.width, rect.y);

        }
        else if (borderTop > 1)
        {
            wxPen pen(col);
            wxBrush brush(col);
            dc.SetPen(pen);
            dc.SetBrush(brush);
            dc.DrawRectangle(rect.x, rect.y, rect.width, borderTop);
        }
    }

    if (attr.GetBottom().IsValid() && attr.GetBottom().GetStyle() != wxTEXT_BOX_ATTR_BORDER_NONE)
    {
        borderBottom = converter.GetPixels(attr.GetBottom().GetWidth());
        wxColour col(attr.GetTop().GetColour());

        // If pen width is > 1, resorts to a solid rectangle.
        if (borderBottom == 1)
        {
            int penStyle = wxSOLID;
            if (attr.GetBottom().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DOTTED)
                penStyle = wxDOT;
            else if (attr.GetBottom().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DASHED)
                penStyle = wxLONG_DASH;
            wxPen pen(col, 1, penStyle);
            dc.SetPen(pen);
            dc.DrawLine(rect.x, rect.y + rect.height, rect.x + rect.width, rect.y + rect.height);

        }
        else if (borderBottom > 1)
        {
            wxPen pen(col);
            wxBrush brush(col);
            dc.SetPen(pen);
            dc.SetBrush(brush);
            dc.DrawRectangle(rect.x, rect.y - rect.height - borderBottom, rect.width, borderBottom);
        }
    }

    return true;
}

// Get the various rectangles of the box model in pixels. You can either specify contentRect (inner)
// or marginRect (outer), and the other must be the default rectangle (no width or height).
// Note that the outline doesn't affect the position of the rectangle, it's drawn in whatever space
// is available.
//
// | Margin | Border | Padding | CONTENT | Padding | Border | Margin |

bool wxRichTextObject::GetBoxRects(wxDC& dc, wxRichTextBuffer* buffer, const wxRichTextAttr& attr, wxRect& marginRect, wxRect& borderRect, wxRect& contentRect, wxRect& paddingRect, wxRect& outlineRect)
{
    int borderLeft = 0, borderRight = 0, borderTop = 0, borderBottom = 0;
    int outlineLeft = 0, outlineRight = 0, outlineTop = 0, outlineBottom = 0;
    int paddingLeft = 0, paddingRight = 0, paddingTop = 0, paddingBottom = 0;
    int marginLeft = 0, marginRight = 0, marginTop = 0, marginBottom = 0;

    wxTextAttrDimensionConverter converter(dc, buffer ? buffer->GetScale() : 1.0);

    if (attr.GetTextBoxAttr().GetMargins().GetLeft().IsValid())
        marginLeft = converter.GetPixels(attr.GetTextBoxAttr().GetMargins().GetLeft());
    if (attr.GetTextBoxAttr().GetMargins().GetRight().IsValid())
        marginRight = converter.GetPixels(attr.GetTextBoxAttr().GetMargins().GetRight());
    if (attr.GetTextBoxAttr().GetMargins().GetTop().IsValid())
        marginTop = converter.GetPixels(attr.GetTextBoxAttr().GetMargins().GetTop());
    if (attr.GetTextBoxAttr().GetMargins().GetLeft().IsValid())
        marginBottom = converter.GetPixels(attr.GetTextBoxAttr().GetMargins().GetBottom());

    if (attr.GetTextBoxAttr().GetBorder().GetLeft().GetWidth().IsValid())
        borderLeft = converter.GetPixels(attr.GetTextBoxAttr().GetBorder().GetLeft().GetWidth());
    if (attr.GetTextBoxAttr().GetBorder().GetRight().GetWidth().IsValid())
        borderRight = converter.GetPixels(attr.GetTextBoxAttr().GetBorder().GetRight().GetWidth());
    if (attr.GetTextBoxAttr().GetBorder().GetTop().GetWidth().IsValid())
        borderTop = converter.GetPixels(attr.GetTextBoxAttr().GetBorder().GetTop().GetWidth());
    if (attr.GetTextBoxAttr().GetBorder().GetLeft().GetWidth().IsValid())
        borderBottom = converter.GetPixels(attr.GetTextBoxAttr().GetBorder().GetBottom().GetWidth());

    if (attr.GetTextBoxAttr().GetPadding().GetLeft().IsValid())
        paddingLeft = converter.GetPixels(attr.GetTextBoxAttr().GetPadding().GetLeft());
    if (attr.GetTextBoxAttr().GetPadding().GetRight().IsValid())
        paddingRight = converter.GetPixels(attr.GetTextBoxAttr().GetPadding().GetRight());
    if (attr.GetTextBoxAttr().GetPadding().GetTop().IsValid())
        paddingTop = converter.GetPixels(attr.GetTextBoxAttr().GetPadding().GetTop());
    if (attr.GetTextBoxAttr().GetPadding().GetBottom().IsValid())
        paddingBottom = converter.GetPixels(attr.GetTextBoxAttr().GetPadding().GetBottom());

    if (attr.GetTextBoxAttr().GetOutline().GetLeft().GetWidth().IsValid())
        outlineLeft = converter.GetPixels(attr.GetTextBoxAttr().GetOutline().GetLeft().GetWidth());
    if (attr.GetTextBoxAttr().GetOutline().GetRight().GetWidth().IsValid())
        outlineRight = converter.GetPixels(attr.GetTextBoxAttr().GetOutline().GetRight().GetWidth());
    if (attr.GetTextBoxAttr().GetOutline().GetTop().GetWidth().IsValid())
        outlineTop = converter.GetPixels(attr.GetTextBoxAttr().GetOutline().GetTop().GetWidth());
    if (attr.GetTextBoxAttr().GetOutline().GetBottom().GetWidth().IsValid())
        outlineBottom = converter.GetPixels(attr.GetTextBoxAttr().GetOutline().GetBottom().GetWidth());

    int leftTotal = marginLeft + borderLeft + paddingLeft;
    int rightTotal = marginRight + borderRight + paddingRight;
    int topTotal = marginTop + borderTop + paddingTop;
    int bottomTotal = marginBottom + borderBottom + paddingBottom;

    if (marginRect != wxRect())
    {
        contentRect.x = marginRect.x + leftTotal;
        contentRect.y = marginRect.y + topTotal;
        contentRect.width = marginRect.width - (leftTotal + rightTotal);
        contentRect.height = marginRect.height - (topTotal + bottomTotal);
    }
    else
    {
        marginRect.x = contentRect.x - leftTotal;
        marginRect.y = contentRect.y - topTotal;
        marginRect.width = contentRect.width + (leftTotal + rightTotal);
        marginRect.height = contentRect.height + (topTotal + bottomTotal);
    }

    borderRect.x = marginRect.x + marginLeft;
    borderRect.y = marginRect.y + marginTop;
    borderRect.width = marginRect.width - (marginLeft + marginRight);
    borderRect.height = marginRect.height - (marginTop + marginBottom);

    paddingRect.x = marginRect.x + marginLeft + borderLeft;
    paddingRect.y = marginRect.y + marginTop + borderTop;
    paddingRect.width = marginRect.width - (marginLeft + marginRight + borderLeft + borderRight);
    paddingRect.height = marginRect.height - (marginTop + marginBottom + borderTop + borderBottom);

    // The outline is outside the margin and doesn't influence the overall box position or content size.
    outlineRect.x = marginRect.x - outlineLeft;
    outlineRect.y = marginRect.y - outlineTop;
    outlineRect.width = marginRect.width + (outlineLeft + outlineRight);
    outlineRect.height = marginRect.height + (outlineTop + outlineBottom);

    return true;
}

// Get the total margin for the object in pixels, taking into account margin, padding and border size
bool wxRichTextObject::GetTotalMargin(wxDC& dc, wxRichTextBuffer* buffer, const wxRichTextAttr& attr, int& leftMargin, int& rightMargin,
        int& topMargin, int& bottomMargin)
{
    // Assume boxRect is the area around the content
    wxRect contentRect, marginRect, borderRect, paddingRect, outlineRect;
    marginRect = wxRect(0, 0, 1000, 1000);

    GetBoxRects(dc, buffer, attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);

    leftMargin = contentRect.GetLeft() - marginRect.GetLeft();
    rightMargin = marginRect.GetRight() - contentRect.GetRight();
    topMargin = contentRect.GetTop() - marginRect.GetTop();
    bottomMargin = marginRect.GetBottom() - contentRect.GetBottom();

    return true;
}

// Returns the rectangle which the child has available to it given restrictions specified in the
// child attribute, e.g. 50% width of the parent, 400 pixels, x position 20% of the parent, etc.
wxRect wxRichTextObject::AdjustAvailableSpace(wxDC& dc, wxRichTextBuffer* buffer, const wxRichTextAttr& WXUNUSED(parentAttr), const wxRichTextAttr& childAttr, const wxRect& availableParentSpace)
{
    wxRect rect = availableParentSpace;
    double scale = 1.0;
    if (buffer)
        scale = buffer->GetScale();

    wxTextAttrDimensionConverter converter(dc, scale, availableParentSpace.GetSize());

    if (childAttr.GetTextBoxAttr().GetWidth().IsValid())
        rect.width = converter.GetPixels(childAttr.GetTextBoxAttr().GetWidth());

    if (childAttr.GetTextBoxAttr().GetHeight().IsValid())
        rect.height = converter.GetPixels(childAttr.GetTextBoxAttr().GetHeight());

    // Can specify either left or right for the position (we're assuming we can't
    // set the left and right edges to effectively set the size. Would we want to do that?)
    if (childAttr.GetTextBoxAttr().GetPosition().GetLeft().IsValid())
    {
        rect.x = rect.x + converter.GetPixels(childAttr.GetTextBoxAttr().GetPosition().GetLeft());
    }
    else if (childAttr.GetTextBoxAttr().GetPosition().GetRight().IsValid())
    {
        int x = converter.GetPixels(childAttr.GetTextBoxAttr().GetPosition().GetRight());
        if (childAttr.GetTextBoxAttr().GetPosition().GetRight().GetPosition() == wxTEXT_BOX_ATTR_POSITION_RELATIVE)
            rect.x = availableParentSpace.x + availableParentSpace.width - rect.width;
        else
            rect.x += x;
    }

    if (childAttr.GetTextBoxAttr().GetPosition().GetTop().IsValid())
    {
        rect.y = rect.y + converter.GetPixels(childAttr.GetTextBoxAttr().GetPosition().GetTop());
    }
    else if (childAttr.GetTextBoxAttr().GetPosition().GetBottom().IsValid())
    {
        int y = converter.GetPixels(childAttr.GetTextBoxAttr().GetPosition().GetBottom());
        if (childAttr.GetTextBoxAttr().GetPosition().GetBottom().GetPosition() == wxTEXT_BOX_ATTR_POSITION_RELATIVE)
            rect.y = availableParentSpace.y + availableParentSpace.height - rect.height;
        else
            rect.y += y;
    }

    return rect;
}

// Dump to output stream for debugging
void wxRichTextObject::Dump(wxTextOutputStream& stream)
{
    stream << GetClassInfo()->GetClassName() << wxT("\n");
    stream << wxString::Format(wxT("Size: %d,%d. Position: %d,%d, Range: %ld,%ld"), m_size.x, m_size.y, m_pos.x, m_pos.y, m_range.GetStart(), m_range.GetEnd()) << wxT("\n");
    stream << wxString::Format(wxT("Text colour: %d,%d,%d."), (int) m_attributes.GetTextColour().Red(), (int) m_attributes.GetTextColour().Green(), (int) m_attributes.GetTextColour().Blue()) << wxT("\n");
}

// Gets the containing buffer
wxRichTextBuffer* wxRichTextObject::GetBuffer() const
{
    const wxRichTextObject* obj = this;
    while (obj && !obj->IsKindOf(CLASSINFO(wxRichTextBuffer)))
        obj = obj->GetParent();
    return wxDynamicCast(obj, wxRichTextBuffer);
}

// Get the absolute object position, by traversing up the child/parent hierarchy
wxPoint wxRichTextObject::GetAbsolutePosition() const
{
    wxPoint pt = GetPosition();

    wxRichTextObject* p = GetParent();
    while (p)
    {
        pt = pt + p->GetPosition();
        p = p->GetParent();
    }

    return pt;
}

// Hit-testing: returns a flag indicating hit test details, plus
// information about position
int wxRichTextObject::HitTest(wxDC& WXUNUSED(dc), const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int WXUNUSED(flags))
{
    if (!IsShown())
        return wxRICHTEXT_HITTEST_NONE;

    wxRect rect = GetRect();
    if (pt.x >= rect.x && pt.x < rect.x + rect.width &&
        pt.y >= rect.y && pt.y < rect.y + rect.height)
    {
        *obj = this;
        *contextObj = GetParentContainer();
        textPosition = GetRange().GetStart();
        return wxRICHTEXT_HITTEST_ON;
    }
    else
        return wxRICHTEXT_HITTEST_NONE;
}

// Lays out the object first with a given amount of space, and then if no width was specified in attr,
// lays out the object again using the maximum ('best') size
bool wxRichTextObject::LayoutToBestSize(wxDC& dc, wxRichTextBuffer* buffer,
    const wxRichTextAttr& parentAttr, const wxRichTextAttr& attr, const wxRect& availableParentSpace,
    int style)
{
    wxRect availableChildRect = AdjustAvailableSpace(dc, buffer, parentAttr, attr, availableParentSpace);
    wxRect originalAvailableRect = availableChildRect;
    Layout(dc, availableChildRect, style);

    wxSize maxSize = GetMaxSize();

    // Don't ignore if maxSize.x is zero, since we need to redo the paragraph's lines
    // on this basis
    if (!attr.GetTextBoxAttr().GetWidth().IsValid() && maxSize.x < availableChildRect.width /* && maxSize.x > 0 */)
    {
        // Redo the layout with a fixed, minimum size this time.
        Invalidate(wxRICHTEXT_ALL);
        wxRichTextAttr newAttr(attr);
        newAttr.GetTextBoxAttr().GetWidth().SetValue(maxSize.x, wxTEXT_ATTR_UNITS_PIXELS);
        newAttr.GetTextBoxAttr().GetWidth().SetPosition(wxTEXT_BOX_ATTR_POSITION_ABSOLUTE);

        availableChildRect = AdjustAvailableSpace(dc, buffer, parentAttr, newAttr, availableParentSpace);

        // If a paragraph, align the whole paragraph.
        // Problem with this: if we're limited by a floating object, a line may be centered
        // w.r.t. the smaller resulting box rather than the actual available width.
        if (attr.HasAlignment() && !GetContainer()->GetFloatCollector()->HasFloats()) // FIXME: aligning whole paragraph not compatible with floating objects
        {
            // centering, right-justification
            if (GetAttributes().GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
            {
                availableChildRect.x = (originalAvailableRect.GetWidth() - availableChildRect.GetWidth())/2 + availableChildRect.x;
            }
            else if (GetAttributes().GetAlignment() == wxTEXT_ALIGNMENT_RIGHT)
            {
                availableChildRect.x = availableChildRect.x + originalAvailableRect.GetWidth() - availableChildRect.GetWidth();
            }
        }

        Layout(dc, availableChildRect, style);
    }

    /*
     __________________
    |   ____________   |
    |  |            |  |


    */

    return true;
}

// Move the object recursively, by adding the offset from old to new
void wxRichTextObject::Move(const wxPoint& pt)
{
    SetPosition(pt);
}


/*!
 * wxRichTextCompositeObject
 * This is the base for drawable objects.
 */

IMPLEMENT_CLASS(wxRichTextCompositeObject, wxRichTextObject)

wxRichTextCompositeObject::wxRichTextCompositeObject(wxRichTextObject* parent):
    wxRichTextObject(parent)
{
}

wxRichTextCompositeObject::~wxRichTextCompositeObject()
{
    DeleteChildren();
}

/// Get the nth child
wxRichTextObject* wxRichTextCompositeObject::GetChild(size_t n) const
{
    wxASSERT ( n < m_children.GetCount() );

    return m_children.Item(n)->GetData();
}

/// Append a child, returning the position
size_t wxRichTextCompositeObject::AppendChild(wxRichTextObject* child)
{
    m_children.Append(child);
    child->SetParent(this);
    return m_children.GetCount() - 1;
}

/// Insert the child in front of the given object, or at the beginning
bool wxRichTextCompositeObject::InsertChild(wxRichTextObject* child, wxRichTextObject* inFrontOf)
{
    if (inFrontOf)
    {
        wxRichTextObjectList::compatibility_iterator node = m_children.Find(inFrontOf);
        m_children.Insert(node, child);
    }
    else
        m_children.Insert(child);
    child->SetParent(this);

    return true;
}

/// Delete the child
bool wxRichTextCompositeObject::RemoveChild(wxRichTextObject* child, bool deleteChild)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.Find(child);
    if (node)
    {
        wxRichTextObject* obj = node->GetData();
        m_children.Erase(node);
        if (deleteChild)
            delete obj;

        return true;
    }
    return false;
}

/// Delete all children
bool wxRichTextCompositeObject::DeleteChildren()
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObjectList::compatibility_iterator oldNode = node;

        wxRichTextObject* child = node->GetData();
        child->Dereference(); // Only delete if reference count is zero

        node = node->GetNext();
        m_children.Erase(oldNode);
    }

    return true;
}

/// Get the child count
size_t wxRichTextCompositeObject::GetChildCount() const
{
    return m_children.GetCount();
}

/// Copy
void wxRichTextCompositeObject::Copy(const wxRichTextCompositeObject& obj)
{
    wxRichTextObject::Copy(obj);

    DeleteChildren();

    wxRichTextObjectList::compatibility_iterator node = obj.m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        wxRichTextObject* newChild = child->Clone();
        newChild->SetParent(this);
        m_children.Append(newChild);

        node = node->GetNext();
    }
}

/// Hit-testing: returns a flag indicating hit test details, plus
/// information about position
int wxRichTextCompositeObject::HitTest(wxDC& dc, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags)
{
    if (!IsShown())
        return wxRICHTEXT_HITTEST_NONE;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        if (child->IsShown() && child->IsTopLevel() && (flags & wxRICHTEXT_HITTEST_NO_NESTED_OBJECTS))
        {
            // Just check if we hit the overall object
            int ret = child->wxRichTextObject::HitTest(dc, pt, textPosition, obj, contextObj, flags);
            if (ret != wxRICHTEXT_HITTEST_NONE)
                return ret;
        }
        else if (child->IsShown())
        {
            int ret = child->HitTest(dc, pt, textPosition, obj, contextObj, flags);
            if (ret != wxRICHTEXT_HITTEST_NONE)
                return ret;
        }

        node = node->GetNext();
    }

    return wxRICHTEXT_HITTEST_NONE;
}

/// Finds the absolute position and row height for the given character position
bool wxRichTextCompositeObject::FindPosition(wxDC& dc, long index, wxPoint& pt, int* height, bool forceLineStart)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        // Don't recurse if the child is a top-level object,
        // such as a text box, because the character position will no longer
        // apply. By definition, a top-level object has its own range of
        // character positions.
        if (!child->IsTopLevel() && child->FindPosition(dc, index, pt, height, forceLineStart))
            return true;

        node = node->GetNext();
    }

    return false;
}

/// Calculate range
void wxRichTextCompositeObject::CalculateRange(long start, long& end)
{
    long current = start;
    long lastEnd = current;

    if (IsTopLevel())
    {
        current = 0;
        lastEnd = 0;
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        long childEnd = 0;

        child->CalculateRange(current, childEnd);
        lastEnd = childEnd;

        current = childEnd + 1;

        node = node->GetNext();
    }

    if (IsTopLevel())
    {
        // A top-level object always has a range of size 1,
        // because its children don't count at this level.
        end = start;
        m_range.SetRange(start, start);

        // An object with no children has zero length
        if (m_children.GetCount() == 0)
            lastEnd --;
        m_ownRange.SetRange(0, lastEnd);
    }
    else
    {
        end = lastEnd;

        // An object with no children has zero length
        if (m_children.GetCount() == 0)
            end --;

        m_range.SetRange(start, end);
    }
}

/// Delete range from layout.
bool wxRichTextCompositeObject::DeleteRange(const wxRichTextRange& range)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();

    while (node)
    {
        wxRichTextObject* obj = (wxRichTextObject*) node->GetData();
        wxRichTextObjectList::compatibility_iterator next = node->GetNext();

        // Delete the range in each paragraph

        // When a chunk has been deleted, internally the content does not
        // now match the ranges.
        // However, so long as deletion is not done on the same object twice this is OK.
        // If you may delete content from the same object twice, recalculate
        // the ranges inbetween DeleteRange calls by calling CalculateRanges, and
        // adjust the range you're deleting accordingly.

        if (!obj->GetRange().IsOutside(range))
        {
            // No need to delete within a top-level object; just removing this object will do fine
            if (!obj->IsTopLevel())
                obj->DeleteRange(range);

            // Delete an empty object, or paragraph within this range.
            if (obj->IsEmpty() ||
                (range.GetStart() <= obj->GetRange().GetStart() && range.GetEnd() >= obj->GetRange().GetEnd()))
            {
                // An empty paragraph has length 1, so won't be deleted unless the
                // whole range is deleted.
                RemoveChild(obj, true);
            }
        }

        node = next;
    }

    return true;
}

/// Get any text in this object for the given range
wxString wxRichTextCompositeObject::GetTextForRange(const wxRichTextRange& range) const
{
    wxString text;
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        wxRichTextRange childRange = range;
        if (!child->GetRange().IsOutside(range))
        {
            childRange.LimitTo(child->GetRange());

            wxString childText = child->GetTextForRange(childRange);

            text += childText;
        }
        node = node->GetNext();
    }

    return text;
}

/// Get the child object at the given character position
wxRichTextObject* wxRichTextCompositeObject::GetChildAtPosition(long pos) const
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        if (child->GetRange().GetStart() == pos)
            return child;
        node = node->GetNext();
    }
    return NULL;
}

/// Recursively merge all pieces that can be merged.
bool wxRichTextCompositeObject::Defragment(const wxRichTextRange& range)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        if (range == wxRICHTEXT_ALL || !child->GetRange().IsOutside(range))
        {
            wxRichTextCompositeObject* composite = wxDynamicCast(child, wxRichTextCompositeObject);
            if (composite)
                composite->Defragment();

            if (node->GetNext())
            {
                wxRichTextObject* nextChild = node->GetNext()->GetData();
                if (child->CanMerge(nextChild) && child->Merge(nextChild))
                {
                    nextChild->Dereference();
                    m_children.Erase(node->GetNext());

                    // Don't set node -- we'll see if we can merge again with the next
                    // child.
                }
                else
                    node = node->GetNext();
            }
            else
                node = node->GetNext();
        }
        else
            node = node->GetNext();
    }

    // Delete any remaining empty objects, but leave at least one empty object per composite object.
    if (GetChildCount() > 1)
    {
        node = m_children.GetFirst();
        while (node)
        {
            wxRichTextObjectList::compatibility_iterator next = node->GetNext();
            wxRichTextObject* child = node->GetData();
            if (range == wxRICHTEXT_ALL || !child->GetRange().IsOutside(range))
            {
                if (child->IsEmpty())
                {
                    child->Dereference();
                    m_children.Erase(node);
                }
                node = next;
            }
            else
                node = node->GetNext();
        }
    }

    return true;
}

/// Dump to output stream for debugging
void wxRichTextCompositeObject::Dump(wxTextOutputStream& stream)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        child->Dump(stream);
        node = node->GetNext();
    }
}

/// Get/set the object size for the given range. Returns false if the range
/// is invalid for this object.
bool wxRichTextCompositeObject::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position, wxArrayInt* partialExtents) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    wxSize sz;

    wxArrayInt childExtents;
    wxArrayInt* p;
    if (partialExtents)
        p = & childExtents;
    else
        p = NULL;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        if (!child->GetRange().IsOutside(range))
        {
            // Floating objects have a zero size within the paragraph.
            if (child->IsFloating())
            {
                if (partialExtents)
                {
                    int lastSize;
                    if (partialExtents->GetCount() > 0)
                        lastSize = (*partialExtents)[partialExtents->GetCount()-1];
                    else
                        lastSize = 0;

                    partialExtents->Add(0 /* zero size */ + lastSize);
                }
            }
            else
            {
                wxSize childSize;

                wxRichTextRange rangeToUse = range;
                rangeToUse.LimitTo(child->GetRange());
                if (child->IsTopLevel())
                    rangeToUse = child->GetOwnRange();

                int childDescent = 0;

                // At present wxRICHTEXT_HEIGHT_ONLY is only fast if we're already cached the size,
                // but it's only going to be used after caching has taken place.
                if ((flags & wxRICHTEXT_HEIGHT_ONLY) && child->GetCachedSize().y != 0)
                {
                    childDescent = child->GetDescent();
                    childSize = child->GetCachedSize();

                    sz.y = wxMax(sz.y, childSize.y);
                    sz.x += childSize.x;
                    descent = wxMax(descent, childDescent);
                }
                else if (child->GetRangeSize(rangeToUse, childSize, childDescent, dc, flags, wxPoint(position.x + sz.x, position.y), p))
                {
                    sz.y = wxMax(sz.y, childSize.y);
                    sz.x += childSize.x;
                    descent = wxMax(descent, childDescent);

                    if ((flags & wxRICHTEXT_CACHE_SIZE) && (rangeToUse == child->GetRange() || child->IsTopLevel()))
                    {
                        child->SetCachedSize(childSize);
                        child->SetDescent(childDescent);
                    }

                    if (partialExtents)
                    {
                        int lastSize;
                        if (partialExtents->GetCount() > 0)
                            lastSize = (*partialExtents)[partialExtents->GetCount()-1];
                        else
                            lastSize = 0;

                        size_t i;
                        for (i = 0; i < childExtents.GetCount(); i++)
                        {
                            partialExtents->Add(childExtents[i] + lastSize);
                        }
                    }
                }
            }

            if (p)
                p->Clear();
        }

        node = node->GetNext();
    }
    size = sz;
    return true;
}

// Invalidate the buffer. With no argument, invalidates whole buffer.
void wxRichTextCompositeObject::Invalidate(const wxRichTextRange& invalidRange)
{
    wxRichTextObject::Invalidate(invalidRange);

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        if (invalidRange != wxRICHTEXT_ALL && invalidRange != wxRICHTEXT_NONE && child->GetRange().IsOutside(invalidRange))
        {
            // Skip
        }
        else if (child->IsTopLevel())
        {
            if (invalidRange == wxRICHTEXT_NONE)
                child->Invalidate(wxRICHTEXT_NONE);
            else
                child->Invalidate(wxRICHTEXT_ALL); // All children must be invalidated if within parent range
        }
        else
            child->Invalidate(invalidRange);
        node = node->GetNext();
    }
}

// Move the object recursively, by adding the offset from old to new
void wxRichTextCompositeObject::Move(const wxPoint& pt)
{
    wxPoint oldPos = GetPosition();
    SetPosition(pt);
    wxPoint offset = pt - oldPos;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        wxPoint childPos = child->GetPosition() + offset;
        child->Move(childPos);
        node = node->GetNext();
    }
}


/*!
 * wxRichTextParagraphLayoutBox
 * This box knows how to lay out paragraphs.
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextParagraphLayoutBox, wxRichTextCompositeObject)

wxRichTextParagraphLayoutBox::wxRichTextParagraphLayoutBox(wxRichTextObject* parent):
    wxRichTextCompositeObject(parent)
{
    Init();
}

wxRichTextParagraphLayoutBox::~wxRichTextParagraphLayoutBox()
{
    if (m_floatCollector)
    {
        delete m_floatCollector;
        m_floatCollector = NULL;
    }
}

/// Initialize the object.
void wxRichTextParagraphLayoutBox::Init()
{
    m_ctrl = NULL;

    // For now, assume is the only box and has no initial size.
    m_range = wxRichTextRange(0, -1);
    m_ownRange = wxRichTextRange(0, -1);

    m_invalidRange = wxRICHTEXT_ALL;

    SetMargins(4);
    m_partialParagraph = false;
    m_floatCollector = NULL;
}

void wxRichTextParagraphLayoutBox::Clear()
{
    DeleteChildren();

    if (m_floatCollector)
        delete m_floatCollector;
    m_floatCollector = NULL;
    m_partialParagraph = false;
}

/// Copy
void wxRichTextParagraphLayoutBox::Copy(const wxRichTextParagraphLayoutBox& obj)
{
    Clear();

    wxRichTextCompositeObject::Copy(obj);

    m_partialParagraph = obj.m_partialParagraph;
    m_defaultAttributes = obj.m_defaultAttributes;
}

// Gather information about floating objects; only gather floats for those paragraphs that
// will not be formatted again due to optimization, after which floats will be gathered per-paragraph
// during layout.
bool wxRichTextParagraphLayoutBox::UpdateFloatingObjects(const wxRect& availableRect, wxRichTextObject* untilObj)
{
    if (m_floatCollector != NULL)
        delete m_floatCollector;
    m_floatCollector = new wxRichTextFloatCollector(availableRect);
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    // Only gather floats up to the point we'll start formatting paragraphs.
    while (untilObj && node && node->GetData() != untilObj)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (child != NULL);
        if (child)
            m_floatCollector->CollectFloat(child);
        node = node->GetNext();
    }

    return true;
}

// Returns the style sheet associated with the overall buffer.
wxRichTextStyleSheet* wxRichTextParagraphLayoutBox::GetStyleSheet() const
{
    return GetBuffer() ? GetBuffer()->GetStyleSheet() : (wxRichTextStyleSheet*) NULL;
}

// Get the number of floating objects at this level
int wxRichTextParagraphLayoutBox::GetFloatingObjectCount() const
{
    if (m_floatCollector)
        return m_floatCollector->GetFloatingObjectCount();
    else
        return 0;
}

// Get a list of floating objects
bool wxRichTextParagraphLayoutBox::GetFloatingObjects(wxRichTextObjectList& objects) const
{
    if (m_floatCollector)
    {
        return m_floatCollector->GetFloatingObjects(objects);
    }
    else
        return false;
}

// Calculate ranges
void wxRichTextParagraphLayoutBox::UpdateRanges()
{
    long start = 0;
    if (GetParent())
        start = GetRange().GetStart();
    long end;
    CalculateRange(start, end);
}

// HitTest
int wxRichTextParagraphLayoutBox::HitTest(wxDC& dc, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags)
{
    if (!IsShown())
        return wxRICHTEXT_HITTEST_NONE;

    int ret = wxRICHTEXT_HITTEST_NONE;
    if (m_floatCollector && (flags & wxRICHTEXT_HITTEST_NO_FLOATING_OBJECTS) == 0)
        ret = m_floatCollector->HitTest(dc, pt, textPosition, obj, flags);

    if (ret == wxRICHTEXT_HITTEST_NONE)
        return wxRichTextCompositeObject::HitTest(dc, pt, textPosition, obj, contextObj, flags);
    else
    {
        *contextObj = this;
        return ret;
    }
}

/// Draw the floating objects
void wxRichTextParagraphLayoutBox::DrawFloats(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    if (m_floatCollector)
        m_floatCollector->Draw(dc, range, selection, rect, descent, style);
}

void wxRichTextParagraphLayoutBox::MoveAnchoredObjectToParagraph(wxRichTextParagraph* from, wxRichTextParagraph* to, wxRichTextObject* obj)
{
    if (from == to)
        return;

    from->RemoveChild(obj);
    to->AppendChild(obj);
}

/// Draw the item
bool wxRichTextParagraphLayoutBox::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    if (!IsShown())
        return true;

    wxRect thisRect(GetPosition(), GetCachedSize());

    int flags = style;
    if (selection.IsValid() && GetParentContainer() != this && selection.WithinSelection(GetRange().GetStart(), GetParentContainer()))
        flags |= wxRICHTEXT_DRAW_SELECTED;

    // Don't draw guidelines if at top level
    int theseFlags = flags;
    if (!GetParent())
        theseFlags &= ~wxRICHTEXT_DRAW_GUIDELINES;
    DrawBoxAttributes(dc, GetBuffer(), GetAttributes(), thisRect, theseFlags);

    DrawFloats(dc, range, selection, rect, descent, style);
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        if (child && !child->GetRange().IsOutside(range))
        {
            wxRect childRect(child->GetPosition(), child->GetCachedSize());
            wxRichTextRange childRange = range;
            if (child->IsTopLevel())
            {
                childRange = child->GetOwnRange();
            }

            if (((style & wxRICHTEXT_DRAW_IGNORE_CACHE) == 0) && childRect.GetTop() > rect.GetBottom())
            {
                // Stop drawing
                break;
            }
            else if (((style & wxRICHTEXT_DRAW_IGNORE_CACHE) == 0) && childRect.GetBottom() < rect.GetTop())
            {
                // Skip
            }
            else
                child->Draw(dc, childRange, selection, rect, descent, style);
        }

        node = node->GetNext();
    }
    return true;
}

/// Lay the item out
bool wxRichTextParagraphLayoutBox::Layout(wxDC& dc, const wxRect& rect, int style)
{
    SetPosition(rect.GetPosition());

    if (!IsShown())
        return true;

    wxRect availableSpace;
    bool formatRect = (style & wxRICHTEXT_LAYOUT_SPECIFIED_RECT) == wxRICHTEXT_LAYOUT_SPECIFIED_RECT;

    // If only laying out a specific area, the passed rect has a different meaning:
    // the visible part of the buffer. This is used in wxRichTextCtrl::OnSize,
    // so that during a size, only the visible part will be relaid out, or
    // it would take too long causing flicker. As an approximation, we assume that
    // everything up to the start of the visible area is laid out correctly.
    if (formatRect)
    {
        wxRect rect2(0, 0, rect.width, rect.height);
        availableSpace = GetAvailableContentArea(dc, rect2);

        // Invalidate the part of the buffer from the first visible line
        // to the end. If other parts of the buffer are currently invalid,
        // then they too will be taken into account if they are above
        // the visible point.
        long startPos = 0;
        wxRichTextLine* line = GetLineAtYPosition(rect.y);
        if (line)
            startPos = line->GetAbsoluteRange().GetStart();

        Invalidate(wxRichTextRange(startPos, GetOwnRange().GetEnd()));
    }
    else
    {
        availableSpace = GetAvailableContentArea(dc, rect);
    }

    int leftMargin, rightMargin, topMargin, bottomMargin;
    wxRichTextObject::GetTotalMargin(dc, GetBuffer(), GetAttributes(), leftMargin, rightMargin,
            topMargin, bottomMargin);

    int maxWidth = 0;
    int maxHeight = 0;

    // The maximum paragraph maximum width, so we can set the overall maximum width for this object
    int maxMaxWidth = 0;

    // The maximum paragraph minimum width, so we can set the overall minimum width for this object
    int maxMinWidth = 0;

    // If we have vertical alignment, we must recalculate everything.
    bool hasVerticalAlignment = (GetAttributes().GetTextBoxAttr().HasVerticalAlignment() &&
        (GetAttributes().GetTextBoxAttr().GetVerticalAlignment() > wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_TOP));

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();

    bool layoutAll = true;

    // Get invalid range, rounding to paragraph start/end.
    wxRichTextRange invalidRange = GetInvalidRange(true);

    if (invalidRange == wxRICHTEXT_NONE && !formatRect)
        return true;

    if (invalidRange == wxRICHTEXT_ALL || hasVerticalAlignment)
        layoutAll = true;
    else    // If we know what range is affected, start laying out from that point on.
        if (invalidRange.GetStart() >= GetOwnRange().GetStart())
    {
        wxRichTextParagraph* firstParagraph = GetParagraphAtPosition(invalidRange.GetStart());
        if (firstParagraph)
        {
            wxRichTextObjectList::compatibility_iterator firstNode = m_children.Find(firstParagraph);
            wxRichTextObjectList::compatibility_iterator previousNode;
            if ( firstNode )
                previousNode = firstNode->GetPrevious();
            if (firstNode)
            {
                if (previousNode)
                {
                    wxRichTextParagraph* previousParagraph = wxDynamicCast(previousNode->GetData(), wxRichTextParagraph);
                    availableSpace.y = previousParagraph->GetPosition().y + previousParagraph->GetCachedSize().y;
                }

                // Now we're going to start iterating from the first affected paragraph.
                node = firstNode;

                layoutAll = false;
            }
        }
    }

    // Gather information about only those floating objects that will not be formatted,
    // after which floats will be gathered per-paragraph during layout.
    UpdateFloatingObjects(availableSpace, node ? node->GetData() : (wxRichTextObject*) NULL);

    // A way to force speedy rest-of-buffer layout (the 'else' below)
    bool forceQuickLayout = false;

    while (node)
    {
        // Assume this box only contains paragraphs

        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // Unsure if this is needed
        // wxCHECK_MSG( child, false, wxT("Unknown object in layout") );

        if (child && child->IsShown())
        {
            // TODO: what if the child hasn't been laid out (e.g. involved in Undo) but still has 'old' lines
            if ( !forceQuickLayout &&
                    (layoutAll ||
                        child->GetLines().IsEmpty() ||
                            !child->GetRange().IsOutside(invalidRange)) )
            {
                // Lays out the object first with a given amount of space, and then if no width was specified in attr,
                // lays out the object again using the minimum size
                child->LayoutToBestSize(dc, GetBuffer(),
                        GetAttributes(), child->GetAttributes(), availableSpace, style&~wxRICHTEXT_LAYOUT_SPECIFIED_RECT);

                // Layout must set the cached size
                availableSpace.y += child->GetCachedSize().y;
                maxWidth = wxMax(maxWidth, child->GetCachedSize().x);
                maxMinWidth = wxMax(maxMinWidth, child->GetMinSize().x);
                maxMaxWidth = wxMax(maxMaxWidth, child->GetMaxSize().x);

                // If we're just formatting the visible part of the buffer,
                // and we're now past the bottom of the window, and we don't have any
                // floating objects (since they may cause wrapping to change for the rest of the
                // the buffer), start quick layout.
                if (!hasVerticalAlignment && formatRect && child->GetPosition().y > rect.GetBottom() && GetFloatingObjectCount() == 0)
                    forceQuickLayout = true;
            }
            else
            {
                // We're outside the immediately affected range, so now let's just
                // move everything up or down. This assumes that all the children have previously
                // been laid out and have wrapped line lists associated with them.
                // TODO: check all paragraphs before the affected range.

                int inc = availableSpace.y - child->GetPosition().y;

                while (node)
                {
                    wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
                    if (child)
                    {
                        if (child->GetLines().GetCount() == 0)
                        {
                            // Lays out the object first with a given amount of space, and then if no width was specified in attr,
                            // lays out the object again using the minimum size
                            child->LayoutToBestSize(dc, GetBuffer(),
                                        GetAttributes(), child->GetAttributes(), availableSpace, style&~wxRICHTEXT_LAYOUT_SPECIFIED_RECT);

                            //child->Layout(dc, availableChildRect, style);
                        }
                        else
                            child->Move(wxPoint(child->GetPosition().x, child->GetPosition().y + inc));

                        availableSpace.y += child->GetCachedSize().y;
                        maxWidth = wxMax(maxWidth, child->GetCachedSize().x);
                        maxMinWidth = wxMax(maxMinWidth, child->GetMinSize().x);
                        maxMaxWidth = wxMax(maxMaxWidth, child->GetMaxSize().x);
                    }

                    node = node->GetNext();
                }
                break;
            }
        }

        node = node->GetNext();
    }

    node = m_children.GetLast();
    if (node && node->GetData()->IsShown())
    {
        wxRichTextObject* child = node->GetData();
        // maxHeight = (child->GetPosition().y - GetPosition().y) + child->GetCachedSize().y;
        maxHeight = child->GetPosition().y - (GetPosition().y + topMargin) + child->GetCachedSize().y;
    }
    else
        maxHeight = 0; // topMargin + bottomMargin;

    // TODO: (also in para layout) should set the
    // object's size to an absolute one if specified,
    // but if not specified, calculate it from content.

    // We need to add back the margins etc.
    {
        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(maxWidth, maxHeight));
        GetBoxRects(dc, GetBuffer(), GetAttributes(), marginRect, borderRect, contentRect, paddingRect, outlineRect);
        SetCachedSize(marginRect.GetSize());
    }

    // The maximum size is the greatest of all maximum widths for all paragraphs.
    {
        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(maxMaxWidth, maxHeight)); // Actually max height is a lie, we can't know it
        GetBoxRects(dc, GetBuffer(), GetAttributes(), marginRect, borderRect, contentRect, paddingRect, outlineRect);
        SetMaxSize(marginRect.GetSize());
    }

    // The minimum size is the greatest of all minimum widths for all paragraphs.
    {
        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(maxMinWidth, maxHeight)); // Actually max height is a lie, we can't know it
        GetBoxRects(dc, GetBuffer(), GetAttributes(), marginRect, borderRect, contentRect, paddingRect, outlineRect);
        SetMinSize(marginRect.GetSize());
    }

    if (GetAttributes().GetTextBoxAttr().HasVerticalAlignment() &&
        (GetAttributes().GetTextBoxAttr().GetVerticalAlignment() > wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_TOP))
    {
        int yOffset = 0;
        int leftOverSpace = availableSpace.height - topMargin - bottomMargin - maxHeight;
        if (leftOverSpace > 0)
        {
            if (GetAttributes().GetTextBoxAttr().GetVerticalAlignment() == wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_CENTRE)
            {
                yOffset = (leftOverSpace/2);
            }
            else if (GetAttributes().GetTextBoxAttr().GetVerticalAlignment() == wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_BOTTOM)
            {
                yOffset = leftOverSpace;
            }
        }

        // Move all the children to vertically align the content
        // This doesn't take into account floating objects, unfortunately.
        if (yOffset != 0)
        {
            wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
            while (node)
            {
                wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
                if (child)
                    child->Move(wxPoint(child->GetPosition().x, child->GetPosition().y + yOffset));

                node = node->GetNext();
            }
        }
    }

    m_invalidRange = wxRICHTEXT_NONE;

    return true;
}

/// Get/set the size for the given range.
bool wxRichTextParagraphLayoutBox::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position, wxArrayInt* WXUNUSED(partialExtents)) const
{
    wxSize sz;

    wxRichTextObjectList::compatibility_iterator startPara = wxRichTextObjectList::compatibility_iterator();
    wxRichTextObjectList::compatibility_iterator endPara = wxRichTextObjectList::compatibility_iterator();

    // First find the first paragraph whose starting position is within the range.
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        // child is a paragraph
        wxRichTextObject* child = node->GetData();
        const wxRichTextRange& r = child->GetRange();

        if (r.GetStart() <= range.GetStart() && r.GetEnd() >= range.GetStart())
        {
            startPara = node;
            break;
        }

        node = node->GetNext();
    }

    // Next find the last paragraph containing part of the range
    node = m_children.GetFirst();
    while (node)
    {
        // child is a paragraph
        wxRichTextObject* child = node->GetData();
        const wxRichTextRange& r = child->GetRange();

        if (r.GetStart() <= range.GetEnd() && r.GetEnd() >= range.GetEnd())
        {
            endPara = node;
            break;
        }

        node = node->GetNext();
    }

    if (!startPara || !endPara)
        return false;

    // Now we can add up the sizes
    for (node = startPara; node ; node = node->GetNext())
    {
        // child is a paragraph
        wxRichTextObject* child = node->GetData();
        const wxRichTextRange& childRange = child->GetRange();
        wxRichTextRange rangeToFind = range;
        rangeToFind.LimitTo(childRange);

        if (child->IsTopLevel())
            rangeToFind = child->GetOwnRange();

        wxSize childSize;

        int childDescent = 0;
        child->GetRangeSize(rangeToFind, childSize, childDescent, dc, flags, position);

        descent = wxMax(childDescent, descent);

        sz.x = wxMax(sz.x, childSize.x);
        sz.y += childSize.y;

        if (node == endPara)
            break;
    }

    size = sz;

    return true;
}

/// Get the paragraph at the given position
wxRichTextParagraph* wxRichTextParagraphLayoutBox::GetParagraphAtPosition(long pos, bool caretPosition) const
{
    if (caretPosition)
        pos ++;

    // First find the first paragraph whose starting position is within the range.
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        // child is a paragraph
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (child != NULL);

        if (child)
        {
            // Return first child in buffer if position is -1
            // if (pos == -1)
            //    return child;

            if (child->GetRange().Contains(pos))
                return child;
        }

        node = node->GetNext();
    }
    return NULL;
}

/// Get the line at the given position
wxRichTextLine* wxRichTextParagraphLayoutBox::GetLineAtPosition(long pos, bool caretPosition) const
{
    if (caretPosition)
        pos ++;

    // First find the first paragraph whose starting position is within the range.
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* obj = (wxRichTextObject*) node->GetData();
        if (obj->GetRange().Contains(pos))
        {
            // child is a paragraph
            wxRichTextParagraph* child = wxDynamicCast(obj, wxRichTextParagraph);
            // wxASSERT (child != NULL);

            if (child)
            {
                wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
                while (node2)
                {
                    wxRichTextLine* line = node2->GetData();

                    wxRichTextRange range = line->GetAbsoluteRange();

                    if (range.Contains(pos) ||

                        // If the position is end-of-paragraph, then return the last line of
                        // of the paragraph.
                        ((range.GetEnd() == child->GetRange().GetEnd()-1) && (pos == child->GetRange().GetEnd())))
                        return line;

                    node2 = node2->GetNext();
                }
            }
        }

        node = node->GetNext();
    }

    int lineCount = GetLineCount();
    if (lineCount > 0)
        return GetLineForVisibleLineNumber(lineCount-1);
    else
        return NULL;
}

/// Get the line at the given y pixel position, or the last line.
wxRichTextLine* wxRichTextParagraphLayoutBox::GetLineAtYPosition(int y) const
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (child != NULL);

        if (child)
        {
            wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
            while (node2)
            {
                wxRichTextLine* line = node2->GetData();

                wxRect rect(line->GetRect());

                if (y <= rect.GetBottom())
                    return line;

                node2 = node2->GetNext();
            }
        }

        node = node->GetNext();
    }

    // Return last line
    int lineCount = GetLineCount();
    if (lineCount > 0)
        return GetLineForVisibleLineNumber(lineCount-1);
    else
        return NULL;
}

/// Get the number of visible lines
int wxRichTextParagraphLayoutBox::GetLineCount() const
{
    int count = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (child != NULL);

        if (child)
            count += child->GetLines().GetCount();

        node = node->GetNext();
    }
    return count;
}


/// Get the paragraph for a given line
wxRichTextParagraph* wxRichTextParagraphLayoutBox::GetParagraphForLine(wxRichTextLine* line) const
{
    return GetParagraphAtPosition(line->GetAbsoluteRange().GetStart());
}

/// Get the line size at the given position
wxSize wxRichTextParagraphLayoutBox::GetLineSizeAtPosition(long pos, bool caretPosition) const
{
    wxRichTextLine* line = GetLineAtPosition(pos, caretPosition);
    if (line)
    {
        return line->GetSize();
    }
    else
        return wxSize(0, 0);
}


/// Convenience function to add a paragraph of text
wxRichTextRange wxRichTextParagraphLayoutBox::AddParagraph(const wxString& text, wxRichTextAttr* paraStyle)
{
    // Don't use the base style, just the default style, and the base style will
    // be combined at display time.
    // Divide into paragraph and character styles.

    wxRichTextAttr defaultCharStyle;
    wxRichTextAttr defaultParaStyle;

    // If the default style is a named paragraph style, don't apply any character formatting
    // to the initial text string.
    if (GetDefaultStyle().HasParagraphStyleName() && GetStyleSheet())
    {
        wxRichTextParagraphStyleDefinition* def = GetStyleSheet()->FindParagraphStyle(GetDefaultStyle().GetParagraphStyleName());
        if (def)
            defaultParaStyle = def->GetStyleMergedWithBase(GetStyleSheet());
    }
    else
        wxRichTextSplitParaCharStyles(GetDefaultStyle(), defaultParaStyle, defaultCharStyle);

    wxRichTextAttr* pStyle = paraStyle ? paraStyle : (wxRichTextAttr*) & defaultParaStyle;
    wxRichTextAttr* cStyle = & defaultCharStyle;

    wxRichTextParagraph* para = new wxRichTextParagraph(text, this, pStyle, cStyle);

    AppendChild(para);

    UpdateRanges();

    return para->GetRange();
}

/// Adds multiple paragraphs, based on newlines.
wxRichTextRange wxRichTextParagraphLayoutBox::AddParagraphs(const wxString& text, wxRichTextAttr* paraStyle)
{
    // Don't use the base style, just the default style, and the base style will
    // be combined at display time.
    // Divide into paragraph and character styles.

    wxRichTextAttr defaultCharStyle;
    wxRichTextAttr defaultParaStyle;

    // If the default style is a named paragraph style, don't apply any character formatting
    // to the initial text string.
    if (GetDefaultStyle().HasParagraphStyleName() && GetStyleSheet())
    {
        wxRichTextParagraphStyleDefinition* def = GetStyleSheet()->FindParagraphStyle(GetDefaultStyle().GetParagraphStyleName());
        if (def)
            defaultParaStyle = def->GetStyleMergedWithBase(GetStyleSheet());
    }
    else
        wxRichTextSplitParaCharStyles(GetDefaultStyle(), defaultParaStyle, defaultCharStyle);

    wxRichTextAttr* pStyle = paraStyle ? paraStyle : (wxRichTextAttr*) & defaultParaStyle;
    wxRichTextAttr* cStyle = & defaultCharStyle;

    wxRichTextParagraph* firstPara = NULL;
    wxRichTextParagraph* lastPara = NULL;

    wxRichTextRange range(-1, -1);

    size_t i = 0;
    size_t len = text.length();
    wxString line;
    wxRichTextParagraph* para = new wxRichTextParagraph(wxEmptyString, this, pStyle, cStyle);

    AppendChild(para);

    firstPara = para;
    lastPara = para;

    while (i < len)
    {
        wxChar ch = text[i];
        if (ch == wxT('\n') || ch == wxT('\r'))
        {
            if (i != (len-1))
            {
                wxRichTextPlainText* plainText = (wxRichTextPlainText*) para->GetChildren().GetFirst()->GetData();
                plainText->SetText(line);

                para = new wxRichTextParagraph(wxEmptyString, this, pStyle, cStyle);

                AppendChild(para);

                lastPara = para;
                line = wxEmptyString;
            }
        }
        else
            line += ch;

        i ++;
    }

    if (!line.empty())
    {
        wxRichTextPlainText* plainText = (wxRichTextPlainText*) para->GetChildren().GetFirst()->GetData();
        plainText->SetText(line);
    }

    UpdateRanges();

    return wxRichTextRange(firstPara->GetRange().GetStart(), lastPara->GetRange().GetEnd());
}

/// Convenience function to add an image
wxRichTextRange wxRichTextParagraphLayoutBox::AddImage(const wxImage& image, wxRichTextAttr* paraStyle)
{
    // Don't use the base style, just the default style, and the base style will
    // be combined at display time.
    // Divide into paragraph and character styles.

    wxRichTextAttr defaultCharStyle;
    wxRichTextAttr defaultParaStyle;

    // If the default style is a named paragraph style, don't apply any character formatting
    // to the initial text string.
    if (GetDefaultStyle().HasParagraphStyleName() && GetStyleSheet())
    {
        wxRichTextParagraphStyleDefinition* def = GetStyleSheet()->FindParagraphStyle(GetDefaultStyle().GetParagraphStyleName());
        if (def)
            defaultParaStyle = def->GetStyleMergedWithBase(GetStyleSheet());
    }
    else
        wxRichTextSplitParaCharStyles(GetDefaultStyle(), defaultParaStyle, defaultCharStyle);

    wxRichTextAttr* pStyle = paraStyle ? paraStyle : (wxRichTextAttr*) & defaultParaStyle;
    wxRichTextAttr* cStyle = & defaultCharStyle;

    wxRichTextParagraph* para = new wxRichTextParagraph(this, pStyle);
    AppendChild(para);
    para->AppendChild(new wxRichTextImage(image, this, cStyle));

    UpdateRanges();

    return para->GetRange();
}


/// Insert fragment into this box at the given position. If partialParagraph is true,
/// it is assumed that the last (or only) paragraph is just a piece of data with no paragraph
/// marker.

bool wxRichTextParagraphLayoutBox::InsertFragment(long position, wxRichTextParagraphLayoutBox& fragment)
{
    // First, find the first paragraph whose starting position is within the range.
    wxRichTextParagraph* para = GetParagraphAtPosition(position);
    if (para)
    {
        wxRichTextAttr originalAttr = para->GetAttributes();

        wxRichTextObjectList::compatibility_iterator node = m_children.Find(para);

        // Now split at this position, returning the object to insert the new
        // ones in front of.
        wxRichTextObject* nextObject = para->SplitAt(position);

        // Special case: partial paragraph, just one paragraph. Might be a small amount of
        // text, for example, so let's optimize.

        if (fragment.GetPartialParagraph() && fragment.GetChildren().GetCount() == 1)
        {
            // Add the first para to this para...
            wxRichTextObjectList::compatibility_iterator firstParaNode = fragment.GetChildren().GetFirst();
            if (!firstParaNode)
                return false;

            // Iterate through the fragment paragraph inserting the content into this paragraph.
            wxRichTextParagraph* firstPara = wxDynamicCast(firstParaNode->GetData(), wxRichTextParagraph);
            wxASSERT (firstPara != NULL);

            wxRichTextObjectList::compatibility_iterator objectNode = firstPara->GetChildren().GetFirst();
            while (objectNode)
            {
                wxRichTextObject* newObj = objectNode->GetData()->Clone();

                if (!nextObject)
                {
                    // Append
                    para->AppendChild(newObj);
                }
                else
                {
                    // Insert before nextObject
                    para->InsertChild(newObj, nextObject);
                }

                objectNode = objectNode->GetNext();
            }

            return true;
        }
        else
        {
            // Procedure for inserting a fragment consisting of a number of
            // paragraphs:
            //
            // 1. Remove and save the content that's after the insertion point, for adding
            //    back once we've added the fragment.
            // 2. Add the content from the first fragment paragraph to the current
            //    paragraph.
            // 3. Add remaining fragment paragraphs after the current paragraph.
            // 4. Add back the saved content from the first paragraph. If partialParagraph
            //    is true, add it to the last paragraph added and not a new one.

            // 1. Remove and save objects after split point.
            wxList savedObjects;
            if (nextObject)
                para->MoveToList(nextObject, savedObjects);

            // 2. Add the content from the 1st fragment paragraph.
            wxRichTextObjectList::compatibility_iterator firstParaNode = fragment.GetChildren().GetFirst();
            if (!firstParaNode)
                return false;

            wxRichTextParagraph* firstPara = wxDynamicCast(firstParaNode->GetData(), wxRichTextParagraph);
            wxASSERT(firstPara != NULL);

            if (!(fragment.GetAttributes().GetFlags() & wxTEXT_ATTR_KEEP_FIRST_PARA_STYLE))
                para->SetAttributes(firstPara->GetAttributes());

            // Save empty paragraph attributes for appending later
            // These are character attributes deliberately set for a new paragraph. Without this,
            // we couldn't pass default attributes when appending a new paragraph.
            wxRichTextAttr emptyParagraphAttributes;

            wxRichTextObjectList::compatibility_iterator objectNode = firstPara->GetChildren().GetFirst();

            if (objectNode && firstPara->GetChildren().GetCount() == 1 && objectNode->GetData()->IsEmpty())
                emptyParagraphAttributes = objectNode->GetData()->GetAttributes();

            while (objectNode)
            {
                wxRichTextObject* newObj = objectNode->GetData()->Clone();

                // Append
                para->AppendChild(newObj);

                objectNode = objectNode->GetNext();
            }

            // 3. Add remaining fragment paragraphs after the current paragraph.
            wxRichTextObjectList::compatibility_iterator nextParagraphNode = node->GetNext();
            wxRichTextObject* nextParagraph = NULL;
            if (nextParagraphNode)
                nextParagraph = nextParagraphNode->GetData();

            wxRichTextObjectList::compatibility_iterator i = fragment.GetChildren().GetFirst()->GetNext();
            wxRichTextParagraph* finalPara = para;

            bool needExtraPara = (!i || !fragment.GetPartialParagraph());

            // If there was only one paragraph, we need to insert a new one.
            while (i)
            {
                wxRichTextParagraph* para = wxDynamicCast(i->GetData(), wxRichTextParagraph);
                wxASSERT( para != NULL );

                finalPara = (wxRichTextParagraph*) para->Clone();

                if (nextParagraph)
                    InsertChild(finalPara, nextParagraph);
                else
                    AppendChild(finalPara);

                i = i->GetNext();
            }

            // If there was only one paragraph, or we have full paragraphs in our fragment,
            // we need to insert a new one.
            if (needExtraPara)
            {
                finalPara = new wxRichTextParagraph;

                if (nextParagraph)
                    InsertChild(finalPara, nextParagraph);
                else
                    AppendChild(finalPara);
            }

            // 4. Add back the remaining content.
            if (finalPara)
            {
                if (nextObject)
                    finalPara->MoveFromList(savedObjects);

                // Ensure there's at least one object
                if (finalPara->GetChildCount() == 0)
                {
                    wxRichTextPlainText* text = new wxRichTextPlainText(wxEmptyString);
                    text->SetAttributes(emptyParagraphAttributes);

                    finalPara->AppendChild(text);
                }
            }

            if ((fragment.GetAttributes().GetFlags() & wxTEXT_ATTR_KEEP_FIRST_PARA_STYLE) && firstPara)
                finalPara->SetAttributes(firstPara->GetAttributes());
            else if (finalPara && finalPara != para)
                finalPara->SetAttributes(originalAttr);

            return true;
        }
    }
    else
    {
        // Append
        wxRichTextObjectList::compatibility_iterator i = fragment.GetChildren().GetFirst();
        while (i)
        {
            wxRichTextParagraph* para = wxDynamicCast(i->GetData(), wxRichTextParagraph);
            wxASSERT( para != NULL );

            AppendChild(para->Clone());

            i = i->GetNext();
        }

        return true;
    }
}

/// Make a copy of the fragment corresponding to the given range, putting it in 'fragment'.
/// If there was an incomplete paragraph at the end, partialParagraph is set to true.
bool wxRichTextParagraphLayoutBox::CopyFragment(const wxRichTextRange& range, wxRichTextParagraphLayoutBox& fragment)
{
    wxRichTextObjectList::compatibility_iterator i = GetChildren().GetFirst();
    while (i)
    {
        wxRichTextParagraph* para = wxDynamicCast(i->GetData(), wxRichTextParagraph);
        wxASSERT( para != NULL );

        if (!para->GetRange().IsOutside(range))
        {
            fragment.AppendChild(para->Clone());
        }
        i = i->GetNext();
    }

    // Now top and tail the first and last paragraphs in our new fragment (which might be the same).
    if (!fragment.IsEmpty())
    {
        wxRichTextParagraph* firstPara = wxDynamicCast(fragment.GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
        wxASSERT( firstPara != NULL );

        wxRichTextParagraph* lastPara = wxDynamicCast(fragment.GetChildren().GetLast()->GetData(), wxRichTextParagraph);
        wxASSERT( lastPara != NULL );

        if (!firstPara || !lastPara)
            return false;

        bool isFragment = (range.GetEnd() < lastPara->GetRange().GetEnd());

        long firstPos = firstPara->GetRange().GetStart();

        // Adjust for renumbering from zero
        wxRichTextRange topTailRange(range.GetStart() - firstPos, range.GetEnd() - firstPos);

        long end;
        fragment.CalculateRange(0, end);

        // Chop off the start of the paragraph
        if (topTailRange.GetStart() > 0)
        {
            wxRichTextRange r(0, topTailRange.GetStart()-1);
            firstPara->DeleteRange(r);

            // Make sure the numbering is correct
            fragment.CalculateRange(0, end);

            // Now, we've deleted some positions, so adjust the range
            // accordingly.
            topTailRange.SetStart(range.GetLength());
            topTailRange.SetEnd(fragment.GetOwnRange().GetEnd());
        }
        else
        {
            topTailRange.SetStart(range.GetLength());
            topTailRange.SetEnd(fragment.GetOwnRange().GetEnd());
        }

        if (topTailRange.GetStart() < (lastPara->GetRange().GetEnd()-1))
        {
            lastPara->DeleteRange(topTailRange);

            // Make sure the numbering is correct
            long end;
            fragment.CalculateRange(0, end);

            // We only have part of a paragraph at the end
            fragment.SetPartialParagraph(true);
        }
        else
        {
            // We have a partial paragraph (don't save last new paragraph marker)
            // or complete paragraph
            fragment.SetPartialParagraph(isFragment);
        }
    }

    return true;
}

/// Given a position, get the number of the visible line (potentially many to a paragraph),
/// starting from zero at the start of the buffer.
long wxRichTextParagraphLayoutBox::GetVisibleLineNumber(long pos, bool caretPosition, bool startOfLine) const
{
    if (caretPosition)
        pos ++;

    int lineCount = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT( child != NULL );

        if (child)
        {
            if (child->GetRange().Contains(pos))
            {
                wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
                while (node2)
                {
                    wxRichTextLine* line = node2->GetData();
                    wxRichTextRange lineRange = line->GetAbsoluteRange();

                    if (lineRange.Contains(pos) || pos == lineRange.GetStart())
                    {
                        // If the caret is displayed at the end of the previous wrapped line,
                        // we want to return the line it's _displayed_ at (not the actual line
                        // containing the position).
                        if (lineRange.GetStart() == pos && !startOfLine && child->GetRange().GetStart() != pos)
                            return lineCount - 1;
                        else
                            return lineCount;
                    }

                    lineCount ++;

                    node2 = node2->GetNext();
                }
                // If we didn't find it in the lines, it must be
                // the last position of the paragraph. So return the last line.
                return lineCount-1;
            }
            else
                lineCount += child->GetLines().GetCount();
        }

        node = node->GetNext();
    }

    // Not found
    return -1;
}

/// Given a line number, get the corresponding wxRichTextLine object.
wxRichTextLine* wxRichTextParagraphLayoutBox::GetLineForVisibleLineNumber(long lineNumber) const
{
    int lineCount = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT(child != NULL);

        if (child)
        {
            if (lineNumber < (int) (child->GetLines().GetCount() + lineCount))
            {
                wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
                while (node2)
                {
                    wxRichTextLine* line = node2->GetData();

                    if (lineCount == lineNumber)
                        return line;

                    lineCount ++;

                    node2 = node2->GetNext();
                }
            }
            else
                lineCount += child->GetLines().GetCount();
        }

        node = node->GetNext();
    }

    // Didn't find it
    return NULL;
}

/// Delete range from layout.
bool wxRichTextParagraphLayoutBox::DeleteRange(const wxRichTextRange& range)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();

    wxRichTextParagraph* firstPara = NULL;
    while (node)
    {
        wxRichTextParagraph* obj = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (obj != NULL);

        wxRichTextObjectList::compatibility_iterator next = node->GetNext();

        if (obj)
        {
            // Delete the range in each paragraph

            if (!obj->GetRange().IsOutside(range))
            {
                // Deletes the content of this object within the given range
                obj->DeleteRange(range);

                wxRichTextRange thisRange = obj->GetRange();
                wxRichTextAttr thisAttr = obj->GetAttributes();

                // If the whole paragraph is within the range to delete,
                // delete the whole thing.
                if (range.GetStart() <= thisRange.GetStart() && range.GetEnd() >= thisRange.GetEnd())
                {
                    // Delete the whole object
                    RemoveChild(obj, true);
                    obj = NULL;
                }
                else if (!firstPara)
                    firstPara = obj;

                // If the range includes the paragraph end, we need to join this
                // and the next paragraph.
                if (range.GetEnd() <= thisRange.GetEnd())
                {
                    // We need to move the objects from the next paragraph
                    // to this paragraph

                    wxRichTextParagraph* nextParagraph = NULL;
                    if ((range.GetEnd() < thisRange.GetEnd()) && obj)
                        nextParagraph = obj;
                    else
                    {
                        // We're ending at the end of the paragraph, so merge the _next_ paragraph.
                        if (next)
                            nextParagraph = wxDynamicCast(next->GetData(), wxRichTextParagraph);
                    }

                    bool applyFinalParagraphStyle = firstPara && nextParagraph && nextParagraph != firstPara;

                    wxRichTextAttr nextParaAttr;
                    if (applyFinalParagraphStyle)
                    {
                        // Special case when deleting the end of a paragraph - use _this_ paragraph's style,
                        // not the next one.
                        if (range.GetStart() == range.GetEnd() && range.GetStart() == thisRange.GetEnd())
                            nextParaAttr = thisAttr;
                        else
                            nextParaAttr = nextParagraph->GetAttributes();
                    }

                    if (firstPara && nextParagraph && firstPara != nextParagraph)
                    {
                        // Move the objects to the previous para
                        wxRichTextObjectList::compatibility_iterator node1 = nextParagraph->GetChildren().GetFirst();

                        while (node1)
                        {
                            wxRichTextObject* obj1 = node1->GetData();

                            firstPara->AppendChild(obj1);

                            wxRichTextObjectList::compatibility_iterator next1 = node1->GetNext();
                            nextParagraph->GetChildren().Erase(node1);

                            node1 = next1;
                        }

                        // Delete the paragraph
                        RemoveChild(nextParagraph, true);
                    }

                    // Avoid empty paragraphs
                    if (firstPara && firstPara->GetChildren().GetCount() == 0)
                    {
                        wxRichTextPlainText* text = new wxRichTextPlainText(wxEmptyString);
                        firstPara->AppendChild(text);
                    }

                    if (applyFinalParagraphStyle)
                        firstPara->SetAttributes(nextParaAttr);

                    return true;
                }
            }
        }

        node = next;
    }

    return true;
}

/// Get any text in this object for the given range
wxString wxRichTextParagraphLayoutBox::GetTextForRange(const wxRichTextRange& range) const
{
    int lineCount = 0;
    wxString text;
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        if (!child->GetRange().IsOutside(range))
        {
            wxRichTextRange childRange = range;
            childRange.LimitTo(child->GetRange());

            wxString childText = child->GetTextForRange(childRange);

            text += childText;

            if ((childRange.GetEnd() == child->GetRange().GetEnd()) && node->GetNext())
                text += wxT("\n");

            lineCount ++;
        }
        node = node->GetNext();
    }

    return text;
}

/// Get all the text
wxString wxRichTextParagraphLayoutBox::GetText() const
{
    return GetTextForRange(GetOwnRange());
}

/// Get the paragraph by number
wxRichTextParagraph* wxRichTextParagraphLayoutBox::GetParagraphAtLine(long paragraphNumber) const
{
    if ((size_t) paragraphNumber >= GetChildCount())
        return NULL;

    return (wxRichTextParagraph*) GetChild((size_t) paragraphNumber);
}

/// Get the length of the paragraph
int wxRichTextParagraphLayoutBox::GetParagraphLength(long paragraphNumber) const
{
    wxRichTextParagraph* para = GetParagraphAtLine(paragraphNumber);
    if (para)
        return para->GetRange().GetLength() - 1; // don't include newline
    else
        return 0;
}

/// Get the text of the paragraph
wxString wxRichTextParagraphLayoutBox::GetParagraphText(long paragraphNumber) const
{
    wxRichTextParagraph* para = GetParagraphAtLine(paragraphNumber);
    if (para)
        return para->GetTextForRange(para->GetRange());
    else
        return wxEmptyString;
}

/// Convert zero-based line column and paragraph number to a position.
long wxRichTextParagraphLayoutBox::XYToPosition(long x, long y) const
{
    wxRichTextParagraph* para = GetParagraphAtLine(y);
    if (para)
    {
        return para->GetRange().GetStart() + x;
    }
    else
        return -1;
}

/// Convert zero-based position to line column and paragraph number
bool wxRichTextParagraphLayoutBox::PositionToXY(long pos, long* x, long* y) const
{
    wxRichTextParagraph* para = GetParagraphAtPosition(pos);
    if (para)
    {
        int count = 0;
        wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
        while (node)
        {
            wxRichTextObject* child = node->GetData();
            if (child == para)
                break;
            count ++;
            node = node->GetNext();
        }

        *y = count;
        *x = pos - para->GetRange().GetStart();

        return true;
    }
    else
        return false;
}

/// Get the leaf object in a paragraph at this position.
/// Given a line number, get the corresponding wxRichTextLine object.
wxRichTextObject* wxRichTextParagraphLayoutBox::GetLeafObjectAtPosition(long position) const
{
    wxRichTextParagraph* para = GetParagraphAtPosition(position);
    if (para)
    {
        wxRichTextObjectList::compatibility_iterator node = para->GetChildren().GetFirst();

        while (node)
        {
            wxRichTextObject* child = node->GetData();
            if (child->GetRange().Contains(position))
                return child;

            node = node->GetNext();
        }
        if (position == para->GetRange().GetEnd() && para->GetChildCount() > 0)
            return para->GetChildren().GetLast()->GetData();
    }
    return NULL;
}

/// Set character or paragraph text attributes: apply character styles only to immediate text nodes
bool wxRichTextParagraphLayoutBox::SetStyle(const wxRichTextRange& range, const wxRichTextAttr& style, int flags)
{
    bool characterStyle = false;
    bool paragraphStyle = false;

    if (style.IsCharacterStyle())
        characterStyle = true;
    if (style.IsParagraphStyle())
        paragraphStyle = true;

    wxRichTextBuffer* buffer = GetBuffer();

    bool withUndo = ((flags & wxRICHTEXT_SETSTYLE_WITH_UNDO) != 0);
    bool applyMinimal = ((flags & wxRICHTEXT_SETSTYLE_OPTIMIZE) != 0);
    bool parasOnly = ((flags & wxRICHTEXT_SETSTYLE_PARAGRAPHS_ONLY) != 0);
    bool charactersOnly = ((flags & wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY) != 0);
    bool resetExistingStyle = ((flags & wxRICHTEXT_SETSTYLE_RESET) != 0);
    bool removeStyle = ((flags & wxRICHTEXT_SETSTYLE_REMOVE) != 0);

    // Apply paragraph style first, if any
    wxRichTextAttr wholeStyle(style);

    if (!removeStyle && wholeStyle.HasParagraphStyleName() && buffer->GetStyleSheet())
    {
        wxRichTextParagraphStyleDefinition* def = buffer->GetStyleSheet()->FindParagraphStyle(wholeStyle.GetParagraphStyleName());
        if (def)
            wxRichTextApplyStyle(wholeStyle, def->GetStyleMergedWithBase(buffer->GetStyleSheet()));
    }

    // Limit the attributes to be set to the content to only character attributes.
    wxRichTextAttr characterAttributes(wholeStyle);
    characterAttributes.SetFlags(characterAttributes.GetFlags() & (wxTEXT_ATTR_CHARACTER));

    if (!removeStyle && characterAttributes.HasCharacterStyleName() && buffer->GetStyleSheet())
    {
        wxRichTextCharacterStyleDefinition* def = buffer->GetStyleSheet()->FindCharacterStyle(characterAttributes.GetCharacterStyleName());
        if (def)
            wxRichTextApplyStyle(characterAttributes, def->GetStyleMergedWithBase(buffer->GetStyleSheet()));
    }

    // If we are associated with a control, make undoable; otherwise, apply immediately
    // to the data.

    bool haveControl = (buffer->GetRichTextCtrl() != NULL);

    wxRichTextAction* action = NULL;

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(NULL, _("Change Style"), wxRICHTEXT_CHANGE_STYLE, buffer, this, buffer->GetRichTextCtrl());
        action->SetRange(range);
        action->SetPosition(buffer->GetRichTextCtrl()->GetCaretPosition());
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (para != NULL);

        if (para && para->GetChildCount() > 0)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                break;

            if (!para->GetRange().IsOutside(range))
            {
                // We'll be using a copy of the paragraph to make style changes,
                // not updating the buffer directly.
                wxRichTextParagraph* newPara wxDUMMY_INITIALIZE(NULL);

                if (haveControl && withUndo)
                {
                    newPara = new wxRichTextParagraph(*para);
                    action->GetNewParagraphs().AppendChild(newPara);

                    // Also store the old ones for Undo
                    action->GetOldParagraphs().AppendChild(new wxRichTextParagraph(*para));
                }
                else
                    newPara = para;

                // If we're specifying paragraphs only, then we really mean character formatting
                // to be included in the paragraph style
                if ((paragraphStyle || parasOnly) && !charactersOnly)
                {
                    if (removeStyle)
                    {
                        // Removes the given style from the paragraph
                        wxRichTextRemoveStyle(newPara->GetAttributes(), style);
                    }
                    else if (resetExistingStyle)
                        newPara->GetAttributes() = wholeStyle;
                    else
                    {
                        if (applyMinimal)
                        {
                            // Only apply attributes that will make a difference to the combined
                            // style as seen on the display
                            wxRichTextAttr combinedAttr(para->GetCombinedAttributes(true));
                            wxRichTextApplyStyle(newPara->GetAttributes(), wholeStyle, & combinedAttr);
                        }
                        else
                            wxRichTextApplyStyle(newPara->GetAttributes(), wholeStyle);
                    }
                }

                // When applying paragraph styles dynamically, don't change the text objects' attributes
                // since they will computed as needed. Only apply the character styling if it's _only_
                // character styling. This policy is subject to change and might be put under user control.

                // Hm. we might well be applying a mix of paragraph and character styles, in which
                // case we _do_ want to apply character styles regardless of what para styles are set.
                // But if we're applying a paragraph style, which has some character attributes, but
                // we only want the paragraphs to hold this character style, then we _don't_ want to
                // apply the character style. So we need to be able to choose.

                if (!parasOnly && (characterStyle|charactersOnly) && range.GetStart() != newPara->GetRange().GetEnd())
                {
                    wxRichTextRange childRange(range);
                    childRange.LimitTo(newPara->GetRange());

                    // Find the starting position and if necessary split it so
                    // we can start applying a different style.
                    // TODO: check that the style actually changes or is different
                    // from style outside of range
                    wxRichTextObject* firstObject wxDUMMY_INITIALIZE(NULL);
                    wxRichTextObject* lastObject wxDUMMY_INITIALIZE(NULL);

                    if (childRange.GetStart() == newPara->GetRange().GetStart())
                        firstObject = newPara->GetChildren().GetFirst()->GetData();
                    else
                        firstObject = newPara->SplitAt(range.GetStart());

                    // Increment by 1 because we're apply the style one _after_ the split point
                    long splitPoint = childRange.GetEnd();
                    if (splitPoint != newPara->GetRange().GetEnd())
                        splitPoint ++;

                    // Find last object
                    if (splitPoint == newPara->GetRange().GetEnd())
                        lastObject = newPara->GetChildren().GetLast()->GetData();
                    else
                        // lastObject is set as a side-effect of splitting. It's
                        // returned as the object before the new object.
                        (void) newPara->SplitAt(splitPoint, & lastObject);

                    wxASSERT(firstObject != NULL);
                    wxASSERT(lastObject != NULL);

                    if (!firstObject || !lastObject)
                        continue;

                    wxRichTextObjectList::compatibility_iterator firstNode = newPara->GetChildren().Find(firstObject);
                    wxRichTextObjectList::compatibility_iterator lastNode = newPara->GetChildren().Find(lastObject);

                    wxASSERT(firstNode);
                    wxASSERT(lastNode);

                    wxRichTextObjectList::compatibility_iterator node2 = firstNode;

                    while (node2)
                    {
                        wxRichTextObject* child = node2->GetData();

                        if (removeStyle)
                        {
                            // Removes the given style from the paragraph
                            wxRichTextRemoveStyle(child->GetAttributes(), style);
                        }
                        else if (resetExistingStyle)
                            child->GetAttributes() = characterAttributes;
                        else
                        {
                            if (applyMinimal)
                            {
                                // Only apply attributes that will make a difference to the combined
                                // style as seen on the display
                                wxRichTextAttr combinedAttr(newPara->GetCombinedAttributes(child->GetAttributes(), true));
                                wxRichTextApplyStyle(child->GetAttributes(), characterAttributes, & combinedAttr);
                            }
                            else
                                wxRichTextApplyStyle(child->GetAttributes(), characterAttributes);
                        }

                        if (node2 == lastNode)
                            break;

                        node2 = node2->GetNext();
                    }
                }
            }
        }

        node = node->GetNext();
    }

    // Do action, or delay it until end of batch.
    if (haveControl && withUndo)
        buffer->SubmitAction(action);

    return true;
}

// Just change the attributes for this single object.
void wxRichTextParagraphLayoutBox::SetStyle(wxRichTextObject* obj, const wxRichTextAttr& textAttr, int flags)
{
    wxRichTextBuffer* buffer = GetBuffer();
    bool withUndo = flags & wxRICHTEXT_SETSTYLE_WITH_UNDO;
    bool resetExistingStyle = ((flags & wxRICHTEXT_SETSTYLE_RESET) != 0);
    bool haveControl = (buffer->GetRichTextCtrl() != NULL);

    wxRichTextAction *action = NULL;
    wxRichTextAttr newAttr = obj->GetAttributes();
    if (resetExistingStyle)
        newAttr = textAttr;
    else
        newAttr.Apply(textAttr);

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(NULL, _("Change Object Style"), wxRICHTEXT_CHANGE_ATTRIBUTES, buffer, obj->GetContainer(), buffer->GetRichTextCtrl());
        action->SetRange(obj->GetRange().FromInternal());
        action->SetPosition(buffer->GetRichTextCtrl()->GetCaretPosition());
        action->MakeObject(obj);

        action->GetAttributes() = newAttr;
    }
    else
        obj->GetAttributes() = newAttr;

    if (haveControl && withUndo)
        buffer->SubmitAction(action);
}

/// Get the text attributes for this position.
bool wxRichTextParagraphLayoutBox::GetStyle(long position, wxRichTextAttr& style)
{
    return DoGetStyle(position, style, true);
}

bool wxRichTextParagraphLayoutBox::GetUncombinedStyle(long position, wxRichTextAttr& style)
{
    return DoGetStyle(position, style, false);
}

/// Implementation helper for GetStyle. If combineStyles is true, combine base, paragraph and
/// context attributes.
bool wxRichTextParagraphLayoutBox::DoGetStyle(long position, wxRichTextAttr& style, bool combineStyles)
{
    wxRichTextObject* obj wxDUMMY_INITIALIZE(NULL);

    if (style.IsParagraphStyle())
    {
        obj = GetParagraphAtPosition(position);
        if (obj)
        {
            if (combineStyles)
            {
                // Start with the base style
                style = GetAttributes();

                // Apply the paragraph style
                wxRichTextApplyStyle(style, obj->GetAttributes());
            }
            else
                style = obj->GetAttributes();

            return true;
        }
    }
    else
    {
        obj = GetLeafObjectAtPosition(position);
        if (obj)
        {
            if (combineStyles)
            {
                wxRichTextParagraph* para = wxDynamicCast(obj->GetParent(), wxRichTextParagraph);
                style = para ? para->GetCombinedAttributes(obj->GetAttributes()) : obj->GetAttributes();
            }
            else
                style = obj->GetAttributes();

            return true;
        }
    }
    return false;
}

static bool wxHasStyle(long flags, long style)
{
    return (flags & style) != 0;
}

/// Combines 'style' with 'currentStyle' for the purpose of summarising the attributes of a range of
/// content.
bool wxRichTextParagraphLayoutBox::CollectStyle(wxRichTextAttr& currentStyle, const wxRichTextAttr& style, wxRichTextAttr& clashingAttr, wxRichTextAttr& absentAttr)
{
    currentStyle.CollectCommonAttributes(style, clashingAttr, absentAttr);

    return true;
}

/// Get the combined style for a range - if any attribute is different within the range,
/// that attribute is not present within the flags.
/// *** Note that this is not recursive, and so assumes that content inside a paragraph is not itself
/// nested.
bool wxRichTextParagraphLayoutBox::GetStyleForRange(const wxRichTextRange& range, wxRichTextAttr& style)
{
    style = wxRichTextAttr();

    wxRichTextAttr clashingAttr;
    wxRichTextAttr absentAttrPara, absentAttrChar;

    wxRichTextObjectList::compatibility_iterator node = GetChildren().GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        if (para && !(para->GetRange().GetStart() > range.GetEnd() || para->GetRange().GetEnd() < range.GetStart()))
        {
            if (para->GetChildren().GetCount() == 0)
            {
                wxRichTextAttr paraStyle = para->GetCombinedAttributes(true /* use box attributes */);

                CollectStyle(style, paraStyle, clashingAttr, absentAttrPara);
            }
            else
            {
                wxRichTextRange paraRange(para->GetRange());
                paraRange.LimitTo(range);

                // First collect paragraph attributes only
                wxRichTextAttr paraStyle = para->GetCombinedAttributes();
                paraStyle.SetFlags(paraStyle.GetFlags() & wxTEXT_ATTR_PARAGRAPH);
                CollectStyle(style, paraStyle, clashingAttr, absentAttrPara);

                wxRichTextObjectList::compatibility_iterator childNode = para->GetChildren().GetFirst();

                while (childNode)
                {
                    wxRichTextObject* child = childNode->GetData();
                    if (!(child->GetRange().GetStart() > range.GetEnd() || child->GetRange().GetEnd() < range.GetStart()))
                    {
                        wxRichTextAttr childStyle = para->GetCombinedAttributes(child->GetAttributes(), true /* include box attributes */);

                        // Now collect character attributes only
                        childStyle.SetFlags(childStyle.GetFlags() & wxTEXT_ATTR_CHARACTER);

                        CollectStyle(style, childStyle, clashingAttr, absentAttrChar);
                    }

                    childNode = childNode->GetNext();
                }
            }
        }
        node = node->GetNext();
    }
    return true;
}

/// Set default style
bool wxRichTextParagraphLayoutBox::SetDefaultStyle(const wxRichTextAttr& style)
{
    m_defaultAttributes = style;
    return true;
}

/// Test if this whole range has character attributes of the specified kind. If any
/// of the attributes are different within the range, the test fails. You
/// can use this to implement, for example, bold button updating. style must have
/// flags indicating which attributes are of interest.
bool wxRichTextParagraphLayoutBox::HasCharacterAttributes(const wxRichTextRange& range, const wxRichTextAttr& style) const
{
    int foundCount = 0;
    int matchingCount = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (para != NULL);

        if (para)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                return foundCount == matchingCount && foundCount != 0;

            if (!para->GetRange().IsOutside(range))
            {
                wxRichTextObjectList::compatibility_iterator node2 = para->GetChildren().GetFirst();

                while (node2)
                {
                    wxRichTextObject* child = node2->GetData();
                    // Allow for empty string if no buffer
                    wxRichTextRange childRange = child->GetRange();
                    if (childRange.GetLength() == 0 && GetRange().GetLength() == 1)
                        childRange.SetEnd(childRange.GetEnd()+1);

                    if (!childRange.IsOutside(range) && child->IsKindOf(CLASSINFO(wxRichTextPlainText)))
                    {
                        foundCount ++;
                        wxRichTextAttr textAttr = para->GetCombinedAttributes(child->GetAttributes());

                        if (wxTextAttrEqPartial(textAttr, style))
                            matchingCount ++;
                    }

                    node2 = node2->GetNext();
                }
            }
        }

        node = node->GetNext();
    }

    return foundCount == matchingCount && foundCount != 0;
}

/// Test if this whole range has paragraph attributes of the specified kind. If any
/// of the attributes are different within the range, the test fails. You
/// can use this to implement, for example, centering button updating. style must have
/// flags indicating which attributes are of interest.
bool wxRichTextParagraphLayoutBox::HasParagraphAttributes(const wxRichTextRange& range, const wxRichTextAttr& style) const
{
    int foundCount = 0;
    int matchingCount = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (para != NULL);

        if (para)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                return foundCount == matchingCount && foundCount != 0;

            if (!para->GetRange().IsOutside(range))
            {
                wxRichTextAttr textAttr = GetAttributes();
                // Apply the paragraph style
                wxRichTextApplyStyle(textAttr, para->GetAttributes());

                foundCount ++;
                if (wxTextAttrEqPartial(textAttr, style))
                    matchingCount ++;
            }
        }

        node = node->GetNext();
    }
    return foundCount == matchingCount && foundCount != 0;
}

void wxRichTextParagraphLayoutBox::Reset()
{
    Clear();

    wxRichTextBuffer* buffer = GetBuffer();
    if (buffer && buffer->GetRichTextCtrl())
    {
        wxRichTextEvent event(wxEVT_COMMAND_RICHTEXT_BUFFER_RESET, buffer->GetRichTextCtrl()->GetId());
        event.SetEventObject(buffer->GetRichTextCtrl());
        event.SetContainer(this);

        buffer->SendEvent(event, true);
    }

    AddParagraph(wxEmptyString);

    InvalidateHierarchy(wxRICHTEXT_ALL);
}

/// Invalidate the buffer. With no argument, invalidates whole buffer.
void wxRichTextParagraphLayoutBox::Invalidate(const wxRichTextRange& invalidRange)
{
    wxRichTextCompositeObject::Invalidate(invalidRange);

    DoInvalidate(invalidRange);
}

// Do the (in)validation for this object only
void wxRichTextParagraphLayoutBox::DoInvalidate(const wxRichTextRange& invalidRange)
{
    if (invalidRange == wxRICHTEXT_ALL)
    {
        m_invalidRange = wxRICHTEXT_ALL;
    }
    // Already invalidating everything
    else if (m_invalidRange == wxRICHTEXT_ALL)
    {
    }
    else
    {
        if ((invalidRange.GetStart() < m_invalidRange.GetStart()) || m_invalidRange.GetStart() == -1)
            m_invalidRange.SetStart(invalidRange.GetStart());
        if (invalidRange.GetEnd() > m_invalidRange.GetEnd())
            m_invalidRange.SetEnd(invalidRange.GetEnd());
    }
}

// Do the (in)validation both up and down the hierarchy
void wxRichTextParagraphLayoutBox::InvalidateHierarchy(const wxRichTextRange& invalidRange)
{
    Invalidate(invalidRange);

    if (invalidRange != wxRICHTEXT_NONE)
    {
        // Now go up the hierarchy
        wxRichTextObject* thisObj = this;
        wxRichTextObject* p = GetParent();
        while (p)
        {
            wxRichTextParagraphLayoutBox* l = wxDynamicCast(p, wxRichTextParagraphLayoutBox);
            if (l)
                l->DoInvalidate(thisObj->GetRange());

            thisObj = p;
            p = p->GetParent();
        }
    }
}

/// Get invalid range, rounding to entire paragraphs if argument is true.
wxRichTextRange wxRichTextParagraphLayoutBox::GetInvalidRange(bool wholeParagraphs) const
{
    if (m_invalidRange == wxRICHTEXT_ALL || m_invalidRange == wxRICHTEXT_NONE)
        return m_invalidRange;

    wxRichTextRange range = m_invalidRange;

    if (wholeParagraphs)
    {
        wxRichTextParagraph* para1 = GetParagraphAtPosition(range.GetStart());
        if (para1)
            range.SetStart(para1->GetRange().GetStart());
        // floating layout make all child should be relayout
        range.SetEnd(GetOwnRange().GetEnd());
    }
    return range;
}

/// Apply the style sheet to the buffer, for example if the styles have changed.
bool wxRichTextParagraphLayoutBox::ApplyStyleSheet(wxRichTextStyleSheet* styleSheet)
{
    wxASSERT(styleSheet != NULL);
    if (!styleSheet)
        return false;

    int foundCount = 0;

    wxRichTextAttr attr(GetBasicStyle());
    if (GetBasicStyle().HasParagraphStyleName())
    {
        wxRichTextParagraphStyleDefinition* paraDef = styleSheet->FindParagraphStyle(GetBasicStyle().GetParagraphStyleName());
        if (paraDef)
        {
            attr.Apply(paraDef->GetStyleMergedWithBase(styleSheet));
            SetBasicStyle(attr);
            foundCount ++;
        }
    }

    if (GetBasicStyle().HasCharacterStyleName())
    {
        wxRichTextCharacterStyleDefinition* charDef = styleSheet->FindCharacterStyle(GetBasicStyle().GetCharacterStyleName());
        if (charDef)
        {
            attr.Apply(charDef->GetStyleMergedWithBase(styleSheet));
            SetBasicStyle(attr);
            foundCount ++;
        }
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (para != NULL);

        if (para)
        {
            // Combine paragraph and list styles. If there is a list style in the original attributes,
            // the current indentation overrides anything else and is used to find the item indentation.
            // Also, for applying paragraph styles, consider having 2 modes: (1) we merge with what we have,
            // thereby taking into account all user changes, (2) reset the style completely (except for indentation/list
            // exception as above).
            // Problem: when changing from one list style to another, there's a danger that the level info will get lost.
            // So when changing a list style interactively, could retrieve level based on current style, then
            // set appropriate indent and apply new style.

            int outline = -1;
            int num = -1;
            if (para->GetAttributes().HasOutlineLevel())
                outline = para->GetAttributes().GetOutlineLevel();
            if (para->GetAttributes().HasBulletNumber())
                num = para->GetAttributes().GetBulletNumber();

            if (!para->GetAttributes().GetParagraphStyleName().IsEmpty() && !para->GetAttributes().GetListStyleName().IsEmpty())
            {
                int currentIndent = para->GetAttributes().GetLeftIndent();

                wxRichTextParagraphStyleDefinition* paraDef = styleSheet->FindParagraphStyle(para->GetAttributes().GetParagraphStyleName());
                wxRichTextListStyleDefinition* listDef = styleSheet->FindListStyle(para->GetAttributes().GetListStyleName());
                if (paraDef && !listDef)
                {
                    para->GetAttributes() = paraDef->GetStyleMergedWithBase(styleSheet);
                    foundCount ++;
                }
                else if (listDef && !paraDef)
                {
                    // Set overall style defined for the list style definition
                    para->GetAttributes() = listDef->GetStyleMergedWithBase(styleSheet);

                    // Apply the style for this level
                    wxRichTextApplyStyle(para->GetAttributes(), * listDef->GetLevelAttributes(listDef->FindLevelForIndent(currentIndent)));
                    foundCount ++;
                }
                else if (listDef && paraDef)
                {
                    // Combines overall list style, style for level, and paragraph style
                    para->GetAttributes() = listDef->CombineWithParagraphStyle(currentIndent, paraDef->GetStyleMergedWithBase(styleSheet));
                    foundCount ++;
                }
            }
            else if (para->GetAttributes().GetParagraphStyleName().IsEmpty() && !para->GetAttributes().GetListStyleName().IsEmpty())
            {
                int currentIndent = para->GetAttributes().GetLeftIndent();

                wxRichTextListStyleDefinition* listDef = styleSheet->FindListStyle(para->GetAttributes().GetListStyleName());

                // Overall list definition style
                para->GetAttributes() = listDef->GetStyleMergedWithBase(styleSheet);

                // Style for this level
                wxRichTextApplyStyle(para->GetAttributes(), * listDef->GetLevelAttributes(listDef->FindLevelForIndent(currentIndent)));

                foundCount ++;
            }
            else if (!para->GetAttributes().GetParagraphStyleName().IsEmpty() && para->GetAttributes().GetListStyleName().IsEmpty())
            {
                wxRichTextParagraphStyleDefinition* def = styleSheet->FindParagraphStyle(para->GetAttributes().GetParagraphStyleName());
                if (def)
                {
                    para->GetAttributes() = def->GetStyleMergedWithBase(styleSheet);
                    foundCount ++;
                }
            }

            if (outline != -1)
                para->GetAttributes().SetOutlineLevel(outline);
            if (num != -1)
                para->GetAttributes().SetBulletNumber(num);
        }

        node = node->GetNext();
    }
    return foundCount != 0;
}

/// Set list style
bool wxRichTextParagraphLayoutBox::SetListStyle(const wxRichTextRange& range, wxRichTextListStyleDefinition* def, int flags, int startFrom, int specifiedLevel)
{
    wxRichTextBuffer* buffer = GetBuffer();
    wxRichTextStyleSheet* styleSheet = buffer->GetStyleSheet();

    bool withUndo = ((flags & wxRICHTEXT_SETSTYLE_WITH_UNDO) != 0);
    // bool applyMinimal = ((flags & wxRICHTEXT_SETSTYLE_OPTIMIZE) != 0);
    bool specifyLevel = ((flags & wxRICHTEXT_SETSTYLE_SPECIFY_LEVEL) != 0);
    bool renumber = ((flags & wxRICHTEXT_SETSTYLE_RENUMBER) != 0);

    // Current number, if numbering
    int n = startFrom;

    wxASSERT (!specifyLevel || (specifyLevel && (specifiedLevel >= 0)));

    // If we are associated with a control, make undoable; otherwise, apply immediately
    // to the data.

    bool haveControl = (buffer->GetRichTextCtrl() != NULL);

    wxRichTextAction* action = NULL;

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(NULL, _("Change List Style"), wxRICHTEXT_CHANGE_STYLE, buffer, this, buffer->GetRichTextCtrl());
        action->SetRange(range);
        action->SetPosition(buffer->GetRichTextCtrl()->GetCaretPosition());
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (para != NULL);

        if (para && para->GetChildCount() > 0)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                break;

            if (!para->GetRange().IsOutside(range))
            {
                // We'll be using a copy of the paragraph to make style changes,
                // not updating the buffer directly.
                wxRichTextParagraph* newPara wxDUMMY_INITIALIZE(NULL);

                if (haveControl && withUndo)
                {
                    newPara = new wxRichTextParagraph(*para);
                    action->GetNewParagraphs().AppendChild(newPara);

                    // Also store the old ones for Undo
                    action->GetOldParagraphs().AppendChild(new wxRichTextParagraph(*para));
                }
                else
                    newPara = para;

                if (def)
                {
                    int thisIndent = newPara->GetAttributes().GetLeftIndent();
                    int thisLevel = specifyLevel ? specifiedLevel : def->FindLevelForIndent(thisIndent);

                    // How is numbering going to work?
                    // If we are renumbering, or numbering for the first time, we need to keep
                    // track of the number for each level. But we might be simply applying a different
                    // list style.
                    // In Word, applying a style to several paragraphs, even if at different levels,
                    // reverts the level back to the same one. So we could do the same here.
                    // Renumbering will need to be done when we promote/demote a paragraph.

                    // Apply the overall list style, and item style for this level
                    wxRichTextAttr listStyle(def->GetCombinedStyleForLevel(thisLevel, styleSheet));
                    wxRichTextApplyStyle(newPara->GetAttributes(), listStyle);

                    // Now we need to do numbering
                    if (renumber)
                    {
                        newPara->GetAttributes().SetBulletNumber(n);
                    }

                    n ++;
                }
                else if (!newPara->GetAttributes().GetListStyleName().IsEmpty())
                {
                    // if def is NULL, remove list style, applying any associated paragraph style
                    // to restore the attributes

                    newPara->GetAttributes().SetListStyleName(wxEmptyString);
                    newPara->GetAttributes().SetLeftIndent(0, 0);
                    newPara->GetAttributes().SetBulletText(wxEmptyString);

                    // Eliminate the main list-related attributes
                    newPara->GetAttributes().SetFlags(newPara->GetAttributes().GetFlags() & ~wxTEXT_ATTR_LEFT_INDENT & ~wxTEXT_ATTR_BULLET_STYLE & ~wxTEXT_ATTR_BULLET_NUMBER & ~wxTEXT_ATTR_BULLET_TEXT & wxTEXT_ATTR_LIST_STYLE_NAME);

                    if (styleSheet && !newPara->GetAttributes().GetParagraphStyleName().IsEmpty())
                    {
                        wxRichTextParagraphStyleDefinition* def = styleSheet->FindParagraphStyle(newPara->GetAttributes().GetParagraphStyleName());
                        if (def)
                        {
                            newPara->GetAttributes() = def->GetStyleMergedWithBase(styleSheet);
                        }
                    }
                }
            }
        }

        node = node->GetNext();
    }

    // Do action, or delay it until end of batch.
    if (haveControl && withUndo)
        buffer->SubmitAction(action);

    return true;
}

bool wxRichTextParagraphLayoutBox::SetListStyle(const wxRichTextRange& range, const wxString& defName, int flags, int startFrom, int specifiedLevel)
{
    wxRichTextBuffer* buffer = GetBuffer();
    if (buffer && buffer->GetStyleSheet())
    {
        wxRichTextListStyleDefinition* def = buffer->GetStyleSheet()->FindListStyle(defName);
        if (def)
            return SetListStyle(range, def, flags, startFrom, specifiedLevel);
    }
    return false;
}

/// Clear list for given range
bool wxRichTextParagraphLayoutBox::ClearListStyle(const wxRichTextRange& range, int flags)
{
    return SetListStyle(range, NULL, flags);
}

/// Number/renumber any list elements in the given range
bool wxRichTextParagraphLayoutBox::NumberList(const wxRichTextRange& range, wxRichTextListStyleDefinition* def, int flags, int startFrom, int specifiedLevel)
{
    return DoNumberList(range, range, 0, def, flags, startFrom, specifiedLevel);
}

/// Number/renumber any list elements in the given range. Also do promotion or demotion of items, if specified
bool wxRichTextParagraphLayoutBox::DoNumberList(const wxRichTextRange& range, const wxRichTextRange& promotionRange, int promoteBy,
                                                wxRichTextListStyleDefinition* def, int flags, int startFrom, int specifiedLevel)
{
    wxRichTextBuffer* buffer = GetBuffer();
    wxRichTextStyleSheet* styleSheet = buffer->GetStyleSheet();

    bool withUndo = ((flags & wxRICHTEXT_SETSTYLE_WITH_UNDO) != 0);
    // bool applyMinimal = ((flags & wxRICHTEXT_SETSTYLE_OPTIMIZE) != 0);
#if wxDEBUG_LEVEL
    bool specifyLevel = ((flags & wxRICHTEXT_SETSTYLE_SPECIFY_LEVEL) != 0);
#endif

    bool renumber = ((flags & wxRICHTEXT_SETSTYLE_RENUMBER) != 0);

    // Max number of levels
    const int maxLevels = 10;

    // The level we're looking at now
    int currentLevel = -1;

    // The item number for each level
    int levels[maxLevels];
    int i;

    // Reset all numbering
    for (i = 0; i < maxLevels; i++)
    {
        if (startFrom != -1)
            levels[i] = startFrom-1;
        else if (renumber) // start again
            levels[i] = 0;
        else
            levels[i] = -1; // start from the number we found, if any
    }

    wxASSERT(!specifyLevel || (specifyLevel && (specifiedLevel >= 0)));

    // If we are associated with a control, make undoable; otherwise, apply immediately
    // to the data.

    bool haveControl = (buffer->GetRichTextCtrl() != NULL);

    wxRichTextAction* action = NULL;

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(NULL, _("Renumber List"), wxRICHTEXT_CHANGE_STYLE, buffer, this, buffer->GetRichTextCtrl());
        action->SetRange(range);
        action->SetPosition(buffer->GetRichTextCtrl()->GetCaretPosition());
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (para != NULL);

        if (para && para->GetChildCount() > 0)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                break;

            if (!para->GetRange().IsOutside(range))
            {
                // We'll be using a copy of the paragraph to make style changes,
                // not updating the buffer directly.
                wxRichTextParagraph* newPara wxDUMMY_INITIALIZE(NULL);

                if (haveControl && withUndo)
                {
                    newPara = new wxRichTextParagraph(*para);
                    action->GetNewParagraphs().AppendChild(newPara);

                    // Also store the old ones for Undo
                    action->GetOldParagraphs().AppendChild(new wxRichTextParagraph(*para));
                }
                else
                    newPara = para;

                wxRichTextListStyleDefinition* defToUse = def;
                if (!defToUse)
                {
                    if (styleSheet && !newPara->GetAttributes().GetListStyleName().IsEmpty())
                        defToUse = styleSheet->FindListStyle(newPara->GetAttributes().GetListStyleName());
                }

                if (defToUse)
                {
                    int thisIndent = newPara->GetAttributes().GetLeftIndent();
                    int thisLevel = defToUse->FindLevelForIndent(thisIndent);

                    // If we've specified a level to apply to all, change the level.
                    if (specifiedLevel != -1)
                        thisLevel = specifiedLevel;

                    // Do promotion if specified
                    if ((promoteBy != 0) && !para->GetRange().IsOutside(promotionRange))
                    {
                        thisLevel = thisLevel - promoteBy;
                        if (thisLevel < 0)
                            thisLevel = 0;
                        if (thisLevel > 9)
                            thisLevel = 9;
                    }

                    // Apply the overall list style, and item style for this level
                    wxRichTextAttr listStyle(defToUse->GetCombinedStyleForLevel(thisLevel, styleSheet));
                    wxRichTextApplyStyle(newPara->GetAttributes(), listStyle);

                    // OK, we've (re)applied the style, now let's get the numbering right.

                    if (currentLevel == -1)
                        currentLevel = thisLevel;

                    // Same level as before, do nothing except increment level's number afterwards
                    if (currentLevel == thisLevel)
                    {
                    }
                    // A deeper level: start renumbering all levels after current level
                    else if (thisLevel > currentLevel)
                    {
                        for (i = currentLevel+1; i <= thisLevel; i++)
                        {
                            levels[i] = 0;
                        }
                        currentLevel = thisLevel;
                    }
                    else if (thisLevel < currentLevel)
                    {
                        currentLevel = thisLevel;
                    }

                    // Use the current numbering if -1 and we have a bullet number already
                    if (levels[currentLevel] == -1)
                    {
                        if (newPara->GetAttributes().HasBulletNumber())
                            levels[currentLevel] = newPara->GetAttributes().GetBulletNumber();
                        else
                            levels[currentLevel] = 1;
                    }
                    else
                    {
                        levels[currentLevel] ++;
                    }

                    newPara->GetAttributes().SetBulletNumber(levels[currentLevel]);

                    // Create the bullet text if an outline list
                    if (listStyle.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_OUTLINE)
                    {
                        wxString text;
                        for (i = 0; i <= currentLevel; i++)
                        {
                            if (!text.IsEmpty())
                                text += wxT(".");
                            text += wxString::Format(wxT("%d"), levels[i]);
                        }
                        newPara->GetAttributes().SetBulletText(text);
                    }
                }
            }
        }

        node = node->GetNext();
    }

    // Do action, or delay it until end of batch.
    if (haveControl && withUndo)
        buffer->SubmitAction(action);

    return true;
}

bool wxRichTextParagraphLayoutBox::NumberList(const wxRichTextRange& range, const wxString& defName, int flags, int startFrom, int specifiedLevel)
{
    wxRichTextBuffer* buffer = GetBuffer();
    if (buffer->GetStyleSheet())
    {
        wxRichTextListStyleDefinition* def = NULL;
        if (!defName.IsEmpty())
            def = buffer->GetStyleSheet()->FindListStyle(defName);
        return NumberList(range, def, flags, startFrom, specifiedLevel);
    }
    return false;
}

/// Promote the list items within the given range. promoteBy can be a positive or negative number, e.g. 1 or -1
bool wxRichTextParagraphLayoutBox::PromoteList(int promoteBy, const wxRichTextRange& range, wxRichTextListStyleDefinition* def, int flags, int specifiedLevel)
{
    // TODO
    // One strategy is to first work out the range within which renumbering must occur. Then could pass these two ranges
    // to NumberList with a flag indicating promotion is required within one of the ranges.
    // Find first and last paragraphs in range. Then for first, calculate new indentation and look back until we find
    // a paragraph that either has no list style, or has one that is different or whose indentation is less.
    // We start renumbering from the para after that different para we found. We specify that the numbering of that
    // list position will start from 1.
    // Similarly, we look after the last para in the promote range for an indentation that is less (or no list style).
    // We can end the renumbering at this point.

    // For now, only renumber within the promotion range.

    return DoNumberList(range, range, promoteBy, def, flags, 1, specifiedLevel);
}

bool wxRichTextParagraphLayoutBox::PromoteList(int promoteBy, const wxRichTextRange& range, const wxString& defName, int flags, int specifiedLevel)
{
    wxRichTextBuffer* buffer = GetBuffer();
    if (buffer->GetStyleSheet())
    {
        wxRichTextListStyleDefinition* def = NULL;
        if (!defName.IsEmpty())
            def = buffer->GetStyleSheet()->FindListStyle(defName);
        return PromoteList(promoteBy, range, def, flags, specifiedLevel);
    }
    return false;
}

/// Fills in the attributes for numbering a paragraph after previousParagraph. It also finds the
/// position of the paragraph that it had to start looking from.
bool wxRichTextParagraphLayoutBox::FindNextParagraphNumber(wxRichTextParagraph* previousParagraph, wxRichTextAttr& attr) const
{
    if (!previousParagraph->GetAttributes().HasFlag(wxTEXT_ATTR_BULLET_STYLE) || previousParagraph->GetAttributes().GetBulletStyle() == wxTEXT_ATTR_BULLET_STYLE_NONE)
        return false;

    wxRichTextBuffer* buffer = GetBuffer();
    wxRichTextStyleSheet* styleSheet = buffer->GetStyleSheet();
    if (styleSheet && !previousParagraph->GetAttributes().GetListStyleName().IsEmpty())
    {
        wxRichTextListStyleDefinition* def = styleSheet->FindListStyle(previousParagraph->GetAttributes().GetListStyleName());
        if (def)
        {
            // int thisIndent = previousParagraph->GetAttributes().GetLeftIndent();
            // int thisLevel = def->FindLevelForIndent(thisIndent);

            bool isOutline = (previousParagraph->GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_OUTLINE) != 0;

            attr.SetFlags(previousParagraph->GetAttributes().GetFlags() & (wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_NUMBER|wxTEXT_ATTR_BULLET_TEXT|wxTEXT_ATTR_BULLET_NAME));
            if (previousParagraph->GetAttributes().HasBulletName())
                attr.SetBulletName(previousParagraph->GetAttributes().GetBulletName());
            attr.SetBulletStyle(previousParagraph->GetAttributes().GetBulletStyle());
            attr.SetListStyleName(previousParagraph->GetAttributes().GetListStyleName());

            int nextNumber = previousParagraph->GetAttributes().GetBulletNumber() + 1;
            attr.SetBulletNumber(nextNumber);

            if (isOutline)
            {
                wxString text = previousParagraph->GetAttributes().GetBulletText();
                if (!text.IsEmpty())
                {
                    int pos = text.Find(wxT('.'), true);
                    if (pos != wxNOT_FOUND)
                    {
                        text = text.Mid(0, text.Length() - pos - 1);
                    }
                    else
                        text = wxEmptyString;
                    if (!text.IsEmpty())
                        text += wxT(".");
                    text += wxString::Format(wxT("%d"), nextNumber);
                    attr.SetBulletText(text);
                }
            }

            return true;
        }
        else
            return false;
    }
    else
        return false;
}

/*!
 * wxRichTextParagraph
 * This object represents a single paragraph (or in a straight text editor, a line).
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextParagraph, wxRichTextCompositeObject)

wxArrayInt wxRichTextParagraph::sm_defaultTabs;

wxRichTextParagraph::wxRichTextParagraph(wxRichTextObject* parent, wxRichTextAttr* style):
    wxRichTextCompositeObject(parent)
{
    if (style)
        SetAttributes(*style);
}

wxRichTextParagraph::wxRichTextParagraph(const wxString& text, wxRichTextObject* parent, wxRichTextAttr* paraStyle, wxRichTextAttr* charStyle):
    wxRichTextCompositeObject(parent)
{
    if (paraStyle)
        SetAttributes(*paraStyle);

    AppendChild(new wxRichTextPlainText(text, this, charStyle));
}

wxRichTextParagraph::~wxRichTextParagraph()
{
    ClearLines();
}

/// Draw the item
bool wxRichTextParagraph::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int WXUNUSED(descent), int style)
{
    if (!IsShown())
        return true;

    // Currently we don't merge these attributes with the parent, but we
    // should consider whether we should (e.g. if we set a border colour
    // for all paragraphs). But generally box attributes are likely to be
    // different for different objects.
    wxRect paraRect = GetRect();
    DrawBoxAttributes(dc, GetBuffer(), GetAttributes(), paraRect);

    wxRichTextAttr attr = GetCombinedAttributes();

    // Draw the bullet, if any
    if (attr.GetBulletStyle() != wxTEXT_ATTR_BULLET_STYLE_NONE)
    {
        if (attr.GetLeftSubIndent() != 0)
        {
            int spaceBeforePara = ConvertTenthsMMToPixels(dc, attr.GetParagraphSpacingBefore());
            int leftIndent = ConvertTenthsMMToPixels(dc, attr.GetLeftIndent());

            wxRichTextAttr bulletAttr(GetCombinedAttributes());

            // Combine with the font of the first piece of content, if one is specified
            if (GetChildren().GetCount() > 0)
            {
                wxRichTextObject* firstObj = (wxRichTextObject*) GetChildren().GetFirst()->GetData();
                if (!firstObj->IsFloatable() && firstObj->GetAttributes().HasFont())
                {
                    wxRichTextApplyStyle(bulletAttr, firstObj->GetAttributes());
                }
            }

            // Get line height from first line, if any
            wxRichTextLine* line = m_cachedLines.GetFirst() ? (wxRichTextLine* ) m_cachedLines.GetFirst()->GetData() : NULL;

            wxPoint linePos;
            int lineHeight wxDUMMY_INITIALIZE(0);
            if (line)
            {
                lineHeight = line->GetSize().y;
                linePos = line->GetPosition() + GetPosition();
            }
            else
            {
                wxFont font;
                if (bulletAttr.HasFont() && GetBuffer())
                    font = GetBuffer()->GetFontTable().FindFont(bulletAttr);
                else
                    font = (*wxNORMAL_FONT);

                wxCheckSetFont(dc, font);

                lineHeight = dc.GetCharHeight();
                linePos = GetPosition();
                linePos.y += spaceBeforePara;
            }

            wxRect bulletRect(GetPosition().x + leftIndent, linePos.y, linePos.x - (GetPosition().x + leftIndent), lineHeight);

            if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_BITMAP)
            {
                if (wxRichTextBuffer::GetRenderer())
                    wxRichTextBuffer::GetRenderer()->DrawBitmapBullet(this, dc, bulletAttr, bulletRect);
            }
            else if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_STANDARD)
            {
                if (wxRichTextBuffer::GetRenderer())
                    wxRichTextBuffer::GetRenderer()->DrawStandardBullet(this, dc, bulletAttr, bulletRect);
            }
            else
            {
                wxString bulletText = GetBulletText();

                if (!bulletText.empty() && wxRichTextBuffer::GetRenderer())
                    wxRichTextBuffer::GetRenderer()->DrawTextBullet(this, dc, bulletAttr, bulletRect, bulletText);
            }
        }
    }

    // Draw the range for each line, one object at a time.

    wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetFirst();
    while (node)
    {
        wxRichTextLine* line = node->GetData();
        wxRichTextRange lineRange = line->GetAbsoluteRange();

        // Lines are specified relative to the paragraph

        wxPoint linePosition = line->GetPosition() + GetPosition();

        // Don't draw if off the screen
        if (((style & wxRICHTEXT_DRAW_IGNORE_CACHE) != 0) || ((linePosition.y + line->GetSize().y) >= rect.y && linePosition.y <= rect.y + rect.height))
        {
            wxPoint objectPosition = linePosition;
            int maxDescent = line->GetDescent();

            // Loop through objects until we get to the one within range
            wxRichTextObjectList::compatibility_iterator node2 = m_children.GetFirst();

            int i = 0;
            while (node2)
            {
                wxRichTextObject* child = node2->GetData();

                if (!child->IsFloating() && child->GetRange().GetLength() > 0 && !child->GetRange().IsOutside(lineRange) && !lineRange.IsOutside(range))
                {
                    // Draw this part of the line at the correct position
                    wxRichTextRange objectRange(child->GetRange());
                    objectRange.LimitTo(lineRange);

                    wxSize objectSize;
                    if (child->IsTopLevel())
                    {
                        objectSize = child->GetCachedSize();
                        objectRange = child->GetOwnRange();
                    }
                    else
                    {
#if wxRICHTEXT_USE_OPTIMIZED_LINE_DRAWING && wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
                        if (i < (int) line->GetObjectSizes().GetCount())
                        {
                            objectSize.x = line->GetObjectSizes()[(size_t) i];
                        }
                        else
#endif
                        {
                            int descent = 0;
                            child->GetRangeSize(objectRange, objectSize, descent, dc, wxRICHTEXT_UNFORMATTED, objectPosition);
                        }
                    }

                    // Use the child object's width, but the whole line's height
                    wxRect childRect(objectPosition, wxSize(objectSize.x, line->GetSize().y));
                    child->Draw(dc, objectRange, selection, childRect, maxDescent, style);

                    objectPosition.x += objectSize.x;
                    i ++;
                }
                else if (child->GetRange().GetStart() > lineRange.GetEnd())
                    // Can break out of inner loop now since we've passed this line's range
                    break;

                node2 = node2->GetNext();
            }
        }

        node = node->GetNext();
    }

    return true;
}

// Get the range width using partial extents calculated for the whole paragraph.
static int wxRichTextGetRangeWidth(const wxRichTextParagraph& para, const wxRichTextRange& range, const wxArrayInt& partialExtents)
{
    wxASSERT(partialExtents.GetCount() >= (size_t) range.GetLength());

    if (partialExtents.GetCount() < (size_t) range.GetLength())
        return 0;

    int leftMostPos = 0;
    if (range.GetStart() - para.GetRange().GetStart() > 0)
        leftMostPos = partialExtents[range.GetStart() - para.GetRange().GetStart() - 1];

    int rightMostPos = partialExtents[range.GetEnd() - para.GetRange().GetStart()];

    int w = rightMostPos - leftMostPos;

    return w;
}

/// Lay the item out
bool wxRichTextParagraph::Layout(wxDC& dc, const wxRect& rect, int style)
{
    // Deal with floating objects firstly before the normal layout
    wxRichTextBuffer* buffer = GetBuffer();
    wxASSERT(buffer);
    wxRichTextFloatCollector* collector = GetContainer()->GetFloatCollector();
    wxASSERT(collector);
    LayoutFloat(dc, rect, style, collector);

    wxRichTextAttr attr = GetCombinedAttributes();

    // ClearLines();

    // Increase the size of the paragraph due to spacing
    int spaceBeforePara = ConvertTenthsMMToPixels(dc, attr.GetParagraphSpacingBefore());
    int spaceAfterPara = ConvertTenthsMMToPixels(dc, attr.GetParagraphSpacingAfter());
    int leftIndent = ConvertTenthsMMToPixels(dc, attr.GetLeftIndent());
    int leftSubIndent = ConvertTenthsMMToPixels(dc, attr.GetLeftSubIndent());
    int rightIndent = ConvertTenthsMMToPixels(dc, attr.GetRightIndent());

    int lineSpacing = 0;

    // Let's assume line spacing of 10 is normal, 15 is 1.5, 20 is 2, etc.
    if (attr.HasLineSpacing() && attr.GetLineSpacing() > 0 && attr.GetFont().Ok())
    {
        wxCheckSetFont(dc, attr.GetFont());
        lineSpacing = (int) (double(dc.GetCharHeight()) * (double(attr.GetLineSpacing())/10.0 - 1.0));
    }

    // Start position for each line relative to the paragraph
    int startPositionFirstLine = leftIndent;
    int startPositionSubsequentLines = leftIndent + leftSubIndent;

    // If we have a bullet in this paragraph, the start position for the first line's text
    // is actually leftIndent + leftSubIndent.
    if (attr.GetBulletStyle() != wxTEXT_ATTR_BULLET_STYLE_NONE)
        startPositionFirstLine = startPositionSubsequentLines;

    long lastEndPos = GetRange().GetStart()-1;
    long lastCompletedEndPos = lastEndPos;

    int currentWidth = 0;
    SetPosition(rect.GetPosition());

    wxPoint currentPosition(0, spaceBeforePara); // We will calculate lines relative to paragraph
    int lineHeight = 0;
    int maxWidth = 0;
    int maxHeight = currentPosition.y;
    int maxAscent = 0;
    int maxDescent = 0;
    int lineCount = 0;
    int lineAscent = 0;
    int lineDescent = 0;

    wxRichTextObjectList::compatibility_iterator node;

#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
    wxUnusedVar(style);
    wxArrayInt partialExtents;

    wxSize paraSize;
    int paraDescent = 0;

    // This calculates the partial text extents
    GetRangeSize(GetRange(), paraSize, paraDescent, dc, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_CACHE_SIZE, rect.GetPosition(), & partialExtents);
#else
    node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        //child->SetCachedSize(wxDefaultSize);
        child->Layout(dc, rect, style);

        node = node->GetNext();
    }

#endif

    // Split up lines

    // We may need to go back to a previous child, in which case create the new line,
    // find the child corresponding to the start position of the string, and
    // continue.

    wxRect availableRect;

    node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        // If floating, ignore. We already laid out floats.
        // Also ignore if empty object, except if we haven't got any
        // size yet.
        if (child->IsFloating() || !child->IsShown() ||
            (child->GetRange().GetLength() == 0 && maxHeight > spaceBeforePara)
            )
        {
            node = node->GetNext();
            continue;
        }

        // If this is e.g. a composite text box, it will need to be laid out itself.
        // But if just a text fragment or image, for example, this will
        // do nothing. NB: won't we need to set the position after layout?
        // since for example if position is dependent on vertical line size, we
        // can't tell the position until the size is determined. So possibly introduce
        // another layout phase.

        // We may only be looking at part of a child, if we searched back for wrapping
        // and found a suitable point some way into the child. So get the size for the fragment
        // if necessary.

        long nextBreakPos = GetFirstLineBreakPosition(lastEndPos+1);
        long lastPosToUse = child->GetRange().GetEnd();
        bool lineBreakInThisObject = (nextBreakPos > -1 && nextBreakPos <= child->GetRange().GetEnd());

        if (lineBreakInThisObject)
            lastPosToUse = nextBreakPos;

        wxSize childSize;
        int childDescent = 0;

        int startOffset = (lineCount == 0 ? startPositionFirstLine : startPositionSubsequentLines);
        availableRect = wxRect(rect.x + startOffset, rect.y + currentPosition.y,
                                     rect.width - startOffset - rightIndent, rect.height);

        if (child->IsTopLevel())
        {
            wxSize oldSize = child->GetCachedSize();

            child->Invalidate(wxRICHTEXT_ALL);
            child->SetPosition(wxPoint(0, 0));

            // Lays out the object first with a given amount of space, and then if no width was specified in attr,
            // lays out the object again using the minimum size
            // The position will be determined by its location in its line,
            // and not by the child's actual position.
            child->LayoutToBestSize(dc, buffer,
                    GetAttributes(), child->GetAttributes(), availableRect, style);

            if (oldSize != child->GetCachedSize())
            {
                partialExtents.Clear();

                // Recalculate the partial text extents since the child object changed size
                GetRangeSize(GetRange(), paraSize, paraDescent, dc, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_CACHE_SIZE, wxPoint(0,0), & partialExtents);
            }
        }

        // Problem: we need to layout composites here for which we need the available width,
        // but we can't get the available width without using the float collector which
        // needs to know the object height.

        if ((nextBreakPos == -1) && (lastEndPos == child->GetRange().GetStart() - 1)) // i.e. we want to get the whole thing
        {
            childSize = child->GetCachedSize();
            childDescent = child->GetDescent();
        }
        else
        {
#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
            // Get height only, then the width using the partial extents
            GetRangeSize(wxRichTextRange(lastEndPos+1, lastPosToUse), childSize, childDescent, dc, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_HEIGHT_ONLY);
            childSize.x = wxRichTextGetRangeWidth(*this, wxRichTextRange(lastEndPos+1, lastPosToUse), partialExtents);
#else
            GetRangeSize(wxRichTextRange(lastEndPos+1, lastPosToUse), childSize, childDescent, dc, wxRICHTEXT_UNFORMATTED, rect.GetPosition());
#endif
        }

        bool doLoop = true;
        int loopIterations = 0;

        // If there are nested objects that need to lay themselves out, we have to do this in a
        // loop because the height of the object may well depend on the available width.
        // And because of floating object positioning, the available width depends on the
        // height of the object and whether it will clash with the floating objects.
        // So, we see whether the available width changes due to the presence of floating images.
        // If it does, then we'll use the new restricted width to find the object height again.
        // If this causes another restriction in the available width, we'll try again, until
        // either we lose patience or the available width settles down.
        do
        {
            loopIterations ++;

            wxRect oldAvailableRect = availableRect;

            // Available width depends on the floating objects and the line height.
            // Note: the floating objects may be placed vertically along the two side of
            // buffer, so we may have different available line widths with different
            // [startY, endY]. So, we can't determine how wide the available
            // space is until we know the exact line height.
            lineDescent = wxMax(childDescent, maxDescent);
            lineAscent = wxMax(childSize.y-childDescent, maxAscent);
            lineHeight = lineDescent + lineAscent;
            wxRect floatAvailableRect = collector->GetAvailableRect(rect.y + currentPosition.y, rect.y + currentPosition.y + lineHeight);

            // Adjust availableRect to the space that is available when taking floating objects into account.

            if (floatAvailableRect.x + startOffset > availableRect.x)
            {
                int newX = floatAvailableRect.x + startOffset;
                int newW = availableRect.width - (newX - availableRect.x);
                availableRect.x = newX;
                availableRect.width = newW;
            }

            if (floatAvailableRect.width < availableRect.width)
                availableRect.width = floatAvailableRect.width;

            currentPosition.x = availableRect.x - rect.x;

            if (child->IsTopLevel() && loopIterations <= 20)
            {
                if (availableRect != oldAvailableRect)
                {
                    wxSize oldSize = child->GetCachedSize();

                    //child->SetCachedSize(wxDefaultSize);
                    // Lays out the object first with a given amount of space, and then if no width was specified in attr,
                    // lays out the object again using the minimum size
                    child->Invalidate(wxRICHTEXT_ALL);
                    child->LayoutToBestSize(dc, buffer,
                                GetAttributes(), child->GetAttributes(), availableRect, style);
                    childSize = child->GetCachedSize();
                    childDescent = child->GetDescent();
                    //child->SetPosition(availableRect.GetPosition());

                    if (oldSize != child->GetCachedSize())
                    {
                        partialExtents.Clear();

                        // Recalculate the partial text extents since the child object changed size
                        GetRangeSize(GetRange(), paraSize, paraDescent, dc, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_CACHE_SIZE, wxPoint(0,0), & partialExtents);
                    }

                    // Go around the loop finding the available rect for the given floating objects
                }
                else
                    doLoop = false;
            }
            else
                doLoop = false;
        }
        while (doLoop);

        // Cases:
        // 1) There was a line break BEFORE the natural break
        // 2) There was a line break AFTER the natural break
        // 3) It's the last line
        // 4) The child still fits (carry on) - 'else' clause

        if ((lineBreakInThisObject && (childSize.x + currentWidth <= availableRect.width))
            ||
            (childSize.x + currentWidth > availableRect.width)
            ||
            ((childSize.x + currentWidth <= availableRect.width) && !node->GetNext())

            )
        {
            if (child->IsTopLevel())
            {
                // We can move it to the correct position at this point
                child->Move(GetPosition() + wxPoint(currentWidth, currentPosition.y));
            }

            long wrapPosition = 0;
            if ((childSize.x + currentWidth <= availableRect.width) && !node->GetNext() && !lineBreakInThisObject)
                wrapPosition = child->GetRange().GetEnd();
            else

            // Find a place to wrap. This may walk back to previous children,
            // for example if a word spans several objects.
            // Note: one object must contains only one wxTextAtrr, so the line height will not
            //       change inside one object. Thus, we can pass the remain line width to the
            //       FindWrapPosition function.
            if (!FindWrapPosition(wxRichTextRange(lastCompletedEndPos+1, child->GetRange().GetEnd()), dc, availableRect.width, wrapPosition, & partialExtents))
            {
                // If the function failed, just cut it off at the end of this child.
                wrapPosition = child->GetRange().GetEnd();
            }

            // FindWrapPosition can still return a value that will put us in an endless wrapping loop
            if (wrapPosition <= lastCompletedEndPos)
                wrapPosition = wxMax(lastCompletedEndPos+1,child->GetRange().GetEnd());

            // Line end position shouldn't be the same as the end, or greater.
            if (wrapPosition >= GetRange().GetEnd())
                wrapPosition = GetRange().GetEnd()-1;

            // wxLogDebug(wxT("Split at %ld"), wrapPosition);

            // Let's find the actual size of the current line now
            wxSize actualSize;
            wxRichTextRange actualRange(lastCompletedEndPos+1, wrapPosition);

            /// Use previous descent, not the wrapping descent we just found, since this may be too big
            /// for the fragment we're about to add.
            childDescent = maxDescent;

#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
            if (!child->IsEmpty())
            {
                // Get height only, then the width using the partial extents
                GetRangeSize(actualRange, actualSize, childDescent, dc, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_HEIGHT_ONLY);
                actualSize.x = wxRichTextGetRangeWidth(*this, actualRange, partialExtents);
            }
            else
#endif
                GetRangeSize(actualRange, actualSize, childDescent, dc, wxRICHTEXT_UNFORMATTED);

            currentWidth = actualSize.x;
            maxDescent = wxMax(childDescent, maxDescent);
            maxAscent = wxMax(actualSize.y-childDescent, maxAscent);
            lineHeight = maxDescent + maxAscent;

            if (lineHeight == 0 && buffer)
            {
                wxFont font(buffer->GetFontTable().FindFont(attr));
                wxCheckSetFont(dc, font);
                lineHeight = dc.GetCharHeight();
            }

            if (maxDescent == 0)
            {
                int w, h;
                dc.GetTextExtent(wxT("X"), & w, &h, & maxDescent);
            }

            // Add a new line
            wxRichTextLine* line = AllocateLine(lineCount);

            // Set relative range so we won't have to change line ranges when paragraphs are moved
            line->SetRange(wxRichTextRange(actualRange.GetStart() - GetRange().GetStart(), actualRange.GetEnd() - GetRange().GetStart()));
            line->SetPosition(currentPosition);
            line->SetSize(wxSize(currentWidth, lineHeight));
            line->SetDescent(maxDescent);

            maxHeight = currentPosition.y + lineHeight;

            // Now move down a line. TODO: add margins, spacing
            currentPosition.y += lineHeight;
            currentPosition.y += lineSpacing;
            maxDescent = 0;
            maxAscent = 0;
            maxWidth = wxMax(maxWidth, currentWidth+startOffset);
            currentWidth = 0;

            lineCount ++;

            // TODO: account for zero-length objects, such as fields
            // wxASSERT(wrapPosition > lastCompletedEndPos);

            lastEndPos = wrapPosition;
            lastCompletedEndPos = lastEndPos;

            lineHeight = 0;

            if (wrapPosition < GetRange().GetEnd()-1)
            {
                // May need to set the node back to a previous one, due to searching back in wrapping
                wxRichTextObject* childAfterWrapPosition = FindObjectAtPosition(wrapPosition+1);
                if (childAfterWrapPosition)
                    node = m_children.Find(childAfterWrapPosition);
                else
                    node = node->GetNext();
            }
            else
                node = node->GetNext();

            // Apply paragraph styles such as alignment to the wrapped line
            ApplyParagraphStyle(line, attr, availableRect, dc);
        }
        else
        {
            // We still fit, so don't add a line, and keep going
            currentWidth += childSize.x;
            maxDescent = wxMax(childDescent, maxDescent);
            maxAscent = wxMax(childSize.y-childDescent, maxAscent);
            lineHeight = maxDescent + maxAscent;

            maxWidth = wxMax(maxWidth, currentWidth+startOffset);
            lastEndPos = child->GetRange().GetEnd();

            node = node->GetNext();
        }
    }

    //wxASSERT(!(lastCompletedEndPos != -1 && lastCompletedEndPos < GetRange().GetEnd()-1));

    // Remove remaining unused line objects, if any
    ClearUnusedLines(lineCount);

    // We need to add back the margins etc.
    {
        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(maxWidth, currentPosition.y + spaceAfterPara));
        GetBoxRects(dc, buffer, GetAttributes(), marginRect, borderRect, contentRect, paddingRect, outlineRect);
        SetCachedSize(marginRect.GetSize());
    }

    // The maximum size is the length of the paragraph stretched out into a line.
    // So if there were a single word, or an image, or a fixed-size text box, the object could be shrunk around
    // this size. TODO: take into account line breaks.
    {
        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(paraSize.x, currentPosition.y + spaceAfterPara));
        GetBoxRects(dc, buffer, GetAttributes(), marginRect, borderRect, contentRect, paddingRect, outlineRect);
        SetMaxSize(marginRect.GetSize());
    }

    // Find the greatest minimum size. Currently we only look at non-text objects,
    // which isn't ideal but it would be slow to find the maximum word width to
    // use as the minimum.
    {
        int minWidth = 0;
        node = m_children.GetFirst();
        while (node)
        {
            wxRichTextObject* child = node->GetData();

            // If floating, ignore. We already laid out floats.
            // Also ignore if empty object, except if we haven't got any
            // size yet.
            if (!child->IsFloating() && child->GetRange().GetLength() != 0 && !child->IsKindOf(CLASSINFO(wxRichTextPlainText)))
            {
                if (child->GetCachedSize().x > minWidth)
                    minWidth = child->GetMinSize().x;
            }
            node = node->GetNext();
        }

        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(minWidth, currentPosition.y + spaceAfterPara));
        GetBoxRects(dc, buffer, GetAttributes(), marginRect, borderRect, contentRect, paddingRect, outlineRect);
        SetMinSize(marginRect.GetSize());
    }


#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
#if wxRICHTEXT_USE_OPTIMIZED_LINE_DRAWING
    // Use the text extents to calculate the size of each fragment in each line
    wxRichTextLineList::compatibility_iterator lineNode = m_cachedLines.GetFirst();
    while (lineNode)
    {
        wxRichTextLine* line = lineNode->GetData();
        wxRichTextRange lineRange = line->GetAbsoluteRange();

        // Loop through objects until we get to the one within range
        wxRichTextObjectList::compatibility_iterator node2 = m_children.GetFirst();

        while (node2)
        {
            wxRichTextObject* child = node2->GetData();

            if (child->GetRange().GetLength() > 0 && !child->GetRange().IsOutside(lineRange))
            {
                wxRichTextRange rangeToUse = lineRange;
                rangeToUse.LimitTo(child->GetRange());

                // Find the size of the child from the text extents, and store in an array
                // for drawing later
                int left = 0;
                if (rangeToUse.GetStart() > GetRange().GetStart())
                    left = partialExtents[(rangeToUse.GetStart()-1) - GetRange().GetStart()];
                int right = partialExtents[rangeToUse.GetEnd() - GetRange().GetStart()];
                int sz = right - left;
                line->GetObjectSizes().Add(sz);
            }
            else if (child->GetRange().GetStart() > lineRange.GetEnd())
                // Can break out of inner loop now since we've passed this line's range
                break;

            node2 = node2->GetNext();
        }

        lineNode = lineNode->GetNext();
    }
#endif
#endif

    return true;
}

/// Apply paragraph styles, such as centering, to wrapped lines
/// TODO: take into account box attributes, possibly
void wxRichTextParagraph::ApplyParagraphStyle(wxRichTextLine* line, const wxRichTextAttr& attr, const wxRect& rect, wxDC& dc)
{
    if (!attr.HasAlignment())
        return;

    wxPoint pos = line->GetPosition();
    wxSize size = line->GetSize();

    // centering, right-justification
    if (attr.HasAlignment() && GetAttributes().GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
    {
        int rightIndent = ConvertTenthsMMToPixels(dc, attr.GetRightIndent());
        pos.x = (rect.GetWidth() - rightIndent - size.x)/2 + pos.x;
        line->SetPosition(pos);
    }
    else if (attr.HasAlignment() && GetAttributes().GetAlignment() == wxTEXT_ALIGNMENT_RIGHT)
    {
        int rightIndent = ConvertTenthsMMToPixels(dc, attr.GetRightIndent());
        pos.x = pos.x + rect.GetWidth() - size.x - rightIndent;
        line->SetPosition(pos);
    }
}

/// Insert text at the given position
bool wxRichTextParagraph::InsertText(long pos, const wxString& text)
{
    wxRichTextObject* childToUse = NULL;
    wxRichTextObjectList::compatibility_iterator nodeToUse = wxRichTextObjectList::compatibility_iterator();

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        if (child->GetRange().Contains(pos) && child->GetRange().GetLength() > 0)
        {
            childToUse = child;
            nodeToUse = node;
            break;
        }

        node = node->GetNext();
    }

    if (childToUse)
    {
        wxRichTextPlainText* textObject = wxDynamicCast(childToUse, wxRichTextPlainText);
        if (textObject)
        {
            int posInString = pos - textObject->GetRange().GetStart();

            wxString newText = textObject->GetText().Mid(0, posInString) +
                               text + textObject->GetText().Mid(posInString);
            textObject->SetText(newText);

            int textLength = text.length();

            textObject->SetRange(wxRichTextRange(textObject->GetRange().GetStart(),
                                                 textObject->GetRange().GetEnd() + textLength));

            // Increment the end range of subsequent fragments in this paragraph.
            // We'll set the paragraph range itself at a higher level.

            wxRichTextObjectList::compatibility_iterator node = nodeToUse->GetNext();
            while (node)
            {
                wxRichTextObject* child = node->GetData();
                child->SetRange(wxRichTextRange(textObject->GetRange().GetStart() + textLength,
                                                 textObject->GetRange().GetEnd() + textLength));

                node = node->GetNext();
            }

            return true;
        }
        else
        {
            // TODO: if not a text object, insert at closest position, e.g. in front of it
        }
    }
    else
    {
        // Add at end.
        // Don't pass parent initially to suppress auto-setting of parent range.
        // We'll do that at a higher level.
        wxRichTextPlainText* textObject = new wxRichTextPlainText(text, this);

        AppendChild(textObject);
        return true;
    }

    return false;
}

void wxRichTextParagraph::Copy(const wxRichTextParagraph& obj)
{
    wxRichTextCompositeObject::Copy(obj);
}

/// Clear the cached lines
void wxRichTextParagraph::ClearLines()
{
    WX_CLEAR_LIST(wxRichTextLineList, m_cachedLines);
}

/// Get/set the object size for the given range. Returns false if the range
/// is invalid for this object.
bool wxRichTextParagraph::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position, wxArrayInt* partialExtents) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    if (flags & wxRICHTEXT_UNFORMATTED)
    {
        // Just use unformatted data, assume no line breaks
        // TODO: take into account line breaks

        wxSize sz;

        wxArrayInt childExtents;
        wxArrayInt* p;
        if (partialExtents)
            p = & childExtents;
        else
            p = NULL;

        wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
        while (node)
        {

            wxRichTextObject* child = node->GetData();
            if (!child->GetRange().IsOutside(range))
            {
                // Floating objects have a zero size within the paragraph.
                if (child->IsFloating())
                {
                    if (partialExtents)
                    {
                        int lastSize;
                        if (partialExtents->GetCount() > 0)
                            lastSize = (*partialExtents)[partialExtents->GetCount()-1];
                        else
                            lastSize = 0;

                        partialExtents->Add(0 /* zero size */ + lastSize);
                    }
                }
                else
                {
                    wxSize childSize;

                    wxRichTextRange rangeToUse = range;
                    rangeToUse.LimitTo(child->GetRange());
#if 0
                    if (child->IsTopLevel())
                        rangeToUse = child->GetOwnRange();
#endif
                    int childDescent = 0;

                    // At present wxRICHTEXT_HEIGHT_ONLY is only fast if we're already cached the size,
                    // but it's only going to be used after caching has taken place.
                    if ((flags & wxRICHTEXT_HEIGHT_ONLY) && child->GetCachedSize().y != 0)
                    {
                        childDescent = child->GetDescent();
                        childSize = child->GetCachedSize();

                        sz.y = wxMax(sz.y, childSize.y);
                        sz.x += childSize.x;
                        descent = wxMax(descent, childDescent);
                    }
                    else if (child->IsTopLevel())
                    {
                        childDescent = child->GetDescent();
                        childSize = child->GetCachedSize();

                        sz.y = wxMax(sz.y, childSize.y);
                        sz.x += childSize.x;
                        descent = wxMax(descent, childDescent);
                        if ((flags & wxRICHTEXT_CACHE_SIZE) && (rangeToUse == child->GetRange()))
                        {
                            child->SetCachedSize(childSize);
                            child->SetDescent(childDescent);
                        }

                        if (partialExtents)
                        {
                            int lastSize;
                            if (partialExtents->GetCount() > 0)
                                lastSize = (*partialExtents)[partialExtents->GetCount()-1];
                            else
                                lastSize = 0;

                            partialExtents->Add(childSize.x + lastSize);
                        }
                    }
                    else if (child->GetRangeSize(rangeToUse, childSize, childDescent, dc, flags, wxPoint(position.x + sz.x, position.y), p))
                    {
                        sz.y = wxMax(sz.y, childSize.y);
                        sz.x += childSize.x;
                        descent = wxMax(descent, childDescent);

                        if ((flags & wxRICHTEXT_CACHE_SIZE) && (rangeToUse == child->GetRange()))
                        {
                            child->SetCachedSize(childSize);
                            child->SetDescent(childDescent);
                        }

                        if (partialExtents)
                        {
                            int lastSize;
                            if (partialExtents->GetCount() > 0)
                                lastSize = (*partialExtents)[partialExtents->GetCount()-1];
                            else
                                lastSize = 0;

                            size_t i;
                            for (i = 0; i < childExtents.GetCount(); i++)
                            {
                                partialExtents->Add(childExtents[i] + lastSize);
                            }
                        }
                    }
                }

                if (p)
                    p->Clear();
            }

            node = node->GetNext();
        }
        size = sz;
    }
    else
    {
        // Use formatted data, with line breaks
        wxSize sz;

        // We're going to loop through each line, and then for each line,
        // call GetRangeSize for the fragment that comprises that line.
        // Only we have to do that multiple times within the line, because
        // the line may be broken into pieces. For now ignore line break commands
        // (so we can assume that getting the unformatted size for a fragment
        // within a line is the actual size)

        wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetFirst();
        while (node)
        {
            wxRichTextLine* line = node->GetData();
            wxRichTextRange lineRange = line->GetAbsoluteRange();
            if (!lineRange.IsOutside(range))
            {
                wxSize lineSize;

                wxRichTextObjectList::compatibility_iterator node2 = m_children.GetFirst();
                while (node2)
                {
                    wxRichTextObject* child = node2->GetData();

                    if (!child->IsFloating() && !child->GetRange().IsOutside(lineRange))
                    {
                        wxRichTextRange rangeToUse = lineRange;
                        rangeToUse.LimitTo(child->GetRange());
                        if (child->IsTopLevel())
                            rangeToUse = child->GetOwnRange();

                        wxSize childSize;
                        int childDescent = 0;
                        if (child->GetRangeSize(rangeToUse, childSize, childDescent, dc, flags, wxPoint(position.x + sz.x, position.y)))
                        {
                            lineSize.y = wxMax(lineSize.y, childSize.y);
                            lineSize.x += childSize.x;
                        }
                        descent = wxMax(descent, childDescent);
                    }

                    node2 = node2->GetNext();
                }

                // Increase size by a line (TODO: paragraph spacing)
                sz.y += lineSize.y;
                sz.x = wxMax(sz.x, lineSize.x);
            }
            node = node->GetNext();
        }
        size = sz;
    }
    return true;
}

/// Finds the absolute position and row height for the given character position
bool wxRichTextParagraph::FindPosition(wxDC& dc, long index, wxPoint& pt, int* height, bool forceLineStart)
{
    if (index == -1)
    {
        wxRichTextLine* line = ((wxRichTextParagraphLayoutBox*)GetParent())->GetLineAtPosition(0);
        if (line)
            *height = line->GetSize().y;
        else
            *height = dc.GetCharHeight();

        // -1 means 'the start of the buffer'.
        pt = GetPosition();
        if (line)
            pt = pt + line->GetPosition();

        return true;
    }

    // The final position in a paragraph is taken to mean the position
    // at the start of the next paragraph.
    if (index == GetRange().GetEnd())
    {
        wxRichTextParagraphLayoutBox* parent = wxDynamicCast(GetParent(), wxRichTextParagraphLayoutBox);
        wxASSERT( parent != NULL );

        // Find the height at the next paragraph, if any
        wxRichTextLine* line = parent->GetLineAtPosition(index + 1);
        if (line)
        {
            *height = line->GetSize().y;
            pt = line->GetAbsolutePosition();
        }
        else
        {
            *height = dc.GetCharHeight();
            int indent = ConvertTenthsMMToPixels(dc, m_attributes.GetLeftIndent());
            pt = wxPoint(indent, GetCachedSize().y);
        }

        return true;
    }

    if (index < GetRange().GetStart() || index > GetRange().GetEnd())
        return false;

    wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetFirst();
    while (node)
    {
        wxRichTextLine* line = node->GetData();
        wxRichTextRange lineRange = line->GetAbsoluteRange();
        if (index >= lineRange.GetStart() && index <= lineRange.GetEnd())
        {
            // If this is the last point in the line, and we're forcing the
            // returned value to be the start of the next line, do the required
            // thing.
            if (index == lineRange.GetEnd() && forceLineStart)
            {
                if (node->GetNext())
                {
                    wxRichTextLine* nextLine = node->GetNext()->GetData();
                    *height = nextLine->GetSize().y;
                    pt = nextLine->GetAbsolutePosition();
                    return true;
                }
            }

            pt.y = line->GetPosition().y + GetPosition().y;

            wxRichTextRange r(lineRange.GetStart(), index);
            wxSize rangeSize;
            int descent = 0;

            // We find the size of the line up to this point,
            // then we can add this size to the line start position and
            // paragraph start position to find the actual position.

            if (GetRangeSize(r, rangeSize, descent, dc, wxRICHTEXT_UNFORMATTED, line->GetPosition()+ GetPosition()))
            {
                pt.x = line->GetPosition().x + GetPosition().x + rangeSize.x;
                *height = line->GetSize().y;

                return true;
            }

        }

        node = node->GetNext();
    }

    return false;
}

/// Hit-testing: returns a flag indicating hit test details, plus
/// information about position
int wxRichTextParagraph::HitTest(wxDC& dc, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags)
{
    if (!IsShown())
        return wxRICHTEXT_HITTEST_NONE;

    // If we're in the top-level container, then we can return
    // a suitable hit test code even if the point is outside the container area,
    // so that we can position the caret sensibly even if we don't
    // click on valid content. If we're not at the top-level, and the point
    // is not within this paragraph object, then we don't want to stop more
    // precise hit-testing from working prematurely, so return immediately.
    // NEW STRATEGY: use the parent boundary to test whether we're in the
    // right region, not the paragraph, since the paragraph may be positioned
    // some way in from where the user clicks.
    {
        long tmpPos;
        wxRichTextObject* tempObj, *tempContextObj;
        if (GetParent() && GetParent()->wxRichTextObject::HitTest(dc, pt, tmpPos, & tempObj, & tempContextObj, flags) == wxRICHTEXT_HITTEST_NONE)
            return wxRICHTEXT_HITTEST_NONE;
    }

    wxRichTextObjectList::compatibility_iterator objNode = m_children.GetFirst();
    while (objNode)
    {
        wxRichTextObject* child = objNode->GetData();
        if (child->IsTopLevel() && ((flags & wxRICHTEXT_HITTEST_NO_NESTED_OBJECTS) == 0))
        {
            {
                int hitTest = child->HitTest(dc, pt, textPosition, obj, contextObj);
                if (hitTest != wxRICHTEXT_HITTEST_NONE)
                    return hitTest;
            }
        }

        objNode = objNode->GetNext();
    }

    wxPoint paraPos = GetPosition();

    wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetFirst();
    while (node)
    {
        wxRichTextLine* line = node->GetData();
        wxPoint linePos = paraPos + line->GetPosition();
        wxSize lineSize = line->GetSize();
        wxRichTextRange lineRange = line->GetAbsoluteRange();

        if (pt.y <= linePos.y + lineSize.y)
        {
            if (pt.x < linePos.x)
            {
                textPosition = lineRange.GetStart();
                *obj = FindObjectAtPosition(textPosition);
                *contextObj = GetContainer();
                return wxRICHTEXT_HITTEST_BEFORE|wxRICHTEXT_HITTEST_OUTSIDE;
            }
            else if (pt.x >= (linePos.x + lineSize.x))
            {
                textPosition = lineRange.GetEnd();
                *obj = FindObjectAtPosition(textPosition);
                *contextObj = GetContainer();
                return wxRICHTEXT_HITTEST_AFTER|wxRICHTEXT_HITTEST_OUTSIDE;
            }
            else
            {
#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
                wxArrayInt partialExtents;

                wxSize paraSize;
                int paraDescent;

                // This calculates the partial text extents
                GetRangeSize(lineRange, paraSize, paraDescent, dc, wxRICHTEXT_UNFORMATTED, linePos, & partialExtents);

                int lastX = linePos.x;
                size_t i;
                for (i = 0; i < partialExtents.GetCount(); i++)
                {
                    int nextX = partialExtents[i] + linePos.x;

                    if (pt.x >= lastX && pt.x <= nextX)
                    {
                        textPosition = i + lineRange.GetStart(); // minus 1?

                        *obj = FindObjectAtPosition(textPosition);
                        *contextObj = GetContainer();

                        // So now we know it's between i-1 and i.
                        // Let's see if we can be more precise about
                        // which side of the position it's on.

                        int midPoint = (nextX + lastX)/2;
                        if (pt.x >= midPoint)
                            return wxRICHTEXT_HITTEST_AFTER;
                        else
                            return wxRICHTEXT_HITTEST_BEFORE;
                    }

                    lastX = nextX;
                }
#else
                long i;
                int lastX = linePos.x;
                for (i = lineRange.GetStart(); i <= lineRange.GetEnd(); i++)
                {
                    wxSize childSize;
                    int descent = 0;

                    wxRichTextRange rangeToUse(lineRange.GetStart(), i);

                    GetRangeSize(rangeToUse, childSize, descent, dc, wxRICHTEXT_UNFORMATTED, linePos);

                    int nextX = childSize.x + linePos.x;

                    if (pt.x >= lastX && pt.x <= nextX)
                    {
                        textPosition = i;

                        *obj = FindObjectAtPosition(textPosition);
                        *contextObj = GetContainer();

                        // So now we know it's between i-1 and i.
                        // Let's see if we can be more precise about
                        // which side of the position it's on.

                        int midPoint = (nextX + lastX)/2;
                        if (pt.x >= midPoint)
                            return wxRICHTEXT_HITTEST_AFTER;
                        else
                            return wxRICHTEXT_HITTEST_BEFORE;
                    }
                    else
                    {
                        lastX = nextX;
                    }
                }
#endif
            }
        }

        node = node->GetNext();
    }

    return wxRICHTEXT_HITTEST_NONE;
}

/// Split an object at this position if necessary, and return
/// the previous object, or NULL if inserting at beginning.
wxRichTextObject* wxRichTextParagraph::SplitAt(long pos, wxRichTextObject** previousObject)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        if (pos == child->GetRange().GetStart())
        {
            if (previousObject)
            {
                if (node->GetPrevious())
                    *previousObject = node->GetPrevious()->GetData();
                else
                    *previousObject = NULL;
            }

            return child;
        }

        if (child->GetRange().Contains(pos))
        {
            // This should create a new object, transferring part of
            // the content to the old object and the rest to the new object.
            wxRichTextObject* newObject = child->DoSplit(pos);

            // If we couldn't split this object, just insert in front of it.
            if (!newObject)
            {
                // Maybe this is an empty string, try the next one
                // return child;
            }
            else
            {
                // Insert the new object after 'child'
                if (node->GetNext())
                    m_children.Insert(node->GetNext(), newObject);
                else
                    m_children.Append(newObject);
                newObject->SetParent(this);

                if (previousObject)
                    *previousObject = child;

                return newObject;
            }
        }

        node = node->GetNext();
    }
    if (previousObject)
        *previousObject = NULL;
    return NULL;
}

/// Move content to a list from obj on
void wxRichTextParagraph::MoveToList(wxRichTextObject* obj, wxList& list)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.Find(obj);
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        list.Append(child);

        wxRichTextObjectList::compatibility_iterator oldNode = node;

        node = node->GetNext();

        m_children.DeleteNode(oldNode);
    }
}

/// Add content back from list
void wxRichTextParagraph::MoveFromList(wxList& list)
{
    for (wxList::compatibility_iterator node = list.GetFirst(); node; node = node->GetNext())
    {
        AppendChild((wxRichTextObject*) node->GetData());
    }
}

/// Calculate range
void wxRichTextParagraph::CalculateRange(long start, long& end)
{
    wxRichTextCompositeObject::CalculateRange(start, end);

    // Add one for end of paragraph
    end ++;

    m_range.SetRange(start, end);
}

/// Find the object at the given position
wxRichTextObject* wxRichTextParagraph::FindObjectAtPosition(long position)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* obj = node->GetData();
        if (obj->GetRange().Contains(position) ||
            obj->GetRange().GetStart() == position ||
            obj->GetRange().GetEnd() == position)
            return obj;

        node = node->GetNext();
    }
    return NULL;
}

/// Get the plain text searching from the start or end of the range.
/// The resulting string may be shorter than the range given.
bool wxRichTextParagraph::GetContiguousPlainText(wxString& text, const wxRichTextRange& range, bool fromStart)
{
    text = wxEmptyString;

    if (fromStart)
    {
        wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
        while (node)
        {
            wxRichTextObject* obj = node->GetData();
            if (!obj->GetRange().IsOutside(range))
            {
                wxRichTextPlainText* textObj = wxDynamicCast(obj, wxRichTextPlainText);
                if (textObj)
                {
                    text += textObj->GetTextForRange(range);
                }
                else
                {
                    text += wxT(" ");
                }
            }

            node = node->GetNext();
        }
    }
    else
    {
        wxRichTextObjectList::compatibility_iterator node = m_children.GetLast();
        while (node)
        {
            wxRichTextObject* obj = node->GetData();
            if (!obj->GetRange().IsOutside(range))
            {
                wxRichTextPlainText* textObj = wxDynamicCast(obj, wxRichTextPlainText);
                if (textObj)
                {
                    text = textObj->GetTextForRange(range) + text;
                }
                else
                {
                    text = wxT(" ") + text;
                }
            }

            node = node->GetPrevious();
        }
    }

    return true;
}

/// Find a suitable wrap position.
bool wxRichTextParagraph::FindWrapPosition(const wxRichTextRange& range, wxDC& dc, int availableSpace, long& wrapPosition, wxArrayInt* partialExtents)
{
    if (range.GetLength() <= 0)
        return false;

    // Find the first position where the line exceeds the available space.
    wxSize sz;
    long breakPosition = range.GetEnd();

#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
    if (partialExtents && partialExtents->GetCount() >= (size_t) (GetRange().GetLength()-1)) // the final position in a paragraph is the newline
    {
        int widthBefore;

        if (range.GetStart() > GetRange().GetStart())
            widthBefore = (*partialExtents)[range.GetStart() - GetRange().GetStart() - 1];
        else
            widthBefore = 0;

        size_t i;
        for (i = (size_t) range.GetStart(); i <= (size_t) range.GetEnd(); i++)
        {
            int widthFromStartOfThisRange = (*partialExtents)[i - GetRange().GetStart()] - widthBefore;

            if (widthFromStartOfThisRange > availableSpace)
            {
                breakPosition = i-1;
                break;
            }
        }
    }
    else
#endif
    {
        // Binary chop for speed
        long minPos = range.GetStart();
        long maxPos = range.GetEnd();
        while (true)
        {
            if (minPos == maxPos)
            {
                int descent = 0;
                GetRangeSize(wxRichTextRange(range.GetStart(), minPos), sz, descent, dc, wxRICHTEXT_UNFORMATTED);

                if (sz.x > availableSpace)
                    breakPosition = minPos - 1;
                break;
            }
            else if ((maxPos - minPos) == 1)
            {
                int descent = 0;
                GetRangeSize(wxRichTextRange(range.GetStart(), minPos), sz, descent, dc, wxRICHTEXT_UNFORMATTED);

                if (sz.x > availableSpace)
                    breakPosition = minPos - 1;
                else
                {
                    GetRangeSize(wxRichTextRange(range.GetStart(), maxPos), sz, descent, dc, wxRICHTEXT_UNFORMATTED);
                    if (sz.x > availableSpace)
                        breakPosition = maxPos-1;
                }
                break;
            }
            else
            {
                long nextPos = minPos + ((maxPos - minPos) / 2);

                int descent = 0;
                GetRangeSize(wxRichTextRange(range.GetStart(), nextPos), sz, descent, dc, wxRICHTEXT_UNFORMATTED);

                if (sz.x > availableSpace)
                {
                    maxPos = nextPos;
                }
                else
                {
                    minPos = nextPos;
                }
            }
        }
    }

    // Now we know the last position on the line.
    // Let's try to find a word break.

    wxString plainText;
    if (GetContiguousPlainText(plainText, wxRichTextRange(range.GetStart(), breakPosition), false))
    {
        int newLinePos = plainText.Find(wxRichTextLineBreakChar);
        if (newLinePos != wxNOT_FOUND)
        {
            breakPosition = wxMax(0, range.GetStart() + newLinePos);
        }
        else
        {
            int spacePos = plainText.Find(wxT(' '), true);
            int tabPos = plainText.Find(wxT('\t'), true);
            int pos = wxMax(spacePos, tabPos);
            if (pos != wxNOT_FOUND)
            {
                int positionsFromEndOfString = plainText.length() - pos - 1;
                breakPosition = breakPosition - positionsFromEndOfString;
            }
        }
    }

    wrapPosition = breakPosition;

    return true;
}

/// Get the bullet text for this paragraph.
wxString wxRichTextParagraph::GetBulletText()
{
    if (GetAttributes().GetBulletStyle() == wxTEXT_ATTR_BULLET_STYLE_NONE ||
        (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_BITMAP))
        return wxEmptyString;

    int number = GetAttributes().GetBulletNumber();

    wxString text;
    if ((GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ARABIC) || (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_OUTLINE))
    {
        text.Printf(wxT("%d"), number);
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER)
    {
        // TODO: Unicode, and also check if number > 26
        text.Printf(wxT("%c"), (wxChar) (number+64));
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER)
    {
        // TODO: Unicode, and also check if number > 26
        text.Printf(wxT("%c"), (wxChar) (number+96));
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER)
    {
        text = wxRichTextDecimalToRoman(number);
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER)
    {
        text = wxRichTextDecimalToRoman(number);
        text.MakeLower();
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_SYMBOL)
    {
        text = GetAttributes().GetBulletText();
    }

    if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_OUTLINE)
    {
        // The outline style relies on the text being computed statically,
        // since it depends on other levels points (e.g. 1.2.1.1). So normally the bullet text
        // should be stored in the attributes; if not, just use the number for this
        // level, as previously computed.
        if (!GetAttributes().GetBulletText().IsEmpty())
            text = GetAttributes().GetBulletText();
    }

    if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_PARENTHESES)
    {
        text = wxT("(") + text + wxT(")");
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_RIGHT_PARENTHESIS)
    {
        text = text + wxT(")");
    }

    if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_PERIOD)
    {
        text += wxT(".");
    }

    return text;
}

/// Allocate or reuse a line object
wxRichTextLine* wxRichTextParagraph::AllocateLine(int pos)
{
    if (pos < (int) m_cachedLines.GetCount())
    {
        wxRichTextLine* line = m_cachedLines.Item(pos)->GetData();
        line->Init(this);
        return line;
    }
    else
    {
        wxRichTextLine* line = new wxRichTextLine(this);
        m_cachedLines.Append(line);
        return line;
    }
}

/// Clear remaining unused line objects, if any
bool wxRichTextParagraph::ClearUnusedLines(int lineCount)
{
    int cachedLineCount = m_cachedLines.GetCount();
    if ((int) cachedLineCount > lineCount)
    {
        for (int i = 0; i < (int) (cachedLineCount - lineCount); i ++)
        {
            wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetLast();
            wxRichTextLine* line = node->GetData();
            m_cachedLines.Erase(node);
            delete line;
        }
    }
    return true;
}

/// Get combined attributes of the base style, paragraph style and character style. We use this to dynamically
/// retrieve the actual style.
wxRichTextAttr wxRichTextParagraph::GetCombinedAttributes(const wxRichTextAttr& contentStyle, bool includingBoxAttr) const
{
    wxRichTextAttr attr;
    wxRichTextParagraphLayoutBox* buf = wxDynamicCast(GetParent(), wxRichTextParagraphLayoutBox);
    if (buf)
    {
        attr = buf->GetBasicStyle();
        if (!includingBoxAttr)
        {
            attr.GetTextBoxAttr().Reset();
            // The background colour will be painted by the container, and we don't
            // want to unnecessarily overwrite the background when we're drawing text
            // because this may erase the guideline (which appears just under the text
            // if there's no padding).
            attr.SetFlags(attr.GetFlags() & ~wxTEXT_ATTR_BACKGROUND_COLOUR);
        }
        wxRichTextApplyStyle(attr, GetAttributes());
    }
    else
        attr = GetAttributes();

    wxRichTextApplyStyle(attr, contentStyle);
    return attr;
}

/// Get combined attributes of the base style and paragraph style.
wxRichTextAttr wxRichTextParagraph::GetCombinedAttributes(bool includingBoxAttr) const
{
    wxRichTextAttr attr;
    wxRichTextParagraphLayoutBox* buf = wxDynamicCast(GetParent(), wxRichTextParagraphLayoutBox);
    if (buf)
    {
        attr = buf->GetBasicStyle();
        if (!includingBoxAttr)
            attr.GetTextBoxAttr().Reset();
        wxRichTextApplyStyle(attr, GetAttributes());
    }
    else
        attr = GetAttributes();

    return attr;
}

// Create default tabstop array
void wxRichTextParagraph::InitDefaultTabs()
{
    // create a default tab list at 10 mm each.
    for (int i = 0; i < 20; ++i)
    {
        sm_defaultTabs.Add(i*100);
    }
}

// Clear default tabstop array
void wxRichTextParagraph::ClearDefaultTabs()
{
    sm_defaultTabs.Clear();
}

void wxRichTextParagraph::LayoutFloat(wxDC& dc, const wxRect& rect, int style, wxRichTextFloatCollector* floatCollector)
{
    wxRichTextObjectList::compatibility_iterator node = GetChildren().GetFirst();
    while (node)
    {
        wxRichTextObject* anchored = node->GetData();
        if (anchored && anchored->IsFloating() && !floatCollector->HasFloat(anchored))
        {
            wxSize size;
            int descent, x = 0;
            anchored->GetRangeSize(anchored->GetRange(), size, descent, dc, style);

            int offsetY = 0;
            if (anchored->GetAttributes().GetTextBoxAttr().GetTop().IsValid())
            {
                offsetY = anchored->GetAttributes().GetTextBoxAttr().GetTop().GetValue();
                if (anchored->GetAttributes().GetTextBoxAttr().GetTop().GetUnits() == wxTEXT_ATTR_UNITS_TENTHS_MM)
                {
                    offsetY = ConvertTenthsMMToPixels(dc, offsetY);
                }
            }

            int pos = floatCollector->GetFitPosition(anchored->GetAttributes().GetTextBoxAttr().GetFloatMode(), rect.y + offsetY, size.y);

            /* Update the offset */
            int newOffsetY = pos - rect.y;
            if (newOffsetY != offsetY)
            {
                if (anchored->GetAttributes().GetTextBoxAttr().GetTop().GetUnits() == wxTEXT_ATTR_UNITS_TENTHS_MM)
                    newOffsetY = ConvertPixelsToTenthsMM(dc, newOffsetY);
                anchored->GetAttributes().GetTextBoxAttr().GetTop().SetValue(newOffsetY);
            }

            if (anchored->GetAttributes().GetTextBoxAttr().GetFloatMode() == wxTEXT_BOX_ATTR_FLOAT_LEFT)
                x = rect.x;
            else if (anchored->GetAttributes().GetTextBoxAttr().GetFloatMode() == wxTEXT_BOX_ATTR_FLOAT_RIGHT)
                x = rect.x + rect.width - size.x;

            anchored->SetPosition(wxPoint(x, pos));
            anchored->SetCachedSize(size);
            floatCollector->CollectFloat(this, anchored);
        }

        node = node->GetNext();
    }
}

// Get the first position from pos that has a line break character.
long wxRichTextParagraph::GetFirstLineBreakPosition(long pos)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* obj = node->GetData();
        if (pos >= obj->GetRange().GetStart() && pos <= obj->GetRange().GetEnd())
        {
            wxRichTextPlainText* textObj = wxDynamicCast(obj, wxRichTextPlainText);
            if (textObj)
            {
                long breakPos = textObj->GetFirstLineBreakPosition(pos);
                if (breakPos > -1)
                    return breakPos;
            }
        }
        node = node->GetNext();
    }
    return -1;
}

/*!
 * wxRichTextLine
 * This object represents a line in a paragraph, and stores
 * offsets from the start of the paragraph representing the
 * start and end positions of the line.
 */

wxRichTextLine::wxRichTextLine(wxRichTextParagraph* parent)
{
    Init(parent);
}

/// Initialisation
void wxRichTextLine::Init(wxRichTextParagraph* parent)
{
    m_parent = parent;
    m_range.SetRange(-1, -1);
    m_pos = wxPoint(0, 0);
    m_size = wxSize(0, 0);
    m_descent = 0;
#if wxRICHTEXT_USE_OPTIMIZED_LINE_DRAWING
    m_objectSizes.Clear();
#endif
}

/// Copy
void wxRichTextLine::Copy(const wxRichTextLine& obj)
{
    m_range = obj.m_range;
#if wxRICHTEXT_USE_OPTIMIZED_LINE_DRAWING
    m_objectSizes = obj.m_objectSizes;
#endif
}

/// Get the absolute object position
wxPoint wxRichTextLine::GetAbsolutePosition() const
{
    return m_parent->GetPosition() + m_pos;
}

/// Get the absolute range
wxRichTextRange wxRichTextLine::GetAbsoluteRange() const
{
    wxRichTextRange range(m_range.GetStart() + m_parent->GetRange().GetStart(), 0);
    range.SetEnd(range.GetStart() + m_range.GetLength()-1);
    return range;
}

/*!
 * wxRichTextPlainText
 * This object represents a single piece of text.
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextPlainText, wxRichTextObject)

wxRichTextPlainText::wxRichTextPlainText(const wxString& text, wxRichTextObject* parent, wxRichTextAttr* style):
    wxRichTextObject(parent)
{
    if (style)
        SetAttributes(*style);

    m_text = text;
}

#define USE_KERNING_FIX 1

// If insufficient tabs are defined, this is the tab width used
#define WIDTH_FOR_DEFAULT_TABS 50

/// Draw the item
bool wxRichTextPlainText::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int WXUNUSED(style))
{
    wxRichTextParagraph* para = wxDynamicCast(GetParent(), wxRichTextParagraph);
    wxASSERT (para != NULL);

    wxRichTextAttr textAttr(para ? para->GetCombinedAttributes(GetAttributes(), false /* no box attributes */) : GetAttributes());

    // Let's make the assumption for now that for content in a paragraph, including
    // text, we never have a discontinuous selection. So we only deal with a
    // single range.
    wxRichTextRange selectionRange;
    if (selection.IsValid())
    {
        wxRichTextRangeArray selectionRanges = selection.GetSelectionForObject(this);
        if (selectionRanges.GetCount() > 0)
            selectionRange = selectionRanges[0];
        else
            selectionRange = wxRICHTEXT_NO_SELECTION;
    }
    else
        selectionRange = wxRICHTEXT_NO_SELECTION;

    int offset = GetRange().GetStart();

    // Replace line break characters with spaces
    wxString str = m_text;
    wxString toRemove = wxRichTextLineBreakChar;
    str.Replace(toRemove, wxT(" "));
    if (textAttr.HasTextEffects() && (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_CAPITALS))
        str.MakeUpper();

    long len = range.GetLength();
    wxString stringChunk = str.Mid(range.GetStart() - offset, (size_t) len);

    // Test for the optimized situations where all is selected, or none
    // is selected.

    wxFont textFont(GetBuffer()->GetFontTable().FindFont(textAttr));
    wxCheckSetFont(dc, textFont);
    int charHeight = dc.GetCharHeight();

    int x, y;
    if ( textFont.Ok() )
    {
        if ( textAttr.HasTextEffects() && (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUPERSCRIPT) )
        {
            double size = static_cast<double>(textFont.GetPointSize()) / wxSCRIPT_MUL_FACTOR;
            textFont.SetPointSize( static_cast<int>(size) );
            x = rect.x;
            y = rect.y;
            wxCheckSetFont(dc, textFont);
        }
        else if ( textAttr.HasTextEffects() && (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUBSCRIPT) )
        {
            double size = static_cast<double>(textFont.GetPointSize()) / wxSCRIPT_MUL_FACTOR;
            textFont.SetPointSize( static_cast<int>(size) );
            x = rect.x;
            int sub_height = static_cast<int>( static_cast<double>(charHeight) / wxSCRIPT_MUL_FACTOR);
            y = rect.y + (rect.height - sub_height + (descent - m_descent));
            wxCheckSetFont(dc, textFont);
        }
        else
        {
            x = rect.x;
            y = rect.y + (rect.height - charHeight - (descent - m_descent));
        }
    }
    else
    {
        x = rect.x;
        y = rect.y + (rect.height - charHeight - (descent - m_descent));
    }

    // TODO: new selection code

    // (a) All selected.
    if (selectionRange.GetStart() <= range.GetStart() && selectionRange.GetEnd() >= range.GetEnd())
    {
        DrawTabbedString(dc, textAttr, rect, stringChunk, x, y, true);
    }
    // (b) None selected.
    else if (selectionRange.GetEnd() < range.GetStart() || selectionRange.GetStart() > range.GetEnd())
    {
        // Draw all unselected
        DrawTabbedString(dc, textAttr, rect, stringChunk, x, y, false);
    }
    else
    {
        // (c) Part selected, part not
        // Let's draw unselected chunk, selected chunk, then unselected chunk.

        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

        // 1. Initial unselected chunk, if any, up until start of selection.
        if (selectionRange.GetStart() > range.GetStart() && selectionRange.GetStart() <= range.GetEnd())
        {
            int r1 = range.GetStart();
            int s1 = selectionRange.GetStart()-1;
            int fragmentLen = s1 - r1 + 1;
            if (fragmentLen < 0)
            {
                wxLogDebug(wxT("Mid(%d, %d"), (int)(r1 - offset), (int)fragmentLen);
            }
            wxString stringFragment = str.Mid(r1 - offset, fragmentLen);

            DrawTabbedString(dc, textAttr, rect, stringFragment, x, y, false);

#if USE_KERNING_FIX
            if (stringChunk.Find(wxT("\t")) == wxNOT_FOUND)
            {
                // Compensate for kerning difference
                wxString stringFragment2(str.Mid(r1 - offset, fragmentLen+1));
                wxString stringFragment3(str.Mid(r1 - offset + fragmentLen, 1));

                wxCoord w1, h1, w2, h2, w3, h3;
                dc.GetTextExtent(stringFragment,  & w1, & h1);
                dc.GetTextExtent(stringFragment2, & w2, & h2);
                dc.GetTextExtent(stringFragment3, & w3, & h3);

                int kerningDiff = (w1 + w3) - w2;
                x = x - kerningDiff;
            }
#endif
        }

        // 2. Selected chunk, if any.
        if (selectionRange.GetEnd() >= range.GetStart())
        {
            int s1 = wxMax(selectionRange.GetStart(), range.GetStart());
            int s2 = wxMin(selectionRange.GetEnd(), range.GetEnd());

            int fragmentLen = s2 - s1 + 1;
            if (fragmentLen < 0)
            {
                wxLogDebug(wxT("Mid(%d, %d"), (int)(s1 - offset), (int)fragmentLen);
            }
            wxString stringFragment = str.Mid(s1 - offset, fragmentLen);

            DrawTabbedString(dc, textAttr, rect, stringFragment, x, y, true);

#if USE_KERNING_FIX
            if (stringChunk.Find(wxT("\t")) == wxNOT_FOUND)
            {
                // Compensate for kerning difference
                wxString stringFragment2(str.Mid(s1 - offset, fragmentLen+1));
                wxString stringFragment3(str.Mid(s1 - offset + fragmentLen, 1));

                wxCoord w1, h1, w2, h2, w3, h3;
                dc.GetTextExtent(stringFragment,  & w1, & h1);
                dc.GetTextExtent(stringFragment2, & w2, & h2);
                dc.GetTextExtent(stringFragment3, & w3, & h3);

                int kerningDiff = (w1 + w3) - w2;
                x = x - kerningDiff;
            }
#endif
        }

        // 3. Remaining unselected chunk, if any
        if (selectionRange.GetEnd() < range.GetEnd())
        {
            int s2 = wxMin(selectionRange.GetEnd()+1, range.GetEnd());
            int r2 = range.GetEnd();

            int fragmentLen = r2 - s2 + 1;
            if (fragmentLen < 0)
            {
                wxLogDebug(wxT("Mid(%d, %d"), (int)(s2 - offset), (int)fragmentLen);
            }
            wxString stringFragment = str.Mid(s2 - offset, fragmentLen);

            DrawTabbedString(dc, textAttr, rect, stringFragment, x, y, false);
        }
    }

    return true;
}

bool wxRichTextPlainText::DrawTabbedString(wxDC& dc, const wxRichTextAttr& attr, const wxRect& rect,wxString& str, wxCoord& x, wxCoord& y, bool selected)
{
    bool hasTabs = (str.Find(wxT('\t')) != wxNOT_FOUND);

    wxArrayInt tabArray;
    int tabCount;
    if (hasTabs)
    {
        if (attr.GetTabs().IsEmpty())
            tabArray = wxRichTextParagraph::GetDefaultTabs();
        else
            tabArray = attr.GetTabs();
        tabCount = tabArray.GetCount();

        for (int i = 0; i < tabCount; ++i)
        {
            int pos = tabArray[i];
            pos = ConvertTenthsMMToPixels(dc, pos);
            tabArray[i] = pos;
        }
    }
    else
        tabCount = 0;

    int nextTabPos = -1;
    int tabPos = -1;
    wxCoord w, h;

    if (selected)
    {
        wxColour highlightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
        wxColour highlightTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

        wxCheckSetBrush(dc, wxBrush(highlightColour));
        wxCheckSetPen(dc, wxPen(highlightColour));
        dc.SetTextForeground(highlightTextColour);
        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    }
    else
    {
        dc.SetTextForeground(attr.GetTextColour());

        if (attr.HasFlag(wxTEXT_ATTR_BACKGROUND_COLOUR) && attr.GetBackgroundColour().IsOk())
        {
            dc.SetBackgroundMode(wxBRUSHSTYLE_SOLID);
            dc.SetTextBackground(attr.GetBackgroundColour());
        }
        else
            dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    }

    wxCoord x_orig = GetParent()->GetPosition().x;
    while (hasTabs)
    {
        // the string has a tab
        // break up the string at the Tab
        wxString stringChunk = str.BeforeFirst(wxT('\t'));
        str = str.AfterFirst(wxT('\t'));
        dc.GetTextExtent(stringChunk, & w, & h);
        tabPos = x + w;
        bool not_found = true;
        for (int i = 0; i < tabCount && not_found; ++i)
        {
            nextTabPos = tabArray.Item(i) + x_orig;

            // Find the next tab position.
            // Even if we're at the end of the tab array, we must still draw the chunk.

            if (nextTabPos > tabPos || (i == (tabCount - 1)))
            {
                if (nextTabPos <= tabPos)
                {
                    int defaultTabWidth = ConvertTenthsMMToPixels(dc, WIDTH_FOR_DEFAULT_TABS);
                    nextTabPos = tabPos + defaultTabWidth;
                }

                not_found = false;
                if (selected)
                {
                    w = nextTabPos - x;
                    wxRect selRect(x, rect.y, w, rect.GetHeight());
                    dc.DrawRectangle(selRect);
                }
                dc.DrawText(stringChunk, x, y);

                if (attr.HasTextEffects() && (attr.GetTextEffects() & wxTEXT_ATTR_EFFECT_STRIKETHROUGH))
                {
                    wxPen oldPen = dc.GetPen();
                    wxCheckSetPen(dc, wxPen(attr.GetTextColour(), 1));
                    dc.DrawLine(x, (int) (y+(h/2)+0.5), x+w, (int) (y+(h/2)+0.5));
                    wxCheckSetPen(dc, oldPen);
                }

                x = nextTabPos;
            }
        }
        hasTabs = (str.Find(wxT('\t')) != wxNOT_FOUND);
    }

    if (!str.IsEmpty())
    {
        dc.GetTextExtent(str, & w, & h);
        if (selected)
        {
            wxRect selRect(x, rect.y, w, rect.GetHeight());
            dc.DrawRectangle(selRect);
        }
        dc.DrawText(str, x, y);

        if (attr.HasTextEffects() && (attr.GetTextEffects() & wxTEXT_ATTR_EFFECT_STRIKETHROUGH))
        {
            wxPen oldPen = dc.GetPen();
            wxCheckSetPen(dc, wxPen(attr.GetTextColour(), 1));
            dc.DrawLine(x, (int) (y+(h/2)+0.5), x+w, (int) (y+(h/2)+0.5));
            wxCheckSetPen(dc, oldPen);
        }

        x += w;
    }
    return true;

}

/// Lay the item out
bool wxRichTextPlainText::Layout(wxDC& dc, const wxRect& WXUNUSED(rect), int WXUNUSED(style))
{
    // Only lay out if we haven't already cached the size
    if (m_size.x == -1)
        GetRangeSize(GetRange(), m_size, m_descent, dc, 0, wxPoint(0, 0));
    m_maxSize = m_size;
    // Eventually we want to have a reasonable estimate of minimum size.
    m_minSize = wxSize(0, 0);
    return true;
}

/// Copy
void wxRichTextPlainText::Copy(const wxRichTextPlainText& obj)
{
    wxRichTextObject::Copy(obj);

    m_text = obj.m_text;
}

/// Get/set the object size for the given range. Returns false if the range
/// is invalid for this object.
bool wxRichTextPlainText::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int WXUNUSED(flags), wxPoint position, wxArrayInt* partialExtents) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    wxRichTextParagraph* para = wxDynamicCast(GetParent(), wxRichTextParagraph);
    wxASSERT (para != NULL);

    int relativeX = position.x - GetParent()->GetPosition().x;

    wxRichTextAttr textAttr(para ? para->GetCombinedAttributes(GetAttributes()) : GetAttributes());

    // Always assume unformatted text, since at this level we have no knowledge
    // of line breaks - and we don't need it, since we'll calculate size within
    // formatted text by doing it in chunks according to the line ranges

    bool bScript(false);
    wxFont font(GetBuffer()->GetFontTable().FindFont(textAttr));
    if (font.Ok())
    {
        if ( textAttr.HasTextEffects() && ( (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUPERSCRIPT)
            || (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUBSCRIPT) ) )
        {
            wxFont textFont = font;
            double size = static_cast<double>(textFont.GetPointSize()) / wxSCRIPT_MUL_FACTOR;
            textFont.SetPointSize( static_cast<int>(size) );
            wxCheckSetFont(dc, textFont);
            bScript = true;
        }
        else
        {
            wxCheckSetFont(dc, font);
        }
    }

    bool haveDescent = false;
    int startPos = range.GetStart() - GetRange().GetStart();
    long len = range.GetLength();

    wxString str(m_text);
    wxString toReplace = wxRichTextLineBreakChar;
    str.Replace(toReplace, wxT(" "));

    wxString stringChunk = str.Mid(startPos, (size_t) len);

    if (textAttr.HasTextEffects() && (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_CAPITALS))
        stringChunk.MakeUpper();

    wxCoord w, h;
    int width = 0;
    if (stringChunk.Find(wxT('\t')) != wxNOT_FOUND)
    {
        // the string has a tab
        wxArrayInt tabArray;
        if (textAttr.GetTabs().IsEmpty())
            tabArray = wxRichTextParagraph::GetDefaultTabs();
        else
            tabArray = textAttr.GetTabs();

        int tabCount = tabArray.GetCount();

        for (int i = 0; i < tabCount; ++i)
        {
            int pos = tabArray[i];
            pos = ((wxRichTextPlainText*) this)->ConvertTenthsMMToPixels(dc, pos);
            tabArray[i] = pos;
        }

        int nextTabPos = -1;

        while (stringChunk.Find(wxT('\t')) >= 0)
        {
            int absoluteWidth = 0;

            // the string has a tab
            // break up the string at the Tab
            wxString stringFragment = stringChunk.BeforeFirst(wxT('\t'));
            stringChunk = stringChunk.AfterFirst(wxT('\t'));

            if (partialExtents)
            {
                int oldWidth;
                if (partialExtents->GetCount() > 0)
                    oldWidth = (*partialExtents)[partialExtents->GetCount()-1];
                else
                    oldWidth = 0;

                // Add these partial extents
                wxArrayInt p;
                dc.GetPartialTextExtents(stringFragment, p);
                size_t j;
                for (j = 0; j < p.GetCount(); j++)
                    partialExtents->Add(oldWidth + p[j]);

                if (partialExtents->GetCount() > 0)
                    absoluteWidth = (*partialExtents)[(*partialExtents).GetCount()-1] + relativeX;
                else
                    absoluteWidth = relativeX;
            }
            else
            {
                dc.GetTextExtent(stringFragment, & w, & h);
                width += w;
                absoluteWidth = width + relativeX;
                haveDescent = true;
            }

            bool notFound = true;
            for (int i = 0; i < tabCount && notFound; ++i)
            {
                nextTabPos = tabArray.Item(i);

                // Find the next tab position.
                // Even if we're at the end of the tab array, we must still process the chunk.

                if (nextTabPos > absoluteWidth || (i == (tabCount - 1)))
                {
                    if (nextTabPos <= absoluteWidth)
                    {
                        int defaultTabWidth = ((wxRichTextPlainText*) this)->ConvertTenthsMMToPixels(dc, WIDTH_FOR_DEFAULT_TABS);
                        nextTabPos = absoluteWidth + defaultTabWidth;
                    }

                    notFound = false;
                    width = nextTabPos - relativeX;

                    if (partialExtents)
                        partialExtents->Add(width);
                }
            }
        }
    }

    if (!stringChunk.IsEmpty())
    {
        if (partialExtents)
        {
            int oldWidth;
            if (partialExtents->GetCount() > 0)
                oldWidth = (*partialExtents)[partialExtents->GetCount()-1];
            else
                oldWidth = 0;

            // Add these partial extents
            wxArrayInt p;
            dc.GetPartialTextExtents(stringChunk, p);
            size_t j;
            for (j = 0; j < p.GetCount(); j++)
                partialExtents->Add(oldWidth + p[j]);
        }
        else
        {
            dc.GetTextExtent(stringChunk, & w, & h, & descent);
            width += w;
            haveDescent = true;
        }
    }

    if (partialExtents)
    {
        int charHeight = dc.GetCharHeight();
        if ((*partialExtents).GetCount() > 0)
            w = (*partialExtents)[partialExtents->GetCount()-1];
        else
            w = 0;
        size = wxSize(w, charHeight);
    }
    else
    {
        size = wxSize(width, dc.GetCharHeight());
    }

    if (!haveDescent)
        dc.GetTextExtent(wxT("X"), & w, & h, & descent);

    if ( bScript )
        dc.SetFont(font);

    return true;
}

/// Do a split, returning an object containing the second part, and setting
/// the first part in 'this'.
wxRichTextObject* wxRichTextPlainText::DoSplit(long pos)
{
    long index = pos - GetRange().GetStart();

    if (index < 0 || index >= (int) m_text.length())
        return NULL;

    wxString firstPart = m_text.Mid(0, index);
    wxString secondPart = m_text.Mid(index);

    m_text = firstPart;

    wxRichTextPlainText* newObject = new wxRichTextPlainText(secondPart);
    newObject->SetAttributes(GetAttributes());

    newObject->SetRange(wxRichTextRange(pos, GetRange().GetEnd()));
    GetRange().SetEnd(pos-1);

    return newObject;
}

/// Calculate range
void wxRichTextPlainText::CalculateRange(long start, long& end)
{
    end = start + m_text.length() - 1;
    m_range.SetRange(start, end);
}

/// Delete range
bool wxRichTextPlainText::DeleteRange(const wxRichTextRange& range)
{
    wxRichTextRange r = range;

    r.LimitTo(GetRange());

    if (r.GetStart() == GetRange().GetStart() && r.GetEnd() == GetRange().GetEnd())
    {
        m_text.Empty();
        return true;
    }

    long startIndex = r.GetStart() - GetRange().GetStart();
    long len = r.GetLength();

    m_text = m_text.Mid(0, startIndex) + m_text.Mid(startIndex+len);
    return true;
}

/// Get text for the given range.
wxString wxRichTextPlainText::GetTextForRange(const wxRichTextRange& range) const
{
    wxRichTextRange r = range;

    r.LimitTo(GetRange());

    long startIndex = r.GetStart() - GetRange().GetStart();
    long len = r.GetLength();

    return m_text.Mid(startIndex, len);
}

/// Returns true if this object can merge itself with the given one.
bool wxRichTextPlainText::CanMerge(wxRichTextObject* object) const
{
    return object->GetClassInfo() == CLASSINFO(wxRichTextPlainText) &&
        (m_text.empty() || wxTextAttrEq(GetAttributes(), object->GetAttributes()));
}

/// Returns true if this object merged itself with the given one.
/// The calling code will then delete the given object.
bool wxRichTextPlainText::Merge(wxRichTextObject* object)
{
    wxRichTextPlainText* textObject = wxDynamicCast(object, wxRichTextPlainText);
    wxASSERT( textObject != NULL );

    if (textObject)
    {
        m_text += textObject->GetText();
        wxRichTextApplyStyle(m_attributes, textObject->GetAttributes());
        return true;
    }
    else
        return false;
}

/// Dump to output stream for debugging
void wxRichTextPlainText::Dump(wxTextOutputStream& stream)
{
    wxRichTextObject::Dump(stream);
    stream << m_text << wxT("\n");
}

/// Get the first position from pos that has a line break character.
long wxRichTextPlainText::GetFirstLineBreakPosition(long pos)
{
    int i;
    int len = m_text.length();
    int startPos = pos - m_range.GetStart();
    for (i = startPos; i < len; i++)
    {
        wxChar ch = m_text[i];
        if (ch == wxRichTextLineBreakChar)
        {
            return i + m_range.GetStart();
        }
    }
    return -1;
}

/*!
 * wxRichTextBuffer
 * This is a kind of box, used to represent the whole buffer
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextBuffer, wxRichTextParagraphLayoutBox)

wxList                  wxRichTextBuffer::sm_handlers;
wxRichTextRenderer*     wxRichTextBuffer::sm_renderer = NULL;
int                     wxRichTextBuffer::sm_bulletRightMargin = 20;
float                   wxRichTextBuffer::sm_bulletProportion = (float) 0.3;

/// Initialisation
void wxRichTextBuffer::Init()
{
    m_commandProcessor = new wxCommandProcessor;
    m_styleSheet = NULL;
    m_modified = false;
    m_batchedCommandDepth = 0;
    m_batchedCommand = NULL;
    m_suppressUndo = 0;
    m_handlerFlags = 0;
    m_scale = 1.0;
}

/// Initialisation
wxRichTextBuffer::~wxRichTextBuffer()
{
    delete m_commandProcessor;
    delete m_batchedCommand;

    ClearStyleStack();
    ClearEventHandlers();
}

void wxRichTextBuffer::ResetAndClearCommands()
{
    Reset();

    GetCommandProcessor()->ClearCommands();

    Modify(false);
    Invalidate(wxRICHTEXT_ALL);
}

void wxRichTextBuffer::Copy(const wxRichTextBuffer& obj)
{
    wxRichTextParagraphLayoutBox::Copy(obj);

    m_styleSheet = obj.m_styleSheet;
    m_modified = obj.m_modified;
    m_batchedCommandDepth = 0;
    if (m_batchedCommand)
        delete m_batchedCommand;
    m_batchedCommand = NULL;
    m_suppressUndo = obj.m_suppressUndo;
    m_invalidRange = obj.m_invalidRange;
}

/// Push style sheet to top of stack
bool wxRichTextBuffer::PushStyleSheet(wxRichTextStyleSheet* styleSheet)
{
    if (m_styleSheet)
        styleSheet->InsertSheet(m_styleSheet);

    SetStyleSheet(styleSheet);

    return true;
}

/// Pop style sheet from top of stack
wxRichTextStyleSheet* wxRichTextBuffer::PopStyleSheet()
{
    if (m_styleSheet)
    {
        wxRichTextStyleSheet* oldSheet = m_styleSheet;
        m_styleSheet = oldSheet->GetNextSheet();
        oldSheet->Unlink();

        return oldSheet;
    }
    else
        return NULL;
}

/// Submit command to insert paragraphs
bool wxRichTextBuffer::InsertParagraphsWithUndo(long pos, const wxRichTextParagraphLayoutBox& paragraphs, wxRichTextCtrl* ctrl, int flags)
{
    return ctrl->GetFocusObject()->InsertParagraphsWithUndo(pos, paragraphs, ctrl, this, flags);
}

/// Submit command to insert paragraphs
bool wxRichTextParagraphLayoutBox::InsertParagraphsWithUndo(long pos, const wxRichTextParagraphLayoutBox& paragraphs, wxRichTextCtrl* ctrl, wxRichTextBuffer* buffer, int WXUNUSED(flags))
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Text"), wxRICHTEXT_INSERT, buffer, this, ctrl, false);

    action->GetNewParagraphs() = paragraphs;

    action->SetPosition(pos);

    wxRichTextRange range = wxRichTextRange(pos, pos + paragraphs.GetOwnRange().GetEnd() - 1);
    if (!paragraphs.GetPartialParagraph())
        range.SetEnd(range.GetEnd()+1);

    // Set the range we'll need to delete in Undo
    action->SetRange(range);

    buffer->SubmitAction(action);

    return true;
}

/// Submit command to insert the given text
bool wxRichTextBuffer::InsertTextWithUndo(long pos, const wxString& text, wxRichTextCtrl* ctrl, int flags)
{
    return ctrl->GetFocusObject()->InsertTextWithUndo(pos, text, ctrl, this, flags);
}

/// Submit command to insert the given text
bool wxRichTextParagraphLayoutBox::InsertTextWithUndo(long pos, const wxString& text, wxRichTextCtrl* ctrl, wxRichTextBuffer* buffer, int flags)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Text"), wxRICHTEXT_INSERT, buffer, this, ctrl, false);

    wxRichTextAttr* p = NULL;
    wxRichTextAttr paraAttr;
    if (flags & wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE)
    {
        // Get appropriate paragraph style
        paraAttr = GetStyleForNewParagraph(buffer, pos, false, false);
        if (!paraAttr.IsDefault())
            p = & paraAttr;
    }

    action->GetNewParagraphs().AddParagraphs(text, p);

    int length = action->GetNewParagraphs().GetOwnRange().GetLength();

    if (!text.empty() && text.Last() != wxT('\n'))
    {
        // Don't count the newline when undoing
        length --;
        action->GetNewParagraphs().SetPartialParagraph(true);
    }
    else if (!text.empty() && text.Last() == wxT('\n'))
        length --;

    action->SetPosition(pos);

    // Set the range we'll need to delete in Undo
    action->SetRange(wxRichTextRange(pos, pos + length - 1));

    buffer->SubmitAction(action);

    return true;
}

/// Submit command to insert the given text
bool wxRichTextBuffer::InsertNewlineWithUndo(long pos, wxRichTextCtrl* ctrl, int flags)
{
    return ctrl->GetFocusObject()->InsertNewlineWithUndo(pos, ctrl, this, flags);
}

/// Submit command to insert the given text
bool wxRichTextParagraphLayoutBox::InsertNewlineWithUndo(long pos, wxRichTextCtrl* ctrl, wxRichTextBuffer* buffer, int flags)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Text"), wxRICHTEXT_INSERT, buffer, this, ctrl, false);

    wxRichTextAttr* p = NULL;
    wxRichTextAttr paraAttr;
    if (flags & wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE)
    {
        paraAttr = GetStyleForNewParagraph(buffer, pos, false, true /* look for next paragraph style */);
        if (!paraAttr.IsDefault())
            p = & paraAttr;
    }

    wxRichTextAttr attr(buffer->GetDefaultStyle());

    wxRichTextParagraph* newPara = new wxRichTextParagraph(wxEmptyString, this, & attr);
    action->GetNewParagraphs().AppendChild(newPara);
    action->GetNewParagraphs().UpdateRanges();
    action->GetNewParagraphs().SetPartialParagraph(false);
    wxRichTextParagraph* para = GetParagraphAtPosition(pos, false);
    long pos1 = pos;

    if (p)
        newPara->SetAttributes(*p);

    if (flags & wxRICHTEXT_INSERT_INTERACTIVE)
    {
        if (para && para->GetRange().GetEnd() == pos)
            pos1 ++;

        // Now see if we need to number the paragraph.
        if (newPara->GetAttributes().HasBulletNumber())
        {
            wxRichTextAttr numberingAttr;
            if (FindNextParagraphNumber(para, numberingAttr))
                wxRichTextApplyStyle(newPara->GetAttributes(), (const wxRichTextAttr&) numberingAttr);
        }
    }

    action->SetPosition(pos);

    // Use the default character style
    // Use the default character style
    if (!buffer->GetDefaultStyle().IsDefault() && newPara->GetChildren().GetFirst())
    {
        // Check whether the default style merely reflects the paragraph/basic style,
        // in which case don't apply it.
        wxRichTextAttr defaultStyle(buffer->GetDefaultStyle());
        wxRichTextAttr toApply;
        if (para)
        {
            wxRichTextAttr combinedAttr = para->GetCombinedAttributes(true /* include box attributes */);
            wxRichTextAttr newAttr;
            // This filters out attributes that are accounted for by the current
            // paragraph/basic style
            wxRichTextApplyStyle(toApply, defaultStyle, & combinedAttr);
        }
        else
            toApply = defaultStyle;

        if (!toApply.IsDefault())
            newPara->GetChildren().GetFirst()->GetData()->SetAttributes(toApply);
    }

    // Set the range we'll need to delete in Undo
    action->SetRange(wxRichTextRange(pos1, pos1));

    buffer->SubmitAction(action);

    return true;
}

/// Submit command to insert the given image
bool wxRichTextBuffer::InsertImageWithUndo(long pos, const wxRichTextImageBlock& imageBlock, wxRichTextCtrl* ctrl, int flags,
                                            const wxRichTextAttr& textAttr)
{
    return ctrl->GetFocusObject()->InsertImageWithUndo(pos, imageBlock, ctrl, this, flags, textAttr);
}

/// Submit command to insert the given image
bool wxRichTextParagraphLayoutBox::InsertImageWithUndo(long pos, const wxRichTextImageBlock& imageBlock,
                                                        wxRichTextCtrl* ctrl, wxRichTextBuffer* buffer, int flags,
                                                        const wxRichTextAttr& textAttr)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Image"), wxRICHTEXT_INSERT, buffer, this, ctrl, false);

    wxRichTextAttr* p = NULL;
    wxRichTextAttr paraAttr;
    if (flags & wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE)
    {
        paraAttr = GetStyleForNewParagraph(buffer, pos);
        if (!paraAttr.IsDefault())
            p = & paraAttr;
    }

    wxRichTextAttr attr(buffer->GetDefaultStyle());

    wxRichTextParagraph* newPara = new wxRichTextParagraph(this, & attr);
    if (p)
        newPara->SetAttributes(*p);

    wxRichTextImage* imageObject = new wxRichTextImage(imageBlock, newPara);
    newPara->AppendChild(imageObject);
    imageObject->SetAttributes(textAttr);
    action->GetNewParagraphs().AppendChild(newPara);
    action->GetNewParagraphs().UpdateRanges();

    action->GetNewParagraphs().SetPartialParagraph(true);

    action->SetPosition(pos);

    // Set the range we'll need to delete in Undo
    action->SetRange(wxRichTextRange(pos, pos));

    buffer->SubmitAction(action);

    return true;
}

// Insert an object with no change of it
wxRichTextObject* wxRichTextBuffer::InsertObjectWithUndo(long pos, wxRichTextObject *object, wxRichTextCtrl* ctrl, int flags)
{
    return ctrl->GetFocusObject()->InsertObjectWithUndo(pos, object, ctrl, this, flags);
}

// Insert an object with no change of it
wxRichTextObject* wxRichTextParagraphLayoutBox::InsertObjectWithUndo(long pos, wxRichTextObject *object, wxRichTextCtrl* ctrl, wxRichTextBuffer* buffer, int flags)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Object"), wxRICHTEXT_INSERT, buffer, this, ctrl, false);

    wxRichTextAttr* p = NULL;
    wxRichTextAttr paraAttr;
    if (flags & wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE)
    {
        paraAttr = GetStyleForNewParagraph(buffer, pos);
        if (!paraAttr.IsDefault())
            p = & paraAttr;
    }

    wxRichTextAttr attr(buffer->GetDefaultStyle());

    wxRichTextParagraph* newPara = new wxRichTextParagraph(this, & attr);
    if (p)
        newPara->SetAttributes(*p);

    newPara->AppendChild(object);
    action->GetNewParagraphs().AppendChild(newPara);
    action->GetNewParagraphs().UpdateRanges();

    action->GetNewParagraphs().SetPartialParagraph(true);

    action->SetPosition(pos);

    // Set the range we'll need to delete in Undo
    action->SetRange(wxRichTextRange(pos, pos));

    buffer->SubmitAction(action);

    wxRichTextObject* obj = GetLeafObjectAtPosition(pos);
    return obj;
}

/// Get the style that is appropriate for a new paragraph at this position.
/// If the previous paragraph has a paragraph style name, look up the next-paragraph
/// style.
wxRichTextAttr wxRichTextParagraphLayoutBox::GetStyleForNewParagraph(wxRichTextBuffer* buffer, long pos, bool caretPosition, bool lookUpNewParaStyle) const
{
    wxRichTextParagraph* para = GetParagraphAtPosition(pos, caretPosition);
    if (para)
    {
        wxRichTextAttr attr;
        bool foundAttributes = false;

        // Look for a matching paragraph style
        if (lookUpNewParaStyle && !para->GetAttributes().GetParagraphStyleName().IsEmpty() && buffer->GetStyleSheet())
        {
            wxRichTextParagraphStyleDefinition* paraDef = buffer->GetStyleSheet()->FindParagraphStyle(para->GetAttributes().GetParagraphStyleName());
            if (paraDef)
            {
                // If we're not at the end of the paragraph, then we apply THIS style, and not the designated next style.
                if (para->GetRange().GetEnd() == pos && !paraDef->GetNextStyle().IsEmpty())
                {
                    wxRichTextParagraphStyleDefinition* nextParaDef = buffer->GetStyleSheet()->FindParagraphStyle(paraDef->GetNextStyle());
                    if (nextParaDef)
                    {
                        foundAttributes = true;
                        attr = nextParaDef->GetStyleMergedWithBase(buffer->GetStyleSheet());
                    }
                }

                // If we didn't find the 'next style', use this style instead.
                if (!foundAttributes)
                {
                    foundAttributes = true;
                    attr = paraDef->GetStyleMergedWithBase(buffer->GetStyleSheet());
                }
            }
        }

        // Also apply list style if present
        if (lookUpNewParaStyle && !para->GetAttributes().GetListStyleName().IsEmpty() && buffer->GetStyleSheet())
        {
            wxRichTextListStyleDefinition* listDef = buffer->GetStyleSheet()->FindListStyle(para->GetAttributes().GetListStyleName());
            if (listDef)
            {
                int thisIndent = para->GetAttributes().GetLeftIndent();
                int thisLevel = para->GetAttributes().HasOutlineLevel() ? para->GetAttributes().GetOutlineLevel() : listDef->FindLevelForIndent(thisIndent);

                // Apply the overall list style, and item style for this level
                wxRichTextAttr listStyle(listDef->GetCombinedStyleForLevel(thisLevel, buffer->GetStyleSheet()));
                wxRichTextApplyStyle(attr, listStyle);
                attr.SetOutlineLevel(thisLevel);
                if (para->GetAttributes().HasBulletNumber())
                    attr.SetBulletNumber(para->GetAttributes().GetBulletNumber());
            }
        }

        if (!foundAttributes)
        {
            attr = para->GetAttributes();
            int flags = attr.GetFlags();

            // Eliminate character styles
            flags &= ( (~ wxTEXT_ATTR_FONT) |
                    (~ wxTEXT_ATTR_TEXT_COLOUR) |
                    (~ wxTEXT_ATTR_BACKGROUND_COLOUR) );
            attr.SetFlags(flags);
        }

        return attr;
    }
    else
        return wxRichTextAttr();
}

/// Submit command to delete this range
bool wxRichTextBuffer::DeleteRangeWithUndo(const wxRichTextRange& range, wxRichTextCtrl* ctrl)
{
    return ctrl->GetFocusObject()->DeleteRangeWithUndo(range, ctrl, this);
}

/// Submit command to delete this range
bool wxRichTextParagraphLayoutBox::DeleteRangeWithUndo(const wxRichTextRange& range, wxRichTextCtrl* ctrl, wxRichTextBuffer* buffer)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Delete"), wxRICHTEXT_DELETE, buffer, this, ctrl);

    action->SetPosition(ctrl->GetCaretPosition());

    // Set the range to delete
    action->SetRange(range);

    // Copy the fragment that we'll need to restore in Undo
    CopyFragment(range, action->GetOldParagraphs());

    // See if we're deleting a paragraph marker, in which case we need to
    // make a note not to copy the attributes from the 2nd paragraph to the 1st.
    if (range.GetStart() == range.GetEnd())
    {
        wxRichTextParagraph* para = GetParagraphAtPosition(range.GetStart());
        if (para && para->GetRange().GetEnd() == range.GetEnd())
        {
            wxRichTextParagraph* nextPara = GetParagraphAtPosition(range.GetStart()+1);
            if (nextPara && nextPara != para)
            {
                action->GetOldParagraphs().GetChildren().GetFirst()->GetData()->SetAttributes(nextPara->GetAttributes());
                action->GetOldParagraphs().GetAttributes().SetFlags(action->GetOldParagraphs().GetAttributes().GetFlags() | wxTEXT_ATTR_KEEP_FIRST_PARA_STYLE);
            }
        }
    }

    buffer->SubmitAction(action);

    return true;
}

/// Collapse undo/redo commands
bool wxRichTextBuffer::BeginBatchUndo(const wxString& cmdName)
{
    if (m_batchedCommandDepth == 0)
    {
        wxASSERT(m_batchedCommand == NULL);
        if (m_batchedCommand)
        {
            GetCommandProcessor()->Store(m_batchedCommand);
        }
        m_batchedCommand = new wxRichTextCommand(cmdName);
    }

    m_batchedCommandDepth ++;

    return true;
}

/// Collapse undo/redo commands
bool wxRichTextBuffer::EndBatchUndo()
{
    m_batchedCommandDepth --;

    wxASSERT(m_batchedCommandDepth >= 0);
    wxASSERT(m_batchedCommand != NULL);

    if (m_batchedCommandDepth == 0)
    {
        GetCommandProcessor()->Store(m_batchedCommand);
        m_batchedCommand = NULL;
    }

    return true;
}

/// Submit immediately, or delay according to whether collapsing is on
bool wxRichTextBuffer::SubmitAction(wxRichTextAction* action)
{
    if (BatchingUndo() && m_batchedCommand && !SuppressingUndo())
    {
        wxRichTextCommand* cmd = new wxRichTextCommand(action->GetName());
        cmd->AddAction(action);
        cmd->Do();
        cmd->GetActions().Clear();
        delete cmd;

        m_batchedCommand->AddAction(action);
    }
    else
    {
        wxRichTextCommand* cmd = new wxRichTextCommand(action->GetName());
        cmd->AddAction(action);

        // Only store it if we're not suppressing undo.
        return GetCommandProcessor()->Submit(cmd, !SuppressingUndo());
    }

    return true;
}

/// Begin suppressing undo/redo commands.
bool wxRichTextBuffer::BeginSuppressUndo()
{
    m_suppressUndo ++;

    return true;
}

/// End suppressing undo/redo commands.
bool wxRichTextBuffer::EndSuppressUndo()
{
    m_suppressUndo --;

    return true;
}

/// Begin using a style
bool wxRichTextBuffer::BeginStyle(const wxRichTextAttr& style)
{
    wxRichTextAttr newStyle(GetDefaultStyle());

    // Save the old default style
    m_attributeStack.Append((wxObject*) new wxRichTextAttr(GetDefaultStyle()));

    wxRichTextApplyStyle(newStyle, style);
    newStyle.SetFlags(style.GetFlags()|newStyle.GetFlags());

    SetDefaultStyle(newStyle);

    return true;
}

/// End the style
bool wxRichTextBuffer::EndStyle()
{
    if (!m_attributeStack.GetFirst())
    {
        wxLogDebug(_("Too many EndStyle calls!"));
        return false;
    }

    wxList::compatibility_iterator node = m_attributeStack.GetLast();
    wxRichTextAttr* attr = (wxRichTextAttr*)node->GetData();
    m_attributeStack.Erase(node);

    SetDefaultStyle(*attr);

    delete attr;
    return true;
}

/// End all styles
bool wxRichTextBuffer::EndAllStyles()
{
    while (m_attributeStack.GetCount() != 0)
        EndStyle();
    return true;
}

/// Clear the style stack
void wxRichTextBuffer::ClearStyleStack()
{
    for (wxList::compatibility_iterator node = m_attributeStack.GetFirst(); node; node = node->GetNext())
        delete (wxRichTextAttr*) node->GetData();
    m_attributeStack.Clear();
}

/// Begin using bold
bool wxRichTextBuffer::BeginBold()
{
    wxRichTextAttr attr;
    attr.SetFontWeight(wxFONTWEIGHT_BOLD);

    return BeginStyle(attr);
}

/// Begin using italic
bool wxRichTextBuffer::BeginItalic()
{
    wxRichTextAttr attr;
    attr.SetFontStyle(wxFONTSTYLE_ITALIC);

    return BeginStyle(attr);
}

/// Begin using underline
bool wxRichTextBuffer::BeginUnderline()
{
    wxRichTextAttr attr;
    attr.SetFontUnderlined(true);

    return BeginStyle(attr);
}

/// Begin using point size
bool wxRichTextBuffer::BeginFontSize(int pointSize)
{
    wxRichTextAttr attr;
    attr.SetFontSize(pointSize);

    return BeginStyle(attr);
}

/// Begin using this font
bool wxRichTextBuffer::BeginFont(const wxFont& font)
{
    wxRichTextAttr attr;
    attr.SetFont(font);

    return BeginStyle(attr);
}

/// Begin using this colour
bool wxRichTextBuffer::BeginTextColour(const wxColour& colour)
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_TEXT_COLOUR);
    attr.SetTextColour(colour);

    return BeginStyle(attr);
}

/// Begin using alignment
bool wxRichTextBuffer::BeginAlignment(wxTextAttrAlignment alignment)
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_ALIGNMENT);
    attr.SetAlignment(alignment);

    return BeginStyle(attr);
}

/// Begin left indent
bool wxRichTextBuffer::BeginLeftIndent(int leftIndent, int leftSubIndent)
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_LEFT_INDENT);
    attr.SetLeftIndent(leftIndent, leftSubIndent);

    return BeginStyle(attr);
}

/// Begin right indent
bool wxRichTextBuffer::BeginRightIndent(int rightIndent)
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_RIGHT_INDENT);
    attr.SetRightIndent(rightIndent);

    return BeginStyle(attr);
}

/// Begin paragraph spacing
bool wxRichTextBuffer::BeginParagraphSpacing(int before, int after)
{
    long flags = 0;
    if (before != 0)
        flags |= wxTEXT_ATTR_PARA_SPACING_BEFORE;
    if (after != 0)
        flags |= wxTEXT_ATTR_PARA_SPACING_AFTER;

    wxRichTextAttr attr;
    attr.SetFlags(flags);
    attr.SetParagraphSpacingBefore(before);
    attr.SetParagraphSpacingAfter(after);

    return BeginStyle(attr);
}

/// Begin line spacing
bool wxRichTextBuffer::BeginLineSpacing(int lineSpacing)
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);
    attr.SetLineSpacing(lineSpacing);

    return BeginStyle(attr);
}

/// Begin numbered bullet
bool wxRichTextBuffer::BeginNumberedBullet(int bulletNumber, int leftIndent, int leftSubIndent, int bulletStyle)
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_LEFT_INDENT);
    attr.SetBulletStyle(bulletStyle);
    attr.SetBulletNumber(bulletNumber);
    attr.SetLeftIndent(leftIndent, leftSubIndent);

    return BeginStyle(attr);
}

/// Begin symbol bullet
bool wxRichTextBuffer::BeginSymbolBullet(const wxString& symbol, int leftIndent, int leftSubIndent, int bulletStyle)
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_LEFT_INDENT);
    attr.SetBulletStyle(bulletStyle);
    attr.SetLeftIndent(leftIndent, leftSubIndent);
    attr.SetBulletText(symbol);

    return BeginStyle(attr);
}

/// Begin standard bullet
bool wxRichTextBuffer::BeginStandardBullet(const wxString& bulletName, int leftIndent, int leftSubIndent, int bulletStyle)
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_LEFT_INDENT);
    attr.SetBulletStyle(bulletStyle);
    attr.SetLeftIndent(leftIndent, leftSubIndent);
    attr.SetBulletName(bulletName);

    return BeginStyle(attr);
}

/// Begin named character style
bool wxRichTextBuffer::BeginCharacterStyle(const wxString& characterStyle)
{
    if (GetStyleSheet())
    {
        wxRichTextCharacterStyleDefinition* def = GetStyleSheet()->FindCharacterStyle(characterStyle);
        if (def)
        {
            wxRichTextAttr attr = def->GetStyleMergedWithBase(GetStyleSheet());
            return BeginStyle(attr);
        }
    }
    return false;
}

/// Begin named paragraph style
bool wxRichTextBuffer::BeginParagraphStyle(const wxString& paragraphStyle)
{
    if (GetStyleSheet())
    {
        wxRichTextParagraphStyleDefinition* def = GetStyleSheet()->FindParagraphStyle(paragraphStyle);
        if (def)
        {
            wxRichTextAttr attr = def->GetStyleMergedWithBase(GetStyleSheet());
            return BeginStyle(attr);
        }
    }
    return false;
}

/// Begin named list style
bool wxRichTextBuffer::BeginListStyle(const wxString& listStyle, int level, int number)
{
    if (GetStyleSheet())
    {
        wxRichTextListStyleDefinition* def = GetStyleSheet()->FindListStyle(listStyle);
        if (def)
        {
            wxRichTextAttr attr(def->GetCombinedStyleForLevel(level));

            attr.SetBulletNumber(number);

            return BeginStyle(attr);
        }
    }
    return false;
}

/// Begin URL
bool wxRichTextBuffer::BeginURL(const wxString& url, const wxString& characterStyle)
{
    wxRichTextAttr attr;

    if (!characterStyle.IsEmpty() && GetStyleSheet())
    {
        wxRichTextCharacterStyleDefinition* def = GetStyleSheet()->FindCharacterStyle(characterStyle);
        if (def)
        {
            attr = def->GetStyleMergedWithBase(GetStyleSheet());
        }
    }
    attr.SetURL(url);

    return BeginStyle(attr);
}

/// Adds a handler to the end
void wxRichTextBuffer::AddHandler(wxRichTextFileHandler *handler)
{
    sm_handlers.Append(handler);
}

/// Inserts a handler at the front
void wxRichTextBuffer::InsertHandler(wxRichTextFileHandler *handler)
{
    sm_handlers.Insert( handler );
}

/// Removes a handler
bool wxRichTextBuffer::RemoveHandler(const wxString& name)
{
    wxRichTextFileHandler *handler = FindHandler(name);
    if (handler)
    {
        sm_handlers.DeleteObject(handler);
        delete handler;
        return true;
    }
    else
        return false;
}

/// Finds a handler by filename or, if supplied, type
wxRichTextFileHandler *wxRichTextBuffer::FindHandlerFilenameOrType(const wxString& filename,
                                                                   wxRichTextFileType imageType)
{
    if (imageType != wxRICHTEXT_TYPE_ANY)
        return FindHandler(imageType);
    else if (!filename.IsEmpty())
    {
        wxString path, file, ext;
        wxFileName::SplitPath(filename, & path, & file, & ext);
        return FindHandler(ext, imageType);
    }
    else
        return NULL;
}


/// Finds a handler by name
wxRichTextFileHandler* wxRichTextBuffer::FindHandler(const wxString& name)
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxRichTextFileHandler *handler = (wxRichTextFileHandler*)node->GetData();
        if (handler->GetName().Lower() == name.Lower()) return handler;

        node = node->GetNext();
    }
    return NULL;
}

/// Finds a handler by extension and type
wxRichTextFileHandler* wxRichTextBuffer::FindHandler(const wxString& extension, wxRichTextFileType type)
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxRichTextFileHandler *handler = (wxRichTextFileHandler*)node->GetData();
        if ( handler->GetExtension().Lower() == extension.Lower() &&
            (type == wxRICHTEXT_TYPE_ANY || handler->GetType() == type) )
            return handler;
        node = node->GetNext();
    }
    return 0;
}

/// Finds a handler by type
wxRichTextFileHandler* wxRichTextBuffer::FindHandler(wxRichTextFileType type)
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxRichTextFileHandler *handler = (wxRichTextFileHandler *)node->GetData();
        if (handler->GetType() == type) return handler;
        node = node->GetNext();
    }
    return NULL;
}

void wxRichTextBuffer::InitStandardHandlers()
{
    if (!FindHandler(wxRICHTEXT_TYPE_TEXT))
        AddHandler(new wxRichTextPlainTextHandler);
}

void wxRichTextBuffer::CleanUpHandlers()
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxRichTextFileHandler* handler = (wxRichTextFileHandler*)node->GetData();
        wxList::compatibility_iterator next = node->GetNext();
        delete handler;
        node = next;
    }

    sm_handlers.Clear();
}

wxString wxRichTextBuffer::GetExtWildcard(bool combine, bool save, wxArrayInt* types)
{
    if (types)
        types->Clear();

    wxString wildcard;

    wxList::compatibility_iterator node = GetHandlers().GetFirst();
    int count = 0;
    while (node)
    {
        wxRichTextFileHandler* handler = (wxRichTextFileHandler*) node->GetData();
        if (handler->IsVisible() && ((save && handler->CanSave()) || (!save && handler->CanLoad())))
        {
            if (combine)
            {
                if (count > 0)
                    wildcard += wxT(";");
                wildcard += wxT("*.") + handler->GetExtension();
            }
            else
            {
                if (count > 0)
                    wildcard += wxT("|");
                wildcard += handler->GetName();
                wildcard += wxT(" ");
                wildcard += _("files");
                wildcard += wxT(" (*.");
                wildcard += handler->GetExtension();
                wildcard += wxT(")|*.");
                wildcard += handler->GetExtension();
                if (types)
                    types->Add(handler->GetType());
            }
            count ++;
        }

        node = node->GetNext();
    }

    if (combine)
        wildcard = wxT("(") + wildcard + wxT(")|") + wildcard;
    return wildcard;
}

/// Load a file
bool wxRichTextBuffer::LoadFile(const wxString& filename, wxRichTextFileType type)
{
    wxRichTextFileHandler* handler = FindHandlerFilenameOrType(filename, type);
    if (handler)
    {
        SetDefaultStyle(wxRichTextAttr());
        handler->SetFlags(GetHandlerFlags());
        bool success = handler->LoadFile(this, filename);
        Invalidate(wxRICHTEXT_ALL);
        return success;
    }
    else
        return false;
}

/// Save a file
bool wxRichTextBuffer::SaveFile(const wxString& filename, wxRichTextFileType type)
{
    wxRichTextFileHandler* handler = FindHandlerFilenameOrType(filename, type);
    if (handler)
    {
        handler->SetFlags(GetHandlerFlags());
        return handler->SaveFile(this, filename);
    }
    else
        return false;
}

/// Load from a stream
bool wxRichTextBuffer::LoadFile(wxInputStream& stream, wxRichTextFileType type)
{
    wxRichTextFileHandler* handler = FindHandler(type);
    if (handler)
    {
        SetDefaultStyle(wxRichTextAttr());
        handler->SetFlags(GetHandlerFlags());
        bool success = handler->LoadFile(this, stream);
        Invalidate(wxRICHTEXT_ALL);
        return success;
    }
    else
        return false;
}

/// Save to a stream
bool wxRichTextBuffer::SaveFile(wxOutputStream& stream, wxRichTextFileType type)
{
    wxRichTextFileHandler* handler = FindHandler(type);
    if (handler)
    {
        handler->SetFlags(GetHandlerFlags());
        return handler->SaveFile(this, stream);
    }
    else
        return false;
}

/// Copy the range to the clipboard
bool wxRichTextBuffer::CopyToClipboard(const wxRichTextRange& range)
{
    bool success = false;
    wxRichTextParagraphLayoutBox* container = this;
    if (GetRichTextCtrl())
        container = GetRichTextCtrl()->GetFocusObject();

#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ

    if (!wxTheClipboard->IsOpened() && wxTheClipboard->Open())
    {
        wxTheClipboard->Clear();

        // Add composite object

        wxDataObjectComposite* compositeObject = new wxDataObjectComposite();

        {
            wxString text = container->GetTextForRange(range);

#ifdef __WXMSW__
            text = wxTextFile::Translate(text, wxTextFileType_Dos);
#endif

            compositeObject->Add(new wxTextDataObject(text), false /* not preferred */);
        }

        // Add rich text buffer data object. This needs the XML handler to be present.

        if (FindHandler(wxRICHTEXT_TYPE_XML))
        {
            wxRichTextBuffer* richTextBuf = new wxRichTextBuffer;
            container->CopyFragment(range, *richTextBuf);

            compositeObject->Add(new wxRichTextBufferDataObject(richTextBuf), true /* preferred */);
        }

        if (wxTheClipboard->SetData(compositeObject))
            success = true;

        wxTheClipboard->Close();
    }

#else
    wxUnusedVar(range);
#endif
    return success;
}

/// Paste the clipboard content to the buffer
bool wxRichTextBuffer::PasteFromClipboard(long position)
{
    bool success = false;
    wxRichTextParagraphLayoutBox* container = this;
    if (GetRichTextCtrl())
        container = GetRichTextCtrl()->GetFocusObject();

#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ
    if (CanPasteFromClipboard())
    {
        if (wxTheClipboard->Open())
        {
            if (wxTheClipboard->IsSupported(wxDataFormat(wxRichTextBufferDataObject::GetRichTextBufferFormatId())))
            {
                wxRichTextBufferDataObject data;
                wxTheClipboard->GetData(data);
                wxRichTextBuffer* richTextBuffer = data.GetRichTextBuffer();
                if (richTextBuffer)
                {
                    container->InsertParagraphsWithUndo(position+1, *richTextBuffer, GetRichTextCtrl(), this, 0);
                    if (GetRichTextCtrl())
                        GetRichTextCtrl()->ShowPosition(position + richTextBuffer->GetOwnRange().GetEnd());
                    delete richTextBuffer;
                }
            }
            else if (wxTheClipboard->IsSupported(wxDF_TEXT)
 #if wxUSE_UNICODE
                     || wxTheClipboard->IsSupported(wxDF_UNICODETEXT)
 #endif
                    )
            {
                wxTextDataObject data;
                wxTheClipboard->GetData(data);
                wxString text(data.GetText());
#ifdef __WXMSW__
                wxString text2;
                text2.Alloc(text.Length()+1);
                size_t i;
                for (i = 0; i < text.Length(); i++)
                {
                    wxChar ch = text[i];
                    if (ch != wxT('\r'))
                        text2 += ch;
                }
#else
                wxString text2 = text;
#endif
                container->InsertTextWithUndo(position+1, text2, GetRichTextCtrl(), this, wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE);

                if (GetRichTextCtrl())
                    GetRichTextCtrl()->ShowPosition(position + text2.Length());

                success = true;
            }
            else if (wxTheClipboard->IsSupported(wxDF_BITMAP))
            {
                wxBitmapDataObject data;
                wxTheClipboard->GetData(data);
                wxBitmap bitmap(data.GetBitmap());
                wxImage image(bitmap.ConvertToImage());

                wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Image"), wxRICHTEXT_INSERT, this, container, GetRichTextCtrl(), false);

                action->GetNewParagraphs().AddImage(image);

                if (action->GetNewParagraphs().GetChildCount() == 1)
                    action->GetNewParagraphs().SetPartialParagraph(true);

                action->SetPosition(position+1);

                // Set the range we'll need to delete in Undo
                action->SetRange(wxRichTextRange(position+1, position+1));

                SubmitAction(action);

                success = true;
            }
            wxTheClipboard->Close();
        }
    }
#else
    wxUnusedVar(position);
#endif
    return success;
}

/// Can we paste from the clipboard?
bool wxRichTextBuffer::CanPasteFromClipboard() const
{
    bool canPaste = false;
#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ
    if (!wxTheClipboard->IsOpened() && wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported(wxDF_TEXT)
#if wxUSE_UNICODE
            || wxTheClipboard->IsSupported(wxDF_UNICODETEXT)
#endif
            || wxTheClipboard->IsSupported(wxDataFormat(wxRichTextBufferDataObject::GetRichTextBufferFormatId())) ||
            wxTheClipboard->IsSupported(wxDF_BITMAP))
        {
            canPaste = true;
        }
        wxTheClipboard->Close();
    }
#endif
    return canPaste;
}

/// Dumps contents of buffer for debugging purposes
void wxRichTextBuffer::Dump()
{
    wxString text;
    {
        wxStringOutputStream stream(& text);
        wxTextOutputStream textStream(stream);
        Dump(textStream);
    }

    wxLogDebug(text);
}

/// Add an event handler
bool wxRichTextBuffer::AddEventHandler(wxEvtHandler* handler)
{
    m_eventHandlers.Append(handler);
    return true;
}

/// Remove an event handler
bool wxRichTextBuffer::RemoveEventHandler(wxEvtHandler* handler, bool deleteHandler)
{
    wxList::compatibility_iterator node = m_eventHandlers.Find(handler);
    if (node)
    {
        m_eventHandlers.Erase(node);
        if (deleteHandler)
            delete handler;

        return true;
    }
    else
        return false;
}

/// Clear event handlers
void wxRichTextBuffer::ClearEventHandlers()
{
    m_eventHandlers.Clear();
}

/// Send event to event handlers. If sendToAll is true, will send to all event handlers,
/// otherwise will stop at the first successful one.
bool wxRichTextBuffer::SendEvent(wxEvent& event, bool sendToAll)
{
    bool success = false;
    for (wxList::compatibility_iterator node = m_eventHandlers.GetFirst(); node; node = node->GetNext())
    {
        wxEvtHandler* handler = (wxEvtHandler*) node->GetData();
        if (handler->ProcessEvent(event))
        {
            success = true;
            if (!sendToAll)
                return true;
        }
    }
    return success;
}

/// Set style sheet and notify of the change
bool wxRichTextBuffer::SetStyleSheetAndNotify(wxRichTextStyleSheet* sheet)
{
    wxRichTextStyleSheet* oldSheet = GetStyleSheet();

    wxWindowID winid = wxID_ANY;
    if (GetRichTextCtrl())
        winid = GetRichTextCtrl()->GetId();

    wxRichTextEvent event(wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING, winid);
    event.SetEventObject(GetRichTextCtrl());
    event.SetContainer(GetRichTextCtrl()->GetFocusObject());
    event.SetOldStyleSheet(oldSheet);
    event.SetNewStyleSheet(sheet);
    event.Allow();

    if (SendEvent(event) && !event.IsAllowed())
    {
        if (sheet != oldSheet)
            delete sheet;

        return false;
    }

    if (oldSheet && oldSheet != sheet)
        delete oldSheet;

    SetStyleSheet(sheet);

    event.SetEventType(wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED);
    event.SetOldStyleSheet(NULL);
    event.Allow();

    return SendEvent(event);
}

/// Set renderer, deleting old one
void wxRichTextBuffer::SetRenderer(wxRichTextRenderer* renderer)
{
    if (sm_renderer)
        delete sm_renderer;
    sm_renderer = renderer;
}

/// Hit-testing: returns a flag indicating hit test details, plus
/// information about position
int wxRichTextBuffer::HitTest(wxDC& dc, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags)
{
    int ret = wxRichTextParagraphLayoutBox::HitTest(dc, pt, textPosition, obj, contextObj, flags);
    if (ret != wxRICHTEXT_HITTEST_NONE)
    {
        return ret;
    }
    else
    {
        textPosition = m_ownRange.GetEnd()-1;
        *obj = this;
        *contextObj = this;
        return wxRICHTEXT_HITTEST_AFTER|wxRICHTEXT_HITTEST_OUTSIDE;
    }
}

bool wxRichTextStdRenderer::DrawStandardBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxRichTextAttr& bulletAttr, const wxRect& rect)
{
    if (bulletAttr.GetTextColour().Ok())
    {
        wxCheckSetPen(dc, wxPen(bulletAttr.GetTextColour()));
        wxCheckSetBrush(dc, wxBrush(bulletAttr.GetTextColour()));
    }
    else
    {
        wxCheckSetPen(dc, *wxBLACK_PEN);
        wxCheckSetBrush(dc, *wxBLACK_BRUSH);
    }

    wxFont font;
    if (bulletAttr.HasFont())
    {
        font = paragraph->GetBuffer()->GetFontTable().FindFont(bulletAttr);
    }
    else
        font = (*wxNORMAL_FONT);

    wxCheckSetFont(dc, font);

    int charHeight = dc.GetCharHeight();

    int bulletWidth = (int) (((float) charHeight) * wxRichTextBuffer::GetBulletProportion());
    int bulletHeight = bulletWidth;

    int x = rect.x;

    // Calculate the top position of the character (as opposed to the whole line height)
    int y = rect.y + (rect.height - charHeight);

    // Calculate where the bullet should be positioned
    y = y + (charHeight+1)/2 - (bulletHeight+1)/2;

    // The margin between a bullet and text.
    int margin = paragraph->ConvertTenthsMMToPixels(dc, wxRichTextBuffer::GetBulletRightMargin());

    if (bulletAttr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT)
        x = rect.x + rect.width - bulletWidth - margin;
    else if (bulletAttr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE)
        x = x + (rect.width)/2 - bulletWidth/2;

    if (bulletAttr.GetBulletName() == wxT("standard/square"))
    {
        dc.DrawRectangle(x, y, bulletWidth, bulletHeight);
    }
    else if (bulletAttr.GetBulletName() == wxT("standard/diamond"))
    {
        wxPoint pts[5];
        pts[0].x = x;                   pts[0].y = y + bulletHeight/2;
        pts[1].x = x + bulletWidth/2;   pts[1].y = y;
        pts[2].x = x + bulletWidth;     pts[2].y = y + bulletHeight/2;
        pts[3].x = x + bulletWidth/2;   pts[3].y = y + bulletHeight;

        dc.DrawPolygon(4, pts);
    }
    else if (bulletAttr.GetBulletName() == wxT("standard/triangle"))
    {
        wxPoint pts[3];
        pts[0].x = x;                   pts[0].y = y;
        pts[1].x = x + bulletWidth;     pts[1].y = y + bulletHeight/2;
        pts[2].x = x;                   pts[2].y = y + bulletHeight;

        dc.DrawPolygon(3, pts);
    }
    else if (bulletAttr.GetBulletName() == wxT("standard/circle-outline"))
    {
        wxCheckSetBrush(dc, *wxTRANSPARENT_BRUSH);
        dc.DrawEllipse(x, y, bulletWidth, bulletHeight);
    }
    else // "standard/circle", and catch-all
    {
        dc.DrawEllipse(x, y, bulletWidth, bulletHeight);
    }

    return true;
}

bool wxRichTextStdRenderer::DrawTextBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxRichTextAttr& attr, const wxRect& rect, const wxString& text)
{
    if (!text.empty())
    {
        wxFont font;
        if ((attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_SYMBOL) && !attr.GetBulletFont().IsEmpty() && attr.HasFont())
        {
            wxRichTextAttr fontAttr;
            fontAttr.SetFontSize(attr.GetFontSize());
            fontAttr.SetFontStyle(attr.GetFontStyle());
            fontAttr.SetFontWeight(attr.GetFontWeight());
            fontAttr.SetFontUnderlined(attr.GetFontUnderlined());
            fontAttr.SetFontFaceName(attr.GetBulletFont());
            font = paragraph->GetBuffer()->GetFontTable().FindFont(fontAttr);
        }
        else if (attr.HasFont())
            font = paragraph->GetBuffer()->GetFontTable().FindFont(attr);
        else
            font = (*wxNORMAL_FONT);

        wxCheckSetFont(dc, font);

        if (attr.GetTextColour().Ok())
            dc.SetTextForeground(attr.GetTextColour());

        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

        int charHeight = dc.GetCharHeight();
        wxCoord tw, th;
        dc.GetTextExtent(text, & tw, & th);

        int x = rect.x;

        // Calculate the top position of the character (as opposed to the whole line height)
        int y = rect.y + (rect.height - charHeight);

        // The margin between a bullet and text.
        int margin = paragraph->ConvertTenthsMMToPixels(dc, wxRichTextBuffer::GetBulletRightMargin());

        if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT)
            x = (rect.x + rect.width) - tw - margin;
        else if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE)
            x = x + (rect.width)/2 - tw/2;

        dc.DrawText(text, x, y);

        return true;
    }
    else
        return false;
}

bool wxRichTextStdRenderer::DrawBitmapBullet(wxRichTextParagraph* WXUNUSED(paragraph), wxDC& WXUNUSED(dc), const wxRichTextAttr& WXUNUSED(attr), const wxRect& WXUNUSED(rect))
{
    // Currently unimplemented. The intention is to store bitmaps by name in a media store associated
    // with the buffer. The store will allow retrieval from memory, disk or other means.
    return false;
}

/// Enumerate the standard bullet names currently supported
bool wxRichTextStdRenderer::EnumerateStandardBulletNames(wxArrayString& bulletNames)
{
    bulletNames.Add(wxTRANSLATE("standard/circle"));
    bulletNames.Add(wxTRANSLATE("standard/circle-outline"));
    bulletNames.Add(wxTRANSLATE("standard/square"));
    bulletNames.Add(wxTRANSLATE("standard/diamond"));
    bulletNames.Add(wxTRANSLATE("standard/triangle"));

    return true;
}

/*!
 * wxRichTextBox
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextBox, wxRichTextParagraphLayoutBox)

wxRichTextBox::wxRichTextBox(wxRichTextObject* parent):
    wxRichTextParagraphLayoutBox(parent)
{
}

/// Draw the item
bool wxRichTextBox::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    if (!IsShown())
        return true;

    // TODO: if the active object in the control, draw an indication.
    // We need to add the concept of active object, and not just focus object,
    // so we can apply commands (properties, delete, ...) to objects such as text boxes and images.
    // Ultimately we would like to be able to interactively resize an active object
    // using drag handles.
    return wxRichTextParagraphLayoutBox::Draw(dc, range, selection, rect, descent, style);
}

/// Copy
void wxRichTextBox::Copy(const wxRichTextBox& obj)
{
    wxRichTextParagraphLayoutBox::Copy(obj);
}

// Edit properties via a GUI
bool wxRichTextBox::EditProperties(wxWindow* parent, wxRichTextBuffer* buffer)
{
    wxRichTextObjectPropertiesDialog boxDlg(this, wxGetTopLevelParent(parent), wxID_ANY, _("Box Properties"));
    boxDlg.SetAttributes(GetAttributes());

    if (boxDlg.ShowModal() == wxID_OK)
    {
        // By passing wxRICHTEXT_SETSTYLE_RESET, indeterminate attributes set by the user will be set as
        // indeterminate in the object.
        boxDlg.ApplyStyle(buffer->GetRichTextCtrl(), wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_RESET);
        return true;
    }
    else
        return false;
}

IMPLEMENT_DYNAMIC_CLASS(wxRichTextCell, wxRichTextBox)

wxRichTextCell::wxRichTextCell(wxRichTextObject* parent):
    wxRichTextBox(parent)
{
}

/// Draw the item
bool wxRichTextCell::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    return wxRichTextBox::Draw(dc, range, selection, rect, descent, style);
}

/// Copy
void wxRichTextCell::Copy(const wxRichTextCell& obj)
{
    wxRichTextBox::Copy(obj);
}

// Edit properties via a GUI
bool wxRichTextCell::EditProperties(wxWindow* parent, wxRichTextBuffer* buffer)
{
    // We need to gather common attributes for all selected cells.

    wxRichTextTable* table = wxDynamicCast(GetParent(), wxRichTextTable);
    bool multipleCells = false;
    wxRichTextAttr attr;

    if (table && buffer && buffer->GetRichTextCtrl() && buffer->GetRichTextCtrl()->GetSelection().IsValid() &&
        buffer->GetRichTextCtrl()->GetSelection().GetContainer() == GetParent())
    {
        wxRichTextAttr clashingAttr, absentAttr;
        const wxRichTextSelection& sel = buffer->GetRichTextCtrl()->GetSelection();
        size_t i;
        int selectedCellCount = 0;
        for (i = 0; i < sel.GetCount(); i++)
        {
            const wxRichTextRange& range = sel[i];
            wxRichTextCell* cell = table->GetCell(range.GetStart());
            if (cell)
            {
                wxRichTextAttr cellStyle = cell->GetAttributes();

                CollectStyle(attr, cellStyle, clashingAttr, absentAttr);

                selectedCellCount ++;
            }
        }
        multipleCells = selectedCellCount > 1;
    }
    else
    {
        attr = GetAttributes();
    }

    wxString caption;
    if (multipleCells)
        caption = _("Multiple Cell Properties");
    else
        caption = _("Cell Properties");

    wxRichTextObjectPropertiesDialog cellDlg(this, wxGetTopLevelParent(parent), wxID_ANY, caption);
    cellDlg.SetAttributes(attr);

    wxRichTextSizePage* sizePage = wxDynamicCast(cellDlg.FindPage(CLASSINFO(wxRichTextSizePage)), wxRichTextSizePage);
    if (sizePage)
    {
        // We don't want position and floating controls for a cell.
        sizePage->ShowPositionControls(false);
        sizePage->ShowFloatingControls(false);
    }

    if (cellDlg.ShowModal() == wxID_OK)
    {
        if (multipleCells)
        {
            const wxRichTextSelection& sel = buffer->GetRichTextCtrl()->GetSelection();
            // Apply the style; we interpret indeterminate attributes as 'don't touch this attribute'
            // since it may represent clashing attributes across multiple objects.
            table->SetCellStyle(sel, attr);
        }
        else
            // For a single object, indeterminate attributes set by the user should be reflected in the
            // actual object style, so pass the wxRICHTEXT_SETSTYLE_RESET flag to assign
            // the style directly instead of applying (which ignores indeterminate attributes,
            // leaving them as they were).
            cellDlg.ApplyStyle(buffer->GetRichTextCtrl(), wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_RESET);
        return true;
    }
    else
        return false;
}

WX_DEFINE_OBJARRAY(wxRichTextObjectPtrArrayArray)

IMPLEMENT_DYNAMIC_CLASS(wxRichTextTable, wxRichTextBox)

wxRichTextTable::wxRichTextTable(wxRichTextObject* parent): wxRichTextBox(parent)
{
    m_rowCount = 0;
    m_colCount = 0;
}

// Draws the object.
bool wxRichTextTable::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    return wxRichTextBox::Draw(dc, range, selection, rect, descent, style);
}

WX_DECLARE_OBJARRAY(wxRect, wxRichTextRectArray);
WX_DEFINE_OBJARRAY(wxRichTextRectArray);

// Lays the object out. rect is the space available for layout. Often it will
// be the specified overall space for this object, if trying to constrain
// layout to a particular size, or it could be the total space available in the
// parent. rect is the overall size, so we must subtract margins and padding.
// to get the actual available space.
bool wxRichTextTable::Layout(wxDC& dc, const wxRect& rect, int style)
{
    SetPosition(rect.GetPosition());

    // TODO: the meaty bit. Calculate sizes of all cells and rows. Try to use
    // minimum size if within alloted size, then divide up remaining size
    // between rows/cols.

    double scale = 1.0;
    wxRichTextBuffer* buffer = GetBuffer();
    if (buffer) scale = buffer->GetScale();

    wxRect availableSpace = GetAvailableContentArea(dc, rect);
    wxTextAttrDimensionConverter converter(dc, scale, availableSpace.GetSize());

    // If we have no fixed table size, and assuming we're not pushed for
    // space, then we don't have to try to stretch the table to fit the contents.
    bool stretchToFitTableWidth = false;

    int tableWidth = rect.width;
    if (GetAttributes().GetTextBoxAttr().GetWidth().IsValid())
    {
        tableWidth = converter.GetPixels(GetAttributes().GetTextBoxAttr().GetWidth());

        // Fixed table width, so we do want to stretch columns out if necessary.
        stretchToFitTableWidth = true;

        // Shouldn't be able to exceed the size passed to this function
        tableWidth = wxMin(rect.width, tableWidth);
    }

    // Get internal padding
    int paddingLeft = 0, paddingRight = 0, paddingTop = 0, paddingBottom = 0;
    if (GetAttributes().GetTextBoxAttr().GetPadding().GetLeft().IsValid())
        paddingLeft = converter.GetPixels(GetAttributes().GetTextBoxAttr().GetPadding().GetLeft());
    if (GetAttributes().GetTextBoxAttr().GetPadding().GetRight().IsValid())
        paddingRight = converter.GetPixels(GetAttributes().GetTextBoxAttr().GetPadding().GetRight());
    if (GetAttributes().GetTextBoxAttr().GetPadding().GetTop().IsValid())
        paddingTop = converter.GetPixels(GetAttributes().GetTextBoxAttr().GetPadding().GetTop());
    if (GetAttributes().GetTextBoxAttr().GetPadding().GetLeft().IsValid())
        paddingBottom = converter.GetPixels(GetAttributes().GetTextBoxAttr().GetPadding().GetBottom());

    // Assume that left and top padding are also used for inter-cell padding.
    int paddingX = paddingLeft;
    int paddingY = paddingTop;

    int totalLeftMargin = 0, totalRightMargin = 0, totalTopMargin = 0, totalBottomMargin = 0;
    GetTotalMargin(dc, buffer, GetAttributes(), totalLeftMargin, totalRightMargin, totalTopMargin, totalBottomMargin);

    // Internal table width - the area for content
    int internalTableWidth = tableWidth - totalLeftMargin - totalRightMargin;

    int rowCount = m_cells.GetCount();
    if (m_colCount == 0 || rowCount == 0)
    {
        wxRect overallRect(rect.x, rect.y, totalLeftMargin + totalRightMargin, totalTopMargin + totalBottomMargin);
        SetCachedSize(overallRect.GetSize());

        // Zero content size
        SetMinSize(overallRect.GetSize());
        SetMaxSize(GetMinSize());
        return true;
    }

    // The final calculated widths
    wxArrayInt colWidths(m_colCount);

    wxArrayInt absoluteColWidths(m_colCount);
    // wxArrayInt absoluteColWidthsSpanning(m_colCount);
    wxArrayInt percentageColWidths(m_colCount);
    // wxArrayInt percentageColWidthsSpanning(m_colCount);
    // These are only relevant when the first column contains spanning information.
    // wxArrayInt columnSpans(m_colCount); // Each contains 1 for non-spanning cell, > 1 for spanning cell.
    wxArrayInt maxColWidths(m_colCount);
    wxArrayInt minColWidths(m_colCount);

    wxSize tableSize(tableWidth, 0);

    int i, j, k;

    for (i = 0; i < m_colCount; i++)
    {
        absoluteColWidths[i] = 0;
        // absoluteColWidthsSpanning[i] = 0;
        percentageColWidths[i] = -1;
        // percentageColWidthsSpanning[i] = -1;
        colWidths[i] = 0;
        maxColWidths[i] = 0;
        minColWidths[i] = 0;
        // columnSpans[i] = 1;
    }

    // (0) Determine which cells are visible according to spans
    //   1  2   3  4   5
    //  __________________
    // |  |   |      |    | 1
    // |------|      |----|
    // |------|      |    | 2
    // |------|      |    | 3
    // |------------------|
    // |__________________| 4

    // To calculate cell visibility:
    // First find all spanning cells. Build an array of span records with start x, y and end x, y.
    // Then for each cell, test whether we're within one of those cells, and unless we're at the start of
    // that cell, hide the cell.

    // We can also use this array to match the size of spanning cells to the grid. Or just do
    // this when we iterate through all cells.

    // 0.1: add spanning cells to an array
    wxRichTextRectArray rectArray;
    for (j = 0; j < m_rowCount; j++)
    {
        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextBox* cell = GetCell(j, i);
            int colSpan = 1, rowSpan = 1;
            if (cell->GetProperties().HasProperty(wxT("colspan")))
                colSpan = cell->GetProperties().GetPropertyLong(wxT("colspan"));
            if (cell->GetProperties().HasProperty(wxT("rowspan")))
                rowSpan = cell->GetProperties().GetPropertyLong(wxT("rowspan"));
            if (colSpan > 1 || rowSpan > 1)
            {
                rectArray.Add(wxRect(i, j, colSpan, rowSpan));
            }
        }
    }
    // 0.2: find which cells are subsumed by a spanning cell
    for (j = 0; j < m_rowCount; j++)
    {
        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextBox* cell = GetCell(j, i);
            if (rectArray.GetCount() == 0)
            {
                cell->Show(true);
            }
            else
            {
                int colSpan = 1, rowSpan = 1;
                if (cell->GetProperties().HasProperty(wxT("colspan")))
                    colSpan = cell->GetProperties().GetPropertyLong(wxT("colspan"));
                if (cell->GetProperties().HasProperty(wxT("rowspan")))
                    rowSpan = cell->GetProperties().GetPropertyLong(wxT("rowspan"));
                if (colSpan > 1 || rowSpan > 1)
                {
                    // Assume all spanning cells are shown
                    cell->Show(true);
                }
                else
                {
                    bool shown = true;
                    for (k = 0; k < (int) rectArray.GetCount(); k++)
                    {
                        if (rectArray[k].Contains(wxPoint(i, j)))
                        {
                            shown = false;
                            break;
                        }
                    }
                    cell->Show(shown);
                }
            }
        }
    }

    // TODO: find the first spanned cell in each row that spans the most columns and doesn't
    // overlap with a spanned cell starting at a previous column position.
    // This means we need to keep an array of rects so we can check. However
    // it does also mean that some spans simply may not be taken into account
    // where there are different spans happening on different rows. In these cases,
    // they will simply be as wide as their constituent columns.

    // (1) Do an initial layout for all cells to get minimum and maximum size, and get
    // the absolute or percentage width of each column.

    for (j = 0; j < m_rowCount; j++)
    {
        // First get the overall margins so we can calculate percentage widths based on
        // the available content space for all cells on the row

        int overallRowContentMargin = 0;
        int visibleCellCount = 0;

        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextBox* cell = GetCell(j, i);
            if (cell->IsShown())
            {
                int cellTotalLeftMargin = 0, cellTotalRightMargin = 0, cellTotalTopMargin = 0, cellTotalBottomMargin = 0;
                GetTotalMargin(dc, buffer, cell->GetAttributes(), cellTotalLeftMargin, cellTotalRightMargin, cellTotalTopMargin, cellTotalBottomMargin);

                overallRowContentMargin += (cellTotalLeftMargin + cellTotalRightMargin);
                visibleCellCount ++;
            }
        }

        // Add in inter-cell padding
        overallRowContentMargin += ((visibleCellCount-1) * paddingX);

        int rowContentWidth = internalTableWidth - overallRowContentMargin;
        wxSize rowTableSize(rowContentWidth, 0);
        wxTextAttrDimensionConverter converter(dc, scale, rowTableSize);

        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextBox* cell = GetCell(j, i);
            if (cell->IsShown())
            {
                int colSpan = 1;
                if (cell->GetProperties().HasProperty(wxT("colspan")))
                    colSpan = cell->GetProperties().GetPropertyLong(wxT("colspan"));

                // Lay out cell to find min/max widths
                cell->Invalidate(wxRICHTEXT_ALL);
                cell->Layout(dc, availableSpace, style);

                if (colSpan == 1)
                {
                    int absoluteCellWidth = -1;
                    int percentageCellWidth = -1;

                    // I think we need to calculate percentages from the internal table size,
                    // minus the padding between cells which we'll need to calculate from the
                    // (number of VISIBLE cells - 1)*paddingX. Then percentages that add up to 100%
                    // will add up to 100%. In CSS, the width specifies the cell's content rect width,
                    // so if we want to conform to that we'll need to add in the overall cell margins.
                    // However, this will make it difficult to specify percentages that add up to
                    // 100% and still fit within the table width.
                    // Let's say two cells have 50% width. They have 10 pixels of overall margin each.
                    // The table content rect is 500 pixels and the inter-cell padding is 20 pixels.
                    // If we're using internal content size for the width, we would calculate the
                    // the overall cell width for n cells as:
                    // (500 - 20*(n-1) - overallCellMargin1 - overallCellMargin2 - ...) * percentage / 100
                    // + thisOverallCellMargin
                    // = 500 - 20 - 10 - 10) * 0.5 + 10 = 240 pixels overall cell width.
                    // Adding this back, we get 240 + 240 + 20 = 500 pixels.

                    if (cell->GetAttributes().GetTextBoxAttr().GetWidth().IsValid())
                    {
                        int w = converter.GetPixels(cell->GetAttributes().GetTextBoxAttr().GetWidth());
                        if (cell->GetAttributes().GetTextBoxAttr().GetWidth().GetUnits() == wxTEXT_ATTR_UNITS_PERCENTAGE)
                        {
                            percentageCellWidth = w;
                        }
                        else
                        {
                            absoluteCellWidth = w;
                        }
                        // Override absolute width with minimum width if necessary
                        if (cell->GetMinSize().x > 0 && absoluteCellWidth !=1 && cell->GetMinSize().x > absoluteCellWidth)
                            absoluteCellWidth = cell->GetMinSize().x;
                    }

                    if (absoluteCellWidth != -1)
                    {
                        if (absoluteCellWidth > absoluteColWidths[i])
                            absoluteColWidths[i] = absoluteCellWidth;
                    }

                    if (percentageCellWidth != -1)
                    {
                        if (percentageCellWidth > percentageColWidths[i])
                            percentageColWidths[i] = percentageCellWidth;
                    }

                    if (colSpan == 1 && cell->GetMinSize().x && cell->GetMinSize().x > minColWidths[i])
                        minColWidths[i] = cell->GetMinSize().x;
                    if (colSpan == 1 && cell->GetMaxSize().x && cell->GetMaxSize().x > maxColWidths[i])
                        maxColWidths[i] = cell->GetMaxSize().x;
                }
            }
        }
    }

    // (2) Allocate initial column widths from minimum widths, absolute values and proportions
    // TODO: simply merge this into (1).
    for (i = 0; i < m_colCount; i++)
    {
        if (absoluteColWidths[i] > 0)
        {
            colWidths[i] = absoluteColWidths[i];
        }
        else if (percentageColWidths[i] > 0)
        {
            colWidths[i] = percentageColWidths[i];

            // This is rubbish - we calculated the absolute widths from percentages, so
            // we can't do it again here.
            //colWidths[i] = (int) (double(percentageColWidths[i]) * double(tableWidth) / 100.0 + 0.5);
        }
    }

    // (3) Process absolute or proportional widths of spanning columns,
    // now that we know what our fixed column widths are going to be.
    // Spanned cells will try to adjust columns so the span will fit.
    // Even existing fixed column widths can be expanded if necessary.
    // Actually, currently fixed columns widths aren't adjusted; instead,
    // the algorithm favours earlier rows and adjusts unspecified column widths
    // the first time only. After that, we can't know whether the column has been
    // specified explicitly or not. (We could make a note if necessary.)
    for (j = 0; j < m_rowCount; j++)
    {
        // First get the overall margins so we can calculate percentage widths based on
        // the available content space for all cells on the row

        int overallRowContentMargin = 0;
        int visibleCellCount = 0;

        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextBox* cell = GetCell(j, i);
            if (cell->IsShown())
            {
                int cellTotalLeftMargin = 0, cellTotalRightMargin = 0, cellTotalTopMargin = 0, cellTotalBottomMargin = 0;
                GetTotalMargin(dc, buffer, cell->GetAttributes(), cellTotalLeftMargin, cellTotalRightMargin, cellTotalTopMargin, cellTotalBottomMargin);

                overallRowContentMargin += (cellTotalLeftMargin + cellTotalRightMargin);
                visibleCellCount ++;
            }
        }

        // Add in inter-cell padding
        overallRowContentMargin += ((visibleCellCount-1) * paddingX);

        int rowContentWidth = internalTableWidth - overallRowContentMargin;
        wxSize rowTableSize(rowContentWidth, 0);
        wxTextAttrDimensionConverter converter(dc, scale, rowTableSize);

        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextBox* cell = GetCell(j, i);
            if (cell->IsShown())
            {
                int colSpan = 1;
                if (cell->GetProperties().HasProperty(wxT("colspan")))
                    colSpan = cell->GetProperties().GetPropertyLong(wxT("colspan"));

                if (colSpan > 1)
                {
                    int spans = wxMin(colSpan, m_colCount - i);
                    int cellWidth = 0;
                    if (spans > 0)
                    {
                        if (cell->GetAttributes().GetTextBoxAttr().GetWidth().IsValid())
                        {
                            cellWidth = converter.GetPixels(cell->GetAttributes().GetTextBoxAttr().GetWidth());
                            // Override absolute width with minimum width if necessary
                            if (cell->GetMinSize().x > 0 && cellWidth !=1 && cell->GetMinSize().x > cellWidth)
                                cellWidth = cell->GetMinSize().x;
                        }
                        else
                        {
                            // Do we want to do this? It's the only chance we get to
                            // use the cell's min/max sizes, so we need to work out
                            // how we're going to balance the unspecified spanning cell
                            // width with the possibility more-constrained constituent cell widths.
                            // Say there's a tiny bitmap giving it a max width of 10 pixels. We
                            // don't want to constraint all the spanned columns to fit into this cell.
                            // OK, let's say that if any of the constituent columns don't fit,
                            // then we simply stop constraining the columns; instead, we'll just fit the spanning
                            // cells to the columns later.
                            cellWidth = cell->GetMinSize().x;
                            if (cell->GetMaxSize().x > cellWidth)
                                cellWidth = cell->GetMaxSize().x;
                        }

                        // Subtract the padding between cells
                        int spanningWidth = cellWidth;
                        spanningWidth -= paddingX * (spans-1);

                        if (spanningWidth > 0)
                        {
                            // Now share the spanning width between columns within that span
                            // TODO: take into account min widths of columns within the span
                            int spanningWidthLeft = spanningWidth;
                            int stretchColCount = 0;
                            for (k = i; k < (i+spans); k++)
                            {
                                if (colWidths[k] > 0) // absolute or proportional width has been specified
                                    spanningWidthLeft -= colWidths[k];
                                else
                                    stretchColCount ++;
                            }
                            // Now divide what's left between the remaining columns
                            int colShare = 0;
                            if (stretchColCount > 0)
                                colShare = spanningWidthLeft / stretchColCount;
                            int colShareRemainder = spanningWidthLeft - (colShare * stretchColCount);

                            // If fixed-width columns are currently too big, then we'll later
                            // stretch the spanned cell to fit.

                            if (spanningWidthLeft > 0)
                            {
                                for (k = i; k < (i+spans); k++)
                                {
                                    if (colWidths[k] <= 0) // absolute or proportional width has not been specified
                                    {
                                        int newWidth = colShare;
                                        if (k == (i+spans-1))
                                            newWidth += colShareRemainder; // ensure all pixels are filled
                                        colWidths[k] = newWidth;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // (4) Next, share any remaining space out between columns that have not yet been calculated.
    // TODO: take into account min widths of columns within the span
    int tableWidthMinusPadding = internalTableWidth - (m_colCount-1)*paddingX;
    int widthLeft = tableWidthMinusPadding;
    int stretchColCount = 0;
    for (i = 0; i < m_colCount; i++)
    {
        // TODO: we need to take into account min widths.
        // Subtract min width from width left, then
        // add the colShare to the min width
        if (colWidths[i] > 0) // absolute or proportional width has been specified
            widthLeft -= colWidths[i];
        else
        {
            if (minColWidths[i] > 0)
                widthLeft -= minColWidths[i];

            stretchColCount ++;
        }
    }

    // Now divide what's left between the remaining columns
    int colShare = 0;
    if (stretchColCount > 0)
        colShare = widthLeft / stretchColCount;
    int colShareRemainder = widthLeft - (colShare * stretchColCount);

    // Check we don't have enough space, in which case shrink all columns, overriding
    // any absolute/proportional widths
    // TODO: actually we would like to divide up the shrinkage according to size.
    // How do we calculate the proportions that will achieve this?
    // Could first choose an arbitrary value for stretching cells, and then calculate
    // factors to multiply each width by.
    // TODO: want to record this fact and pass to an iteration that tries e.g. min widths
    if (widthLeft < 0 || (stretchToFitTableWidth && (stretchColCount == 0)))
    {
        colShare = tableWidthMinusPadding / m_colCount;
        colShareRemainder = tableWidthMinusPadding - (colShare * m_colCount);
        for (i = 0; i < m_colCount; i++)
        {
            colWidths[i] = 0;
            minColWidths[i] = 0;
        }
    }

    // We have to adjust the columns if either we need to shrink the
    // table to fit the parent/table width, or we explicitly set the
    // table width and need to stretch out the table.
    if (widthLeft < 0 || stretchToFitTableWidth)
    {
        for (i = 0; i < m_colCount; i++)
        {
            if (colWidths[i] <= 0) // absolute or proportional width has not been specified
            {
                if (minColWidths[i] > 0)
                    colWidths[i] = minColWidths[i] + colShare;
                else
                    colWidths[i] = colShare;
                if (i == (m_colCount-1))
                    colWidths[i] += colShareRemainder; // ensure all pixels are filled
            }
        }
    }

    // TODO: if spanned cells have no specified or max width, make them the
    // as big as the columns they span. Do this for all spanned cells in all
    // rows, of course. Size any spanned cells left over at the end - even if they
    // have width > 0, make sure they're limited to the appropriate column edge.


/*
    Sort out confusion between content width
    and overall width later. For now, assume we specify overall width.

    So, now we've laid out the table to fit into the given space
    and have used specified widths and minimum widths.

    Now we need to consider how we will try to take maximum width into account.

*/

    // (??) TODO: take max width into account

    // (6) Lay out all cells again with the current values

    int maxRight = 0;
    int y = availableSpace.y;
    for (j = 0; j < m_rowCount; j++)
    {
        int x = availableSpace.x; // TODO: take into account centering etc.
        int maxCellHeight = 0;
        int maxSpecifiedCellHeight = 0;

        wxArrayInt actualWidths(m_colCount);

        wxTextAttrDimensionConverter converter(dc, scale);
        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextCell* cell = GetCell(j, i);
            if (cell->IsShown())
            {
                // Get max specified cell height
                // Don't handle percentages for height
                if (cell->GetAttributes().GetTextBoxAttr().GetHeight().IsValid() && cell->GetAttributes().GetTextBoxAttr().GetHeight().GetUnits() != wxTEXT_ATTR_UNITS_PERCENTAGE)
                {
                    int h = converter.GetPixels(cell->GetAttributes().GetTextBoxAttr().GetHeight());
                    if (h > maxSpecifiedCellHeight)
                        maxSpecifiedCellHeight = h;
                }

                if (colWidths[i] > 0) // absolute or proportional width has been specified
                {
                    int colSpan = 1;
                    if (cell->GetProperties().HasProperty(wxT("colspan")))
                        colSpan = cell->GetProperties().GetPropertyLong(wxT("colspan"));

                    wxRect availableCellSpace;

                    // TODO: take into acount spans
                    if (colSpan > 1)
                    {
                        // Calculate the size of this spanning cell from its constituent columns
                        int xx = x;
                        int spans = wxMin(colSpan, m_colCount - i);
                        for (k = i; k < spans; k++)
                        {
                            if (k != i)
                                xx += paddingX;
                            xx += colWidths[k];
                        }
                        availableCellSpace = wxRect(x, y, xx, -1);
                    }
                    else
                        availableCellSpace = wxRect(x, y, colWidths[i], -1);

                    // Store actual width so we can force cell to be the appropriate width on the final loop
                    actualWidths[i] = availableCellSpace.GetWidth();

                    // Lay out cell
                    cell->Invalidate(wxRICHTEXT_ALL);
                    cell->Layout(dc, availableCellSpace, style);

                    // TODO: use GetCachedSize().x to compute 'natural' size

                    x += (availableCellSpace.GetWidth() + paddingX);
                    if (cell->GetCachedSize().y > maxCellHeight)
                        maxCellHeight = cell->GetCachedSize().y;
                }
            }
        }

        maxCellHeight = wxMax(maxCellHeight, maxSpecifiedCellHeight);

        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextCell* cell = GetCell(j, i);
            if (cell->IsShown())
            {
                wxRect availableCellSpace = wxRect(cell->GetPosition(), wxSize(actualWidths[i], maxCellHeight));
                // Lay out cell with new height
                cell->Invalidate(wxRICHTEXT_ALL);
                cell->Layout(dc, availableCellSpace, style);

                // Make sure the cell size really is the appropriate size,
                // not the calculated box size
                cell->SetCachedSize(wxSize(actualWidths[i], maxCellHeight));

                maxRight = wxMax(maxRight, cell->GetPosition().x + cell->GetCachedSize().x);
            }
        }

        y += maxCellHeight;
        if (j < (m_rowCount-1))
            y += paddingY;
    }

    // We need to add back the margins etc.
    {
        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(maxRight - availableSpace.x, y - availableSpace.y));
        GetBoxRects(dc, GetBuffer(), GetAttributes(), marginRect, borderRect, contentRect, paddingRect, outlineRect);
        SetCachedSize(marginRect.GetSize());
    }

    // TODO: calculate max size
    {
        SetMaxSize(GetCachedSize());
    }

    // TODO: calculate min size
    {
        SetMinSize(GetCachedSize());
    }

    // TODO: currently we use either a fixed table width or the parent's size.
    // We also want to be able to calculate the table width from its content,
    // whether using fixed column widths or cell content min/max width.
    // Probably need a boolean flag to say whether we need to stretch cells
    // to fit the table width, or to simply use min/max cell widths. The
    // trouble with this is that if cell widths are not specified, they
    // will be tiny; we could use arbitrary defaults but this seems unsatisfactory.
    // Anyway, ignoring that problem, we probably need to factor layout into a function
    // that can can calculate the maximum unconstrained layout in case table size is
    // not specified. Then LayoutToBestSize() can choose to use either parent size to
    // constrain Layout(), or the previously-calculated max size to constraint layout.

    return true;
}

// Finds the absolute position and row height for the given character position
bool wxRichTextTable::FindPosition(wxDC& dc, long index, wxPoint& pt, int* height, bool forceLineStart)
{
    wxRichTextCell* child = GetCell(index+1);
    if (child)
    {
        // Find the position at the start of the child cell, since the table doesn't
        // have any caret position of its own.
        return child->FindPosition(dc, -1, pt, height, forceLineStart);
    }
    else
        return false;
}

// Get the cell at the given character position (in the range of the table).
wxRichTextCell* wxRichTextTable::GetCell(long pos) const
{
    int row = 0, col = 0;
    if (GetCellRowColumnPosition(pos, row, col))
    {
        return GetCell(row, col);
    }
    else
        return NULL;
}

// Get the row/column for a given character position
bool wxRichTextTable::GetCellRowColumnPosition(long pos, int& row, int& col) const
{
    if (m_colCount == 0 || m_rowCount == 0)
        return false;

    row = (int) (pos / m_colCount);
    col = pos - (row * m_colCount);

    wxASSERT(row < m_rowCount && col < m_colCount);

    if (row < m_rowCount && col < m_colCount)
        return true;
    else
        return false;
}

// Calculate range, taking row/cell ordering into account instead of relying
// on list ordering.
void wxRichTextTable::CalculateRange(long start, long& end)
{
    long current = start;
    long lastEnd = current;

    if (IsTopLevel())
    {
        current = 0;
        lastEnd = 0;
    }

    int i, j;
    for (i = 0; i < m_rowCount; i++)
    {
        for (j = 0; j < m_colCount; j++)
        {
            wxRichTextCell* child = GetCell(i, j);
            if (child)
            {
                long childEnd = 0;

                child->CalculateRange(current, childEnd);

                lastEnd = childEnd;
                current = childEnd + 1;
            }
        }
    }

    // A top-level object always has a range of size 1,
    // because its children don't count at this level.
    end = start;
    m_range.SetRange(start, start);

    // An object with no children has zero length
    if (m_children.GetCount() == 0)
        lastEnd --;
    m_ownRange.SetRange(0, lastEnd);
}

// Gets the range size.
bool wxRichTextTable::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position, wxArrayInt* partialExtents) const
{
    return wxRichTextBox::GetRangeSize(range, size, descent, dc, flags, position, partialExtents);
}

// Deletes content in the given range.
bool wxRichTextTable::DeleteRange(const wxRichTextRange& WXUNUSED(range))
{
    // TODO: implement deletion of cells
    return true;
}

// Gets any text in this object for the given range.
wxString wxRichTextTable::GetTextForRange(const wxRichTextRange& range) const
{
    return wxRichTextBox::GetTextForRange(range);
}

// Copies this object.
void wxRichTextTable::Copy(const wxRichTextTable& obj)
{
    wxRichTextBox::Copy(obj);

    ClearTable();

    m_rowCount = obj.m_rowCount;
    m_colCount = obj.m_colCount;

    m_cells.Add(wxRichTextObjectPtrArray(), m_rowCount);

    int i, j;
    for (i = 0; i < m_rowCount; i++)
    {
        wxRichTextObjectPtrArray& colArray = m_cells[i];
        for (j = 0; j < m_colCount; j++)
        {
            wxRichTextCell* cell = wxDynamicCast(obj.GetCell(i, j)->Clone(), wxRichTextCell);
            AppendChild(cell);

            colArray.Add(cell);
        }
    }
}

void wxRichTextTable::ClearTable()
{
    m_cells.Clear();
    DeleteChildren();
}

bool wxRichTextTable::CreateTable(int rows, int cols)
{
    ClearTable();

    m_rowCount = rows;
    m_colCount = cols;

    m_cells.Add(wxRichTextObjectPtrArray(), rows);

    int i, j;
    for (i = 0; i < rows; i++)
    {
        wxRichTextObjectPtrArray& colArray = m_cells[i];
        for (j = 0; j < cols; j++)
        {
            wxRichTextCell* cell = new wxRichTextCell;
            AppendChild(cell);
            cell->AddParagraph(wxEmptyString);

            colArray.Add(cell);
        }
    }

    return true;
}

wxRichTextCell* wxRichTextTable::GetCell(int row, int col) const
{
    wxASSERT(row < m_rowCount);
    wxASSERT(col < m_colCount);

    if (row < m_rowCount && col < m_colCount)
    {
        wxRichTextObjectPtrArray& colArray = m_cells[row];
        wxRichTextObject* obj = colArray[col];
        return wxDynamicCast(obj, wxRichTextCell);
    }
    else
        return NULL;
}

// Returns a selection object specifying the selections between start and end character positions.
// For example, a table would deduce what cells (of range length 1) are selected when dragging across the table.
wxRichTextSelection wxRichTextTable::GetSelection(long start, long end) const
{
    wxRichTextSelection selection;
    selection.SetContainer((wxRichTextTable*) this);

    if (start > end)
    {
        long tmp = end;
        end = start;
        start = tmp;
    }

    wxASSERT( start >= 0 && end < (m_colCount * m_rowCount));

    if (end >= (m_colCount * m_rowCount))
        return selection;

    // We need to find the rectangle of cells that is described by the rectangle
    // with start, end as the diagonal. Make sure we don't add cells that are
    // not currenty visible because they are overlapped by spanning cells.
/*
    --------------------------
    | 0  | 1  | 2  | 3  | 4  |
    --------------------------
    | 5  | 6  | 7  | 8  | 9  |
    --------------------------
    | 10 | 11 | 12 | 13 | 14 |
    --------------------------
    | 15 | 16 | 17 | 18 | 19 |
    --------------------------

    Let's say we select 6 -> 18.

    Left and right edge cols of rectangle are 1 and 3 inclusive. Find least/greatest to find
    which is left and which is right.

    Top and bottom edge rows are 1 and 3 inclusive. Again, find least/greatest to find top and bottom.

    Now go through rows from 1 to 3 and only add cells that are (a) within above column range
    and (b) shown.


*/

    int leftCol = start - m_colCount * int(start/m_colCount);
    int rightCol = end - m_colCount * int(end/m_colCount);

    int topRow = int(start/m_colCount);
    int bottomRow = int(end/m_colCount);

    if (leftCol > rightCol)
    {
        int tmp = rightCol;
        rightCol = leftCol;
        leftCol = tmp;
    }

    if (topRow > bottomRow)
    {
        int tmp = bottomRow;
        bottomRow = topRow;
        topRow = tmp;
    }

    int i, j;
    for (i = topRow; i <= bottomRow; i++)
    {
        for (j = leftCol; j <= rightCol; j++)
        {
            wxRichTextCell* cell = GetCell(i, j);
            if (cell && cell->IsShown())
                selection.Add(cell->GetRange());
        }
    }

    return selection;
}

// Sets the attributes for the cells specified by the selection.
bool wxRichTextTable::SetCellStyle(const wxRichTextSelection& selection, const wxRichTextAttr& style, int flags)
{
    if (selection.GetContainer() != this)
        return false;

    wxRichTextBuffer* buffer = GetBuffer();
    bool haveControl = (buffer && buffer->GetRichTextCtrl() != NULL);
    bool withUndo = haveControl && ((flags & wxRICHTEXT_SETSTYLE_WITH_UNDO) != 0);

    if (withUndo)
        buffer->BeginBatchUndo(_("Set Cell Style"));

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextCell* cell = wxDynamicCast(node->GetData(), wxRichTextCell);
        if (cell && selection.WithinSelection(cell->GetRange().GetStart()))
            SetStyle(cell, style, flags);
        node = node->GetNext();
    }

    // Do action, or delay it until end of batch.
    if (withUndo)
        buffer->EndBatchUndo();

    return true;
}

bool wxRichTextTable::DeleteRows(int startRow, int noRows)
{
    wxASSERT((startRow + noRows) < m_rowCount);
    if ((startRow + noRows) >= m_rowCount)
        return false;

    int i, j;
    for (i = startRow; i < (startRow+noRows); i++)
    {
        wxRichTextObjectPtrArray& colArray = m_cells[startRow];
        for (j = 0; j < (int) colArray.GetCount(); j++)
        {
            wxRichTextObject* cell = colArray[j];
            RemoveChild(cell, true);
        }

        // Keep deleting at the same position, since we move all
        // the others up
        m_cells.RemoveAt(startRow);
    }

    m_rowCount = m_rowCount - noRows;

    return true;
}

bool wxRichTextTable::DeleteColumns(int startCol, int noCols)
{
    wxASSERT((startCol + noCols) < m_colCount);
    if ((startCol + noCols) >= m_colCount)
        return false;

    bool deleteRows = (noCols == m_colCount);

    int i, j;
    for (i = 0; i < m_rowCount; i++)
    {
        wxRichTextObjectPtrArray& colArray = m_cells[deleteRows ? 0 : i];
        for (j = startCol; j < (startCol+noCols); j++)
        {
            wxRichTextObject* cell = colArray[j];
            RemoveChild(cell, true);
        }

        if (deleteRows)
            m_cells.RemoveAt(0);
    }

    if (deleteRows)
        m_rowCount = 0;
    m_colCount = m_colCount - noCols;

    return true;
}

bool wxRichTextTable::AddRows(int startRow, int noRows, const wxRichTextAttr& attr)
{
    wxASSERT(startRow <= m_rowCount);
    if (startRow > m_rowCount)
        return false;

    int i, j;
    for (i = 0; i < noRows; i++)
    {
        int idx;
        if (startRow == m_rowCount)
        {
            m_cells.Add(wxRichTextObjectPtrArray());
            idx = m_cells.GetCount() - 1;
        }
        else
        {
            m_cells.Insert(wxRichTextObjectPtrArray(), startRow+i);
            idx = startRow+i;
        }

        wxRichTextObjectPtrArray& colArray = m_cells[idx];
        for (j = 0; j < m_colCount; j++)
        {
            wxRichTextCell* cell = new wxRichTextCell;
            cell->GetAttributes() = attr;

            AppendChild(cell);
            colArray.Add(cell);
        }
    }

    m_rowCount = m_rowCount + noRows;
    return true;
}

bool wxRichTextTable::AddColumns(int startCol, int noCols, const wxRichTextAttr& attr)
{
    wxASSERT(startCol <= m_colCount);
    if (startCol > m_colCount)
        return false;

    int i, j;
    for (i = 0; i < m_rowCount; i++)
    {
        wxRichTextObjectPtrArray& colArray = m_cells[i];
        for (j = 0; j < noCols; j++)
        {
            wxRichTextCell* cell = new wxRichTextCell;
            cell->GetAttributes() = attr;

            AppendChild(cell);

            if (startCol == m_colCount)
                colArray.Add(cell);
            else
                colArray.Insert(cell, startCol+j);
        }
    }

    m_colCount = m_colCount + noCols;

    return true;
}

// Edit properties via a GUI
bool wxRichTextTable::EditProperties(wxWindow* parent, wxRichTextBuffer* buffer)
{
    wxRichTextObjectPropertiesDialog boxDlg(this, wxGetTopLevelParent(parent), wxID_ANY, _("Table Properties"));
    boxDlg.SetAttributes(GetAttributes());

    if (boxDlg.ShowModal() == wxID_OK)
    {
        boxDlg.ApplyStyle(buffer->GetRichTextCtrl(), wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_RESET);
        return true;
    }
    else
        return false;
}

/*
 * Module to initialise and clean up handlers
 */

class wxRichTextModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxRichTextModule)
public:
    wxRichTextModule() {}
    bool OnInit()
    {
        wxRichTextBuffer::SetRenderer(new wxRichTextStdRenderer);
        wxRichTextBuffer::InitStandardHandlers();
        wxRichTextParagraph::InitDefaultTabs();
        return true;
    }
    void OnExit()
    {
        wxRichTextBuffer::CleanUpHandlers();
        wxRichTextDecimalToRoman(-1);
        wxRichTextParagraph::ClearDefaultTabs();
        wxRichTextCtrl::ClearAvailableFontNames();
        wxRichTextBuffer::SetRenderer(NULL);
    }
};

IMPLEMENT_DYNAMIC_CLASS(wxRichTextModule, wxModule)


// If the richtext lib is dynamically loaded after the app has already started
// (such as from wxPython) then the built-in module system will not init this
// module.  Provide this function to do it manually.
void wxRichTextModuleInit()
{
    wxModule* module = new wxRichTextModule;
    module->Init();
    wxModule::RegisterModule(module);
}


/*!
 * Commands for undo/redo
 *
 */

wxRichTextCommand::wxRichTextCommand(const wxString& name, wxRichTextCommandId id, wxRichTextBuffer* buffer,
                                     wxRichTextParagraphLayoutBox* container, wxRichTextCtrl* ctrl, bool ignoreFirstTime): wxCommand(true, name)
{
    /* wxRichTextAction* action = */ new wxRichTextAction(this, name, id, buffer, container, ctrl, ignoreFirstTime);
}

wxRichTextCommand::wxRichTextCommand(const wxString& name): wxCommand(true, name)
{
}

wxRichTextCommand::~wxRichTextCommand()
{
    ClearActions();
}

void wxRichTextCommand::AddAction(wxRichTextAction* action)
{
    if (!m_actions.Member(action))
        m_actions.Append(action);
}

bool wxRichTextCommand::Do()
{
    for (wxList::compatibility_iterator node = m_actions.GetFirst(); node; node = node->GetNext())
    {
        wxRichTextAction* action = (wxRichTextAction*) node->GetData();
        action->Do();
    }

    return true;
}

bool wxRichTextCommand::Undo()
{
    for (wxList::compatibility_iterator node = m_actions.GetLast(); node; node = node->GetPrevious())
    {
        wxRichTextAction* action = (wxRichTextAction*) node->GetData();
        action->Undo();
    }

    return true;
}

void wxRichTextCommand::ClearActions()
{
    WX_CLEAR_LIST(wxList, m_actions);
}

/*!
 * Individual action
 *
 */

wxRichTextAction::wxRichTextAction(wxRichTextCommand* cmd, const wxString& name, wxRichTextCommandId id,
                                   wxRichTextBuffer* buffer, wxRichTextParagraphLayoutBox* container,
                                   wxRichTextCtrl* ctrl, bool ignoreFirstTime)
{
    m_buffer = buffer;
    m_object = NULL;
    m_containerAddress.Create(buffer, container);
    m_ignoreThis = ignoreFirstTime;
    m_cmdId = id;
    m_position = -1;
    m_ctrl = ctrl;
    m_name = name;
    m_newParagraphs.SetDefaultStyle(buffer->GetDefaultStyle());
    m_newParagraphs.SetBasicStyle(buffer->GetBasicStyle());
    if (cmd)
        cmd->AddAction(this);
}

wxRichTextAction::~wxRichTextAction()
{
    if (m_object)
        delete m_object;
}

// Returns the container that this action refers to, using the container address and top-level buffer.
wxRichTextParagraphLayoutBox* wxRichTextAction::GetContainer() const
{
    wxRichTextParagraphLayoutBox* container = wxDynamicCast(GetContainerAddress().GetObject(m_buffer), wxRichTextParagraphLayoutBox);
    return container;
}


void wxRichTextAction::CalculateRefreshOptimizations(wxArrayInt& optimizationLineCharPositions, wxArrayInt& optimizationLineYPositions)
{
    // Store a list of line start character and y positions so we can figure out which area
    // we need to refresh

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
    wxRichTextParagraphLayoutBox* container = GetContainer();
    wxASSERT(container != NULL);
    if (!container)
        return;

    // NOTE: we're assuming that the buffer is laid out correctly at this point.
    // If we had several actions, which only invalidate and leave layout until the
    // paint handler is called, then this might not be true. So we may need to switch
    // optimisation on only when we're simply adding text and not simultaneously
    // deleting a selection, for example. Or, we make sure the buffer is laid out correctly
    // first, but of course this means we'll be doing it twice.
    if (!m_buffer->IsDirty() && m_ctrl) // can only do optimisation if the buffer is already laid out correctly
    {
        wxSize clientSize = m_ctrl->GetClientSize();
        wxPoint firstVisiblePt = m_ctrl->GetFirstVisiblePoint();
        int lastY = firstVisiblePt.y + clientSize.y;

        wxRichTextParagraph* para = container->GetParagraphAtPosition(GetRange().GetStart());
        wxRichTextObjectList::compatibility_iterator node = container->GetChildren().Find(para);
        while (node)
        {
            wxRichTextParagraph* child = (wxRichTextParagraph*) node->GetData();
            wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
            while (node2)
            {
                wxRichTextLine* line = node2->GetData();
                wxPoint pt = line->GetAbsolutePosition();
                wxRichTextRange range = line->GetAbsoluteRange();

                if (pt.y > lastY)
                {
                    node2 = wxRichTextLineList::compatibility_iterator();
                    node = wxRichTextObjectList::compatibility_iterator();
                }
                else if (range.GetStart() > GetPosition() && pt.y >= firstVisiblePt.y)
                {
                    optimizationLineCharPositions.Add(range.GetStart());
                    optimizationLineYPositions.Add(pt.y);
                }

                if (node2)
                    node2 = node2->GetNext();
            }

            if (node)
                node = node->GetNext();
        }
    }
#endif
}

bool wxRichTextAction::Do()
{
    m_buffer->Modify(true);

    wxRichTextParagraphLayoutBox* container = GetContainer();
    wxASSERT(container != NULL);
    if (!container)
        return false;

    switch (m_cmdId)
    {
    case wxRICHTEXT_INSERT:
        {
            // Store a list of line start character and y positions so we can figure out which area
            // we need to refresh
            wxArrayInt optimizationLineCharPositions;
            wxArrayInt optimizationLineYPositions;

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            CalculateRefreshOptimizations(optimizationLineCharPositions, optimizationLineYPositions);
#endif

            container->InsertFragment(GetRange().GetStart(), m_newParagraphs);
            container->UpdateRanges();

            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            container->InvalidateHierarchy(wxRichTextRange(wxMax(0, GetRange().GetStart()-1), GetRange().GetEnd()));

            long newCaretPosition = GetPosition() + m_newParagraphs.GetOwnRange().GetLength();

            // Character position to caret position
            newCaretPosition --;

            // Don't take into account the last newline
            if (m_newParagraphs.GetPartialParagraph())
                newCaretPosition --;
            else
                if (m_newParagraphs.GetChildren().GetCount() > 1)
                {
                    wxRichTextObject* p = (wxRichTextObject*) m_newParagraphs.GetChildren().GetLast()->GetData();
                    if (p->GetRange().GetLength() == 1)
                        newCaretPosition --;
                }

            newCaretPosition = wxMin(newCaretPosition, (container->GetOwnRange().GetEnd()-1));

            UpdateAppearance(newCaretPosition, true /* send update event */, & optimizationLineCharPositions, & optimizationLineYPositions, true /* do */);

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());
            cmdEvent.SetContainer(container);

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_DELETE:
        {
            wxArrayInt optimizationLineCharPositions;
            wxArrayInt optimizationLineYPositions;

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            CalculateRefreshOptimizations(optimizationLineCharPositions, optimizationLineYPositions);
#endif

            container->DeleteRange(GetRange());
            container->UpdateRanges();
            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            container->InvalidateHierarchy(wxRichTextRange(GetRange().GetStart(), GetRange().GetStart()));

            long caretPos = GetRange().GetStart()-1;
            if (caretPos >= container->GetOwnRange().GetEnd())
                caretPos --;

            UpdateAppearance(caretPos, true /* send update event */, & optimizationLineCharPositions, & optimizationLineYPositions, true /* do */);

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());
            cmdEvent.SetContainer(container);

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_CHANGE_STYLE:
        {
            ApplyParagraphs(GetNewParagraphs());

            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            container->InvalidateHierarchy(GetRange());

            UpdateAppearance(GetPosition());

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());
            cmdEvent.SetContainer(container);

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_CHANGE_ATTRIBUTES:
        {
            wxRichTextObject* obj = m_objectAddress.GetObject(m_buffer); // container->GetChildAtPosition(GetRange().GetStart());
            if (obj)
            {
                wxRichTextAttr oldAttr = obj->GetAttributes();
                obj->GetAttributes() = m_attributes;
                m_attributes = oldAttr;
            }

            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            container->InvalidateHierarchy(GetRange());

            UpdateAppearance(GetPosition());

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());
            cmdEvent.SetContainer(container);

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_CHANGE_OBJECT:
        {
            wxRichTextObject* obj = m_objectAddress.GetObject(m_buffer);
            // wxRichTextObject* obj = container->GetChildAtPosition(GetRange().GetStart());
            if (obj && m_object)
            {
                wxRichTextObjectList::compatibility_iterator node = container->GetChildren().Find(obj);
                if (node)
                {
                    wxRichTextObject* obj = node->GetData();
                    node->SetData(m_object);
                    m_object = obj;
                }
            }

            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            container->InvalidateHierarchy(GetRange());

            UpdateAppearance(GetPosition());

            // TODO: send new kind of modification event

            break;
        }
    default:
        break;
    }

    return true;
}

bool wxRichTextAction::Undo()
{
    m_buffer->Modify(true);

    wxRichTextParagraphLayoutBox* container = GetContainer();
    wxASSERT(container != NULL);
    if (!container)
        return false;

    switch (m_cmdId)
    {
    case wxRICHTEXT_INSERT:
        {
            wxArrayInt optimizationLineCharPositions;
            wxArrayInt optimizationLineYPositions;

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            CalculateRefreshOptimizations(optimizationLineCharPositions, optimizationLineYPositions);
#endif

            container->DeleteRange(GetRange());
            container->UpdateRanges();
            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            container->InvalidateHierarchy(wxRichTextRange(GetRange().GetStart(), GetRange().GetStart()));

            long newCaretPosition = GetPosition() - 1;

            UpdateAppearance(newCaretPosition, true, /* send update event */ & optimizationLineCharPositions, & optimizationLineYPositions, false /* undo */);

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());
            cmdEvent.SetContainer(container);

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_DELETE:
        {
            wxArrayInt optimizationLineCharPositions;
            wxArrayInt optimizationLineYPositions;

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            CalculateRefreshOptimizations(optimizationLineCharPositions, optimizationLineYPositions);
#endif

            container->InsertFragment(GetRange().GetStart(), m_oldParagraphs);
            container->UpdateRanges();
            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            container->InvalidateHierarchy(GetRange());

            UpdateAppearance(GetPosition(), true, /* send update event */ & optimizationLineCharPositions, & optimizationLineYPositions, false /* undo */);

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());
            cmdEvent.SetContainer(container);

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_CHANGE_STYLE:
        {
            ApplyParagraphs(GetOldParagraphs());
            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            container->InvalidateHierarchy(GetRange());

            UpdateAppearance(GetPosition());

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());
            cmdEvent.SetContainer(container);

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_CHANGE_ATTRIBUTES:
    case wxRICHTEXT_CHANGE_OBJECT:
        {
            return Do();
        }
    default:
        break;
    }

    return true;
}

/// Update the control appearance
void wxRichTextAction::UpdateAppearance(long caretPosition, bool sendUpdateEvent, wxArrayInt* optimizationLineCharPositions, wxArrayInt* optimizationLineYPositions, bool isDoCmd)
{
    wxRichTextParagraphLayoutBox* container = GetContainer();
    wxASSERT(container != NULL);
    if (!container)
        return;

    if (m_ctrl)
    {
        m_ctrl->SetFocusObject(container);
        m_ctrl->SetCaretPosition(caretPosition);

        if (!m_ctrl->IsFrozen())
        {
            wxRect containerRect = container->GetRect();

            m_ctrl->LayoutContent();

            // Refresh everything if there were floating objects or the container changed size
            // (we can't yet optimize in these cases, since more complex interaction with other content occurs)
            if (container->GetFloatingObjectCount() > 0 || (container->GetParent() && containerRect != container->GetRect()))
            {
                m_ctrl->Refresh(false);
            }
            else

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            // Find refresh rectangle if we are in a position to optimise refresh
            if ((m_cmdId == wxRICHTEXT_INSERT || m_cmdId == wxRICHTEXT_DELETE) && optimizationLineCharPositions)
            {
                size_t i;

                wxSize clientSize = m_ctrl->GetClientSize();
                wxPoint firstVisiblePt = m_ctrl->GetFirstVisiblePoint();

                // Start/end positions
                int firstY = 0;
                int lastY = firstVisiblePt.y + clientSize.y;

                bool foundEnd = false;

                // position offset - how many characters were inserted
                int positionOffset = GetRange().GetLength();

                // Determine whether this is Do or Undo, and adjust positionOffset accordingly
                if ((m_cmdId == wxRICHTEXT_DELETE && isDoCmd) || (m_cmdId == wxRICHTEXT_INSERT && !isDoCmd))
                    positionOffset = - positionOffset;

                // find the first line which is being drawn at the same position as it was
                // before. Since we're talking about a simple insertion, we can assume
                // that the rest of the window does not need to be redrawn.

                wxRichTextParagraph* para = container->GetParagraphAtPosition(GetPosition());
                // Since we support floating layout, we should redraw the whole para instead of just
                // the first line touching the invalid range.
                if (para)
                {
                    firstY = para->GetPosition().y;
                }

                wxRichTextObjectList::compatibility_iterator node = container->GetChildren().Find(para);
                while (node)
                {
                    wxRichTextParagraph* child = (wxRichTextParagraph*) node->GetData();
                    wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
                    while (node2)
                    {
                        wxRichTextLine* line = node2->GetData();
                        wxPoint pt = line->GetAbsolutePosition();
                        wxRichTextRange range = line->GetAbsoluteRange();

                        // we want to find the first line that is in the same position
                        // as before. This will mean we're at the end of the changed text.

                        if (pt.y > lastY) // going past the end of the window, no more info
                        {
                            node2 = wxRichTextLineList::compatibility_iterator();
                            node = wxRichTextObjectList::compatibility_iterator();
                        }
                        // Detect last line in the buffer
                        else if (!node2->GetNext() && para->GetRange().Contains(container->GetOwnRange().GetEnd()))
                        {
                            // If deleting text, make sure we refresh below as well as above
                            if (positionOffset >= 0)
                            {
                                foundEnd = true;
                                lastY = pt.y + line->GetSize().y;
                            }

                            node2 = wxRichTextLineList::compatibility_iterator();
                            node = wxRichTextObjectList::compatibility_iterator();

                            break;
                        }
                        else
                        {
                            // search for this line being at the same position as before
                            for (i = 0; i < optimizationLineCharPositions->GetCount(); i++)
                            {
                                if (((*optimizationLineCharPositions)[i] + positionOffset == range.GetStart()) &&
                                    ((*optimizationLineYPositions)[i] == pt.y))
                                {
                                    // Stop, we're now the same as we were
                                    foundEnd = true;

                                    lastY = pt.y;

                                    node2 = wxRichTextLineList::compatibility_iterator();
                                    node = wxRichTextObjectList::compatibility_iterator();

                                    break;
                                }
                            }
                        }

                        if (node2)
                            node2 = node2->GetNext();
                    }

                    if (node)
                        node = node->GetNext();
                }

                firstY = wxMax(firstVisiblePt.y, firstY);
                if (!foundEnd)
                    lastY = firstVisiblePt.y + clientSize.y;

                // Convert to device coordinates
                wxRect rect(m_ctrl->GetPhysicalPoint(wxPoint(firstVisiblePt.x, firstY)), wxSize(clientSize.x, lastY - firstY));
                m_ctrl->RefreshRect(rect);
            }
            else
#endif
                m_ctrl->Refresh(false);

            m_ctrl->PositionCaret();

            // This causes styles to persist when doing programmatic
            // content creation except when Freeze/Thaw is used, so
            // disable this and check for the consequences.
            // m_ctrl->SetDefaultStyleToCursorStyle();

            if (sendUpdateEvent)
                wxTextCtrl::SendTextUpdatedEvent(m_ctrl);
        }
    }
}

/// Replace the buffer paragraphs with the new ones.
void wxRichTextAction::ApplyParagraphs(const wxRichTextParagraphLayoutBox& fragment)
{
    wxRichTextParagraphLayoutBox* container = GetContainer();
    wxASSERT(container != NULL);
    if (!container)
        return;

    wxRichTextObjectList::compatibility_iterator node = fragment.GetChildren().GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

        // We'll replace the existing paragraph by finding the paragraph at this position,
        // delete its node data, and setting a copy as the new node data.
        // TODO: make more efficient by simply swapping old and new paragraph objects.

        wxRichTextParagraph* existingPara = container->GetParagraphAtPosition(para->GetRange().GetStart());
        if (existingPara)
        {
            wxRichTextObjectList::compatibility_iterator bufferParaNode = container->GetChildren().Find(existingPara);
            if (bufferParaNode)
            {
                wxRichTextParagraph* newPara = new wxRichTextParagraph(*para);
                newPara->SetParent(container);

                bufferParaNode->SetData(newPara);

                delete existingPara;
            }
        }

        node = node->GetNext();
    }
}


/*!
 * wxRichTextRange
 * This stores beginning and end positions for a range of data.
 */

WX_DEFINE_OBJARRAY(wxRichTextRangeArray);

/// Limit this range to be within 'range'
bool wxRichTextRange::LimitTo(const wxRichTextRange& range)
{
    if (m_start < range.m_start)
        m_start = range.m_start;

    if (m_end > range.m_end)
        m_end = range.m_end;

    return true;
}

/*!
 * wxRichTextImage implementation
 * This object represents an image.
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextImage, wxRichTextObject)

wxRichTextImage::wxRichTextImage(const wxImage& image, wxRichTextObject* parent, wxRichTextAttr* charStyle):
    wxRichTextObject(parent)
{
    m_imageBlock.MakeImageBlockDefaultQuality(image, wxBITMAP_TYPE_PNG);
    if (charStyle)
        SetAttributes(*charStyle);
}

wxRichTextImage::wxRichTextImage(const wxRichTextImageBlock& imageBlock, wxRichTextObject* parent, wxRichTextAttr* charStyle):
    wxRichTextObject(parent)
{
    m_imageBlock = imageBlock;
    if (charStyle)
        SetAttributes(*charStyle);
}

/// Create a cached image at the required size
bool wxRichTextImage::LoadImageCache(wxDC& dc, bool resetCache)
{
    if (resetCache || !m_imageCache.IsOk() /* || m_imageCache.GetWidth() != size.x || m_imageCache.GetHeight() != size.y */)
    {
        if (!m_imageBlock.IsOk())
            return false;

        wxImage image;
        m_imageBlock.Load(image);
        if (!image.IsOk())
            return false;

        int width = image.GetWidth();
        int height = image.GetHeight();

        if (GetAttributes().GetTextBoxAttr().GetWidth().IsValid() && GetAttributes().GetTextBoxAttr().GetWidth().GetValue() > 0)
        {
            if (GetAttributes().GetTextBoxAttr().GetWidth().GetUnits() == wxTEXT_ATTR_UNITS_TENTHS_MM)
                width = ConvertTenthsMMToPixels(dc, GetAttributes().GetTextBoxAttr().GetWidth().GetValue());
            else
                width = GetAttributes().GetTextBoxAttr().GetWidth().GetValue();
        }
        if (GetAttributes().GetTextBoxAttr().GetHeight().IsValid() && GetAttributes().GetTextBoxAttr().GetHeight().GetValue() > 0)
        {
            if (GetAttributes().GetTextBoxAttr().GetHeight().GetUnits() == wxTEXT_ATTR_UNITS_TENTHS_MM)
                height = ConvertTenthsMMToPixels(dc, GetAttributes().GetTextBoxAttr().GetHeight().GetValue());
            else
                height = GetAttributes().GetTextBoxAttr().GetHeight().GetValue();
        }

        if (image.GetWidth() == width && image.GetHeight() == height)
            m_imageCache = wxBitmap(image);
        else
        {
            // If the original width and height is small, e.g. 400 or below,
            // scale up and then down to improve image quality. This can make
            // a big difference, with not much performance hit.
            int upscaleThreshold = 400;
            wxImage img;
            if (image.GetWidth() <= upscaleThreshold || image.GetHeight() <= upscaleThreshold)
            {
                img = image.Scale(image.GetWidth()*2, image.GetHeight()*2);
                img.Rescale(width, height, wxIMAGE_QUALITY_HIGH);
            }
            else
                img = image.Scale(width, height, wxIMAGE_QUALITY_HIGH);
            m_imageCache = wxBitmap(img);
        }
    }

    return m_imageCache.IsOk();
}

/// Draw the item
bool wxRichTextImage::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int WXUNUSED(descent), int WXUNUSED(style))
{
    if (!IsShown())
        return true;

    // Don't need cached size AFAIK
    // wxSize size = GetCachedSize();
    if (!LoadImageCache(dc))
        return false;

    DrawBoxAttributes(dc, GetBuffer(), GetAttributes(), wxRect(GetPosition(), GetCachedSize()));

#if 0
    int y = rect.y + (rect.height - m_imageCache.GetHeight());

    dc.DrawBitmap(m_imageCache, rect.x, y, true);
#endif

    wxSize imageSize(m_imageCache.GetWidth(), m_imageCache.GetHeight());
    wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
    marginRect = rect; // outer rectangle, will calculate contentRect
    GetBoxRects(dc, GetBuffer(), GetAttributes(), marginRect, borderRect, contentRect, paddingRect, outlineRect);

    dc.DrawBitmap(m_imageCache, contentRect.x, contentRect.y, true);

    if (selection.WithinSelection(range.GetStart(), this))
    {
        wxCheckSetBrush(dc, *wxBLACK_BRUSH);
        wxCheckSetPen(dc, *wxBLACK_PEN);
        dc.SetLogicalFunction(wxINVERT);
        dc.DrawRectangle(contentRect);
        dc.SetLogicalFunction(wxCOPY);
    }

    return true;
}

/// Lay the item out
bool wxRichTextImage::Layout(wxDC& dc, const wxRect& rect, int WXUNUSED(style))
{
    if (!LoadImageCache(dc))
        return false;

    wxSize imageSize(m_imageCache.GetWidth(), m_imageCache.GetHeight());
    wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
    contentRect = wxRect(wxPoint(0,0), imageSize);
    GetBoxRects(dc, GetBuffer(), GetAttributes(), marginRect, borderRect, contentRect, paddingRect, outlineRect);

    wxSize overallSize = marginRect.GetSize();

    SetCachedSize(overallSize);
    SetMaxSize(overallSize);
    SetMinSize(overallSize);
    SetPosition(rect.GetPosition());

    return true;
}

/// Get/set the object size for the given range. Returns false if the range
/// is invalid for this object.
bool wxRichTextImage::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& WXUNUSED(descent), wxDC& dc, int WXUNUSED(flags), wxPoint WXUNUSED(position), wxArrayInt* partialExtents) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    if (!((wxRichTextImage*)this)->LoadImageCache(dc))
    {
        size.x = 0; size.y = 0;
        if (partialExtents)
            partialExtents->Add(0);
        return false;
    }

    wxSize imageSize(m_imageCache.GetWidth(), m_imageCache.GetHeight());
    wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
    contentRect = wxRect(wxPoint(0,0), imageSize);
    GetBoxRects(dc, GetBuffer(), GetAttributes(), marginRect, borderRect, contentRect, paddingRect, outlineRect);

    wxSize overallSize = marginRect.GetSize();

    if (partialExtents)
        partialExtents->Add(overallSize.x);

    size = overallSize;

    return true;
}

// Get the 'natural' size for an object. For an image, it would be the
// image size.
wxTextAttrSize wxRichTextImage::GetNaturalSize() const
{
    wxTextAttrSize size;
    if (GetImageCache().IsOk())
    {
        size.SetWidth(GetImageCache().GetWidth(), wxTEXT_ATTR_UNITS_PIXELS);
        size.SetHeight(GetImageCache().GetHeight(), wxTEXT_ATTR_UNITS_PIXELS);
    }
    return size;
}


/// Copy
void wxRichTextImage::Copy(const wxRichTextImage& obj)
{
    wxRichTextObject::Copy(obj);

    m_imageBlock = obj.m_imageBlock;
}

/// Edit properties via a GUI
bool wxRichTextImage::EditProperties(wxWindow* parent, wxRichTextBuffer* buffer)
{
    wxRichTextObjectPropertiesDialog imageDlg(this, wxGetTopLevelParent(parent), wxID_ANY, _("Picture Properties"));
    imageDlg.SetAttributes(GetAttributes());

    if (imageDlg.ShowModal() == wxID_OK)
    {
        // By passing wxRICHTEXT_SETSTYLE_RESET, indeterminate attributes set by the user will be set as
        // indeterminate in the object.
        imageDlg.ApplyStyle(buffer->GetRichTextCtrl(), wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_RESET);
        return true;
    }
    else
        return false;
}

/*!
 * Utilities
 *
 */

/// Compare two attribute objects
bool wxTextAttrEq(const wxRichTextAttr& attr1, const wxRichTextAttr& attr2)
{
    return (attr1 == attr2);
}

// Partial equality test taking flags into account
bool wxTextAttrEqPartial(const wxRichTextAttr& attr1, const wxRichTextAttr& attr2)
{
    return attr1.EqPartial(attr2);
}

/// Compare tabs
bool wxRichTextTabsEq(const wxArrayInt& tabs1, const wxArrayInt& tabs2)
{
    if (tabs1.GetCount() != tabs2.GetCount())
        return false;

    size_t i;
    for (i = 0; i < tabs1.GetCount(); i++)
    {
        if (tabs1[i] != tabs2[i])
            return false;
    }
    return true;
}

bool wxRichTextApplyStyle(wxRichTextAttr& destStyle, const wxRichTextAttr& style, wxRichTextAttr* compareWith)
{
    return destStyle.Apply(style, compareWith);
}

// Remove attributes
bool wxRichTextRemoveStyle(wxRichTextAttr& destStyle, const wxRichTextAttr& style)
{
    return destStyle.RemoveStyle(style);
}

/// Combine two bitlists, specifying the bits of interest with separate flags.
bool wxRichTextCombineBitlists(int& valueA, int valueB, int& flagsA, int flagsB)
{
    return wxRichTextAttr::CombineBitlists(valueA, valueB, flagsA, flagsB);
}

/// Compare two bitlists
bool wxRichTextBitlistsEqPartial(int valueA, int valueB, int flags)
{
    return wxRichTextAttr::BitlistsEqPartial(valueA, valueB, flags);
}

/// Split into paragraph and character styles
bool wxRichTextSplitParaCharStyles(const wxRichTextAttr& style, wxRichTextAttr& parStyle, wxRichTextAttr& charStyle)
{
    return wxRichTextAttr::SplitParaCharStyles(style, parStyle, charStyle);
}

/// Convert a decimal to Roman numerals
wxString wxRichTextDecimalToRoman(long n)
{
    static wxArrayInt decimalNumbers;
    static wxArrayString romanNumbers;

    // Clean up arrays
    if (n == -1)
    {
        decimalNumbers.Clear();
        romanNumbers.Clear();
        return wxEmptyString;
    }

    if (decimalNumbers.GetCount() == 0)
    {
        #define wxRichTextAddDecRom(n, r) decimalNumbers.Add(n); romanNumbers.Add(r);

        wxRichTextAddDecRom(1000, wxT("M"));
        wxRichTextAddDecRom(900, wxT("CM"));
        wxRichTextAddDecRom(500, wxT("D"));
        wxRichTextAddDecRom(400, wxT("CD"));
        wxRichTextAddDecRom(100, wxT("C"));
        wxRichTextAddDecRom(90, wxT("XC"));
        wxRichTextAddDecRom(50, wxT("L"));
        wxRichTextAddDecRom(40, wxT("XL"));
        wxRichTextAddDecRom(10, wxT("X"));
        wxRichTextAddDecRom(9, wxT("IX"));
        wxRichTextAddDecRom(5, wxT("V"));
        wxRichTextAddDecRom(4, wxT("IV"));
        wxRichTextAddDecRom(1, wxT("I"));
    }

    int i = 0;
    wxString roman;

    while (n > 0 && i < 13)
    {
        if (n >= decimalNumbers[i])
        {
            n -= decimalNumbers[i];
            roman += romanNumbers[i];
        }
        else
        {
            i ++;
        }
    }
    if (roman.IsEmpty())
        roman = wxT("0");
    return roman;
}

/*!
 * wxRichTextFileHandler
 * Base class for file handlers
 */

IMPLEMENT_CLASS(wxRichTextFileHandler, wxObject)

#if wxUSE_FFILE && wxUSE_STREAMS
bool wxRichTextFileHandler::LoadFile(wxRichTextBuffer *buffer, const wxString& filename)
{
    wxFFileInputStream stream(filename);
    if (stream.Ok())
        return LoadFile(buffer, stream);

    return false;
}

bool wxRichTextFileHandler::SaveFile(wxRichTextBuffer *buffer, const wxString& filename)
{
    wxFFileOutputStream stream(filename);
    if (stream.Ok())
        return SaveFile(buffer, stream);

    return false;
}
#endif // wxUSE_FFILE && wxUSE_STREAMS

/// Can we handle this filename (if using files)? By default, checks the extension.
bool wxRichTextFileHandler::CanHandle(const wxString& filename) const
{
    wxString path, file, ext;
    wxFileName::SplitPath(filename, & path, & file, & ext);

    return (ext.Lower() == GetExtension());
}

/*!
 * wxRichTextTextHandler
 * Plain text handler
 */

IMPLEMENT_CLASS(wxRichTextPlainTextHandler, wxRichTextFileHandler)

#if wxUSE_STREAMS
bool wxRichTextPlainTextHandler::DoLoadFile(wxRichTextBuffer *buffer, wxInputStream& stream)
{
    if (!stream.IsOk())
        return false;

    wxString str;
    int lastCh = 0;

    while (!stream.Eof())
    {
        int ch = stream.GetC();

        if (!stream.Eof())
        {
            if (ch == 10 && lastCh != 13)
                str += wxT('\n');

            if (ch > 0 && ch != 10)
                str += wxChar(ch);

            lastCh = ch;
        }
    }

    buffer->ResetAndClearCommands();
    buffer->Clear();
    buffer->AddParagraphs(str);
    buffer->UpdateRanges();

    return true;
}

bool wxRichTextPlainTextHandler::DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream)
{
    if (!stream.IsOk())
        return false;

    wxString text = buffer->GetText();

    wxString newLine = wxRichTextLineBreakChar;
    text.Replace(newLine, wxT("\n"));

    wxCharBuffer buf = text.ToAscii();

    stream.Write((const char*) buf, text.length());
    return true;
}
#endif // wxUSE_STREAMS

/*
 * Stores information about an image, in binary in-memory form
 */

wxRichTextImageBlock::wxRichTextImageBlock()
{
    Init();
}

wxRichTextImageBlock::wxRichTextImageBlock(const wxRichTextImageBlock& block):wxObject()
{
    Init();
    Copy(block);
}

wxRichTextImageBlock::~wxRichTextImageBlock()
{
    wxDELETEA(m_data);
}

void wxRichTextImageBlock::Init()
{
    m_data = NULL;
    m_dataSize = 0;
    m_imageType = wxBITMAP_TYPE_INVALID;
}

void wxRichTextImageBlock::Clear()
{
    wxDELETEA(m_data);
    m_dataSize = 0;
    m_imageType = wxBITMAP_TYPE_INVALID;
}


// Load the original image into a memory block.
// If the image is not a JPEG, we must convert it into a JPEG
// to conserve space.
// If it's not a JPEG we can make use of 'image', already scaled, so we don't have to
// load the image a 2nd time.

bool wxRichTextImageBlock::MakeImageBlock(const wxString& filename, wxBitmapType imageType,
                                          wxImage& image, bool convertToJPEG)
{
    m_imageType = imageType;

    wxString filenameToRead(filename);
    bool removeFile = false;

    if (imageType == wxBITMAP_TYPE_INVALID)
        return false; // Could not determine image type

    if ((imageType != wxBITMAP_TYPE_JPEG) && convertToJPEG)
    {
        wxString tempFile =
            wxFileName::CreateTempFileName(_("image"));

        wxASSERT(!tempFile.IsEmpty());

        image.SaveFile(tempFile, wxBITMAP_TYPE_JPEG);
        filenameToRead = tempFile;
        removeFile = true;

        m_imageType = wxBITMAP_TYPE_JPEG;
    }
    wxFile file;
    if (!file.Open(filenameToRead))
        return false;

    m_dataSize = (size_t) file.Length();
    file.Close();

    if (m_data)
        delete[] m_data;
    m_data = ReadBlock(filenameToRead, m_dataSize);

    if (removeFile)
        wxRemoveFile(filenameToRead);

    return (m_data != NULL);
}

// Make an image block from the wxImage in the given
// format.
bool wxRichTextImageBlock::MakeImageBlock(wxImage& image, wxBitmapType imageType, int quality)
{
    image.SetOption(wxT("quality"), quality);

    if (imageType == wxBITMAP_TYPE_INVALID)
        return false; // Could not determine image type

    return DoMakeImageBlock(image, imageType);
}

// Uses a const wxImage for efficiency, but can't set quality (only relevant for JPEG)
bool wxRichTextImageBlock::MakeImageBlockDefaultQuality(const wxImage& image, wxBitmapType imageType)
{
    if (imageType == wxBITMAP_TYPE_INVALID)
        return false; // Could not determine image type

    return DoMakeImageBlock(image, imageType);
}

// Makes the image block
bool wxRichTextImageBlock::DoMakeImageBlock(const wxImage& image, wxBitmapType imageType)
{
    wxMemoryOutputStream memStream;
    if (!image.SaveFile(memStream, imageType))
    {
        return false;
    }

    unsigned char* block = new unsigned char[memStream.GetSize()];
    if (!block)
        return false;

    if (m_data)
        delete[] m_data;
    m_data = block;

    m_imageType = imageType;
    m_dataSize = memStream.GetSize();

    memStream.CopyTo(m_data, m_dataSize);

    return (m_data != NULL);
}

// Write to a file
bool wxRichTextImageBlock::Write(const wxString& filename)
{
    return WriteBlock(filename, m_data, m_dataSize);
}

void wxRichTextImageBlock::Copy(const wxRichTextImageBlock& block)
{
    m_imageType = block.m_imageType;
    wxDELETEA(m_data);
    m_dataSize = block.m_dataSize;
    if (m_dataSize == 0)
        return;

    m_data = new unsigned char[m_dataSize];
    unsigned int i;
    for (i = 0; i < m_dataSize; i++)
        m_data[i] = block.m_data[i];
}

//// Operators
void wxRichTextImageBlock::operator=(const wxRichTextImageBlock& block)
{
    Copy(block);
}

// Load a wxImage from the block
bool wxRichTextImageBlock::Load(wxImage& image)
{
    if (!m_data)
        return false;

    // Read in the image.
#if wxUSE_STREAMS
    wxMemoryInputStream mstream(m_data, m_dataSize);
    bool success = image.LoadFile(mstream, GetImageType());
#else
    wxString tempFile = wxFileName::CreateTempFileName(_("image"));
    wxASSERT(!tempFile.IsEmpty());

    if (!WriteBlock(tempFile, m_data, m_dataSize))
    {
        return false;
    }
    success = image.LoadFile(tempFile, GetImageType());
    wxRemoveFile(tempFile);
#endif

    return success;
}

// Write data in hex to a stream
bool wxRichTextImageBlock::WriteHex(wxOutputStream& stream)
{
    if (m_dataSize == 0)
        return true;

    int bufSize = 100000;
    if (int(2*m_dataSize) < bufSize)
        bufSize = 2*m_dataSize;
    char* buf = new char[bufSize+1];

    int left = m_dataSize;
    int n, i, j;
    j = 0;
    while (left > 0)
    {
        if (left*2 > bufSize)
        {
            n = bufSize; left -= (bufSize/2);
        }
        else
        {
            n = left*2; left = 0;
        }

        char* b = buf;
        for (i = 0; i < (n/2); i++)
        {
            wxDecToHex(m_data[j], b, b+1);
            b += 2; j ++;
        }

        buf[n] = 0;
        stream.Write((const char*) buf, n);
    }
    delete[] buf;
    return true;
}

// Read data in hex from a stream
bool wxRichTextImageBlock::ReadHex(wxInputStream& stream, int length, wxBitmapType imageType)
{
    int dataSize = length/2;

    if (m_data)
        delete[] m_data;

    // create a null terminated temporary string:
    char str[3];
    str[2] = '\0';

    m_data = new unsigned char[dataSize];
    int i;
    for (i = 0; i < dataSize; i ++)
    {
        str[0] = (char)stream.GetC();
        str[1] = (char)stream.GetC();

        m_data[i] = (unsigned char)wxHexToDec(str);
    }

    m_dataSize = dataSize;
    m_imageType = imageType;

    return true;
}

// Allocate and read from stream as a block of memory
unsigned char* wxRichTextImageBlock::ReadBlock(wxInputStream& stream, size_t size)
{
    unsigned char* block = new unsigned char[size];
    if (!block)
        return NULL;

    stream.Read(block, size);

    return block;
}

unsigned char* wxRichTextImageBlock::ReadBlock(const wxString& filename, size_t size)
{
    wxFileInputStream stream(filename);
    if (!stream.Ok())
        return NULL;

    return ReadBlock(stream, size);
}

// Write memory block to stream
bool wxRichTextImageBlock::WriteBlock(wxOutputStream& stream, unsigned char* block, size_t size)
{
    stream.Write((void*) block, size);
    return stream.IsOk();

}

// Write memory block to file
bool wxRichTextImageBlock::WriteBlock(const wxString& filename, unsigned char* block, size_t size)
{
    wxFileOutputStream outStream(filename);
    if (!outStream.Ok())
        return false;

    return WriteBlock(outStream, block, size);
}

// Gets the extension for the block's type
wxString wxRichTextImageBlock::GetExtension() const
{
    wxImageHandler* handler = wxImage::FindHandler(GetImageType());
    if (handler)
        return handler->GetExtension();
    else
        return wxEmptyString;
}

#if wxUSE_DATAOBJ

/*!
 * The data object for a wxRichTextBuffer
 */

const wxChar *wxRichTextBufferDataObject::ms_richTextBufferFormatId = wxT("wxShape");

wxRichTextBufferDataObject::wxRichTextBufferDataObject(wxRichTextBuffer* richTextBuffer)
{
    m_richTextBuffer = richTextBuffer;

    // this string should uniquely identify our format, but is otherwise
    // arbitrary
    m_formatRichTextBuffer.SetId(GetRichTextBufferFormatId());

    SetFormat(m_formatRichTextBuffer);
}

wxRichTextBufferDataObject::~wxRichTextBufferDataObject()
{
    delete m_richTextBuffer;
}

// after a call to this function, the richTextBuffer is owned by the caller and it
// is responsible for deleting it!
wxRichTextBuffer* wxRichTextBufferDataObject::GetRichTextBuffer()
{
    wxRichTextBuffer* richTextBuffer = m_richTextBuffer;
    m_richTextBuffer = NULL;

    return richTextBuffer;
}

wxDataFormat wxRichTextBufferDataObject::GetPreferredFormat(Direction WXUNUSED(dir)) const
{
    return m_formatRichTextBuffer;
}

size_t wxRichTextBufferDataObject::GetDataSize() const
{
    if (!m_richTextBuffer)
        return 0;

    wxString bufXML;

    {
        wxStringOutputStream stream(& bufXML);
        if (!m_richTextBuffer->SaveFile(stream, wxRICHTEXT_TYPE_XML))
        {
            wxLogError(wxT("Could not write the buffer to an XML stream.\nYou may have forgotten to add the XML file handler."));
            return 0;
        }
    }

#if wxUSE_UNICODE
    wxCharBuffer buffer = bufXML.mb_str(wxConvUTF8);
    return strlen(buffer) + 1;
#else
    return bufXML.Length()+1;
#endif
}

bool wxRichTextBufferDataObject::GetDataHere(void *pBuf) const
{
    if (!pBuf || !m_richTextBuffer)
        return false;

    wxString bufXML;

    {
        wxStringOutputStream stream(& bufXML);
        if (!m_richTextBuffer->SaveFile(stream, wxRICHTEXT_TYPE_XML))
        {
            wxLogError(wxT("Could not write the buffer to an XML stream.\nYou may have forgotten to add the XML file handler."));
            return 0;
        }
    }

#if wxUSE_UNICODE
    wxCharBuffer buffer = bufXML.mb_str(wxConvUTF8);
    size_t len = strlen(buffer);
    memcpy((char*) pBuf, (const char*) buffer, len);
    ((char*) pBuf)[len] = 0;
#else
    size_t len = bufXML.Length();
    memcpy((char*) pBuf, (const char*) bufXML.c_str(), len);
    ((char*) pBuf)[len] = 0;
#endif

    return true;
}

bool wxRichTextBufferDataObject::SetData(size_t WXUNUSED(len), const void *buf)
{
    wxDELETE(m_richTextBuffer);

    wxString bufXML((const char*) buf, wxConvUTF8);

    m_richTextBuffer = new wxRichTextBuffer;

    wxStringInputStream stream(bufXML);
    if (!m_richTextBuffer->LoadFile(stream, wxRICHTEXT_TYPE_XML))
    {
        wxLogError(wxT("Could not read the buffer from an XML stream.\nYou may have forgotten to add the XML file handler."));

        wxDELETE(m_richTextBuffer);

        return false;
    }
    return true;
}

#endif
    // wxUSE_DATAOBJ


/*
 * wxRichTextFontTable
 * Manages quick access to a pool of fonts for rendering rich text
 */

WX_DECLARE_STRING_HASH_MAP_WITH_DECL(wxFont, wxRichTextFontTableHashMap, class WXDLLIMPEXP_RICHTEXT);

class wxRichTextFontTableData: public wxObjectRefData
{
public:
    wxRichTextFontTableData() {}

    wxFont FindFont(const wxRichTextAttr& fontSpec);

    wxRichTextFontTableHashMap  m_hashMap;
};

wxFont wxRichTextFontTableData::FindFont(const wxRichTextAttr& fontSpec)
{
    wxString facename(fontSpec.GetFontFaceName());
    wxString spec(wxString::Format(wxT("%d-%d-%d-%d-%s-%d"), fontSpec.GetFontSize(), fontSpec.GetFontStyle(), fontSpec.GetFontWeight(), (int) fontSpec.GetFontUnderlined(), facename.c_str(), (int) fontSpec.GetFontEncoding()));
    wxRichTextFontTableHashMap::iterator entry = m_hashMap.find(spec);

    if ( entry == m_hashMap.end() )
    {
        wxFont font(fontSpec.GetFontSize(), wxDEFAULT, fontSpec.GetFontStyle(), fontSpec.GetFontWeight(), fontSpec.GetFontUnderlined(), facename.c_str());
        m_hashMap[spec] = font;
        return font;
    }
    else
    {
        return entry->second;
    }
}

IMPLEMENT_DYNAMIC_CLASS(wxRichTextFontTable, wxObject)

wxRichTextFontTable::wxRichTextFontTable()
{
    m_refData = new wxRichTextFontTableData;
}

wxRichTextFontTable::wxRichTextFontTable(const wxRichTextFontTable& table)
    : wxObject()
{
    (*this) = table;
}

wxRichTextFontTable::~wxRichTextFontTable()
{
    UnRef();
}

bool wxRichTextFontTable::operator == (const wxRichTextFontTable& table) const
{
    return (m_refData == table.m_refData);
}

void wxRichTextFontTable::operator= (const wxRichTextFontTable& table)
{
    Ref(table);
}

wxFont wxRichTextFontTable::FindFont(const wxRichTextAttr& fontSpec)
{
    wxRichTextFontTableData* data = (wxRichTextFontTableData*) m_refData;
    if (data)
        return data->FindFont(fontSpec);
    else
        return wxFont();
}

void wxRichTextFontTable::Clear()
{
    wxRichTextFontTableData* data = (wxRichTextFontTableData*) m_refData;
    if (data)
        data->m_hashMap.clear();
}

// wxTextBoxAttr


void wxTextBoxAttr::Reset()
{
    m_flags = 0;
    m_floatMode = wxTEXT_BOX_ATTR_FLOAT_NONE;
    m_clearMode = wxTEXT_BOX_ATTR_CLEAR_NONE;
    m_collapseMode = wxTEXT_BOX_ATTR_COLLAPSE_NONE;
    m_verticalAlignment = wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_NONE;

    m_margins.Reset();
    m_padding.Reset();
    m_position.Reset();

    m_size.Reset();

    m_border.Reset();
    m_outline.Reset();
}

// Equality test
bool wxTextBoxAttr::operator== (const wxTextBoxAttr& attr) const
{
    return (
        m_flags == attr.m_flags &&
        m_floatMode == attr.m_floatMode &&
        m_clearMode == attr.m_clearMode &&
        m_collapseMode == attr.m_collapseMode &&
        m_verticalAlignment == attr.m_verticalAlignment &&

        m_margins == attr.m_margins &&
        m_padding == attr.m_padding &&
        m_position == attr.m_position &&

        m_size == attr.m_size &&

        m_border == attr.m_border &&
        m_outline == attr.m_outline
        );
}

// Partial equality test
bool wxTextBoxAttr::EqPartial(const wxTextBoxAttr& attr) const
{
    if (attr.HasFloatMode() && HasFloatMode() && (GetFloatMode() != attr.GetFloatMode()))
        return false;

    if (attr.HasClearMode() && HasClearMode() && (GetClearMode() != attr.GetClearMode()))
        return false;

    if (attr.HasCollapseBorders() && HasCollapseBorders() && (attr.GetCollapseBorders() != GetCollapseBorders()))
        return false;

    if (attr.HasVerticalAlignment() && HasVerticalAlignment() && (attr.GetVerticalAlignment() != GetVerticalAlignment()))
        return false;

    // Position

    if (!m_position.EqPartial(attr.m_position))
        return false;

    // Margins

    if (!m_margins.EqPartial(attr.m_margins))
        return false;

    // Padding

    if (!m_padding.EqPartial(attr.m_padding))
        return false;

    // Border

    if (!GetBorder().EqPartial(attr.GetBorder()))
        return false;

    // Outline

    if (!GetOutline().EqPartial(attr.GetOutline()))
        return false;

    return true;
}

// Merges the given attributes. If compareWith
// is non-NULL, then it will be used to mask out those attributes that are the same in style
// and compareWith, for situations where we don't want to explicitly set inherited attributes.
bool wxTextBoxAttr::Apply(const wxTextBoxAttr& attr, const wxTextBoxAttr* compareWith)
{
    if (attr.HasFloatMode())
    {
        if (!(compareWith && compareWith->HasFloatMode() && compareWith->GetFloatMode() == attr.GetFloatMode()))
            SetFloatMode(attr.GetFloatMode());
    }

    if (attr.HasClearMode())
    {
        if (!(compareWith && compareWith->HasClearMode() && compareWith->GetClearMode() == attr.GetClearMode()))
            SetClearMode(attr.GetClearMode());
    }

    if (attr.HasCollapseBorders())
    {
        if (!(compareWith && compareWith->HasCollapseBorders() && compareWith->GetCollapseBorders() == attr.GetCollapseBorders()))
            SetCollapseBorders(attr.GetCollapseBorders());
    }

    if (attr.HasVerticalAlignment())
    {
        if (!(compareWith && compareWith->HasVerticalAlignment() && compareWith->GetVerticalAlignment() == attr.GetVerticalAlignment()))
            SetVerticalAlignment(attr.GetVerticalAlignment());
    }

    m_margins.Apply(attr.m_margins, compareWith ? (& attr.m_margins) : (const wxTextAttrDimensions*) NULL);
    m_padding.Apply(attr.m_padding, compareWith ? (& attr.m_padding) : (const wxTextAttrDimensions*) NULL);
    m_position.Apply(attr.m_position, compareWith ? (& attr.m_position) : (const wxTextAttrDimensions*) NULL);

    m_size.Apply(attr.m_size, compareWith ? (& attr.m_size) : (const wxTextAttrSize*) NULL);

    m_border.Apply(attr.m_border, compareWith ? (& attr.m_border) : (const wxTextAttrBorders*) NULL);
    m_outline.Apply(attr.m_outline, compareWith ? (& attr.m_outline) : (const wxTextAttrBorders*) NULL);

    return true;
}

// Remove specified attributes from this object
bool wxTextBoxAttr::RemoveStyle(const wxTextBoxAttr& attr)
{
    if (attr.HasFloatMode())
        RemoveFlag(wxTEXT_BOX_ATTR_FLOAT);

    if (attr.HasClearMode())
        RemoveFlag(wxTEXT_BOX_ATTR_CLEAR);

    if (attr.HasCollapseBorders())
        RemoveFlag(wxTEXT_BOX_ATTR_COLLAPSE_BORDERS);

    if (attr.HasVerticalAlignment())
        RemoveFlag(wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT);

    m_margins.RemoveStyle(attr.m_margins);
    m_padding.RemoveStyle(attr.m_padding);
    m_position.RemoveStyle(attr.m_position);

    m_size.RemoveStyle(attr.m_size);

    m_border.RemoveStyle(attr.m_border);
    m_outline.RemoveStyle(attr.m_outline);

    return true;
}

// Collects the attributes that are common to a range of content, building up a note of
// which attributes are absent in some objects and which clash in some objects.
void wxTextBoxAttr::CollectCommonAttributes(const wxTextBoxAttr& attr, wxTextBoxAttr& clashingAttr, wxTextBoxAttr& absentAttr)
{
    if (attr.HasFloatMode())
    {
        if (!clashingAttr.HasFloatMode() && !absentAttr.HasFloatMode())
        {
            if (HasFloatMode())
            {
                if (GetFloatMode() != attr.GetFloatMode())
                {
                    clashingAttr.AddFlag(wxTEXT_BOX_ATTR_FLOAT);
                    RemoveFlag(wxTEXT_BOX_ATTR_FLOAT);
                }
            }
            else
                SetFloatMode(attr.GetFloatMode());
        }
    }
    else
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_FLOAT);

    if (attr.HasClearMode())
    {
        if (!clashingAttr.HasClearMode() && !absentAttr.HasClearMode())
        {
            if (HasClearMode())
            {
                if (GetClearMode() != attr.GetClearMode())
                {
                    clashingAttr.AddFlag(wxTEXT_BOX_ATTR_CLEAR);
                    RemoveFlag(wxTEXT_BOX_ATTR_CLEAR);
                }
            }
            else
                SetClearMode(attr.GetClearMode());
        }
    }
    else
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_CLEAR);

    if (attr.HasCollapseBorders())
    {
        if (!clashingAttr.HasCollapseBorders() && !absentAttr.HasCollapseBorders())
        {
            if (HasCollapseBorders())
            {
                if (GetCollapseBorders() != attr.GetCollapseBorders())
                {
                    clashingAttr.AddFlag(wxTEXT_BOX_ATTR_COLLAPSE_BORDERS);
                    RemoveFlag(wxTEXT_BOX_ATTR_COLLAPSE_BORDERS);
                }
            }
            else
                SetCollapseBorders(attr.GetCollapseBorders());
        }
    }
    else
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_COLLAPSE_BORDERS);

    if (attr.HasVerticalAlignment())
    {
        if (!clashingAttr.HasVerticalAlignment() && !absentAttr.HasVerticalAlignment())
        {
            if (HasVerticalAlignment())
            {
                if (GetVerticalAlignment() != attr.GetVerticalAlignment())
                {
                    clashingAttr.AddFlag(wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT);
                    RemoveFlag(wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT);
                }
            }
            else
                SetVerticalAlignment(attr.GetVerticalAlignment());
        }
    }
    else
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT);

    m_margins.CollectCommonAttributes(attr.m_margins, clashingAttr.m_margins, absentAttr.m_margins);
    m_padding.CollectCommonAttributes(attr.m_padding, clashingAttr.m_padding, absentAttr.m_padding);
    m_position.CollectCommonAttributes(attr.m_position, clashingAttr.m_position, absentAttr.m_position);

    m_size.CollectCommonAttributes(attr.m_size, clashingAttr.m_size, absentAttr.m_size);

    m_border.CollectCommonAttributes(attr.m_border, clashingAttr.m_border, absentAttr.m_border);
    m_outline.CollectCommonAttributes(attr.m_outline, clashingAttr.m_outline, absentAttr.m_outline);
}

// wxRichTextAttr

void wxRichTextAttr::Copy(const wxRichTextAttr& attr)
{
    wxTextAttr::Copy(attr);

    m_textBoxAttr = attr.m_textBoxAttr;
}

bool wxRichTextAttr::operator==(const wxRichTextAttr& attr) const
{
    if (!(wxTextAttr::operator==(attr)))
        return false;

    return (m_textBoxAttr == attr.m_textBoxAttr);
}

// Partial equality test taking comparison object into account
bool wxRichTextAttr::EqPartial(const wxRichTextAttr& attr) const
{
    if (!(wxTextAttr::EqPartial(attr)))
        return false;

    return m_textBoxAttr.EqPartial(attr.m_textBoxAttr);
}

// Merges the given attributes. If compareWith
// is non-NULL, then it will be used to mask out those attributes that are the same in style
// and compareWith, for situations where we don't want to explicitly set inherited attributes.
bool wxRichTextAttr::Apply(const wxRichTextAttr& style, const wxRichTextAttr* compareWith)
{
    wxTextAttr::Apply(style, compareWith);

    return m_textBoxAttr.Apply(style.m_textBoxAttr, compareWith ? (& compareWith->m_textBoxAttr) : (const wxTextBoxAttr*) NULL);
}

// Remove specified attributes from this object
bool wxRichTextAttr::RemoveStyle(const wxRichTextAttr& attr)
{
    wxTextAttr::RemoveStyle(*this, attr);

    return m_textBoxAttr.RemoveStyle(attr.m_textBoxAttr);
}

// Collects the attributes that are common to a range of content, building up a note of
// which attributes are absent in some objects and which clash in some objects.
void wxRichTextAttr::CollectCommonAttributes(const wxRichTextAttr& attr, wxRichTextAttr& clashingAttr, wxRichTextAttr& absentAttr)
{
    wxTextAttrCollectCommonAttributes(*this, attr, clashingAttr, absentAttr);

    m_textBoxAttr.CollectCommonAttributes(attr.m_textBoxAttr, clashingAttr.m_textBoxAttr, absentAttr.m_textBoxAttr);
}

// Partial equality test
bool wxTextAttrBorder::EqPartial(const wxTextAttrBorder& border) const
{
    if (border.HasStyle() && !HasStyle() && (border.GetStyle() != GetStyle()))
        return false;

    if (border.HasColour() && !HasColour() && (border.GetColourLong() != GetColourLong()))
        return false;

    if (border.HasWidth() && !HasWidth() && !(border.GetWidth() == GetWidth()))
        return false;

    return true;
}

// Apply border to 'this', but not if the same as compareWith
bool wxTextAttrBorder::Apply(const wxTextAttrBorder& border, const wxTextAttrBorder* compareWith)
{
    if (border.HasStyle())
    {
        if (!(compareWith && (border.GetStyle() == compareWith->GetStyle())))
            SetStyle(border.GetStyle());
    }
    if (border.HasColour())
    {
        if (!(compareWith && (border.GetColourLong() == compareWith->GetColourLong())))
            SetColour(border.GetColourLong());
    }
    if (border.HasWidth())
    {
        if (!(compareWith && (border.GetWidth() == compareWith->GetWidth())))
            SetWidth(border.GetWidth());
    }

    return true;
}

// Remove specified attributes from this object
bool wxTextAttrBorder::RemoveStyle(const wxTextAttrBorder& attr)
{
    if (attr.HasStyle() && HasStyle())
        SetFlags(GetFlags() & ~wxTEXT_BOX_ATTR_BORDER_STYLE);
    if (attr.HasColour() && HasColour())
        SetFlags(GetFlags() & ~wxTEXT_BOX_ATTR_BORDER_COLOUR);
    if (attr.HasWidth() && HasWidth())
        m_borderWidth.Reset();

    return true;
}

// Collects the attributes that are common to a range of content, building up a note of
// which attributes are absent in some objects and which clash in some objects.
void wxTextAttrBorder::CollectCommonAttributes(const wxTextAttrBorder& attr, wxTextAttrBorder& clashingAttr, wxTextAttrBorder& absentAttr)
{
    if (attr.HasStyle())
    {
        if (!clashingAttr.HasStyle() && !absentAttr.HasStyle())
        {
            if (HasStyle())
            {
                if (GetStyle() != attr.GetStyle())
                {
                    clashingAttr.AddFlag(wxTEXT_BOX_ATTR_BORDER_STYLE);
                    RemoveFlag(wxTEXT_BOX_ATTR_BORDER_STYLE);
                }
            }
            else
                SetStyle(attr.GetStyle());
        }
    }
    else
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_BORDER_STYLE);

    if (attr.HasColour())
    {
        if (!clashingAttr.HasColour() && !absentAttr.HasColour())
        {
            if (HasColour())
            {
                if (GetColour() != attr.GetColour())
                {
                    clashingAttr.AddFlag(wxTEXT_BOX_ATTR_BORDER_COLOUR);
                    RemoveFlag(wxTEXT_BOX_ATTR_BORDER_COLOUR);
                }
            }
            else
                SetColour(attr.GetColourLong());
        }
    }
    else
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_BORDER_COLOUR);

    m_borderWidth.CollectCommonAttributes(attr.m_borderWidth, clashingAttr.m_borderWidth, absentAttr.m_borderWidth);
}

// Partial equality test
bool wxTextAttrBorders::EqPartial(const wxTextAttrBorders& borders) const
{
    return m_left.EqPartial(borders.m_left) && m_right.EqPartial(borders.m_right) &&
            m_top.EqPartial(borders.m_top) && m_bottom.EqPartial(borders.m_bottom);
}

// Apply border to 'this', but not if the same as compareWith
bool wxTextAttrBorders::Apply(const wxTextAttrBorders& borders, const wxTextAttrBorders* compareWith)
{
    m_left.Apply(borders.m_left, compareWith ? (& compareWith->m_left) : (const wxTextAttrBorder*) NULL);
    m_right.Apply(borders.m_right, compareWith ? (& compareWith->m_right) : (const wxTextAttrBorder*) NULL);
    m_top.Apply(borders.m_top, compareWith ? (& compareWith->m_top) : (const wxTextAttrBorder*) NULL);
    m_bottom.Apply(borders.m_bottom, compareWith ? (& compareWith->m_bottom) : (const wxTextAttrBorder*) NULL);
    return true;
}

// Remove specified attributes from this object
bool wxTextAttrBorders::RemoveStyle(const wxTextAttrBorders& attr)
{
    m_left.RemoveStyle(attr.m_left);
    m_right.RemoveStyle(attr.m_right);
    m_top.RemoveStyle(attr.m_top);
    m_bottom.RemoveStyle(attr.m_bottom);
    return true;
}

// Collects the attributes that are common to a range of content, building up a note of
// which attributes are absent in some objects and which clash in some objects.
void wxTextAttrBorders::CollectCommonAttributes(const wxTextAttrBorders& attr, wxTextAttrBorders& clashingAttr, wxTextAttrBorders& absentAttr)
{
    m_left.CollectCommonAttributes(attr.m_left, clashingAttr.m_left, absentAttr.m_left);
    m_right.CollectCommonAttributes(attr.m_right, clashingAttr.m_right, absentAttr.m_right);
    m_top.CollectCommonAttributes(attr.m_top, clashingAttr.m_top, absentAttr.m_top);
    m_bottom.CollectCommonAttributes(attr.m_bottom, clashingAttr.m_bottom, absentAttr.m_bottom);
}

// Set style of all borders
void wxTextAttrBorders::SetStyle(int style)
{
    m_left.SetStyle(style);
    m_right.SetStyle(style);
    m_top.SetStyle(style);
    m_bottom.SetStyle(style);
}

// Set colour of all borders
void wxTextAttrBorders::SetColour(unsigned long colour)
{
    m_left.SetColour(colour);
    m_right.SetColour(colour);
    m_top.SetColour(colour);
    m_bottom.SetColour(colour);
}

void wxTextAttrBorders::SetColour(const wxColour& colour)
{
    m_left.SetColour(colour);
    m_right.SetColour(colour);
    m_top.SetColour(colour);
    m_bottom.SetColour(colour);
}

// Set width of all borders
void wxTextAttrBorders::SetWidth(const wxTextAttrDimension& width)
{
    m_left.SetWidth(width);
    m_right.SetWidth(width);
    m_top.SetWidth(width);
    m_bottom.SetWidth(width);
}

// Partial equality test
bool wxTextAttrDimension::EqPartial(const wxTextAttrDimension& dim) const
{
    if (dim.IsValid() && IsValid() && !((*this) == dim))
        return false;
    else
        return true;
}

bool wxTextAttrDimension::Apply(const wxTextAttrDimension& dim, const wxTextAttrDimension* compareWith)
{
    if (dim.IsValid())
    {
        if (!(compareWith && dim == (*compareWith)))
            (*this) = dim;
    }

    return true;
}

// Collects the attributes that are common to a range of content, building up a note of
// which attributes are absent in some objects and which clash in some objects.
void wxTextAttrDimension::CollectCommonAttributes(const wxTextAttrDimension& attr, wxTextAttrDimension& clashingAttr, wxTextAttrDimension& absentAttr)
{
    if (attr.IsValid())
    {
        if (!clashingAttr.IsValid() && !absentAttr.IsValid())
        {
            if (IsValid())
            {
                if (!((*this) == attr))
                {
                    clashingAttr.SetValid(true);
                    SetValid(false);
                }
            }
            else
                (*this) = attr;
        }
    }
    else
        absentAttr.SetValid(true);
}

wxTextAttrDimensionConverter::wxTextAttrDimensionConverter(wxDC& dc, double scale, const wxSize& parentSize)
{
    m_ppi = dc.GetPPI().x; m_scale = scale; m_parentSize = parentSize;
}

wxTextAttrDimensionConverter::wxTextAttrDimensionConverter(int ppi, double scale, const wxSize& parentSize)
{
    m_ppi = ppi; m_scale = scale; m_parentSize = parentSize;
}

int wxTextAttrDimensionConverter::ConvertTenthsMMToPixels(int units) const
{
    return wxRichTextObject::ConvertTenthsMMToPixels(m_ppi, units, m_scale);
}

int wxTextAttrDimensionConverter::ConvertPixelsToTenthsMM(int pixels) const
{
    return wxRichTextObject::ConvertPixelsToTenthsMM(m_ppi, pixels, m_scale);
}

int wxTextAttrDimensionConverter::GetPixels(const wxTextAttrDimension& dim, int direction) const
{
    if (dim.GetUnits() == wxTEXT_ATTR_UNITS_TENTHS_MM)
        return ConvertTenthsMMToPixels(dim.GetValue());
    else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_PIXELS)
        return dim.GetValue();
    else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_PERCENTAGE)
    {
        wxASSERT(m_parentSize != wxDefaultSize);
        if (direction == wxHORIZONTAL)
            return (int) (double(m_parentSize.x) * double(dim.GetValue()) / 100.0);
        else
            return (int) (double(m_parentSize.y) * double(dim.GetValue()) / 100.0);
    }
    else
    {
        wxASSERT(false);
        return 0;
    }
}

int wxTextAttrDimensionConverter::GetTenthsMM(const wxTextAttrDimension& dim) const
{
    if (dim.GetUnits() == wxTEXT_ATTR_UNITS_TENTHS_MM)
        return dim.GetValue();
    else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_PIXELS)
        return ConvertPixelsToTenthsMM(dim.GetValue());
    else
    {
        wxASSERT(false);
        return 0;
    }
}

// Partial equality test
bool wxTextAttrDimensions::EqPartial(const wxTextAttrDimensions& dims) const
{
    if (!m_left.EqPartial(dims.m_left))
        return false;

    if (!m_right.EqPartial(dims.m_right))
        return false;

    if (!m_top.EqPartial(dims.m_top))
        return false;

    if (!m_bottom.EqPartial(dims.m_bottom))
        return false;

    return true;
}

// Apply border to 'this', but not if the same as compareWith
bool wxTextAttrDimensions::Apply(const wxTextAttrDimensions& dims, const wxTextAttrDimensions* compareWith)
{
    m_left.Apply(dims.m_left, compareWith ? (& compareWith->m_left) : (const wxTextAttrDimension*) NULL);
    m_right.Apply(dims.m_right, compareWith ? (& compareWith->m_right): (const wxTextAttrDimension*) NULL);
    m_top.Apply(dims.m_top, compareWith ? (& compareWith->m_top): (const wxTextAttrDimension*) NULL);
    m_bottom.Apply(dims.m_bottom, compareWith ? (& compareWith->m_bottom): (const wxTextAttrDimension*) NULL);

    return true;
}

// Remove specified attributes from this object
bool wxTextAttrDimensions::RemoveStyle(const wxTextAttrDimensions& attr)
{
    if (attr.m_left.IsValid())
        m_left.Reset();
    if (attr.m_right.IsValid())
        m_right.Reset();
    if (attr.m_top.IsValid())
        m_top.Reset();
    if (attr.m_bottom.IsValid())
        m_bottom.Reset();

    return true;
}

// Collects the attributes that are common to a range of content, building up a note of
// which attributes are absent in some objects and which clash in some objects.
void wxTextAttrDimensions::CollectCommonAttributes(const wxTextAttrDimensions& attr, wxTextAttrDimensions& clashingAttr, wxTextAttrDimensions& absentAttr)
{
    m_left.CollectCommonAttributes(attr.m_left, clashingAttr.m_left, absentAttr.m_left);
    m_right.CollectCommonAttributes(attr.m_right, clashingAttr.m_right, absentAttr.m_right);
    m_top.CollectCommonAttributes(attr.m_top, clashingAttr.m_top, absentAttr.m_top);
    m_bottom.CollectCommonAttributes(attr.m_bottom, clashingAttr.m_bottom, absentAttr.m_bottom);
}

// Partial equality test
bool wxTextAttrSize::EqPartial(const wxTextAttrSize& size) const
{
    if (!m_width.EqPartial(size.m_width))
        return false;

    if (!m_height.EqPartial(size.m_height))
        return false;

    return true;
}

// Apply border to 'this', but not if the same as compareWith
bool wxTextAttrSize::Apply(const wxTextAttrSize& size, const wxTextAttrSize* compareWith)
{
    m_width.Apply(size.m_width, compareWith ? (& compareWith->m_width) : (const wxTextAttrDimension*) NULL);
    m_height.Apply(size.m_height, compareWith ? (& compareWith->m_height): (const wxTextAttrDimension*) NULL);

    return true;
}

// Remove specified attributes from this object
bool wxTextAttrSize::RemoveStyle(const wxTextAttrSize& attr)
{
    if (attr.m_width.IsValid())
        m_width.Reset();
    if (attr.m_height.IsValid())
        m_height.Reset();

    return true;
}

// Collects the attributes that are common to a range of content, building up a note of
// which attributes are absent in some objects and which clash in some objects.
void wxTextAttrSize::CollectCommonAttributes(const wxTextAttrSize& attr, wxTextAttrSize& clashingAttr, wxTextAttrSize& absentAttr)
{
    m_width.CollectCommonAttributes(attr.m_width, clashingAttr.m_width, absentAttr.m_width);
    m_height.CollectCommonAttributes(attr.m_height, clashingAttr.m_height, absentAttr.m_height);
}

// Collects the attributes that are common to a range of content, building up a note of
// which attributes are absent in some objects and which clash in some objects.
void wxTextAttrCollectCommonAttributes(wxTextAttr& currentStyle, const wxTextAttr& attr, wxTextAttr& clashingAttr, wxTextAttr& absentAttr)
{
    absentAttr.SetFlags(absentAttr.GetFlags() | (~attr.GetFlags() & wxTEXT_ATTR_ALL));
    absentAttr.SetTextEffectFlags(absentAttr.GetTextEffectFlags() | (~attr.GetTextEffectFlags() & 0xFFFF));

    long forbiddenFlags = clashingAttr.GetFlags()|absentAttr.GetFlags();

    if (attr.HasFont())
    {
        if (attr.HasFontSize() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_FONT_SIZE))
        {
            if (currentStyle.HasFontSize())
            {
                if (currentStyle.GetFontSize() != attr.GetFontSize())
                {
                    // Clash of attr - mark as such
                    clashingAttr.AddFlag(wxTEXT_ATTR_FONT_SIZE);
                    currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_SIZE);
                }
            }
            else
                currentStyle.SetFontSize(attr.GetFontSize());
        }

        if (attr.HasFontItalic() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_FONT_ITALIC))
        {
            if (currentStyle.HasFontItalic())
            {
                if (currentStyle.GetFontStyle() != attr.GetFontStyle())
                {
                    // Clash of attr - mark as such
                    clashingAttr.AddFlag(wxTEXT_ATTR_FONT_ITALIC);
                    currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_ITALIC);
                }
            }
            else
                currentStyle.SetFontStyle(attr.GetFontStyle());
        }

        if (attr.HasFontFamily() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_FONT_FAMILY))
        {
            if (currentStyle.HasFontFamily())
            {
                if (currentStyle.GetFontFamily() != attr.GetFontFamily())
                {
                    // Clash of attr - mark as such
                    clashingAttr.AddFlag(wxTEXT_ATTR_FONT_FAMILY);
                    currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_FAMILY);
                }
            }
            else
                currentStyle.SetFontFamily(attr.GetFontFamily());
        }

        if (attr.HasFontWeight() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_FONT_WEIGHT))
        {
            if (currentStyle.HasFontWeight())
            {
                if (currentStyle.GetFontWeight() != attr.GetFontWeight())
                {
                    // Clash of attr - mark as such
                    clashingAttr.AddFlag(wxTEXT_ATTR_FONT_WEIGHT);
                    currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_WEIGHT);
                }
            }
            else
                currentStyle.SetFontWeight(attr.GetFontWeight());
        }

        if (attr.HasFontFaceName() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_FONT_FACE))
        {
            if (currentStyle.HasFontFaceName())
            {
                wxString faceName1(currentStyle.GetFontFaceName());
                wxString faceName2(attr.GetFontFaceName());

                if (faceName1 != faceName2)
                {
                    // Clash of attr - mark as such
                    clashingAttr.AddFlag(wxTEXT_ATTR_FONT_FACE);
                    currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_FACE);
                }
            }
            else
                currentStyle.SetFontFaceName(attr.GetFontFaceName());
        }

        if (attr.HasFontUnderlined() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_FONT_UNDERLINE))
        {
            if (currentStyle.HasFontUnderlined())
            {
                if (currentStyle.GetFontUnderlined() != attr.GetFontUnderlined())
                {
                    // Clash of attr - mark as such
                    clashingAttr.AddFlag(wxTEXT_ATTR_FONT_UNDERLINE);
                    currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_UNDERLINE);
                }
            }
            else
                currentStyle.SetFontUnderlined(attr.GetFontUnderlined());
        }
    }

    if (attr.HasTextColour() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_TEXT_COLOUR))
    {
        if (currentStyle.HasTextColour())
        {
            if (currentStyle.GetTextColour() != attr.GetTextColour())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_TEXT_COLOUR);
                currentStyle.RemoveFlag(wxTEXT_ATTR_TEXT_COLOUR);
            }
        }
        else
            currentStyle.SetTextColour(attr.GetTextColour());
    }

    if (attr.HasBackgroundColour() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_BACKGROUND_COLOUR))
    {
        if (currentStyle.HasBackgroundColour())
        {
            if (currentStyle.GetBackgroundColour() != attr.GetBackgroundColour())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_BACKGROUND_COLOUR);
                currentStyle.RemoveFlag(wxTEXT_ATTR_BACKGROUND_COLOUR);
            }
        }
        else
            currentStyle.SetBackgroundColour(attr.GetBackgroundColour());
    }

    if (attr.HasAlignment() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_ALIGNMENT))
    {
        if (currentStyle.HasAlignment())
        {
            if (currentStyle.GetAlignment() != attr.GetAlignment())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_ALIGNMENT);
                currentStyle.RemoveFlag(wxTEXT_ATTR_ALIGNMENT);
            }
        }
        else
            currentStyle.SetAlignment(attr.GetAlignment());
    }

    if (attr.HasTabs() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_TABS))
    {
        if (currentStyle.HasTabs())
        {
            if (!wxRichTextTabsEq(currentStyle.GetTabs(), attr.GetTabs()))
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_TABS);
                currentStyle.RemoveFlag(wxTEXT_ATTR_TABS);
            }
        }
        else
            currentStyle.SetTabs(attr.GetTabs());
    }

    if (attr.HasLeftIndent() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_LEFT_INDENT))
    {
        if (currentStyle.HasLeftIndent())
        {
            if (currentStyle.GetLeftIndent() != attr.GetLeftIndent() || currentStyle.GetLeftSubIndent() != attr.GetLeftSubIndent())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_LEFT_INDENT);
                currentStyle.RemoveFlag(wxTEXT_ATTR_LEFT_INDENT);
            }
        }
        else
            currentStyle.SetLeftIndent(attr.GetLeftIndent(), attr.GetLeftSubIndent());
    }

    if (attr.HasRightIndent() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_RIGHT_INDENT))
    {
        if (currentStyle.HasRightIndent())
        {
            if (currentStyle.GetRightIndent() != attr.GetRightIndent())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_RIGHT_INDENT);
                currentStyle.RemoveFlag(wxTEXT_ATTR_RIGHT_INDENT);
            }
        }
        else
            currentStyle.SetRightIndent(attr.GetRightIndent());
    }

    if (attr.HasParagraphSpacingAfter() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_PARA_SPACING_AFTER))
    {
        if (currentStyle.HasParagraphSpacingAfter())
        {
            if (currentStyle.GetParagraphSpacingAfter() != attr.GetParagraphSpacingAfter())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_PARA_SPACING_AFTER);
                currentStyle.RemoveFlag(wxTEXT_ATTR_PARA_SPACING_AFTER);
            }
        }
        else
            currentStyle.SetParagraphSpacingAfter(attr.GetParagraphSpacingAfter());
    }

    if (attr.HasParagraphSpacingBefore() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_PARA_SPACING_BEFORE))
    {
        if (currentStyle.HasParagraphSpacingBefore())
        {
            if (currentStyle.GetParagraphSpacingBefore() != attr.GetParagraphSpacingBefore())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_PARA_SPACING_BEFORE);
                currentStyle.RemoveFlag(wxTEXT_ATTR_PARA_SPACING_BEFORE);
            }
        }
        else
            currentStyle.SetParagraphSpacingBefore(attr.GetParagraphSpacingBefore());
    }

    if (attr.HasLineSpacing() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_LINE_SPACING))
    {
        if (currentStyle.HasLineSpacing())
        {
            if (currentStyle.GetLineSpacing() != attr.GetLineSpacing())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_LINE_SPACING);
                currentStyle.RemoveFlag(wxTEXT_ATTR_LINE_SPACING);
            }
        }
        else
            currentStyle.SetLineSpacing(attr.GetLineSpacing());
    }

    if (attr.HasCharacterStyleName() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_CHARACTER_STYLE_NAME))
    {
        if (currentStyle.HasCharacterStyleName())
        {
            if (currentStyle.GetCharacterStyleName() != attr.GetCharacterStyleName())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_CHARACTER_STYLE_NAME);
                currentStyle.RemoveFlag(wxTEXT_ATTR_CHARACTER_STYLE_NAME);
            }
        }
        else
            currentStyle.SetCharacterStyleName(attr.GetCharacterStyleName());
    }

    if (attr.HasParagraphStyleName() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_PARAGRAPH_STYLE_NAME))
    {
        if (currentStyle.HasParagraphStyleName())
        {
            if (currentStyle.GetParagraphStyleName() != attr.GetParagraphStyleName())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_PARAGRAPH_STYLE_NAME);
                currentStyle.RemoveFlag(wxTEXT_ATTR_PARAGRAPH_STYLE_NAME);
            }
        }
        else
            currentStyle.SetParagraphStyleName(attr.GetParagraphStyleName());
    }

    if (attr.HasListStyleName() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_LIST_STYLE_NAME))
    {
        if (currentStyle.HasListStyleName())
        {
            if (currentStyle.GetListStyleName() != attr.GetListStyleName())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_LIST_STYLE_NAME);
                currentStyle.RemoveFlag(wxTEXT_ATTR_LIST_STYLE_NAME);
            }
        }
        else
            currentStyle.SetListStyleName(attr.GetListStyleName());
    }

    if (attr.HasBulletStyle() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_BULLET_STYLE))
    {
        if (currentStyle.HasBulletStyle())
        {
            if (currentStyle.GetBulletStyle() != attr.GetBulletStyle())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_BULLET_STYLE);
                currentStyle.RemoveFlag(wxTEXT_ATTR_BULLET_STYLE);
            }
        }
        else
            currentStyle.SetBulletStyle(attr.GetBulletStyle());
    }

    if (attr.HasBulletNumber() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_BULLET_NUMBER))
    {
        if (currentStyle.HasBulletNumber())
        {
            if (currentStyle.GetBulletNumber() != attr.GetBulletNumber())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_BULLET_NUMBER);
                currentStyle.RemoveFlag(wxTEXT_ATTR_BULLET_NUMBER);
            }
        }
        else
            currentStyle.SetBulletNumber(attr.GetBulletNumber());
    }

    if (attr.HasBulletText() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_BULLET_TEXT))
    {
        if (currentStyle.HasBulletText())
        {
            if (currentStyle.GetBulletText() != attr.GetBulletText())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_BULLET_TEXT);
                currentStyle.RemoveFlag(wxTEXT_ATTR_BULLET_TEXT);
            }
        }
        else
        {
            currentStyle.SetBulletText(attr.GetBulletText());
            currentStyle.SetBulletFont(attr.GetBulletFont());
        }
    }

    if (attr.HasBulletName() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_BULLET_NAME))
    {
        if (currentStyle.HasBulletName())
        {
            if (currentStyle.GetBulletName() != attr.GetBulletName())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_BULLET_NAME);
                currentStyle.RemoveFlag(wxTEXT_ATTR_BULLET_NAME);
            }
        }
        else
        {
            currentStyle.SetBulletName(attr.GetBulletName());
        }
    }

    if (attr.HasURL() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_URL))
    {
        if (currentStyle.HasURL())
        {
            if (currentStyle.GetURL() != attr.GetURL())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_URL);
                currentStyle.RemoveFlag(wxTEXT_ATTR_URL);
            }
        }
        else
        {
            currentStyle.SetURL(attr.GetURL());
        }
    }

    if (attr.HasTextEffects() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_EFFECTS))
    {
        if (currentStyle.HasTextEffects())
        {
            // We need to find the bits in the new attr that are different:
            // just look at those bits that are specified by the new attr.

            // We need to remove the bits and flags that are not common between current attr
            // and new attr. In so doing we need to take account of the styles absent from one or more of the
            // previous styles.

            int currentRelevantTextEffects = currentStyle.GetTextEffects() & attr.GetTextEffectFlags();
            int newRelevantTextEffects = attr.GetTextEffects() & attr.GetTextEffectFlags();

            if (currentRelevantTextEffects != newRelevantTextEffects)
            {
                // Find the text effects that were different, using XOR
                int differentEffects = currentRelevantTextEffects ^ newRelevantTextEffects;

                // Clash of attr - mark as such
                clashingAttr.SetTextEffectFlags(clashingAttr.GetTextEffectFlags() | differentEffects);
                currentStyle.SetTextEffectFlags(currentStyle.GetTextEffectFlags() & ~differentEffects);
            }
        }
        else
        {
            currentStyle.SetTextEffects(attr.GetTextEffects());
            currentStyle.SetTextEffectFlags(attr.GetTextEffectFlags());
        }

        // Mask out the flags and values that cannot be common because they were absent in one or more objecrs
        // that we've looked at so far
        currentStyle.SetTextEffects(currentStyle.GetTextEffects() & ~absentAttr.GetTextEffectFlags());
        currentStyle.SetTextEffectFlags(currentStyle.GetTextEffectFlags() & ~absentAttr.GetTextEffectFlags());

        if (currentStyle.GetTextEffectFlags() == 0)
            currentStyle.RemoveFlag(wxTEXT_ATTR_EFFECTS);
    }

    if (attr.HasOutlineLevel() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_OUTLINE_LEVEL))
    {
        if (currentStyle.HasOutlineLevel())
        {
            if (currentStyle.GetOutlineLevel() != attr.GetOutlineLevel())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_OUTLINE_LEVEL);
                currentStyle.RemoveFlag(wxTEXT_ATTR_OUTLINE_LEVEL);
            }
        }
        else
            currentStyle.SetOutlineLevel(attr.GetOutlineLevel());
    }
}

WX_DEFINE_OBJARRAY(wxRichTextVariantArray);

IMPLEMENT_DYNAMIC_CLASS(wxRichTextProperties, wxObject)

bool wxRichTextProperties::operator==(const wxRichTextProperties& props) const
{
    if (m_properties.GetCount() != props.GetCount())
        return false;

    size_t i;
    for (i = 0; i < m_properties.GetCount(); i++)
    {
        const wxVariant& var1 = m_properties[i];
        int idx = props.Find(var1.GetName());
        if (idx == -1)
            return false;
        const wxVariant& var2 = props.m_properties[idx];
        if (!(var1 == var2))
            return false;
    }

    return true;
}

wxArrayString wxRichTextProperties::GetPropertyNames() const
{
    wxArrayString arr;
    size_t i;
    for (i = 0; i < m_properties.GetCount(); i++)
    {
        arr.Add(m_properties[i].GetName());
    }
    return arr;
}

int wxRichTextProperties::Find(const wxString& name) const
{
    size_t i;
    for (i = 0; i < m_properties.GetCount(); i++)
    {
        if (m_properties[i].GetName() == name)
            return (int) i;
    }
    return -1;
}

wxVariant* wxRichTextProperties::FindOrCreateProperty(const wxString& name)
{
    int idx = Find(name);
    if (idx == wxNOT_FOUND)
        SetProperty(name, wxString());
    idx = Find(name);
    if (idx != wxNOT_FOUND)
    {
        return & (*this)[idx];
    }
    else
        return NULL;
}

const wxVariant& wxRichTextProperties::GetProperty(const wxString& name) const
{
    static const wxVariant nullVariant;
    int idx = Find(name);
    if (idx != -1)
        return m_properties[idx];
    else
        return nullVariant;
}

wxString wxRichTextProperties::GetPropertyString(const wxString& name) const
{
    return GetProperty(name).GetString();
}

long wxRichTextProperties::GetPropertyLong(const wxString& name) const
{
    return GetProperty(name).GetLong();
}

bool wxRichTextProperties::GetPropertyBool(const wxString& name) const
{
    return GetProperty(name).GetBool();
}

double wxRichTextProperties::GetPropertyDouble(const wxString& name) const
{
    return GetProperty(name).GetDouble();
}

void wxRichTextProperties::SetProperty(const wxVariant& variant)
{
    wxASSERT(!variant.GetName().IsEmpty());

    int idx = Find(variant.GetName());

    if (idx == -1)
        m_properties.Add(variant);
    else
        m_properties[idx] = variant;
}

void wxRichTextProperties::SetProperty(const wxString& name, const wxVariant& variant)
{
    int idx = Find(name);
    wxVariant var(variant);
    var.SetName(name);

    if (idx == -1)
        m_properties.Add(var);
    else
        m_properties[idx] = var;
}

void wxRichTextProperties::SetProperty(const wxString& name, const wxString& value)
{
    SetProperty(name, wxVariant(value, name));
}

void wxRichTextProperties::SetProperty(const wxString& name, long value)
{
    SetProperty(name, wxVariant(value, name));
}

void wxRichTextProperties::SetProperty(const wxString& name, double value)
{
    SetProperty(name, wxVariant(value, name));
}

void wxRichTextProperties::SetProperty(const wxString& name, bool value)
{
    SetProperty(name, wxVariant(value, name));
}

wxRichTextObject* wxRichTextObjectAddress::GetObject(wxRichTextParagraphLayoutBox* topLevelContainer) const
{
    if (m_address.GetCount() == 0)
        return topLevelContainer;

    wxRichTextCompositeObject* p = topLevelContainer;
    size_t i = 0;
    while (p && i < m_address.GetCount())
    {
        int pos = m_address[i];
        wxASSERT(pos >= 0 && pos < (int) p->GetChildren().GetCount());
        if (pos < 0 || pos >= (int) p->GetChildren().GetCount())
            return NULL;

        wxRichTextObject* p1 = p->GetChild(pos);
        if (i == (m_address.GetCount()-1))
            return p1;

        p = wxDynamicCast(p1, wxRichTextCompositeObject);
        i ++;
    }
    return NULL;
}

bool wxRichTextObjectAddress::Create(wxRichTextParagraphLayoutBox* topLevelContainer, wxRichTextObject* obj)
{
    m_address.Clear();

    if (topLevelContainer == obj)
        return true;

    wxRichTextObject* o = obj;
    while (o)
    {
        wxRichTextCompositeObject* p = wxDynamicCast(o->GetParent(), wxRichTextCompositeObject);
        if (!p)
            return false;

        int pos = p->GetChildren().IndexOf(o);
        if (pos == -1)
            return false;

        m_address.Insert(pos, 0);

        if (p == topLevelContainer)
            return true;

        o = p;
    }
    return false;
}

// Equality test
bool wxRichTextSelection::operator==(const wxRichTextSelection& sel) const
{
    if (m_container != sel.m_container)
        return false;
    if (m_ranges.GetCount() != sel.m_ranges.GetCount())
        return false;
    size_t i;
    for (i = 0; i < m_ranges.GetCount(); i++)
        if (!(m_ranges[i] == sel.m_ranges[i]))
            return false;
    return true;
}

// Get the selections appropriate to the specified object, if any; returns wxRICHTEXT_NO_SELECTION if none
// or none at the level of the object's container.
wxRichTextRangeArray wxRichTextSelection::GetSelectionForObject(wxRichTextObject* obj) const
{
    if (IsValid())
    {
        wxRichTextParagraphLayoutBox* container = obj->GetParentContainer();

        if (container == m_container)
            return m_ranges;

        container = obj->GetContainer();
        while (container)
        {
            if (container->GetParent())
            {
                // If we found that our object's container is within the range of
                // a selection higher up, then assume the whole original object
                // is also selected.
                wxRichTextParagraphLayoutBox* parentContainer = container->GetParentContainer();
                if (parentContainer == m_container)
                {
                    if (WithinSelection(container->GetRange().GetStart(), m_ranges))
                    {
                        wxRichTextRangeArray ranges;
                        ranges.Add(obj->GetRange());
                        return ranges;
                    }
                }

                container = parentContainer;
            }
            else
            {
                container = NULL;
                break;
            }
        }
    }
    return wxRichTextRangeArray();
}

// Is the given position within the selection?
bool wxRichTextSelection::WithinSelection(long pos, wxRichTextObject* obj) const
{
    if (!IsValid())
        return false;
    else
    {
        wxRichTextRangeArray selectionRanges = GetSelectionForObject(obj);
        return WithinSelection(pos, selectionRanges);
    }
}

// Is the given position within the selection range?
bool wxRichTextSelection::WithinSelection(long pos, const wxRichTextRangeArray& ranges)
{
    size_t i;
    for (i = 0; i < ranges.GetCount(); i++)
    {
        const wxRichTextRange& range = ranges[i];
        if (pos >= range.GetStart() && pos <= range.GetEnd())
            return true;
    }
    return false;
}

// Is the given range completely within the selection range?
bool wxRichTextSelection::WithinSelection(const wxRichTextRange& range, const wxRichTextRangeArray& ranges)
{
    size_t i;
    for (i = 0; i < ranges.GetCount(); i++)
    {
        const wxRichTextRange& eachRange = ranges[i];
        if (range.IsWithin(eachRange))
            return true;
    }
    return false;
}


#endif
    // wxUSE_RICHTEXT
