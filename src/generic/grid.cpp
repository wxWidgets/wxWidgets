/////////////////////////////////////////////////////////////////////////////
// Name:        grid.cpp
// Purpose:     wxGrid and related classes
// Author:      Michael Bedward (based on code by Julian Smart, Robin Dunn)
// Modified by:
// Created:     1/08/1999
// RCS-ID:      $Id$
// Copyright:   (c) Michael Bedward (mbedward@ozemail.com.au)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "grid.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/defs.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if !defined(wxUSE_NEW_GRID) || !(wxUSE_NEW_GRID)
#include "gridg.cpp"
#else

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/log.h"
    #include "wx/sizer.h"
#endif

#include "wx/generic/grid.h"

//////////////////////////////////////////////////////////////////////

wxGridCellCoords wxGridNoCellCoords( -1, -1 );
wxRect           wxGridNoCellRect( -1, -1, -1, -1 );

// this is a magic incantation which must be done!
#include "wx/arrimpl.cpp"

WX_DEFINE_OBJARRAY(wxGridCellCoordsArray)

// scroll line size
// TODO: fixed so far - make configurable later (and also different for x/y)
static const size_t GRID_SCROLL_LINE = 10;

//////////////////////////////////////////////////////////////////////
//
// Abstract base class for grid data (the model)
//
IMPLEMENT_ABSTRACT_CLASS( wxGridTableBase, wxObject )


wxGridTableBase::wxGridTableBase()
        : wxObject()
{
    m_view = (wxGrid *) NULL;
}

wxGridTableBase::~wxGridTableBase()
{
}


bool wxGridTableBase::InsertRows( size_t pos, size_t numRows )
{
    wxLogWarning( wxT("Called grid table class function InsertRows(pos=%d, N=%d)\n"
                  "but your derived table class does not override this function"),
                  pos, numRows );

    return FALSE;
}

bool wxGridTableBase::AppendRows( size_t numRows )
{
    wxLogWarning( wxT("Called grid table class function AppendRows(N=%d)\n"
                  "but your derived table class does not override this function"),
                  numRows );

    return FALSE;
}

bool wxGridTableBase::DeleteRows( size_t pos, size_t numRows )
{
    wxLogWarning( wxT("Called grid table class function DeleteRows(pos=%d, N=%d)\n"
                  "but your derived table class does not override this function"),
                  pos, numRows );

    return FALSE;
}

bool wxGridTableBase::InsertCols( size_t pos, size_t numCols )
{
    wxLogWarning( wxT("Called grid table class function InsertCols(pos=%d, N=%d)\n"
                  "but your derived table class does not override this function"),
                  pos, numCols );

    return FALSE;
}

bool wxGridTableBase::AppendCols( size_t numCols )
{
    wxLogWarning( wxT("Called grid table class function AppendCols(N=%d)\n"
                  "but your derived table class does not override this function"),
                  numCols );

    return FALSE;
}

bool wxGridTableBase::DeleteCols( size_t pos, size_t numCols )
{
    wxLogWarning( wxT("Called grid table class function DeleteCols(pos=%d, N=%d)\n"
                  "but your derived table class does not override this function"),
                  pos, numCols );

    return FALSE;
}


wxString wxGridTableBase::GetRowLabelValue( int row )
{
    wxString s;
    s << row;
    return s;
}

wxString wxGridTableBase::GetColLabelValue( int col )
{
    // default col labels are:
    //   cols 0 to 25   : A-Z
    //   cols 26 to 675 : AA-ZZ
    //   etc.

    wxString s;
    unsigned int i, n;
    for ( n = 1; ; n++ )
    {
        s += (_T('A') + (wxChar)( col%26 ));
        col = col/26 - 1;
        if ( col < 0 ) break;
    }

    // reverse the string...
    wxString s2;
    for ( i = 0;  i < n;  i++ )
    {
        s2 += s[n-i-1];
    }

    return s2;
}



//////////////////////////////////////////////////////////////////////
//
// Message class for the grid table to send requests and notifications
// to the grid view
//

wxGridTableMessage::wxGridTableMessage()
{
    m_table = (wxGridTableBase *) NULL;
    m_id = -1;
    m_comInt1 = -1;
    m_comInt2 = -1;
}

wxGridTableMessage::wxGridTableMessage( wxGridTableBase *table, int id,
                                        int commandInt1, int commandInt2 )
{
    m_table = table;
    m_id = id;
    m_comInt1 = commandInt1;
    m_comInt2 = commandInt2;
}



//////////////////////////////////////////////////////////////////////
//
// A basic grid table for string data. An object of this class will
// created by wxGrid if you don't specify an alternative table class.
//

WX_DEFINE_OBJARRAY(wxGridStringArray)

IMPLEMENT_DYNAMIC_CLASS( wxGridStringTable, wxGridTableBase )

wxGridStringTable::wxGridStringTable()
        : wxGridTableBase()
{
}

wxGridStringTable::wxGridStringTable( int numRows, int numCols )
        : wxGridTableBase()
{
    int row, col;

    m_data.Alloc( numRows );

    wxArrayString sa;
    sa.Alloc( numCols );
    for ( col = 0;  col < numCols;  col++ )
    {
        sa.Add( wxEmptyString );
    }

    for ( row = 0;  row < numRows;  row++ )
    {
        m_data.Add( sa );
    }
}

wxGridStringTable::~wxGridStringTable()
{
}

long wxGridStringTable::GetNumberRows()
{
    return m_data.GetCount();
}

long wxGridStringTable::GetNumberCols()
{
    if ( m_data.GetCount() > 0 )
        return m_data[0].GetCount();
    else
        return 0;
}

wxString wxGridStringTable::GetValue( int row, int col )
{
    // TODO: bounds checking
    //
    return m_data[row][col];
}

void wxGridStringTable::SetValue( int row, int col, const wxString& s )
{
    // TODO: bounds checking
    //
    m_data[row][col] = s;
}

bool wxGridStringTable::IsEmptyCell( int row, int col )
{
    // TODO: bounds checking
    //
    return (m_data[row][col] == wxEmptyString);
}


void wxGridStringTable::Clear()
{
    int row, col;
    int numRows, numCols;

    numRows = m_data.GetCount();
    if ( numRows > 0 )
    {
        numCols = m_data[0].GetCount();

        for ( row = 0;  row < numRows;  row++ )
        {
            for ( col = 0;  col < numCols;  col++ )
            {
                m_data[row][col] = wxEmptyString;
            }
        }
    }
}


bool wxGridStringTable::InsertRows( size_t pos, size_t numRows )
{
    size_t row, col;

    size_t curNumRows = m_data.GetCount();
    size_t curNumCols = ( curNumRows > 0 ? m_data[0].GetCount() : 0 );

    if ( pos >= curNumRows )
    {
        return AppendRows( numRows );
    }

    wxArrayString sa;
    sa.Alloc( curNumCols );
    for ( col = 0;  col < curNumCols;  col++ )
    {
        sa.Add( wxEmptyString );
    }

    for ( row = pos;  row < pos + numRows;  row++ )
    {
        m_data.Insert( sa, row );
    }

    if ( GetView() )
    {
        wxGridTableMessage msg( this,
                                wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
                                pos,
                                numRows );

        GetView()->ProcessTableMessage( msg );
    }

    return TRUE;
}

bool wxGridStringTable::AppendRows( size_t numRows )
{
    size_t row, col;

    size_t curNumRows = m_data.GetCount();
    size_t curNumCols = ( curNumRows > 0 ? m_data[0].GetCount() : 0 );

    wxArrayString sa;
    if ( curNumCols > 0 )
    {
        sa.Alloc( curNumCols );
        for ( col = 0;  col < curNumCols;  col++ )
        {
            sa.Add( wxEmptyString );
        }
    }

    for ( row = 0;  row < numRows;  row++ )
    {
        m_data.Add( sa );
    }

    if ( GetView() )
    {
        wxGridTableMessage msg( this,
                                wxGRIDTABLE_NOTIFY_ROWS_APPENDED,
                                numRows );

        GetView()->ProcessTableMessage( msg );
    }

    return TRUE;
}

bool wxGridStringTable::DeleteRows( size_t pos, size_t numRows )
{
    size_t n;

    size_t curNumRows = m_data.GetCount();

    if ( pos >= curNumRows )
    {
        wxLogError( wxT("Called wxGridStringTable::DeleteRows(pos=%d, N=%d)...\n"
                    "Pos value is invalid for present table with %d rows"),
                    pos, numRows, curNumRows );
        return FALSE;
    }

    if ( numRows > curNumRows - pos )
    {
        numRows = curNumRows - pos;
    }

    if ( numRows >= curNumRows )
    {
        m_data.Empty();  // don't release memory just yet
    }
    else
    {
        for ( n = 0;  n < numRows;  n++ )
        {
            m_data.Remove( pos );
        }
    }

    if ( GetView() )
    {
        wxGridTableMessage msg( this,
                                wxGRIDTABLE_NOTIFY_ROWS_DELETED,
                                pos,
                                numRows );

        GetView()->ProcessTableMessage( msg );
    }

    return TRUE;
}

bool wxGridStringTable::InsertCols( size_t pos, size_t numCols )
{
    size_t row, col;

    size_t curNumRows = m_data.GetCount();
    size_t curNumCols = ( curNumRows > 0 ? m_data[0].GetCount() : 0 );

    if ( pos >= curNumCols )
    {
        return AppendCols( numCols );
    }

    for ( row = 0;  row < curNumRows;  row++ )
    {
        for ( col = pos;  col < pos + numCols;  col++ )
        {
            m_data[row].Insert( wxEmptyString, col );
        }
    }

    if ( GetView() )
    {
        wxGridTableMessage msg( this,
                                wxGRIDTABLE_NOTIFY_COLS_INSERTED,
                                pos,
                                numCols );

        GetView()->ProcessTableMessage( msg );
    }

    return TRUE;
}

bool wxGridStringTable::AppendCols( size_t numCols )
{
    size_t row, n;

    size_t curNumRows = m_data.GetCount();
    if ( !curNumRows )
    {
        // TODO: something better than this ?
        //
        wxLogError( wxT("Unable to append cols to a grid table with no rows.\n"
                    "Call AppendRows() first") );
        return FALSE;
    }

    for ( row = 0;  row < curNumRows;  row++ )
    {
        for ( n = 0;  n < numCols;  n++ )
        {
            m_data[row].Add( wxEmptyString );
        }
    }

    if ( GetView() )
    {
        wxGridTableMessage msg( this,
                                wxGRIDTABLE_NOTIFY_COLS_APPENDED,
                                numCols );

        GetView()->ProcessTableMessage( msg );
    }

    return TRUE;
}

bool wxGridStringTable::DeleteCols( size_t pos, size_t numCols )
{
    size_t row, n;

    size_t curNumRows = m_data.GetCount();
    size_t curNumCols = ( curNumRows > 0 ? m_data[0].GetCount() : 0 );

    if ( pos >= curNumCols )
    {
        wxLogError( wxT("Called wxGridStringTable::DeleteCols(pos=%d, N=%d)...\n"
                    "Pos value is invalid for present table with %d cols"),
                    pos, numCols, curNumCols );
        return FALSE;
    }

    if ( numCols > curNumCols - pos )
    {
        numCols = curNumCols - pos;
    }

    for ( row = 0;  row < curNumRows;  row++ )
    {
        if ( numCols >= curNumCols )
        {
            m_data[row].Clear();
        }
        else
        {
            for ( n = 0;  n < numCols;  n++ )
            {
                m_data[row].Remove( pos );
            }
        }
    }

    if ( GetView() )
    {
        wxGridTableMessage msg( this,
                                wxGRIDTABLE_NOTIFY_COLS_DELETED,
                                pos,
                                numCols );

        GetView()->ProcessTableMessage( msg );
    }

    return TRUE;
}

wxString wxGridStringTable::GetRowLabelValue( int row )
{
    if ( row > (int)(m_rowLabels.GetCount()) - 1 )
    {
        // using default label
        //
        return wxGridTableBase::GetRowLabelValue( row );
    }
    else
    {
        return m_rowLabels[ row ];
    }
}

wxString wxGridStringTable::GetColLabelValue( int col )
{
    if ( col > (int)(m_colLabels.GetCount()) - 1 )
    {
        // using default label
        //
        return wxGridTableBase::GetColLabelValue( col );
    }
    else
    {
        return m_colLabels[ col ];
    }
}

void wxGridStringTable::SetRowLabelValue( int row, const wxString& value )
{
    if ( row > (int)(m_rowLabels.GetCount()) - 1 )
    {
        int n = m_rowLabels.GetCount();
        int i;
        for ( i = n;  i <= row;  i++ )
        {
            m_rowLabels.Add( wxGridTableBase::GetRowLabelValue(i) );
        }
    }

    m_rowLabels[row] = value;
}

void wxGridStringTable::SetColLabelValue( int col, const wxString& value )
{
    if ( col > (int)(m_colLabels.GetCount()) - 1 )
    {
        int n = m_colLabels.GetCount();
        int i;
        for ( i = n;  i <= col;  i++ )
        {
            m_colLabels.Add( wxGridTableBase::GetColLabelValue(i) );
        }
    }

    m_colLabels[col] = value;
}




//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS( wxGridTextCtrl, wxTextCtrl )

BEGIN_EVENT_TABLE( wxGridTextCtrl, wxTextCtrl )
    EVT_KEY_DOWN( wxGridTextCtrl::OnKeyDown )
END_EVENT_TABLE()


wxGridTextCtrl::wxGridTextCtrl( wxWindow *par,
                                wxGrid *grid,
                                bool isCellControl,
                                wxWindowID id,
                                const wxString& value,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style )
        : wxTextCtrl( par, id, value, pos, size, style )
{
    m_grid = grid;
    m_isCellControl = isCellControl;
}


