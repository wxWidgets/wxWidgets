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

#if wxUSE_TOOLBAR

#include "wx/frame.h"

#include "glib.h"
#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool       g_blockEventsOnDrag;
extern wxCursor   g_globalCursor;

//-----------------------------------------------------------------------------
// "clicked" (internal from gtk_toolbar)
//-----------------------------------------------------------------------------

static void gtk_toolbar_callback( GtkWidget *WXUNUSED(widget), wxToolBarTool *tool )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (g_blockEventsOnDrag) return;
    if (!tool->m_enabled) return;

    if (tool->m_isToggle)
    { 
        tool->m_toggleState = !tool->m_toggleState;
	
	if (tool->m_bitmap2.Ok())
	{
	    wxBitmap bitmap = tool->m_bitmap1;
	    if (tool->m_toggleState) bitmap = tool->m_bitmap2;
	    
            GtkPixmap *pixmap = GTK_PIXMAP( tool->m_pixmap );
	    
            GdkBitmap *mask = (GdkBitmap *) NULL;
            if (bitmap.GetMask()) mask = bitmap.GetMask()->GetBitmap();
  
            gtk_pixmap_set( pixmap, bitmap.GetPixmap(), mask );
	}
    }

    tool->m_owner->OnLeftClick( tool->m_index, tool->m_toggleState );
}

//-----------------------------------------------------------------------------
// "enter_notify_event"
//-----------------------------------------------------------------------------

static gint gtk_toolbar_enter_callback( GtkWidget *WXUNUSED(widget), 
  GdkEventCrossing *WXUNUSED(gdk_event), wxToolBarTool *tool )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (g_blockEventsOnDrag) return TRUE;
    
    
    wxToolBar *tb = tool->m_owner;
    
#if (GTK_MINOR_VERSION == 0)
    /* we grey-out the tip text of disabled tool in GTK 1.0 */
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
#endif
    
    /* emit the event */
  
    tb->OnMouseEnter( tool->m_index );
  
    return FALSE;
}

//-----------------------------------------------------------------------------
// wxToolBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToolBar,wxControl)

BEGIN_EVENT_TABLE(wxToolBar, wxControl)
  EVT_IDLE(wxToolBar::OnIdle)
END_EVENT_TABLE()

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

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( T("wxToolBar creation failed") );
	return FALSE;
    }

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
        
#if (GTK_MINOR_VERSION > 0)
        if (style & wxTB_FLAT)
            gtk_handle_box_set_shadow_type( GTK_HANDLE_BOX(m_widget), GTK_SHADOW_NONE );
#endif
    }
    else
    {     
        m_widget = GTK_WIDGET(m_toolbar);
    }
                                            
    gtk_toolbar_set_tooltips( GTK_TOOLBAR(m_toolbar), TRUE );
    
#if (GTK_MINOR_VERSION > 0)
    if (style & wxTB_FLAT)
        gtk_toolbar_set_button_relief( GTK_TOOLBAR(m_toolbar), GTK_RELIEF_NONE );
#endif

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
  
#if (GTK_MINOR_VERSION > 0)
    gtk_tooltips_force_window( GTK_TOOLBAR(m_toolbar)->tooltips );

    GtkStyle *g_style = 
      gtk_style_copy(
         gtk_widget_get_style( 
            GTK_TOOLBAR(m_toolbar)->tooltips->tip_window ) );
            
    g_style->bg[GTK_STATE_NORMAL] = *m_bg;
    gtk_widget_set_style( GTK_TOOLBAR(m_toolbar)->tooltips->tip_window, g_style );
#else
    gtk_tooltips_set_colors( GTK_TOOLBAR(m_toolbar)->tooltips, m_bg, m_fg );
#endif

    m_xMargin = 0;
    m_yMargin = 0;
    
    m_parent->DoAddChild( this );
  
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
                 T("invalid bitmap for wxToolBar icon") );

    wxCHECK_MSG( bitmap.GetBitmap() == NULL, (wxToolBarTool *)NULL,
                 T("wxToolBar doesn't support GdkBitmap") );

    wxCHECK_MSG( bitmap.GetPixmap() != NULL, (wxToolBarTool *)NULL,
                 T("wxToolBar::Add needs a wxBitmap") );
  
    GtkWidget *tool_pixmap = (GtkWidget *)NULL;
  
    GdkPixmap *pixmap = bitmap.GetPixmap();

    GdkBitmap *mask = (GdkBitmap *)NULL;
    if ( bitmap.GetMask() )
      mask = bitmap.GetMask()->GetBitmap();
    
    tool_pixmap = gtk_pixmap_new( pixmap, mask );
#if (GTK_MINOR_VERSION > 0)
    gtk_pixmap_set_build_insensitive( GTK_PIXMAP(tool_pixmap), TRUE );
