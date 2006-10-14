/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/tbargtk.cpp
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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_TOOLBAR_NATIVE

#include "wx/toolbar.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
#endif

// FIXME: Use GtkImage instead of GtkPixmap. Use the new toolbar API for when gtk runtime is new enough?
// Beware that the new and old toolbar API may not be mixed in usage.
#include <gtk/gtkversion.h>
#ifdef GTK_DISABLE_DEPRECATED
#undef GTK_DISABLE_DEPRECATED
#endif

#include "wx/gtk/private.h"

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// data
extern bool       g_blockEventsOnDrag;
extern wxCursor   g_globalCursor;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// translate wxWidgets toolbar style flags to GTK orientation and style
static void GetGtkStyle(long style,
                        GtkOrientation *orient, GtkToolbarStyle *gtkStyle)
{
    *orient = style & wxTB_VERTICAL ? GTK_ORIENTATION_VERTICAL
                                    : GTK_ORIENTATION_HORIZONTAL;


    if ( style & wxTB_TEXT )
    {
        *gtkStyle = style & wxTB_NOICONS
                        ? GTK_TOOLBAR_TEXT
                        : (
                          style & wxTB_HORZ_LAYOUT ? GTK_TOOLBAR_BOTH_HORIZ :
                          GTK_TOOLBAR_BOTH);
    }
    else // no text, hence we must have the icons or what would we show?
    {
        *gtkStyle = GTK_TOOLBAR_ICONS;
    }
}

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar,
                  int id,
                  const wxString& label,
                  const wxBitmap& bitmap1,
                  const wxBitmap& bitmap2,
                  wxItemKind kind,
                  wxObject *clientData,
                  const wxString& shortHelpString,
                  const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, label, bitmap1, bitmap2, kind,
                            clientData, shortHelpString, longHelpString)
    {
        Init();
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control)
        : wxToolBarToolBase(tbar, control)
    {
        Init();
    }

    // is this a radio button?
    //
    // unlike GetKind(), can be called for any kind of tools, not just buttons
    bool IsRadio() const { return IsButton() && GetKind() == wxITEM_RADIO; }

    // this is only called for the normal buttons, i.e. not separators nor
    // controls
    GtkToolbarChildType GetGtkChildType() const
    {
        switch ( GetKind() )
        {
            case wxITEM_CHECK:
                return GTK_TOOLBAR_CHILD_TOGGLEBUTTON;

            case wxITEM_RADIO:
                return GTK_TOOLBAR_CHILD_RADIOBUTTON;

            default:
                wxFAIL_MSG( _T("unknown toolbar child type") );
                // fall through

            case wxITEM_NORMAL:
                return GTK_TOOLBAR_CHILD_BUTTON;
        }
    }

    void SetImage(const wxBitmap& bitmap)
    {
        if (bitmap.Ok())
        {
            // setting from pixmap doesn't seem to work right, but pixbuf works well
            gtk_image_set_from_pixbuf((GtkImage*)m_image, bitmap.GetPixbuf());
        }
    }

    GtkWidget            *m_item;
    GtkWidget            *m_image;

protected:
    void Init();
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl)

// ============================================================================
// implementation
// ============================================================================

//-----------------------------------------------------------------------------
// "clicked" (internal from gtk_toolbar)
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_toolbar_callback( GtkWidget *WXUNUSED(widget),
                                  wxToolBarTool *tool )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    wxToolBar *tbar = (wxToolBar *)tool->GetToolBar();

    if (tbar->m_blockEvent) return;

    if (g_blockEventsOnDrag) return;
    if (!tool->IsEnabled()) return;

    if (tool->CanBeToggled())
    {
        tool->Toggle();

        tool->SetImage(tool->GetBitmap());

        if ( tool->IsRadio() && !tool->IsToggled() )
        {
            // radio button went up, don't report this as a wxWin event
            return;
        }
    }

    if( !tbar->OnLeftClick( tool->GetId(), tool->IsToggled() ) && tool->CanBeToggled() )
    {
        // revert back
        tool->Toggle();

        tool->SetImage(tool->GetBitmap());
    }
}
}

//-----------------------------------------------------------------------------
// "enter_notify_event" / "leave_notify_event"
//-----------------------------------------------------------------------------

