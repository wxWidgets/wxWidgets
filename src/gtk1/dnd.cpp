///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.cpp
// Purpose:     wxDropTarget class
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dnd.h"
#endif

#include "wx/dnd.h"

#if wxUSE_DRAG_AND_DROP

#include "wx/window.h"
#include "wx/app.h"
#include "wx/gdicmn.h"
#include "wx/intl.h"
#include "wx/utils.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"
#include "gdk/gdkprivate.h"

#include <X11/Xlib.h>

//----------------------------------------------------------------------------
// global data
//----------------------------------------------------------------------------

extern bool g_blockEventsOnDrag;

//----------------------------------------------------------------------------
// standard icons
//----------------------------------------------------------------------------

/* XPM */
static char * gv_xpm[] = {
"40 34 3 1",
" 	s None c None",
".	c black",
"X	c white",
"                                        ",
"                                        ",
"                  ......                ",
"                ..XXXXXX..              ",
"               .XXXXXXXXXX.             ",
"              .XXXXXXXXXXXX.            ",
"              .XXXXXXXXXXXX.            ",
"             .XXXXXXXXXXXXXX.           ",
"             .XXX..XXXX..XXX.           ",
"          ....XX....XX....XX.           ",
"         .XXX.XXX..XXXX..XXX....        ",
"        .XXXXXXXXXXXXXXXXXXX.XXX.       ",
"        .XXXXXXXXXXXXXXXXXXXXXXXX.      ",
"        .XXXXXXXXXXXXXXXXXXXXXXXX.      ",
"         ..XXXXXXXXXXXXXXXXXXXXXX.      ",
"           .XXXXXXXXXXXXXXXXXX...       ",
"           ..XXXXXXXXXXXXXXXX.          ",
"            .XXXXXXXXXXXXXXXX.          ",
"            .XXXXXXXXXXXXXXXX.          ",
"            .XXXXXXXXXXXXXXXXX.         ",
"            .XXXXXXXXXXXXXXXXX.         ",
"            .XXXXXXXXXXXXXXXXXX.        ",
"            .XXXXXXXXXXXXXXXXXXX.       ",
"           .XXXXXXXXXXXXXXXXXXXXX.      ",
"           .XXXXXXXXXXXXXX.XXXXXXX.     ",
"          .XXXXXXX.XXXXXXX.XXXXXXX.     ",
"         .XXXXXXXX.XXXXXXX.XXXXXXX.     ",
"         .XXXXXXX...XXXXX...XXXXX.      ",
"         .XXXXXXX.  .....   .....       ",
"         ..XXXX..                       ",
"           ....                         ",
"                                        ",
"                                        ",
"                                        "};
			      
/* XPM */
static char * page_xpm[] = {
/* width height ncolors chars_per_pixel */
"32 32 5 1",
/* colors */
" 	s None	c None",
".	c black",
"X	c wheat",
"o	c tan",
"O	c #6699FF",
/* pixels */
"    ...................         ",
"    .XXXXXXXXXXXXXXXXX..        ",
"    .XXXXXXXXXXXXXXXXX.o.       ",
"    .XXXXXXXXXXXXXXXXX.oo.      ",
"    .XXXXXXXXXXXXXXXXX.ooo.     ",
"    .XXXXXXXXXXXXXXXXX.oooo.    ",
"    .XXXXXXXXXXXXXXXXX.......   ",
"    .XXXXXOOOOOOOOOOXXXooooo.   ",
"    .XXXXXXXXXXXXXXXXXXooooo.   ",
"    .XXXXXOOOOOOOOOOXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXOOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXOOOOOOOOOOXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXOOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXOOOOOOOOOOXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXOOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXOOOOOOOXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .........................   "};
			      
			      
#if (GTK_MINOR_VERSION > 0)

#include "gtk/gtkdnd.h"
#include "gtk/gtkselection.h"


// ----------------------------------------------------------------------------
// "drag_leave"
// ----------------------------------------------------------------------------

static void target_drag_leave( GtkWidget *WXUNUSED(widget),
			       GdkDragContext *context,
			       guint WXUNUSED(time),
			       wxDropTarget *drop_target )
{
    /* inform the wxDropTarget about the current GdkDragContext.
       this is only valid for the duration of this call */
    drop_target->SetDragContext( context );
    
    /* we don't need return values. this event is just for
       information */
    drop_target->OnLeave();
    
    /* this has to be done because GDK has no "drag_enter" event */
    drop_target->m_firstMotion = TRUE;
    
    /* after this, invalidate the drop_target's GdkDragContext */
    drop_target->SetDragContext( (GdkDragContext*) NULL );
}

// ----------------------------------------------------------------------------
// "drag_motion"
// ----------------------------------------------------------------------------

static gboolean target_drag_motion( GtkWidget *WXUNUSED(widget),
			            GdkDragContext *context,
			            gint x,
			            gint y,
			            guint time,
			            wxDropTarget *drop_target )
{
    /* Owen Taylor: "if the coordinates not in a drop zone,
       return FALSE, otherwise call gtk_drag_status() and
       return TRUE" */
       
    /* inform the wxDropTarget about the current GdkDragContext.
       this is only valid for the duration of this call */
    drop_target->SetDragContext( context );
    
    if (drop_target->m_firstMotion)
    {
        /* the first "drag_motion" event substitutes a "drag_enter" event */
	drop_target->OnEnter();
    }
    
    /* give program a chance to react (i.e. to say no by returning FALSE) */
    bool ret = drop_target->OnMove( x, y );
    
    /* we don't yet handle which "actions" (i.e. copy or move)
       the target accepts. so far we simply accept the
       suggested action. TODO. */
    if (ret)
        gdk_drag_status( context, context->suggested_action, time );
    
    /* after this, invalidate the drop_target's GdkDragContext */
    drop_target->SetDragContext( (GdkDragContext*) NULL );
    
    /* this has to be done because GDK has no "drag_enter" event */
    drop_target->m_firstMotion = FALSE;
    
    return ret;
}

