/////////////////////////////////////////////////////////////////////////////
// Name:        tabctrl.h
// Purpose:     wxTabCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __TABCTRLH__
#define __TABCTRLH__

#ifdef __GNUG__
#pragma interface "tabctrl.h"
#endif

class wxImageList;

// WXDLLEXPORT_DATA(extern const char*) wxToolBarNameStr;

/*
 * Flags returned by HitTest
 */

#define wxTAB_HITTEST_NOWHERE           1
#define wxTAB_HITTEST_ONICON            2
#define wxTAB_HITTEST_ONLABEL           4
#define wxTAB_HITTEST_ONITEM            6

class WXDLLEXPORT wxTabCtrl: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxTabCtrl)
 public:
   /*
    * Public interface
    */

    wxTabCtrl(void);

    inline wxTabCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            const long style = 0, const wxString& name = "tabCtrl")
    {
        Create(parent, id, pos, size, style, name);
    }
    ~wxTabCtrl(void);

// Accessors

    // Get the selection
    int GetSelection(void) const;

    // Get the associated image list
    wxImageList* GetImageList(void) const;

    // Get the number of items
    int GetItemCount(void) const;

    // Get the rect corresponding to the tab
    bool GetItemRect(const int item, wxRect& rect) const;

    // Get the number of rows
    int GetRowCount(void) const;

    // Get the item text
    wxString GetItemText(const int item) const ;

    // Get the item image
    int GetItemImage(const int item) const;

    // Get the item data
    void* GetItemData(const int item) const;

    // Set the selection
    int SetSelection(const int item);

    // Set the image list
    void SetImageList(wxImageList* imageList);

    // Set the text for an item
    bool SetItemText(const int item, const wxString& text);

    // Set the image for an item
    bool SetItemImage(const int item, const int image);

    // Set the data for an item
    bool SetItemData(const int item, void* data);

    // Set the size for a fixed-width tab control
    void SetItemSize(const wxSize& size);

    // Set the padding between tabs
    void SetPadding(const wxSize& padding);

// Operations

    bool Create(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                const long style = 0, const wxString& name = "tabCtrl");

    // Delete all items
    bool DeleteAllItems(void);

    // Delete an item
    bool DeleteItem(const int item);

    // Hit test
    int HitTest(const wxPoint& pt, long& flags);

    // Insert an item
    int InsertItem(const int item, const wxString& text, const int imageId = -1, void* data = NULL);

// Implementation

    // Call default behaviour
    void OnPaint(wxPaintEvent& event) { Default() ; }
    void OnSize(wxSizeEvent& event) { Default() ; }
    void OnMouseEvent(wxMouseEvent& event) { Default() ; }
    void OnKillFocus(wxFocusEvent& event) { Default() ; }

    void Command(wxCommandEvent& event);
    bool MSWCommand(const WXUINT param, const WXWORD id);
    bool MSWNotify(const WXWPARAM wParam, const WXLPARAM lParam);

    // Responds to colour changes
    void OnSysColourChanged(wxSysColourChangedEvent& event);

protected:
    wxImageList*    m_imageList;

DECLARE_EVENT_TABLE()
};

class WXDLLEXPORT wxTabEvent: public wxCommandEvent
{
  DECLARE_DYNAMIC_CLASS(wxTabEvent)

 public:
  wxTabEvent(WXTYPE commandType = 0, int id = 0);
};

typedef void (wxEvtHandler::*wxTabEventFunction)(wxTabEvent&);

#define EVT_TAB_SEL_CHANGED(id, fn) { wxEVT_COMMAND_TAB_SEL_CHANGED, \
  id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTabEventFunction) & fn, NULL },
#define EVT_TAB_SEL_CHANGING(id, fn) { wxEVT_COMMAND_TAB_SEL_CHANGING, \
  id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTabEventFunction) & fn, NULL },

#endif
    // __TABCTRLH__
