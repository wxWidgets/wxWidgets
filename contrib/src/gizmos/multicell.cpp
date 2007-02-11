/////////////////////////////////////////////////////////////////////////////
// Name:        multicell.cpp
// Purpose:     provide two new classes for layout, wxMultiCellSizer and wxMultiCellCanvas
// Author:      Jonathan Bayer
// Modified by:
// Created:
// RCS-ID:      $Id$
// Copyright:   (c) Jonathan Bayer
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// This was inspired by the gbsizer class written by Alex Andruschak

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/gizmos/multicell.h"




//---------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxMultiCellSizer, wxSizer);
IMPLEMENT_ABSTRACT_CLASS(wxMultiCellItemHandle, wxObject);

//---------------------------------------------------------------------------
// wxMultiCellItemHandle
//---------------------------------------------------------------------------
/*
 *Function Name: wxMultiCellItemHandle :: wxMultiCellItemHandle
 *
 *Parameters:    int         row
 *               int         column
 *               int         height
 *               int         width
 *               wxSize      size
 *               wxResizable Style
 *               wxSize      weight
 *               int         align
 *
 *Description:   This is the constructor for the class.
 *
 */

void wxMultiCellItemHandle :: Initialize( int row, int column, int height, int width, wxSize size, wxResizable Style, wxSize weight, int align)
{
    m_column = column;
    m_row = row;
    m_width = width;
    m_height = height;

    m_style = Style;
    m_fixedSize = size;
    m_alignment = align;
    m_weight = weight;
}
//---------------------------------------------------------------------------
wxMultiCellItemHandle :: wxMultiCellItemHandle( int row, int column, int height, int width, wxSize size, wxResizable Style, wxSize weight, int align)
{
    Initialize(row, column,height, width, size, Style, weight, align);
}
//---------------------------------------------------------------------------
wxMultiCellItemHandle :: wxMultiCellItemHandle( int row, int column, wxSize size, wxResizable style, wxSize weight, int align)
{
    Initialize(row, column,1, 1, size, style, weight, align);
}
//---------------------------------------------------------------------------
wxMultiCellItemHandle :: wxMultiCellItemHandle( int row, int column, wxResizable style, wxSize weight, int align)
{
    Initialize(row, column, 1, 1, wxSize(1, 1), style, weight, align);
}
//---------------------------------------------------------------------------
int wxMultiCellItemHandle::GetColumn() const
{
    return m_column;
}
//---------------------------------------------------------------------------
int wxMultiCellItemHandle::GetRow() const
{
    return m_row;
}
//---------------------------------------------------------------------------
int wxMultiCellItemHandle::GetWidth() const
{
    return m_width;
}
//---------------------------------------------------------------------------
int wxMultiCellItemHandle::GetHeight() const
{
    return m_height;
}
//---------------------------------------------------------------------------
wxResizable    wxMultiCellItemHandle :: GetStyle() const
{
    return m_style;
};
//---------------------------------------------------------------------------
wxSize wxMultiCellItemHandle :: GetLocalSize() const
{
    return m_fixedSize;
};
//---------------------------------------------------------------------------
int wxMultiCellItemHandle :: GetAlignment() const
{
    return m_alignment;
};
//---------------------------------------------------------------------------
wxSize wxMultiCellItemHandle :: GetWeight() const
{
    return m_weight;
};



//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// wxMultiCellSizer
//---------------------------------------------------------------------------

/*
 *Function Name: wxMultiCellSizer::Initialize
 *
 *Parameters:    wxsize Initial size of sizer
 *
 *Description:   This is a common function to initialize all the members of
 *               this class.  It is only called from the constructors
 *
 */

