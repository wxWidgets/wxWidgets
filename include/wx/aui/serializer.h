///////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/serializer.h
// Purpose:     Declaration of wxAuiSerializer and wxAuiDeserializer classes.
// Author:      Vadim Zeitlin
// Created:     2024-01-20
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_AUI_SERIALIZER_H_
#define _WX_AUI_SERIALIZER_H_

#include <utility>

// ----------------------------------------------------------------------------
// Classes used to save/load wxAuiManager layout.
// ----------------------------------------------------------------------------

// This struct contains the pane name and information about its layout that can
// be manipulated by the user interactively.
struct wxAuiPaneLayoutInfo
{
    // Ctor sets the name, which is always required.
    explicit wxAuiPaneLayoutInfo(wxString name_) : name{std::move(name_)} { }

    // Unique name of the pane.
    wxString name;


    // Identifies the dock containing the pane.
    int dock_direction   = wxAUI_DOCK_LEFT;
    int dock_layer       = 0;
    int dock_row         = 0;
    int dock_pos         = 0;
    int dock_proportion  = 0;

    // Size of the containing dock.
    //
    // Note that storing the dock size is redundant as it can be calculated
    // from pane sizes, but storing all pane sizes would be redundant too, so
    // we prefer to keep things simple and store just this size.
    int dock_size        = 0;


    // Floating pane geometry, may be invalid.
    wxPoint floating_pos = wxDefaultPosition;
    wxSize floating_size = wxDefaultSize;


    // True if the pane is currently maximized.
    //
    // Note that it's the only field of this struct which doesn't directly
    // correspond to a field of wxAuiPaneInfo.
    bool is_maximized   = false;
};

// wxAuiSerializer is used with wxAuiManager::SaveLayout().
//
// This is an abstract base class, you need to inherit from it and override its
// pure virtual functions in your derived class.
//
// If any of the functions of the derived class throw an exception, it is
// propagated out of wxAuiManager::SaveLayout() and it's callers responsibility
// to handle it.
class wxAuiSerializer
{
public:
    // Trivial default ctor.
    wxAuiSerializer() = default;

    // Trivial but virtual dtor for a base class.
    virtual ~wxAuiSerializer() = default;


    // Called before doing anything else, does nothing by default.
    virtual void BeforeSave() { }

    // Called before starting to save information about the panes, does nothing
    // by default.
    virtual void BeforeSavePanes() { }

    // Save information about the given pane.
    virtual void SavePane(const wxAuiPaneLayoutInfo& pane) = 0;

    // Called after the last call to SavePane(), does nothing by default.
    virtual void AfterSavePanes() { }

    // Called after saving everything, does nothing by default.
    virtual void AfterSave() { }
};

// wxAuiDeserializer is used with wxAuiManager::LoadLayout().
//
// As wxAuiSerializer, this is an abstract base class, you need to inherit from
// it and override its pure virtual functions in your derived class.
//
// Derived class function also may throw and, if any of them other than
// AfterLoad() does, the existing layout is not changed, i.e.
// wxAuiManager::LoadLayout() is exception-safe.
class wxAuiDeserializer
{
public:
    // Ctor takes the manager for which we're restoring the layout, it must
    // remain valid for the lifetime of this object.
    explicit wxAuiDeserializer(wxAuiManager& manager) : m_manager(manager) { }

    // Trivial but virtual dtor for a base class.
    virtual ~wxAuiDeserializer() = default;


    // Called before doing anything else, does nothing by default.
    virtual void BeforeLoad() { }

    // Load information about all the panes previously saved with SavePane().
    virtual std::vector<wxAuiPaneLayoutInfo> LoadPanes() = 0;

    // Create the window to be managed by the given pane: this is called if any
    // of the panes returned by LoadPanes() doesn't exist in the existing
    // layout and allows to create windows on the fly.
    //
    // If this function returns nullptr, the pane is not added to the manager.
    //
    // Note that the pane info may (and usually will, if a window is created)
    // be modified, to set fields such as caption or icon and any flags other
    // "maximized".
    virtual wxWindow* CreatePaneWindow(wxAuiPaneInfo& WXUNUSED(pane))
    {
        return nullptr;
    }

    // Called after restoring everything, calls Update() on the manager by
    // default.
    virtual void AfterLoad() { m_manager.Update(); }

protected:
    // The manager for which we're restoring the layout.
    wxAuiManager& m_manager;
};

#endif // _WX_AUI_SERIALIZER_H_