// ----------------------------------------------------------------------------
// "drag_drop"
// ----------------------------------------------------------------------------

static gboolean target_drag_drop( GtkWidget *widget,
			          GdkDragContext *context,
			          gint x,
			          gint y,
			          guint time,
				  wxDropTarget *drop_target )
{
    /* Owen Taylor: "if the drop is not in a drop zone,
       return FALSE, otherwise, if you aren't accepting
       the drop, call gtk_drag_finish() with success == FALSE
       otherwise call gtk_drag_data_get()" */

//    printf( "drop.\n" );
       
    /* this seems to make a difference between not accepting
       due to wrong target area and due to wrong format. let
       us hope that this is not required.. */
  
    /* inform the wxDropTarget about the current GdkDragContext.
       this is only valid for the duration of this call */
    drop_target->SetDragContext( context );
    
    /* inform the wxDropTarget about the current drag widget.
       this is only valid for the duration of this call */
    drop_target->SetDragWidget( widget );
    
    /* inform the wxDropTarget about the current drag time.
       this is only valid for the duration of this call */
    drop_target->SetDragTime( time );
    
    bool ret = drop_target->OnDrop( x, y );
    
    if (ret)
    {
        /* this should trigger an "drag_data_received" event */
        gtk_drag_get_data( widget, 
	                   context, 
		           GPOINTER_TO_INT (context->targets->data), 
		           time );
    }
    else
    {
        /* cancel the whole thing */
        gtk_drag_finish( context,
	                  FALSE,        /* no success */
			  FALSE,        /* don't delete data on dropping side */
			  time );
    }
    
    /* after this, invalidate the drop_target's GdkDragContext */
    drop_target->SetDragContext( (GdkDragContext*) NULL );
    
    /* after this, invalidate the drop_target's drag widget */
    drop_target->SetDragWidget( (GtkWidget*) NULL );
    
    /* this has to be done because GDK has no "drag_enter" event */
    drop_target->m_firstMotion = TRUE;
    
    return ret;
}

// ----------------------------------------------------------------------------
// "drag_data_received"
// ----------------------------------------------------------------------------

static void target_drag_data_received( GtkWidget *WXUNUSED(widget),
			               GdkDragContext *context,
			               gint x,
			               gint y,
			               GtkSelectionData *data,
			               guint WXUNUSED(info),
			               guint time,
				       wxDropTarget *drop_target )
{
    /* Owen Taylor: "call gtk_drag_finish() with
       success == TRUE" */

//    printf( "data received.\n" );
       
    /* strangely, we get a "drag_data_received" event even when
       we don't request them. this checks this. */
    if (!drop_target->m_currentDataObject) return;
    
    wxDataObject *data_object = drop_target->m_currentDataObject;

    if ((data->length <= 0) || (data->format != 8))
    {
        /* negative data length and non 8-bit data format
           qualifies for junk */
        gtk_drag_finish (context, FALSE, FALSE, time);
    }
    else
    {
        wxASSERT_MSG( data->target ==  data_object->GetFormat().GetAtom(), "DnD GetData target mismatch."  );
	
	if (data_object->GetFormat().GetType() == wxDF_TEXT)
	{
	    wxTextDataObject *text_object = (wxTextDataObject*)data_object;
	    text_object->SetText( (const char*)data->data );
	} else
	
	if (data_object->GetFormat().GetType() == wxDF_FILENAME)
	{
	} else
	
	if (data_object->GetFormat().GetType() == wxDF_PRIVATE)
	{
	    wxPrivateDataObject *priv_object = (wxPrivateDataObject*)data_object;
	    priv_object->SetData( (const char*)data->data, (size_t)data->length );
	}
	
	/* tell wxDropTarget that data transfer was successfull */
	drop_target->m_dataRetrieveSuccess = TRUE;
	
	/* tell GTK that data transfer was successfull */
        gtk_drag_finish( context, TRUE, FALSE, time );
    }

    /* tell wxDropTarget that data has arrived (or not) */
    drop_target->m_waiting = FALSE;
}

//----------------------------------------------------------------------------
// wxDropTarget
//----------------------------------------------------------------------------

wxDropTarget::wxDropTarget()
{
    m_firstMotion = TRUE;
    m_dragContext = (GdkDragContext*) NULL;
    m_dragWidget = (GtkWidget*) NULL;
    m_dragTime = 0;
    m_currentDataObject = (wxDataObject*) NULL;
    m_dataRetrieveSuccess = FALSE;
}

wxDropTarget::~wxDropTarget()
{
}

void wxDropTarget::OnEnter()
{
}

void wxDropTarget::OnLeave()
{
}

bool wxDropTarget::OnMove( int WXUNUSED(x), int WXUNUSED(y) )
{
    return TRUE;
}

bool wxDropTarget::OnDrop( int WXUNUSED(x), int WXUNUSED(y) )
{
    return FALSE;
}

bool wxDropTarget::IsSupported( wxDataFormat format )
{ 
    if (!m_dragContext) return FALSE;
    
    GList *child = m_dragContext->targets;
    while (child)
    {
        GdkAtom formatAtom = (GdkAtom) GPOINTER_TO_INT(child->data);
  
//        char *name = gdk_atom_name( formatAtom );
//        if (name) printf( "Format available: %s.\n", name );
  
        if (formatAtom == format.GetAtom()) return TRUE;
        child = child->next;
    }

    return FALSE;
}
  
