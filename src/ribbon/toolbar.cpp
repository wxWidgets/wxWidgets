///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/toolbar.cpp
// Purpose:     Ribbon-style tool bar
// Author:      Peter Cawley
// Created:     2009-07-06
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_RIBBON

#include "wx/ribbon/toolbar.h"
#include "wx/ribbon/art.h"
#include "wx/ribbon/bar.h"
#include "wx/dcbuffer.h"

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#endif

#include <memory>

class wxRibbonToolBarToolBase
{
public:
    wxString helpString;
    wxBitmapBundle bitmap;
    wxBitmapBundle bitmapDisabled;
    wxRect dropdown;
    wxPoint position;
    wxSize size;
    wxObject* clientData = nullptr;
    int id;
    wxRibbonButtonKind kind;
    long state;
};

WX_DEFINE_ARRAY_PTR(wxRibbonToolBarToolBase*, wxArrayRibbonToolBarToolBase);

class wxRibbonToolBarToolGroup
{
public:
    // To identify the group as a wxRibbonToolBarToolBase*
    wxRibbonToolBarToolBase dummyTool;

    wxArrayRibbonToolBarToolBase tools;
    wxPoint position;
    wxSize size;
};

wxDEFINE_EVENT(wxEVT_RIBBONTOOLBAR_CLICKED, wxRibbonToolBarEvent);
wxDEFINE_EVENT(wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED, wxRibbonToolBarEvent);

wxIMPLEMENT_DYNAMIC_CLASS(wxRibbonToolBarEvent, wxCommandEvent);
wxIMPLEMENT_CLASS(wxRibbonToolBar, wxRibbonControl);

wxBEGIN_EVENT_TABLE(wxRibbonToolBar, wxRibbonControl)
    EVT_ENTER_WINDOW(wxRibbonToolBar::OnMouseEnter)
    EVT_ERASE_BACKGROUND(wxRibbonToolBar::OnEraseBackground)
    EVT_LEAVE_WINDOW(wxRibbonToolBar::OnMouseLeave)
    EVT_LEFT_DOWN(wxRibbonToolBar::OnMouseDown)
    EVT_LEFT_DCLICK(wxRibbonToolBar::OnMouseDown)
    EVT_LEFT_UP(wxRibbonToolBar::OnMouseUp)
    EVT_MOTION(wxRibbonToolBar::OnMouseMove)
    EVT_PAINT(wxRibbonToolBar::OnPaint)
    EVT_SIZE(wxRibbonToolBar::OnSize)
    EVT_DPI_CHANGED(wxRibbonToolBar::OnDPIChanged)
wxEND_EVENT_TABLE()

wxRibbonToolBar::wxRibbonToolBar()
{
}

wxRibbonToolBar::wxRibbonToolBar(wxWindow* parent,
                  wxWindowID id,
                  const wxPoint& pos,
                  const wxSize& size,
                  long style)
    : wxRibbonControl(parent, id, pos, size, wxBORDER_NONE)
{
    CommonInit(style);
}

bool wxRibbonToolBar::Create(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                long style)
{
    if ( !wxRibbonControl::Create(parent, id, pos, size, wxBORDER_NONE) )
    {
        return false;
    }

    CommonInit(style);
    return true;
}

void wxRibbonToolBar::CommonInit(long WXUNUSED(style))
{
    AppendGroup();
    m_hoverTool = nullptr;
    m_activeTool = nullptr;
    m_nrowsMin = 1;
    m_nrowsMax = 1;
    m_sizes = new wxSize[1];
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

wxRibbonToolBar::~wxRibbonToolBar()
{
    size_t count = m_groups.GetCount();
    size_t i, t;
    for ( i = 0; i < count; ++i )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(i);
        size_t toolCount = group->tools.GetCount();
        for ( t = 0; t < toolCount; ++t )
        {
            wxRibbonToolBarToolBase* tool = group->tools.Item(t);
            delete tool;
        }
        delete group;
    }
    m_groups.Clear();
    delete[] m_sizes;
}

wxRibbonToolBarToolBase* wxRibbonToolBar::AddTool(
                int toolId,
                const wxBitmapBundle& bitmap,
                const wxString& helpString,
                wxRibbonButtonKind kind)
{
    return AddTool(toolId, bitmap, wxBitmapBundle(), helpString, kind, nullptr);
}

wxRibbonToolBarToolBase* wxRibbonToolBar::AddDropdownTool(
            int toolId,
            const wxBitmapBundle& bitmap,
            const wxString& helpString)
{
    return AddTool(toolId, bitmap, wxBitmapBundle(), helpString,
        wxRIBBON_BUTTON_DROPDOWN, nullptr);
}

wxRibbonToolBarToolBase* wxRibbonToolBar::AddHybridTool(
            int toolId,
            const wxBitmapBundle& bitmap,
            const wxString& helpString)
{
    return AddTool(toolId, bitmap, wxBitmapBundle(), helpString,
        wxRIBBON_BUTTON_HYBRID, nullptr);
}

