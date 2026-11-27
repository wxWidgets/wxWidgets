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
class wxBitmap;
class wxPropertyGrid;
class wxPGProperty;
class wxRect;
class wxWindow;
struct wxPGPaintData;

// The measures of the cells of a property grid of WinUI, in DIPs. The cell
// which shows a value and the editor which takes its place while the value
// is being edited use the same ones: nothing moves when the editor comes.
enum
{
    // Between the edges of a value cell and its editor.
    wxPG_WINUI_VALUE_INSET = 2,

    // Between the edge of a value cell and its text: the inset of its editor
    // and what a WinUI TextBox keeps before its text.
    wxPG_WINUI_TEXT_INSET = 13,

    // The picture before a value (the colour of a colour property): where
    // it starts in the cell, its side, and what separates it from the text.
    wxPG_WINUI_SWATCH_START = 12,
    wxPG_WINUI_SWATCH_SIDE = 16,
    wxPG_WINUI_SWATCH_GAP = 8,

    // The height of an editor, lower than its row.
    wxPG_WINUI_EDITOR_HEIGHT = 32,

    // The height of an item of the list of a WinUI ComboBox.
    wxPG_WINUI_CHOICE_HEIGHT = 36
};

// The picture a property paints before its value or before a choice of its
// list, as WinUI shows a swatch: a square with rounded corners and a thin
// outline, of the given side in pixels. What is outside its corners is
// transparent. Invalid if the property painted nothing.
wxBitmap wxPGWinUIPaintSwatch(wxWindow* window, wxPGProperty* property,
                              wxPGPaintData& paint, int side);

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
