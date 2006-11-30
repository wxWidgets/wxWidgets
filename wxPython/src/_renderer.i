/////////////////////////////////////////////////////////////////////////////
// Name:        _renderer.i
// Purpose:     SWIG interface for wxRendererNative
//
// Author:      Robin Dunn
//
// Created:     9-June-2005
// RCS-ID:      $Id$
// Copyright:   (c) 2005 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module

//---------------------------------------------------------------------------
%newgroup

%{
#include "wx/renderer.h"
%}


// control state flags used in wxRenderer and wxColourScheme
enum
{
    wxCONTROL_DISABLED   = 0x00000001,  // control is disabled
    wxCONTROL_FOCUSED    = 0x00000002,  // currently has keyboard focus
    wxCONTROL_PRESSED    = 0x00000004,  // (button) is pressed
    wxCONTROL_SPECIAL    = 0x00000008,  // control-specific bit:
    wxCONTROL_ISDEFAULT  = wxCONTROL_SPECIAL, // only for the buttons
    wxCONTROL_ISSUBMENU  = wxCONTROL_SPECIAL, // only for the menu items
    wxCONTROL_EXPANDED   = wxCONTROL_SPECIAL, // only for the tree items
    wxCONTROL_SIZEGRIP   = wxCONTROL_SPECIAL, // only for the status bar panes
    wxCONTROL_CURRENT    = 0x00000010,  // mouse is currently over the control
    wxCONTROL_SELECTED   = 0x00000020,  // selected item in e.g. listbox
    wxCONTROL_CHECKED    = 0x00000040,  // (check/radio button) is checked
    wxCONTROL_CHECKABLE  = 0x00000080,  // (menu) item can be checked
    wxCONTROL_UNDETERMINED = wxCONTROL_CHECKABLE, // (check) undetermined state

    wxCONTROL_FLAGS_MASK = 0x000000ff,

    // this is a pseudo flag not used directly by wxRenderer but rather by some
    // controls internally
    wxCONTROL_DIRTY      = 0x80000000
};



