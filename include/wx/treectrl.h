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

#ifdef __WXMSW__
WXDLLEXPORT_DATA(extern const char*) wxTreeCtrlNameStr;
#else
#define wxTreeCtrlNameStr "wxTreeCtrl"
#endif

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
#ifdef __MWERKS__
    friend class wxTreeCtrl;
#else
    friend wxTreeCtrl;
#endif

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

#endif
    // _WX_TREECTRL_H_BASE_
