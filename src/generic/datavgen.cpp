/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/datavgen.cpp
// Purpose:     wxDataViewCtrl generic implementation
// Author:      Robert Roebling
// Modified by: Francesco Montorsi, Guru Kathiresan, Otto Wyss
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#ifdef wxUSE_GENERICDATAVIEWCTRL

#ifndef WX_PRECOMP
    #ifdef __WXMSW__
        #include "wx/msw/private.h"
        #include "wx/msw/wrapwin.h"
        #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #endif
    #include "wx/sizer.h"
    #include "wx/log.h"
    #include "wx/dcclient.h"
    #include "wx/timer.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/dcscreen.h"
#endif

#include "wx/stockitem.h"
#include "wx/calctrl.h"
#include "wx/popupwin.h"
#include "wx/renderer.h"
#include "wx/dcbuffer.h"
#include "wx/icon.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDataViewCtrl;

static const int SCROLL_UNIT_X = 15;

// the cell padding on the left/right
static const int PADDING_RIGHTLEFT = 3;

// the cell padding on the top/bottom
static const int PADDING_TOPBOTTOM = 1;


//-----------------------------------------------------------------------------
// wxDataViewHeaderWindow
//-----------------------------------------------------------------------------

#define USE_NATIVE_HEADER_WINDOW    1

// NB: for some reason, this class must be dllexport'ed or we get warnings from
//     MSVC in DLL build
class WXDLLIMPEXP_ADV wxDataViewHeaderWindowBase : public wxControl
{
public:
    wxDataViewHeaderWindowBase()
        { m_owner = NULL; }

    bool Create(wxDataViewCtrl *parent, wxWindowID id,
                const wxPoint &pos, const wxSize &size,
                const wxString &name)
    {
        return wxWindow::Create(parent, id, pos, size, wxNO_BORDER, name);
    }

    void SetOwner( wxDataViewCtrl* owner ) { m_owner = owner; }
    wxDataViewCtrl *GetOwner() { return m_owner; }

    // called on column addition/removal
    virtual void UpdateDisplay() { /* by default, do nothing */ }

    // returns the n-th column
    virtual wxDataViewColumn *GetColumn(unsigned int n)
    {
        wxASSERT(m_owner);
        wxDataViewColumn *ret = m_owner->GetColumn(n);
        wxASSERT(ret);

        return ret;
    }

protected:
    wxDataViewCtrl      *m_owner;

    // sends an event generated from the n-th wxDataViewColumn
    void SendEvent(wxEventType type, unsigned int n);
};

// on wxMSW the header window (only that part however) can be made native!
#if defined(__WXMSW__) && USE_NATIVE_HEADER_WINDOW

#define COLUMN_WIDTH_OFFSET         2
#define wxDataViewHeaderWindowMSW   wxDataViewHeaderWindow

class wxDataViewHeaderWindowMSW : public wxDataViewHeaderWindowBase
{
public:

    wxDataViewHeaderWindowMSW( wxDataViewCtrl *parent,
                               wxWindowID id,
                               const wxPoint &pos = wxDefaultPosition,
                               const wxSize &size = wxDefaultSize,
                               const wxString &name = wxT("wxdataviewctrlheaderwindow") )
    {
        Create(parent, id, pos, size, name);
    }

    bool Create(wxDataViewCtrl *parent, wxWindowID id,
                const wxPoint &pos, const wxSize &size,
                const wxString &name);

    ~wxDataViewHeaderWindowMSW();

    // called when any column setting is changed and/or changed
    // the column count
    virtual void UpdateDisplay();

    // called when the main window gets scrolled
    virtual void ScrollWindow(int dx, int dy, const wxRect *rect = NULL);

protected:
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags);

    unsigned int GetColumnIdxFromHeader(NMHEADER *nmHDR);

    wxDataViewColumn *GetColumnFromHeader(NMHEADER *nmHDR)
        { return GetColumn(GetColumnIdxFromHeader(nmHDR)); }

private:
    DECLARE_DYNAMIC_CLASS(wxDataViewHeaderWindowMSW)
};

#else       // !defined(__WXMSW__)

#define HEADER_WINDOW_HEIGHT            25
#define HEADER_HORIZ_BORDER             5
#define HEADER_VERT_BORDER              3
#define wxGenericDataViewHeaderWindow   wxDataViewHeaderWindow

class wxGenericDataViewHeaderWindow : public wxDataViewHeaderWindowBase
{
public:
    wxGenericDataViewHeaderWindow( wxDataViewCtrl *parent,
                                    wxWindowID id,
                                    const wxPoint &pos = wxDefaultPosition,
                                    const wxSize &size = wxDefaultSize,
                                    const wxString &name = wxT("wxdataviewctrlheaderwindow") )
    {
        Init();
        Create(parent, id, pos, size, name);
    }

    bool Create(wxDataViewCtrl *parent, wxWindowID id,
                const wxPoint &pos, const wxSize &size,
                const wxString &name);

    ~wxGenericDataViewHeaderWindow()
    {
        delete m_resizeCursor;
    }

    // event handlers:

    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );


protected:

    // vars used for column resizing:

    wxCursor            *m_resizeCursor;
    const wxCursor      *m_currentCursor;
    bool                 m_isDragging;

    bool                 m_dirty;     // needs refresh?
    int                  m_column;    // index of the column being resized
    int                  m_currentX;  // divider line position in logical (unscrolled) coords
    int                  m_minX;      // minimal position beyond which the divider line
                                      // can't be dragged in logical coords

    // the pen used to draw the current column width drag line
    // when resizing the columsn
    wxPen m_penCurrent;


    // internal utilities:

    void Init()
    {
        m_currentCursor = (wxCursor *) NULL;
        m_resizeCursor = new wxCursor( wxCURSOR_SIZEWE );

        m_isDragging = false;
        m_dirty = false;

        m_column = wxNOT_FOUND;
        m_currentX = 0;
        m_minX = 0;

        wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
        m_penCurrent = wxPen(col, 1, wxSOLID);
    }

    void DrawCurrent();
    void AdjustDC(wxDC& dc);

private:
    DECLARE_DYNAMIC_CLASS(wxGenericDataViewHeaderWindow)
    DECLARE_EVENT_TABLE()
};

#endif      // defined(__WXMSW__)

//-----------------------------------------------------------------------------
// wxDataViewRenameTimer
//-----------------------------------------------------------------------------

class wxDataViewRenameTimer: public wxTimer
{
private:
    wxDataViewMainWindow *m_owner;

public:
    wxDataViewRenameTimer( wxDataViewMainWindow *owner );
    void Notify();
};

//-----------------------------------------------------------------------------
// wxDataViewMainWindow
//-----------------------------------------------------------------------------

WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_SIZE_T(unsigned int, wxDataViewSelection,
                                            WXDLLIMPEXP_ADV);

class wxDataViewMainWindow: public wxWindow
{
public:
    wxDataViewMainWindow( wxDataViewCtrl *parent,
                            wxWindowID id,
                            const wxPoint &pos = wxDefaultPosition,
                            const wxSize &size = wxDefaultSize,
                            const wxString &name = wxT("wxdataviewctrlmainwindow") );
    virtual ~wxDataViewMainWindow();

    // notifications from wxDataViewListModel
    bool RowAppended();
    bool RowPrepended();
    bool RowInserted( unsigned int before );
    bool RowDeleted( unsigned int row );
    bool RowChanged( unsigned int row );
    bool ValueChanged( unsigned int col, unsigned int row );
    bool RowsReordered( unsigned int *new_order );
    bool Cleared();

    void SetOwner( wxDataViewCtrl* owner ) { m_owner = owner; }
    wxDataViewCtrl *GetOwner() { return m_owner; }
    const wxDataViewCtrl *GetOwner() const { return m_owner; }

    void OnPaint( wxPaintEvent &event );
    void OnArrowChar(unsigned int newCurrent, const wxKeyEvent& event);
    void OnChar( wxKeyEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );

    void UpdateDisplay();
    void RecalculateDisplay();
    void OnInternalIdle();

    void OnRenameTimer();

    void ScrollWindow( int dx, int dy, const wxRect *rect = NULL );

    bool HasCurrentRow() { return m_currentRow != (unsigned int)-1; }
    void ChangeCurrentRow( unsigned int row );

    bool IsSingleSel() const { return !GetParent()->HasFlag(wxDV_MULTIPLE); }
    bool IsEmpty() { return GetRowCount() == 0; }

    int GetCountPerPage() const;
    int GetEndOfLastCol() const;
    unsigned int GetFirstVisibleRow() const;
    unsigned int GetLastVisibleRow() const;
    unsigned int GetRowCount() const;

    void Select( const wxArrayInt& aSelections );
    void SelectAllRows( bool on );
    void SelectRow( unsigned int row, bool on );
    void SelectRows( unsigned int from, unsigned int to, bool on );
    void ReverseRowSelection( unsigned int row );
    bool IsRowSelected( unsigned int row );

