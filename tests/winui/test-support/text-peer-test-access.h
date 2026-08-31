/////////////////////////////////////////////////////////////////////////////
// Private, non-installed WinUI text-peer lifetime test access.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_TEXT_PEER_TEST_ACCESS_H
#define WX_WINUI_TEXT_PEER_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"

#include <cstddef>

#if wxUSE_WINUI3

class WXDLLIMPEXP_CORE wxWinUITextPeerTestAccess final
{
public:
    static std::size_t GetLiveCallbackStateCount();
};

#endif // wxUSE_WINUI3

#endif // WX_WINUI_TEXT_PEER_TEST_ACCESS_H
