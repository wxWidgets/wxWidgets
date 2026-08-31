/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to deterministic coordinate arithmetic.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_VISUAL_COORDINATE_TEST_ACCESS_H
#define WX_WINUI_VISUAL_COORDINATE_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"

#if wxUSE_WINUI3

namespace winrt::Windows::Foundation
{
struct Point;
struct Rect;
}

class WXDLLIMPEXP_CORE wxWinUIVisualCoordinateTestAccess final
{
public:
    using Point = winrt::Windows::Foundation::Point;
    using Rect = winrt::Windows::Foundation::Rect;

    static bool RoundTripPhysicalSpaces(
        const Rect& rootScreenPixels,
        const Rect& anchorScreenPixels,
        bool anchorMirrored,
        double scale,
        const Point& clientPixels,
        Point *rootDips,
        Point *roundTrippedClientPixels);
};

#endif // wxUSE_WINUI3

#endif // WX_WINUI_VISUAL_COORDINATE_TEST_ACCESS_H