extern "C" {
static gint gtk_toolbar_tool_callback( GtkWidget *WXUNUSED(widget),
                                       GdkEventCrossing *gdk_event,
                                       wxToolBarTool *tool )
{
    // don't need to install idle handler, its done from "event" signal

    if (g_blockEventsOnDrag) return TRUE;

    wxToolBar *tb = (wxToolBar *)tool->GetToolBar();

    // emit the event
    if( gdk_event->type == GDK_ENTER_NOTIFY )
        tb->OnMouseEnter( tool->GetId() );
    else
        tb->OnMouseEnter( -1 );

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// InsertChild callback for wxToolBar
//-----------------------------------------------------------------------------

static void wxInsertChildInToolBar( wxToolBar* WXUNUSED(parent),
                                    wxWindow* WXUNUSED(child) )
{
    // we don't do anything here
}

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

void wxToolBarTool::Init()
{
    m_item =
    m_image = NULL;
}

wxToolBarToolBase *wxToolBar::CreateTool(int id,
                                         const wxString& text,
                                         const wxBitmap& bitmap1,
                                         const wxBitmap& bitmap2,
                                         wxItemKind kind,
                                         wxObject *clientData,
                                         const wxString& shortHelpString,
                                         const wxString& longHelpString)
{
    return new wxToolBarTool(this, id, text, bitmap1, bitmap2, kind,
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
    m_toolbar = (GtkToolbar *)NULL;
    m_blockEvent = false;
    m_defaultWidth = 32;
    m_defaultHeight = 32;
}

wxToolBar::~wxToolBar()
{
}

bool wxToolBar::Create( wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name )
{
    m_needParent = true;
    m_insertCallback = (wxInsertChildFunction)wxInsertChildInToolBar;

    if ( !PreCreation( parent, pos, size ) ||
         !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxToolBar creation failed") );

        return false;
    }

    m_toolbar = GTK_TOOLBAR( gtk_toolbar_new() );
    GtkSetStyle();

    // Doesn't work this way.
    // GtkToolbarSpaceStyle space_style = GTK_TOOLBAR_SPACE_EMPTY;
    // gtk_widget_style_set (GTK_WIDGET (m_toolbar), "space_style", &space_style, NULL);

    SetToolSeparation(7);

    if (style & wxTB_DOCKABLE)
    {
        m_widget = gtk_handle_box_new();
        gtk_container_add( GTK_CONTAINER(m_widget), GTK_WIDGET(m_toolbar) );
        gtk_widget_show( GTK_WIDGET(m_toolbar) );

        if (style & wxTB_FLAT)
            gtk_handle_box_set_shadow_type( GTK_HANDLE_BOX(m_widget), GTK_SHADOW_NONE );
    }
    else
    {
        m_widget = gtk_event_box_new();
        gtk_container_add( GTK_CONTAINER(m_widget), GTK_WIDGET(m_toolbar) );
        ConnectWidget( m_widget );
        gtk_widget_show(GTK_WIDGET(m_toolbar));
    }

    // FIXME: there is no such function for toolbars in 2.0
#if 0
    if (style & wxTB_FLAT)
        gtk_toolbar_set_button_relief( GTK_TOOLBAR(m_toolbar), GTK_RELIEF_NONE );
#endif

    m_parent->DoAddChild( this );

    PostCreation(size);

    return true;
}

GdkWindow *wxToolBar::GTKGetWindow(wxArrayGdkWindows& windows) const
{
    return GTK_WIDGET(m_toolbar)->window;
}

void wxToolBar::GtkSetStyle()
{
    GtkOrientation orient;
    GtkToolbarStyle style;
    GetGtkStyle(GetWindowStyle(), &orient, &style);

    gtk_toolbar_set_orientation(m_toolbar, orient);
    gtk_toolbar_set_style(m_toolbar, style);
    gtk_toolbar_set_tooltips( GTK_TOOLBAR(m_toolbar), !(style & wxTB_NO_TOOLTIPS) );
}

void wxToolBar::SetWindowStyleFlag( long style )
{
    wxToolBarBase::SetWindowStyleFlag(style);

    if ( m_toolbar )
        GtkSetStyle();
}

bool wxToolBar::DoInsertTool(size_t pos, wxToolBarToolBase *toolBase)
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    size_t posGtk = pos;

    if ( tool->IsButton() )
    {
        if ( !HasFlag(wxTB_NOICONS) )
        {
            wxBitmap bitmap = tool->GetNormalBitmap();

            wxCHECK_MSG( bitmap.Ok(), false,
                         wxT("invalid bitmap for wxToolBar icon") );

            wxCHECK_MSG( bitmap.GetDepth() != 1, false,
                         wxT("wxToolBar doesn't support GdkBitmap") );

            wxCHECK_MSG( bitmap.GetPixmap() != NULL, false,
                         wxT("wxToolBar::Add needs a wxBitmap") );

            tool->m_image = gtk_image_new();
            tool->SetImage(bitmap);

            gtk_misc_set_alignment((GtkMisc*)tool->m_image, 0.5, 0.5);
        }
    }

    switch ( tool->GetStyle() )
    {
        case wxTOOL_STYLE_BUTTON:
            // for a radio button we need the widget which starts the radio
            // group it belongs to, i.e. the first radio button immediately
            // preceding this one
            {
                GtkWidget *widget = NULL;

                if ( tool->IsRadio() )
                {
                    wxToolBarToolsList::compatibility_iterator node
                        = wxToolBarToolsList::compatibility_iterator();
                    if ( pos )
                        node = m_tools.Item(pos - 1);

                    while ( node )
                    {
                        wxToolBarTool *toolNext = (wxToolBarTool *)node->GetData();
                        if ( !toolNext->IsRadio() )
                            break;

                        widget = toolNext->m_item;

                        node = node->GetPrevious();
                    }

                    if ( !widget )
                    {
                        // this is the first button in the radio button group,
                        // it will be toggled automatically by GTK so bring the
                        // internal flag in sync
                        tool->Toggle(true);
                    }
                }

                tool->m_item = gtk_toolbar_insert_element
                               (
                                  m_toolbar,
                                  tool->GetGtkChildType(),
                                  widget,
                                  tool->GetLabel().empty()
                                    ? NULL
                                    : (const char*) wxGTK_CONV( tool->GetLabel() ),
                                  tool->GetShortHelp().empty()
                                    ? NULL
                                    : (const char*) wxGTK_CONV( tool->GetShortHelp() ),
                                  "", // tooltip_private_text (?)
                                  tool->m_image,
                                  (GtkSignalFunc)gtk_toolbar_callback,
                                  (gpointer)tool,
                                  posGtk
                               );

                wxCHECK_MSG(tool->m_item != NULL, false, _T("gtk_toolbar_insert_element() failed"));

                g_signal_connect (tool->m_item, "enter_notify_event",
                                  G_CALLBACK (gtk_toolbar_tool_callback),
                                  tool);
                g_signal_connect (tool->m_item, "leave_notify_event",
                                  G_CALLBACK (gtk_toolbar_tool_callback),
                                  tool);
            }
            break;

        case wxTOOL_STYLE_SEPARATOR:
            gtk_toolbar_insert_space( m_toolbar, posGtk );

            // skip the rest
            return true;

        case wxTOOL_STYLE_CONTROL:
            gtk_toolbar_insert_widget(
                                       m_toolbar,
                                       tool->GetControl()->m_widget,
                                       (const char *) NULL,
                                       (const char *) NULL,
                                       posGtk
                                      );
            break;
    }

    GtkRequisition req;
    (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(m_widget) )->size_request )
        (m_widget, &req );
    m_width = req.width + m_xMargin;
    m_height = req.height + 2*m_yMargin;
    InvalidateBestSize();

    return true;
}

bool wxToolBar::DoDeleteTool(size_t pos, wxToolBarToolBase *toolBase)
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

        case wxTOOL_STYLE_SEPARATOR:
            gtk_toolbar_remove_space( m_toolbar, pos );
            break;
    }

    InvalidateBestSize();
    return true;
}