#endif
    
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
                      helpString1.mbc_str(),
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
    wxFAIL_MSG( T("wxToolBar::ClearTools not implemented") );
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
            
#if (GTK_MINOR_VERSION > 0)
            /* we don't disable the tools for GTK 1.0 as the bitmaps don't get
               greyed anyway and this also disables tooltips */
            if (tool->m_item)
                gtk_widget_set_sensitive( tool->m_item, enable );
#endif
                
            return;
        }
        node = node->Next();
    }
  
    wxFAIL_MSG( T("wrong toolbar index") );
}

void wxToolBar::ToggleTool( int toolIndex, bool toggle ) 
{
    wxNode *node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool*)node->Data();
        if (tool->m_index == toolIndex)
        { 
            if ((tool->m_item) && (GTK_IS_TOGGLE_BUTTON(tool->m_item)))
	    {
                tool->m_toggleState = toggle;
		
	        if (tool->m_bitmap2.Ok())
	        {
	            wxBitmap bitmap = tool->m_bitmap1;
	            if (tool->m_toggleState) bitmap = tool->m_bitmap2;
	    
                    GtkPixmap *pixmap = GTK_PIXMAP( tool->m_pixmap );
	    
                    GdkBitmap *mask = (GdkBitmap *) NULL;
                    if (bitmap.GetMask()) mask = bitmap.GetMask()->GetBitmap();
  
                    gtk_pixmap_set( pixmap, bitmap.GetPixmap(), mask );
	        }
		
                gtk_signal_disconnect_by_func( GTK_OBJECT(tool->m_item), 
                  GTK_SIGNAL_FUNC(gtk_toolbar_callback), (gpointer*)tool );
		
                gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(tool->m_item), toggle );
		
                gtk_signal_connect( GTK_OBJECT(tool->m_item), "clicked",
                  GTK_SIGNAL_FUNC(gtk_toolbar_callback), (gpointer*)tool );
	    }

            return;
        }
        node = node->Next();
    }
  
    wxFAIL_MSG( T("wrong toolbar index") );
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
  
    wxFAIL_MSG( T("wrong toolbar index") );
  
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
  
    wxFAIL_MSG( T("wrong toolbar index") );
  
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
  
    wxFAIL_MSG( T("wrong toolbar index") );
  
    return FALSE;
}

void wxToolBar::SetMargins( int x, int y )
{
    wxCHECK_RET( !m_hasToolAlready, T("wxToolBar::SetMargins must be called before adding tool.") );
    
    if (x > 2) gtk_toolbar_append_space( m_toolbar );  // oh well
    
    m_xMargin = x;
    m_yMargin = y;
}

void wxToolBar::SetToolPacking( int WXUNUSED(packing) )
{
    wxFAIL_MSG( T("wxToolBar::SetToolPacking not implemented") );
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
  
    wxFAIL_MSG( T("wrong toolbar index") );
    
    return T("");
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
  
    wxFAIL_MSG( T("wrong toolbar index") );
    
    return T("");
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
  
    wxFAIL_MSG( T("wrong toolbar index") );
    
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
  
    wxFAIL_MSG( T("wrong toolbar index") );
    
    return;
}

void wxToolBar::OnIdle( wxIdleEvent &WXUNUSED(ievent) )
{
    wxEvtHandler* evtHandler = GetEventHandler();

    wxNode* node = m_tools.First();
    while (node)
    {
        wxToolBarTool* tool = (wxToolBarTool*) node->Data();

        wxUpdateUIEvent event( tool->m_index );
        event.SetEventObject(this);

        if (evtHandler->ProcessEvent( event ))
        {
            if (event.GetSetEnabled())
                EnableTool(tool->m_index, event.GetEnabled());
            if (event.GetSetChecked())
                ToggleTool(tool->m_index, event.GetChecked());
/*
            if (event.GetSetText())
                // Set tooltip?
*/
        }

        node = node->Next();
    }
}

void wxToolBar::OnInternalIdle()
{
    wxCursor cursor = m_cursor;
    if (g_globalCursor.Ok()) cursor = g_globalCursor;

    if (cursor.Ok() && m_currentGdkCursor != cursor)
    {
        wxCursor oldGdkCursor = m_currentGdkCursor;
        m_currentGdkCursor = cursor;
	
        wxNode *node = m_tools.First();
	while (node)
        {
	    wxToolBarTool *tool = (wxToolBarTool*)node->Data();
	    if (!tool->m_item->window)
	    {
	        /* windows not yet realized. come back later. */
                m_currentGdkCursor = oldGdkCursor;
		break;
	    }
	    else
	    {
	        gdk_window_set_cursor( tool->m_item->window, m_currentGdkCursor.GetCursor() );
	    }
	    node = node->Next();
        }
    }

    UpdateWindowUI();
}

#endif
