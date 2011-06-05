/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_wheelsctrl_g.cpp
// Purpose:     wxMoWheelsCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcclient.h"
#include "wx/dcscreen.h"
#include "wx/settings.h"
#include "wx/dcbuffer.h"

#include "wx/mobile/generic/wheelsctrl.h"
#include "wx/mobile/generic/utils.h"

#include "wx/arrimpl.cpp"

WX_DEFINE_EXPORTED_OBJARRAY(wxMoArrayOfStringArrays);
WX_DEFINE_EXPORTED_OBJARRAY(wxMoSizeArray);

extern WXDLLEXPORT_DATA(const wxChar) wxWheelsCtrlNameStr[] = wxT("wheelsctrl");

IMPLEMENT_DYNAMIC_CLASS(wxMoWheelsCtrl, wxControl)

BEGIN_EVENT_TABLE(wxMoWheelsCtrl, wxControl)
    EVT_SIZE(wxMoWheelsCtrl::OnSize)
    EVT_PAINT(wxMoWheelsCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(wxMoWheelsCtrl::OnEraseBackground)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxWheelsCtrlEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_WHEEL_SELECTED)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxMoWheelsCtrl::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_WHEELS_BG));
    SetForegroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_WHEELS_TEXT));
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_NORMAL_BUTTON));

    SetInitialSize(size);

    return true;
}

wxMoWheelsCtrl::~wxMoWheelsCtrl()
{
    ClearComponents();
    if (m_dataSource && m_ownDataSource)
    {
        delete m_dataSource;
        m_dataSource = NULL;
    }
}

void wxMoWheelsCtrl::Init()
{
    m_showRowSelection = true;
    m_dataSource = NULL;
    m_ownDataSource = false;

    m_leftMargin = 4;
    m_rightMargin = 4;
    m_topMargin = 2;
    m_bottomMargin = 2;
    m_interWheelSpacing = 2*3 + 2;
}

void wxMoWheelsCtrl::ClearComponents()
{
    // TODO

    int n = m_components.GetCount();
    int i;

    // Process any display events
    Update();

    for (i = n-1; i >= 0; i--)
    {
        wxMoWheelComponent* component = GetComponent(i);
        delete component->GetListBox();
        component->SetListBox(NULL);
    }

    // Process any display events
    Update();

    for (i = 0; i < n; i++)
    {
        wxMoWheelComponent* component = GetComponent(i);
        delete component;
    }

    m_components.Clear();
}

bool wxMoWheelsCtrl::SendClickEvent()
{
    // TODO
#if 0
    wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, GetId());
    event.SetInt(GetValue() ? 1 : 0);
    event.SetEventObject(this);

    return ProcessCommand(event);
#endif
    return false;
}

bool wxMoWheelsCtrl::SetBackgroundColour(const wxColour &colour)
{
    if ( !wxControl::SetBackgroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    int n = m_components.GetCount();
    for (int component = 0; component < n; component ++)
    {
        wxMoWheelComponent* comp = m_components[component];
        if (comp->GetListBox())
            comp->GetListBox()->SetBackgroundColour(colour);
    }
    
    Refresh();
    
    return true;
}

bool wxMoWheelsCtrl::SetForegroundColour(const wxColour &colour)
{
    if ( !wxControl::SetForegroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    int n = m_components.GetCount();
    for (int component = 0; component < n; component ++)
    {
        wxMoWheelComponent* comp = m_components[component];
        if (comp->GetListBox())
            comp->GetListBox()->SetForegroundColour(colour);
    }

    Refresh();
    return true;
}

bool wxMoWheelsCtrl::Enable(bool enable)
{
    wxControl::Enable(enable);
    Refresh();
    return true;
}

bool wxMoWheelsCtrl::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);

    int n = m_components.GetCount();
    for (int component = 0; component < n; component ++)
    {
        wxMoWheelComponent* comp = m_components[component];
        if (comp->GetListBox())
            comp->GetListBox()->SetFont(font);
    }

    Refresh();

    return true;
}

