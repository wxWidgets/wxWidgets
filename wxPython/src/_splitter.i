/////////////////////////////////////////////////////////////////////////////
// Name:        _splitter.i
// Purpose:     SWIG interface defs for wxSplitterWindow
//
// Author:      Robin Dunn
//
// Created:     2-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
    static const wxChar* wxSplitterNameStr = wxT("splitter");
    DECLARE_DEF_STRING(SplitterNameStr);
%}


//---------------------------------------------------------------------------
%newgroup

enum {
    wxSP_NOBORDER,
    wxSP_NOSASH,
    wxSP_PERMIT_UNSPLIT,
    wxSP_LIVE_UPDATE,
    wxSP_3DSASH,
    wxSP_3DBORDER,
    wxSP_BORDER,
    wxSP_3D,
};


enum wxSplitMode
{
    wxSPLIT_HORIZONTAL = 1,
    wxSPLIT_VERTICAL
};

enum
{
    wxSPLIT_DRAG_NONE,
    wxSPLIT_DRAG_DRAGGING,
    wxSPLIT_DRAG_LEFT_DOWN
};

//---------------------------------------------------------------------------


// wxSplitterWindow maintains one or two panes, with an optional vertical or
// horizontal split which can be used with the mouse or programmatically.
class wxSplitterWindow: public wxWindow
{
public:
    %addtofunc wxSplitterWindow         "self._setOORInfo(self)"
    %addtofunc wxSplitterWindow()       ""

    wxSplitterWindow(wxWindow* parent, wxWindowID id,
                     const wxPoint& point = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style=wxSP_3D,
                     const wxString& name = wxPySplitterNameStr);
    %name(PreSplitterWindow)wxSplitterWindow();

    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& point = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style=wxSP_3D,
                const wxString& name = wxPySplitterNameStr);

    // Gets the only or left/top pane
    wxWindow *GetWindow1() const;

    // Gets the right/bottom pane
    wxWindow *GetWindow2() const;

    // Sets the split mode
    void SetSplitMode(int mode);

    // Gets the split mode
    wxSplitMode GetSplitMode() const;

    // Initialize with one window
    void Initialize(wxWindow *window);

    // Associates the given window with window 2, drawing the appropriate sash
    // and changing the split mode.
    // Does nothing and returns FALSE if the window is already split.
    // A sashPosition of 0 means choose a default sash position,
    // negative sashPosition specifies the size of right/lower pane as it's
    // absolute value rather than the size of left/upper pane.
    virtual bool SplitVertically(wxWindow *window1,
                                 wxWindow *window2,
                                 int sashPosition = 0);

    virtual bool SplitHorizontally(wxWindow *window1,
                                   wxWindow *window2,
                                   int sashPosition = 0);

    // Removes the specified (or second) window from the view
    // Doesn't actually delete the window.
    bool Unsplit(wxWindow *toRemove = NULL);

    // Replaces one of the windows with another one (neither old nor new
    // parameter should be NULL)
    bool ReplaceWindow(wxWindow *winOld, wxWindow *winNew);

    // Is the window split?
    bool IsSplit() const;

    // Sets the sash size
    void SetSashSize(int width);

    // Sets the border size
    void SetBorderSize(int width);

    // Gets the sash size
    int GetSashSize() const;

    // Gets the border size
    int GetBorderSize() const;

    // Set the sash position
    void SetSashPosition(int position, bool redraw = TRUE);

    // Gets the sash position
    int GetSashPosition() const;

    // If this is zero, we can remove panes by dragging the sash.
    void SetMinimumPaneSize(int min);
    int GetMinimumPaneSize() const;


    // Tests for x, y over sash
    virtual bool SashHitTest(int x, int y, int tolerance = 5);

    // Resizes subwindows
    virtual void SizeWindows();

    void SetNeedUpdating(bool needUpdating);
    bool GetNeedUpdating() const;
};




// we reuse the same class for all splitter event types because this is the
// usual wxWin convention, but the three event types have different kind of
// data associated with them, so the accessors can be only used if the real
// event type matches with the one for which the accessors make sense
class wxSplitterEvent : public wxNotifyEvent
{
public:
    wxSplitterEvent(wxEventType type = wxEVT_NULL,
                    wxSplitterWindow *splitter = (wxSplitterWindow *)NULL);


    // SASH_POS_CHANGED methods

    // setting the sash position to -1 prevents the change from taking place at
    // all
    void SetSashPosition(int pos);
    int GetSashPosition() const;

    // UNSPLIT event methods
    wxWindow *GetWindowBeingRemoved() const;

    // DCLICK event methods
    int GetX() const;
    int GetY() const;
};



%constant wxEventType wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED;
%constant wxEventType wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING;
%constant wxEventType wxEVT_COMMAND_SPLITTER_DOUBLECLICKED;
%constant wxEventType wxEVT_COMMAND_SPLITTER_UNSPLIT;


%pythoncode {
EVT_SPLITTER_SASH_POS_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, 1 )
EVT_SPLITTER_SASH_POS_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING, 1 )
EVT_SPLITTER_DOUBLECLICKED = wx.PyEventBinder( wxEVT_COMMAND_SPLITTER_DOUBLECLICKED, 1 )
EVT_SPLITTER_UNSPLIT = wx.PyEventBinder( wxEVT_COMMAND_SPLITTER_UNSPLIT, 1 )
}

//---------------------------------------------------------------------------