wxRibbonToolBarToolBase* wxRibbonToolBar::AddToggleTool(
        int toolId,
        const wxBitmapBundle& bitmap,
        const wxString& helpString)
{
    return AddTool(toolId, bitmap, wxBitmapBundle(), helpString,
        wxRIBBON_BUTTON_TOGGLE, nullptr);
}

wxRibbonToolBarToolBase* wxRibbonToolBar::AddTool(
            int toolId,
            const wxBitmapBundle& bitmap,
            const wxBitmapBundle& bitmapDisabled,
            const wxString& helpString,
            wxRibbonButtonKind kind,
            wxObject* clientData)
{
    return InsertTool(GetToolCount(), toolId, bitmap, bitmapDisabled,
        helpString, kind, clientData);
}

wxRibbonToolBarToolBase* wxRibbonToolBar::AddSeparator()
{
    if ( m_groups.Last()->tools.IsEmpty() )
        return nullptr;

    AppendGroup();
    return &m_groups.Last()->dummyTool;
}


wxRibbonToolBarToolBase* wxRibbonToolBar::InsertTool(
                size_t pos,
                int toolId,
                const wxBitmapBundle& bitmap,
                const wxString& helpString,
                wxRibbonButtonKind kind)
{
    return InsertTool(pos, toolId, bitmap, wxBitmapBundle(), helpString, kind,
        nullptr);
}

wxRibbonToolBarToolBase* wxRibbonToolBar::InsertDropdownTool(
            size_t pos,
            int toolId,
            const wxBitmapBundle& bitmap,
            const wxString& helpString)
{
    return InsertTool(pos, toolId, bitmap, wxBitmapBundle(), helpString,
        wxRIBBON_BUTTON_DROPDOWN, nullptr);
}

wxRibbonToolBarToolBase* wxRibbonToolBar::InsertHybridTool(
            size_t pos,
            int toolId,
            const wxBitmapBundle& bitmap,
            const wxString& helpString)
{
    return InsertTool(pos, toolId, bitmap, wxBitmapBundle(), helpString,
        wxRIBBON_BUTTON_HYBRID, nullptr);
}

wxRibbonToolBarToolBase* wxRibbonToolBar::InsertToggleTool(
        size_t pos,
        int toolId,
        const wxBitmapBundle& bitmap,
        const wxString& helpString)
{
    return InsertTool(pos, toolId, bitmap, wxBitmapBundle(), helpString,
        wxRIBBON_BUTTON_TOGGLE, nullptr);
}

wxRibbonToolBarToolBase* wxRibbonToolBar::InsertTool(
            size_t pos,
            int toolId,
            const wxBitmapBundle& bitmap,
            const wxBitmapBundle& bitmapDisabled,
            const wxString& helpString,
            wxRibbonButtonKind kind,
            wxObject* clientData)
{
    wxASSERT(bitmap.IsOk());

    // Create the wxRibbonToolBarToolBase with parameters
    std::unique_ptr<wxRibbonToolBarToolBase> tool(new wxRibbonToolBarToolBase);
    tool->id = toolId;
    tool->bitmap = bitmap;
    if ( bitmapDisabled.IsOk() )
    {
        wxASSERT(bitmap.GetDefaultSize() == bitmapDisabled.GetDefaultSize());
        tool->bitmapDisabled = bitmapDisabled;
    }
    else
    {
        // Generate disabled bitmap from normal bitmap
        wxBitmap bmp = bitmap.GetBitmap(bitmap.GetDefaultSize());
        tool->bitmapDisabled = wxBitmapBundle::FromBitmap(MakeDisabledBitmap(bmp));
    }
    tool->helpString = helpString;
    tool->kind = kind;
    tool->clientData = clientData;
    tool->state = 0;

    // Find the position where insert tool
    size_t groupCount = m_groups.GetCount();
    size_t g;
    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        size_t toolCount = group->tools.GetCount();
        if ( pos <= toolCount )
        {
            // Give the ownership of the tool to group->tools
            wxRibbonToolBarToolBase* const p = tool.release();
            group->tools.Insert(p, pos);
            return p;
        }
        pos -= toolCount + 1;
    }
    wxFAIL_MSG("Tool position out of toolbar bounds.");
    return nullptr;
}

wxRibbonToolBarToolBase* wxRibbonToolBar::InsertSeparator(size_t pos)
{
    size_t groupCount = m_groups.GetCount();
    size_t g;
    for ( g = 0; g < groupCount; ++g )
    {
        if ( pos== 0 ) // Prepend group
            return &InsertGroup(g)->dummyTool;
        if ( pos == groupCount ) // Append group
            return &InsertGroup(g+1)->dummyTool;

        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        size_t toolCount = group->tools.GetCount();
        if ( pos < toolCount )
        {
            wxRibbonToolBarToolGroup* newGroup = InsertGroup(g+1);

            for ( size_t t = pos; t < toolCount; t++ )
                newGroup->tools.Add(group->tools[t]);
            group->tools.RemoveAt(pos, toolCount-pos);

            return &group->dummyTool;
        }
        pos -= toolCount + 1;
    }
    // Add an empty group at the end of the bar.
    if ( m_groups.Last()->tools.IsEmpty() )
        return nullptr;
    AppendGroup();
    return &m_groups.Last()->dummyTool;
}

