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

// --------------------------------------------------------- 
// wxGenericDataViewListModelNotifier
// --------------------------------------------------------- 

class wxGenericDataViewListModelNotifier: public wxDataViewListModelNotifier
{
public:
    wxGenericDataViewListModelNotifier( wxDataViewListModel *wx_model );
    
    virtual bool RowAppended();
    virtual bool RowPrepended();
    virtual bool RowInserted( size_t before );
    virtual bool RowDeleted( size_t row );
    virtual bool RowChanged( size_t row );
    virtual bool ValueChanged( size_t col, size_t row );
    virtual bool RowsReordered( size_t *new_order );
    virtual bool Cleared();
    
    wxDataViewListModel *m_wx_model;
};

// --------------------------------------------------------- 
// wxGenericDataViewListModelNotifier
// --------------------------------------------------------- 

wxGenericDataViewListModelNotifier::wxGenericDataViewListModelNotifier( 
    wxDataViewListModel *wx_model )
{
    m_wx_model = wx_model;
}
    
bool wxGenericDataViewListModelNotifier::RowAppended()
{
    size_t pos = m_wx_model->GetNumberOfRows()-1;
    
    return false;
}

bool wxGenericDataViewListModelNotifier::RowPrepended()
{
    return false;
}

bool wxGenericDataViewListModelNotifier::RowInserted( size_t before )
{
    return false;
}

bool wxGenericDataViewListModelNotifier::RowDeleted( size_t row )
{
    return false;
}

bool wxGenericDataViewListModelNotifier::RowChanged( size_t row )
{
    return true;
}

bool wxGenericDataViewListModelNotifier::ValueChanged( size_t model_col, size_t model_row )
{
    wxNode *node = GetOwner()->m_viewingColumns.GetFirst();
    while (node)
    {
        wxDataViewViewingColumn* viewing_column = (wxDataViewViewingColumn*) node->GetData();
        if (viewing_column->m_modelColumn == model_col)
        {
        
        }

        node = node->GetNext();
    }
    
    return false;
}

bool wxGenericDataViewListModelNotifier::RowsReordered( size_t *new_order )
{
    wxNode *node = GetOwner()->m_viewingColumns.GetFirst();
    while (node)
    {
        wxDataViewViewingColumn* viewing_column = (wxDataViewViewingColumn*) node->GetData();

        node = node->GetNext();
    }
    
    return false;
}

bool wxGenericDataViewListModelNotifier::Cleared()
{
    return false;
}

// --------------------------------------------------------- 
// wxDataViewCell
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewCell, wxDataViewCellBase)

wxDataViewCell::wxDataViewCell( const wxString &varianttype, wxDataViewCellMode mode ) :
    wxDataViewCellBase( varianttype, mode )
{
}

// --------------------------------------------------------- 
// wxDataViewTextCell
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewTextCell, wxDataViewCell)

wxDataViewTextCell::wxDataViewTextCell( const wxString &varianttype, wxDataViewCellMode mode ) :
    wxDataViewCell( varianttype, mode )
{
}

bool wxDataViewTextCell::SetValue( const wxVariant &value )
{
    return false;
}

bool wxDataViewTextCell::GetValue( wxVariant &value )
{
    return false;
}

// --------------------------------------------------------- 
// wxDataViewToggleCell
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewToggleCell, wxDataViewCell)

wxDataViewToggleCell::wxDataViewToggleCell( const wxString &varianttype, 
                        wxDataViewCellMode mode ) :
    wxDataViewCell( varianttype, mode )
{
}

bool wxDataViewToggleCell::SetValue( const wxVariant &value )
{
    return false;
}

bool wxDataViewToggleCell::GetValue( wxVariant &value )
{
    return false;
}
    
// --------------------------------------------------------- 
// wxDataViewCustomCell
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewCustomCell, wxDataViewCell)

wxDataViewCustomCell::wxDataViewCustomCell( const wxString &varianttype, 
                          wxDataViewCellMode mode ) :
    wxDataViewCell( varianttype, mode )
{
    m_dc = NULL;
    
    Init();
}

bool wxDataViewCustomCell::Init()
{
    return false;
}

wxDataViewCustomCell::~wxDataViewCustomCell()
{
    if (m_dc)
        delete m_dc;
}

wxDC *wxDataViewCustomCell::GetDC()
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
    wxPaintDC dc;
    PrepareDC( dc );
    
    int xpix;
    m_owner->GetScrollPixelsPerUnit( &xpix, NULL );

    int x;
    m_owner->GetViewStart( &x, NULL );

    // account for the horz scrollbar offset
    dc.SetDeviceOrigin( -x * xpix, 0 );
    
    dc.SetFont( GetFont() );
    
    dc.DrawText( wxT("This is the header.."), 5, 5 );
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

class wxDataViewMainWindow: public wxWindow
{
public:
    wxDataViewMainWindow( wxDataViewCtrl *parent,
                            wxWindowID id,
                            const wxPoint &pos = wxDefaultPosition,
                            const wxSize &size = wxDefaultSize,
                            const wxString &name = wxT("wxdataviewctrlmainwindow") );
    ~wxDataViewMainWindow();

    void SetOwner( wxDataViewCtrl* owner ) { m_owner = owner; }
    wxDataViewCtrl *GetOwner() { return m_owner; }

    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    
private:
    wxDataViewCtrl      *m_owner;
    
private:
    DECLARE_DYNAMIC_CLASS(wxDataViewMainWindow)
    DECLARE_EVENT_TABLE()
};

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
}

wxDataViewMainWindow::~wxDataViewMainWindow()
{
}

void wxDataViewMainWindow::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc( this );

    PrepareDC( dc );

    int dev_x, dev_y;
    m_owner->CalcScrolledPosition( 0, 0, &dev_x, &dev_y );

    dc.SetFont( GetFont() );
    
    dc.DrawText( wxT("main window"), 5, 5 );
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

void wxDataViewCtrl::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    wxDataViewCtrlBase::ScrollWindow( dx, dy, rect );
    m_headerArea->ScrollWindow( dx, 0, rect );
}

bool wxDataViewCtrl::AssociateModel( wxDataViewListModel *model )
{
    if (!wxDataViewCtrlBase::AssociateModel( model ))
        return false;

    m_notifier = new wxGenericDataViewListModelNotifier( model );

    model->AddNotifier( m_notifier );    

    return true;
}

bool wxDataViewCtrl::AppendColumn( wxDataViewColumn *col )
{
    if (!wxDataViewCtrlBase::AppendColumn(col))
        return false;
        
    return true;
}

#endif 
    // !wxUSE_GENERICDATAVIEWCTRL

#endif 
    // wxUSE_DATAVIEWCTRL