DocStr(wxSplitterRenderParams,
"This is just a simple struct used as a return value of
`wx.RendererNative.GetSplitterParams` and contains some platform
specific metrics about splitters.

    * widthSash: the width of the splitter sash.
    * border: the width of the border of the splitter window.
    * isHotSensitive: ``True`` if the splitter changes its
      appearance when the mouse is over it.

", "");

struct wxSplitterRenderParams
{
    wxSplitterRenderParams(wxCoord widthSash_, wxCoord border_, bool isSens_);
    ~wxSplitterRenderParams();
    
    // the width of the splitter sash
    const wxCoord widthSash;

    // the width of the border of the splitter window
    const wxCoord border;

    // true if the splitter changes its appearance when the mouse is over it
    const bool isHotSensitive;
};



DocStr(wxHeaderButtonParams,
"Extra (optional) parameters for `wx.RendererNative.DrawHeaderButton`", "");

struct wxHeaderButtonParams
{
    wxHeaderButtonParams();
    ~wxHeaderButtonParams();
    
    // So wxColour_helper will be used when assigning to the colour items in the struct
    %typemap(in) wxColour* (wxColour temp) {
        $1 = &temp;
        if ( ! wxColour_helper($input, &$1)) SWIG_fail;
    }
    wxColour    m_arrowColour;
    wxColour    m_selectionColour;
    wxString    m_labelText;
    wxFont      m_labelFont;
    wxColour    m_labelColour;
    wxBitmap    m_labelBitmap;
    int         m_labelAlignment;
};

enum wxHeaderSortIconType {
    wxHDR_SORT_ICON_NONE,        // Header button has no sort arrow
    wxHDR_SORT_ICON_UP,          // Header button an an up sort arrow icon
    wxHDR_SORT_ICON_DOWN         // Header button an a down sort arrow icon
};


DocStr(wxRendererVersion,
"This simple struct represents the `wx.RendererNative` interface
version and is only used as the return value of
`wx.RendererNative.GetVersion`.", "");

struct wxRendererVersion
{
    wxRendererVersion(int version_, int age_);
    ~wxRendererVersion();

    enum
    {
        Current_Version,
        Current_Age
    };


    // check if the given version is compatible with the current one
    static bool IsCompatible(const wxRendererVersion& ver);

    const int version;
    const int age;
};

//---------------------------------------------------------------------------


DocStr(wxRendererNative,
"One of the design principles of wxWidgets is to use the native
widgets on every platform in order to be as close as possible to
the native look and feel on every platform.  However there are
still cases when some generic widgets are needed for various
reasons, but it can sometimes take a lot of messy work to make
them conform to the native LnF.

The wx.RendererNative class is a collection of functions that have
platform-specific implementations for drawing certain parts of
genereic controls in ways that are as close to the native look as
possible.

Note that each drawing function restores the `wx.DC` attributes if it
changes them, so it is safe to assume that the same pen, brush and
colours that were active before the call to this function are still in
effect after it.
", "");

class wxRendererNative
{
public:

     
    DocDeclStr(
        virtual int , DrawHeaderButton(wxWindow *win,
                                       wxDC& dc,
                                       const wxRect& rect,
                                       int flags = 0,
                                       wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE,
                                       wxHeaderButtonParams* params=NULL),
        "Draw a header control button (such as what is used by `wx.ListCtrl` in report
mode.)  The optimal size of the label (text and icons) is returned.", "");
    

    DocDeclStr(
        virtual int , DrawHeaderButtonContents(wxWindow *win,
                                               wxDC& dc,
                                               const wxRect& rect,
                                               int flags = 0,
                                               wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE,
                                               wxHeaderButtonParams* params=NULL),
        "Draw the contents of a header control button, (label, sort
arrows, etc.)  Normally this is only called by `DrawHeaderButton`.", "");

    DocDeclStr(
        virtual int , GetHeaderButtonHeight(wxWindow *win),
        "Returns the default height of a header button, either a fixed platform
height if available, or a generic height based on the window's font.", "");    


    DocDeclStr(
        virtual void , DrawTreeItemButton(wxWindow *win,
                                          wxDC& dc,
                                          const wxRect& rect,
                                          int flags = 0),
        "Draw the expanded/collapsed icon for a tree control item.", "");
    

    DocDeclStr(
        virtual void , DrawSplitterBorder(wxWindow *win,
                                          wxDC& dc,
                                          const wxRect& rect,
                                          int flags = 0),
        "Draw the border for a sash window: this border must be such that the
sash drawn by `DrawSplitterSash` blends into it well.", "");
    

    DocDeclStr(
        virtual void , DrawSplitterSash(wxWindow *win,
                                        wxDC& dc,
                                        const wxSize& size,
                                        wxCoord position,
                                        wxOrientation orient,
                                        int flags = 0),
        "Draw a sash. The orient parameter defines whether the sash should be
vertical or horizontal and how the position should be interpreted.", "");
    

    DocDeclStr(
        virtual void , DrawComboBoxDropButton(wxWindow *win,
                                              wxDC& dc,
                                              const wxRect& rect,
                                              int flags = 0),
        "Draw a button like the one used by `wx.ComboBox` to show a drop down
window. The usual appearance is a downwards pointing arrow.

The ``flags`` parameter may have the ``wx.CONTROL_PRESSED`` or
``wx.CONTROL_CURRENT`` bits set.", "");
    

    DocDeclStr(
        virtual void , DrawDropArrow(wxWindow *win,
                                     wxDC& dc,
                                     const wxRect& rect,
                                     int flags = 0),
        "Draw a drop down arrow that is suitable for use outside a combo
box. Arrow will have a transparent background.

``rect`` is not entirely filled by the arrow. Instead, you should use
bounding rectangle of a drop down button which arrow matches the size
you need. ``flags`` may have the ``wx.CONTROL_PRESSED`` or
``wx.CONTROL_CURRENT`` bit set.", "");
    

    DocDeclStr(
        virtual void , DrawCheckBox(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0),
        "Draw a check button.  Flags may use wx.CONTROL_CHECKED,
wx.CONTROL_UNDETERMINED and wx.CONTROL_CURRENT.", "");
    

    DocDeclStr(
        virtual void , DrawPushButton(wxWindow *win,
                                      wxDC& dc,
                                      const wxRect& rect,
                                      int flags = 0),
        "Draw a blank button.  Flags may be wx.CONTROL_PRESSED, wx.CONTROL_CURRENT and
wx.CONTROL_ISDEFAULT", "");
    

    DocDeclStr(
        virtual void , DrawItemSelectionRect(wxWindow *win,
                                             wxDC& dc,
                                             const wxRect& rect,
                                             int flags = 0),
        "Draw rectangle indicating that an item in e.g. a list control has been
selected or focused

The flags parameter may be:

    ====================  ============================================
    wx.CONTROL_SELECTED   item is selected, e.g. draw background
    wx.CONTROL_CURRENT    item is the current item, e.g. dotted border
    wx.CONTROL_FOCUSED    the whole control has focus, e.g. blue
                          background vs. grey otherwise
    ====================  ============================================
", "");
    
    
    DocDeclStr(
        virtual wxSplitterRenderParams , GetSplitterParams(const wxWindow *win),
        "Get the splitter parameters, see `wx.SplitterRenderParams`.", "");
    


    MustHaveApp(Get);
    DocDeclStr(
        static wxRendererNative& , Get(),
        "Return the currently used renderer", "");
    

    MustHaveApp(GetGeneric);
    DocDeclStr(
        static wxRendererNative& , GetGeneric(),
        "Return the generic implementation of the renderer. Under some
platforms, this is the default renderer implementation, others have
platform-specific default renderer which can be retrieved by calling
`wx.RendererNative.GetDefault`.", "");
    

    MustHaveApp(GetDefault);
    DocDeclStr(
        static wxRendererNative& , GetDefault(),
        "Return the default (native) implementation for this platform -- this
is also the one used by default but this may be changed by calling
`wx.RendererNative.Set` in which case the return value of this method
may be different from the return value of `wx.RendererNative.Get`.", "");
    



//     // load the renderer from the specified DLL, the returned pointer must be
//     // deleted by caller if not NULL when it is not used any more
//     static wxRendererNative *Load(const wxString& name);


    MustHaveApp(Set);
    DocDeclStr(
        static wxRendererNative *, Set(wxRendererNative *renderer),
        "Set the renderer to use, passing None reverts to using the default
renderer.  Returns the previous renderer used with Set or None.", "");
    


    DocDeclStr(
        virtual wxRendererVersion , GetVersion() const,
        "Returns the version of the renderer.  Will be used for ensuring
compatibility of dynamically loaded renderers.", "");

    
    %property(SplitterParams, GetSplitterParams, doc="See `GetSplitterParams`");
    %property(Version, GetVersion, doc="See `GetVersion`");
};


//---------------------------------------------------------------------------