wxRibbonToolBarToolGroup* wxRibbonToolBar::InsertGroup(size_t pos)
{
    wxRibbonToolBarToolGroup* group = new wxRibbonToolBarToolGroup;
    m_groups.Insert(group, pos);
    return group;
}

void wxRibbonToolBar::ClearTools()
{
    size_t count = m_groups.GetCount();
    size_t i, t;
    for ( i = 0; i < count; ++i )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(i);
        size_t toolCount = group->tools.GetCount();
        for ( t = 0; t < toolCount; ++t )
        {
            wxRibbonToolBarToolBase* tool = group->tools.Item(t);
            delete tool;
        }
        delete group;
    }
    m_groups.Clear();
}

bool wxRibbonToolBar::DeleteTool(int toolId)
{
    size_t groupCount = m_groups.GetCount();
    size_t g, t;
    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        size_t toolCount = group->tools.GetCount();
        for ( t = 0; t < toolCount; ++t )
        {
            wxRibbonToolBarToolBase* tool = group->tools.Item(t);
            if ( tool->id == toolId )
            {
                group->tools.RemoveAt(t);
                delete tool;
                return true;
            }
        }
    }
    return false;
}

bool wxRibbonToolBar::DeleteToolByPos(size_t pos)
{
    size_t groupCount = m_groups.GetCount();
    size_t g, t;
    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        size_t toolCount = group->tools.GetCount();
        if ( pos < toolCount )
        {
            // Remove tool
            wxRibbonToolBarToolBase* tool = group->tools.Item(pos);
            group->tools.RemoveAt(pos);
            delete tool;
            return true;
        }
        else if ( pos == toolCount )
        {
            // Remove separator
            if ( g < groupCount - 1 )
            {
                wxRibbonToolBarToolGroup* nextGroup = m_groups.Item(g+1);
                for ( t = 0; t < nextGroup->tools.GetCount(); ++t )
                    group->tools.Add(nextGroup->tools[t]);
                m_groups.RemoveAt(g+1);
                delete nextGroup;
            }
            return true;
        }
        pos -= toolCount + 1;
    }
    return false;
}

wxRibbonToolBarToolBase* wxRibbonToolBar::FindById(int toolId) const
{
    size_t groupCount = m_groups.GetCount();
    size_t g, t;
    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        size_t toolCount = group->tools.GetCount();
        for ( t = 0; t < toolCount; ++t )
        {
            wxRibbonToolBarToolBase* tool = group->tools.Item(t);
            if ( tool->id == toolId )
            {
                return tool;
            }
        }
    }
    return nullptr;
}

wxRibbonToolBarToolBase* wxRibbonToolBar::GetToolByPos(size_t pos) const
{
    size_t groupCount = m_groups.GetCount();
    size_t g;
    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        size_t toolCount = group->tools.GetCount();
        if ( pos < toolCount )
        {
            return group->tools[pos];
        }
        else if ( pos == toolCount )
        {
            return nullptr;
        }
        pos -= toolCount + 1;
    }
    return nullptr;
}

wxRibbonToolBarToolBase* wxRibbonToolBar::GetToolByPos(wxCoord x, wxCoord y) const
{
    size_t groupCount = m_groups.GetCount();
    for ( size_t g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        size_t toolCount = group->tools.GetCount();
        for ( size_t t = 0; t < toolCount; ++t )
        {
            wxRibbonToolBarToolBase* tool = group->tools.Item(t);
            wxRect rect(group->position + tool->position, tool->size);
            if ( rect.Contains(x, y) )
            {
                return tool;
            }
        }
    }
    return nullptr;
}

size_t wxRibbonToolBar::GetToolCount() const
{
    size_t count = 0;
    for ( size_t g = 0; g < m_groups.GetCount(); ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        count += group->tools.GetCount();
    }
    // There is a splitter in front of every group except for the first
    // If only one group, no separator.
    if ( m_groups.GetCount() > 1 )
        count += m_groups.GetCount() - 1;
    return count;
}

int wxRibbonToolBar::GetToolId(const wxRibbonToolBarToolBase* tool) const
{
    wxCHECK_MSG(tool != nullptr , wxNOT_FOUND, "The tool pointer must not be null");
    return tool->id;
}

wxRibbonToolBarToolBase* wxRibbonToolBar::GetActiveTool() const
{
    return m_activeTool == nullptr ? nullptr : m_activeTool;
}

wxObject* wxRibbonToolBar::GetToolClientData(int toolId) const
{
    wxRibbonToolBarToolBase* tool = FindById(toolId);
    wxCHECK_MSG(tool != nullptr , nullptr, "Invalid tool id");
    return tool->clientData;
}

bool wxRibbonToolBar::GetToolEnabled(int toolId) const
{
    wxRibbonToolBarToolBase* tool = FindById(toolId);
    wxCHECK_MSG(tool != nullptr , false, "Invalid tool id");
    return (tool->state & wxRIBBON_TOOLBAR_TOOL_DISABLED) == 0;
}

