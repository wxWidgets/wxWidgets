/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/listprojection.h
// Purpose:     Source-private native presentation of the wxListCtrl model
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_WINUI_LISTPROJECTION_H_
#define _WX_WINUI_LISTPROJECTION_H_
#include "wx/defs.h"
#include <memory>
#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_LISTCTRL
class wxGenericListCtrl;
class wxWindow;
class wxFont;
class wxSize;

// No independent item store: values, geometry, selection and edit transactions
// remain owned by wxGenericListCtrl, including its virtual OnGetItem callbacks.
class wxWinUIListProjection final
{
public:
    static std::shared_ptr<wxWinUIListProjection> Create(wxGenericListCtrl* list);
    ~wxWinUIListProjection();
    void Invalidate();
    void SyncViewport();
    void NotifySelectionChanged();
    void Update();
    void Close();
    bool OwnsWindow(wxWindow* window) const;
    static bool MeasureItemText(wxWindow* window, const wxString& text,
                                const wxFont& font, wxSize& size);
private:
    struct Impl;
    explicit wxWinUIListProjection(std::shared_ptr<Impl> impl);
    std::shared_ptr<Impl> m_impl;
};
#endif
#endif
