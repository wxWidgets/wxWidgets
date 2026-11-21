/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/headerprojection.h
// Purpose:     Source-private native presentation of wxHeaderCtrl
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_WINUI_HEADERPROJECTION_H_
#define _WX_WINUI_HEADERPROJECTION_H_
#include "wx/defs.h"
#include <memory>
#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_HEADERCTRL
class wxHeaderCtrl;
class wxRect;

// wxHeaderCtrl remains the sole column and pointer-gesture authority.
class wxWinUIHeaderProjection final
{
public:
    static std::shared_ptr<wxWinUIHeaderProjection> Create(wxHeaderCtrl* owner);
    ~wxWinUIHeaderProjection();
    void Invalidate(bool topologyChanged = false);
    void Update();
    void Close();
    bool IsPublished() const;
    void ShowMarkers(const wxRect& ghost, int insertionX);
    void ClearMarkers();
private:
    struct Impl;
    explicit wxWinUIHeaderProjection(std::shared_ptr<Impl> impl);
    std::shared_ptr<Impl> m_impl;
};
#endif
#endif