wxString wxRibbonToolBar::GetToolHelpString(int toolId) const
{
    wxRibbonToolBarToolBase* tool = FindById(toolId);
    wxCHECK_MSG(tool != nullptr , wxEmptyString, "Invalid tool id");
    return tool->helpString;
}

wxRibbonButtonKind wxRibbonToolBar::GetToolKind(int toolId) const
{
    wxRibbonToolBarToolBase* tool = FindById(toolId);
    wxCHECK_MSG(tool != nullptr , wxRIBBON_BUTTON_NORMAL, "Invalid tool id");
    return tool->kind;
}

int wxRibbonToolBar::GetToolPos(int toolId) const
{
    size_t groupCount = m_groups.GetCount();
    size_t g, t;
    int pos = 0;
    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        size_t toolCount = group->tools.GetCount();
        for ( t = 0; t < toolCount; ++t )
        {
            wxRibbonToolBarToolBase* tool = group->tools.Item(t);
            if ( tool->id == toolId )
            {
                return pos;
            }
            ++pos;
        }
        ++pos; // Increment pos for group separator.
    }
    return wxNOT_FOUND;
}

wxRect wxRibbonToolBar::GetToolRect(int toolId) const
{
    size_t groupCount = m_groups.GetCount();
    size_t g, t;

    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        size_t toolCount = group->tools.GetCount();
        for ( t = 0; t < toolCount; ++t )
        {
            wxRibbonToolBarToolBase* tool = group->tools.Item(t);
            if ( tool->id == toolId )
            {
                return wxRect(group->position + tool->position, tool->size);
            }
        }
    }
    return wxRect();
}

bool wxRibbonToolBar::GetToolState(int toolId) const
{
    wxRibbonToolBarToolBase* tool = FindById(toolId);
    wxCHECK_MSG(tool != nullptr , false, "Invalid tool id");
    return (tool->state & wxRIBBON_TOOLBAR_TOOL_TOGGLED) != 0;
}

wxBitmap wxRibbonToolBar::MakeDisabledBitmap(const wxBitmap& original)
{
    wxImage img(original.ConvertToImage());
    return wxBitmap(img.ConvertToGreyscale(), -1, original.GetScaleFactor());
}

void wxRibbonToolBar::AppendGroup()
{
    wxRibbonToolBarToolGroup* group = new wxRibbonToolBarToolGroup;
    m_groups.Add(group);
}

bool wxRibbonToolBar::IsSizingContinuous() const
{
    return false;
}

void wxRibbonToolBar::SetToolClientData(int toolId, wxObject* clientData)
{
    wxRibbonToolBarToolBase* tool = FindById(toolId);
    wxCHECK_RET(tool != nullptr , "Invalid tool id");
    tool->clientData = clientData;
}

void wxRibbonToolBar::SetToolDisabledBitmap(int toolId, const wxBitmapBundle &bitmap)
{
    wxRibbonToolBarToolBase* tool = FindById(toolId);
    wxCHECK_RET(tool != nullptr , "Invalid tool id");
    tool->bitmapDisabled = bitmap;
}

void wxRibbonToolBar::SetToolHelpString(int toolId, const wxString& helpString)
{
    wxRibbonToolBarToolBase* tool = FindById(toolId);
    wxCHECK_RET(tool != nullptr , "Invalid tool id");
    tool->helpString = helpString;
}

void wxRibbonToolBar::SetToolNormalBitmap(int toolId, const wxBitmapBundle &bitmap)
{
    wxRibbonToolBarToolBase* tool = FindById(toolId);
    wxCHECK_RET(tool != nullptr , "Invalid tool id");
    tool->bitmap = bitmap;
}

void wxRibbonToolBar::EnableTool(int toolId, bool enable)
{
    wxRibbonToolBarToolBase* tool = FindById(toolId);
    wxCHECK_RET(tool != nullptr , "Invalid tool id");
    if ( enable )
    {
        if ( tool->state & wxRIBBON_TOOLBAR_TOOL_DISABLED )
        {
            tool->state &= ~wxRIBBON_TOOLBAR_TOOL_DISABLED;
            Refresh();
        }
    }
    else
    {
        if ( (tool->state & wxRIBBON_TOOLBAR_TOOL_DISABLED) == 0 )
        {
            tool->state |= wxRIBBON_TOOLBAR_TOOL_DISABLED;
            Refresh();
        }
    }
}

void wxRibbonToolBar::ToggleTool(int toolId, bool checked)
{
    wxRibbonToolBarToolBase* tool = FindById(toolId);
    wxCHECK_RET(tool != nullptr , "Invalid tool id");
    if ( checked )
    {
        if ( (tool->state & wxRIBBON_TOOLBAR_TOOL_TOGGLED) == 0 )
        {
            tool->state |= wxRIBBON_TOOLBAR_TOOL_TOGGLED;
            Refresh();
        }
    }
    else
    {
        if ( tool->state & wxRIBBON_TOOLBAR_TOOL_TOGGLED )
        {
            tool->state &= ~wxRIBBON_TOOLBAR_TOOL_TOGGLED;
            Refresh();
        }
    }
}

