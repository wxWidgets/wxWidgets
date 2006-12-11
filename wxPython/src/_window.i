/////////////////////////////////////////////////////////////////////////////
// Name:        _window.i
// Purpose:     SWIG interface for wxWindow
//
// Author:      Robin Dunn
//
// Created:     24-June-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
%}

MAKE_CONST_WXSTRING(PanelNameStr);

//---------------------------------------------------------------------------
%newgroup


DocStr(wxVisualAttributes,
    "struct containing all the visual attributes of a control", "");

struct wxVisualAttributes
{
    %extend {
        wxVisualAttributes() { return new wxVisualAttributes; }
        ~wxVisualAttributes() { delete self; }
    } 
    
    // the font used for control label/text inside it
    wxFont font;

    // the foreground colour
    wxColour colFg;

    // the background colour, may be wxNullColour if the controls background
    // colour is not solid
    wxColour colBg;
};




enum wxWindowVariant 
{
    wxWINDOW_VARIANT_NORMAL,        // Normal size
    wxWINDOW_VARIANT_SMALL,         // Smaller size (about 25 % smaller than normal )
    wxWINDOW_VARIANT_MINI,          // Mini size (about 33 % smaller than normal )
    wxWINDOW_VARIANT_LARGE,         // Large size (about 25 % larger than normal )
    wxWINDOW_VARIANT_MAX
};


