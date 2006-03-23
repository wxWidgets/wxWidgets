/////////////////////////////////////////////////////////////////////////////
// Name:        datavgen.cpp
// Purpose:     wxDataViewCtrl generic implementation
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/defs.h"

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#ifdef wxUSE_GENERICDATAVIEWCTRL

#include "wx/stockitem.h"
#include "wx/dcclient.h"
#include "wx/calctrl.h"
#include "wx/popupwin.h"
#include "wx/sizer.h"
#include "wx/log.h"
#include "wx/renderer.h"

#ifdef __WXMSW__
    #include <windows.h> // for DLGC_WANTARROWS
    #include "wx/msw/winundef.h"
#endif

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDataViewCtrl;

//-----------------------------------------------------------------------------
// wxDataViewHeaderWindow
//-----------------------------------------------------------------------------

class wxDataViewHeaderWindow: public wxWindow
{
public:
    wxDataViewHeaderWindow( wxDataViewCtrl *parent,
                            wxWindowID id,
                            const wxPoint &pos = wxDefaultPosition,
                            const wxSize &size = wxDefaultSize,
                            const wxString &name = wxT("wxdataviewctrlheaderwindow") );
    ~wxDataViewHeaderWindow();

    void SetOwner( wxDataViewCtrl* owner ) { m_owner = owner; }
    wxDataViewCtrl *GetOwner() { return m_owner; }

    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    
private:
    wxDataViewCtrl      *m_owner;
    wxCursor            *m_resizeCursor;
    
private:
    DECLARE_DYNAMIC_CLASS(wxDataViewHeaderWindow)
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxDataViewMainWindow
//-----------------------------------------------------------------------------

class wxDataViewMainWindow: public wxWindow
{
public:
    wxDataViewMainWindow( wxDataViewCtrl *parent,
                            wxWindowID id,
                            const wxPoint &pos = wxDefaultPosition,
                            const wxSize &size = wxDefaultSize,
                            const wxString &name = wxT("wxdataviewctrlmainwindow") );
    ~wxDataViewMainWindow();

    // notifications from wxDataViewListModel
    bool RowAppended();
    bool RowPrepended();
    bool RowInserted( size_t before );
    bool RowDeleted( size_t row );
    bool RowChanged( size_t row );
    bool ValueChanged( size_t col, size_t row );
    bool RowsReordered( size_t *new_order );
    bool Cleared();

    void SetOwner( wxDataViewCtrl* owner ) { m_owner = owner; }
    wxDataViewCtrl *GetOwner() { return m_owner; }

    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    
    void UpdateDisplay();
    void RecalculateDisplay();
    void OnInternalIdle();
    
    void ScrollWindow( int dx, int dy, const wxRect *rect );
private:
    wxDataViewCtrl      *m_owner;
    int                  m_lineHeight;
    bool                 m_dirty;
    
private:
    DECLARE_DYNAMIC_CLASS(wxDataViewMainWindow)
    DECLARE_EVENT_TABLE()
};

// --------------------------------------------------------- 
// wxGenericDataViewListModelNotifier
// --------------------------------------------------------- 

class wxGenericDataViewListModelNotifier: public wxDataViewListModelNotifier
{
public:
    wxGenericDataViewListModelNotifier( wxDataViewMainWindow *mainWindow )
        { m_mainWindow = mainWindow; }
        
    virtual bool RowAppended()
        { return m_mainWindow->RowAppended(); }
    virtual bool RowPrepended()
        { return m_mainWindow->RowPrepended(); }
    virtual bool RowInserted( size_t before )
        { return m_mainWindow->RowInserted( before ); }
    virtual bool RowDeleted( size_t row )
        { return m_mainWindow->RowDeleted( row ); }
    virtual bool RowChanged( size_t row )
        { return m_mainWindow->RowChanged( row ); }
    virtual bool ValueChanged( size_t col, size_t row )
        { return m_mainWindow->ValueChanged( col, row ); }
    virtual bool RowsReordered( size_t *new_order )
        { return m_mainWindow->RowsReordered( new_order ); }
    virtual bool Cleared()
        { return m_mainWindow->Cleared(); }
    
    wxDataViewMainWindow    *m_mainWindow;
};

// --------------------------------------------------------- 
// wxDataViewCell
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewCell, wxDataViewCellBase)

wxDataViewCell::wxDataViewCell( const wxString &varianttype, wxDataViewCellMode mode ) :
    wxDataViewCellBase( varianttype, mode )
{
    m_dc = NULL;
}