bool wxDropTarget::GetData( wxDataObject *data )
{
    if (!m_dragContext) return FALSE;
    if (!m_dragWidget) return FALSE;
    
    m_currentDataObject = data;
    m_dataRetrieveSuccess = FALSE;
    
    /* this should trigger an "drag_data_received" event */
    gtk_drag_get_data( m_dragWidget, 
	               m_dragContext,
		       data->GetFormat().GetAtom(),
		       m_dragTime );
		       
    /* wait for the "drag_data_received" event */
    
//    printf( "pre wait.\n" );
    
    m_waiting = TRUE;
    while (m_waiting) wxYield();
    
//    printf( "post wait.\n" );
    
    m_currentDataObject = (wxDataObject*) NULL;
    
    return m_dataRetrieveSuccess;
}
  
void wxDropTarget::UnregisterWidget( GtkWidget *widget )
{
    wxCHECK_RET( widget != NULL, "unregister widget is NULL" );
  
    gtk_drag_dest_unset( widget );
		     
    gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
		      GTK_SIGNAL_FUNC(target_drag_leave), (gpointer) this );

    gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
		      GTK_SIGNAL_FUNC(target_drag_motion), (gpointer) this );

    gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
		      GTK_SIGNAL_FUNC(target_drag_drop), (gpointer) this );

    gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
		      GTK_SIGNAL_FUNC(target_drag_data_received), (gpointer) this );
}