void wxMultiCellSizer::Initialize( wxSize size )
{
    m_cell_count = size;
    m_maxHeight = (int *)malloc((1 + m_cell_count.GetHeight()) * sizeof(int));
    m_maxWidth = (int *)malloc( (1 + m_cell_count.GetWidth()) * sizeof(int));
    m_rowStretch = (int *)malloc( (1 + m_cell_count.GetHeight()) * sizeof(int));
    m_colStretch = (int *)malloc((1 + m_cell_count.GetWidth()) * sizeof(int));

    m_weights = (wxSize **)malloc((1 + wxMax(m_cell_count.GetHeight(), m_cell_count.GetWidth())) * sizeof(wxSize *));
    m_minSizes = (wxSize **)malloc((1 + wxMax(m_cell_count.GetHeight(), m_cell_count.GetWidth())) * sizeof(wxSize *));
    for (int x = 0; x < 1 + wxMax(m_cell_count.GetHeight(), m_cell_count.GetWidth()); x++)
    {
        m_weights[x] = new wxSize(0,0);
        m_minSizes[x] = new wxSize(0,0);
    }

    m_maxWeights = 1 + wxMax(m_cell_count.GetHeight(), m_cell_count.GetWidth());
    m_defaultCellSize = wxSize(5, 5);
    m_win = NULL;
    m_pen = wxRED_PEN;
}
//---------------------------------------------------------------------------
wxMultiCellSizer::wxMultiCellSizer( wxSize & size )
{
    Initialize(size);
}
//---------------------------------------------------------------------------
wxMultiCellSizer::wxMultiCellSizer( int rows, int cols)
{
    wxSize size(cols, rows);
    Initialize(size);
}
//---------------------------------------------------------------------------
wxMultiCellSizer::~wxMultiCellSizer()
{
    WX_CLEAR_LIST(wxSizerItemList, m_children);

    free(m_maxHeight);
    free(m_maxWidth);
    free(m_rowStretch);
    free(m_colStretch);

    for (int x = 0; x < 1 + wxMax(m_cell_count.GetHeight(), m_cell_count.GetWidth()); x++)
    {
        delete m_weights[x];
        delete m_minSizes[x];
    }
    free(m_weights);
    free(m_minSizes);
}
//---------------------------------------------------------------------------
bool wxMultiCellSizer::EnableGridLines(wxWindow *win)
{
    m_win = win;
    return true;
}
//---------------------------------------------------------------------------
bool wxMultiCellSizer::SetGridPen(const wxPen *pen)
{
    m_pen = pen;
    return true;
}

