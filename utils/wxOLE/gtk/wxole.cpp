/////////////////////////////////////////////////////////////////////////////
// Name:        wxole.cpp
// Purpose:     wxOLE
// Author:      Robert Roebling
// Modified by:
// Created:     20/04/99
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "wxole.h"
#endif

#include "wx/defs.h"
#include "wxole.h"

#include "wx/app.h"
#include "wx/menu.h"
#include "wx/statusbr.h"
#include "wx/toolbar.h"
#include "wx/stream.h"

#include "wx/gtk/win_gtk.h"

extern "C" {
#include "gtk/gtk.h"
#include "gdk/gdk.h"

#include <gnome.h>
#include <libgnorba/gnorba.h>
#include <bonobo/bonobo.h>
#include <bonobo/gnome-main.h>
#include <bonobo/gnome-component.h>
#include <bonobo/gnome-component-factory.h>
#include <bonobo/gnome-persist-stream.h>
#include <bonobo/gtk-interfaces.h>
}


//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

const wxChar *wxOleNameStr = _T("olecontrol");

//---------------------------------------------------------------------------
// wxOleServerEnvPrivate
//---------------------------------------------------------------------------

class wxOleServerEnvPrivate
{
public:

  wxOleServerEnvPrivate() {}
  ~wxOleServerEnvPrivate() {}

  CORBA_Environment    m_ev;
  CORBA_ORB            m_orb;
};

//---------------------------------------------------------------------------
// wxOleInputStream
//---------------------------------------------------------------------------

class wxOleInputStream : public wxInputStream
{
public:

  wxOleInputStream( GNOME_Stream stream );
  ~wxOleInputStream();

  bool Ok() const { return m_error; }

protected:
 
  bool            m_error;
  GNOME_Stream    m_gstream;

  size_t OnSysRead(void *buffer, size_t size);
  off_t OnSysSeek(off_t pos, wxSeekMode mode);
  off_t OnSysTell() const;
};

//---------------------------------------------------------------------------
// wxOleServerEnv
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxOleServerEnv,wxObject)

wxOleServerEnv::wxOleServerEnv( const wxString &name, const wxString &version )
{
    m_serverName = name;
    m_serverVersion = version;
    
    m_priv = new wxOleServerEnvPrivate();
    
    CORBA_exception_init( &(m_priv->m_ev) );

    gnome_CORBA_init(
       m_serverName.mb_str(), 
       m_serverVersion.mb_str(), 
       &wxTheApp->argc,
       wxTheApp->argv,
       GNORBA_INIT_SERVER_FUNC, 
       &(m_priv->m_ev) );
       
    if (m_priv->m_ev._major != CORBA_NO_EXCEPTION)
    {
	return;
    }
   
    m_priv->m_orb = gnome_CORBA_ORB();
	
    if (bonobo_init( m_priv->m_orb, NULL, NULL ) == FALSE)
    {
	return;
    }

}

wxOleServerEnv::~wxOleServerEnv()
{
    CORBA_exception_free( &(m_priv->m_ev) );
    delete m_priv;
}
  
//---------------------------------------------------------------------------
// wxOleInputStream
//---------------------------------------------------------------------------

wxOleInputStream::wxOleInputStream( GNOME_Stream stream )
{
  m_gstream = stream;
  m_error = (m_gstream);
}

wxOleInputStream::~wxOleInputStream()
{
    /* we don't create the stream so we
       don't destroy it either. */
}

size_t wxOleInputStream::OnSysRead( void *buffer, size_t size )
{
    GNOME_Stream_iobuf *gbuffer = GNOME_Stream_iobuf__alloc();

    CORBA_Environment ev;
    CORBA_exception_init( &ev );
    
    GNOME_Stream_read( m_gstream, size, &gbuffer, &ev );
    
    CORBA_exception_free( &ev );
    
    memcpy( buffer, gbuffer->_buffer, gbuffer->_length );

    m_error = (gbuffer->_length != size);
    
    CORBA_free( gbuffer );
}

off_t wxOleInputStream::OnSysSeek( off_t pos, wxSeekMode mode )
{
    CORBA_Environment ev;
    CORBA_exception_init( &ev );
    
    GNOME_Stream_seek( m_gstream, pos /* offset */, 0 /* whence */, &ev );
    
    CORBA_exception_free( &ev );
}

off_t wxOleInputStream::OnSysTell() const
{
    return 0;  /* oh well */
}


//---------------------------------------------------------------------------
// wxOleServerPrivate
//---------------------------------------------------------------------------

class wxOleServerPrivate
{
public:

  wxOleServerPrivate() {}
  ~wxOleServerPrivate() {}

  GnomeComponentFactory   *m_factory;
};

//---------------------------------------------------------------------------
// wxOleServer
//---------------------------------------------------------------------------

static GnomeView* 
gnome_view_factory_callback( GnomeComponent *component, wxOleServer *server )
{
/*
    printf( "Create OLE control.\n" );
*/

    wxOleControl *ctx = server->CreateOleControl();
    
    if (!ctx) return (GnomeView*) NULL;
    
/*
    printf( "Creating OLE control succeeded. Returning as GnomeView\n" );
*/
    
    return gnome_view_new( ctx->m_widget );
}

static int
gnome_load_from_stream_callback( GnomePersistStream *ps, GNOME_Stream stream, GnomeComponent* component )
{
    wxOleInputStream wxstream( stream );
    
    
}

