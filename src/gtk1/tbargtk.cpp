/////////////////////////////////////////////////////////////////////////////
// Name:        tbargtk.cpp
// Purpose:     GTK toolbar
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "tbargtk.h"
#endif

#include "wx/toolbar.h"
#include "wx/frame.h"

#include "glib.h"
#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "clicked" (internal from gtk_toolbar)
//-----------------------------------------------------------------------------

static void gtk_toolbar_callback( GtkWidget *WXUNUSED(widget), wxToolBarTool *tool )
{
    if (g_blockEventsOnDrag) return;
    if (!tool->m_enabled) return;

    if (tool->m_isToggle) tool->m_toggleState = !tool->m_toggleState;

    tool->m_owner->OnLeftClick( tool->m_index, tool->m_toggleState );
}

//-----------------------------------------------------------------------------
// "enter_notify_event"
//-----------------------------------------------------------------------------

static gint gtk_toolbar_enter_callback( GtkWidget *WXUNUSED(widget), 
  GdkEventCrossing *WXUNUSED(gdk_event), wxToolBarTool *tool )
{
    if (g_blockEventsOnDrag) return TRUE;
    
    /* we grey-out the tip text of disabled tool */
    
    wxToolBar *tb = tool->m_owner;
    
    if (tool->m_enabled)
    {
        if (tb->m_fg->red != 0)
	{
            tb->m_fg->red = 0;
            tb->m_fg->green = 0;
            tb->m_fg->blue = 0;
            gdk_color_alloc( gtk_widget_get_colormap( GTK_WIDGET(tb->m_toolbar) ), tb->m_fg );
            gtk_tooltips_set_colors( GTK_TOOLBAR(tb->m_toolbar)->tooltips, tb->m_bg, tb->m_fg );
	}
    }
    else
    {
        if (tb->m_fg->red == 0)
	{
            tb->m_fg->red = 33000;
            tb->m_fg->green = 33000;
            tb->m_fg->blue = 33000;
            gdk_color_alloc( gtk_widget_get_colormap( GTK_WIDGET(tb->m_toolbar) ), tb->m_fg );
            gtk_tooltips_set_colors( GTK_TOOLBAR(tb->m_toolbar)->tooltips, tb->m_bg, tb->m_fg );
	}
    }
    
    /* emit the event */
  
    tb->OnMouseEnter( tool->m_index );
  
    return FALSE;
}

//-----------------------------------------------------------------------------
// wxToolBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToolBar,wxControl)

wxToolBar::wxToolBar()
{
}

wxToolBar::wxToolBar( wxWindow *parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size,
  long style, const wxString& name )
{
    Create( parent, id, pos, size, style, name );
}

wxToolBar::~wxToolBar()
{
    delete m_fg;
    delete m_bg;
}

bool wxToolBar::Create( wxWindow *parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size,
  long style, const wxString& name )
{
    m_needParent = TRUE;

    PreCreation( parent, id, pos, size, style, name );

    m_tools.DeleteContents( TRUE );

    m_toolbar = GTK_TOOLBAR( gtk_toolbar_new( GTK_ORIENTATION_HORIZONTAL,
                                              GTK_TOOLBAR_ICONS ) );

    m_separation = 5;
    gtk_toolbar_set_space_size( m_toolbar, m_separation );
    m_hasToolAlready = FALSE;

    if (style & wxTB_DOCKABLE)
    {
        m_widget = gtk_handle_box_new();
	gtk_container_add( GTK_CONTAINER(m_widget), GTK_WIDGET(m_toolbar) );
	gtk_widget_show( GTK_WIDGET(m_toolbar) );
    }
    else
    {     
        m_widget = GTK_WIDGET(m_toolbar);
    }
					    
    gtk_toolbar_set_tooltips( GTK_TOOLBAR(m_toolbar), TRUE );

    m_fg = new GdkColor;
    m_fg->red = 0;
    m_fg->green = 0;
    m_fg->blue = 0;
    gdk_color_alloc( gtk_widget_get_colormap( GTK_WIDGET(m_toolbar) ), m_fg );
  
    m_bg = new GdkColor;
    m_bg->red = 65535;
    m_bg->green = 65535;
    m_bg->blue = 50000;
    gdk_color_alloc( gtk_widget_get_colormap( GTK_WIDGET(m_toolbar) ), m_bg );
  
    gtk_tooltips_set_colors( GTK_TOOLBAR(m_toolbar)->tooltips, m_bg, m_fg );

    m_xMargin = 0;
    m_yMargin = 0;
    
    m_parent->AddChild( this );

    (m_parent->m_insertCallback)( m_parent, this );
  
    PostCreation();

    Show( TRUE );

    return TRUE;
}

