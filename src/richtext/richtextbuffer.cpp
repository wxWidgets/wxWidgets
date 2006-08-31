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

#include "wx/filename.h"
#include "wx/clipbrd.h"
#include "wx/wfstream.h"
#include "wx/mstream.h"
#include "wx/sstream.h"

#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"

#include "wx/listimpl.cpp"

WX_DEFINE_LIST(wxRichTextObjectList)
WX_DEFINE_LIST(wxRichTextLineList)

/*!
 * wxRichTextObject
 * This is the base for drawable objects.
 */

IMPLEMENT_CLASS(wxRichTextObject, wxObject)

wxRichTextObject::wxRichTextObject(wxRichTextObject* parent)
{
    m_dirty = false;
    m_refCount = 1;
    m_parent = parent;
    m_leftMargin = 0;
    m_rightMargin = 0;
    m_topMargin = 0;
    m_bottomMargin = 0;
    m_descent = 0;
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
    m_pos = obj.m_pos;
    m_dirty = obj.m_dirty;
    m_range = obj.m_range;
    m_attributes = obj.m_attributes;
    m_descent = obj.m_descent;

    if (!m_attributes.GetFont().Ok())
        wxLogDebug(wxT("No font!"));
    if (!obj.m_attributes.GetFont().Ok())
        wxLogDebug(wxT("Parent has no font!"));
}

void wxRichTextObject::SetMargins(int margin)
{
    m_leftMargin = m_rightMargin = m_topMargin = m_bottomMargin = margin;
}

void wxRichTextObject::SetMargins(int leftMargin, int rightMargin, int topMargin, int bottomMargin)
{
    m_leftMargin = leftMargin;
    m_rightMargin = rightMargin;
    m_topMargin = topMargin;
    m_bottomMargin = bottomMargin;
}

// Convert units in tends of a millimetre to device units
int wxRichTextObject::ConvertTenthsMMToPixels(wxDC& dc, int units)
{
    int ppi = dc.GetPPI().x;

    // There are ppi pixels in 254.1 "1/10 mm"

    double pixels = ((double) units * (double)ppi) / 254.1;

    return (int) pixels;
}

/// Dump to output stream for debugging
void wxRichTextObject::Dump(wxTextOutputStream& stream)
{
    stream << GetClassInfo()->GetClassName() << wxT("\n");
    stream << wxString::Format(wxT("Size: %d,%d. Position: %d,%d, Range: %ld,%ld"), m_size.x, m_size.y, m_pos.x, m_pos.y, m_range.GetStart(), m_range.GetEnd()) << wxT("\n");
    stream << wxString::Format(wxT("Text colour: %d,%d,%d."), (int) m_attributes.GetTextColour().Red(), (int) m_attributes.GetTextColour().Green(), (int) m_attributes.GetTextColour().Blue()) << wxT("\n");
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
        m_children.Append(child->Clone());

        node = node->GetNext();
    }
}

/// Hit-testing: returns a flag indicating hit test details, plus
/// information about position
int wxRichTextCompositeObject::HitTest(wxDC& dc, const wxPoint& pt, long& textPosition)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        int ret = child->HitTest(dc, pt, textPosition);
        if (ret != wxRICHTEXT_HITTEST_NONE)
            return ret;

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

        if (child->FindPosition(dc, index, pt, height, forceLineStart))
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

    end = lastEnd;

    // An object with no children has zero length
    if (m_children.GetCount() == 0)
        end --;

    m_range.SetRange(start, end);
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

/// Recursively merge all pieces that can be merged.
bool wxRichTextCompositeObject::Defragment()
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
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