    void RefreshRow( unsigned int row );
    void RefreshRows( unsigned int from, unsigned int to );
    void RefreshRowsAfter( unsigned int firstRow );

    // returns the colour to be used for drawing the rules
    wxColour GetRuleColour() const
    {
        return wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
    }

    //void EnsureVisible( unsigned int row );
    wxRect GetLineRect( unsigned int row ) const;

private:
    wxDataViewCtrl             *m_owner;
    int                         m_lineHeight;
    bool                        m_dirty;

    wxDataViewColumn           *m_currentCol;
    unsigned int                m_currentRow;
    wxDataViewSelection         m_selection;

    wxDataViewRenameTimer      *m_renameTimer;
    bool                        m_lastOnSame;

    bool                        m_hasFocus;

    int                         m_dragCount;
    wxPoint                     m_dragStart;

    // for double click logic
    unsigned int m_lineLastClicked,
           m_lineBeforeLastClicked,
           m_lineSelectSingleOnUp;

    // the pen used to draw horiz/vertical rules
    wxPen m_penRule;

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
    virtual bool RowInserted( unsigned int before )
        { return m_mainWindow->RowInserted( before ); }
    virtual bool RowDeleted( unsigned int row )
        { return m_mainWindow->RowDeleted( row ); }
    virtual bool RowChanged( unsigned int row )
        { return m_mainWindow->RowChanged( row ); }
    virtual bool ValueChanged( unsigned int col, unsigned int row )
        { return m_mainWindow->ValueChanged( col, row ); }
    virtual bool RowsReordered( unsigned int *new_order )
        { return m_mainWindow->RowsReordered( new_order ); }
    virtual bool Cleared()
        { return m_mainWindow->Cleared(); }

    wxDataViewMainWindow    *m_mainWindow;
};

// ---------------------------------------------------------
// wxDataViewRenderer
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewRenderer, wxDataViewRendererBase)

wxDataViewRenderer::wxDataViewRenderer( const wxString &varianttype,
                                        wxDataViewCellMode mode,
                                        int align) :
    wxDataViewRendererBase( varianttype, mode, align )
{
    m_dc = NULL;
    m_align = align;
    m_mode = mode;
}

wxDataViewRenderer::~wxDataViewRenderer()
{
    if (m_dc)
        delete m_dc;
}

wxDC *wxDataViewRenderer::GetDC()
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
// wxDataViewCustomRenderer
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewCustomRenderer, wxDataViewRenderer)

wxDataViewCustomRenderer::wxDataViewCustomRenderer( const wxString &varianttype,
                          wxDataViewCellMode mode, int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
}

// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------

IMPLEMENT_CLASS(wxDataViewTextRenderer, wxDataViewCustomRenderer)

wxDataViewTextRenderer::wxDataViewTextRenderer( const wxString &varianttype,
                                                wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align )
{
}

bool wxDataViewTextRenderer::SetValue( const wxVariant &value )
{
    m_text = value.GetString();

    return true;
}

bool wxDataViewTextRenderer::GetValue( wxVariant& WXUNUSED(value) ) const
{
    return false;
}

bool wxDataViewTextRenderer::HasEditorCtrl()
{ 
    return true;
}

wxControl* wxDataViewTextRenderer::CreateEditorCtrl( wxWindow *parent,
        wxRect labelRect, const wxVariant &value )
{
    return new wxTextCtrl( parent, wxID_ANY, value, 
                           wxPoint(labelRect.x,labelRect.y),
                           wxSize(labelRect.width,labelRect.height) );
}

bool wxDataViewTextRenderer::GetValueFromEditorCtrl( wxControl *editor, wxVariant &value )
{
    wxTextCtrl *text = (wxTextCtrl*) editor;
    value = text->GetValue();
    return true;
}

bool wxDataViewTextRenderer::Render( wxRect cell, wxDC *dc, int state )
{
    wxDataViewCtrl *view = GetOwner()->GetOwner();
    wxColour col = (state & wxDATAVIEW_CELL_SELECTED) ?
                        wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT) :
                        view->GetForegroundColour();

    dc->SetTextForeground(col);
    dc->DrawText( m_text, cell.x, cell.y );

    return true;
}

wxSize wxDataViewTextRenderer::GetSize() const
{
    const wxDataViewCtrl *view = GetView();
    if (!m_text.empty())
    {
        int x,y;
        view->GetTextExtent( m_text, &x, &y );
        return wxSize( x, y );
    }
    return wxSize(80,20);
}

// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------

IMPLEMENT_CLASS(wxDataViewBitmapRenderer, wxDataViewCustomRenderer)

wxDataViewBitmapRenderer::wxDataViewBitmapRenderer( const wxString &varianttype,
                                                    wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align )
{
}

bool wxDataViewBitmapRenderer::SetValue( const wxVariant &value )
{
    if (value.GetType() == wxT("wxBitmap"))
        m_bitmap << value;
    if (value.GetType() == wxT("wxIcon"))
        m_icon << value;

    return true;
}

bool wxDataViewBitmapRenderer::GetValue( wxVariant& WXUNUSED(value) ) const
{
    return false;
}

bool wxDataViewBitmapRenderer::Render( wxRect cell, wxDC *dc, int WXUNUSED(state) )
{
    if (m_bitmap.Ok())
        dc->DrawBitmap( m_bitmap, cell.x, cell.y );
    else if (m_icon.Ok())
        dc->DrawIcon( m_icon, cell.x, cell.y );

    return true;
}

wxSize wxDataViewBitmapRenderer::GetSize() const
{
    if (m_bitmap.Ok())
        return wxSize( m_bitmap.GetWidth(), m_bitmap.GetHeight() );
    else if (m_icon.Ok())
        return wxSize( m_icon.GetWidth(), m_icon.GetHeight() );

    return wxSize(16,16);
}

// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewToggleRenderer, wxDataViewCustomRenderer)

wxDataViewToggleRenderer::wxDataViewToggleRenderer( const wxString &varianttype,
                        wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align )
{
    m_toggle = false;
}

bool wxDataViewToggleRenderer::SetValue( const wxVariant &value )
{
    m_toggle = value.GetBool();

    return true;
}

bool wxDataViewToggleRenderer::GetValue( wxVariant &WXUNUSED(value) ) const
{
    return false;
}

bool wxDataViewToggleRenderer::Render( wxRect cell, wxDC *dc, int WXUNUSED(state) )
{
    // User wxRenderer here

    wxRect rect;
    rect.x = cell.x + cell.width/2 - 10;
    rect.width = 20;
    rect.y = cell.y + cell.height/2 - 10;
    rect.height = 20;

    int flags = 0;
    if (m_toggle)
        flags |= wxCONTROL_CHECKED;
    if (GetMode() != wxDATAVIEW_CELL_ACTIVATABLE)
        flags |= wxCONTROL_DISABLED;

    wxRendererNative::Get().DrawCheckBox(
            GetOwner()->GetOwner(),
            *dc,
            rect,
            flags );

    return true;
}

bool wxDataViewToggleRenderer::Activate( wxRect WXUNUSED(cell),
                                         wxDataViewListModel *model,
                                         unsigned int col, unsigned int row )
{
    bool value = !m_toggle;
    wxVariant variant = value;
    model->SetValue( variant, col, row );
    model->ValueChanged( col, row );
    return true;
}

wxSize wxDataViewToggleRenderer::GetSize() const
{
    return wxSize(20,20);
}

// ---------------------------------------------------------
// wxDataViewProgressRenderer
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewProgressRenderer, wxDataViewCustomRenderer)

wxDataViewProgressRenderer::wxDataViewProgressRenderer( const wxString &label,
    const wxString &varianttype, wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align )
{
    m_label = label;
    m_value = 0;
}

wxDataViewProgressRenderer::~wxDataViewProgressRenderer()
{
}

bool wxDataViewProgressRenderer::SetValue( const wxVariant &value )
{
    m_value = (long) value;

    if (m_value < 0) m_value = 0;
    if (m_value > 100) m_value = 100;

    return true;
}

bool wxDataViewProgressRenderer::GetValue( wxVariant &value ) const
{
    value = (long) m_value;
    return true;
}

bool wxDataViewProgressRenderer::Render( wxRect cell, wxDC *dc, int WXUNUSED(state) )
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

wxSize wxDataViewProgressRenderer::GetSize() const
{
    return wxSize(40,12);
}

// ---------------------------------------------------------
// wxDataViewDateRenderer
// ---------------------------------------------------------

#define wxUSE_DATE_RENDERER_POPUP (wxUSE_CALENDARCTRL && wxUSE_POPUPWIN)

#if wxUSE_DATE_RENDERER_POPUP

