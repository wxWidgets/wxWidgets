/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/wheelsctrl.mm
// Purpose:     wxWheelsCtrl class
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

#include "wx/wheelsctrl.h"

#include "wx/arrimpl.cpp"

WX_DEFINE_EXPORTED_OBJARRAY(wxArrayOfStringArrays);
WX_DEFINE_EXPORTED_OBJARRAY(wxSizeArray);

extern WXDLLEXPORT_DATA(const wxChar) wxWheelsCtrlNameStr[] = wxT("wheelsctrl");

IMPLEMENT_DYNAMIC_CLASS(wxWheelsCtrl, wxWheelsCtrlBase)

BEGIN_EVENT_TABLE(wxWheelsCtrl, wxWheelsCtrlBase)
#if 0
    EVT_SIZE(wxWheelsCtrl::OnSize)
    EVT_PAINT(wxWheelsCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(wxWheelsCtrl::OnEraseBackground)
#endif
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxWheelsCtrlEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_WHEEL_SELECTED)


#pragma mark wxWheelsCtrl

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

/// Default constructor.
wxWheelsCtrl::wxWheelsCtrl()
{
    Init();
}

/// Constructor.
wxWheelsCtrl::wxWheelsCtrl(wxWindow *parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxValidator& validator,
                               const wxString& name)
{
    Init();
    Create(parent, id, pos, size, style, validator, name);
}

bool wxWheelsCtrl::Create(wxWindow *parent,
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

wxWheelsCtrl::~wxWheelsCtrl()
{
    // FIXME stub
}

void wxWheelsCtrl::Init()
{
    // FIXME stub
}

void wxWheelsCtrl::ClearComponents()
{
    // FIXME stub
    // TODO
    
    // FIXME stub
}

#if 0
bool wxWheelsCtrl::SendClickEvent()
{
    // FIXME stub
    
    return false;
}
#endif

bool wxWheelsCtrl::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub
    
    return true;
}

bool wxWheelsCtrl::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub
    
    return true;
}

bool wxWheelsCtrl::Enable(bool enable)
{
    // FIXME stub
    
    return true;
}

bool wxWheelsCtrl::SetFont(const wxFont& font)
{
    // FIXME stub
    
    return true;
}

// Reloads and displays all components.
bool wxWheelsCtrl::ReloadAllComponents()
{
    // FIXME stub
    
    return true;
}

// Reloads and displays the specified component.
bool wxWheelsCtrl::ReloadComponent(int component)
{
    // FIXME stub
    
    return true;
}

// Sets the selection (row) in the given component.
void wxWheelsCtrl::SetSelection(int component, int selection)
{
    // FIXME stub
}

// Gets the selection for the given component.
int wxWheelsCtrl::GetSelection(int component) const
{
    // FIXME stub
    
    return -1;
}

// Returns the number of rows in the given component.
int wxWheelsCtrl::GetComponentRowCount(int component) const
{
    // FIXME stub
    
    return 0;
}

// Returns the number of components.
int wxWheelsCtrl::GetComponentCount() const
{
    // FIXME stub
    
    return 0;
}

// Returns the size required to display the largest view in the given component.
wxSize wxWheelsCtrl::GetRowSizeForComponent(int component) const
{
    // FIXME stub
    
    return wxSize(0, 0);
}

// Sets the data source object, recreating the components.
void wxWheelsCtrl::SetDataSource(wxWheelsDataSource* dataSource,
                                   bool ownsDataSource)
{
    // FIXME stub
}

// Initializes the components from the current data source
bool wxWheelsCtrl::InitializeComponents()
{
    // FIXME stub
    
    return true;
}

#if 0
void wxWheelsCtrl::ResizeComponents()
{
    // FIXME stub
}
#endif

#if 0
void wxWheelsCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}
#endif

#if 0
void wxWheelsCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // FIXME stub
}
#endif

#if 0
void wxWheelsCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // FIXME stub
}
#endif


#pragma mark wxWheelsDataSource

IMPLEMENT_CLASS(wxWheelsDataSource, wxEvtHandler)

// Override to intercept row click; by default, sends an event to itself and then to control.
bool wxWheelsDataSource::OnSelectRow(wxWheelsCtrl* ctrl,
                                       int component,
                                       int row)
{
    // FIXME stub
    
    return true;
}

