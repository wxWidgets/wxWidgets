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

MAKE_CONST_WXSTRING2(SplitterNameStr, wxT("splitter"));

//---------------------------------------------------------------------------
%newgroup

enum {
    wxSP_NOBORDER,
    wxSP_NOSASH,
    wxSP_PERMIT_UNSPLIT,
    wxSP_LIVE_UPDATE,
    wxSP_3DSASH,
    wxSP_3DBORDER,
    wxSP_NO_XP_THEME,
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

DocStr(wxSplitterWindow,
"wx.SplitterWindow manages up to two subwindows or panes, with an
optional vertical or horizontal split which can be used with the mouse
or programmatically.", "

Styles
-------
    ====================   ======================================
    wx.SP_3D               Draws a 3D effect border and sash.
    wx.SP_3DSASH           Draws a 3D effect sash.
    wx.SP_3DBORDER         Synonym for wxSP_BORDER.
    wx.SP_BORDER           Draws a standard border.
    wx.SP_NOBORDER         No border (default).
    wx.SP_NO_XP_THEME      Under Windows XP, switches off the
                           attempt to draw the splitter
                           using Windows XP theming, so the
                           borders and sash will take on the
                           pre-XP look.
    wx.SP_PERMIT_UNSPLIT   Always allow to unsplit, even with
                           the minimum pane size other than zero.
    wx.SP_LIVE_UPDATE      Don't draw XOR line but resize the
                           child windows immediately.
    ====================   ======================================

Events
------
    ==============================  =======================================
    EVT_SPLITTER_SASH_POS_CHANGING  The sash position is in the
                                    process of being changed. May be
                                    used to modify the position of
                                    the tracking bar to properly
                                    reflect the position that would
                                    be set if the drag were to be
                                    completed at this point.

    EVT_SPLITTER_SASH_POS_CHANGED
                                    The sash position was
                                    changed. May be used to modify
                                    the sash position before it is
                                    set, or to prevent the change
                                    from taking place. 

    EVT_SPLITTER_UNSPLIT            The splitter has been just unsplit.

    EVT_SPLITTER_DCLICK             The sash was double clicked. The
                                    default behaviour is to unsplit
                                    the window when this happens
                                    (unless the minimum pane size has
                                    been set to a value greater than
                                    zero.)
    ==============================  =======================================

");



MustHaveApp(wxSplitterWindow);

class wxSplitterWindow: public wxWindow
{
public:
    %pythonPrepend wxSplitterWindow         "if kwargs.has_key('point'): kwargs['pos'] = kwargs['point'];del kwargs['point']"
    %pythonPrepend wxSplitterWindow()       ""
    %pythonAppend  wxSplitterWindow         "self._setOORInfo(self)"
    %pythonAppend  wxSplitterWindow()       ""

    DocCtorStr(
        wxSplitterWindow(wxWindow* parent, wxWindowID id=-1,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style=wxSP_3D,
                         const wxString& name = wxPySplitterNameStr),
        "Constructor.  Creates and shows a SplitterWindow.", "");
    
    DocCtorStrName(
        wxSplitterWindow(),
        "Precreate a SplitterWindow for 2-phase creation.", "",
        PreSplitterWindow);


    DocDeclStr(
        bool , Create(wxWindow* parent, wxWindowID id=-1,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style=wxSP_3D,
                      const wxString& name = wxPySplitterNameStr),
        "Create the GUI part of the SplitterWindow for the 2-phase create.", "");
    


    DocDeclStr(
        wxWindow *, GetWindow1() const,
        "Gets the only or left/top pane.", "");
    

    DocDeclStr(
        wxWindow *, GetWindow2() const,
        "Gets the right/bottom pane.", "");


    DocDeclStr(
        void , SetSplitMode(int mode),
        "Sets the split mode.  The mode can be wx.SPLIT_VERTICAL or
wx.SPLIT_HORIZONTAL.  This only sets the internal variable; does not
update the display.", "");
    

    DocDeclStr(
        wxSplitMode , GetSplitMode() const,
        "Gets the split mode", "");
    

    DocDeclStr(
        void , Initialize(wxWindow *window),
        "Initializes the splitter window to have one pane.  This should be
called if you wish to initially view only a single pane in the
splitter window.  The child window is shown if it is currently hidden.", "");
    

    // Associates the given window with window 2, drawing the appropriate sash
    // and changing the split mode.
    // Does nothing and returns False if the window is already split.
    // A sashPosition of 0 means choose a default sash position,
    // negative sashPosition specifies the size of right/lower pane as it's
    // absolute value rather than the size of left/upper pane.
    
    DocDeclStr(
        virtual bool , SplitVertically(wxWindow *window1,
                                       wxWindow *window2,
                                       int sashPosition = 0),
        "Initializes the left and right panes of the splitter window.  The
child windows are shown if they are currently hidden.","

    :param window1:       The left pane.
    :param window2:       The right pane.
    :param sashPosition:  The initial position of the sash. If this
                  value is positive, it specifies the size of the left
                  pane. If it is negative, its absolute value gives
                  the size of the right pane. Finally, specify 0
                  (default) to choose the default position (half of
                  the total window width).

Returns True if successful, False otherwise (the window was already
split).

SplitVertically should be called if you wish to initially view two
panes. It can also be called at any subsequent time, but the
application should check that the window is not currently split using
`IsSplit`.
");
    

    DocDeclStr(
        virtual bool , SplitHorizontally(wxWindow *window1,
                                         wxWindow *window2,
                                         int sashPosition = 0),
        "Initializes the top and bottom panes of the splitter window.  The
child windows are shown if they are currently hidden.","

    :param window1:       The top pane.
    :param window2:       The bottom pane.
    :param sashPosition:  The initial position of the sash. If this
                  value is positive, it specifies the size of the
                  upper pane. If it is negative, its absolute value
                  gives the size of the lower pane. Finally, specify 0
                  (default) to choose the default position (half of
                  the total window height).

Returns True if successful, False otherwise (the window was already
split).

SplitHorizontally should be called if you wish to initially view two
panes. It can also be called at any subsequent time, but the
application should check that the window is not currently split using
`IsSplit`.
");

    

    DocDeclStr(
        bool , Unsplit(wxWindow *toRemove = NULL),
        "Unsplits the window.  Pass the pane to remove, or None to remove the
right or bottom pane.  Returns True if successful, False otherwise (the
window was not split).

This function will not actually delete the pane being
removed; it sends EVT_SPLITTER_UNSPLIT which can be handled
for the desired behaviour. By default, the pane being
removed is only hidden.", "");
    


    DocDeclStr(
        bool , ReplaceWindow(wxWindow *winOld, wxWindow *winNew),
        "This function replaces one of the windows managed by the
SplitterWindow with another one. It is in general better to use it
instead of calling Unsplit() and then resplitting the window back
because it will provoke much less flicker. It is valid to call this
function whether the splitter has two windows or only one.

Both parameters should be non-None and winOld must specify one of the
windows managed by the splitter. If the parameters are incorrect or
the window couldn't be replaced, False is returned. Otherwise the
function will return True, but please notice that it will not Destroy
the replaced window and you may wish to do it yourself.", "");
    

    DocDeclStr(
        void , UpdateSize(),
        "Causes any pending sizing of the sash and child panes to take place
immediately.

Such resizing normally takes place in idle time, in order to wait for
layout to be completed. However, this can cause unacceptable flicker
as the panes are resized after the window has been shown. To work
around this, you can perform window layout (for example by sending a
size event to the parent window), and then call this function, before
showing the top-level window.", "");


   
    DocDeclStr(
        bool , IsSplit() const,
        "Is the window split?", "");
    

    DocDeclStr(
        void , SetSashSize(int width),
        "Sets the sash size.", "");
    

    DocDeclStr(
        void , SetBorderSize(int width),
        "Sets the border size. Currently a NOP.", "");
    

    DocDeclStr(
        int , GetSashSize() const,
        "Gets the sash size", "");
    

    DocDeclStr(
        int , GetBorderSize() const,
        "Gets the border size", "");
    

    DocDeclStr(
        void , SetSashPosition(int position, bool redraw = true),
        "Sets the sash position, in pixels.  If redraw is Ttrue then the panes
are resized and the sash and border are redrawn.", "");
    

    DocDeclStr(
        int , GetSashPosition() const,
        "Returns the surrent sash position.", "");
    

    DocDeclStr(
        void , SetSashGravity(double gravity),
        "Set the sash gravity.  Gravity is a floating-point factor between 0.0
and 1.0 which controls position of sash while resizing the
`wx.SplitterWindow`.  The gravity specifies how much the left/top
window will grow while resizing.","

Example values:

    ======= =======================================
    0.0      Only the bottom or right window is
             automaticaly resized.
    0.5      Both windows grow equally.
    1.0      Only left/top window grows.
    ======= =======================================

The default value of sash gravity is 0.0. That value is compatible
with the previous (before gravity was introduced) behaviour of the 
`wx.SplitterWindow`.");

    DocDeclStr(
        double , GetSashGravity() const,
        "Gets the sash gravity.

:see: `SetSashGravity`
", "");
    

    DocDeclStr(
        void , SetMinimumPaneSize(int min),
        "Sets the minimum pane size in pixels.

The default minimum pane size is zero, which means that either pane
can be reduced to zero by dragging the sash, thus removing one of the
panes. To prevent this behaviour (and veto out-of-range sash
dragging), set a minimum size, for example 20 pixels. If the
wx.SP_PERMIT_UNSPLIT style is used when a splitter window is created,
the window may be unsplit even if minimum size is non-zero.", "");
    
    DocDeclStr(
        int , GetMinimumPaneSize() const,
        "Gets the minimum pane size in pixels.", "");
    

    DocDeclStr(
        virtual bool , SashHitTest(int x, int y, int tolerance = 5),
        "Tests for x, y over the sash", "");
    

    DocDeclStr(
        virtual void , SizeWindows(),
        "Resizes subwindows", "");
    

    void SetNeedUpdating(bool needUpdating);
    bool GetNeedUpdating() const;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
    
    %property(BorderSize, GetBorderSize, SetBorderSize, doc="See `GetBorderSize` and `SetBorderSize`");
    %property(MinimumPaneSize, GetMinimumPaneSize, SetMinimumPaneSize, doc="See `GetMinimumPaneSize` and `SetMinimumPaneSize`");
    %property(NeedUpdating, GetNeedUpdating, SetNeedUpdating, doc="See `GetNeedUpdating` and `SetNeedUpdating`");
    %property(SashGravity, GetSashGravity, SetSashGravity, doc="See `GetSashGravity` and `SetSashGravity`");
    %property(SashPosition, GetSashPosition, SetSashPosition, doc="See `GetSashPosition` and `SetSashPosition`");
    %property(SashSize, GetSashSize, SetSashSize, doc="See `GetSashSize` and `SetSashSize`");
    %property(SplitMode, GetSplitMode, SetSplitMode, doc="See `GetSplitMode` and `SetSplitMode`");
    %property(Window1, GetWindow1, doc="See `GetWindow1`");
    %property(Window2, GetWindow2, doc="See `GetWindow2`");
};





DocStr(wxSplitterEvent,
       "This class represents the events generated by a splitter control.", "");

class wxSplitterEvent : public wxNotifyEvent
{
public:
    wxSplitterEvent(wxEventType type = wxEVT_NULL,
                    wxSplitterWindow *splitter = (wxSplitterWindow *)NULL);


    DocDeclStr(
        void , SetSashPosition(int pos),
        "This function is only meaningful during EVT_SPLITTER_SASH_POS_CHANGING
and EVT_SPLITTER_SASH_POS_CHANGED events.  In the case of _CHANGED
events, sets the the new sash position. In the case of _CHANGING
events, sets the new tracking bar position so visual feedback during
dragging will represent that change that will actually take place. Set
to -1 from the event handler code to prevent repositioning.", "");
    
    DocDeclStr(
        int , GetSashPosition() const,
        "Returns the new sash position while in EVT_SPLITTER_SASH_POS_CHANGING
and EVT_SPLITTER_SASH_POS_CHANGED events.", "");
    

    DocDeclStr(
        wxWindow *, GetWindowBeingRemoved() const,
        "Returns a pointer to the window being removed when a splitter window
is unsplit.", "");
    

    DocDeclStr(
        int , GetX() const,
        "Returns the x coordinate of the double-click point in a
EVT_SPLITTER_DCLICK event.", "");
    
    DocDeclStr(
        int , GetY() const,
        "Returns the y coordinate of the double-click point in a
EVT_SPLITTER_DCLICK event.", "");
    
    %property(SashPosition, GetSashPosition, SetSashPosition, doc="See `GetSashPosition` and `SetSashPosition`");
    %property(WindowBeingRemoved, GetWindowBeingRemoved, doc="See `GetWindowBeingRemoved`");
    %property(X, GetX, doc="See `GetX`");
    %property(Y, GetY, doc="See `GetY`");
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
EVT_SPLITTER_DCLICK = EVT_SPLITTER_DOUBLECLICKED
}

//---------------------------------------------------------------------------