class wxDataViewDateRendererPopupTransient: public wxPopupTransientWindow
{
public:
    wxDataViewDateRendererPopupTransient( wxWindow* parent, wxDateTime *value,
        wxDataViewListModel *model, unsigned int col, unsigned int row ) :
        wxPopupTransientWindow( parent, wxBORDER_SIMPLE )
    {
        m_model = model;
        m_col = col;
        m_row = row;
        m_cal = new wxCalendarCtrl( this, wxID_ANY, *value );
        wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
        sizer->Add( m_cal, 1, wxGROW );
        SetSizer( sizer );
        sizer->Fit( this );
    }

    void OnCalendar( wxCalendarEvent &event );

    wxCalendarCtrl      *m_cal;
    wxDataViewListModel *m_model;
    unsigned int               m_col;
    unsigned int               m_row;

protected:
    virtual void OnDismiss()
    {
    }

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxDataViewDateRendererPopupTransient,wxPopupTransientWindow)
    EVT_CALENDAR( wxID_ANY, wxDataViewDateRendererPopupTransient::OnCalendar )
END_EVENT_TABLE()

void wxDataViewDateRendererPopupTransient::OnCalendar( wxCalendarEvent &event )
{
    wxDateTime date = event.GetDate();
    wxVariant value = date;
    m_model->SetValue( value, m_col, m_row );
    m_model->ValueChanged( m_col, m_row );
    DismissAndNotify();
}

#endif // wxUSE_DATE_RENDERER_POPUP

IMPLEMENT_ABSTRACT_CLASS(wxDataViewDateRenderer, wxDataViewCustomRenderer)

wxDataViewDateRenderer::wxDataViewDateRenderer( const wxString &varianttype,
                        wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align )
{
}

bool wxDataViewDateRenderer::SetValue( const wxVariant &value )
{
    m_date = value.GetDateTime();

    return true;
}

bool wxDataViewDateRenderer::GetValue( wxVariant &value ) const
{
    value = m_date;
    return true;
}

bool wxDataViewDateRenderer::Render( wxRect cell, wxDC *dc, int WXUNUSED(state) )
{
    dc->SetFont( GetOwner()->GetOwner()->GetFont() );
    wxString tmp = m_date.FormatDate();
    dc->DrawText( tmp, cell.x, cell.y );

    return true;
}

wxSize wxDataViewDateRenderer::GetSize() const
{
    const wxDataViewCtrl* view = GetView();
    wxString tmp = m_date.FormatDate();
    wxCoord x,y,d;
    view->GetTextExtent( tmp, &x, &y, &d );
    return wxSize(x,y+d);
}

bool wxDataViewDateRenderer::Activate( wxRect WXUNUSED(cell), wxDataViewListModel *model,
                                       unsigned int col, unsigned int row )
{
    wxVariant variant;
    model->GetValue( variant, col, row );
    wxDateTime value = variant.GetDateTime();

#if wxUSE_DATE_RENDERER_POPUP
    wxDataViewDateRendererPopupTransient *popup = new wxDataViewDateRendererPopupTransient(
        GetOwner()->GetOwner()->GetParent(), &value, model, col, row );
    wxPoint pos = wxGetMousePosition();
    popup->Move( pos );
    popup->Layout();
    popup->Popup( popup->m_cal );
#else // !wxUSE_DATE_RENDERER_POPUP
    wxMessageBox(value.Format());
#endif // wxUSE_DATE_RENDERER_POPUP/!wxUSE_DATE_RENDERER_POPUP
    return true;
}

// ---------------------------------------------------------
// wxDataViewColumn
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewColumn, wxDataViewColumnBase)

wxDataViewColumn::wxDataViewColumn( const wxString &title, wxDataViewRenderer *cell,
                                    unsigned int model_column,
                                    int width, wxAlignment align, int flags ) :
    wxDataViewColumnBase( title, cell, model_column, width, align, flags )
{
    SetAlignment(align);
    SetTitle(title);
    SetFlags(flags);

    Init(width < 0 ? wxDVC_DEFAULT_WIDTH : width);
}

wxDataViewColumn::wxDataViewColumn( const wxBitmap &bitmap, wxDataViewRenderer *cell,
                                    unsigned int model_column,
                                    int width, wxAlignment align, int flags ) :
    wxDataViewColumnBase( bitmap, cell, model_column, width, align, flags )
{
    SetAlignment(align);
    SetFlags(flags);

    Init(width < 0 ? wxDVC_TOGGLE_DEFAULT_WIDTH : width);
}

wxDataViewColumn::~wxDataViewColumn()
{
}

void wxDataViewColumn::Init( int width )
{
    m_width = width;
    m_minWidth = wxDVC_DEFAULT_MINWIDTH;
}

void wxDataViewColumn::SetResizeable( bool resizeable )
{
    if (resizeable)
        m_flags |= wxDATAVIEW_COL_RESIZABLE;
    else
        m_flags &= ~wxDATAVIEW_COL_RESIZABLE;
}

void wxDataViewColumn::SetHidden( bool hidden )
{
    if (hidden)
        m_flags |= wxDATAVIEW_COL_HIDDEN;
    else
        m_flags &= ~wxDATAVIEW_COL_HIDDEN;

    // tell our owner to e.g. update its scrollbars:
    if (GetOwner())
        GetOwner()->OnColumnChange();
}

void wxDataViewColumn::SetSortable( bool sortable )
{
    if (sortable)
        m_flags |= wxDATAVIEW_COL_SORTABLE;
    else
        m_flags &= ~wxDATAVIEW_COL_SORTABLE;
}

void wxDataViewColumn::SetSortOrder( bool WXUNUSED(ascending) )
{
    // TODO
}

bool wxDataViewColumn::IsSortOrderAscending() const
{
    // TODO
    return true;
}

void wxDataViewColumn::SetInternalWidth( int width )
{
    m_width = width;

    // the scrollbars of the wxDataViewCtrl needs to be recalculated!
    if (m_owner && m_owner->m_clientArea)
        m_owner->m_clientArea->RecalculateDisplay();
}

void wxDataViewColumn::SetWidth( int width )
{
    m_owner->m_headerArea->UpdateDisplay();

    SetInternalWidth(width);
}


//-----------------------------------------------------------------------------
// wxDataViewHeaderWindowBase
//-----------------------------------------------------------------------------

void wxDataViewHeaderWindowBase::SendEvent(wxEventType type, unsigned int n)
{
    wxWindow *parent = GetParent();
    wxDataViewEvent le(type, parent->GetId());

    le.SetEventObject(parent);
    le.SetColumn(n);
    le.SetDataViewColumn(GetColumn(n));
    le.SetModel(GetOwner()->GetModel());

    // for events created by wxDataViewHeaderWindow the
    // row / value fields are not valid

    parent->GetEventHandler()->ProcessEvent(le);
}

#if defined(__WXMSW__) && USE_NATIVE_HEADER_WINDOW

// implemented in msw/listctrl.cpp:
int WXDLLIMPEXP_CORE wxMSWGetColumnClicked(NMHDR *nmhdr, POINT *ptClick);

IMPLEMENT_ABSTRACT_CLASS(wxDataViewHeaderWindowMSW, wxWindow)

bool wxDataViewHeaderWindowMSW::Create( wxDataViewCtrl *parent, wxWindowID id,
                                        const wxPoint &pos, const wxSize &size,
                                        const wxString &name )
{
    m_owner = parent;

    if ( !CreateControl(parent, id, pos, size, 0, wxDefaultValidator, name) )
        return false;

    int x = pos.x == wxDefaultCoord ? 0 : pos.x,
        y = pos.y == wxDefaultCoord ? 0 : pos.y,
        w = size.x == wxDefaultCoord ? 1 : size.x,
        h = size.y == wxDefaultCoord ? 22 : size.y;

    // create the native WC_HEADER window:
    WXHWND hwndParent = (HWND)parent->GetHandle();
    WXDWORD msStyle = WS_CHILD | HDS_BUTTONS | HDS_HORZ | HDS_HOTTRACK | HDS_FULLDRAG;
    m_hWnd = CreateWindowEx(0,
                            WC_HEADER,
                            (LPCTSTR) NULL,
                            msStyle,
                            x, y, w, h,
                            (HWND)hwndParent,
                            (HMENU)-1,
                            wxGetInstance(),
                            (LPVOID) NULL);
    if (m_hWnd == NULL)
    {
        wxLogLastError(_T("CreateWindowEx"));
        return false;
    }

    // we need to subclass the m_hWnd to force wxWindow::HandleNotify
    // to call wxDataViewHeaderWindow::MSWOnNotify
    SubclassWin(m_hWnd);

    // the following is required to get the default win's font for
    // header windows and must be done befor sending the HDM_LAYOUT msg
    SetFont(GetFont());

    RECT rcParent;
    HDLAYOUT hdl;
    WINDOWPOS wp;

    // Retrieve the bounding rectangle of the parent window's
    // client area, and then request size and position values
    // from the header control.
    ::GetClientRect((HWND)hwndParent, &rcParent);

    hdl.prc = &rcParent;
    hdl.pwpos = &wp;
    if (!SendMessage((HWND)m_hWnd, HDM_LAYOUT, 0, (LPARAM) &hdl))
    {
        wxLogLastError(_T("SendMessage"));
        return false;
    }

    // Set the size, position, and visibility of the header control.
    SetWindowPos((HWND)m_hWnd,
                 wp.hwndInsertAfter,
                 wp.x, wp.y,
                 wp.cx, wp.cy,
                 wp.flags | SWP_SHOWWINDOW);

    // set our size hints: wxDataViewCtrl will put this wxWindow inside
    // a wxBoxSizer and in order to avoid super-big header windows,
    // we need to set our height as fixed
    SetMinSize(wxSize(-1, wp.cy));
    SetMaxSize(wxSize(-1, wp.cy));

    return true;
}