//---------------------------------------------------------------------------
bool wxMultiCellSizer::SetDefaultCellSize(wxSize size)
{
    m_defaultCellSize = size;
    return true;
}
//---------------------------------------------------------------------------
bool wxMultiCellSizer::SetColumnWidth(int column, int colSize, bool expandable)
{
    if (expandable)
    {
        m_minSizes[column]->SetWidth(-colSize);
    }
    else
    {
        m_minSizes[column]->SetWidth(colSize);
    }
    return true;
}
//---------------------------------------------------------------------------
bool wxMultiCellSizer::SetRowHeight(int row, int rowSize, bool expandable)
{
    if (expandable)
    {
        m_minSizes[row]->SetHeight(-rowSize);
    }
    else
    {
        m_minSizes[row]->SetHeight(rowSize);
    }
    return true;
}
//---------------------------------------------------------------------------
void wxMultiCellSizer::RecalcSizes()
{
    if (m_children.GetCount() == 0)
        return;
    wxSize size = GetSize();
    wxPoint pos = GetPosition();

    GetMinimums();

    // We need to take the unused space and equally give it out to all the rows/columns
    // which are stretchable

    int unUsedWidth = size.GetWidth() - Sum(m_maxWidth, m_cell_count.GetWidth());
    int unUsedHeight = size.GetHeight() - Sum(m_maxHeight, m_cell_count.GetHeight());
    int totalWidthWeight = 0;
    int totalHeightWeight = 0;
    int x;

    for (x = 0; x < wxMax(m_cell_count.GetHeight(), m_cell_count.GetWidth()); x++)
    {
        if (m_rowStretch[x])
        {
            totalHeightWeight += m_weights[x]->GetHeight();
        }
        if (x < m_cell_count.GetWidth() && m_colStretch[x])
        {
            totalWidthWeight += m_weights[x]->GetWidth();
        }
    }
    for (x = 0; x < wxMax(m_cell_count.GetHeight(), m_cell_count.GetWidth()); x++)
    {
        if (x < m_cell_count.GetHeight() && m_rowStretch[x])
        {
            m_maxHeight[x] += unUsedHeight * m_weights[x]->GetHeight() / totalHeightWeight;
        }
        if (x < m_cell_count.GetWidth() && m_colStretch[x])
        {
            m_maxWidth[x] += unUsedWidth * m_weights[x]->GetWidth() / totalWidthWeight;
        }
    }
    // We now have everything we need to figure each cell position and size
    // The arrays m_maxHeight and m_maxWidth now contain the final widths and height of
    // each row and column.

    double cell_width = (double)size.GetWidth() / (double)m_cell_count.GetWidth();
    double cell_height = (double)size.GetHeight() / (double)m_cell_count.GetHeight();
    wxPoint c_point;
    wxSize  c_size;

    wxSizerItemList::compatibility_iterator current = m_children.GetFirst();
    while (current)
    {
        wxSizerItem *item = current->GetData();

        wxMultiCellItemHandle *rect;
        if (item != NULL &&
            (rect = (wxMultiCellItemHandle *)item->GetUserData()) != NULL)
        {
            c_point.x = pos.x + (int)(rect->GetColumn() * cell_width);
            c_point.y = pos.y + (int)(rect->GetRow() * cell_height);

            c_point.x = pos.x + Sum(m_maxWidth, rect->GetColumn());
            c_point.y = pos.y + Sum(m_maxHeight, rect->GetRow());


            c_size = rect->GetLocalSize();
            wxSize minSize( item->CalcMin() );
            if (c_size.GetHeight() != wxDefaultCoord ||
                c_size.GetWidth() != wxDefaultCoord)
            {
                minSize.SetHeight(wxMax(minSize.GetHeight(), c_size.GetHeight()));
                minSize.SetWidth(wxMax(minSize.GetWidth(), c_size.GetWidth()));
            }
            if (rect->GetStyle() & wxHORIZONTAL_RESIZABLE ||
                rect->GetWidth() > 1
                || m_minSizes[rect->GetColumn()]->GetWidth() < 0)
            {
                int w = 0;
                for (int x = 0; x < rect->GetWidth(); x++)
                {
                    w += m_maxWidth[rect->GetColumn() + x];
                }
                c_size.SetWidth(w);
            }
            else
            {
                c_size.SetWidth(minSize.GetWidth() );
            }
            if (rect->GetStyle() & wxVERTICAL_RESIZABLE ||
                rect->GetHeight() > 1 ||
                m_minSizes[rect->GetRow()]->GetHeight() < 0)
            {
                int h = 0;
                for (int x = 0; x < rect->GetHeight(); x++)
                {
                    h += m_maxHeight[rect->GetRow() + x];
                }
                c_size.SetHeight(h);
            }
            else
            {
                c_size.SetHeight(minSize.GetHeight());
            }
            int extraHeight = (m_maxHeight[rect->GetRow()] - c_size.GetHeight());
            int extraWidth = (m_maxWidth[rect->GetColumn()] - c_size.GetWidth());

            if (rect->GetWidth() == 1 && rect->GetAlignment() & wxALIGN_CENTER_HORIZONTAL)
            {
                c_point.x += extraWidth / 2;
            }
            if (rect->GetWidth() == 1 && rect->GetAlignment() & wxALIGN_RIGHT)
            {
                c_point.x += extraWidth;
            }
            if (rect->GetHeight() == 1 && rect->GetAlignment() & wxALIGN_CENTER_VERTICAL)
            {
                c_point.y += extraHeight / 2;
            }
            if (rect->GetHeight() == 1 && rect->GetAlignment() & wxALIGN_BOTTOM)
            {
                c_point.y += extraHeight;
            }
            item->SetDimension(c_point, c_size);
        }
        current = current->GetNext();
    }
}
//---------------------------------------------------------------------------
wxSize wxMultiCellSizer::CalcMin()
{
    if (m_children.GetCount() == 0)
        return wxSize(10,10);

    GetMinimums();
    int m_minWidth = Sum(m_maxWidth, m_cell_count.GetWidth());
    int m_minHeight = Sum(m_maxHeight, m_cell_count.GetHeight());
    return wxSize( m_minWidth, m_minHeight );
}
//---------------------------------------------------------------------------
void wxMultiCellSizer :: GetMinimums()
{
    // We first initial all the arrays EXCEPT for the m_minsizes array.

    memset(m_maxHeight, 0, sizeof(int) * m_cell_count.GetHeight());
    memset(m_maxWidth, 0, sizeof(int) * m_cell_count.GetWidth());
    memset(m_rowStretch, 0, sizeof(int) * m_cell_count.GetHeight());
    memset(m_colStretch, 0, sizeof(int) * m_cell_count.GetWidth());
    for (int x = 0; x < 1 + wxMax(m_cell_count.GetHeight(), m_cell_count.GetWidth()); x++)
    {
        m_weights[x]->SetHeight(0);
        m_weights[x]->SetWidth(0);
    }

    wxSizerItemList::compatibility_iterator     node = m_children.GetFirst();
    while (node)
    {
        wxSizerItem     *item = node->GetData();
        wxMultiCellItemHandle *rect;
        if (item != NULL &&
            (rect = (wxMultiCellItemHandle *)item->GetUserData()) != NULL)
        {
            int row = rect->GetRow();
            int col = rect->GetColumn();

            // First make sure that the control knows about the max rows and columns

            int changed = false;
            if (row + 1 > m_cell_count.GetHeight())
            {
                changed++;
                m_maxHeight = (int *)realloc(m_maxHeight, (1 + row) * sizeof(int));
                m_rowStretch = (int *)realloc(m_rowStretch, (1 + row) * sizeof(int));
                for (int x = m_cell_count.GetHeight(); x < row + 1; x++)
                {
                    m_maxHeight[x - 1] = 0;
                    m_rowStretch[x - 1] = 0;
                }
                m_cell_count.SetHeight(row + 1);
            }
            if (col + 1 > m_cell_count.GetWidth())
            {
                changed++;
                m_maxWidth = (int *)realloc(m_maxWidth, (1 + col) * sizeof(int));
                m_colStretch = (int *)realloc(m_colStretch, ( 1 + col) * sizeof(int));
                for (int x = m_cell_count.GetWidth(); x < col + 1; x++)
                {
                    m_maxWidth[x - 1] = 0;
                    m_colStretch[x - 1] = 0;
                }
                m_cell_count.SetWidth(col + 1);
            }
            if (changed)
            {
                m_weights = (wxSize **)realloc(m_weights, (1 + wxMax(m_cell_count.GetHeight(), m_cell_count.GetWidth())) * sizeof(wxSize *));
                m_minSizes = (wxSize **)realloc(m_minSizes, (1 + wxMax(m_cell_count.GetHeight(), m_cell_count.GetWidth())) * sizeof(wxSize *));
                for (int x = m_maxWeights; x < 1 + wxMax(m_cell_count.GetHeight(), m_cell_count.GetWidth()); x++)
                {
                    m_weights[x - 1] = new wxSize(0,0);
                    m_minSizes[x - 1] = new wxSize(0,0);
                }
                m_maxWeights = 1 + wxMax(m_cell_count.GetHeight(), m_cell_count.GetWidth());
            }

            // Sum the m_weights for each row/column, but only if they are resizable

            wxSize minSize( item->CalcMin() );
            wxSize c_size = rect->GetLocalSize();
            if (c_size.GetHeight() != wxDefaultCoord ||
                c_size.GetWidth() != wxDefaultCoord)
            {
                minSize.SetHeight(wxMax(minSize.GetHeight(), c_size.GetHeight()));
                minSize.SetWidth(wxMax(minSize.GetWidth(), c_size.GetWidth()));
            }

            // For each row, calculate the max height for those fields which are not
            // resizable in the vertical pane

            if (!(rect->GetStyle() & wxVERTICAL_RESIZABLE || m_minSizes[row]->GetHeight() < 0))
            {
                m_maxHeight[row] = wxMax(m_maxHeight[row], minSize.GetHeight() / rect->GetHeight());
            }
            else
            {
                m_rowStretch[row] = 1;
                if (m_minSizes[row]->GetHeight())
                {
                    m_maxHeight[row] = abs(m_minSizes[row]->GetHeight());
                }
                else
                {
                    m_maxHeight[row] = wxMax(m_maxHeight[row], m_defaultCellSize.GetHeight());
                }
                m_weights[row]->SetHeight(wxMax(m_weights[row]->GetHeight(), rect->GetWeight().GetHeight()));
            }

            // For each column, calculate the max width for those fields which are not
            // resizable in the horizontal pane

            if (!(rect->GetStyle() & wxHORIZONTAL_RESIZABLE || m_minSizes[col]->GetWidth() < 0))
            {
                if (m_minSizes[col]->GetWidth())
                {
                    m_maxWidth[col] = abs(m_minSizes[col]->GetWidth());
                }
                else
                {
                    m_maxWidth[col] = wxMax(m_maxWidth[col], minSize.GetWidth() / rect->GetWidth());
                }
            }
            else
            {
                m_colStretch[col] = 1;
                m_maxWidth[col] = wxMax(m_maxWidth[col], m_defaultCellSize.GetWidth());
                m_weights[col]->SetWidth(wxMax(m_weights[col]->GetWidth(), rect->GetWeight().GetWidth()));
            }
            node = node->GetNext();
        }
    }
} // wxMultiCellSizer :: GetMinimums
//---------------------------------------------------------------------------
/*
 *Function Name: wxMultiCellSizer :: Sum
 *
 *Parameters:    int* pointer to array of ints
 *               int  Number of cells to sum up
 *
 *Description:   This member function sums up all the elements of the array which
 *               preceed the specified cell.
 *
 *Returns:       int Sum
 *
 */

