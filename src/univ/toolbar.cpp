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
#include "wx/image.h"

//-----------------------------------------------------------------------------
// wxToolBar
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxToolBar,wxToolBarBase)
    EVT_MOUSE_EVENTS( wxToolBar::OnMouse )
    EVT_PAINT( wxToolBar::OnPaint )
    EVT_SIZE( wxToolBar::OnSize )
    EVT_ENTER_WINDOW( wxToolBar::OnEnter )
    EVT_LEAVE_WINDOW( wxToolBar::OnLeave )
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
    m_underMouse = NULL;
    
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
    // Comment this out if you don't want the disabled look,
    // which currently acts weirdly for the scissors icon
    // in the toolbar sample. See src/common/tbarbase.cpp
    // for the wxCreateGreyedImage function.
#if 1
    // Created disabled-state bitmap on demand
    if (!enable && !tool->GetBitmap2().Ok())
    {
        wxImage image( tool->GetBitmap1() );

        unsigned char bg_red = 180;
        unsigned char bg_green = 180;
        unsigned char bg_blue = 180;
        
        unsigned char mask_red = image.GetMaskRed();
        unsigned char mask_green = image.GetMaskGreen();
        unsigned char mask_blue = image.GetMaskBlue();
        
        bool has_mask = image.HasMask();
        
        int x,y;
        for (y = 0; y < image.GetHeight(); y++)
        {
            for (x = 0; x < image.GetWidth(); x++) 
            {
                unsigned char red = image.GetRed(x,y);
                unsigned char green = image.GetGreen(x,y);
                unsigned char blue = image.GetBlue(x,y);
                if (!has_mask || red != mask_red || green != mask_green || blue != mask_blue)
                {
                    red = (((wxInt32) red  - bg_red) >> 1) + bg_red;
                    green = (((wxInt32) green  - bg_green) >> 1) + bg_green;
                    blue = (((wxInt32) blue  - bg_blue) >> 1) + bg_blue;
                    image.SetRGB( x, y, red, green, blue );                
                }
            }
        }

        for (y = 0; y < image.GetHeight(); y++)
        {
            for (x = y % 2; x < image.GetWidth(); x += 2) 
            {
                unsigned char red = image.GetRed(x,y);
                unsigned char green = image.GetGreen(x,y);
                unsigned char blue = image.GetBlue(x,y);
                if (!has_mask || red != mask_red || green != mask_green || blue != mask_blue)
                {
                    red = (((wxInt32) red  - bg_red) >> 1) + bg_red;
                    green = (((wxInt32) green  - bg_green) >> 1) + bg_green;
                    blue = (((wxInt32) blue  - bg_blue) >> 1) + bg_blue;
                    image.SetRGB( x, y, red, green, blue );
                }
            }
        }

        tool->SetBitmap2( image.ConvertToBitmap() );
    }
    
    RefreshTool((wxToolBarTool*) tool);
#endif
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
    const wxBitmap& bitmap = (tool->IsEnabled() || !tool->GetBitmap2().Ok()) ? tool->GetBitmap1() : tool->GetBitmap2() ;
    
    if (HasFlag(wxTB_FLAT) && (tool != m_underMouse))
    {
        if (down)
            dc.DrawBitmap( bitmap, tool->m_x+4, tool->m_y+4, TRUE );
        else
            dc.DrawBitmap( bitmap, tool->m_x+3, tool->m_y+3, TRUE );
            
        return;
    }
    
    if (down)
    {
        dc.DrawBitmap( bitmap, tool->m_x+4, tool->m_y+4, TRUE );
        
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
        dc.DrawBitmap( bitmap, tool->m_x+3, tool->m_y+3, TRUE );
        
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
        m_xMargin += 1;   // Cannot help it, but otherwise it look ugly
        
        x = m_xMargin;
        y = 5;
    }
    else
    {
        m_yMargin += 1;   // Cannot help it, but otherwise it look ugly
        
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

    return TRUE;
}

void wxToolBar::OnLeave(wxMouseEvent &event)
{
    if (HasFlag( wxTB_FLAT ))
    {
        wxToolBarTool *oldMouseUnder = m_underMouse;
        if (m_underMouse)
        {
            m_underMouse = NULL;
            RefreshTool( oldMouseUnder );
        }
    }
}

void wxToolBar::OnEnter(wxMouseEvent &event)
{
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
    
    wxToolBarTool *oldMouseUnder = m_underMouse;
    if (HasFlag( wxTB_FLAT))
    {
        if (m_underMouse && (m_underMouse != hit))
        {
            m_underMouse = NULL;
            RefreshTool( oldMouseUnder );
        }
        m_underMouse = hit;
    }
    
    if (event.LeftDown() && (hit) && hit->IsEnabled())
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
        
        // Bad trick...
        m_underMouse = NULL;
        
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
    
    if (HasFlag(wxTB_FLAT))
    {
        if (m_underMouse && (m_underMouse != oldMouseUnder))
            RefreshTool( m_underMouse );
    }
}