void wxDropTarget::RegisterWidget( GtkWidget *widget )
{
    wxCHECK_RET( widget != NULL, "register widget is NULL" );
  
    /* gtk_drag_dest_set() determines what default behaviour we'd like
       GTK to supply. we don't want to specify out targets (=formats)
       or actions in advance (i.e. not GTK_DEST_DEFAULT_MOTION and
       not GTK_DEST_DEFAULT_DROP). instead we react individually to
       "drag_motion" and "drag_drop" events. this makes it possible
       to allow dropping on only a small area. we should set 
       GTK_DEST_DEFAULT_HIGHLIGHT as this will switch on the nice
       highlighting if dragging over standard controls, but this
       seems to be broken without the other two. */
  
    gtk_drag_dest_set( widget,
		       (GtkDestDefaults) 0,         /* no default behaviour */
		       (GtkTargetEntry*) NULL,      /* we don't supply any formats here */
		       0,                           /* number of targets = 0 */
		       (GdkDragAction) 0 );         /* we don't supply any actions here */
		     
    gtk_signal_connect( GTK_OBJECT(widget), "drag_leave",
		      GTK_SIGNAL_FUNC(target_drag_leave), (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(widget), "drag_motion",
		      GTK_SIGNAL_FUNC(target_drag_motion), (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(widget), "drag_drop",
		      GTK_SIGNAL_FUNC(target_drag_drop), (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(widget), "drag_data_received",
		      GTK_SIGNAL_FUNC(target_drag_data_received), (gpointer) this );
}

//-------------------------------------------------------------------------
// wxTextDropTarget
//-------------------------------------------------------------------------

bool wxTextDropTarget::OnMove( int WXUNUSED(x), int WXUNUSED(y) )
{
    return IsSupported( wxDF_TEXT );  // same as "STRING"
}

bool wxTextDropTarget::OnDrop( int x, int y )
{
    if (!IsSupported( wxDF_TEXT )) return FALSE;
    
    wxTextDataObject data;
    if (!GetData( &data )) return FALSE;
    
    OnDropText( x, y, data.GetText() );
    return TRUE;
}

//-------------------------------------------------------------------------
// wxPrivateDropTarget
//-------------------------------------------------------------------------

wxPrivateDropTarget::wxPrivateDropTarget()
{
    m_id = wxTheApp->GetAppName();
}

wxPrivateDropTarget::wxPrivateDropTarget( const wxString &id )
{
    m_id = id;
}

bool wxPrivateDropTarget::OnMove( int WXUNUSED(x), int WXUNUSED(y) )
{
    return IsSupported( m_id );
}

bool wxPrivateDropTarget::OnDrop( int x, int y )
{
    if (!IsSupported( m_id )) return FALSE;
    
    wxPrivateDataObject data;
    if (!GetData( &data )) return FALSE;
    
    OnDropData( x, y, data.GetData(), data.GetSize() );
    
    return TRUE;
}

//----------------------------------------------------------------------------
// A drop target which accepts files (dragged from File Manager or Explorer)
//----------------------------------------------------------------------------

bool wxFileDropTarget::OnMove( int WXUNUSED(x), int WXUNUSED(y) )
{
    return IsSupported( wxDF_FILENAME );  // same as "file:ALL"
}

bool wxFileDropTarget::OnDrop( int x, int y )
{
    return IsSupported( wxDF_FILENAME );  // same as "file:ALL"
}

void wxFileDropTarget::OnData( int x, int y )
{
    wxFileDataObject data;
    if (!GetData( &data )) return;

    /* get number of substrings /root/mytext.txt/0/root/myothertext.txt/0/0 */
    size_t number = 0;
    size_t i;
    size_t size = data.GetFiles().Length();
    char *text = WXSTRINGCAST data.GetFiles();
    for ( i = 0; i < size; i++)
        if (text[i] == 0) number++;

    if (number == 0) return;
    
    char **files = new char*[number];
  
    text = WXSTRINGCAST data.GetFiles();
    for (i = 0; i < number; i++)
    {
        files[i] = text;
        int len = strlen( text );
        text += len+1;
    }

    OnDropFiles( x, y, number, files ); 
  
    free( files );
}

//----------------------------------------------------------------------------
// "drag_data_get"
//----------------------------------------------------------------------------

static void  
source_drag_data_get  (GtkWidget          *WXUNUSED(widget),
		       GdkDragContext     *context,
		       GtkSelectionData   *selection_data,
		       guint               WXUNUSED(info),
		       guint               WXUNUSED(time),
		       wxDropSource       *drop_source )
{
    char *name = gdk_atom_name( selection_data->target );
    if (name) printf( "Format requested: %s.\n", name );
    
    wxNode *node = drop_source->m_data->m_dataObjects.First();
    while (node)
    {
        wxDataObject *data_object = (wxDataObject*) node->Data();
	if (data_object->GetFormat().GetAtom() == selection_data->target)
	{
	    size_t data_size = data_object->GetSize();
	    if (data_size > 0)
	    {
	        guchar *buffer = new guchar[data_size];
		data_object->WriteData( buffer );
		
                gtk_selection_data_set( selection_data,
			                selection_data->target,
			                8,   /* 8-bit */
					buffer, 
					data_size );
					
		free( buffer );
		
	        /* so far only copy, no moves. TODO. */
                drop_source->m_retValue = wxDragCopy;
	    
	        return;
	    }
	}
    
        node = node->Next();
    }
    
    drop_source->m_retValue = wxDragCancel;
}
  
//----------------------------------------------------------------------------
// "drag_data_delete"
//----------------------------------------------------------------------------

static void source_drag_data_delete( GtkWidget          *WXUNUSED(widget),
			             GdkDragContext     *WXUNUSED(context),
		                     wxDropSource       *drop_source )
{
//    printf( "Delete the data!\n" );

    drop_source->m_retValue = wxDragMove;
}
  
//----------------------------------------------------------------------------
// "drag_begin"
//----------------------------------------------------------------------------

static void source_drag_begin( GtkWidget          *WXUNUSED(widget),
			       GdkDragContext     *WXUNUSED(context),
		               wxDropSource       *WXUNUSED(drop_source) )
{
//    printf( "drag_begin.\n" );
}
  
//----------------------------------------------------------------------------
// "drag_end"
//----------------------------------------------------------------------------

static void source_drag_end( GtkWidget          *WXUNUSED(widget),
			     GdkDragContext     *WXUNUSED(context),
		             wxDropSource       *drop_source )
{
//    printf( "drag_end.\n" );

    drop_source->m_waiting = FALSE;
}
  
//---------------------------------------------------------------------------
// wxDropSource
//---------------------------------------------------------------------------

wxDropSource::wxDropSource( wxWindow *win, const wxIcon &go, const wxIcon &stop )
{
    g_blockEventsOnDrag = TRUE;
    m_waiting = TRUE;
  
    m_window = win;
    m_widget = win->m_widget;
    if (win->m_wxwindow) m_widget = win->m_wxwindow;
  
    m_data = (wxDataBroker*) NULL;
    m_retValue = wxDragCancel;

    m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
    m_goaheadCursor = wxCursor( wxCURSOR_HAND );
    
    m_goIcon = go;
    if (wxNullIcon == go) m_goIcon = wxIcon( page_xpm );
    m_stopIcon = stop;
    if (wxNullIcon == stop) m_stopIcon = wxIcon( gv_xpm );
}

wxDropSource::wxDropSource( wxDataObject *data, wxWindow *win, const wxIcon &go, const wxIcon &stop )
{
    m_waiting = TRUE;
  
    m_window = win;
    m_widget = win->m_widget;
    if (win->m_wxwindow) m_widget = win->m_wxwindow;
    m_retValue = wxDragCancel;
  
    if (data)
    {
        m_data = new wxDataBroker();
	m_data->Add( data );
    }
    else
    {
        m_data = (wxDataBroker*) NULL;
    }

    m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
    m_goaheadCursor = wxCursor( wxCURSOR_HAND );
    
    m_goIcon = go;
    if (wxNullIcon == go) m_goIcon = wxIcon( page_xpm );
    m_stopIcon = stop;
    if (wxNullIcon == stop) m_stopIcon = wxIcon( gv_xpm );
}

wxDropSource::wxDropSource( wxDataBroker *data, wxWindow *win )
{
    m_window = win;
    m_widget = win->m_widget;
    if (win->m_wxwindow) m_widget = win->m_wxwindow;
    m_retValue = wxDragCancel;
  
    m_data = data;

    m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
    m_goaheadCursor = wxCursor( wxCURSOR_HAND );
}

void wxDropSource::SetData( wxDataObject *data )
{
    if (m_data) delete m_data;
    
    if (data)
    {
        m_data = new wxDataBroker();
	m_data->Add( data );
    }
    else
    {
        m_data = (wxDataBroker*) NULL;
    }
}

void wxDropSource::SetData( wxDataBroker *data )
{
    if (m_data) delete m_data;
    
    m_data = data;
}

wxDropSource::~wxDropSource(void)
{
    if (m_data) delete m_data;

    g_blockEventsOnDrag = FALSE;
}
   
wxDragResult wxDropSource::DoDragDrop( bool WXUNUSED(bAllowMove) )
{
    wxASSERT_MSG( m_data, "wxDragSource: no data" );
  
    if (!m_data) return (wxDragResult) wxDragNone;
  
    g_blockEventsOnDrag = TRUE;
  
    RegisterWindow();
    
    m_waiting = TRUE;

    GtkTargetList *target_list = gtk_target_list_new( (GtkTargetEntry*) NULL, 0 );
    gtk_target_list_add( target_list, gdk_atom_intern( "STRING", FALSE ), 0, 0 );
    
    GdkEventMotion event;
    event.window = m_widget->window;
    int x = 0;
    int y = 0;
    GdkModifierType state;
    gdk_window_get_pointer( event.window, &x, &y, &state );
    event.x = x;
    event.y = y;
    event.state = state;
    
    /* GTK wants to know which button was pressed which caused the dragging */
    int button_number = 0;
    if (event.state & GDK_BUTTON1_MASK) button_number = 1;
    else if (event.state & GDK_BUTTON2_MASK) button_number = 2;
    else if (event.state & GDK_BUTTON3_MASK) button_number = 3;

    /* don't start dragging if no button is down */
    if (button_number)
    {
        GdkDragContext *context = gtk_drag_begin( m_widget,
                                              target_list,
					      GDK_ACTION_COPY,
					      button_number,  /* number of mouse button which started drag */
					      (GdkEvent*) &event );
	
        wxMask *mask = m_goIcon.GetMask();
        GdkBitmap *bm = (GdkBitmap *) NULL;
        if (mask) bm = mask->GetBitmap();
	GdkPixmap *pm = m_goIcon.GetPixmap();
	
        gtk_drag_set_icon_pixmap( context,
	                          gtk_widget_get_colormap( m_widget ),
				  pm,
				  bm,
				  0,
				  0 );
    
        while (m_waiting) wxYield();
    }

    g_blockEventsOnDrag = FALSE;
    
    UnregisterWindow();

    return m_retValue;
}

void wxDropSource::RegisterWindow()
{
    if (!m_widget) return;
    
    gtk_signal_connect( GTK_OBJECT(m_widget), "drag_data_get",
		      GTK_SIGNAL_FUNC (source_drag_data_get), (gpointer) this);
    gtk_signal_connect (GTK_OBJECT(m_widget), "drag_data_delete",
		      GTK_SIGNAL_FUNC (source_drag_data_delete),  (gpointer) this );
    gtk_signal_connect (GTK_OBJECT(m_widget), "drag_begin",
		      GTK_SIGNAL_FUNC (source_drag_begin),  (gpointer) this );
    gtk_signal_connect (GTK_OBJECT(m_widget), "drag_end",
		      GTK_SIGNAL_FUNC (source_drag_end),  (gpointer) this );

}

void wxDropSource::UnregisterWindow()
{
    if (!m_widget) return;
    
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget),
		      GTK_SIGNAL_FUNC(source_drag_data_get), (gpointer) this );
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget),
		      GTK_SIGNAL_FUNC(source_drag_data_delete), (gpointer) this );
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget),
		      GTK_SIGNAL_FUNC(source_drag_begin), (gpointer) this );
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget),
		      GTK_SIGNAL_FUNC(source_drag_end), (gpointer) this );
}


