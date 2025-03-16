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

#include "wx/aui/framemanager.h" // Just for wxAUI_DOCK_LEFT

#if wxUSE_AUI

#include <utility>
#include <vector>

class WXDLLIMPEXP_FWD_AUI wxAuiNotebook;
class WXDLLIMPEXP_FWD_AUI wxAuiTabCtrl;

// ----------------------------------------------------------------------------
// Classes used to save/load wxAuiManager layout.
// ----------------------------------------------------------------------------

// Fields common to wxAuiPaneLayoutInfo and wxAuiTabLayoutInfo containing
// information about a docked pane or tab layout.
struct wxAuiDockLayoutInfo
{
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
};

// This struct contains information about the layout of a tab control in a
// wxAuiNotebook, including where it is docked and the order of pages in it.
struct wxAuiTabLayoutInfo : wxAuiDockLayoutInfo
{
    // If this vector is empty, it means that the tab control contains all
    // notebook pages in natural order.
    std::vector<int> pages;

    // Vectors contain indices of pinned pages, if any, i.e. it can be empty.
    std::vector<int> pinned;

    // Currently active page in this tab control.
    int active = 0;
};

// This struct contains the pane name and information about its layout that can
// be manipulated by the user interactively.
struct wxAuiPaneLayoutInfo : wxAuiDockLayoutInfo
{
    // Ctor sets the name, which is always required.
    explicit wxAuiPaneLayoutInfo(wxString name_) : name{std::move(name_)} { }

    // Unique name of the pane.
    wxString name;


    // Floating pane geometry, may be invalid.
    wxPoint floating_pos = wxDefaultPosition;
    wxSize floating_size = wxDefaultSize;


    // The remaining fields correspond to individual bits of the pane state
    // flags instead of corresponding to wxAuiPaneInfo fields directly because
    // we prefer not storing the entire state -- this would be less readable
    // and extensible.

    // True if the pane is currently maximized.
    bool is_maximized   = false;

    // True if the pane is currently hidden.
    bool is_hidden      = false;
};

// wxAuiBookSerializer is used for serializing wxAuiNotebook layout.
//
// This includes the tab controls layout and the order of pages in them.
//
// It can be used standalone with wxAuiNotebook::SaveLayout() or as base class
// of wxAuiSerializer for saving and restoring the entire layout.
class wxAuiBookSerializer
{
public:
    // Trivial default ctor.
    wxAuiBookSerializer() = default;

    // Trivial but virtual dtor for a base class.
    virtual ~wxAuiBookSerializer() = default;


    // Called before starting to save information about the tabs in the
    // notebook in the AUI pane with the given name.
    virtual void BeforeSaveNotebook(const wxString& name) = 0;

    // Called to save information about a single tab control in the given
    // notebook.
    virtual void SaveNotebookTabControl(const wxAuiTabLayoutInfo& tab) = 0;

    // Called after saving information about all the pages of the notebook in
    // the AUI pane with the given name, does nothing by default.
    virtual void AfterSaveNotebook() { }
};

// wxAuiSerializer is used with wxAuiManager::SaveLayout().
//
// This is an abstract base class, you need to inherit from it and override its
// pure virtual functions, including those inherited from wxAuiBookSerializer,
// in your derived class.
//
// If any of the functions of the derived class throw an exception, it is
// propagated out of wxAuiManager::SaveLayout() and it's callers responsibility
// to handle it.
class wxAuiSerializer : public wxAuiBookSerializer
{
public:
    // Trivial default ctor.
    wxAuiSerializer() = default;


    // Called before doing anything else, does nothing by default.
    virtual void BeforeSave() { }

    // Called before starting to save information about the panes, does nothing
    // by default.
    virtual void BeforeSavePanes() { }

    // Save information about the given pane.
    virtual void SavePane(const wxAuiPaneLayoutInfo& pane) = 0;

    // Called after the last call to SavePane(), does nothing by default.
    virtual void AfterSavePanes() { }

    // Called before starting to save information about the notebooks, does
    // nothing by default.
    virtual void BeforeSaveNotebooks() { }

    // Called after the last call to SaveNotebook(), does nothing by default.
    virtual void AfterSaveNotebooks() { }

    // Called after saving everything, does nothing by default.
    virtual void AfterSave() { }
};

// wxAuiBookDeserializer is used for deserializing wxAuiNotebook layout.
//
// Similarly to wxAuiBookSerializer, it can be used standalone with
// wxAuiNotebook::LoadLayout() or as base class of wxAuiDeserializer.
class wxAuiBookDeserializer
{
public:
    // Trivial default ctor.
    wxAuiBookDeserializer() = default;

    // Trivial but virtual dtor for a base class.
    virtual ~wxAuiBookDeserializer() = default;

    // Load information about all the tab controls in the pane containing
    // wxAuiNotebook with the given name.
    virtual std::vector<wxAuiTabLayoutInfo>
    LoadNotebookTabs(const wxString& name) = 0;

    // If any pages haven't been assigned to any tab control after restoring
    // the pages order, they are passed to this function to determine what to
    // do with them.
    //
    // By default, it returns true without modifying the output arguments,
    // which results in the page being appended to the main tab control. It may
    // also modify tabCtrl and tabIndex arguments to modify where the page
    // should appear or return false to remove the page from the notebook
    // completely.
    virtual bool
    HandleOrphanedPage(wxAuiNotebook& WXUNUSED(book),
                       int WXUNUSED(page),
                       wxAuiTabCtrl** WXUNUSED(tabCtrl),
                       int* WXUNUSED(tabIndex))
        { return true; }
};

// wxAuiDeserializer is used with wxAuiManager::LoadLayout().
//
// As wxAuiSerializer, this is an abstract base class, you need to inherit from
// it and override its pure virtual functions in your derived class.
//
// Derived class function also may throw and, if any of them other than
// AfterLoad() does, the existing layout is not changed, i.e.
// wxAuiManager::LoadLayout() is exception-safe.
class wxAuiDeserializer : public wxAuiBookDeserializer
{
public:
    // Ctor takes the manager for which we're restoring the layout, it must
    // remain valid for the lifetime of this object.
    explicit wxAuiDeserializer(wxAuiManager& manager) : m_manager(manager) { }


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

#endif // wxUSE_AUI

#endif // _WX_AUI_SERIALIZER_H_