bool wxToolBar::OnLeftClick( int toolIndex, bool toggleDown )
{
    wxCommandEvent event( wxEVT_COMMAND_TOOL_CLICKED, toolIndex );
    event.SetEventObject(this);
    event.SetInt( toolIndex );
    event.SetExtraLong((long) toggleDown);

    // First try sending the command to a window that has the focus, within a frame that
    // also contains this toolbar.
    wxFrame* frame = (wxFrame*) NULL;
    wxWindow* win = this;
    wxWindow* focusWin = (wxWindow*) NULL;

    while (win)
    {
        if (win->IsKindOf(CLASSINFO(wxFrame)))
        {
            frame = (wxFrame*) win;
            break;
        }
        else
            win = win->GetParent();
    }
    if (frame)
        focusWin = wxFindFocusDescendant(frame);

    if (focusWin && focusWin->GetEventHandler()->ProcessEvent(event))
        return TRUE;

    GetEventHandler()->ProcessEvent(event);

    return TRUE;
}

void wxToolBar::OnRightClick( int toolIndex, float WXUNUSED(x), float WXUNUSED(y) )
{
    wxCommandEvent event( wxEVT_COMMAND_TOOL_RCLICKED, toolIndex );
    event.SetEventObject( this );
    event.SetInt( toolIndex );

    GetEventHandler()->ProcessEvent(event);
}

void wxToolBar::OnMouseEnter( int toolIndex )
{
    wxCommandEvent event( wxEVT_COMMAND_TOOL_ENTER, GetId() );
    event.SetEventObject(this);
    event.SetInt( toolIndex );
  
    GetEventHandler()->ProcessEvent(event);
}

wxToolBarTool *wxToolBar::AddTool( int toolIndex, const wxBitmap& bitmap,
  const wxBitmap& pushedBitmap, bool toggle,
  float WXUNUSED(xPos), float WXUNUSED(yPos), wxObject *clientData,
  const wxString& helpString1, const wxString& helpString2 )
{
    m_hasToolAlready = TRUE;
    
    wxCHECK_MSG( bitmap.Ok(), (wxToolBarTool *)NULL,
                 "invalid bitmap for wxToolBar icon" );

    wxCHECK_MSG( bitmap.GetBitmap() == NULL, (wxToolBarTool *)NULL,
                 "wxToolBar doesn't support GdkBitmap" );

    wxCHECK_MSG( bitmap.GetPixmap() != NULL, (wxToolBarTool *)NULL,
                 "wxToolBar::Add needs a wxBitmap" );
  
    GtkWidget *tool_pixmap = (GtkWidget *)NULL;
  
    GdkPixmap *pixmap = bitmap.GetPixmap();

    GdkBitmap *mask = (GdkBitmap *)NULL;
    if ( bitmap.GetMask() )
      mask = bitmap.GetMask()->GetBitmap();
    
    tool_pixmap = gtk_pixmap_new( pixmap, mask );
  
    gtk_misc_set_alignment( GTK_MISC(tool_pixmap), 0.5, 0.5 );

    wxToolBarTool *tool = new wxToolBarTool( this, toolIndex, bitmap, pushedBitmap,
                                             toggle, clientData,
                                             helpString1, helpString2,
					     tool_pixmap );

    GtkToolbarChildType ctype = toggle ? GTK_TOOLBAR_CHILD_TOGGLEBUTTON
                                       : GTK_TOOLBAR_CHILD_BUTTON;

    GtkWidget *item = gtk_toolbar_append_element
                     (
                      GTK_TOOLBAR(m_toolbar),
                      ctype,
                      (GtkWidget *)NULL,
                      (const char *)NULL,
                      helpString1,
                      "",
                      tool_pixmap,
                      (GtkSignalFunc)gtk_toolbar_callback,
                      (gpointer)tool
                     );

    tool->m_item = item;

    gtk_signal_connect( GTK_OBJECT(tool->m_item),
                        "enter_notify_event", 
                        GTK_SIGNAL_FUNC(gtk_toolbar_enter_callback),
                        (gpointer)tool );

    m_tools.Append( tool );

    return tool;
}

void wxToolBar::AddSeparator()
{
    gtk_toolbar_append_space( m_toolbar );
}

void wxToolBar::ClearTools()
{
    wxFAIL_MSG( "wxToolBar::ClearTools not implemented" );
}

