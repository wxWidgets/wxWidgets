#ifndef _WX_DDE_H_BASE_
#define _WX_DDE_H_BASE_

#include "wx/list.h"

class WXDLLIMPEXP_BASE wxDDEClient;
class WXDLLIMPEXP_BASE wxDDEServer;
class WXDLLIMPEXP_BASE wxDDEConnection;

WX_DECLARE_USER_EXPORTED_LIST(wxDDEClient, wxDDEClientList, WXDLLIMPEXP_BASE);
WX_DECLARE_USER_EXPORTED_LIST(wxDDEServer, wxDDEServerList, WXDLLIMPEXP_BASE);
WX_DECLARE_USER_EXPORTED_LIST(wxDDEConnection, wxDDEConnectionList, WXDLLIMPEXP_BASE);

#if defined(__WXMSW__)
    #include "wx/msw/dde.h"
#else
    #error DDE is only supported on MSW
#endif

#endif
    // _WX_DDE_H_BASE_