/*!
 * wxRichTextBox
 * This defines a 2D space to lay out objects
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextBox, wxRichTextCompositeObject)

wxRichTextBox::wxRichTextBox(wxRichTextObject* parent):
    wxRichTextCompositeObject(parent)
{
}

/// Draw the item
bool wxRichTextBox::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& WXUNUSED(rect), int descent, int style)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        wxRect childRect = wxRect(child->GetPosition(), child->GetCachedSize());
        child->Draw(dc, range, selectionRange, childRect, descent, style);

        node = node->GetNext();
    }
    return true;
}

/// Lay the item out
bool wxRichTextBox::Layout(wxDC& dc, const wxRect& rect, int style)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        child->Layout(dc, rect, style);

        node = node->GetNext();
    }
    m_dirty = false;
    return true;
}

/// Get/set the size for the given range. Assume only has one child.
bool wxRichTextBox::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position) const
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    if (node)
    {
        wxRichTextObject* child = node->GetData();
        return child->GetRangeSize(range, size, descent, dc, flags, position);
    }
    else
        return false;
}

/// Copy
void wxRichTextBox::Copy(const wxRichTextBox& obj)
{
    wxRichTextCompositeObject::Copy(obj);
}


/*!
 * wxRichTextParagraphLayoutBox
 * This box knows how to lay out paragraphs.
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextParagraphLayoutBox, wxRichTextBox)

wxRichTextParagraphLayoutBox::wxRichTextParagraphLayoutBox(wxRichTextObject* parent):
    wxRichTextBox(parent)
{
    Init();
}

/// Initialize the object.
void wxRichTextParagraphLayoutBox::Init()
{
    m_ctrl = NULL;

    // For now, assume is the only box and has no initial size.
    m_range = wxRichTextRange(0, -1);

    m_invalidRange.SetRange(-1, -1);
    m_leftMargin = 4;
    m_rightMargin = 4;
    m_topMargin = 4;
    m_bottomMargin = 4;
}

/// Draw the item
bool wxRichTextParagraphLayoutBox::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int descent, int style)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (child != NULL);

        if (child && !child->GetRange().IsOutside(range))
        {
            wxRect childRect(child->GetPosition(), child->GetCachedSize());

            if (childRect.GetTop() > rect.GetBottom() || childRect.GetBottom() < rect.GetTop())
            {
                // Skip
            }
            else
                child->Draw(dc, child->GetRange(), selectionRange, childRect, descent, style);
        }

        node = node->GetNext();
    }
    return true;
}

/// Lay the item out
bool wxRichTextParagraphLayoutBox::Layout(wxDC& dc, const wxRect& rect, int style)
{
    wxRect availableSpace;
    bool formatRect = (style & wxRICHTEXT_LAYOUT_SPECIFIED_RECT) == wxRICHTEXT_LAYOUT_SPECIFIED_RECT;

    // If only laying out a specific area, the passed rect has a different meaning:
    // the visible part of the buffer.
    if (formatRect)
    {
        availableSpace = wxRect(0 + m_leftMargin,
                          0 + m_topMargin,
                          rect.width - m_leftMargin - m_rightMargin,
                          rect.height);

        // Invalidate the part of the buffer from the first visible line
        // to the end. If other parts of the buffer are currently invalid,
        // then they too will be taken into account if they are above
        // the visible point.
        long startPos = 0;
        wxRichTextLine* line = GetLineAtYPosition(rect.y);
        if (line)
            startPos = line->GetAbsoluteRange().GetStart();

        Invalidate(wxRichTextRange(startPos, GetRange().GetEnd()));
    }
    else
        availableSpace = wxRect(rect.x + m_leftMargin,
                          rect.y + m_topMargin,
                          rect.width - m_leftMargin - m_rightMargin,
                          rect.height - m_topMargin - m_bottomMargin);

    int maxWidth = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();

    bool layoutAll = true;

    // Get invalid range, rounding to paragraph start/end.
    wxRichTextRange invalidRange = GetInvalidRange(true);

    if (invalidRange == wxRICHTEXT_NONE && !formatRect)
        return true;

    if (invalidRange == wxRICHTEXT_ALL)
        layoutAll = true;
    else    // If we know what range is affected, start laying out from that point on.
        if (invalidRange.GetStart() > GetRange().GetStart())
    {
        wxRichTextParagraph* firstParagraph = GetParagraphAtPosition(invalidRange.GetStart());
        if (firstParagraph)
        {
            wxRichTextObjectList::compatibility_iterator firstNode = m_children.Find(firstParagraph);
            wxRichTextObjectList::compatibility_iterator previousNode;
            if ( firstNode )
                previousNode = firstNode->GetPrevious();
            if (firstNode && previousNode)
            {
                wxRichTextParagraph* previousParagraph = wxDynamicCast(previousNode->GetData(), wxRichTextParagraph);
                availableSpace.y = previousParagraph->GetPosition().y + previousParagraph->GetCachedSize().y;

                // Now we're going to start iterating from the first affected paragraph.
                node = firstNode;

                layoutAll = false;
            }
        }
    }

    // A way to force speedy rest-of-buffer layout (the 'else' below)
    bool forceQuickLayout = false;

    while (node)
    {
        // Assume this box only contains paragraphs

        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxCHECK_MSG( child, false, _T("Unknown object in layout") );

        // TODO: what if the child hasn't been laid out (e.g. involved in Undo) but still has 'old' lines
        if ( !forceQuickLayout &&
                (layoutAll ||
                    child->GetLines().IsEmpty() ||
                        !child->GetRange().IsOutside(invalidRange)) )
        {
            child->Layout(dc, availableSpace, style);

            // Layout must set the cached size
            availableSpace.y += child->GetCachedSize().y;
            maxWidth = wxMax(maxWidth, child->GetCachedSize().x);

            // If we're just formatting the visible part of the buffer,
            // and we're now past the bottom of the window, start quick
            // layout.
            if (formatRect && child->GetPosition().y > rect.GetBottom())
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
                        child->Layout(dc, availableSpace, style);
                    else
                        child->SetPosition(wxPoint(child->GetPosition().x, child->GetPosition().y + inc));

                    availableSpace.y += child->GetCachedSize().y;
                    maxWidth = wxMax(maxWidth, child->GetCachedSize().x);
                }

                node = node->GetNext();
            }
            break;
        }

        node = node->GetNext();
    }

    SetCachedSize(wxSize(maxWidth, availableSpace.y));

    m_dirty = false;
    m_invalidRange = wxRICHTEXT_NONE;

    return true;
}

/// Copy
void wxRichTextParagraphLayoutBox::Copy(const wxRichTextParagraphLayoutBox& obj)
{
    wxRichTextBox::Copy(obj);
}

/// Get/set the size for the given range.
bool wxRichTextParagraphLayoutBox::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position) const
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
        wxASSERT (child != NULL);

        // Return first child in buffer if position is -1
        // if (pos == -1)
        //    return child;

        if (child->GetRange().Contains(pos))
            return child;

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
        // child is a paragraph
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (child != NULL);

        wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
        while (node2)
        {
            wxRichTextLine* line = node2->GetData();

            wxRichTextRange range = line->GetAbsoluteRange();

            if (range.Contains(pos) ||

                // If the position is end-of-paragraph, then return the last line of
                // of the paragraph.
                (range.GetEnd() == child->GetRange().GetEnd()-1) && (pos == child->GetRange().GetEnd()))
                return line;

            node2 = node2->GetNext();
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
        wxASSERT (child != NULL);

        wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
        while (node2)
        {
            wxRichTextLine* line = node2->GetData();

            wxRect rect(line->GetRect());

            if (y <= rect.GetBottom())
                return line;

            node2 = node2->GetNext();
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
        wxASSERT (child != NULL);

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
wxRichTextRange wxRichTextParagraphLayoutBox::AddParagraph(const wxString& text)
{
    wxTextAttrEx style(GetAttributes());

    // Apply default style. If the style has no attributes set,
    // then the attributes will remain the 'basic style' (i.e. the
    // layout box's style).
    wxRichTextApplyStyle(style, GetDefaultStyle());

    wxRichTextParagraph* para = new wxRichTextParagraph(text, this, & style);

    AppendChild(para);

    UpdateRanges();
    SetDirty(true);

    return para->GetRange();
}

/// Adds multiple paragraphs, based on newlines.
wxRichTextRange wxRichTextParagraphLayoutBox::AddParagraphs(const wxString& text)
{
    wxTextAttrEx style(GetAttributes());
    //wxLogDebug("Initial style = %s", style.GetFont().GetFaceName());
    //wxLogDebug("Initial size = %d", style.GetFont().GetPointSize());

    // Apply default style. If the style has no attributes set,
    // then the attributes will remain the 'basic style' (i.e. the
    // layout box's style).
    wxRichTextApplyStyle(style, GetDefaultStyle());

    //wxLogDebug("Style after applying default style = %s", style.GetFont().GetFaceName());
    //wxLogDebug("Size after applying default style = %d", style.GetFont().GetPointSize());

    wxRichTextParagraph* firstPara = NULL;
    wxRichTextParagraph* lastPara = NULL;

    wxRichTextRange range(-1, -1);
    size_t i = 0;
    size_t len = text.length();
    wxString line;
    while (i < len)
    {
        wxChar ch = text[i];
        if (ch == wxT('\n') || ch == wxT('\r'))
        {
            wxRichTextParagraph* para = new wxRichTextParagraph(line, this, & style);

            AppendChild(para);
            if (!firstPara)
                firstPara = para;
            lastPara = para;
            line = wxEmptyString;
        }
        else
            line += ch;

        i ++;
    }
    if (!line.empty())
    {
        lastPara = new wxRichTextParagraph(line, this, & style);
        //wxLogDebug("Para Face = %s", lastPara->GetAttributes().GetFont().GetFaceName());
        AppendChild(lastPara);
    }

    if (firstPara)
        range.SetStart(firstPara->GetRange().GetStart());
    else if (lastPara)
        range.SetStart(lastPara->GetRange().GetStart());

    if (lastPara)
        range.SetEnd(lastPara->GetRange().GetEnd());
    else if (firstPara)
        range.SetEnd(firstPara->GetRange().GetEnd());

    UpdateRanges();
    SetDirty(false);

    return GetRange();
}

/// Convenience function to add an image
wxRichTextRange wxRichTextParagraphLayoutBox::AddImage(const wxImage& image)
{
    wxTextAttrEx style(GetAttributes());

    // Apply default style. If the style has no attributes set,
    // then the attributes will remain the 'basic style' (i.e. the
    // layout box's style).
    wxRichTextApplyStyle(style, GetDefaultStyle());

    wxRichTextParagraph* para = new wxRichTextParagraph(this, & style);
    AppendChild(para);
    para->AppendChild(new wxRichTextImage(image, this));

    UpdateRanges();
    SetDirty(true);

    return para->GetRange();
}


/// Insert fragment into this box at the given position. If partialParagraph is true,
/// it is assumed that the last (or only) paragraph is just a piece of data with no paragraph
/// marker.
/// TODO: if fragment is inserted inside styled fragment, must apply that style to
/// to the data (if it has a default style, anyway).

bool wxRichTextParagraphLayoutBox::InsertFragment(long position, wxRichTextFragment& fragment)
{
    SetDirty(true);

    // First, find the first paragraph whose starting position is within the range.
    wxRichTextParagraph* para = GetParagraphAtPosition(position);
    if (para)
    {
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

            wxRichTextObjectList::compatibility_iterator objectNode = firstPara->GetChildren().GetFirst();
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

            // If there was only one paragraph, we need to insert a new one.
            if (!i)
            {
                finalPara = new wxRichTextParagraph;

                // TODO: These attributes should come from the subsequent paragraph
                // when originally deleted, since the subsequent para takes on
                // the previous para's attributes.
                finalPara->SetAttributes(firstPara->GetAttributes());

                if (nextParagraph)
                    InsertChild(finalPara, nextParagraph);
                else
                    AppendChild(finalPara);
            }
            else while (i)
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

            // 4. Add back the remaining content.
            if (finalPara)
            {
                finalPara->MoveFromList(savedObjects);

                // Ensure there's at least one object
                if (finalPara->GetChildCount() == 0)
                {
                    wxRichTextPlainText* text = new wxRichTextPlainText(wxEmptyString);
                    text->SetAttributes(finalPara->GetAttributes());

                    finalPara->AppendChild(text);
                }
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
bool wxRichTextParagraphLayoutBox::CopyFragment(const wxRichTextRange& range, wxRichTextFragment& fragment)
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
        wxRichTextRange topTailRange(range);

        wxRichTextParagraph* firstPara = wxDynamicCast(fragment.GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
        wxASSERT( firstPara != NULL );

        // Chop off the start of the paragraph
        if (topTailRange.GetStart() > firstPara->GetRange().GetStart())
        {
            wxRichTextRange r(firstPara->GetRange().GetStart(), topTailRange.GetStart()-1);
            firstPara->DeleteRange(r);

            // Make sure the numbering is correct
            long end;
            fragment.CalculateRange(firstPara->GetRange().GetStart(), end);

            // Now, we've deleted some positions, so adjust the range
            // accordingly.
            topTailRange.SetEnd(topTailRange.GetEnd() - r.GetLength());
        }

        wxRichTextParagraph* lastPara = wxDynamicCast(fragment.GetChildren().GetLast()->GetData(), wxRichTextParagraph);
        wxASSERT( lastPara != NULL );

        if (topTailRange.GetEnd() < (lastPara->GetRange().GetEnd()-1))
        {
            wxRichTextRange r(topTailRange.GetEnd()+1, lastPara->GetRange().GetEnd()-1); /* -1 since actual text ends 1 position before end of para marker */
            lastPara->DeleteRange(r);

            // Make sure the numbering is correct
            long end;
            fragment.CalculateRange(firstPara->GetRange().GetStart(), end);

            // We only have part of a paragraph at the end
            fragment.SetPartialParagraph(true);
        }
        else
        {
            if (topTailRange.GetEnd() == (lastPara->GetRange().GetEnd() - 1))
                // We have a partial paragraph (don't save last new paragraph marker)
                fragment.SetPartialParagraph(true);
            else
                // We have a complete paragraph
                fragment.SetPartialParagraph(false);
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
        wxASSERT( child != NULL );

        if (child->GetRange().Contains(pos))
        {
            wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
            while (node2)
            {
                wxRichTextLine* line = node2->GetData();
                wxRichTextRange lineRange = line->GetAbsoluteRange();

                if (lineRange.Contains(pos))
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
        wxASSERT(child != NULL);

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

        node = node->GetNext();
    }

    // Didn't find it
    return NULL;
}

/// Delete range from layout.
bool wxRichTextParagraphLayoutBox::DeleteRange(const wxRichTextRange& range)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();

    while (node)
    {
        wxRichTextParagraph* obj = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (obj != NULL);

        wxRichTextObjectList::compatibility_iterator next = node->GetNext();

        // Delete the range in each paragraph

        if (!obj->GetRange().IsOutside(range))
        {
            // Deletes the content of this object within the given range
            obj->DeleteRange(range);

            // If the whole paragraph is within the range to delete,
            // delete the whole thing.
            if (range.GetStart() <= obj->GetRange().GetStart() && range.GetEnd() >= obj->GetRange().GetEnd())
            {
                // Delete the whole object
                RemoveChild(obj, true);
            }
            // If the range includes the paragraph end, we need to join this
            // and the next paragraph.
            else if (range.Contains(obj->GetRange().GetEnd()))
            {
                // We need to move the objects from the next paragraph
                // to this paragraph

                if (next)
                {
                    wxRichTextParagraph* nextParagraph = wxDynamicCast(next->GetData(), wxRichTextParagraph);
                    next = next->GetNext();
                    if (nextParagraph)
                    {
                        // Delete the stuff we need to delete
                        nextParagraph->DeleteRange(range);

                        // Move the objects to the previous para
                        wxRichTextObjectList::compatibility_iterator node1 = nextParagraph->GetChildren().GetFirst();

                        while (node1)
                        {
                            wxRichTextObject* obj1 = node1->GetData();

                            // If the object is empty, optimise it out
                            if (obj1->IsEmpty())
                            {
                                delete obj1;
                            }
                            else
                            {
                                obj->AppendChild(obj1);
                            }

                            wxRichTextObjectList::compatibility_iterator next1 = node1->GetNext();
                            nextParagraph->GetChildren().Erase(node1);

                            node1 = next1;
                        }

                        // Delete the paragraph
                        RemoveChild(nextParagraph, true);

                    }
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
            if (lineCount > 0)
                text += wxT("\n");
            wxRichTextRange childRange = range;
            childRange.LimitTo(child->GetRange());

            wxString childText = child->GetTextForRange(childRange);

            text += childText;

            lineCount ++;
        }
        node = node->GetNext();
    }

    return text;
}

/// Get all the text
wxString wxRichTextParagraphLayoutBox::GetText() const
{
    return GetTextForRange(GetRange());
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
bool wxRichTextParagraphLayoutBox::SetStyle(const wxRichTextRange& range, const wxRichTextAttr& style, bool withUndo)
{
    bool characterStyle = false;
    bool paragraphStyle = false;

    if (style.IsCharacterStyle())
        characterStyle = true;
    if (style.IsParagraphStyle())
        paragraphStyle = true;

    // If we are associated with a control, make undoable; otherwise, apply immediately
    // to the data.

    bool haveControl = (GetRichTextCtrl() != NULL);

    wxRichTextAction* action = NULL;

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(NULL, _("Change Style"), wxRICHTEXT_CHANGE_STYLE, & GetRichTextCtrl()->GetBuffer(), GetRichTextCtrl());
        action->SetRange(range);
        action->SetPosition(GetRichTextCtrl()->GetCaretPosition());
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

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

                if (paragraphStyle)
                    wxRichTextApplyStyle(newPara->GetAttributes(), style);

                if (characterStyle && range.GetStart() != newPara->GetRange().GetEnd())
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
                    if (splitPoint == newPara->GetRange().GetEnd() || splitPoint == (newPara->GetRange().GetEnd() - 1))
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

                        wxRichTextApplyStyle(child->GetAttributes(), style);
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
        GetRichTextCtrl()->GetBuffer().SubmitAction(action);

    return true;
}

/// Set text attributes
bool wxRichTextParagraphLayoutBox::SetStyle(const wxRichTextRange& range, const wxTextAttrEx& style, bool withUndo)
{
    wxRichTextAttr richStyle = style;
    return SetStyle(range, richStyle, withUndo);
}

/// Get the text attributes for this position.
bool wxRichTextParagraphLayoutBox::GetStyle(long position, wxTextAttrEx& style) const
{
    wxRichTextObject* obj wxDUMMY_INITIALIZE(NULL);

    if (style.IsParagraphStyle())
        obj = GetParagraphAtPosition(position);
    else
        obj = GetLeafObjectAtPosition(position);

    if (obj)
    {
        style = obj->GetAttributes();
        return true;
    }
    else
        return false;
}

/// Get the text attributes for this position.
bool wxRichTextParagraphLayoutBox::GetStyle(long position, wxRichTextAttr& style) const
{
    wxRichTextObject* obj wxDUMMY_INITIALIZE(NULL);

    if (style.IsParagraphStyle())
        obj = GetParagraphAtPosition(position);
    else
        obj = GetLeafObjectAtPosition(position);

    if (obj)
    {
        style = obj->GetAttributes();
        return true;
    }
    else
        return false;
}

/// Set default style
bool wxRichTextParagraphLayoutBox::SetDefaultStyle(const wxTextAttrEx& style)
{
    // keep the old attributes if the new style doesn't specify them unless the
    // new style is empty - then reset m_defaultStyle (as there is no other way
    // to do it)
    if ( style.IsDefault() )
        m_defaultAttributes = style;
    else
        m_defaultAttributes = wxTextAttrEx::CombineEx(style, m_defaultAttributes, NULL);

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
        wxASSERT (para != NULL);

        if (para)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                return foundCount == matchingCount;

            if (!para->GetRange().IsOutside(range))
            {
                wxRichTextObjectList::compatibility_iterator node2 = para->GetChildren().GetFirst();

                while (node2)
                {
                    wxRichTextObject* child = node2->GetData();
                    if (!child->GetRange().IsOutside(range) && child->IsKindOf(CLASSINFO(wxRichTextPlainText)))
                    {
                        foundCount ++;
                        if (wxTextAttrEqPartial(child->GetAttributes(), style, style.GetFlags()))
                            matchingCount ++;
                    }

                    node2 = node2->GetNext();
                }
            }
        }

        node = node->GetNext();
    }

    return foundCount == matchingCount;
}

bool wxRichTextParagraphLayoutBox::HasCharacterAttributes(const wxRichTextRange& range, const wxTextAttrEx& style) const
{
    wxRichTextAttr richStyle = style;
    return HasCharacterAttributes(range, richStyle);
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
        wxASSERT (para != NULL);

        if (para)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                return foundCount == matchingCount;

            if (!para->GetRange().IsOutside(range))
            {
                foundCount ++;
                if (wxTextAttrEqPartial(para->GetAttributes(), style, style.GetFlags()))
                    matchingCount ++;
            }
        }

        node = node->GetNext();
    }
    return foundCount == matchingCount;
}

bool wxRichTextParagraphLayoutBox::HasParagraphAttributes(const wxRichTextRange& range, const wxTextAttrEx& style) const
{
    wxRichTextAttr richStyle = style;
    return HasParagraphAttributes(range, richStyle);
}

void wxRichTextParagraphLayoutBox::Clear()
{
    DeleteChildren();
}

void wxRichTextParagraphLayoutBox::Reset()
{
    Clear();

    AddParagraph(wxEmptyString);
}

/// Invalidate the buffer. With no argument, invalidates whole buffer.
void wxRichTextParagraphLayoutBox::Invalidate(const wxRichTextRange& invalidRange)
{
    SetDirty(true);

    if (invalidRange == wxRICHTEXT_ALL)
    {
        m_invalidRange = wxRICHTEXT_ALL;
        return;
    }

    // Already invalidating everything
    if (m_invalidRange == wxRICHTEXT_ALL)
        return;

    if ((invalidRange.GetStart() < m_invalidRange.GetStart()) || m_invalidRange.GetStart() == -1)
        m_invalidRange.SetStart(invalidRange.GetStart());
    if (invalidRange.GetEnd() > m_invalidRange.GetEnd())
        m_invalidRange.SetEnd(invalidRange.GetEnd());
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
        wxRichTextParagraph* para2 = GetParagraphAtPosition(range.GetEnd());
        if (para1)
            range.SetStart(para1->GetRange().GetStart());
        if (para2)
            range.SetEnd(para2->GetRange().GetEnd());
    }
    return range;
}