wxDataViewHeaderWindowMSW::~wxDataViewHeaderWindow()
{
    UnsubclassWin();
}

void wxDataViewHeaderWindowMSW::UpdateDisplay()
{
    // remove old columns
    for (int j=0, max=Header_GetItemCount((HWND)m_hWnd); j < max; j++)
        Header_DeleteItem((HWND)m_hWnd, 0);

    // add the updated array of columns to the header control
    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int added = 0;
    for (unsigned int i = 0; i < cols; i++)
    {
        wxDataViewColumn *col = GetColumn( i );
        if (col->IsHidden())
            continue;      // don't add it!

        HDITEM hdi;
        hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
        hdi.pszText = (wxChar *) col->GetTitle().wx_str();
        hdi.cxy = col->GetWidth();
        hdi.cchTextMax = sizeof(hdi.pszText)/sizeof(hdi.pszText[0]);
        hdi.fmt = HDF_LEFT | HDF_STRING;

        // lParam is reserved for application's use:
        // we store there the column index to use it later in MSWOnNotify
        // (since columns may have been hidden)
        hdi.lParam = (LPARAM)i;

        // the native wxMSW implementation of the header window
        // draws the column separator COLUMN_WIDTH_OFFSET pixels
        // on the right: to correct this effect we make the column
        // exactly COLUMN_WIDTH_OFFSET wider (for the first column):
        if (i == 0)
            hdi.cxy += COLUMN_WIDTH_OFFSET;

        switch (col->GetAlignment())
        {
        case wxALIGN_LEFT:
            hdi.fmt |= HDF_LEFT;
            break;
        case wxALIGN_CENTER:
        case wxALIGN_CENTER_HORIZONTAL:
            hdi.fmt |= HDF_CENTER;
            break;
        case wxALIGN_RIGHT:
            hdi.fmt |= HDF_RIGHT;
            break;

        default:
            // such alignment is not allowed for the column header!
            wxFAIL;
        }

        SendMessage((HWND)m_hWnd, HDM_INSERTITEM,
                    (WPARAM)added, (LPARAM)&hdi);
        added++;
    }
}

unsigned int wxDataViewHeaderWindowMSW::GetColumnIdxFromHeader(NMHEADER *nmHDR)
{
    unsigned int idx;

    // NOTE: we don't just return nmHDR->iItem because when there are
    //       hidden columns, nmHDR->iItem may be different from
    //       nmHDR->pitem->lParam

    if (nmHDR->pitem && nmHDR->pitem->mask & HDI_LPARAM)
    {
        idx = (unsigned int)nmHDR->pitem->lParam;
        return idx;
    }

    HDITEM item;
    item.mask = HDI_LPARAM;
    Header_GetItem((HWND)m_hWnd, nmHDR->iItem, &item);

    return (unsigned int)item.lParam;
}

bool wxDataViewHeaderWindowMSW::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    NMHDR *nmhdr = (NMHDR *)lParam;

    // is it a message from the header?
    if ( nmhdr->hwndFrom != (HWND)m_hWnd )
        return wxWindow::MSWOnNotify(idCtrl, lParam, result);

    NMHEADER *nmHDR = (NMHEADER *)nmhdr;
    switch ( nmhdr->code )
    {
        case HDN_BEGINTRACK:
            // user has started to resize a column:
            // do we need to veto it?
            if (!GetColumn(nmHDR->iItem)->IsResizeable())
            {
                // veto it!
                *result = TRUE;
            }
            break;

        case HDN_BEGINDRAG:
            // user has started to reorder a column
            break;

        case HDN_ITEMCHANGING:
            if (nmHDR->pitem != NULL &&
                (nmHDR->pitem->mask & HDI_WIDTH) != 0)
            {
                int minWidth = GetColumnFromHeader(nmHDR)->GetMinWidth();
                if (nmHDR->pitem->cxy < minWidth)
                {
                    // do not allow the user to resize this column under
                    // its minimal width:
                    *result = TRUE;
                }
            }
            break;

        case HDN_ITEMCHANGED:   // user is resizing a column
        case HDN_ENDTRACK:      // user has finished resizing a column
        case HDN_ENDDRAG:       // user has finished reordering a column

            // update the width of the modified column:
            if (nmHDR->pitem != NULL &&
                (nmHDR->pitem->mask & HDI_WIDTH) != 0)
            {
                unsigned int idx = GetColumnIdxFromHeader(nmHDR);
                unsigned int w = nmHDR->pitem->cxy;
                wxDataViewColumn *col = GetColumn(idx);

                // see UpdateDisplay() for more info about COLUMN_WIDTH_OFFSET
                if (idx == 0 && w > COLUMN_WIDTH_OFFSET)
                    w -= COLUMN_WIDTH_OFFSET;

                if (w >= (unsigned)col->GetMinWidth())
                    col->SetInternalWidth(w);
            }
            break;

        case HDN_ITEMCLICK:
            {
                unsigned int idx = GetColumnIdxFromHeader(nmHDR);
                wxEventType evt = nmHDR->iButton == 0 ?
                        wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK :
                        wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK;
                SendEvent(evt, idx);
            }
            break;

        case NM_RCLICK:
            {
                // NOTE: for some reason (i.e. for a bug in Windows)
                //       the HDN_ITEMCLICK notification is not sent on
                //       right clicks, so we need to handle NM_RCLICK

                POINT ptClick;
                int column = wxMSWGetColumnClicked(nmhdr, &ptClick);
                if (column != wxNOT_FOUND)
                {
                    HDITEM item;
                    item.mask = HDI_LPARAM;
                    Header_GetItem((HWND)m_hWnd, column, &item);

                    // 'idx' may be different from 'column' if there are
                    // hidden columns...
                    unsigned int idx = (unsigned int)item.lParam;
                    SendEvent(wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK,
                              idx);
                }
            }
            break;

        case HDN_GETDISPINFOW:
            // see wxListCtrl::MSWOnNotify for more info!
            break;

        case HDN_ITEMDBLCLICK:
            {
                unsigned int idx = GetColumnIdxFromHeader(nmHDR);
                int w = GetOwner()->GetBestColumnWidth(idx);

                // update the native control:
                HDITEM hd;
                ZeroMemory(&hd, sizeof(hd));
                hd.mask = HDI_WIDTH;
                hd.cxy = w;
                Header_SetItem(GetHwnd(),
                               nmHDR->iItem,  // NOTE: we don't want 'idx' here!
                               &hd);

                // update the wxDataViewColumn class:
                GetColumn(idx)->SetInternalWidth(w);
            }
            break;

        default:
            return wxWindow::MSWOnNotify(idCtrl, lParam, result);
    }

    return true;
}

void wxDataViewHeaderWindowMSW::ScrollWindow(int WXUNUSED(dx), int WXUNUSED(dy),
                                             const wxRect *WXUNUSED(rect))
{
    wxSize ourSz = GetClientSize();
    wxSize ownerSz = m_owner->GetClientSize();

    // where should the (logical) origin of this window be placed?
    int x1 = 0, y1 = 0;
    m_owner->CalcUnscrolledPosition(0, 0, &x1, &y1);

    // put this window on top of our parent and
    SetWindowPos((HWND)m_hWnd, HWND_TOP, -x1, 0,
                  ownerSz.GetWidth() + x1, ourSz.GetHeight(),
                  SWP_SHOWWINDOW);
}

void wxDataViewHeaderWindowMSW::DoSetSize(int WXUNUSED(x), int WXUNUSED(y),
                                          int WXUNUSED(w), int WXUNUSED(h),
                                          int WXUNUSED(f))
{
    // the wxDataViewCtrl's internal wxBoxSizer will call this function when
    // the wxDataViewCtrl window gets resized: the following dummy call
    // to ScrollWindow() is required in order to get this header window
    // correctly repainted when it's (horizontally) scrolled:

    ScrollWindow(0, 0);
}

#else       // !defined(__WXMSW__)