#else  // NEW_CODE

//----------------------------------------------------------------------------
// forward
//----------------------------------------------------------------------------

GtkWidget *shape_create_icon ( const wxIcon &shape,
			      gint      x,
			      gint      y,
			      gint      px,
			      gint      py,
			      gint      window_type);

//-----------------------------------------------------------------------------
// globals
//-----------------------------------------------------------------------------

wxDropSource *gs_currentDropSource = (wxDropSource*) NULL;

//-----------------------------------------------------------------------------
// "drop_enter_event"
//-----------------------------------------------------------------------------

static void gtk_target_enter_callback( GtkWidget *WXUNUSED(widget), 
                                       GdkEventDropEnter *WXUNUSED(event), 
			               wxDropTarget *target )
{
    if (target)
        target->OnEnter();
}

//-----------------------------------------------------------------------------
// "drop_leave_event"
//-----------------------------------------------------------------------------

static void gtk_target_leave_callback( GtkWidget *WXUNUSED(widget), 
                                       GdkEventDropLeave *WXUNUSED(event), 
			               wxDropTarget *target )
{
    if (target)
        target->OnLeave();
}

//-----------------------------------------------------------------------------
// "drop_data_available_event"
//-----------------------------------------------------------------------------

static void gtk_target_callback( GtkWidget *widget, 
                                 GdkEventDropDataAvailable *event, 
			         wxDropTarget *target )
{
    if (target)
    {
        int x = 0;
        int y = 0;
        gdk_window_get_pointer( widget->window, &x, &y, (GdkModifierType *) NULL );
/*
        printf( "Drop data is of type %s.\n", event->data_type );
*/
        target->OnDrop( x, y, (const void*)event->data, (size_t)event->data_numbytes );
    }

/*
    g_free (event->data);
    g_free (event->data_type);
*/
}

// ----------------------------------------------------------------------------
// wxDropTarget
// ----------------------------------------------------------------------------

wxDropTarget::wxDropTarget()
{
    m_format = (wxDataFormat*) NULL;
}

wxDropTarget::~wxDropTarget()
{
    if (m_format) delete m_format;
}

wxDataFormat &wxDropTarget::GetFormat(size_t n) const
{
    return (*m_format);
}

void wxDropTarget::UnregisterWidget( GtkWidget *widget )
{
    if (!widget) return;
  
    gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
      GTK_SIGNAL_FUNC(gtk_target_callback), (gpointer) this );

    gtk_widget_dnd_drop_set( widget, FALSE, (gchar **) NULL, 0, FALSE );
}

void wxDropTarget::RegisterWidget( GtkWidget *widget )
{
    wxString formats;
    int valid = 0;
  
    for ( size_t i = 0; i < GetFormatCount(); i++ )
    {
        switch (GetFormat(i).GetType()) 
        {
            case wxDF_TEXT:
	    {
	        if (i > 0) formats += ";";
                formats += "text/plain";
	        valid++;
	        break;
	    }
            case wxDF_FILENAME:
	    {
	        if (i > 0) formats += ";";
                formats += "file:ALL";
	        valid++;
	        break;
	    }
            case wxDF_PRIVATE:
	    {
	        if (i > 0) formats += ";";
                wxPrivateDropTarget *pdt = (wxPrivateDropTarget *)this;
                formats += pdt->GetId();
	        valid++;
		break;
	    }
            default:
              break;
        }
    }
  
    char *str = WXSTRINGCAST formats;
    
    gtk_widget_dnd_drop_set( widget, TRUE, &str, valid, FALSE );
  
    gtk_signal_connect( GTK_OBJECT(widget), "drop_data_available_event",
      GTK_SIGNAL_FUNC(gtk_target_callback), (gpointer) this );
      
    gtk_signal_connect( GTK_OBJECT(widget), "drop_enter_event",
      GTK_SIGNAL_FUNC(gtk_target_enter_callback), (gpointer) this );
      
    gtk_signal_connect( GTK_OBJECT(widget), "drop_leave_event",
      GTK_SIGNAL_FUNC(gtk_target_leave_callback), (gpointer) this );
}