/*!
 * wxRichTextFragment class declaration
 * This is a lind of paragraph layout box used for storing
 * paragraphs for Undo/Redo, for example.
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextFragment, wxRichTextParagraphLayoutBox)

/// Initialise
void wxRichTextFragment::Init()
{
    m_partialParagraph = false;
}

/// Copy
void wxRichTextFragment::Copy(const wxRichTextFragment& obj)
{
    wxRichTextParagraphLayoutBox::Copy(obj);

    m_partialParagraph = obj.m_partialParagraph;
}

/*!
 * wxRichTextParagraph
 * This object represents a single paragraph (or in a straight text editor, a line).
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextParagraph, wxRichTextBox)

wxRichTextParagraph::wxRichTextParagraph(wxRichTextObject* parent, wxTextAttrEx* style):
    wxRichTextBox(parent)
{
    if (parent && !style)
        SetAttributes(parent->GetAttributes());
    if (style)
        SetAttributes(*style);
}

wxRichTextParagraph::wxRichTextParagraph(const wxString& text, wxRichTextObject* parent, wxTextAttrEx* style):
    wxRichTextBox(parent)
{
    if (parent && !style)
        SetAttributes(parent->GetAttributes());
    if (style)
        SetAttributes(*style);

    AppendChild(new wxRichTextPlainText(text, this));
}

wxRichTextParagraph::~wxRichTextParagraph()
{
    ClearLines();
}

/// Draw the item
bool wxRichTextParagraph::Draw(wxDC& dc, const wxRichTextRange& WXUNUSED(range), const wxRichTextRange& selectionRange, const wxRect& WXUNUSED(rect), int WXUNUSED(descent), int style)
{
    // Draw the bullet, if any
    if (GetAttributes().GetBulletStyle() != wxTEXT_ATTR_BULLET_STYLE_NONE)
    {
        if (GetAttributes().GetLeftSubIndent() != 0)
        {
            int spaceBeforePara = ConvertTenthsMMToPixels(dc, GetAttributes().GetParagraphSpacingBefore());
            // int spaceAfterPara = ConvertTenthsMMToPixels(dc, GetAttributes().GetParagraphSpacingAfter());
            int leftIndent = ConvertTenthsMMToPixels(dc, GetAttributes().GetLeftIndent());
            // int leftSubIndent = ConvertTenthsMMToPixels(dc, GetAttributes().GetLeftSubIndent());
            // int rightIndent = ConvertTenthsMMToPixels(dc, GetAttributes().GetRightIndent());

            if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_BITMAP)
            {
                // TODO
            }
            else
            {
                wxString bulletText = GetBulletText();
                if (!bulletText.empty())
                {
                    if (GetAttributes().GetFont().Ok())
                        dc.SetFont(GetAttributes().GetFont());

                    if (GetAttributes().GetTextColour().Ok())
                        dc.SetTextForeground(GetAttributes().GetTextColour());

                    dc.SetBackgroundMode(wxTRANSPARENT);

                    // Get line height from first line, if any
                    wxRichTextLine* line = m_cachedLines.GetFirst() ? (wxRichTextLine* ) m_cachedLines.GetFirst()->GetData() : (wxRichTextLine*) NULL;

                    wxPoint linePos;
                    int lineHeight wxDUMMY_INITIALIZE(0);
                    if (line)
                    {
                        lineHeight = line->GetSize().y;
                        linePos = line->GetPosition() + GetPosition();
                    }
                    else
                    {
                        lineHeight = dc.GetCharHeight();
                        linePos = GetPosition();
                        linePos.y += spaceBeforePara;
                    }

                    int charHeight = dc.GetCharHeight();

                    int x = GetPosition().x + leftIndent;
                    int y = linePos.y + (lineHeight - charHeight);

                    dc.DrawText(bulletText, x, y);
                }
            }
        }
    }

    // Draw the range for each line, one object at a time.

    wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetFirst();
    while (node)
    {
        wxRichTextLine* line = node->GetData();
        wxRichTextRange lineRange = line->GetAbsoluteRange();

        int maxDescent = line->GetDescent();

        // Lines are specified relative to the paragraph

        wxPoint linePosition = line->GetPosition() + GetPosition();
        wxPoint objectPosition = linePosition;

        // Loop through objects until we get to the one within range
        wxRichTextObjectList::compatibility_iterator node2 = m_children.GetFirst();
        while (node2)
        {
            wxRichTextObject* child = node2->GetData();
            if (!child->GetRange().IsOutside(lineRange))
            {
                // Draw this part of the line at the correct position
                wxRichTextRange objectRange(child->GetRange());
                objectRange.LimitTo(lineRange);

                wxSize objectSize;
                int descent = 0;
                child->GetRangeSize(objectRange, objectSize, descent, dc, wxRICHTEXT_UNFORMATTED, objectPosition);

                // Use the child object's width, but the whole line's height
                wxRect childRect(objectPosition, wxSize(objectSize.x, line->GetSize().y));
                child->Draw(dc, objectRange, selectionRange, childRect, maxDescent, style);

                objectPosition.x += objectSize.x;
            }
            else if (child->GetRange().GetStart() > lineRange.GetEnd())
                // Can break out of inner loop now since we've passed this line's range
                break;

            node2 = node2->GetNext();
        }

        node = node->GetNext();
    }

    return true;
}

/// Lay the item out
bool wxRichTextParagraph::Layout(wxDC& dc, const wxRect& rect, int style)
{
    // ClearLines();

    // Increase the size of the paragraph due to spacing
    int spaceBeforePara = ConvertTenthsMMToPixels(dc, GetAttributes().GetParagraphSpacingBefore());
    int spaceAfterPara = ConvertTenthsMMToPixels(dc, GetAttributes().GetParagraphSpacingAfter());
    int leftIndent = ConvertTenthsMMToPixels(dc, GetAttributes().GetLeftIndent());
    int leftSubIndent = ConvertTenthsMMToPixels(dc, GetAttributes().GetLeftSubIndent());
    int rightIndent = ConvertTenthsMMToPixels(dc, GetAttributes().GetRightIndent());

    int lineSpacing = 0;

    // Let's assume line spacing of 10 is normal, 15 is 1.5, 20 is 2, etc.
    if (GetAttributes().GetLineSpacing() > 10 && GetAttributes().GetFont().Ok())
    {
        dc.SetFont(GetAttributes().GetFont());
        lineSpacing = (ConvertTenthsMMToPixels(dc, dc.GetCharHeight()) * GetAttributes().GetLineSpacing())/10;
    }

    // Available space for text on each line differs.
    int availableTextSpaceFirstLine = rect.GetWidth() - leftIndent - rightIndent;

    // Bullets start the text at the same position as subsequent lines
    if (GetAttributes().GetBulletStyle() != wxTEXT_ATTR_BULLET_STYLE_NONE)
        availableTextSpaceFirstLine -= leftSubIndent;

    int availableTextSpaceSubsequentLines = rect.GetWidth() - leftIndent - rightIndent - leftSubIndent;

    // Start position for each line relative to the paragraph
    int startPositionFirstLine = leftIndent;
    int startPositionSubsequentLines = leftIndent + leftSubIndent;

    // If we have a bullet in this paragraph, the start position for the first line's text
    // is actually leftIndent + leftSubIndent.
    if (GetAttributes().GetBulletStyle() != wxTEXT_ATTR_BULLET_STYLE_NONE)
        startPositionFirstLine = startPositionSubsequentLines;

    //bool restrictWidth = wxRichTextHasStyle(style, wxRICHTEXT_FIXED_WIDTH);
    //bool restrictHeight = wxRichTextHasStyle(style, wxRICHTEXT_FIXED_HEIGHT);

    long lastEndPos = GetRange().GetStart()-1;
    long lastCompletedEndPos = lastEndPos;

    int currentWidth = 0;
    SetPosition(rect.GetPosition());

    wxPoint currentPosition(0, spaceBeforePara); // We will calculate lines relative to paragraph
    int lineHeight = 0;
    int maxWidth = 0;
    int maxDescent = 0;

    int lineCount = 0;

    // Split up lines

    // We may need to go back to a previous child, in which case create the new line,
    // find the child corresponding to the start position of the string, and
    // continue.

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        // If this is e.g. a composite text box, it will need to be laid out itself.
        // But if just a text fragment or image, for example, this will
        // do nothing. NB: won't we need to set the position after layout?
        // since for example if position is dependent on vertical line size, we
        // can't tell the position until the size is determined. So possibly introduce
        // another layout phase.

        child->Layout(dc, rect, style);

        // Available width depends on whether we're on the first or subsequent lines
        int availableSpaceForText = (lineCount == 0 ? availableTextSpaceFirstLine : availableTextSpaceSubsequentLines);

        currentPosition.x = (lineCount == 0 ? startPositionFirstLine : startPositionSubsequentLines);

        // We may only be looking at part of a child, if we searched back for wrapping
        // and found a suitable point some way into the child. So get the size for the fragment
        // if necessary.

        wxSize childSize;
        int childDescent = 0;
        if (lastEndPos == child->GetRange().GetStart() - 1)
        {
            childSize = child->GetCachedSize();
            childDescent = child->GetDescent();
        }
        else
            GetRangeSize(wxRichTextRange(lastEndPos+1, child->GetRange().GetEnd()), childSize, childDescent, dc, wxRICHTEXT_UNFORMATTED,rect.GetPosition());

        if (childSize.x + currentWidth > availableSpaceForText)
        {
            long wrapPosition = 0;

            // Find a place to wrap. This may walk back to previous children,
            // for example if a word spans several objects.
            if (!FindWrapPosition(wxRichTextRange(lastCompletedEndPos+1, child->GetRange().GetEnd()), dc, availableSpaceForText, wrapPosition))
            {
                // If the function failed, just cut it off at the end of this child.
                wrapPosition = child->GetRange().GetEnd();
            }

            // FindWrapPosition can still return a value that will put us in an endless wrapping loop
            if (wrapPosition <= lastCompletedEndPos)
                wrapPosition = wxMax(lastCompletedEndPos+1,child->GetRange().GetEnd());

            // wxLogDebug(wxT("Split at %ld"), wrapPosition);

            // Let's find the actual size of the current line now
            wxSize actualSize;
            wxRichTextRange actualRange(lastCompletedEndPos+1, wrapPosition);
            GetRangeSize(actualRange, actualSize, childDescent, dc, wxRICHTEXT_UNFORMATTED);
            currentWidth = actualSize.x;
            lineHeight = wxMax(lineHeight, actualSize.y);
            maxDescent = wxMax(childDescent, maxDescent);

            // Add a new line
            wxRichTextLine* line = AllocateLine(lineCount);

            // Set relative range so we won't have to change line ranges when paragraphs are moved
            line->SetRange(wxRichTextRange(actualRange.GetStart() - GetRange().GetStart(), actualRange.GetEnd() - GetRange().GetStart()));
            line->SetPosition(currentPosition);
            line->SetSize(wxSize(currentWidth, lineHeight));
            line->SetDescent(maxDescent);

            // Now move down a line. TODO: add margins, spacing
            currentPosition.y += lineHeight;
            currentPosition.y += lineSpacing;
            currentWidth = 0;
            maxDescent = 0;
            maxWidth = wxMax(maxWidth, currentWidth);

            lineCount ++;

            // TODO: account for zero-length objects, such as fields
            wxASSERT(wrapPosition > lastCompletedEndPos);

            lastEndPos = wrapPosition;
            lastCompletedEndPos = lastEndPos;

            lineHeight = 0;

            // May need to set the node back to a previous one, due to searching back in wrapping
            wxRichTextObject* childAfterWrapPosition = FindObjectAtPosition(wrapPosition+1);
            if (childAfterWrapPosition)
                node = m_children.Find(childAfterWrapPosition);
            else
                node = node->GetNext();
        }
        else
        {
            // We still fit, so don't add a line, and keep going
            currentWidth += childSize.x;
            lineHeight = wxMax(lineHeight, childSize.y);
            maxDescent = wxMax(childDescent, maxDescent);

            maxWidth = wxMax(maxWidth, currentWidth);
            lastEndPos = child->GetRange().GetEnd();

            node = node->GetNext();
        }
    }

    // Add the last line - it's the current pos -> last para pos
    // Substract -1 because the last position is always the end-paragraph position.
    if (lastCompletedEndPos <= GetRange().GetEnd()-1)
    {
        currentPosition.x = (lineCount == 0 ? startPositionFirstLine : startPositionSubsequentLines);

        wxRichTextLine* line = AllocateLine(lineCount);

        wxRichTextRange actualRange(lastCompletedEndPos+1, GetRange().GetEnd()-1);

        // Set relative range so we won't have to change line ranges when paragraphs are moved
        line->SetRange(wxRichTextRange(actualRange.GetStart() - GetRange().GetStart(), actualRange.GetEnd() - GetRange().GetStart()));

        line->SetPosition(currentPosition);

        if (lineHeight == 0)
        {
            if (GetAttributes().GetFont().Ok())
                dc.SetFont(GetAttributes().GetFont());
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
    }

    // Remove remaining unused line objects, if any
    ClearUnusedLines(lineCount);

    // Apply styles to wrapped lines
    ApplyParagraphStyle(rect);

    SetCachedSize(wxSize(maxWidth, currentPosition.y + spaceBeforePara + spaceAfterPara));

    m_dirty = false;

    return true;
}

/// Apply paragraph styles, such as centering, to wrapped lines
void wxRichTextParagraph::ApplyParagraphStyle(const wxRect& rect)
{
    if (!GetAttributes().HasAlignment())
        return;

    wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetFirst();
    while (node)
    {
        wxRichTextLine* line = node->GetData();

        wxPoint pos = line->GetPosition();
        wxSize size = line->GetSize();

        // centering, right-justification
        if (GetAttributes().HasAlignment() && GetAttributes().GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
        {
            pos.x = (rect.GetWidth() - size.x)/2 + pos.x;
            line->SetPosition(pos);
        }
        else if (GetAttributes().HasAlignment() && GetAttributes().GetAlignment() == wxTEXT_ALIGNMENT_RIGHT)
        {
            pos.x = rect.GetRight() - size.x;
            line->SetPosition(pos);
        }

        node = node->GetNext();
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
    wxRichTextBox::Copy(obj);
}

/// Clear the cached lines
void wxRichTextParagraph::ClearLines()
{
    WX_CLEAR_LIST(wxRichTextLineList, m_cachedLines);
}

/// Get/set the object size for the given range. Returns false if the range
/// is invalid for this object.
bool wxRichTextParagraph::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    if (flags & wxRICHTEXT_UNFORMATTED)
    {
        // Just use unformatted data, assume no line breaks
        // TODO: take into account line breaks

        wxSize sz;

        wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
        while (node)
        {
            wxRichTextObject* child = node->GetData();
            if (!child->GetRange().IsOutside(range))
            {
                wxSize childSize;

                wxRichTextRange rangeToUse = range;
                rangeToUse.LimitTo(child->GetRange());
                int childDescent = 0;

                if (child->GetRangeSize(rangeToUse, childSize, childDescent, dc, flags, position))
                {
                    sz.y = wxMax(sz.y, childSize.y);
                    sz.x += childSize.x;
                    descent = wxMax(descent, childDescent);
                }
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

                    if (!child->GetRange().IsOutside(lineRange))
                    {
                        wxRichTextRange rangeToUse = lineRange;
                        rangeToUse.LimitTo(child->GetRange());

                        wxSize childSize;
                        int childDescent = 0;
                        if (child->GetRangeSize(rangeToUse, childSize, childDescent, dc, flags, position))
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

        *height = dc.GetCharHeight();

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
int wxRichTextParagraph::HitTest(wxDC& dc, const wxPoint& pt, long& textPosition)
{
    wxPoint paraPos = GetPosition();

    wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetFirst();
    while (node)
    {
        wxRichTextLine* line = node->GetData();
        wxPoint linePos = paraPos + line->GetPosition();
        wxSize lineSize = line->GetSize();
        wxRichTextRange lineRange = line->GetAbsoluteRange();

        if (pt.y >= linePos.y && pt.y <= linePos.y + lineSize.y)
        {
            if (pt.x < linePos.x)
            {
                textPosition = lineRange.GetStart();
                return wxRICHTEXT_HITTEST_BEFORE;
            }
            else if (pt.x >= (linePos.x + lineSize.x))
            {
                textPosition = lineRange.GetEnd();
                return wxRICHTEXT_HITTEST_AFTER;
            }
            else
            {
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

                        // So now we know it's between i-1 and i.
                        // Let's see if we can be more precise about
                        // which side of the position it's on.

                        int midPoint = (nextX - lastX)/2 + lastX;
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
        if (obj->GetRange().Contains(position))
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
                    return true;
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
                    return true;
            }

            node = node->GetPrevious();
        }
    }

    return true;
}

/// Find a suitable wrap position.
bool wxRichTextParagraph::FindWrapPosition(const wxRichTextRange& range, wxDC& dc, int availableSpace, long& wrapPosition)
{
    // Find the first position where the line exceeds the available space.
    wxSize sz;
    long i;
    long breakPosition = range.GetEnd();
    for (i = range.GetStart(); i <= range.GetEnd(); i++)
    {
        int descent = 0;
        GetRangeSize(wxRichTextRange(range.GetStart(), i), sz, descent, dc, wxRICHTEXT_UNFORMATTED);

        if (sz.x > availableSpace)
        {
            breakPosition = i-1;
            break;
        }
    }

    // Now we know the last position on the line.
    // Let's try to find a word break.

    wxString plainText;
    if (GetContiguousPlainText(plainText, wxRichTextRange(range.GetStart(), breakPosition), false))
    {
        int spacePos = plainText.Find(wxT(' '), true);
        if (spacePos != wxNOT_FOUND)
        {
            int positionsFromEndOfString = plainText.length() - spacePos - 1;
            breakPosition = breakPosition - positionsFromEndOfString;
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
    if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ARABIC)
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
        // TODO: convert from number to roman numeral
        if (number == 1)
            text = wxT("I");
        else if (number == 2)
            text = wxT("II");
        else if (number == 3)
            text = wxT("III");
        else if (number == 4)
            text = wxT("IV");
        else
            text = wxT("TODO");
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER)
    {
        // TODO: convert from number to roman numeral
        if (number == 1)
            text = wxT("i");
        else if (number == 2)
            text = wxT("ii");
        else if (number == 3)
            text = wxT("iii");
        else if (number == 4)
            text = wxT("iv");
        else
            text = wxT("TODO");
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_SYMBOL)
    {
        text = GetAttributes().GetBulletSymbol();
    }

    if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_PARENTHESES)
    {
        text = wxT("(") + text + wxT(")");
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
}

/// Copy
void wxRichTextLine::Copy(const wxRichTextLine& obj)
{
    m_range = obj.m_range;
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

wxRichTextPlainText::wxRichTextPlainText(const wxString& text, wxRichTextObject* parent, wxTextAttrEx* style):
    wxRichTextObject(parent)
{
    if (parent && !style)
        SetAttributes(parent->GetAttributes());
    if (style)
        SetAttributes(*style);

    m_text = text;
}

/// Draw the item
bool wxRichTextPlainText::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int descent, int WXUNUSED(style))
{
    int offset = GetRange().GetStart();

    long len = range.GetLength();
    wxString stringChunk = m_text.Mid(range.GetStart() - offset, (size_t) len);

    int charHeight = dc.GetCharHeight();

    int x = rect.x;
    int y = rect.y + (rect.height - charHeight - (descent - m_descent));

    // Test for the optimized situations where all is selected, or none
    // is selected.

    if (GetAttributes().GetFont().Ok())
        dc.SetFont(GetAttributes().GetFont());

    // (a) All selected.
    if (selectionRange.GetStart() <= range.GetStart() && selectionRange.GetEnd() >= range.GetEnd())
    {/*
        // Draw all selected
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.SetPen(*wxBLACK_PEN);
        wxCoord w, h;
        dc.GetTextExtent(stringChunk, & w, & h);
        wxRect selRect(x, rect.y, w, rect.GetHeight());
        dc.DrawRectangle(selRect);
        dc.SetTextForeground(*wxWHITE);
        dc.SetBackgroundMode(wxTRANSPARENT);
        dc.DrawText(stringChunk, x, y);*/
        DrawTabbedString(dc, rect,stringChunk, x, y, true);
    }
    // (b) None selected.
    else if (selectionRange.GetEnd() < range.GetStart() || selectionRange.GetStart() > range.GetEnd())
    {
        // Draw all unselected
        /*
        dc.SetTextForeground(GetAttributes().GetTextColour());
        dc.SetBackgroundMode(wxTRANSPARENT);
        dc.DrawText(stringChunk, x, y);*/
        DrawTabbedString(dc, rect,stringChunk, x, y, false);
    }
    else
    {
        // (c) Part selected, part not
        // Let's draw unselected chunk, selected chunk, then unselected chunk.

        dc.SetBackgroundMode(wxTRANSPARENT);

        // 1. Initial unselected chunk, if any, up until start of selection.
        if (selectionRange.GetStart() > range.GetStart() && selectionRange.GetStart() <= range.GetEnd())
        {
            int r1 = range.GetStart();
            int s1 = selectionRange.GetStart()-1;
            int fragmentLen = s1 - r1 + 1;
            if (fragmentLen < 0)
                wxLogDebug(wxT("Mid(%d, %d"), (int)(r1 - offset), (int)fragmentLen);
            wxString stringFragment = m_text.Mid(r1 - offset, fragmentLen);
/*
            dc.SetTextForeground(GetAttributes().GetTextColour());
            dc.DrawText(stringFragment, x, y);

            wxCoord w, h;
            dc.GetTextExtent(stringFragment, & w, & h);
            x += w;*/
            DrawTabbedString(dc, rect,stringFragment, x, y, false);
        }

        // 2. Selected chunk, if any.
        if (selectionRange.GetEnd() >= range.GetStart())
        {
            int s1 = wxMax(selectionRange.GetStart(), range.GetStart());
            int s2 = wxMin(selectionRange.GetEnd(), range.GetEnd());

            int fragmentLen = s2 - s1 + 1;
            if (fragmentLen < 0)
                wxLogDebug(wxT("Mid(%d, %d"), (int)(s1 - offset), (int)fragmentLen);
            wxString stringFragment = m_text.Mid(s1 - offset, fragmentLen);
/*
            wxCoord w, h;
            dc.GetTextExtent(stringFragment, & w, & h);
            wxRect selRect(x, rect.y, w, rect.GetHeight());

            dc.SetBrush(*wxBLACK_BRUSH);
            dc.SetPen(*wxBLACK_PEN);
            dc.DrawRectangle(selRect);
            dc.SetTextForeground(*wxWHITE);
            dc.DrawText(stringFragment, x, y);

            x += w;*/
            DrawTabbedString(dc, rect,stringFragment, x, y, true);
        }

        // 3. Remaining unselected chunk, if any
        if (selectionRange.GetEnd() < range.GetEnd())
        {
            int s2 = wxMin(selectionRange.GetEnd()+1, range.GetEnd());
            int r2 = range.GetEnd();

            int fragmentLen = r2 - s2 + 1;
            if (fragmentLen < 0)
                wxLogDebug(wxT("Mid(%d, %d"), (int)(s2 - offset), (int)fragmentLen);
            wxString stringFragment = m_text.Mid(s2 - offset, fragmentLen);
/*
            dc.SetTextForeground(GetAttributes().GetTextColour());
            dc.DrawText(stringFragment, x, y);*/
            DrawTabbedString(dc, rect,stringFragment, x, y, false);
        }
    }

    return true;
}