// Reloads and displays all components.
bool wxMoWheelsCtrl::ReloadAllComponents()
{
    int n = GetDataSource()->GetNumberOfComponents(this);
    for (int component = 0; component < n; component ++)
    {
        ReloadComponent(component);
    }

    return true;
}

// Reloads and displays the specified component.
bool wxMoWheelsCtrl::ReloadComponent(int component)
{
    InvalidateBestSize();

    if (GetDataSource() && component < (int) m_components.GetCount())
    {
        wxMoWheelComponent* c = m_components[component];
        if (c && c->GetListBox())
        {
            int rowCount = GetDataSource()->GetNumberOfRowsForComponent(this, component);
            c->GetListBox()->SetItemCount(rowCount);
            c->GetListBox()->Refresh();
        }

        ResizeComponents();
    }

    return true;
}

// Sets the selection (row) in the given component.
void wxMoWheelsCtrl::SetSelection(int component, int selection)
{
    if (GetDataSource() && component < (int) m_components.GetCount())
    {
        wxMoWheelComponent* c = m_components[component];
        if (c)
        {
            if (c->GetListBox())
                c->GetListBox()->SetSelection(selection);
        }
    }
}

// Gets the selection for the given component.
int wxMoWheelsCtrl::GetSelection(int component) const
{
    if (GetDataSource() && component < (int) m_components.GetCount())
    {
        wxMoWheelComponent* c = m_components[component];
        if (c)
        {
            if (c->GetListBox())
                return c->GetListBox()->GetSelection();
        }
    }
    return -1;
}

// Returns the number of rows in the given component.
int wxMoWheelsCtrl::GetComponentRowCount(int component) const
{
    if (GetDataSource())
    {
        int rows = GetDataSource()->GetNumberOfRowsForComponent(const_cast<wxMoWheelsCtrl*>(this), component);
        return rows;
    }
    else
        return 0;
}

// Returns the number of components.
int wxMoWheelsCtrl::GetComponentCount() const
{
    if (GetDataSource())
    {
        int n = GetDataSource()->GetNumberOfComponents(const_cast<wxMoWheelsCtrl*>(this));
        return n;
    }
    else
        return 0;
}

// Returns the size required to display the largest view in the given component.
wxSize wxMoWheelsCtrl::GetRowSizeForComponent(int component) const
{
    if (GetDataSource())
    {
        int width = GetDataSource()->GetRowWidthForComponent(const_cast<wxMoWheelsCtrl*>(this), component);
        int height = GetDataSource()->GetRowHeightForComponent(const_cast<wxMoWheelsCtrl*>(this), component);

        return wxSize(width, height);
    }
    else
        return wxSize(0, 0);
}

// Sets the data source object, recreating the components.
void wxMoWheelsCtrl::SetDataSource(wxMoWheelsDataSource* dataSource, bool ownsDataSource)
{
    if (m_dataSource && m_dataSource != dataSource && m_ownDataSource)
        delete m_dataSource;

    m_ownDataSource = ownsDataSource;
    m_dataSource = dataSource;

    InvalidateBestSize();
    InitializeComponents();
}

// Initializes the components from the current data source
bool wxMoWheelsCtrl::InitializeComponents()
{
    ClearComponents();

    if (GetDataSource())
    {
        int n = GetDataSource()->GetNumberOfComponents(this);
        for (int component = 0; component < n; component ++)
        {
            wxMoWheelsListBox* listBox = new wxMoWheelsListBox(this, wxID_ANY);
            listBox->SetFont(GetFont());
            listBox->SetForegroundColour(GetForegroundColour());
            listBox->SetBackgroundColour(GetBackgroundColour());

            // listBox->Show(false);
            listBox->SetComponentNumber(component);
            listBox->SetWheelsCtrl(this);
            
            wxMoWheelComponent* comp = new wxMoWheelComponent(this, listBox, component);
            m_components.Add(comp);

            int rowCount = GetDataSource()->GetNumberOfRowsForComponent(this, component);
            listBox->SetItemCount(rowCount);
            listBox->Refresh();
        }

        ResizeComponents();
    }

    return true;
}