// ----------------------------------------------------------------------------
// wxTextDropTarget
// ----------------------------------------------------------------------------

wxTextDropTarget::wxTextDropTarget()
{
    m_format = new wxDataFormat( wxDF_TEXT );
}

bool wxTextDropTarget::OnDrop( long x, long y, const void *data, size_t WXUNUSED(size) )
{
    OnDropText( x, y, (const char*)data );
    return TRUE;
}

bool wxTextDropTarget::OnDropText( long x, long y, const char *psz )
{
/*
    printf( "Got dropped text: %s.\n", psz );
    printf( "At x: %d, y: %d.\n", (int)x, (int)y );
*/
    return TRUE;
}

size_t wxTextDropTarget::GetFormatCount() const
{
    return 1;
}

// ----------------------------------------------------------------------------
// wxPrivateDropTarget
// ----------------------------------------------------------------------------

wxPrivateDropTarget::wxPrivateDropTarget()
{
    m_id = wxTheApp->GetAppName();
    m_format = new wxDataFormat( m_id );
}

void wxPrivateDropTarget::SetId( const wxString& id )
{
    m_id = id;
    m_format->SetId( id );
}

size_t wxPrivateDropTarget::GetFormatCount() const
{
    return 1;
}

// ----------------------------------------------------------------------------
// wxFileDropTarget
// ----------------------------------------------------------------------------

wxFileDropTarget::wxFileDropTarget()
{
    m_format = new wxDataFormat( wxDF_FILENAME );
}    

bool wxFileDropTarget::OnDropFiles( long x, long y, size_t nFiles, const char * const aszFiles[] )
{
    printf( "Got %d dropped files.\n", (int)nFiles );
    printf( "At x: %d, y: %d.\n", (int)x, (int)y );

    for (size_t i = 0; i < nFiles; i++)
    {
        printf( aszFiles[i] );
        printf( "\n" );
    }
    
    return TRUE;
}

bool wxFileDropTarget::OnDrop(long x, long y, const void *data, size_t size )
{
    size_t number = 0;
    size_t i;
    char *text = (char*) data;
    for ( i = 0; i < size; i++)
        if (text[i] == 0) number++;

    if (number == 0) return TRUE;    
    
    char **files = new char*[number];
  
    text = (char*) data;
    for (i = 0; i < number; i++)
    {
        files[i] = text;
        int len = strlen( text );
        text += len+1;
    }

    bool ret = OnDropFiles( x, y, 1, files ); 
  
    free( files );
  
    return ret;
}

size_t wxFileDropTarget::GetFormatCount() const
{
    return 1;
}

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

static void
shape_motion (GtkWidget      *widget, 
	      GdkEventMotion * /*event*/);
	      
//-----------------------------------------------------------------------------
// "drag_request_event"
//-----------------------------------------------------------------------------

void gtk_drag_callback( GtkWidget *widget, GdkEventDragRequest *event, wxDropSource *source )
{
    wxDataBroker *data = source->m_data;
    
    if (!data) return;
    
    wxNode *node = data->m_dataObjects.First();
    {
        wxDataObject *dobj = (wxDataObject*) node->Data();
	
	if ((strcmp(event->data_type,"file:ALL") == 0) &&
	    (dobj->GetFormat().GetType() == wxDF_FILENAME))
	{
	    wxFileDataObject *file_object = (wxFileDataObject*) dobj;
	    
	    wxString text = file_object->GetFiles();
	    
	    char *s = WXSTRINGCAST text;
	    
            gtk_widget_dnd_data_set( widget, 
	                             (GdkEvent*)event, 
				     (unsigned char*) s, 
				     (int) text.Length()+1 );
	
            source->m_retValue = wxDragCopy;
	    
	    return;
	}
	
	if ((strcmp(event->data_type,"text/plain") == 0) &&
	    (dobj->GetFormat().GetType() == wxDF_TEXT))
	{
	    wxTextDataObject *text_object = (wxTextDataObject*) dobj;
	    
	    wxString text = text_object->GetText();
	    
	    char *s = WXSTRINGCAST text;
	    
            gtk_widget_dnd_data_set( widget, 
	                             (GdkEvent*)event, 
				     (unsigned char*) s, 
				     (int) text.Length()+1 );
	
            source->m_retValue = wxDragCopy;
	    
	    return;
	}
	
	if (dobj->GetFormat().GetType() == wxDF_PRIVATE)
	{
            wxPrivateDataObject *pdo = (wxPrivateDataObject*) dobj;
	    
	    if (pdo->GetId() == event->data_type)
	    {
                gtk_widget_dnd_data_set( widget, 
	                             (GdkEvent*)event,
				     (unsigned char*) pdo->GetData(), 
				     (int) pdo->GetSize() );
				     
                source->m_retValue = wxDragCopy;
	    
		return;
	    }
	}
	
	node = node->Next();
   }
}