DocStr(wxWindow,
"wx.Window is the base class for all windows and represents any visible
object on the screen. All controls, top level windows and so on are
wx.Windows. Sizers and device contexts are not however, as they don't
appear on screen themselves.
",
"      
Styles
-------
    =============================  =====================================
    wx.SIMPLE_BORDER               Displays a thin border around the window.
                                   
    wx.DOUBLE_BORDER               Displays a double border. Windows and Mac only.
                                   
    wx.SUNKEN_BORDER               Displays a sunken border.
                                   
    wx.RAISED_BORDER               Displays a raised border.
                                   
    wx.STATIC_BORDER               Displays a border suitable for a static
                                   control. Windows only.
                                   
    wx.NO_BORDER                   Displays no border, overriding the default
                                   border style for the window.
                                   
    wx.TRANSPARENT_WINDOW          The window is transparent, that is, it
                                   will not receive paint events. Windows only.
                                   
    wx.TAB_TRAVERSAL               Use this to enable tab traversal for
                                   non-dialog windows.
                                   
    wx.WANTS_CHARS                 Use this to indicate that the window
                                   wants to get all char/key events for
                                   all keys - even for keys like TAB or
                                   ENTER which are usually used for
                                   dialog navigation and which wouldn't
                                   be generated without this style. If
                                   you need to use this style in order to
                                   get the arrows or etc., but would
                                   still like to have normal keyboard
                                   navigation take place, you should
                                   create and send a wxNavigationKeyEvent
                                   in response to the key events for Tab
                                   and Shift-Tab.
                                   
    wx.NO_FULL_REPAINT_ON_RESIZE   Disables repainting the window
                                   completely when its size is changed.
                                   You will have to repaint the new
                                   window area manually if you use this
                                   style. As of version 2.5.1 this
                                   style is on by default.  Use
                                   wx.FULL_REPAINT_ON_RESIZE to
                                   deactivate it.
                                   
    wx.VSCROLL                     Use this style to enable a vertical scrollbar.
                                   
    wx.HSCROLL                     Use this style to enable a horizontal scrollbar.
                                   
    wx.ALWAYS_SHOW_SB              If a window has scrollbars, disable them
                                   instead of hiding them when they are
                                   not needed (i.e. when the size of the
                                   window is big enough to not require
                                   the scrollbars to navigate it). This
                                   style is currently only implemented
                                   for wxMSW and wxUniversal and does
                                   nothing on the other platforms.
                                   
    wx.CLIP_CHILDREN               Use this style to eliminate flicker caused by
                                   the background being repainted, then
                                   children being painted over
                                   them. Windows only.
                                   
    wx.FULL_REPAINT_ON_RESIZE      Use this style to force a complete
                                   redraw of the window whenever it is
                                   resized instead of redrawing just the
                                   part of the window affected by
                                   resizing. Note that this was the
                                   behaviour by default before 2.5.1
                                   release and that if you experience
                                   redraw problems with the code which
                                   previously used to work you may want
                                   to try this.
    =============================  =====================================


Extra Styles
------------
    =============================   =====================================
    wx.WS_EX_VALIDATE_RECURSIVELY   By default,
                                    Validate/TransferDataTo/FromWindow()
                                    only work on direct children of
                                    the window (compatible
                                    behaviour). Set this flag to make
                                    them recursively descend into all
                                    subwindows.

    wx.WS_EX_BLOCK_EVENTS           wx.CommandEvents and the objects of the
                                    derived classes are forwarded to
                                    the parent window and so on
                                    recursively by default. Using this
                                    flag for the given window allows
                                    to block this propagation at this
                                    window, i.e. prevent the events
                                    from being propagated further
                                    upwards. Dialogs have this flag on
                                    by default.

    wx.WS_EX_TRANSIENT              Don't use this window as an implicit parent for
                                    the other windows: this must be
                                    used with transient windows as
                                    otherwise there is the risk of
                                    creating a dialog/frame with this
                                    window as a parent which would
                                    lead to a crash if the parent is
                                    destroyed before the child.

    wx.WS_EX_PROCESS_IDLE           This window should always process idle
                                    events, even if the mode set by
                                    `wx.IdleEvent.SetMode` is
                                    wx.IDLE_PROCESS_SPECIFIED.

    wx.WS_EX_PROCESS_UI_UPDATES     This window should always process UI
                                    update events, even if the mode
                                    set by `wx.UpdateUIEvent.SetMode` is
                                    wxUPDATE_UI_PROCESS_SPECIFIED.
    =============================   =====================================

");


MustHaveApp(wxWindow);
MustHaveApp(wxWindow::FindFocus);
MustHaveApp(wxWindow::GetCapture);

// This one is not restricted to wxWindow
MustHaveApp(GetClassDefaultAttributes);

class wxWindow : public wxEvtHandler
{
public:
    %pythonAppend wxWindow         "self._setOORInfo(self)"
    %pythonAppend wxWindow()       ""
    %typemap(out) wxWindow*;    // turn off this typemap

    DocCtorStr(
        wxWindow(wxWindow* parent, const wxWindowID id=-1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxPyPanelNameStr),
        "Construct and show a generic Window.", "");

    DocCtorStrName(
        wxWindow(),
        "Precreate a Window for 2-phase creation.", "",
        PreWindow);
    
    // Turn it back on again
    %typemap(out) wxWindow* { $result = wxPyMake_wxObject($1, $owner); }

    
    DocDeclStr(
        bool , Create(wxWindow* parent, const wxWindowID id=-1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                      const wxString& name = wxPyPanelNameStr),
        "Create the GUI part of the Window for 2-phase creation mode.", "");
    

    // deleting the window
    // -------------------


    DocDeclStr(
        bool , Close( bool force = false ),
        "This function simply generates a EVT_CLOSE event whose handler usually
tries to close the window. It doesn't close the window itself,
however.  If force is False (the default) then the window's close
handler will be allowed to veto the destruction of the window.",
"
Usually Close is only used with the top level windows (wx.Frame and
wx.Dialog classes) as the others are not supposed to have any special
EVT_CLOSE logic.

The close handler should check whether the window is being deleted
forcibly, using wx.CloseEvent.GetForce, in which case it should
destroy the window using wx.Window.Destroy.

Note that calling Close does not guarantee that the window will be
destroyed; but it provides a way to simulate a manual close of a
window, which may or may not be implemented by destroying the
window. The default EVT_CLOSE handler for wx.Dialog does not
necessarily delete the dialog, since it will simply simulate an
wxID_CANCEL event which is handled by the appropriate button event
handler and may do anything at all.

To guarantee that the window will be destroyed, call wx.Window.Destroy
instead.");
    


    %pythonPrepend Destroy "args[0].this.own(False)"
    DocDeclStr(
        virtual bool , Destroy(),
        "Destroys the window safely.  Frames and dialogs are not destroyed
immediately when this function is called -- they are added to a list
of windows to be deleted on idle time, when all the window's events
have been processed. This prevents problems with events being sent to
non-existent windows.

Returns True if the window has either been successfully deleted, or it
has been added to the list of windows pending real deletion.", "");
    

    DocDeclStr(
        bool , DestroyChildren(),
        "Destroys all children of a window. Called automatically by the
destructor.", "");
    

    DocDeclStr(
        bool , IsBeingDeleted() const,
        "Is the window in the process of being deleted?", "");
    

    
    // window attributes
    // -----------------

    //DocDeclStr(
    //    virtual void , SetTitle( const wxString& title),
    //    "Sets the window's title. Applicable only to frames and dialogs.", "");
    
    //DocDeclStr(
    //    virtual wxString , GetTitle() const,
    //    "Gets the window's title. Applicable only to frames and dialogs.", "");    

    DocDeclStr(
        virtual void , SetLabel(const wxString& label),
        "Set the text which the window shows in its label if applicable.", "");
    
    DocDeclStr(
        virtual wxString , GetLabel() const,
        "Generic way of getting a label from any window, for identification
purposes.  The interpretation of this function differs from class to
class. For frames and dialogs, the value returned is the title. For
buttons or static text controls, it is the button text. This function
can be useful for meta-programs such as testing tools or special-needs
access programs)which need to identify windows by name.", "");
    

    DocDeclStr(
        virtual void , SetName( const wxString &name ),
        "Sets the window's name.  The window name is used for ressource setting
in X, it is not the same as the window title/label", "");
    
    DocDeclStr(
        virtual wxString , GetName() const,
        "Returns the windows name.  This name is not guaranteed to be unique;
it is up to the programmer to supply an appropriate name in the window
constructor or via wx.Window.SetName.", "");
    
    
    DocDeclStr(
        void , SetWindowVariant( wxWindowVariant variant ),
        "Sets the variant of the window/font size to use for this window, if
the platform supports variants, for example, wxMac.",
        "
Variant values are:

    ========================    =======================================
    wx.WINDOW_VARIANT_NORMAL    Normal size
    wx.WINDOW_VARIANT_SMALL     Smaller size (about 25 % smaller than normal)
    wx.WINDOW_VARIANT_MINI      Mini size (about 33 % smaller than normal)
    wx.WINDOW_VARIANT_LARGE     Large size (about 25 % larger than normal)
    ========================    =======================================
");
    
    DocDeclStr(
        wxWindowVariant , GetWindowVariant() const,
        "", "");
    

    DocDeclStr(
        void , SetId( wxWindowID winid ),
        "Sets the identifier of the window.  Each window has an integer
identifier. If the application has not provided one, an identifier
will be generated. Normally, the identifier should be provided on
creation and should not be modified subsequently.", "");
    
    DocDeclStr(
        wxWindowID , GetId() const,
        "Returns the identifier of the window.  Each window has an integer
identifier. If the application has not provided one (or the default Id
-1 is used) then an unique identifier with a negative value will be
generated.", "");
    

    DocDeclStr(
        static int , NewControlId(),
        "Generate a control id for the controls which were not given one.", "");
    

    DocDeclStr(
        static int , NextControlId(int winid),
        "Get the id of the control following the one with the given
autogenerated) id", "");
    

    DocDeclStr(
        static int , PrevControlId(int winid),
        "Get the id of the control preceding the one with the given
autogenerated) id", "");
    

    DocDeclStr(
        virtual wxLayoutDirection , GetLayoutDirection() const,
        "Get the layout direction (LTR or RTL) for this window.  Returns
``wx.Layout_Default`` if layout direction is not supported.", "");
    
    DocDeclStr(
        virtual void , SetLayoutDirection(wxLayoutDirection dir),
        "Set the layout direction (LTR or RTL) for this window.", "");
    

    DocDeclStr(
        virtual wxCoord , AdjustForLayoutDirection(wxCoord x,
                                             wxCoord width,
                                                   wxCoord widthTotal) const,
        "Mirror coordinates for RTL layout if this window uses it and if the
mirroring is not done automatically like Win32.", "");
    

    


    // moving/resizing
    // ---------------


    DocDeclStr(
        void , SetSize( const wxSize& size ),
        "Sets the size of the window in pixels.", "");


    DocDeclStrName(
        void , SetSize( int x, int y, int width, int height,
                        int sizeFlags = wxSIZE_AUTO ),
        "Sets the position and size of the window in pixels.  The sizeFlags
parameter indicates the interpretation of the other params if they are
equal to -1.

    ========================  ======================================
    wx.SIZE_AUTO              A -1 indicates that a class-specific
                              default should be used.
    wx.SIZE_USE_EXISTING      Axisting dimensions should be used if
                              -1 values are supplied.
    wxSIZE_ALLOW_MINUS_ONE    Allow dimensions of -1 and less to be
                              interpreted as real dimensions, not
                              default values.
    ========================  ======================================
", "",
        SetDimensions);
    

    DocDeclStrName(
        void , SetSize(const wxRect& rect, int sizeFlags = wxSIZE_AUTO),
        "Sets the position and size of the window in pixels using a wx.Rect.", "",
        SetRect);
    

    DocDeclStrName(
        void , SetSize( int width, int height ),
        "Sets the size of the window in pixels.", "",
        SetSizeWH);
    

    DocDeclStr(
        void , Move(const wxPoint& pt, int flags = wxSIZE_USE_EXISTING),
        "Moves the window to the given position.", "");
    
    %pythoncode { SetPosition = Move }


    DocDeclStrName(
        void , Move(int x, int y, int flags = wxSIZE_USE_EXISTING),
        "Moves the window to the given position.", "",
        MoveXY);
    
    DocDeclStr(
        void , SetInitialSize(const wxSize& size=wxDefaultSize),
        "A 'Smart' SetSize that will fill in default size components with the
window's *best size* values.  Also set's the minsize for use with sizers.", "");
    %pythoncode { SetBestFittingSize = wx._deprecated(SetInitialSize, 'Use `SetInitialSize`') }

    
    DocDeclStr(
        virtual void , Raise(),
        "Raises the window to the top of the window hierarchy.  In current
version of wxWidgets this works both for managed and child windows.", "");
    
    DocDeclStr(
        virtual void , Lower(),
        "Lowers the window to the bottom of the window hierarchy.  In current
version of wxWidgets this works both for managed and child windows.", "");
    

    
    // client size is the size of the area available for subwindows
    DocStr(SetClientSize,
           "This sets the size of the window client area in pixels. Using this
function to size a window tends to be more device-independent than
wx.Window.SetSize, since the application need not worry about what
dimensions the border or title bar have when trying to fit the window
around panel items, for example.", "");
    void SetClientSize( const wxSize& size );
    %Rename(SetClientSizeWH, void,  SetClientSize( int width, int height ));
    %Rename(SetClientRect,  void,  SetClientSize(const wxRect& rect));


    DocStr(GetPosition,   // sets the docstring for both
           "Get the window's position.  Notice that the position is in client
coordinates for child windows and screen coordinates for the top level
ones, use `GetScreenPosition` if you need screen coordinates for all
kinds of windows.", "");
    wxPoint GetPosition() const;

    DocDeclAName(
        void, GetPosition(int *OUTPUT, int *OUTPUT) const,
        "GetPositionTuple() -> (x,y)",
        GetPositionTuple);

    
    DocStr(GetScreenPosition,   // sets the docstring for both
           "Get the position of the window in screen coordinantes.", "");
    wxPoint GetScreenPosition() const;
    DocDeclAName(
        void, GetScreenPosition(int *OUTPUT, int *OUTPUT) const,
        "GetScreenPositionTuple() -> (x,y)",
        GetScreenPositionTuple);

    DocDeclStr(
        wxRect , GetScreenRect() const,
        "Returns the size and position of the window in screen coordinantes as
a `wx.Rect` object.", "
:see: `GetRect`, `GetScreenPosition`");
    
   
    DocStr(GetSize, "Get the window size.", "");
    wxSize GetSize() const;
    DocDeclAName(
        void, GetSize( int *OUTPUT, int *OUTPUT ) const,
        "GetSizeTuple() -> (width, height)",
        GetSizeTuple);
    
    

    DocDeclStr(
        wxRect , GetRect() const,
        "Returns the size and position of the window as a `wx.Rect` object.", "");
    

    DocStr(GetClientSize,
           "This gets the size of the window's 'client area' in pixels. The client
area is the area which may be drawn on by the programmer, excluding
title bar, border, scrollbars, etc.", "");
    wxSize GetClientSize() const;
    DocDeclAName(
        void, GetClientSize( int *OUTPUT, int *OUTPUT ) const,
        "GetClientSizeTuple() -> (width, height)",
        GetClientSizeTuple);

    

    DocDeclStr(
        virtual wxPoint , GetClientAreaOrigin() const,
        "Get the origin of the client area of the window relative to the
window's top left corner (the client area may be shifted because of
the borders, scrollbars, other decorations...)", "");
    

    DocDeclStr(
        wxRect , GetClientRect() const,
        "Get the client area position and size as a `wx.Rect` object.", "");
    

    
    DocStr(GetBestSize,
           "This function returns the best acceptable minimal size for the
window, if applicable. For example, for a static text control, it will
be the minimal size such that the control label is not truncated. For
windows containing subwindows (suzh aswx.Panel), the size returned by
this function will be the same as the size the window would have had
after calling Fit.", "");
    wxSize GetBestSize() const;
    DocDeclAName(
        void, GetBestSize(int *OUTPUT, int *OUTPUT) const,
        "GetBestSizeTuple() -> (width, height)",
        GetBestSizeTuple);
    

    DocDeclStr(
        void , InvalidateBestSize(),
        "Reset the cached best size value so it will be recalculated the next
time it is needed.", "");

    DocDeclStr(
        void , CacheBestSize(const wxSize& size) const,
        "Cache the best size so it doesn't need to be calculated again, (at least until
some properties of the window change.)", "");

    
    DocDeclStr(
        wxSize , GetEffectiveMinSize() const,
        "This function will merge the window's best size into the window's
minimum size, giving priority to the min size components, and returns
the results.
", "");
    %pythoncode { GetBestFittingSize = wx._deprecated(GetEffectiveMinSize, 'Use `GetEffectiveMinSize` instead.') }

    %pythoncode {
        def GetAdjustedBestSize(self):
            s = self.GetBestSize()
            return wx.Size(max(s.width,  self.GetMinWidth()),
                           max(s.height, self.GetMinHeight()))
        GetAdjustedBestSize = wx._deprecated(GetAdjustedBestSize, 'Use `GetEffectiveMinSize` instead.')
    }
    


    DocDeclStr(
        void , Center( int direction = wxBOTH ),
        "Centers the window.  The parameter specifies the direction for
cetering, and may be wx.HORIZONTAL, wx.VERTICAL or wx.BOTH. It may
also include wx.CENTER_ON_SCREEN flag if you want to center the window
on the entire screen and not on its parent window.  If it is a
top-level window and has no parent then it will always be centered
relative to the screen.", "");    
    %pythoncode { Centre = Center }

    
    DocDeclStr(
        void , CenterOnParent(int dir = wxBOTH),
        "Center with respect to the the parent window", "");
    %pythoncode { CentreOnParent = CenterOnParent }

    

    DocDeclStr(
        virtual void , Fit(),
        "Sizes the window so that it fits around its subwindows. This function
won't do anything if there are no subwindows and will only really work
correctly if sizers are used for the subwindows layout. Also, if the
window has exactly one subwindow it is better (faster and the result
is more precise as Fit adds some margin to account for fuzziness of
its calculations) to call window.SetClientSize(child.GetSize())
instead of calling Fit.", "");
    

    DocDeclStr(
        virtual void , FitInside(),
        "Similar to Fit, but sizes the interior (virtual) size of a
window. Mainly useful with scrolled windows to reset scrollbars after
sizing changes that do not trigger a size event, and/or scrolled
windows without an interior sizer. This function similarly won't do
anything if there are no subwindows.", "");
    

    
    DocStr(SetSizeHints,
        "Allows specification of minimum and maximum window sizes, and window
size increments. If a pair of values is not set (or set to -1), the
default values will be used.  If this function is called, the user
will not be able to size the window outside the given bounds (if it is
a top-level window.)  Sizers will also inspect the minimum window size
and will use that value if set when calculating layout.

The resizing increments are only significant under Motif or Xt.", "

:see: `GetMinSize`, `GetMaxSize`, `SetMinSize`, `SetMaxSize`
");
    virtual void SetSizeHints( int minW, int minH,
                               int maxW = -1, int maxH = -1,
                               int incW = -1, int incH = -1 );
    %Rename(SetSizeHintsSz, void,  SetSizeHints( const wxSize& minSize,
                                                 const wxSize& maxSize=wxDefaultSize,
                                                 const wxSize& incSize=wxDefaultSize));

    
    DocStr(SetVirtualSizeHints,
        "Allows specification of minimum and maximum virtual window sizes. If a
pair of values is not set (or set to -1), the default values will be
used.  If this function is called, the user will not be able to size
the virtual area of the window outside the given bounds.", "");
    virtual void SetVirtualSizeHints( int minW, int minH,
                                      int maxW = -1, int maxH = -1 );
    %Rename(SetVirtualSizeHintsSz, void,  SetVirtualSizeHints(
                const wxSize& minSize, const wxSize& maxSize=wxDefaultSize));


    
    DocDeclStr(
        virtual wxSize , GetMaxSize() const,
        "", "");
    
    DocDeclStr(
        virtual wxSize , GetMinSize() const,
        "", "");
    
    DocDeclStr(
        virtual void , SetMinSize(const wxSize& minSize),
        "A more convenient method than `SetSizeHints` for setting just the
min size.", "");
    
    DocDeclStr(
        virtual void , SetMaxSize(const wxSize& maxSize),
        "A more convenient method than `SetSizeHints` for setting just the
max size.", "");
    


    DocDeclStr(
        virtual int , GetMinWidth() const,
        "", "");
    
    DocDeclStr(
        virtual int , GetMinHeight() const,
        "", "");
    
    DocDeclStr(
        int , GetMaxWidth() const,
        "", "");
    
    DocDeclStr(
        int , GetMaxHeight() const,
        "", "");
    
    
    
    DocStr(SetVirtualSize,
           "Set the the virtual size of a window in pixels.  For most windows this
is just the client area of the window, but for some like scrolled
windows it is more or less independent of the screen window size.", "");
    void SetVirtualSize(const wxSize& size );
    %Rename(SetVirtualSizeWH, void,  SetVirtualSize( int w, int h ));

    
    DocStr(GetVirtualSize,
           "Get the the virtual size of the window in pixels.  For most windows
this is just the client area of the window, but for some like scrolled
windows it is more or less independent of the screen window size.", "");           
    wxSize GetVirtualSize() const;
    DocDeclAName(
        void, GetVirtualSize( int *OUTPUT, int *OUTPUT ) const,
        "GetVirtualSizeTuple() -> (width, height)",
        GetVirtualSizeTuple);


    DocDeclStr(
        virtual wxSize , GetWindowBorderSize() const,
        "Return the size of the left/right and top/bottom borders.", "");
    
   
// TODO: using directors?
//     // Override these methods for windows that have a virtual size
//     // independent of their client size.  eg. the virtual area of a
//     // wxScrolledWindow.  Default is to alias VirtualSize to ClientSize.
//     virtual void DoSetVirtualSize( int x, int y );
//     virtual wxSize DoGetVirtualSize() const; // { return m_virtualSize; }


    DocDeclStr(
        virtual wxSize , GetBestVirtualSize() const,
        "Return the largest of ClientSize and BestSize (as determined by a
sizer, interior children, or other means)", "");
    


//     void SetScrollHelper( wxScrollHelper *sh );
//     wxScrollHelper *GetScrollHelper();
    

    // window state
    // ------------

    DocDeclStr(
        virtual bool , Show( bool show = true ),
        "Shows or hides the window. You may need to call Raise for a top level
window if you want to bring it to top, although this is not needed if
Show is called immediately after the frame creation.  Returns True if
the window has been shown or hidden or False if nothing was done
because it already was in the requested state.", "");
    
    DocDeclStr(
        bool , Hide(),
        "Equivalent to calling Show(False).", "");
    

    DocDeclStr(
        virtual bool , Enable( bool enable = true ),
        "Enable or disable the window for user input. Note that when a parent
window is disabled, all of its children are disabled as well and they
are reenabled again when the parent is.  Returns true if the window
has been enabled or disabled, false if nothing was done, i.e. if the
window had already been in the specified state.", "");
    
    DocDeclStr(
        bool , Disable(),
        "Disables the window, same as Enable(false).", "");
    

    DocDeclStr(
        bool , IsShown() const,
        "Returns true if the window is shown, false if it has been hidden.", "");
    
    DocDeclStr(
        bool , IsEnabled() const,
        "Returns true if the window is enabled for input, false otherwise.", "");

    DocDeclStr(
        virtual bool , IsShownOnScreen() const,
        "Returns ``True`` if the window is physically visible on the screen,
i.e. it is shown and all its parents up to the toplevel window are
shown as well.", "");
    
    


    DocDeclStr(
        virtual void , SetWindowStyleFlag( long style ),
        "Sets the style of the window. Please note that some styles cannot be
changed after the window creation and that Refresh() might need to be
called after changing the others for the change to take place
immediately.", "");
    
    DocDeclStr(
        virtual long , GetWindowStyleFlag() const,
        "Gets the window style that was passed to the constructor or Create
method.", "");    

    %pythoncode { SetWindowStyle = SetWindowStyleFlag; GetWindowStyle = GetWindowStyleFlag }
    
  
    DocDeclStr(
        bool , HasFlag(int flag) const,
        "Test if the given style is set for this window.", "");
    

    DocDeclStr(
        virtual bool , IsRetained() const,
        "Returns true if the window is retained, false otherwise.  Retained
windows are only available on X platforms.", "");
    

    DocDeclStr(
        bool , ToggleWindowStyle(int flag),
        "Turn the flag on if it had been turned off before and vice versa,
returns True if the flag is turned on by this function call.", "");
    
    

    DocDeclStr(
        virtual void , SetExtraStyle(long exStyle),
        "Sets the extra style bits for the window.  Extra styles are the less
often used style bits which can't be set with the constructor or with
SetWindowStyleFlag()", "");
    
    DocDeclStr(
        long , GetExtraStyle() const,
        "Returns the extra style bits for the window.", "");
    

    
    DocDeclStr(
        virtual void , MakeModal(bool modal = true),
        "Disables all other windows in the application so that the user can
only interact with this window.  Passing False will reverse this
effect.", "");
    

    
    DocDeclStr(
        virtual void , SetThemeEnabled(bool enableTheme),
        "This function tells a window if it should use the system's \"theme\"
 code to draw the windows' background instead if its own background
 drawing code. This will only have an effect on platforms that support
 the notion of themes in user defined windows. One such platform is
 GTK+ where windows can have (very colourful) backgrounds defined by a
 user's selected theme.

Dialogs, notebook pages and the status bar have this flag set to true
by default so that the default look and feel is simulated best.", "");
    
    DocDeclStr(
        virtual bool , GetThemeEnabled() const,
        "Return the themeEnabled flag.", "");
    

// TODO with directors    
//     // controls by default inherit the colours of their parents, if a
//     // particular control class doesn't want to do it, it can override
//     // ShouldInheritColours() to return False
//     virtual bool ShouldInheritColours() const;



    

    // focus and keyboard handling
    // ---------------------------

    
    DocDeclStr(
        virtual void , SetFocus(),
        "Set's the focus to this window, allowing it to receive keyboard input.", "");
    
    DocDeclStr(
        virtual void , SetFocusFromKbd(),
        "Set focus to this window as the result of a keyboard action.  Normally
only called internally.", "");
    

    
    DocDeclStr(
        static wxWindow *, FindFocus(),
        "Returns the window or control that currently has the keyboard focus,
or None.", "");
    

    DocDeclStr(
        virtual bool , AcceptsFocus() const,
        "Can this window have focus?", "");
    

    DocDeclStr(
        virtual bool , AcceptsFocusFromKeyboard() const,
        "Can this window be given focus by keyboard navigation? if not, the
only way to give it focus (provided it accepts it at all) is to click
it.", "");
    



    DocDeclAStr(
        virtual bool , Navigate(int flags = wxNavigationKeyEvent::IsForward),
        "Navigate(self, int flags=NavigationKeyEvent.IsForward) -> bool",
        "Does keyboard navigation from this window to another, by sending a
`wx.NavigationKeyEvent`.", "
 
    :param flags: A combination of the ``IsForward`` or ``IsBackward``
        and the ``WinChange`` values in the `wx.NavigationKeyEvent`
        class, which determine if the navigation should be in forward
        or reverse order, and if it should be able to cross parent
        window boundaries, such as between notebook pages or MDI child
        frames.  Typically the status of the Shift key (for forward or
        backward) or the Control key (for WinChange) would be used to
        determine how to set the flags.

One situation in which you may wish to call this method is from a text
control custom keypress handler to do the default navigation behaviour
for the tab key, since the standard default behaviour for a multiline
text control with the wx.TE_PROCESS_TAB style is to insert a tab and
not navigate to the next control.");

    

    DocDeclStr(
        void , MoveAfterInTabOrder(wxWindow *win),
        "Moves this window in the tab navigation order after the specified
sibling window.  This means that when the user presses the TAB key on
that other window, the focus switches to this window.

The default tab order is the same as creation order.  This function
and `MoveBeforeInTabOrder` allow to change it after creating all the
windows.
", "");

    
    DocDeclStr(
        void , MoveBeforeInTabOrder(wxWindow *win),
        "Same as `MoveAfterInTabOrder` except that it inserts this window just
before win instead of putting it right after it.", "");
    

    





    // parent/children relations
    // -------------------------


    //wxWindowList& GetChildren();  // TODO: Do a typemap or a wrapper for wxWindowList
    %extend {
        DocStr(GetChildren,
               "Returns a list of the window's children.  NOTE: Currently this is a
copy of the child window list maintained by the window, so the return
value of this function is only valid as long as the window's children
do not change.", "");
        PyObject* GetChildren() {
            wxWindowList& list = self->GetChildren();
            return wxPy_ConvertList(&list);
        }
    }

    DocDeclStr(
        wxWindow *, GetParent() const,
        "Returns the parent window of this window, or None if there isn't one.", "");
    
    DocDeclStr(
        wxWindow *, GetGrandParent() const,
        "Returns the parent of the parent of this window, or None if there
isn't one.", "");
    


    DocDeclStr(
        virtual bool , IsTopLevel() const,
        "Returns true if the given window is a top-level one. Currently all
frames and dialogs are always considered to be top-level windows (even
if they have a parent window).", "");
    

    // change the real parent of this window, return True if the parent
    // was changed, False otherwise (error or newParent == oldParent)
    DocDeclStr(
        virtual bool , Reparent( wxWindow *newParent ),
        "Reparents the window, i.e the window will be removed from its current
parent window (e.g. a non-standard toolbar in a wxFrame) and then
re-inserted into another. Available on Windows and GTK.  Returns True
if the parent was changed, False otherwise (error or newParent ==
oldParent)", "");
    

    DocDeclStr(
        virtual void , AddChild( wxWindow *child ),
        "Adds a child window. This is called automatically by window creation
functions so should not be required by the application programmer.", "");
    
    DocDeclStr(
        virtual void , RemoveChild( wxWindow *child ),
        "Removes a child window. This is called automatically by window
deletion functions so should not be required by the application
programmer.", "");


    DocStr(SetDoubleBuffered,
           "Currently wxGTK2 only.", "");
#ifdef __WXGTK__
    void SetDoubleBuffered(bool on);
#else
    %extend {
        void SetDoubleBuffered(bool on) {}
    }
#endif


    // looking for windows
    // -------------------

    DocDeclStrName(
        wxWindow *, FindWindow( long winid ),
        "Find a chld of this window by window ID", "",
        FindWindowById);
    
    DocDeclStrName(
        wxWindow *, FindWindow( const wxString& name ),
        "Find a child of this window by name", "",
        FindWindowByName);
    


    // event handler stuff
    // -------------------

    DocDeclStr(
        wxEvtHandler *, GetEventHandler() const,
        "Returns the event handler for this window. By default, the window is
its own event handler.", "");
    

    DocDeclStr(
        void , SetEventHandler( wxEvtHandler *handler ),
        "Sets the event handler for this window.  An event handler is an object
that is capable of processing the events sent to a window.  (In other
words, is able to dispatch the events to handler function.)  By
default, the window is its own event handler, but an application may
wish to substitute another, for example to allow central
implementation of event-handling for a variety of different window
classes.

It is usually better to use `wx.Window.PushEventHandler` since this sets
up a chain of event handlers, where an event not handled by one event
handler is handed off to the next one in the chain.", "");
    

    DocDeclStr(
        void , PushEventHandler( wxEvtHandler *handler ),
        "Pushes this event handler onto the event handler stack for the window.
An event handler is an object that is capable of processing the events
sent to a window.  (In other words, is able to dispatch the events to
handler function.)  By default, the window is its own event handler,
but an application may wish to substitute another, for example to
allow central implementation of event-handling for a variety of
different window classes.

wx.Window.PushEventHandler allows an application to set up a chain of
event handlers, where an event not handled by one event handler is
handed to the next one in the chain.  Use `wx.Window.PopEventHandler`
to remove the event handler.  Ownership of the handler is *not* given
to the window, so you should be sure to pop the handler before the
window is destroyed and either let PopEventHandler destroy it, or call
its Destroy method yourself.", "");

    
    DocDeclStr(
        wxEvtHandler *, PopEventHandler( bool deleteHandler = false ),
        "Removes and returns the top-most event handler on the event handler
stack.  If deleteHandler is True then the wx.EvtHandler object will be
destroyed after it is popped, and ``None`` will be returned instead.", "");
    

    DocDeclStr(
        bool , RemoveEventHandler(wxEvtHandler *handler),
        "Find the given handler in the event handler chain and remove (but not
delete) it from the event handler chain, returns True if it was found
and False otherwise (this also results in an assert failure so this
function should only be called when the handler is supposed to be
there.)", "");
    



    // validators
    // ----------

    // a window may have an associated validator which is used to control
    // user input
    DocDeclStr(
        virtual void , SetValidator( const wxValidator &validator ),
        "Deletes the current validator (if any) and sets the window validator,
having called wx.Validator.Clone to create a new validator of this
type.", "");
    
    DocDeclStr(
        virtual wxValidator *, GetValidator(),
        "Returns a pointer to the current validator for the window, or None if
there is none.", "");


    DocDeclStr(
        virtual bool , Validate(),
        "Validates the current values of the child controls using their
validators.  If the window has wx.WS_EX_VALIDATE_RECURSIVELY extra
style flag set, the method will also call Validate() of all child
windows.  Returns false if any of the validations failed.", "");
    

    DocDeclStr(
        virtual bool , TransferDataToWindow(),
        "Transfers values to child controls from data areas specified by their
validators.  If the window has wx.WS_EX_VALIDATE_RECURSIVELY extra
style flag set, the method will also call TransferDataToWindow() of
all child windows.", "");
    
    DocDeclStr(
        virtual bool , TransferDataFromWindow(),
        "Transfers values from child controls to data areas specified by their
validators. Returns false if a transfer failed.  If the window has
wx.WS_EX_VALIDATE_RECURSIVELY extra style flag set, the method will
also call TransferDataFromWindow() of all child windows.", "");
    

    DocDeclStr(
        virtual void , InitDialog(),
        "Sends an EVT_INIT_DIALOG event, whose handler usually transfers data
to the dialog via validators.", "");
    
    


    // accelerators
    // ------------

    DocDeclStr(
        virtual void , SetAcceleratorTable( const wxAcceleratorTable& accel ),
        "Sets the accelerator table for this window.", "");
    
    DocDeclStr(
        wxAcceleratorTable *, GetAcceleratorTable(),
        "Gets the accelerator table for this window.", "");
    




    // hot keys (system wide accelerators)
    // -----------------------------------
    %extend {
        DocStr(RegisterHotKey,
               "Registers a system wide hotkey. Every time the user presses the hotkey
registered here, this window will receive a hotkey event. It will
receive the event even if the application is in the background and
does not have the input focus because the user is working with some
other application.  To bind an event handler function to this hotkey
use EVT_HOTKEY with an id equal to hotkeyId.  Returns True if the
hotkey was registered successfully.", "");
        bool RegisterHotKey(int hotkeyId, int modifiers, int keycode) {
        %#if wxUSE_HOTKEY
            return self->RegisterHotKey(hotkeyId, modifiers, keycode);
        %#else
            return false;
        %#endif
        }

        
        DocStr(UnregisterHotKey,
               "Unregisters a system wide hotkey.", "");
        bool UnregisterHotKey(int hotkeyId) {
        #if wxUSE_HOTKEY
            return self->UnregisterHotKey(hotkeyId);
        #else
            return false;
        #endif
        }
    }



    // "dialog units" translations
    // ---------------------------

    DocStr(ConvertDialogToPixels,
           "Converts a point or size from dialog units to pixels.  Dialog units
are used for maintaining a dialog's proportions even if the font
changes. For the x dimension, the dialog units are multiplied by the
average character width and then divided by 4. For the y dimension,
the dialog units are multiplied by the average character height and
then divided by 8.", "");
    %Rename(ConvertDialogPointToPixels, wxPoint, ConvertDialogToPixels(const wxPoint& pt));
    %Rename(ConvertDialogSizeToPixels,  wxSize,  ConvertDialogToPixels(const wxSize& sz));
    %Rename(DLG_PNT, wxPoint, ConvertDialogToPixels(const wxPoint& pt));
    %Rename(DLG_SZE, wxSize,  ConvertDialogToPixels(const wxSize& sz));


    DocStr(ConvertPixelPointToDialog,
           "Converts a point or size from pixels to dialog units.  Dialog units
are used for maintaining a dialog's proportions even if the font
changes. For the x dimension, the dialog units are multiplied by the
average character width and then divided by 4. For the y dimension,
the dialog units are multiplied by the average character height and
then divided by 8.", "");
    %Rename(ConvertPixelPointToDialog, wxPoint, ConvertPixelsToDialog(const wxPoint& pt));
    %Rename(ConvertPixelSizeToDialog,  wxSize,  ConvertPixelsToDialog(const wxSize& sz));



    // mouse functions
    // ---------------

    DocDeclStr(
        virtual void , WarpPointer(int x, int y),
        "Moves the pointer to the given position on the window.

NOTE: This function is not supported under Mac because Apple Human
Interface Guidelines forbid moving the mouse cursor programmatically.", "");
    

    DocDeclStr(
        void , CaptureMouse(),
        "Directs all mouse input to this window. Call wx.Window.ReleaseMouse to
release the capture.

Note that wxWindows maintains the stack of windows having captured the
mouse and when the mouse is released the capture returns to the window
which had had captured it previously and it is only really released if
there were no previous window. In particular, this means that you must
release the mouse as many times as you capture it, unless the window
receives the `wx.MouseCaptureLostEvent` event.
 
Any application which captures the mouse in the beginning of some
operation *must* handle `wx.MouseCaptureLostEvent` and cancel this
operation when it receives the event. The event handler must not
recapture mouse.", "");
    
    DocDeclStr(
        void , ReleaseMouse(),
        "Releases mouse input captured with wx.Window.CaptureMouse.", "");
    

    DocDeclStr(
        static wxWindow *, GetCapture(),
        "Returns the window which currently captures the mouse or None", "");
    

    DocDeclStr(
        virtual bool , HasCapture() const,
        "Returns true if this window has the current mouse capture.", "");
    


    

    // painting the window
    // -------------------

    DocDeclStr(
        virtual void , Refresh( bool eraseBackground = true,
                                const wxRect *rect = NULL ),
        "Mark the specified rectangle (or the whole window) as \"dirty\" so it
will be repainted.  Causes an EVT_PAINT event to be generated and sent
to the window.", "");
    

    DocDeclStr(
        void , RefreshRect(const wxRect& rect, bool eraseBackground = true),
        "Redraws the contents of the given rectangle: the area inside it will
be repainted.  This is the same as Refresh but has a nicer syntax.", "");
    

    DocDeclStr(
        virtual void , Update(),
        "Calling this method immediately repaints the invalidated area of the
window instead of waiting for the EVT_PAINT event to happen, (normally
this would usually only happen when the flow of control returns to the
event loop.)  Notice that this function doesn't refresh the window and
does nothing if the window has been already repainted.  Use `Refresh`
first if you want to immediately redraw the window (or some portion of
it) unconditionally.", "");
    

    DocDeclStr(
        virtual void , ClearBackground(),
        "Clears the window by filling it with the current background
colour. Does not cause an erase background event to be generated.", "");
    


    DocDeclStr(
        virtual void , Freeze(),
        "Freezes the window or, in other words, prevents any updates from
taking place on screen, the window is not redrawn at all. Thaw must be
called to reenable window redrawing.  Calls to Freeze/Thaw may be
nested, with the actual Thaw being delayed until all the nesting has
been undone.

This method is useful for visual appearance optimization (for example,
it is a good idea to use it before inserting large amount of text into
a wxTextCtrl under wxGTK) but is not implemented on all platforms nor
for all controls so it is mostly just a hint to wxWindows and not a
mandatory directive.", "");

    
    DocDeclStr(
        virtual bool , IsFrozen() const,
        "Returns ``True`` if the window has been frozen and not thawed yet.

:see: `Freeze` and `Thaw`", "");
    

    
    DocDeclStr(
        virtual void , Thaw(),
        "Reenables window updating after a previous call to Freeze.  Calls to
Freeze/Thaw may be nested, so Thaw must be called the same number of
times that Freeze was before the window will be updated.", "");
    

    DocDeclStr(
        virtual void , PrepareDC( wxDC & dc ),
        "Call this function to prepare the device context for drawing a
scrolled image. It sets the device origin according to the current
scroll position.", "");

    DocDeclStr(
        virtual bool , IsDoubleBuffered() const,
        "Returns ``True`` if the window contents is double-buffered by the
system, i.e. if any drawing done on the window is really done on a
temporary backing surface and transferred to the screen all at once
later.", "");
    

    DocDeclStr(
        wxRegion& , GetUpdateRegion(),
        "Returns the region specifying which parts of the window have been
damaged. Should only be called within an EVT_PAINT handler.", "");
    

    DocDeclStr(
        wxRect , GetUpdateClientRect() const,
        "Get the update rectangle region bounding box in client coords.", "");
    

    DocStr(IsExposed,
           "Returns true if the given point or rectangle area has been exposed
since the last repaint. Call this in an paint event handler to
optimize redrawing by only redrawing those areas, which have been
exposed.", "");
    bool IsExposed( int x, int y, int w=1, int h=1 ) const;
    %Rename(IsExposedPoint, bool, IsExposed( const wxPoint& pt ) const);
    %Rename(IsExposedRect,  bool, IsExposed( const wxRect& rect ) const);



    // colours, fonts and cursors
    // --------------------------


    DocDeclStr(
        virtual wxVisualAttributes , GetDefaultAttributes() const,
        "Get the default attributes for an instance of this class.  This is
useful if you want to use the same font or colour in your own control
as in a standard control -- which is a much better idea than hard
coding specific colours or fonts which might look completely out of
place on the user's system, especially if it uses themes.", "");
    

    DocDeclStr(
        static wxVisualAttributes ,
        GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL),
        "Get the default attributes for this class.  This is useful if you want
to use the same font or colour in your own control as in a standard
control -- which is a much better idea than hard coding specific
colours or fonts which might look completely out of place on the
user's system, especially if it uses themes.

The variant parameter is only relevant under Mac currently and is
ignore under other platforms. Under Mac, it will change the size of
the returned font. See `wx.Window.SetWindowVariant` for more about
this.", "");
    
    
    DocDeclStr(
        virtual bool , SetBackgroundColour( const wxColour &colour ),
        "Sets the background colour of the window.  Returns True if the colour
was changed.  The background colour is usually painted by the default
EVT_ERASE_BACKGROUND event handler function under Windows and
automatically under GTK.  Using `wx.NullColour` will reset the window
to the default background colour.

Note that setting the background colour may not cause an immediate
refresh, so you may wish to call `ClearBackground` or `Refresh` after
calling this function.

Using this function will disable attempts to use themes for this
window, if the system supports them.  Use with care since usually the
themes represent the appearance chosen by the user to be used for all
applications on the system.", "");

    DocDeclStr(
        void , SetOwnBackgroundColour(const wxColour& colour),
        "", "");
    

    
    DocDeclStr(
        virtual bool , SetForegroundColour( const wxColour &colour ),
        "Sets the foreground colour of the window.  Returns True is the colour
was changed.  The interpretation of foreground colour is dependent on
the window class; it may be the text colour or other colour, or it may
not be used at all.", "");

    DocDeclStr(
        void , SetOwnForegroundColour(const wxColour& colour),
        "", "");
    

    
    DocDeclStr(
        wxColour , GetBackgroundColour() const,
        "Returns the background colour of the window.", "");
    
    DocDeclStr(
        wxColour , GetForegroundColour() const,
        "Returns the foreground colour of the window.  The interpretation of
foreground colour is dependent on the window class; it may be the text
colour or other colour, or it may not be used at all.", "");

    DocDeclStr(
        bool , InheritsBackgroundColour() const,
        "", "");
    
    DocDeclStr(
        bool , UseBgCol() const,
        "", "");
    
    
// TODO:
//     // if the window shouldn't inherit its colour from the parent, override
//     // this function to return true
//     //
//     // this is currently only used by wxMSW and wxUniv but should be useful for
//     // the other ports too
//     virtual bool ProvidesBackground() const;


    // Set/get the background style.
    // Pass one of wxBG_STYLE_SYSTEM, wxBG_STYLE_COLOUR, wxBG_STYLE_CUSTOM
    DocDeclStr(
        virtual bool , SetBackgroundStyle(wxBackgroundStyle style),
        "Returns the background style of the window. The background style
indicates how the background of the window is drawn.

    ======================  ========================================
    wx.BG_STYLE_SYSTEM      The background colour or pattern should
                            be determined by the system
    wx.BG_STYLE_COLOUR      The background should be a solid colour
    wx.BG_STYLE_CUSTOM      The background will be implemented by the
                            application.
    ======================  ========================================

On GTK+, use of wx.BG_STYLE_CUSTOM allows the flicker-free drawing of
a custom background, such as a tiled bitmap. Currently the style has
no effect on other platforms.

:see: `GetBackgroundStyle`, `SetBackgroundColour`", "");
    
    DocDeclStr(
        virtual wxBackgroundStyle , GetBackgroundStyle() const,
        "Returns the background style of the window.

:see: `SetBackgroundStyle`", "");

    
    DocDeclStr(
        bool , HasTransparentBackground(),
        "Returns True if this window's background is transparent (as, for
example, for `wx.StaticText`) and should show the parent window's
background.

This method is mostly used internally by the library itself and you
normally shouldn't have to call it. You may, however, have to override
it in your custom control classes to ensure that background is painted
correctly.", "");
    
    
    DocDeclStr(
        virtual bool , SetCursor( const wxCursor &cursor ),
        "Sets the window's cursor. Notice that the window cursor also sets it
for the children of the window implicitly.

The cursor may be wx.NullCursor in which case the window cursor will
be reset back to default.", "");
    
    DocDeclStr(
        wxCursor , GetCursor(),
        "Return the cursor associated with this window.", "");
    


    DocDeclStr(
        virtual bool , SetFont( const wxFont &font ),
        "Sets the font for this window.", "");
    
    DocDeclStr(
        void , SetOwnFont(const wxFont& font),
        "", "");
    

    
    DocDeclStr(
        wxFont , GetFont(),
        "Returns the default font used for this window.", "");

    

    %disownarg( wxCaret *caret );
    DocDeclStr(
        void , SetCaret(wxCaret *caret),
        "Sets the caret associated with the window.", "");
    %cleardisown( wxCaret *caret );
    
    DocDeclStr(
        wxCaret *, GetCaret() const,
        "Returns the caret associated with the window.", "");
    


    DocDeclStr(
        virtual int , GetCharHeight() const,
        "Get the (average) character size for the current font.", "");
    
    DocDeclStr(
        virtual int , GetCharWidth() const,
        "Get the (average) character size for the current font.", "");
    


    DocDeclAStr(
        void, GetTextExtent(const wxString& string, int *OUTPUT, int *OUTPUT),
        "GetTextExtent(String string) -> (width, height)",
        "Get the width and height of the text using the current font.", "");
    DocDeclAStrName(
        void, GetTextExtent(const wxString& string,
                            int *OUTPUT, int *OUTPUT, int *OUTPUT, int* OUTPUT, 
                            const wxFont* font = NULL),
        "GetFullTextExtent(String string, Font font=None) ->\n   (width, height, descent, externalLeading)",
        "Get the width, height, decent and leading of the text using the
current or specified font.", "",
        GetFullTextExtent);



    // client <-> screen coords
    // ------------------------

    %apply int * INOUT { int* x, int* y };

    // translate to/from screen/client coordinates
    DocDeclAStrName(
        void , ClientToScreen( int *x, int *y ) const,
        "ClientToScreenXY(int x, int y) -> (x,y)",
        "Converts to screen coordinates from coordinates relative to this window.", "",
        ClientToScreenXY);
    
    DocDeclAStrName(
        void , ScreenToClient( int *x, int *y ) const,
        "ScreenToClientXY(int x, int y) -> (x,y)",
        "Converts from screen to client window coordinates.", "",
        ScreenToClientXY);
    

    DocDeclStr(
        wxPoint , ClientToScreen(const wxPoint& pt) const,
        "Converts to screen coordinates from coordinates relative to this window.", "");
    
    DocDeclStr(
        wxPoint , ScreenToClient(const wxPoint& pt) const,
        "Converts from screen to client window coordinates.", "");
    


    DocDeclStrName(
        wxHitTest , HitTest(wxCoord x, wxCoord y) const,
        "Test where the given (in client coords) point lies", "",
        HitTestXY);
    
    DocDeclStr(
        wxHitTest , HitTest(const wxPoint& pt) const,
        "Test where the given (in client coords) point lies", "");
    



    // misc
    // ----

    %nokwargs GetBorder;
    DocDeclStr(
        wxBorder , GetBorder(long flags) const,
        "Get the window border style from the given flags: this is different
from simply doing flags & wxBORDER_MASK because it uses
GetDefaultBorder() to translate wxBORDER_DEFAULT to something
reasonable.
", "");
    
    DocDeclStr(
        wxBorder , GetBorder() const,
        "Get border for the flags of this window", "");
    

    

    DocDeclStr(
        virtual void , UpdateWindowUI(long flags = wxUPDATE_UI_NONE),
        "This function sends EVT_UPDATE_UI events to the window. The particular
implementation depends on the window; for example a wx.ToolBar will
send an update UI event for each toolbar button, and a wx.Frame will
send an update UI event for each menubar menu item. You can call this
function from your application to ensure that your UI is up-to-date at
a particular point in time (as far as your EVT_UPDATE_UI handlers are
concerned). This may be necessary if you have called
`wx.UpdateUIEvent.SetMode` or `wx.UpdateUIEvent.SetUpdateInterval` to
limit the overhead that wxWindows incurs by sending update UI events
in idle time.",
"
The flags should be a bitlist of one or more of the following values:

    =====================      ==============================
    wx.UPDATE_UI_NONE          No particular value
    wx.UPDATE_UI_RECURSE       Call the function for descendants
    wx.UPDATE_UI_FROMIDLE      Invoked from OnIdle
    =====================      ==============================

If you are calling this function from an OnIdle function, make sure
you pass the wx.UPDATE_UI_FROMIDLE flag, since this tells the window
to only update the UI elements that need to be updated in idle
time. Some windows update their elements only when necessary, for
example when a menu is about to be shown. The following is an example
of how to call UpdateWindowUI from an idle function::

    def OnIdle(self, evt):
        if wx.UpdateUIEvent.CanUpdate(self):
            self.UpdateWindowUI(wx.UPDATE_UI_FROMIDLE);
");
    

// TODO: using directors?
//     // do the window-specific processing after processing the update event
//     virtual void DoUpdateWindowUI(wxUpdateUIEvent& event) ;


    DocStr(PopupMenu,
           "Pops up the given menu at the specified coordinates, relative to this window,
and returns control when the user has dismissed the menu. If a menu item is
selected, the corresponding menu event is generated and will be processed as
usual.  If the default position is given then the current position of the
mouse cursor will be used.", "");
    %Rename(PopupMenuXY,  bool, PopupMenu(wxMenu *menu, int x=-1, int y=-1));
    bool PopupMenu(wxMenu *menu, const wxPoint& pos=wxDefaultPosition);


    DocDeclStr(
        virtual bool , HasMultiplePages() const,
        "", "");
    
    
    %extend {
        DocStr(GetHandle,
               "Returns the platform-specific handle (as a long integer) of the
physical window.  Currently on wxMac it returns the handle of the
toplevel parent of the window.", "");
        long GetHandle() {
            return wxPyGetWinHandle(self);
        }
    }

    DocStr(
        AssociateHandle,
        "Associate the window with a new native handle", "");
    %extend {
        void  AssociateHandle(long handle) {
            self->AssociateHandle((WXWidget)handle);
        }
    }
        
           
    DocDeclStr(
        virtual void , DissociateHandle(),
        "Dissociate the current native handle from the window", "");
    

    
#ifdef __WXMSW__
    // A way to do the native draw first...  Too bad it isn't in wxGTK too.
    void OnPaint(wxPaintEvent& event);
#endif



    // scrollbars
    // ----------

    
    DocDeclStr(
        bool , HasScrollbar(int orient) const,
        "Does the window have the scrollbar for this orientation?", "");
    

    // configure the window scrollbars
    DocDeclStr(
        virtual void , SetScrollbar( int orientation,
                                     int position,
                                     int thumbSize,
                                     int range,
                                     bool refresh = true ),
        "Sets the scrollbar properties of a built-in scrollbar.",
        "
    :param orientation: Determines the scrollbar whose page size is to
                 be set. May be wx.HORIZONTAL or wx.VERTICAL.

    :param position: The position of the scrollbar in scroll units.

    :param thumbSize: The size of the thumb, or visible portion of the
                 scrollbar, in scroll units.

    :param range: The maximum position of the scrollbar.

    :param refresh: True to redraw the scrollbar, false otherwise.
");
    
    DocDeclStr(
        virtual void , SetScrollPos( int orientation, int pos, bool refresh = true ),
        "Sets the position of one of the built-in scrollbars.", "");
    
    DocDeclStr(
        virtual int , GetScrollPos( int orientation ) const,
        "Returns the built-in scrollbar position.", "");
    
    DocDeclStr(
        virtual int , GetScrollThumb( int orientation ) const,
        "Returns the built-in scrollbar thumb size.", "");
    
    DocDeclStr(
        virtual int , GetScrollRange( int orientation ) const,
        "Returns the built-in scrollbar range.", "");
    

    

    DocDeclStr(
        virtual void , ScrollWindow( int dx, int dy,
                                     const wxRect* rect = NULL ),
        "Physically scrolls the pixels in the window and move child windows
accordingly.  Use this function to optimise your scrolling
implementations, to minimise the area that must be redrawn. Note that
it is rarely required to call this function from a user program.","

    :param dx:   Amount to scroll horizontally.

    :param dy:   Amount to scroll vertically.

    :param rect: Rectangle to invalidate. If this is None, the whole
          window is invalidated. If you pass a rectangle corresponding
          to the area of the window exposed by the scroll, your
          painting handler can optimize painting by checking for the
          invalidated region.");
    

    DocDeclStr(
        virtual bool , ScrollLines(int lines),
        "If the platform and window class supports it, scrolls the window by
the given number of lines down, if lines is positive, or up if lines
is negative.  Returns True if the window was scrolled, False if it was
already on top/bottom and nothing was done.", "");
    
    DocDeclStr(
        virtual bool , ScrollPages(int pages),
        "If the platform and window class supports it, scrolls the window by
the given number of pages down, if pages is positive, or up if pages
is negative.  Returns True if the window was scrolled, False if it was
already on top/bottom and nothing was done.", "");
    

    DocDeclStr(
        bool , LineUp(),
        "This is just a wrapper for ScrollLines(-1).", "");
    
    DocDeclStr(
        bool , LineDown(),
        "This is just a wrapper for ScrollLines(1).", "");
    
    DocDeclStr(
        bool , PageUp(),
        "This is just a wrapper for ScrollPages(-1).", "");
    
    DocDeclStr(
        bool , PageDown(),
        "This is just a wrapper for ScrollPages(1).", "");
    



    // context-sensitive help
    // ----------------------

    DocDeclStr(
        void , SetHelpText(const wxString& text),
        "Sets the help text to be used as context-sensitive help for this
window.  Note that the text is actually stored by the current
`wx.HelpProvider` implementation, and not in the window object itself.", "");
    

    DocDeclStr(
        void , SetHelpTextForId(const wxString& text),
        "Associate this help text with all windows with the same id as this
one.", "");
    

    DocDeclStr(
        virtual wxString , GetHelpTextAtPoint(const wxPoint& pt,
                                              wxHelpEvent::Origin origin) const,
        "Get the help string associated with the given position in this window.

Notice that pt may be invalid if event origin is keyboard or unknown
and this method should return the global window help text then
", "");
    
    
   DocDeclStr(
        wxString , GetHelpText() const,
        "Gets the help text to be used as context-sensitive help for this
window.  Note that the text is actually stored by the current
`wx.HelpProvider` implementation, and not in the window object itself.", "");
    


#ifndef __WXX11__
    // tooltips
    // --------

    DocStr(SetToolTip,
           "Attach a tooltip to the window.", "");
    %Rename(SetToolTipString, void,  SetToolTip( const wxString &tip ));

    %disownarg( wxToolTip *tip );
    void SetToolTip( wxToolTip *tip );
    %cleardisown( wxToolTip *tip );

    DocDeclStr(
        wxToolTip* , GetToolTip() const,
        "get the associated tooltip or None if none", "");
    
    // LINK ERROR --> wxString GetToolTipText() const;
#endif


    
#ifndef __WXX11__
    // drag and drop
    // -------------

    %disownarg( wxPyDropTarget *dropTarget );
    DocDeclStr(
        virtual void , SetDropTarget( wxPyDropTarget *dropTarget ),
        "Associates a drop target with this window.  If the window already has
a drop target, it is deleted.", "");   
    %cleardisown( wxPyDropTarget *dropTarget );

    
    DocDeclStr(
        virtual wxPyDropTarget *, GetDropTarget() const,
        "Returns the associated drop target, which may be None.", "");
    

    DocStr(DragAcceptFiles,
        "Enables or disables eligibility for drop file events, EVT_DROP_FILES.
Only functional on Windows.", "");
#ifdef __WXMSW__  
    void DragAcceptFiles(bool accept);
#else
    %extend {
        void DragAcceptFiles(bool accept) {}
    }
#endif
#endif
    

    // constraints and sizers
    // ----------------------

    %disownarg(wxLayoutConstraints*);
    DocDeclStr(
        void , SetConstraints( wxLayoutConstraints *constraints ),
        "Sets the window to have the given layout constraints. If an existing
layout constraints object is already owned by the window, it will be
deleted.  Pass None to disassociate and delete the window's current
constraints.

You must call SetAutoLayout to tell a window to use the constraints
automatically in its default EVT_SIZE handler; otherwise, you must
handle EVT_SIZE yourself and call Layout() explicitly. When setting
both a wx.LayoutConstraints and a wx.Sizer, only the sizer will have
effect.", "");
    
    DocDeclStr(
        wxLayoutConstraints *, GetConstraints() const,
        "Returns a pointer to the window's layout constraints, or None if there
are none.", "");
    %cleardisown(wxLayoutConstraints*);
    

    DocDeclStr(
        void , SetAutoLayout( bool autoLayout ),
        "Determines whether the Layout function will be called automatically
when the window is resized.  lease note that this only happens for the
windows usually used to contain children, namely `wx.Panel` and
`wx.TopLevelWindow` (and the classes deriving from them).

This method is called implicitly by `SetSizer` but if you use
`SetConstraints` you should call it manually or otherwise the window
layout won't be correctly updated when its size changes.", "");
    
    DocDeclStr(
        bool , GetAutoLayout() const,
        "Returns the current autoLayout setting", "");
    

    DocDeclStr(
        virtual bool , Layout(),
        "Invokes the constraint-based layout algorithm or the sizer-based
algorithm for this window.  See SetAutoLayout: when auto layout is on,
this function gets called automatically by the default EVT_SIZE
handler when the window is resized.", "");
    

    %disownarg( wxSizer *sizer );
    DocDeclStr(
        void , SetSizer(wxSizer *sizer, bool deleteOld = true ),
        "Sets the window to have the given layout sizer. The window will then
own the object, and will take care of its deletion. If an existing
layout sizer object is already owned by the window, it will be deleted
if the deleteOld parameter is true. Note that this function will also
call SetAutoLayout implicitly with a True parameter if the sizer is
non-None, and False otherwise.", "");
    
    DocDeclStr(
        void , SetSizerAndFit( wxSizer *sizer, bool deleteOld = true ),
        "The same as SetSizer, except it also sets the size hints for the
window based on the sizer's minimum size.", "");
    %cleardisown( wxSizer *sizer );


    DocDeclStr(
        wxSizer *, GetSizer() const,
        "Return the sizer associated with the window by a previous call to
SetSizer or None if there isn't one.", "");
    

    // Track if this window is a member of a sizer
    DocDeclStr(
        void , SetContainingSizer(wxSizer* sizer),
        "This normally does not need to be called by application code. It is
called internally when a window is added to a sizer, and is used so
the window can remove itself from the sizer when it is destroyed.", "");
    
    DocDeclStr(
        wxSizer *, GetContainingSizer() const,
        "Return the sizer that this window is a member of, if any, otherwise None.", "");
    



    // accessibility
    // ----------------------
#if wxUSE_ACCESSIBILITY
    // Override to create a specific accessible object.
    virtual wxAccessible* CreateAccessible();

    // Sets the accessible object.
    void SetAccessible(wxAccessible* accessible) ;

    // Returns the accessible object.
    wxAccessible* GetAccessible() { return m_accessible; };

    // Returns the accessible object, creating if necessary.
    wxAccessible* GetOrCreateAccessible() ;
#endif




    DocDeclStr(
        virtual void , InheritAttributes(),
        "This function is (or should be, in case of custom controls) called
during window creation to intelligently set up the window visual
attributes, that is the font and the foreground and background
colours.

By 'intelligently' the following is meant: by default, all windows use
their own default attributes. However if some of the parent's
attributes are explicitly changed (that is, using SetFont and not
SetOwnFont) and if the corresponding attribute hadn't been
explicitly set for this window itself, then this window takes the same
value as used by the parent. In addition, if the window overrides
ShouldInheritColours to return false, the colours will not be changed
no matter what and only the font might.

This rather complicated logic is necessary in order to accommodate the
different usage scenarios. The most common one is when all default
attributes are used and in this case, nothing should be inherited as
in modern GUIs different controls use different fonts (and colours)
than their siblings so they can't inherit the same value from the
parent. However it was also deemed desirable to allow to simply change
the attributes of all children at once by just changing the font or
colour of their common parent, hence in this case we do inherit the
parents attributes.
", "");

    
// TODO:  Virtualize this with directors    
    DocDeclStr(
        virtual bool , ShouldInheritColours() const,
        "Return true from here to allow the colours of this window to be
changed by InheritAttributes, returning false forbids inheriting them
from the parent window.

The base class version returns false, but this method is overridden in
wxControl where it returns true.", "");
    


    DocDeclStr(
        virtual bool , CanSetTransparent(),
        "Returns ``True`` if the platform supports setting the transparency for
this window.  Note that this method will err on the side of caution,
so it is possible that this will return ``False`` when it is in fact
possible to set the transparency.

NOTE: On X-windows systems the X server must have the composite
extension loaded, and there must be a composite manager program (such
as xcompmgr) running.", "");

    DocDeclStr(
        virtual bool , SetTransparent(byte alpha),
        "Attempt to set the transparency of this window to the ``alpha`` value,
returns True on success.  The ``alpha`` value is an integer in the
range of 0 to 255, where 0 is fully transparent and 255 is fully
opaque.", "");
    
    

    %pythoncode {
    def PostCreate(self, pre):
        """
        Phase 3 of the 2-phase create <wink!>
        Call this method after precreating the window with the 2-phase create method.
        """
        self.this = pre.this
        self.thisown = pre.thisown
        pre.thisown = 0
        if hasattr(self, '_setOORInfo'):
            self._setOORInfo(self)
        if hasattr(self, '_setCallbackInfo'):
            self._setCallbackInfo(self, pre.__class__)
    }

    %pythoncode {
    def SendSizeEvent(self):
        self.GetEventHandler().ProcessEvent(wx.SizeEvent((-1,-1)))
    }

    
    %property(AcceleratorTable, GetAcceleratorTable, SetAcceleratorTable, doc="See `GetAcceleratorTable` and `SetAcceleratorTable`");
    %property(AdjustedBestSize, GetAdjustedBestSize, doc="See `GetAdjustedBestSize`");
    %property(AutoLayout, GetAutoLayout, SetAutoLayout, doc="See `GetAutoLayout` and `SetAutoLayout`");
    %property(BackgroundColour, GetBackgroundColour, SetBackgroundColour, doc="See `GetBackgroundColour` and `SetBackgroundColour`");
    %property(BackgroundStyle, GetBackgroundStyle, SetBackgroundStyle, doc="See `GetBackgroundStyle` and `SetBackgroundStyle`");
    %property(EffectiveMinSize, GetEffectiveMinSize, doc="See `GetEffectiveMinSize`");
    %property(BestSize, GetBestSize, doc="See `GetBestSize`");
    %property(BestVirtualSize, GetBestVirtualSize, doc="See `GetBestVirtualSize`");
    %property(Border, GetBorder, doc="See `GetBorder`");
    %property(Caret, GetCaret, SetCaret, doc="See `GetCaret` and `SetCaret`");
    %property(CharHeight, GetCharHeight, doc="See `GetCharHeight`");
    %property(CharWidth, GetCharWidth, doc="See `GetCharWidth`");
    %property(Children, GetChildren, doc="See `GetChildren`");
    %property(ClientAreaOrigin, GetClientAreaOrigin, doc="See `GetClientAreaOrigin`");
    %property(ClientRect, GetClientRect, SetClientRect, doc="See `GetClientRect` and `SetClientRect`");
    %property(ClientSize, GetClientSize, SetClientSize, doc="See `GetClientSize` and `SetClientSize`");
    %property(Constraints, GetConstraints, SetConstraints, doc="See `GetConstraints` and `SetConstraints`");
    %property(ContainingSizer, GetContainingSizer, SetContainingSizer, doc="See `GetContainingSizer` and `SetContainingSizer`");
    %property(Cursor, GetCursor, SetCursor, doc="See `GetCursor` and `SetCursor`");
    %property(DefaultAttributes, GetDefaultAttributes, doc="See `GetDefaultAttributes`");
    %property(DropTarget, GetDropTarget, SetDropTarget, doc="See `GetDropTarget` and `SetDropTarget`");
    %property(EventHandler, GetEventHandler, SetEventHandler, doc="See `GetEventHandler` and `SetEventHandler`");
    %property(ExtraStyle, GetExtraStyle, SetExtraStyle, doc="See `GetExtraStyle` and `SetExtraStyle`");
    %property(Font, GetFont, SetFont, doc="See `GetFont` and `SetFont`");
    %property(ForegroundColour, GetForegroundColour, SetForegroundColour, doc="See `GetForegroundColour` and `SetForegroundColour`");
    %property(GrandParent, GetGrandParent, doc="See `GetGrandParent`");
    %property(Handle, GetHandle, doc="See `GetHandle`");
    %property(HelpText, GetHelpText, SetHelpText, doc="See `GetHelpText` and `SetHelpText`");
    %property(Id, GetId, SetId, doc="See `GetId` and `SetId`");
    %property(Label, GetLabel, SetLabel, doc="See `GetLabel` and `SetLabel`");
    %property(LayoutDirection, GetLayoutDirection, SetLayoutDirection, doc="See `GetLayoutDirection` and `SetLayoutDirection`");
    %property(MaxHeight, GetMaxHeight, doc="See `GetMaxHeight`");
    %property(MaxSize, GetMaxSize, SetMaxSize, doc="See `GetMaxSize` and `SetMaxSize`");
    %property(MaxWidth, GetMaxWidth, doc="See `GetMaxWidth`");
    %property(MinHeight, GetMinHeight, doc="See `GetMinHeight`");
    %property(MinSize, GetMinSize, SetMinSize, doc="See `GetMinSize` and `SetMinSize`");
    %property(MinWidth, GetMinWidth, doc="See `GetMinWidth`");
    %property(Name, GetName, SetName, doc="See `GetName` and `SetName`");
    %property(Parent, GetParent, doc="See `GetParent`");
    %property(Position, GetPosition, SetPosition, doc="See `GetPosition` and `SetPosition`");
    %property(Rect, GetRect, SetRect, doc="See `GetRect` and `SetRect`");
    %property(ScreenPosition, GetScreenPosition, doc="See `GetScreenPosition`");
    %property(ScreenRect, GetScreenRect, doc="See `GetScreenRect`");
    %property(Size, GetSize, SetSize, doc="See `GetSize` and `SetSize`");
    %property(Sizer, GetSizer, SetSizer, doc="See `GetSizer` and `SetSizer`");
    %property(ThemeEnabled, GetThemeEnabled, SetThemeEnabled, doc="See `GetThemeEnabled` and `SetThemeEnabled`");
    %property(ToolTip, GetToolTip, SetToolTip, doc="See `GetToolTip` and `SetToolTip`");
    %property(UpdateClientRect, GetUpdateClientRect, doc="See `GetUpdateClientRect`");
    %property(UpdateRegion, GetUpdateRegion, doc="See `GetUpdateRegion`");
    %property(Validator, GetValidator, SetValidator, doc="See `GetValidator` and `SetValidator`");
    %property(VirtualSize, GetVirtualSize, SetVirtualSize, doc="See `GetVirtualSize` and `SetVirtualSize`");
    %property(WindowStyle, GetWindowStyle, SetWindowStyle, doc="See `GetWindowStyle` and `SetWindowStyle`");
    %property(WindowStyleFlag, GetWindowStyleFlag, SetWindowStyleFlag, doc="See `GetWindowStyleFlag` and `SetWindowStyleFlag`");
    %property(WindowVariant, GetWindowVariant, SetWindowVariant, doc="See `GetWindowVariant` and `SetWindowVariant`");

    %property(Shown, IsShown, Show, doc="See `IsShown` and `Show`");
    %property(Enabled, IsEnabled, Enable, doc="See `IsEnabled` and `Enable`");
    %property(TopLevel, IsTopLevel, doc="See `IsTopLevel`");
    
};






%pythoncode {
def DLG_PNT(win, point_or_x, y=None):
    """
    Convenience function for converting a Point or (x,y) in
    dialog units to pixel units.
    """
    if y is None:
        return win.ConvertDialogPointToPixels(point_or_x)
    else:
        return win.ConvertDialogPointToPixels(wx.Point(point_or_x, y))

def DLG_SZE(win, size_width, height=None):
    """
    Convenience function for converting a Size or (w,h) in
    dialog units to pixel units.
    """
    if height is None:
        return win.ConvertDialogSizeToPixels(size_width)
    else:
        return win.ConvertDialogSizeToPixels(wx.Size(size_width, height))
}




// Unfortunatly the names of these new static methods clash with the
// names wxPython has been using forever for the overloaded
// wxWindow::FindWindow, so instead of swigging them as statics create
// standalone functions for them.


DocStr(wxFindWindowById,
"Find the first window in the application with the given id. If parent
is None, the search will start from all top-level frames and dialog
boxes; if non-None, the search will be limited to the given window
hierarchy. The search is recursive in both cases.", "");

DocStr(wxFindWindowByName,
"Find a window by its name (as given in a window constructor or Create
function call). If parent is None, the search will start from all
top-level frames and dialog boxes; if non-None, the search will be
limited to the given window hierarchy. The search is recursive in both
cases.

If no window with such name is found, wx.FindWindowByLabel is called.", "");

DocStr(wxFindWindowByLabel,
"Find a window by its label. Depending on the type of window, the label
may be a window title or panel item label. If parent is None, the
search will start from all top-level frames and dialog boxes; if
non-None, the search will be limited to the given window
hierarchy. The search is recursive in both cases.", "");


MustHaveApp(wxFindWindowById);
MustHaveApp(wxFindWindowByName);
MustHaveApp(wxFindWindowByLabel);

%inline %{
wxWindow* wxFindWindowById( long id, const wxWindow *parent = NULL ) {
    return wxWindow::FindWindowById(id, parent);
}

wxWindow* wxFindWindowByName( const wxString& name,
                              const wxWindow *parent = NULL ) {
     return wxWindow::FindWindowByName(name, parent);
}

wxWindow* wxFindWindowByLabel( const wxString& label,
                               const wxWindow *parent = NULL ) {
    return wxWindow::FindWindowByLabel(label, parent);
}
%}



%{
#ifdef __WXMSW__
#include <wx/msw/private.h>  // to get wxGetWindowId
#endif
%}

MustHaveApp(wxWindow_FromHWND);

%inline %{
    wxWindow* wxWindow_FromHWND(wxWindow* parent, unsigned long _hWnd) {
#ifdef __WXMSW__
        WXHWND hWnd = (WXHWND)_hWnd;
        long id = wxGetWindowId(hWnd);
        wxWindow* win = new wxWindow;
        if (parent)
            parent->AddChild(win);
        win->SetEventHandler(win);
        win->SetHWND(hWnd);
        win->SetId(id);
        win->SubclassWin(hWnd);
        win->AdoptAttributesFromHWND();
        win->SetupColours();
        return win;
#else
        wxPyRaiseNotImplemented();
        return NULL;
#endif
    }
%}

//---------------------------------------------------------------------------

DocStr(GetTopLevelWindows,
"Returns a list of the the application's top-level windows, (frames,
dialogs, etc.)  NOTE: Currently this is a copy of the list maintained
by wxWidgets, and so it is only valid as long as no top-level windows
are closed or new top-level windows are created.
", "");
%inline %{
    PyObject* GetTopLevelWindows() {
        return wxPy_ConvertList(&wxTopLevelWindows);
    }
%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