void wxMoWheelsCtrl::ResizeComponents()
{
    if (GetDataSource())
    {
        int n = GetDataSource()->GetNumberOfComponents(this);
        // Size the components
        wxSize sz(GetClientSize());
        int x = m_leftMargin;
        for (int component = 0; component < n; component ++)
        {
            wxMoWheelComponent* comp = m_components[component];
            int width = GetDataSource()->GetRowWidthForComponent(this, component);

            // If the whole control is wider than the last component, increase the
            // size of this component.
            if (component == (n-1))
            {
                if ((x + width + m_rightMargin) < sz.x)
                    width = sz.x - m_rightMargin - x;
            }

            int height = sz.y - m_topMargin - m_bottomMargin;
            comp->GetListBox()->SetSize(x, m_topMargin, width, height);

            x += width;
            if (component < (n-1))
                x += m_interWheelSpacing;
        }
    }
}

void wxMoWheelsCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    ResizeComponents();
}

void wxMoWheelsCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif

    wxSize clientSize(GetClientSize());

    wxColour bgColour = GetParent()->GetBackgroundColour();

    dc.SetBackground(wxBrush(bgColour));
    dc.Clear();

    wxColour lightGrey(wxMoSystemSettings::GetColour(wxMO_COLOUR_WHEELS_LIGHT_GREY)); // 200, 200, 200);
    wxPen lightGreyPen(lightGrey);
    wxBrush lightGreyBrush(lightGrey);

    // Left of first component
    dc.SetBrush(lightGreyBrush);
    dc.SetPen(lightGreyPen);

    int bezelSize = 4;

#if 1
    wxPoint leftEdgePoints[4];
    leftEdgePoints[0] = wxPoint(0, bezelSize);
    leftEdgePoints[1] = wxPoint(m_leftMargin, m_topMargin);
    leftEdgePoints[2] = wxPoint(m_leftMargin, clientSize.y-m_bottomMargin);
    leftEdgePoints[3] = wxPoint(0, clientSize.y-m_bottomMargin-bezelSize);
    dc.DrawPolygon(4, leftEdgePoints);
    
    dc.SetPen(*wxBLACK);
    dc.DrawLine(0, bezelSize+2, 0, clientSize.y - bezelSize - 2);
#else
    wxRect edgeRect(wxPoint(0, m_topMargin),
        wxSize(3, GetSize().y - m_topMargin - m_bottomMargin));
    dc.DrawRectangle(edgeRect);