wxDropSource::wxDropSource( wxWindow *win, const wxIcon &go, const wxIcon &stop )
{
    g_blockEventsOnDrag = TRUE;
  
    m_window = win;
    m_widget = win->m_widget;
    if (win->m_wxwindow) m_widget = win->m_wxwindow;
  
    m_data = (wxDataBroker*) NULL;
    m_retValue = wxDragCancel;

    m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
    m_goaheadCursor = wxCursor( wxCURSOR_HAND );
    
    m_goIcon = go;
    if (wxNullIcon == go) m_goIcon = wxIcon( page_xpm );
    m_stopIcon = stop;
    if (wxNullIcon == stop) m_stopIcon = wxIcon( gv_xpm );
}

wxDropSource::wxDropSource( wxDataObject *data, wxWindow *win, const wxIcon &go, const wxIcon &stop )
{
    g_blockEventsOnDrag = TRUE;
  
    m_window = win;
    m_widget = win->m_widget;
    if (win->m_wxwindow) m_widget = win->m_wxwindow;
    m_retValue = wxDragCancel;
  
    if (data)
    {
        m_data = new wxDataBroker();
	m_data->Add( data );
    }
    else
    {
        m_data = (wxDataBroker*) NULL;
    }

    m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
    m_goaheadCursor = wxCursor( wxCURSOR_HAND );
    
    m_goIcon = go;
    if (wxNullIcon == go) m_goIcon = wxIcon( page_xpm );
    m_stopIcon = stop;
    if (wxNullIcon == stop) m_stopIcon = wxIcon( gv_xpm );
}

wxDropSource::wxDropSource( wxDataBroker *data, wxWindow *win )
{
    g_blockEventsOnDrag = TRUE;
  
    m_window = win;
    m_widget = win->m_widget;
    if (win->m_wxwindow) m_widget = win->m_wxwindow;
    m_retValue = wxDragCancel;
  
    m_data = data;

    m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
    m_goaheadCursor = wxCursor( wxCURSOR_HAND );
}

void wxDropSource::SetData( wxDataObject *data )
{
    if (m_data) delete m_data;
    
    if (data)
    {
        m_data = new wxDataBroker();
	m_data->Add( data );
    }
    else
    {
        m_data = (wxDataBroker*) NULL;
    }
}

void wxDropSource::SetData( wxDataBroker *data )
{
    if (m_data) delete m_data;
    
    m_data = data;
}

wxDropSource::~wxDropSource(void)
{
    if (m_data) delete m_data;

    g_blockEventsOnDrag = FALSE;
}
   
wxDragResult wxDropSource::DoDragDrop( bool WXUNUSED(bAllowMove) )
{
    if (gdk_dnd.dnd_grabbed) return (wxDragResult) wxDragNone;
    if (gdk_dnd.drag_really) return (wxDragResult) wxDragNone;
  
    wxASSERT_MSG( m_data, "wxDragSource: no data" );
  
    if (!m_data) return (wxDragResult) wxDragNone;
  
    static GtkWidget *drag_icon = (GtkWidget*) NULL;
    static GtkWidget *drop_icon = (GtkWidget*) NULL;

    GdkPoint hotspot_1 = {0,-5 };
      
    if (!drag_icon)
    {
	  drag_icon = shape_create_icon ( m_stopIcon,
					 440, 140, 0,0, GTK_WINDOW_POPUP);
	  
	  gtk_signal_connect (GTK_OBJECT (drag_icon), "destroy",
			      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			      &drag_icon);

	  gtk_widget_hide (drag_icon);
    }
      
    GdkPoint hotspot_2 = {-5,-5};
	
    if (!drop_icon)
    {
	  drop_icon = shape_create_icon ( m_goIcon,
					 440, 140, 0,0, GTK_WINDOW_POPUP);
	  
	  gtk_signal_connect (GTK_OBJECT (drop_icon), "destroy",
			      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			      &drop_icon);

	  gtk_widget_hide (drop_icon);
    }
	

    gdk_dnd_set_drag_shape( drag_icon->window,
			     &hotspot_1,
			     drop_icon->window,
			     &hotspot_2);
  
  
    GdkWindowPrivate *wp = (GdkWindowPrivate*) m_widget->window;
  
    RegisterWindow();
  
    gdk_dnd.drag_perhaps = TRUE;

    gdk_dnd.dnd_drag_start.x = 5;
    gdk_dnd.dnd_drag_start.y = 5;
    gdk_dnd.real_sw = wp;
	  
    if (gdk_dnd.drag_startwindows)
    {
        g_free( gdk_dnd.drag_startwindows );
        gdk_dnd.drag_startwindows = (GdkWindow **) NULL;
    }
    gdk_dnd.drag_numwindows = gdk_dnd.drag_really = 0;
  
    XWindowAttributes dnd_winattr;
    XGetWindowAttributes( gdk_display, wp->xwindow, &dnd_winattr );
    wp->dnd_drag_savedeventmask = dnd_winattr.your_event_mask;
  
    gdk_dnd_drag_addwindow( m_widget->window );
  
    GdkEventDragBegin ev;
    ev.type = GDK_DRAG_BEGIN;
    ev.window = m_widget->window;
    ev.u.allflags = 0;
    ev.u.flags.protocol_version = DND_PROTOCOL_VERSION;
  
    gdk_event_put( (GdkEvent*)&ev );
  
    XGrabPointer( gdk_display, wp->xwindow, False, 
                  ButtonMotionMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
		  GrabModeAsync, GrabModeAsync, gdk_root_window, None, CurrentTime );
		
    gdk_dnd_set_drag_cursors( m_defaultCursor.GetCursor(), m_goaheadCursor.GetCursor() );
  
    gdk_dnd.dnd_grabbed = TRUE;
    gdk_dnd.drag_really = 1;
  
    int x = 0;
    int y = 0;
    wxGetMousePosition( &x, &y );  
  
    gdk_dnd_display_drag_cursor( x, y, FALSE, TRUE );
    
    gs_currentDropSource = this;
    
    while (gdk_dnd.drag_really || gdk_dnd.drag_perhaps) wxYield();
    
    gs_currentDropSource = (wxDropSource*) NULL;
  
    UnregisterWindow();
  
    g_blockEventsOnDrag = FALSE;
  
    return m_retValue;
}