static int GetSizeInOrientation(wxSize size, wxOrientation orientation)
{
    switch ( orientation )
    {
    case wxHORIZONTAL: return size.GetWidth();
    case wxVERTICAL: return size.GetHeight();
    case wxBOTH: return size.GetWidth() * size.GetHeight();
    default: return 0;
    }
}

wxSize wxRibbonToolBar::DoGetNextSmallerSize(wxOrientation direction,
                                      wxSize relativeTo) const
{
    wxSize result(relativeTo);
    int area = 0;
    int nrows;
    for ( nrows = m_nrowsMin; nrows <= m_nrowsMax; ++nrows )
    {
        wxSize size(m_sizes[nrows - m_nrowsMin]);
        wxSize original(size);
        switch ( direction )
        {
        case wxHORIZONTAL:
            if ( size.GetWidth() < relativeTo.GetWidth()
                && size.GetHeight() <= relativeTo.GetHeight() )
            {
                size.SetHeight(relativeTo.GetHeight());
                break;
            }
            continue;
        case wxVERTICAL:
            if ( size.GetWidth() <= relativeTo.GetWidth()
                && size.GetHeight() < relativeTo.GetHeight() )
            {
                size.SetWidth(relativeTo.GetWidth());
                break;
            }
            continue;
        case wxBOTH:
            if ( size.GetWidth() < relativeTo.GetWidth()
                && size.GetHeight() < relativeTo.GetHeight() )
            {
                break;
            }
            continue;
        }
        if ( GetSizeInOrientation(original, direction) > area )
        {
            result = size;
            area = GetSizeInOrientation(original, direction);
        }
    }
    return result;
}

wxSize wxRibbonToolBar::DoGetNextLargerSize(wxOrientation direction,
                                 wxSize relativeTo) const
{
    // Pick the smallest of our sizes which are larger than the given size
    wxSize result(relativeTo);
    int area = INT_MAX;
    int nrows;
    for ( nrows = m_nrowsMin; nrows <= m_nrowsMax; ++nrows )
    {
        wxSize size(m_sizes[nrows - m_nrowsMin]);
        wxSize original(size);
        switch ( direction )
        {
        case wxHORIZONTAL:
            if ( size.GetWidth() > relativeTo.GetWidth()
                && size.GetHeight() <= relativeTo.GetHeight() )
            {
                size.SetHeight(relativeTo.GetHeight());
                break;
            }
            continue;
        case wxVERTICAL:
            if ( size.GetWidth() <= relativeTo.GetWidth()
                && size.GetHeight() > relativeTo.GetHeight() )
            {
                size.SetWidth(relativeTo.GetWidth());
                break;
            }
            continue;
        case wxBOTH:
            if ( size.GetWidth() > relativeTo.GetWidth()
                && size.GetHeight() > relativeTo.GetHeight() )
            {
                break;
            }
            continue;
        }
        if ( GetSizeInOrientation(original, direction) < area )
        {
            result = size;
            area = GetSizeInOrientation(original, direction);
        }
    }

    return result;
}

void wxRibbonToolBar::SetRows(int nMin, int nMax)
{
    if ( nMax == -1 )
        nMax = nMin;

    wxASSERT(1 <= nMin);
    wxASSERT(nMin <= nMax);

    m_nrowsMin = nMin;
    m_nrowsMax = nMax;

    delete[] m_sizes;
    m_sizes = new wxSize[m_nrowsMax - m_nrowsMin + 1];

    Realize();
}