static GnomeComponent* 
gnome_component_factory_callback( GnomeComponentFactory *factory, const char *path, wxOleServer *server )
{
/*
    printf( "new component.\n" );
    if (path) printf( "path is %s.\n", path );
*/

    GnomeComponent *component = 
        gnome_component_new( gnome_view_factory_callback, (void*) server );
    
/*
    if (!component)
        printf( "component creation failed.\n" );
    else
        printf( "component creation succeded.\n" );
*/

    GnomePersistStream *stream = 
       gnome_persist_stream_new( gnome_load_from_stream_callback, NULL /*save*/, (void*) component );

/*
    if (!stream)
        printf( "stream creation failed.\n" );
    else
        printf( "stream creation succeded.\n" );
*/

    gtk_object_add_interface( GTK_OBJECT(component), GTK_OBJECT(stream) );

    return component;
}

IMPLEMENT_CLASS(wxOleServer,wxObject)

wxOleServer::wxOleServer( const wxString &id )
{
    m_ID = "component:";
    m_ID += id;
    
    m_priv = new wxOleServerPrivate();
    
/*
    printf( "new component factory.\n" );
*/
    
    m_priv->m_factory = 
       gnome_component_factory_new( m_ID.mb_str(), gnome_component_factory_callback, (void*) this );
}

wxOleServer::~wxOleServer()
{
    delete m_priv;
}

wxOleControl *wxOleServer::CreateOleControl()
{
    return new wxOleControl( -1 );
}

//-----------------------------------------------------------------------------
// "size_allocate"
//-----------------------------------------------------------------------------

static void gtk_olectx_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxOleControl *win )
{
    if (!win->m_hasVMT) return;

/*
    printf( "OnFrameResize from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

   if ((win->m_width != alloc->width) || (win->m_height != alloc->height))
   {
       win->m_sizeSet = FALSE;
       win->m_width = alloc->width;
       win->m_height = alloc->height;
   }
}

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

static gint gtk_olectx_delete_callback( GtkWidget *WXUNUSED(widget), GdkEvent *WXUNUSED(event), wxOleControl *win )
{
/*
    printf( "OnDelete from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    win->Close();

    return TRUE;
}

//-----------------------------------------------------------------------------
// "configure_event"
//-----------------------------------------------------------------------------

static gint gtk_olectx_configure_callback( GtkWidget *WXUNUSED(widget), GdkEventConfigure *event, wxOleControl *win )
{
    if (!win->m_hasVMT) return FALSE;

    win->m_x = event->x;
    win->m_y = event->y;

    wxMoveEvent mevent( wxPoint(win->m_x,win->m_y), win->GetId() );
    mevent.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( mevent );

    return FALSE;
}

//---------------------------------------------------------------------------
// wxOleControl
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxOleControl,wxFrame)

wxOleControl::wxOleControl( wxWindowID id, long style, const wxString &name )
{
    Create( id, style, name );
}

bool wxOleControl::Create( wxWindowID id, long style, const wxString &name )
{
    wxTopLevelWindows.Append( this );

    m_needParent = FALSE;

    PreCreation( (wxWindow*) NULL, id, wxDefaultPosition, wxDefaultSize, style, name );

    m_title = _T("wxWindows OLE Server");
    
    /* any widget that can contain another widget and resizes it
       to its full size */
    m_widget = gtk_hbox_new(0,0);

    GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );

    gtk_signal_connect( GTK_OBJECT(m_widget), "delete_event",
        GTK_SIGNAL_FUNC(gtk_olectx_delete_callback), (gpointer)this );

    /* m_mainWidget holds the toolbar, the menubar and the client area */
    m_mainWidget = gtk_myfixed_new();
    gtk_widget_show( m_mainWidget );
    GTK_WIDGET_UNSET_FLAGS( m_mainWidget, GTK_CAN_FOCUS );
    gtk_container_add( GTK_CONTAINER(m_widget), m_mainWidget );
    
    /* m_wxwindow only represents the client area without toolbar and menubar */
    m_wxwindow = gtk_myfixed_new();
    gtk_widget_show( m_wxwindow );
    GTK_WIDGET_UNSET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );
    gtk_container_add( GTK_CONTAINER(m_mainWidget), m_wxwindow );

    PostCreation();

    /* the user resized the frame by dragging etc. */
    gtk_signal_connect( GTK_OBJECT(m_widget), "size_allocate",
        GTK_SIGNAL_FUNC(gtk_olectx_size_callback), (gpointer)this );

    /* the only way to get the window size is to connect to this event */
    gtk_signal_connect( GTK_OBJECT(m_widget), "configure_event",
        GTK_SIGNAL_FUNC(gtk_olectx_configure_callback), (gpointer)this );

    gtk_widget_show_all( m_widget );

    return TRUE;
}

wxOleControl::~wxOleControl()
{
    if (m_frameMenuBar) delete m_frameMenuBar;
    m_frameMenuBar = (wxMenuBar *) NULL;
    
    if (m_frameStatusBar) delete m_frameStatusBar;
    m_frameStatusBar = (wxStatusBar *) NULL;
    
    if (m_frameToolBar) delete m_frameToolBar;
    m_frameToolBar = (wxToolBar *) NULL;

    wxTopLevelWindows.DeleteObject( this );

    if (wxTheApp->GetTopWindow() == this)
        wxTheApp->SetTopWindow( (wxWindow*) NULL );

    if (wxTopLevelWindows.Number() == 0)
        wxTheApp->ExitMainLoop();
}


void wxOleControl::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    // ignore
}

void wxOleControl::DoSetClientSize(int width, int height)
{
    // ignore
}
