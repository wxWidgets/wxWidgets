/////////////////////////////////////////////////////////////////////////////
// Name:        tbargtk.cpp
// Purpose:     GTK toolbar
// Author:      Robert Roebling
// Modified:    13.12.99 by VZ to derive from wxToolBarBase
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "tbargtk.h"
#endif

#include "wx/toolbar.h"

#if wxUSE_TOOLBAR_NATIVE

#include "wx/frame.h"

#include "glib.h"
#include "gdk/gdk.h"
#include "gtk/gtk.h"

extern GdkFont *GtkGetDefaultGuiFont();

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// idle system
extern void wxapp_install_idle_handler();
extern bool g_isIdle;

// data
extern bool       g_blockEventsOnDrag;
extern wxCursor   g_globalCursor;

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar,
                  int id,
                  const wxBitmap& bitmap1,
                  const wxBitmap& bitmap2,
                  bool toggle,
                  wxObject *clientData,
                  const wxString& shortHelpString,
                  const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, bitmap1, bitmap2, toggle,
                            clientData, shortHelpString, longHelpString)
    {
        Init();
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control)
        : wxToolBarToolBase(tbar, control)
    {
        Init();
    }

    GtkWidget            *m_item;
    GtkWidget            *m_pixmap;

protected:
    void Init();
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxToolBarBase)

// ============================================================================
// implementation
// ============================================================================

//-----------------------------------------------------------------------------
// "clicked" (internal from gtk_toolbar)
//-----------------------------------------------------------------------------

static void gtk_toolbar_callback( GtkWidget *WXUNUSED(widget),
                                  wxToolBarTool *tool )
{
    if (g_isIdle) 
        wxapp_install_idle_handler();

    wxToolBar *tbar = (wxToolBar *)tool->GetToolBar();
    if ( tbar->m_blockNextEvent )
    { 
        tbar->m_blockNextEvent = FALSE;
        return;
    }

    if (g_blockEventsOnDrag) return;
    if (!tool->IsEnabled()) return;

    if (tool->CanBeToggled())
    { 
        tool->Toggle();

        wxBitmap bitmap = tool->GetBitmap();
        if ( bitmap.Ok() )
        {
            GtkPixmap *pixmap = GTK_PIXMAP( tool->m_pixmap );

            GdkBitmap *mask = bitmap.GetMask() ? bitmap.GetMask()->GetBitmap()
                                               : (GdkBitmap *)NULL;

            gtk_pixmap_set( pixmap, bitmap.GetPixmap(), mask );
        }
    }

    tbar->OnLeftClick( tool->GetId(), tool->IsToggled() );
}

//-----------------------------------------------------------------------------
// "enter_notify_event"
//-----------------------------------------------------------------------------

static gint gtk_toolbar_enter_callback( GtkWidget *WXUNUSED(widget), 
                                        GdkEventCrossing *WXUNUSED(gdk_event),
                                        wxToolBarTool *tool )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (g_blockEventsOnDrag) return TRUE;
    
    wxToolBar *tb = (wxToolBar *)tool->GetToolBar();
    
#if (GTK_MINOR_VERSION == 0)
    /* we grey-out the tip text of disabled tool in GTK 1.0 */
    if (tool->IsEnabled())
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
  
    tb->OnMouseEnter( tool->GetId() );
  
    return FALSE;
}

//-----------------------------------------------------------------------------
// InsertChild callback for wxToolBar
//-----------------------------------------------------------------------------

static void wxInsertChildInToolBar( wxToolBar* WXUNUSED(parent),
                                    wxWindow* WXUNUSED(child) )
{
    /* we don't do anything here but pray */
}

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

void wxToolBarTool::Init()
{
    m_item =
    m_pixmap = (GtkWidget *)NULL;
}

wxToolBarToolBase *wxToolBar::CreateTool(int id,
                                         const wxBitmap& bitmap1,
                                         const wxBitmap& bitmap2,
                                         bool toggle,
                                         wxObject *clientData,
                                         const wxString& shortHelpString,
                                         const wxString& longHelpString)
{
    return new wxToolBarTool(this, id, bitmap1, bitmap2, toggle,
                             clientData, shortHelpString, longHelpString);
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control)
{
    return new wxToolBarTool(this, control);
}

//-----------------------------------------------------------------------------
// wxToolBar construction
//-----------------------------------------------------------------------------

void wxToolBar::Init()
{
    m_fg =
    m_bg = (GdkColor *)NULL;
    m_toolbar = (GtkToolbar *)NULL;
    m_blockNextEvent = FALSE;
}

wxToolBar::~wxToolBar()
{
    delete m_fg;
    delete m_bg;
}