bool wxRibbonToolBar::Realize()
{
    if ( m_art == nullptr )
        return false;

    // Calculate the size of each group and the position/size of each tool
    wxMemoryDC tempDc;
    size_t groupCount = m_groups.GetCount();
    size_t g, t;
    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolBase* prev = nullptr;
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        size_t toolCount = group->tools.GetCount();
        int tallest = 0;
        for ( t = 0; t < toolCount; ++t )
        {
            wxRibbonToolBarToolBase* tool = group->tools.Item(t);
            tool->size = m_art->GetToolSize(tempDc, this,
                tool->bitmap.GetPreferredLogicalSizeFor(this), tool->kind, t == 0,
                t == (toolCount - 1), &tool->dropdown);
            if ( t == 0 )
                tool->state |= wxRIBBON_TOOLBAR_TOOL_FIRST;
            if ( t == toolCount - 1 )
                tool->state |= wxRIBBON_TOOLBAR_TOOL_LAST;
            if ( tool->size.GetHeight() > tallest )
                tallest = tool->size.GetHeight();
            if ( prev )
            {
                tool->position = prev->position;
                tool->position.x += prev->size.x;
            }
            else
            {
                tool->position = wxPoint(0, 0);
            }
            prev = tool;
        }
        if ( toolCount == 0 )
            group->size = wxSize(0, 0);
        else
        {
            group->size = wxSize(prev->position.x + prev->size.x, tallest);
            for ( t = 0; t < toolCount; ++t )
                group->tools.Item(t)->size.SetHeight(tallest);
        }
    }

    // Calculate the minimum size for each possible number of rows
    int nrows, r;
    int sep = m_art->GetMetric(wxRIBBON_ART_TOOL_GROUP_SEPARATION_SIZE);
    int smallestArea = INT_MAX;
    wxSize* rowSizes = new wxSize[m_nrowsMax];
    wxOrientation majorAxis = m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL ?
        wxVERTICAL : wxHORIZONTAL;

    SetMinSize(wxSize(0, 0));
    wxSize minSize(INT_MAX, INT_MAX);

    // See if we're sizing flexibly (i.e. wrapping), and set min size differently
    bool sizingFlexibly = false;
    wxRibbonPanel* panel = wxDynamicCast(GetParent(), wxRibbonPanel);
    if ( panel && (panel->GetFlags() & wxRIBBON_PANEL_FLEXIBLE) )
        sizingFlexibly = true;

    // Without this, there will be redundant horizontal space because SetMinSize will
    // use the smallest possible height (and therefore largest width).
    if ( sizingFlexibly )
        majorAxis = wxHORIZONTAL;

    for ( nrows = m_nrowsMin; nrows <= m_nrowsMax; ++nrows )
    {
        for ( r = 0; r < nrows; ++r )
            rowSizes[r] = wxSize(0, 0);
        for ( g = 0; g < groupCount; ++g )
        {
            wxRibbonToolBarToolGroup* group = m_groups.Item(g);
            int shortest_row = 0;
            for ( r = 1; r < nrows; ++r )
            {
                if ( rowSizes[r].GetWidth() < rowSizes[shortest_row].GetWidth() )
                    shortest_row = r;
            }
            rowSizes[shortest_row].x += group->size.x + sep;
            if ( group->size.y > rowSizes[shortest_row].y )
                rowSizes[shortest_row].y = group->size.y;
        }
        wxSize size(0, 0);
        for ( r = 0; r < nrows; ++r )
        {
            if ( rowSizes[r].GetWidth() != 0 )
                rowSizes[r].DecBy(sep, 0);
            if ( rowSizes[r].GetWidth() > size.GetWidth() )
                size.SetWidth(rowSizes[r].GetWidth());
            size.IncBy(0, rowSizes[r].y);
        }
        m_sizes[nrows - m_nrowsMin] = size;

        if ( GetSizeInOrientation(size, majorAxis) < smallestArea )
        {
            smallestArea = GetSizeInOrientation(size, majorAxis);
            SetMinSize(size);
        }

        if ( sizingFlexibly )
        {
            if ( size.x < minSize.x )
                minSize.x = size.x;
            if ( size.y < minSize.y )
                minSize.y = size.y;
        }
    }

    if ( sizingFlexibly )
    {
        // Give it the min size in either direction regardless of row,
        // so that we're able to vary the size of the panel according to
        // the space the toolbar takes up.
        SetMinSize(minSize);
    }
    delete[] rowSizes;

    // Position the groups
    wxSizeEvent dummyEvent(GetSize());
    OnSize(dummyEvent);

    return true;
}

void wxRibbonToolBar::OnSize(wxSizeEvent& evt)
{
    if ( m_art == nullptr )
        return;

    // Choose row count with largest possible area
    wxSize size = evt.GetSize();
    int rowCount = m_nrowsMax;
    wxOrientation majorAxis = m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL ?
        wxVERTICAL : wxHORIZONTAL;

    // See if we're sizing flexibly, and set min size differently
    bool sizingFlexibly = false;
    wxRibbonPanel* panel = wxDynamicCast(GetParent(), wxRibbonPanel);
    if ( panel && (panel->GetFlags() & wxRIBBON_PANEL_FLEXIBLE) )
        sizingFlexibly = true;

    // Without this, there will be redundant horizontal space because SetMinSize will
    // use the smallest possible height (and therefore largest width).
    if ( sizingFlexibly )
        majorAxis = wxHORIZONTAL;

    if ( m_nrowsMax != m_nrowsMin )
    {
        int area = 0;
        for ( int i = 0; i <= m_nrowsMax - m_nrowsMin; ++i )
        {
            if ( m_sizes[i].x <= size.x && m_sizes[i].y <= size.y &&
                GetSizeInOrientation(m_sizes[i], majorAxis) > area )
            {
                area = GetSizeInOrientation(m_sizes[i], majorAxis);
                rowCount = m_nrowsMin + i;
            }
        }
    }

    // Assign groups to rows and calculate row widths
    wxSize* rowSizes = new wxSize[rowCount];
    int sep = m_art->GetMetric(wxRIBBON_ART_TOOL_GROUP_SEPARATION_SIZE);

    int r;
    size_t g;
    size_t groupCount = m_groups.GetCount();
    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        int shortest_row = 0;
        for ( r = 1; r < rowCount; ++r )
        {
            if ( rowSizes[r].GetWidth() < rowSizes[shortest_row].GetWidth() )
                shortest_row = r;
        }
        group->position = wxPoint(rowSizes[shortest_row].x, shortest_row);
        rowSizes[shortest_row].x += group->size.x + sep;
        if ( group->size.y > rowSizes[shortest_row].y )
            rowSizes[shortest_row].y = group->size.y;
    }

    // Calculate row positions
    int totalHeight = 0;
    for ( r = 0; r < rowCount; ++r )
        totalHeight += rowSizes[r].GetHeight();
    int rowsep = (size.GetHeight() - totalHeight) / (rowCount + 1);
    int* rowypos = new int[rowCount];
    rowypos[0] = rowsep;
    for ( r = 1; r < rowCount; ++r )
    {
        rowypos[r] = rowypos[r - 1] + rowSizes[r - 1].GetHeight() + rowsep;
    }

    // Set group y positions
    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        group->position.y = rowypos[group->position.y];
    }

    delete[] rowypos;
    delete[] rowSizes;
}