IMPLEMENT_ABSTRACT_CLASS(wxGenericDataViewHeaderWindow, wxWindow)
BEGIN_EVENT_TABLE(wxGenericDataViewHeaderWindow, wxWindow)
    EVT_PAINT         (wxGenericDataViewHeaderWindow::OnPaint)
    EVT_MOUSE_EVENTS  (wxGenericDataViewHeaderWindow::OnMouse)
    EVT_SET_FOCUS     (wxGenericDataViewHeaderWindow::OnSetFocus)
END_EVENT_TABLE()

bool wxGenericDataViewHeaderWindow::Create(wxDataViewCtrl *parent, wxWindowID id,
                                           const wxPoint &pos, const wxSize &size,
                                           const wxString &name )
{
    m_owner = parent;

    if (!wxDataViewHeaderWindowBase::Create(parent, id, pos, size, name))
        return false;

    wxVisualAttributes attr = wxPanel::GetClassDefaultAttributes();
    SetBackgroundStyle( wxBG_STYLE_CUSTOM );
    SetOwnForegroundColour( attr.colFg );
    SetOwnBackgroundColour( attr.colBg );
    if (!m_hasFont)
        SetOwnFont( attr.font );

    // set our size hints: wxDataViewCtrl will put this wxWindow inside
    // a wxBoxSizer and in order to avoid super-big header windows,
    // we need to set our height as fixed
    SetMinSize(wxSize(-1, HEADER_WINDOW_HEIGHT));
    SetMaxSize(wxSize(-1, HEADER_WINDOW_HEIGHT));

    return true;
}

void wxGenericDataViewHeaderWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    int w, h;
    GetClientSize( &w, &h );

    wxAutoBufferedPaintDC dc( this );

    dc.SetBackground(GetBackgroundColour());
    dc.Clear();

    int xpix;
    m_owner->GetScrollPixelsPerUnit( &xpix, NULL );

    int x;
    m_owner->GetViewStart( &x, NULL );

    // account for the horz scrollbar offset
    dc.SetDeviceOrigin( -x * xpix, 0 );

    dc.SetFont( GetFont() );

    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int i;
    int xpos = 0;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *col = GetColumn( i );
        if (col->IsHidden())
            continue;      // skip it!

        int cw = col->GetWidth();
        int ch = h;

        wxRendererNative::Get().DrawHeaderButton
                                (
                                    this,
                                    dc,
                                    wxRect(xpos, 0, cw, ch-1),
                                    m_parent->IsEnabled() ? 0
                                                          : (int)wxCONTROL_DISABLED
                                );

        // align as required the column title:
        int x = xpos;
        wxSize titleSz = dc.GetTextExtent(col->GetTitle());
        switch (col->GetAlignment())
        {
        case wxALIGN_LEFT:
            x += HEADER_HORIZ_BORDER;
            break;
        case wxALIGN_CENTER:
        case wxALIGN_CENTER_HORIZONTAL:
            x += (cw - titleSz.GetWidth() - 2 * HEADER_HORIZ_BORDER)/2;
            break;
        case wxALIGN_RIGHT:
            x += cw - titleSz.GetWidth() - HEADER_HORIZ_BORDER;
            break;
        }

        // always center the title vertically:
        int y = wxMax((ch - titleSz.GetHeight()) / 2, HEADER_VERT_BORDER);

        dc.SetClippingRegion( xpos+HEADER_HORIZ_BORDER,
                              HEADER_VERT_BORDER,
                              wxMax(cw - 2 * HEADER_HORIZ_BORDER, 1),  // width
                              wxMax(ch - 2 * HEADER_VERT_BORDER, 1));  // height
        dc.DrawText( col->GetTitle(), x, y );
        dc.DestroyClippingRegion();

        xpos += cw;
    }
}

void wxGenericDataViewHeaderWindow::OnSetFocus( wxFocusEvent &event )
{
    GetParent()->SetFocus();
    event.Skip();
}

void wxGenericDataViewHeaderWindow::OnMouse( wxMouseEvent &event )
{
    // we want to work with logical coords
    int x;
    m_owner->CalcUnscrolledPosition(event.GetX(), 0, &x, NULL);
    int y = event.GetY();

    if (m_isDragging)
    {
        // we don't draw the line beyond our window,
        // but we allow dragging it there
        int w = 0;
        GetClientSize( &w, NULL );
        m_owner->CalcUnscrolledPosition(w, 0, &w, NULL);
        w -= 6;

        // erase the line if it was drawn
        if (m_currentX < w)
            DrawCurrent();

        if (event.ButtonUp())
        {
            m_isDragging = false;
            if (HasCapture())
                ReleaseMouse();

            m_dirty = true;

            GetColumn(m_column)->SetWidth(m_currentX - m_minX);

            Refresh();
            GetOwner()->Refresh();
        }
        else
        {
            m_currentX = wxMax(m_minX + 7, x);

            // draw in the new location
            if (m_currentX < w) DrawCurrent();
        }

    }
    else     // not dragging
    {
        m_minX = 0;
        m_column = wxNOT_FOUND;

        bool hit_border = false;

        // end of the current column
        int xpos = 0;

        // find the column where this event occured
        int countCol = m_owner->GetColumnCount();
        for (int column = 0; column < countCol; column++)
        {
            wxDataViewColumn *p = GetColumn(column);

            if (p->IsHidden())
                continue;   // skip if not shown

            xpos += p->GetWidth();
            m_column = column;
            if ((abs(x-xpos) < 3) && (y < 22))
            {
                hit_border = true;
                break;
            }

            if (x < xpos)
            {
                // inside the column
                break;
            }

            m_minX = xpos;
        }

        if (m_column == wxNOT_FOUND)
            return;

        bool resizeable = GetColumn(m_column)->IsResizeable();
        if (event.LeftDClick() && resizeable)
        {
            GetColumn(m_column)->SetWidth(GetOwner()->GetBestColumnWidth(m_column));
            Refresh();
        }
        else if (event.LeftDown() || event.RightUp())
        {
            if (hit_border && event.LeftDown() && resizeable)
            {
                m_isDragging = true;
                CaptureMouse();
                m_currentX = x;
                DrawCurrent();
            }
            else    // click on a column
            {
                wxEventType evt = event.LeftDown() ?
                        wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK :
                        wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK;
                SendEvent(evt, m_column);
            }
        }
        else if (event.Moving())
        {
            if (hit_border && resizeable)
                m_currentCursor = m_resizeCursor;
            else
                m_currentCursor = wxSTANDARD_CURSOR;

            SetCursor(*m_currentCursor);
        }
    }
}

void wxGenericDataViewHeaderWindow::DrawCurrent()
{
    int x1 = m_currentX;
    int y1 = 0;
    ClientToScreen (&x1, &y1);

    int x2 = m_currentX-1;
#ifdef __WXMSW__
    ++x2; // but why ????
#endif
    int y2 = 0;
    m_owner->GetClientSize( NULL, &y2 );
    m_owner->ClientToScreen( &x2, &y2 );

    wxScreenDC dc;
    dc.SetLogicalFunction(wxINVERT);
    dc.SetPen(m_penCurrent);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    AdjustDC(dc);
    dc.DrawLine(x1, y1, x2, y2);
}

void wxGenericDataViewHeaderWindow::AdjustDC(wxDC& dc)
{
    int xpix, x;

    m_owner->GetScrollPixelsPerUnit( &xpix, NULL );
    m_owner->GetViewStart( &x, NULL );

    // shift the DC origin to match the position of the main window horizontal
    // scrollbar: this allows us to always use logical coords
    dc.SetDeviceOrigin( -x * xpix, 0 );
}

#endif      // defined(__WXMSW__)

//-----------------------------------------------------------------------------
// wxDataViewRenameTimer
//-----------------------------------------------------------------------------

wxDataViewRenameTimer::wxDataViewRenameTimer( wxDataViewMainWindow *owner )
{
    m_owner = owner;
}

void wxDataViewRenameTimer::Notify()
{
    m_owner->OnRenameTimer();
}

//-----------------------------------------------------------------------------
// wxDataViewMainWindow
//-----------------------------------------------------------------------------

int LINKAGEMODE wxDataViewSelectionCmp( unsigned int row1, unsigned int row2 )
{
    if (row1 > row2) return 1;
    if (row1 == row2) return 0;
    return -1;
}


IMPLEMENT_ABSTRACT_CLASS(wxDataViewMainWindow, wxWindow)

BEGIN_EVENT_TABLE(wxDataViewMainWindow,wxWindow)
    EVT_PAINT         (wxDataViewMainWindow::OnPaint)
    EVT_MOUSE_EVENTS  (wxDataViewMainWindow::OnMouse)
    EVT_SET_FOCUS     (wxDataViewMainWindow::OnSetFocus)
    EVT_KILL_FOCUS    (wxDataViewMainWindow::OnKillFocus)
    EVT_CHAR          (wxDataViewMainWindow::OnChar)
END_EVENT_TABLE()

wxDataViewMainWindow::wxDataViewMainWindow( wxDataViewCtrl *parent, wxWindowID id,
    const wxPoint &pos, const wxSize &size, const wxString &name ) :
    wxWindow( parent, id, pos, size, wxWANTS_CHARS, name ),
    m_selection( wxDataViewSelectionCmp )

