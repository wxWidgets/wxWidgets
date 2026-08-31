/////////////////////////////////////////////////////////////////////////////
// Private, non-installed WinUI appearance policy test adapters.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_APPEARANCE_TEST_ACCESS_H
#define WX_WINUI_APPEARANCE_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"

#if wxUSE_WINUI3

class WXDLLIMPEXP_CORE wxWinUIAppearanceTestAccess final
{
public:
    enum class HighContrastOverride
    {
        System,
        ForceOff,
        ForceOn
    };

    static void SetHighContrastOverride(HighContrastOverride value);
};

#endif // wxUSE_WINUI3

#endif // WX_WINUI_APPEARANCE_TEST_ACCESS_H
