#ifndef _WX_LISTCTRL_H_BASE_
#define _WX_LISTCTRL_H_BASE_

#if defined(__WXMSW__)
#ifdef __WIN16__
#include "wx/generic/listctrl.h"
#else
#include "wx/msw/listctrl.h"
#endif
#elif defined(__WXMOTIF__)
#include "wx/generic/listctrl.h"
#elif defined(__WXGTK__)
#include "wx/generic/listctrl.h"
#elif defined(__WXQT__)
#include "wx/generic/listctrl.h"
#elif defined(__WXMAC__)
#include "wx/generic/listctrl.h"
#elif defined(__WXPM__)
#include "wx/generic/listctrl.h"
#elif defined(__WXSTUBS__)
#include "wx/generic/listctrl.h"
#endif

// ----------------------------------------------------------------------------
// wxListEvent - the event class for the wxListCtrl notifications
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListEvent : public wxNotifyEvent
{
public:
    wxListEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

    int           m_code;
    long          m_itemIndex;
    long          m_oldItemIndex;
    int           m_col;
    bool          m_cancelled;
    wxPoint       m_pointDrag;

    wxListItem    m_item;

    inline int GetCode() { return m_code; }
    inline long GetIndex() { return m_itemIndex; }
    inline long GetOldIndex() { return m_oldItemIndex; }
    inline long GetItem() { return m_itemIndex; }
    inline long GetOldItem() { return m_oldItemIndex; }
    inline int GetColumn() { return m_col; }
    inline bool Cancelled() { return m_cancelled; }
    inline wxPoint GetPoint() { return m_pointDrag; }
    inline const wxString &GetLabel() const { return m_item.m_text; }
    inline const wxString &GetText() const { return m_item.m_text; }
    inline int GetImage() { return m_item.m_image; }
    inline long GetData() { return m_item.m_data; }
    inline long GetMask() { return m_item.m_mask; }
    inline const wxListItem &GetItem() const { return m_item; }

#ifndef __WXMSW__
    void CopyObject(wxObject& object_dest) const;
#endif

private:
    DECLARE_DYNAMIC_CLASS(wxListEvent)
};

typedef void (wxEvtHandler::*wxListEventFunction)(wxListEvent&);

#define EVT_LIST_BEGIN_DRAG(id, fn) { wxEVT_COMMAND_LIST_BEGIN_DRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_BEGIN_RDRAG(id, fn) { wxEVT_COMMAND_LIST_BEGIN_RDRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_BEGIN_LABEL_EDIT(id, fn) { wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_END_LABEL_EDIT(id, fn) { wxEVT_COMMAND_LIST_END_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_DELETE_ITEM(id, fn) { wxEVT_COMMAND_LIST_DELETE_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_DELETE_ALL_ITEMS(id, fn) { wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_GET_INFO(id, fn) { wxEVT_COMMAND_LIST_GET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_SET_INFO(id, fn) { wxEVT_COMMAND_LIST_SET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_ITEM_SELECTED(id, fn) { wxEVT_COMMAND_LIST_ITEM_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_ITEM_DESELECTED(id, fn) { wxEVT_COMMAND_LIST_ITEM_DESELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_KEY_DOWN(id, fn) { wxEVT_COMMAND_LIST_KEY_DOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_INSERT_ITEM(id, fn) { wxEVT_COMMAND_LIST_INSERT_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_COL_CLICK(id, fn) { wxEVT_COMMAND_LIST_COL_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_ITEM_RIGHT_CLICK(id, fn) { wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_ITEM_MIDDLE_CLICK(id, fn) { wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_ITEM_ACTIVATED(id, fn) { wxEVT_COMMAND_LIST_ITEM_ACTIVATED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },

#endif
    // _WX_LISTCTRL_H_BASE_