/* static */ int wxMultiCellSizer :: Sum(int *array, int x)
{
    int sum = 0;
    while (x--)
    {
        sum += array[x];
    }
    return sum;
}
//---------------------------------------------------------------------------
/*
 *Function Name: wxMultiCellSizer :: DrawGridLines
 *
 *Parameters:    wxDC Device context
 *
 *Description:   This function draws the grid lines in the specified device context.
 *
 */

void wxMultiCellSizer :: DrawGridLines(wxDC& dc)
{
    RecalcSizes();
    int maxW = Sum(m_maxWidth, m_cell_count.GetWidth());
    int maxH = Sum(m_maxHeight, m_cell_count.GetHeight());
    int x;

    // Draw the columns
    dc.SetPen(* m_pen);
    for (x = 1; x < m_cell_count.GetWidth(); x++)
    {
        int colPos = Sum(m_maxWidth, x) ;
        dc.DrawLine(colPos, 0, colPos, maxH);
    }

    // Draw the rows
    for (x = 1; x < m_cell_count.GetHeight(); x++)
    {
        int rowPos = Sum(m_maxHeight, x);
        dc.DrawLine(0, rowPos, maxW, rowPos);
    }
}
//---------------------------------------------------------------------------
// Define the repainting behaviour
/*
 *Function Name: wxMultiCellSizer::OnPaint
 *
 *Parameters:    wxDC Device context
 *
 *Description:   This function calls the DrawGridLines() member if a window
 *               has been previously specified.  This functions MUST be called
 *               from an OnPaint member belonging to the window which the sizer
 *               is attached to.
 *
 */

