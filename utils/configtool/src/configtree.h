/////////////////////////////////////////////////////////////////////////////
// Name:        configtree.h
// Purpose:     wxWidgets Configuration Tool tree class
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-03
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////


#ifndef _CT_CONFIGTREE_H_
#define _CT_CONFIGTREE_H_

#include "wx/wx.h"
#include "wx/treectrl.h"

#include "configitem.h"
#include "utils.h"

/*!
 * ctTreeItemData
 * Holds the ctConfigItem for each tree item.
 */

class ctTreeItemData : public wxTreeItemData
{
public:
    ctTreeItemData(ctConfigItem* item) : m_configItem(item) { }
    virtual ~ctTreeItemData() ;

    ctConfigItem *GetConfigItem() const { return m_configItem; }
    void SetConfigItem(ctConfigItem *item) { m_configItem = item; }

private:
    ctConfigItem*   m_configItem;
};


/*!
 * ctConfigTreeCtrl
 * The options hierarchy viewer.
 */

class ctConfigTreeCtrl: public wxTreeCtrl
{
    DECLARE_CLASS(ctConfigTreeCtrl)
public:
    ctConfigTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt = wxDefaultPosition,
        const wxSize& sz = wxDefaultSize, long style = wxTR_HAS_BUTTONS);
    virtual ~ctConfigTreeCtrl();

//// Event handlers    
    void OnMouseEvent(wxMouseEvent& event);
    void OnSelChanged(wxTreeEvent& event);
    void OnHelp(wxHelpEvent& event);
    void OnKeyDown(wxKeyEvent& event);

//// Accessors

    /// Get the table of icons
    wxIconTable& GetIconTable() { return m_iconTable; }

    /// Get the context menu
    wxMenu* GetMenu() { return m_contextMenu; }

    /// Get the item associated with the context menu events
    ctConfigItem* GetContextItem() { return m_contextItem; }

//// Operations

    /// Loads the icons.
    void LoadIcons();

protected:
    wxImageList*        m_imageList;
    wxIconTable         m_iconTable;
    wxMenu*             m_contextMenu;
    // The item associated with the context menu events
    ctConfigItem*       m_contextItem;

    DECLARE_EVENT_TABLE()
};

#endif
// _CT_CONFIGTREE_H_
