/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_wheelsctrl_n.mm
// Purpose:     wxMoWheelsCtrl class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
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

#include "wx/mobile/native/wheelsctrl.h"
#include "wx/mobile/native/utils.h"

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
    // FIXME stub

    return true;
}

wxMoWheelsCtrl::~wxMoWheelsCtrl()
{
    // FIXME stub
}

void wxMoWheelsCtrl::Init()
{
    // FIXME stub
}

void wxMoWheelsCtrl::ClearComponents()
{
    // FIXME stub
    // TODO

    // FIXME stub
}

bool wxMoWheelsCtrl::SendClickEvent()
{
    // FIXME stub

    return false;
}

bool wxMoWheelsCtrl::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub
    
    return true;
}

bool wxMoWheelsCtrl::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoWheelsCtrl::Enable(bool enable)
{
    // FIXME stub

    return true;
}

bool wxMoWheelsCtrl::SetFont(const wxFont& font)
{
    // FIXME stub

    return true;
}

// Reloads and displays all components.
bool wxMoWheelsCtrl::ReloadAllComponents()
{
    // FIXME stub

    return true;
}

// Reloads and displays the specified component.
bool wxMoWheelsCtrl::ReloadComponent(int component)
{
    // FIXME stub

    return true;
}

// Sets the selection (row) in the given component.
void wxMoWheelsCtrl::SetSelection(int component, int selection)
{
    // FIXME stub
}

// Gets the selection for the given component.
int wxMoWheelsCtrl::GetSelection(int component) const
{
    // FIXME stub

    return -1;
}

// Returns the number of rows in the given component.
int wxMoWheelsCtrl::GetComponentRowCount(int component) const
{
    // FIXME stub

    return 0;
}

// Returns the number of components.
int wxMoWheelsCtrl::GetComponentCount() const
{
    // FIXME stub
    
    return 0;
}

// Returns the size required to display the largest view in the given component.
wxSize wxMoWheelsCtrl::GetRowSizeForComponent(int component) const
{
    // FIXME stub

    return wxSize(0, 0);
}

// Sets the data source object, recreating the components.
void wxMoWheelsCtrl::SetDataSource(wxMoWheelsDataSource* dataSource, bool ownsDataSource)
{
    // FIXME stub
}

// Initializes the components from the current data source
bool wxMoWheelsCtrl::InitializeComponents()
{
    // FIXME stub

    return true;
}

void wxMoWheelsCtrl::ResizeComponents()
{
    // FIXME stub
}

void wxMoWheelsCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoWheelsCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoWheelsCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

wxSize wxMoWheelsCtrl::DoGetBestSize() const
{
    // FIXME stub

    return wxSize(100, 100);
}

IMPLEMENT_CLASS(wxMoWheelsDataSource, wxEvtHandler)

// Override to intercept row click; by default, sends an event to itself and then to control.
bool wxMoWheelsDataSource::OnSelectRow(wxMoWheelsCtrl* ctrl, int component, int row)
{
    // FIXME stub
    
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
    // FIXME stub
}

void wxMoWheelsTextDataSource::Init()
{
    // FIXME stub
}

void wxMoWheelsTextDataSource::SetComponentCount(int count)
{
    // FIXME stub
}

wxArrayString wxMoWheelsTextDataSource::GetStrings(int component) const
{
    // FIXME stub

    wxArrayString empty;
    return empty;
}

void wxMoWheelsTextDataSource::SetStrings(const wxArrayString& strings, int component)
{
    // FIXME stub
}

wxSize wxMoWheelsTextDataSource::GetRowSize(int component) const
{
    // FIXME stub

    return wxSize(0, 0);
}

void wxMoWheelsTextDataSource::SetRowSize(const wxSize& sz, int component)
{
    // FIXME stub
}

// Override to provide the number of rows in a component.
int wxMoWheelsTextDataSource::GetNumberOfRowsForComponent(wxMoWheelsCtrl* WXUNUSED(ctrl), int component)
{
    // FIXME stub

    return 0;
}

// Override to provide the title for a component's row. If you override GetRowWindow instead,
// this method is optional.
wxString wxMoWheelsTextDataSource::GetRowTitle(wxMoWheelsCtrl* WXUNUSED(ctrl), int component, int row)
{
    // FIXME stub

    return wxEmptyString;
}

// Override to provide the row height for a component. Should returns a height in pixels.
int wxMoWheelsTextDataSource::GetRowHeightForComponent(wxMoWheelsCtrl* ctrl, int component)
{
    // FIXME stub

    return 0;
}

// Override to provide the row width for a component. Should returns a width in pixels.
int wxMoWheelsTextDataSource::GetRowWidthForComponent(wxMoWheelsCtrl* ctrl, int component)
{
    // FIXME stub

    return 0;
}

IMPLEMENT_CLASS(wxMoWheelComponent, wxEvtHandler)

void wxMoWheelComponent::Init()
{
    // FIXME stub
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
    // FIXME stub

    return true;
}

// common part of all ctors
void wxMoWheelsListBox::Init()
{
    // FIXME stub
}

// the derived class must implement this function to actually draw the item
// with the given index on the provided DC
void wxMoWheelsListBox::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
{
    // FIXME stub
}

// the derived class must implement this method to return the height of the
// specified item
wxCoord wxMoWheelsListBox::OnMeasureItem(size_t WXUNUSED(n)) const
{
    // FIXME stub

    return 0;
}

wxMoWheelComponent* wxMoWheelsListBox::GetComponent() const
{
    // FIXME stub

    return NULL;
}

// usually overridden base class virtuals
wxSize wxMoWheelsListBox::DoGetBestSize() const
{
    return wxSize(10, 10);
}

void wxMoWheelsListBox::OnSelect(wxCommandEvent& event)
{
    // FIXME stub
}

void wxMoWheelsListBox::DrawBackground(wxDC& dc)
{
    // FIXME stub
}

void wxMoWheelsListBox::OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const
{
    // FIXME stub
}