void wxGridTextCtrl::OnKeyDown( wxKeyEvent& event )
{
    switch ( event.KeyCode() )
    {
        case WXK_ESCAPE:
            m_grid->SetEditControlValue( startValue );
            SetInsertionPointEnd();
            break;

        case WXK_UP:
        case WXK_DOWN:
        case WXK_LEFT:
        case WXK_RIGHT:
        case WXK_PRIOR:
        case WXK_NEXT:
            if ( m_isCellControl )
            {
                // send the event to the parent grid, skipping the
                // event if nothing happens
                //
                event.Skip( m_grid->ProcessEvent( event ) );
            }
            else
            {
                // default text control response within the top edit
                // control
                //
                event.Skip();
            }
            break;

        case WXK_RETURN:
            if ( m_isCellControl )
            {
                if ( !m_grid->ProcessEvent( event ) )
                {
#ifdef __WXMOTIF__
                    // wxMotif needs a little extra help...
                    //
                    int pos = GetInsertionPoint();
                    wxString s( GetValue() );
                    s = s.Left(pos) + "\n" + s.Mid(pos);
                    SetValue(s);
                    SetInsertionPoint( pos );
#else
                    // the other ports can handle a Return key press
                    //
                    event.Skip();
#endif
                }
            }
            break;

        case WXK_HOME:
        case WXK_END:
            if ( m_isCellControl )
            {
                // send the event to the parent grid, skipping the
                // event if nothing happens
                //
                event.Skip( m_grid->ProcessEvent( event ) );
            }
            else
            {
                // default text control response within the top edit
                // control
                //
                event.Skip();
            }
            break;

        default:
            event.Skip();
    }
}

void wxGridTextCtrl::SetStartValue( const wxString& s )
{
    startValue = s;
    wxTextCtrl::SetValue(s);
}



//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS( wxGridRowLabelWindow, wxWindow )

BEGIN_EVENT_TABLE( wxGridRowLabelWindow, wxWindow )
    EVT_PAINT( wxGridRowLabelWindow::OnPaint )
    EVT_MOUSE_EVENTS( wxGridRowLabelWindow::OnMouseEvent )
    EVT_KEY_DOWN( wxGridRowLabelWindow::OnKeyDown )
END_EVENT_TABLE()

wxGridRowLabelWindow::wxGridRowLabelWindow( wxGrid *parent,
                                            wxWindowID id,
                                            const wxPoint &pos, const wxSize &size )
  : wxWindow( parent, id, pos, size )
{
    m_owner = parent;
}

void wxGridRowLabelWindow::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc(this);

    // NO - don't do this because it will set both the x and y origin
    // coords to match the parent scrolled window and we just want to
    // set the y coord  - MB
    //
    // m_owner->PrepareDC( dc );

    wxCoord x, y;
    m_owner->CalcUnscrolledPosition( 0, 0, &x, &y );
    dc.SetDeviceOrigin( 0, -y );

    m_owner->CalcRowLabelsExposed( GetUpdateRegion() );
    m_owner->DrawRowLabels( dc );
}


void wxGridRowLabelWindow::OnMouseEvent( wxMouseEvent& event )
{
    m_owner->ProcessRowLabelMouseEvent( event );
}


// This seems to be required for wxMotif otherwise the mouse
// cursor must be in the cell edit control to get key events
//
void wxGridRowLabelWindow::OnKeyDown( wxKeyEvent& event )
{
    if ( !m_owner->ProcessEvent( event ) ) event.Skip();
}



//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS( wxGridColLabelWindow, wxWindow )

BEGIN_EVENT_TABLE( wxGridColLabelWindow, wxWindow )
    EVT_PAINT( wxGridColLabelWindow::OnPaint )
    EVT_MOUSE_EVENTS( wxGridColLabelWindow::OnMouseEvent )
    EVT_KEY_DOWN( wxGridColLabelWindow::OnKeyDown )
END_EVENT_TABLE()

wxGridColLabelWindow::wxGridColLabelWindow( wxGrid *parent,
                                            wxWindowID id,
                                            const wxPoint &pos, const wxSize &size )
  : wxWindow( parent, id, pos, size )
{
    m_owner = parent;
}

void wxGridColLabelWindow::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc(this);

    // NO - don't do this because it will set both the x and y origin
    // coords to match the parent scrolled window and we just want to
    // set the x coord  - MB
    //
    // m_owner->PrepareDC( dc );

    wxCoord x, y;
    m_owner->CalcUnscrolledPosition( 0, 0, &x, &y );
    dc.SetDeviceOrigin( -x, 0 );

    m_owner->CalcColLabelsExposed( GetUpdateRegion() );
    m_owner->DrawColLabels( dc );
}


void wxGridColLabelWindow::OnMouseEvent( wxMouseEvent& event )
{
    m_owner->ProcessColLabelMouseEvent( event );
}


// This seems to be required for wxMotif otherwise the mouse
// cursor must be in the cell edit control to get key events
//
void wxGridColLabelWindow::OnKeyDown( wxKeyEvent& event )
{
    if ( !m_owner->ProcessEvent( event ) ) event.Skip();
}



//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS( wxGridCornerLabelWindow, wxWindow )

BEGIN_EVENT_TABLE( wxGridCornerLabelWindow, wxWindow )
    EVT_MOUSE_EVENTS( wxGridCornerLabelWindow::OnMouseEvent )
    EVT_PAINT( wxGridCornerLabelWindow::OnPaint)
    EVT_KEY_DOWN( wxGridCornerLabelWindow::OnKeyDown )
END_EVENT_TABLE()

wxGridCornerLabelWindow::wxGridCornerLabelWindow( wxGrid *parent,
                                                  wxWindowID id,
                                                  const wxPoint &pos, const wxSize &size )
  : wxWindow( parent, id, pos, size )
{
    m_owner = parent;
}

void wxGridCornerLabelWindow::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    
    int client_height = 0;
    int client_width = 0;
    GetClientSize( &client_width, &client_height );
    
    dc.SetPen( *wxBLACK_PEN );
    dc.DrawLine( client_width-1, client_height-1, client_width-1, 0 );
    dc.DrawLine( client_width-1, client_height-1, 0, client_height-1 );
    
    dc.SetPen( *wxWHITE_PEN );
    dc.DrawLine( 0, 0, client_width, 0 );
    dc.DrawLine( 0, 0, 0, client_height );
}


void wxGridCornerLabelWindow::OnMouseEvent( wxMouseEvent& event )
{
    m_owner->ProcessCornerLabelMouseEvent( event );
}


// This seems to be required for wxMotif otherwise the mouse
// cursor must be in the cell edit control to get key events
//
void wxGridCornerLabelWindow::OnKeyDown( wxKeyEvent& event )
{
    if ( !m_owner->ProcessEvent( event ) ) event.Skip();
}



//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS( wxGridWindow, wxPanel )

BEGIN_EVENT_TABLE( wxGridWindow, wxPanel )
    EVT_PAINT( wxGridWindow::OnPaint )
    EVT_SCROLLWIN( wxGridWindow::ScrollWindow )
    EVT_MOUSE_EVENTS( wxGridWindow::OnMouseEvent )
    EVT_KEY_DOWN( wxGridWindow::OnKeyDown )
END_EVENT_TABLE()

wxGridWindow::wxGridWindow( wxGrid *parent,
                            wxGridRowLabelWindow *rowLblWin,
                            wxGridColLabelWindow *colLblWin,
                            wxWindowID id, const wxPoint &pos, const wxSize &size )
        : wxPanel( parent, id, pos, size, wxSUNKEN_BORDER, "grid window" )
{
    m_owner = parent;
    m_rowLabelWin = rowLblWin;
    m_colLabelWin = colLblWin;

    SetBackgroundColour( "WHITE" );
}


wxGridWindow::~wxGridWindow()
{
}


void wxGridWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    m_owner->PrepareDC( dc );

    m_owner->CalcCellsExposed( GetUpdateRegion() );
    m_owner->DrawGridCellArea( dc );
}


void wxGridWindow::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    wxPanel::ScrollWindow( dx, dy, rect );
    m_rowLabelWin->ScrollWindow( 0, dy, rect );
    m_colLabelWin->ScrollWindow( dx, 0, rect );
}


void wxGridWindow::OnMouseEvent( wxMouseEvent& event )
{
    m_owner->ProcessGridCellMouseEvent( event );
}


// This seems to be required for wxMotif otherwise the mouse
// cursor must be in the cell edit control to get key events
//
void wxGridWindow::OnKeyDown( wxKeyEvent& event )
{
    if ( !m_owner->ProcessEvent( event ) ) event.Skip();
}



//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS( wxGrid, wxScrolledWindow )

BEGIN_EVENT_TABLE( wxGrid, wxScrolledWindow )
    EVT_PAINT( wxGrid::OnPaint )
    EVT_SIZE( wxGrid::OnSize )
    EVT_KEY_DOWN( wxGrid::OnKeyDown )
END_EVENT_TABLE()

wxGrid::wxGrid( wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxString& name )
  : wxScrolledWindow( parent, id, pos, size, style, name )
{
    Create();
}


wxGrid::~wxGrid()
{
    delete m_table;
}


//
// ----- internal init and update functions
//

void wxGrid::Create()
{
    int colLblH = WXGRID_DEFAULT_COL_LABEL_HEIGHT;
    int rowLblW = WXGRID_DEFAULT_ROW_LABEL_WIDTH;

    m_rowLabelWin = new wxGridRowLabelWindow( this,
                                              -1,
                                              wxDefaultPosition,
                                              wxSize(rowLblW,-1) );

    m_colLabelWin = new wxGridColLabelWindow( this,
                                              -1,
                                              wxDefaultPosition,
                                              wxSize(-1, colLblH ) );

    m_cornerLabelWin = new wxGridCornerLabelWindow( this,
                                                    -1,
                                                    wxDefaultPosition,
                                                    wxSize(rowLblW, colLblH ) );

    m_gridWin = new wxGridWindow( this,
                                  m_rowLabelWin,
                                  m_colLabelWin,
                                  -1,
                                  wxDefaultPosition,
                                  wxDefaultSize );

    SetTargetWindow( m_gridWin );

    m_mainSizer = new wxBoxSizer( wxVERTICAL );

    m_topSizer = new wxBoxSizer( wxHORIZONTAL );
    m_topSizer->Add( m_cornerLabelWin, 0 );
    m_topSizer->Add( m_colLabelWin, 1 );

    m_mainSizer->Add( m_topSizer, 0, wxEXPAND );

    m_middleSizer = new wxBoxSizer( wxHORIZONTAL );
    m_middleSizer->Add( m_rowLabelWin, 0, wxEXPAND );
    m_middleSizer->Add( m_gridWin, 1, wxEXPAND );

    m_mainSizer->Add( m_middleSizer, 1, wxEXPAND );

    SetAutoLayout( TRUE );
    SetSizer( m_mainSizer );
}


bool wxGrid::CreateGrid( int numRows, int numCols )
{
    if ( m_created )
    {
        wxLogError( wxT("wxGrid::CreateGrid(numRows, numCols) called more than once") );
        return FALSE;
    }
    else
    {
        m_numRows = numRows;
        m_numCols = numCols;

        m_table = new wxGridStringTable( m_numRows, m_numCols );
        m_table->SetView( this );
        Init();
        m_created = TRUE;
    }

    return m_created;
}


void wxGrid::Init()
{
    int i;

    if ( m_numRows <= 0 )
        m_numRows = WXGRID_DEFAULT_NUMBER_ROWS;

    if ( m_numCols <= 0 )
        m_numCols = WXGRID_DEFAULT_NUMBER_COLS;

    m_rowLabelWidth  = WXGRID_DEFAULT_ROW_LABEL_WIDTH;
    m_colLabelHeight = WXGRID_DEFAULT_COL_LABEL_HEIGHT;

    if ( m_rowLabelWin )
    {
        m_labelBackgroundColour = m_rowLabelWin->GetBackgroundColour();
    }
    else
    {
        m_labelBackgroundColour = wxColour( _T("WHITE") );
    }

    m_labelTextColour = wxColour( _T("BLACK") );

    // TODO: something better than this ?
    //
    m_labelFont = this->GetFont();
    m_labelFont.SetWeight( m_labelFont.GetWeight() + 2 );

    m_rowLabelHorizAlign = wxLEFT;
    m_rowLabelVertAlign  = wxCENTRE;

    m_colLabelHorizAlign = wxCENTRE;
    m_colLabelVertAlign  = wxTOP;

    m_defaultColWidth  = WXGRID_DEFAULT_COL_WIDTH;
    m_defaultRowHeight = m_gridWin->GetCharHeight();

#if defined(__WXMOTIF__) || defined(__WXGTK__)  // see also text ctrl sizing in ShowCellEditControl()
    m_defaultRowHeight += 8;
#else
    m_defaultRowHeight += 4;
#endif

    m_rowHeights.Alloc( m_numRows );
    m_rowBottoms.Alloc( m_numRows );
    int rowBottom = 0;
    for ( i = 0;  i < m_numRows;  i++ )
    {
        m_rowHeights.Add( m_defaultRowHeight );
        rowBottom += m_defaultRowHeight;
        m_rowBottoms.Add( rowBottom );
    }

    m_colWidths.Alloc( m_numCols );
    m_colRights.Alloc( m_numCols );
    int colRight = 0;
    for ( i = 0;  i < m_numCols;  i++ )
    {
        m_colWidths.Add( m_defaultColWidth );
        colRight += m_defaultColWidth;
        m_colRights.Add( colRight );
    }

    // TODO: improve this ?
    //
    m_defaultCellFont = this->GetFont();

    m_gridLineColour = wxColour( 128, 128, 255 );
    m_gridLinesEnabled = TRUE;

    m_cursorMode  = WXGRID_CURSOR_SELECT_CELL;
    m_dragLastPos  = -1;
    m_dragRowOrCol = -1;
    m_isDragging = FALSE;

    m_rowResizeCursor = wxCursor( wxCURSOR_SIZENS );
    m_colResizeCursor = wxCursor( wxCURSOR_SIZEWE );

    m_currentCellCoords = wxGridNoCellCoords;

    m_selectedTopLeft = wxGridNoCellCoords;
    m_selectedBottomRight = wxGridNoCellCoords;

    m_editable = TRUE;  // default for whole grid

    m_inOnKeyDown = FALSE;
    m_batchCount = 0;

    // TODO: extend this to other types of controls
    //
    m_cellEditCtrl = new wxGridTextCtrl( m_gridWin,
                                         this,
                                         TRUE,
                                         wxGRID_CELLCTRL,
                                         "",
                                         wxPoint(1,1),
                                         wxSize(1,1)
#if defined(__WXMSW__)
                                         , wxTE_MULTILINE | wxTE_NO_VSCROLL
#endif
                                         );

    m_cellEditCtrl->Show( FALSE );
    m_cellEditCtrlEnabled = TRUE;
    m_editCtrlType = wxGRID_TEXTCTRL;
}