bool wxToolBar::Create( wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name )
{
    m_needParent = TRUE;
    m_insertCallback = (wxInsertChildFunction)wxInsertChildInToolBar;

    if ( !PreCreation( parent, pos, size ) ||
         !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxToolBar creation failed") );

        return FALSE;
    }

    GtkOrientation orient = style & wxTB_VERTICAL ? GTK_ORIENTATION_VERTICAL
                                                  : GTK_ORIENTATION_HORIZONTAL;
    m_toolbar = GTK_TOOLBAR( gtk_toolbar_new( orient, GTK_TOOLBAR_ICONS ) );

    SetToolSeparation(7);

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

    if (style & wxTB_FLAT)
        gtk_toolbar_set_button_relief( GTK_TOOLBAR(m_toolbar), GTK_RELIEF_NONE );


    m_fg = new GdkColor;
    m_fg->red = 0; 
    m_fg->green = 0; 
    m_fg->blue = 0;
    wxColour fg(0,0,0);
    fg.CalcPixel( gtk_widget_get_colormap( GTK_WIDGET(m_toolbar) ) );
    m_fg->pixel = fg.GetPixel();
    
    m_bg = new GdkColor;
    m_bg->red = 65535;
    m_bg->green = 65535;
    m_bg->blue = 49980;
    wxColour bg(255,255,196);
    bg.CalcPixel( gtk_widget_get_colormap( GTK_WIDGET(m_toolbar) ) );
    m_bg->pixel = bg.GetPixel();
    
    gtk_tooltips_force_window( GTK_TOOLBAR(m_toolbar)->tooltips );

    GtkStyle *g_style = 
        gtk_style_copy(
                gtk_widget_get_style( 
                    GTK_TOOLBAR(m_toolbar)->tooltips->tip_window ) );

    g_style->bg[GTK_STATE_NORMAL] = *m_bg;
    gdk_font_unref( g_style->font );
	g_style->font = gdk_font_ref( GtkGetDefaultGuiFont() );
    gtk_widget_set_style( GTK_TOOLBAR(m_toolbar)->tooltips->tip_window, g_style );

    m_parent->DoAddChild( this );

    PostCreation();

    Show( TRUE );

    return TRUE;
}

bool wxToolBar::DoInsertTool(size_t pos, wxToolBarToolBase *toolBase)
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    // we have inserted a space before all the tools
    if (m_xMargin > 1) pos++;
    
    if ( tool->IsButton() )
    {
        wxBitmap bitmap = tool->GetBitmap1();

        wxCHECK_MSG( bitmap.Ok(), FALSE,
                     wxT("invalid bitmap for wxToolBar icon") );

        wxCHECK_MSG( bitmap.GetBitmap() == NULL, FALSE,
                     wxT("wxToolBar doesn't support GdkBitmap") );

        wxCHECK_MSG( bitmap.GetPixmap() != NULL, FALSE,
                     wxT("wxToolBar::Add needs a wxBitmap") );
      
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

        tool->m_pixmap = tool_pixmap;
    }

    switch ( tool->GetStyle() )
    {
        case wxTOOL_STYLE_BUTTON:
            tool->m_item = gtk_toolbar_insert_element
                           (
                              m_toolbar,
                              tool->CanBeToggled()
                                ? GTK_TOOLBAR_CHILD_TOGGLEBUTTON
                                : GTK_TOOLBAR_CHILD_BUTTON,
                              (GtkWidget *)NULL,
                              (const char *)NULL,
                              tool->GetShortHelp().mbc_str(),
                              "", // tooltip_private_text (?)
                              tool->m_pixmap,
                              (GtkSignalFunc)gtk_toolbar_callback,
                              (gpointer)tool,
                              pos
                           );

            if ( !tool->m_item )
            {
                wxFAIL_MSG( _T("gtk_toolbar_insert_element() failed") );

                return FALSE;
            }

            gtk_signal_connect( GTK_OBJECT(tool->m_item),
                                "enter_notify_event", 
                                GTK_SIGNAL_FUNC(gtk_toolbar_enter_callback),
                                (gpointer)tool );
            break;

        case wxTOOL_STYLE_SEPARATOR:
            gtk_toolbar_append_space( m_toolbar );

            // skip the rest
            return TRUE;

        case wxTOOL_STYLE_CONTROL:
            gtk_toolbar_insert_widget(
                                       m_toolbar,
                                       tool->GetControl()->m_widget,
                                       (const char *) NULL,
                                       (const char *) NULL,
                                       pos
                                      );
            break;
    }

    GtkRequisition req;
    (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(m_widget) )->size_request )
        (m_widget, &req );
    m_width = req.width + m_xMargin;
    m_height = req.height + 2*m_yMargin;

    return TRUE;
}