wxDataViewCell::~wxDataViewCell()
{
    if (m_dc)
        delete m_dc;
}

wxDC *wxDataViewCell::GetDC()
{
    if (m_dc == NULL)
    {
        if (GetOwner() == NULL)
            return NULL;
        if (GetOwner()->GetOwner() == NULL)
            return NULL;
        m_dc = new wxClientDC( GetOwner()->GetOwner() );
    }
        
    return m_dc;
}

// --------------------------------------------------------- 
// wxDataViewCustomCell
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewCustomCell, wxDataViewCell)

wxDataViewCustomCell::wxDataViewCustomCell( const wxString &varianttype, 
                          wxDataViewCellMode mode ) :
    wxDataViewCell( varianttype, mode )
{
}

    
// --------------------------------------------------------- 
// wxDataViewTextCell
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewTextCell, wxDataViewCustomCell)

wxDataViewTextCell::wxDataViewTextCell( const wxString &varianttype, wxDataViewCellMode mode ) :
    wxDataViewCustomCell( varianttype, mode )
{
}

bool wxDataViewTextCell::SetValue( const wxVariant &value )
{
    m_text = value.GetString();
    
    return true;
}

bool wxDataViewTextCell::GetValue( wxVariant &value )
{
    return false;
}

bool wxDataViewTextCell::Render( wxRect cell, wxDC *dc, int state )
{
    dc->DrawText( m_text, cell.x, cell.y );

    return true;
}

wxSize wxDataViewTextCell::GetSize()
{
    return wxSize(80,20);
}

// --------------------------------------------------------- 
// wxDataViewToggleCell
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewToggleCell, wxDataViewCustomCell)

wxDataViewToggleCell::wxDataViewToggleCell( const wxString &varianttype, 
                        wxDataViewCellMode mode ) :
    wxDataViewCustomCell( varianttype, mode )
{
    m_toggle = false;
}

bool wxDataViewToggleCell::SetValue( const wxVariant &value )
{
    m_toggle = value.GetBool();
    
    return true;;
}

bool wxDataViewToggleCell::GetValue( wxVariant &value )
{
    return false;
}
    
bool wxDataViewToggleCell::Render( wxRect cell, wxDC *dc, int state )
{
    // User wxRenderer here
    
    dc->SetPen( *wxBLACK_PEN );
    dc->SetBrush( *wxTRANSPARENT_BRUSH );
    wxRect rect;
    rect.x = cell.x + cell.width/2 - 10;
    rect.width = 20;
    rect.y = cell.y + cell.height/2 - 10;
    rect.height = 20;
    dc->DrawRectangle( rect );
    if (m_toggle)
    {
        rect.x += 2;
        rect.y += 2;
        rect.width -= 4;
        rect.height -= 4;
        dc->DrawLine( rect.x, rect.y, rect.x+rect.width, rect.y+rect.height );
        dc->DrawLine( rect.x+rect.width, rect.y, rect.x, rect.y+rect.height );
    }
    
    return true;
}

wxSize wxDataViewToggleCell::GetSize()
{
    return wxSize(20,20);
}

// --------------------------------------------------------- 
// wxDataViewProgressCell
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewProgressCell, wxDataViewCustomCell)

wxDataViewProgressCell::wxDataViewProgressCell( const wxString &label, 
    const wxString &varianttype, wxDataViewCellMode mode ) :
    wxDataViewCustomCell( varianttype, mode )  
{
    m_label = label;
    m_value = 0;
}

wxDataViewProgressCell::~wxDataViewProgressCell()
{
}

bool wxDataViewProgressCell::SetValue( const wxVariant &value )
{
    m_value = (long) value;
    
    if (m_value < 0) m_value = 0;
    if (m_value > 100) m_value = 100;
    
    return true;
}
    
bool wxDataViewProgressCell::Render( wxRect cell, wxDC *dc, int state )
{
    double pct = (double)m_value / 100.0;
    wxRect bar = cell;
    bar.width = (int)(cell.width * pct);
    dc->SetPen( *wxTRANSPARENT_PEN );
    dc->SetBrush( *wxBLUE_BRUSH );
    dc->DrawRectangle( bar );

    dc->SetBrush( *wxTRANSPARENT_BRUSH );
    dc->SetPen( *wxBLACK_PEN );
    dc->DrawRectangle( cell );
    
    return true;
}

wxSize wxDataViewProgressCell::GetSize()
{
    return wxSize(40,12);
}
    
