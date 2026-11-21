/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/propgridwinui.h
// Purpose:     Private retained presentation of the wxPropertyGrid model
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_PROPGRID_WINUI_H_
#define _WX_PROPGRID_WINUI_H_
#include "wx/defs.h"
#include <memory>
#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_PROPGRID
class wxPropertyGrid;
class wxPGProperty;
class wxRect;

class wxPGWinUIProjection final
{
public:
    static std::shared_ptr<wxPGWinUIProjection> Create(wxPropertyGrid* grid);
    ~wxPGWinUIProjection();
    void Invalidate(const wxRect* rect = nullptr);
    void Update();
    bool IsPublished() const;
    void Close();
    void BeginGeometry();
    void EndGeometry();
    // Called before page-state reclamation, not after a freed pointer can be
    // reused. A removal transaction cannot publish during a nested event loop.
    void BeginRemoval(wxPGProperty* property);
    void EndRemoval();
private:
    struct Impl;
    explicit wxPGWinUIProjection(std::shared_ptr<Impl> impl);
    std::shared_ptr<Impl> m_impl;
};
#endif
#endif