void wxGrid::CalcDimensions()
{
    int cw, ch;
    GetClientSize( &cw, &ch );

    if ( m_numRows > 0  &&  m_numCols > 0 )
    {
        int right = m_colRights[ m_numCols-1 ] + 20;
        int bottom = m_rowBottoms[ m_numRows-1 ] + 20;

        // TODO: restore the scroll position that we had before sizing
        //
        int x, y;
        GetViewStart( &x, &y );
        SetScrollbars( GRID_SCROLL_LINE, GRID_SCROLL_LINE,
                       right/GRID_SCROLL_LINE, bottom/GRID_SCROLL_LINE,
                       x, y );
    }
}


// this is called when the grid table sends a message to say that it
// has been redimensioned
//
bool wxGrid::Redimension( wxGridTableMessage& msg )
{
    int i;

    switch ( msg.GetId() )
    {
        case wxGRIDTABLE_NOTIFY_ROWS_INSERTED:
        {
            size_t pos = msg.GetCommandInt();
            int numRows = msg.GetCommandInt2();
            for ( i = 0;  i < numRows;  i++ )
            {
                m_rowHeights.Insert( m_defaultRowHeight, pos );
                m_rowBottoms.Insert( 0, pos );
            }
            m_numRows += numRows;

            int bottom = 0;
            if ( pos > 0 ) bottom = m_rowBottoms[pos-1];

            for ( i = pos;  i < m_numRows;  i++ )
            {
                bottom += m_rowHeights[i];
                m_rowBottoms[i] = bottom;
            }
            CalcDimensions();
        }
        return TRUE;

        case wxGRIDTABLE_NOTIFY_ROWS_APPENDED:
        {
            int numRows = msg.GetCommandInt();
            for ( i = 0;  i < numRows;  i++ )
            {
                m_rowHeights.Add( m_defaultRowHeight );
                m_rowBottoms.Add( 0 );
            }

            int oldNumRows = m_numRows;
            m_numRows += numRows;

            int bottom = 0;
            if ( oldNumRows > 0 ) bottom = m_rowBottoms[oldNumRows-1];

            for ( i = oldNumRows;  i < m_numRows;  i++ )
            {
                bottom += m_rowHeights[i];
                m_rowBottoms[i] = bottom;
            }
            CalcDimensions();
        }
        return TRUE;

        case wxGRIDTABLE_NOTIFY_ROWS_DELETED:
        {
            size_t pos = msg.GetCommandInt();
            int numRows = msg.GetCommandInt2();
            for ( i = 0;  i < numRows;  i++ )
            {
                m_rowHeights.Remove( pos );
                m_rowBottoms.Remove( pos );
            }
            m_numRows -= numRows;

            if ( !m_numRows )
            {
                m_numCols = 0;
                m_colWidths.Clear();
                m_colRights.Clear();
                m_currentCellCoords = wxGridNoCellCoords;
            }
            else
            {
                if ( m_currentCellCoords.GetRow() >= m_numRows )
                    m_currentCellCoords.Set( 0, 0 );

                int h = 0;
                for ( i = 0;  i < m_numRows;  i++ )
                {
                    h += m_rowHeights[i];
                    m_rowBottoms[i] = h;
                }
            }

            CalcDimensions();
        }
        return TRUE;

        case wxGRIDTABLE_NOTIFY_COLS_INSERTED:
        {
            size_t pos = msg.GetCommandInt();
            int numCols = msg.GetCommandInt2();
            for ( i = 0;  i < numCols;  i++ )
            {
                m_colWidths.Insert( m_defaultColWidth, pos );
                m_colRights.Insert( 0, pos );
            }
            m_numCols += numCols;

            int right = 0;
            if ( pos > 0 ) right = m_colRights[pos-1];

            for ( i = pos;  i < m_numCols;  i++ )
            {
                right += m_colWidths[i];
                m_colRights[i] = right;
            }
            CalcDimensions();
        }
        return TRUE;

        case wxGRIDTABLE_NOTIFY_COLS_APPENDED:
        {
            int numCols = msg.GetCommandInt();
            for ( i = 0;  i < numCols;  i++ )
            {
                m_colWidths.Add( m_defaultColWidth );
                m_colRights.Add( 0 );
            }

            int oldNumCols = m_numCols;
            m_numCols += numCols;

            int right = 0;
            if ( oldNumCols > 0 ) right = m_colRights[oldNumCols-1];

            for ( i = oldNumCols;  i < m_numCols;  i++ )
            {
                right += m_colWidths[i];
                m_colRights[i] = right;
            }
            CalcDimensions();
        }
        return TRUE;

        case wxGRIDTABLE_NOTIFY_COLS_DELETED:
        {
            size_t pos = msg.GetCommandInt();
            int numCols = msg.GetCommandInt2();
            for ( i = 0;  i < numCols;  i++ )
            {
                m_colWidths.Remove( pos );
                m_colRights.Remove( pos );
            }
            m_numCols -= numCols;

            if ( !m_numCols )
            {
#if 0  // leave the row alone here so that AppendCols will work subsequently
                m_numRows = 0;
                m_rowHeights.Clear();
                m_rowBottoms.Clear();
#endif
                m_currentCellCoords = wxGridNoCellCoords;
            }
            else
            {
                if ( m_currentCellCoords.GetCol() >= m_numCols )
                    m_currentCellCoords.Set( 0, 0 );

                int w = 0;
                for ( i = 0;  i < m_numCols;  i++ )
                {
                    w += m_colWidths[i];
                    m_colRights[i] = w;
                }
            }
            CalcDimensions();
        }
        return TRUE;
    }

    return FALSE;
}


void wxGrid::CalcRowLabelsExposed( wxRegion& reg )
{
    wxRegionIterator iter( reg );
    wxRect r;

    m_rowLabelsExposed.Empty();

    int top, bottom;
    while ( iter )
    {
        r = iter.GetRect();

        // TODO: remove this when we can...
        // There is a bug in wxMotif that gives garbage update
        // rectangles if you jump-scroll a long way by clicking the
        // scrollbar with middle button.  This is a work-around
        //
#if defined(__WXMOTIF__)
        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );
        if ( r.GetTop() > ch ) r.SetTop( 0 );
        r.SetBottom( wxMin( r.GetBottom(), ch ) );
#endif

        // logical bounds of update region
        //
        int dummy;
        CalcUnscrolledPosition( 0, r.GetTop(), &dummy, &top );
        CalcUnscrolledPosition( 0, r.GetBottom(), &dummy, &bottom );

        // find the row labels within these bounds
        //
        int row;
        int rowTop;
        for ( row = 0;  row < m_numRows;  row++ )
        {
            if ( m_rowBottoms[row] < top ) continue;

            rowTop = m_rowBottoms[row] - m_rowHeights[row];
            if ( rowTop > bottom ) break;

            m_rowLabelsExposed.Add( row );
        }

        iter++ ;
    }
}


void wxGrid::CalcColLabelsExposed( wxRegion& reg )
{
    wxRegionIterator iter( reg );
    wxRect r;

    m_colLabelsExposed.Empty();

    int left, right;
    while ( iter )
    {
        r = iter.GetRect();

        // TODO: remove this when we can...
        // There is a bug in wxMotif that gives garbage update
        // rectangles if you jump-scroll a long way by clicking the
        // scrollbar with middle button.  This is a work-around
        //
#if defined(__WXMOTIF__)
        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );
        if ( r.GetLeft() > cw ) r.SetLeft( 0 );
        r.SetRight( wxMin( r.GetRight(), cw ) );
#endif

        // logical bounds of update region
        //
        int dummy;
        CalcUnscrolledPosition( r.GetLeft(), 0, &left, &dummy );
        CalcUnscrolledPosition( r.GetRight(), 0, &right, &dummy );

        // find the cells within these bounds
        //
        int col;
        int colLeft;
        for ( col = 0;  col < m_numCols;  col++ )
        {
            if ( m_colRights[col] < left ) continue;

            colLeft = m_colRights[col] - m_colWidths[col];
            if ( colLeft > right ) break;

            m_colLabelsExposed.Add( col );
        }

        iter++ ;
    }
}


void wxGrid::CalcCellsExposed( wxRegion& reg )
{
    wxRegionIterator iter( reg );
    wxRect r;

    m_cellsExposed.Empty();
    m_rowsExposed.Empty();
    m_colsExposed.Empty();

    int left, top, right, bottom;
    while ( iter )
    {
        r = iter.GetRect();

        // TODO: remove this when we can...
        // There is a bug in wxMotif that gives garbage update
        // rectangles if you jump-scroll a long way by clicking the
        // scrollbar with middle button.  This is a work-around
        //
#if defined(__WXMOTIF__)
        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );
        if ( r.GetTop() > ch ) r.SetTop( 0 );
        if ( r.GetLeft() > cw ) r.SetLeft( 0 );
        r.SetRight( wxMin( r.GetRight(), cw ) );
        r.SetBottom( wxMin( r.GetBottom(), ch ) );
#endif

        // logical bounds of update region
        //
        CalcUnscrolledPosition( r.GetLeft(), r.GetTop(), &left, &top );
        CalcUnscrolledPosition( r.GetRight(), r.GetBottom(), &right, &bottom );

        // find the cells within these bounds
        //
        int row, col;
        int colLeft, rowTop;
        for ( row = 0;  row < m_numRows;  row++ )
        {
            if ( m_rowBottoms[row] < top ) continue;

            rowTop = m_rowBottoms[row] - m_rowHeights[row];
            if ( rowTop > bottom ) break;

            m_rowsExposed.Add( row );

            for ( col = 0;  col < m_numCols;  col++ )
            {
                if ( m_colRights[col] < left ) continue;

                colLeft = m_colRights[col] - m_colWidths[col];
                if ( colLeft > right ) break;

                if ( m_colsExposed.Index( col ) == wxNOT_FOUND ) m_colsExposed.Add( col );
                m_cellsExposed.Add( wxGridCellCoords( row, col ) );
            }
        }

        iter++ ;
    }
}


void wxGrid::ProcessRowLabelMouseEvent( wxMouseEvent& event )
{
    int x, y, row;
    wxPoint pos( event.GetPosition() );
    CalcUnscrolledPosition( pos.x, pos.y, &x, &y );

    if ( event.Dragging() )
    {
        m_isDragging = TRUE;

        if ( event.LeftIsDown() )
        {
            switch( m_cursorMode )
            {
                case WXGRID_CURSOR_RESIZE_ROW:
                {
                    int cw, ch, left, dummy;
                    m_gridWin->GetClientSize( &cw, &ch );
                    CalcUnscrolledPosition( 0, 0, &left, &dummy );

                    wxClientDC dc( m_gridWin );
                    PrepareDC( dc );
                    dc.SetLogicalFunction(wxINVERT);
                    if ( m_dragLastPos >= 0 )
                    {
                        dc.DrawLine( left, m_dragLastPos, left+cw, m_dragLastPos );
                    }
                    dc.DrawLine( left, y, left+cw, y );
                    m_dragLastPos = y;
                }
                break;

                case WXGRID_CURSOR_SELECT_ROW:
                {
                    if ( (row = YToRow( y )) >= 0  &&
                         !IsInSelection( row, 0 ) )
                    {
                        SelectRow( row, TRUE );
                    }
                }
                break;
            }
        }
        return;
    }

    m_isDragging = FALSE;


    // ------------ Left button pressed
    //
    if ( event.LeftDown() )
    {
        // don't send a label click event for a hit on the
        // edge of the row label - this is probably the user
        // wanting to resize the row
        //
        if ( YToEdgeOfRow(y) < 0 )
        {
            row = YToRow(y);
            if ( row >= 0  &&
                 !SendEvent( EVT_GRID_LABEL_LEFT_CLICK, row, -1, event ) )
            {
                SelectRow( row, event.ShiftDown() );
                m_cursorMode = WXGRID_CURSOR_SELECT_ROW;
            }
        }
        else
        {
            // starting to drag-resize a row
            //
            m_rowLabelWin->CaptureMouse();
        }
    }


    // ------------ Left double click
    //
    else if (event.LeftDClick() )
    {
        if ( YToEdgeOfRow(y) < 0 )
        {
            row = YToRow(y);
            SendEvent(  EVT_GRID_LABEL_LEFT_DCLICK, row, -1, event );
        }
    }


    // ------------ Left button released
    //
    else if ( event.LeftUp() )
    {
        if ( m_cursorMode == WXGRID_CURSOR_RESIZE_ROW )
        {
            m_rowLabelWin->ReleaseMouse();

            if ( m_dragLastPos >= 0 )
            {
                // erase the last line and resize the row
                //
                int cw, ch, left, dummy;
                m_gridWin->GetClientSize( &cw, &ch );
                CalcUnscrolledPosition( 0, 0, &left, &dummy );

                wxClientDC dc( m_gridWin );
                PrepareDC( dc );
                dc.SetLogicalFunction( wxINVERT );
                dc.DrawLine( left, m_dragLastPos, left+cw, m_dragLastPos );
                HideCellEditControl();

                int rowTop = m_rowBottoms[m_dragRowOrCol] - m_rowHeights[m_dragRowOrCol];
                SetRowSize( m_dragRowOrCol, wxMax( y - rowTop, WXGRID_MIN_ROW_HEIGHT ) );
                if ( !GetBatchCount() )
                {
                  // TODO: optimize this
                  m_rowLabelWin->Refresh();
                  m_gridWin->Refresh();
                }

                ShowCellEditControl();

                // Note: we are ending the event *after* doing
                // default processing in this case
                //
                SendEvent( EVT_GRID_ROW_SIZE, m_dragRowOrCol, -1, event );
            }
        }

        m_dragLastPos  = -1;
    }


    // ------------ Right button down
    //
    else if ( event.RightDown() )
    {
        row = YToRow(y);
        if ( !SendEvent( EVT_GRID_LABEL_RIGHT_CLICK, row, -1, event ) )
        {
            // no default action at the moment
        }
    }


    // ------------ Right double click
    //
    else if ( event.RightDClick() )
    {
        row = YToRow(y);
        if ( !SendEvent( EVT_GRID_LABEL_RIGHT_DCLICK, row, -1, event ) )
        {
            // no default action at the moment
        }
    }


    // ------------ No buttons down and mouse moving
    //
    else if ( event.Moving() )
    {
        m_dragRowOrCol = YToEdgeOfRow( y );
        if ( m_dragRowOrCol >= 0 )
        {
            if ( m_cursorMode == WXGRID_CURSOR_SELECT_CELL )
            {
                m_cursorMode = WXGRID_CURSOR_RESIZE_ROW;
                m_rowLabelWin->SetCursor( m_rowResizeCursor );
            }
        }
        else
        {
            if ( m_cursorMode != WXGRID_CURSOR_SELECT_CELL )
            {
                m_cursorMode = WXGRID_CURSOR_SELECT_CELL;
                m_rowLabelWin->SetCursor( *wxSTANDARD_CURSOR );
            }
        }
    }
}


