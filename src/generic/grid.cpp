////////////////////////////////////////////////////////////////////////////
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
#endif

#include "wx/generic/grid.h"


//////////////////////////////////////////////////////////////////////

wxGridCellCoords wxGridNoCellCoords( -1, -1 );
wxRect           wxGridNoCellRect( -1, -1, -1, -1 );



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
    wxLogWarning( "Called grid table class function InsertRows(pos=%d, N=%d)\n"
                  "but your derived table class does not override this function",
                  pos, numRows );
    
    return FALSE;
}

bool wxGridTableBase::AppendRows( size_t numRows )
{
    wxLogWarning( "Called grid table class function AppendRows(N=%d)\n"
                  "but your derived table class does not override this function",
                  numRows );
    
    return FALSE;
}

bool wxGridTableBase::DeleteRows( size_t pos, size_t numRows )
{
    wxLogWarning( "Called grid table class function DeleteRows(pos=%d, N=%d)\n"
                  "but your derived table class does not override this function",
                  pos, numRows );
    
    return FALSE;
}

bool wxGridTableBase::InsertCols( size_t pos, size_t numCols )
{
    wxLogWarning( "Called grid table class function InsertCols(pos=%d, N=%d)\n"
                  "but your derived table class does not override this function",
                  pos, numCols );
    
    return FALSE;
}

bool wxGridTableBase::AppendCols( size_t numCols )
{
    wxLogWarning( "Called grid table class function AppendCols(N=%d)\n"
                  "but your derived table class does not override this function",
                  numCols );
    
    return FALSE;
}

