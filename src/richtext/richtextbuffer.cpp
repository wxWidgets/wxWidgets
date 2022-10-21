/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextbuffer.cpp
// Purpose:     Buffer for wxRichTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     2005-09-30
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


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
#include "wx/math.h"

#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"
#include "wx/richtext/richtextimagedlg.h"
#include "wx/richtext/richtextsizepage.h"
#include "wx/richtext/richtextxml.h"
#include "wx/richtext/bitmaps/image_placeholder24x24.xpm"

#include "wx/listimpl.cpp"
#include "wx/arrimpl.cpp"

WX_DEFINE_LIST(wxRichTextObjectList)

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
    void Draw(wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style);

    // HitTest the floats
    int HitTest(wxDC& dc, wxRichTextDrawingContext& context, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags);

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

    static void DrawFloat(const wxRichTextFloatRectMapArray& array, wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style);

    static int HitTestFloat(const wxRichTextFloatRectMapArray& array, wxDC& dc, wxRichTextDrawingContext& context, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags);

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
 * The argument point is the Y coordinate, and this function
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
    , m_availableRect(rect)
{
    m_para = nullptr;
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
        // Our object will fit before this object
        if (array[i]->startY - last >= height)
        {
            // If we are fitting after another object, add a pixel since last
            // is the bottom of another object.
            if (last != start)
                last ++;
            return last;
        }
        last = array[i]->endY;
        i++;
    }

    // If we are fitting after another object, add a pixel since last
    // is the bottom of another object.
    if (last != start)
        last ++;
    return last;
}

int wxRichTextFloatCollector::GetFitPosition(int direction, int start, int height) const
{
    if (direction == wxTEXT_BOX_ATTR_FLOAT_LEFT)
        return GetFitPosition(m_left, start, height);
    else if (direction == wxTEXT_BOX_ATTR_FLOAT_RIGHT)
        return GetFitPosition(m_right, start, height);
    else
    {
        wxFAIL_MSG("Never should be here");
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
            wxFAIL_MSG("Unrecognised float attribute.");
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

void wxRichTextFloatCollector::DrawFloat(const wxRichTextFloatRectMapArray& array, wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& WXUNUSED(range), const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
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
        if (obj->IsTopLevel())
            r = obj->GetOwnRange();
        obj->Draw(dc, context, r, selection, wxRect(obj->GetPosition(), obj->GetCachedSize()), descent, style);
        i++;
    }
}

void wxRichTextFloatCollector::Draw(wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    if (m_left.GetCount() > 0)
        DrawFloat(m_left, dc, context, range, selection, rect, descent, style);
    if (m_right.GetCount() > 0)
        DrawFloat(m_right, dc, context, range, selection, rect, descent, style);
}

int wxRichTextFloatCollector::HitTestFloat(const wxRichTextFloatRectMapArray& array, wxDC& dc, wxRichTextDrawingContext& context, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int WXUNUSED(flags))
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
        if (array[i]->anchor->IsTopLevel())
        {
            int result = array[i]->anchor->HitTest(dc, context, pt, textPosition, obj, contextObj, 0);
            if (result != wxRICHTEXT_HITTEST_NONE)
            {
                return result;
            }
        }

        textPosition = array[i]->anchor->GetRange().GetStart();
        * obj = array[i]->anchor;
        * contextObj = array[i]->anchor->GetParentContainer();
        if (pt.x > (pt.x + pt.x + size.x) / 2)
            return wxRICHTEXT_HITTEST_BEFORE;
        else
            return wxRICHTEXT_HITTEST_AFTER;
    }

    return wxRICHTEXT_HITTEST_NONE;
}

int wxRichTextFloatCollector::HitTest(wxDC& dc, wxRichTextDrawingContext& context, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags)
{
    int ret = HitTestFloat(m_left, dc, context, pt, textPosition, obj, contextObj, flags);
    if (ret == wxRICHTEXT_HITTEST_NONE)
    {
        ret = HitTestFloat(m_right, dc, context, pt, textPosition, obj, contextObj, flags);
    }
    return ret;
}

// Helpers for efficiency
inline void wxCheckSetFont(wxDC& dc, const wxFont& font)
{
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

wxIMPLEMENT_CLASS(wxRichTextObject, wxObject);

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
    return nullptr;
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
wxRect wxRichTextObject::GetAvailableContentArea(wxDC& dc, wxRichTextDrawingContext& context, const wxRect& outerRect) const
{
    wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
    marginRect = outerRect;
    wxRichTextAttr attr(GetAttributes());
    const_cast<wxRichTextObject*>(this)->AdjustAttributes(attr, context);
    GetBoxRects(dc, GetBuffer(), attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);
    return contentRect;
}

// Invalidate the buffer. With no argument, invalidates whole buffer.
void wxRichTextObject::Invalidate(const wxRichTextRange& invalidRange)
{
    if (invalidRange != wxRICHTEXT_NONE)
    {
        // If this is a floating object, size may not be recalculated
        // after floats have been collected in an early stage of Layout.
        // So avoid resetting the cache for floating objects during layout.
        if (!IsFloating() || !wxRichTextBuffer::GetFloatingLayoutMode())
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
        scale = GetBuffer()->GetScale() / GetBuffer()->GetDimensionScale();
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

    int pixelsInt = int(pixels + 0.5);

    // If the result is very small, make it at least one pixel in size.
    if (pixelsInt == 0 && units > 0)
        pixelsInt = 1;

    return pixelsInt;
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
bool wxRichTextObject::DrawBoxAttributes(wxDC& dc, wxRichTextBuffer* buffer, const wxRichTextAttr& attr, const wxRect& boxRect, int flags, wxRichTextObject* obj)
{
    // Assume boxRect is the area around the content
    wxRect marginRect = boxRect;
    wxRect contentRect, borderRect, paddingRect, outlineRect;

    GetBoxRects(dc, buffer, attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);

    if (attr.GetTextBoxAttr().GetShadow().IsValid())
    {
        wxTextAttrDimensionConverter converter(dc, buffer ? buffer->GetScale() : 1.0);
        wxColour shadowColour;
        if (attr.GetTextBoxAttr().GetShadow().HasColour())
            shadowColour = attr.GetTextBoxAttr().GetShadow().GetColour();
        else
            shadowColour = *wxLIGHT_GREY;
        if (attr.GetTextBoxAttr().GetShadow().GetOpacity().IsValid())
        {
            // Let's pretend our background is always white. Calculate a colour value
            // from this and the opacity.
            double p = attr.GetTextBoxAttr().GetShadow().GetOpacity().GetValue() / 100.0;
            shadowColour.Set(wxRound((1.0 - p)*255 + p*shadowColour.Red()),
                             wxRound((1.0 - p)*255 + p*shadowColour.Green()),
                             wxRound((1.0 - p)*255 + p*shadowColour.Blue()));
        }
        wxRect shadowRect = borderRect;
        if (attr.GetTextBoxAttr().GetShadow().GetOffsetX().IsValid())
        {
            int pxX = converter.GetPixels(attr.GetTextBoxAttr().GetShadow().GetOffsetX());
            shadowRect.x += pxX;
        }
        if (attr.GetTextBoxAttr().GetShadow().GetOffsetY().IsValid())
        {
            int pxY = converter.GetPixels(attr.GetTextBoxAttr().GetShadow().GetOffsetY());
            shadowRect.y += pxY;
        }
        if (attr.GetTextBoxAttr().GetShadow().GetSpread().IsValid())
        {
            int pxSpread = converter.GetPixels(attr.GetTextBoxAttr().GetShadow().GetSpread());
            shadowRect.x -= pxSpread;
            shadowRect.y -= pxSpread;
            shadowRect.width += 2*pxSpread;
            shadowRect.height += 2*pxSpread;
        }
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(shadowColour));
        if (attr.GetTextBoxAttr().HasCornerRadius() && attr.GetTextBoxAttr().GetCornerRadius().GetValue() > 0)
        {
            wxTextAttrDimensionConverter radConverter(dc, buffer ? buffer->GetScale() : 1.0);
            int cornerRadius = radConverter.GetPixels(attr.GetTextBoxAttr().GetCornerRadius());
            if (cornerRadius > 0)
            {
                dc.DrawRoundedRectangle(shadowRect, cornerRadius);
            }
            else
                dc.DrawRectangle(shadowRect);
        }
        else
            dc.DrawRectangle(shadowRect);

        // If there's no box colour, draw over the shadow in the nearest available colour
        if (!attr.HasBackgroundColour())
        {
            wxColour bgColour;
            if (obj)
            {
                wxRichTextCompositeObject* composite = obj->GetParentContainer();
                if (composite && composite->GetAttributes().HasBackgroundColour())
                    bgColour = composite->GetAttributes().GetBackgroundColour();
            }
            if (!bgColour.IsOk() && buffer)
                bgColour = buffer->GetAttributes().GetBackgroundColour();
            if (!bgColour.IsOk())
                bgColour = *wxWHITE;
            dc.SetBrush(wxBrush(bgColour));
            dc.DrawRectangle(borderRect);
        }
    }

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
        pen.SetJoin(wxJOIN_MITER);
        wxBrush brush(colour);

        dc.SetPen(pen);
        dc.SetBrush(brush);

        if (attr.GetTextBoxAttr().HasCornerRadius() && attr.GetTextBoxAttr().GetCornerRadius().GetValue() > 0)
        {
            wxTextAttrDimensionConverter converter(dc, buffer ? buffer->GetScale() : 1.0);
            int cornerRadius = converter.GetPixels(attr.GetTextBoxAttr().GetCornerRadius());
            if (cornerRadius > 0)
            {
                dc.DrawRoundedRectangle(borderRect, cornerRadius);
            }
        }
        else
            dc.DrawRectangle(borderRect);
    }

    if (flags & wxRICHTEXT_DRAW_GUIDELINES)
    {
        wxRichTextAttr editBorderAttr;
        // TODO: make guideline colour configurable
        editBorderAttr.GetTextBoxAttr().GetBorder().SetColour(*wxLIGHT_GREY);
        editBorderAttr.GetTextBoxAttr().GetBorder().SetWidth(1, wxTEXT_ATTR_UNITS_PIXELS);
        editBorderAttr.GetTextBoxAttr().GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);

        if (obj)
        {
            wxRichTextCell* cell = wxDynamicCast(obj, wxRichTextCell);
            if (cell)
            {
                // This ensures that thin lines drawn by adjacent cells (left and above)
                // don't get overwritten by the guidelines.
                editBorderAttr.GetTextBoxAttr().GetBorder().GetLeft().Reset();
                editBorderAttr.GetTextBoxAttr().GetBorder().GetTop().Reset();
            }
        }

        DrawBorder(dc, buffer, attr, editBorderAttr.GetTextBoxAttr().GetBorder(), borderRect, flags);
    }

    if (attr.GetTextBoxAttr().GetBorder().IsValid())
        DrawBorder(dc, buffer, attr, attr.GetTextBoxAttr().GetBorder(), borderRect);

    if (attr.GetTextBoxAttr().GetOutline().IsValid())
        DrawBorder(dc, buffer, attr, attr.GetTextBoxAttr().GetOutline(), outlineRect);

    return true;
}

// Draw a border
bool wxRichTextObject::DrawBorder(wxDC& dc, wxRichTextBuffer* buffer, const wxRichTextAttr& attr, const wxTextAttrBorders& borders, const wxRect& rect, int WXUNUSED(flags))
{
    int borderLeft = 0, borderRight = 0, borderTop = 0, borderBottom = 0;
    wxTextAttrDimensionConverter converter(dc, buffer ? buffer->GetScale() : 1.0);

    // If we have a corner radius, assume all borders are the same, and draw a rounded outline.
    if (attr.GetTextBoxAttr().HasCornerRadius() && borders.GetLeft().IsValid() && (borders.GetLeft().GetWidth().GetValue() > 0) && borders.GetLeft().GetStyle() != wxTEXT_BOX_ATTR_BORDER_NONE)
    {
        int cornerRadius = converter.GetPixels(attr.GetTextBoxAttr().GetCornerRadius());
        if (cornerRadius > 0)
        {
            borderLeft = converter.GetPixels(borders.GetLeft().GetWidth());

            // Compensate for border thickness, since the rectangle borders are centred on the rect
            wxRect rect2(rect);
            if (borderLeft > 1)
            {
                int inc = (int) ((double(borderLeft) / 2.0) + 0.5);
                rect2.x += inc;
                rect2.y += inc;
                rect2.width -= (2*inc - 1);
                rect2.height -= (2*inc - 1);
            }

            wxColour col(borders.GetLeft().GetColour());
            wxPenStyle penStyle = wxPENSTYLE_SOLID;
            if (borders.GetLeft().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DOTTED)
                penStyle = wxPENSTYLE_DOT;
            else if (borders.GetLeft().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DASHED)
                penStyle = wxPENSTYLE_LONG_DASH;
            wxPen pen(col, borderLeft, penStyle);
            dc.SetPen(pen);
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawRoundedRectangle(rect2, cornerRadius);
            return true;
        }
    }

    // Don't do this, since it can mess up cell drawing in tables when
    // there are inconsistencies between rectangle and line drawing.
#if 0
    // Draw the border in one go if all the borders are the same
    if (borders.GetLeft().IsValid() && (borders.GetLeft().GetWidth().GetValue() > 0) && borders.GetTop().IsValid() && borders.GetRight().IsValid() &&borders.GetBottom().IsValid() &&
        (borders.GetLeft() == borders.GetTop()) && (borders.GetLeft() == borders.GetRight()) && (borders.GetLeft() == borders.GetBottom()))
    {
        borderLeft = converter.GetPixels(borders.GetLeft().GetWidth());

        // Compensate for border thickness, since the rectangle borders are centred on the rect
        wxRect rect2(rect);
        if (borderLeft > 1)
        {
            int inc = (int) ((double(borderLeft) / 2.0) + 0.5);
            rect2.x += inc;
            rect2.y += inc;
            rect2.width -= (2*inc - 1);
            rect2.height -= (2*inc - 1);
        }

        wxColour col(borders.GetLeft().GetColour());
        wxPenStyle penStyle = wxPENSTYLE_SOLID;
        if (borders.GetLeft().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DOTTED)
            penStyle = wxPENSTYLE_DOT;
        else if (borders.GetLeft().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DASHED)
            penStyle = wxPENSTYLE_LONG_DASH;
        wxPen pen(col, borderLeft, penStyle);
        pen.SetJoin(wxJOIN_MITER);
        dc.SetPen(pen);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(rect);
        return true;
    }
#endif

    if (borders.GetLeft().IsValid() && (borders.GetLeft().GetWidth().GetValue() > 0) && (borders.GetLeft().GetStyle() != wxTEXT_BOX_ATTR_BORDER_NONE))
    {
        borderLeft = converter.GetPixels(borders.GetLeft().GetWidth());
        wxColour col(borders.GetLeft().GetColour());
        wxPenStyle penStyle = wxPENSTYLE_SOLID;
        if (borders.GetLeft().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DOTTED)
            penStyle = wxPENSTYLE_DOT;
        else if (borders.GetLeft().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DASHED)
            penStyle = wxPENSTYLE_LONG_DASH;

        if (borderLeft == 1 || penStyle != wxPENSTYLE_SOLID)
        {
            wxPen pen(col, borderLeft, penStyle);
            dc.SetPen(pen);

            // Note that the last point is not drawn, at least on GTK+ and Windows.
            // On Mac, we must compensate.
            int inc = 0;
#ifdef __WXMAC__
            inc = 1;
#endif
            dc.DrawLine(rect.x, rect.y, rect.x, rect.y + rect.height - inc);
        }
        else
        {
            wxPen pen(col);
            pen.SetJoin(wxJOIN_MITER);
            wxBrush brush(col);
            dc.SetPen(pen);
            dc.SetBrush(brush);
            dc.DrawRectangle(rect.x, rect.y, borderLeft, rect.height);
        }
    }

    if (borders.GetRight().IsValid() && (borders.GetRight().GetWidth().GetValue() > 0) && (borders.GetRight().GetStyle() != wxTEXT_BOX_ATTR_BORDER_NONE))
    {
        borderRight = converter.GetPixels(borders.GetRight().GetWidth());

        wxColour col(borders.GetRight().GetColour());
        wxPenStyle penStyle = wxPENSTYLE_SOLID;
        if (borders.GetRight().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DOTTED)
            penStyle = wxPENSTYLE_DOT;
        else if (borders.GetRight().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DASHED)
            penStyle = wxPENSTYLE_LONG_DASH;

        if (borderRight == 1 || penStyle != wxPENSTYLE_SOLID)
        {
            wxPen pen(col, borderRight, penStyle);
            dc.SetPen(pen);
            // See note above.
            int inc = 0;
#ifdef __WXMAC__
            inc = 1;
#endif
            dc.DrawLine(rect.x + rect.width - 1, rect.y, rect.x + rect.width - 1, rect.y + rect.height - inc);
        }
        else
        {
            wxPen pen(col);
            pen.SetJoin(wxJOIN_MITER);
            wxBrush brush(col);
            dc.SetPen(pen);
            dc.SetBrush(brush);
            dc.DrawRectangle(rect.x + rect.width - borderRight, rect.y, borderRight, rect.height);
        }
    }

    if (borders.GetTop().IsValid() && (borders.GetTop().GetWidth().GetValue() > 0) && (borders.GetTop().GetStyle() != wxTEXT_BOX_ATTR_BORDER_NONE))
    {
        borderTop = converter.GetPixels(borders.GetTop().GetWidth());

        wxColour col(borders.GetTop().GetColour());
        wxPenStyle penStyle = wxPENSTYLE_SOLID;
        if (borders.GetTop().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DOTTED)
            penStyle = wxPENSTYLE_DOT;
        else if (borders.GetTop().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DASHED)
            penStyle = wxPENSTYLE_LONG_DASH;

        if (borderTop == 1 || penStyle != wxPENSTYLE_SOLID)
        {
            wxPen pen(col, borderTop, penStyle);
            dc.SetPen(pen);
            // See note above.
            int inc = 0;
#ifdef __WXMAC__
            inc = 1;
#endif
            dc.DrawLine(rect.x, rect.y, rect.x + rect.width - inc, rect.y);
        }
        else
        {
            wxPen pen(col);
            pen.SetJoin(wxJOIN_MITER);
            wxBrush brush(col);
            dc.SetPen(pen);
            dc.SetBrush(brush);
            dc.DrawRectangle(rect.x, rect.y, rect.width, borderTop);
        }
    }

    if (borders.GetBottom().IsValid() && (borders.GetBottom().GetWidth().GetValue() > 0) && (borders.GetBottom().GetStyle() != wxTEXT_BOX_ATTR_BORDER_NONE))
    {
        borderBottom = converter.GetPixels(borders.GetBottom().GetWidth());
        wxColour col(borders.GetBottom().GetColour());
        wxPenStyle penStyle = wxPENSTYLE_SOLID;
        if (borders.GetBottom().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DOTTED)
            penStyle = wxPENSTYLE_DOT;
        else if (borders.GetBottom().GetStyle() == wxTEXT_BOX_ATTR_BORDER_DASHED)
            penStyle = wxPENSTYLE_LONG_DASH;

        if (borderBottom == 1 || penStyle != wxPENSTYLE_SOLID)
        {
            wxPen pen(col, borderBottom, penStyle);
            dc.SetPen(pen);
            // See note above.
            int inc = 0;
#ifdef __WXMAC__
            inc = 1;
#endif
            dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width - inc, rect.y + rect.height - 1);
        }
        else
        {
            wxPen pen(col);
            pen.SetJoin(wxJOIN_MITER);
            wxBrush brush(col);
            dc.SetPen(pen);
            dc.SetBrush(brush);
            dc.DrawRectangle(rect.x, rect.y + rect.height - borderBottom, rect.width, borderBottom);
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
    if (attr.GetTextBoxAttr().GetMargins().GetBottom().IsValid())
        marginBottom = converter.GetPixels(attr.GetTextBoxAttr().GetMargins().GetBottom());

    if (attr.GetTextBoxAttr().GetBorder().GetLeft().GetWidth().IsValid())
        borderLeft = converter.GetPixels(attr.GetTextBoxAttr().GetBorder().GetLeft().GetWidth());
    if (attr.GetTextBoxAttr().GetBorder().GetRight().GetWidth().IsValid())
        borderRight = converter.GetPixels(attr.GetTextBoxAttr().GetBorder().GetRight().GetWidth());
    if (attr.GetTextBoxAttr().GetBorder().GetTop().GetWidth().IsValid())
        borderTop = converter.GetPixels(attr.GetTextBoxAttr().GetBorder().GetTop().GetWidth());
    if (attr.GetTextBoxAttr().GetBorder().GetBottom().GetWidth().IsValid())
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
// availableContainerSpace might be a parent that the cell has to compute its width relative to.
// E.g. a cell that's 50% of its parent.
wxRect wxRichTextObject::AdjustAvailableSpace(wxDC& dc, wxRichTextBuffer* buffer, const wxRichTextAttr& WXUNUSED(parentAttr), const wxRichTextAttr& childAttr, const wxRect& availableParentSpace, const wxRect& availableContainerSpace)
{
    wxRect rect = availableParentSpace;
    double scale = 1.0;
    if (buffer)
        scale = buffer->GetScale();

    wxTextAttrDimensionConverter converter(dc, scale, availableContainerSpace.GetSize());

    if (childAttr.GetTextBoxAttr().GetWidth().IsValid())
        rect.width = converter.GetPixels(childAttr.GetTextBoxAttr().GetWidth(), wxHORIZONTAL);

    if (childAttr.GetTextBoxAttr().GetHeight().IsValid())
        rect.height = converter.GetPixels(childAttr.GetTextBoxAttr().GetHeight(), wxVERTICAL);

    // Can specify either left or right for the position (we're assuming we can't
    // set the left and right edges to effectively set the size. Would we want to do that?)
    if (childAttr.GetTextBoxAttr().GetPosition().GetLeft().IsValid())
    {
        rect.x = rect.x + converter.GetPixels(childAttr.GetTextBoxAttr().GetPosition().GetLeft(), wxHORIZONTAL);
    }
    else if (childAttr.GetTextBoxAttr().GetPosition().GetRight().IsValid())
    {
        int x = converter.GetPixels(childAttr.GetTextBoxAttr().GetPosition().GetRight(), wxHORIZONTAL);
        if (childAttr.GetTextBoxAttr().GetPosition().GetRight().GetPosition() == wxTEXT_BOX_ATTR_POSITION_RELATIVE)
            rect.x = availableContainerSpace.x + availableContainerSpace.width - rect.width;
        else
            rect.x += x;
    }

    if (childAttr.GetTextBoxAttr().GetPosition().GetTop().IsValid())
    {
        rect.y = rect.y + converter.GetPixels(childAttr.GetTextBoxAttr().GetPosition().GetTop(), wxVERTICAL);
    }
    else if (childAttr.GetTextBoxAttr().GetPosition().GetBottom().IsValid())
    {
        int y = converter.GetPixels(childAttr.GetTextBoxAttr().GetPosition().GetBottom(), wxVERTICAL);
        if (childAttr.GetTextBoxAttr().GetPosition().GetBottom().GetPosition() == wxTEXT_BOX_ATTR_POSITION_RELATIVE)
            rect.y = availableContainerSpace.y + availableContainerSpace.height - rect.height;
        else
            rect.y += y;
    }

    if (rect.GetWidth() > availableParentSpace.GetWidth())
        rect.SetWidth(availableParentSpace.GetWidth());

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
    while (obj && !wxDynamicCast(obj, wxRichTextBuffer))
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
int wxRichTextObject::HitTest(wxDC& WXUNUSED(dc), wxRichTextDrawingContext& WXUNUSED(context), const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int WXUNUSED(flags))
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
bool wxRichTextObject::LayoutToBestSize(wxDC& dc, wxRichTextDrawingContext& context, wxRichTextBuffer* buffer,
    const wxRichTextAttr& parentAttr, const wxRichTextAttr& attr,
    const wxRect& availableParentSpace, const wxRect& availableContainerSpace,
    int style)
{
    wxRect availableChildRect = AdjustAvailableSpace(dc, buffer, parentAttr, attr, availableParentSpace, availableContainerSpace);
#if 0
    wxRect originalAvailableRect = availableChildRect;
#endif
    Layout(dc, context, availableChildRect, availableContainerSpace, style);

    wxSize maxSize = GetMaxSize();

    // Don't ignore if maxSize.x is zero, since we need to redo the paragraph's lines
    // on this basis
    if (!attr.GetTextBoxAttr().GetWidth().IsValid() && maxSize.x < availableChildRect.width)
    {
        if (!attr.HasAlignment() || attr.GetAlignment() == wxTEXT_ALIGNMENT_LEFT)
        {
            // Redo the layout with a fixed, minimum size this time.
            Invalidate(wxRICHTEXT_ALL);
            wxRichTextAttr newAttr(attr);
            newAttr.GetTextBoxAttr().GetWidth().SetValue(maxSize.x, wxTEXT_ATTR_UNITS_PIXELS);
            newAttr.GetTextBoxAttr().GetWidth().SetPosition(wxTEXT_BOX_ATTR_POSITION_ABSOLUTE);

            availableChildRect = AdjustAvailableSpace(dc, buffer, parentAttr, newAttr, availableParentSpace, availableContainerSpace);

            Layout(dc, context, availableChildRect, availableContainerSpace, style);
        }

#if 0
        // Redo the layout with a fixed, minimum size this time.
        Invalidate(wxRICHTEXT_ALL);
        wxRichTextAttr newAttr(attr);
        newAttr.GetTextBoxAttr().GetWidth().SetValue(maxSize.x, wxTEXT_ATTR_UNITS_PIXELS);
        newAttr.GetTextBoxAttr().GetWidth().SetPosition(wxTEXT_BOX_ATTR_POSITION_ABSOLUTE);

        availableChildRect = AdjustAvailableSpace(dc, buffer, parentAttr, newAttr, availableParentSpace, availableContainerSpace);

        // If a paragraph, align the whole paragraph.
        // Problem with this: if we're limited by a floating object, a line may be centered
        // w.r.t. the smaller resulting box rather than the actual available width.
        // FIXME: aligning whole paragraph not compatible with floating objects
        if (attr.HasAlignment() && (!wxRichTextBuffer::GetFloatingLayoutMode() || (GetContainer()->GetFloatCollector() && !GetContainer()->GetFloatCollector()->HasFloats())))
        {
            // centering, right-justification
            if (attr.GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
            {
                availableChildRect.x = (originalAvailableRect.GetWidth() - availableChildRect.GetWidth())/2 + availableChildRect.x;
            }
            else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_RIGHT)
            {
                availableChildRect.x = availableChildRect.x + originalAvailableRect.GetWidth() - availableChildRect.GetWidth();
            }
        }

        Layout(dc, context, availableChildRect, availableContainerSpace, style);
#endif
    }

    /*
     __________________
    |   ____________   |
    |  |            |  |


    */

    return true;
}

// Adjusts the attributes for virtual attribute provision, collapsed borders, etc.
bool wxRichTextObject::AdjustAttributes(wxRichTextAttr& attr, wxRichTextDrawingContext& context)
{
    context.ApplyVirtualAttributes(attr, this);

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

wxIMPLEMENT_CLASS(wxRichTextCompositeObject, wxRichTextObject);

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
int wxRichTextCompositeObject::HitTest(wxDC& dc, wxRichTextDrawingContext& context, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags)
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
            int ret = child->wxRichTextObject::HitTest(dc, context, pt, textPosition, obj, contextObj, flags);
            if (ret != wxRICHTEXT_HITTEST_NONE)
                return ret;
        }
        else if (child->IsShown())
        {
            int ret = child->HitTest(dc, context, pt, textPosition, obj, contextObj, flags);
            if (ret != wxRICHTEXT_HITTEST_NONE)
                return ret;
        }

        node = node->GetNext();
    }

    return wxRICHTEXT_HITTEST_NONE;
}

/// Finds the absolute position and row height for the given character position
bool wxRichTextCompositeObject::FindPosition(wxDC& dc, wxRichTextDrawingContext& context, long index, wxPoint& pt, int* height, bool forceLineStart)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        // Don't recurse if the child is a top-level object,
        // such as a text box, because the character position will no longer
        // apply. By definition, a top-level object has its own range of
        // character positions.
        if (!child->IsTopLevel() && child->FindPosition(dc, context, index, pt, height, forceLineStart))
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
        // the ranges between DeleteRange calls by calling CalculateRanges, and
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
            if (child->IsTopLevel())
                childRange = child->GetOwnRange();
            else
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
    return nullptr;
}

/// Recursively merge all pieces that can be merged.
bool wxRichTextCompositeObject::Defragment(wxRichTextDrawingContext& context, const wxRichTextRange& range)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        if (range == wxRICHTEXT_ALL || !child->GetRange().IsOutside(range))
        {
            wxRichTextCompositeObject* composite = wxDynamicCast(child, wxRichTextCompositeObject);
            if (composite)
            {
                composite->Defragment(context);
                node = node->GetNext();
            }
            else
            {
                // Optimization: if there are no virtual attributes, we won't need to
                // to split objects in order to paint individually attributed chunks.
                // So only merge in this case.
                if (!context.GetVirtualAttributesEnabled())
                {
                    if (node->GetNext())
                    {
                        wxRichTextObject* nextChild = node->GetNext()->GetData();
                        if (child->CanMerge(nextChild, context) && child->Merge(nextChild, context))
                        {
                            nextChild->Dereference();
                            m_children.Erase(node->GetNext());
                        }
                        else
                            node = node->GetNext();
                    }
                    else
                        node = node->GetNext();
                }
                else
                {
                    // If we might have virtual attributes, we first see if we have to split
                    // objects so that they may be painted with potential virtual attributes,
                    // since text objects can only draw or measure with a single attributes object
                    // at a time.
                    wxRichTextObject* childAfterSplit = child;
                    if (child->CanSplit(context))
                    {
                        childAfterSplit = child->Split(context);
                        node = m_children.Find(childAfterSplit);
                    }

                    if (node->GetNext())
                    {
                        wxRichTextObject* nextChild = node->GetNext()->GetData();

                        // First split child and nextChild so we have smaller fragments to merge.
                        // Then Merge only has to test per-object virtual attributes
                        // because for an object with all the same sub-object attributes,
                        // then any general virtual attributes should be merged with sub-objects by
                        // the implementation.

                        wxRichTextObject* nextChildAfterSplit = nextChild;

                        if (nextChildAfterSplit->CanSplit(context))
                            nextChildAfterSplit = nextChild->Split(context);

                        bool splitNextChild = nextChild != nextChildAfterSplit;

                        // See if we can merge this new fragment with (perhaps the first part of) the next object.
                        // Note that we use nextChild because if we had split nextChild, the first object always
                        // remains (and further parts are appended). However we must use childAfterSplit since
                        // it's the last part of a possibly split child.

                        if (childAfterSplit->CanMerge(nextChild, context) && childAfterSplit->Merge(nextChild, context))
                        {
                            nextChild->Dereference();
                            m_children.Erase(node->GetNext());

                            // Don't set node -- we'll see if we can merge again with the next
                            // child. UNLESS we split this or the next child, in which case we know we have to
                            // move on to the end of the next child.
                            if (splitNextChild)
                                node = m_children.Find(nextChildAfterSplit);
                        }
                        else
                        {
                            if (splitNextChild)
                                node = m_children.Find(nextChildAfterSplit); // start from the last object in the split
                            else
                                node = node->GetNext();
                        }
                    }
                    else
                        node = node->GetNext();
                }
            }
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
    wxRichTextObject::Dump(stream);

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
bool wxRichTextCompositeObject::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, wxRichTextDrawingContext& context, int flags, const wxPoint& position, const wxSize& parentSize, wxArrayInt* partialExtents) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    wxSize sz;

    wxArrayInt childExtents;
    wxArrayInt* p;
    if (partialExtents)
        p = & childExtents;
    else
        p = nullptr;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        if (!child->GetRange().IsOutside(range))
        {
            // Floating objects have a zero size within the paragraph.
            if (child->IsFloating() && wxRichTextBuffer::GetFloatingLayoutMode())
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
                else if (child->GetRangeSize(rangeToUse, childSize, childDescent, dc, context, flags, wxPoint(position.x + sz.x, position.y), parentSize, p))
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
            // Not sure why we did this, but it stops updated layout happening for floating objects.
#if 0
            if (wxRichTextBuffer::GetFloatingLayoutMode() && child->IsFloating() && GetBuffer()->GetFloatCollector() && GetBuffer()->GetFloatCollector()->HasFloat(child))
            {
                // Don't invalidate subhierarchy if we've already been laid out
            }
            else
#endif
            {
                if (invalidRange == wxRICHTEXT_NONE)
                    child->Invalidate(wxRICHTEXT_NONE);
                else
                    child->Invalidate(wxRICHTEXT_ALL); // All children must be invalidated if within parent range
            }
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

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextParagraphLayoutBox, wxRichTextCompositeObject);

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
        m_floatCollector = nullptr;
    }
}

/// Initialize the object.
void wxRichTextParagraphLayoutBox::Init()
{
    m_ctrl = nullptr;

    // For now, assume is the only box and has no initial size.
    m_range = wxRichTextRange(0, -1);
    m_ownRange = wxRichTextRange(0, -1);

    m_invalidRange = wxRICHTEXT_ALL;

    m_partialParagraph = false;
    m_floatCollector = nullptr;
}

void wxRichTextParagraphLayoutBox::Clear()
{
    DeleteChildren();

    if (m_floatCollector)
        delete m_floatCollector;
    m_floatCollector = nullptr;
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
    if (m_floatCollector != nullptr)
        delete m_floatCollector;
    m_floatCollector = new wxRichTextFloatCollector(availableRect);
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    // Only gather floats up to the point we'll start formatting paragraphs.
    while (untilObj && node && node->GetData() != untilObj)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (child != nullptr);
        if (child)
            m_floatCollector->CollectFloat(child);
        node = node->GetNext();
    }

    return true;
}

// Returns the style sheet associated with the overall buffer.
wxRichTextStyleSheet* wxRichTextParagraphLayoutBox::GetStyleSheet() const
{
    return GetBuffer() ? GetBuffer()->GetStyleSheet() : (wxRichTextStyleSheet*) nullptr;
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
int wxRichTextParagraphLayoutBox::HitTest(wxDC& dc, wxRichTextDrawingContext& context, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags)
{
    if (!IsShown())
        return wxRICHTEXT_HITTEST_NONE;

    int ret = wxRICHTEXT_HITTEST_NONE;
    if (wxRichTextBuffer::GetFloatingLayoutMode() && m_floatCollector && (flags & wxRICHTEXT_HITTEST_NO_FLOATING_OBJECTS) == 0)
    {
        ret = m_floatCollector->HitTest(dc, context, pt, textPosition, obj, contextObj, flags);
        if (ret != wxRICHTEXT_HITTEST_NONE)
        {
            return ret;
        }
    }

    if (ret == wxRICHTEXT_HITTEST_NONE)
        return wxRichTextCompositeObject::HitTest(dc, context, pt, textPosition, obj, contextObj, flags);
    else
    {
        *contextObj = this;
        return ret;
    }
}

/// Draw the floating objects
void wxRichTextParagraphLayoutBox::DrawFloats(wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    if (wxRichTextBuffer::GetFloatingLayoutMode() && m_floatCollector)
        m_floatCollector->Draw(dc, context, range, selection, rect, descent, style);
}

void wxRichTextParagraphLayoutBox::MoveAnchoredObjectToParagraph(wxRichTextParagraph* from, wxRichTextParagraph* to, wxRichTextObject* obj)
{
    if (from == to)
        return;

    from->RemoveChild(obj);
    to->AppendChild(obj);
}

/// Draw the item
bool wxRichTextParagraphLayoutBox::Draw(wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    context.SetLayingOut(false);

    if (!IsShown())
        return true;

    wxRect thisRect(GetPosition(), GetCachedSize());

    wxRichTextAttr attr(GetAttributes());
    AdjustAttributes(attr, context);

    int flags = style;
    if (selection.IsValid() &&
        ((GetParentContainer() != this && selection.GetContainer() == this && selection.WithinSelection(GetRange().GetStart(), GetParentContainer()))))
    {
        flags |= wxRICHTEXT_DRAW_SELECTED;
    }

    // Don't draw guidelines if at top level
    int theseFlags = flags;
    if (!GetParent())
        theseFlags &= ~wxRICHTEXT_DRAW_GUIDELINES;
    DrawBoxAttributes(dc, GetBuffer(), attr, thisRect, theseFlags, this);

    if (wxRichTextBuffer::GetFloatingLayoutMode())
        DrawFloats(dc, context, range, selection, rect, descent, style);

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        if (child && child->IsShown() && !child->GetRange().IsOutside(range))
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
                child->Draw(dc, context, childRange, selection, rect, descent, style);
        }

        node = node->GetNext();
    }
    return true;
}