void wxGrid::ProcessColLabelMouseEvent( wxMouseEvent& event )
{
    int x, y, col;
    wxPoint pos( event.GetPosition() );
    CalcUnscrolledPosition( pos.x, pos.y, &x, &y );

    if ( event.Dragging() )
    {
        m_isDragging = TRUE;

        if ( event.LeftIsDown() )
        {
            switch( m_cursorMode )
            {
                case WXGRID_CURSOR_RESIZE_COL:
                {
                    int cw, ch, dummy, top;
                    m_gridWin->GetClientSize( &cw, &ch );
                    CalcUnscrolledPosition( 0, 0, &dummy, &top );

                    wxClientDC dc( m_gridWin );
                    PrepareDC( dc );
                    dc.SetLogicalFunction(wxINVERT);
                    if ( m_dragLastPos >= 0 )
                    {
                        dc.DrawLine( m_dragLastPos, top, m_dragLastPos, top+ch );
                    }
                    dc.DrawLine( x, top, x, top+ch );
                    m_dragLastPos = x;
                }
                break;

                case WXGRID_CURSOR_SELECT_COL:
                {
                    if ( (col = XToCol( x )) >= 0  &&
                         !IsInSelection( 0, col ) )
                    {
                        SelectCol( col, TRUE );
                    }
                }
                break;
            }
        }
        return;
    }

    m_isDragging = FALSE;


    // ------------ Left button pressed
    //
    if ( event.LeftDown() )
    {
        // don't send a label click event for a hit on the
        // edge of the col label - this is probably the user
        // wanting to resize the col
        //
        if ( XToEdgeOfCol(x) < 0 )
        {
            col = XToCol(x);
            if ( col >= 0  &&
                 !SendEvent( EVT_GRID_LABEL_LEFT_CLICK, -1, col, event ) )
            {
                SelectCol( col, event.ShiftDown() );
                m_cursorMode = WXGRID_CURSOR_SELECT_COL;
            }
        }
        else
        {
            // starting to drag-resize a col
            //
            m_colLabelWin->CaptureMouse();
        }
    }


    // ------------ Left double click
    //
    if ( event.LeftDClick() )
    {
        if ( XToEdgeOfCol(x) < 0 )
        {
            col = XToCol(x);
            SendEvent(  EVT_GRID_LABEL_LEFT_DCLICK, -1, col, event );
        }
    }


    // ------------ Left button released
    //
    else if ( event.LeftUp() )
    {
        if ( m_cursorMode == WXGRID_CURSOR_RESIZE_COL )
        {
            m_colLabelWin->ReleaseMouse();

            if ( m_dragLastPos >= 0 )
            {
                // erase the last line and resize the col
                //
                int cw, ch, dummy, top;
                m_gridWin->GetClientSize( &cw, &ch );
                CalcUnscrolledPosition( 0, 0, &dummy, &top );

                wxClientDC dc( m_gridWin );
                PrepareDC( dc );
                dc.SetLogicalFunction( wxINVERT );
                dc.DrawLine( m_dragLastPos, top, m_dragLastPos, top+ch );
                HideCellEditControl();

                int colLeft = m_colRights[m_dragRowOrCol] - m_colWidths[m_dragRowOrCol];
                SetColSize( m_dragRowOrCol, wxMax( x - colLeft, WXGRID_MIN_COL_WIDTH ) );

                if ( !GetBatchCount() )
                {
                  // TODO: optimize this
                  m_colLabelWin->Refresh();
                  m_gridWin->Refresh();
                }

                ShowCellEditControl();

                // Note: we are ending the event *after* doing
                // default processing in this case
                //
                SendEvent( EVT_GRID_COL_SIZE, -1, m_dragRowOrCol, event );
            }
        }

        m_dragLastPos  = -1;
    }


    // ------------ Right button down
    //
    else if ( event.RightDown() )
    {
        col = XToCol(x);
        if ( !SendEvent( EVT_GRID_LABEL_RIGHT_CLICK, -1, col, event ) )
        {
            // no default action at the moment
        }
    }


    // ------------ Right double click
    //
    else if ( event.RightDClick() )
    {
        col = XToCol(x);
        if ( !SendEvent( EVT_GRID_LABEL_RIGHT_DCLICK, -1, col, event ) )
        {
            // no default action at the moment
        }
    }


    // ------------ No buttons down and mouse moving
    //
    else if ( event.Moving() )
    {
        m_dragRowOrCol = XToEdgeOfCol( x );
        if ( m_dragRowOrCol >= 0 )
        {
            if ( m_cursorMode == WXGRID_CURSOR_SELECT_CELL )
            {
                m_cursorMode = WXGRID_CURSOR_RESIZE_COL;
                m_colLabelWin->SetCursor( m_colResizeCursor );
            }
        }
        else
        {
            if ( m_cursorMode != WXGRID_CURSOR_SELECT_CELL )
            {
                m_cursorMode = WXGRID_CURSOR_SELECT_CELL;
                m_colLabelWin->SetCursor( *wxSTANDARD_CURSOR );
            }
        }
    }
}


void wxGrid::ProcessCornerLabelMouseEvent( wxMouseEvent& event )
{
    if ( event.LeftDown() )
    {
        // indicate corner label by having both row and
        // col args == -1
        //
        if ( !SendEvent( EVT_GRID_LABEL_LEFT_CLICK, -1, -1, event ) )
        {
            SelectAll();
        }
    }

    else if ( event.LeftDClick() )
    {
        SendEvent( EVT_GRID_LABEL_LEFT_DCLICK, -1, -1, event );
    }

    else if ( event.RightDown() )
    {
        if ( !SendEvent( EVT_GRID_LABEL_RIGHT_CLICK, -1, -1, event ) )
        {
            // no default action at the moment
        }
    }

    else if ( event.RightDClick() )
    {
        if ( !SendEvent( EVT_GRID_LABEL_RIGHT_DCLICK, -1, -1, event ) )
        {
            // no default action at the moment
        }
    }
}


void wxGrid::ProcessGridCellMouseEvent( wxMouseEvent& event )
{
    int x, y;
    wxPoint pos( event.GetPosition() );
    CalcUnscrolledPosition( pos.x, pos.y, &x, &y );

    wxGridCellCoords coords;
    XYToCell( x, y, coords );

    if ( event.Dragging() )
    {
        m_isDragging = TRUE;
        if ( m_cursorMode == WXGRID_CURSOR_SELECT_CELL )
        {
            // Hide the edit control, so it
            // won't interfer with drag-shrinking.
            if ( IsCellEditControlEnabled() )
                HideCellEditControl();
            if ( coords != wxGridNoCellCoords )
            {
                if ( !IsSelection() )
                {
                    SelectBlock( coords, coords );
                }
                else
                {
                    SelectBlock( m_currentCellCoords, coords );
                }
            }
        }

        return;
    }

    m_isDragging = FALSE;

    if ( coords != wxGridNoCellCoords )
    {
        if ( event.LeftDown() )
        {
            if ( event.ShiftDown() )
            {
                SelectBlock( m_currentCellCoords, coords );
            }
            else
            {
                if ( !SendEvent( EVT_GRID_CELL_LEFT_CLICK,
                                 coords.GetRow(),
                                 coords.GetCol(),
                                 event ) )
                {
                    MakeCellVisible( coords );
                    SetCurrentCell( coords );
                }
            }
        }


        // ------------ Left double click
        //
        else if ( event.LeftDClick() )
        {
            SendEvent( EVT_GRID_CELL_LEFT_DCLICK,
                       coords.GetRow(),
                       coords.GetCol(),
                       event );
        }


        // ------------ Left button released
        //
        else if ( event.LeftUp() )
        {
            if ( m_cursorMode == WXGRID_CURSOR_SELECT_CELL )
            {
                if ( IsSelection() )
                {
                    SendEvent( EVT_GRID_RANGE_SELECT, -1, -1, event );
                }
            }

            // Show the edit control, if it has
            // been hidden for drag-shrinking.
            if ( IsCellEditControlEnabled() )
                ShowCellEditControl();

            m_dragLastPos = -1;
        }


        // ------------ Right button down
        //
        else if ( event.RightDown() )
        {
            if ( !SendEvent( EVT_GRID_CELL_RIGHT_CLICK,
                             coords.GetRow(),
                             coords.GetCol(),
                             event ) )
            {
                // no default action at the moment
            }
        }


        // ------------ Right double click
        //
        else if ( event.RightDClick() )
        {
            if ( !SendEvent( EVT_GRID_CELL_RIGHT_DCLICK,
                             coords.GetRow(),
                             coords.GetCol(),
                             event ) )
            {
                // no default action at the moment
            }
        }
    }
}


//
// ------ interaction with data model
//
bool wxGrid::ProcessTableMessage( wxGridTableMessage& msg )
{
    switch ( msg.GetId() )
    {
        case wxGRIDTABLE_REQUEST_VIEW_GET_VALUES:
            return GetModelValues();

        case wxGRIDTABLE_REQUEST_VIEW_SEND_VALUES:
            return SetModelValues();

        case wxGRIDTABLE_NOTIFY_ROWS_INSERTED:
        case wxGRIDTABLE_NOTIFY_ROWS_APPENDED:
        case wxGRIDTABLE_NOTIFY_ROWS_DELETED:
        case wxGRIDTABLE_NOTIFY_COLS_INSERTED:
        case wxGRIDTABLE_NOTIFY_COLS_APPENDED:
        case wxGRIDTABLE_NOTIFY_COLS_DELETED:
            return Redimension( msg );

        default:
            return FALSE;
    }
}



// The behaviour of this function depends on the grid table class
// Clear() function.  For the default wxGridStringTable class the
// behavious is to replace all cell contents with wxEmptyString but
// not to change the number of rows or cols.
//
void wxGrid::ClearGrid()
{
    if ( m_table )
    {
        m_table->Clear();
        SetEditControlValue();
        if ( !GetBatchCount() ) m_gridWin->Refresh();
    }
}


bool wxGrid::InsertRows( int pos, int numRows, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxLogError( wxT("Called wxGrid::InsertRows() before calling CreateGrid()") );
        return FALSE;
    }

    if ( m_table )
    {
        bool ok = m_table->InsertRows( pos, numRows );

        // the table will have sent the results of the insert row
        // operation to this view object as a grid table message
        //
        if ( ok )
        {
            if ( m_numCols == 0 )
            {
                m_table->AppendCols( WXGRID_DEFAULT_NUMBER_COLS );
                //
                // TODO: perhaps instead of appending the default number of cols
                // we should remember what the last non-zero number of cols was ?
                //
            }

            if ( m_currentCellCoords == wxGridNoCellCoords )
            {
                // if we have just inserted cols into an empty grid the current
                // cell will be undefined...
                //
                SetCurrentCell( 0, 0 );
            }

            ClearSelection();
            if ( !GetBatchCount() ) Refresh();
        }

        SetEditControlValue();
        return ok;
    }
    else
    {
        return FALSE;
    }
}