{
    SetOwner( parent );

    m_lastOnSame = false;
    m_renameTimer = new wxDataViewRenameTimer( this );

    // TODO: user better initial values/nothing selected
    m_currentCol = NULL;
    m_currentRow = 0;

    // TODO: we need to calculate this smartly
    m_lineHeight =
#ifdef __WXMSW__
        17;
#else
        20;
#endif
    wxASSERT(m_lineHeight > 2*PADDING_TOPBOTTOM);

    m_dragCount = 0;
    m_dragStart = wxPoint(0,0);
    m_lineLastClicked = (unsigned int) -1;
    m_lineBeforeLastClicked = (unsigned int) -1;
    m_lineSelectSingleOnUp = (unsigned int) -1;

    m_hasFocus = false;

    SetBackgroundStyle( wxBG_STYLE_CUSTOM );
    SetBackgroundColour( *wxWHITE );

    m_penRule = wxPen(GetRuleColour(), 1, wxSOLID);

    UpdateDisplay();
}

wxDataViewMainWindow::~wxDataViewMainWindow()
{
    delete m_renameTimer;
}

void wxDataViewMainWindow::OnRenameTimer()
{
    // We have to call this here because changes may just have
    // been made and no screen update taken place.
    if ( m_dirty )
        wxSafeYield();

    int xpos = 0;
    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int i;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *c = GetOwner()->GetColumn( i );
        if (c->IsHidden())
            continue;      // skip it!

        if (c == m_currentCol)
            break;
        xpos += c->GetWidth();
    }
    wxRect labelRect( xpos, m_currentRow * m_lineHeight,
                      m_currentCol->GetWidth(), m_lineHeight );

    GetOwner()->CalcScrolledPosition( labelRect.x, labelRect.y,
                                     &labelRect.x, &labelRect.y);

    m_currentCol->GetRenderer()->StartEditing( m_currentRow, labelRect );
}

bool wxDataViewMainWindow::RowAppended()
{
    UpdateDisplay();
    return true;
}

bool wxDataViewMainWindow::RowPrepended()
{
    UpdateDisplay();
    return true;
}

bool wxDataViewMainWindow::RowInserted( unsigned int WXUNUSED(before) )
{
    UpdateDisplay();
    return true;
}

bool wxDataViewMainWindow::RowDeleted( unsigned int WXUNUSED(row) )
{
    UpdateDisplay();
    return true;
}

bool wxDataViewMainWindow::RowChanged( unsigned int WXUNUSED(row) )
{
    UpdateDisplay();
    return true;
}

bool wxDataViewMainWindow::ValueChanged( unsigned int WXUNUSED(col), unsigned int row )
{
    // NOTE: to be valid, we cannot use e.g. INT_MAX - 1
#define MAX_VIRTUAL_WIDTH       100000

    wxRect rect( 0, row*m_lineHeight, MAX_VIRTUAL_WIDTH, m_lineHeight );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );
    Refresh( true, &rect );

    return true;
}

bool wxDataViewMainWindow::RowsReordered( unsigned int *WXUNUSED(new_order) )
{
    UpdateDisplay();
    return true;
}

bool wxDataViewMainWindow::Cleared()
{
    UpdateDisplay();
    return true;
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

    int width = GetEndOfLastCol();
    int height = model->GetRowCount() * m_lineHeight;

    SetVirtualSize( width, height );
    GetOwner()->SetScrollRate( 10, m_lineHeight );

    Refresh();
}

void wxDataViewMainWindow::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    wxWindow::ScrollWindow( dx, dy, rect );

    if (GetOwner()->m_headerArea)
        GetOwner()->m_headerArea->ScrollWindow( dx, 0 );
}

void wxDataViewMainWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxDataViewListModel *model = GetOwner()->GetModel();
    wxAutoBufferedPaintDC dc( this );

    // prepare the DC
    dc.SetBackground(GetBackgroundColour());
    dc.Clear();
    GetOwner()->PrepareDC( dc );
    dc.SetFont( GetFont() );

    wxRect update = GetUpdateRegion().GetBox();
    m_owner->CalcUnscrolledPosition( update.x, update.y, &update.x, &update.y );

    // compute which items needs to be redrawn
    unsigned int item_start = wxMax( 0, (update.y / m_lineHeight) );
    unsigned int item_count =
        wxMin( (int)(((update.y + update.height) / m_lineHeight) - item_start + 1),
               (int)(model->GetRowCount() - item_start) );
    unsigned int item_last = item_start + item_count;

    // compute which columns needs to be redrawn
    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int col_start = 0;
    unsigned int x_start = 0;
    for (x_start = 0; col_start < cols; col_start++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumn(col_start);
        if (col->IsHidden())
            continue;      // skip it!

        unsigned int w = col->GetWidth();
        if (x_start+w >= (unsigned int)update.x)
            break;

        x_start += w;
    }

    unsigned int col_last = col_start;
    unsigned int x_last = x_start;
    for (; col_last < cols; col_last++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumn(col_last);
        if (col->IsHidden())
            continue;      // skip it!

        if (x_last > (unsigned int)update.GetRight())
            break;

        x_last += col->GetWidth();
    }

    // Draw horizontal rules if required
    if ( m_owner->HasFlag(wxDV_HORIZ_RULES) )
    {
        dc.SetPen(m_penRule);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        for (unsigned int i = item_start; i <= item_last+1; i++)
        {
            int y = i * m_lineHeight;
            dc.DrawLine(x_start, y, x_last, y);
        }
    }

    // Draw vertical rules if required
    if ( m_owner->HasFlag(wxDV_VERT_RULES) )
    {
        dc.SetPen(m_penRule);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        int x = x_start;
        for (unsigned int i = col_start; i < col_last; i++)
        {
            wxDataViewColumn *col = GetOwner()->GetColumn(i);
            if (col->IsHidden())
                continue;       // skip it

            dc.DrawLine(x, item_start * m_lineHeight,
                        x, item_last * m_lineHeight);

            x += col->GetWidth();
        }

        // Draw last vertical rule
        dc.DrawLine(x, item_start * m_lineHeight,
                    x, item_last * m_lineHeight);
    }

    // redraw the background for the items which are selected/current
    for (unsigned int item = item_start; item < item_last; item++)
    {
        bool selected = m_selection.Index( item ) != wxNOT_FOUND;
        if (selected || item == m_currentRow)
        {
            int flags = selected ? (int)wxCONTROL_SELECTED : 0;
            if (item == m_currentRow)
                flags |= wxCONTROL_CURRENT;
            if (m_hasFocus)
                flags |= wxCONTROL_FOCUSED;

            wxRect rect( x_start, item*m_lineHeight, x_last, m_lineHeight );
            wxRendererNative::Get().DrawItemSelectionRect
                                (
                                    this,
                                    dc,
                                    rect,
                                    flags
                                );
        }
    }

    // redraw all cells for all rows which must be repainted and for all columns
    wxRect cell_rect;
    cell_rect.x = x_start;
    cell_rect.height = m_lineHeight;        // -1 is for the horizontal rules
    for (unsigned int i = col_start; i < col_last; i++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumn( i );
        wxDataViewRenderer *cell = col->GetRenderer();
        cell_rect.width = col->GetWidth();

        if (col->IsHidden())
            continue;       // skipt it!

        for (unsigned int item = item_start; item < item_last; item++)
        {
            // get the cell value and set it into the renderer
            wxVariant value;
            model->GetValue( value, col->GetModelColumn(), item );
            cell->SetValue( value );

            // update the y offset
            cell_rect.y = item * m_lineHeight;

            // cannot be bigger than allocated space
            wxSize size = cell->GetSize();
            size.x = wxMin( size.x + 2*PADDING_RIGHTLEFT, cell_rect.width );
            size.y = wxMin( size.y + 2*PADDING_TOPBOTTOM, cell_rect.height );

            wxRect item_rect(cell_rect.GetTopLeft(), size);
            int align = cell->GetAlignment();

            // horizontal alignment:
            item_rect.x = cell_rect.x;
            if (align & wxALIGN_CENTER_HORIZONTAL)
                item_rect.x = cell_rect.x + (cell_rect.width / 2) - (size.x / 2);
            else if (align & wxALIGN_RIGHT)
                item_rect.x = cell_rect.x + cell_rect.width - size.x;
            //else: wxALIGN_LEFT is the default

            // vertical alignment:
            item_rect.y = cell_rect.y;
            if (align & wxALIGN_CENTER_VERTICAL)
                item_rect.y = cell_rect.y + (cell_rect.height / 2) - (size.y / 2);
            else if (align & wxALIGN_BOTTOM)
                item_rect.y = cell_rect.y + cell_rect.height - size.y;
            //else: wxALIGN_TOP is the default

            // add padding
            item_rect.x += PADDING_RIGHTLEFT;
            item_rect.y += PADDING_TOPBOTTOM;
            item_rect.width = size.x - 2 * PADDING_RIGHTLEFT;
            item_rect.height = size.y - 2 * PADDING_TOPBOTTOM;

            int state = 0;
            if (m_selection.Index(item) != wxNOT_FOUND)
                state |= wxDATAVIEW_CELL_SELECTED;

            // TODO: it would be much more efficient to create a clipping
            //       region for the entire column being rendered (in the OnPaint
            //       of wxDataViewMainWindow) instead of a single clip region for
            //       each cell. However it would mean that each renderer should
            //       respect the given wxRect's top & bottom coords, eventually
            //       violating only the left & right coords - however the user can
            //       make its own renderer and thus we cannot be sure of that.
            dc.SetClippingRegion( item_rect );
            cell->Render( item_rect, &dc, state );
            dc.DestroyClippingRegion();
        }

        cell_rect.x += cell_rect.width;
    }
}