/// Lay the item out
bool wxRichTextParagraphLayoutBox::Layout(wxDC& dc, wxRichTextDrawingContext& context, const wxRect& rect, const wxRect& parentRect, int style)
{
    context.SetLayingOut(true);

    Move(rect.GetPosition());

    if (!IsShown())
        return true;

    wxRect availableSpace;
    bool formatRect = (style & wxRICHTEXT_LAYOUT_SPECIFIED_RECT) == wxRICHTEXT_LAYOUT_SPECIFIED_RECT;

    wxRichTextAttr attr(GetAttributes());
    AdjustAttributes(attr, context);

    // If only laying out a specific area, the passed rect has a different meaning:
    // the visible part of the buffer. This is used in wxRichTextCtrl::OnSize,
    // so that during a size, only the visible part will be relaid out, or
    // it would take too long causing flicker. As an approximation, we assume that
    // everything up to the start of the visible area is laid out correctly.
    if (formatRect)
    {
        wxRect rect2(0, 0, rect.width, rect.height);
        availableSpace = GetAvailableContentArea(dc, context, rect2);

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
        availableSpace = GetAvailableContentArea(dc, context, rect);
    }

    // Fix the width if we're at the top level or if we're asked to
    if (!GetParent() || (style & wxRICHTEXT_FIXED_WIDTH))
        attr.GetTextBoxAttr().GetWidth().SetValue(rect.GetWidth(), wxTEXT_ATTR_UNITS_PIXELS);

    // Fix the height to the passed rect height if we're asked to
    if (style & wxRICHTEXT_FIXED_HEIGHT)
        attr.GetTextBoxAttr().GetHeight().SetValue(rect.GetHeight(), wxTEXT_ATTR_UNITS_PIXELS);

    // Don't pass fixed width/height styles to children
    style &= ~(wxRICHTEXT_FIXED_WIDTH|wxRICHTEXT_FIXED_HEIGHT);

    int leftMargin, rightMargin, topMargin, bottomMargin;
    wxRichTextObject::GetTotalMargin(dc, GetBuffer(), attr, leftMargin, rightMargin,
            topMargin, bottomMargin);

    int maxWidth = 0;
    int maxHeight = 0;

    // The maximum paragraph maximum width, so we can set the overall maximum width for this object
    int maxMaxWidth = 0;

    // The maximum paragraph minimum width, so we can set the overall minimum width for this object
    int maxMinWidth = 0;

    // If we have vertical alignment, we must recalculate everything.
    bool hasVerticalAlignment = (attr.GetTextBoxAttr().HasVerticalAlignment() &&
        (attr.GetTextBoxAttr().GetVerticalAlignment() > wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_TOP));

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
    if (wxRichTextBuffer::GetFloatingLayoutMode())
        UpdateFloatingObjects(availableSpace, node ? node->GetData() : (wxRichTextObject*) nullptr);

    // A way to force speedy rest-of-buffer layout (the 'else' below)
    bool forceQuickLayout = false;

    // First get the size of the paragraphs we won't be laying out
    wxRichTextObjectList::compatibility_iterator n = m_children.GetFirst();
    while (n && n != node)
    {
        wxRichTextParagraph* child = wxDynamicCast(n->GetData(), wxRichTextParagraph);
        if (child)
        {
            maxWidth = wxMax(maxWidth, child->GetCachedSize().x);
            maxMinWidth = wxMax(maxMinWidth, child->GetMinSize().x);
            maxMaxWidth = wxMax(maxMaxWidth, child->GetMaxSize().x);
        }
        n = n->GetNext();
    }

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
                        child->GetLines().empty() ||
                            !child->GetRange().IsOutside(invalidRange)) )
            {
                // Lays out the object first with a given amount of space, and then if no width was specified in attr,
                // lays out the object again using the minimum size
                child->LayoutToBestSize(dc, context, GetBuffer(),
                        attr, child->GetAttributes(), availableSpace, rect, style&~wxRICHTEXT_LAYOUT_SPECIFIED_RECT);

                // Layout must set the cached size
                availableSpace.y += child->GetCachedSize().y;
                maxWidth = wxMax(maxWidth, child->GetCachedSize().x);
                maxMinWidth = wxMax(maxMinWidth, child->GetMinSize().x);
                maxMaxWidth = wxMax(maxMaxWidth, child->GetMaxSize().x);

                // If we're just formatting the visible part of the buffer,
                // and we're now past the bottom of the window, start quick layout.
                if (!hasVerticalAlignment && formatRect && child->GetPosition().y > rect.GetBottom())
                    forceQuickLayout = true;
            }
            else
            {
                // We're outside the immediately affected range, so now let's just
                // move everything up or down. This assumes that all the children have previously
                // been laid out and have wrapped line lists associated with them.
                // TODO: check all paragraphs before the affected range.

                // Lay out paragraphs until they are (and were) not affected
                // by floating objects from above the paragraphs.
                if (wxRichTextBuffer::GetFloatingLayoutMode())
                {
                    while (node)
                    {
                        child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
                        if (child)
                        {
                            int oldImpactedByFloats = child->GetImpactedByFloatingObjects();

                            child->SetImpactedByFloatingObjects(-1);

                            child->LayoutToBestSize(dc, context, GetBuffer(),
                                attr, child->GetAttributes(), availableSpace, rect, style&~wxRICHTEXT_LAYOUT_SPECIFIED_RECT);

                            availableSpace.y += child->GetCachedSize().y;
                            maxWidth = wxMax(maxWidth, child->GetCachedSize().x);
                            maxMinWidth = wxMax(maxMinWidth, child->GetMinSize().x);
                            maxMaxWidth = wxMax(maxMaxWidth, child->GetMaxSize().x);

                            int newImpactedByFloats = child->GetImpactedByFloatingObjects();

                            // We can stop laying out if this paragraph is unaffected by floating
                            // objects, and was previously too.
                            if (oldImpactedByFloats == 0 && newImpactedByFloats == 0)
                            {
                                node = node->GetNext();
                                break;
                            }
                        }
                        node = node->GetNext();
                    }
                }

                int inc = 0;
                if (node)
                {
                    child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
                    inc = availableSpace.y - child->GetPosition().y;
                }

                while (node)
                {
                    wxRichTextParagraph* nodeChild = wxDynamicCast(node->GetData(), wxRichTextParagraph);
                    if (nodeChild)
                    {
                        if (nodeChild->GetLines().empty())
                        {
                            nodeChild->SetImpactedByFloatingObjects(-1);

                            // Lays out the object first with a given amount of space, and then if no width was specified in attr,
                            // lays out the object again using the minimum size
                            nodeChild->LayoutToBestSize(dc, context, GetBuffer(),
                                        attr, nodeChild->GetAttributes(), availableSpace, rect, style&~wxRICHTEXT_LAYOUT_SPECIFIED_RECT);
                        }
                        else
                        {
                            if (wxRichTextBuffer::GetFloatingLayoutMode() && GetFloatCollector())
                                GetFloatCollector()->CollectFloat(nodeChild);
                            nodeChild->Move(wxPoint(nodeChild->GetPosition().x, nodeChild->GetPosition().y + inc));
                        }

                        availableSpace.y += nodeChild->GetCachedSize().y;
                        maxWidth = wxMax(maxWidth, nodeChild->GetCachedSize().x);
                        maxMinWidth = wxMax(maxMinWidth, nodeChild->GetMinSize().x);
                        maxMaxWidth = wxMax(maxMaxWidth, nodeChild->GetMaxSize().x);
                    }

                    node = node->GetNext();
                }
                break;
            }
        }

        node = node->GetNext();
    }

    int maxContentHeight = 0;

    node = m_children.GetLast();
    if (node && node->GetData()->IsShown())
    {
        wxRichTextObject* child = node->GetData();
        maxHeight = child->GetPosition().y - (GetPosition().y + topMargin) + child->GetCachedSize().y;
        maxContentHeight = maxHeight;
    }
    else
        maxHeight = 0; // topMargin + bottomMargin;

    // Check the bottom edge of any floating object
    if (wxRichTextBuffer::GetFloatingLayoutMode() && GetFloatCollector() && GetFloatCollector()->HasFloats())
    {
        // The floating objects are positioned relative to entire buffer, not this box
        int maxFloatHeight = GetFloatCollector()->GetLastRectBottom() - GetPosition().y - topMargin;
        if (maxFloatHeight > maxHeight)
            maxHeight = maxFloatHeight;
    }

    if (attr.GetTextBoxAttr().GetSize().GetWidth().IsValid())
    {
        wxRect r = AdjustAvailableSpace(dc, GetBuffer(), wxRichTextAttr() /* not used */, attr, parentRect, parentRect);
        int w = r.GetWidth();

        // Convert external to content rect
        w = w - leftMargin - rightMargin;
        maxWidth = wxMax(maxWidth, w);
        maxMaxWidth = wxMax(maxMaxWidth, w);
    }
    else
    {
        // TODO: Make sure the layout box's position reflects
        // the position of the children, but without
        // breaking layout of a box within a paragraph.
    }

    if (attr.GetTextBoxAttr().GetSize().GetHeight().IsValid())
    {
        wxRect r = AdjustAvailableSpace(dc, GetBuffer(), wxRichTextAttr() /* not used */, attr, parentRect, parentRect);
        int h = r.GetHeight();

        // Convert external to content rect
        h = h - topMargin - bottomMargin;
        maxHeight = wxMax(maxHeight, h);
    }

    // We need to add back the margins etc.
    {
        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(maxWidth, maxHeight));
        GetBoxRects(dc, GetBuffer(), attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);
        SetCachedSize(marginRect.GetSize());
    }

    // The maximum size is the greatest of all maximum widths for all paragraphs.
    {
        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(maxMaxWidth, maxHeight)); // Actually max height is a lie, we can't know it
        GetBoxRects(dc, GetBuffer(), attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);
        SetMaxSize(marginRect.GetSize());
    }

    // The minimum size is the greatest of all minimum widths for all paragraphs.
    {
        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(maxMinWidth, maxHeight)); // Actually max height is a lie, we can't know it
        GetBoxRects(dc, GetBuffer(), attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);
        SetMinSize(marginRect.GetSize());
    }

    if (attr.GetTextBoxAttr().HasVerticalAlignment() &&
        (attr.GetTextBoxAttr().GetVerticalAlignment() > wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_TOP))
    {
        int yOffset = 0;
        int leftOverSpace = GetCachedSize().y - topMargin - bottomMargin - maxContentHeight;
        if (leftOverSpace > 0)
        {
            if (attr.GetTextBoxAttr().GetVerticalAlignment() == wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_CENTRE)
            {
                yOffset = (leftOverSpace/2);
            }
            else if (attr.GetTextBoxAttr().GetVerticalAlignment() == wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_BOTTOM)
            {
                yOffset = leftOverSpace;
            }
        }

        // Move all the children to vertically align the content
        // This doesn't take into account floating objects, unfortunately.
        if (yOffset != 0)
        {
            wxRichTextObjectList::compatibility_iterator childNode = m_children.GetFirst();
            while (childNode)
            {
                wxRichTextParagraph* child = wxDynamicCast(childNode->GetData(), wxRichTextParagraph);
                if (child)
                    child->Move(wxPoint(child->GetPosition().x, child->GetPosition().y + yOffset));

                childNode = childNode->GetNext();
            }
        }
    }

    m_invalidRange = wxRICHTEXT_NONE;

    return true;
}

/// Get/set the size for the given range.
bool wxRichTextParagraphLayoutBox::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, wxRichTextDrawingContext& context, int flags, const wxPoint& position, const wxSize& parentSize, wxArrayInt* WXUNUSED(partialExtents)) const
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
        child->GetRangeSize(rangeToFind, childSize, childDescent, dc, context, flags, position, parentSize);

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
        // wxASSERT (child != nullptr);

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
    return nullptr;
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
            // wxASSERT (child != nullptr);

            if (child)
            {
                wxRichTextLineVector::const_iterator it = child->GetLines().begin();
                while (it != child->GetLines().end())
                {
                    wxRichTextLine* line = *it;

                    wxRichTextRange range = line->GetAbsoluteRange();

                    if (range.Contains(pos) ||

                        // If the position is end-of-paragraph, then return the last line of
                        // of the paragraph.
                        ((range.GetEnd() == child->GetRange().GetEnd()-1) && (pos == child->GetRange().GetEnd())))
                        return line;

                    ++it;
                }
            }
        }

        node = node->GetNext();
    }

    int lineCount = GetLineCount();
    if (lineCount > 0)
        return GetLineForVisibleLineNumber(lineCount-1);
    else
        return nullptr;
}

/// Get the line at the given y pixel position, or the last line.
wxRichTextLine* wxRichTextParagraphLayoutBox::GetLineAtYPosition(int y) const
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (child != nullptr);

        if (child)
        {
            wxRichTextLineVector::const_iterator it = child->GetLines().begin();
            while (it != child->GetLines().end())
            {
                wxRichTextLine* line = *it;

                wxRect rect(line->GetRect());

                if (y <= rect.GetBottom())
                    return line;

                ++it;
            }
        }

        node = node->GetNext();
    }

    // Return last line
    int lineCount = GetLineCount();
    if (lineCount > 0)
        return GetLineForVisibleLineNumber(lineCount-1);
    else
        return nullptr;
}