#endif

    int n = m_components.GetCount();
    for (int component = 0; component < n; component ++)
    {
        wxMoWheelComponent* comp = m_components[component];
        if (comp->GetListBox())
        {
            wxRect listBoxRect = comp->GetListBox()->GetRect();
            if (component < (n-1))
            {
                // Black filling between components
                dc.SetBrush(*wxBLACK_BRUSH);
                dc.SetPen(*wxBLACK_PEN);
                wxRect barRect(wxPoint(listBoxRect.GetRight()+1, listBoxRect.y),
                    wxSize(m_interWheelSpacing, listBoxRect.height));
                dc.DrawRectangle(barRect);

                // Left of next component
                dc.SetBrush(lightGreyBrush);
                dc.SetPen(lightGreyPen);
                wxRect edgeRect(wxPoint(listBoxRect.GetRight()+1+3+2, listBoxRect.y),
                    wxSize(3, listBoxRect.height));
                dc.DrawRectangle(edgeRect);

                // Right of this component
                dc.SetBrush(lightGreyBrush);
                dc.SetPen(lightGreyPen);
                wxRect edgeRect2(wxPoint(listBoxRect.GetRight()+1, listBoxRect.y),
                    wxSize(3, listBoxRect.height));
                dc.DrawRectangle(edgeRect2);
            }
        }
    }

    dc.SetPen(lightGreyPen);

    wxPoint rightEdgePoints[4];
    rightEdgePoints[0] = wxPoint(clientSize.x-1, bezelSize);
    rightEdgePoints[1] = wxPoint(clientSize.x-m_rightMargin-1, m_topMargin);
    rightEdgePoints[2] = wxPoint(clientSize.x-m_rightMargin-1, clientSize.y-m_bottomMargin);
    rightEdgePoints[3] = wxPoint(clientSize.x-1, clientSize.y-m_bottomMargin-bezelSize);
    dc.DrawPolygon(4, rightEdgePoints);
    
    dc.SetPen(*wxBLACK);
    dc.DrawLine(clientSize.x-1, bezelSize+2, clientSize.x-1, clientSize.y - bezelSize - 2);

    // Top and bottom line
    dc.SetPen(lightGreyPen);
    dc.DrawLine(m_leftMargin, m_topMargin-1, clientSize.x - m_rightMargin, m_topMargin-1);
    dc.DrawLine(m_leftMargin, clientSize.y-m_topMargin, clientSize.x - m_rightMargin, clientSize.y-m_topMargin);
}

void wxMoWheelsCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

wxSize wxMoWheelsCtrl::DoGetBestSize() const
{
    if (GetDataSource())
    {
        int n = GetDataSource()->GetNumberOfComponents(const_cast<wxMoWheelsCtrl*>(this));
        //wxSize sz(GetClientSize());
        wxSize bestSize(0, 100);
        for (int component = 0; component < n; component ++)
        {
            int width = GetDataSource()->GetRowWidthForComponent(const_cast<wxMoWheelsCtrl*>(this), component);
            bestSize.x += width;
            if (component < (n-1))
                bestSize.x += m_interWheelSpacing;
        }
        bestSize.x += (m_leftMargin + m_rightMargin);

        CacheBestSize(bestSize);
        return bestSize;
    }
    else
        return wxSize(100, 100);
}

IMPLEMENT_CLASS(wxMoWheelsDataSource, wxEvtHandler)

// Override to intercept row click; by default, sends an event to itself and then to control.
bool wxMoWheelsDataSource::OnSelectRow(wxMoWheelsCtrl* ctrl, int component, int row)
{
    wxWheelsCtrlEvent event(wxEVT_COMMAND_WHEEL_SELECTED, ctrl->GetId(), component, row);
    event.SetEventObject(this);

    if (!ProcessEvent(event))
        return ctrl->GetEventHandler()->ProcessEvent(event);
    else
        return true;
}

// Override to provide the row height for a component. Should returns a height in pixels.
int wxMoWheelsDataSource::GetRowHeightForComponent(wxMoWheelsCtrl* WXUNUSED(ctrl), int WXUNUSED(component))
{
    return 0;
}

// Override to provide the row width for a component. Should returns a width in pixels.
int wxMoWheelsDataSource::GetRowWidthForComponent(wxMoWheelsCtrl* WXUNUSED(ctrl), int WXUNUSED(component))
{
    return 0;
}

IMPLEMENT_CLASS(wxMoWheelsTextDataSource, wxMoWheelsDataSource)

wxMoWheelsTextDataSource::wxMoWheelsTextDataSource(int componentCount, const wxSize& rowSize)
{
    Init();

    SetComponentCount(componentCount);

    for (int i = 0; i < componentCount; i++)
    {
        SetRowSize(rowSize, i);
    }
}

void wxMoWheelsTextDataSource::Init()
{
    m_componentCount = 1;
    m_stringArrays.Add(wxArrayString());
    m_rowSizes.Add(wxSize(-1, -1));
}

