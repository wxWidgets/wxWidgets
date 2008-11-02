/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    src/osx/corefoundation/gsockosx.c
 * Purpose: GSocket: Mac OS X mach-o part
 * CVSID:   $Id$
 * Mac code by Brian Victor, February 2002.  Email comments to bhv1@psu.edu
 * ------------------------------------------------------------------------- */

#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include "wx/gsocket.h"
#include "wx/apptrait.h"

#include <CoreFoundation/CoreFoundation.h>

// ----------------------------------------------------------------------------
// Mac-specific data associated with each socket by GSocketCFManager
// ----------------------------------------------------------------------------

class MacGSocketData
{
public:
    // default ctor creates the object in uninitialized state, use Initialize()
    // later to make it usable
    MacGSocketData()
    {
        m_socket = NULL;
        m_source = NULL;
    }

    // initialize the data associated with the given socket
    bool Initialize(GSocket *socket)
    {
        wxASSERT_MSG( !IsInitialized(), "shouldn't be called twice" );

        // we need a valid Unix socket to create a CFSocket
        if ( socket->m_fd < 0 )
            return false;

        CFSocketContext cont;
        cont.version = 0;               // this currently must be 0
        cont.info = socket;             // pointer passed to our callback
        cont.retain = NULL;             // no need to retain/release/copy the
        cont.release = NULL;            //  socket pointer, so all callbacks
        cont.copyDescription = NULL;    //  can be left NULL

        m_socket = CFSocketCreateWithNative
                   (
                        NULL,                   // default allocator
                        socket->m_fd,
                        kCFSocketReadCallBack |
                        kCFSocketWriteCallBack |
                        kCFSocketConnectCallBack,
                        SocketCallback,
                        &cont
                   );
        if ( !m_socket )
            return false;

        m_source = CFSocketCreateRunLoopSource(NULL, m_socket, 0);

        return m_source != NULL;
    }

    // free the objects created by Initialize()
    ~MacGSocketData()
    {
        if ( m_source )
            CFRelease(m_source);
        if ( m_socket )
            CFRelease(m_socket);
    }

    // return true if Initialize() had already been called successfully
    bool IsInitialized() const { return m_source && m_socket; }


    // accessors: should only be called if IsInitialized()
    CFSocketRef GetSocket() const
    {
        wxASSERT( IsInitialized() );

        return m_socket;
    }

    CFRunLoopSourceRef GetSource() const
    {
        wxASSERT( IsInitialized() );

        return m_source;
    }

private:
    static void SocketCallback(CFSocketRef WXUNUSED(s),
                               CFSocketCallBackType callbackType,
                               CFDataRef WXUNUSED(address),
                               const void* data,
                               void* info)
    {
        GSocket * const socket = static_cast<GSocket *>(info);
        MacGSocketData * const
            macdata = static_cast<MacGSocketData *>(socket->m_gui_dependent);
        if ( !macdata )
            return;

        switch (callbackType)
        {
            case kCFSocketConnectCallBack:
                wxASSERT(!socket->m_server);
                // KH: If data is non-NULL, the connect failed, do not call Detected_Write,
                // which will only end up creating a spurious connect event because the
                // call to getsocketopt SO_ERROR inexplicably returns no error.
                // The change in behavior cannot be traced to any particular commit or
                // timeframe so I'm not sure what to think, but after so many hours,
                // this seems to address the issue and it's time to move on.
                if (data == NULL)
                    socket->Detected_Write();
                break;

            case kCFSocketReadCallBack:
                socket->Detected_Read();
                break;

            case kCFSocketWriteCallBack:
                socket->Detected_Write();
                break;

            default:
                wxFAIL_MSG( "unexpected socket callback" );
        }
    }

    CFSocketRef m_socket;
    CFRunLoopSourceRef m_source;

    DECLARE_NO_COPY_CLASS(MacGSocketData);
};

// ----------------------------------------------------------------------------
// CoreFoundation implementation of GSocketManager
// ----------------------------------------------------------------------------

class GSocketCFManager : public GSocketManager
{
public:
    virtual bool OnInit();
    virtual void OnExit();

    virtual bool Init_Socket(GSocket *socket);
    virtual void Destroy_Socket(GSocket *socket);

    virtual void Install_Callback(GSocket *socket, GSocketEvent event);
    virtual void Uninstall_Callback(GSocket *socket, GSocketEvent event);

