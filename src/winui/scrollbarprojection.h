/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/scrollbarprojection.h
// Purpose:     Native non-client scrollbar presentation, HWND model authoritative
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_WINUI_SCROLLBARPROJECTION_H_
#define _WX_WINUI_SCROLLBARPROJECTION_H_

#include "wx/winui/private/tlwhost.h"

class wxWinUIScrollBarProjection final
    : public std::enable_shared_from_this<wxWinUIScrollBarProjection>
{
public:
    static std::shared_ptr<wxWinUIScrollBarProjection> Create(
        wxWindow* topLevel,
        const winrt::Microsoft::UI::Xaml::Controls::Canvas& root,
        const std::shared_ptr<wxWinUIHostLifetime>& lifetime);
    ~wxWinUIScrollBarProjection();
    void Sync(const RECT& clientScreen, double scale);
    bool Covers(HWND hwnd, LONG objectId) const;
    // Paint ownership is committed only with the bridge region containing the
    // corresponding native peer. Merely allocating a peer never hides MSW.
    void CommitRegion();
    void Shutdown();

    // Source-private NCPAINT plumbing, not a wx model or test interface.
    bool GetPaintRects(HWND hwnd, std::vector<RECT>& rects) const;
    bool Paints(HWND hwnd, LONG objectId) const;
    void ModelChanged(HWND hwnd, int orient, bool nativePaint);
    void ModelCommitted(HWND hwnd, int orient);
    void ObservePointer(HWND target, POINT screen, bool touch);

private:
    struct Impl;
    explicit wxWinUIScrollBarProjection(std::shared_ptr<Impl> impl);
    std::shared_ptr<Impl> m_impl;
};

// Observation only: never consumes, synthesizes or redispatches native input.
WXDLLIMPEXP_CORE void wxWinUIWindowScrollbarsObserveMessage(const MSG& message) noexcept;

#endif