void wxDropSource::RegisterWindow(void)
{
    if (!m_data) return;

    wxString formats;
    
    wxNode *node = m_data->m_dataObjects.First();
    while (node)
    {
        wxDataObject* dobj = (wxDataObject*) node->Data();
    
        switch (dobj->GetFormat().GetType()) 
        {
            case wxDF_TEXT:
	    { 
                  formats += "text/plain";
	          break;
	    }
            case wxDF_FILENAME:
	    {
                  formats += "file:ALL";
	          break;
	    }
            case wxDF_PRIVATE:
	    {
	          wxPrivateDataObject* pdo = (wxPrivateDataObject*) m_data;
	          formats += pdo->GetId();
	          break;
	    }
            default:
              break;
	}
	node = node->Next();
    }
  
    char *str = WXSTRINGCAST formats;
  
    gtk_widget_dnd_drag_set( m_widget, TRUE, &str, 1 );

    gtk_signal_connect( GTK_OBJECT(m_widget), "drag_request_event",
      GTK_SIGNAL_FUNC(gtk_drag_callback), (gpointer)this );
}

void wxDropSource::UnregisterWindow(void)
{
    if (!m_widget) return;
  
    gtk_widget_dnd_drag_set( m_widget, FALSE, (gchar **) NULL, 0 );
  
    gtk_signal_disconnect_by_data( GTK_OBJECT(m_widget), (gpointer)this );
}


/*
 * Shaped Windows
 */
static GdkWindow *root_win = (GdkWindow*) NULL;

typedef struct _cursoroffset {gint x,y;} CursorOffset;

static void
shape_pressed (GtkWidget *widget, GdkEventButton *event)
{
  CursorOffset *p;

  /* ignore double and triple click */
  if (event->type != GDK_BUTTON_PRESS)
    return;

  p = (CursorOffset *)gtk_object_get_user_data (GTK_OBJECT(widget));
  p->x = (int) event->x;
  p->y = (int) event->y;

  gtk_grab_add (widget);
  gdk_pointer_grab (widget->window, TRUE,
		    (GdkEventMask)
		      (GDK_BUTTON_RELEASE_MASK |
		       GDK_BUTTON_MOTION_MASK |
		       GDK_POINTER_MOTION_HINT_MASK),
		    (GdkWindow*)NULL, 
		    (GdkCursor*) NULL, 0);
}


static void
shape_released (GtkWidget *widget)
{
  gtk_grab_remove (widget);
  gdk_pointer_ungrab (0);
}

static void
shape_motion (GtkWidget      *widget, 
	      GdkEventMotion * /*event*/ )
{
  gint xp, yp;
  CursorOffset * p;
  GdkModifierType mask;

  p = (CursorOffset *)gtk_object_get_user_data (GTK_OBJECT (widget));

  /*
   * Can't use event->x / event->y here 
   * because I need absolute coordinates.
   */
   
  gdk_window_get_pointer (root_win, &xp, &yp, &mask);
  gtk_widget_set_uposition (widget, xp  - p->x, yp  - p->y);
  
  if (gs_currentDropSource) gs_currentDropSource->GiveFeedback( wxDragCopy, FALSE );
}

GtkWidget *
shape_create_icon (const wxIcon &shape,
		   gint      x,
		   gint      y,
		   gint      px,
		   gint      py,
		   gint      window_type)
{
  /*
   * GDK_WINDOW_TOPLEVEL works also, giving you a title border
   */
  GtkWidget *window = gtk_window_new ((GtkWindowType)window_type);
  
  GtkWidget *fixed = gtk_fixed_new ();
  gtk_widget_set_usize (fixed, 100,100);
  gtk_container_add (GTK_CONTAINER (window), fixed);
  gtk_widget_show (fixed);
  
  gtk_widget_set_events (window, 
			 gtk_widget_get_events (window) |
			 GDK_BUTTON_MOTION_MASK |
			 GDK_POINTER_MOTION_HINT_MASK |
			 GDK_BUTTON_PRESS_MASK);

  gtk_widget_realize (window);
  
  GdkBitmap *mask = (GdkBitmap*) NULL;
  if (shape.GetMask()) mask = shape.GetMask()->GetBitmap();

  GtkWidget *pixmap = gtk_pixmap_new (shape.GetPixmap(), mask);
  gtk_fixed_put (GTK_FIXED (fixed), pixmap, px,py);
  gtk_widget_show (pixmap);
  
  gtk_widget_shape_combine_mask (window, mask, px,py);


  gtk_signal_connect (GTK_OBJECT (window), "button_press_event",
		      GTK_SIGNAL_FUNC (shape_pressed),NULL);
  gtk_signal_connect (GTK_OBJECT (window), "button_release_event",
		      GTK_SIGNAL_FUNC (shape_released),NULL);
  gtk_signal_connect (GTK_OBJECT (window), "motion_notify_event",
		      GTK_SIGNAL_FUNC (shape_motion),NULL);

  CursorOffset*icon_pos = g_new (CursorOffset, 1);
  gtk_object_set_user_data(GTK_OBJECT(window), icon_pos);

  gtk_widget_set_uposition (window, x, y);
  gtk_widget_show (window);
  
  return window;
}

#endif 
       // NEW_GTK_DND_CODE
       
#endif

      // wxUSE_DRAG_AND_DROP