/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    src/mac/corefoundation/gsockosx.c
 * Purpose: GSocket: Mac OS X mach-o part
 * CVSID:   $Id$
 * Mac code by Brian Victor, February 2002.  Email comments to bhv1@psu.edu
 * ------------------------------------------------------------------------- */

#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include <stdlib.h>
#include "wx/gsocket.h"

#include <CoreFoundation/CoreFoundation.h>

#define ALL_CALLBACK_TYPES (kCFSocketReadCallBack | kCFSocketWriteCallBack | kCFSocketConnectCallBack)

class MacGSocketData
{
  MacGSocketData()
  {
    socket = NULL;
    source = NULL;
  }
  
  ~MacGSocketData()
  {
    if ( data->source )
      CFRelease(source);
    if ( data->socket )
      CFRelease(socket);
  }
  
  CFSocketRef socket;
  CFRunLoopSourceRef source;
};

#define DATATYPE MacGSocketData
#define PLATFORM_POINTER(x) ((DATATYPE*)x)

// Sockets must use the event loop on the main thread
// We will store the main loop's reference when Initialize is called
static CFRunLoopRef s_mainRunLoop = NULL;

int TranslateEventCondition(GSocket* socket, GSocketEvent event) {
  switch (event)
  {
    case GSOCK_LOST:        // Fallback
    case GSOCK_INPUT:      return kCFSocketReadCallBack; 
    case GSOCK_OUTPUT:   return kCFSocketWriteCallBack;
    case GSOCK_CONNECTION: return socket->m_server ? kCFSocketReadCallBack : kCFSocketConnectCallBack;
    default: wxASSERT(0); return 0; // This is invalid
  }	
}

void Mac_Socket_Callback(CFSocketRef s, CFSocketCallBackType callbackType,
                         CFDataRef address, const void* data, void* info)
{
  GSocket* socket = (GSocket*)info;

  if (!socket->m_platform_specific_data)
    return;
  
  switch (callbackType)
  {
    case kCFSocketConnectCallBack:
      assert(!socket->m_server);
      // KH: If data is non-NULL, the connect failed, do not call Detected_Write,
      // which will only end up creating a spurious connect event because the
      // call to getsocketopt SO_ERROR inexplicably returns no error.
      // The change in behavior cannot be traced to any particular commit or
      // timeframe so I'm not sure what to think, but after so many hours,
      // this seems to address the issue and it's time to move on.
      if (data == NULL)
        socket->Detected_Connect();
      else
        socket->Detected_Lost();
      break;
    case kCFSocketReadCallBack:
      socket->Detected_Read();
      break;
    case kCFSocketWriteCallBack:
      socket->Detected_Write();
      break;
    default:
      break;  /* We shouldn't get here. */
  }
}

DATATYPE* _GSocket_Get_Mac_Socket(GSocket *socket)
{
  /* If socket is already created, returns a pointer to the data */
  /* Otherwise, creates socket and returns the pointer */
  CFSocketContext cont;
  DATATYPE* data = PLATFORM_POINTER(socket->m_platform_specific_data);

  if (data && data->source) 
    return ;

  /* CFSocket has not been created, create it: */
  if (socket->m_fd < 0 || !data) 
    return NULL;
  
  cont.version = 0; 
  cont.retain = NULL;
  cont.release = NULL; 
  cont.copyDescription = NULL;
  cont.info = socket;

  CFSocketRef cf = CFSocketCreateWithNative(NULL, socket->m_fd,
                   ALL_CALLBACK_TYPES, Mac_Socket_Callback, &cont);
  CFRunLoopSourceRef source = CFSocketCreateRunLoopSource(NULL, cf, 0);
  assert(source);

  /* Keep the source and the socket around. */
  data->source = source;
  data->socket = cf;

  return data;
}

bool GSocketGUIFunctionsTableConcrete::CanUseEventLoop()
{   return true; }

bool GSocketGUIFunctionsTableConcrete::OnInit(void)
{
    // No need to store the main loop again
    if (s_mainRunLoop != NULL)
        return true;

    // Get the loop for the main thread so our events will actually fire.
    // The common socket.cpp code will assert if initialize is called from a
    // secondary thread, otherwise Mac would have the same problems as MSW
    s_mainRunLoop = CFRunLoopGetCurrent();
    CFRetain(s_mainRunLoop);

    return true;
}

void GSocketGUIFunctionsTableConcrete::OnExit(void)
{
    // Release the reference count, and set the reference back to NULL
    CFRelease(s_mainRunLoop);
    s_mainRunLoop = NULL;
}

bool GSocketGUIFunctionsTableConcrete::Init_Socket(GSocket *socket)
{
  wxCHECK_MSG( !socket->m_platform_specific_data, false, wxT("Critical: Double inited socket.") );
  
  socket->m_platform_specific_data = (void*) new DATATYPE;
  return (socket->m_platform_specific_data != NULL);
}

void GSocketGUIFunctionsTableConcrete::Destroy_Socket(GSocket *socket)
{
  wxCHECK_RET( socket->m_platform_specific_data, wxT("Critical: Destroying non-inited socket or double destroy.") );
  
  if (socket->m_platform_specific_data)
    delete PLATFORM_POINTER(socket->m_platform_specific_data);
  socket->m_platform_specific_data = NULL;
}

void GSocketGUIFunctionsTableConcrete::Enable_Events(GSocket *socket)
{
    DATATYPE* data = _GSocket_Get_Mac_Socket(socket);
  
    if (!data)
      return;

    socket->m_eventflags = ALL_CALLBACK_TYPES;
    
    CFRunLoopAddSource(s_mainRunLoop, data->source, kCFRunLoopCommonModes);
}

void GSocketGUIFunctionsTableConcrete::Disable_Events(GSocket *socket)
{
    DATATYPE* data = _GSocket_Get_Mac_Socket(socket);
  
    if (!data) 
      return;

    socket->m_eventflags = 0;
    
    CFSocketInvalidate(data->socket);
}

void GSocketGUIFunctionsTableConcrete::Enable_Event(GSocket *socket, GSocketEvent event)
{
    DATATYPE* data = _GSocket_Get_Mac_Socket(socket);
  
    if (!data) 
      return;
    
    wxCHECK_RET(!(socket->m_eventflags & TranslateEventCondition(socket, event)), wxT("Warning: Trying to re-enable an already enabled event callback"));
    
    socket->m_eventflags |= TranslateEventCondition(socket, event);
    
    CFSocketEnableCallBacks(data->socket, TranslateEventCondition(socket,event));
}

void GSocketGUIFunctionsTableConcrete::Disable_Event(GSocket *socket, GSocketEvent event)
{
    DATATYPE* data = _GSocket_Get_Mac_Socket(socket);
  
    if (!data) 
      return;
    
    wxCHECK_RET((socket->m_eventflags & TranslateEventCondition(socket, event)), wxT("Warning: Trying to re-disable an already disabled event callback"));
    
    socket->m_eventflags &= ~TranslateEventCondition(socket, event);
    
    CFSocketDisableCallBacks(data->socket, TranslateEventCondition(socket,event));
}

#endif // wxUSE_SOCKETS
