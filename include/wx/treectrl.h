#ifndef _WX_TREECTRL_H_BASE_
#define _WX_TREECTRL_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/control.h"
#include "wx/event.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// enum for different images associated with a treectrl item
enum wxTreeItemIcon
{
    wxTreeItemIcon_Normal,              // not selected, not expanded
    wxTreeItemIcon_Selected,            //     selected, not expanded
    wxTreeItemIcon_Expanded,            // not selected,     expanded
    wxTreeItemIcon_SelectedExpanded,    //     selected,     expanded
    wxTreeItemIcon_Max
};


// values for the `flags' parameter of wxTreeCtrl::HitTest() which determine
// where exactly the specified point is situated:

static const int wxTREE_HITTEST_ABOVE            = 0x0001;
static const int wxTREE_HITTEST_BELOW            = 0x0002;
static const int wxTREE_HITTEST_NOWHERE          = 0x0004;
    // on the button associated with an item.
static const int wxTREE_HITTEST_ONITEMBUTTON     = 0x0008;
    // on the bitmap associated with an item.
static const int wxTREE_HITTEST_ONITEMICON       = 0x0010;
    // on the ident associated with an item.
static const int wxTREE_HITTEST_ONITEMIDENT      = 0x0020;
    // on the label (string) associated with an item.
static const int wxTREE_HITTEST_ONITEMLABEL      = 0x0040;
    // on the right of the label associated with an item.
static const int wxTREE_HITTEST_ONITEMRIGHT      = 0x0080;
    // on the label (string) associated with an item.
//static const int wxTREE_HITTEST_ONITEMSTATEICON  = 0x0100;
    // on the left of the wxTreeCtrl.
static const int wxTREE_HITTEST_TOLEFT           = 0x0200;
    // on the right of the wxTreeCtrl.
static const int wxTREE_HITTEST_TORIGHT          = 0x0400;
    // on the upper part (first half) of the item.
static const int wxTREE_HITTEST_ONITEMUPPERPART  = 0x0800;
    // on the lower part (second half) of the item.
static const int wxTREE_HITTEST_ONITEMLOWERPART  = 0x1000;

    // anywhere on the item
static const int wxTREE_HITTEST_ONITEM  = wxTREE_HITTEST_ONITEMICON |
                                          wxTREE_HITTEST_ONITEMLABEL;

// tree ctrl default name
WXDLLEXPORT_DATA(extern const char*) wxTreeCtrlNameStr;

// ----------------------------------------------------------------------------
// wxTreeItemAttr: a structure containing the visual attributes of an item
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTreeItemAttr
{
public:
    // ctors
    wxTreeItemAttr() { }
    wxTreeItemAttr(const wxColour& colText,
                   const wxColour& colBack,
                   const wxFont& font)
        : m_colText(colText), m_colBack(colBack), m_font(font) { }

    // setters
    void SetTextColour(const wxColour& colText) { m_colText = colText; }
    void SetBackgroundColour(const wxColour& colBack) { m_colBack = colBack; }
    void SetFont(const wxFont& font) { m_font = font; }

    // accessors
    bool HasTextColour() const { return m_colText.Ok(); }
    bool HasBackgroundColour() const { return m_colBack.Ok(); }
    bool HasFont() const { return m_font.Ok(); }

    const wxColour& GetTextColour() const { return m_colText; }
    const wxColour& GetBackgroundColour() const { return m_colBack; }
    const wxFont& GetFont() const { return m_font; }

private:
    wxColour m_colText,
             m_colBack;
    wxFont   m_font;
};

// ----------------------------------------------------------------------------
// include the platform-dependent wxTreeCtrl class
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #ifdef __WIN16__
        #include "wx/generic/treectrl.h"
    #else
        #include "wx/msw/treectrl.h"
    #endif
#elif defined(__WXMOTIF__)
    #include "wx/generic/treectrl.h"
#elif defined(__WXGTK__)
    #include "wx/generic/treectrl.h"
#elif defined(__WXQT__)
    #include "wx/qt/treectrl.h"
#elif defined(__WXMAC__)
    #include "wx/generic/treectrl.h"
#elif defined(__WXPM__)
    #include "wx/generic/treectrl.h"
#elif defined(__WXSTUBS__)
    #include "wx/generic/treectrl.h"
#endif