bool wxRichTextPlainText::DrawTabbedString(wxDC& dc,const wxRect& rect,wxString& str, wxCoord& x, wxCoord& y, bool selected)
{
    wxArrayInt tab_array =  GetAttributes().GetTabs();
    if(tab_array.IsEmpty()){// create a default tab list at 10 mm each.
        for( int i = 0; i < 20; ++i){
            tab_array.Add(i*100);
        }
    }
    int map_mode = dc.GetMapMode();
    dc.SetMapMode(wxMM_LOMETRIC );
    int num_tabs = tab_array.GetCount();
    for( int i = 0; i < num_tabs; ++i){
       tab_array[i] = dc.LogicalToDeviceXRel(tab_array[i]);
    }
    dc.SetMapMode(map_mode );
    int next_tab_pos = -1;
    int tab_pos = -1;
    wxCoord w, h;
    if(selected){
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.SetPen(*wxBLACK_PEN);
        dc.SetTextForeground(*wxWHITE);
        dc.SetBackgroundMode(wxTRANSPARENT);
    }
    else{
        dc.SetTextForeground(GetAttributes().GetTextColour());
        dc.SetBackgroundMode(wxTRANSPARENT);
    }
    while(str.Find(wxT('\t')) >= 0){// the string has a tab
        // break up the string at the Tab
        wxString stringChunk = str.BeforeFirst(wxT('\t'));
        str = str.AfterFirst(wxT('\t'));
        dc.GetTextExtent(stringChunk, & w, & h);
        tab_pos = x + w;
        bool not_found = true;
        for( int i = 0; i < num_tabs && not_found; ++i){
            next_tab_pos = tab_array.Item(i);
            if( next_tab_pos > tab_pos){
                not_found = false;
                if(selected){
                    w = next_tab_pos - x;
                    wxRect selRect(x, rect.y, w, rect.GetHeight());
                    dc.DrawRectangle(selRect);
                }
                dc.DrawText(stringChunk, x, y);
                x = next_tab_pos;
            }
        }
    }

    dc.GetTextExtent(str, & w, & h);
    if(selected){
        wxRect selRect(x, rect.y, w, rect.GetHeight());
        dc.DrawRectangle(selRect);
    }
    dc.DrawText(str, x, y);
    x += w;
    return true;

}
/// Lay the item out
bool wxRichTextPlainText::Layout(wxDC& dc, const wxRect& WXUNUSED(rect), int WXUNUSED(style))
{
    if (GetAttributes().GetFont().Ok())
        dc.SetFont(GetAttributes().GetFont());

    wxCoord w, h;
    dc.GetTextExtent(m_text, & w, & h, & m_descent);
    m_size = wxSize(w, dc.GetCharHeight());

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
bool wxRichTextPlainText::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int WXUNUSED(flags), wxPoint position) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    // Always assume unformatted text, since at this level we have no knowledge
    // of line breaks - and we don't need it, since we'll calculate size within
    // formatted text by doing it in chunks according to the line ranges

    if (GetAttributes().GetFont().Ok())
        dc.SetFont(GetAttributes().GetFont());

    int startPos = range.GetStart() - GetRange().GetStart();
    long len = range.GetLength();
    wxString stringChunk = m_text.Mid(startPos, (size_t) len);
    wxCoord w, h;
    int width = 0;
    if(stringChunk.Find(wxT('\t')) >= 0){// the string has a tab
        wxArrayInt tab_array =  GetAttributes().GetTabs();
        if(tab_array.IsEmpty())
        {
            // create a default tab list at 10 mm each.
            for( int i = 0; i < 20; ++i)
            {
                tab_array.Add(i*100);
            }
        }
        int map_mode = dc.GetMapMode();
        dc.SetMapMode(wxMM_LOMETRIC );
        int num_tabs = tab_array.GetCount();
        for( int i = 0; i < num_tabs; ++i)
        {
            tab_array[i] = dc.LogicalToDeviceXRel(tab_array[i]);
        }
        dc.SetMapMode(map_mode );
        int next_tab_pos = -1;

        while(stringChunk.Find(wxT('\t')) >= 0){// the string has a tab
            // break up the string at the Tab
            wxString stringFragment = stringChunk.BeforeFirst(wxT('\t'));
            stringChunk = stringChunk.AfterFirst(wxT('\t'));
            dc.GetTextExtent(stringFragment, & w, & h);
            width += w;
            int absolute_width = width + position.x;
            bool not_found = true;
            for( int i = 0; i < num_tabs && not_found; ++i){
                next_tab_pos = tab_array.Item(i);
                if( next_tab_pos > absolute_width){
                    not_found = false;
                    width = next_tab_pos - position.x;
                }
            }
        }
    }
    dc.GetTextExtent(stringChunk, & w, & h, & descent);
    width += w;
    size = wxSize(width, dc.GetCharHeight());

    return true;
}