    virtual void Enable_Events(GSocket *socket);
    virtual void Disable_Events(GSocket *socket);

private:
    // retrieve our custom data associated with the given socket
    //
    // this is a low level function, use GetInitializedData() instead if the
    // data pointer should also be correctly initialized if it hadn't been done
    // yet
    //
    // may return NULL if we hadn't created the data for this socket yet
    MacGSocketData *GetData(GSocket *socket) const
    {
        return static_cast<MacGSocketData *>(socket->m_gui_dependent);
    }

    // return the custom data pointer initializing it if it hadn't been done
    // yet
    //
    // may return NULL if there is no associated data
    MacGSocketData *GetInitializedData(GSocket *socket) const
    {
        MacGSocketData * const data = GetData(socket);
        if ( data && !data->IsInitialized() )
        {
            if ( !data->Initialize(socket) )
                return NULL;
        }

        return data;
    }

    // return CFSocket callback mask corresponding to the given event (the
    // socket parameter is needed because some events are interpreted
    // differently depending on whether they happen on a server or on a client
    // socket)
    static int GetCFCallback(GSocket *socket, GSocketEvent event);


    // Sockets must use the event loop on the main thread so we store a
    // reference to the main loop here in OnInit()
    static CFRunLoopRef ms_mainRunLoop;
};

CFRunLoopRef GSocketCFManager::ms_mainRunLoop = NULL;

bool GSocketCFManager::OnInit()
{
    // No need to store the main loop again
    if (ms_mainRunLoop != NULL)
        return true;

    // Get the loop for the main thread so our events will actually fire.
    // The common socket.cpp code will assert if initialize is called from a
    // secondary thread, otherwise Mac would have the same problems as MSW
    ms_mainRunLoop = CFRunLoopGetCurrent();
    if ( !ms_mainRunLoop )
        return false;

    CFRetain(ms_mainRunLoop);

    return true;
}

void GSocketCFManager::OnExit()
{
    // Release the reference count, and set the reference back to NULL
    CFRelease(ms_mainRunLoop);
    ms_mainRunLoop = NULL;
}

bool GSocketCFManager::Init_Socket(GSocket *socket)
{
    socket->m_gui_dependent = new MacGSocketData;
    return true;
}

void GSocketCFManager::Destroy_Socket(GSocket *socket)
{
    MacGSocketData * const data = GetData(socket);
    if ( data )
    {
        delete data;
        socket->m_gui_dependent = NULL;
    }
}

/* static */
int GSocketCFManager::GetCFCallback(GSocket *socket, GSocketEvent event)
{
    switch ( event )
    {
        case GSOCK_CONNECTION:
            return socket->m_server ? kCFSocketReadCallBack
                                    : kCFSocketConnectCallBack;

        case GSOCK_LOST:
        case GSOCK_INPUT:
            return kCFSocketReadCallBack;

        case GSOCK_OUTPUT:
            return kCFSocketWriteCallBack;

        case GSOCK_MAX_EVENT:
            wxFAIL_MSG( "invalid GSocketEvent" );
            return 0;

        default:
            wxFAIL_MSG( "unknown GSocketEvent" );
            return 0;
    }
}

void GSocketCFManager::Install_Callback(GSocket *socket, GSocketEvent event)
{
    const MacGSocketData * const data = GetInitializedData(socket);
    if ( !data )
        return;

    CFSocketEnableCallBacks(data->GetSocket(), GetCFCallback(socket, event));
}

void GSocketCFManager::Uninstall_Callback(GSocket *socket, GSocketEvent event)
{
    const MacGSocketData * const data = GetInitializedData(socket);
    if ( !data )
        return;

    CFSocketDisableCallBacks(data->GetSocket(), GetCFCallback(socket, event));
}

void GSocketCFManager::Enable_Events(GSocket *socket)
{
    const MacGSocketData * const data = GetInitializedData(socket);
    if ( !data )
        return;

    CFRunLoopAddSource(ms_mainRunLoop, data->GetSource(), kCFRunLoopCommonModes);
}

void GSocketCFManager::Disable_Events(GSocket *socket)
{
    const MacGSocketData * const data = GetInitializedData(socket);
    if ( !data )
        return;

    // CFSocketInvalidate does CFRunLoopRemoveSource anyway
    CFRunLoopRemoveSource(ms_mainRunLoop, data->GetSource(), kCFRunLoopCommonModes);
    CFSocketInvalidate(data->GetSocket());

    // CFSocketInvalidate has closed the socket so we want to make sure GSocket knows this
    socket->m_fd = -1;
}

GSocketManager *wxAppTraits::GetSocketManager()
{
    static GSocketCFManager s_manager;

    return &s_manager;
};

#endif // wxUSE_SOCKETS
