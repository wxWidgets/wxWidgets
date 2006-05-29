/////////////////////////////////////////////////////////////////////////////
// Name:        wx/hyperlink.h
// Purpose:     Hyperlink control
// Author:      David Norris <danorris@gmail.com>, Otto Wyss
// Modified by: Ryan Norton, Francesco Montorsi
// Created:     04/02/2005
// RCS-ID:      $Id$
// Copyright:   (c) 2005 David Norris
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HYPERLINK_H__
#define _WX_HYPERLINK_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "hyperlink.h"
#endif

#include "wx/defs.h"

#if wxUSE_HYPERLINKCTRL

#include "wx/control.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define wxHL_CONTEXTMENU        0x0001
#define wxHL_DEFAULT_STYLE      wxHL_CONTEXTMENU|wxNO_BORDER

extern WXDLLEXPORT_DATA(const wxChar) wxHyperlinkCtrlNameStr[];


// ----------------------------------------------------------------------------
// wxHyperlinkCtrl
// ----------------------------------------------------------------------------

// A static text control that emulates a hyperlink. The link is displayed
// in an appropriate text style, derived from the control's normal font.
// When the mouse rolls over the link, the cursor changes to a hand and the
// link's color changes to the active color.
//
// Clicking on the link does not launch a web browser; instead, a
// HyperlinkEvent is fired. The event propagates upward until it is caught,
// just like a wxCommandEvent.
//
// Use the EVT_HYPERLINK() to catch link events.
class WXDLLIMPEXP_CORE wxHyperlinkCtrl : public wxControl
{
public:
    // Default constructor (for two-step construction).
    wxHyperlinkCtrl() { }

    // Constructor.
    wxHyperlinkCtrl(wxWindow *parent,
                    wxWindowID id,
                    const wxString& label, const wxString& url,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxHL_DEFAULT_STYLE,
                    const wxString& name = wxHyperlinkCtrlNameStr)
    {
        (void)Create(parent, id, label, url, pos, size, style, name);
    }

    // Creation function (for two-step construction).
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label, const wxString& url,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHL_DEFAULT_STYLE,
                const wxString& name = wxHyperlinkCtrlNameStr);


    // get/set
    wxColour GetHoverColour() const { return m_hoverColour; }
    void SetHoverColour(const wxColour &colour) { m_hoverColour = colour; }

    wxColour GetNormalColour() const { return m_normalColour; }
    void SetNormalColour(const wxColour &colour);

    wxColour GetVisitedColour() const { return m_visitedColour; }
    void SetVisitedColour(const wxColour &colour);

    wxString GetURL() const { return m_url; }
    void SetURL (const wxString &url) { m_url=url; }

    void SetVisited(bool visited = true) { m_visited=visited; }
    bool GetVisited() const { return m_visited; }

    // NOTE: also wxWindow::Set/GetLabel, wxWindow::Set/GetBackgroundColour,
    //       wxWindow::Get/SetFont, wxWindow::Get/SetCursor are important !


protected:
    // event handlers

    // Renders the hyperlink.
    void OnPaint(wxPaintEvent& event);

    // If the click originates inside the bounding box of the label,
    // a flag is set so that an event will be fired when the left
    // button is released.
    void OnLeftDown(wxMouseEvent& event);

    // If the click both originated and finished inside the bounding box
    // of the label, a HyperlinkEvent is fired.
    void OnLeftUp(wxMouseEvent& event);
    void OnRightUp(wxMouseEvent& event);

    // Changes the cursor to a hand, if the mouse is inside the label's
    // bounding box.
    void OnEnterWindow(wxMouseEvent& event);

    // Changes the cursor back to the default, if necessary.
    void OnLeaveWindow(wxMouseEvent& event);

    // handles "Copy URL" menuitem
    void OnPopUpCopy(wxCommandEvent& event);


    // overridden base class virtuals

    // Returns the best size for the window, which is the size needed
    // to display the text label.
    virtual void DoGetSize(int *width, int *height) const;
    virtual wxSize DoGetBestSize() const;

    // creates a context menu with "Copy URL" menuitem
    virtual void DoContextMenu(const wxPoint &);

private:
    // URL associated with the link. This is transmitted inside
    // the HyperlinkEvent fired when the user clicks on the label.
    wxString m_url;

    // Foreground colours for various link types.
    // NOTE: wxWindow::m_backgroundColour is used for background,
    //       wxWindow::m_foregroundColour is used to render non-visited links
    wxColour m_hoverColour;
    wxColour m_normalColour;
    wxColour m_visitedColour;

    // True if the mouse cursor is inside the label's bounding box.
    bool m_rollover;

    // True if the link has been clicked before.
    bool m_visited;

    // True if a click is in progress (left button down) and the click
    // originated inside the label's bounding box.
    bool m_clicking;

private:
    DECLARE_DYNAMIC_CLASS(wxHyperlinkCtrl)
    DECLARE_EVENT_TABLE()
};


// ----------------------------------------------------------------------------
// wxHyperlinkEvent
// ----------------------------------------------------------------------------

// Declare an event identifier.
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_CORE, wxEVT_COMMAND_HYPERLINK, 3700)
END_DECLARE_EVENT_TYPES()

//
// An event fired when the user clicks on the label in a hyperlink control.
// See HyperlinkControl for details.
//
class WXDLLIMPEXP_CORE wxHyperlinkEvent : public wxCommandEvent
{
public:

    wxHyperlinkEvent() {}
    wxHyperlinkEvent(wxObject *generator, wxWindowID id, const wxString& url)
     : wxCommandEvent(wxEVT_COMMAND_HYPERLINK, id), m_url(url)
        { SetEventObject(generator); }

    // Returns the URL associated with the hyperlink control
    // that the user clicked on.
    wxString GetURL() const { return m_url; }
    void SetURL(const wxString &url) { m_url=url; }

private:

    // URL associated with the hyperlink control that the used clicked on.
    wxString m_url;
};

// Define a typedef for event handler functions.
typedef void (wxEvtHandler::*wxHyperlinkEventFunction)(wxHyperlinkEvent&);

// Define an event table macro.
#define EVT_HYPERLINK(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_HYPERLINK, \
    id, id, (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent(wxHyperlinkEventFunction, &fn), (wxObject *) NULL),

// Newer event macro
#define wxHyperlinkEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxHyperlinkEventFunction, &func)

#endif // wxUSE_HYPERLINKCTRL

#endif // _WX_HYPERLINK_H__