int wxDataViewMainWindow::GetCountPerPage() const
{
    wxSize size = GetClientSize();
    return size.y / m_lineHeight;
}

int wxDataViewMainWindow::GetEndOfLastCol() const
{
    int width = 0;
    unsigned int i;
    for (i = 0; i < GetOwner()->GetColumnCount(); i++)
    {
        const wxDataViewColumn *c =
            wx_const_cast(wxDataViewCtrl*, GetOwner())->GetColumn( i );

        if (!c->IsHidden())
            width += c->GetWidth();
    }
    return width;
}

unsigned int wxDataViewMainWindow::GetFirstVisibleRow() const
{
    int x = 0;
    int y = 0;
    m_owner->CalcUnscrolledPosition( x, y, &x, &y );

    return y / m_lineHeight;
}

unsigned int wxDataViewMainWindow::GetLastVisibleRow() const
{
    wxSize client_size = GetClientSize();
    m_owner->CalcUnscrolledPosition( client_size.x, client_size.y,
                                     &client_size.x, &client_size.y );

    return wxMin( GetRowCount()-1, ((unsigned)client_size.y/m_lineHeight)+1 );
}

unsigned int wxDataViewMainWindow::GetRowCount() const
{
    return wx_const_cast(wxDataViewCtrl*, GetOwner())->GetModel()->GetRowCount();
}

void wxDataViewMainWindow::ChangeCurrentRow( unsigned int row )
{
    m_currentRow = row;

    // send event
}

void wxDataViewMainWindow::SelectAllRows( bool on )
{
    if (IsEmpty())
        return;

    if (on)
    {
        m_selection.Clear();
        for (unsigned int i = 0; i < GetRowCount(); i++)
            m_selection.Add( i );
        Refresh();
    }
    else
    {
        unsigned int first_visible = GetFirstVisibleRow();
        unsigned int last_visible = GetLastVisibleRow();
        unsigned int i;
        for (i = 0; i < m_selection.GetCount(); i++)
        {
            unsigned int row = m_selection[i];
            if ((row >= first_visible) && (row <= last_visible))
                RefreshRow( row );
        }
        m_selection.Clear();
    }
}

void wxDataViewMainWindow::SelectRow( unsigned int row, bool on )
{
    if (m_selection.Index( row ) == wxNOT_FOUND)
    {
        if (on)
        {
            m_selection.Add( row );
            RefreshRow( row );
        }
    }
    else
    {
        if (!on)
        {
            m_selection.Remove( row );
            RefreshRow( row );
        }
    }
}

void wxDataViewMainWindow::SelectRows( unsigned int from, unsigned int to, bool on )
{
    if (from > to)
    {
        unsigned int tmp = from;
        from = to;
        to = tmp;
    }

    unsigned int i;
    for (i = from; i <= to; i++)
    {
        if (m_selection.Index( i ) == wxNOT_FOUND)
        {
            if (on)
                m_selection.Add( i );
        }
        else
        {
            if (!on)
                m_selection.Remove( i );
        }
    }
    RefreshRows( from, to );
}

void wxDataViewMainWindow::Select( const wxArrayInt& aSelections )
{
    for (size_t i=0; i < aSelections.GetCount(); i++)
    {
        int n = aSelections[i];

        m_selection.Add( n );
        RefreshRow( n );
    }
}

void wxDataViewMainWindow::ReverseRowSelection( unsigned int row )
{
    if (m_selection.Index( row ) == wxNOT_FOUND)
        m_selection.Add( row );
    else
        m_selection.Remove( row );
    RefreshRow( row );
}

bool wxDataViewMainWindow::IsRowSelected( unsigned int row )
{
    return (m_selection.Index( row ) != wxNOT_FOUND);
}

void wxDataViewMainWindow::RefreshRow( unsigned int row )
{
    wxRect rect( 0, row*m_lineHeight, GetEndOfLastCol(), m_lineHeight );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );

    wxSize client_size = GetClientSize();
    wxRect client_rect( 0, 0, client_size.x, client_size.y );
    wxRect intersect_rect = client_rect.Intersect( rect );
    if (intersect_rect.width > 0)
        Refresh( true, &intersect_rect );
}

void wxDataViewMainWindow::RefreshRows( unsigned int from, unsigned int to )
{
    if (from > to)
    {
        unsigned int tmp = to;
        to = from;
        from = tmp;
    }

    wxRect rect( 0, from*m_lineHeight, GetEndOfLastCol(), (to-from+1) * m_lineHeight );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );

    wxSize client_size = GetClientSize();
    wxRect client_rect( 0, 0, client_size.x, client_size.y );
    wxRect intersect_rect = client_rect.Intersect( rect );
    if (intersect_rect.width > 0)
        Refresh( true, &intersect_rect );
}

void wxDataViewMainWindow::RefreshRowsAfter( unsigned int firstRow )
{
    unsigned int count = GetRowCount();
    if (firstRow > count)
        return;

    wxRect rect( 0, firstRow*m_lineHeight, GetEndOfLastCol(), count * m_lineHeight );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );

    wxSize client_size = GetClientSize();
    wxRect client_rect( 0, 0, client_size.x, client_size.y );
    wxRect intersect_rect = client_rect.Intersect( rect );
    if (intersect_rect.width > 0)
        Refresh( true, &intersect_rect );
}

void wxDataViewMainWindow::OnArrowChar(unsigned int newCurrent, const wxKeyEvent& event)
{
    wxCHECK_RET( newCurrent < GetRowCount(),
                 _T("invalid item index in OnArrowChar()") );

    // if there is no selection, we cannot move it anywhere
    if (!HasCurrentRow())
        return;

    unsigned int oldCurrent = m_currentRow;

    // in single selection we just ignore Shift as we can't select several
    // items anyhow
    if ( event.ShiftDown() && !IsSingleSel() )
    {
        RefreshRow( oldCurrent );

        ChangeCurrentRow( newCurrent );

        // select all the items between the old and the new one
        if ( oldCurrent > newCurrent )
        {
            newCurrent = oldCurrent;
            oldCurrent = m_currentRow;
        }

        SelectRows( oldCurrent, newCurrent, true );
    }
    else // !shift
    {
        RefreshRow( oldCurrent );

        // all previously selected items are unselected unless ctrl is held
        if ( !event.ControlDown() )
            SelectAllRows(false);

        ChangeCurrentRow( newCurrent );

        if ( !event.ControlDown() )
            SelectRow( m_currentRow, true );
        else
            RefreshRow( m_currentRow );
    }

    //EnsureVisible( m_currentRow );
}

wxRect wxDataViewMainWindow::GetLineRect( unsigned int row ) const
{
    wxRect rect;
    rect.x = 0;
    rect.y = m_lineHeight * row;
    rect.width = GetEndOfLastCol();
    rect.height = m_lineHeight;

    return rect;
}

void wxDataViewMainWindow::OnChar( wxKeyEvent &event )
{
    if (event.GetKeyCode() == WXK_TAB)
    {
        wxNavigationKeyEvent nevent;
        nevent.SetWindowChange( event.ControlDown() );
        nevent.SetDirection( !event.ShiftDown() );
        nevent.SetEventObject( GetParent()->GetParent() );
        nevent.SetCurrentFocus( m_parent );
        if (GetParent()->GetParent()->GetEventHandler()->ProcessEvent( nevent ))
            return;
    }

    // no item -> nothing to do
    if (!HasCurrentRow())
    {
        event.Skip();
        return;
    }

    // don't use m_linesPerPage directly as it might not be computed yet
    const int pageSize = GetCountPerPage();
    wxCHECK_RET( pageSize, _T("should have non zero page size") );

    switch ( event.GetKeyCode() )
    {
        case WXK_UP:
            if ( m_currentRow > 0 )
                OnArrowChar( m_currentRow - 1, event );
            break;

        case WXK_DOWN:
            if ( m_currentRow < GetRowCount() - 1 )
                OnArrowChar( m_currentRow + 1, event );
            break;

        case WXK_END:
            if (!IsEmpty())
                OnArrowChar( GetRowCount() - 1, event );
            break;

        case WXK_HOME:
            if (!IsEmpty())
                OnArrowChar( 0, event );
            break;

        case WXK_PAGEUP:
            {
                int steps = pageSize - 1;
                int index = m_currentRow - steps;
                if (index < 0)
                    index = 0;

                OnArrowChar( index, event );
            }
            break;

        case WXK_PAGEDOWN:
            {
                int steps = pageSize - 1;
                unsigned int index = m_currentRow + steps;
                unsigned int count = GetRowCount();
                if ( index >= count )
                    index = count - 1;

                OnArrowChar( index, event );
            }
            break;

        default:
            event.Skip();
    }
}