// ----------------------------------------------------------------------------
// wxToolBar tools state
// ----------------------------------------------------------------------------

void wxToolBar::DoEnableTool(wxToolBarToolBase *toolBase, bool enable)
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    if (tool->m_item)
    {
        gtk_widget_set_sensitive( tool->m_item, enable );
    }
}

void wxToolBar::DoToggleTool( wxToolBarToolBase *toolBase, bool toggle )
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    GtkWidget *item = tool->m_item;
    if ( item && GTK_IS_TOGGLE_BUTTON(item) )
    {
        tool->SetImage(tool->GetBitmap());

        m_blockEvent = true;

        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(item), toggle );

        m_blockEvent = false;
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

    m_xMargin = x;
    m_yMargin = y;
}

void wxToolBar::SetToolSeparation( int separation )
{
    // FIXME: this function disappeared
#if 0
    gtk_toolbar_set_space_size( m_toolbar, separation );
#endif

    m_toolSeparation = separation;
}

void wxToolBar::SetToolShortHelp( int id, const wxString& helpString )
{
    wxToolBarTool *tool = (wxToolBarTool *)FindById(id);

    if ( tool )
    {
        (void)tool->SetShortHelp(helpString);
        gtk_tooltips_set_tip(m_toolbar->tooltips, tool->m_item,
                             wxGTK_CONV( helpString ), "");
    }
}

// ----------------------------------------------------------------------------
// wxToolBar idle handling
// ----------------------------------------------------------------------------

void wxToolBar::OnInternalIdle()
{
    // Check if we have to show window now
    if (GtkShowFromOnIdle()) return;
    
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

        wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
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

    if (wxUpdateUIEvent::CanUpdate(this))
        UpdateWindowUI(wxUPDATE_UI_FROMIDLE);
}


// ----------------------------------------------------------------------------

// static
wxVisualAttributes
wxToolBar::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_toolbar_new);
}

#endif // wxUSE_TOOLBAR_NATIVE