void wxMultiCellSizer::OnPaint(wxDC& dc )
{
    if (m_win)
    {
        DrawGridLines(dc);
    }
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------




#define CELL_LOC(row, col) ((row) * m_maxCols + col)

//---------------------------------------------------------------------------
// wxCell
//---------------------------------------------------------------------------
/*
 *Function Name: wxCell : wxLayoutConstraints
 *
 *Description:   This class is used by wxMultiCellCanvas for internal storage
 *
 */

class wxCell : public wxLayoutConstraints
{
public:
    wxCell(wxWindow *win)
    {
        m_window = win;
    };

    wxWindow    *m_window;
};



//---------------------------------------------------------------------------
// wxMultiCellCanvas
//---------------------------------------------------------------------------
wxMultiCellCanvas :: wxMultiCellCanvas(wxWindow *par, int numRows, int numCols)
   : wxFlexGridSizer(numRows, numCols, 0, 0)
{
    m_cells = (wxCell **)calloc(numRows * numCols, sizeof(wxCell *));

    m_parent = par;
    m_maxRows = numRows;
    m_maxCols = numCols;
    m_minCellSize = wxSize(5, 5);
}
//---------------------------------------------------------------------------
void wxMultiCellCanvas :: Add(wxWindow *win, unsigned int row, unsigned int col)
{
  // thanks to unsigned data row and col are always >= 0
    wxASSERT_MSG( /* row >= 0 && */ row < m_maxRows,
                 wxString::Format(_T("Row %d out of bounds (0..%d)"), row, m_maxRows) );
    wxASSERT_MSG( /* col >= 0 && */ col < m_maxCols,
                 wxString::Format(_T("Column %d out of bounds (0..%d)"), col, m_maxCols) );

    wxASSERT_MSG(m_cells[CELL_LOC(row, col)] == NULL, wxT("Cell already occupied"));

    wxCell *newCell = new wxCell(win);
    m_cells[CELL_LOC(row,col)] = newCell;
}
//---------------------------------------------------------------------------
void wxMultiCellCanvas :: CalculateConstraints()
{
    unsigned int    row, col;
    for (row = 0; row < m_maxRows; row++)
    {
        for (col = 0; col < m_maxCols; col++)
        {
            if (!m_cells[CELL_LOC(row, col)])
            {
                // Create an empty static text field as a placeholder
                m_cells[CELL_LOC(row, col)] = new wxCell(new wxStaticText(m_parent, wxID_ANY, wxEmptyString));
            }
            wxFlexGridSizer::Add(m_cells[CELL_LOC(row, col)]->m_window);
        }
    }
}

/*** End of File ***/