void wxRibbonToolBar::OnDPIChanged(wxDPIChangedEvent& event)
{
    Realize();
    event.Skip();
}

// Finds the best width and height given the parents' width and height
wxSize wxRibbonToolBar::GetBestSizeForParentSize(const wxSize& parentSize) const
{
    if ( !m_sizes )
        return GetMinSize();

    // Choose row count with largest possible area
    wxSize size = parentSize;

    // A toolbar should maximize its width whether vertical or horizontal, so
    // force the major axis to be horizontal. Without this, there will be
    // redundant horizontal space.
    wxOrientation majorAxis = wxHORIZONTAL;
    wxSize bestSize = m_sizes[0];

    if ( m_nrowsMax != m_nrowsMin )
    {
        int area = 0;
        for ( int i = 0; i <= m_nrowsMax - m_nrowsMin; ++i )
        {
            if ( m_sizes[i].x <= size.x && m_sizes[i].y <= size.y &&
                GetSizeInOrientation(m_sizes[i], majorAxis) > area )
            {
                area = GetSizeInOrientation(m_sizes[i], majorAxis);
                bestSize = m_sizes[i];
            }
        }
    }
    return bestSize;
}

wxSize wxRibbonToolBar::DoGetBestSize() const
{
    return GetMinSize();
}

void wxRibbonToolBar::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // All painting done in main paint handler to minimise flicker
}

void wxRibbonToolBar::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxAutoBufferedPaintDC dc(this);
    if ( m_art == nullptr )
        return;

    m_art->DrawToolBarBackground(dc, this, GetSize());

    size_t groupCount = m_groups.GetCount();
    size_t g, t;
    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        size_t toolCount = group->tools.GetCount();
        if ( toolCount != 0 )
        {
            m_art->DrawToolGroupBackground(dc, this,
                wxRect(group->position, group->size));
            for ( t = 0; t < toolCount; ++t )
            {
                wxRibbonToolBarToolBase* tool = group->tools.Item(t);
                wxRect rect(group->position + tool->position, tool->size);
                // Resolve bitmap bundle to actual bitmap for current DPI
                wxBitmap bmp;
                if ( tool->state & wxRIBBON_TOOLBAR_TOOL_DISABLED )
                    bmp = tool->bitmapDisabled.GetBitmapFor(this);
                else
                    bmp = tool->bitmap.GetBitmapFor(this);
                m_art->DrawTool(dc, this, rect, bmp, tool->kind, tool->state);
            }
        }
    }
}

void wxRibbonToolBar::OnMouseMove(wxMouseEvent& evt)
{
    wxPoint pos(evt.GetPosition());
    wxRibbonToolBarToolBase *newHover = nullptr;

    size_t groupCount = m_groups.GetCount();
    size_t g, t;
    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        if ( group->position.x <= pos.x && pos.x < group->position.x + group->size.x
            && group->position.y <= pos.y && pos.y < group->position.y + group->size.y )
        {
            size_t toolCount = group->tools.GetCount();
            pos -= group->position;
            for ( t = 0; t < toolCount; ++t )
            {
                wxRibbonToolBarToolBase* tool = group->tools.Item(t);
                if ( tool->position.x <= pos.x && pos.x < tool->position.x + tool->size.x
                    && tool->position.y <= pos.y && pos.y < tool->position.y + tool->size.y )
                {
                    pos -= tool->position;
                    newHover = tool;
                    break;
                }
            }
            break;
        }
    }

#if wxUSE_TOOLTIPS
    if ( newHover )
    {
        if ( newHover != m_hoverTool &&
                !(newHover->state & wxRIBBON_TOOLBAR_TOOL_DROPDOWN_ACTIVE) )
            SetToolTip(newHover->helpString);
    }
    else if ( GetToolTip() )
    {
        UnsetToolTip();
    }
#endif

    if ( newHover != m_hoverTool )
    {
        if ( m_hoverTool )
        {
            m_hoverTool->state &= ~(wxRIBBON_TOOLBAR_TOOL_HOVER_MASK
                | wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK);
        }
        m_hoverTool = newHover;

        if ( newHover && newHover->state & wxRIBBON_TOOLBAR_TOOL_DISABLED )
            newHover = nullptr; // A disabled tool can not be highlighted

        if ( newHover )
        {
            long what = wxRIBBON_TOOLBAR_TOOL_NORMAL_HOVERED;
            if ( newHover->dropdown.Contains(pos) )
                what = wxRIBBON_TOOLBAR_TOOL_DROPDOWN_HOVERED;

            newHover->state |= what;

            if ( newHover == m_activeTool )
            {
                newHover->state &= ~wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK;
                newHover->state |= (what << 2);
            }
        }
        Refresh(false);
    }
    else if ( m_hoverTool && m_hoverTool->kind == wxRIBBON_BUTTON_HYBRID )
    {
        long newState = m_hoverTool->state &~wxRIBBON_TOOLBAR_TOOL_HOVER_MASK;
        long what = wxRIBBON_TOOLBAR_TOOL_NORMAL_HOVERED;
        if ( m_hoverTool->dropdown.Contains(pos) )
            what = wxRIBBON_TOOLBAR_TOOL_DROPDOWN_HOVERED;
        newState |= what;
        if ( newState != m_hoverTool->state )
        {
            m_hoverTool->state = newState;
            if ( m_hoverTool == m_activeTool )
            {
                m_hoverTool->state &= ~wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK;
                m_hoverTool->state |= (what << 2);
            }
            Refresh(false);
        }
    }
}