void wxMoWheelsTextDataSource::SetComponentCount(int count)
{
    int i;
    int arrayCount = m_stringArrays.GetCount();
    if (count > arrayCount)
    {
        int toAdd = count - arrayCount;
        for (i = 0; i < toAdd; i++)
        {
            m_stringArrays.Add(wxArrayString());
            m_rowSizes.Add(wxSize(-1, -1));
        }
    }
    else if (count < arrayCount)
    {
        int toDelete = arrayCount - count;
        for (i = 0; i < toDelete; i++)
        {
            m_stringArrays.RemoveAt(0);
            m_rowSizes.RemoveAt(0);
        }
    }

    m_componentCount = count;
}

wxArrayString wxMoWheelsTextDataSource::GetStrings(int component) const
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount())
        return m_stringArrays[component];
    else
        return wxArrayString();
}

void wxMoWheelsTextDataSource::SetStrings(const wxArrayString& strings, int component)
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount())
        m_stringArrays[component] = strings;
}

wxSize wxMoWheelsTextDataSource::GetRowSize(int component) const
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount())
        return m_rowSizes[component];
    else
        return wxSize(0, 0);
}

void wxMoWheelsTextDataSource::SetRowSize(const wxSize& sz, int component)
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount())
        m_rowSizes[component] = sz;
}

// Override to provide the number of rows in a component.
int wxMoWheelsTextDataSource::GetNumberOfRowsForComponent(wxMoWheelsCtrl* WXUNUSED(ctrl), int component)
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount())
        return (int) m_stringArrays[component].GetCount();
    else
        return 0;
}

// Override to provide the title for a component's row. If you override GetRowWindow instead,
// this method is optional.
wxString wxMoWheelsTextDataSource::GetRowTitle(wxMoWheelsCtrl* WXUNUSED(ctrl), int component, int row)
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount())
    {
        wxASSERT(row < (int) m_stringArrays[component].GetCount());
        if (row < (int) m_stringArrays[component].GetCount())
            return m_stringArrays[component][row];
    }
    return wxEmptyString;
}

// Override to provide the row height for a component. Should returns a height in pixels.
int wxMoWheelsTextDataSource::GetRowHeightForComponent(wxMoWheelsCtrl* ctrl, int component)
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount())
    {
        if (GetRowSize(component).y > 0)
            return GetRowSize(component).y;
        else
        {
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
int wxMoWheelsTextDataSource::GetRowWidthForComponent(wxMoWheelsCtrl* ctrl, int component)
{
    wxASSERT(component < GetComponentCount());
    if (component < GetComponentCount())
    {
        if (GetRowSize(component).y > 0)
            return GetRowSize(component).y;
        else
        {
            // Find the longest width
            int rowWidth = 0;
            int margin = 2;
            int i;
            int rowCount = GetNumberOfRowsForComponent(ctrl, component);
            wxScreenDC dc;
            dc.SetFont(ctrl->GetFont());
            for (i = 0; i < rowCount; i++)
            {
                wxString str = GetRowTitle(ctrl, component, i);
                
                int w, h;
                dc.GetTextExtent(str, & w, & h);
                
                if (w > rowWidth)
                    rowWidth = w;
            }
            
            rowWidth += margin*2;
            return rowWidth;
        }
    }
    return 0;
}

IMPLEMENT_CLASS(wxMoWheelComponent, wxEvtHandler)

void wxMoWheelComponent::Init()
{
    m_listBox = NULL;
    m_componentNumber = 0;
    m_wheelsCtrl = NULL;
}

IMPLEMENT_CLASS(wxMoWheelsListBox, wxMoVListBox)

// Creation function.
bool wxMoWheelsListBox::Create(wxWindow *parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxString& name)
{
    Init();

    wxMoVListBox::Create(parent, id, pos, size, style, name);

    SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_NORMAL_BUTTON));
    SetInitialSize(size);

    SetShowHorizontalScrollIndicator(false);
    SetShowVerticalScrollIndicator(false);

    return true;
}