// ----------------------------------------------------------------------------
// wxTreeEvent is a special class for all events associated with tree controls
//
// NB: note that not all accessors make sense for all events, see the event
//     descriptions below
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTreeEvent : public wxNotifyEvent
{
    friend class WXDLLEXPORT wxTreeCtrl;

public:
    wxTreeEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

    // accessors
        // get the item on which the operation was performed or the newly
        // selected item for wxEVT_COMMAND_TREE_SEL_CHANGED/ING events
    wxTreeItemId GetItem() const { return m_item; }

        // for wxEVT_COMMAND_TREE_SEL_CHANGED/ING events, get the previously
        // selected item
    wxTreeItemId GetOldItem() const { return m_itemOld; }

        // the point where the mouse was when the drag operation started (for
        // wxEVT_COMMAND_TREE_BEGIN_(R)DRAG events only)
    wxPoint GetPoint() const { return m_pointDrag; }

        // keyboard code (for wxEVT_COMMAND_TREE_KEY_DOWN only)
    int GetCode() const { return m_code; }

        // label (for EVT_TREE_{BEGIN|END}_LABEL_EDIT only)
    const wxString& GetLabel() const { return m_label; }

private:
    // TODO we could save some space by using union here
    int           m_code;
    wxTreeItemId  m_item,
                  m_itemOld;
    wxPoint       m_pointDrag;
    wxString      m_label;

    DECLARE_DYNAMIC_CLASS(wxTreeEvent)
};

typedef void (wxEvtHandler::*wxTreeEventFunction)(wxTreeEvent&);

// ----------------------------------------------------------------------------
// macros for handling tree control events
// ----------------------------------------------------------------------------

// GetItem() returns the item being dragged, GetPoint() the mouse coords
#define EVT_TREE_BEGIN_DRAG(id, fn) { wxEVT_COMMAND_TREE_BEGIN_DRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_BEGIN_RDRAG(id, fn) { wxEVT_COMMAND_TREE_BEGIN_RDRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },

// GetItem() returns the itme whose label is being edited, GetLabel() returns
// the current item label for BEGIN and the would be new one for END.
//
// Vetoing BEGIN event means that label editing won't happen at all,
// vetoing END means that the new value is discarded and the old one kept
#define EVT_TREE_BEGIN_LABEL_EDIT(id, fn) { wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_END_LABEL_EDIT(id, fn) { wxEVT_COMMAND_TREE_END_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },

// provide/update information about GetItem() item
#define EVT_TREE_GET_INFO(id, fn) { wxEVT_COMMAND_TREE_GET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_SET_INFO(id, fn) { wxEVT_COMMAND_TREE_SET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },

// GetItem() is the item being expanded/collapsed, the "ING" versions can use
#define EVT_TREE_ITEM_EXPANDED(id, fn) { wxEVT_COMMAND_TREE_ITEM_EXPANDED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_EXPANDING(id, fn) { wxEVT_COMMAND_TREE_ITEM_EXPANDING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_COLLAPSED(id, fn) { wxEVT_COMMAND_TREE_ITEM_COLLAPSED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_COLLAPSING(id, fn) { wxEVT_COMMAND_TREE_ITEM_COLLAPSING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, NULL },

// GetOldItem() is the item which had the selection previously, GetItem() is
// the item which acquires selection
#define EVT_TREE_SEL_CHANGED(id, fn) { wxEVT_COMMAND_TREE_SEL_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, NULL },
#define EVT_TREE_SEL_CHANGING(id, fn) { wxEVT_COMMAND_TREE_SEL_CHANGING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, NULL },

// GetCode() returns the key code
// NB: this is the only message for which GetItem() is invalid (you may get the
//     item from GetSelection())
#define EVT_TREE_KEY_DOWN(id, fn) { wxEVT_COMMAND_TREE_KEY_DOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, NULL },

// GetItem() returns the item being deleted, the associated data (if any) will
// be deleted just after the return of this event handler (if any)
#define EVT_TREE_DELETE_ITEM(id, fn) { wxEVT_COMMAND_TREE_DELETE_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },

// GetItem() returns the item that was activated (double click, enter, space)
#define EVT_TREE_ITEM_ACTIVATED(id, fn) { wxEVT_COMMAND_TREE_ITEM_ACTIVATED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, NULL },

// GetItem() returns the item that was clicked on
#define EVT_TREE_ITEM_RIGHT_CLICK(id, fn) { wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, NULL },
#define EVT_TREE_ITEM_MIDDLE_CLICK(id, fn) { wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, NULL },

#endif
    // _WX_TREECTRL_H_BASE_