/// Get the number of visible lines
int wxRichTextParagraphLayoutBox::GetLineCount() const
{
    int count = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (child != nullptr);

        if (child)
            count += child->GetLines().size();

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
    para->GetAttributes().GetTextBoxAttr().Reset();

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

    wxRichTextParagraph* firstPara = nullptr;
    wxRichTextParagraph* lastPara = nullptr;

    wxRichTextRange range(-1, -1);

    size_t i = 0;
    size_t len = text.length();
    wxString line;
    wxRichTextParagraph* para = new wxRichTextParagraph(wxEmptyString, this, pStyle, cStyle);
    para->GetAttributes().GetTextBoxAttr().Reset();

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
                para->GetAttributes().GetTextBoxAttr().Reset();

                AppendChild(para);

                lastPara = para;
                line.clear();
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
    para->GetAttributes().GetTextBoxAttr().Reset();
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
        wxRichTextProperties originalProperties = para->GetProperties();

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
            wxASSERT (firstPara != nullptr);

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
            wxASSERT(firstPara != nullptr);

            if (!(fragment.GetAttributes().GetFlags() & wxTEXT_ATTR_KEEP_FIRST_PARA_STYLE))
            {
                para->SetAttributes(firstPara->GetAttributes());
                para->SetProperties(firstPara->GetProperties());
            }

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
            wxRichTextObject* nextParagraph = nullptr;
            if (nextParagraphNode)
                nextParagraph = nextParagraphNode->GetData();

            wxRichTextObjectList::compatibility_iterator i = fragment.GetChildren().GetFirst()->GetNext();
            wxRichTextParagraph* finalPara = para;

            bool needExtraPara = (!i || !fragment.GetPartialParagraph());

            // If there was only one paragraph, we need to insert a new one.
            while (i)
            {
                wxRichTextParagraph* searchPara = wxDynamicCast(i->GetData(), wxRichTextParagraph);
                wxASSERT( searchPara != nullptr );

                finalPara = (wxRichTextParagraph*) searchPara->Clone();

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
            {
                finalPara->SetAttributes(firstPara->GetAttributes());
                finalPara->SetProperties(firstPara->GetProperties());
            }
            else if (finalPara && finalPara != para)
            {
                finalPara->SetAttributes(originalAttr);
                finalPara->SetProperties(originalProperties);
            }

            return true;
        }
    }
    else
    {
        // Append
        wxRichTextObjectList::compatibility_iterator i = fragment.GetChildren().GetFirst();
        while (i)
        {
            wxRichTextParagraph* searchPara = wxDynamicCast(i->GetData(), wxRichTextParagraph);
            wxASSERT( searchPara != nullptr );

            AppendChild(searchPara->Clone());

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
        wxASSERT( para != nullptr );

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
        wxASSERT( firstPara != nullptr );

        wxRichTextParagraph* lastPara = wxDynamicCast(fragment.GetChildren().GetLast()->GetData(), wxRichTextParagraph);
        wxASSERT( lastPara != nullptr );

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

        if (topTailRange.GetStart() < lastPara->GetRange().GetEnd())
        {
            lastPara->DeleteRange(topTailRange);

            // Make sure the numbering is correct
            long unusedEnd;
            fragment.CalculateRange(0, unusedEnd);

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
        // wxASSERT( child != nullptr );

        if (child)
        {
            if (child->GetRange().Contains(pos))
            {
                wxRichTextLineVector::const_iterator it = child->GetLines().begin();
                while (it != child->GetLines().end())
                {
                    wxRichTextLine* line = *it;
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

                    ++it;
                }
                // If we didn't find it in the lines, it must be
                // the last position of the paragraph. So return the last line.
                return lineCount-1;
            }
            else
                lineCount += child->GetLines().size();
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
        // wxASSERT(child != nullptr);

        if (child)
        {
            if (lineNumber < (int) (child->GetLines().size() + lineCount))
            {
                wxRichTextLineVector::const_iterator it = child->GetLines().begin();
                while (it != child->GetLines().end())
                {
                    wxRichTextLine* line = *it;

                    if (lineCount == lineNumber)
                        return line;

                    lineCount ++;

                    ++it;
                }
            }
            else
                lineCount += child->GetLines().size();
        }

        node = node->GetNext();
    }

    // Didn't find it
    return nullptr;
}

/// Delete range from layout.
bool wxRichTextParagraphLayoutBox::DeleteRange(const wxRichTextRange& range)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();

    wxRichTextParagraph* firstPara = nullptr;
    while (node)
    {
        wxRichTextParagraph* obj = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (obj != nullptr);

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
                    obj = nullptr;
                }
                else if (!firstPara)
                    firstPara = obj;

                // If the range includes the paragraph end, we need to join this
                // and the next paragraph.
                if (range.GetEnd() <= thisRange.GetEnd())
                {
                    // We need to move the objects from the next paragraph
                    // to this paragraph

                    wxRichTextParagraph* nextParagraph = nullptr;
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
            if (child->IsTopLevel())
                childRange = child->GetOwnRange();
            else
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
        return nullptr;

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
    return nullptr;
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

    bool haveControl = (buffer->GetRichTextCtrl() != nullptr);

    wxRichTextAction* action = nullptr;

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(nullptr, _("Change Style"), wxRICHTEXT_CHANGE_STYLE, buffer, this, buffer->GetRichTextCtrl());
        action->SetRange(range);
        action->SetPosition(buffer->GetRichTextCtrl()->GetCaretPosition());
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (para != nullptr);

        if (para && para->GetChildCount() > 0)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                break;

            if (!para->GetRange().IsOutside(range))
            {
                // We'll be using a copy of the paragraph to make style changes,
                // not updating the buffer directly.
                wxRichTextParagraph* newPara wxDUMMY_INITIALIZE(nullptr);

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

                if (!parasOnly && (characterStyle || charactersOnly) && range.GetStart() != newPara->GetRange().GetEnd())
                {
                    wxRichTextRange childRange(range);
                    childRange.LimitTo(newPara->GetRange());

                    // Find the starting position and if necessary split it so
                    // we can start applying a different style.
                    // TODO: check that the style actually changes or is different
                    // from style outside of range
                    wxRichTextObject* firstObject wxDUMMY_INITIALIZE(nullptr);
                    wxRichTextObject* lastObject wxDUMMY_INITIALIZE(nullptr);

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

                    wxASSERT(firstObject != nullptr);
                    wxASSERT(lastObject != nullptr);

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
                        {
                            // Preserve the URL as it's not really a formatting style but a property of the object
                            wxString url;
                            if (child->GetAttributes().HasURL() && !characterAttributes.HasURL())
                                url = child->GetAttributes().GetURL();

                            child->GetAttributes() = characterAttributes;

                            if (!url.IsEmpty())
                                child->GetAttributes().SetURL(url);
                        }
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
    bool haveControl = (buffer->GetRichTextCtrl() != nullptr);

    wxRichTextAction *action = nullptr;
    wxRichTextAttr newAttr = obj->GetAttributes();
    if (resetExistingStyle)
        newAttr = textAttr;
    else
        newAttr.Apply(textAttr);

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(nullptr, _("Change Object Style"), wxRICHTEXT_CHANGE_ATTRIBUTES, buffer, obj->GetContainer(), buffer->GetRichTextCtrl());
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
    wxRichTextObject* obj wxDUMMY_INITIALIZE(nullptr);

    if (style.IsParagraphStyle())
    {
        obj = GetParagraphAtPosition(position);
        if (obj)
        {
            if (combineStyles)
            {
                // Start with the base style
                style = GetAttributes();
                style.GetTextBoxAttr().Reset();

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

    wxRichTextAttr clashingAttrPara, clashingAttrChar;
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

                CollectStyle(style, paraStyle, clashingAttrPara, absentAttrPara);
            }
            else
            {
                wxRichTextRange paraRange(para->GetRange());
                paraRange.LimitTo(range);

                // First collect paragraph attributes only
                wxRichTextAttr paraStyle = para->GetCombinedAttributes();
                paraStyle.SetFlags(paraStyle.GetFlags() & wxTEXT_ATTR_PARAGRAPH);
                CollectStyle(style, paraStyle, clashingAttrPara, absentAttrPara);

                wxRichTextObjectList::compatibility_iterator childNode = para->GetChildren().GetFirst();

                while (childNode)
                {
                    wxRichTextObject* child = childNode->GetData();
                    if (!(child->GetRange().GetStart() > range.GetEnd() || child->GetRange().GetEnd() < range.GetStart()))
                    {
                        wxRichTextAttr childStyle = para->GetCombinedAttributes(child->GetAttributes(), true /* include box attributes */);

                        // Now collect character attributes only
                        childStyle.SetFlags(childStyle.GetFlags() & wxTEXT_ATTR_CHARACTER);

                        CollectStyle(style, childStyle, clashingAttrChar, absentAttrChar);
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
        // wxASSERT (para != nullptr);

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

                    if (!childRange.IsOutside(range) && wxDynamicCast(child, wxRichTextPlainText))
                    {
                        foundCount ++;
                        wxRichTextAttr textAttr = para->GetCombinedAttributes(child->GetAttributes());
                        textAttr.SetFlags(textAttr.GetFlags() & ~wxTEXT_ATTR_PARAGRAPH);

                        if (textAttr.EqPartial(style, false /* strong test - attributes must be valid in both objects */))
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
        // wxASSERT (para != nullptr);

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

                // These flags can mess up EqPartial because they don't represent existence of the attributes,
                // only the attributes.
                int flags = wxTEXT_ATTR_AVOID_PAGE_BREAK_BEFORE|wxTEXT_ATTR_AVOID_PAGE_BREAK_AFTER|wxTEXT_ATTR_PAGE_BREAK;
                int theseFlags = (textAttr.GetFlags() & ~flags) | (textAttr.GetFlags() & flags & style.GetFlags());
                textAttr.SetFlags(theseFlags);

                foundCount ++;
                if (textAttr.EqPartial(style, false /* strong test */))
                    matchingCount ++;
            }
        }

        node = node->GetNext();
    }
    return foundCount == matchingCount && foundCount != 0;
}

void wxRichTextParagraphLayoutBox::PrepareContent(wxRichTextParagraphLayoutBox& container)
{
    wxRichTextBuffer* buffer = GetBuffer();
    if (buffer && buffer->GetRichTextCtrl())
        buffer->GetRichTextCtrl()->PrepareContent(container);
}

/// Set character or paragraph properties
bool wxRichTextParagraphLayoutBox::SetProperties(const wxRichTextRange& range, const wxRichTextProperties& properties, int flags)
{
    wxRichTextBuffer* buffer = GetBuffer();

    bool withUndo = ((flags & wxRICHTEXT_SETPROPERTIES_WITH_UNDO) != 0);
    bool parasOnly = ((flags & wxRICHTEXT_SETPROPERTIES_PARAGRAPHS_ONLY) != 0);
    bool charactersOnly = ((flags & wxRICHTEXT_SETPROPERTIES_CHARACTERS_ONLY) != 0);
    bool resetExistingProperties = ((flags & wxRICHTEXT_SETPROPERTIES_RESET) != 0);
    bool removeProperties = ((flags & wxRICHTEXT_SETPROPERTIES_REMOVE) != 0);

    // If we are associated with a control, make undoable; otherwise, apply immediately
    // to the data.

    bool haveControl = (buffer->GetRichTextCtrl() != nullptr);

    wxRichTextAction* action = nullptr;

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(nullptr, _("Change Properties"), wxRICHTEXT_CHANGE_PROPERTIES, buffer, this, buffer->GetRichTextCtrl());
        action->SetRange(range);
        action->SetPosition(buffer->GetRichTextCtrl()->GetCaretPosition());
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (para != nullptr);

        if (para && para->GetChildCount() > 0)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                break;

            if (!para->GetRange().IsOutside(range))
            {
                // We'll be using a copy of the paragraph to make style changes,
                // not updating the buffer directly.
                wxRichTextParagraph* newPara wxDUMMY_INITIALIZE(nullptr);

                if (haveControl && withUndo)
                {
                    newPara = new wxRichTextParagraph(*para);
                    action->GetNewParagraphs().AppendChild(newPara);

                    // Also store the old ones for Undo
                    action->GetOldParagraphs().AppendChild(new wxRichTextParagraph(*para));
                }
                else
                    newPara = para;

                if (parasOnly)
                {
                    if (removeProperties)
                    {
                        // Removes the given style from the paragraph
                        // TODO
                        newPara->GetProperties().RemoveProperties(properties);
                    }
                    else if (resetExistingProperties)
                        newPara->GetProperties() = properties;
                    else
                        newPara->GetProperties().MergeProperties(properties);
                }

                // When applying paragraph styles dynamically, don't change the text objects' attributes
                // since they will computed as needed. Only apply the character styling if it's _only_
                // character styling. This policy is subject to change and might be put under user control.

                // Hm. we might well be applying a mix of paragraph and character styles, in which
                // case we _do_ want to apply character styles regardless of what para styles are set.
                // But if we're applying a paragraph style, which has some character attributes, but
                // we only want the paragraphs to hold this character style, then we _don't_ want to
                // apply the character style. So we need to be able to choose.

                if (!parasOnly && charactersOnly && range.GetStart() != newPara->GetRange().GetEnd())
                {
                    wxRichTextRange childRange(range);
                    childRange.LimitTo(newPara->GetRange());

                    // Find the starting position and if necessary split it so
                    // we can start applying different properties.
                    // TODO: check that the properties actually change or are different
                    // from properties outside of range
                    wxRichTextObject* firstObject wxDUMMY_INITIALIZE(nullptr);
                    wxRichTextObject* lastObject wxDUMMY_INITIALIZE(nullptr);

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

                    wxASSERT(firstObject != nullptr);
                    wxASSERT(lastObject != nullptr);

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

                        if (removeProperties)
                        {
                            // Removes the given properties from the paragraph
                            child->GetProperties().RemoveProperties(properties);
                        }
                        else if (resetExistingProperties)
                            child->GetProperties() = properties;
                        else
                        {
                            child->GetProperties().MergeProperties(properties);
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

void wxRichTextParagraphLayoutBox::Reset()
{
    Clear();

    wxRichTextBuffer* buffer = GetBuffer();
    if (buffer && buffer->GetRichTextCtrl())
    {
        wxRichTextEvent event(wxEVT_RICHTEXT_BUFFER_RESET, buffer->GetRichTextCtrl()->GetId());
        event.SetEventObject(buffer->GetRichTextCtrl());
        event.SetContainer(this);

        buffer->SendEvent(event, true);
    }

    AddParagraph(wxEmptyString);

    PrepareContent(*this);

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

        wxRichTextParagraph* para2 = GetParagraphAtPosition(range.GetEnd());
        if (para2)
            range.SetEnd(para2->GetRange().GetEnd());
    }
    return range;
}

/// Apply the style sheet to the buffer, for example if the styles have changed.
bool wxRichTextParagraphLayoutBox::ApplyStyleSheet(wxRichTextStyleSheet* styleSheet)
{
    wxASSERT(styleSheet != nullptr);
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
        // wxASSERT (para != nullptr);

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

    bool haveControl = (buffer->GetRichTextCtrl() != nullptr);

    wxRichTextAction* action = nullptr;

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(nullptr, _("Change List Style"), wxRICHTEXT_CHANGE_STYLE, buffer, this, buffer->GetRichTextCtrl());
        action->SetRange(range);
        action->SetPosition(buffer->GetRichTextCtrl()->GetCaretPosition());
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (para != nullptr);

        if (para && para->GetChildCount() > 0)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                break;

            if (!para->GetRange().IsOutside(range))
            {
                // We'll be using a copy of the paragraph to make style changes,
                // not updating the buffer directly.
                wxRichTextParagraph* newPara wxDUMMY_INITIALIZE(nullptr);

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
                    // Preserve the existing list item continuation bullet style, if any
                    if (para->GetAttributes().HasBulletStyle() && (para->GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_CONTINUATION))
                        newPara->GetAttributes().SetBulletStyle(newPara->GetAttributes().GetBulletStyle()|wxTEXT_ATTR_BULLET_STYLE_CONTINUATION);
                    else
                    {
                        if (renumber)
                        {
                            newPara->GetAttributes().SetBulletNumber(n);
                        }

                        n ++;
                    }
                }
                else if (!newPara->GetAttributes().GetListStyleName().IsEmpty())
                {
                    // if def is null, remove list style, applying any associated paragraph style
                    // to restore the attributes

                    newPara->GetAttributes().SetListStyleName(wxEmptyString);
                    newPara->GetAttributes().SetLeftIndent(0, 0);
                    newPara->GetAttributes().SetBulletText(wxEmptyString);
                    newPara->GetAttributes().SetBulletStyle(0);

                    // Eliminate the main list-related attributes
                    newPara->GetAttributes().SetFlags(newPara->GetAttributes().GetFlags() & ~wxTEXT_ATTR_LEFT_INDENT & ~wxTEXT_ATTR_BULLET_STYLE & ~wxTEXT_ATTR_BULLET_NUMBER & ~wxTEXT_ATTR_BULLET_TEXT & wxTEXT_ATTR_LIST_STYLE_NAME);

                    if (styleSheet && !newPara->GetAttributes().GetParagraphStyleName().IsEmpty())
                    {
                        wxRichTextParagraphStyleDefinition* newParaDef = styleSheet->FindParagraphStyle(newPara->GetAttributes().GetParagraphStyleName());
                        if (newParaDef)
                        {
                            newPara->GetAttributes() = newParaDef->GetStyleMergedWithBase(styleSheet);
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
    return SetListStyle(range, nullptr, flags);
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

#if wxDEBUG_LEVEL
    wxASSERT(!specifyLevel || (specifyLevel && (specifiedLevel >= 0)));
#endif

    // If we are associated with a control, make undoable; otherwise, apply immediately
    // to the data.

    bool haveControl = (buffer->GetRichTextCtrl() != nullptr);

    wxRichTextAction* action = nullptr;

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(nullptr, _("Renumber List"), wxRICHTEXT_CHANGE_STYLE, buffer, this, buffer->GetRichTextCtrl());
        action->SetRange(range);
        action->SetPosition(buffer->GetRichTextCtrl()->GetCaretPosition());
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        // wxASSERT (para != nullptr);

        if (para && para->GetChildCount() > 0)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                break;

            if (!para->GetRange().IsOutside(range))
            {
                // We'll be using a copy of the paragraph to make style changes,
                // not updating the buffer directly.
                wxRichTextParagraph* newPara wxDUMMY_INITIALIZE(nullptr);

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

                    // Preserve the existing list item continuation bullet style, if any
                    if (para->GetAttributes().HasBulletStyle() && (para->GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_CONTINUATION))
                        newPara->GetAttributes().SetBulletStyle(newPara->GetAttributes().GetBulletStyle()|wxTEXT_ATTR_BULLET_STYLE_CONTINUATION);

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
                        if (!(para->GetAttributes().HasBulletStyle() && (para->GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_CONTINUATION)))
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
        wxRichTextListStyleDefinition* def = nullptr;
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
        wxRichTextListStyleDefinition* def = nullptr;
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
    // TODO: add GetNextChild/GetPreviousChild to composite
    // Search for a paragraph that isn't a continuation paragraph (no bullet)
    while (previousParagraph && previousParagraph->GetAttributes().HasBulletStyle() && previousParagraph->GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_CONTINUATION)
    {
        wxRichTextObjectList::compatibility_iterator node = ((wxRichTextCompositeObject*) previousParagraph->GetParent())->GetChildren().Find(previousParagraph);
        if (node)
        {
            node = node->GetPrevious();
            if (node)
                previousParagraph = wxDynamicCast(node->GetData(), wxRichTextParagraph);
            else
                previousParagraph = nullptr;
        }
        else
            previousParagraph = nullptr;
    }

    if (!previousParagraph || !previousParagraph->GetAttributes().HasFlag(wxTEXT_ATTR_BULLET_STYLE) || previousParagraph->GetAttributes().GetBulletStyle() == wxTEXT_ATTR_BULLET_STYLE_NONE)
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
                        text = text.Mid(0, text.length() - pos - 1);
                    }
                    else
                        text.clear();
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

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextParagraph, wxRichTextCompositeObject);

wxArrayInt wxRichTextParagraph::sm_defaultTabs;

wxRichTextParagraph::wxRichTextParagraph(wxRichTextObject* parent, wxRichTextAttr* style):
    wxRichTextCompositeObject(parent)
{
    Init();

    if (style)
        SetAttributes(*style);
}

wxRichTextParagraph::wxRichTextParagraph(const wxString& text, wxRichTextObject* parent, wxRichTextAttr* paraStyle, wxRichTextAttr* charStyle):
    wxRichTextCompositeObject(parent)
{
    Init();

    if (paraStyle)
        SetAttributes(*paraStyle);

    AppendChild(new wxRichTextPlainText(text, this, charStyle));
}

void wxRichTextParagraph::Init()
{
    m_impactedByFloatingObjects = -1;
}

wxRichTextParagraph::~wxRichTextParagraph()
{
    ClearLines();
}

/// Draw the item
bool wxRichTextParagraph::Draw(wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int WXUNUSED(descent), int style)
{
    if (!IsShown())
        return true;

    // Currently we don't merge these attributes with the parent, but we
    // should consider whether we should (e.g. if we set a border colour
    // for all paragraphs). But generally box attributes are likely to be
    // different for different objects.
    wxRect paraRect = GetRect();
    wxRichTextAttr attr = GetCombinedAttributes();
    AdjustAttributes(attr, context);

    DrawBoxAttributes(dc, GetBuffer(), attr, paraRect, 0);

    // Draw the bullet, if any
    if ((attr.GetBulletStyle() != wxTEXT_ATTR_BULLET_STYLE_NONE) && (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_CONTINUATION) == 0)
    {
        {
            int spaceBeforePara = ConvertTenthsMMToPixels(dc, attr.GetParagraphSpacingBefore());
            int leftIndent = ConvertTenthsMMToPixels(dc, attr.GetLeftIndent());

            wxRichTextAttr bulletAttr(attr);

            // Get line height from first line, if any
            wxRichTextLine* line = m_cachedLines.empty() ? nullptr : m_cachedLines[0];

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

    wxRichTextLineVector::const_iterator it = m_cachedLines.begin();
    while (it != m_cachedLines.end())
    {
        wxRichTextLine* line = *it;
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

                if ((!child->IsFloating() || !wxRichTextBuffer::GetFloatingLayoutMode()) && child->GetRange().GetLength() > 0 && !child->GetRange().IsOutside(lineRange) && !lineRange.IsOutside(range))
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
                            child->GetRangeSize(objectRange, objectSize, descent, dc, context, wxRICHTEXT_UNFORMATTED, objectPosition);
                        }
                    }

                    // Use the child object's width, but the whole line's height
                    wxRect childRect(objectPosition, wxSize(objectSize.x, line->GetSize().y));
                    child->Draw(dc, context, objectRange, selection, childRect, maxDescent, style);

                    objectPosition.x += objectSize.x;
                    i ++;
                }
                else if (child->GetRange().GetStart() > lineRange.GetEnd())
                    // Can break out of inner loop now since we've passed this line's range
                    break;

                node2 = node2->GetNext();
            }
        }

        ++it;
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
bool wxRichTextParagraph::Layout(wxDC& dc, wxRichTextDrawingContext& context, const wxRect& rect, const wxRect& parentRect, int style)
{
    // Deal with floating objects firstly before the normal layout
    wxRichTextBuffer* buffer = GetBuffer();
    wxASSERT(buffer);

    wxRichTextFloatCollector* collector = GetContainer()->GetFloatCollector();

    if (wxRichTextBuffer::GetFloatingLayoutMode())
    {
        wxASSERT(collector != nullptr);
        if (collector)
        {
            if (m_impactedByFloatingObjects == -1)
            {
                if (collector->GetLastRectBottom() >= rect.GetTop())
                     m_impactedByFloatingObjects = 1;
                else
                     m_impactedByFloatingObjects = 0;
            }

            LayoutFloat(dc, context, rect, parentRect, style, collector);
        }
    }

    wxRichTextAttr attr = GetCombinedAttributes();
    AdjustAttributes(attr, context);

    // Increase the size of the paragraph due to spacing
    int spaceBeforePara = ConvertTenthsMMToPixels(dc, attr.GetParagraphSpacingBefore());
    int spaceAfterPara = ConvertTenthsMMToPixels(dc, attr.GetParagraphSpacingAfter());
    int leftIndent = ConvertTenthsMMToPixels(dc, attr.GetLeftIndent());
    int leftSubIndent = ConvertTenthsMMToPixels(dc, attr.GetLeftSubIndent());
    int rightIndent = ConvertTenthsMMToPixels(dc, attr.GetRightIndent());

    int lineSpacing = 0;

    // Let's assume line spacing of 10 is normal, 15 is 1.5, 20 is 2, etc.
    if (attr.HasLineSpacing() && attr.GetLineSpacing() > 0 && attr.HasFont())
    {
        wxFont font(buffer->GetFontTable().FindFont(attr));
        if (font.IsOk())
        {
            wxCheckSetFont(dc, font);
            lineSpacing = (int) (double(dc.GetCharHeight()) * (double(attr.GetLineSpacing())/10.0 - 1.0));
        }
    }

    // Make space for a bullet with no subindent.
    if ((leftSubIndent == 0) && (attr.GetBulletStyle() != wxTEXT_ATTR_BULLET_STYLE_NONE))
    {
        wxSize bulletSize;
        if (wxRichTextBuffer::GetRenderer() && wxRichTextBuffer::GetRenderer()->MeasureBullet(this, dc, attr, bulletSize))
        {
            wxFont font(buffer->GetFontTable().FindFont(attr));
            if (font.IsOk())
            {
                wxCheckSetFont(dc, font);
                wxCoord spaceW = 0, spaceH = 0, maxDescent = 0;
                dc.GetTextExtent(wxT(" "), & spaceW, & spaceH, & maxDescent);
                leftSubIndent = bulletSize.x + spaceW;
            }
        }
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
    GetRangeSize(GetRange(), paraSize, paraDescent, dc, context, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_CACHE_SIZE, rect.GetPosition(), parentRect.GetSize(), & partialExtents);
#else
    node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        //child->SetCachedSize(wxDefaultSize);
        child->Layout(dc, context, rect, style);

        node = node->GetNext();
    }
#endif

    // Split up lines

    // We may need to go back to a previous child, in which case create the new line,
    // find the child corresponding to the start position of the string, and
    // continue.

    wxRect availableRect;
    long nextBreakPos = GetFirstLineBreakPosition(lastEndPos+1);

    node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        // If floating, ignore. We already laid out floats.
        // Also ignore if empty object, except if we haven't got any
        // size yet.
        if ((child->IsFloating() && wxRichTextBuffer::GetFloatingLayoutMode())
            || !child->IsShown() || (child->GetRange().GetLength() == 0 && maxHeight > spaceBeforePara)
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

        if (nextBreakPos > -1 && nextBreakPos < lastEndPos+1)
            nextBreakPos = GetFirstLineBreakPosition(lastEndPos+1);
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
            //child->SetPosition(wxPoint(0, 0));

            // Lays out the object first with a given amount of space, and then if no width was specified in attr,
            // lays out the object again using the minimum size
            // The position will be determined by its location in its line,
            // and not by the child's actual position.
            child->LayoutToBestSize(dc, context, buffer,
                    attr, child->GetAttributes(), availableRect, parentRect, style);

            if (oldSize != child->GetCachedSize())
            {
                partialExtents.Clear();

                // Recalculate the partial text extents since the child object changed size
                GetRangeSize(GetRange(), paraSize, paraDescent, dc, context, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_CACHE_SIZE, wxPoint(0,0), parentRect.GetSize(), & partialExtents);
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
            GetRangeSize(wxRichTextRange(lastEndPos+1, lastPosToUse), childSize, childDescent, dc, context, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_HEIGHT_ONLY, wxPoint(0,0), parentRect.GetSize());
            childSize.x = wxRichTextGetRangeWidth(*this, wxRichTextRange(lastEndPos+1, lastPosToUse), partialExtents);
#else
            GetRangeSize(wxRichTextRange(lastEndPos+1, lastPosToUse), childSize, childDescent, dc, context, wxRICHTEXT_UNFORMATTED, rect.GetPosition(), parentRect.GetSize());
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
            // Note: the floating objects may be placed vertically along the two sides of
            // buffer, so we may have different available line widths with different
            // [startY, endY]. So, we can't determine how wide the available
            // space is until we know the exact line height.
            if (childDescent == 0)
            {
                lineHeight = wxMax(lineHeight, childSize.y);
                lineDescent = maxDescent;
                lineAscent = maxAscent;
            }
            else
            {
                lineDescent = wxMax(childDescent, maxDescent);
                lineAscent = wxMax(childSize.y-childDescent, maxAscent);
            }
            lineHeight = wxMax(lineHeight, (lineDescent + lineAscent));

            if (wxRichTextBuffer::GetFloatingLayoutMode() && collector)
            {
                // Adjust availableRect to the space that is available when taking floating objects into account.
                wxRect floatAvailableRect = collector->GetAvailableRect(rect.y + currentPosition.y, rect.y + currentPosition.y + lineHeight);
                int x1 = wxMax(availableRect.x, (floatAvailableRect.x + startOffset));
                int x2 = wxMin(availableRect.GetRight(), (floatAvailableRect.GetRight() - rightIndent));
                oldAvailableRect = availableRect;
                availableRect.x = x1;
                availableRect.width = x2 - x1 + 1;
            }

            currentPosition.x = availableRect.x - rect.x;

            if (child->IsTopLevel() && loopIterations <= 20)
            {
                if (availableRect != oldAvailableRect)
                {
                    wxSize oldSize = child->GetCachedSize();

                    // Lays out the object first with a given amount of space, and then if no width was specified in attr,
                    // lays out the object again using the minimum size
                    child->Invalidate(wxRICHTEXT_ALL);
                    child->LayoutToBestSize(dc, context, buffer,
                                attr, child->GetAttributes(), availableRect, parentRect.GetSize(), style);
                    childSize = child->GetCachedSize();
                    childDescent = child->GetDescent();

                    if (oldSize != child->GetCachedSize())
                    {
                        partialExtents.Clear();

                        // Recalculate the partial text extents since the child object changed size
                        GetRangeSize(GetRange(), paraSize, paraDescent, dc, context, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_CACHE_SIZE, wxPoint(0,0), parentRect.GetSize(), & partialExtents);
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

        // 2014-03-08: also need to set object positions
        //if (child->IsTopLevel())
        {
            // We can move it to the correct position at this point
            // TODO: probably need to add margin
            child->Move(GetPosition() + wxPoint(currentWidth + startOffset, currentPosition.y));
        }

        // Cases:
        // 1) There was a line break BEFORE the natural break
        // 2) There was a line break AFTER the natural break
        // 3) It's the last line
        // 4) The child still fits (carry on) - 'else' clause

        if ((lineBreakInThisObject && (childSize.x + currentWidth <= availableRect.width))
            ||
            (childSize.x + currentWidth > availableRect.width)
#if 0
            ||
            ((childSize.x + currentWidth <= availableRect.width) && !node->GetNext())
#endif
            )
        {
            long wrapPosition = 0;
            if ((childSize.x + currentWidth <= availableRect.width) && !node->GetNext() && !lineBreakInThisObject)
                wrapPosition = child->GetRange().GetEnd();
            else

            // Find a place to wrap. This may walk back to previous children,
            // for example if a word spans several objects.
            // Note: one object must contains only one wxTextAtrr, so the line height will not
            //       change inside one object. Thus, we can pass the remain line width to the
            //       FindWrapPosition function.
            if (!FindWrapPosition(wxRichTextRange(lastCompletedEndPos+1, child->GetRange().GetEnd()), dc, context, availableRect.width, wrapPosition, & partialExtents))
            {
                // If the function failed, just cut it off at the end of this child.
                wrapPosition = child->GetRange().GetEnd();
            }

            // FindWrapPosition can still return a value that will put us in an endless wrapping loop
            if (wrapPosition <= lastCompletedEndPos)
                wrapPosition = wxMax(lastCompletedEndPos+1,child->GetRange().GetEnd());

            // Line end position shouldn't be the same as the end, or greater.
            if (wrapPosition >= GetRange().GetEnd())
                wrapPosition = wxMax(0, GetRange().GetEnd()-1);

            // wxLogDebug(wxT("Split at %ld"), wrapPosition);

            // Let's find the actual size of the current line now
            wxSize actualSize;
            wxRichTextRange actualRange(lastCompletedEndPos+1, wrapPosition);

            childDescent = 0;

#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
            if (!child->IsEmpty())
            {
                // Get height only, then the width using the partial extents
                GetRangeSize(actualRange, actualSize, childDescent, dc, context, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_HEIGHT_ONLY, wxPoint(0,0), parentRect.GetSize());
                actualSize.x = wxRichTextGetRangeWidth(*this, actualRange, partialExtents);
            }
            else
#endif
                GetRangeSize(actualRange, actualSize, childDescent, dc, context, wxRICHTEXT_UNFORMATTED, wxPoint(0,0), parentRect.GetSize());

            currentWidth = actualSize.x;

            // The descent for the whole line at this point, is the correct max descent
            maxDescent = childDescent;

            // lineHeight is given by the height for the whole line, since it will
            // take into account ascend/descend.
            lineHeight = actualSize.y;

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
            maxWidth = wxMax(maxWidth, currentWidth+currentPosition.x);
            currentWidth = 0;

            lineCount ++;

            // TODO: account for zero-length objects
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

            if (childDescent == 0)
            {
                // An object with a zero descend value wants to take up the whole
                // height regardless of baseline
                lineHeight = wxMax(lineHeight, childSize.y);
            }
            else
            {
                maxDescent = wxMax(childDescent, maxDescent);
                maxAscent = wxMax(childSize.y-childDescent, maxAscent);
            }

            lineHeight = wxMax(lineHeight, (maxDescent + maxAscent));

            maxWidth = wxMax(maxWidth, currentWidth+currentPosition.x);
            lastEndPos = child->GetRange().GetEnd();

            node = node->GetNext();
        }
    }

    //wxASSERT(!(lastCompletedEndPos != -1 && lastCompletedEndPos < GetRange().GetEnd()-1));

    // Add the last line - it's the current pos -> last para pos
    // Subtract -1 because the last position is always the end-paragraph position.
    if ((lastCompletedEndPos < GetRange().GetEnd()-1) || lineCount == 0)
    {
        int startOffset = (lineCount == 0 ? startPositionFirstLine : startPositionSubsequentLines);
        availableRect = wxRect(rect.x + startOffset, rect.y + currentPosition.y,
                                     rect.width - startOffset - rightIndent, rect.height);

        // Take into account floating objects for the last line
        if (wxRichTextBuffer::GetFloatingLayoutMode() && collector)
        {
            // Adjust availableRect to the space that is available when taking floating objects into account.
            wxRect floatAvailableRect = collector->GetAvailableRect(rect.y + currentPosition.y, rect.y + currentPosition.y + lineHeight);
            int x1 = wxMax(availableRect.x, (floatAvailableRect.x + startOffset));
            int x2 = wxMin(availableRect.GetRight(), (floatAvailableRect.GetRight() - rightIndent));
            availableRect.x = x1;
            availableRect.width = x2 - x1 + 1;
        }

        currentPosition.x = availableRect.x - rect.x;

        wxRichTextLine* line = AllocateLine(lineCount);

        wxRichTextRange actualRange(lastCompletedEndPos+1, GetRange().GetEnd()-1);

        // Set relative range so we won't have to change line ranges when paragraphs are moved
        line->SetRange(wxRichTextRange(actualRange.GetStart() - GetRange().GetStart(), actualRange.GetEnd() - GetRange().GetStart()));

        line->SetPosition(currentPosition);

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

        line->SetSize(wxSize(currentWidth, lineHeight));
        line->SetDescent(maxDescent);
        currentPosition.y += lineHeight;
        currentPosition.y += lineSpacing;
        lineCount ++;

        // Apply paragraph styles such as alignment to the wrapped line
        ApplyParagraphStyle(line, attr, availableRect, dc);
    }

    // Remove remaining unused line objects, if any
    ClearUnusedLines(lineCount);

    // We need to add back the margins etc.
    {
        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(maxWidth, currentPosition.y + spaceAfterPara));
        GetBoxRects(dc, buffer, attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);
        SetCachedSize(marginRect.GetSize());
    }

    // The maximum size is the length of the paragraph stretched out into a line.
    // So if there were a single word, or an image, or a fixed-size text box, the object could be shrunk around
    // this size. TODO: take into account line breaks.
    {
        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(paraSize.x + wxMax(leftIndent, leftIndent + leftSubIndent) + rightIndent, currentPosition.y + spaceAfterPara));
        GetBoxRects(dc, buffer, attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);
        SetMaxSize(wxSize(wxMax(GetCachedSize().x, marginRect.GetSize().x), wxMax(GetCachedSize().y, marginRect.y)));
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
            if ((!child->IsFloating() || !wxRichTextBuffer::GetFloatingLayoutMode()) && child->GetRange().GetLength() != 0 && !wxDynamicCast(child, wxRichTextPlainText))
            {
                if (child->GetCachedSize().x > minWidth)
                    minWidth = child->GetMinSize().x;
            }
            node = node->GetNext();
        }

        {
            // Give the minimum width at least one character width
            wxFont font(buffer->GetFontTable().FindFont(attr));
            wxCheckSetFont(dc, font);
            int charWidth = dc.GetCharWidth();
            minWidth = wxMax(charWidth, minWidth);
        }

        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(minWidth, currentPosition.y + spaceAfterPara));
        GetBoxRects(dc, buffer, attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);
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
void wxRichTextParagraph::ApplyParagraphStyle(wxRichTextLine* line, const wxRichTextAttr& attr, const wxRect& rect, wxDC& WXUNUSED(dc))
{
    if (!attr.HasAlignment())
        return;

    wxPoint pos = line->GetPosition();
    wxPoint originalPos = pos;
    wxSize size = line->GetSize();

    // centering, right-justification
    if (attr.HasAlignment() && attr.GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
    {
        pos.x = (rect.GetWidth() - size.x)/2 + pos.x;
        line->SetPosition(pos);
    }
    else if (attr.HasAlignment() && attr.GetAlignment() == wxTEXT_ALIGNMENT_RIGHT)
    {
        pos.x = pos.x + rect.GetWidth() - size.x;
        line->SetPosition(pos);
    }

    if (pos != originalPos)
    {
        wxPoint inc = pos - originalPos;

        wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();

        while (node)
        {
            wxRichTextObject* child = node->GetData();
            if (child->IsTopLevel() && !child->GetRange().IsOutside(line->GetAbsoluteRange()))
                child->Move(child->GetPosition() + inc);

            node = node->GetNext();
        }
    }
}

/// Insert text at the given position
bool wxRichTextParagraph::InsertText(long pos, const wxString& text)
{
    wxRichTextObject* childToUse = nullptr;
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

            wxRichTextObjectList::compatibility_iterator childNode = nodeToUse->GetNext();
            while (childNode)
            {
                wxRichTextObject* child = childNode->GetData();
                child->SetRange(wxRichTextRange(textObject->GetRange().GetStart() + textLength,
                                                 textObject->GetRange().GetEnd() + textLength));

                childNode = childNode->GetNext();
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
    m_impactedByFloatingObjects = obj.m_impactedByFloatingObjects;
}

/// Clear the cached lines
void wxRichTextParagraph::ClearLines()
{
    size_t cachedLineCount = m_cachedLines.size();
    for (size_t i = 0; i < cachedLineCount; i++)
        delete m_cachedLines[i];
    m_cachedLines.clear();
}

/// Get/set the object size for the given range. Returns false if the range
/// is invalid for this object.
bool wxRichTextParagraph::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, wxRichTextDrawingContext& context, int flags, const wxPoint& position, const wxSize& parentSize, wxArrayInt* partialExtents) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    if (flags & wxRICHTEXT_UNFORMATTED)
    {
        // Just use unformatted data, assume no line breaks
        wxSize sz;

        wxArrayInt childExtents;
        wxArrayInt* p;
        if (partialExtents)
            p = & childExtents;
        else
            p = nullptr;

        int maxDescent = 0;
        int maxAscent = 0;
        int maxLineHeight = 0;

        wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
        while (node)
        {
            wxRichTextObject* child = node->GetData();
            if (!child->GetRange().IsOutside(range))
            {
                // Floating objects have a zero size within the paragraph.
                if (child->IsFloating() && wxRichTextBuffer::GetFloatingLayoutMode())
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
                    int childDescent = 0;

                    // At present wxRICHTEXT_HEIGHT_ONLY is only fast if we've already cached the size,
                    // but it's only going to be used after caching has taken place.
                    if ((flags & wxRICHTEXT_HEIGHT_ONLY) && child->GetCachedSize().y != 0)
                    {
                        childDescent = child->GetDescent();
                        childSize = child->GetCachedSize();

                        if (childDescent == 0)
                        {
                            maxLineHeight = wxMax(maxLineHeight, childSize.y);
                        }
                        else
                        {
                            maxDescent = wxMax(maxDescent, childDescent);
                            maxAscent = wxMax(maxAscent, (childSize.y - childDescent));
                        }

                        maxLineHeight = wxMax(maxLineHeight, (maxAscent + maxDescent));

                        sz.y = wxMax(sz.y, maxLineHeight);
                        sz.x += childSize.x;
                        descent = maxDescent;
                    }
                    else if (child->IsTopLevel())
                    {
                        childDescent = child->GetDescent();
                        childSize = child->GetCachedSize();

                        if (childDescent == 0)
                        {
                            maxLineHeight = wxMax(maxLineHeight, childSize.y);
                        }
                        else
                        {
                            maxDescent = wxMax(maxDescent, childDescent);
                            maxAscent = wxMax(maxAscent, (childSize.y - childDescent));
                        }

                        maxLineHeight = wxMax(maxLineHeight, (maxAscent + maxDescent));

                        sz.y = wxMax(sz.y, maxLineHeight);
                        sz.x += childSize.x;
                        descent = maxDescent;

                        // FIXME: this won't change the original values.
                        // Should we be calling GetRangeSize above instead of using cached values?
#if 0
                        if ((flags & wxRICHTEXT_CACHE_SIZE) && (rangeToUse == child->GetRange()))
                        {
                            child->SetCachedSize(childSize);
                            child->SetDescent(childDescent);
                        }
#endif

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
                    else if (child->GetRangeSize(rangeToUse, childSize, childDescent, dc, context, flags, wxPoint(position.x + sz.x, position.y), parentSize, p))
                    {
                        if (childDescent == 0)
                        {
                            maxLineHeight = wxMax(maxLineHeight, childSize.y);
                        }
                        else
                        {
                            maxDescent = wxMax(maxDescent, childDescent);
                            maxAscent = wxMax(maxAscent, (childSize.y - childDescent));
                        }

                        maxLineHeight = wxMax(maxLineHeight, (maxAscent + maxDescent));

                        sz.y = wxMax(sz.y, maxLineHeight);
                        sz.x += childSize.x;
                        descent = maxDescent;

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

        wxRichTextLineVector::const_iterator it = m_cachedLines.begin();
        while (it != m_cachedLines.end())
        {
            wxRichTextLine* line = *it;
            wxRichTextRange lineRange = line->GetAbsoluteRange();
            if (!lineRange.IsOutside(range))
            {
                int maxDescent = 0;
                int maxAscent = 0;
                int maxLineHeight = 0;
                int maxLineWidth = 0;

                wxRichTextObjectList::compatibility_iterator node2 = m_children.GetFirst();
                while (node2)
                {
                    wxRichTextObject* child = node2->GetData();

                    if ((!child->IsFloating() || !wxRichTextBuffer::GetFloatingLayoutMode()) && !child->GetRange().IsOutside(lineRange))
                    {
                        wxRichTextRange rangeToUse = lineRange;
                        rangeToUse.LimitTo(child->GetRange());
                        if (child->IsTopLevel())
                            rangeToUse = child->GetOwnRange();

                        wxSize childSize;
                        int childDescent = 0;
                        if (child->GetRangeSize(rangeToUse, childSize, childDescent, dc, context, flags, wxPoint(position.x + sz.x, position.y), parentSize))
                        {
                            if (childDescent == 0)
                            {
                                // Assume that if descent is zero, this child can occupy the full line height
                                // and does not need space for the line's maximum descent. So we influence
                                // the overall max line height only.
                                maxLineHeight = wxMax(maxLineHeight, childSize.y);
                            }
                            else
                            {
                                maxAscent = wxMax(maxAscent, (childSize.y - childDescent));
                                maxDescent = wxMax(maxAscent, childDescent);
                            }
                            maxLineHeight = wxMax(maxLineHeight, (maxAscent + maxDescent));
                            maxLineWidth += childSize.x;
                        }
                    }

                    node2 = node2->GetNext();
                }

                descent = wxMax(descent, maxDescent);

                // Increase size by a line (TODO: paragraph spacing)
                sz.y += maxLineHeight;
                sz.x = wxMax(sz.x, maxLineWidth);
            }
            ++it;
        }
        size = sz;
    }
    return true;
}

/// Finds the absolute position and row height for the given character position
bool wxRichTextParagraph::FindPosition(wxDC& dc, wxRichTextDrawingContext& context, long index, wxPoint& pt, int* height, bool forceLineStart)
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
        wxASSERT( parent != nullptr );

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

    wxRichTextLineVector::const_iterator it = m_cachedLines.begin();
    while (it != m_cachedLines.end())
    {
        wxRichTextLine* line = *it;
        wxRichTextRange lineRange = line->GetAbsoluteRange();
        if (index >= lineRange.GetStart() && index <= lineRange.GetEnd())
        {
            // If this is the last point in the line, and we're forcing the
            // returned value to be the start of the next line, do the required
            // thing.
            if (index == lineRange.GetEnd() && forceLineStart)
            {
                wxRichTextLineVector::const_iterator itNext = it;
                ++itNext;

                if (itNext != m_cachedLines.end())
                {
                    wxRichTextLine* nextLine = *itNext;
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

            if (GetRangeSize(r, rangeSize, descent, dc, context, wxRICHTEXT_UNFORMATTED, line->GetPosition()+ GetPosition()))
            {
                pt.x = line->GetPosition().x + GetPosition().x + rangeSize.x;
                *height = line->GetSize().y;

                return true;
            }

        }

        ++it;
    }

    return false;
}

/// Hit-testing: returns a flag indicating hit test details, plus
/// information about position
int wxRichTextParagraph::HitTest(wxDC& dc, wxRichTextDrawingContext& context, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags)
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
        if (GetParent() && GetParent()->wxRichTextObject::HitTest(dc, context, pt, tmpPos, & tempObj, & tempContextObj, flags) == wxRICHTEXT_HITTEST_NONE)
            return wxRICHTEXT_HITTEST_NONE;
    }

    wxRichTextObjectList::compatibility_iterator objNode = m_children.GetFirst();
    while (objNode)
    {
        wxRichTextObject* child = objNode->GetData();
        // Don't recurse if we have wxRICHTEXT_HITTEST_NO_NESTED_OBJECTS,
        // and also, if this seems composite but actually is marked as atomic,
        // don't recurse.
        if (child->IsTopLevel() && ((flags & wxRICHTEXT_HITTEST_NO_NESTED_OBJECTS) == 0) &&
            (! (((flags & wxRICHTEXT_HITTEST_HONOUR_ATOMIC) != 0) && child->IsAtomic())))
        {
            {
                int hitTest = child->HitTest(dc, context, pt, textPosition, obj, contextObj);
                if (hitTest != wxRICHTEXT_HITTEST_NONE)
                    return hitTest;
            }
        }

        objNode = objNode->GetNext();
    }

    wxPoint paraPos = GetPosition();

    wxRichTextLineVector::const_iterator it = m_cachedLines.begin();
    while (it != m_cachedLines.end())
    {
        wxRichTextLine* line = *it;
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
                GetRangeSize(lineRange, paraSize, paraDescent, dc, context, wxRICHTEXT_UNFORMATTED, linePos, wxDefaultSize, & partialExtents);

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

                    GetRangeSize(rangeToUse, childSize, descent, dc, context, wxRICHTEXT_UNFORMATTED, linePos);

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

        ++it;
    }

    return wxRICHTEXT_HITTEST_NONE;
}

/// Split an object at this position if necessary, and return
/// the previous object, or nullptr if inserting at beginning.
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
                    *previousObject = nullptr;
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
        *previousObject = nullptr;
    return nullptr;
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
    return nullptr;
}

/// Get the plain text searching from the start or end of the range.
/// The resulting string may be shorter than the range given.
bool wxRichTextParagraph::GetContiguousPlainText(wxString& text, const wxRichTextRange& range, bool fromStart)
{
    text.clear();

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
bool wxRichTextParagraph::FindWrapPosition(const wxRichTextRange& range, wxDC& dc, wxRichTextDrawingContext& context, int availableSpace, long& wrapPosition, wxArrayInt* partialExtents)
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
                GetRangeSize(wxRichTextRange(range.GetStart(), minPos), sz, descent, dc, context, wxRICHTEXT_UNFORMATTED);

                if (sz.x > availableSpace)
                    breakPosition = minPos - 1;
                break;
            }
            else if ((maxPos - minPos) == 1)
            {
                int descent = 0;
                GetRangeSize(wxRichTextRange(range.GetStart(), minPos), sz, descent, dc, context, wxRICHTEXT_UNFORMATTED);

                if (sz.x > availableSpace)
                    breakPosition = minPos - 1;
                else
                {
                    GetRangeSize(wxRichTextRange(range.GetStart(), maxPos), sz, descent, dc, context, wxRICHTEXT_UNFORMATTED);
                    if (sz.x > availableSpace)
                        breakPosition = maxPos-1;
                }
                break;
            }
            else
            {
                long nextPos = minPos + ((maxPos - minPos) / 2);

                int descent = 0;
                GetRangeSize(wxRichTextRange(range.GetStart(), nextPos), sz, descent, dc, context, wxRICHTEXT_UNFORMATTED);

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
    if (pos < (int) m_cachedLines.size())
    {
        wxRichTextLine* line = m_cachedLines[pos];
        line->Init(this);
        return line;
    }
    else
    {
        wxRichTextLine* line = new wxRichTextLine(this);
        m_cachedLines.push_back(line);
        return line;
    }
}

/// Clear remaining unused line objects, if any
bool wxRichTextParagraph::ClearUnusedLines(int lineCount)
{
    size_t cachedLineCount = m_cachedLines.size();

    if ((size_t)lineCount < cachedLineCount)
    {
        for (size_t i = lineCount; i < cachedLineCount; i++)
            delete m_cachedLines[i];
        m_cachedLines.resize(lineCount);
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
            if (attr.HasBackgroundColour())
            {
                attr.SetBackgroundColour(wxColour());
                attr.SetFlags(attr.GetFlags() & ~wxTEXT_ATTR_BACKGROUND_COLOUR);
            }
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
        {
            attr.GetTextBoxAttr().Reset();
            if (attr.HasBackgroundColour())
            {
                attr.SetBackgroundColour(wxColour());
                attr.SetFlags(attr.GetFlags() & ~wxTEXT_ATTR_BACKGROUND_COLOUR);
            }
        }
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

void wxRichTextParagraph::LayoutFloat(wxDC& dc, wxRichTextDrawingContext& context, const wxRect& rect, const wxRect& parentRect, int style, wxRichTextFloatCollector* floatCollector)
{
    wxTextAttrDimensionConverter converter(dc, GetBuffer() ? GetBuffer()->GetScale() : 1.0, parentRect.GetSize());

    wxRichTextObjectList::compatibility_iterator node = GetChildren().GetFirst();
    while (node)
    {
        wxRichTextObject* anchored = node->GetData();
        if (anchored && anchored->IsFloating() && !floatCollector->HasFloat(anchored))
        {
            int x = 0;
            wxRichTextAttr parentAttr(GetAttributes());
            AdjustAttributes(parentAttr, context);
#if 1
            // 27-09-2012
            wxRect availableSpace = GetParent()->GetAvailableContentArea(dc, context, rect);

            anchored->LayoutToBestSize(dc, context, GetBuffer(),
                parentAttr, anchored->GetAttributes(),
                parentRect, availableSpace,
                style);
            wxSize size = anchored->GetCachedSize();
#else
            wxSize size;
            int descent = 0;
            anchored->GetRangeSize(anchored->GetRange(), size, descent, dc, context, style);
#endif

            int offsetY = 0;
            if (anchored->GetAttributes().GetTextBoxAttr().GetTop().IsValid())
                offsetY = converter.GetPixels(anchored->GetAttributes().GetTextBoxAttr().GetTop(), wxVERTICAL);

            int pos = floatCollector->GetFitPosition(anchored->GetAttributes().GetTextBoxAttr().GetFloatMode(), rect.y + offsetY, size.y);

            // I can't remember why we tried to update the top offset here, but anyhow it results in
            // a wrong position being computed, so don't.
#if 0
            /* Update the offset */
            int newOffsetY = pos - rect.y;
            if (newOffsetY != offsetY)
            {
                if (!anchored->GetAttributes().GetTextBoxAttr().GetTop().IsValid() || anchored->GetAttributes().GetTextBoxAttr().GetTop().GetUnits() == wxTEXT_ATTR_UNITS_PIXELS)
                {
                    // We unscaled in GetPixels, so apply scale again.
                    anchored->GetAttributes().GetTextBoxAttr().GetTop().SetValue(int((double(newOffsetY) * converter.GetScale()) + 0.5), wxTEXT_ATTR_UNITS_PIXELS);
                }
                else
                {
                    newOffsetY = converter.ConvertPixelsToTenthsMM(newOffsetY);
                    anchored->GetAttributes().GetTextBoxAttr().GetTop().SetValue(newOffsetY, wxTEXT_ATTR_UNITS_TENTHS_MM);
                }
            }
#endif
            if (anchored->GetAttributes().GetTextBoxAttr().GetFloatMode() == wxTEXT_BOX_ATTR_FLOAT_LEFT)
                x = rect.x;
            else if (anchored->GetAttributes().GetTextBoxAttr().GetFloatMode() == wxTEXT_BOX_ATTR_FLOAT_RIGHT)
                x = rect.x + rect.width - size.x;

            //anchored->SetPosition(wxPoint(x, pos));
            anchored->Move(wxPoint(x, pos)); // should move children
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

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextPlainText, wxRichTextObject);

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
bool wxRichTextPlainText::Draw(wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int WXUNUSED(style))
{
    wxRichTextParagraph* para = wxDynamicCast(GetParent(), wxRichTextParagraph);
    wxASSERT (para != nullptr);

    wxRichTextAttr textAttr(para ? para->GetCombinedAttributes(GetAttributes(), false /* no box attributes */) : GetAttributes());
    AdjustAttributes(textAttr, context);

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

    const bool allSelected = selectionRange.GetStart() <= range.GetStart() && selectionRange.GetEnd() >= range.GetEnd();
    const bool noneSelected = selectionRange.GetEnd() < range.GetStart() || selectionRange.GetStart() > range.GetEnd();

    wxString stringChunk;
    wxString stringWhole;

    // If nothing or everything is selected, our algorithm does not need stringWhole. It
    // is enough to preprocess stringChunk only.
    // In case of partial selection we need to preprocess stringWhole too.
    if (allSelected || noneSelected)
    {
        const wxString* pWholeString = &m_text;
        if (context.HasVirtualText(this))
        {
            if (context.GetVirtualText(this, stringWhole) && stringWhole.length() == m_text.length())
                pWholeString = &stringWhole;
        }

        long len = range.GetLength();
        stringChunk = pWholeString->Mid(range.GetStart() - offset, (size_t) len);

        // Replace line break characters with spaces
        wxString toRemove = wxRichTextLineBreakChar;
        stringChunk.Replace(toRemove, wxT(" "));
        if (textAttr.HasTextEffects() &&
            (textAttr.GetTextEffects() & (wxTEXT_ATTR_EFFECT_CAPITALS|wxTEXT_ATTR_EFFECT_SMALL_CAPITALS)))
        {
            stringChunk.MakeUpper();
        }
    }
    else
    {
        stringWhole = m_text;
        if (context.HasVirtualText(this))
        {
            if (!context.GetVirtualText(this, stringWhole) || stringWhole.length() != m_text.length())
                stringWhole = m_text;
        }

        // Replace line break characters with spaces
        wxString toRemove = wxRichTextLineBreakChar;
        stringWhole.Replace(toRemove, wxT(" "));
        if (textAttr.HasTextEffects() &&
            (textAttr.GetTextEffects() & (wxTEXT_ATTR_EFFECT_CAPITALS|wxTEXT_ATTR_EFFECT_SMALL_CAPITALS)))
        {
            stringWhole.MakeUpper();
        }

        long len = range.GetLength();
        stringChunk = stringWhole.Mid(range.GetStart() - offset, (size_t) len);
    }

    // Test for the optimized situations where all is selected, or none
    // is selected.

    wxFont textFont(GetBuffer()->GetFontTable().FindFont(textAttr));
    wxCheckSetFont(dc, textFont);
    int charHeight = dc.GetCharHeight();

    int x, y;
    if ( textFont.IsOk() )
    {
        if (textAttr.HasTextEffects() && (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SMALL_CAPITALS))
        {
            textFont.SetFractionalPointSize(textFont.GetFractionalPointSize()*0.75);
            wxCheckSetFont(dc, textFont);
            charHeight = dc.GetCharHeight();
        }

        if ( textAttr.HasTextEffects() && (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUPERSCRIPT) )
        {
            if (textFont.IsUsingSizeInPixels())
            {
                double size = static_cast<double>(textFont.GetPixelSize().y) / wxSCRIPT_MUL_FACTOR;
                textFont.SetPixelSize(wxSize(0, static_cast<int>(size)));
                x = rect.x;
                y = rect.y;
            }
            else
            {
                textFont.SetFractionalPointSize(textFont.GetFractionalPointSize() / wxSCRIPT_MUL_FACTOR);
                x = rect.x;
                y = rect.y;
            }
            wxCheckSetFont(dc, textFont);
        }
        else if ( textAttr.HasTextEffects() && (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUBSCRIPT) )
        {
            if (textFont.IsUsingSizeInPixels())
            {
                double size = static_cast<double>(textFont.GetPixelSize().y) / wxSCRIPT_MUL_FACTOR;
                textFont.SetPixelSize(wxSize(0, static_cast<int>(size)));
                x = rect.x;
                int sub_height = static_cast<int>(static_cast<double>(charHeight) / wxSCRIPT_MUL_FACTOR);
                y = rect.y + (rect.height - sub_height + (descent - m_descent));
            }
            else
            {
                textFont.SetFractionalPointSize(textFont.GetFractionalPointSize() / wxSCRIPT_MUL_FACTOR);
                x = rect.x;
                int sub_height = static_cast<int>(static_cast<double>(charHeight) / wxSCRIPT_MUL_FACTOR);
                y = rect.y + (rect.height - sub_height + (descent - m_descent));
            }
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
    if (allSelected)
    {
        DrawTabbedString(dc, textAttr, rect, stringChunk, x, y, true);
    }
    // (b) None selected.
    else if (noneSelected)
    {
        // Draw all unselected
        DrawTabbedString(dc, textAttr, rect, stringChunk, x, y, false);
    }
    else
    {
        // (c) Part selected, part not
        // Let's draw unselected chunk, selected chunk, then unselected chunk.

        const wxString& str = stringWhole;

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
        int tabPos;
        tabPos = x + w;
        bool not_found = true;
        for (int i = 0; i < tabCount && not_found; ++i)
        {
            int nextTabPos;
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
bool wxRichTextPlainText::Layout(wxDC& dc, wxRichTextDrawingContext& context, const wxRect& WXUNUSED(rect), const wxRect& WXUNUSED(parentRect), int WXUNUSED(style))
{
    // Only lay out if we haven't already cached the size
    if (m_size.x == -1)
        GetRangeSize(GetRange(), m_size, m_descent, dc, context, 0, wxPoint(0, 0));
    m_maxSize = m_size;
    // Eventually we want to have a reasonable estimate of minimum size.
    m_minSize = wxSize(0, 0);
    return true;
}

// Adjusts the attributes for virtual attribute provision, collapsed borders, etc.
bool wxRichTextPlainText::AdjustAttributes(wxRichTextAttr& attr, wxRichTextDrawingContext& context)
{
    wxRichTextObject::AdjustAttributes(attr, context);

    if (!attr.HasTextColour())
    {
        wxRichTextBuffer* buf = GetBuffer();
        if (buf && buf->GetBasicStyle().HasTextColour())
            attr.SetTextColour(buf->GetBasicStyle().GetTextColour());
        else
            attr.SetTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    }

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
bool wxRichTextPlainText::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, wxRichTextDrawingContext& context, int WXUNUSED(flags), const wxPoint& position, const wxSize& WXUNUSED(parentSize), wxArrayInt* partialExtents) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    wxRichTextParagraph* para = wxDynamicCast(GetParent(), wxRichTextParagraph);
    wxASSERT (para != nullptr);

    int relativeX = position.x - GetParent()->GetPosition().x;

    wxRichTextAttr textAttr(para ? para->GetCombinedAttributes(GetAttributes()) : GetAttributes());
    const_cast<wxRichTextPlainText*>(this)->AdjustAttributes(textAttr, context);

    // Always assume unformatted text, since at this level we have no knowledge
    // of line breaks - and we don't need it, since we'll calculate size within
    // formatted text by doing it in chunks according to the line ranges

    bool bScript(false);
    wxFont font(GetBuffer()->GetFontTable().FindFont(textAttr));
    if (font.IsOk())
    {
        if ( textAttr.HasTextEffects() && ( (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUPERSCRIPT)
            || (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUBSCRIPT) ) )
        {
            wxFont textFont = font;
            if (textFont.IsUsingSizeInPixels())
            {
                double textSize = static_cast<double>(textFont.GetPixelSize().y) / wxSCRIPT_MUL_FACTOR;
                textFont.SetPixelSize(wxSize(0, static_cast<int>(textSize)));
            }
            else
            {
                textFont.SetFractionalPointSize(textFont.GetFractionalPointSize() / wxSCRIPT_MUL_FACTOR);
            }
            wxCheckSetFont(dc, textFont);
            bScript = true;
        }
        else if (textAttr.HasTextEffects() && (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SMALL_CAPITALS))
        {
            wxFont textFont = font;
            textFont.SetFractionalPointSize(textFont.GetFractionalPointSize()*0.75);
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

    wxString stringChunk;

    {
        // We don't need stringWhole. Only prepare stringChunk.
        wxString stringWhole;
        const wxString* pWholeString = &m_text;
        if (context.HasVirtualText(this))
        {
            if (context.GetVirtualText(this, stringWhole) && stringWhole.length() == m_text.length())
                pWholeString = &stringWhole;
        }

        long len = range.GetLength();
        stringChunk = pWholeString->Mid(startPos, (size_t) len);

        // Replace line break characters with spaces
        wxString toRemove = wxRichTextLineBreakChar;
        stringChunk.Replace(toRemove, wxT(" "));
        if (textAttr.HasTextEffects() &&
            (textAttr.GetTextEffects() & (wxTEXT_ATTR_EFFECT_CAPITALS|wxTEXT_ATTR_EFFECT_SMALL_CAPITALS)))
        {
            stringChunk.MakeUpper();
        }
    }

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
            pos = const_cast<wxRichTextPlainText*>(this)->ConvertTenthsMMToPixels(dc, pos);
            tabArray[i] = pos;
        }

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
                int nextTabPos;
                nextTabPos = tabArray.Item(i);

                // Find the next tab position.
                // Even if we're at the end of the tab array, we must still process the chunk.

                if (nextTabPos > absoluteWidth || (i == (tabCount - 1)))
                {
                    if (nextTabPos <= absoluteWidth)
                    {
                        int defaultTabWidth = const_cast<wxRichTextPlainText*>(this)->ConvertTenthsMMToPixels(dc, WIDTH_FOR_DEFAULT_TABS);
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
        return nullptr;

    wxString firstPart = m_text.Mid(0, index);
    wxString secondPart = m_text.Mid(index);

    m_text = firstPart;

    wxRichTextPlainText* newObject = new wxRichTextPlainText(secondPart);
    newObject->SetAttributes(GetAttributes());
    newObject->SetProperties(GetProperties());

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
bool wxRichTextPlainText::CanMerge(wxRichTextObject* object, wxRichTextDrawingContext& context) const
{
    // JACS 2013-01-27
    if (!context.GetVirtualAttributesEnabled())
    {
        return object->GetClassInfo() == wxCLASSINFO(wxRichTextPlainText) &&
            (m_text.empty() || (wxTextAttrEq(GetAttributes(), object->GetAttributes()) && m_properties == object->GetProperties()));
    }
    else
    {
        wxRichTextPlainText* otherObj = wxDynamicCast(object, wxRichTextPlainText);
        if (!otherObj || m_text.empty())
            return false;

        if (!wxTextAttrEq(GetAttributes(), object->GetAttributes()) || !(m_properties == object->GetProperties()))
            return false;

        // Check if differing virtual attributes makes it impossible to merge
        // these strings.

        bool hasVirtualAttr1 = context.HasVirtualAttributes(const_cast<wxRichTextPlainText*>(this));
        bool hasVirtualAttr2 = context.HasVirtualAttributes(object);
        if (!hasVirtualAttr1 && !hasVirtualAttr2)
            return true;
        else if (hasVirtualAttr1 != hasVirtualAttr2)
            return false;
        else
        {
            wxRichTextAttr virtualAttr1 = context.GetVirtualAttributes(const_cast<wxRichTextPlainText*>(this));
            wxRichTextAttr virtualAttr2 = context.GetVirtualAttributes(object);
            return virtualAttr1 == virtualAttr2;
        }
    }
}

/// Returns true if this object merged itself with the given one.
/// The calling code will then delete the given object.
bool wxRichTextPlainText::Merge(wxRichTextObject* object, wxRichTextDrawingContext& WXUNUSED(context))
{
    wxRichTextPlainText* textObject = wxDynamicCast(object, wxRichTextPlainText);
    wxASSERT( textObject != nullptr );

    if (textObject)
    {
        m_text += textObject->GetText();
        wxRichTextApplyStyle(m_attributes, textObject->GetAttributes());
        return true;
    }
    else
        return false;
}

bool wxRichTextPlainText::CanSplit(wxRichTextDrawingContext& context) const
{
    // If this object has any virtual attributes at all, whether for the whole object
    // or individual ones, we should try splitting it by calling Split.
    // Must be more than one character in order to be able to split.
    return m_text.length() > 1 && context.HasVirtualAttributes(const_cast<wxRichTextPlainText*>(this));
}

wxRichTextObject* wxRichTextPlainText::Split(wxRichTextDrawingContext& context)
{
    int count = context.GetVirtualSubobjectAttributesCount(this);
    if (count > 0 && GetParent())
    {
        wxRichTextCompositeObject* parent = wxDynamicCast(GetParent(), wxRichTextCompositeObject);
        wxRichTextObjectList::compatibility_iterator node = parent->GetChildren().Find(this);
        if (node)
        {
            const wxRichTextAttr emptyAttr;
            wxRichTextObjectList::compatibility_iterator next = node->GetNext();

            wxArrayInt positions;
            wxRichTextAttrArray attributes;
            if (context.GetVirtualSubobjectAttributes(this, positions, attributes) && positions.GetCount() > 0)
            {
                wxASSERT(positions.GetCount() == attributes.GetCount());

                // We will gather up runs of text with the same virtual attributes

                int len = m_text.length();
                int i = 0;

                // runStart and runEnd represent the accumulated run with a consistent attribute
                // that hasn't yet been appended
                int runStart = -1;
                int runEnd = -1;
                wxRichTextAttr currentAttr;
                wxString text = m_text;
                wxRichTextPlainText* lastPlainText = this;

                for (i = 0; i < (int) positions.GetCount(); i++)
                {
                    int pos = positions[i];
                    wxASSERT(pos >= 0 && pos < len);
                    if (pos >= 0 && pos < len)
                    {
                        const wxRichTextAttr& attr = attributes[i];

                        if (pos == 0)
                        {
                            runStart = 0;
                            currentAttr = attr;
                        }
                        // Check if there was a gap from the last known attribute and this.
                        // In that case, we need to do something with the span of non-attributed text.
                        else if ((pos-1) > runEnd)
                        {
                            if (runEnd == -1)
                            {
                                // We hadn't processed anything previously, so the previous run is from the text start
                                // to just before this position. The current attribute remains empty.
                                runStart = 0;
                                runEnd = pos-1;
                            }
                            else
                            {
                                // If the previous attribute matches the gap's attribute (i.e., no attributes)
                                // then just extend the run.
                                if (currentAttr.IsDefault())
                                {
                                    runEnd = pos-1;
                                }
                                else
                                {
                                    // We need to add an object, or reuse the existing one.
                                    if (runStart == 0)
                                    {
                                        lastPlainText = this;
                                        SetText(text.Mid(runStart, runEnd - runStart + 1));
                                    }
                                    else
                                    {
                                        wxRichTextPlainText* obj = new wxRichTextPlainText;
                                        lastPlainText = obj;
                                        obj->SetAttributes(GetAttributes());
                                        obj->SetProperties(GetProperties());
                                        obj->SetParent(parent);

                                        obj->SetText(text.Mid(runStart, runEnd - runStart + 1));
                                        if (next)
                                            parent->GetChildren().Insert(next, obj);
                                        else
                                            parent->GetChildren().Append(obj);
                                    }

                                    runStart = runEnd+1;
                                    runEnd = pos-1;

                                    currentAttr = emptyAttr;
                                }
                            }
                        }

                        wxASSERT(runEnd == pos-1);

                        // Now we only have to deal with the previous run
                        if (currentAttr == attr)
                        {
                            // If we still have the same attributes, then we
                            // simply increase the run size.
                            runEnd = pos;
                        }
                        else
                        {
                            if (runEnd >= 0)
                            {
                                // We need to add an object, or reuse the existing one.
                                if (runStart == 0)
                                {
                                    lastPlainText = this;
                                    SetText(text.Mid(runStart, runEnd - runStart + 1));
                                }
                                else
                                {
                                    wxRichTextPlainText* obj = new wxRichTextPlainText;
                                    lastPlainText = obj;
                                    obj->SetAttributes(GetAttributes());
                                    obj->SetProperties(GetProperties());
                                    obj->SetParent(parent);

                                    obj->SetText(text.Mid(runStart, runEnd - runStart + 1));
                                    if (next)
                                        parent->GetChildren().Insert(next, obj);
                                    else
                                        parent->GetChildren().Append(obj);
                                }
                            }

                            runStart = pos;
                            runEnd = pos;

                            currentAttr = attr;
                        }
                    }
                }

                // We may still have a run to add, and possibly a no-attribute text fragment after that.
                // If the whole string was already a single attribute (the run covers the whole string), don't split.
                if ((runStart != -1) && !(runStart == 0 && runEnd == (len-1)))
                {
                    // If the current attribute is empty, merge the run with the next fragment
                    // which by definition (because it's not specified) has empty attributes.
                    if (currentAttr.IsDefault())
                        runEnd = (len-1);

                    if (runEnd < (len-1))
                    {
                        // We need to add an object, or reuse the existing one.
                        if (runStart == 0)
                        {
                            lastPlainText = this;
                            SetText(text.Mid(runStart, runEnd - runStart + 1));
                        }
                        else
                        {
                            wxRichTextPlainText* obj = new wxRichTextPlainText;
                            lastPlainText = obj;
                            obj->SetAttributes(GetAttributes());
                            obj->SetProperties(GetProperties());
                            obj->SetParent(parent);

                            obj->SetText(text.Mid(runStart, runEnd - runStart + 1));
                            if (next)
                                parent->GetChildren().Insert(next, obj);
                            else
                                parent->GetChildren().Append(obj);
                        }

                        runStart = runEnd+1;
                        runEnd = (len-1);
                    }

                    // Now the last, non-attributed fragment at the end, if any
                    if ((runStart < len) && !(runStart == 0 && runEnd == (len-1)))
                    {
                        wxASSERT(runStart != 0);

                        wxRichTextPlainText* obj = new wxRichTextPlainText;
                        obj->SetAttributes(GetAttributes());
                        obj->SetProperties(GetProperties());
                        obj->SetParent(parent);

                        obj->SetText(text.Mid(runStart, runEnd - runStart + 1));
                        if (next)
                            parent->GetChildren().Insert(next, obj);
                        else
                            parent->GetChildren().Append(obj);

                        lastPlainText = obj;
                    }
                }

                return lastPlainText;
            }
        }
    }
    return this;
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

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextBuffer, wxRichTextParagraphLayoutBox);

wxList                      wxRichTextBuffer::sm_handlers;
wxList                      wxRichTextBuffer::sm_drawingHandlers;
wxRichTextFieldTypeHashMap  wxRichTextBuffer::sm_fieldTypes;
wxRichTextRenderer*         wxRichTextBuffer::sm_renderer = nullptr;
int                         wxRichTextBuffer::sm_bulletRightMargin = 20;
float                       wxRichTextBuffer::sm_bulletProportion = (float) 0.3;
bool                        wxRichTextBuffer::sm_floatingLayoutMode = true;

/// Initialisation
void wxRichTextBuffer::Init()
{
    m_commandProcessor = new wxCommandProcessor;
    m_styleSheet = nullptr;
    m_modified = false;
    m_batchedCommandDepth = 0;
    m_batchedCommand = nullptr;
    m_suppressUndo = 0;
    m_handlerFlags = 0;
    m_scale = 1.0;
    m_dimensionScale = 1.0;
    m_fontScale = 1.0;
    SetMargins(4);
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
    m_batchedCommand = nullptr;
    m_suppressUndo = obj.m_suppressUndo;
    m_invalidRange = obj.m_invalidRange;
    m_dimensionScale = obj.m_dimensionScale;
    m_fontScale = obj.m_fontScale;
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
        return nullptr;
}

/// Submit command to insert paragraphs
bool wxRichTextBuffer::InsertParagraphsWithUndo(long pos, const wxRichTextParagraphLayoutBox& paragraphs, wxRichTextCtrl* ctrl, int flags)
{
    return ctrl->GetFocusObject()->InsertParagraphsWithUndo(this, pos, paragraphs, ctrl, flags);
}

/// Submit command to insert paragraphs
bool wxRichTextParagraphLayoutBox::InsertParagraphsWithUndo(wxRichTextBuffer* buffer, long pos, const wxRichTextParagraphLayoutBox& paragraphs, wxRichTextCtrl* ctrl, int WXUNUSED(flags))
{
    wxRichTextAction* action = new wxRichTextAction(nullptr, _("Insert Text"), wxRICHTEXT_INSERT, buffer, this, ctrl, false);

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
    if (ctrl)
        return ctrl->GetFocusObject()->InsertTextWithUndo(this, pos, text, ctrl, flags);
    else
        return wxRichTextParagraphLayoutBox::InsertTextWithUndo(this, pos, text, ctrl, flags);
}

/// Submit command to insert the given text
bool wxRichTextParagraphLayoutBox::InsertTextWithUndo(wxRichTextBuffer* buffer, long pos, const wxString& text, wxRichTextCtrl* ctrl, int flags)
{
    wxRichTextAction* action = new wxRichTextAction(nullptr, _("Insert Text"), wxRICHTEXT_INSERT, buffer, this, ctrl, false);

    wxRichTextAttr* p = nullptr;
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

    action->SetPosition(pos);

    // Set the range we'll need to delete in Undo
    action->SetRange(wxRichTextRange(pos, pos + length - 1));

    buffer->SubmitAction(action);

    return true;
}

/// Submit command to insert the given text
bool wxRichTextBuffer::InsertNewlineWithUndo(long pos, wxRichTextCtrl* ctrl, int flags)
{
    return ctrl->GetFocusObject()->InsertNewlineWithUndo(this, pos, ctrl, flags);
}

/// Submit command to insert the given text
bool wxRichTextParagraphLayoutBox::InsertNewlineWithUndo(wxRichTextBuffer* buffer, long pos, wxRichTextCtrl* ctrl, int flags)
{
    wxRichTextAction* action = new wxRichTextAction(nullptr, _("Insert Text"), wxRICHTEXT_INSERT, buffer, this, ctrl, false);

    wxRichTextAttr* p = nullptr;
    wxRichTextAttr paraAttr;
    if (flags & wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE)
    {
        paraAttr = GetStyleForNewParagraph(buffer, pos, false, true /* look for next paragraph style */);
        if (!paraAttr.IsDefault())
            p = & paraAttr;
    }

    wxRichTextAttr attr(buffer->GetDefaultStyle());
    // Don't include box attributes such as margins
    attr.GetTextBoxAttr().Reset();

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
    if (!buffer->GetDefaultStyle().IsDefault() && newPara->GetChildren().GetFirst())
    {
        // Check whether the default style merely reflects the paragraph/basic style,
        // in which case don't apply it.
        wxRichTextAttr defaultStyle(buffer->GetDefaultStyle());
        defaultStyle.GetTextBoxAttr().Reset();
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
    return ctrl->GetFocusObject()->InsertImageWithUndo(this, pos, imageBlock, ctrl, flags, textAttr);
}

/// Submit command to insert the given image
bool wxRichTextParagraphLayoutBox::InsertImageWithUndo(wxRichTextBuffer* buffer, long pos, const wxRichTextImageBlock& imageBlock,
                                                        wxRichTextCtrl* ctrl, int flags,
                                                        const wxRichTextAttr& textAttr)
{
    wxRichTextAction* action = new wxRichTextAction(nullptr, _("Insert Image"), wxRICHTEXT_INSERT, buffer, this, ctrl, false);

    wxRichTextAttr* p = nullptr;
    wxRichTextAttr paraAttr;
    if (flags & wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE)
    {
        paraAttr = GetStyleForNewParagraph(buffer, pos);
        if (!paraAttr.IsDefault())
            p = & paraAttr;
    }

    wxRichTextAttr attr(buffer->GetDefaultStyle());

    // Don't include box attributes such as margins
    attr.GetTextBoxAttr().Reset();

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
    return ctrl->GetFocusObject()->InsertObjectWithUndo(this, pos, object, ctrl, flags);
}

// Insert an object with no change of it
wxRichTextObject* wxRichTextParagraphLayoutBox::InsertObjectWithUndo(wxRichTextBuffer* buffer, long pos, wxRichTextObject *object, wxRichTextCtrl* ctrl, int flags)
{
    wxRichTextAction* action = new wxRichTextAction(nullptr, _("Insert Object"), wxRICHTEXT_INSERT, buffer, this, ctrl, false);

    wxRichTextAttr* p = nullptr;
    wxRichTextAttr paraAttr;
    if (flags & wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE)
    {
        paraAttr = GetStyleForNewParagraph(buffer, pos);
        if (!paraAttr.IsDefault())
            p = & paraAttr;
    }

    wxRichTextAttr attr(buffer->GetDefaultStyle());

    // Don't include box attributes such as margins
    attr.GetTextBoxAttr().Reset();

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

wxRichTextField* wxRichTextParagraphLayoutBox::InsertFieldWithUndo(wxRichTextBuffer* buffer, long pos, const wxString& fieldType,
                                                        const wxRichTextProperties& properties,
                                                        wxRichTextCtrl* ctrl, int flags,
                                                        const wxRichTextAttr& textAttr)
{
    wxRichTextAction* action = new wxRichTextAction(nullptr, _("Insert Field"), wxRICHTEXT_INSERT, buffer, this, ctrl, false);

    wxRichTextAttr* p = nullptr;
    wxRichTextAttr paraAttr;
    if (flags & wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE)
    {
        paraAttr = GetStyleForNewParagraph(buffer, pos);
        if (!paraAttr.IsDefault())
            p = & paraAttr;
    }

    wxRichTextAttr attr(buffer->GetDefaultStyle());

    // Don't include box attributes such as margins
    attr.GetTextBoxAttr().Reset();

    wxRichTextParagraph* newPara = new wxRichTextParagraph(this, & attr);
    if (p)
        newPara->SetAttributes(*p);

    wxRichTextField* fieldObject = new wxRichTextField();
    fieldObject->wxRichTextObject::SetProperties(properties);
    fieldObject->SetFieldType(fieldType);
    fieldObject->SetAttributes(textAttr);
    newPara->AppendChild(fieldObject);
    action->GetNewParagraphs().AppendChild(newPara);
    action->GetNewParagraphs().UpdateRanges();
    action->GetNewParagraphs().SetPartialParagraph(true);
    action->SetPosition(pos);

    // Set the range we'll need to delete in Undo
    action->SetRange(wxRichTextRange(pos, pos));

    buffer->SubmitAction(action);

    wxRichTextField* obj = wxDynamicCast(GetLeafObjectAtPosition(pos), wxRichTextField);
    return obj;
}

bool wxRichTextParagraphLayoutBox::SetObjectPropertiesWithUndo(wxRichTextObject& obj, const wxRichTextProperties& properties, wxRichTextObject* objToSet)
{
    wxRichTextBuffer* buffer = GetBuffer();
    wxCHECK_MSG(buffer, false, wxT("Invalid buffer"));
    wxRichTextCtrl* rtc = buffer->GetRichTextCtrl();
    wxCHECK_MSG(rtc, false, wxT("Invalid rtc"));

    wxRichTextAction* action = nullptr;
    wxRichTextObject* clone = nullptr;

    // The object on which to set properties will usually be 'obj', but use objToSet if it's valid.
    // This is necessary e.g. on setting a wxRichTextCell's properties, when obj will be the parent table
    if (objToSet == nullptr)
        objToSet = &obj;

    if (rtc->SuppressingUndo())
        objToSet->SetProperties(properties);
    else
    {
        clone = obj.Clone();
        objToSet->SetProperties(properties);

        // The 'true' parameter in the next line says "Ignore first time"; otherwise the objects are prematurely switched
        action = new wxRichTextAction(nullptr, _("Change Properties"), wxRICHTEXT_CHANGE_OBJECT, buffer, obj.GetParentContainer(), rtc, true);
        action->SetOldAndNewObjects(& obj, clone);
        action->SetPosition(obj.GetRange().GetStart());
        action->SetRange(obj.GetRange());
        buffer->SubmitAction(action);
    }

    return true;
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
    wxRichTextAction* action = new wxRichTextAction(nullptr, _("Delete"), wxRICHTEXT_DELETE, buffer, this, ctrl);

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
        wxASSERT(m_batchedCommand == nullptr);
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
    wxASSERT(m_batchedCommand != nullptr);

    if (m_batchedCommandDepth == 0)
    {
        GetCommandProcessor()->Store(m_batchedCommand);
        m_batchedCommand = nullptr;
    }

    return true;
}

/// Submit immediately, or delay according to whether collapsing is on
bool wxRichTextBuffer::SubmitAction(wxRichTextAction* action)
{
    if (action && !action->GetNewParagraphs().IsEmpty())
        PrepareContent(action->GetNewParagraphs());

    if (BatchingUndo() && m_batchedCommand && !SuppressingUndo())
    {
        if (!action->GetIgnoreFirstTime())
        {
            wxRichTextCommand* cmd = new wxRichTextCommand(action->GetName());
            cmd->AddAction(action);
            cmd->Do();
            cmd->GetActions().Clear();
            delete cmd;
        }

        m_batchedCommand->AddAction(action);
    }
    else
    {
        wxRichTextCommand* cmd = new wxRichTextCommand(action->GetName());
        cmd->AddAction(action);

        // Only store it if we're not suppressing undo.
        if (!action->GetIgnoreFirstTime())
        {
            return GetCommandProcessor()->Submit(cmd, !SuppressingUndo());
        }
        else if (!SuppressingUndo())
        {
            GetCommandProcessor()->Store(cmd); // Just store it, without Do()ing anything
        }
        else
            delete cmd;
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
    newStyle.GetTextBoxAttr().Reset();

    // Save the old default style
    m_attributeStack.Append((wxObject*) new wxRichTextAttr(newStyle));

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
        return nullptr;
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
    return nullptr;
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
    return nullptr;
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
    return nullptr;
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

#if wxUSE_FFILE && wxUSE_STREAMS
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
#endif // wxUSE_FFILE && wxUSE_STREAMS

#if wxUSE_STREAMS
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
#endif // wxUSE_STREAMS

/// Copy the range to the clipboard
bool wxRichTextBuffer::CopyToClipboard(const wxRichTextRange& range)
{
    bool success = false;
#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ

    wxRichTextParagraphLayoutBox* container = this;
    if (GetRichTextCtrl())
        container = GetRichTextCtrl()->GetFocusObject();

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
                    container->InsertParagraphsWithUndo(this, position+1, *richTextBuffer, GetRichTextCtrl(), 0);
                    if (GetRichTextCtrl())
                        GetRichTextCtrl()->ShowPosition(position + richTextBuffer->GetOwnRange().GetEnd());
                    if (richTextBuffer->GetStyleSheet())
                    {
                        delete richTextBuffer->GetStyleSheet();
                        richTextBuffer->SetStyleSheet(nullptr);
                    }
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
                text2.Alloc(text.length()+1);
                size_t i;
                for (i = 0; i < text.length(); i++)
                {
                    wxChar ch = text[i];
                    if (ch != wxT('\r'))
                        text2 += ch;
                }
#else
                wxString text2 = text;
#endif
                container->InsertTextWithUndo(this, position+1, text2, GetRichTextCtrl(), wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE);

                if (GetRichTextCtrl())
                    GetRichTextCtrl()->ShowPosition(position + text2.length());

                success = true;
            }
            else if (wxTheClipboard->IsSupported(wxDF_BITMAP))
            {
                wxBitmapDataObject data;
                wxTheClipboard->GetData(data);
                wxBitmap bitmap(data.GetBitmap());
                wxImage image(bitmap.ConvertToImage());

                wxRichTextAction* action = new wxRichTextAction(nullptr, _("Insert Image"), wxRICHTEXT_INSERT, this, container, GetRichTextCtrl(), false);

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

    wxRichTextEvent event(wxEVT_RICHTEXT_STYLESHEET_REPLACING, winid);
    event.SetEventObject(GetRichTextCtrl());
    if (GetRichTextCtrl())
        event.SetContainer(GetRichTextCtrl()->GetFocusObject());
    else
        event.SetContainer(this);
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

    event.SetEventType(wxEVT_RICHTEXT_STYLESHEET_REPLACED);
    event.SetOldStyleSheet(nullptr);
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
int wxRichTextBuffer::HitTest(wxDC& dc, wxRichTextDrawingContext& context, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags)
{
    int ret = wxRichTextParagraphLayoutBox::HitTest(dc, context, pt, textPosition, obj, contextObj, flags);
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

void wxRichTextBuffer::SetFontScale(double fontScale)
{
    m_fontScale = fontScale;
    m_fontTable.SetFontScale(fontScale);
}

void wxRichTextBuffer::SetDimensionScale(double dimScale)
{
    m_dimensionScale = dimScale;
}

bool wxRichTextStdRenderer::DrawStandardBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxRichTextAttr& bulletAttr, const wxRect& rect)
{
    if (bulletAttr.GetTextColour().IsOk())
    {
        wxCheckSetPen(dc, wxPen(bulletAttr.GetTextColour()));
        wxCheckSetBrush(dc, wxBrush(bulletAttr.GetTextColour()));
    }
    else
    {
        wxCheckSetPen(dc, *wxBLACK_PEN);
        wxCheckSetBrush(dc, *wxBLACK_BRUSH);
    }

    SetFontForBullet(*(paragraph->GetBuffer()), dc, bulletAttr);

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
        SetFontForBullet(*(paragraph->GetBuffer()), dc, attr);

        wxString text1(text);
        if (attr.HasTextEffects() && (attr.GetTextEffects() & (wxTEXT_ATTR_EFFECT_CAPITALS|wxTEXT_ATTR_EFFECT_SMALL_CAPITALS)))
            text1.MakeUpper();

        if (attr.GetTextColour().IsOk())
            dc.SetTextForeground(attr.GetTextColour());

        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

        int charHeight = dc.GetCharHeight();
        wxCoord tw, th;
        dc.GetTextExtent(text1, & tw, & th);

        int x = rect.x;

        // Calculate the top position of the character (as opposed to the whole line height)
        int y = rect.y + (rect.height - charHeight);

        // The margin between a bullet and text.
        int margin = paragraph->ConvertTenthsMMToPixels(dc, wxRichTextBuffer::GetBulletRightMargin());

        if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT)
            x = (rect.x + rect.width) - tw - margin;
        else if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE)
            x = x + (rect.width)/2 - tw/2;

        dc.DrawText(text1, x, y);

        return true;
    }
    else
        return false;
}

void wxRichTextStdRenderer::SetFontForBullet(wxRichTextBuffer& buffer, wxDC& dc, const wxRichTextAttr& attr)
{
    wxFont font;
    if ((attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_SYMBOL) && !attr.GetBulletFont().IsEmpty() && attr.HasFont())
    {
        wxRichTextAttr fontAttr;
        if (attr.HasFontPixelSize())
            fontAttr.SetFontPixelSize(attr.GetFontSize());
        else
            fontAttr.SetFontPointSize(attr.GetFontSize());
        fontAttr.SetFontStyle(attr.GetFontStyle());
        fontAttr.SetFontWeight(attr.GetFontWeight());
        fontAttr.SetFontUnderlined(attr.GetFontUnderlined());
        fontAttr.SetFontFaceName(attr.GetBulletFont());
        font = buffer.GetFontTable().FindFont(fontAttr);
    }
    else if (attr.HasFont())
        font = buffer.GetFontTable().FindFont(attr);
    else
        font = (*wxNORMAL_FONT);

    if (font.IsOk())
    {
        if ( attr.HasTextEffects() && ( (attr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUPERSCRIPT)
            || (attr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUBSCRIPT) ) )
        {
            wxFont textFont = font;
            if (textFont.IsUsingSizeInPixels())
            {
                double size = static_cast<double>(textFont.GetPixelSize().y) / wxSCRIPT_MUL_FACTOR;
                textFont.SetPixelSize(wxSize(0, static_cast<int>(size)));
            }
            else
            {
                textFont.SetFractionalPointSize(textFont.GetFractionalPointSize() / wxSCRIPT_MUL_FACTOR);
            }
            wxCheckSetFont(dc, textFont);
        }
        else if (attr.HasTextEffects() && (attr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SMALL_CAPITALS))
        {
            wxFont textFont = font;
            if (textFont.IsUsingSizeInPixels())
            {
                textFont.SetPixelSize(wxSize(0, (int) (textFont.GetPixelSize().y*0.75)));
            }
            else
            {
                textFont.SetFractionalPointSize(textFont.GetFractionalPointSize()*0.75);
            }

            wxCheckSetFont(dc, textFont);
        }
        else
        {
            wxCheckSetFont(dc, font);
        }
    }
}

bool wxRichTextStdRenderer::DrawBitmapBullet(wxRichTextParagraph* WXUNUSED(paragraph), wxDC& WXUNUSED(dc), const wxRichTextAttr& WXUNUSED(attr), const wxRect& WXUNUSED(rect))
{
    // Currently unimplemented. The intention is to store bitmaps by name in a media store associated
    // with the buffer. The store will allow retrieval from memory, disk or other means.
    return false;
}

// Measure the bullet.
bool wxRichTextStdRenderer::MeasureBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxRichTextAttr& attr, wxSize& sz)
{
    SetFontForBullet(*(paragraph->GetBuffer()), dc, attr);

    if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_STANDARD)
    {
        sz.x = (int) (((float) dc.GetCharHeight()) * wxRichTextBuffer::GetBulletProportion());
        sz.y = sz.x;
    }
    else if (attr.HasBulletText())
    {
        wxCoord w, h, maxDescent;
        wxString text(attr.GetBulletText());
        if (attr.HasTextEffects() && (attr.GetTextEffects() & (wxTEXT_ATTR_EFFECT_CAPITALS|wxTEXT_ATTR_EFFECT_SMALL_CAPITALS)))
            text.MakeUpper();

        dc.GetTextExtent(text, &w, &h, & maxDescent);
        sz.x = w;
        sz.y = h;
    }
    else if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_BITMAP)
    {
        // A guess, at present.
        sz.x = 10;
        sz.y = 10;
    }
    else
    {
        // Need to guess a size for a number bullet.
        wxCoord w, h, maxDescent;
        dc.GetTextExtent(wxT("8888."), & w, &h, & maxDescent);
        sz.x = w;
        sz.y = h;
    }

    return true;
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

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextBox, wxRichTextParagraphLayoutBox);

wxRichTextBox::wxRichTextBox(wxRichTextObject* parent):
    wxRichTextParagraphLayoutBox(parent)
{
}

/// Draw the item
bool wxRichTextBox::Draw(wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    if (!IsShown())
        return true;

    // TODO: if the active object in the control, draw an indication.
    // We need to add the concept of active object, and not just focus object,
    // so we can apply commands (properties, delete, ...) to objects such as text boxes and images.
    // Ultimately we would like to be able to interactively resize an active object
    // using drag handles.
    return wxRichTextParagraphLayoutBox::Draw(dc, context, range, selection, rect, descent, style);
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

    if (boxDlg.ShowModal() == wxID_OK && buffer->GetRichTextCtrl()->IsEditable())
    {
        // By passing wxRICHTEXT_SETSTYLE_RESET, indeterminate attributes set by the user will be set as
        // indeterminate in the object.
        boxDlg.ApplyStyle(buffer->GetRichTextCtrl(), wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_RESET);
        return true;
    }
    else
        return false;
}

/*!
 * wxRichTextField
 */

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextField, wxRichTextParagraphLayoutBox);

wxRichTextField::wxRichTextField(const wxString& fieldType, wxRichTextObject* parent):
    wxRichTextParagraphLayoutBox(parent)
{
    SetFieldType(fieldType);
}

/// Draw the item
bool wxRichTextField::Draw(wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    if (!IsShown())
        return true;

    wxRichTextFieldType* fieldType = wxRichTextBuffer::FindFieldType(GetFieldType());
    if (fieldType && fieldType->Draw(this, dc, context, range, selection, rect, descent, style))
        return true;

    // Fallback so unknown fields don't become invisible.
    wxString fieldTypeStr(GetFieldType());
    wxRichTextFieldTypeStandard defaultFieldType;
    defaultFieldType.SetLabel(wxString::Format(wxT("unknown field %s"), fieldTypeStr.c_str()));
    return defaultFieldType.Draw(this, dc, context, range, selection, rect, descent, style);
}

bool wxRichTextField::Layout(wxDC& dc, wxRichTextDrawingContext& context, const wxRect& rect, const wxRect& parentRect, int style)
{
    wxRichTextFieldType* fieldType = wxRichTextBuffer::FindFieldType(GetFieldType());
    if (fieldType && fieldType->Layout(this, dc, context, rect, parentRect, style))
        return true;

    // Fallback so unknown fields don't become invisible.
    wxString fieldTypeStr(GetFieldType());
    wxRichTextFieldTypeStandard defaultFieldType;
    defaultFieldType.SetLabel(wxString::Format(wxT("unknown field %s"), fieldTypeStr.c_str()));
    return defaultFieldType.Layout(this, dc, context, rect, parentRect, style);
}

bool wxRichTextField::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, wxRichTextDrawingContext& context, int flags, const wxPoint& position, const wxSize& parentSize, wxArrayInt* partialExtents) const
{
    wxRichTextFieldType* fieldType = wxRichTextBuffer::FindFieldType(GetFieldType());
    if (fieldType)
        return fieldType->GetRangeSize(const_cast<wxRichTextField*>(this), range, size, descent, dc, context, flags, position, parentSize, partialExtents);

    // Fallback so unknown fields don't become invisible.
    wxString fieldTypeStr(GetFieldType());
    wxRichTextFieldTypeStandard defaultFieldType;
    defaultFieldType.SetLabel(wxString::Format(wxT("unknown field %s"), fieldTypeStr.c_str()));
    return defaultFieldType.GetRangeSize(const_cast<wxRichTextField*>(this), range, size, descent, dc, context, flags, position, parentSize, partialExtents);
}

/// Calculate range
void wxRichTextField::CalculateRange(long start, long& end)
{
    if (IsTopLevel())
        wxRichTextParagraphLayoutBox::CalculateRange(start, end);
    else
        wxRichTextObject::CalculateRange(start, end);
}

/// Copy
void wxRichTextField::Copy(const wxRichTextField& obj)
{
    wxRichTextParagraphLayoutBox::Copy(obj);

    UpdateField(GetBuffer());
}

// Edit properties via a GUI
bool wxRichTextField::EditProperties(wxWindow* parent, wxRichTextBuffer* buffer)
{
    wxRichTextFieldType* fieldType = wxRichTextBuffer::FindFieldType(GetFieldType());
    if (fieldType)
        return fieldType->EditProperties(this, parent, buffer);

    return false;
}

bool wxRichTextField::CanEditProperties() const
{
    wxRichTextFieldType* fieldType = wxRichTextBuffer::FindFieldType(GetFieldType());
    if (fieldType)
        return fieldType->CanEditProperties(const_cast<wxRichTextField*>(this));

    return false;
}

wxString wxRichTextField::GetPropertiesMenuLabel() const
{
    wxRichTextFieldType* fieldType = wxRichTextBuffer::FindFieldType(GetFieldType());
    if (fieldType)
        return fieldType->GetPropertiesMenuLabel(const_cast<wxRichTextField*>(this));

    return wxEmptyString;
}

bool wxRichTextField::UpdateField(wxRichTextBuffer* buffer)
{
    wxRichTextFieldType* fieldType = wxRichTextBuffer::FindFieldType(GetFieldType());
    if (fieldType)
        return fieldType->UpdateField(buffer, (wxRichTextField*) this);

    return false;
}

bool wxRichTextField::IsTopLevel() const
{
    wxRichTextFieldType* fieldType = wxRichTextBuffer::FindFieldType(GetFieldType());
    if (fieldType)
        return fieldType->IsTopLevel(const_cast<wxRichTextField*>(this));

    return true;
}

wxIMPLEMENT_CLASS(wxRichTextFieldType, wxObject);

wxIMPLEMENT_CLASS(wxRichTextFieldTypeStandard, wxRichTextFieldType);

wxRichTextFieldTypeStandard::wxRichTextFieldTypeStandard(const wxString& name, const wxString& label, int displayStyle)
{
    Init();

    SetName(name);
    SetLabel(label);
    SetDisplayStyle(displayStyle);
}

wxRichTextFieldTypeStandard::wxRichTextFieldTypeStandard(const wxString& name, const wxBitmap& bitmap, int displayStyle)
{
    Init();

    SetName(name);
    SetBitmap(bitmap);
    SetDisplayStyle(displayStyle);
}

void wxRichTextFieldTypeStandard::Init()
{
    m_displayStyle = wxRICHTEXT_FIELD_STYLE_RECTANGLE;
    m_font = wxFont(6, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_textColour = *wxWHITE;
    m_borderColour = *wxBLACK;
    m_backgroundColour = *wxBLACK;
    m_verticalPadding = 1;
    m_horizontalPadding = 3;
    m_horizontalMargin = 2;
    m_verticalMargin = 0;
}

void wxRichTextFieldTypeStandard::Copy(const wxRichTextFieldTypeStandard& field)
{
    wxRichTextFieldType::Copy(field);

    m_label = field.m_label;
    m_displayStyle = field.m_displayStyle;
    m_font = field.m_font;
    m_textColour = field.m_textColour;
    m_borderColour = field.m_borderColour;
    m_backgroundColour = field.m_backgroundColour;
    m_verticalPadding = field.m_verticalPadding;
    m_horizontalPadding = field.m_horizontalPadding;
    m_horizontalMargin = field.m_horizontalMargin;
    m_bitmap = field.m_bitmap;
}

bool wxRichTextFieldTypeStandard::Draw(wxRichTextField* obj, wxDC& dc, wxRichTextDrawingContext& WXUNUSED(context), const wxRichTextRange& WXUNUSED(range), const wxRichTextSelection& selection, const wxRect& rect, int descent, int WXUNUSED(style))
{
    if (m_displayStyle == wxRICHTEXT_FIELD_STYLE_COMPOSITE)
        return false; // USe default composite drawing
    else // if (m_displayStyle == wxRICHTEXT_FIELD_STYLE_RECTANGLE || m_displayStyle == wxRICHTEXT_FIELD_STYLE_NOBORDER)
    {
        int borderSize = 1;

        wxPen borderPen(m_borderColour, 1, wxPENSTYLE_SOLID);
        wxBrush backgroundBrush(m_backgroundColour);
        wxColour textColour(m_textColour);

        if (selection.WithinSelection(obj->GetRange().GetStart(), obj))
        {
            wxColour highlightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
            wxColour highlightTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

            borderPen = wxPen(highlightTextColour, 1, wxPENSTYLE_SOLID);
            backgroundBrush = wxBrush(highlightColour);

            wxCheckSetBrush(dc, backgroundBrush);
            wxCheckSetPen(dc, wxPen(highlightColour, 1, wxPENSTYLE_SOLID));
            dc.DrawRectangle(rect);
        }

        if (m_displayStyle != wxRICHTEXT_FIELD_STYLE_NO_BORDER)
            borderSize = 0;

        // objectRect is the area where the content is drawn, after margins around it have been taken into account
        wxRect objectRect = wxRect(wxPoint(rect.x + m_horizontalMargin, rect.y + wxMax(0, rect.height - descent - obj->GetCachedSize().y)),
            wxSize(obj->GetCachedSize().x - 2*m_horizontalMargin - borderSize, obj->GetCachedSize().y));

        // clientArea is where the text is actually written
        wxRect clientArea = objectRect;

        if (m_displayStyle == wxRICHTEXT_FIELD_STYLE_RECTANGLE)
        {
            dc.SetPen(borderPen);
            dc.SetBrush(backgroundBrush);
            dc.DrawRoundedRectangle(objectRect, 4.0);
        }
        else if (m_displayStyle == wxRICHTEXT_FIELD_STYLE_START_TAG)
        {
            int arrowLength = objectRect.height/2;
            clientArea.width -= (arrowLength - m_horizontalPadding);

            wxPoint pts[5];
            pts[0].x = objectRect.x; pts[0].y = objectRect.y;
            pts[1].x = objectRect.x + objectRect.width - arrowLength; pts[1].y = objectRect.y;
            pts[2].x = objectRect.x + objectRect.width; pts[2].y = objectRect.y + (objectRect.height/2);
            pts[3].x = objectRect.x + objectRect.width - arrowLength; pts[3].y = objectRect.y + objectRect.height;
            pts[4].x = objectRect.x; pts[4].y = objectRect.y + objectRect.height;
            dc.SetPen(borderPen);
            dc.SetBrush(backgroundBrush);
            dc.DrawPolygon(5, pts);
        }
        else if (m_displayStyle == wxRICHTEXT_FIELD_STYLE_END_TAG)
        {
            int arrowLength = objectRect.height/2;
            clientArea.width -= (arrowLength - m_horizontalPadding);
            clientArea.x += (arrowLength - m_horizontalPadding);

            wxPoint pts[5];
            pts[0].x = objectRect.x + objectRect.width; pts[0].y = objectRect.y;
            pts[1].x = objectRect.x + arrowLength; pts[1].y = objectRect.y;
            pts[2].x = objectRect.x; pts[2].y = objectRect.y + (objectRect.height/2);
            pts[3].x = objectRect.x + arrowLength; pts[3].y = objectRect.y + objectRect.height;
            pts[4].x = objectRect.x + objectRect.width; pts[4].y = objectRect.y + objectRect.height;
            dc.SetPen(borderPen);
            dc.SetBrush(backgroundBrush);
            dc.DrawPolygon(5, pts);
        }

        if (m_bitmap.IsOk())
        {
            int x = clientArea.x + (clientArea.width - m_bitmap.GetWidth())/2;
            int y = clientArea.y + m_verticalPadding;
            dc.DrawBitmap(m_bitmap, x, y, true);

            if (selection.WithinSelection(obj->GetRange().GetStart(), obj))
            {
                wxCheckSetBrush(dc, *wxBLACK_BRUSH);
                wxCheckSetPen(dc, *wxBLACK_PEN);
                dc.SetLogicalFunction(wxINVERT);
                dc.DrawRectangle(wxRect(x, y, m_bitmap.GetWidth(), m_bitmap.GetHeight()));
                dc.SetLogicalFunction(wxCOPY);
            }
        }
        else
        {
            wxString label(m_label);
            if (label.IsEmpty())
                label = wxT("??");
            int w, h, maxDescent;
            dc.SetFont(m_font);
            dc.GetTextExtent(label, &w, &h, &maxDescent);
            dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
            dc.SetTextForeground(textColour);

            int x = clientArea.x + (clientArea.width - w)/2;
            int y = clientArea.y + (clientArea.height - h)/2;
            dc.DrawText(label, x, y);
        }
    }

    return true;
}

bool wxRichTextFieldTypeStandard::Layout(wxRichTextField* obj, wxDC& dc, wxRichTextDrawingContext& context, const wxRect& WXUNUSED(rect), const wxRect& WXUNUSED(parentRect), int style)
{
    if (m_displayStyle == wxRICHTEXT_FIELD_STYLE_COMPOSITE)
        return false; // USe default composite layout

    wxSize size = GetSize(obj, dc, context, style);
    obj->SetCachedSize(size);
    obj->SetMinSize(size);
    obj->SetMaxSize(size);
    return true;
}

bool wxRichTextFieldTypeStandard::GetRangeSize(wxRichTextField* obj, const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, wxRichTextDrawingContext& context, int flags, const wxPoint& position, const wxSize& parentSize, wxArrayInt* partialExtents) const
{
    if (IsTopLevel(obj))
        return obj->wxRichTextParagraphLayoutBox::GetRangeSize(range, size, descent, dc, context, flags, position, parentSize);
    else
    {
        wxSize sz = GetSize(obj, dc, context, 0);
        if (partialExtents)
        {
            int lastSize;
            if (partialExtents->GetCount() > 0)
                lastSize = (*partialExtents)[partialExtents->GetCount()-1];
            else
                lastSize = 0;
            partialExtents->Add(lastSize + sz.x);
        }
        size = sz;
       return true;
    }
}

wxSize wxRichTextFieldTypeStandard::GetSize(wxRichTextField* WXUNUSED(obj), wxDC& dc, wxRichTextDrawingContext& WXUNUSED(context), int WXUNUSED(style)) const
{
    int borderSize = 1;
    int w = 0, h = 0, maxDescent = 0;

    wxSize sz;
    if (m_bitmap.IsOk())
    {
        w = m_bitmap.GetWidth();
        h = m_bitmap.GetHeight();

        sz = wxSize(w + m_horizontalMargin*2, h + m_verticalMargin*2);
    }
    else
    {
        wxString label(m_label);
        if (label.IsEmpty())
            label = wxT("??");
        dc.SetFont(m_font);
        dc.GetTextExtent(label, & w, &h, & maxDescent);

        sz = wxSize(w + m_horizontalPadding*2 + m_horizontalMargin*2, h + m_verticalPadding *2 + m_verticalMargin*2);
    }

    if (m_displayStyle != wxRICHTEXT_FIELD_STYLE_NO_BORDER)
    {
        sz.x += borderSize*2;
        sz.y += borderSize*2;
    }

    if (m_displayStyle == wxRICHTEXT_FIELD_STYLE_START_TAG || m_displayStyle == wxRICHTEXT_FIELD_STYLE_END_TAG)
    {
        // Add space for the arrow
        sz.x += (sz.y/2 - m_horizontalPadding);
    }

    return sz;
}

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextCell, wxRichTextBox);

wxRichTextCell::wxRichTextCell(wxRichTextObject* parent):
    wxRichTextBox(parent)
{
}

/// Draw the item
bool wxRichTextCell::Draw(wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    if (selection.IsValid() &&
       (selection.WithinSelection(GetRange().GetStart(), this)))
    {
        style |= wxRICHTEXT_DRAW_SELECTED;
    }

    return wxRichTextBox::Draw(dc, context, range, selection, rect, descent, style);
}

int wxRichTextCell::HitTest(wxDC& dc, wxRichTextDrawingContext& context, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags)
{
    int ret = wxRichTextParagraphLayoutBox::HitTest(dc, context, pt, textPosition, obj, contextObj, flags);
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

// Adjusts the attributes for virtual attribute provision, collapsed borders, etc.
bool wxRichTextCell::AdjustAttributes(wxRichTextAttr& attr, wxRichTextDrawingContext& context)
{
    wxRichTextObject::AdjustAttributes(attr, context);

    wxRichTextTable* table = wxDynamicCast(GetParent(), wxRichTextTable);
    if (IsShown() && table && table->GetAttributes().GetTextBoxAttr().HasCollapseBorders() &&
        table->GetAttributes().GetTextBoxAttr().GetCollapseBorders() == wxTEXT_BOX_ATTR_COLLAPSE_FULL)
    {
        // Collapse borders:
        // (1) Reset left and top for all cells unless there is no table border there;
        // (2) for bottom and right, reset if at edge of table and there are no table borders,
        //     otherwise use this cell's border if present, otherwise adjacent border if not.
        // Takes into account spanning by checking if adjacent cells are shown.
        int row, col;
        if (table->GetCellRowColumnPosition(GetRange().GetStart(), row, col))
        {
            if (col == 0)
            {
                // Only remove the cell border on the left edge if we have a table border
                if (table->GetAttributes().GetTextBoxAttr().GetBorder().GetLeft().IsValid())
                    attr.GetTextBoxAttr().GetBorder().GetLeft().Reset();
            }
            else
                attr.GetTextBoxAttr().GetBorder().GetLeft().Reset();

            if (row == 0)
            {
                // Only remove the cell border on the top edge if we have a table border
                if (table->GetAttributes().GetTextBoxAttr().GetBorder().GetTop().IsValid())
                    attr.GetTextBoxAttr().GetBorder().GetTop().Reset();
            }
            else
                attr.GetTextBoxAttr().GetBorder().GetTop().Reset();

            // Compute right border

            // We need to explicitly look at the spans, not just whether
            // the cell is visible, because that doesn't tell us which
            // cell to look at for border information.
            wxRichTextCell* adjacentCellRight = nullptr;

            int nextCol = col + GetColSpan();
            if  (nextCol >= table->GetColumnCount())
            {
                // Do nothing - at edge of table
            }
            else
            {
                wxRichTextCell* nextRightCell = table->GetCell(row, nextCol);
                if (nextRightCell->IsShown())
                {
                    adjacentCellRight = nextRightCell;
                }
                else
                {
                    // Must be hidden by a rowspan above. Go hunting for it.
                    int r;
                    for (r = row-1; r >= 0; r--)
                    {
                        nextRightCell = table->GetCell(r, nextCol);
                        if (nextRightCell->IsShown())
                        {
                            adjacentCellRight = nextRightCell;
                            break;
                        }
                    }
                }
            }

            // If no adjacent cell (either because they were hidden or at the edge of the table)
            // then we must reset the border, if there's a right table border.
            if (!adjacentCellRight)
            {
                if (table->GetAttributes().GetTextBoxAttr().GetBorder().GetRight().IsValid())
                    attr.GetTextBoxAttr().GetBorder().GetRight().Reset();
            }
            else
            {
                if (!attr.GetTextBoxAttr().GetBorder().GetRight().IsValid() ||
                    attr.GetTextBoxAttr().GetBorder().GetRight().GetWidth().GetValue() == 0)
                {
                    attr.GetTextBoxAttr().GetBorder().GetRight() = adjacentCellRight->GetAttributes().GetTextBoxAttr().GetBorder().GetLeft();
                }
            }

            // Compute bottom border
            wxRichTextCell* adjacentCellBelow = nullptr;

            int nextRow = row + GetRowSpan();
            if  (nextRow >= table->GetRowCount())
            {
                // Do nothing - at edge of table
            }
            else
            {
                wxRichTextCell* nextBottomCell = table->GetCell(nextRow, col);
                if (nextBottomCell->IsShown())
                {
                    adjacentCellBelow = nextBottomCell;
                }
                else
                {
                    // Must be hidden by a colspan to the left. Go hunting for it.
                    int c;
                    for (c = col-1; c >= 0; c--)
                    {
                        nextBottomCell = table->GetCell(nextRow, c);
                        if (nextBottomCell->IsShown())
                        {
                            adjacentCellBelow = nextBottomCell;
                            break;
                        }
                    }
                }
            }

            // If no adjacent cell (either because they were hidden or at the edge of the table)
            // then we must reset the border, if there's a bottom table border.
            if (!adjacentCellBelow)
            {
                if (table->GetAttributes().GetTextBoxAttr().GetBorder().GetBottom().IsValid())
                    attr.GetTextBoxAttr().GetBorder().GetBottom().Reset();
            }
            else
            {
                if (!attr.GetTextBoxAttr().GetBorder().GetBottom().IsValid() ||
                    attr.GetTextBoxAttr().GetBorder().GetBottom().GetWidth().GetValue() == 0)
                {
                    attr.GetTextBoxAttr().GetBorder().GetBottom() = adjacentCellBelow->GetAttributes().GetTextBoxAttr().GetBorder().GetTop();
                }
            }
        }
    }

    return true;
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

    wxRichTextSelection sel;
    if (buffer && buffer->GetRichTextCtrl())
        sel = buffer->GetRichTextCtrl()->GetSelection();

    if (table && buffer && buffer->GetRichTextCtrl() && sel.IsValid() &&
        sel.GetContainer() == GetParent())
    {
        wxRichTextAttr clashingAttr, absentAttr;
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

    // We don't want position and floating controls for a cell.
    wxRichTextSizePage::ShowPositionControls(false);
    wxRichTextSizePage::ShowFloatingControls(false);
    wxRichTextSizePage::ShowAlignmentControls(true);

    wxRichTextObjectPropertiesDialog cellDlg(this, wxGetTopLevelParent(parent), wxID_ANY, caption);
    cellDlg.SetAttributes(attr);

    bool ok = (cellDlg.ShowModal() == wxID_OK);

    wxRichTextSizePage::ShowPositionControls(true);
    wxRichTextSizePage::ShowFloatingControls(true);

    if (ok && buffer->GetRichTextCtrl()->IsEditable())
    {
        wxRichTextAttr newAttr = cellDlg.GetAttributes();
        if (!(newAttr == attr))
        {
            if (multipleCells)
            {
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
        }
        return true;
    }
    else
        return false;
}

void wxRichTextCell::SetColSpan(int span)
{
    wxASSERT(span >= 1);
    if (span >= 1)
        GetProperties().SetProperty(wxT("colspan"), (long) span);
}

void wxRichTextCell::SetRowSpan(int span)
{
    wxASSERT(span >= 1);
    if (span >= 1)
        GetProperties().SetProperty(wxT("rowspan"), (long) span);
}

// The next 2 methods return span values. Note that the default is 1, not 0
int wxRichTextCell::GetColSpan() const
{
    int span = 1;
    if (GetProperties().HasProperty(wxT("colspan")))
    {
        span = GetProperties().GetPropertyLong(wxT("colspan"));
    }

    return span;
}

int wxRichTextCell::GetRowSpan() const
{
    int span = 1;
    if (GetProperties().HasProperty(wxT("rowspan")))
    {
        span = GetProperties().GetPropertyLong(wxT("rowspan"));
    }

    return span;
}

WX_DEFINE_OBJARRAY(wxRichTextObjectPtrArrayArray)

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextTable, wxRichTextBox);

wxRichTextTable::wxRichTextTable(wxRichTextObject* parent): wxRichTextBox(parent)
{
    m_rowCount = 0;
    m_colCount = 0;
}

// Draws the object.
bool wxRichTextTable::Draw(wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& range, const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
    wxRichTextBox::Draw(dc, context, range, selection, rect, descent, style);

    // If the table is not collapsed (in which case the outer table box provides the border),
    // draw the overall border again using cell borders in case it has been overwritten by
    // adjacent cell borders of different colours.
    if (!GetAttributes().GetTextBoxAttr().HasCollapseBorders() ||
        GetAttributes().GetTextBoxAttr().GetCollapseBorders() != wxTEXT_BOX_ATTR_COLLAPSE_FULL)
    {
        int colCount = GetColumnCount();
        int rowCount = GetRowCount();
        int col, row;
        for (col = 0; col < colCount; col++)
        {
            for (row = 0; row < rowCount; row++)
            {
                if (row == 0 || row == (rowCount-1) || col == 0 || col == (colCount-1))
                {
                    wxRichTextCell* cell = GetCell(row, col);
                    if (cell && cell->IsShown() && !cell->GetRange().IsOutside(range))
                    {
                        wxRect childRect(cell->GetPosition(), cell->GetCachedSize());
                        wxRichTextAttr attr(cell->GetAttributes());
                        cell->AdjustAttributes(attr, context);
                        if (row != 0)
                            attr.GetTextBoxAttr().GetBorder().GetTop().Reset();
                        if (row != (rowCount-1))
                            attr.GetTextBoxAttr().GetBorder().GetBottom().Reset();
                        if (col != 0)
                            attr.GetTextBoxAttr().GetBorder().GetLeft().Reset();
                        if (col != (colCount-1))
                            attr.GetTextBoxAttr().GetBorder().GetRight().Reset();

                        if (attr.GetTextBoxAttr().GetBorder().IsValid())
                        {
                            wxRect boxRect(cell->GetPosition(), cell->GetCachedSize());
                            wxRect marginRect = boxRect;
                            wxRect contentRect, borderRect, paddingRect, outlineRect;

                            cell->GetBoxRects(dc, GetBuffer(), attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);
                            cell->DrawBorder(dc, GetBuffer(), attr, attr.GetTextBoxAttr().GetBorder(), borderRect);
                        }
                    }
                }
            }
        }
    }

    return true;
}

    // Helper function for Layout() that clears the space needed by a cell with rowspan > 1
static
int GetRowspanDisplacement(const wxRichTextTable* table, int row, int col, int paddingX, const wxArrayInt& colWidths)
{
    // If one or more cells above-left of this one has rowspan > 1, the affected cells below it
    // will have been hidden and have width 0. As a result they are ignored by the layout algorithm,
    // and all cells to their right are effectively shifted left. As a result there's no hole for
    // the spanning cell to fill.
    // So search back along the current row for hidden cells. However there's also the annoying issue of a
    // rowspanning cell that also has colspam. So we can't rely on the rowspanning cell being directly above
    // the first hidden one we come to. We also can't rely on a cell being hidden only by one type of span;
    // there's nothing to stop a cell being hidden by colspan, and then again hidden from above by rowspan.
    // The answer is to look above each hidden cell in turn, which I think covers all bases.
    int deltaX = 0;
    for (int prevcol = 0; prevcol < col; ++prevcol)
    {
        if (!table->GetCell(row, prevcol)->IsShown())
        {
            // We've found a hidden cell. If it's hidden because of colspan to its left, it's
            // already been taken into account; but not if there's a rowspanning cell above
            for (int prevrow = row-1; prevrow >= 0; --prevrow)
            {
                wxRichTextCell* cell = table->GetCell(prevrow, prevcol);
                if (cell && cell->IsShown())
                {
                    int rowSpan = cell->GetRowSpan();
                    if (rowSpan > 1 && rowSpan > (row-prevrow))
                    {
                        // There is a rowspanning cell above above the hidden one, so we need
                        // to right-shift the index cell by this column's width. Furthermore,
                        // if the cell also colspans, we need to shift by all affected columns
                        for (int colSpan = 0; colSpan < cell->GetColSpan(); ++colSpan)
                            deltaX += (colWidths[prevcol+colSpan] + paddingX);
                        break;
                    }
                }
            }
        }
    }
    return deltaX;
}

    // Helper function for Layout() that expands any cell with rowspan > 1
static
void ExpandCellsWithRowspan(const wxRichTextTable* table, int paddingY, int& bottomY, wxDC& dc, wxRichTextDrawingContext& context, const wxRect& availableSpace, int style)
{
    // This is called when the table's cell layout is otherwise complete.
    // For any cell with rowspan > 1, expand downwards into the row(s) below.

    // Start by finding the current 'y' of the top of each row, plus the bottom of the available area for cells.
    // Deduce this from the top of a visible cell in the row below. (If none are visible, the row will be invisible anyway and can be ignored.)
    const int rowCount = table->GetRowCount();
    const int colCount = table->GetColumnCount();
    wxArrayInt rowTops;
    rowTops.Add(0, rowCount+1);
    int row;
    for (row = 0; row < rowCount; ++row)
    {
        for (int column = 0; column < colCount; ++column)
        {
            wxRichTextCell* cell = table->GetCell(row, column);
            if (cell && cell->IsShown())
            {
                rowTops[row] = cell->GetPosition().y;
                break;
            }
        }
    }
    rowTops[rowCount] = bottomY + paddingY;  // The table bottom, which was passed to us

    bool needsRelay = false;

    for (row = 0; row < rowCount-1; ++row) // -1 as the bottom row can't rowspan
    {
        for (int col = 0; col < colCount; ++col)
        {
            wxRichTextCell* cell = table->GetCell(row, col);
            if (cell && cell->IsShown())
            {
                int span = cell->GetRowSpan();
                if (span > 1)
                {
                    span = wxMin(span, rowCount-row); // Don't try to span below the table!
                    if (span < 2)
                        continue;

                    int availableHeight = rowTops[row+span] - rowTops[row] - paddingY;
                    wxSize newSize = wxSize(cell->GetCachedSize().GetWidth(), availableHeight);
                    wxRect availableCellSpace = wxRect(cell->GetPosition(), newSize);
                    cell->Invalidate(wxRICHTEXT_ALL);
                    cell->Layout(dc, context, availableCellSpace, availableSpace, style|wxRICHTEXT_FIXED_WIDTH|wxRICHTEXT_FIXED_HEIGHT);
                    // Ensure there's room in the span to display its contents, else it'll overwrite lower rows
                    int overhang = cell->GetCachedSize().GetHeight() - availableHeight;

                    if (overhang > 0)
                    {
                        // There are 3 things to get right:
                        // 1) The easiest is the rows below the span: they need to be downshifted by the overhang, and so does the table bottom itself
                        // 2) The rows within the span, including the one holding this cell, need to be deepened by their share of the overhang
                        //    e.g. if rowspan == 3, each row should increase in depth by 1/3rd of the overhang.
                        // 3) The cell with the rowspan shouldn't be touched in 2); its height will be set to the whole span later.
                        int deltaY = overhang / span;
                        int spare  = overhang % span;

                        // Each row in the span needs to by deepened by its share of the overhang (give the first row any spare).
                        // This is achieved by increasing the value stored in the following row's rowTops
                        for (int spannedRows = 0; spannedRows < span; ++spannedRows)
                        {
                            rowTops[row+spannedRows+1] += ((deltaY * (spannedRows+1))  + (spannedRows == 0 ? spare:0));
                        }

                        // Any rows below the span need shifting down
                        for (int rowsBelow = row + span+1; rowsBelow <= rowCount; ++rowsBelow)
                        {
                            rowTops[rowsBelow] += overhang;
                        }

                        needsRelay = true;
                    }
                }
            }
        }
    }

    if (!needsRelay)
        return;

    // There were overflowing rowspanning cells, so layout yet again to make the increased row depths show
    for (row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            wxRichTextCell* cell = table->GetCell(row, col);
            if (cell && cell->IsShown())
            {
                wxPoint position(cell->GetPosition().x, rowTops[row]);

                // GetRowspan() will usually return 1, but may be greater
                wxSize size(cell->GetCachedSize().GetWidth(), rowTops[row + cell->GetRowSpan()] - rowTops[row] - paddingY);

                wxRect availableCellSpace = wxRect(position, size);
                cell->Invalidate(wxRICHTEXT_ALL);
                cell->Layout(dc, context, availableCellSpace, availableSpace, style|wxRICHTEXT_FIXED_WIDTH|wxRICHTEXT_FIXED_HEIGHT);
            }
        }

        bottomY = rowTops[rowCount] - paddingY;
    }
}

// Lays the object out. rect is the space available for layout. Often it will
// be the specified overall space for this object, if trying to constrain
// layout to a particular size, or it could be the total space available in the
// parent. rect is the overall size, so we must subtract margins and padding.
// to get the actual available space.
bool wxRichTextTable::Layout(wxDC& dc, wxRichTextDrawingContext& context, const wxRect& rect, const wxRect& WXUNUSED(parentRect), int style)
{
    SetPosition(rect.GetPosition());

    // The meaty bit. Calculate sizes of all cells and rows. Try to use
    // minimum size if within alloted size, then divide up remaining size
    // between rows/cols.

    double scale = 1.0;
    wxRichTextBuffer* buffer = GetBuffer();
    if (buffer) scale = buffer->GetScale();

    wxRect availableSpace = GetAvailableContentArea(dc, context, rect);
    wxTextAttrDimensionConverter converter(dc, scale, availableSpace.GetSize());

    wxRichTextAttr attr(GetAttributes());
    AdjustAttributes(attr, context);

    bool tableHasPercentWidth = (attr.GetTextBoxAttr().GetWidth().GetUnits() == wxTEXT_ATTR_UNITS_PERCENTAGE);
    // If we have no fixed table size, and assuming we're not pushed for
    // space, then we don't have to try to stretch the table to fit the contents.
    bool stretchToFitTableWidth = tableHasPercentWidth;

    int tableWidth = rect.width;
    if (attr.GetTextBoxAttr().GetWidth().IsValid() && !tableHasPercentWidth)
    {
        tableWidth = converter.GetPixels(attr.GetTextBoxAttr().GetWidth(), wxHORIZONTAL);

        // Fixed table width, so we do want to stretch columns out if necessary.
        stretchToFitTableWidth = true;

        // Shouldn't be able to exceed the size passed to this function
        tableWidth = wxMin(rect.width, tableWidth);
    }

    // Get internal padding
    int paddingLeft = 0, paddingTop = 0;
    if (attr.GetTextBoxAttr().GetPadding().GetLeft().IsValid())
        paddingLeft = converter.GetPixels(attr.GetTextBoxAttr().GetPadding().GetLeft(), wxHORIZONTAL);
    if (attr.GetTextBoxAttr().GetPadding().GetTop().IsValid())
        paddingTop = converter.GetPixels(attr.GetTextBoxAttr().GetPadding().GetTop(), wxVERTICAL);

    // Assume that left and top padding are also used for inter-cell padding.
    int paddingX = paddingLeft;
    int paddingY = paddingTop;

    int totalLeftMargin = 0, totalRightMargin = 0, totalTopMargin = 0, totalBottomMargin = 0;
    GetTotalMargin(dc, buffer, attr, totalLeftMargin, totalRightMargin, totalTopMargin, totalBottomMargin);

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
    wxArrayInt colWidths;
    colWidths.Add(0, m_colCount);

    wxArrayInt absoluteColWidths;
    absoluteColWidths.Add(0, m_colCount);

    wxArrayInt percentageColWidths;
    percentageColWidths.Add(0, m_colCount);

    // The required width of a column calculated from the content, in case we don't specify any widths
    wxArrayInt maxUnspecifiedColumnWidths;
    maxUnspecifiedColumnWidths.Add(0, m_colCount);

    wxArrayInt maxColWidths;
    maxColWidths.Add(0, m_colCount);

    wxArrayInt minColWidths;
    minColWidths.Add(0, m_colCount);

    // Separately record the minimum width of columns with
    // nowrap cells
    wxArrayInt minColWidthsNoWrap;
    minColWidthsNoWrap.Add(0, m_colCount);

    // Record the maximum spanning widths
    wxArrayInt spanningWidths, spanningWidthsSpanLengths;
    spanningWidths.Add(0, m_colCount);
    spanningWidthsSpanLengths.Add(0, m_colCount);

    wxSize tableSize(tableWidth, 0);

    int i, j, k;

    for (i = 0; i < m_colCount; i++)
    {
        absoluteColWidths[i] = 0;
        percentageColWidths[i] = -1;
        colWidths[i] = 0;
        maxColWidths[i] = 0;
        minColWidths[i] = 0;
        minColWidthsNoWrap[i] = 0;
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
            wxRichTextCell* cell = GetCell(j, i);
            int colSpan = cell->GetColSpan();
            int rowSpan = cell->GetRowSpan();
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
            wxRichTextCell* cell = GetCell(j, i);
            if (rectArray.GetCount() == 0)
            {
                cell->Show(true);
            }
            else
            {
                int colSpan = cell->GetColSpan();
                int rowSpan = cell->GetRowSpan();

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

    // Find the first spanned cell in each row that spans the most columns and doesn't
    // overlap with a spanned cell starting at a previous column position.
    // This means we need to keep an array of rects so we can check. However
    // it does also mean that some spans simply may not be taken into account
    // where there are different spans happening on different rows. In these cases,
    // they will simply be as wide as their constituent columns.

    // (1) Do an initial layout for all cells to get minimum and maximum size, and get
    // the absolute or percentage width of each column.

    for (j = 0; j < m_rowCount; j++)
    {
        int visibleCellCount = 0;

        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextBox* cell = GetCell(j, i);
            if (cell->IsShown())
            {
                visibleCellCount ++;
            }
        }

        // Cell width percentages are for the overall cell width, so ignore margins and
        // only take into account table margins and inter-cell padding.
        int availableWidthForPercentageCellWidths = internalTableWidth - ((visibleCellCount-1) * paddingX);
        wxTextAttrDimensionConverter cellConverter(dc, scale, wxSize(availableWidthForPercentageCellWidths, 0));

        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextCell* cell = GetCell(j, i);
            if (cell->IsShown())
            {
                int colSpan = cell->GetColSpan();

                // Lay out cell to find min/max widths
                cell->Invalidate(wxRICHTEXT_ALL);
                cell->Layout(dc, context, availableSpace, availableSpace, style);

                if (colSpan == 1)
                {
                    int absoluteCellWidth = -1;
                    int percentageCellWidth = -1;

                    if (cell->GetAttributes().GetTextBoxAttr().GetWidth().IsValid())
                    {
                        int w = cellConverter.GetPixels(cell->GetAttributes().GetTextBoxAttr().GetWidth(), wxHORIZONTAL);
                        if (cell->GetAttributes().GetTextBoxAttr().GetWidth().GetUnits() == wxTEXT_ATTR_UNITS_PERCENTAGE)
                        {
                            percentageCellWidth = w;
                        }
                        else
                        {
                            absoluteCellWidth = w;
                        }
                        // Override absolute width with minimum width if necessary
                        if (cell->GetMinSize().x > 0 && absoluteCellWidth != -1 && cell->GetMinSize().x > absoluteCellWidth)
                            absoluteCellWidth = cell->GetMinSize().x;
                    }
                    else
                        maxUnspecifiedColumnWidths[i] = wxMax(cell->GetMaxSize().x, maxUnspecifiedColumnWidths[i]);

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

                    if (cell->GetMinSize().x && cell->GetMinSize().x > minColWidths[i])
                        minColWidths[i] = cell->GetMinSize().x;

                    if (cell->GetMaxSize().x && cell->GetMaxSize().x > maxColWidths[i])
                        maxColWidths[i] = cell->GetMaxSize().x;

                    if (cell->GetAttributes().GetTextBoxAttr().HasWhitespaceMode() &&
                        (cell->GetAttributes().GetTextBoxAttr().GetWhitespaceMode() == wxTEXT_BOX_ATTR_WHITESPACE_NO_WRAP))
                    {
                        if (cell->GetMaxSize().x > minColWidthsNoWrap[i])
                            minColWidthsNoWrap[i] = cell->GetMaxSize().x;
                    }
                }
            }
        }
    }

    // (2) Allocate initial column widths from absolute values and proportions
    for (i = 0; i < m_colCount; i++)
    {
        if (absoluteColWidths[i] > 0)
        {
            colWidths[i] = absoluteColWidths[i];
        }
        else if (percentageColWidths[i] > 0)
        {
            colWidths[i] = percentageColWidths[i];
        }
    }

    // (3) Process absolute or proportional widths of spanning columns,
    // now that we know what our fixed column widths are going to be.
    // Spanned cells will try to adjust columns so the span will fit.
    // Currently fixed columns widths aren't adjusted.
    for (j = 0; j < m_rowCount; j++)
    {
        int visibleCellCount = 0;

        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextBox* cell = GetCell(j, i);
            if (cell->IsShown())
            {
                visibleCellCount ++;
            }
        }

        // Cell width percentages are for the overall cell width, so ignore margins and
        // only take into account table margins and inter-cell padding.
        int availableWidthForPercentageCellWidths = internalTableWidth - ((visibleCellCount-1) * paddingX);
        wxTextAttrDimensionConverter cellConverter(dc, scale, wxSize(availableWidthForPercentageCellWidths, 0));

        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextCell* cell = GetCell(j, i);
            if (cell->IsShown())
            {
                int colSpan = cell->GetColSpan();
                if (colSpan > 1)
                {
                    int spans = wxMin(colSpan, m_colCount - i);
                    int cellWidth = 0;
                    if (spans > 0)
                    {
                        if (cell->GetAttributes().GetTextBoxAttr().GetWidth().IsValid())
                        {
                            cellWidth = cellConverter.GetPixels(cell->GetAttributes().GetTextBoxAttr().GetWidth(), wxHORIZONTAL);
                            // Override absolute width with minimum width if necessary
                            if (cell->GetMinSize().x > 0 && cellWidth != -1 && cell->GetMinSize().x > cellWidth)
                                cellWidth = cell->GetMinSize().x;
                        }
                        else
                        {
                            cellWidth = cell->GetMinSize().x;

                            if (cell->GetMaxSize().x > cellWidth)
                                cellWidth = cell->GetMaxSize().x;
                        }

                        // Subtract the padding between cells
                        int spanningWidth = cellWidth;
                        spanningWidth -= paddingX * (spans-1);

                        if (spanningWidth > 0)
                        {
                            if (spanningWidth > spanningWidths[i])
                            {
                                // Remember the largest spanning cell for this column,
                                // so we can adjust the spanned columns in the next step.
                                spanningWidths[i] = spanningWidth;
                                spanningWidthsSpanLengths[i] = spans;
                            }
                        }
                    }
                }
            }
        }
    }

    // Complete the spanning width calculation, now we have the maximum spanning size
    // for each spanning cell
    for (i = 0; i < m_colCount; i++)
    {
        int spanningWidth = spanningWidths[i];
        int spans = spanningWidthsSpanLengths[i];
        if (spanningWidth > 0)
        {
            // Now share the spanning width between columns within that span
            int spanningWidthLeft = spanningWidth;
            int stretchColCount = 0;
            for (k = i; k < (i+spans); k++)
            {
                int minColWidth = wxMax(minColWidths[k], minColWidthsNoWrap[k]);

                if (colWidths[k] > 0) // absolute or proportional width has been specified
                    spanningWidthLeft -= colWidths[k];
                else if (minColWidth > 0)
                {
                    spanningWidthLeft -= minColWidth;
                    // Allow this to stretch, otherwise we're likely to not allow
                    // any stretching and the spanned column will end up tiny.
                    stretchColCount ++;
                }
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
                    int minColWidth = wxMax(minColWidths[k], minColWidthsNoWrap[k]);
                    if (colWidths[k] <= 0) // absolute or proportional width has not been specified
                    {
                        int newWidth = colShare;
                        if (minColWidth > 0)
                            newWidth += minColWidth;

                        if (k == (i+spans-1))
                            newWidth += colShareRemainder; // ensure all pixels are filled
                        minColWidths[k] = newWidth;
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
    bool relaxConstraints = false;

    size_t phase;
    for (phase = 0; phase < 2; phase ++)
    {
        widthLeft = tableWidthMinusPadding;
        stretchColCount = 0;
        for (i = 0; i < m_colCount; i++)
        {
            // Subtract min width from width left, then
            // add the colShare to the min width
            if (colWidths[i] > 0) // absolute or proportional width has been specified
                widthLeft -= colWidths[i];
            else
            {
                int minColWidth = wxMax(minColWidths[i], minColWidthsNoWrap[i]);

                // If we're at phase 2, it means we had insufficient space, so
                // this time, don't take maxUnspecifiedColumnWidths into account
                // since we will simply apportion the remaining space.
                if (phase == 0)
                    minColWidth = wxMax(minColWidth, maxUnspecifiedColumnWidths[i]);
                if (minColWidth > 0)
                    widthLeft -= minColWidth;

                // Don't allow this to stretch if we're not wrapping; give the space
                // to other columns instead.
                if (minColWidthsNoWrap[i] == 0)
                    stretchColCount ++;
            }
        }
        if (widthLeft >= 0)
            break;
        else if (phase == 0)
        {
            relaxConstraints = true;

            // If there was insufficient space, we're now shrinking to fit the available width.
            stretchToFitTableWidth = true;
        }
    }

    // Now divide what's left between the remaining columns
    int colShare = 0;
    if (stretchColCount > 0)
        colShare = widthLeft / stretchColCount;
    int colShareRemainder = widthLeft - (colShare * stretchColCount);

    // Check if any columns will go below their minimum width. If so, give
    // up and size columns equally to avoid rendering problems.
    if (colShare < 0)
    {
        for (i = 0; i < m_colCount; i++)
        {
            int w = colWidths[i];
            if (w == 0)
                w = wxMax(minColWidths[i], minColWidthsNoWrap[i]);
            if ((w + colShare) < minColWidths[i])
            {
                stretchColCount = 0;
                break;
            }
        }
    }

    // Check we don't have enough space, in which case shrink all columns, overriding
    // any absolute/proportional widths
    // TODO: actually we would like to divide up the shrinkage according to size.
    // How do we calculate the proportions that will achieve this?
    // Could first choose an arbitrary value for stretching cells, and then calculate
    // factors to multiply each width by.
    // TODO: want to record this fact and pass to an iteration that tries e.g. min widths
    bool shareEqually = false;
    if (widthLeft < 0 || (stretchToFitTableWidth && (stretchColCount == 0)))
    {
        if (stretchColCount == 0)
        {
            // No columns to stretch or squash, so give up and divide space equally
            colShare = tableWidthMinusPadding / m_colCount;
            colShareRemainder = tableWidthMinusPadding - (colShare * m_colCount);
            shareEqually = true;
        }

        for (i = 0; i < m_colCount; i++)
        {
            colWidths[i] = 0;
        }
    }

    // We have to adjust the columns if either we need to shrink the
    // table to fit the parent/table width, or we explicitly set the
    // table width and need to stretch out the table.
    for (i = 0; i < m_colCount; i++)
    {
        if (colWidths[i] <= 0) // absolute or proportional width has not been specified
        {
            if (widthLeft < 0 || stretchToFitTableWidth)
            {
                int minColWidth = wxMax(minColWidths[i], minColWidthsNoWrap[i]);

                // Don't use a value for unspecified widths if we have insufficient space,
                // unless it's a nowrap cell which is likely to be small.
                // Actually this code is useless because if minColWidthsNoWrap exists,
                // it'll be the same value as maxUnspecifiedColumnWidths.
                if (!relaxConstraints)
                    minColWidth = wxMax(minColWidth, maxUnspecifiedColumnWidths[i]);

                if (minColWidth > 0 && !shareEqually)
                    colWidths[i] = minColWidth;

                // Don't allocate extra space if not wrapping since we assume a tight fit.
                // Unless shareEqually forces us to distribute space because we didn't have any
                // stretchable columns.
                if ((minColWidthsNoWrap[i] == 0) || shareEqually)
                    colWidths[i] += colShare;

                if (i == (m_colCount-1))
                    colWidths[i] += colShareRemainder; // ensure all pixels are filled
            }
            else
            {
                // We're not stretching or shrinking, so calculate the column width
                // consistent with how we calculated the remaining table width previously.
                int minColWidth = wxMax(minColWidths[i], minColWidthsNoWrap[i]);
                minColWidth = wxMax(minColWidth, maxUnspecifiedColumnWidths[i]);
                colWidths[i] = minColWidth;
            }
        }
    }

/*
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

        wxArrayInt actualWidths;
        actualWidths.Add(0, m_colCount);

        wxTextAttrDimensionConverter cellConverter(dc, scale);
        for (i = 0; i < m_colCount; i++)
        {
            wxRichTextCell* cell = GetCell(j, i);
            if (cell->IsShown())
            {
                // Get max specified cell height
                // Don't handle percentages for height
                if (cell->GetAttributes().GetTextBoxAttr().GetHeight().IsValid() && cell->GetAttributes().GetTextBoxAttr().GetHeight().GetUnits() != wxTEXT_ATTR_UNITS_PERCENTAGE)
                {
                    int h = cellConverter.GetPixels(cell->GetAttributes().GetTextBoxAttr().GetHeight());
                    if (h > maxSpecifiedCellHeight)
                        maxSpecifiedCellHeight = h;
                }

                if (colWidths[i] > 0) // absolute or proportional width has been specified
                {
                    int colSpan = cell->GetColSpan();
                    wxRect availableCellSpace;

                    // Take into account spans
                    if (colSpan > 1)
                    {
                        // Calculate the size of this spanning cell from its constituent columns
                        int xx = 0;
                        int spans = wxMin(colSpan, m_colCount - i);
                        for (k = i; k < (i+spans); k++)
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

                    // We now need to shift right by the width of any rowspanning cells above-left of us
                    int deltaX = GetRowspanDisplacement(this, j, i, paddingX, colWidths);
                    availableCellSpace.SetX(availableCellSpace.GetX() + deltaX);

                    // Lay out cell
                    cell->Invalidate(wxRICHTEXT_ALL);
                    cell->Layout(dc, context, availableCellSpace, availableSpace, style);

                    // TODO: use GetCachedSize().x to compute 'natural' size

                    x += (availableCellSpace.GetWidth() + paddingX);
                    if ((cell->GetCachedSize().y > maxCellHeight) && (cell->GetRowSpan() < 2))
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
                cell->Layout(dc, context, availableCellSpace, availableSpace, style|wxRICHTEXT_FIXED_HEIGHT|wxRICHTEXT_FIXED_WIDTH);

                maxRight = wxMax(maxRight, cell->GetPosition().x + cell->GetCachedSize().x);
            }
        }

        y += maxCellHeight;
        if (j < (m_rowCount-1))
            y += paddingY;
    }

    // Finally we need to expand any cell with rowspan > 1. We couldn't earlier; lower rows' heights weren't known
    ExpandCellsWithRowspan(this, paddingY, y, dc, context, availableSpace, style);

    // We need to add back the margins etc.
    {
        wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
        contentRect = wxRect(wxPoint(0, 0), wxSize(maxRight - availableSpace.x, y - availableSpace.y));
        GetBoxRects(dc, GetBuffer(), attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);
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

    return true;
}

// Adjusts the attributes for virtual attribute provision, collapsed borders, etc.
bool wxRichTextTable::AdjustAttributes(wxRichTextAttr& attr, wxRichTextDrawingContext& context)
{
    wxRichTextObject::AdjustAttributes(attr, context);

    if (attr.GetTextBoxAttr().HasCollapseBorders() &&
        attr.GetTextBoxAttr().GetCollapseBorders() == wxTEXT_BOX_ATTR_COLLAPSE_FULL)
    {
        // Padding between the table border and the table cells no longer
        // applies in collapsed mode.
        attr.GetTextBoxAttr().GetPadding().Reset();
    }

    return true;
}

// Finds the absolute position and row height for the given character position
bool wxRichTextTable::FindPosition(wxDC& dc, wxRichTextDrawingContext& context, long index, wxPoint& pt, int* height, bool forceLineStart)
{
    wxRichTextCell* child = GetCell(index+1);
    if (child)
    {
        // Find the position at the start of the child cell, since the table doesn't
        // have any caret position of its own.
        return child->FindPosition(dc, context, -1, pt, height, forceLineStart);
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
        return nullptr;
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
bool wxRichTextTable::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, wxRichTextDrawingContext& context, int flags, const wxPoint& position, const wxSize& parentSize, wxArrayInt* partialExtents) const
{
    return wxRichTextBox::GetRangeSize(range, size, descent, dc, context, flags, position, parentSize, partialExtents);
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
    m_rowCount = 0;
    m_colCount = 0;
}

bool wxRichTextTable::CreateTable(int rows, int cols)
{
    ClearTable();

    wxRichTextAttr cellattr;
    cellattr.SetTextColour(GetBasicStyle().GetTextColour());

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
            cell->GetAttributes() = cellattr;

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
        return nullptr;
}

// Returns a selection object specifying the selections between start and end character positions.
// For example, a table would deduce what cells (of range length 1) are selected when dragging across the table.
wxRichTextSelection wxRichTextTable::GetSelection(long start, long end) const
{
    wxRichTextSelection selection;
    selection.SetContainer(const_cast<wxRichTextTable*>(this));

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
    bool haveControl = (buffer && buffer->GetRichTextCtrl() != nullptr);
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

wxPosition wxRichTextTable::GetFocusedCell() const
{
    wxPosition position(-1, -1);
    const wxRichTextObject* focus = GetBuffer()->GetRichTextCtrl()->GetFocusObject();

    for (int row = 0; row < GetRowCount(); ++row)
    {
        for (int col = 0; col < GetColumnCount(); ++col)
        {
            if (GetCell(row, col) == focus)
            {
              position.SetRow(row);
              position.SetCol(col);
              return position;
            }
        }
    }

    return position;
}

int wxRichTextTable::HitTest(wxDC& dc, wxRichTextDrawingContext& context, const wxPoint& pt, long& textPosition, wxRichTextObject** obj, wxRichTextObject** contextObj, int flags)
{
    for (int row = 0; row < GetRowCount(); ++row)
    {
        for (int col = 0; col < GetColumnCount(); ++col)
        {
            wxRichTextCell* cell = GetCell(row, col);
            if (cell->wxRichTextObject::HitTest(dc, context, pt, textPosition, obj, contextObj, flags) != wxRICHTEXT_HITTEST_NONE)
            {
                return cell->HitTest(dc, context, pt, textPosition, obj, contextObj, flags);
            }
        }
    }

    return wxRICHTEXT_HITTEST_NONE;
}

bool wxRichTextTable::DeleteRows(int startRow, int noRows)
{
    wxASSERT((startRow + noRows) <= m_rowCount);
    if ((startRow + noRows) > m_rowCount)
        return false;

    wxCHECK_MSG(noRows != m_rowCount, false, "Trying to delete all the cells in a table");

    wxRichTextBuffer* buffer = GetBuffer();
    wxRichTextCtrl* rtc = buffer->GetRichTextCtrl();

    wxRichTextAction* action = nullptr;
    wxRichTextTable* clone = nullptr;
    if (!rtc->SuppressingUndo())
    {
        // Create a clone containing the current state of the table. It will be used to Undo the action
        clone = wxStaticCast(this->Clone(), wxRichTextTable);
        clone->SetParent(GetParent());
        action = new wxRichTextAction(nullptr, _("Delete Row"), wxRICHTEXT_CHANGE_OBJECT, buffer, this, rtc);
        action->SetObject(this);
        action->SetPosition(GetRange().GetStart());
    }

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

    if (!rtc->SuppressingUndo())
    {
        buffer->SubmitAction(action);
        // Finally store the original-state clone; doing so earlier would cause various failures
        action->StoreObject(clone);
    }

    return true;
}

bool wxRichTextTable::DeleteColumns(int startCol, int noCols)
{
    wxASSERT((startCol + noCols) <= m_colCount);
    if ((startCol + noCols) > m_colCount)
        return false;

    wxCHECK_MSG(noCols != m_colCount, false, "Trying to delete all the cells in a table");

    wxRichTextBuffer* buffer = GetBuffer();
    wxRichTextCtrl* rtc = buffer->GetRichTextCtrl();

    wxRichTextAction* action = nullptr;
    wxRichTextTable* clone = nullptr;
    if (!rtc->SuppressingUndo())
    {
        // Create a clone containing the current state of the table. It will be used to Undo the action
        clone = wxStaticCast(this->Clone(), wxRichTextTable);
        clone->SetParent(GetParent());
        action = new wxRichTextAction(nullptr, _("Delete Column"), wxRICHTEXT_CHANGE_OBJECT, buffer, this, rtc);
        action->SetObject(this);
        action->SetPosition(GetRange().GetStart());
    }

    bool deleteRows = (noCols == m_colCount);

    int i, j;
    for (i = 0; i < m_rowCount; i++)
    {
        wxRichTextObjectPtrArray& colArray = m_cells[deleteRows ? 0 : i];
        for (j = 0; j < noCols; j++)
        {
            wxRichTextObject* cell = colArray[startCol];
            RemoveChild(cell, true);
            colArray.RemoveAt(startCol);
        }

        if (deleteRows)
            m_cells.RemoveAt(0);
    }

    if (deleteRows)
        m_rowCount = 0;
    m_colCount = m_colCount - noCols;

    if (!rtc->SuppressingUndo())
    {
        buffer->SubmitAction(action);
        // Finally store the original-state clone; doing so earlier would cause various failures
        action->StoreObject(clone);
    }

    return true;
}

bool wxRichTextTable::AddRows(int startRow, int noRows, const wxRichTextAttr& attr)
{
    wxASSERT(startRow <= m_rowCount);
    if (startRow > m_rowCount)
        return false;

    wxRichTextBuffer* buffer = GetBuffer();
    wxRichTextAction* action = nullptr;
    wxRichTextTable* clone = nullptr;

    if (!buffer->GetRichTextCtrl()->SuppressingUndo())
    {
        // Create a clone containing the current state of the table. It will be used to Undo the action
        clone = wxStaticCast(this->Clone(), wxRichTextTable);
        clone->SetParent(GetParent());
        action = new wxRichTextAction(nullptr, _("Add Row"), wxRICHTEXT_CHANGE_OBJECT, buffer, this, buffer->GetRichTextCtrl());
        action->SetObject(this);
        action->SetPosition(GetRange().GetStart());
    }

    wxRichTextAttr cellattr = attr;
    if (!cellattr.GetTextColour().IsOk())
        cellattr.SetTextColour(buffer->GetBasicStyle().GetTextColour());

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
            cell->GetAttributes() = cellattr;

            AppendChild(cell);
            cell->AddParagraph(wxEmptyString);
            colArray.Add(cell);
        }
    }

    m_rowCount = m_rowCount + noRows;

    if (!buffer->GetRichTextCtrl()->SuppressingUndo())
    {
        buffer->SubmitAction(action);
        // Finally store the original-state clone; doing so earlier would cause various failures
        action->StoreObject(clone);
    }

    return true;
}

bool wxRichTextTable::AddColumns(int startCol, int noCols, const wxRichTextAttr& attr)
{
    wxASSERT(startCol <= m_colCount);
    if (startCol > m_colCount)
        return false;

    wxRichTextBuffer* buffer = GetBuffer();
    wxRichTextAction* action = nullptr;
    wxRichTextTable* clone = nullptr;

    if (!buffer->GetRichTextCtrl()->SuppressingUndo())
    {
        // Create a clone containing the current state of the table. It will be used to Undo the action
        clone = wxStaticCast(this->Clone(), wxRichTextTable);
        clone->SetParent(GetParent());
        action = new wxRichTextAction(nullptr, _("Add Column"), wxRICHTEXT_CHANGE_OBJECT, buffer, this, buffer->GetRichTextCtrl());
        action->SetObject(this);
        action->SetPosition(GetRange().GetStart());
    }

    wxRichTextAttr cellattr = attr;
    if (!cellattr.GetTextColour().IsOk())
        cellattr.SetTextColour(buffer->GetBasicStyle().GetTextColour());

    int i, j;
    for (i = 0; i < m_rowCount; i++)
    {
        wxRichTextObjectPtrArray& colArray = m_cells[i];
        for (j = 0; j < noCols; j++)
        {
            wxRichTextCell* cell = new wxRichTextCell;
            cell->GetAttributes() = cellattr;

            AppendChild(cell);
            cell->AddParagraph(wxEmptyString);

            if (startCol == m_colCount)
                colArray.Add(cell);
            else
                colArray.Insert(cell, startCol+j);
        }
    }

    m_colCount = m_colCount + noCols;

    if (!buffer->GetRichTextCtrl()->SuppressingUndo())
    {
        buffer->SubmitAction(action);
        // Finally store the original-state clone; doing so earlier would cause various failures
        action->StoreObject(clone);
    }

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

bool wxRichTextTableBlock::ComputeBlockForSelection(wxRichTextTable* table, wxRichTextCtrl* ctrl, bool requireCellSelection)
{
    if (!ctrl)
        return false;

    ColStart() = 0;
    ColEnd() = table->GetColumnCount()-1;
    RowStart() = 0;
    RowEnd() = table->GetRowCount()-1;

    wxRichTextSelection selection = ctrl->GetSelection();
    if (selection.IsValid() && selection.GetContainer() == table)
    {
        // Start with an invalid block and increase.
        wxRichTextTableBlock selBlock(-1, -1, -1, -1);
        int row, col;
        for (row = 0; row < table->GetRowCount(); row++)
        {
            for (col = 0; col < table->GetColumnCount(); col++)
            {
                if (selection.WithinSelection(table->GetCell(row, col)->GetRange().GetStart()))
                {
                    if (selBlock.ColStart() == -1)
                        selBlock.ColStart() = col;
                    if (selBlock.ColEnd() == -1)
                        selBlock.ColEnd() = col;
                    if (col < selBlock.ColStart())
                        selBlock.ColStart() = col;
                    if (col > selBlock.ColEnd())
                        selBlock.ColEnd() = col;

                    if (selBlock.RowStart() == -1)
                        selBlock.RowStart() = row;
                    if (selBlock.RowEnd() == -1)
                        selBlock.RowEnd() = row;
                    if (row < selBlock.RowStart())
                        selBlock.RowStart() = row;
                    if (row > selBlock.RowEnd())
                        selBlock.RowEnd() = row;
                }
            }
        }

        if (selBlock.RowStart() != -1 && selBlock.RowEnd() != -1 && selBlock.ColStart() != -1 && selBlock.ColEnd() != -1)
            (*this) = selBlock;
    }
    else
    {
        // See if a whole cell's contents is selected, in which case we can treat the cell as selected.
        // wxRTC lacks the ability to select a single cell.
        wxRichTextCell* cell = wxDynamicCast(ctrl->GetFocusObject(), wxRichTextCell);
        if (cell && (!requireCellSelection || (ctrl->HasSelection() && ctrl->GetSelectionRange() == cell->GetOwnRange())))
        {
            int row, col;
            if (table->GetCellRowColumnPosition(cell->GetRange().GetStart(), row, col))
            {
                RowStart() = row;
                RowEnd() = row;
                ColStart() = col;
                ColEnd() = col;
            }
        }
    }

    return true;
}

// Does this block represent the whole table?
bool wxRichTextTableBlock::IsWholeTable(wxRichTextTable* table) const
{
    return (ColStart() == 0 && RowStart() == 0 && ColEnd() == (table->GetColumnCount()-1) && RowEnd() == (table->GetRowCount()-1));
}

// Returns the cell focused in the table, if any
wxRichTextCell* wxRichTextTableBlock::GetFocusedCell(wxRichTextCtrl* ctrl)
{
    if (!ctrl)
        return nullptr;

    wxRichTextCell* cell = wxDynamicCast(ctrl->GetFocusObject(), wxRichTextCell);
    return cell;
}

/*
 * Module to initialise and clean up handlers
 */

class wxRichTextModule: public wxModule
{
    wxDECLARE_DYNAMIC_CLASS(wxRichTextModule);
public:
    wxRichTextModule() {}
    bool OnInit() override
    {
        wxRichTextBuffer::SetRenderer(new wxRichTextStdRenderer);
        wxRichTextBuffer::InitStandardHandlers();
        wxRichTextParagraph::InitDefaultTabs();

#if wxUSE_XML
        wxRichTextXMLHandler::RegisterNodeName(wxT("text"), wxT("wxRichTextPlainText"));
        wxRichTextXMLHandler::RegisterNodeName(wxT("symbol"), wxT("wxRichTextPlainText"));
        wxRichTextXMLHandler::RegisterNodeName(wxT("image"), wxT("wxRichTextImage"));
        wxRichTextXMLHandler::RegisterNodeName(wxT("paragraph"), wxT("wxRichTextParagraph"));
        wxRichTextXMLHandler::RegisterNodeName(wxT("paragraphlayout"), wxT("wxRichTextParagraphLayoutBox"));
        wxRichTextXMLHandler::RegisterNodeName(wxT("textbox"), wxT("wxRichTextBox"));
        wxRichTextXMLHandler::RegisterNodeName(wxT("cell"), wxT("wxRichTextCell"));
        wxRichTextXMLHandler::RegisterNodeName(wxT("table"), wxT("wxRichTextTable"));
        wxRichTextXMLHandler::RegisterNodeName(wxT("field"), wxT("wxRichTextField"));
#endif // wxUSE_XML

        return true;
    }
    void OnExit() override
    {
        wxRichTextBuffer::CleanUpHandlers();
        wxRichTextBuffer::CleanUpDrawingHandlers();
        wxRichTextBuffer::CleanUpFieldTypes();
#if wxUSE_XML
        wxRichTextXMLHandler::ClearNodeToClassMap();
#endif // wxUSE_XML
        wxRichTextDecimalToRoman(-1);
        wxRichTextParagraph::ClearDefaultTabs();
        wxRichTextCtrl::ClearAvailableFontNames();
        wxRichTextBuffer::SetRenderer(nullptr);
    }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextModule, wxModule);


// If the richtext lib is dynamically loaded after the app has already started
// (such as from wxPython) then the built-in module system will not init this
// module.  Provide this function to do it manually.
void wxRichTextModuleInit()
{
    wxModule* module = new wxRichTextModule;
    wxModule::RegisterModule(module);
    wxModule::InitializeModules();
}


/*!
 * Commands for undo/redo
 *
 */

wxRichTextCommand::wxRichTextCommand(const wxString& name, wxRichTextCommandId id, wxRichTextBuffer* buffer,
                                     wxRichTextParagraphLayoutBox* container, wxRichTextCtrl* ctrl, bool ignoreFirstTime): wxCommand(true, name)
{
    m_freeze = ctrl ? ctrl->IsFrozen() : false;
    /* wxRichTextAction* action = */ new wxRichTextAction(this, name, id, buffer, container, ctrl, ignoreFirstTime);
}

wxRichTextCommand::wxRichTextCommand(const wxString& name): wxCommand(true, name)
{
    m_freeze = false;
}

wxRichTextCommand::~wxRichTextCommand()
{
    ClearActions();
}

void wxRichTextCommand::AddAction(wxRichTextAction* action)
{
    if (!m_actions.Member(action))
        m_actions.Append(action);

    if (!m_freeze && action->GetRichTextCtrl() && action->GetRichTextCtrl()->IsFrozen())
        m_freeze = true;
}

bool wxRichTextCommand::Do()
{
    for (wxList::compatibility_iterator node = m_actions.GetFirst(); node; node = node->GetNext())
    {
        wxRichTextAction* action = (wxRichTextAction*) node->GetData();
        if (GetFreeze() && node == m_actions.GetFirst() && action->GetRichTextCtrl())
            action->GetRichTextCtrl()->Freeze();

        action->Do();

        if (GetFreeze() && node == m_actions.GetLast() && action->GetRichTextCtrl())
            action->GetRichTextCtrl()->Thaw();
    }

    return true;
}

bool wxRichTextCommand::Undo()
{
    for (wxList::compatibility_iterator node = m_actions.GetLast(); node; node = node->GetPrevious())
    {
        wxRichTextAction* action = (wxRichTextAction*) node->GetData();
        if (GetFreeze() && node == m_actions.GetLast() && action->GetRichTextCtrl())
            action->GetRichTextCtrl()->Freeze();

        action->Undo();

        if (GetFreeze() && node == m_actions.GetFirst() && action->GetRichTextCtrl())
            action->GetRichTextCtrl()->Thaw();
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
    m_object = nullptr;
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


void wxRichTextAction::CalculateRefreshOptimizations(wxArrayInt& optimizationLineCharPositions, wxArrayInt& optimizationLineYPositions,
    wxRect& oldFloatRect)
{
    // Store a list of line start character and y positions so we can figure out which area
    // we need to refresh

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
    wxRichTextParagraphLayoutBox* container = GetContainer();
    wxASSERT(container != nullptr);
    if (!container)
        return;

    // No point in doing optimizations if we're not going to use them
    if (m_ctrl && m_ctrl->IsFrozen())
        return;

    // NOTE: we're assuming that the buffer is laid out correctly at this point.
    // If we had several actions, which only invalidate and leave layout until the
    // paint handler is called, then this might not be true. So we may need to switch
    // optimisation on only when we're simply adding text and not simultaneously
    // deleting a selection, for example. Or, we make sure the buffer is laid out correctly
    // first, but of course this means we'll be doing it twice.
    if (!m_buffer->IsDirty() && m_ctrl) // can only do optimisation if the buffer is already laid out correctly
    {
        wxSize clientSize = m_ctrl->GetUnscaledSize(m_ctrl->GetClientSize());
        wxPoint firstVisiblePt = m_ctrl->GetUnscaledPoint(m_ctrl->GetFirstVisiblePoint());
        int lastY = firstVisiblePt.y + clientSize.y;

        wxRichTextParagraph* para = container->GetParagraphAtPosition(GetRange().GetStart());
        wxRichTextObjectList::compatibility_iterator firstNode = container->GetChildren().Find(para);
        wxRichTextObjectList::compatibility_iterator node = firstNode;
        bool stop = false;

        while (!stop && node)
        {
            wxRichTextParagraph* child = (wxRichTextParagraph*) node->GetData();
            wxRichTextLineVector::const_iterator it = child->GetLines().begin();
            while (!stop && it != child->GetLines().end())
            {
                wxRichTextLine* line = *it;
                wxPoint pt = line->GetAbsolutePosition();
                wxRichTextRange range = line->GetAbsoluteRange();

                if (pt.y > lastY)
                {
                    stop = true;
                }
                else if (range.GetStart() > GetPosition() && pt.y >= firstVisiblePt.y)
                {
                    optimizationLineCharPositions.Add(range.GetStart());
                    optimizationLineYPositions.Add(pt.y);
                }

                ++it;
            }

            node = node->GetNext();
        }

        if (wxRichTextBuffer::GetFloatingLayoutMode() && container->GetFloatingObjectCount() > 0)
        {
            // We will use a simple criterion - if any of the paragraphs following the
            // modification point are affected by floats in other paragraphs,
            // then we will simply update the rest of the screen.
            wxRichTextObjectList::compatibility_iterator childNode = firstNode;
            while (childNode)
            {
                wxRichTextParagraph* child = (wxRichTextParagraph*) childNode->GetData();
                if (child->GetRect().GetTop() > lastY)
                    break;
                else if (child->GetImpactedByFloatingObjects() == 1)
                {
                    oldFloatRect = wxRect(0, 0, clientSize.x, lastY);
                    break;
                }
                childNode = childNode->GetNext();
            }
        }
    }
#endif
}

bool wxRichTextAction::Do()
{
    m_buffer->Modify(true);

    wxRichTextParagraphLayoutBox* container = GetContainer();
    wxASSERT(container != nullptr);
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
            wxRect oldFloatRect;

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            CalculateRefreshOptimizations(optimizationLineCharPositions, optimizationLineYPositions, oldFloatRect);
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

            UpdateAppearance(newCaretPosition, true /* send update event */, oldFloatRect, & optimizationLineCharPositions, & optimizationLineYPositions, true /* do */);

            wxRichTextEvent cmdEvent(
                wxEVT_RICHTEXT_CONTENT_INSERTED,
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
            wxRect oldFloatRect;

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            CalculateRefreshOptimizations(optimizationLineCharPositions, optimizationLineYPositions, oldFloatRect);
#endif

            // Check if the current object focus needs to be changed before deletion of content
            if (m_ctrl)
            {
                wxRichTextObject* c = m_ctrl->GetFocusObject();
                while (c)
                {
                    if (c == container)
                        m_ctrl->StoreFocusObject(container);

                    if (m_ctrl && (c == m_ctrl->GetSelection().GetContainer()))
                        m_ctrl->SelectNone();
                    c = c->GetParent();
                }
            }

            container->DeleteRange(GetRange());
            container->UpdateRanges();
            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            container->InvalidateHierarchy(wxRichTextRange(GetRange().GetStart(), GetRange().GetStart()));

            long caretPos = GetRange().GetStart()-1;
            if (caretPos >= container->GetOwnRange().GetEnd())
                caretPos --;

            UpdateAppearance(caretPos, true /* send update event */, oldFloatRect, & optimizationLineCharPositions, & optimizationLineYPositions, true /* do */);

            wxRichTextEvent cmdEvent(
                wxEVT_RICHTEXT_CONTENT_DELETED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());
            cmdEvent.SetContainer(container);

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_CHANGE_STYLE:
    case wxRICHTEXT_CHANGE_PROPERTIES:
        {
            ApplyParagraphs(GetNewParagraphs());

            // Invalidate the whole buffer if there were floating objects
            if (wxRichTextBuffer::GetFloatingLayoutMode() && container->GetFloatingObjectCount() > 0)
                m_buffer->InvalidateHierarchy(wxRICHTEXT_ALL);
            else
            {
                // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
                // Layout() would stop prematurely at the top level.
                container->InvalidateHierarchy(GetRange());
            }

            UpdateAppearance(GetPosition());

            wxRichTextEvent cmdEvent(
                m_cmdId == wxRICHTEXT_CHANGE_STYLE ? wxEVT_RICHTEXT_STYLE_CHANGED : wxEVT_RICHTEXT_PROPERTIES_CHANGED,
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
            // Invalidate the whole buffer if there were floating objects
            if (wxRichTextBuffer::GetFloatingLayoutMode() && container->GetFloatingObjectCount() > 0)
                m_buffer->InvalidateHierarchy(wxRICHTEXT_ALL);
            else
                container->InvalidateHierarchy(GetRange());

            UpdateAppearance(GetPosition(), true);

            wxRichTextEvent cmdEvent(
                wxEVT_RICHTEXT_STYLE_CHANGED,
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
            if (obj && m_object && m_ctrl)
            {
                // The plan is to swap the current object with the stored, previous-state, clone
                // We can't get 'node' from the containing buffer (as it doesn't directly store objects)
                // so use the parent paragraph
                wxRichTextCompositeObject* parent = wxDynamicCast(obj->GetParent(), wxRichTextCompositeObject);
                wxCHECK_MSG(parent, false, wxT("Invalid parent"));

                // Check that at least one is a paragraph, but not both.
                wxCHECK_MSG((!obj->IsKindOf(CLASSINFO(wxRichTextParagraph)) && parent->IsKindOf(CLASSINFO(wxRichTextParagraph))) ||
                            (obj->IsKindOf(CLASSINFO(wxRichTextParagraph)) && !parent->IsKindOf(CLASSINFO(wxRichTextParagraph)))
                            , false, wxT("Either the object or the parent must be a paragraph"));

                // The stored object, m_object, may have a stale parent. This would cause
                // a crash during layout, so use obj's parent, which should be the correct one.
                // (An alternative would be to return the parent too from m_objectAddress.GetObject(),
                // or to set obj's parent there before returning)
                m_object->SetParent(parent);
                if (parent)
                {
                    wxRichTextObjectList::compatibility_iterator node = parent->GetChildren().Find(obj);
                    if (node)
                    {
                        wxRichTextObject* nodeObj = node->GetData();
                        node->SetData(m_object);
                        m_object = nodeObj;
                    }
                }
            }

            // We can't rely on the current focus-object remaining valid, if it's e.g. a table's cell.
            // And we can't cope with this in the calling code: a user may later click in the cell
            // before deciding to Undo() or Redo(). So play safe and set focus to the buffer.
            if (m_ctrl)
                m_ctrl->SetFocusObject(m_buffer, false);

            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            // Invalidate the whole buffer if there were floating objects
            if (wxRichTextBuffer::GetFloatingLayoutMode() && container->GetFloatingObjectCount() > 0)
                m_buffer->InvalidateHierarchy(wxRICHTEXT_ALL);
            else
                container->InvalidateHierarchy(GetRange());

            UpdateAppearance(GetPosition(), true);

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
    wxASSERT(container != nullptr);
    if (!container)
        return false;

    switch (m_cmdId)
    {
    case wxRICHTEXT_INSERT:
        {
            wxArrayInt optimizationLineCharPositions;
            wxArrayInt optimizationLineYPositions;
            wxRect oldFloatRect;

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            CalculateRefreshOptimizations(optimizationLineCharPositions, optimizationLineYPositions, oldFloatRect);
#endif

            // Check if the current object focus needs to be changed before deletion of content
            if (m_ctrl)
            {
                wxRichTextObject* c = m_ctrl->GetFocusObject();
                while (c)
                {
                    if (c == container)
                        m_ctrl->StoreFocusObject(container);

                    if (m_ctrl && (c == m_ctrl->GetSelection().GetContainer()))
                        m_ctrl->SelectNone();
                    c = c->GetParent();
                }
            }

            container->DeleteRange(GetRange());
            container->UpdateRanges();

            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            container->InvalidateHierarchy(wxRichTextRange(GetRange().GetStart(), GetRange().GetStart()));

            long newCaretPosition = GetPosition() - 1;

            UpdateAppearance(newCaretPosition, true, /* send update event */ oldFloatRect, & optimizationLineCharPositions, & optimizationLineYPositions, false /* undo */);

            wxRichTextEvent cmdEvent(
                wxEVT_RICHTEXT_CONTENT_DELETED,
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
            wxRect oldFloatRect;

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            CalculateRefreshOptimizations(optimizationLineCharPositions, optimizationLineYPositions, oldFloatRect);
#endif

            container->InsertFragment(GetRange().GetStart(), m_oldParagraphs);
            container->UpdateRanges();

            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            container->InvalidateHierarchy(GetRange());

            UpdateAppearance(GetPosition(), true, /* send update event */ oldFloatRect, & optimizationLineCharPositions, & optimizationLineYPositions, false /* undo */);

            wxRichTextEvent cmdEvent(
                wxEVT_RICHTEXT_CONTENT_INSERTED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());
            cmdEvent.SetContainer(container);

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_CHANGE_STYLE:
    case wxRICHTEXT_CHANGE_PROPERTIES:
        {
            ApplyParagraphs(GetOldParagraphs());
            // InvalidateHierarchy goes up the hierarchy as well as down, otherwise with a nested object,
            // Layout() would stop prematurely at the top level.
            container->InvalidateHierarchy(GetRange());

            UpdateAppearance(GetPosition());

            wxRichTextEvent cmdEvent(
                m_cmdId == wxRICHTEXT_CHANGE_STYLE ? wxEVT_RICHTEXT_STYLE_CHANGED : wxEVT_RICHTEXT_PROPERTIES_CHANGED,
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
void wxRichTextAction::UpdateAppearance(long caretPosition, bool sendUpdateEvent, const wxRect& oldFloatRect, wxArrayInt* optimizationLineCharPositions, wxArrayInt* optimizationLineYPositions, bool isDoCmd)
{
    wxRichTextParagraphLayoutBox* container = GetContainer();
    wxASSERT(container != nullptr);
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
            // Refresh everything if container changed size.
            if (container->GetParent() && containerRect != container->GetRect())
            {
                m_ctrl->Refresh(false);
            }
            else

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            // Find refresh rectangle if we are in a position to optimise refresh
            if ((m_cmdId == wxRICHTEXT_INSERT || m_cmdId == wxRICHTEXT_DELETE) && optimizationLineCharPositions)
            {
                size_t i;

                wxSize clientSize = m_ctrl->GetUnscaledSize(m_ctrl->GetClientSize());
                wxPoint firstVisiblePt = m_ctrl->GetUnscaledPoint(m_ctrl->GetFirstVisiblePoint());
                int lastPossibleY = firstVisiblePt.y + clientSize.y;

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
                long pos = GetRange().GetStart();

                wxRichTextParagraph* para = container->GetParagraphAtPosition(pos, false /* is not caret pos */);
                // Since we support floating layout, we should redraw the whole para instead of just
                // the first line touching the invalid range.
                if (para)
                {
                    // In case something was drawn above the paragraph,
                    // such as a line break, allow a little extra.
                    firstY = para->GetPosition().y - 4;
                }

                wxRichTextObjectList::compatibility_iterator firstNode = container->GetChildren().Find(para);
                wxRichTextObjectList::compatibility_iterator node = firstNode;
                wxRichTextObjectList::compatibility_iterator lastNode = wxRichTextObjectList::compatibility_iterator();
                bool stop = false;

                while (!stop && node)
                {
                    wxRichTextParagraph* child = (wxRichTextParagraph*) node->GetData();
                    wxRichTextLineVector::const_iterator it = child->GetLines().begin();
                    while (!stop && it != child->GetLines().end())
                    {
                        wxRichTextLine* line = *it;
                        wxPoint pt = line->GetAbsolutePosition();
                        wxRichTextRange range = line->GetAbsoluteRange();
                        const bool isLastLine = it + 1 == child->GetLines().end();

                        // we want to find the first line that is in the same position
                        // as before. This will mean we're at the end of the changed text.

                        if (pt.y > lastY) // going past the end of the window, no more info
                        {
                            stop = true;
                        }
                        // Detect last line in the buffer
                        else if (isLastLine && para->GetRange().Contains(container->GetOwnRange().GetEnd()))
                        {
                            // If deleting text, make sure we refresh below as well as above
                            if (positionOffset >= 0)
                            {
                                foundEnd = true;
                                lastY = pt.y + line->GetSize().y;
                            }

                            lastNode = node;

                            stop = true;

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

                                    lastY = pt.y + line->GetSize().y;

                                    lastNode = node;

                                    stop = true;

                                    break;
                                }
                            }
                        }

                        ++it;
                    }

                    node = node->GetNext();
                }

                firstY = wxMax(firstVisiblePt.y, firstY);
                if (!foundEnd)
                    lastY = firstVisiblePt.y + clientSize.y;

                if (wxRichTextBuffer::GetFloatingLayoutMode())
                {
                    if (oldFloatRect.GetBottom() > 0)
                        lastY = wxMax(lastY, oldFloatRect.GetBottom());

                    // Now find the first paragraph that isn't affected by any floating objects,
                    // which means the reformatting stopped at this point.
                    if (lastNode && (container->GetFloatingObjectCount() > 0) && (lastY < lastPossibleY))
                    {
                        wxRichTextObjectList::compatibility_iterator childNode = lastNode;
                        while (childNode)
                        {
                            wxRichTextParagraph* child = (wxRichTextParagraph*) childNode->GetData();
                            if (child->GetImpactedByFloatingObjects() == 0)
                            {
                                wxRect childRect = child->GetRect();
                                if (childRect.GetBottom() > lastY)
                                    lastY = wxMin(childRect.GetBottom(), lastPossibleY);
                                break;
                            }

                            childNode = childNode->GetNext();
                        }
                    }
                }

                // Convert to device coordinates
                wxRect rect(m_ctrl->GetPhysicalPoint(m_ctrl->GetScaledPoint(wxPoint(firstVisiblePt.x, firstY))), m_ctrl->GetScaledSize(wxSize(clientSize.x, lastY - firstY)));
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
    wxASSERT(container != nullptr);
    if (!container)
        return;

    wxRichTextObjectList::compatibility_iterator node = fragment.GetChildren().GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != nullptr);

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

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextImage, wxRichTextObject);

wxRichTextImage::wxRichTextImage(const wxImage& image, wxRichTextObject* parent, wxRichTextAttr* charStyle):
    wxRichTextObject(parent)
{
    Init();
    m_imageBlock.MakeImageBlockDefaultQuality(image, wxBITMAP_TYPE_PNG);
    if (charStyle)
        SetAttributes(*charStyle);
}

wxRichTextImage::wxRichTextImage(const wxRichTextImageBlock& imageBlock, wxRichTextObject* parent, wxRichTextAttr* charStyle):
    wxRichTextObject(parent)
{
    Init();
    m_imageBlock = imageBlock;
    if (charStyle)
        SetAttributes(*charStyle);
}

wxRichTextImage::~wxRichTextImage()
{
}

void wxRichTextImage::Init()
{
    m_originalImageSize = wxSize(-1, -1);
    m_imageState = ImageState_Unloaded;
}

/// Create a cached image at the required size
bool wxRichTextImage::LoadImageCache(wxDC& dc, wxRichTextDrawingContext& context, wxSize& retImageSize, bool resetCache, const wxSize& parentSize)
{
    if (!m_imageBlock.IsOk())
    {
        m_imageState = ImageState_Bad;
        return false;
    }

    // Don't repeat unless absolutely necessary
    if (m_imageCache.IsOk() && !resetCache && !context.GetLayingOut())
    {
        retImageSize = wxSize(m_imageCache.GetLogicalWidth(), m_imageCache.GetLogicalHeight());
        return true;
    }

    wxRichTextAttr attr(GetAttributes());
    AdjustAttributes(attr, context);

    if (!context.GetImagesEnabled())
    {
        if (resetCache || !m_imageCache.IsOk())
        {
            wxBitmap bitmap(image_placeholder24x24_xpm);
            m_imageCache = bitmap;
            m_imageState = ImageState_Loaded;
        }
        retImageSize = wxSize(m_imageCache.GetLogicalWidth(), m_imageCache.GetLogicalHeight());
        return true;
    }

    // If we have an original image size, use that to compute the cached bitmap size
    // instead of loading the image each time. This way we can avoid loading
    // the image so long as the new cached bitmap size hasn't changed.

    wxImage image;
    if (resetCache || m_originalImageSize.GetWidth() <= 0 || m_originalImageSize.GetHeight() <= 0)
    {
        m_imageCache = wxNullBitmap;
        m_imageState = ImageState_Unloaded;

        if (!m_imageBlock.Load(image) || !image.IsOk())
        {
            wxBitmap bitmap(image_placeholder24x24_xpm);
            m_imageCache = bitmap;
            m_originalImageSize = wxSize(bitmap.GetLogicalWidth(), bitmap.GetLogicalHeight());
            m_imageState = ImageState_Bad;
            retImageSize = m_originalImageSize;
            return false;
        }

        m_originalImageSize = wxSize(image.GetWidth(), image.GetHeight());
    }

    int width = m_originalImageSize.GetWidth();
    int height = m_originalImageSize.GetHeight();

    int parentWidth = 0;
    int parentHeight = 0;

    int maxWidth = -1;
    int maxHeight = -1;

    wxSize sz = parentSize;
    if (sz == wxDefaultSize)
    {
        if (GetParent() && GetParent()->GetParent())
            sz = GetParent()->GetParent()->GetCachedSize();
    }

    wxRichTextBuffer* buffer = GetBuffer();

    if (sz != wxDefaultSize)
    {
        if (buffer)
        {
            // Find the actual space available when margin is taken into account
            wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
            marginRect = wxRect(0, 0, sz.x, sz.y);
            if (GetParent() && GetParent()->GetParent())
            {
                buffer->GetBoxRects(dc, buffer, GetParent()->GetParent()->GetAttributes(), marginRect, borderRect, contentRect, paddingRect, outlineRect);
                sz = contentRect.GetSize();
            }

            // Take away space used by the image's margins
            {
                // Find the actual space available when margin is taken into account
                wxRect imgMarginRect, imgBorderRect, imgContentRect, imgPaddingRect, imgOutlineRect;
                imgMarginRect = wxRect(0, 0, 100, 100); // To force GetBoxRects to return content rect
                GetBoxRects(dc, buffer, GetAttributes(), imgMarginRect, imgBorderRect, imgContentRect, imgPaddingRect, imgOutlineRect);
                sz += (imgContentRect.GetSize() - wxSize(100, 100));
            }

            // Use a minimum size to stop images becoming very small
            parentWidth = wxMax(100, sz.GetWidth());
            parentHeight = wxMax(100, sz.GetHeight());

            // Start with a maximum width of the control size, even if not specified by the content,
            // to minimize the amount of picture overlapping the right-hand side
            maxWidth = parentWidth;
        }
    }

    wxTextAttrDimensionConverter converter(dc, buffer ? buffer->GetScale() : 1.0, wxSize(parentWidth, parentHeight));

    if (attr.GetTextBoxAttr().GetWidth().IsValid() && attr.GetTextBoxAttr().GetWidth().GetValue() > 0)
    {
        int widthPixels = converter.GetPixels(attr.GetTextBoxAttr().GetWidth(), wxHORIZONTAL);
        if (widthPixels > 0)
            width = widthPixels;
    }

    // Limit to max width

    if (attr.GetTextBoxAttr().GetMaxSize().GetWidth().IsValid() && attr.GetTextBoxAttr().GetMaxSize().GetWidth().GetValue() > 0)
    {
        int mw = converter.GetPixels(attr.GetTextBoxAttr().GetMaxSize().GetWidth(), wxHORIZONTAL);

        // If we already have a smaller max width due to the constraints of the control size,
        // don't use the larger max width.
        if (mw > 0 && ((maxWidth == -1) || (mw < maxWidth)))
            maxWidth = mw;
    }

    if (maxWidth > 0 && width > maxWidth)
        width = maxWidth;

    // Preserve the aspect ratio
    if (width != m_originalImageSize.GetWidth())
        height = (int) (float(m_originalImageSize.GetHeight()) * (float(width)/float(m_originalImageSize.GetWidth())));

    if (attr.GetTextBoxAttr().GetHeight().IsValid() && attr.GetTextBoxAttr().GetHeight().GetValue() > 0)
    {
        int heightPixels = converter.GetPixels(attr.GetTextBoxAttr().GetHeight(), wxVERTICAL);
        if (heightPixels > 0)
            height = heightPixels;

        // Preserve the aspect ratio
        if (height != m_originalImageSize.GetHeight())
            width = (int) (float(m_originalImageSize.GetWidth()) * (float(height)/float(m_originalImageSize.GetHeight())));
    }

    // Limit to max height

    if (attr.GetTextBoxAttr().GetMaxSize().GetHeight().IsValid() && attr.GetTextBoxAttr().GetMaxSize().GetHeight().GetValue() > 0)
    {
        int mh = converter.GetPixels(attr.GetTextBoxAttr().GetMaxSize().GetHeight(), wxVERTICAL);
        if (mh > 0)
            maxHeight = mh;
    }

    if (maxHeight > 0 && height > maxHeight)
    {
        height = maxHeight;

        // Preserve the aspect ratio
        if (height != m_originalImageSize.GetHeight())
            width = (int) (float(m_originalImageSize.GetWidth()) * (float(height)/float(m_originalImageSize.GetHeight())));
    }

    // Prevent the use of zero size
    width = wxMax(1, width);
    height = wxMax(1, height);

    retImageSize = wxSize(width, height);

    bool changed = false;
    return LoadAndScaleImageCache(image, retImageSize, context, changed);
}

// Do the loading and scaling
bool wxRichTextImage::LoadAndScaleImageCache(wxImage& image, const wxSize& sz, wxRichTextDrawingContext& context, bool& changed)
{
    int width = sz.x;
    int height = sz.y;

    if (m_imageCache.IsOk() && m_imageCache.GetLogicalWidth() == width && m_imageCache.GetLogicalHeight() == height)
    {
        // Do nothing, we didn't need to change the image cache
        changed = false;
    }
    else
    {
        changed = true;

        if (context.GetDelayedImageLoading())
        {
            if (m_imageCache.IsOk())
                m_imageCache = wxNullBitmap;
            m_imageState = ImageState_Unloaded;
            return true;
        }

        if (!image.IsOk())
        {
            if (!m_imageBlock.Load(image) || !image.IsOk())
            {
                wxBitmap bitmap(image_placeholder24x24_xpm);
                m_imageCache = bitmap;
                m_originalImageSize = wxSize(bitmap.GetWidth(), bitmap.GetHeight());
                m_imageState = ImageState_Bad;
                return false;
            }
        }

        m_originalImageSize = wxSize(image.GetWidth(), image.GetHeight());

        if (image.GetWidth() == width && image.GetHeight() == height)
            m_imageCache = wxBitmap(image);
        else
        {
            double scaleFactor = 1.0;
            if (context.GetBuffer() && context.GetBuffer()->GetRichTextCtrl())
                scaleFactor = context.GetBuffer()->GetRichTextCtrl()->GetContentScaleFactor();

            // If the original width and height is small, e.g. 400 or below,
            // scale up and then down to improve image quality. This can make
            // a big difference, with not much performance hit.
            int upscaleThreshold = 400;
            wxImage img;
            if (image.GetWidth() <= upscaleThreshold || image.GetHeight() <= upscaleThreshold)
            {
                img = image.Scale(image.GetWidth()*2, image.GetHeight()*2);
                img.Rescale(width*scaleFactor, height*scaleFactor, wxIMAGE_QUALITY_HIGH);
            }
            else
                img = image.Scale(width*scaleFactor, height*scaleFactor, wxIMAGE_QUALITY_HIGH);

            // On Mac, this will create a bitmap that is twice as big as the required dimensions,
            // with a scale factor that indicates that the extra detail should be used on HiDPI displays.
            m_imageCache = wxBitmap(img, wxBITMAP_SCREEN_DEPTH, scaleFactor);
        }
    }

    if (m_imageCache.IsOk())
        m_imageState = ImageState_Loaded;
    else
        m_imageState = ImageState_Bad;

    return m_imageCache.IsOk();
}

/// Draw the item
bool wxRichTextImage::Draw(wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& WXUNUSED(range), const wxRichTextSelection& selection, const wxRect& rect, int WXUNUSED(descent), int WXUNUSED(style))
{
    if (!IsShown())
        return true;

    wxRichTextAttr attr(GetAttributes());
    AdjustAttributes(attr, context);

    wxPoint position = rect.GetPosition();

    if (attr.GetTextBoxAttr().HasVerticalAlignment() &&
        (attr.GetTextBoxAttr().GetVerticalAlignment() > wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_TOP))
    {
        int leftOverSpace = rect.height - GetCachedSize().y;
        if (leftOverSpace > 0)
        {
            if (attr.GetTextBoxAttr().GetVerticalAlignment() == wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_CENTRE)
                position.y += (leftOverSpace/2);
            else if (attr.GetTextBoxAttr().GetVerticalAlignment() == wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_BOTTOM)
                position.y += leftOverSpace;
        }
    }

    DrawBoxAttributes(dc, GetBuffer(), attr, wxRect(position, GetCachedSize()));

    wxSize imageSize(m_imageCache.GetWidth(), m_imageCache.GetHeight());
    wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
    marginRect = wxRect(position, GetCachedSize()); // outer rectangle, will calculate contentRect
    GetBoxRects(dc, GetBuffer(), attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);

    if (m_imageCache.IsOk())
        dc.DrawBitmap(m_imageCache, contentRect.x, contentRect.y, true);
    else
    {
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(contentRect);
    }

    if (selection.WithinSelection(GetRange().GetStart(), this))
    {
        wxCheckSetBrush(dc, *wxBLACK_BRUSH);
        wxCheckSetPen(dc, *wxBLACK_PEN);

#if defined(__WXMAC__) && wxOSX_USE_COCOA
        dc.SetLogicalFunction(wxXOR);
#else
        dc.SetLogicalFunction(wxINVERT);
#endif

#ifdef __WXMAC__
        if (m_imageCache.IsOk())
            dc.DrawBitmap(m_imageCache, contentRect.x, contentRect.y, true);
#endif
        dc.DrawRectangle(contentRect);
        dc.SetLogicalFunction(wxCOPY);
    }

    return true;
}

/// Lay the item out
bool wxRichTextImage::Layout(wxDC& dc, wxRichTextDrawingContext& context, const wxRect& rect, const wxRect& parentRect, int WXUNUSED(style))
{
    wxSize imageSize;
    if (!LoadImageCache(dc, context, imageSize, false, parentRect.GetSize()))
        return false;

    wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
    contentRect = wxRect(wxPoint(0,0), imageSize);

    wxRichTextAttr attr(GetAttributes());
    AdjustAttributes(attr, context);

    GetBoxRects(dc, GetBuffer(), attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);

    wxSize overallSize = marginRect.GetSize();

    SetCachedSize(overallSize);
    SetMaxSize(overallSize);
    SetMinSize(overallSize);
    SetPosition(rect.GetPosition());

    return true;
}

/// Get/set the object size for the given range. Returns false if the range
/// is invalid for this object.
bool wxRichTextImage::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& WXUNUSED(descent), wxDC& dc, wxRichTextDrawingContext& context, int WXUNUSED(flags), const wxPoint& WXUNUSED(position), const wxSize& parentSize, wxArrayInt* partialExtents) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    wxSize imageSize;
    if (!const_cast<wxRichTextImage*>(this)->LoadImageCache(dc, context, imageSize, false, parentSize))
    {
        size.x = 0; size.y = 0;
        if (partialExtents)
            partialExtents->Add(0);
        return true;
    }

    wxRichTextAttr attr(GetAttributes());
    const_cast<wxRichTextImage*>(this)->AdjustAttributes(attr, context);

    wxRect marginRect, borderRect, contentRect, paddingRect, outlineRect;
    contentRect = wxRect(wxPoint(0,0), imageSize);
    GetBoxRects(dc, GetBuffer(), attr, marginRect, borderRect, contentRect, paddingRect, outlineRect);

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
    m_originalImageSize = obj.m_originalImageSize;
}

/// Edit properties via a GUI
bool wxRichTextImage::EditProperties(wxWindow* parent, wxRichTextBuffer* buffer)
{
    wxRichTextObjectPropertiesDialog imageDlg(this, wxGetTopLevelParent(parent), wxID_ANY, _("Picture Properties"));
    imageDlg.SetAttributes(GetAttributes());

    if (imageDlg.ShowModal() == wxID_OK && buffer->GetRichTextCtrl()->IsEditable())
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

wxIMPLEMENT_CLASS(wxRichTextFileHandler, wxObject);

#if wxUSE_FFILE && wxUSE_STREAMS
bool wxRichTextFileHandler::LoadFile(wxRichTextBuffer *buffer, const wxString& filename)
{
    wxFFileInputStream stream(filename);
    if (stream.IsOk())
        return LoadFile(buffer, stream);

    return false;
}

bool wxRichTextFileHandler::SaveFile(wxRichTextBuffer *buffer, const wxString& filename)
{
    wxFFileOutputStream stream(filename);
    if (stream.IsOk())
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

wxIMPLEMENT_CLASS(wxRichTextPlainTextHandler, wxRichTextFileHandler);

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
    m_data = nullptr;
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

    return (m_data != nullptr);
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

    return (m_data != nullptr);
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
        return nullptr;

    stream.Read(block, size);

    return block;
}

unsigned char* wxRichTextImageBlock::ReadBlock(const wxString& filename, size_t size)
{
    wxFileInputStream stream(filename);
    if (!stream.IsOk())
        return nullptr;

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
    if (!outStream.IsOk())
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

const wxChar *wxRichTextBufferDataObject::ms_richTextBufferFormatId = wxT("wxRichText");

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
    m_richTextBuffer = nullptr;

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
        m_richTextBuffer->SetHandlerFlags(wxRICHTEXT_HANDLER_INCLUDE_STYLESHEET);
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
        m_richTextBuffer->SetHandlerFlags(wxRICHTEXT_HANDLER_INCLUDE_STYLESHEET);
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
    m_richTextBuffer->SetHandlerFlags(wxRICHTEXT_HANDLER_INCLUDE_STYLESHEET);
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

    wxFont FindFont(const wxRichTextAttr& fontSpec, double fontScale);

    wxRichTextFontTableHashMap  m_hashMap;
};

wxFont wxRichTextFontTableData::FindFont(const wxRichTextAttr& fontSpec, double fontScale)
{
    wxString facename(fontSpec.GetFontFaceName());

    int fontSize = fontSpec.GetFontSize();
    if (fontScale != 1.0)
        fontSize = (int) ((double(fontSize) * fontScale) + 0.5);

    wxString units;
    if (fontSpec.HasFontPixelSize() && !fontSpec.HasFontPointSize())
        units = wxT("px");
    else
        units = wxT("pt");
    wxString spec = wxString::Format(wxT("%d-%s-%d-%d-%d-%d-%s-%d"),
        fontSize, units.c_str(), fontSpec.GetFontStyle(), fontSpec.GetFontWeight(), (int) fontSpec.GetFontUnderlined(), (int) fontSpec.GetFontStrikethrough(),
        facename.c_str(), (int) fontSpec.GetFontEncoding());

    wxRichTextFontTableHashMap::iterator entry = m_hashMap.find(spec);
    if ( entry == m_hashMap.end() )
    {
        if (fontSpec.HasFontPixelSize() && !fontSpec.HasFontPointSize())
        {
            wxFont font(wxSize(0, fontSize), wxFONTFAMILY_DEFAULT, fontSpec.GetFontStyle(), fontSpec.GetFontWeight(), fontSpec.GetFontUnderlined(), facename);
            if (fontSpec.HasFontStrikethrough() && fontSpec.GetFontStrikethrough())
                font.SetStrikethrough(true);
            m_hashMap[spec] = font;
            return font;
        }
        else
        {
            wxFont font(fontSize, wxFONTFAMILY_DEFAULT, fontSpec.GetFontStyle(), fontSpec.GetFontWeight(), fontSpec.GetFontUnderlined(), facename.c_str());
            if (fontSpec.HasFontStrikethrough() && fontSpec.GetFontStrikethrough())
                font.SetStrikethrough(true);

            m_hashMap[spec] = font;
            return font;
        }
    }
    else
    {
        return entry->second;
    }
}

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextFontTable, wxObject);

wxRichTextFontTable::wxRichTextFontTable()
{
    m_refData = new wxRichTextFontTableData;
    m_fontScale = 1.0;
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
    m_fontScale = table.m_fontScale;
}

wxFont wxRichTextFontTable::FindFont(const wxRichTextAttr& fontSpec)
{
    wxRichTextFontTableData* data = (wxRichTextFontTableData*) m_refData;
    if (data)
        return data->FindFont(fontSpec, m_fontScale);
    else
        return wxFont();
}

void wxRichTextFontTable::Clear()
{
    wxRichTextFontTableData* data = (wxRichTextFontTableData*) m_refData;
    if (data)
        data->m_hashMap.clear();
}

void wxRichTextFontTable::SetFontScale(double fontScale)
{
    if (fontScale != m_fontScale)
        Clear();
    m_fontScale = fontScale;
}

// wxTextBoxAttr

void wxTextBoxAttr::Reset()
{
    m_flags = 0;
    m_floatMode = wxTEXT_BOX_ATTR_FLOAT_NONE;
    m_clearMode = wxTEXT_BOX_ATTR_CLEAR_NONE;
    m_whitespaceMode = wxTEXT_BOX_ATTR_WHITESPACE_NONE;
    m_collapseMode = wxTEXT_BOX_ATTR_COLLAPSE_NONE;
    m_verticalAlignment = wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_NONE;
    m_boxStyleName.clear();

    m_margins.Reset();
    m_padding.Reset();
    m_position.Reset();

    m_size.Reset();
    m_minSize.Reset();
    m_maxSize.Reset();

    m_border.Reset();
    m_outline.Reset();
    m_cornerRadius.Reset();
    m_shadow.Reset();
}

// Equality test
bool wxTextBoxAttr::operator== (const wxTextBoxAttr& attr) const
{
    return (
        m_flags == attr.m_flags &&
        m_floatMode == attr.m_floatMode &&
        m_clearMode == attr.m_clearMode &&
        m_whitespaceMode == attr.m_whitespaceMode &&
        m_collapseMode == attr.m_collapseMode &&
        m_verticalAlignment == attr.m_verticalAlignment &&
        m_cornerRadius == attr.m_cornerRadius &&

        m_margins == attr.m_margins &&
        m_padding == attr.m_padding &&
        m_position == attr.m_position &&

        m_size == attr.m_size &&
        m_minSize == attr.m_minSize &&
        m_maxSize == attr.m_maxSize &&

        m_border == attr.m_border &&
        m_outline == attr.m_outline &&

        m_boxStyleName == attr.m_boxStyleName &&
        m_shadow == attr.m_shadow
        );
}

// Partial equality test
bool wxTextBoxAttr::EqPartial(const wxTextBoxAttr& attr, bool weakTest) const
{
    if (!weakTest &&
            ((!HasFloatMode() && attr.HasFloatMode()) ||
             (!HasClearMode() && attr.HasClearMode()) ||
             (!HasCollapseBorders() && attr.HasCollapseBorders()) ||
             (!HasVerticalAlignment() && attr.HasVerticalAlignment()) ||
             (!HasWhitespaceMode() && attr.HasWhitespaceMode()) ||
             (!HasCornerRadius() && attr.HasCornerRadius()) ||
             (!m_shadow.IsValid() && attr.m_shadow.IsValid()) ||
             (!HasBoxStyleName() && attr.HasBoxStyleName())))
    {
        return false;
    }
    if (attr.HasFloatMode() && HasFloatMode() && (GetFloatMode() != attr.GetFloatMode()))
        return false;

    if (attr.HasClearMode() && HasClearMode() && (GetClearMode() != attr.GetClearMode()))
        return false;

    if (attr.HasCollapseBorders() && HasCollapseBorders() && (attr.GetCollapseBorders() != GetCollapseBorders()))
        return false;

    if (attr.HasVerticalAlignment() && HasVerticalAlignment() && (attr.GetVerticalAlignment() != GetVerticalAlignment()))
        return false;

    if (attr.HasWhitespaceMode() && HasWhitespaceMode() && (GetWhitespaceMode() != attr.GetWhitespaceMode()))
        return false;

    if (attr.HasCornerRadius() && HasCornerRadius() && !(attr.GetCornerRadius() == GetCornerRadius()))
        return false;

    if (attr.HasBoxStyleName() && HasBoxStyleName() && (attr.GetBoxStyleName() != GetBoxStyleName()))
        return false;

    // Position

    if (!m_position.EqPartial(attr.m_position, weakTest))
        return false;

    // Size

    if (!m_size.EqPartial(attr.m_size, weakTest))
        return false;
    if (!m_minSize.EqPartial(attr.m_minSize, weakTest))
        return false;
    if (!m_maxSize.EqPartial(attr.m_maxSize, weakTest))
        return false;

    // Margins

    if (!m_margins.EqPartial(attr.m_margins, weakTest))
        return false;

    // Padding

    if (!m_padding.EqPartial(attr.m_padding, weakTest))
        return false;

    // Border

    if (!GetBorder().EqPartial(attr.GetBorder(), weakTest))
        return false;

    // Outline

    if (!GetOutline().EqPartial(attr.GetOutline(), weakTest))
        return false;

    // Shadow

    if (!GetShadow().EqPartial(attr.GetShadow(), weakTest))
        return false;

    return true;
}

// Merges the given attributes. If compareWith
// is non-null, then it will be used to mask out those attributes that are the same in style
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

    if (attr.HasWhitespaceMode())
    {
        if (!(compareWith && compareWith->HasWhitespaceMode() && compareWith->GetWhitespaceMode() == attr.GetWhitespaceMode()))
            SetWhitespaceMode(attr.GetWhitespaceMode());
    }

    if (attr.HasCornerRadius())
    {
        if (!(compareWith && compareWith->HasCornerRadius() && compareWith->GetCornerRadius() == attr.GetCornerRadius()))
            SetCornerRadius(attr.GetCornerRadius());
    }
    if (attr.HasBoxStyleName())
    {
        if (!(compareWith && compareWith->HasBoxStyleName() && compareWith->GetBoxStyleName() == attr.GetBoxStyleName()))
            SetBoxStyleName(attr.GetBoxStyleName());
    }

    m_margins.Apply(attr.m_margins, compareWith ? (& compareWith->m_margins) : (const wxTextAttrDimensions*) nullptr);
    m_padding.Apply(attr.m_padding, compareWith ? (& compareWith->m_padding) : (const wxTextAttrDimensions*) nullptr);
    m_position.Apply(attr.m_position, compareWith ? (& compareWith->m_position) : (const wxTextAttrDimensions*) nullptr);

    m_size.Apply(attr.m_size, compareWith ? (& compareWith->m_size) : (const wxTextAttrSize*) nullptr);
    m_minSize.Apply(attr.m_minSize, compareWith ? (& compareWith->m_minSize) : (const wxTextAttrSize*) nullptr);
    m_maxSize.Apply(attr.m_maxSize, compareWith ? (& compareWith->m_maxSize) : (const wxTextAttrSize*) nullptr);

    m_border.Apply(attr.m_border, compareWith ? (& compareWith->m_border) : (const wxTextAttrBorders*) nullptr);
    m_outline.Apply(attr.m_outline, compareWith ? (& compareWith->m_outline) : (const wxTextAttrBorders*) nullptr);

    m_shadow.Apply(attr.m_shadow, compareWith ? (& compareWith->m_shadow) : (const wxTextAttrShadow*) nullptr);

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

    if (attr.HasWhitespaceMode())
        RemoveFlag(wxTEXT_BOX_ATTR_WHITESPACE);

    if (attr.HasCornerRadius())
        RemoveFlag(wxTEXT_BOX_ATTR_CORNER_RADIUS);

    if (attr.HasBoxStyleName())
    {
        SetBoxStyleName(wxEmptyString);
        RemoveFlag(wxTEXT_BOX_ATTR_BOX_STYLE_NAME);
    }

    m_margins.RemoveStyle(attr.m_margins);
    m_padding.RemoveStyle(attr.m_padding);
    m_position.RemoveStyle(attr.m_position);

    m_size.RemoveStyle(attr.m_size);
    m_minSize.RemoveStyle(attr.m_minSize);
    m_maxSize.RemoveStyle(attr.m_maxSize);

    m_border.RemoveStyle(attr.m_border);
    m_outline.RemoveStyle(attr.m_outline);

    m_shadow.RemoveStyle(attr.m_shadow);

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
    {
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_FLOAT);
        RemoveFlag(wxTEXT_BOX_ATTR_FLOAT);
    }

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
    {
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_CLEAR);
        RemoveFlag(wxTEXT_BOX_ATTR_CLEAR);
    }

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
    {
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_COLLAPSE_BORDERS);
        RemoveFlag(wxTEXT_BOX_ATTR_COLLAPSE_BORDERS);
    }

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
    {
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT);
        RemoveFlag(wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT);
    }

    if (attr.HasWhitespaceMode())
    {
        if (!clashingAttr.HasWhitespaceMode() && !absentAttr.HasWhitespaceMode())
        {
            if (HasWhitespaceMode())
            {
                if (GetWhitespaceMode() != attr.GetWhitespaceMode())
                {
                    clashingAttr.AddFlag(wxTEXT_BOX_ATTR_WHITESPACE);
                    RemoveFlag(wxTEXT_BOX_ATTR_WHITESPACE);
                }
            }
            else
                SetWhitespaceMode(attr.GetWhitespaceMode());
        }
    }
    else
    {
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_WHITESPACE);
        RemoveFlag(wxTEXT_BOX_ATTR_WHITESPACE);
    }

    if (attr.HasCornerRadius())
    {
        if (!clashingAttr.HasCornerRadius() && !absentAttr.HasCornerRadius())
        {
            if (HasCornerRadius())
            {
                if (!(GetCornerRadius() == attr.GetCornerRadius()))
                {
                    clashingAttr.AddFlag(wxTEXT_BOX_ATTR_CORNER_RADIUS);
                    GetCornerRadius().Reset();
                    RemoveFlag(wxTEXT_BOX_ATTR_CORNER_RADIUS);
                }
            }
            else
                SetCornerRadius(attr.GetCornerRadius());
        }
    }
    else
    {
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_CORNER_RADIUS);
        RemoveFlag(wxTEXT_BOX_ATTR_CORNER_RADIUS);
    }

    if (attr.HasBoxStyleName())
    {
        if (!clashingAttr.HasBoxStyleName() && !absentAttr.HasBoxStyleName())
        {
            if (HasBoxStyleName())
            {
                if (GetBoxStyleName() != attr.GetBoxStyleName())
                {
                    clashingAttr.AddFlag(wxTEXT_BOX_ATTR_BOX_STYLE_NAME);
                    RemoveFlag(wxTEXT_BOX_ATTR_BOX_STYLE_NAME);
                }
            }
            else
                SetBoxStyleName(attr.GetBoxStyleName());
        }
    }
    else
    {
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_BOX_STYLE_NAME);
        RemoveFlag(wxTEXT_BOX_ATTR_BOX_STYLE_NAME);
    }

    m_margins.CollectCommonAttributes(attr.m_margins, clashingAttr.m_margins, absentAttr.m_margins);
    m_padding.CollectCommonAttributes(attr.m_padding, clashingAttr.m_padding, absentAttr.m_padding);
    m_position.CollectCommonAttributes(attr.m_position, clashingAttr.m_position, absentAttr.m_position);

    m_size.CollectCommonAttributes(attr.m_size, clashingAttr.m_size, absentAttr.m_size);
    m_minSize.CollectCommonAttributes(attr.m_minSize, clashingAttr.m_minSize, absentAttr.m_minSize);
    m_maxSize.CollectCommonAttributes(attr.m_maxSize, clashingAttr.m_maxSize, absentAttr.m_maxSize);

    m_border.CollectCommonAttributes(attr.m_border, clashingAttr.m_border, absentAttr.m_border);
    m_outline.CollectCommonAttributes(attr.m_outline, clashingAttr.m_outline, absentAttr.m_outline);

    m_shadow.CollectCommonAttributes(attr.m_shadow, clashingAttr.m_shadow, absentAttr.m_shadow);
}

bool wxTextBoxAttr::IsDefault() const
{
    return GetFlags() == 0 && m_border.IsDefault() && m_outline.IsDefault() &&
        !m_size.GetWidth().IsValid() && !m_size.GetHeight().IsValid() &&
        !m_minSize.GetWidth().IsValid() && !m_minSize.GetHeight().IsValid() &&
        !m_maxSize.GetWidth().IsValid() && !m_maxSize.GetHeight().IsValid() &&
        !m_position.GetLeft().IsValid() && !m_position.GetRight().IsValid() && !m_position.GetTop().IsValid() && !m_position.GetBottom().IsValid() &&
        !m_padding.GetLeft().IsValid() && !m_padding.GetRight().IsValid() && !m_padding.GetTop().IsValid() && !m_padding.GetBottom().IsValid() &&
        !m_margins.GetLeft().IsValid() && !m_margins.GetRight().IsValid() && !m_margins.GetTop().IsValid() && !m_margins.GetBottom().IsValid() &&
        m_shadow.IsDefault();
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

// Partial equality test
bool wxRichTextAttr::EqPartial(const wxRichTextAttr& attr, bool weakTest) const
{
    if (!(wxTextAttr::EqPartial(attr, weakTest)))
        return false;

    return m_textBoxAttr.EqPartial(attr.m_textBoxAttr, weakTest);
}

// Merges the given attributes. If compareWith
// is non-null, then it will be used to mask out those attributes that are the same in style
// and compareWith, for situations where we don't want to explicitly set inherited attributes.
bool wxRichTextAttr::Apply(const wxRichTextAttr& style, const wxRichTextAttr* compareWith)
{
    wxTextAttr::Apply(style, compareWith);

    return m_textBoxAttr.Apply(style.m_textBoxAttr, compareWith ? (& compareWith->m_textBoxAttr) : (const wxTextBoxAttr*) nullptr);
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
bool wxTextAttrBorder::EqPartial(const wxTextAttrBorder& border, bool weakTest) const
{
    if (!weakTest &&
        ((!HasStyle() && border.HasStyle()) ||
         (!HasColour() && border.HasColour()) ||
         (!HasWidth() && border.HasWidth())))
    {
        return false;
    }

    if (border.HasStyle() && HasStyle() && (border.GetStyle() != GetStyle()))
        return false;

    if (border.HasColour() && HasColour() && (border.GetColourLong() != GetColourLong()))
        return false;

    if (border.HasWidth() && HasWidth() && !(border.GetWidth() == GetWidth()))
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
    {
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_BORDER_STYLE);
        RemoveFlag(wxTEXT_BOX_ATTR_BORDER_STYLE);
    }

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
    {
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_BORDER_COLOUR);
        RemoveFlag(wxTEXT_BOX_ATTR_BORDER_COLOUR);
    }

    m_borderWidth.CollectCommonAttributes(attr.m_borderWidth, clashingAttr.m_borderWidth, absentAttr.m_borderWidth);
}

// Partial equality test
bool wxTextAttrBorders::EqPartial(const wxTextAttrBorders& borders, bool weakTest) const
{
    return m_left.EqPartial(borders.m_left, weakTest) && m_right.EqPartial(borders.m_right, weakTest) &&
            m_top.EqPartial(borders.m_top, weakTest) && m_bottom.EqPartial(borders.m_bottom, weakTest);
}

// Apply border to 'this', but not if the same as compareWith
bool wxTextAttrBorders::Apply(const wxTextAttrBorders& borders, const wxTextAttrBorders* compareWith)
{
    m_left.Apply(borders.m_left, compareWith ? (& compareWith->m_left) : (const wxTextAttrBorder*) nullptr);
    m_right.Apply(borders.m_right, compareWith ? (& compareWith->m_right) : (const wxTextAttrBorder*) nullptr);
    m_top.Apply(borders.m_top, compareWith ? (& compareWith->m_top) : (const wxTextAttrBorder*) nullptr);
    m_bottom.Apply(borders.m_bottom, compareWith ? (& compareWith->m_bottom) : (const wxTextAttrBorder*) nullptr);
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
bool wxTextAttrDimension::EqPartial(const wxTextAttrDimension& dim, bool weakTest) const
{
    if (!weakTest && !IsValid() && dim.IsValid())
        return false;

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
    {
        absentAttr.SetValid(true);
        SetValue(0, 0);
        SetValid(false);
    }
}

wxTextAttrDimensionConverter::wxTextAttrDimensionConverter(wxDC& dc, double scale, const wxSize& parentSize)
    : m_parentSize(parentSize)
{
    m_ppi = dc.GetPPI().x; m_scale = scale;
}

wxTextAttrDimensionConverter::wxTextAttrDimensionConverter(int ppi, double scale, const wxSize& parentSize)
    : m_parentSize(parentSize)
{
    m_ppi = ppi; m_scale = scale;
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
        return ConvertTenthsMMToPixels(dim.GetValue()); // Incorporates scaling
    else
    {
        double pixelsDouble = 0.0;
        if (dim.GetUnits() == wxTEXT_ATTR_UNITS_PIXELS)
            pixelsDouble = (double) dim.GetValue();
        else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_POINTS)
            pixelsDouble = (double(dim.GetValue()) * (double(m_ppi)/72.0));
        else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_HUNDREDTHS_POINT)
            pixelsDouble = ((double(dim.GetValue())/100.0) * (double(m_ppi)/72.0));
        else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_PERCENTAGE)
        {
            wxASSERT(m_parentSize != wxDefaultSize);
            if (direction == wxHORIZONTAL)
                pixelsDouble = (double(m_parentSize.x) * double(dim.GetValue()) / 100.0);
            else
                pixelsDouble = (double(m_parentSize.y) * double(dim.GetValue()) / 100.0);
        }
        else
        {
            wxASSERT(false);
            return 0;
        }

        // Scaling is used in e.g. printing
        if (m_scale != 1.0 && dim.GetUnits() != wxTEXT_ATTR_UNITS_PIXELS && dim.GetUnits() != wxTEXT_ATTR_UNITS_PERCENTAGE)
            pixelsDouble /= m_scale;

        int pixelsInt = int(pixelsDouble + 0.5);

        // If the result is very small, make it at least one pixel in size.
        if (pixelsInt == 0 && dim.GetValue() > 0)
            pixelsInt = 1;

        return pixelsInt;
    }
}

int wxTextAttrDimensionConverter::GetTenthsMM(const wxTextAttrDimension& dim) const
{
    if (dim.GetUnits() == wxTEXT_ATTR_UNITS_TENTHS_MM)
        return dim.GetValue();
    else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_PIXELS)
        return ConvertPixelsToTenthsMM(dim.GetValue());
    else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_POINTS)
        return (int) ((double(dim.GetValue())/0.28346456692913384) + 0.5);
    else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_HUNDREDTHS_POINT)
        return (int) ((double(dim.GetValue())/28.346456692913384) + 0.5);
    else
    {
        wxASSERT(false);
        return 0;
    }
}

// Partial equality test
bool wxTextAttrDimensions::EqPartial(const wxTextAttrDimensions& dims, bool weakTest) const
{
    if (!m_left.EqPartial(dims.m_left, weakTest))
        return false;

    if (!m_right.EqPartial(dims.m_right, weakTest))
        return false;

    if (!m_top.EqPartial(dims.m_top, weakTest))
        return false;

    if (!m_bottom.EqPartial(dims.m_bottom, weakTest))
        return false;

    return true;
}

// Apply border to 'this', but not if the same as compareWith
bool wxTextAttrDimensions::Apply(const wxTextAttrDimensions& dims, const wxTextAttrDimensions* compareWith)
{
    m_left.Apply(dims.m_left, compareWith ? (& compareWith->m_left) : (const wxTextAttrDimension*) nullptr);
    m_right.Apply(dims.m_right, compareWith ? (& compareWith->m_right): (const wxTextAttrDimension*) nullptr);
    m_top.Apply(dims.m_top, compareWith ? (& compareWith->m_top): (const wxTextAttrDimension*) nullptr);
    m_bottom.Apply(dims.m_bottom, compareWith ? (& compareWith->m_bottom): (const wxTextAttrDimension*) nullptr);

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
bool wxTextAttrSize::EqPartial(const wxTextAttrSize& size, bool weakTest) const
{
    if (!m_width.EqPartial(size.m_width, weakTest))
        return false;

    if (!m_height.EqPartial(size.m_height, weakTest))
        return false;

    return true;
}

// Apply border to 'this', but not if the same as compareWith
bool wxTextAttrSize::Apply(const wxTextAttrSize& size, const wxTextAttrSize* compareWith)
{
    m_width.Apply(size.m_width, compareWith ? (& compareWith->m_width) : (const wxTextAttrDimension*) nullptr);
    m_height.Apply(size.m_height, compareWith ? (& compareWith->m_height): (const wxTextAttrDimension*) nullptr);

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

    // Remove flags for attributes that are absent
    currentStyle.SetFlags(currentStyle.GetFlags() & ~absentAttr.GetFlags());
    currentStyle.SetTextEffectFlags(currentStyle.GetTextEffectFlags() & ~absentAttr.GetTextEffectFlags());

    long forbiddenFlags = clashingAttr.GetFlags()|absentAttr.GetFlags();

    // If different font size units are being used, this is a clash.
    if (((attr.GetFlags() & wxTEXT_ATTR_FONT_SIZE) | (currentStyle.GetFlags() & wxTEXT_ATTR_FONT_SIZE)) == wxTEXT_ATTR_FONT_SIZE)
    {
        currentStyle.SetFontSize(0);
        currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_SIZE);
        clashingAttr.AddFlag(wxTEXT_ATTR_FONT_SIZE);
    }
    else
    {
        if (attr.HasFontPointSize() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_FONT_POINT_SIZE))
        {
            if (currentStyle.HasFontPointSize())
            {
                if (currentStyle.GetFontSize() != attr.GetFontSize())
                {
                    // Clash of attr - mark as such
                    clashingAttr.AddFlag(wxTEXT_ATTR_FONT_POINT_SIZE);
                    currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_POINT_SIZE);
                }
            }
            else
                currentStyle.SetFontSize(attr.GetFontSize());
        }
        else if (!attr.HasFontPointSize() && currentStyle.HasFontPointSize())
        {
            clashingAttr.AddFlag(wxTEXT_ATTR_FONT_POINT_SIZE);
            currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_POINT_SIZE);
        }

        if (attr.HasFontPixelSize() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_FONT_PIXEL_SIZE))
        {
            if (currentStyle.HasFontPixelSize())
            {
                if (currentStyle.GetFontSize() != attr.GetFontSize())
                {
                    // Clash of attr - mark as such
                    clashingAttr.AddFlag(wxTEXT_ATTR_FONT_PIXEL_SIZE);
                    currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_PIXEL_SIZE);
                }
            }
            else
                currentStyle.SetFontPixelSize(attr.GetFontSize());
        }
        else if (!attr.HasFontPixelSize() && currentStyle.HasFontPixelSize())
        {
            clashingAttr.AddFlag(wxTEXT_ATTR_FONT_PIXEL_SIZE);
            currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_PIXEL_SIZE);
        }
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
    else if (!attr.HasFontItalic() && currentStyle.HasFontItalic())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_FONT_ITALIC);
        currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_ITALIC);
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
    else if (!attr.HasFontFamily() && currentStyle.HasFontFamily())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_FONT_FAMILY);
        currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_FAMILY);
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
    else if (!attr.HasFontWeight() && currentStyle.HasFontWeight())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_FONT_WEIGHT);
        currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_WEIGHT);
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
    else if (!attr.HasFontFaceName() && currentStyle.HasFontFaceName())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_FONT_FACE);
        currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_FACE);
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
    else if (!attr.HasFontUnderlined() && currentStyle.HasFontUnderlined())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_FONT_UNDERLINE);
        currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_UNDERLINE);
    }

    if (attr.HasFontStrikethrough() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_FONT_STRIKETHROUGH))
    {
        if (currentStyle.HasFontStrikethrough())
        {
            if (currentStyle.GetFontStrikethrough() != attr.GetFontStrikethrough())
            {
                // Clash of attr - mark as such
                clashingAttr.AddFlag(wxTEXT_ATTR_FONT_STRIKETHROUGH);
                currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_STRIKETHROUGH);
            }
        }
        else
            currentStyle.SetFontStrikethrough(attr.GetFontStrikethrough());
    }
    else if (!attr.HasFontStrikethrough() && currentStyle.HasFontStrikethrough())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_FONT_STRIKETHROUGH);
        currentStyle.RemoveFlag(wxTEXT_ATTR_FONT_STRIKETHROUGH);
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
    else if (!attr.HasTextColour() && currentStyle.HasTextColour())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_TEXT_COLOUR);
        currentStyle.RemoveFlag(wxTEXT_ATTR_TEXT_COLOUR);
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
    else if (!attr.HasBackgroundColour() && currentStyle.HasBackgroundColour())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_BACKGROUND_COLOUR);
        currentStyle.RemoveFlag(wxTEXT_ATTR_BACKGROUND_COLOUR);
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
    else if (!attr.HasAlignment() && currentStyle.HasAlignment())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_ALIGNMENT);
        currentStyle.RemoveFlag(wxTEXT_ATTR_ALIGNMENT);
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
    else if (!attr.HasTabs() && currentStyle.HasTabs())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_TABS);
        currentStyle.RemoveFlag(wxTEXT_ATTR_TABS);
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
    else if (!attr.HasLeftIndent() && currentStyle.HasLeftIndent())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_LEFT_INDENT);
        currentStyle.RemoveFlag(wxTEXT_ATTR_LEFT_INDENT);
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
    else if (!attr.HasRightIndent() && currentStyle.HasRightIndent())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_RIGHT_INDENT);
        currentStyle.RemoveFlag(wxTEXT_ATTR_RIGHT_INDENT);
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
    else if (!attr.HasParagraphSpacingAfter() && currentStyle.HasParagraphSpacingAfter())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_PARA_SPACING_AFTER);
        currentStyle.RemoveFlag(wxTEXT_ATTR_PARA_SPACING_AFTER);
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
    else if (!attr.HasParagraphSpacingBefore() && currentStyle.HasParagraphSpacingBefore())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_PARA_SPACING_BEFORE);
        currentStyle.RemoveFlag(wxTEXT_ATTR_PARA_SPACING_BEFORE);
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
    else if (!attr.HasLineSpacing() && currentStyle.HasLineSpacing())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_LINE_SPACING);
        currentStyle.RemoveFlag(wxTEXT_ATTR_LINE_SPACING);
    }

    if (attr.HasPageBreak() && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_PAGE_BREAK))
    {
        currentStyle.SetPageBreak(true);
    }
    else if (!attr.HasPageBreak() && currentStyle.HasPageBreak())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_PAGE_BREAK);
        currentStyle.RemoveFlag(wxTEXT_ATTR_PAGE_BREAK);
    }

    if ((attr.GetFlags() & wxTEXT_ATTR_AVOID_PAGE_BREAK_BEFORE) && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_AVOID_PAGE_BREAK_BEFORE))
    {
        currentStyle.AddFlag(wxTEXT_ATTR_AVOID_PAGE_BREAK_BEFORE);
    }
    else if (!(attr.GetFlags() & wxTEXT_ATTR_AVOID_PAGE_BREAK_BEFORE) && (currentStyle.GetFlags() & wxTEXT_ATTR_AVOID_PAGE_BREAK_BEFORE))
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_AVOID_PAGE_BREAK_BEFORE);
        currentStyle.RemoveFlag(wxTEXT_ATTR_AVOID_PAGE_BREAK_BEFORE);
    }

    if ((attr.GetFlags() & wxTEXT_ATTR_AVOID_PAGE_BREAK_AFTER) && !wxHasStyle(forbiddenFlags, wxTEXT_ATTR_AVOID_PAGE_BREAK_AFTER))
    {
        currentStyle.AddFlag(wxTEXT_ATTR_AVOID_PAGE_BREAK_AFTER);
    }
    else if (!(attr.GetFlags() & wxTEXT_ATTR_AVOID_PAGE_BREAK_AFTER) && (currentStyle.GetFlags() & wxTEXT_ATTR_AVOID_PAGE_BREAK_AFTER))
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_AVOID_PAGE_BREAK_AFTER);
        currentStyle.RemoveFlag(wxTEXT_ATTR_AVOID_PAGE_BREAK_AFTER);
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
    else if (!attr.HasCharacterStyleName() && currentStyle.HasCharacterStyleName())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_CHARACTER_STYLE_NAME);
        currentStyle.RemoveFlag(wxTEXT_ATTR_CHARACTER_STYLE_NAME);
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
    else if (!attr.HasParagraphStyleName() && currentStyle.HasParagraphStyleName())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_PARAGRAPH_STYLE_NAME);
        currentStyle.RemoveFlag(wxTEXT_ATTR_PARAGRAPH_STYLE_NAME);
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
    else if (!attr.HasListStyleName() && currentStyle.HasListStyleName())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_LIST_STYLE_NAME);
        currentStyle.RemoveFlag(wxTEXT_ATTR_LIST_STYLE_NAME);
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
    else if (!attr.HasBulletStyle() && currentStyle.HasBulletStyle())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_BULLET_STYLE);
        currentStyle.RemoveFlag(wxTEXT_ATTR_BULLET_STYLE);
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
    else if (!attr.HasBulletNumber() && currentStyle.HasBulletNumber())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_BULLET_NUMBER);
        currentStyle.RemoveFlag(wxTEXT_ATTR_BULLET_NUMBER);
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
    else if (!attr.HasBulletText() && currentStyle.HasBulletText())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_BULLET_TEXT);
        currentStyle.RemoveFlag(wxTEXT_ATTR_BULLET_TEXT);
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
    else if (!attr.HasBulletName() && currentStyle.HasBulletName())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_BULLET_NAME);
        currentStyle.RemoveFlag(wxTEXT_ATTR_BULLET_NAME);
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
    else if (!attr.HasURL() && currentStyle.HasURL())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_URL);
        currentStyle.RemoveFlag(wxTEXT_ATTR_URL);
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
    else if (!attr.HasTextEffects() && currentStyle.HasTextEffects())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_EFFECTS);
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
    else if (!attr.HasOutlineLevel() && currentStyle.HasOutlineLevel())
    {
        clashingAttr.AddFlag(wxTEXT_ATTR_OUTLINE_LEVEL);
        currentStyle.RemoveFlag(wxTEXT_ATTR_OUTLINE_LEVEL);
    }
}