// common part of all ctors
void wxMoWheelsListBox::Init()
{
    m_wheelsCtrl = NULL;
    m_componentNumber = -1;
}

// the derived class must implement this function to actually draw the item
// with the given index on the provided DC
void wxMoWheelsListBox::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
{
    if (GetWheelsCtrl() && GetWheelsCtrl()->GetDataSource())
    {
        wxString title = GetWheelsCtrl()->GetDataSource()->GetRowTitle(GetWheelsCtrl(), GetComponentNumber(), (int) n);
        if (!title.IsEmpty())
        {
            int margin = 2;
            dc.SetFont(GetFont());
            dc.SetTextForeground(GetForegroundColour());
            dc.SetBackgroundMode(wxTRANSPARENT);
            int w, h;
            dc.GetTextExtent(title, & w, & h);
            h += margin;
            
            int x = rect.x + margin;
            int y = rect.y + (rect.height - h)/2;
            dc.DrawText(title, x, y);            
        }
    }
}

// the derived class must implement this method to return the height of the
// specified item
wxCoord wxMoWheelsListBox::OnMeasureItem(size_t WXUNUSED(n)) const
{
    if (GetWheelsCtrl() && GetWheelsCtrl()->GetDataSource())
    {
        int height = GetWheelsCtrl()->GetDataSource()->GetRowHeightForComponent(GetWheelsCtrl(), GetComponentNumber());

        return (wxCoord) height;
    }
    return 0;
}

wxMoWheelComponent* wxMoWheelsListBox::GetComponent() const
{
    if (GetWheelsCtrl() && m_componentNumber > -1)
    {
        wxMoWheelComponent* component = GetWheelsCtrl()->GetComponent(m_componentNumber);
        return component;
    }
    return NULL;
}

// usually overridden base class virtuals
wxSize wxMoWheelsListBox::DoGetBestSize() const
{
    return wxSize(10, 10);
}

void wxMoWheelsListBox::OnSelect(wxCommandEvent& event)
{
    int selection = event.GetSelection();
    if (GetWheelsCtrl() && GetWheelsCtrl()->GetDataSource())
    {
        GetWheelsCtrl()->GetDataSource()->OnSelectRow(GetWheelsCtrl(), GetComponentNumber(), selection);
    }
}

void wxMoWheelsListBox::DrawBackground(wxDC& dc)
{
    wxSize clientSize(GetClientSize());

    wxColour startColour(100, 100, 100);

    int shadowSize = 20;
    int colourInc = 8;

    int i;
    for (i = 0; i < shadowSize; i++)
    {
        wxColour col(startColour.Red()+i*colourInc,
                     startColour.Green()+i*colourInc, 
                     startColour.Blue()+i*colourInc);
        dc.SetPen(wxPen(col));
        dc.DrawLine(0, i, clientSize.x, i);
        dc.DrawLine(0, clientSize.y - i, clientSize.x, clientSize.y - i);
    }
}

void wxMoWheelsListBox::OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const
{
    wxColour lightGrey(wxMoSystemSettings::GetColour(wxMO_COLOUR_WHEELS_LIGHT_GREY)); // (220, 220, 220);
    wxColour darkGrey(wxMoSystemSettings::GetColour(wxMO_COLOUR_WHEELS_DARK_GREY));//  50, 50, 50);

    // we need to render selected and current items differently
    const bool isSelected = IsSelected(n),
               isCurrent = IsCurrent(n);
    if ( isSelected || isCurrent )
    {
        if ( isSelected )
        {
            dc.SetBrush(wxBrush(lightGrey, wxSOLID));
        }
        else // !selected
        {
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
        }

        if (isCurrent)
            dc.SetPen(wxPen(darkGrey));
        else
            dc.SetPen(*wxTRANSPARENT_PEN);

        dc.DrawRectangle(rect);
    }
    //else: do nothing for the normal items
}
