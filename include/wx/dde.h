#ifndef _WX_DDE_H_BASE_
#define _WX_DDE_H_BASE_

#include "wx/list.h"

class wxDDEClient;
class wxDDEServer;
class wxDDEConnection;

WX_DECLARE_USER_EXPORTED_LIST(wxDDEClient, wxDDEClientList, WXDLLEXPORT_BASE);
WX_DECLARE_USER_EXPORTED_LIST(wxDDEServer, wxDDEServerList, WXDLLEXPORT_BASE);
WX_DECLARE_USER_EXPORTED_LIST(wxDDEConnection, wxDDEConnectionList, WXDLLEXPORT_BASE);

#if defined(__WXMSW__)
    #include "wx/msw/dde.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/dde.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/dde.h"
#elif defined(__WXMAC__)
    #include "wx/mac/dde.h"
#elif defined(__WXPM__)
    #include "wx/os2/dde.h"
#endif

#endif
    // _WX_DDE_H_BASE_