WX_DEFINE_OBJARRAY(wxRichTextVariantArray);
WX_DEFINE_OBJARRAY(wxRichTextAttrArray);
WX_DEFINE_OBJARRAY(wxRichTextRectArray);

wxIMPLEMENT_DYNAMIC_CLASS(wxRichTextProperties, wxObject);

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

bool wxRichTextProperties::Remove(const wxString& name)
{
    int idx = Find(name);
    if (idx != -1)
    {
        m_properties.RemoveAt(idx);
        return true;
    }
    else
        return false;
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
        return nullptr;
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

void wxRichTextProperties::RemoveProperties(const wxRichTextProperties& properties)
{
    size_t i;
    for (i = 0; i < properties.GetCount(); i++)
    {
        wxString name = properties.GetProperties()[i].GetName();
        if (HasProperty(name))
            Remove(name);
    }
}

void wxRichTextProperties::MergeProperties(const wxRichTextProperties& properties)
{
    size_t i;
    for (i = 0; i < properties.GetCount(); i++)
    {
        SetProperty(properties.GetProperties()[i]);
    }
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
            return nullptr;

        wxRichTextObject* p1 = p->GetChild(pos);
        if (i == (m_address.GetCount()-1))
            return p1;

        p = wxDynamicCast(p1, wxRichTextCompositeObject);
        i ++;
    }
    return nullptr;
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
                container = nullptr;
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

wxIMPLEMENT_CLASS(wxRichTextDrawingHandler, wxObject);
wxIMPLEMENT_CLASS(wxRichTextDrawingContext, wxObject);

wxRichTextDrawingContext::wxRichTextDrawingContext(wxRichTextBuffer* buffer)
{
    Init();
    m_buffer = buffer;
    if (m_buffer && m_buffer->GetRichTextCtrl())
    {
        EnableVirtualAttributes(m_buffer->GetRichTextCtrl()->GetVirtualAttributesEnabled());
        m_enableImages = m_buffer->GetRichTextCtrl()->GetImagesEnabled();
        m_enableDelayedImageLoading = m_buffer->GetRichTextCtrl()->GetDelayedImageLoading();
    }
}

bool wxRichTextDrawingContext::HasVirtualAttributes(wxRichTextObject* obj) const
{
    if (!GetVirtualAttributesEnabled())
        return false;

    wxList::compatibility_iterator node = m_buffer->GetDrawingHandlers().GetFirst();
    while (node)
    {
        wxRichTextDrawingHandler *handler = (wxRichTextDrawingHandler*)node->GetData();
        if (handler->HasVirtualAttributes(obj))
            return true;

        node = node->GetNext();
    }
    return false;
}

wxRichTextAttr wxRichTextDrawingContext::GetVirtualAttributes(wxRichTextObject* obj) const
{
    wxRichTextAttr attr;
    if (!GetVirtualAttributesEnabled())
        return attr;

    // We apply all handlers, so we can may combine several different attributes
    wxList::compatibility_iterator node = m_buffer->GetDrawingHandlers().GetFirst();
    while (node)
    {
        wxRichTextDrawingHandler *handler = (wxRichTextDrawingHandler*)node->GetData();
        if (handler->HasVirtualAttributes(obj))
        {
            bool success = handler->GetVirtualAttributes(attr, obj);
            wxASSERT(success);
            wxUnusedVar(success);
        }

        node = node->GetNext();
    }
    return attr;
}

bool wxRichTextDrawingContext::ApplyVirtualAttributes(wxRichTextAttr& attr, wxRichTextObject* obj) const
{
    if (!GetVirtualAttributesEnabled())
        return false;

    if (HasVirtualAttributes(obj))
    {
        wxRichTextAttr a(GetVirtualAttributes(obj));
        attr.Apply(a);
        return true;
    }
    else
        return false;
}

int wxRichTextDrawingContext::GetVirtualSubobjectAttributesCount(wxRichTextObject* obj) const
{
    if (!GetVirtualAttributesEnabled())
        return 0;

    wxList::compatibility_iterator node = m_buffer->GetDrawingHandlers().GetFirst();
    while (node)
    {
        wxRichTextDrawingHandler *handler = (wxRichTextDrawingHandler*)node->GetData();
        int count = handler->GetVirtualSubobjectAttributesCount(obj);
        if (count > 0)
            return count;

        node = node->GetNext();
    }
    return 0;
}

int wxRichTextDrawingContext::GetVirtualSubobjectAttributes(wxRichTextObject* obj, wxArrayInt& positions, wxRichTextAttrArray& attributes) const
{
    if (!GetVirtualAttributesEnabled())
        return 0;

    wxList::compatibility_iterator node = m_buffer->GetDrawingHandlers().GetFirst();
    while (node)
    {
        wxRichTextDrawingHandler *handler = (wxRichTextDrawingHandler*)node->GetData();
        if (handler->GetVirtualSubobjectAttributes(obj, positions, attributes))
            return positions.GetCount();

        node = node->GetNext();
    }
    return 0;
}

bool wxRichTextDrawingContext::HasVirtualText(const wxRichTextPlainText* obj) const
{
    if (!GetVirtualAttributesEnabled())
        return false;

    wxList::compatibility_iterator node = m_buffer->GetDrawingHandlers().GetFirst();
    while (node)
    {
        wxRichTextDrawingHandler *handler = (wxRichTextDrawingHandler*)node->GetData();
        if (handler->HasVirtualText(obj))
            return true;

        node = node->GetNext();
    }
    return false;
}

bool wxRichTextDrawingContext::GetVirtualText(const wxRichTextPlainText* obj, wxString& text) const
{
    if (!GetVirtualAttributesEnabled())
        return false;

    wxList::compatibility_iterator node = m_buffer->GetDrawingHandlers().GetFirst();
    while (node)
    {
        wxRichTextDrawingHandler *handler = (wxRichTextDrawingHandler*)node->GetData();
        if (handler->GetVirtualText(obj, text))
            return true;

        node = node->GetNext();
    }
    return false;
}

/// Adds a handler to the end
void wxRichTextBuffer::AddDrawingHandler(wxRichTextDrawingHandler *handler)
{
    sm_drawingHandlers.Append(handler);
}

/// Inserts a handler at the front
void wxRichTextBuffer::InsertDrawingHandler(wxRichTextDrawingHandler *handler)
{
    sm_drawingHandlers.Insert( handler );
}

/// Removes a handler
bool wxRichTextBuffer::RemoveDrawingHandler(const wxString& name)
{
    wxRichTextDrawingHandler *handler = FindDrawingHandler(name);
    if (handler)
    {
        sm_drawingHandlers.DeleteObject(handler);
        delete handler;
        return true;
    }
    else
        return false;
}

wxRichTextDrawingHandler* wxRichTextBuffer::FindDrawingHandler(const wxString& name)
{
    wxList::compatibility_iterator node = sm_drawingHandlers.GetFirst();
    while (node)
    {
        wxRichTextDrawingHandler *handler = (wxRichTextDrawingHandler*)node->GetData();
        if (handler->GetName().Lower() == name.Lower()) return handler;

        node = node->GetNext();
    }
    return nullptr;
}

void wxRichTextBuffer::CleanUpDrawingHandlers()
{
    wxList::compatibility_iterator node = sm_drawingHandlers.GetFirst();
    while (node)
    {
        wxRichTextDrawingHandler* handler = (wxRichTextDrawingHandler*)node->GetData();
        wxList::compatibility_iterator next = node->GetNext();
        delete handler;
        node = next;
    }

    sm_drawingHandlers.Clear();
}

void wxRichTextBuffer::AddFieldType(wxRichTextFieldType *fieldType)
{
    sm_fieldTypes[fieldType->GetName()] = fieldType;
}

bool wxRichTextBuffer::RemoveFieldType(const wxString& name)
{
    wxRichTextFieldTypeHashMap::iterator it = sm_fieldTypes.find(name);
    if (it == sm_fieldTypes.end())
        return false;
    else
    {
        wxRichTextFieldType* fieldType = it->second;
        sm_fieldTypes.erase(it);
        delete fieldType;
        return true;
    }
}

wxRichTextFieldType *wxRichTextBuffer::FindFieldType(const wxString& name)
{
    wxRichTextFieldTypeHashMap::iterator it = sm_fieldTypes.find(name);
    if (it == sm_fieldTypes.end())
        return nullptr;
    else
        return it->second;
}

void wxRichTextBuffer::CleanUpFieldTypes()
{
    wxRichTextFieldTypeHashMap::iterator it;
    for( it = sm_fieldTypes.begin(); it != sm_fieldTypes.end(); ++it )
    {
        wxRichTextFieldType* fieldType = it->second;
        delete fieldType;
    }

    sm_fieldTypes.clear();
}

// Shadow

bool wxTextAttrShadow::operator==(const wxTextAttrShadow& shadow) const
{
    return (m_flags == shadow.m_flags &&
            m_shadowColour == shadow.m_shadowColour &&
            m_offsetX == shadow.m_offsetX &&
            m_offsetY == shadow.m_offsetY &&
            m_spread == shadow.m_spread &&
            m_blurDistance == shadow.m_blurDistance &&
            m_opacity == shadow.m_opacity);
}

void wxTextAttrShadow::Reset()
{
    m_flags = 0;
    m_shadowColour = 0;
    m_offsetX.Reset();
    m_offsetY.Reset();
    m_spread.Reset();
    m_blurDistance.Reset();
    m_opacity.Reset();
}

bool wxTextAttrShadow::EqPartial(const wxTextAttrShadow& shadow, bool weakTest) const
{
    if (!weakTest &&
        ((!m_offsetX.IsValid() && shadow.m_offsetX.IsValid()) ||
         (!m_offsetY.IsValid() && shadow.m_offsetY.IsValid()) ||
         (!m_spread.IsValid() && shadow.m_spread.IsValid()) ||
         (!m_blurDistance.IsValid() && shadow.m_blurDistance.IsValid()) ||
         (!m_opacity.IsValid() && shadow.m_opacity.IsValid()) ||
         (!HasColour() && shadow.HasColour())
         ))
    {
        return false;
    }

    if (m_offsetX.IsValid() && shadow.m_offsetX.IsValid() && !(m_offsetX == shadow.m_offsetX))
        return false;

    if (m_offsetY.IsValid() && shadow.m_offsetY.IsValid() && !(m_offsetY == shadow.m_offsetY))
        return false;

    if (m_spread.IsValid() && shadow.m_spread.IsValid() && !(m_spread == shadow.m_spread))
        return false;

    if (m_blurDistance.IsValid() && shadow.m_blurDistance.IsValid() && !(m_blurDistance == shadow.m_blurDistance))
        return false;

    if (m_opacity.IsValid() && shadow.m_opacity.IsValid() && !(m_opacity == shadow.m_opacity))
        return false;

    if (HasColour() && shadow.HasColour() && m_shadowColour != shadow.m_shadowColour)
        return false;

    return true;
}

bool wxTextAttrShadow::Apply(const wxTextAttrShadow& shadow, const wxTextAttrShadow* compareWith)
{
    m_offsetX.Apply(shadow.m_offsetX, compareWith ? (& compareWith->m_offsetX) : nullptr);
    m_offsetY.Apply(shadow.m_offsetY, compareWith ? (& compareWith->m_offsetY) : nullptr);
    m_spread.Apply(shadow.m_spread, compareWith ? (& compareWith->m_spread) : nullptr);
    m_blurDistance.Apply(shadow.m_blurDistance, compareWith ? (& compareWith->m_blurDistance) : nullptr);
    m_opacity.Apply(shadow.m_opacity, compareWith ? (& compareWith->m_opacity) : nullptr);

    if (shadow.HasColour() && !(compareWith && (shadow.m_shadowColour == compareWith->m_shadowColour)))
        SetColour(shadow.m_shadowColour);

    if (!IsDefault())
        SetValid(true);

    return true;
}

bool wxTextAttrShadow::RemoveStyle(const wxTextAttrShadow& attr)
{
    if (attr.GetOffsetX().IsValid() && GetOffsetX().IsValid())
        GetOffsetX().Reset();
    if (attr.GetOffsetY().IsValid() && GetOffsetY().IsValid())
        GetOffsetY().Reset();
    if (attr.GetSpread().IsValid() && GetSpread().IsValid())
        GetSpread().Reset();
    if (attr.GetBlurDistance().IsValid() && GetBlurDistance().IsValid())
        GetBlurDistance().Reset();
    if (attr.GetOpacity().IsValid() && GetOpacity().IsValid())
        GetOpacity().Reset();
    if (attr.HasColour() && HasColour())
        RemoveFlag(wxTEXT_BOX_ATTR_BORDER_COLOUR);

    return true;
}

void wxTextAttrShadow::CollectCommonAttributes(const wxTextAttrShadow& attr, wxTextAttrShadow& clashingAttr, wxTextAttrShadow& absentAttr)
{
    GetOffsetX().CollectCommonAttributes(attr.GetOffsetX(), clashingAttr.GetOffsetX(), absentAttr.GetOffsetX());
    GetOffsetY().CollectCommonAttributes(attr.GetOffsetY(), clashingAttr.GetOffsetY(), absentAttr.GetOffsetY());
    GetSpread().CollectCommonAttributes(attr.GetSpread(), clashingAttr.GetSpread(), absentAttr.GetSpread());
    GetBlurDistance().CollectCommonAttributes(attr.GetBlurDistance(), clashingAttr.GetBlurDistance(), absentAttr.GetBlurDistance());
    GetOpacity().CollectCommonAttributes(attr.GetOpacity(), clashingAttr.GetOpacity(), absentAttr.GetOpacity());

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
    {
        absentAttr.AddFlag(wxTEXT_BOX_ATTR_BORDER_COLOUR);
        RemoveFlag(wxTEXT_BOX_ATTR_BORDER_COLOUR);
    }
}

#endif
    // wxUSE_RICHTEXT
