/////////////////////////////////////////////////////////////////////////////
// Name:        _hyperlink.i
// Purpose:     SWIG interface defs for wxHyperlinkCtrl
//
// Author:      Robin Dunn
//
// Created:     28-May-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(HyperlinkCtrlNameStr);

enum {
    wxHL_CONTEXTMENU,
    wxHL_DEFAULT_STYLE
};

//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxHyperlinkCtrl);


DocStr( wxHyperlinkCtrl,
"A static text control that emulates a hyperlink. The link is displayed
in an appropriate text style, derived from the control's normal font.
When the mouse rolls over the link, the cursor changes to a hand and
the link's color changes to the active color.

Clicking on the link does not launch a web browser; instead, a
wx.HyperlinkEvent is fired. Use the wx.EVT_HYPERLINK to catch link
events.
", "");

class wxHyperlinkCtrl : public wxControl
{
public:
    %pythonAppend wxHyperlinkCtrl      "self._setOORInfo(self)"
    %pythonAppend wxHyperlinkCtrl()    ""

    
    // Constructor.
    wxHyperlinkCtrl(wxWindow *parent,
                    wxWindowID id,
                    const wxString& label, const wxString& url,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxHL_DEFAULT_STYLE,
                    const wxString& name = wxPyHyperlinkCtrlNameStr);
    %RenameCtor(PreHyperlinkCtrl, wxHyperlinkCtrl());

    // Creation function (for two-step construction).
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label, const wxString& url,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHL_DEFAULT_STYLE,
                const wxString& name = wxPyHyperlinkCtrlNameStr);

    // get/set
    wxColour GetHoverColour() const;
    void SetHoverColour(const wxColour &colour);

    wxColour GetNormalColour() const;
    void SetNormalColour(const wxColour &colour);

    wxColour GetVisitedColour() const;
    void SetVisitedColour(const wxColour &colour);

    wxString GetURL() const;
    void SetURL (const wxString &url);

    void SetVisited(bool visited = true);
    bool GetVisited() const;

    %property(HoverColour, GetHoverColour, SetHoverColour, doc="See `GetHoverColour` and `SetHoverColour`");
    %property(NormalColour, GetNormalColour, SetNormalColour, doc="See `GetNormalColour` and `SetNormalColour`");
    %property(URL, GetURL, SetURL, doc="See `GetURL` and `SetURL`");
    %property(Visited, GetVisited, SetVisited, doc="See `GetVisited` and `SetVisited`");
    %property(VisitedColour, GetVisitedColour, SetVisitedColour, doc="See `GetVisitedColour` and `SetVisitedColour`");
};


%constant wxEventType wxEVT_COMMAND_HYPERLINK;


//
// An event fired when the user clicks on the label in a hyperlink control.
// See HyperlinkControl for details.
//
class wxHyperlinkEvent : public wxCommandEvent
{
public:

    wxHyperlinkEvent(wxObject *generator, wxWindowID id, const wxString& url);

    // Returns the URL associated with the hyperlink control
    // that the user clicked on.
    wxString GetURL() const;
    void SetURL(const wxString &url);

    %property(URL, GetURL, SetURL, doc="See `GetURL` and `SetURL`");
};


%pythoncode {
    EVT_HYPERLINK = wx.PyEventBinder( wxEVT_COMMAND_HYPERLINK, 1 )
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