/// Do a split, returning an object containing the second part, and setting
/// the first part in 'this'.
wxRichTextObject* wxRichTextPlainText::DoSplit(long pos)
{
    int index = pos - GetRange().GetStart();
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

/*!
 * wxRichTextBuffer
 * This is a kind of box, used to represent the whole buffer
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextBuffer, wxRichTextParagraphLayoutBox)

wxList wxRichTextBuffer::sm_handlers;

/// Initialisation
void wxRichTextBuffer::Init()
{
    m_commandProcessor = new wxCommandProcessor;
    m_styleSheet = NULL;
    m_modified = false;
    m_batchedCommandDepth = 0;
    m_batchedCommand = NULL;
    m_suppressUndo = 0;
}

/// Initialisation
wxRichTextBuffer::~wxRichTextBuffer()
{
    delete m_commandProcessor;
    delete m_batchedCommand;

    ClearStyleStack();
}

void wxRichTextBuffer::Clear()
{
    DeleteChildren();
    GetCommandProcessor()->ClearCommands();
    Modify(false);
    Invalidate(wxRICHTEXT_ALL);
}

void wxRichTextBuffer::Reset()
{
    DeleteChildren();
    AddParagraph(wxEmptyString);
    GetCommandProcessor()->ClearCommands();
    Modify(false);
    Invalidate(wxRICHTEXT_ALL);
}

/// Submit command to insert the given text
bool wxRichTextBuffer::InsertTextWithUndo(long pos, const wxString& text, wxRichTextCtrl* ctrl)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Text"), wxRICHTEXT_INSERT, this, ctrl, false);

    action->GetNewParagraphs().AddParagraphs(text);
    if (action->GetNewParagraphs().GetChildCount() == 1)
        action->GetNewParagraphs().SetPartialParagraph(true);

    action->SetPosition(pos);

    // Set the range we'll need to delete in Undo
    action->SetRange(wxRichTextRange(pos, pos + text.length() - 1));

    SubmitAction(action);

    return true;
}

/// Submit command to insert the given text
bool wxRichTextBuffer::InsertNewlineWithUndo(long pos, wxRichTextCtrl* ctrl)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Text"), wxRICHTEXT_INSERT, this, ctrl, false);

    wxTextAttrEx attr(GetBasicStyle());
    wxRichTextApplyStyle(attr, GetDefaultStyle());

    wxRichTextParagraph* newPara = new wxRichTextParagraph(wxEmptyString, this, & attr);
    action->GetNewParagraphs().AppendChild(newPara);
    action->GetNewParagraphs().UpdateRanges();
    action->GetNewParagraphs().SetPartialParagraph(false);
    action->SetPosition(pos);

    // Set the range we'll need to delete in Undo
    action->SetRange(wxRichTextRange(pos, pos));

    SubmitAction(action);

    return true;
}

/// Submit command to insert the given image
bool wxRichTextBuffer::InsertImageWithUndo(long pos, const wxRichTextImageBlock& imageBlock, wxRichTextCtrl* ctrl)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Image"), wxRICHTEXT_INSERT, this, ctrl, false);

    wxTextAttrEx attr(GetBasicStyle());
    wxRichTextApplyStyle(attr, GetDefaultStyle());

    wxRichTextParagraph* newPara = new wxRichTextParagraph(this, & attr);
    wxRichTextImage* imageObject = new wxRichTextImage(imageBlock, newPara);
    newPara->AppendChild(imageObject);
    action->GetNewParagraphs().AppendChild(newPara);
    action->GetNewParagraphs().UpdateRanges();

    action->GetNewParagraphs().SetPartialParagraph(true);

    action->SetPosition(pos);

    // Set the range we'll need to delete in Undo
    action->SetRange(wxRichTextRange(pos, pos));

    SubmitAction(action);

    return true;
}

/// Submit command to delete this range
bool wxRichTextBuffer::DeleteRangeWithUndo(const wxRichTextRange& range, long initialCaretPosition, long WXUNUSED(newCaretPositon), wxRichTextCtrl* ctrl)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Delete"), wxRICHTEXT_DELETE, this, ctrl);

    action->SetPosition(initialCaretPosition);

    // Set the range to delete
    action->SetRange(range);

    // Copy the fragment that we'll need to restore in Undo
    CopyFragment(range, action->GetOldParagraphs());

    // Special case: if there is only one (non-partial) paragraph,
    // we must save the *next* paragraph's style, because that
    // is the style we must apply when inserting the content back
    // when undoing the delete. (This is because we're merging the
    // paragraph with the previous paragraph and throwing away
    // the style, and we need to restore it.)
    if (!action->GetOldParagraphs().GetPartialParagraph() && action->GetOldParagraphs().GetChildCount() == 1)
    {
        wxRichTextParagraph* lastPara = GetParagraphAtPosition(range.GetStart());
        if (lastPara)
        {
            wxRichTextParagraph* nextPara = GetParagraphAtPosition(range.GetEnd()+1);
            if (nextPara)
            {
                wxRichTextParagraph* para = (wxRichTextParagraph*) action->GetOldParagraphs().GetChild(0);
                para->SetAttributes(nextPara->GetAttributes());
            }
        }
    }

    SubmitAction(action);

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
            GetCommandProcessor()->Submit(m_batchedCommand);
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
        GetCommandProcessor()->Submit(m_batchedCommand);
        m_batchedCommand = NULL;
    }

    return true;
}

/// Submit immediately, or delay according to whether collapsing is on
bool wxRichTextBuffer::SubmitAction(wxRichTextAction* action)
{
    if (BatchingUndo() && m_batchedCommand && !SuppressingUndo())
        m_batchedCommand->AddAction(action);
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
bool wxRichTextBuffer::BeginStyle(const wxTextAttrEx& style)
{
    wxTextAttrEx newStyle(GetDefaultStyle());

    // Save the old default style
    m_attributeStack.Append((wxObject*) new wxTextAttrEx(GetDefaultStyle()));

    wxRichTextApplyStyle(newStyle, style);
    newStyle.SetFlags(style.GetFlags()|newStyle.GetFlags());

    SetDefaultStyle(newStyle);

    // wxLogDebug("Default style size = %d", GetDefaultStyle().GetFont().GetPointSize());

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
    wxTextAttrEx* attr = (wxTextAttrEx*)node->GetData();
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
        delete (wxTextAttrEx*) node->GetData();
    m_attributeStack.Clear();
}

/// Begin using bold
bool wxRichTextBuffer::BeginBold()
{
    wxFont font(GetBasicStyle().GetFont());
    font.SetWeight(wxBOLD);

    wxTextAttrEx attr;
    attr.SetFont(font,wxTEXT_ATTR_FONT_WEIGHT);

    return BeginStyle(attr);
}

/// Begin using italic
bool wxRichTextBuffer::BeginItalic()
{
    wxFont font(GetBasicStyle().GetFont());
    font.SetStyle(wxITALIC);

    wxTextAttrEx attr;
    attr.SetFont(font, wxTEXT_ATTR_FONT_ITALIC);

    return BeginStyle(attr);
}

/// Begin using underline
bool wxRichTextBuffer::BeginUnderline()
{
    wxFont font(GetBasicStyle().GetFont());
    font.SetUnderlined(true);

    wxTextAttrEx attr;
    attr.SetFont(font, wxTEXT_ATTR_FONT_UNDERLINE);

    return BeginStyle(attr);
}

/// Begin using point size
bool wxRichTextBuffer::BeginFontSize(int pointSize)
{
    wxFont font(GetBasicStyle().GetFont());
    font.SetPointSize(pointSize);

    wxTextAttrEx attr;
    attr.SetFont(font, wxTEXT_ATTR_FONT_SIZE);

    return BeginStyle(attr);
}

/// Begin using this font
bool wxRichTextBuffer::BeginFont(const wxFont& font)
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_FONT);
    attr.SetFont(font);

    return BeginStyle(attr);
}

/// Begin using this colour
bool wxRichTextBuffer::BeginTextColour(const wxColour& colour)
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_TEXT_COLOUR);
    attr.SetTextColour(colour);

    return BeginStyle(attr);
}

/// Begin using alignment
bool wxRichTextBuffer::BeginAlignment(wxTextAttrAlignment alignment)
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_ALIGNMENT);
    attr.SetAlignment(alignment);

    return BeginStyle(attr);
}

/// Begin left indent
bool wxRichTextBuffer::BeginLeftIndent(int leftIndent, int leftSubIndent)
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_LEFT_INDENT);
    attr.SetLeftIndent(leftIndent, leftSubIndent);

    return BeginStyle(attr);
}

/// Begin right indent
bool wxRichTextBuffer::BeginRightIndent(int rightIndent)
{
    wxTextAttrEx attr;
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

    wxTextAttrEx attr;
    attr.SetFlags(flags);
    attr.SetParagraphSpacingBefore(before);
    attr.SetParagraphSpacingAfter(after);

    return BeginStyle(attr);
}

/// Begin line spacing
bool wxRichTextBuffer::BeginLineSpacing(int lineSpacing)
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);
    attr.SetLineSpacing(lineSpacing);

    return BeginStyle(attr);
}

/// Begin numbered bullet
bool wxRichTextBuffer::BeginNumberedBullet(int bulletNumber, int leftIndent, int leftSubIndent, int bulletStyle)
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_NUMBER|wxTEXT_ATTR_LEFT_INDENT);
    attr.SetBulletStyle(bulletStyle);
    attr.SetBulletNumber(bulletNumber);
    attr.SetLeftIndent(leftIndent, leftSubIndent);

    return BeginStyle(attr);
}

/// Begin symbol bullet
bool wxRichTextBuffer::BeginSymbolBullet(wxChar symbol, int leftIndent, int leftSubIndent, int bulletStyle)
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_SYMBOL|wxTEXT_ATTR_LEFT_INDENT);
    attr.SetBulletStyle(bulletStyle);
    attr.SetLeftIndent(leftIndent, leftSubIndent);
    attr.SetBulletSymbol(symbol);

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
            wxTextAttrEx attr;
            def->GetStyle().CopyTo(attr);
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
            wxTextAttrEx attr;
            def->GetStyle().CopyTo(attr);
            return BeginStyle(attr);
        }
    }
    return false;
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
wxRichTextFileHandler *wxRichTextBuffer::FindHandlerFilenameOrType(const wxString& filename, int imageType)
{
    if (imageType != wxRICHTEXT_TYPE_ANY)
        return FindHandler(imageType);
    else
    {
        wxString path, file, ext;
        wxSplitPath(filename, & path, & file, & ext);
        return FindHandler(ext, imageType);
    }
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
wxRichTextFileHandler* wxRichTextBuffer::FindHandler(const wxString& extension, int type)
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
wxRichTextFileHandler* wxRichTextBuffer::FindHandler(int type)
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
        if (handler->IsVisible() && ((save && handler->CanSave()) || !save && handler->CanLoad()))
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
bool wxRichTextBuffer::LoadFile(const wxString& filename, int type)
{
    wxRichTextFileHandler* handler = FindHandlerFilenameOrType(filename, type);
    if (handler)
    {
        SetDefaultStyle(wxTextAttrEx());

        bool success = handler->LoadFile(this, filename);
        Invalidate(wxRICHTEXT_ALL);
        return success;
    }
    else
        return false;
}

/// Save a file
bool wxRichTextBuffer::SaveFile(const wxString& filename, int type)
{
    wxRichTextFileHandler* handler = FindHandlerFilenameOrType(filename, type);
    if (handler)
        return handler->SaveFile(this, filename);
    else
        return false;
}

/// Load from a stream
bool wxRichTextBuffer::LoadFile(wxInputStream& stream, int type)
{
    wxRichTextFileHandler* handler = FindHandler(type);
    if (handler)
    {
        SetDefaultStyle(wxTextAttrEx());
        bool success = handler->LoadFile(this, stream);
        Invalidate(wxRICHTEXT_ALL);
        return success;
    }
    else
        return false;
}

/// Save to a stream
bool wxRichTextBuffer::SaveFile(wxOutputStream& stream, int type)
{
    wxRichTextFileHandler* handler = FindHandler(type);
    if (handler)
        return handler->SaveFile(this, stream);
    else
        return false;
}

/// Copy the range to the clipboard
bool wxRichTextBuffer::CopyToClipboard(const wxRichTextRange& range)
{
    bool success = false;
#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ
    wxString text = GetTextForRange(range);
    if (!wxTheClipboard->IsOpened() && wxTheClipboard->Open())
    {
        success = wxTheClipboard->SetData(new wxTextDataObject(text));
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
#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ
    if (CanPasteFromClipboard())
    {
        if (wxTheClipboard->Open())
        {
            if (wxTheClipboard->IsSupported(wxDF_TEXT))
            {
                wxTextDataObject data;
                wxTheClipboard->GetData(data);
                wxString text(data.GetText());
                text.Replace(_T("\r\n"), _T("\n"));

                InsertTextWithUndo(position+1, text, GetRichTextCtrl());

                success = true;
            }
            else if (wxTheClipboard->IsSupported(wxDF_BITMAP))
            {
                wxBitmapDataObject data;
                wxTheClipboard->GetData(data);
                wxBitmap bitmap(data.GetBitmap());
                wxImage image(bitmap.ConvertToImage());

                wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Image"), wxRICHTEXT_INSERT, this, GetRichTextCtrl(), false);

                action->GetNewParagraphs().AddImage(image);

                if (action->GetNewParagraphs().GetChildCount() == 1)
                    action->GetNewParagraphs().SetPartialParagraph(true);

                action->SetPosition(position);

                // Set the range we'll need to delete in Undo
                action->SetRange(wxRichTextRange(position, position));

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
        if (wxTheClipboard->IsSupported(wxDF_TEXT) || wxTheClipboard->IsSupported(wxDF_BITMAP))
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


/*
 * Module to initialise and clean up handlers
 */

class wxRichTextModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxRichTextModule)
public:
    wxRichTextModule() {}
    bool OnInit() { wxRichTextBuffer::InitStandardHandlers(); return true; };
    void OnExit() { wxRichTextBuffer::CleanUpHandlers(); };
};

IMPLEMENT_DYNAMIC_CLASS(wxRichTextModule, wxModule)


/*!
 * Commands for undo/redo
 *
 */

