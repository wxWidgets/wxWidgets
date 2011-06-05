/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/tabctrl.h
// Purpose:     wxMoTabCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_TABCTRL_H_
#define _WX_MOBILE_GENERIC_TABCTRL_H_

class WXDLLIMPEXP_CORE wxImageList;

#include "wx/mobile/generic/buttonstrip.h"

#define wxTAB_TEXT   0x00010000
#define wxTAB_BITMAP 0x00020000


#if wxUSE_TAB_DIALOG
#include "wx/tabctrl.h"
#else

/*
 * Flags returned by HitTest
 */

#define wxTAB_HITTEST_NOWHERE           1
#define wxTAB_HITTEST_ONICON            2
#define wxTAB_HITTEST_ONLABEL           4
#define wxTAB_HITTEST_ONITEM            6

class WXDLLEXPORT wxTabEvent : public wxNotifyEvent
{
public:
    wxTabEvent(wxEventType commandType = wxEVT_NULL, int id = 0,
                    int nSel = -1, int nOldSel = -1)
        : wxNotifyEvent(commandType, id)
        {
            m_nSel = nSel;
            m_nOldSel = nOldSel;
        }

    /// Get the currently selected page (-1 if none)
    int GetSelection() const { return m_nSel; }

    /// Set the currently selected page (-1 if none)
    void SetSelection(int nSel) { m_nSel = nSel; }

    /// Get the page that was selected before the change (-1 if none)
    int GetOldSelection() const { return m_nOldSel; }

    /// Set the page that was selected before the change (-1 if none)
    void SetOldSelection(int nOldSel) { m_nOldSel = nOldSel; }

private:
    int m_nSel,     // currently selected page
        m_nOldSel;  // previously selected page

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxTabEvent)
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGED, 800)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGING, 801)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxTabEventFunction)(wxTabEvent&);

#define EVT_TAB_SEL_CHANGED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_TAB_SEL_CHANGED, \
  id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTabEventFunction, & fn ), NULL),
#define EVT_TAB_SEL_CHANGING(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_TAB_SEL_CHANGING, \
  id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTabEventFunction, & fn ), NULL),

#endif
    // wxUSE_TAB_DIALOG

/**
    @class wxMoTabCtrl

    A tab control. Images can be referenced either using an image list,
    as per the standard wxWidgets tab/notebook API, or by passing
    bitmaps directly.

    TODO: customisation of the set of tabs.

    @category{wxMobile}
 */


class WXDLLEXPORT wxMoTabCtrl: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxMoTabCtrl)
 public:

    /// Default constructor.
    wxMoTabCtrl();

    /// Constructor.
    wxMoTabCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxTAB_TEXT|wxTAB_BITMAP, const wxString& name = wxT("tabCtrl"))
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }
    ~wxMoTabCtrl();

    bool Create(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxTAB_TEXT|wxTAB_BITMAP, const wxString& name = wxT("tabCtrl"));

    void Init();

// Accessors

    /// Get the selection
    int GetSelection() const;

    /// Get the tab with the current keyboard focus. Not implemented.
    int GetCurFocus() const;

    /// Get the associated image list
    wxImageList* GetImageList() const { return m_buttonStrip.GetImageList(); }

    /// Get the number of items
    int GetItemCount() const;

    /// Get the rect corresponding to the tab. Not implemented.
    bool GetItemRect(int item, wxRect& rect) const;

    // Get the number of rows
    int GetRowCount() const;

    /// Get the item text
    wxString GetItemText(int item) const ;

    /// Get the item image
    int GetItemImage(int item) const;

    // Get the item data
    //void* GetItemData(int item) const;

    /// Set the selection, generating events
    int SetSelection(int item);

    /// Set the selection, without generating events
    int ChangeSelection(int item);

    /// Set the image list
    void SetImageList(wxImageList* imageList);

    /// Assign (own) the image list
    void AssignImageList(wxImageList* imageList);

    /// Set the text for an item
    bool SetItemText(int item, const wxString& text);

    /// Set the image for an item
    bool SetItemImage(int item, int image);

    // Set the data for an item
    // bool SetItemData(int item, void* data);

    /// Set the size for a fixed-width tab control. Not implemented.
    void SetItemSize(const wxSize& size);

    /// Set the padding between tabs. Not implemented.
    void SetPadding(const wxSize& padding);

// Operations

    /// Delete all items
    bool DeleteAllItems();

    /// Delete an item
    bool DeleteItem(int item);

    /// Hit test. Not implemented.
    int HitTest(const wxPoint& pt, long& flags);

    /// Insert an item, passing an optional index into the image list.
    bool InsertItem(int item, const wxString& text, int imageId = -1);

    /// Insert an item, passing a bitmap.
    bool InsertItem(int item, const wxString& text, const wxBitmap& bitmap);

    /// Add an item, passing an optional index into the image list.
    bool AddItem(const wxString& text, int imageId = -1);

    /// Add an item, passing a bitmap.
    bool AddItem(const wxString& text, const wxBitmap& bitmap);

    /// Set a text badge for the given item
    bool SetBadge(int item, const wxString& badge);

    /// Get the text badge for the given item
    wxString GetBadge(int item) const;

// Implementation

    virtual wxSize DoGetBestSize() const;

    virtual void OnInsertItem(wxMoBarButton* button);

    void OnMouseEvent(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    bool SendCommand(wxEventType eventType, int selection);

    void OnSysColourChanged(wxSysColourChangedEvent& event);

    wxMoButtonStrip& GetButtonStrip() { return m_buttonStrip; }
    const wxMoButtonStrip& GetButtonStrip() const { return m_buttonStrip; }

    /// Sets the border colour.
    virtual void SetBorderColour(const wxColour &colour);

    /// Gets the border colour.
    virtual wxColour GetBorderColour() const;

    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual wxColour GetBackgroundColour() const;

    /// Sets the button background colour.
    virtual bool SetButtonBackgroundColour(const wxColour &colour);

    /// Gets the button background colour.
    virtual wxColour GetButtonBackgroundColour() const;

    virtual bool SetForegroundColour(const wxColour &colour);
    virtual wxColour GetForegroundColour() const;

    virtual bool SetFont(const wxFont& font);
    virtual wxFont GetFont() const;

protected:
    wxMoButtonStrip m_buttonStrip;
    wxColour        m_borderColour;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxMoTabCtrl)
};

#endif
    // _WX_MOBILE_GENERIC_TABCTRL_H_
