/////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/toolbar.cpp
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 2001 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univtoolbar.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"
#endif

#include "wx/toolbar.h"
#include "wx/validate.h"

//-----------------------------------------------------------------------------
// wxToolBar
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxToolBar,wxToolBarBase)
    EVT_MOUSE_EVENTS( wxToolBar::OnMouse )
    EVT_PAINT( wxToolBar::OnPaint )
    EVT_SIZE( wxToolBar::OnSize )
END_EVENT_TABLE()

bool wxToolBar::Create( wxWindow *parent, wxWindowID id,
                 const wxPoint& pos, const wxSize& size,
                 long style, const wxString& name )
{
    bool ret = wxToolBarBase::Create( parent, id, pos, size, style, wxDefaultValidator, name );
    
    return ret;
}

void wxToolBar::Init()
{
    // TODO: this is from tbarbase.cpp, but should be in
    // wxToolbarBase::Init
    // the list owns the pointers
    m_tools.DeleteContents(TRUE);
    m_xMargin = m_yMargin = 0;
    m_maxRows = m_maxCols = 0;
    // End TODO

    m_maxWidth = 0;
    m_maxHeight = 0;

    m_captured = NULL;
    SetToolBitmapSize( wxSize(16,15) );
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    return NULL;
}

void wxToolBar::SetToolShortHelp(int id, const wxString& helpString)
{
}

bool wxToolBar::DoInsertTool(size_t pos, wxToolBarToolBase *tool)
{
    return TRUE;
}

bool wxToolBar::DoDeleteTool(size_t pos, wxToolBarToolBase *tool)
{
    return TRUE;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *tool, bool enable)
{
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *tool, bool toggle)
{
    wxToolBarTool *my_tool = (wxToolBarTool*) tool;
    
    bool refresh = (my_tool->IsToggled() != toggle);
    
    my_tool->m_isDown = toggle;
    
    if (refresh)
        RefreshTool( my_tool );
}

void wxToolBar::DoSetToggle(wxToolBarToolBase *tool, bool toggle)
{
}

wxToolBarToolBase *wxToolBar::CreateTool(int id,
                                          const wxBitmap& bitmap1,
                                          const wxBitmap& bitmap2,
                                          bool toggle,
                                          wxObject *clientData,
                                          const wxString& shortHelpString,
                                          const wxString& longHelpString)
{
    return new wxToolBarTool( this, id, bitmap1, bitmap2, toggle,
                              clientData, shortHelpString, longHelpString);
}
                                          
wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control)
{
    wxFAIL_MSG( wxT("Toolbar doesn't support controls yet.") );
    
    return NULL;
}

void wxToolBar::RefreshTool( wxToolBarTool *tool )
{
    wxRect rect( tool->m_x, tool->m_y, m_defaultWidth+6, m_defaultHeight+6 );
    Refresh( TRUE, &rect );
}

void wxToolBar::DrawToolBarTool( wxToolBarTool *tool, wxDC &dc, bool down )
{
    if (down)
    {
        dc.DrawBitmap( tool->GetBitmap1(), tool->m_x+4, tool->m_y+4, TRUE );
        
        dc.SetPen( *wxGREY_PEN );
        dc.DrawLine( tool->m_x, tool->m_y, tool->m_x+m_defaultWidth+5, tool->m_y );
        dc.DrawLine( tool->m_x, tool->m_y, tool->m_x, tool->m_y+m_defaultHeight+5 );
        
        dc.SetPen( *wxBLACK_PEN );
        dc.DrawLine( tool->m_x+1, tool->m_y+1, tool->m_x+m_defaultWidth+4, tool->m_y+1 );
        dc.DrawLine( tool->m_x+1, tool->m_y+1, tool->m_x+1, tool->m_y+m_defaultHeight+4 );
        
        dc.SetPen( *wxWHITE_PEN );
        dc.DrawLine( tool->m_x, tool->m_y+m_defaultHeight+5, tool->m_x+m_defaultWidth+6, tool->m_y+m_defaultHeight+5 );
        dc.DrawLine( tool->m_x+m_defaultWidth+5, tool->m_y, tool->m_x+m_defaultWidth+5, tool->m_y+m_defaultHeight+6 );
    }
    else
    {
        dc.DrawBitmap( tool->GetBitmap1(), tool->m_x+3, tool->m_y+3, TRUE );
        
        dc.SetPen( *wxWHITE_PEN );
        dc.DrawLine( tool->m_x, tool->m_y, tool->m_x+m_defaultWidth+5, tool->m_y );
        dc.DrawLine( tool->m_x, tool->m_y, tool->m_x, tool->m_y+m_defaultHeight+5 );
        dc.DrawLine( tool->m_x+m_defaultWidth+4, tool->m_y, tool->m_x+m_defaultWidth+4, tool->m_y+2 );
        dc.DrawLine( tool->m_x, tool->m_y+m_defaultHeight+4, tool->m_x+2, tool->m_y+m_defaultHeight+4 );
        
        dc.SetPen( *wxBLACK_PEN );
        dc.DrawLine( tool->m_x, tool->m_y+m_defaultHeight+5, tool->m_x+m_defaultWidth+6, tool->m_y+m_defaultHeight+5 );
        dc.DrawLine( tool->m_x+m_defaultWidth+5, tool->m_y, tool->m_x+m_defaultWidth+5, tool->m_y+m_defaultHeight+6 );
        
        dc.SetPen( *wxGREY_PEN );
        dc.DrawLine( tool->m_x+1, tool->m_y+m_defaultHeight+4, tool->m_x+m_defaultWidth+5, tool->m_y+m_defaultHeight+4 );
        dc.DrawLine( tool->m_x+m_defaultWidth+4, tool->m_y+1, tool->m_x+m_defaultWidth+4, tool->m_y+m_defaultHeight+5 );
    }
}