// --------------------------------------------------------- 
// wxDataViewDateCell
// --------------------------------------------------------- 

class wxDataViewDateCellPopupTransient: public wxPopupTransientWindow
{
public: 
    wxDataViewDateCellPopupTransient( wxWindow* parent, wxDateTime *value,
        wxDataViewListModel *model, size_t col, size_t row ) :
        wxPopupTransientWindow( parent, wxBORDER_SIMPLE )
    {
        m_model = model;
        m_col = col;
        m_row = row;
        m_cal = new wxCalendarCtrl( this, -1, *value );
        wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
        sizer->Add( m_cal, 1, wxGROW );
        SetSizer( sizer );
        sizer->Fit( this );
    }
    
    virtual void OnDismiss()
    {
    }
    
    void OnCalendar( wxCalendarEvent &event );
    
    wxCalendarCtrl      *m_cal;
    wxDataViewListModel *m_model; 
    size_t               m_col;
    size_t               m_row;
    
private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxDataViewDateCellPopupTransient,wxPopupTransientWindow)
    EVT_CALENDAR( -1, wxDataViewDateCellPopupTransient::OnCalendar )
END_EVENT_TABLE()

void wxDataViewDateCellPopupTransient::OnCalendar( wxCalendarEvent &event )
{
    wxDateTime date = event.GetDate();
    wxVariant value = date;
    m_model->SetValue( value, m_col, m_row );
    m_model->ValueChanged( m_col, m_row );
    DismissAndNotify();
}

IMPLEMENT_ABSTRACT_CLASS(wxDataViewDateCell, wxDataViewCustomCell)

wxDataViewDateCell::wxDataViewDateCell( const wxString &varianttype,
                        wxDataViewCellMode mode ) :
    wxDataViewCustomCell( varianttype, mode )
{
}
    
bool wxDataViewDateCell::SetValue( const wxVariant &value )
{
    m_date = value.GetDateTime();
    
    return true;
}

bool wxDataViewDateCell::Render( wxRect cell, wxDC *dc, int state )
{
    dc->SetFont( GetOwner()->GetOwner()->GetFont() );
    wxString tmp = m_date.FormatDate();
    dc->DrawText( tmp, cell.x, cell.y );

    return true;
}

wxSize wxDataViewDateCell::GetSize()
{
    wxDataViewCtrl* view = GetOwner()->GetOwner();
    wxString tmp = m_date.FormatDate();
    wxCoord x,y,d;
    view->GetTextExtent( tmp, &x, &y, &d );
    return wxSize(x,y+d);
}

bool wxDataViewDateCell::Activate( wxRect cell, wxDataViewListModel *model, size_t col, size_t row )
{
    wxVariant variant;
    model->GetValue( variant, col, row );
    wxDateTime value = variant.GetDateTime();

    wxDataViewDateCellPopupTransient *popup = new wxDataViewDateCellPopupTransient( 
        GetOwner()->GetOwner()->GetParent(), &value, model, col, row );
    wxPoint pos = wxGetMousePosition();
    popup->Move( pos );
    popup->Layout();
    popup->Popup( popup->m_cal );

    return true;
}

// --------------------------------------------------------- 
// wxDataViewColumn
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewColumn, wxDataViewColumnBase)

wxDataViewColumn::wxDataViewColumn( const wxString &title, wxDataViewCell *cell, 
    size_t model_column, int flags ) :
    wxDataViewColumnBase( title, cell, model_column, flags )
{
    m_width = 80;
}

wxDataViewColumn::~wxDataViewColumn()
{
}

void wxDataViewColumn::SetTitle( const wxString &title )
{
    wxDataViewColumnBase::SetTitle( title );
    
}

//-----------------------------------------------------------------------------
// wxDataViewHeaderWindow
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewHeaderWindow, wxWindow)

BEGIN_EVENT_TABLE(wxDataViewHeaderWindow,wxWindow)
    EVT_PAINT         (wxDataViewHeaderWindow::OnPaint)
    EVT_MOUSE_EVENTS  (wxDataViewHeaderWindow::OnMouse)
    EVT_SET_FOCUS     (wxDataViewHeaderWindow::OnSetFocus)
END_EVENT_TABLE()

wxDataViewHeaderWindow::wxDataViewHeaderWindow( wxDataViewCtrl *parent, wxWindowID id,
     const wxPoint &pos, const wxSize &size, const wxString &name ) :
    wxWindow( parent, id, pos, size, 0, name )
{
    SetOwner( parent );

    m_resizeCursor = new wxCursor( wxCURSOR_SIZEWE );
    
    wxVisualAttributes attr = wxPanel::GetClassDefaultAttributes();
    SetOwnForegroundColour( attr.colFg );
    SetOwnBackgroundColour( attr.colBg );
    if (!m_hasFont)
        SetOwnFont( attr.font );
}