bool wxGrid::AppendRows( int numRows, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxLogError( wxT("Called wxGrid::AppendRows() before calling CreateGrid()") );
        return FALSE;
    }

    if ( m_table && m_table->AppendRows( numRows ) )
    {
        if ( m_currentCellCoords == wxGridNoCellCoords )
        {
            // if we have just inserted cols into an empty grid the current
            // cell will be undefined...
            //
            SetCurrentCell( 0, 0 );
        }

        // the table will have sent the results of the append row
        // operation to this view object as a grid table message
        //
        ClearSelection();
        if ( !GetBatchCount() ) Refresh();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool wxGrid::DeleteRows( int pos, int numRows, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxLogError( wxT("Called wxGrid::DeleteRows() before calling CreateGrid()") );
        return FALSE;
    }

    if ( m_table && m_table->DeleteRows( pos, numRows ) )
    {
        // the table will have sent the results of the delete row
        // operation to this view object as a grid table message
        //
        if ( m_numRows > 0 )
            SetEditControlValue();
        else
            HideCellEditControl();

        ClearSelection();
        if ( !GetBatchCount() ) Refresh();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool wxGrid::InsertCols( int pos, int numCols, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxLogError( wxT("Called wxGrid::InsertCols() before calling CreateGrid()") );
        return FALSE;
    }

    if ( m_table )
    {
        HideCellEditControl();
        bool ok = m_table->InsertCols( pos, numCols );

        // the table will have sent the results of the insert col
        // operation to this view object as a grid table message
        //
        if ( ok )
        {
            if ( m_currentCellCoords == wxGridNoCellCoords )
            {
                // if we have just inserted cols into an empty grid the current
                // cell will be undefined...
                //
                SetCurrentCell( 0, 0 );
            }

            ClearSelection();
            if ( !GetBatchCount() ) Refresh();
        }

        SetEditControlValue();
        return ok;
    }
    else
    {
        return FALSE;
    }
}


bool wxGrid::AppendCols( int numCols, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxLogError( wxT("Called wxGrid::AppendCols() before calling CreateGrid()") );
        return FALSE;
    }

    if ( m_table && m_table->AppendCols( numCols ) )
    {
        // the table will have sent the results of the append col
        // operation to this view object as a grid table message
        //
        if ( m_currentCellCoords == wxGridNoCellCoords )
        {
            // if we have just inserted cols into an empty grid the current
            // cell will be undefined...
            //
            SetCurrentCell( 0, 0 );
        }

        ClearSelection();
        if ( !GetBatchCount() ) Refresh();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool wxGrid::DeleteCols( int pos, int numCols, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxLogError( wxT("Called wxGrid::DeleteCols() before calling CreateGrid()") );
        return FALSE;
    }

    if ( m_table && m_table->DeleteCols( pos, numCols ) )
    {
        // the table will have sent the results of the delete col
        // operation to this view object as a grid table message
        //
        if ( m_numCols > 0 )
            SetEditControlValue();
        else
            HideCellEditControl();

        ClearSelection();
        if ( !GetBatchCount() ) Refresh();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



//
// ----- event handlers
//

// Generate a grid event based on a mouse event and
// return the result of ProcessEvent()
//
bool wxGrid::SendEvent( const wxEventType type,
                        int row, int col,
                        wxMouseEvent& mouseEv )
{
    if ( type == EVT_GRID_ROW_SIZE ||
         type == EVT_GRID_COL_SIZE )
    {
        int rowOrCol = (row == -1 ? col : row);

        wxGridSizeEvent gridEvt( GetId(),
                                 type,
                                 this,
                                 rowOrCol,
                                 mouseEv.GetX(), mouseEv.GetY(),
                                 mouseEv.ControlDown(),
                                 mouseEv.ShiftDown(),
                                 mouseEv.AltDown(),
                                 mouseEv.MetaDown() );

        return GetEventHandler()->ProcessEvent(gridEvt);
    }
    else if ( type == EVT_GRID_RANGE_SELECT )
    {
        wxGridRangeSelectEvent gridEvt( GetId(),
                                        type,
                                        this,
                                        m_selectedTopLeft,
                                        m_selectedBottomRight,
                                        mouseEv.ControlDown(),
                                        mouseEv.ShiftDown(),
                                        mouseEv.AltDown(),
                                        mouseEv.MetaDown() );

        return GetEventHandler()->ProcessEvent(gridEvt);
    }
    else
    {
        wxGridEvent gridEvt( GetId(),
                             type,
                             this,
                             row, col,
                             mouseEv.GetX(), mouseEv.GetY(),
                             mouseEv.ControlDown(),
                             mouseEv.ShiftDown(),
                             mouseEv.AltDown(),
                             mouseEv.MetaDown() );

        return GetEventHandler()->ProcessEvent(gridEvt);
    }
}


// Generate a grid event of specified type and return the result
// of ProcessEvent().
//
bool wxGrid::SendEvent( const wxEventType type,
                        int row, int col )
{
    if ( type == EVT_GRID_ROW_SIZE ||
         type == EVT_GRID_COL_SIZE )
    {
        int rowOrCol = (row == -1 ? col : row);

        wxGridSizeEvent gridEvt( GetId(),
                                 type,
                                 this,
                                 rowOrCol );

        return GetEventHandler()->ProcessEvent(gridEvt);
    }
    else
    {
        wxGridEvent gridEvt( GetId(),
                             type,
                             this,
                             row, col );

        return GetEventHandler()->ProcessEvent(gridEvt);
    }
}


void wxGrid::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc( this );

    if ( m_currentCellCoords == wxGridNoCellCoords  &&
         m_numRows && m_numCols )
    {
        m_currentCellCoords.Set(0, 0);
        SetEditControlValue();
        ShowCellEditControl();
    }
}


// This is just here to make sure that CalcDimensions gets called when
// the grid view is resized... then the size event is skipped to allow
// the box sizers to handle everything
//
void wxGrid::OnSize( wxSizeEvent& event )
{
    CalcDimensions();
    event.Skip();
}


void wxGrid::OnKeyDown( wxKeyEvent& event )
{
    if ( m_inOnKeyDown )
    {
        // shouldn't be here - we are going round in circles...
        //
        wxLogFatalError( wxT("wxGrid::OnKeyDown called while alread active") );
    }

    m_inOnKeyDown = TRUE;

    // propagate the event up and see if it gets processed
    //
    wxWindow *parent = GetParent();
    wxKeyEvent keyEvt( event );
    keyEvt.SetEventObject( parent );

    if ( !parent->GetEventHandler()->ProcessEvent( keyEvt ) )
    {
        // try local handlers
        //
        switch ( event.KeyCode() )
        {
            case WXK_UP:
                if ( event.ControlDown() )
                {
                    MoveCursorUpBlock();
                }
                else
                {
                    MoveCursorUp();
                }
                break;

            case WXK_DOWN:
                if ( event.ControlDown() )
                {
                    MoveCursorDownBlock();
                }
                else
                {
                    MoveCursorDown();
                }
                break;

            case WXK_LEFT:
                if ( event.ControlDown() )
                {
                    MoveCursorLeftBlock();
                }
                else
                {
                    MoveCursorLeft();
                }
                break;

            case WXK_RIGHT:
                if ( event.ControlDown() )
                {
                    MoveCursorRightBlock();
                }
                else
                {
                    MoveCursorRight();
                }
                break;

            case WXK_RETURN:
                if ( event.ControlDown() )
                {
                    event.Skip();  // to let the edit control have the return
                }
                else
                {
                    MoveCursorDown();
                }
                break;

            case WXK_HOME:
                if ( event.ControlDown() )
                {
                    MakeCellVisible( 0, 0 );
                    SetCurrentCell( 0, 0 );
                }
                else
                {
                    event.Skip();
                }
                break;

            case WXK_END:
                if ( event.ControlDown() )
                {
                    MakeCellVisible( m_numRows-1, m_numCols-1 );
                    SetCurrentCell( m_numRows-1, m_numCols-1 );
                }
                else
                {
                    event.Skip();
                }
                break;

            case WXK_PRIOR:
                MovePageUp();
                break;

            case WXK_NEXT:
                MovePageDown();
                break;

            default:
                // now try the cell edit control
                //
                if ( IsCellEditControlEnabled() )
                {
                    event.SetEventObject( m_cellEditCtrl );
                    m_cellEditCtrl->GetEventHandler()->ProcessEvent( event );
                }
                break;
        }
    }

    m_inOnKeyDown = FALSE;
}


void wxGrid::SetCurrentCell( const wxGridCellCoords& coords )
{
    if ( SendEvent( EVT_GRID_SELECT_CELL, coords.GetRow(), coords.GetCol() ) )
    {
        // the event has been intercepted - do nothing
        return;
    }

    wxClientDC dc( m_gridWin );
    PrepareDC( dc );

    if ( m_currentCellCoords != wxGridNoCellCoords )
    {
        HideCellEditControl();
        SaveEditControlValue();
    }

    m_currentCellCoords = coords;

    SetEditControlValue();
    ShowCellEditControl();

    if ( IsSelection() )
    {
        wxRect r( SelectionToDeviceRect() );
        ClearSelection();
        if ( !GetBatchCount() ) m_gridWin->Refresh( TRUE, &r );
    }
}


//
// ------ functions to get/send data (see also public functions)
//

bool wxGrid::GetModelValues()
{
    if ( m_table )
    {
        // all we need to do is repaint the grid
        //
        m_gridWin->Refresh();
        return TRUE;
    }

    return FALSE;
}


bool wxGrid::SetModelValues()
{
    int row, col;

    if ( m_table )
    {
        for ( row = 0;  row < m_numRows;  row++ )
        {
            for ( col = 0;  col < m_numCols;  col++ )
            {
                m_table->SetValue( row, col, GetCellValue(row, col) );
            }
        }

        return TRUE;
    }

    return FALSE;
}



// Note - this function only draws cells that are in the list of
// exposed cells (usually set from the update region by
// CalcExposedCells)
//
void wxGrid::DrawGridCellArea( wxDC& dc )
{
    if ( !m_numRows || !m_numCols ) return;

    size_t i;
    size_t numCells = m_cellsExposed.GetCount();

    for ( i = 0;  i < numCells;  i++ )
    {
        DrawCell( dc, m_cellsExposed[i] );
    }
}


void wxGrid::DrawCell( wxDC& dc, const wxGridCellCoords& coords )
{
    if ( m_colWidths[coords.GetCol()] <=0  ||
         m_rowHeights[coords.GetRow()] <= 0 ) return;

    if ( m_gridLinesEnabled )
        DrawCellBorder( dc, coords );

    DrawCellBackground( dc, coords );

    // TODO: separate functions here for different kinds of cells ?
    // e.g. text, image
    //
    DrawCellValue( dc, coords );
}


void wxGrid::DrawCellBorder( wxDC& dc, const wxGridCellCoords& coords )
{
    if ( m_colWidths[coords.GetCol()] <=0  ||
         m_rowHeights[coords.GetRow()] <= 0 ) return;

    dc.SetPen( wxPen(GetGridLineColour(), 1, wxSOLID) );
    int row = coords.GetRow();
    int col = coords.GetCol();

    // right hand border
    //
    dc.DrawLine( m_colRights[col], m_rowBottoms[row] - m_rowHeights[row],
                 m_colRights[col], m_rowBottoms[row] );

    // bottom border
    //
    dc.DrawLine( m_colRights[col] - m_colWidths[col], m_rowBottoms[row],
                 m_colRights[col], m_rowBottoms[row] );
}


void wxGrid::DrawCellBackground( wxDC& dc, const wxGridCellCoords& coords )
{
    if ( m_colWidths[coords.GetCol()] <=0  ||
         m_rowHeights[coords.GetRow()] <= 0 ) return;

    int row = coords.GetRow();
    int col = coords.GetCol();

    dc.SetBackgroundMode( wxSOLID );

    if ( IsInSelection( coords ) )
    {
        // TODO: improve this
        //
        dc.SetBrush( *wxBLACK_BRUSH );
    }
    else
    {
        dc.SetBrush( wxBrush(GetCellBackgroundColour(row, col), wxSOLID) );
    }

    dc.SetPen( *wxTRANSPARENT_PEN );

    dc.DrawRectangle( m_colRights[col] - m_colWidths[col] + 1,
                      m_rowBottoms[row] - m_rowHeights[row] + 1,
                      m_colWidths[col]-1,
                      m_rowHeights[row]-1 );
}


void wxGrid::DrawCellValue( wxDC& dc, const wxGridCellCoords& coords )
{
    if ( m_colWidths[coords.GetCol()] <=0  ||
         m_rowHeights[coords.GetRow()] <= 0 ) return;

    int row = coords.GetRow();
    int col = coords.GetCol();

    dc.SetBackgroundMode( wxTRANSPARENT );

    if ( IsInSelection( row, col ) )
    {
        // TODO: improve this
        //
        dc.SetTextBackground( wxColour(0, 0, 0) );
        dc.SetTextForeground( wxColour(255, 255, 255) );
    }
    else
    {
        dc.SetTextBackground( GetCellBackgroundColour(row, col) );
        dc.SetTextForeground( GetCellTextColour(row, col) );
    }
    dc.SetFont( GetCellFont(row, col) );

    int hAlign, vAlign;
    GetCellAlignment( row, col, &hAlign, &vAlign );

    wxRect rect;
    rect.SetX( m_colRights[col] - m_colWidths[col] + 2 );
    rect.SetY( m_rowBottoms[row] - m_rowHeights[row] + 2 );
    rect.SetWidth( m_colWidths[col] - 4 );
    rect.SetHeight( m_rowHeights[row] - 4 );

    DrawTextRectangle( dc, GetCellValue( row, col ), rect, hAlign, vAlign );
}



// TODO: remove this ???
// This is used to redraw all grid lines e.g. when the grid line colour
// has been changed
//
void wxGrid::DrawAllGridLines( wxDC& dc )
{
    if ( !m_gridLinesEnabled ||
         !m_numRows ||
         !m_numCols ) return;

    int cw, ch;
    m_gridWin->GetClientSize(&cw, &ch);

    // virtual coords of visible area
    //
    int top, bottom, left, right;
    CalcUnscrolledPosition( 0, 0, &left, &top );
    CalcUnscrolledPosition( cw, ch, &right, &bottom );

    dc.SetPen( wxPen(GetGridLineColour(), 1, wxSOLID) );

    // horizontal grid lines
    //
    int i;
    for ( i = 0; i <= m_numRows; i++ )
    {
        if ( m_rowBottoms[i] > bottom )
        {
            break;
        }
        else if ( m_rowBottoms[i] >= top )
        {
            dc.DrawLine( left, m_rowBottoms[i], right, m_rowBottoms[i] );
        }
    }


    // vertical grid lines
    //
    for ( i = 0; i <= m_numCols; i++ )
    {
        if ( m_colRights[i] > right )
        {
            break;
        }
        else if ( m_colRights[i] >= left )
        {
            dc.DrawLine( m_colRights[i], top, m_colRights[i], bottom );
        }
    }
}


void wxGrid::DrawRowLabels( wxDC& dc )
{
    if ( !m_numRows || !m_numCols ) return;

    size_t i;
    size_t numLabels = m_rowLabelsExposed.GetCount();

    for ( i = 0;  i < numLabels;  i++ )
    {
        DrawRowLabel( dc, m_rowLabelsExposed[i] );
    }
}


void wxGrid::DrawRowLabel( wxDC& dc, int row )
{
    if ( m_rowHeights[row] <= 0 ) return;

    // draw the label's horizontal border (the vertical border is
    // provided by the cell area window margin)
    //
    dc.SetPen( *wxBLACK_PEN );

    dc.DrawLine( 0, m_rowBottoms[row]+1,
                 m_rowLabelWidth, m_rowBottoms[row]+1 );

    dc.SetPen( *wxWHITE_PEN );

    dc.DrawLine( 0, m_rowBottoms[row]+2,
                 m_rowLabelWidth, m_rowBottoms[row]+2 );

    dc.SetBackgroundMode( wxTRANSPARENT );
    dc.SetTextForeground( GetLabelTextColour() );
    dc.SetFont( GetLabelFont() );

    int hAlign, vAlign;
    GetRowLabelAlignment( &hAlign, &vAlign );

    wxRect rect;
    rect.SetX( 2 );
    rect.SetY( m_rowBottoms[row] - m_rowHeights[row] + 2 );
    rect.SetWidth( m_rowLabelWidth - 4 );
    rect.SetHeight( m_rowHeights[row] - 4 );
    DrawTextRectangle( dc, GetRowLabelValue( row ), rect, hAlign, vAlign );
}


void wxGrid::DrawColLabels( wxDC& dc )
{
    if ( !m_numRows || !m_numCols ) return;

    size_t i;
    size_t numLabels = m_colLabelsExposed.GetCount();

    for ( i = 0;  i < numLabels;  i++ )
    {
        DrawColLabel( dc, m_colLabelsExposed[i] );
    }
}


void wxGrid::DrawColLabel( wxDC& dc, int col )
{
    if ( m_colWidths[col] <= 0 ) return;

    // draw the label's vertical border (the horizontal border is
    // provided by the cell area window margin)
    //
    dc.SetPen( *wxBLACK_PEN );

    dc.DrawLine( m_colRights[col]+1, 0,
                 m_colRights[col]+1, m_colLabelHeight );

    dc.SetPen( *wxWHITE_PEN );

    dc.DrawLine( m_colRights[col]+2, 0,
                 m_colRights[col]+2, m_colLabelHeight );

    dc.SetBackgroundMode( wxTRANSPARENT );
    dc.SetTextForeground( GetLabelTextColour() );
    dc.SetFont( GetLabelFont() );

    int hAlign, vAlign;
    GetColLabelAlignment( &hAlign, &vAlign );

    wxRect rect;
    rect.SetX( m_colRights[col] - m_colWidths[col] + 2 );
    rect.SetY( 2 );
    rect.SetWidth( m_colWidths[col] - 4 );
    rect.SetHeight( m_colLabelHeight - 4 );
    DrawTextRectangle( dc, GetColLabelValue( col ), rect, hAlign, vAlign );
}


void wxGrid::DrawTextRectangle( wxDC& dc,
                                const wxString& value,
                                const wxRect& rect,
                                int horizAlign,
                                int vertAlign )
{
    long textWidth, textHeight;
    long lineWidth, lineHeight;
    wxArrayString lines;

    dc.SetClippingRegion( rect );
    StringToLines( value, lines );
    if ( lines.GetCount() )
    {
        GetTextBoxSize( dc, lines, &textWidth, &textHeight );
        dc.GetTextExtent( lines[0], &lineWidth, &lineHeight );

        float x, y;
        switch ( horizAlign )
        {
            case wxRIGHT:
                x = rect.x + (rect.width - textWidth - 1);
                break;

            case wxCENTRE:
                x = rect.x + ((rect.width - textWidth)/2);
                break;

            case wxLEFT:
            default:
                x = rect.x + 1;
                break;
        }

        switch ( vertAlign )
        {
            case wxBOTTOM:
                y = rect.y + (rect.height - textHeight - 1);
                break;

            case wxCENTRE:
                y = rect.y + ((rect.height - textHeight)/2);
                break;

            case wxTOP:
            default:
                y = rect.y + 1;
                break;
        }

        for ( size_t i = 0;  i < lines.GetCount();  i++ )
        {
            dc.DrawText( lines[i], (long)x, (long)y );
            y += lineHeight;
        }
    }

    dc.DestroyClippingRegion();
}


// Split multi line text up into an array of strings.  Any existing
// contents of the string array are preserved.
//
void wxGrid::StringToLines( const wxString& value, wxArrayString& lines )
{
    // TODO: this won't work for WXMAC ? (lines end with '\r')
    //       => use wxTextFile functions then (VZ)
    int startPos = 0;
    int pos;
    while ( startPos < (int)value.Length() )
    {
        pos = value.Mid(startPos).Find( '\n' );
        if ( pos < 0 )
        {
            break;
        }
        else if ( pos == 0 )
        {
            lines.Add( wxEmptyString );
        }
        else
        {
            if ( value[startPos+pos-1] == '\r' )
            {
                lines.Add( value.Mid(startPos, pos-1) );
            }
            else
            {
                lines.Add( value.Mid(startPos, pos) );
            }
        }
        startPos += pos+1;
    }
    if ( startPos < (int)value.Length() )
    {
        lines.Add( value.Mid( startPos ) );
    }
}


void wxGrid::GetTextBoxSize( wxDC& dc,
                             wxArrayString& lines,
                             long *width, long *height )
{
    long w = 0;
    long h = 0;
    long lineW, lineH;

    size_t i;
    for ( i = 0;  i < lines.GetCount();  i++ )
    {
        dc.GetTextExtent( lines[i], &lineW, &lineH );
        w = wxMax( w, lineW );
        h += lineH;
    }

    *width = w;
    *height = h;
}


//
// ------ Edit control functions
//


void wxGrid::EnableEditing( bool edit )
{
    // TODO: improve this ?
    //
    if ( edit != m_editable )
    {
        m_editable = edit;

        // TODO: extend this for other edit control types
        //
        if ( m_editCtrlType == wxGRID_TEXTCTRL )
        {
            ((wxTextCtrl *)m_cellEditCtrl)->SetEditable( m_editable );
        }
    }
}


#if 0  // disabled for the moment - the cell control is always active
void wxGrid::EnableCellEditControl( bool enable )
{
    if ( m_cellEditCtrl &&
         enable != m_cellEditCtrlEnabled )
    {
        m_cellEditCtrlEnabled = enable;

        if ( m_cellEditCtrlEnabled )
        {
            SetEditControlValue();
            ShowCellEditControl();
        }
        else
        {
            HideCellEditControl();
            SaveEditControlValue();
        }
    }
}
#endif


void wxGrid::ShowCellEditControl()
{
    wxRect rect;

    if ( IsCellEditControlEnabled() )
    {
        if ( !IsVisible( m_currentCellCoords ) )
        {
            return;
        }
        else
        {
            rect = CellToRect( m_currentCellCoords );

            // convert to scrolled coords
            //
            int left, top, right, bottom;
            CalcScrolledPosition( rect.GetLeft(), rect.GetTop(), &left, &top );
            CalcScrolledPosition( rect.GetRight(), rect.GetBottom(), &right, &bottom );

            int cw, ch;
            m_gridWin->GetClientSize( &cw, &ch );

            // Make the edit control large enough to allow for internal margins
            // TODO: remove this if the text ctrl sizing is improved esp. for unix
            //
            int extra;
#if defined(__WXMOTIF__)
            if ( m_currentCellCoords.GetRow() == 0  ||
                 m_currentCellCoords.GetCol() == 0 )
            {
                extra = 2;
            }
            else
            {
                extra = 4;
            }
#else
            if ( m_currentCellCoords.GetRow() == 0  ||
                 m_currentCellCoords.GetCol() == 0 )
            {
                extra = 1;
            }
            else
            {
                extra = 2;
            }
#endif

#if defined(__WXGTK__)
            int top_diff = 0;
            int left_diff = 0;
            if (left != 0) left_diff++;
            if (top != 0) top_diff++;
            rect.SetLeft( left + left_diff );
            rect.SetTop( top + top_diff );
            rect.SetRight( rect.GetRight() - left_diff );
            rect.SetBottom( rect.GetBottom() - top_diff );
#else
            rect.SetLeft( wxMax(0, left - extra) );
            rect.SetTop( wxMax(0, top - extra) );
            rect.SetRight( rect.GetRight() + 2*extra );
            rect.SetBottom( rect.GetBottom() + 2*extra );
#endif

            m_cellEditCtrl->SetSize( rect );
            m_cellEditCtrl->Show( TRUE );

            switch ( m_editCtrlType )
            {
                case wxGRID_TEXTCTRL:
                    ((wxTextCtrl *) m_cellEditCtrl)->SetInsertionPointEnd();
                    break;

                case wxGRID_CHECKBOX:
                    // TODO: anything ???
                    //
                    break;

                case wxGRID_CHOICE:
                    // TODO: anything ???
                    //
                    break;

                case wxGRID_COMBOBOX:
                    // TODO: anything ???
                    //
                    break;
            }

            m_cellEditCtrl->SetFocus();
        }
    }
}


void wxGrid::HideCellEditControl()
{
    if ( IsCellEditControlEnabled() )
    {
        m_cellEditCtrl->Show( FALSE );
    }
}


void wxGrid::SetEditControlValue( const wxString& value )
{
    if ( m_table )
    {
        wxString s;
        if ( !value )
            s = GetCellValue(m_currentCellCoords);
        else
            s = value;

        if ( IsCellEditControlEnabled() )
        {
            switch ( m_editCtrlType )
            {
                case wxGRID_TEXTCTRL:
                    ((wxGridTextCtrl *)m_cellEditCtrl)->SetStartValue(s);
                    break;

                case wxGRID_CHECKBOX:
                    // TODO: implement this
                    //
                    break;

                case wxGRID_CHOICE:
                    // TODO: implement this
                    //
                    break;

                case wxGRID_COMBOBOX:
                    // TODO: implement this
                    //
                    break;
            }
        }
    }
}


void wxGrid::SaveEditControlValue()
{
    if ( m_table )
    {
        wxWindow *ctrl = (wxWindow *)NULL;

        if ( IsCellEditControlEnabled() )
        {
            ctrl = m_cellEditCtrl;
        }
        else
        {
            return;
        }

        bool valueChanged = FALSE;

        switch ( m_editCtrlType )
        {
            case wxGRID_TEXTCTRL:
                valueChanged = (((wxGridTextCtrl *)ctrl)->GetValue() !=
                                ((wxGridTextCtrl *)ctrl)->GetStartValue());
                SetCellValue( m_currentCellCoords,
                              ((wxTextCtrl *) ctrl)->GetValue() );
                break;

            case wxGRID_CHECKBOX:
                // TODO: implement this
                //
                break;

            case wxGRID_CHOICE:
                // TODO: implement this
                //
                break;

            case wxGRID_COMBOBOX:
                // TODO: implement this
                //
                break;
        }

        if ( valueChanged )
        {
            SendEvent( EVT_GRID_CELL_CHANGE,
                       m_currentCellCoords.GetRow(),
                       m_currentCellCoords.GetCol() );
        }
    }
}


//
// ------ Grid location functions
//  Note that all of these functions work with the logical coordinates of
//  grid cells and labels so you will need to convert from device
//  coordinates for mouse events etc.
//

void wxGrid::XYToCell( int x, int y, wxGridCellCoords& coords )
{
    int row = YToRow(y);
    int col = XToCol(x);

    if ( row == -1  ||  col == -1 )
    {
        coords = wxGridNoCellCoords;
    }
    else
    {
        coords.Set( row, col );
    }
}


int wxGrid::YToRow( int y )
{
    int i;

    for ( i = 0;  i < m_numRows;  i++ )
    {
        if ( y < m_rowBottoms[i] ) return i;
    }

    return -1;
}


int wxGrid::XToCol( int x )
{
    int i;

    for ( i = 0;  i < m_numCols;  i++ )
    {
        if ( x < m_colRights[i] ) return i;
    }

    return -1;
}


// return the row number that that the y coord is near the edge of, or
// -1 if not near an edge
//
int wxGrid::YToEdgeOfRow( int y )
{
    int i, d;

    for ( i = 0;  i < m_numRows;  i++ )
    {
        if ( m_rowHeights[i] > WXGRID_LABEL_EDGE_ZONE )
        {
            d = abs( y - m_rowBottoms[i] );
            {
                if ( d < WXGRID_LABEL_EDGE_ZONE ) return i;
            }
        }
    }

    return -1;
}


// return the col number that that the x coord is near the edge of, or
// -1 if not near an edge
//
int wxGrid::XToEdgeOfCol( int x )
{
    int i, d;

    for ( i = 0;  i < m_numCols;  i++ )
    {
        if ( m_colWidths[i] > WXGRID_LABEL_EDGE_ZONE )
        {
            d = abs( x - m_colRights[i] );
            {
                if ( d < WXGRID_LABEL_EDGE_ZONE ) return i;
            }
        }
    }

    return -1;
}


wxRect wxGrid::CellToRect( int row, int col )
{
    wxRect rect( -1, -1, -1, -1 );

    if ( row >= 0  &&  row < m_numRows  &&
         col >= 0  &&  col < m_numCols )
    {
        rect.x = m_colRights[col] - m_colWidths[col];
        rect.y = m_rowBottoms[row] - m_rowHeights[row];
        rect.width = m_colWidths[col];
        rect.height = m_rowHeights[ row ];
    }

    return rect;
}


bool wxGrid::IsVisible( int row, int col, bool wholeCellVisible )
{
    // get the cell rectangle in logical coords
    //
    wxRect r( CellToRect( row, col ) );

    // convert to device coords
    //
    int left, top, right, bottom;
    CalcScrolledPosition( r.GetLeft(), r.GetTop(), &left, &top );
    CalcScrolledPosition( r.GetRight(), r.GetBottom(), &right, &bottom );

    // check against the client area of the grid window
    //
    int cw, ch;
    m_gridWin->GetClientSize( &cw, &ch );

    if ( wholeCellVisible )
    {
        // is the cell wholly visible ?
        //
        return ( left >= 0  &&  right <= cw  &&
                 top >= 0  &&  bottom <= ch );
    }
    else
    {
        // is the cell partly visible ?
        //
        return ( ((left >=0 && left < cw) || (right > 0 && right <= cw))  &&
                 ((top >=0 && top < ch) || (bottom > 0 && bottom <= ch)) );
    }
}


// make the specified cell location visible by doing a minimal amount
// of scrolling
//
void wxGrid::MakeCellVisible( int row, int col )
{
    int i;
    int xpos = -1, ypos = -1;

    if ( row >= 0  &&  row < m_numRows  &&
         col >= 0  &&  col < m_numCols )
    {
        // get the cell rectangle in logical coords
        //
        wxRect r( CellToRect( row, col ) );

        // convert to device coords
        //
        int left, top, right, bottom;
        CalcScrolledPosition( r.GetLeft(), r.GetTop(), &left, &top );
        CalcScrolledPosition( r.GetRight(), r.GetBottom(), &right, &bottom );

        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );

        if ( top < 0 )
        {
            ypos = r.GetTop();
        }
        else if ( bottom > ch )
        {
            int h = r.GetHeight();
            ypos = r.GetTop();
            for ( i = row-1;  i >= 0;  i-- )
            {
                if ( h + m_rowHeights[i] > ch ) break;

                h += m_rowHeights[i];
                ypos -= m_rowHeights[i];
            }

            // we divide it later by GRID_SCROLL_LINE, make sure that we don't
            // have rounding errors (this is important, because if we do, we
            // might not scroll at all and some cells won't be redrawn)
            ypos += GRID_SCROLL_LINE / 2;
        }

        if ( left < 0 )
        {
            xpos = r.GetLeft();
        }
        else if ( right > cw )
        {
            int w = r.GetWidth();
            xpos = r.GetLeft();
            for ( i = col-1;  i >= 0;  i-- )
            {
                if ( w + m_colWidths[i] > cw ) break;

                w += m_colWidths[i];
                xpos -= m_colWidths[i];
            }

            // see comment for ypos above
            xpos += GRID_SCROLL_LINE / 2;
        }

        if ( xpos != -1  ||  ypos != -1 )
        {
            if ( xpos != -1 ) xpos /= GRID_SCROLL_LINE;
            if ( ypos != -1 ) ypos /= GRID_SCROLL_LINE;
            Scroll( xpos, ypos );
            AdjustScrollbars();
        }
    }
}


//
// ------ Grid cursor movement functions
//

bool wxGrid::MoveCursorUp()
{
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetRow() > 0 )
    {
        MakeCellVisible( m_currentCellCoords.GetRow() - 1,
                        m_currentCellCoords.GetCol() );

        SetCurrentCell( m_currentCellCoords.GetRow() - 1,
                        m_currentCellCoords.GetCol() );

        return TRUE;
    }

    return FALSE;
}