void wxToolBar::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    
    wxSize clientSize = GetClientSize();
    dc.SetPen( *wxBLACK_PEN );
    dc.DrawLine( 0,0, clientSize.x,0 );
    
    for ( wxToolBarToolsList::Node *node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxToolBarTool *tool = (wxToolBarTool*) node->Data();
        
        if (tool->GetId() == -1) continue;
        
        DrawToolBarTool( tool, dc, tool->m_isDown );
    }
}

bool wxToolBar::Realize()
{
    if (!wxToolBarBase::Realize())
        return FALSE;

    int x;
    int y;

    if (GetWindowStyleFlag() & wxTB_VERTICAL)
    {
        x = m_xMargin;
        y = 5;
    }
    else
    {
        y = m_yMargin;
        x = 5;
    }

    for ( wxToolBarToolsList::Node *node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxToolBarTool *tool = (wxToolBarTool*) node->Data();
        
        if (GetWindowStyleFlag() & wxTB_VERTICAL)
        {
            if (tool->GetId() == -1)
            {
                y += 6;
                continue;
            }
            tool->m_x = m_xMargin;
            tool->m_y = y;
            y += m_defaultHeight + 6;

            // Calculate the maximum height or width (depending on style)
            // so we know how to size the toolbar in Realize.
            // We could get the size of the tool instead of the
            // default bitmap size
            if (m_maxWidth < (m_defaultWidth + 2*(m_xMargin + 2)))
                m_maxWidth = (m_defaultWidth + 2*(m_xMargin + 2)) ;
        }
        else
        {
            if (tool->GetId() == -1)
            {
                x += 6;
                continue;
            }
            tool->m_x = x;
            tool->m_y = m_yMargin;
            x += m_defaultWidth + 6;

            // Calculate the maximum height or width (depending on style)
            // so we know how to size the toolbar in Realize.
            // We could get the size of the tool instead of the
            // default bitmap size
            if (m_maxHeight < (m_defaultHeight + 2*(m_yMargin + 2)))
                m_maxHeight = (m_defaultHeight + 2*(m_yMargin + 2)) ;
        }
        
    }

    wxSize sz = GetSize();
    if (GetWindowStyleFlag() & wxTB_VERTICAL)
    {
        SetSize(m_maxWidth, sz.y);
    }
    else
    {
        SetSize(sz.x, m_maxHeight);
    }
    
    SetSize( x+16, m_defaultHeight + 14 );
    
    return TRUE;
}

void wxToolBar::OnMouse(wxMouseEvent &event)
{
    wxToolBarTool *hit = NULL;
    int x = event.GetX();
    int y = event.GetY();
    
    for ( wxToolBarToolsList::Node *node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxToolBarTool *tool = (wxToolBarTool*) node->Data();
        
        if ((x > tool->m_x) && (x < tool->m_x+m_defaultWidth+5)  &&
            (y > tool->m_y) && (y < tool->m_y+m_defaultHeight+5))
        {
            hit = tool;
            break;
        }
    }
    
    if (event.LeftDown() && (hit))
    {
        CaptureMouse();
        m_captured = hit;
        
        m_captured->m_isDown = TRUE;
        RefreshTool( m_captured );
        
        return;
    }
    
    if (event.Dragging() && (m_captured))
    {
        bool is_down = (hit == m_captured);
        if (is_down != m_captured->m_isDown)
        {
            m_captured->m_isDown = is_down;
            RefreshTool( m_captured );
        }
        
        return;
    }
    
    if (event.LeftUp() && (m_captured))
    {
        ReleaseMouse();
        
        m_captured->m_isDown = FALSE;
        RefreshTool( m_captured );
        
        if (hit == m_captured)
        {
            wxCommandEvent cevent( wxEVT_COMMAND_TOOL_CLICKED, m_captured->GetId() );
            cevent.SetEventObject( this );
            // cevent.SetExtraLong((long) toggleDown);
            GetEventHandler()->ProcessEvent( cevent );
        }
        
        m_captured = NULL;
        
        return;
    }
}