wxDataViewHeaderWindow::~wxDataViewHeaderWindow()
{
    delete m_resizeCursor;
}

void wxDataViewHeaderWindow::OnPaint( wxPaintEvent &event )
{
    int w, h;
    GetClientSize( &w, &h );

    wxPaintDC dc( this );
    
    int xpix;
    m_owner->GetScrollPixelsPerUnit( &xpix, NULL );

    int x;
    m_owner->GetViewStart( &x, NULL );

    // account for the horz scrollbar offset
    dc.SetDeviceOrigin( -x * xpix, 0 );
    
    dc.SetFont( GetFont() );
    
    size_t cols = GetOwner()->GetNumberOfColumns();
    size_t i;
    int xpos = 0;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumn( i );
        int width = col->GetWidth();
        
        // the width of the rect to draw: make it smaller to fit entirely
        // inside the column rect
#ifdef __WXMAC__
        int cw = width;
        int ch = h;
#else
        int cw = width - 2;
        int ch = h - 2;
#endif

        wxRendererNative::Get().DrawHeaderButton
                                (
                                    this,
                                    dc,
                                    wxRect(xpos, 0, cw, ch),
                                    m_parent->IsEnabled() ? 0
                                                          : (int)wxCONTROL_DISABLED
                                );

        dc.DrawText( col->GetTitle(), xpos+3, 3 );   
                                
        xpos += width;
    }
}

void wxDataViewHeaderWindow::OnMouse( wxMouseEvent &event )
{
}

void wxDataViewHeaderWindow::OnSetFocus( wxFocusEvent &event )
{
    event.Skip();
}

//-----------------------------------------------------------------------------
// wxDataViewMainWindow
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewMainWindow, wxWindow)

BEGIN_EVENT_TABLE(wxDataViewMainWindow,wxWindow)
    EVT_PAINT         (wxDataViewMainWindow::OnPaint)
    EVT_MOUSE_EVENTS  (wxDataViewMainWindow::OnMouse)
    EVT_SET_FOCUS     (wxDataViewMainWindow::OnSetFocus)
END_EVENT_TABLE()

wxDataViewMainWindow::wxDataViewMainWindow( wxDataViewCtrl *parent, wxWindowID id,
    const wxPoint &pos, const wxSize &size, const wxString &name ) :
    wxWindow( parent, id, pos, size, 0, name )
{
    SetOwner( parent );
    
    // We need to calculate this smartly..
    m_lineHeight = 20;
    
    UpdateDisplay();
}

wxDataViewMainWindow::~wxDataViewMainWindow()
{
}

bool wxDataViewMainWindow::RowAppended()
{
    return false;
}

bool wxDataViewMainWindow::RowPrepended()
{
    return false;
}

bool wxDataViewMainWindow::RowInserted( size_t before )
{
    return false;
}

bool wxDataViewMainWindow::RowDeleted( size_t row )
{
    return false;
}

bool wxDataViewMainWindow::RowChanged( size_t row )
{
    return false;
}

bool wxDataViewMainWindow::ValueChanged( size_t col, size_t row )
{
    return false;
}

bool wxDataViewMainWindow::RowsReordered( size_t *new_order )
{
    return false;
}

bool wxDataViewMainWindow::Cleared()
{
    return false;
}

void wxDataViewMainWindow::UpdateDisplay()
{
    m_dirty = true;
}

void wxDataViewMainWindow::OnInternalIdle()
{
    wxWindow::OnInternalIdle();
    
    if (m_dirty)
    {
        RecalculateDisplay();
        m_dirty = false;
    }
}

void wxDataViewMainWindow::RecalculateDisplay()
{
    wxDataViewListModel *model = GetOwner()->GetModel();
    if (!model)
    {
        Refresh();
        return;
    }
    
    int width = 0;
    size_t cols = GetOwner()->GetNumberOfColumns();
    size_t i;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumn( i );
        width += col->GetWidth();
    }
    
    int height = model->GetNumberOfRows() * m_lineHeight;

    SetVirtualSize( width, height );
    GetOwner()->SetScrollRate( 10, m_lineHeight );
    
    Refresh();
}