bool wxToolBar::DoDeleteTool(size_t WXUNUSED(pos), wxToolBarToolBase *toolBase)
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    switch ( tool->GetStyle() )
    {
        case wxTOOL_STYLE_CONTROL:
            tool->GetControl()->Destroy();
            break;

        case wxTOOL_STYLE_BUTTON:
            gtk_widget_destroy( tool->m_item );
            break;

        //case wxTOOL_STYLE_SEPARATOR: -- nothing to do
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxToolBar tools state
// ----------------------------------------------------------------------------

void wxToolBar::DoEnableTool(wxToolBarToolBase *toolBase, bool enable)
{
#if (GTK_MINOR_VERSION > 0)
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    /* we don't disable the tools for GTK 1.0 as the bitmaps don't get
       greyed anyway and this also disables tooltips */
    if (tool->m_item)
        gtk_widget_set_sensitive( tool->m_item, enable );
#endif
}

void wxToolBar::DoToggleTool( wxToolBarToolBase *toolBase, bool toggle ) 
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    GtkWidget *item = tool->m_item;
    if ( item && GTK_IS_TOGGLE_BUTTON(item) )
    {
        wxBitmap bitmap = tool->GetBitmap();
        if ( bitmap.Ok() )
        {
            GtkPixmap *pixmap = GTK_PIXMAP( tool->m_pixmap );

            GdkBitmap *mask = bitmap.GetMask() ? bitmap.GetMask()->GetBitmap()
                                               : (GdkBitmap *)NULL;

            gtk_pixmap_set( pixmap, bitmap.GetPixmap(), mask );
        }

        m_blockNextEvent = TRUE;  // we cannot use gtk_signal_disconnect here

        gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(item), toggle );
    }
}

void wxToolBar::DoSetToggle(wxToolBarToolBase * WXUNUSED(tool),
                            bool WXUNUSED(toggle))
{
    // VZ: absolutely no idea about how to do it
    wxFAIL_MSG( _T("not implemented") );
}

// ----------------------------------------------------------------------------
// wxToolBar geometry
// ----------------------------------------------------------------------------

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord WXUNUSED(x),
                                                  wxCoord WXUNUSED(y)) const
{
    // VZ: GTK+ doesn't seem to have such thing
    wxFAIL_MSG( _T("wxToolBar::FindToolForPosition() not implemented") );

    return (wxToolBarToolBase *)NULL;
}

void wxToolBar::SetMargins( int x, int y )
{
    wxCHECK_RET( GetToolsCount() == 0,
                 wxT("wxToolBar::SetMargins must be called before adding tools.") );
    
    if (x > 1) gtk_toolbar_append_space( m_toolbar );  // oh well
    
    m_xMargin = x;
    m_yMargin = y;
}

void wxToolBar::SetToolSeparation( int separation )
{
    gtk_toolbar_set_space_size( m_toolbar, separation );
    m_toolSeparation = separation;
}

void wxToolBar::SetToolShortHelp( int id, const wxString& helpString )
{
    wxToolBarTool *tool = (wxToolBarTool *)FindById(id);

    if ( tool )
    {
        (void)tool->SetShortHelp(helpString);
        gtk_tooltips_set_tip(m_toolbar->tooltips, tool->m_item,
                             helpString.mbc_str(), "");
    }
}

// ----------------------------------------------------------------------------
// wxToolBar idle handling
// ----------------------------------------------------------------------------

void wxToolBar::OnInternalIdle()
{
    wxCursor cursor = m_cursor;
    if (g_globalCursor.Ok()) cursor = g_globalCursor;

    if (cursor.Ok())
    {
        /* I now set the cursor the anew in every OnInternalIdle call
           as setting the cursor in a parent window also effects the
           windows above so that checking for the current cursor is
           not possible. */

        if (HasFlag(wxTB_DOCKABLE) && (m_widget->window))
        {
            /* if the toolbar is dockable, then m_widget stands for the
               GtkHandleBox widget, which uses its own window so that we
               can set the cursor for it. if the toolbar is not dockable,
               m_widget comes from m_toolbar which uses its parent's
               window ("windowless windows") and thus we cannot set the
               cursor. */
            gdk_window_set_cursor( m_widget->window, cursor.GetCursor() );
        }

        wxToolBarToolsList::Node *node = m_tools.GetFirst();
        while ( node )
        {
            wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
            node = node->GetNext();

            GtkWidget *item = tool->m_item;
            if ( item )
            {
                GdkWindow *window = item->window;

                if ( window )
                {
                    gdk_window_set_cursor( window, cursor.GetCursor() );
                }
            }
        }
    }

    UpdateWindowUI();
}

#endif // wxUSE_TOOLBAR_NATIVE
