/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/gridprojection.h
// Purpose:     Source-private, viewport-bounded native wxGrid presentation
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_GRIDPROJECTION_H_
#define _WX_WINUI_GRIDPROJECTION_H_

#include "wx/defs.h"
#include <memory>

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_GRID
class wxGrid;
class wxWindow;

// wxGrid owns all data, selection, line order, geometry and edit transactions.
// This object owns only the currently realized XAML presentation. In particular,
// it is not a second table and never eagerly enumerates all table values.
class wxWinUIGridProjection final
{
public:
    static std::shared_ptr<wxWinUIGridProjection> Create(wxGrid* grid);
    ~wxWinUIGridProjection();

    void Invalidate();
    void SyncViewport();
    void Update();
    void Close();
    bool OwnsWindow(wxWindow* window) const;

private:
    struct Impl;
    explicit wxWinUIGridProjection(std::shared_ptr<Impl> impl);
    std::shared_ptr<Impl> m_impl;
};
#endif
#endif