void wxDataViewMainWindow::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    wxWindow::ScrollWindow( dx, dy, rect );
    GetOwner()->m_headerArea->ScrollWindow( dx, 0 );
}

void wxDataViewMainWindow::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc( this );

    GetOwner()->PrepareDC( dc );

    dc.SetFont( GetFont() );

    wxRect update = GetUpdateRegion().GetBox();
    m_owner->CalcUnscrolledPosition( update.x, update.y, &update.x, &update.y );
    
    wxDataViewListModel *model = GetOwner()->GetModel();
    
    size_t item_start = update.y / m_lineHeight;
    size_t item_count = (update.height / m_lineHeight) + 1;

    wxRect cell_rect;
    cell_rect.x = 0;
    cell_rect.height = m_lineHeight;
    size_t cols = GetOwner()->GetNumberOfColumns();
    size_t i;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumn( i );
        wxDataViewCell *cell = col->GetCell();
        cell_rect.width = col->GetWidth();
        
        size_t item;
        for (item = item_start; item <= item_start+item_count; item++)
        {
            cell_rect.y = item*m_lineHeight;
            wxVariant value;
            model->GetValue( value, col->GetModelColumn(), item );
            cell->SetValue( value );
            cell->Render( cell_rect, &dc, 0 );
        }
        
        cell_rect.x += cell_rect.width;
    }
}

void wxDataViewMainWindow::OnMouse( wxMouseEvent &event )
{
    event.Skip();
}

void wxDataViewMainWindow::OnSetFocus( wxFocusEvent &event )
{
    event.Skip();
}

//-----------------------------------------------------------------------------
// wxDataViewCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDataViewCtrl, wxDataViewCtrlBase)

BEGIN_EVENT_TABLE(wxDataViewCtrl, wxDataViewCtrlBase)
    EVT_SIZE(wxDataViewCtrl::OnSize)
END_EVENT_TABLE()

wxDataViewCtrl::~wxDataViewCtrl()
{
    if (m_notifier)
        GetModel()->RemoveNotifier( m_notifier );
}

void wxDataViewCtrl::Init()
{
    m_notifier = NULL;
}

bool wxDataViewCtrl::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size, 
           long style, const wxValidator& validator )
{
    if (!wxControl::Create( parent, id, pos, size, style | wxScrolledWindowStyle|wxSUNKEN_BORDER, validator))
        return false;

    Init();
    
#ifdef __WXMAC__
    MacSetClipChildren( true ) ;
#endif

    m_clientArea = new wxDataViewMainWindow( this, -1 );
    m_headerArea = new wxDataViewHeaderWindow( this, -1, wxDefaultPosition, wxSize(-1,25) );
    
    SetTargetWindow( m_clientArea );
    
    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add( m_headerArea, 0, wxGROW );
    sizer->Add( m_clientArea, 1, wxGROW );
    SetSizer( sizer );

    return true;
}

#ifdef __WXMSW__
WXLRESULT wxDataViewCtrl::MSWWindowProc(WXUINT nMsg,
                                       WXWPARAM wParam,
                                       WXLPARAM lParam)
{
    WXLRESULT rc = wxPanel::MSWWindowProc(nMsg, wParam, lParam);

#ifndef __WXWINCE__
    // we need to process arrows ourselves for scrolling
    if ( nMsg == WM_GETDLGCODE )
    {
        rc |= DLGC_WANTARROWS;
    }
#endif

    return rc;
}
#endif

void wxDataViewCtrl::OnSize( wxSizeEvent &event )
{
    // We need to override OnSize so that our scrolled
    // window a) does call Layout() to use sizers for
    // positioning the controls but b) does not query
    // the sizer for their size and use that for setting
    // the scrollable area as set that ourselves by
    // calling SetScrollbar() further down.

    Layout();

    AdjustScrollbars();
}

bool wxDataViewCtrl::AssociateModel( wxDataViewListModel *model )
{
    if (!wxDataViewCtrlBase::AssociateModel( model ))
        return false;

    m_notifier = new wxGenericDataViewListModelNotifier( m_clientArea );

    model->AddNotifier( m_notifier );    

    m_clientArea->UpdateDisplay();
    
    return true;
}

bool wxDataViewCtrl::AppendColumn( wxDataViewColumn *col )
{
    if (!wxDataViewCtrlBase::AppendColumn(col))
        return false;
    
    m_clientArea->UpdateDisplay();
    
    return true;
}

#endif 
    // !wxUSE_GENERICDATAVIEWCTRL

#endif 
    // wxUSE_DATAVIEWCTRL