bool wxToolBar::Realize()
{
    m_x = 0;
    m_y = 0;
    m_width = 100;
    m_height = 0;
  
    wxNode *node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool*)node->Data();
        if (tool->m_bitmap1.Ok())
        {
            int tool_height = tool->m_bitmap1.GetHeight();
            if (tool_height > m_height) m_height = tool_height;
        }
    
        node = node->Next();
    }
  
    m_height += 5 + 2*m_yMargin;
   
    return TRUE;
}

void wxToolBar::EnableTool(int toolIndex, bool enable)
{
    wxNode *node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool*)node->Data();
        if (tool->m_index == toolIndex)
        {
            tool->m_enabled = enable;
	    
/*   we don't disable the tools for now as the bitmaps don't get
     greyed anyway and this also disables tooltips

	    if (tool->m_item)
	        gtk_widget_set_sensitive( tool->m_item, enable );
*/
		
            return;
        }
        node = node->Next();
    }
  
    wxFAIL_MSG( "wrong toolbar index" );
}

void wxToolBar::ToggleTool( int toolIndex, bool toggle ) 
{
    wxNode *node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool*)node->Data();
        if (tool->m_index == toolIndex)
        { 
            tool->m_toggleState = toggle;
            if ((tool->m_item) && (GTK_IS_TOGGLE_BUTTON(tool->m_item)))
                gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(tool->m_item), toggle );
            return;
        }
        node = node->Next();
    }
  
    wxFAIL_MSG( "wrong toolbar index" );
}

wxObject *wxToolBar::GetToolClientData( int index ) const
{
    wxNode *node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool*)node->Data();
        if (tool->m_index == index) return tool->m_clientData;;
        node = node->Next();
    }
  
    wxFAIL_MSG( "wrong toolbar index" );
  
    return (wxObject*)NULL;
}

bool wxToolBar::GetToolState(int toolIndex) const
{
    wxNode *node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool*)node->Data();
        if (tool->m_index == toolIndex) return tool->m_toggleState;
        node = node->Next();
    }
  
    wxFAIL_MSG( "wrong toolbar index" );
  
    return FALSE;
}

bool wxToolBar::GetToolEnabled(int toolIndex) const
{
    wxNode *node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool*)node->Data();
        if (tool->m_index == toolIndex) return tool->m_enabled;
        node = node->Next();
    }
  
    wxFAIL_MSG( "wrong toolbar index" );
  
    return FALSE;
}

void wxToolBar::SetMargins( int x, int y )
{
    wxCHECK_RET( !m_hasToolAlready, "wxToolBar::SetMargins must be called before adding tool." );
    
    if (x > 2) gtk_toolbar_append_space( m_toolbar );  // oh well
    
    m_xMargin = x;
    m_yMargin = y;
}

void wxToolBar::SetToolPacking( int WXUNUSED(packing) )
{
    wxFAIL_MSG( "wxToolBar::SetToolPacking not implemented" );
}

void wxToolBar::SetToolSeparation( int separation )
{
    gtk_toolbar_set_space_size( m_toolbar, separation );
    m_separation = separation;
}

int wxToolBar::GetToolPacking()
{
    return 0;
}

int wxToolBar::GetToolSeparation()
{
    return m_separation;
}

wxString wxToolBar::GetToolLongHelp(int toolIndex)
{
    wxNode *node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool*)node->Data();
        if (tool->m_index == toolIndex)
        { 
            return tool->m_longHelpString;
        }
        node = node->Next();
    }
  
    wxFAIL_MSG( "wrong toolbar index" );
    
    return "";
}

wxString wxToolBar::GetToolShortHelp(int toolIndex)
{
    wxNode *node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool*)node->Data();
        if (tool->m_index == toolIndex)
        { 
            return tool->m_shortHelpString;
        }
        node = node->Next();
    }
  
    wxFAIL_MSG( "wrong toolbar index" );
    
    return "";
}

void wxToolBar::SetToolLongHelp(int toolIndex, const wxString& helpString)
{
    wxNode *node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool*)node->Data();
        if (tool->m_index == toolIndex)
        { 
            tool->m_longHelpString = helpString;
	    return;
        }
        node = node->Next();
    }
  
    wxFAIL_MSG( "wrong toolbar index" );
    
    return;
}

void wxToolBar::SetToolShortHelp(int toolIndex, const wxString& helpString)
{
    wxNode *node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool*)node->Data();
        if (tool->m_index == toolIndex)
        { 
            tool->m_shortHelpString = helpString;
	    return;
        }
        node = node->Next();
    }
  
    wxFAIL_MSG( "wrong toolbar index" );
    
    return;
}


