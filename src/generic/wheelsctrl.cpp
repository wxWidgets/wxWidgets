/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/wheelsctrl.cpp
// Purpose:     Common wxWheelsCtrl parts
// Author:      Julian Smart, Linas Valiukas
// Created:     2011-08-19
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/sizer.h"
#endif

#include "wx/wheelsctrl.h"
#include "wx/arrimpl.cpp"


#pragma mark wxWheelsCtrlEvent

IMPLEMENT_DYNAMIC_CLASS(wxWheelsCtrlEvent, wxNotifyEvent)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_WHEEL_SELECTED)


WX_DEFINE_EXPORTED_OBJARRAY(wxArrayOfStringArrays);
WX_DEFINE_EXPORTED_OBJARRAY(wxSizeArray);


#pragma mark wxWheelsDataSource

IMPLEMENT_CLASS(wxWheelsDataSource, wxEvtHandler)


#pragma mark wxWheelsTextDataSource

IMPLEMENT_CLASS(wxWheelsTextDataSource, wxWheelsDataSource)

wxWheelsTextDataSource::wxWheelsTextDataSource(int componentCount,
                                               const wxSize& rowSize)
{
    Init();
    
    SetComponentCount(componentCount);
    
    for (int i = 0; i < componentCount; i++) {
        SetRowSize(rowSize, i);
    }
}

void wxWheelsTextDataSource::Init()
{
    m_componentCount = 1;
    m_stringArrays.Add(wxArrayString());
    m_rowSizes.Add(wxSize(-1, -1));
}

int wxWheelsTextDataSource::GetNumberOfComponents(wxWheelsCtrl* WXUNUSED(ctrl))
{
    return m_componentCount;
}

void wxWheelsTextDataSource::SetComponentCount(int count)
{
    int i;
    int arrayCount = m_stringArrays.GetCount();
    
    if (count > arrayCount) {
        int toAdd = count - arrayCount;
        for (i = 0; i < toAdd; i++) {
            m_stringArrays.Add(wxArrayString());
            m_rowSizes.Add(wxSize(-1, -1));
        }
    } else if (count < arrayCount) {
        int toDelete = arrayCount - count;
        for (i = 0; i < toDelete; i++) {
            m_stringArrays.RemoveAt(0);
            m_rowSizes.RemoveAt(0);
        }
    }
    
    m_componentCount = count;
}

int wxWheelsTextDataSource::GetComponentCount() const
{
    return m_componentCount;
}

wxArrayString wxWheelsTextDataSource::GetStrings(int component) const
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount()) {
        return m_stringArrays[component];
    } else {
        return wxArrayString();
    }
}

void wxWheelsTextDataSource::SetStrings(const wxArrayString& strings,
                                        int component)
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount()) {
        m_stringArrays[component] = strings;
    }
}

wxSize wxWheelsTextDataSource::GetRowSize(int component) const
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount()) {
        return m_rowSizes[component];
    } else {
        return wxSize(0, 0);
    }
}

void wxWheelsTextDataSource::SetRowSize(const wxSize& sz,
                                        int component)
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount()) {
        m_rowSizes[component] = sz;
    }
}

// Override to provide the number of rows in a component.
int wxWheelsTextDataSource::GetNumberOfRowsForComponent(wxWheelsCtrl* WXUNUSED(ctrl),
                                                        int component)
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount()) {
        return (int) m_stringArrays[component].GetCount();
    } else {
        return 0;
    }
}

// Override to provide the title for a component's row. If you override GetRowWindow instead,
// this method is optional.
wxString wxWheelsTextDataSource::GetRowTitle(wxWheelsCtrl* WXUNUSED(ctrl),
                                             int component,
                                             int row)
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount()) {
        wxASSERT(row < (int) m_stringArrays[component].GetCount());
        if (row < (int) m_stringArrays[component].GetCount()) {
            return m_stringArrays[component][row];
        }
    }
    
    return wxEmptyString;
}

// Override to provide the row height for a component. Should returns a height in pixels.
int wxWheelsTextDataSource::GetRowHeightForComponent(wxWheelsCtrl* ctrl,
                                                     int component)
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount()) {
        if (GetRowSize(component).y > 0) {
            return GetRowSize(component).y;
        } else {
            wxScreenDC dc;
            dc.SetFont(ctrl->GetFont());
            int w, h;
            dc.GetTextExtent(wxT("Xy"), & w, & h);
            return h;
        }
    }
    
    return 0;
}

// Override to provide the row width for a component. Should returns a width in pixels.
int wxWheelsTextDataSource::GetRowWidthForComponent(wxWheelsCtrl* ctrl,
                                                    int component)
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount()) {
        if (GetRowSize(component).y > 0) {
            return GetRowSize(component).y;
        } else {
            // Find the longest width
            int rowWidth = 0;
            int margin = 2;
            int i;
            int rowCount = GetNumberOfRowsForComponent(ctrl, component);
            wxScreenDC dc;
            dc.SetFont(ctrl->GetFont());
            for (i = 0; i < rowCount; i++) {
                wxString str = GetRowTitle(ctrl, component, i);
                
                int w, h;
                dc.GetTextExtent(str, & w, & h);
                
                if (w > rowWidth) {
                    rowWidth = w;
                }
            }
            
            rowWidth += margin*2;
            return rowWidth;
        }
    }
    
    return 0;
}


#pragma mark wxWheelsDateTimeDataSource

IMPLEMENT_CLASS(wxWheelsDateTimeDataSource, wxWheelsDataSource)


#pragma mark wxWheelComponent

IMPLEMENT_CLASS(wxWheelComponent, wxEvtHandler)
