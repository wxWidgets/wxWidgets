/////////////////////////////////////////////////////////////////////////////
// Private, non-installed adapters for the existing host input paths.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_HOST_INPUT_TEST_ACCESS_H
#define WX_WINUI_HOST_INPUT_TEST_ACCESS_H
#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif
#include "wx/winui/private/tlwhost.h"
#if wxUSE_WINUI3
class WXDLLIMPEXP_CORE wxWinUIHostInputTestAccess final
{
public:
    static bool PointerContactActiveSnapshot(
        wxWinUIInputDevice device,
        int virtualKey,
        unsigned pointerType,
        unsigned pointerFlags,
        unsigned penFlags);
    static wxWinUIRootPointerOutcome RouteRootPointerSample(
        wxWinUITopLevelHost& host,
        const wxWinUIPointerSample& sample,
        bool sourceAlreadyHandled = false);
    static bool RouteRootPointerBoundary(
        wxWinUITopLevelHost& host,
        bool entered,
        const wxWinUIPointerSample& originalSample);
    static void HandleRootPointerInterrupted(
        wxWinUITopLevelHost& host,
        bool captureLost,
        const wxWinUIPointerSample& sample);
    static bool RouteSlotPointer(
        wxWinUITopLevelHost& host,
        wxWindow *window,
        const wxWinUIPointerSample& sample,
        wxWinUIInputSurface physicalSurface = wxWinUIInputSurface::Native);
    static bool MapRootPointerPointForSlot(
        const wxWinUITopLevelHost& host,
        wxWindow *window,
        double xDIP,
        double yDIP,
        wxPoint *screen,
        bool *inside);
    static bool RouteOwnedSlotRelease(
        wxWinUITopLevelHost& host,
        const wxWinUIPointerSample& sample);
    static bool RememberModalAwaitingRelease(
        wxWinUITopLevelHost& host,
        const wxWinUIInputAction& action);
    static wxWinUIInputAction TakeModalAwaitingRelease(
        wxWinUITopLevelHost& host,
        wxWinUIInputDevice device,
        std::uint32_t pointerId,
        wxWinUIInputButton button);
    static std::size_t GetModalAwaitingReleaseCount(
        const wxWinUITopLevelHost& host);
    static void CancelSlotPointerOwners(
        wxWinUITopLevelHost& host,
        const wxWinUIPointerSample& sample,
        bool allPointers);
    static std::size_t GetSlotPointerOwnerCount(
        const wxWinUITopLevelHost& host);
    static std::uint32_t GetInputSiteGeneration(
        const wxWinUITopLevelHost& host);
    static bool CanPostIslandCancelMode(
        const wxWinUITopLevelHost& host,
        std::uint32_t expectedGeneration);
    static void PostIslandCancelMode(
        wxWinUITopLevelHost& host,
        std::uint32_t expectedGeneration);
    static bool HasDeferredIslandCancel(
        const wxWinUITopLevelHost& host);
    static unsigned GetPostedIslandCancelCount(
        const wxWinUITopLevelHost& host);
    static void RetireIslandCancelTarget(
        wxWinUITopLevelHost& host);
};
#endif // wxUSE_WINUI3
#endif // WX_WINUI_HOST_INPUT_TEST_ACCESS_H
