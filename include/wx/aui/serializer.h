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

// ----------------------------------------------------------------------------
// Classes used to save/load wxAuiManager layout.
// ----------------------------------------------------------------------------

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
    virtual void SavePane(const wxAuiPaneInfo& pane) = 0;

    // Called after the last call to SavePane(), does nothing by default.
    virtual void AfterSavePanes() { }

    // Called before starting to save information about the docks, does nothing
    // by default.
    virtual void BeforeSaveDocks() { }

    // Save information about the given dock.
    virtual void SaveDock(const wxAuiDockInfo& dock) = 0;

    // Called after the last call to SaveDock(), does nothing by default.
    virtual void AfterSaveDocks() { }

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
    virtual std::vector<wxAuiPaneInfo> LoadPanes() = 0;

    // Create the window to be managed by the given pane: this is called if any
    // of the panes returned by LoadPanes() doesn't exist in the existing
    // layout and allows to create windows on the fly.
    //
    // If this function returns nullptr, the pane is not added to the manager.
    virtual wxWindow* CreatePaneWindow(const wxAuiPaneInfo& WXUNUSED(pane))
    {
        return nullptr;
    }

    // Load information about all the docks previously saved with SaveDock().
    virtual std::vector<wxAuiDockInfo> LoadDocks() = 0;

    // Called after restoring everything, calls Update() on the manager by
    // default.
    virtual void AfterLoad() { m_manager.Update(); }

protected:
    // The manager for which we're restoring the layout.
    wxAuiManager& m_manager;
};

#endif // _WX_AUI_SERIALIZER_H_