wxRichTextCommand::wxRichTextCommand(const wxString& name, wxRichTextCommandId id, wxRichTextBuffer* buffer,
                                     wxRichTextCtrl* ctrl, bool ignoreFirstTime): wxCommand(true, name)
{
    /* wxRichTextAction* action = */ new wxRichTextAction(this, name, id, buffer, ctrl, ignoreFirstTime);
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

wxRichTextAction::wxRichTextAction(wxRichTextCommand* cmd, const wxString& name, wxRichTextCommandId id, wxRichTextBuffer* buffer,
                                     wxRichTextCtrl* ctrl, bool ignoreFirstTime)
{
    m_buffer = buffer;
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
}

bool wxRichTextAction::Do()
{
    m_buffer->Modify(true);

    switch (m_cmdId)
    {
    case wxRICHTEXT_INSERT:
        {
            m_buffer->InsertFragment(GetPosition(), m_newParagraphs);
            m_buffer->UpdateRanges();
            m_buffer->Invalidate(GetRange());

            long newCaretPosition = GetPosition() + m_newParagraphs.GetRange().GetLength() - 1;
            if (m_newParagraphs.GetPartialParagraph())
                newCaretPosition --;

            UpdateAppearance(newCaretPosition, true /* send update event */);

            break;
        }
    case wxRICHTEXT_DELETE:
        {
            m_buffer->DeleteRange(GetRange());
            m_buffer->UpdateRanges();
            m_buffer->Invalidate(wxRichTextRange(GetRange().GetStart(), GetRange().GetStart()));

            UpdateAppearance(GetRange().GetStart()-1, true /* send update event */);

            break;
        }
    case wxRICHTEXT_CHANGE_STYLE:
        {
            ApplyParagraphs(GetNewParagraphs());
            m_buffer->Invalidate(GetRange());

            UpdateAppearance(GetPosition());

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

    switch (m_cmdId)
    {
    case wxRICHTEXT_INSERT:
        {
            m_buffer->DeleteRange(GetRange());
            m_buffer->UpdateRanges();
            m_buffer->Invalidate(wxRichTextRange(GetRange().GetStart(), GetRange().GetStart()));

            long newCaretPosition = GetPosition() - 1;
            // if (m_newParagraphs.GetPartialParagraph())
            //    newCaretPosition --;

            UpdateAppearance(newCaretPosition, true /* send update event */);

            break;
        }
    case wxRICHTEXT_DELETE:
        {
            m_buffer->InsertFragment(GetRange().GetStart(), m_oldParagraphs);
            m_buffer->UpdateRanges();
            m_buffer->Invalidate(GetRange());

            UpdateAppearance(GetPosition(), true /* send update event */);

            break;
        }
    case wxRICHTEXT_CHANGE_STYLE:
        {
            ApplyParagraphs(GetOldParagraphs());
            m_buffer->Invalidate(GetRange());

            UpdateAppearance(GetPosition());

            break;
        }
    default:
        break;
    }

    return true;
}

/// Update the control appearance
void wxRichTextAction::UpdateAppearance(long caretPosition, bool sendUpdateEvent)
{
    if (m_ctrl)
    {
        m_ctrl->SetCaretPosition(caretPosition);
        if (!m_ctrl->IsFrozen())
        {
            m_ctrl->LayoutContent();
            m_ctrl->PositionCaret();
            m_ctrl->Refresh(false);

            if (sendUpdateEvent)
                m_ctrl->SendUpdateEvent();
        }
    }
}

/// Replace the buffer paragraphs with the new ones.
void wxRichTextAction::ApplyParagraphs(const wxRichTextFragment& fragment)
{
    wxRichTextObjectList::compatibility_iterator node = fragment.GetChildren().GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

        // We'll replace the existing paragraph by finding the paragraph at this position,
        // delete its node data, and setting a copy as the new node data.
        // TODO: make more efficient by simply swapping old and new paragraph objects.

        wxRichTextParagraph* existingPara = m_buffer->GetParagraphAtPosition(para->GetRange().GetStart());
        if (existingPara)
        {
            wxRichTextObjectList::compatibility_iterator bufferParaNode = m_buffer->GetChildren().Find(existingPara);
            if (bufferParaNode)
            {
                wxRichTextParagraph* newPara = new wxRichTextParagraph(*para);
                newPara->SetParent(m_buffer);

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

wxRichTextImage::wxRichTextImage(const wxImage& image, wxRichTextObject* parent):
    wxRichTextObject(parent)
{
    m_image = image;
}

wxRichTextImage::wxRichTextImage(const wxRichTextImageBlock& imageBlock, wxRichTextObject* parent):
    wxRichTextObject(parent)
{
    m_imageBlock = imageBlock;
    m_imageBlock.Load(m_image);
}

/// Load wxImage from the block
bool wxRichTextImage::LoadFromBlock()
{
    m_imageBlock.Load(m_image);
    return m_imageBlock.Ok();
}

/// Make block from the wxImage
bool wxRichTextImage::MakeBlock()
{
    if (m_imageBlock.GetImageType() == wxBITMAP_TYPE_ANY || m_imageBlock.GetImageType() == -1)
        m_imageBlock.SetImageType(wxBITMAP_TYPE_PNG);

    m_imageBlock.MakeImageBlock(m_image, m_imageBlock.GetImageType());
    return m_imageBlock.Ok();
}


/// Draw the item
bool wxRichTextImage::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int WXUNUSED(descent), int WXUNUSED(style))
{
    if (!m_image.Ok() && m_imageBlock.Ok())
        LoadFromBlock();

    if (!m_image.Ok())
        return false;

    if (m_image.Ok() && !m_bitmap.Ok())
        m_bitmap = wxBitmap(m_image);

    int y = rect.y + (rect.height - m_image.GetHeight());

    if (m_bitmap.Ok())
        dc.DrawBitmap(m_bitmap, rect.x, y, true);

    if (selectionRange.Contains(range.GetStart()))
    {
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.SetPen(*wxBLACK_PEN);
        dc.SetLogicalFunction(wxINVERT);
        dc.DrawRectangle(rect);
        dc.SetLogicalFunction(wxCOPY);
    }

    return true;
}

/// Lay the item out
bool wxRichTextImage::Layout(wxDC& WXUNUSED(dc), const wxRect& rect, int WXUNUSED(style))
{
    if (!m_image.Ok())
        LoadFromBlock();

    if (m_image.Ok())
    {
        SetCachedSize(wxSize(m_image.GetWidth(), m_image.GetHeight()));
        SetPosition(rect.GetPosition());
    }

    return true;
}

/// Get/set the object size for the given range. Returns false if the range
/// is invalid for this object.
bool wxRichTextImage::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& WXUNUSED(descent), wxDC& WXUNUSED(dc), int WXUNUSED(flags), wxPoint WXUNUSED(position)) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    if (!m_image.Ok())
        return false;

    size.x = m_image.GetWidth();
    size.y = m_image.GetHeight();

    return true;
}

/// Copy
void wxRichTextImage::Copy(const wxRichTextImage& obj)
{
    m_image = obj.m_image;
    m_imageBlock = obj.m_imageBlock;
}

/*!
 * Utilities
 *
 */

/// Compare two attribute objects
bool wxTextAttrEq(const wxTextAttrEx& attr1, const wxTextAttrEx& attr2)
{
    return (
        attr1.GetTextColour() == attr2.GetTextColour() &&
        attr1.GetBackgroundColour() == attr2.GetBackgroundColour() &&
        attr1.GetFont() == attr2.GetFont() &&
        attr1.GetAlignment() == attr2.GetAlignment() &&
        attr1.GetLeftIndent() == attr2.GetLeftIndent() &&
        attr1.GetRightIndent() == attr2.GetRightIndent() &&
        attr1.GetLeftSubIndent() == attr2.GetLeftSubIndent() &&
        attr1.GetTabs().GetCount() == attr2.GetTabs().GetCount() && // heuristic
        attr1.GetLineSpacing() == attr2.GetLineSpacing() &&
        attr1.GetParagraphSpacingAfter() == attr2.GetParagraphSpacingAfter() &&
        attr1.GetParagraphSpacingBefore() == attr2.GetParagraphSpacingBefore() &&
        attr1.GetBulletStyle() == attr2.GetBulletStyle() &&
        attr1.GetBulletNumber() == attr2.GetBulletNumber() &&
        attr1.GetBulletSymbol() == attr2.GetBulletSymbol() &&
        attr1.GetCharacterStyleName() == attr2.GetCharacterStyleName() &&
        attr1.GetParagraphStyleName() == attr2.GetParagraphStyleName());
}

bool wxTextAttrEq(const wxTextAttrEx& attr1, const wxRichTextAttr& attr2)
{
    return (
        attr1.GetTextColour() == attr2.GetTextColour() &&
        attr1.GetBackgroundColour() == attr2.GetBackgroundColour() &&
        attr1.GetFont().GetPointSize() == attr2.GetFontSize() &&
        attr1.GetFont().GetStyle() == attr2.GetFontStyle() &&
        attr1.GetFont().GetWeight() == attr2.GetFontWeight() &&
        attr1.GetFont().GetFaceName() == attr2.GetFontFaceName() &&
        attr1.GetFont().GetUnderlined() == attr2.GetFontUnderlined() &&
        attr1.GetAlignment() == attr2.GetAlignment() &&
        attr1.GetLeftIndent() == attr2.GetLeftIndent() &&
        attr1.GetRightIndent() == attr2.GetRightIndent() &&
        attr1.GetLeftSubIndent() == attr2.GetLeftSubIndent() &&
        attr1.GetTabs().GetCount() == attr2.GetTabs().GetCount() && // heuristic
        attr1.GetLineSpacing() == attr2.GetLineSpacing() &&
        attr1.GetParagraphSpacingAfter() == attr2.GetParagraphSpacingAfter() &&
        attr1.GetParagraphSpacingBefore() == attr2.GetParagraphSpacingBefore() &&
        attr1.GetBulletStyle() == attr2.GetBulletStyle() &&
        attr1.GetBulletNumber() == attr2.GetBulletNumber() &&
        attr1.GetBulletSymbol() == attr2.GetBulletSymbol() &&
        attr1.GetCharacterStyleName() == attr2.GetCharacterStyleName() &&
        attr1.GetParagraphStyleName() == attr2.GetParagraphStyleName());
}

/// Compare two attribute objects, but take into account the flags
/// specifying attributes of interest.
bool wxTextAttrEqPartial(const wxTextAttrEx& attr1, const wxTextAttrEx& attr2, int flags)
{
    if ((flags & wxTEXT_ATTR_TEXT_COLOUR) && attr1.GetTextColour() != attr2.GetTextColour())
        return false;

    if ((flags & wxTEXT_ATTR_BACKGROUND_COLOUR) && attr1.GetBackgroundColour() != attr2.GetBackgroundColour())
        return false;

    if ((flags & wxTEXT_ATTR_FONT_FACE) && attr1.GetFont().Ok() && attr2.GetFont().Ok() &&
        attr1.GetFont().GetFaceName() != attr2.GetFont().GetFaceName())
        return false;

    if ((flags & wxTEXT_ATTR_FONT_SIZE) && attr1.GetFont().Ok() && attr2.GetFont().Ok() &&
        attr1.GetFont().GetPointSize() != attr2.GetFont().GetPointSize())
        return false;

    if ((flags & wxTEXT_ATTR_FONT_WEIGHT) && attr1.GetFont().Ok() && attr2.GetFont().Ok() &&
        attr1.GetFont().GetWeight() != attr2.GetFont().GetWeight())
        return false;

    if ((flags & wxTEXT_ATTR_FONT_ITALIC) && attr1.GetFont().Ok() && attr2.GetFont().Ok() &&
        attr1.GetFont().GetStyle() != attr2.GetFont().GetStyle())
        return false;

    if ((flags & wxTEXT_ATTR_FONT_UNDERLINE) && attr1.GetFont().Ok() && attr2.GetFont().Ok() &&
        attr1.GetFont().GetUnderlined() != attr2.GetFont().GetUnderlined())
        return false;

    if ((flags & wxTEXT_ATTR_ALIGNMENT) && attr1.GetAlignment() != attr2.GetAlignment())
        return false;

    if ((flags & wxTEXT_ATTR_LEFT_INDENT) &&
        ((attr1.GetLeftIndent() != attr2.GetLeftIndent()) || (attr1.GetLeftSubIndent() != attr2.GetLeftSubIndent())))
        return false;

    if ((flags & wxTEXT_ATTR_RIGHT_INDENT) &&
        (attr1.GetRightIndent() != attr2.GetRightIndent()))
        return false;

    if ((flags & wxTEXT_ATTR_PARA_SPACING_AFTER) &&
        (attr1.GetParagraphSpacingAfter() != attr2.GetParagraphSpacingAfter()))
        return false;

    if ((flags & wxTEXT_ATTR_PARA_SPACING_BEFORE) &&
        (attr1.GetParagraphSpacingBefore() != attr2.GetParagraphSpacingBefore()))
        return false;

    if ((flags & wxTEXT_ATTR_LINE_SPACING) &&
        (attr1.GetLineSpacing() != attr2.GetLineSpacing()))
        return false;

    if ((flags & wxTEXT_ATTR_CHARACTER_STYLE_NAME) &&
        (attr1.GetCharacterStyleName() != attr2.GetCharacterStyleName()))
        return false;

    if ((flags & wxTEXT_ATTR_PARAGRAPH_STYLE_NAME) &&
        (attr1.GetParagraphStyleName() != attr2.GetParagraphStyleName()))
        return false;

    if ((flags & wxTEXT_ATTR_BULLET_STYLE) &&
        (attr1.GetBulletStyle() != attr2.GetBulletStyle()))
         return false;

    if ((flags & wxTEXT_ATTR_BULLET_NUMBER) &&
        (attr1.GetBulletNumber() != attr2.GetBulletNumber()))
         return false;

    if ((flags & wxTEXT_ATTR_BULLET_SYMBOL) &&
        (attr1.GetBulletSymbol() != attr2.GetBulletSymbol()))
         return false;

/* TODO
    if ((flags & wxTEXT_ATTR_TABS) &&
        return false;
*/

    return true;
}

bool wxTextAttrEqPartial(const wxTextAttrEx& attr1, const wxRichTextAttr& attr2, int flags)
{
    if ((flags & wxTEXT_ATTR_TEXT_COLOUR) && attr1.GetTextColour() != attr2.GetTextColour())
        return false;

    if ((flags & wxTEXT_ATTR_BACKGROUND_COLOUR) && attr1.GetBackgroundColour() != attr2.GetBackgroundColour())
        return false;

    if ((flags & (wxTEXT_ATTR_FONT)) && !attr1.GetFont().Ok())
        return false;

    if ((flags & wxTEXT_ATTR_FONT_FACE) && attr1.GetFont().Ok() &&
        attr1.GetFont().GetFaceName() != attr2.GetFontFaceName())
        return false;

    if ((flags & wxTEXT_ATTR_FONT_SIZE) && attr1.GetFont().Ok() &&
        attr1.GetFont().GetPointSize() != attr2.GetFontSize())
        return false;

    if ((flags & wxTEXT_ATTR_FONT_WEIGHT) && attr1.GetFont().Ok() &&
        attr1.GetFont().GetWeight() != attr2.GetFontWeight())
        return false;

    if ((flags & wxTEXT_ATTR_FONT_ITALIC) && attr1.GetFont().Ok() &&
        attr1.GetFont().GetStyle() != attr2.GetFontStyle())
        return false;

    if ((flags & wxTEXT_ATTR_FONT_UNDERLINE) && attr1.GetFont().Ok() &&
        attr1.GetFont().GetUnderlined() != attr2.GetFontUnderlined())
        return false;

    if ((flags & wxTEXT_ATTR_ALIGNMENT) && attr1.GetAlignment() != attr2.GetAlignment())
        return false;

    if ((flags & wxTEXT_ATTR_LEFT_INDENT) &&
        ((attr1.GetLeftIndent() != attr2.GetLeftIndent()) || (attr1.GetLeftSubIndent() != attr2.GetLeftSubIndent())))
        return false;

    if ((flags & wxTEXT_ATTR_RIGHT_INDENT) &&
        (attr1.GetRightIndent() != attr2.GetRightIndent()))
        return false;

    if ((flags & wxTEXT_ATTR_PARA_SPACING_AFTER) &&
        (attr1.GetParagraphSpacingAfter() != attr2.GetParagraphSpacingAfter()))
        return false;

    if ((flags & wxTEXT_ATTR_PARA_SPACING_BEFORE) &&
        (attr1.GetParagraphSpacingBefore() != attr2.GetParagraphSpacingBefore()))
        return false;

    if ((flags & wxTEXT_ATTR_LINE_SPACING) &&
        (attr1.GetLineSpacing() != attr2.GetLineSpacing()))
        return false;

    if ((flags & wxTEXT_ATTR_CHARACTER_STYLE_NAME) &&
        (attr1.GetCharacterStyleName() != attr2.GetCharacterStyleName()))
        return false;

    if ((flags & wxTEXT_ATTR_PARAGRAPH_STYLE_NAME) &&
        (attr1.GetParagraphStyleName() != attr2.GetParagraphStyleName()))
        return false;

    if ((flags & wxTEXT_ATTR_BULLET_STYLE) &&
        (attr1.GetBulletStyle() != attr2.GetBulletStyle()))
         return false;

    if ((flags & wxTEXT_ATTR_BULLET_NUMBER) &&
        (attr1.GetBulletNumber() != attr2.GetBulletNumber()))
         return false;

    if ((flags & wxTEXT_ATTR_BULLET_SYMBOL) &&
        (attr1.GetBulletSymbol() != attr2.GetBulletSymbol()))
         return false;

/* TODO
    if ((flags & wxTEXT_ATTR_TABS) &&
        return false;
*/

    return true;
}


/// Apply one style to another
bool wxRichTextApplyStyle(wxTextAttrEx& destStyle, const wxTextAttrEx& style)
{
    // Whole font
    if (style.GetFont().Ok() && ((style.GetFlags() & (wxTEXT_ATTR_FONT)) == (wxTEXT_ATTR_FONT)))
        destStyle.SetFont(style.GetFont());
    else if (style.GetFont().Ok())
    {
        wxFont font = destStyle.GetFont();

        if (style.GetFlags() & wxTEXT_ATTR_FONT_FACE)
            font.SetFaceName(style.GetFont().GetFaceName());

        if (style.GetFlags() & wxTEXT_ATTR_FONT_SIZE)
            font.SetPointSize(style.GetFont().GetPointSize());

        if (style.GetFlags() & wxTEXT_ATTR_FONT_ITALIC)
            font.SetStyle(style.GetFont().GetStyle());

        if (style.GetFlags() & wxTEXT_ATTR_FONT_WEIGHT)
            font.SetWeight(style.GetFont().GetWeight());

        if (style.GetFlags() & wxTEXT_ATTR_FONT_UNDERLINE)
            font.SetUnderlined(style.GetFont().GetUnderlined());

        if (font != destStyle.GetFont())
            destStyle.SetFont(font);
    }

    if ( style.GetTextColour().Ok() && style.HasTextColour())
        destStyle.SetTextColour(style.GetTextColour());

    if ( style.GetBackgroundColour().Ok() && style.HasBackgroundColour())
        destStyle.SetBackgroundColour(style.GetBackgroundColour());

    if (style.HasAlignment())
        destStyle.SetAlignment(style.GetAlignment());

    if (style.HasTabs())
        destStyle.SetTabs(style.GetTabs());

    if (style.HasLeftIndent())
        destStyle.SetLeftIndent(style.GetLeftIndent(), style.GetLeftSubIndent());

    if (style.HasRightIndent())
        destStyle.SetRightIndent(style.GetRightIndent());

    if (style.HasParagraphSpacingAfter())
        destStyle.SetParagraphSpacingAfter(style.GetParagraphSpacingAfter());

    if (style.HasParagraphSpacingBefore())
        destStyle.SetParagraphSpacingBefore(style.GetParagraphSpacingBefore());

    if (style.HasLineSpacing())
        destStyle.SetLineSpacing(style.GetLineSpacing());

    if (style.HasCharacterStyleName())
        destStyle.SetCharacterStyleName(style.GetCharacterStyleName());

    if (style.HasParagraphStyleName())
        destStyle.SetParagraphStyleName(style.GetParagraphStyleName());

    if (style.HasBulletStyle())
    {
        destStyle.SetBulletStyle(style.GetBulletStyle());
        destStyle.SetBulletSymbol(style.GetBulletSymbol());
    }

    if (style.HasBulletNumber())
        destStyle.SetBulletNumber(style.GetBulletNumber());

    return true;
}

bool wxRichTextApplyStyle(wxRichTextAttr& destStyle, const wxTextAttrEx& style)
{
    wxTextAttrEx destStyle2;
    destStyle.CopyTo(destStyle2);
    wxRichTextApplyStyle(destStyle2, style);
    destStyle = destStyle2;
    return true;
}

bool wxRichTextApplyStyle(wxTextAttrEx& destStyle, const wxRichTextAttr& style)
{

    // Whole font. Avoiding setting individual attributes if possible, since
    // it recreates the font each time.
    if ((style.GetFlags() & (wxTEXT_ATTR_FONT)) == (wxTEXT_ATTR_FONT))
    {
        destStyle.SetFont(wxFont(style.GetFontSize(), destStyle.GetFont().Ok() ? destStyle.GetFont().GetFamily() : wxDEFAULT,
            style.GetFontStyle(), style.GetFontWeight(), style.GetFontUnderlined(), style.GetFontFaceName()));
    }
    else if (style.GetFlags() & (wxTEXT_ATTR_FONT))
    {
        wxFont font = destStyle.GetFont();

        if (style.GetFlags() & wxTEXT_ATTR_FONT_FACE)
            font.SetFaceName(style.GetFontFaceName());

        if (style.GetFlags() & wxTEXT_ATTR_FONT_SIZE)
            font.SetPointSize(style.GetFontSize());

        if (style.GetFlags() & wxTEXT_ATTR_FONT_ITALIC)
            font.SetStyle(style.GetFontStyle());

        if (style.GetFlags() & wxTEXT_ATTR_FONT_WEIGHT)
            font.SetWeight(style.GetFontWeight());

        if (style.GetFlags() & wxTEXT_ATTR_FONT_UNDERLINE)
            font.SetUnderlined(style.GetFontUnderlined());

        if (font != destStyle.GetFont())
            destStyle.SetFont(font);
    }

    if ( style.GetTextColour().Ok() && style.HasTextColour())
        destStyle.SetTextColour(style.GetTextColour());

    if ( style.GetBackgroundColour().Ok() && style.HasBackgroundColour())
        destStyle.SetBackgroundColour(style.GetBackgroundColour());

    if (style.HasAlignment())
        destStyle.SetAlignment(style.GetAlignment());

    if (style.HasTabs())
        destStyle.SetTabs(style.GetTabs());

    if (style.HasLeftIndent())
        destStyle.SetLeftIndent(style.GetLeftIndent(), style.GetLeftSubIndent());

    if (style.HasRightIndent())
        destStyle.SetRightIndent(style.GetRightIndent());

    if (style.HasParagraphSpacingAfter())
        destStyle.SetParagraphSpacingAfter(style.GetParagraphSpacingAfter());

    if (style.HasParagraphSpacingBefore())
        destStyle.SetParagraphSpacingBefore(style.GetParagraphSpacingBefore());

    if (style.HasLineSpacing())
        destStyle.SetLineSpacing(style.GetLineSpacing());

    if (style.HasCharacterStyleName())
        destStyle.SetCharacterStyleName(style.GetCharacterStyleName());

    if (style.HasParagraphStyleName())
        destStyle.SetParagraphStyleName(style.GetParagraphStyleName());

    if (style.HasBulletStyle())
    {
        destStyle.SetBulletStyle(style.GetBulletStyle());
        destStyle.SetBulletSymbol(style.GetBulletSymbol());
    }

    if (style.HasBulletNumber())
        destStyle.SetBulletNumber(style.GetBulletNumber());

    return true;
}


/*!
 * wxRichTextAttr stores attributes without a wxFont object, so is a much more
 * efficient way to query styles.
 */

// ctors
wxRichTextAttr::wxRichTextAttr(const wxColour& colText,
               const wxColour& colBack,
               wxTextAttrAlignment alignment): m_textAlignment(alignment), m_colText(colText), m_colBack(colBack)
{
    Init();

    if (m_colText.Ok()) m_flags |= wxTEXT_ATTR_TEXT_COLOUR;
    if (m_colBack.Ok()) m_flags |= wxTEXT_ATTR_BACKGROUND_COLOUR;
    if (alignment != wxTEXT_ALIGNMENT_DEFAULT)
        m_flags |= wxTEXT_ATTR_ALIGNMENT;
}

wxRichTextAttr::wxRichTextAttr(const wxTextAttrEx& attr)
{
    Init();

    (*this) = attr;
}

// operations
void wxRichTextAttr::Init()
{
    m_textAlignment = wxTEXT_ALIGNMENT_DEFAULT;
    m_flags = 0;
    m_leftIndent = 0;
    m_leftSubIndent = 0;
    m_rightIndent = 0;

    m_fontSize = 12;
    m_fontStyle = wxNORMAL;
    m_fontWeight = wxNORMAL;
    m_fontUnderlined = false;

    m_paragraphSpacingAfter = 0;
    m_paragraphSpacingBefore = 0;
    m_lineSpacing = 0;
    m_bulletStyle = wxTEXT_ATTR_BULLET_STYLE_NONE;
    m_bulletNumber = 0;
    m_bulletSymbol = wxT('*');
}

// operators
void wxRichTextAttr::operator= (const wxRichTextAttr& attr)
{
    m_colText = attr.m_colText;
    m_colBack = attr.m_colBack;
    m_textAlignment = attr.m_textAlignment;
    m_leftIndent = attr.m_leftIndent;
    m_leftSubIndent = attr.m_leftSubIndent;
    m_rightIndent = attr.m_rightIndent;
    m_tabs = attr.m_tabs;
    m_flags = attr.m_flags;

    m_fontSize = attr.m_fontSize;
    m_fontStyle = attr.m_fontStyle;
    m_fontWeight = attr.m_fontWeight;
    m_fontUnderlined = attr.m_fontUnderlined;
    m_fontFaceName = attr.m_fontFaceName;

    m_paragraphSpacingAfter = attr.m_paragraphSpacingAfter;
    m_paragraphSpacingBefore = attr.m_paragraphSpacingBefore;
    m_lineSpacing = attr.m_lineSpacing;
    m_characterStyleName = attr.m_characterStyleName;
    m_paragraphStyleName = attr.m_paragraphStyleName;
    m_bulletStyle = attr.m_bulletStyle;
    m_bulletNumber = attr.m_bulletNumber;
    m_bulletSymbol = attr.m_bulletSymbol;
}

// operators
void wxRichTextAttr::operator= (const wxTextAttrEx& attr)
{
    m_colText = attr.GetTextColour();
    m_colBack = attr.GetBackgroundColour();
    m_textAlignment = attr.GetAlignment();
    m_leftIndent = attr.GetLeftIndent();
    m_leftSubIndent = attr.GetLeftSubIndent();
    m_rightIndent = attr.GetRightIndent();
    m_tabs = attr.GetTabs();
    m_flags = attr.GetFlags();

    m_paragraphSpacingAfter = attr.GetParagraphSpacingAfter();
    m_paragraphSpacingBefore = attr.GetParagraphSpacingBefore();
    m_lineSpacing = attr.GetLineSpacing();
    m_characterStyleName = attr.GetCharacterStyleName();
    m_paragraphStyleName = attr.GetParagraphStyleName();

    if (attr.GetFont().Ok())
        GetFontAttributes(attr.GetFont());
}

// Making a wxTextAttrEx object.
wxRichTextAttr::operator wxTextAttrEx () const
{
    wxTextAttrEx attr;
    CopyTo(attr);
    return attr;
}

// Copy to a wxTextAttr
void wxRichTextAttr::CopyTo(wxTextAttrEx& attr) const
{
    attr.SetTextColour(GetTextColour());
    attr.SetBackgroundColour(GetBackgroundColour());
    attr.SetAlignment(GetAlignment());
    attr.SetTabs(GetTabs());
    attr.SetLeftIndent(GetLeftIndent(), GetLeftSubIndent());
    attr.SetRightIndent(GetRightIndent());
    attr.SetFont(CreateFont());
    attr.SetFlags(GetFlags()); // Important: set after SetFont, since SetFont sets flags

    attr.SetParagraphSpacingAfter(m_paragraphSpacingAfter);
    attr.SetParagraphSpacingBefore(m_paragraphSpacingBefore);
    attr.SetLineSpacing(m_lineSpacing);
    attr.SetBulletStyle(m_bulletStyle);
    attr.SetBulletNumber(m_bulletNumber);
    attr.SetBulletSymbol(m_bulletSymbol);
    attr.SetCharacterStyleName(m_characterStyleName);
    attr.SetParagraphStyleName(m_paragraphStyleName);

}

// Create font from font attributes.
wxFont wxRichTextAttr::CreateFont() const
{
    wxFont font(m_fontSize, wxDEFAULT, m_fontStyle, m_fontWeight, m_fontUnderlined, m_fontFaceName);
#ifdef __WXMAC__
    font.SetNoAntiAliasing(true);
#endif
    return font;
}

// Get attributes from font.
bool wxRichTextAttr::GetFontAttributes(const wxFont& font)
{
    if (!font.Ok())
        return false;

    m_fontSize = font.GetPointSize();
    m_fontStyle = font.GetStyle();
    m_fontWeight = font.GetWeight();
    m_fontUnderlined = font.GetUnderlined();
    m_fontFaceName = font.GetFaceName();

    return true;
}

wxRichTextAttr wxRichTextAttr::Combine(const wxRichTextAttr& attr,
                               const wxRichTextAttr& attrDef,
                               const wxTextCtrlBase *text)
{
    wxColour colFg = attr.GetTextColour();
    if ( !colFg.Ok() )
    {
        colFg = attrDef.GetTextColour();

        if ( text && !colFg.Ok() )
            colFg = text->GetForegroundColour();
    }

    wxColour colBg = attr.GetBackgroundColour();
    if ( !colBg.Ok() )
    {
        colBg = attrDef.GetBackgroundColour();

        if ( text && !colBg.Ok() )
            colBg = text->GetBackgroundColour();
    }

    wxRichTextAttr newAttr(colFg, colBg);

    if (attr.HasWeight())
        newAttr.SetFontWeight(attr.GetFontWeight());

    if (attr.HasSize())
        newAttr.SetFontSize(attr.GetFontSize());

    if (attr.HasItalic())
        newAttr.SetFontStyle(attr.GetFontStyle());

    if (attr.HasUnderlined())
        newAttr.SetFontUnderlined(attr.GetFontUnderlined());

    if (attr.HasFaceName())
        newAttr.SetFontFaceName(attr.GetFontFaceName());

    if (attr.HasAlignment())
        newAttr.SetAlignment(attr.GetAlignment());
    else if (attrDef.HasAlignment())
        newAttr.SetAlignment(attrDef.GetAlignment());

    if (attr.HasTabs())
        newAttr.SetTabs(attr.GetTabs());
    else if (attrDef.HasTabs())
        newAttr.SetTabs(attrDef.GetTabs());

    if (attr.HasLeftIndent())
        newAttr.SetLeftIndent(attr.GetLeftIndent(), attr.GetLeftSubIndent());
    else if (attrDef.HasLeftIndent())
        newAttr.SetLeftIndent(attrDef.GetLeftIndent(), attr.GetLeftSubIndent());

    if (attr.HasRightIndent())
        newAttr.SetRightIndent(attr.GetRightIndent());
    else if (attrDef.HasRightIndent())
        newAttr.SetRightIndent(attrDef.GetRightIndent());

    // NEW ATTRIBUTES

    if (attr.HasParagraphSpacingAfter())
        newAttr.SetParagraphSpacingAfter(attr.GetParagraphSpacingAfter());

    if (attr.HasParagraphSpacingBefore())
        newAttr.SetParagraphSpacingBefore(attr.GetParagraphSpacingBefore());

    if (attr.HasLineSpacing())
        newAttr.SetLineSpacing(attr.GetLineSpacing());

    if (attr.HasCharacterStyleName())
        newAttr.SetCharacterStyleName(attr.GetCharacterStyleName());

    if (attr.HasParagraphStyleName())
        newAttr.SetParagraphStyleName(attr.GetParagraphStyleName());

    if (attr.HasBulletStyle())
        newAttr.SetBulletStyle(attr.GetBulletStyle());

    if (attr.HasBulletNumber())
        newAttr.SetBulletNumber(attr.GetBulletNumber());

    if (attr.HasBulletSymbol())
        newAttr.SetBulletSymbol(attr.GetBulletSymbol());

    return newAttr;
}

/*!
 * wxTextAttrEx is an extended version of wxTextAttr with more paragraph attributes.
 */

wxTextAttrEx::wxTextAttrEx(const wxTextAttrEx& attr): wxTextAttr(attr)
{
    m_paragraphSpacingAfter = attr.m_paragraphSpacingAfter;
    m_paragraphSpacingBefore = attr.m_paragraphSpacingBefore;
    m_lineSpacing = attr.m_lineSpacing;
    m_paragraphStyleName = attr.m_paragraphStyleName;
    m_characterStyleName = attr.m_characterStyleName;
    m_bulletStyle = attr.m_bulletStyle;
    m_bulletNumber = attr.m_bulletNumber;
    m_bulletSymbol = attr.m_bulletSymbol;
}

// Initialise this object.
void wxTextAttrEx::Init()
{
    m_paragraphSpacingAfter = 0;
    m_paragraphSpacingBefore = 0;
    m_lineSpacing = 0;
    m_bulletStyle = wxTEXT_ATTR_BULLET_STYLE_NONE;
    m_bulletNumber = 0;
    m_bulletSymbol = 0;
    m_bulletSymbol = wxT('*');
}

// Assignment from a wxTextAttrEx object
void wxTextAttrEx::operator= (const wxTextAttrEx& attr)
{
    wxTextAttr::operator= (attr);

    m_paragraphSpacingAfter = attr.m_paragraphSpacingAfter;
    m_paragraphSpacingBefore = attr.m_paragraphSpacingBefore;
    m_lineSpacing = attr.m_lineSpacing;
    m_characterStyleName = attr.m_characterStyleName;
    m_paragraphStyleName = attr.m_paragraphStyleName;
    m_bulletStyle = attr.m_bulletStyle;
    m_bulletNumber = attr.m_bulletNumber;
    m_bulletSymbol = attr.m_bulletSymbol;
}

// Assignment from a wxTextAttr object.
void wxTextAttrEx::operator= (const wxTextAttr& attr)
{
    wxTextAttr::operator= (attr);
}

wxTextAttrEx wxTextAttrEx::CombineEx(const wxTextAttrEx& attr,
                               const wxTextAttrEx& attrDef,
                               const wxTextCtrlBase *text)
{
    wxTextAttrEx newAttr;

    // If attr specifies the complete font, just use that font, overriding all
    // default font attributes.
    if ((attr.GetFlags() & wxTEXT_ATTR_FONT) == wxTEXT_ATTR_FONT)
        newAttr.SetFont(attr.GetFont());
    else
    {
        // First find the basic, default font
        long flags = 0;

        wxFont font;
        if (attrDef.HasFont())
        {
            flags = (attrDef.GetFlags() & wxTEXT_ATTR_FONT);
            font = attrDef.GetFont();
        }
        else
        {
            if (text)
                font = text->GetFont();

            // We leave flags at 0 because no font attributes have been specified yet
        }
        if (!font.Ok())
            font = *wxNORMAL_FONT;

        // Otherwise, if there are font attributes in attr, apply them
        if (attr.HasFont())
        {
            if (attr.HasSize())
            {
                flags |= wxTEXT_ATTR_FONT_SIZE;
                font.SetPointSize(attr.GetFont().GetPointSize());
            }
            if (attr.HasItalic())
            {
                flags |= wxTEXT_ATTR_FONT_ITALIC;;
                font.SetStyle(attr.GetFont().GetStyle());
            }
            if (attr.HasWeight())
            {
                flags |= wxTEXT_ATTR_FONT_WEIGHT;
                font.SetWeight(attr.GetFont().GetWeight());
            }
            if (attr.HasFaceName())
            {
                flags |= wxTEXT_ATTR_FONT_FACE;
                font.SetFaceName(attr.GetFont().GetFaceName());
            }
            if (attr.HasUnderlined())
            {
                flags |= wxTEXT_ATTR_FONT_UNDERLINE;
                font.SetUnderlined(attr.GetFont().GetUnderlined());
            }
            newAttr.SetFont(font);
            newAttr.SetFlags(newAttr.GetFlags()|flags);
        }
    }

    // TODO: should really check we are specifying these in the flags,
    // before setting them, as per above; or we will set them willy-nilly.
    // However, we should also check whether this is the intention
    // as per wxTextAttr::Combine, i.e. always to have valid colours
    // in the style.
    wxColour colFg = attr.GetTextColour();
    if ( !colFg.Ok() )
    {
        colFg = attrDef.GetTextColour();

        if ( text && !colFg.Ok() )
            colFg = text->GetForegroundColour();
    }

    wxColour colBg = attr.GetBackgroundColour();
    if ( !colBg.Ok() )
    {
        colBg = attrDef.GetBackgroundColour();

        if ( text && !colBg.Ok() )
            colBg = text->GetBackgroundColour();
    }

    newAttr.SetTextColour(colFg);
    newAttr.SetBackgroundColour(colBg);

    if (attr.HasAlignment())
        newAttr.SetAlignment(attr.GetAlignment());
    else if (attrDef.HasAlignment())
        newAttr.SetAlignment(attrDef.GetAlignment());

    if (attr.HasTabs())
        newAttr.SetTabs(attr.GetTabs());
    else if (attrDef.HasTabs())
        newAttr.SetTabs(attrDef.GetTabs());

    if (attr.HasLeftIndent())
        newAttr.SetLeftIndent(attr.GetLeftIndent(), attr.GetLeftSubIndent());
    else if (attrDef.HasLeftIndent())
        newAttr.SetLeftIndent(attrDef.GetLeftIndent(), attr.GetLeftSubIndent());

    if (attr.HasRightIndent())
        newAttr.SetRightIndent(attr.GetRightIndent());
    else if (attrDef.HasRightIndent())
        newAttr.SetRightIndent(attrDef.GetRightIndent());

    // NEW ATTRIBUTES

    if (attr.HasParagraphSpacingAfter())
        newAttr.SetParagraphSpacingAfter(attr.GetParagraphSpacingAfter());

    if (attr.HasParagraphSpacingBefore())
        newAttr.SetParagraphSpacingBefore(attr.GetParagraphSpacingBefore());

    if (attr.HasLineSpacing())
        newAttr.SetLineSpacing(attr.GetLineSpacing());

    if (attr.HasCharacterStyleName())
        newAttr.SetCharacterStyleName(attr.GetCharacterStyleName());

    if (attr.HasParagraphStyleName())
        newAttr.SetParagraphStyleName(attr.GetParagraphStyleName());

    if (attr.HasBulletStyle())
        newAttr.SetBulletStyle(attr.GetBulletStyle());

    if (attr.HasBulletNumber())
        newAttr.SetBulletNumber(attr.GetBulletNumber());

    if (attr.HasBulletSymbol())
        newAttr.SetBulletSymbol(attr.GetBulletSymbol());

    return newAttr;
}


/*!
 * wxRichTextFileHandler
 * Base class for file handlers
 */

IMPLEMENT_CLASS(wxRichTextFileHandler, wxObject)

#if wxUSE_STREAMS
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
#endif // wxUSE_STREAMS

/// Can we handle this filename (if using files)? By default, checks the extension.
bool wxRichTextFileHandler::CanHandle(const wxString& filename) const
{
    wxString path, file, ext;
    wxSplitPath(filename, & path, & file, & ext);

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
    if (m_data)
    {
        delete[] m_data;
        m_data = NULL;
    }
}

void wxRichTextImageBlock::Init()
{
    m_data = NULL;
    m_dataSize = 0;
    m_imageType = -1;
}

void wxRichTextImageBlock::Clear()
{
    delete[] m_data;
    m_data = NULL;
    m_dataSize = 0;
    m_imageType = -1;
}


// Load the original image into a memory block.
// If the image is not a JPEG, we must convert it into a JPEG
// to conserve space.
// If it's not a JPEG we can make use of 'image', already scaled, so we don't have to
// load the image a 2nd time.

bool wxRichTextImageBlock::MakeImageBlock(const wxString& filename, int imageType, wxImage& image, bool convertToJPEG)
{
    m_imageType = imageType;

    wxString filenameToRead(filename);
    bool removeFile = false;

    if (imageType == -1)
        return false; // Could not determine image type

    if ((imageType != wxBITMAP_TYPE_JPEG) && convertToJPEG)
    {
        wxString tempFile;
        bool success = wxGetTempFileName(_("image"), tempFile) ;

        wxASSERT(success);

        wxUnusedVar(success);

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
bool wxRichTextImageBlock::MakeImageBlock(wxImage& image, int imageType, int quality)
{
    m_imageType = imageType;
    image.SetOption(wxT("quality"), quality);

    if (imageType == -1)
        return false; // Could not determine image type

    wxString tempFile;
    bool success = wxGetTempFileName(_("image"), tempFile) ;

    wxASSERT(success);
    wxUnusedVar(success);

    if (!image.SaveFile(tempFile, m_imageType))
    {
        if (wxFileExists(tempFile))
            wxRemoveFile(tempFile);
        return false;
    }

    wxFile file;
    if (!file.Open(tempFile))
        return false;

    m_dataSize = (size_t) file.Length();
    file.Close();

    if (m_data)
        delete[] m_data;
    m_data = ReadBlock(tempFile, m_dataSize);

    wxRemoveFile(tempFile);

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
    if (m_data)
    {
        delete[] m_data;
        m_data = NULL;
    }
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
#if 1
    wxMemoryInputStream mstream(m_data, m_dataSize);
    bool success = image.LoadFile(mstream, GetImageType());
#else
    wxString tempFile;
    bool success = wxGetTempFileName(_("image"), tempFile) ;
    wxASSERT(success);

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
    wxString hex;
    int i;
    for (i = 0; i < (int) m_dataSize; i++)
    {
        hex = wxDecToHex(m_data[i]);
        wxCharBuffer buf = hex.ToAscii();

        stream.Write((const char*) buf, hex.length());
    }

    return true;
}

// Read data in hex from a stream
bool wxRichTextImageBlock::ReadHex(wxInputStream& stream, int length, int imageType)
{
    int dataSize = length/2;

    if (m_data)
        delete[] m_data;

    wxString str(wxT("  "));
    m_data = new unsigned char[dataSize];
    int i;
    for (i = 0; i < dataSize; i ++)
    {
        str[0] = stream.GetC();
        str[1] = stream.GetC();

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

#endif
    // wxUSE_RICHTEXT