bool wxGrid::MoveCursorDown()
{
    // TODO: allow for scrolling
    //
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetRow() < m_numRows-1 )
    {
        MakeCellVisible( m_currentCellCoords.GetRow() + 1,
                        m_currentCellCoords.GetCol() );

        SetCurrentCell( m_currentCellCoords.GetRow() + 1,
                        m_currentCellCoords.GetCol() );

        return TRUE;
    }

    return FALSE;
}


bool wxGrid::MoveCursorLeft()
{
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetCol() > 0 )
    {
        MakeCellVisible( m_currentCellCoords.GetRow(),
                        m_currentCellCoords.GetCol() - 1 );

        SetCurrentCell( m_currentCellCoords.GetRow(),
                        m_currentCellCoords.GetCol() - 1 );

        return TRUE;
    }

    return FALSE;
}


bool wxGrid::MoveCursorRight()
{
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetCol() < m_numCols - 1 )
    {
        MakeCellVisible( m_currentCellCoords.GetRow(),
                        m_currentCellCoords.GetCol() + 1 );

        SetCurrentCell( m_currentCellCoords.GetRow(),
                        m_currentCellCoords.GetCol() + 1 );

        return TRUE;
    }

    return FALSE;
}


bool wxGrid::MovePageUp()
{
    if ( m_currentCellCoords == wxGridNoCellCoords ) return FALSE;

    int row = m_currentCellCoords.GetRow();
    if ( row > 0 )
    {
        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );

        int y = m_rowBottoms[ row ] - m_rowHeights[ row ];
        int newRow = YToRow( y - ch + 1 );
        if ( newRow == -1 )
        {
            newRow = 0;
        }
        else if ( newRow == row )
        {
            newRow = row - 1;
        }

        MakeCellVisible( newRow, m_currentCellCoords.GetCol() );
        SetCurrentCell( newRow, m_currentCellCoords.GetCol() );

        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MovePageDown()
{
    if ( m_currentCellCoords == wxGridNoCellCoords ) return FALSE;

    int row = m_currentCellCoords.GetRow();
    if ( row < m_numRows )
    {
        int cw, ch;
        m_gridWin->GetClientSize( &cw, &ch );

        int y = m_rowBottoms[ row ] - m_rowHeights[ row ];
        int newRow = YToRow( y + ch );
        if ( newRow == -1 )
        {
            newRow = m_numRows - 1;
        }
        else if ( newRow == row )
        {
            newRow = row + 1;
        }

        MakeCellVisible( newRow, m_currentCellCoords.GetCol() );
        SetCurrentCell( newRow, m_currentCellCoords.GetCol() );

        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MoveCursorUpBlock()
{
    if ( m_table &&
         m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetRow() > 0 )
    {
        int row = m_currentCellCoords.GetRow();
        int col = m_currentCellCoords.GetCol();

        if ( m_table->IsEmptyCell(row, col) )
        {
            // starting in an empty cell: find the next block of
            // non-empty cells
            //
            while ( row > 0 )
            {
                row-- ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else if ( m_table->IsEmptyCell(row-1, col) )
        {
            // starting at the top of a block: find the next block
            //
            row--;
            while ( row > 0 )
            {
                row-- ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else
        {
            // starting within a block: find the top of the block
            //
            while ( row > 0 )
            {
                row-- ;
                if ( m_table->IsEmptyCell(row, col) )
                {
                    row++ ;
                    break;
                }
            }
        }

        MakeCellVisible( row, col );
        SetCurrentCell( row, col );

        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MoveCursorDownBlock()
{
    if ( m_table &&
         m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetRow() < m_numRows-1 )
    {
        int row = m_currentCellCoords.GetRow();
        int col = m_currentCellCoords.GetCol();

        if ( m_table->IsEmptyCell(row, col) )
        {
            // starting in an empty cell: find the next block of
            // non-empty cells
            //
            while ( row < m_numRows-1 )
            {
                row++ ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else if ( m_table->IsEmptyCell(row+1, col) )
        {
            // starting at the bottom of a block: find the next block
            //
            row++;
            while ( row < m_numRows-1 )
            {
                row++ ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else
        {
            // starting within a block: find the bottom of the block
            //
            while ( row < m_numRows-1 )
            {
                row++ ;
                if ( m_table->IsEmptyCell(row, col) )
                {
                    row-- ;
                    break;
                }
            }
        }

        MakeCellVisible( row, col );
        SetCurrentCell( row, col );

        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MoveCursorLeftBlock()
{
    if ( m_table &&
         m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetCol() > 0 )
    {
        int row = m_currentCellCoords.GetRow();
        int col = m_currentCellCoords.GetCol();

        if ( m_table->IsEmptyCell(row, col) )
        {
            // starting in an empty cell: find the next block of
            // non-empty cells
            //
            while ( col > 0 )
            {
                col-- ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else if ( m_table->IsEmptyCell(row, col-1) )
        {
            // starting at the left of a block: find the next block
            //
            col--;
            while ( col > 0 )
            {
                col-- ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else
        {
            // starting within a block: find the left of the block
            //
            while ( col > 0 )
            {
                col-- ;
                if ( m_table->IsEmptyCell(row, col) )
                {
                    col++ ;
                    break;
                }
            }
        }

        MakeCellVisible( row, col );
        SetCurrentCell( row, col );

        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MoveCursorRightBlock()
{
    if ( m_table &&
         m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetCol() < m_numCols-1 )
    {
        int row = m_currentCellCoords.GetRow();
        int col = m_currentCellCoords.GetCol();

        if ( m_table->IsEmptyCell(row, col) )
        {
            // starting in an empty cell: find the next block of
            // non-empty cells
            //
            while ( col < m_numCols-1 )
            {
                col++ ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else if ( m_table->IsEmptyCell(row, col+1) )
        {
            // starting at the right of a block: find the next block
            //
            col++;
            while ( col < m_numCols-1 )
            {
                col++ ;
                if ( !(m_table->IsEmptyCell(row, col)) ) break;
            }
        }
        else
        {
            // starting within a block: find the right of the block
            //
            while ( col < m_numCols-1 )
            {
                col++ ;
                if ( m_table->IsEmptyCell(row, col) )
                {
                    col-- ;
                    break;
                }
            }
        }

        MakeCellVisible( row, col );
        SetCurrentCell( row, col );

        return TRUE;
    }

    return FALSE;
}



//
// ------ Label values and formatting
//

void wxGrid::GetRowLabelAlignment( int *horiz, int *vert )
{
    *horiz = m_rowLabelHorizAlign;
    *vert  = m_rowLabelVertAlign;
}

void wxGrid::GetColLabelAlignment( int *horiz, int *vert )
{
    *horiz = m_colLabelHorizAlign;
    *vert  = m_colLabelVertAlign;
}

wxString wxGrid::GetRowLabelValue( int row )
{
    if ( m_table )
    {
        return m_table->GetRowLabelValue( row );
    }
    else
    {
        wxString s;
        s << row;
        return s;
    }
}

wxString wxGrid::GetColLabelValue( int col )
{
    if ( m_table )
    {
        return m_table->GetColLabelValue( col );
    }
    else
    {
        wxString s;
        s << col;
        return s;
    }
}

void wxGrid::SetRowLabelSize( int width )
{
    // TODO: how to do this with the box sizers ?
}

void wxGrid::SetColLabelSize( int height )
{
    // TODO: how to do this with the box sizers ?
}

void wxGrid::SetLabelBackgroundColour( const wxColour& colour )
{
    if ( m_labelBackgroundColour != colour )
    {
        m_labelBackgroundColour = colour;
        m_rowLabelWin->SetBackgroundColour( colour );
        m_colLabelWin->SetBackgroundColour( colour );
        m_cornerLabelWin->SetBackgroundColour( colour );

        if ( !GetBatchCount() )
        {
            m_rowLabelWin->Refresh();
            m_colLabelWin->Refresh();
            m_cornerLabelWin->Refresh();
        }
    }
}

void wxGrid::SetLabelTextColour( const wxColour& colour )
{
    if ( m_labelTextColour != colour )
    {
        m_labelTextColour = colour;
        if ( !GetBatchCount() )
        {
            m_rowLabelWin->Refresh();
            m_colLabelWin->Refresh();
        }
    }
}

void wxGrid::SetLabelFont( const wxFont& font )
{
    m_labelFont = font;
    if ( !GetBatchCount() )
    {
        m_rowLabelWin->Refresh();
        m_colLabelWin->Refresh();
    }
}

void wxGrid::SetRowLabelAlignment( int horiz, int vert )
{
    if ( horiz == wxLEFT || horiz == wxCENTRE || horiz == wxRIGHT )
    {
        m_rowLabelHorizAlign = horiz;
    }

    if ( vert == wxTOP || vert == wxCENTRE || vert == wxBOTTOM )
    {
        m_rowLabelVertAlign = vert;
    }

    if ( !GetBatchCount() )
    {
        m_rowLabelWin->Refresh();
        m_colLabelWin->Refresh();
    }
}

void wxGrid::SetColLabelAlignment( int horiz, int vert )
{
    if ( horiz == wxLEFT || horiz == wxCENTRE || horiz == wxRIGHT )
    {
        m_colLabelHorizAlign = horiz;
    }

    if ( vert == wxTOP || vert == wxCENTRE || vert == wxBOTTOM )
    {
        m_colLabelVertAlign = vert;
    }

    if ( !GetBatchCount() )
    {
        m_rowLabelWin->Refresh();
        m_colLabelWin->Refresh();
    }
}

void wxGrid::SetRowLabelValue( int row, const wxString& s )
{
    if ( m_table )
    {
        m_table->SetRowLabelValue( row, s );
        if ( !GetBatchCount() )
        {
            // TODO: Optimize this
            //
            m_rowLabelWin->Refresh();
        }
    }
}

void wxGrid::SetColLabelValue( int col, const wxString& s )
{
    if ( m_table )
    {
        m_table->SetColLabelValue( col, s );
        if ( !GetBatchCount() )
        {
            // TODO: optimize this
            //
            m_colLabelWin->Refresh();
        }
    }
}

void wxGrid::SetGridLineColour( const wxColour& colour )
{
    if ( m_gridLineColour != colour )
    {
        m_gridLineColour = colour;

        wxClientDC dc( m_gridWin );
        PrepareDC( dc );
        DrawAllGridLines( dc );
    }
}

void wxGrid::EnableGridLines( bool enable )
{
    if ( enable != m_gridLinesEnabled )
    {
        m_gridLinesEnabled = enable;

        if ( !GetBatchCount() )
        {
            if ( enable )
            {
                wxClientDC dc( m_gridWin );
                PrepareDC( dc );
                DrawAllGridLines( dc );
            }
            else
            {
                m_gridWin->Refresh();
            }
        }
    }
}


int wxGrid::GetDefaultRowSize()
{
    return m_defaultRowHeight;
}

int wxGrid::GetRowSize( int row )
{
    if ( row >= 0  &&  row < m_numRows )
        return m_rowHeights[row];
    else
        return 0;  // TODO: log an error here
}

int wxGrid::GetDefaultColSize()
{
    return m_defaultColWidth;
}

int wxGrid::GetColSize( int col )
{
    if ( col >= 0  &&  col < m_numCols )
        return m_colWidths[col];
    else
        return 0;  // TODO: log an error here
}

wxColour wxGrid::GetDefaultCellBackgroundColour()
{
    // TODO: replace this temp test code
    //
    return wxColour( 255, 255, 255 );
}

wxColour wxGrid::GetCellBackgroundColour( int WXUNUSED(row), int WXUNUSED(col) )
{
    // TODO: replace this temp test code
    //
    return wxColour( 255, 255, 255 );
}

wxColour wxGrid::GetDefaultCellTextColour()
{
    // TODO: replace this temp test code
    //
    return wxColour( 0, 0, 0 );
}

wxColour wxGrid::GetCellTextColour( int WXUNUSED(row), int WXUNUSED(col) )
{
    // TODO: replace this temp test code
    //
    return wxColour( 0, 0, 0 );
}


wxFont wxGrid::GetDefaultCellFont()
{
    return m_defaultCellFont;
}

wxFont wxGrid::GetCellFont( int WXUNUSED(row), int WXUNUSED(col) )
{
    // TODO: replace this temp test code
    //
    return m_defaultCellFont;
}

void wxGrid::GetDefaultCellAlignment( int *horiz, int *vert )
{
    // TODO: replace this temp test code
    //
    *horiz = wxLEFT;
    *vert  = wxTOP;
}

void wxGrid::GetCellAlignment( int WXUNUSED(row), int WXUNUSED(col), int *horiz, int *vert )
{
    // TODO: replace this temp test code
    //
    *horiz = wxLEFT;
    *vert  = wxTOP;
}

void wxGrid::SetDefaultRowSize( int height, bool resizeExistingRows )
{
    m_defaultRowHeight = wxMax( height, WXGRID_MIN_ROW_HEIGHT );

    if ( resizeExistingRows )
    {
        int row;
        int bottom = 0;
        for ( row = 0;  row < m_numRows;  row++ )
        {
            m_rowHeights[row] = m_defaultRowHeight;
            bottom += m_defaultRowHeight;
            m_rowBottoms[row] = bottom;
        }
        CalcDimensions();
    }
}

void wxGrid::SetRowSize( int row, int height )
{
    int i;

    if ( row >= 0  &&  row < m_numRows )
    {
        int h = wxMax( 0, height );
        int diff = h - m_rowHeights[row];

        m_rowHeights[row] = h;
        for ( i = row;  i < m_numRows;  i++ )
        {
            m_rowBottoms[i] += diff;
        }
        CalcDimensions();

        // Note: we are ending the event *after* doing
        // default processing in this case
        //
        SendEvent( EVT_GRID_ROW_SIZE,
                   row, -1 );
    }
    else
    {
        // TODO: log an error here
    }
}

void wxGrid::SetDefaultColSize( int width, bool resizeExistingCols )
{
    m_defaultColWidth = wxMax( width, WXGRID_MIN_COL_WIDTH );

    if ( resizeExistingCols )
    {
        int col;
        int right = 0;
        for ( col = 0;  col < m_numCols;  col++ )
        {
            m_colWidths[col] = m_defaultColWidth;
            right += m_defaultColWidth;
            m_colRights[col] = right;
        }
        CalcDimensions();
    }
}

void wxGrid::SetColSize( int col, int width )
{
    int i;

    if ( col >= 0  &&  col < m_numCols )
    {
        int w = wxMax( 0, width );
        int diff = w - m_colWidths[col];
        m_colWidths[col] = w;

        for ( i = col;  i < m_numCols;  i++ )
        {
            m_colRights[i] += diff;
        }
        CalcDimensions();

        // Note: we are ending the event *after* doing
        // default processing in this case
        //
        SendEvent( EVT_GRID_COL_SIZE,
                   -1, col );
    }
    else
    {
        // TODO: log an error here
    }
}

void wxGrid::SetDefaultCellBackgroundColour( const wxColour& )
{
    // TODO: everything !!!
    //
}

void wxGrid::SetCellBackgroundColour( int WXUNUSED(row), int WXUNUSED(col), const wxColour& )
{
    // TODO: everything !!!
    //
}

void wxGrid::SetDefaultCellTextColour( const wxColour& )
{
    // TODO: everything !!!
    //
}

void wxGrid::SetCellTextColour( int WXUNUSED(row), int WXUNUSED(col), const wxColour& )
{
    // TODO: everything !!!
    //
}

void wxGrid::SetDefaultCellFont( const wxFont& )
{
    // TODO: everything !!!
    //
}

void wxGrid::SetCellFont( int WXUNUSED(row), int WXUNUSED(col), const wxFont& )
{
    // TODO: everything !!!
    //
}

void wxGrid::SetDefaultCellAlignment( int WXUNUSED(horiz), int WXUNUSED(vert) )
{
    // TODO: everything !!!
    //
}

void wxGrid::SetCellAlignment( int WXUNUSED(row), int WXUNUSED(col), int WXUNUSED(horiz), int WXUNUSED(vert) )
{
    // TODO: everything !!!
    //
}



//
// ------ cell value accessor functions
//

void wxGrid::SetCellValue( int row, int col, const wxString& s )
{
    if ( m_table )
    {
        m_table->SetValue( row, col, s.c_str() );
        if ( !GetBatchCount() )
        {
            wxClientDC dc( m_gridWin );
            PrepareDC( dc );
            DrawCell( dc, wxGridCellCoords(row, col) );
        }

#if 0  // TODO: edit in place

        if ( m_currentCellCoords.GetRow() == row &&
             m_currentCellCoords.GetCol() == col )
        {
            SetEditControlValue( s );
        }
#endif

    }
}


//
// ------ Block, row and col selection
//

void wxGrid::SelectRow( int row, bool addToSelected )
{
    wxRect r;

    if ( IsSelection() && addToSelected )
    {
        if ( m_selectedTopLeft.GetRow() > row )
            m_selectedTopLeft.SetRow( row );

        m_selectedTopLeft.SetCol( 0 );

        if ( m_selectedBottomRight.GetRow() < row )
            m_selectedBottomRight.SetRow( row );

        m_selectedBottomRight.SetCol( m_numCols - 1 );

        // TODO: optimize this so that we only refresh the newly
        // selected cells
        //
        r = SelectionToDeviceRect();
        if ( r != wxGridNoCellRect ) m_gridWin->Refresh( TRUE, &r );
    }
    else
    {
        r = SelectionToDeviceRect();
        ClearSelection();
        if ( r != wxGridNoCellRect ) m_gridWin->Refresh( TRUE, &r );

        m_selectedTopLeft.Set( row, 0 );
        m_selectedBottomRight.Set( row, m_numCols-1 );
        r = SelectionToDeviceRect();
        m_gridWin->Refresh( TRUE, &r );
    }

    wxGridRangeSelectEvent gridEvt( GetId(),
                                    EVT_GRID_RANGE_SELECT,
                                    this,
                                    m_selectedTopLeft,
                                    m_selectedBottomRight );

    GetEventHandler()->ProcessEvent(gridEvt);
}


void wxGrid::SelectCol( int col, bool addToSelected )
{
    wxRect r;

    if ( IsSelection() && addToSelected )
    {
        if ( m_selectedTopLeft.GetCol() > col )
            m_selectedTopLeft.SetCol( col );

        m_selectedTopLeft.SetRow( 0 );

        if ( m_selectedBottomRight.GetCol() < col )
            m_selectedBottomRight.SetCol( col );

        m_selectedBottomRight.SetRow( m_numRows - 1 );

        // TODO: optimize this so that we only refresh the newly
        // selected cells
        //
        r = SelectionToDeviceRect();
        if ( r != wxGridNoCellRect ) m_gridWin->Refresh( TRUE, &r );
    }
    else
    {
        r = SelectionToDeviceRect();
        ClearSelection();
        if ( r != wxGridNoCellRect ) m_gridWin->Refresh( TRUE, &r );

        m_selectedTopLeft.Set( 0, col );
        m_selectedBottomRight.Set( m_numRows-1, col );
        r = SelectionToDeviceRect();
        m_gridWin->Refresh( TRUE, &r );
    }

    wxGridRangeSelectEvent gridEvt( GetId(),
                                    EVT_GRID_RANGE_SELECT,
                                    this,
                                    m_selectedTopLeft,
                                    m_selectedBottomRight );

    GetEventHandler()->ProcessEvent(gridEvt);
}


void wxGrid::SelectBlock( int topRow, int leftCol, int bottomRow, int rightCol )
{
    int temp;
    bool changed = false;
    wxGridCellCoords updateTopLeft, updateBottomRight;

    if ( topRow > bottomRow )
    {
        temp = topRow;
        topRow = bottomRow;
        bottomRow = temp;
    }

    if ( leftCol > rightCol )
    {
        temp = leftCol;
        leftCol = rightCol;
        rightCol = temp;
    }

    updateTopLeft = m_selectedTopLeft;
    if (m_selectedTopLeft != wxGridCellCoords( topRow, leftCol ) )
    {
        m_selectedTopLeft = wxGridCellCoords( topRow, leftCol );
        if (updateTopLeft == wxGridNoCellCoords)
        {
            updateTopLeft = m_selectedTopLeft;
        }
        else
        {
             if(updateTopLeft.GetRow() > topRow)
                updateTopLeft.SetRow(topRow);
            if (updateTopLeft.GetCol() > leftCol)
              updateTopLeft.SetCol(leftCol);
        }
        changed = true;
    }

    updateBottomRight = m_selectedBottomRight;
    if (m_selectedBottomRight != wxGridCellCoords( bottomRow, rightCol ) )
    {
        m_selectedBottomRight = wxGridCellCoords( bottomRow, rightCol );
        if (updateBottomRight == wxGridNoCellCoords)
        {
            updateBottomRight = m_selectedBottomRight;
        }
        else
        {
            if (updateBottomRight.GetRow() < bottomRow)
                updateBottomRight.SetRow(bottomRow);
            if (updateBottomRight.GetCol() < rightCol)
                updateBottomRight.SetCol(rightCol);
        }
        changed = true;
    }

    if (changed)
    {
        wxRect r( BlockToDeviceRect( updateTopLeft, updateBottomRight ) );
        m_gridWin->Refresh( TRUE, &r );
    }

    // only generate an event if the block is not being selected by
    // dragging the mouse (in which case the event will be generated in
    // the mouse event handler)
    if ( !m_isDragging )
    {
        wxGridRangeSelectEvent gridEvt( GetId(),
                                        EVT_GRID_RANGE_SELECT,
                                        this,
                                        m_selectedTopLeft,
                                        m_selectedBottomRight );

        GetEventHandler()->ProcessEvent(gridEvt);
    }
}

void wxGrid::SelectAll()
{
    m_selectedTopLeft.Set( 0, 0 );
    m_selectedBottomRight.Set( m_numRows-1, m_numCols-1 );

    m_gridWin->Refresh();
}


void wxGrid::ClearSelection()
{
    m_selectedTopLeft = wxGridNoCellCoords;
    m_selectedBottomRight = wxGridNoCellCoords;
}


// This function returns the rectangle that encloses the given block
// in device coords clipped to the client size of the grid window.
//
wxRect wxGrid::BlockToDeviceRect( const wxGridCellCoords &topLeft,
                                  const wxGridCellCoords &bottomRight )
{
    wxRect rect( wxGridNoCellRect );
    wxRect cellRect;

    cellRect = CellToRect( topLeft );
    if ( cellRect != wxGridNoCellRect )
    {
        rect = cellRect;
    }
    else
    {
        rect = wxRect( 0, 0, 0, 0 );
    }

    cellRect = CellToRect( bottomRight );
    if ( cellRect != wxGridNoCellRect )
    {
        rect += cellRect;
    }
    else
    {
        return wxGridNoCellRect;
    }

    // convert to scrolled coords
    //
    int left, top, right, bottom;
    CalcScrolledPosition( rect.GetLeft(), rect.GetTop(), &left, &top );
    CalcScrolledPosition( rect.GetRight(), rect.GetBottom(), &right, &bottom );

    int cw, ch;
    m_gridWin->GetClientSize( &cw, &ch );

    rect.SetLeft( wxMax(0, left) );
    rect.SetTop( wxMax(0, top) );
    rect.SetRight( wxMin(cw, right) );
    rect.SetBottom( wxMin(ch, bottom) );

    return rect;
}



//
// ------ Grid event classes
//

IMPLEMENT_DYNAMIC_CLASS( wxGridEvent, wxEvent )

wxGridEvent::wxGridEvent( int id, wxEventType type, wxObject* obj,
                          int row, int col, int x, int y,
                          bool control, bool shift, bool alt, bool meta )
        : wxNotifyEvent( type, id )
{
    m_row = row;
    m_col = col;
    m_x = x;
    m_y = y;
    m_control = control;
    m_shift = shift;
    m_alt = alt;
    m_meta = meta;

    SetEventObject(obj);
}


IMPLEMENT_DYNAMIC_CLASS( wxGridSizeEvent, wxEvent )

wxGridSizeEvent::wxGridSizeEvent( int id, wxEventType type, wxObject* obj,
                                  int rowOrCol, int x, int y,
                                  bool control, bool shift, bool alt, bool meta )
        : wxNotifyEvent( type, id )
{
    m_rowOrCol = rowOrCol;
    m_x = x;
    m_y = y;
    m_control = control;
    m_shift = shift;
    m_alt = alt;
    m_meta = meta;

    SetEventObject(obj);
}


IMPLEMENT_DYNAMIC_CLASS( wxGridRangeSelectEvent, wxEvent )

wxGridRangeSelectEvent::wxGridRangeSelectEvent(int id, wxEventType type, wxObject* obj,
                                               const wxGridCellCoords& topLeft,
                                               const wxGridCellCoords& bottomRight,
                                               bool control, bool shift, bool alt, bool meta )
        : wxNotifyEvent( type, id )
{
    m_topLeft     = topLeft;
    m_bottomRight = bottomRight;
    m_control     = control;
    m_shift       = shift;
    m_alt         = alt;
    m_meta        = meta;

    SetEventObject(obj);
}


#endif // ifndef wxUSE_NEW_GRID