bool wxGridTableBase::DeleteCols( size_t pos, size_t numCols )
{
    wxLogWarning( "Called grid table class function DeleteCols(pos=%d, N=%d)\n"
                  "but your derived table class does not override this function",
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
        s += ('A' + (char)( col%26 ));
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


// this is a magic incantation which must be done!
#include <wx/arrimpl.cpp>

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
        wxLogError( "Called wxGridStringTable::DeleteRows(pos=%d, N=%d)...\n"
                    "Pos value is invalid for present table with %d rows",
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
        wxLogError( "Unable to append cols to a grid table with no rows.\n"
                    "Call AppendRows() first" );
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
	wxLogError( "Called wxGridStringTable::DeleteCols(pos=%d, N=%d)...\n"
		    "Pos value is invalid for present table with %d cols",
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
                                bool isCellControl,
                                wxWindowID id,
                                const wxString& value,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style )
        : wxTextCtrl( par, id, value, pos, size, style )
{
    m_isCellControl = isCellControl;
}


void wxGridTextCtrl::OnKeyDown( wxKeyEvent& ev )
{
    switch ( ev.KeyCode() )
    {
        case WXK_ESCAPE:
            ((wxGrid *)GetParent())->SetEditControlValue( startValue );
            SetInsertionPointEnd();
            break;

        case WXK_UP:
        case WXK_DOWN:
        case WXK_LEFT:
        case WXK_RIGHT:
        case WXK_PRIOR:
        case WXK_NEXT:
        case WXK_RETURN:
            if ( m_isCellControl )
            {
                // send the event to the parent grid, skipping the
                // event if nothing happens
                //
                ev.Skip( !GetParent()->ProcessEvent( ev ) );
            }
            else
            {
                // default text control response within the top edit
                // control
                //
                ev.Skip();
            }
            break;

        case WXK_HOME:
        case WXK_END:
            if ( m_isCellControl )
            {
                // send the event to the parent grid, skipping the
                // event if nothing happens
                //
                ev.Skip( !GetParent()->ProcessEvent( ev ) );
            }
            else
            {
                // default text control response within the top edit
                // control
                //
                ev.Skip();
            }
            break;
            
        default:
            ev.Skip();
    }
}

void wxGridTextCtrl::SetStartValue( const wxString& s )
{
    startValue = s;
    wxTextCtrl::SetValue( s.c_str() );
}


//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS( wxGrid, wxPanel )

    
BEGIN_EVENT_TABLE( wxGrid, wxPanel )
    EVT_PAINT( wxGrid::OnPaint )
    EVT_SIZE( wxGrid::OnSize )
    EVT_MOUSE_EVENTS( wxGrid::OnMouse )
    EVT_KEY_DOWN( wxGrid::OnKeyDown )
    EVT_TEXT( wxGRID_CELLCTRL, wxGrid::OnText )
    EVT_TEXT( wxGRID_TOPCTRL, wxGrid::OnText )
    EVT_COMMAND_SCROLL( wxGRID_HORIZSCROLL, wxGrid::OnGridScroll)
    EVT_COMMAND_SCROLL( wxGRID_VERTSCROLL, wxGrid::OnGridScroll)
END_EVENT_TABLE()
    

wxGrid::~wxGrid()
{
    delete m_table;
}


//
// ----- internal init and update functions
//

void wxGrid::Create()
{
    m_table          = (wxGridTableBase *) NULL;
    m_topEditCtrl    = (wxWindow *) NULL;
    m_cellEditCtrl   = (wxWindow *) NULL;
    m_horizScrollBar = (wxScrollBar *) NULL;
    m_vertScrollBar  = (wxScrollBar *) NULL;

    m_numRows = 0;
    m_numCols = 0;
    m_created = FALSE;
}

void wxGrid::Init()
{
    int i;

    m_left = 0;
    m_top  = 0;

    // TODO: perhaps have a style flag for control panel
    //
    m_topEditCtrlEnabled = FALSE;
    m_topEditCtrl = new wxGridTextCtrl( this,
					FALSE,
					wxGRID_TOPCTRL,
					"",
					wxPoint(10, 10),
					wxSize(WXGRID_DEFAULT_TOPEDIT_WIDTH,
                                               WXGRID_DEFAULT_TOPEDIT_HEIGHT),
					wxTE_MULTILINE );
    m_topEditCtrl->Show( FALSE );					

    if ( m_numRows <= 0 )
        m_numRows = WXGRID_DEFAULT_NUMBER_ROWS;

    if ( m_numCols <= 0 )
        m_numCols = WXGRID_DEFAULT_NUMBER_COLS;

    m_rowLabelWidth  = WXGRID_DEFAULT_ROW_LABEL_WIDTH;
    m_colLabelHeight = WXGRID_DEFAULT_COL_LABEL_HEIGHT;

    // default labels are pale grey with black text
    //
    m_labelBackgroundColour = wxColour( 192, 192, 192 );
    m_labelTextColour       = wxColour(   0,   0,   0 );

    // TODO: something better than this ?
    //
    m_labelFont = this->GetFont();
    m_labelFont.SetWeight( m_labelFont.GetWeight() + 2 );
    
    m_rowLabelHorizAlign = wxLEFT;
    m_rowLabelVertAlign  = wxCENTRE;

    m_colLabelHorizAlign = wxCENTRE;
    m_colLabelVertAlign  = wxTOP;

    m_defaultRowHeight = WXGRID_DEFAULT_ROW_HEIGHT;
    m_defaultColWidth  = WXGRID_DEFAULT_COL_WIDTH;

    m_rowHeights.Alloc( m_numRows );
    m_rowBottoms.Alloc( m_numRows );
    for ( i = 0;  i < m_numRows;  i++ )
    {
        m_rowHeights.Add( m_defaultRowHeight );
        m_rowBottoms.Add( 0 );  // set by CalcDimensions()
    }

    m_colWidths.Alloc( m_numCols );
    m_colRights.Alloc( m_numRows );
    for ( i = 0;  i < m_numCols;  i++ )
    {
        m_colWidths.Add( m_defaultColWidth );
        m_colRights.Add( 0 );  // set by CalcDimensions()
    }

    // TODO: improve this ?
    //
    m_defaultCellFont = this->GetFont();
    
    m_gridLineColour = wxColour(   0,   0, 255 );
    m_gridLinesEnabled = TRUE;
    
    m_scrollBarWidth = WXGRID_DEFAULT_SCROLLBAR_WIDTH;

    m_horizScrollBar = new wxScrollBar( this,
                                        wxGRID_HORIZSCROLL,
                                        wxPoint(0, 0),
                                        wxSize(10, 10),
                                        wxHORIZONTAL);
    
    m_vertScrollBar = new wxScrollBar( this,
                                       wxGRID_VERTSCROLL,
                                       wxPoint(0, 0),
                                       wxSize(10, 10),
                                       wxVERTICAL);
    m_scrollPosX = 0;
    m_scrollPosY = 0;
    m_wholeColsVisible = 0;
    m_wholeRowsVisible = 0;    

    m_firstPaint = TRUE;
    m_inOnKeyDown = FALSE;
    m_batchCount = 0;
    
    m_cursorMode  = WXGRID_CURSOR_DEFAULT;
    m_dragLastPos  = -1;
    m_dragRowOrCol = -1;
    m_isDragging = FALSE;

    m_rowResizeCursor = wxCursor( wxCURSOR_SIZENS );
    m_colResizeCursor = wxCursor( wxCURSOR_SIZEWE );

    m_currentCellCoords = wxGridNoCellCoords;
    m_currentCellHighlighted = FALSE;

    m_selectedTopLeft = wxGridNoCellCoords;
    m_selectedBottomRight = wxGridNoCellCoords;
    
    m_editable = TRUE;  // default for whole grid

    // TODO: extend this to other types of controls
    //
    m_cellEditCtrl = new wxGridTextCtrl( this,
                                         TRUE,
                                         wxGRID_CELLCTRL,
                                         "",
                                         wxPoint(1,1),
                                         wxSize(1,1)
#ifdef __WXMSW__
                                         , wxTE_MULTILINE | wxTE_NO_VSCROLL
#endif
                                         );
                                         
    m_cellEditCtrl->Show( FALSE );                                            
    m_cellEditCtrlEnabled = TRUE;
    m_editCtrlType = wxGRID_TEXTCTRL;    
    
    // This is here in case OnSize does not get called when the grid is
    // displayed
    //
    CalcDimensions();
}


void wxGrid::CalcDimensions()
{
    int i;

    if ( IsTopEditControlEnabled() )
    {
        int ctrlW, ctrlH;
        m_topEditCtrl->GetSize( &ctrlW, &ctrlH );
        
        m_top = ctrlH + 20;
    }
    else
    {
        m_top = 0;
    }

    int bottom =  m_top + m_colLabelHeight;
    for ( i = m_scrollPosY;  i < m_numRows;  i++ )
    {
        bottom += m_rowHeights[i];
        m_rowBottoms[i] = bottom;
    }

    int right = m_left + m_rowLabelWidth;
    for ( i = m_scrollPosX;  i < m_numCols;  i++ )
    {
        right += m_colWidths[i];
        m_colRights[i] = right;
    }    

    // adjust the scroll bars
    //
    
    int cw, ch;
    GetClientSize(&cw, &ch);

    // begin by assuming that we don't need either scroll bar
    //
    int vertScrollBarWidth = 0;
    int horizScrollBarHeight = 0;

    // Each scroll bar needs to eventually know if the other one is
    // required in deciding whether or not it is also required - hence
    // this loop. A bit inelegant but simple and it works.
    //
    int check;
    for ( check = 0;  check < 2;  check++ )
    {
        if ( m_numRows > 0  &&
             m_rowBottoms[m_numRows-1] + horizScrollBarHeight > ch )
        {
            vertScrollBarWidth = m_scrollBarWidth;

            m_wholeRowsVisible = 0;
            for ( i = m_scrollPosY; i < m_numRows; i++ )
            {
                // A partial row doesn't count, we still have to scroll to
                // see the rest of it
                if ( m_rowBottoms[i] + horizScrollBarHeight > ch ) break;

                m_wholeRowsVisible++ ;
            }
        }
        else  
        {
            m_wholeRowsVisible = m_numRows - m_scrollPosY;
            if ( m_scrollPosY )
            {
                vertScrollBarWidth = m_scrollBarWidth;
            }
        }
        

        if ( m_numCols  &&
             m_colRights[m_numCols-1] + vertScrollBarWidth > cw)
        {
            horizScrollBarHeight = m_scrollBarWidth;

            m_wholeColsVisible = 0;
            for ( i = m_scrollPosX; i < m_numCols; i++ )
            {
                // A partial col doesn't count, we still have to scroll to
                // see the rest of it
                if ( m_colRights[i] + vertScrollBarWidth > cw ) break;
                
                m_wholeColsVisible++ ;
            }
        }
        else
        {
            // we can see the right-most column
            //
            m_wholeColsVisible = m_numCols - m_scrollPosX;
            if ( m_scrollPosX )
            {
                horizScrollBarHeight = m_scrollBarWidth;
            }
        }
    }

    
    if ( m_vertScrollBar )
    {
        if ( !vertScrollBarWidth )
        {
            m_vertScrollBar->Show(FALSE);
        }
        else
        {
            m_vertScrollBar->Show(TRUE);
            m_vertScrollBar->SetScrollbar(
                m_scrollPosY,
                wxMax(m_wholeRowsVisible, 1),
                (m_wholeRowsVisible == 0 ? 1 : m_numRows),
                wxMax(m_wholeRowsVisible, 1) );
                                       
            m_vertScrollBar->SetSize( cw - m_scrollBarWidth,
                                      m_top,
                                      m_scrollBarWidth,
                                      ch - m_top - horizScrollBarHeight);
        }
    }
    
    if ( m_horizScrollBar )
    {
        if ( !horizScrollBarHeight )
        {
            m_horizScrollBar->Show(FALSE);
        }
        else
        {
            m_horizScrollBar->Show(TRUE);

            m_horizScrollBar->SetScrollbar(
                m_scrollPosX,
                wxMax(m_wholeColsVisible, 1),
                (m_wholeColsVisible == 0) ? 1 : m_numCols,
                wxMax(m_wholeColsVisible, 1) );

            m_horizScrollBar->SetSize( m_left,
                                       ch - m_scrollBarWidth,
                                       cw - m_left - vertScrollBarWidth,
                                       m_scrollBarWidth );
        }
    }

    m_bottom = m_right = 0;
    if ( m_numRows > 0 )
    {
        m_bottom = wxMin( m_rowBottoms[m_numRows-1],
                          ch - horizScrollBarHeight );
    }
    if ( m_numCols > 0 )
    {
        m_right = wxMin( m_colRights[m_numCols-1],
                         cw - vertScrollBarWidth );
    }
}


bool wxGrid::IsOnScreen()
{
    int cw, ch;
    GetClientSize( &cw, &ch );
    return ( cw > 10 );
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
            m_numRows += numRows;
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

            // TODO: improve these adjustments...
            //
            if ( m_scrollPosY >= m_numRows )
                m_scrollPosY = 0;

            if ( !m_numRows )
            {
                m_numCols = 0;
                m_colWidths.Clear();
                m_colRights.Clear();
                m_currentCellCoords = wxGridNoCellCoords;
            }
            else if ( m_currentCellCoords.GetRow() >= m_numRows )
            {
                m_currentCellCoords.Set( 0, 0 );
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
            m_numCols += numCols;
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
            //
            // TODO: improve these adjustments...
            //
            if ( m_scrollPosX >= m_numCols )
                m_scrollPosX = 0;

            if ( !m_numCols )
            {
#if 0  // leave the row alone here so that AppendCols will work subsequently
                m_numRows = 0;
                m_rowHeights.Clear();
                m_rowBottoms.Clear();
#endif                
                m_currentCellCoords = wxGridNoCellCoords;
            }
            else if ( m_currentCellCoords.GetCol() >= m_numCols )
            {
                m_currentCellCoords.Set( 0, 0 );
            }
            CalcDimensions();
        }
        return TRUE;
    }

    return FALSE;
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


void wxGrid::OnPaint( wxPaintEvent& WXUNUSED(ev) )
{
    wxPaintDC dc( this );

    if ( !m_batchCount )
    {
        // define a clipping region to avoid painting over the scroll bars
        //
        int vs = 0;
        if ( m_vertScrollBar && m_vertScrollBar->IsShown() )
            vs = m_scrollBarWidth;

        int hs = 0;
        if ( m_horizScrollBar && m_horizScrollBar->IsShown() )
            hs = m_scrollBarWidth;

        int cw, ch;
        GetClientSize( &cw, &ch );
        dc.SetClippingRegion( 0, 0, cw - vs, ch - hs );
            
        HideCurrentCellHighlight( dc );
        
        DrawLabelAreas( dc );
        DrawColLabels( dc );
        DrawRowLabels( dc );
        DrawCellArea( dc );
        DrawGridLines( dc );
        DrawCells( dc );

        // TODO: something more elegant than this...
        //
        if ( m_firstPaint )
        {
            if ( m_currentCellCoords == wxGridNoCellCoords )
                m_currentCellCoords.Set(0, 0);
        
            SetEditControlValue();
            ShowCellEditControl();
            m_firstPaint = FALSE;
        }

        ShowCurrentCellHighlight( dc );

        dc.DestroyClippingRegion();
    }
}


void wxGrid::OnSize( wxSizeEvent& WXUNUSED(ev) )
{
    if ( m_created ) CalcDimensions();
}


void wxGrid::OnMouse( wxMouseEvent& ev )
{
    int x = ev.GetX();
    int y = ev.GetY();
    int row, col;
    
    // ------------------------------------------------------------
    //
    // Mouse dragging
    //
    if ( ev.Dragging() )
    {
        m_isDragging = TRUE;
        
        if ( ev.LeftIsDown() )
        {
            switch( m_cursorMode )
            {
                case WXGRID_CURSOR_SELECT_CELL:
                {
                    wxGridCellCoords cellCoords;
                    XYToCell( x, y, cellCoords );
                    if ( cellCoords != wxGridNoCellCoords )
                    {
                        if ( !IsSelection() )
                        {
			    SelectBlock( cellCoords, cellCoords );
			}
			else 
			{
                            // check for the mouse being outside the cell area
                            // (we still want to let the user grow the selected block)
                            //
                            if ( cellCoords.GetCol() == -1 )
                            {
                                if ( x >= m_right )
                                    cellCoords.SetCol( m_numCols-1 );
                                else
                                    cellCoords.SetCol( m_scrollPosX );
                            }
                            
                            if ( cellCoords.GetRow() == -1 )
                            {
                                if ( y >= m_bottom )
                                    cellCoords.SetRow( m_numRows-1 );
                                else
                                    cellCoords.SetRow( m_scrollPosY );
                            }
                            
                            if ( !IsInSelection( cellCoords ) )
			        SelectBlock( m_currentCellCoords, cellCoords );
			}
                    }
                }
                break;

                case WXGRID_CURSOR_RESIZE_ROW:
                {
                    wxClientDC dc(this);
                    dc.SetLogicalFunction(wxXOR);
                    if ( m_dragLastPos >= 0 )
                    {
                        dc.DrawLine( m_left, m_dragLastPos,
                                     m_right, m_dragLastPos );
                    }
                    dc.DrawLine( m_left, ev.GetY(),
                                 m_right, ev.GetY());
                
                    m_dragLastPos = ev.GetY();
                }
                break;
                
                case WXGRID_CURSOR_RESIZE_COL:
                {
                    wxClientDC dc(this);
                    dc.SetLogicalFunction(wxINVERT);
                    if ( m_dragLastPos >= 0 )
                    {
                        dc.DrawLine( m_dragLastPos, m_top,
                                     m_dragLastPos, m_bottom );
                    }
                    dc.DrawLine( ev.GetX(), m_top,
                                 ev.GetX(), m_bottom );
                    
                    m_dragLastPos = ev.GetX();
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
    
    // ------------------------------------------------------------
    //
    // Left mouse button down
    //
    if ( ev.LeftDown() )
    {
        row = -1;
        col = -1;
        wxGridCellCoords cellCoords;
        
        switch( XYToArea( x, y ) )
        {
            case WXGRID_ROWLABEL:
            {
                // don't send a label click event for a hit on the
                // edge of the row label - this is probably the user
                // wanting to resize the row
                //
                if ( YToEdgeOfRow(y) < 0 )
                {
                    row = YToRow(y);
                    if ( !SendEvent( EVT_GRID_LABEL_LEFT_CLICK, row, col, ev ) )
                    {
                        SelectRow( row, ev.ShiftDown() );
                        m_cursorMode = WXGRID_CURSOR_SELECT_ROW;
                    }
                }
            }
            break;

            case WXGRID_COLLABEL:
            {
                // don't send a label click event for a hit on the
                // edge of the col label - this is probably the user
                // wanting to resize the col
                //
                if ( XToEdgeOfCol(x) < 0 )
                {
                    col = XToCol(x);
                    if ( !SendEvent( EVT_GRID_LABEL_LEFT_CLICK, row, col, ev ) )
                    {
                        SelectCol( col, ev.ShiftDown() );
                        m_cursorMode = WXGRID_CURSOR_SELECT_COL;
                    }
                }
            }
            break;

            case WXGRID_CORNERLABEL:
            {
                // leave both row and col as -1
                //
                if ( !SendEvent( EVT_GRID_LABEL_LEFT_CLICK, row, col, ev ) )
                {
                    SelectAll();
                }
            }
            break;

            case WXGRID_CELL:
            {
                XYToCell( x, y, cellCoords );
                if ( !SendEvent( EVT_GRID_CELL_LEFT_CLICK,
                                 cellCoords.GetRow(),
                                 cellCoords.GetCol(),
                                 ev ) )
                {
                    MakeCellVisible( cellCoords );
                    SelectCell( cellCoords );
                }
            }
            break;

            default:
#if 0                
                wxLogMessage( "outside grid area" );
#endif                
                break;
        }
    }
    // ------------------------------------------------------------
    //
    // Left mouse button double click
    //
    else if ( ev.LeftDClick() )
    {
        row = -1;
        col = -1;
        wxGridCellCoords cellCoords;
        
        switch( XYToArea( x, y ) )
        {
            case WXGRID_ROWLABEL:
            {
                // don't send a label click event for a hit on the
                // edge of the row label - this is probably the user
                // wanting to resize the row
                //
                if ( YToEdgeOfRow(y) < 0 )
                {
                    row = YToRow(y);
                    SendEvent(  EVT_GRID_LABEL_LEFT_DCLICK, row, col, ev );
                }
            }
            break;

            case WXGRID_COLLABEL:
            {
                // don't send a label click event for a hit on the
                // edge of the col label - this is probably the user
                // wanting to resize the col
                //
                if ( XToEdgeOfCol(x) < 0 )
                {
                    col = XToCol(x);
                    SendEvent(  EVT_GRID_LABEL_LEFT_DCLICK, row, col, ev );
                }
            }
            break;

            case WXGRID_CORNERLABEL:
            {
                // leave both row and col as -1
                //
                SendEvent(  EVT_GRID_LABEL_LEFT_DCLICK, row, col, ev );                
            }
            break;

            case WXGRID_CELL:
            {
                XYToCell( x, y, cellCoords );
                SendEvent( EVT_GRID_CELL_LEFT_DCLICK,
                                 cellCoords.GetRow(),
                                 cellCoords.GetCol(),
                                 ev );
            }
            break;

            default:
#if 0                
                wxLogMessage( "outside grid area" );
#endif                
                break;
        }
    }
    // ------------------------------------------------------------
    //
    // Left mouse button released
    //
    else if ( ev.LeftUp() )
    {
	switch ( m_cursorMode )
	{
	    case WXGRID_CURSOR_RESIZE_ROW:
	    {
		if ( m_dragLastPos >= 0 )
		{
		    // erase the last line and resize the row
		    //
		    wxClientDC dc( this );
		    dc.SetLogicalFunction( wxINVERT );
		    dc.DrawLine( m_left, m_dragLastPos,
				 m_right, m_dragLastPos );
		    HideCellEditControl();
		    int top = m_top + m_colLabelHeight;
		    if ( m_dragRowOrCol > 0 )
			top = m_rowBottoms[m_dragRowOrCol-1];
		    m_rowHeights[m_dragRowOrCol] = wxMax( ev.GetY() - top,
							  WXGRID_MIN_ROW_HEIGHT );
		    CalcDimensions();
		    ShowCellEditControl();
		    Refresh();

		    // Note: we are ending the event *after* doing
		    // default processing in this case
		    //
		    SendEvent( EVT_GRID_ROW_SIZE, m_dragRowOrCol, -1, ev );
		}
	    }
	    break;

	    case WXGRID_CURSOR_RESIZE_COL:
	    {
		if ( m_dragLastPos >= 0 )
		{
		    // erase the last line and resize the col
		    //
		    wxClientDC dc( this );
		    dc.SetLogicalFunction( wxINVERT );
		    dc.DrawLine( m_left, m_dragLastPos,
				 m_right, m_dragLastPos );
		    HideCellEditControl();
		    int left = m_left + m_rowLabelWidth;
		    if ( m_dragRowOrCol > 0 )
			left = m_colRights[m_dragRowOrCol-1];
		    m_colWidths[m_dragRowOrCol] = wxMax( ev.GetX() - left,
							 WXGRID_MIN_COL_WIDTH );
		    CalcDimensions();
		    ShowCellEditControl();
		    Refresh();

		    // Note: we are ending the event *after* doing
		    // default processing in this case
		    //
		    SendEvent( EVT_GRID_COL_SIZE, -1, m_dragRowOrCol, ev );
		}
	    }
	    break;

	    case WXGRID_CURSOR_SELECT_CELL:
	    {
		if ( IsSelection() )
		{
		    // Note: we are ending the event *after* doing
		    // default processing in this case
		    //
		    SendEvent( EVT_GRID_RANGE_SELECT, -1, -1, ev );
		}
	    }
	    break;
	}

	m_dragLastPos  = -1;
    }
    // ------------------------------------------------------------
    //
    // Right mouse button down
    //
    else if ( ev.RightDown() )
    {
	row = -1;
	col = -1;
	wxGridCellCoords cellCoords;

	switch( XYToArea( x, y ) )
	{
            
	    case WXGRID_ROWLABEL:
	    {
		row = YToRow(y);
		if ( !SendEvent( EVT_GRID_LABEL_RIGHT_CLICK, row, col, ev ) )
		{
                    // TODO: default processing ?
                }
            }
            break;

            case WXGRID_COLLABEL:
            {
                col = XToCol(x);
		if ( !SendEvent( EVT_GRID_LABEL_RIGHT_CLICK, row, col, ev ) )
                {
                    // TODO: default processing ?
                }
            }
            break;

            case WXGRID_CORNERLABEL:
            {
                // leave both row and col as -1
                //
                if ( !SendEvent( EVT_GRID_LABEL_RIGHT_CLICK, row, col, ev ) )
                {
                    // TODO: default processing ?
                }
            }
            break;

            case WXGRID_CELL:
            {
                XYToCell( x, y, cellCoords );
                if ( !SendEvent( EVT_GRID_CELL_RIGHT_CLICK,
                                cellCoords.GetRow(),
                                cellCoords.GetCol(),
                                ev ) )
                {
                    // TODO: default processing ?
                }
            }
            break;

            default:
#if 0                
                wxLogMessage( "outside grid area" );
#endif                
                break;
        }
    }
    // ------------------------------------------------------------
    //
    // Right mouse button double click
    //
    else if ( ev.RightDClick() )
    {
	row = -1;
	col = -1;
	wxGridCellCoords cellCoords;

	switch( XYToArea( x, y ) )
	{
            
	    case WXGRID_ROWLABEL:
	    {
		row = YToRow(y);
		SendEvent( EVT_GRID_LABEL_RIGHT_DCLICK, row, col, ev );
            }
            break;

            case WXGRID_COLLABEL:
            {
                col = XToCol(x);
		SendEvent( EVT_GRID_LABEL_RIGHT_DCLICK, row, col, ev );
            }
            break;

            case WXGRID_CORNERLABEL:
            {
                // leave both row and col as -1
                //
                SendEvent( EVT_GRID_LABEL_RIGHT_DCLICK, row, col, ev );
            }
            break;

            case WXGRID_CELL:
            {
                XYToCell( x, y, cellCoords );
                SendEvent( EVT_GRID_CELL_RIGHT_DCLICK,
                           cellCoords.GetRow(),
                           cellCoords.GetCol(),
                           ev );
            }
            break;

            default:
#if 0                
                wxLogMessage( "outside grid area" );
#endif                
                break;
        }
    }
    // ------------------------------------------------------------
    //
    // No buttons down and mouse moving
    //
    else if ( ev.Moving() )
    {
        switch( XYToArea( x, y ) )
        {
            case WXGRID_ROWLABEL:
            {
                m_dragRowOrCol = YToEdgeOfRow( y );
                if ( m_dragRowOrCol >= 0 )
                {
                    if ( m_cursorMode == WXGRID_CURSOR_SELECT_CELL )
                    {
                        m_cursorMode = WXGRID_CURSOR_RESIZE_ROW;
                        SetCursor( m_rowResizeCursor );
                    }
                }
                else
                {
                    if ( m_cursorMode != WXGRID_CURSOR_SELECT_CELL )
                    {
                        m_cursorMode = WXGRID_CURSOR_SELECT_CELL;
                        SetCursor( *wxSTANDARD_CURSOR );
                    }
                }
            }
            break;

            case WXGRID_COLLABEL:
            {
                m_dragRowOrCol = XToEdgeOfCol( x );
                if ( m_dragRowOrCol >= 0 )
                {
                    if ( m_cursorMode == WXGRID_CURSOR_SELECT_CELL )
                    {
                        m_cursorMode = WXGRID_CURSOR_RESIZE_COL;
                        SetCursor( m_colResizeCursor );
                    }
                }
                else
                {
                    if ( m_cursorMode != WXGRID_CURSOR_SELECT_CELL )
                    {
                        m_cursorMode = WXGRID_CURSOR_SELECT_CELL;
                        SetCursor( *wxSTANDARD_CURSOR );
                    }
                }
            }
            break;

            default:
            {
                if ( m_cursorMode != WXGRID_CURSOR_SELECT_CELL )
                {
                    m_cursorMode = WXGRID_CURSOR_SELECT_CELL;
                    SetCursor( *wxSTANDARD_CURSOR );
                }
            }
            break;
        }        
    }
}


void wxGrid::OnKeyDown( wxKeyEvent& ev )
{
    if ( m_inOnKeyDown )
    {
        // shouldn't be here - we are going round in circles...
        //
        wxLogFatalError( "wxGrid::OnKeyDown called while alread active" );
    }

    m_inOnKeyDown = TRUE;
        
    // propagate the event up and see if it gets processed
    //
    wxWindow *parent = GetParent();
    wxKeyEvent keyEvt( ev );
    keyEvt.SetEventObject( parent );
    
    if ( !parent->GetEventHandler()->ProcessEvent( keyEvt ) )
    {
        // try local handlers
        //
        switch ( ev.KeyCode() )
        {
            case WXK_UP:
                if ( ev.ControlDown() )
                {
                    MoveCursorUpBlock();
                }
                else
                {
                    MoveCursorUp();
                }
                break;
                
            case WXK_DOWN:
                if ( ev.ControlDown() )
                {
                    MoveCursorDownBlock();
                }
                else
                {
                    MoveCursorDown();
                }
                break;

            case WXK_LEFT:
                if ( ev.ControlDown() )
                {
                    MoveCursorLeftBlock();
                }
                else
                {
                    MoveCursorLeft();
                }
                break;

            case WXK_RIGHT:
                if ( ev.ControlDown() )
                {
                    MoveCursorRightBlock();
                }
                else
                {
                    MoveCursorRight();
                }
                break;

            case WXK_RETURN:
                MoveCursorDown();
                break;

            case WXK_HOME:
                if ( ev.ControlDown() )
                {
                    MakeCellVisible( 0, 0 );
                    SelectCell( 0, 0 );
                }
                else
                {
                    ev.Skip();
                }
                break;
                
            case WXK_END:
                if ( ev.ControlDown() )
                {
                    MakeCellVisible( m_numRows-1, m_numCols-1 );
                    SelectCell( m_numRows-1, m_numCols-1 );
                }
                else
                {
                    ev.Skip();
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
                    ev.SetEventObject( m_cellEditCtrl );
                    m_cellEditCtrl->GetEventHandler()->ProcessEvent( ev );
                }
                break;
        }
    }

    m_inOnKeyDown = FALSE;
}


// Text updated in an edit control - either a text control or a
// combo box
//
void wxGrid::OnText( wxKeyEvent& ev )
{
    if ( !m_inOnText )
    {
        m_inOnText = TRUE;
        wxWindow *ctrl = (wxWindow *)ev.GetEventObject();

        if ( ctrl == m_cellEditCtrl &&
             IsTopEditControlEnabled() )
        {
            // set the value of the top edit control
            //
            switch ( m_editCtrlType )
            {
                case wxGRID_TEXTCTRL:
                    ((wxTextCtrl *)m_topEditCtrl)->
                        SetValue(((wxTextCtrl *)ctrl)->GetValue());
                    break;
                
                case wxGRID_COMBOBOX:
                    ((wxComboBox *)m_topEditCtrl)->
                        SetValue(((wxComboBox *)ctrl)->GetValue());
                    break;
            }
        }
        else if ( ctrl == m_topEditCtrl )
        {
            if ( IsCellEditControlEnabled() )
            {
                switch ( m_editCtrlType )
                {
                    case wxGRID_TEXTCTRL:
                        ((wxTextCtrl *)m_cellEditCtrl)->
                            SetValue(((wxTextCtrl *)ctrl)->GetValue());
                        break;
                    
                    case wxGRID_COMBOBOX:
                        ((wxComboBox *)m_cellEditCtrl)->
                            SetValue(((wxComboBox *)ctrl)->GetValue());
                        break;
                }
            }
            else
            {
                // in the case when in-place editing is turned off we just want to 
                // echo the text changes in the cell but not yet update the grid table
                //
                switch ( m_editCtrlType )
                {
                    case wxGRID_TEXTCTRL:
                        DrawCellValue( m_currentCellCoords, ((wxTextCtrl *)ctrl)->GetValue() );
                        break;
                    
                    case wxGRID_COMBOBOX:
                        DrawCellValue( m_currentCellCoords, ((wxComboBox *)ctrl)->GetValue() );
                        break;
                }
            }
        }
    }

    m_inOnText = FALSE;
}

void wxGrid::OnGridScroll( wxScrollEvent& ev )
{
    // propagate the event up and see if it gets processed
    //
    wxWindow *parent = GetParent();
    wxScrollEvent scrollEvt( ev );
    if (parent->GetEventHandler()->ProcessEvent( scrollEvt )) return;
    
    HideCellEditControl();

    if ( ev.GetEventObject() == m_horizScrollBar )
    {
        if ( ev.GetPosition() != m_scrollPosX )
        {
            SetHorizontalScrollPos( ev.GetPosition() );
        }
    }
    else
    {
        if ( ev.GetPosition() != m_scrollPosY )
        {
            SetVerticalScrollPos( ev.GetPosition() );
        }
    }

    ShowCellEditControl();
}


void wxGrid::SelectCell( const wxGridCellCoords& coords )
{
    if ( SendEvent( EVT_GRID_SELECT_CELL, coords.GetRow(), coords.GetCol() ) )
    {
        // the event has been intercepted - do nothing
        return;
    }

    wxClientDC dc( this );

    if ( m_currentCellCoords != wxGridNoCellCoords )
    {
        HideCurrentCellHighlight( dc );
        HideCellEditControl();
        SaveEditControlValue();
    }

    m_currentCellCoords = coords;

    SetEditControlValue();
    if ( IsOnScreen() )
    {
        ShowCellEditControl();
        ShowCurrentCellHighlight( dc );
    }

    if ( IsSelection() )
    {
        ClearSelection();
        if ( !GetBatchCount() ) Refresh();
    }
}


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

        if ( IsTopEditControlEnabled() )
        {
            switch ( m_editCtrlType )
            {
                case wxGRID_TEXTCTRL:
                    ((wxGridTextCtrl *)m_topEditCtrl)->SetStartValue(s);
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
        else if ( IsTopEditControlEnabled() )
        {
            ctrl = m_topEditCtrl;
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


int wxGrid::XYToArea( int x, int y )
{
    if ( x > m_left && x < m_right &&
         y > m_top && y < m_bottom )
    {
        if ( y < m_top + m_colLabelHeight )
        {
            if ( x > m_left + m_rowLabelWidth )
            {
                return WXGRID_COLLABEL;
            }
            else
            {
                return WXGRID_CORNERLABEL;
            }
        }
        else if ( x <= m_left + m_rowLabelWidth )
        {
            return WXGRID_ROWLABEL;
        }
        else
        {
            return WXGRID_CELL;
        }
    }

    return WXGRID_NOAREA;
}


void wxGrid::XYToCell( int x, int y, wxGridCellCoords& coords )
{
    coords.SetRow( YToRow(y) );
    coords.SetCol( XToCol(x) );
}


int wxGrid::YToRow( int y )
{
    int i;
    
    if ( y > m_top + m_colLabelHeight )
    {
        for ( i = m_scrollPosY;  i < m_numRows;  i++ )
        {
            if ( y < m_rowBottoms[i] )
            {
                return i;
            }
        }
    }

    return -1;
}


int wxGrid::XToCol( int x )
{
    int i;
    
    if ( x > m_left + m_rowLabelWidth )
    {
        for ( i = m_scrollPosX;  i < m_numCols;  i++ )
        {
            if ( x < m_colRights[i] )
            {
                return i;
            }
        }
    }

    return -1;
}


// return the row number that that the y coord is near the edge of, or
// -1 if not near an edge
//
int wxGrid::YToEdgeOfRow( int y )
{
    int i, d;
    
    if ( y > m_top + m_colLabelHeight )
    {
        for ( i = m_scrollPosY;  i < m_numRows;  i++ )
        {
            if ( m_rowHeights[i] > WXGRID_LABEL_EDGE_ZONE )
            {
                d = abs( y - m_rowBottoms[i] );
                {
                    if ( d < WXGRID_LABEL_EDGE_ZONE ) return i;
                }
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
    
    if ( x > m_left + m_rowLabelWidth )
    {
        for ( i = m_scrollPosX;  i < m_numCols;  i++ )
        {
            if ( m_colWidths[i] > WXGRID_LABEL_EDGE_ZONE )
            {
                d = abs( x - m_colRights[i] );
                {
                    if ( d < WXGRID_LABEL_EDGE_ZONE ) return i;
                }
            }
        }
    }

    return -1;
    
}


wxRect wxGrid::CellToRect( int row, int col )
{
    wxRect rect( -1, -1, -1, -1 );

    if ( row >= m_scrollPosY  &&  col >= m_scrollPosX )
    {
        rect.x = m_colRights[col] - m_colWidths[col];
        rect.y = m_rowBottoms[row] - m_rowHeights[row];
        rect.width = m_colWidths[col];
        rect.height = m_rowHeights[ row ];
    }

    return rect;
}    


bool wxGrid::MoveCursorUp()
{
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetRow() > 0 )
    {
        SelectCell( m_currentCellCoords.GetRow() - 1,
                      m_currentCellCoords.GetCol() );

        if ( !IsVisible( m_currentCellCoords ) )
            MakeCellVisible( m_currentCellCoords );

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
        SelectCell( m_currentCellCoords.GetRow() + 1,
                      m_currentCellCoords.GetCol() );

        if ( !IsVisible( m_currentCellCoords ) )
            MakeCellVisible( m_currentCellCoords );
            
        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MoveCursorLeft()
{
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetCol() > 0 )
    {
        SelectCell( m_currentCellCoords.GetRow(),
                      m_currentCellCoords.GetCol() - 1 );

        if ( !IsVisible( m_currentCellCoords ) )
            MakeCellVisible( m_currentCellCoords );
        
        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MoveCursorRight()
{
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_currentCellCoords.GetCol() < m_numCols - 1 )
    {
        SelectCell( m_currentCellCoords.GetRow(),
                      m_currentCellCoords.GetCol() + 1 );

        if ( !IsVisible( m_currentCellCoords ) )
            MakeCellVisible( m_currentCellCoords );
            
        return TRUE;
    }

    return FALSE;
}

bool wxGrid::MovePageUp()
{
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_scrollPosY > 0 )
    {
        int row = m_currentCellCoords.GetRow();
        int y = m_rowBottoms[ row ] - m_rowHeights[ row ];
        while ( row > 0 )
        {
            if ( y + m_rowHeights[row-1] > m_bottom ) break;
            y += m_rowHeights[ --row ];
        }
        SetVerticalScrollPos( row );

        SelectCell( row, m_currentCellCoords.GetCol() );
        return TRUE;
    }
    
    return FALSE;
}

bool wxGrid::MovePageDown()
{
    if ( m_currentCellCoords != wxGridNoCellCoords  &&
         m_scrollPosY + m_wholeRowsVisible < m_numRows )
    {
        if ( m_wholeRowsVisible > 0 )
        {
            SetVerticalScrollPos( m_scrollPosY + m_wholeRowsVisible );
        }
        else if ( m_scrollPosY < m_numRows - 1 )
        {
            SetVerticalScrollPos( m_scrollPosY + 1 );
        }
        else
        {
            return FALSE;
        }
        
        // m_scrollPosY will have been updated
        //
        SelectCell( m_scrollPosY, m_currentCellCoords.GetCol() );
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
        
        SelectCell( row, col );

        if ( !IsVisible( m_currentCellCoords ) )
            MakeCellVisible( m_currentCellCoords );

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
        
        SelectCell( row, col );

        if ( !IsVisible( m_currentCellCoords ) )
            MakeCellVisible( m_currentCellCoords );

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
        
        SelectCell( row, col );

        if ( !IsVisible( m_currentCellCoords ) )
            MakeCellVisible( m_currentCellCoords );

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
        
        SelectCell( row, col );

        if ( !IsVisible( m_currentCellCoords ) )
            MakeCellVisible( m_currentCellCoords );

        return TRUE;
    }

    return FALSE;
}



//
// ----- grid drawing functions
//

void wxGrid::DrawLabelAreas( wxDC& dc )
{
    int cw, ch;
    GetClientSize(&cw, &ch);

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush( wxBrush(GetLabelBackgroundColour(), wxSOLID) );
    
    dc.DrawRectangle( m_left, m_top,
                      cw - m_left, m_colLabelHeight );
    
    dc.DrawRectangle( m_left, m_top,
                      m_rowLabelWidth, ch - m_top );
}


void wxGrid::DrawColLabels( wxDC& dc )
{
    int cw, ch;
    GetClientSize(&cw, &ch);

    if (m_colLabelHeight == 0) return;

    DrawColLabelBorders( dc );

    wxRect rect;
    rect.y = m_top + 1;
    rect.height = m_colLabelHeight - 1;

    int labelLeft = m_left + m_rowLabelWidth;
    int i;
    
    for ( i = m_scrollPosX;  i < m_numCols;  i++ )
    {
        if ( labelLeft > cw) break;

        rect.x = 1 + labelLeft;
        rect.width = m_colWidths[i];
        DrawColLabel( dc, rect, i );
            
        labelLeft += m_colWidths[i];
    }
}


void wxGrid::DrawColLabelBorders( wxDC& dc )
{
    if ( m_colLabelHeight <= 0 ) return;

    int i;
    int cw, ch;
    GetClientSize( &cw, &ch );

    dc.SetPen( *wxBLACK_PEN );
  
    // horizontal lines
    //
    dc.DrawLine( m_left, m_top, cw, m_top );

    dc.DrawLine( m_left, m_top + m_colLabelHeight,
                 cw,     m_top + m_colLabelHeight );

    // vertical lines
    //
    int colLeft = m_left + m_rowLabelWidth;
    for ( i = m_scrollPosX;  i <= m_numCols;  i++ )
    {
        if (colLeft > cw) break;

        dc.DrawLine( colLeft, m_top,
                     colLeft, m_top + m_colLabelHeight);

        if ( i < m_numCols ) colLeft += m_colWidths[i];
    }

    // Draw white highlights for a 3d effect
    //
    dc.SetPen( *wxWHITE_PEN );

    colLeft = m_left + m_rowLabelWidth;
    for ( i = m_scrollPosX; i < m_numCols; i++ )
    {
        if (colLeft > cw) break;

        dc.DrawLine(colLeft + 1, m_top + 1,
                    colLeft + m_colWidths[i], m_top + 1);

        dc.DrawLine(colLeft + 1, m_top + 1,
                    colLeft + 1, m_top + m_colLabelHeight);
        
        colLeft += m_colWidths[i];
    }
}


void wxGrid::DrawColLabel( wxDC& dc, const wxRect& rect, int col )
{
    wxRect rect2;
    rect2 = rect;
    rect2.x += 3;
    rect2.y += 2;
    rect2.width -= 5;
    rect2.height -= 4;
    
    dc.SetBackgroundMode( wxTRANSPARENT );
    dc.SetTextBackground( GetLabelBackgroundColour() );
    dc.SetTextForeground( GetLabelTextColour() );
    dc.SetFont( GetLabelFont() );
    
    int hAlign, vAlign;
    GetColLabelAlignment( &hAlign, &vAlign );
    DrawTextRectangle( dc, GetColLabelValue( col ), rect2, hAlign, vAlign );
}


void wxGrid::DrawRowLabels( wxDC& dc )
{
    int cw, ch;
    GetClientSize(&cw, &ch);

    if (m_rowLabelWidth == 0) return;

    DrawRowLabelBorders( dc );

    wxRect rect;
    rect.x = m_left + 1;
    rect.width = m_rowLabelWidth - 1;

    int labelTop = m_top + m_colLabelHeight;
    int i;
    
    for ( i = m_scrollPosY;  i < m_numRows;  i++ )
    {
        if ( labelTop > ch ) break;

        rect.y = 1 + labelTop;
        rect.height = m_rowHeights[i];
        DrawRowLabel( dc, rect, i );
            
        labelTop += m_rowHeights[i];
    }
}


void wxGrid::DrawRowLabelBorders( wxDC& dc )
{
    if ( m_rowLabelWidth <= 0 ) return;

    int i;
    int cw, ch;
    GetClientSize( &cw, &ch );

    dc.SetPen( *wxBLACK_PEN );
  
    // vertical lines
    //
    dc.DrawLine( m_left, m_top, m_left, ch );

    dc.DrawLine( m_left + m_rowLabelWidth, m_top,
                 m_left + m_rowLabelWidth, ch );

    // horizontal lines
    //
    int rowTop = m_top + m_colLabelHeight;
    for ( i = m_scrollPosY;  i <= m_numRows;  i++ )
    {
        if ( rowTop > ch ) break;

        dc.DrawLine( m_left, rowTop,
                     m_left + m_rowLabelWidth, rowTop );

        if ( i < m_numRows ) rowTop += m_rowHeights[i];
    }

    // Draw white highlights for a 3d effect
    //
    dc.SetPen( *wxWHITE_PEN );

    rowTop = m_top + m_colLabelHeight;
    for ( i = m_scrollPosY; i < m_numRows; i++ )
    {
        if ( rowTop > ch ) break;

        dc.DrawLine( m_left + 1, rowTop + 1,
                     m_left + m_rowLabelWidth, rowTop + 1 );

        dc.DrawLine( m_left + 1, rowTop + 1,
                     m_left + 1, rowTop + m_rowHeights[i] );
        
        rowTop += m_rowHeights[i];
    }
}


void wxGrid::DrawRowLabel( wxDC& dc, const wxRect& rect, int row )
{
    wxRect rect2;
    rect2 = rect;
    rect2.x += 3;
    rect2.y += 2;
    rect2.width -= 5;
    rect2.height -= 4;
    
    dc.SetBackgroundMode( wxTRANSPARENT );
    dc.SetTextBackground( GetLabelBackgroundColour() );
    dc.SetTextForeground( GetLabelTextColour() );
    dc.SetFont( GetLabelFont() );
    
    int hAlign, vAlign;
    GetRowLabelAlignment( &hAlign, &vAlign );
    DrawTextRectangle( dc, GetRowLabelValue( row ), rect2, hAlign, vAlign );
}


void wxGrid::DrawCellArea( wxDC& dc )
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  dc.SetPen( *wxTRANSPARENT_PEN );
  dc.SetBrush( wxBrush(GetDefaultCellBackgroundColour(), wxSOLID) );

  int left = m_left + m_rowLabelWidth + 1;
  int top  = m_top + m_colLabelHeight + 1;

  dc.DrawRectangle( left, top, cw - left, ch - top );
}


void wxGrid::DrawGridLines( wxDC& dc )
{
    if ( !m_gridLinesEnabled || !m_numRows || !m_numCols ) return;
    
    int i;
    int cw, ch;
    GetClientSize(&cw, &ch);

    dc.SetPen( wxPen(GetGridLineColour(), 1, wxSOLID) );

    // horizontal grid lines
    //
    int rowTop = m_top + m_colLabelHeight + m_rowHeights[m_scrollPosY];
    for ( i = m_scrollPosY + 1; i <= m_numRows; i++ )
    {
        if ( rowTop > ch ) break;

        dc.DrawLine( m_left + m_rowLabelWidth + 1, rowTop,
                     m_right, rowTop);

        if ( i < m_numRows ) rowTop += m_rowHeights[i];
    }


    // vertical grid lines
    //
    int colLeft = m_left + m_rowLabelWidth + m_colWidths[m_scrollPosX];
    for ( i = m_scrollPosX + 1; i <= m_numCols; i++ )
    {
        if ( colLeft > cw ) break;

        dc.DrawLine( colLeft, m_top + m_colLabelHeight + 1,
                     colLeft, m_bottom );

        if ( i < m_numCols ) colLeft += m_colWidths[i];
    }
}


void wxGrid::DrawCells( wxDC& dc )
{
    if ( !m_numRows || !m_numCols ) return;
    
    int row, col;
        
    int cw, ch;
    GetClientSize( &cw, &ch );
    
    wxRect rect;

    if ( m_table )
    {
        rect.y = m_top + m_colLabelHeight;
        for ( row = m_scrollPosY;  row < m_numRows;  row++ )
        {
            if ( rect.y > ch ) break;

            rect.height = m_rowHeights[ row ];
            rect.x = m_left + m_rowLabelWidth;

            for ( col = m_scrollPosX;  col < m_numCols;  col++ )
            {
                if ( rect.x > cw ) break;

                rect.width = m_colWidths[col];
                DrawCellBackground( dc, rect, row, col );
                DrawCellValue( dc, rect, row, col );
                rect.x += rect.width;
            }
            rect.y += rect.height;
        }
    }
}


void wxGrid::DrawCellBackground( wxDC& dc, const wxRect& rect, int row, int col )
{
    wxRect rect2;
    rect2 = rect;
    rect2.x += 1;
    rect2.y += 1;
    rect2.width -= 2;
    rect2.height -= 2;
    
    dc.SetBackgroundMode( wxSOLID );
    
    if ( IsInSelection( row, col ) )
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
    dc.DrawRectangle( rect2 );
}


// This draws a text value in the given cell.  If useValueArg is FALSE
// (the default) then the grid table value will be used
//
void wxGrid::DrawCellValue( wxDC& dc, const wxRect& rect, int row, int col,
                            const wxString& value, bool useValueArg )
{
    wxRect rect2;
    rect2 = rect;
    rect2.x += 3;
    rect2.y += 2;
    rect2.width -= 5;
    rect2.height -= 4;
    
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
    
    if ( useValueArg )
    {
        DrawTextRectangle( dc, value, rect2, hAlign, vAlign );
    }
    else
    {
        DrawTextRectangle( dc, GetCellValue( row, col ), rect2, hAlign, vAlign );
    }
}


// this is used to echo text being entered into the top edit control when 
// in-place editing is turned off
//
void wxGrid::DrawCellValue( const wxGridCellCoords& coords, const wxString& value )
{
    if ( IsVisible( coords ) )
    {
        int row = coords.GetRow();
        int col = coords.GetCol();
        wxRect rect;
        rect.x = m_colRights[ col ] - m_colWidths[ col ];
        rect.y = m_rowBottoms[ row ] - m_rowHeights[ row ];
        rect.width =  m_colWidths[ col ];
        rect.height = m_rowHeights[ row ];

        wxClientDC dc( this );
        DrawCellBackground( dc, rect, row, col );
        DrawCellValue( dc, rect, row, col, value, TRUE );
    }
}


void wxGrid::DrawCellHighlight( wxDC& dc, int row, int col )
{
    // TODO: bounds checking on row, col ?
    //

    if ( row >= m_scrollPosY  &&  col >= m_scrollPosX )
    {
        long x, y;

        int cw, ch;
        GetClientSize( &cw, &ch );
    
        x = m_colRights[col] - m_colWidths[col];
        if ( x >= cw ) return;
        
        y = m_rowBottoms[row] - m_rowHeights[row];
        if ( y >= ch ) return;

        dc.SetLogicalFunction( wxINVERT );
        dc.SetPen( wxPen(GetCellHighlightColour(), 2, wxSOLID) );
        dc.SetBrush( *wxTRANSPARENT_BRUSH );

        dc.DrawRectangle( x-2, y-2,
                          m_colWidths[col] + 6,
                          m_rowHeights[row] + 6 );

        dc.SetLogicalFunction( wxCOPY );
    }
}


// This function is handy when you just want to update one or a few
// cells.  For example, it is used by SetCellValue()
//
void wxGrid::DrawCell( int row, int col )
{
    if ( !GetBatchCount() )
    {
        if ( !IsVisible( wxGridCellCoords(row, col) ) ) return;
    
        int cw, ch;
        GetClientSize( &cw, &ch );
    
        wxRect rect( CellToRect( row, col ) );

        if ( m_table )
        {
            wxClientDC dc( this );
            DrawCellBackground( dc, rect, row, col );
            DrawCellValue( dc, rect, row, col );
        }
    }
}


// this is just to make other code more obvious
//
void wxGrid::HideCurrentCellHighlight( wxDC& dc )
{
    if ( m_currentCellHighlighted  &&
         m_currentCellCoords != wxGridNoCellCoords )
    {
        DrawCellHighlight( dc, m_currentCellCoords );
        m_currentCellHighlighted = FALSE;
    }
}


// this is just to make other code more obvious
//
void wxGrid::ShowCurrentCellHighlight( wxDC& dc )
{
    if ( !m_currentCellHighlighted  &&
         m_currentCellCoords != wxGridNoCellCoords )
    {
        DrawCellHighlight( dc, m_currentCellCoords );
        m_currentCellHighlighted = TRUE;
    }
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
    
    // see if we are already clipping
    //
    wxRect clipRect;
    dc.GetClippingBox( clipRect );

    bool alreadyClipping = TRUE;
    wxRect intersectRect;

    if ( clipRect.x == 0 && clipRect.y == 0 &&
         clipRect.width == 0 && clipRect.height == 0)
    {
        alreadyClipping = FALSE;
        intersectRect = rect;
    }
    else
    {
        // Find the intersection of the clipping rectangle and our
        // rectangle
        //
        wxRegion region( rect );
        region.Intersect( clipRect );
        if ( region.IsEmpty() )
        {
            // nothing to do
            //
            return;
        }
        intersectRect = region.GetBox();
    }

    if ( alreadyClipping ) dc.DestroyClippingRegion();

    dc.SetClippingRegion( intersectRect );

    StringToLines( value, lines );
    if ( lines.GetCount() )
    {
        GetTextBoxSize( dc, lines, &textWidth, &textHeight );
        dc.GetTextExtent( lines[0], &lineWidth, &lineHeight );
        
        float x, y;
        switch ( horizAlign )
        {
            case wxRIGHT:
                x = rect.x + (rect.width - textWidth - 1.0);
                break;

            case wxCENTRE:
                x = rect.x + ((rect.width - textWidth)/2.0);
                break;

            case wxLEFT:
            default:
                x = rect.x + 1.0;
                break;
        }

        switch ( vertAlign )
        {
            case wxBOTTOM:
                y = rect.y + (rect.height - textHeight - 1);
                break;

            case wxCENTRE:
                y = rect.y + ((rect.height - textHeight)/2.0);
                break;

            case wxTOP:
            default:
                y = rect.y + 1.0;
                break;
        }

        for ( size_t i = 0;  i < lines.GetCount();  i++ )
        {
            dc.DrawText( lines[i], (long)x, (long)y );
            y += lineHeight;
        }
    }
    
    dc.DestroyClippingRegion();
    if (alreadyClipping) dc.SetClippingRegion( clipRect );
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
// ------ functions to get/send data (see also public functions)
//

bool wxGrid::GetModelValues()
{
    if ( m_table )
    {
        // all we need to do is repaint the grid
        //
        Refresh();
        return TRUE;
    }

    return FALSE;
}


bool wxGrid::SetModelValues()
{
    int row, col;
    
    if ( m_table )
    {
        for ( row = m_scrollPosY;  row < m_numRows;  row++ )
        {
            for ( col = m_scrollPosX;  col < m_numCols;  col++ )
            {
                m_table->SetValue( row, col, GetCellValue(row, col) );
            }
        }

        return TRUE;
    }
    
    return FALSE;
}


//
// ------ public functions
//

bool wxGrid::CreateGrid( int numRows, int numCols )
{
    if ( m_created )
    {
        wxLogError( "wxGrid::CreateGrid(numRows, numCols) called more than once" );
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
        if ( !GetBatchCount() ) Refresh();
    }
}


bool wxGrid::InsertRows( int pos, int numRows, bool WXUNUSED(updateLabels) )
{
    // TODO: something with updateLabels flag

    if ( !m_created )
    {
        wxLogError( "Called wxGrid::InsertRows() before calling CreateGrid()" );
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
                SelectCell( 0, 0 );  
            }
            
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
        wxLogError( "Called wxGrid::AppendRows() before calling CreateGrid()" );
        return FALSE;
    }
    
    if ( m_table && m_table->AppendRows( numRows ) )
    {
        if ( m_currentCellCoords == wxGridNoCellCoords )
        {
            // if we have just inserted cols into an empty grid the current
            // cell will be undefined...
            //
            SelectCell( 0, 0 );  
        }
        
        // the table will have sent the results of the append row
        // operation to this view object as a grid table message
        //
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
        wxLogError( "Called wxGrid::DeleteRows() before calling CreateGrid()" );
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
        wxLogError( "Called wxGrid::InsertCols() before calling CreateGrid()" );
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
                SelectCell( 0, 0 );  
            }

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
	wxLogError( "Called wxGrid::AppendCols() before calling CreateGrid()" );
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
            SelectCell( 0, 0 );  
        }
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
        wxLogError( "Called wxGrid::DeleteCols() before calling CreateGrid()" );
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
        
        if ( !GetBatchCount() ) Refresh();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
    



// ------ control panel and cell edit control
//

void wxGrid::EnableEditing( bool edit )
{
    // TODO: improve this ?
    //
    if ( edit != m_editable )
    {
        m_editable = edit;
        if ( !m_editable ) HideCellEditControl();
        m_topEditCtrlEnabled = m_editable;
        m_cellEditCtrlEnabled = m_editable;
        if ( !m_editable ) ShowCellEditControl();            
    }
}


void wxGrid::EnableTopEditControl( bool enable )
{
    if ( enable != m_topEditCtrlEnabled )
    {
        HideCellEditControl();
        m_topEditCtrlEnabled = enable;
        CalcDimensions();
        m_topEditCtrl->Show( enable );

        if ( m_currentCellCoords != wxGridNoCellCoords )
          SetEditControlValue();
          
        ShowCellEditControl();
        if ( !GetBatchCount() ) Refresh();
    }
}

void wxGrid::EnableCellEditControl( bool enable )
{
    if ( m_cellEditCtrl &&
         enable != m_cellEditCtrlEnabled )
    {
        wxClientDC dc( this );
        
        HideCurrentCellHighlight( dc );
        HideCellEditControl();
        SaveEditControlValue();
        
        m_cellEditCtrlEnabled = enable;

        SetEditControlValue();
        ShowCellEditControl();
        ShowCurrentCellHighlight( dc );
    }
}


//
// ------ grid formatting functions
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
    m_rowLabelWidth = wxMax( 0, width );
    CalcDimensions();
    ShowCellEditControl();
    if ( !GetBatchCount() ) Refresh();
}

void wxGrid::SetColLabelSize( int height )
{
    m_colLabelHeight = wxMax( 0, height );
    CalcDimensions();
    ShowCellEditControl();
    if ( !GetBatchCount() ) Refresh();
}

void wxGrid::SetLabelBackgroundColour( const wxColour& colour )
{
    m_labelBackgroundColour = colour;
    if ( !GetBatchCount() ) Refresh();
}

void wxGrid::SetLabelTextColour( const wxColour& colour )
{
    m_labelTextColour = colour;
    if ( !GetBatchCount() ) Refresh();
}

void wxGrid::SetLabelFont( const wxFont& font )
{
    m_labelFont = font;
    if ( !GetBatchCount() ) Refresh();
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

    if ( !GetBatchCount() ) Refresh();
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

    if ( !GetBatchCount() ) Refresh();
}

void wxGrid::SetRowLabelValue( int row, const wxString& s )
{
    if ( m_table )
    {
        m_table->SetRowLabelValue( row, s );
        if ( !GetBatchCount() ) Refresh();
    }
}

void wxGrid::SetColLabelValue( int col, const wxString& s )
{
    if ( m_table )
    {
        m_table->SetColLabelValue( col, s );
        if ( !GetBatchCount() ) Refresh();
    }
}

void wxGrid::SetGridLineColour( const wxColour& colour )
{
    m_gridLineColour = colour;
    
    wxClientDC dc( this );
    DrawGridLines( dc );
}

void wxGrid::EnableGridLines( bool enable )
{
    if ( enable != m_gridLinesEnabled )
    {
        m_gridLinesEnabled = enable;
        if ( !GetBatchCount() ) Refresh();
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


wxColour wxGrid::GetCellHighlightColour()
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
        // TODO: what do we do about events here ?
        // Generate an event for each resize ?
        //
        int row;
        for ( row = 0;  row < m_numRows;  row++ )
        {
            m_rowHeights[row] = m_defaultRowHeight;
        }
        CalcDimensions();
        if ( !GetBatchCount() ) Refresh();
    }
}

void wxGrid::SetRowSize( int row, int height )
{
    if ( row >= 0  &&  row < m_numRows )
    {
        m_rowHeights[row] = wxMax( 0, height );
        CalcDimensions();
        if ( !GetBatchCount() ) Refresh();

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
        // TODO: what do we do about events here ?
        // Generate an event for each resize ?
        //
        int col;
        for ( col = 0;  col < m_numCols;  col++ )
        {
            m_colWidths[col] = m_defaultColWidth;
        }
        CalcDimensions();
        if ( !GetBatchCount() ) Refresh();
    }
}

void wxGrid::SetColSize( int col, int width )
{
    if ( col >= 0  &&  col < m_numCols )
    {
        m_colWidths[col] = wxMax( 0, width );
        CalcDimensions();
        if ( !GetBatchCount() ) Refresh();

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

void wxGrid::SetCellHighlightColour( const wxColour& )
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
        DrawCell( row, col );
        if ( m_currentCellCoords.GetRow() == row &&
             m_currentCellCoords.GetCol() == col )
        {
            SetEditControlValue( s );
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


//
// ------ Grid location functions
//
//  (see also inline functions in grid.h)


// check to see if a cell location is wholly visible
//
bool wxGrid::IsVisible( const wxGridCellCoords& coords )
{
    return ( coords.GetRow() >= m_scrollPosY  &&
             coords.GetRow() < m_scrollPosY + m_wholeRowsVisible  &&
             coords.GetCol() >= m_scrollPosX  &&
             coords.GetCol() < m_scrollPosX + m_wholeColsVisible );
}


// make the specified cell location visible by doing a minimal amount
// of scrolling
//
void wxGrid::MakeCellVisible( int row, int col )
{
    int lastX = m_scrollPosX;
    int lastY = m_scrollPosY;
    
    if ( row >= 0  &&  row < m_numRows  &&
         col >= 0  &&  col < m_numCols )
    {
        if ( row < m_scrollPosY )
        {
            SetVerticalScrollPos( row );
        }
        else if ( row >= m_scrollPosY + m_wholeRowsVisible )
        {
            int i;
            int h = m_rowBottoms[row];
            for ( i = m_scrollPosY; i < m_numRows && h > m_bottom; i++ )
            {
                h -= m_rowHeights[i];
            }
            SetVerticalScrollPos( i );
        }

        if ( col < m_scrollPosX )
        {
            SetHorizontalScrollPos( col );
        }
        else if ( col >= m_scrollPosX + m_wholeColsVisible )
        {
            int i;
            int w = m_colRights[col];
            for ( i = m_scrollPosX; i < m_numCols && w > m_right; i++ )
            {
                w -= m_colWidths[i];
            }
            SetHorizontalScrollPos( i );
        }

        if ( m_scrollPosX != lastX  ||  m_scrollPosY != lastY )
        {
            // The cell was not visible before but not it is
            //
            ShowCellEditControl();
        }
    }
    else
    {
        // TODO: log an error
    }
}


void wxGrid::SetVerticalScrollPos( int topMostRow )
{
    if ( m_vertScrollBar && topMostRow != m_scrollPosY )
    {
        m_scrollPosY = topMostRow;

        CalcDimensions();
        Refresh();
    }
}


void wxGrid::SetHorizontalScrollPos( int leftMostCol )
{
    if ( m_horizScrollBar && leftMostCol != m_scrollPosX )
    {
        m_scrollPosX = leftMostCol;

        CalcDimensions();
        Refresh();
    }
}


//
// ------ block, row and col selection
//

void wxGrid::SelectRow( int row, bool addToSelected )
{
    if ( IsSelection() && addToSelected )
    {
        if ( m_selectedTopLeft.GetRow() > row )
            m_selectedTopLeft.SetRow( row );
        
        m_selectedTopLeft.SetCol( 0 );
        
        if ( m_selectedBottomRight.GetRow() < row )
            m_selectedBottomRight.SetRow( row );
        
        m_selectedBottomRight.SetCol( m_numCols - 1 );
    }
    else
    {
        ClearSelection();
        m_selectedTopLeft.Set( row, 0 );
        m_selectedBottomRight.Set( row, m_numCols-1 );
    }

    if ( !GetBatchCount() )
    {
        wxRect rect( SelectionToRect() );
        if ( rect != wxGridNoCellRect ) Refresh( TRUE, &rect );
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
    if ( addToSelected && m_selectedTopLeft != wxGridNoCellCoords )
    {
        if ( m_selectedTopLeft.GetCol() > col )
            m_selectedTopLeft.SetCol( col );

        m_selectedTopLeft.SetRow( 0 );

        if ( m_selectedBottomRight.GetCol() < col )
            m_selectedBottomRight.SetCol( col );

        m_selectedBottomRight.SetRow( m_numRows - 1 );
    }
    else
    {
        ClearSelection();
        m_selectedTopLeft.Set( 0, col );
        m_selectedBottomRight.Set( m_numRows-1, col );
    }

    if ( !GetBatchCount() )
    {
        wxRect rect( SelectionToRect() );
        if ( rect != wxGridNoCellRect ) Refresh( TRUE, &rect );
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

    m_selectedTopLeft.Set( topRow, leftCol );
    m_selectedBottomRight.Set( bottomRow, rightCol );

    if ( !GetBatchCount() )
    {
        wxRect rect( SelectionToRect() );
        if ( rect != wxGridNoCellRect ) Refresh( TRUE, &rect );
    }

    // only generate an event if the block is not being selected by
    // dragging the mouse (in which case the event will be generated in
    // OnMouse) }
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

    if ( !GetBatchCount() ) Refresh();
}


void wxGrid::ClearSelection()
{
    if ( IsSelection() )
    {
        wxRect rect( SelectionToRect() );
        if ( rect != wxGridNoCellRect )
        {
            Refresh( TRUE, &rect );
        }

        m_selectedTopLeft = wxGridNoCellCoords;
        m_selectedBottomRight = wxGridNoCellCoords;
    }
}    


wxRect wxGrid::SelectionToRect()
{
    wxRect rect;
    wxRect cellRect;

    if ( IsSelection() )
    {
        cellRect = CellToRect( m_selectedTopLeft );
        if ( cellRect != wxGridNoCellRect )
        {
            rect = cellRect;
        }
        else
        {
            rect = wxRect( m_left, m_top, 0, 0 );
        }
        
        cellRect = CellToRect( m_selectedBottomRight );
        if ( cellRect != wxGridNoCellRect )
        {
            rect += cellRect;
        }
        else
        {
            return wxGridNoCellRect;
        }
    }

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