// Override to provide the row height for a component. Should returns a height in pixels.
int wxWheelsDataSource::GetRowHeightForComponent(wxWheelsCtrl* WXUNUSED(ctrl),
                                                   int WXUNUSED(component))
{
    return 0;
}

// Override to provide the row width for a component. Should returns a width in pixels.
int wxWheelsDataSource::GetRowWidthForComponent(wxWheelsCtrl* WXUNUSED(ctrl),
                                                  int WXUNUSED(component))
{
    return 0;
}


#pragma mark wxWheelsTextDataSource

IMPLEMENT_CLASS(wxWheelsTextDataSource, wxWheelsDataSource)

wxWheelsTextDataSource::wxWheelsTextDataSource(int componentCount,
                                                   const wxSize& rowSize)
{
    // FIXME stub
}

void wxWheelsTextDataSource::Init()
{
    // FIXME stub
}

void wxWheelsTextDataSource::SetComponentCount(int count)
{
    // FIXME stub
}

wxArrayString wxWheelsTextDataSource::GetStrings(int component) const
{
    // FIXME stub
    
    wxArrayString empty;
    return empty;
}

void wxWheelsTextDataSource::SetStrings(const wxArrayString& strings,
                                          int component)
{
    // FIXME stub
}

wxSize wxWheelsTextDataSource::GetRowSize(int component) const
{
    // FIXME stub
    
    return wxSize(0, 0);
}

void wxWheelsTextDataSource::SetRowSize(const wxSize& sz,
                                          int component)
{
    // FIXME stub
}

// Override to provide the number of rows in a component.
int wxWheelsTextDataSource::GetNumberOfRowsForComponent(wxWheelsCtrl* WXUNUSED(ctrl),
                                                          int component)
{
    // FIXME stub
    
    return 0;
}

// Override to provide the title for a component's row. If you override GetRowWindow instead,
// this method is optional.
wxString wxWheelsTextDataSource::GetRowTitle(wxWheelsCtrl* WXUNUSED(ctrl),
                                               int component,
                                               int row)
{
    // FIXME stub
    
    return wxEmptyString;
}

// Override to provide the row height for a component. Should returns a height in pixels.
int wxWheelsTextDataSource::GetRowHeightForComponent(wxWheelsCtrl* ctrl,
                                                       int component)
{
    // FIXME stub
    
    return 0;
}

// Override to provide the row width for a component. Should returns a width in pixels.
int wxWheelsTextDataSource::GetRowWidthForComponent(wxWheelsCtrl* ctrl,
                                                      int component)
{
    // FIXME stub
    
    return 0;
}


#pragma mark wxWheelComponent

IMPLEMENT_CLASS(wxWheelComponent, wxEvtHandler)


#pragma mark wxWheelsListBox

IMPLEMENT_CLASS(wxWheelsListBox, wxVListBox)

/// Default constructor.
wxWheelsListBox::wxWheelsListBox()
{
    Init();
}

/// Constructor.
wxWheelsListBox::wxWheelsListBox(wxWindow *parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxString& name)
{
    Init();
    
    Create(parent, id, pos, size, style, name);
}

// Creation function.
bool wxWheelsListBox::Create(wxWindow *parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxString& name)
{
    // FIXME stub
    
    return true;
}

wxWheelsListBox::~wxWheelsListBox()
{
    
}

// common part of all ctors
void wxWheelsListBox::Init()
{
    // FIXME stub
}

// the derived class must implement this function to actually draw the item
// with the given index on the provided DC
void wxWheelsListBox::OnDrawItem(wxDC& dc,
                                   const wxRect& rect,
                                   size_t n) const
{
    // FIXME stub
}

// the derived class must implement this method to return the height of the
// specified item
wxCoord wxWheelsListBox::OnMeasureItem(size_t WXUNUSED(n)) const
{
    // FIXME stub
    
    return 0;
}

wxWheelComponent* wxWheelsListBox::GetComponent() const
{
    // FIXME stub
    
    return NULL;
}

// usually overridden base class virtuals
wxSize wxWheelsListBox::DoGetBestSize() const
{
    return wxSize(10, 10);
}

void wxWheelsListBox::OnSelect(wxCommandEvent& event)
{
    // FIXME stub
}

void wxWheelsListBox::DrawBackground(wxDC& dc)
{
    // FIXME stub
}

void wxWheelsListBox::OnDrawBackground(wxDC& dc,
                                         const wxRect& rect,
                                         size_t n) const
{
    // FIXME stub
}