void wxRibbonToolBar::OnMouseDown(wxMouseEvent& evt)
{
    OnMouseMove(evt);
    if ( m_hoverTool )
    {
        m_activeTool = m_hoverTool;
        m_activeTool->state |=
            (m_activeTool->state & wxRIBBON_TOOLBAR_TOOL_HOVER_MASK) << 2;
        UnsetToolTip();
        Refresh(false);
    }
}

void wxRibbonToolBar::OnMouseLeave(wxMouseEvent& WXUNUSED(evt))
{
    if ( m_hoverTool )
    {
        m_hoverTool->state &= ~wxRIBBON_TOOLBAR_TOOL_HOVER_MASK;
        m_hoverTool = nullptr;
        Refresh(false);
    }
}

void wxRibbonToolBar::OnMouseUp(wxMouseEvent& WXUNUSED(evt))
{
    if ( m_activeTool )
    {
        if ( m_activeTool->state & wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK )
        {
            wxEventType evtType = wxEVT_RIBBONTOOLBAR_CLICKED;
            if ( m_activeTool->state & wxRIBBON_TOOLBAR_TOOL_DROPDOWN_ACTIVE )
                evtType = wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED;
            wxRibbonToolBarEvent notification(evtType, m_activeTool->id);
            if ( m_activeTool->kind == wxRIBBON_BUTTON_TOGGLE )
            {
                m_activeTool->state ^=
                    wxRIBBON_TOOLBAR_TOOL_TOGGLED;
                notification.SetInt(m_activeTool->state &
                    wxRIBBON_TOOLBAR_TOOL_TOGGLED);
            }
            notification.SetEventObject(this);
            notification.SetBar(this);
            ProcessEvent(notification);

            if ( auto* const panel = wxCheckedStaticCast<wxRibbonPanel>(GetParent()) )
                panel->HideIfExpanded();
        }

        // Notice that m_activeTool could have been reset by the event handler
        // above so we need to test it again.
        if ( m_activeTool )
        {
            m_activeTool->state &= ~wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK;
            m_activeTool = nullptr;
            Refresh(false);
        }
    }
}

void wxRibbonToolBar::OnMouseEnter(wxMouseEvent& evt)
{
    if ( m_activeTool && !evt.LeftIsDown() )
    {
        m_activeTool->state &= ~wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK;
        m_activeTool = nullptr;
    }
}

void wxRibbonToolBar::UpdateWindowUI(long flags)
{
    wxWindowBase::UpdateWindowUI(flags);

    // don't waste time updating state of tools in a hidden toolbar
    if ( !IsShown() )
        return;

    size_t groupCount = m_groups.GetCount();
    size_t g, t;
    for ( g = 0; g < groupCount; ++g )
    {
        wxRibbonToolBarToolGroup* group = m_groups.Item(g);
        size_t toolCount = group->tools.GetCount();
        for ( t = 0; t < toolCount; ++t )
        {
            wxRibbonToolBarToolBase* tool = group->tools.Item(t);
            int id = tool->id;

            wxUpdateUIEvent event(id);
            event.SetEventObject(this);

            if ( ProcessWindowEvent(event) )
            {
                if ( event.GetSetEnabled() )
                    EnableTool(id, event.GetEnabled());
                if ( event.GetSetChecked() )
                    ToggleTool(id, event.GetChecked());
            }
        }
    }
}

bool wxRibbonToolBarEvent::PopupMenu(wxMenu* menu)
{
    wxPoint pos = wxDefaultPosition;
    if ( m_bar->m_activeTool )
    {
        // Find the group which contains the tool
        size_t groupCount = m_bar->m_groups.GetCount();
        size_t g, t;
        for ( g = 0; g < groupCount; ++g )
        {
            wxRibbonToolBarToolGroup* group = m_bar->m_groups.Item(g);
            size_t toolCount = group->tools.GetCount();
            for ( t = 0; t < toolCount; ++t )
            {
                wxRibbonToolBarToolBase* tool = group->tools.Item(t);
                if ( tool == m_bar->m_activeTool )
                {
                    pos = group->position;
                    pos += tool->position;
                    pos.y += tool->size.GetHeight();
                    g = groupCount;
                    break;
                }
            }
        }
    }
    return m_bar->PopupMenu(menu, pos);
}

#endif // wxUSE_RIBBON