void wxDataViewMainWindow::OnMouse( wxMouseEvent &event )
{
    if (event.GetEventType() == wxEVT_MOUSEWHEEL)
    {
        // let the base handle mouse wheel events.
        event.Skip();
        return;
    }

    int x = event.GetX();
    int y = event.GetY();
    m_owner->CalcUnscrolledPosition( x, y, &x, &y );

    wxDataViewColumn *col = NULL;

    int xpos = 0;
    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int i;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *c = GetOwner()->GetColumn( i );
        if (c->IsHidden())
            continue;      // skip it!

        if (x < xpos + c->GetWidth())
        {
            col = c;
            break;
        }
        xpos += c->GetWidth();
    }
    if (!col)
        return;
    wxDataViewRenderer *cell = col->GetRenderer();

    unsigned int current = y / m_lineHeight;

    if ((current > GetRowCount()) || (x > GetEndOfLastCol()))
    {
        // Unselect all if below the last row ?
        return;
    }

    wxDataViewListModel *model = GetOwner()->GetModel();

    if (event.Dragging())
    {
        if (m_dragCount == 0)
        {
            // we have to report the raw, physical coords as we want to be
            // able to call HitTest(event.m_pointDrag) from the user code to
            // get the item being dragged
            m_dragStart = event.GetPosition();
        }

        m_dragCount++;

        if (m_dragCount != 3)
            return;

        if (event.LeftIsDown())
        {
            // Notify cell about drag
        }
        return;
    }
    else
    {
        m_dragCount = 0;
    }

    bool forceClick = false;

    if (event.ButtonDClick())
    {
        m_renameTimer->Stop();
        m_lastOnSame = false;
    }

    if (event.LeftDClick())
    {
        if ( current == m_lineLastClicked )
        {
            if (cell->GetMode() == wxDATAVIEW_CELL_ACTIVATABLE)
            {
                wxVariant value;
                model->GetValue( value, col->GetModelColumn(), current );
                cell->SetValue( value );
                wxRect cell_rect( xpos, current * m_lineHeight,
                                  col->GetWidth(), m_lineHeight );
                cell->Activate( cell_rect, model, col->GetModelColumn(), current );
            }
            return;
        }
        else
        {
            // The first click was on another item, so don't interpret this as
            // a double click, but as a simple click instead
            forceClick = true;
        }
    }

    if (event.LeftUp())
    {
        if (m_lineSelectSingleOnUp != (unsigned int)-1)
        {
            // select single line
            SelectAllRows( false );
            SelectRow( m_lineSelectSingleOnUp, true );
        }

        if (m_lastOnSame)
        {
            if ((col == m_currentCol) && (current == m_currentRow) &&
                (cell->GetMode() == wxDATAVIEW_CELL_EDITABLE) )
            {
                m_renameTimer->Start( 100, true );
            }
        }

        m_lastOnSame = false;
        m_lineSelectSingleOnUp = (unsigned int)-1;
    }
    else
    {
        // This is necessary, because after a DnD operation in
        // from and to ourself, the up event is swallowed by the
        // DnD code. So on next non-up event (which means here and
        // now) m_lineSelectSingleOnUp should be reset.
        m_lineSelectSingleOnUp = (unsigned int)-1;
    }

    if (event.RightDown())
    {
        m_lineBeforeLastClicked = m_lineLastClicked;
        m_lineLastClicked = current;

        // If the item is already selected, do not update the selection.
        // Multi-selections should not be cleared if a selected item is clicked.
        if (!IsRowSelected(current))
        {
            SelectAllRows(false);
            ChangeCurrentRow(current);
            SelectRow(m_currentRow,true);
        }

        // notify cell about right click
        // cell->...

        // Allow generation of context menu event
        event.Skip();
    }
    else if (event.MiddleDown())
    {
        // notify cell about middle click
        // cell->...
    }
    if (event.LeftDown() || forceClick)
    {
#ifdef __WXMSW__
        SetFocus();
#endif

        m_lineBeforeLastClicked = m_lineLastClicked;
        m_lineLastClicked = current;

        unsigned int oldCurrentRow = m_currentRow;
        bool oldWasSelected = IsRowSelected(m_currentRow);

        bool cmdModifierDown = event.CmdDown();
        if ( IsSingleSel() || !(cmdModifierDown || event.ShiftDown()) )
        {
            if ( IsSingleSel() || !IsRowSelected(current) )
            {
                SelectAllRows( false );

                ChangeCurrentRow(current);

                SelectRow(m_currentRow,true);
            }
            else // multi sel & current is highlighted & no mod keys
            {
                m_lineSelectSingleOnUp = current;
                ChangeCurrentRow(current); // change focus
            }
        }
        else // multi sel & either ctrl or shift is down
        {
            if (cmdModifierDown)
            {
                ChangeCurrentRow(current);

                ReverseRowSelection(m_currentRow);
            }
            else if (event.ShiftDown())
            {
                ChangeCurrentRow(current);

                unsigned int lineFrom = oldCurrentRow,
                       lineTo = current;

                if ( lineTo < lineFrom )
                {
                    lineTo = lineFrom;
                    lineFrom = m_currentRow;
                }

                SelectRows(lineFrom, lineTo, true);
            }
            else // !ctrl, !shift
            {
                // test in the enclosing if should make it impossible
                wxFAIL_MSG( _T("how did we get here?") );
            }
        }

        if (m_currentRow != oldCurrentRow)
            RefreshRow( oldCurrentRow );

        wxDataViewColumn *oldCurrentCol = m_currentCol;

        // Update selection here...
        m_currentCol = col;

        m_lastOnSame = !forceClick && ((col == oldCurrentCol) &&
                        (current == oldCurrentRow)) && oldWasSelected;
    }
}

void wxDataViewMainWindow::OnSetFocus( wxFocusEvent &event )
{
    m_hasFocus = true;

    if (HasCurrentRow())
        Refresh();

    event.Skip();
}

void wxDataViewMainWindow::OnKillFocus( wxFocusEvent &event )
{
    m_hasFocus = false;

    if (HasCurrentRow())
        Refresh();

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
    if (!wxControl::Create( parent, id, pos, size,
                            style | wxScrolledWindowStyle|wxSUNKEN_BORDER, validator))
        return false;

    Init();

#ifdef __WXMAC__
    MacSetClipChildren( true ) ;
#endif

    m_clientArea = new wxDataViewMainWindow( this, wxID_ANY );

    if (HasFlag(wxDV_NO_HEADER))
        m_headerArea = NULL;
    else
        m_headerArea = new wxDataViewHeaderWindow( this, wxID_ANY );

    SetTargetWindow( m_clientArea );

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    if (m_headerArea)
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
    WXLRESULT rc = wxDataViewCtrlBase::MSWWindowProc(nMsg, wParam, lParam);

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

void wxDataViewCtrl::OnSize( wxSizeEvent &WXUNUSED(event) )
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

    OnColumnChange();
    return true;
}

void wxDataViewCtrl::OnColumnChange()
{
    if (m_headerArea)
        m_headerArea->UpdateDisplay();

    m_clientArea->UpdateDisplay();
}

void wxDataViewCtrl::SetSelection( int row )
{
    m_clientArea->SelectRow(row, true);
}

void wxDataViewCtrl::SetSelectionRange( unsigned int from, unsigned int to )
{
    m_clientArea->SelectRows(from, to, true);
}

void wxDataViewCtrl::SetSelections( const wxArrayInt& aSelections )
{
    m_clientArea->Select(aSelections);
}

void wxDataViewCtrl::Unselect( unsigned int WXUNUSED(row) )
{
    // FIXME - TODO
}

bool wxDataViewCtrl::IsSelected( unsigned int WXUNUSED(row) ) const
{
    // FIXME - TODO

    return false;
}

int wxDataViewCtrl::GetSelection() const
{
    // FIXME - TODO

    return -1;
}

int wxDataViewCtrl::GetSelections(wxArrayInt& WXUNUSED(aSelections) ) const
{
    // FIXME - TODO

    return 0;
}

#endif
    // !wxUSE_GENERICDATAVIEWCTRL

#endif
    // wxUSE_DATAVIEWCTRL
