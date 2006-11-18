/////////////////////////////////////////////////////////////////////////////
// Name:        combo.i
// Purpose:     SWIG interface for the owner-drawn combobox classes
//
// Author:      Robin Dunn
//
// Created:     11-Nov-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
"ComboCtrl class that can have any type of popup widget, and also an
owner-drawn combobox control."
%enddef

%module(package="wx", docstring=DOCSTRING) combo

%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"

#include <wx/combo.h>
#include <wx/odcombo.h>
%}

//---------------------------------------------------------------------------

%import windows.i
%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }

//---------------------------------------------------------------------------
%newgroup

MAKE_CONST_WXSTRING_NOSWIG(ComboBoxNameStr);
MAKE_CONST_WXSTRING_NOSWIG(EmptyString);

%{
    const wxArrayString wxPyEmptyStringArray;
%}


enum {
    // Button is preferred outside the border (GTK style)
    wxCC_BUTTON_OUTSIDE_BORDER      = 0x0001,
    // Show popup on mouse up instead of mouse down (which is the Windows style)
    wxCC_POPUP_ON_MOUSE_UP          = 0x0002,
    // All text is not automatically selected on click
    wxCC_NO_TEXT_AUTO_SELECT        = 0x0004,
};


// Flags used by PreprocessMouseEvent and HandleButtonMouseEvent
enum
{
    wxCC_MF_ON_BUTTON               =   0x0001, // cursor is on dropbutton area
    wxCC_MF_ON_CLICK_AREA           =   0x0002  // cursor is on dropbutton or other area
                                                // that can be clicked to show the popup.
};


DocStr( wxComboCtrlFeatures,
        "Namespace for `wx.combo.ComboCtrl` feature flags.  See
`wx.combo.ComboCtrl.GetFeatures`.", "");
struct wxComboCtrlFeatures
{
    enum
    {
        MovableButton       = 0x0001, // Button can be on either side of control
        BitmapButton        = 0x0002, // Button may be replaced with bitmap
        ButtonSpacing       = 0x0004, // Button can have spacing from the edge
                                      // of the control
        TextIndent          = 0x0008, // SetTextIndent can be used
        PaintControl        = 0x0010, // Combo control itself can be custom painted
        PaintWritable       = 0x0020, // A variable-width area in front of writable
                                      // combo control's textctrl can be custom
                                      // painted
        Borderless          = 0x0040, // wxNO_BORDER window style works

        // There are no feature flags for...
        // PushButtonBitmapBackground - if its in wxRendererNative, then it should be
        //   not an issue to have it automatically under the bitmap.

        All                 = MovableButton|BitmapButton|
                              ButtonSpacing|TextIndent|
                              PaintControl|PaintWritable|
                              Borderless
    };
};

//---------------------------------------------------------------------------

// C++ implemetation of Python aware wxComboCtrl
%{
class wxPyComboCtrl : public wxComboCtrl
{
    DECLARE_ABSTRACT_CLASS(wxPyComboCtrl)
public:
    wxPyComboCtrl() : wxComboCtrl() {}
    wxPyComboCtrl(wxWindow *parent,
                  wxWindowID id = wxID_ANY,
                  const wxString& value = wxEmptyString,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxPyComboBoxNameStr)
        : wxComboCtrl(parent, id, value, pos, size, style, validator, name)
    {}

    void DoSetPopupControl(wxComboPopup* popup)
    {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DoSetPopupControl"))) {
            PyObject* obj = wxPyConstructObject(popup, wxT("wxComboPopup"), false);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)",obj));
            Py_DECREF(obj);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxComboCtrl::DoSetPopupControl(popup);
    }

    virtual bool IsKeyPopupToggle( const wxKeyEvent& event ) const
    {
        bool found;
        bool rval = false;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "OnComboKeyEvent"))) {
            PyObject* oevt = wxPyConstructObject((void*)&event, wxT("wxKeyEvent"), 0);
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", oevt));
            Py_DECREF(oevt);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            rval = wxComboCtrl::IsKeyPopupToggle(event);
        return rval;
    }


    virtual wxWindow *GetMainWindowOfCompositeControl()
    {
        return wxComboCtrl::GetMainWindowOfCompositeControl();
    }

    
    enum
    {
        ShowBelow       = 0x0000,  // Showing popup below the control
        ShowAbove       = 0x0001,  // Showing popup above the control
        CanDeferShow    = 0x0002  // Can only return true from AnimateShow if this is set
    };


    DEC_PYCALLBACK_VOID_(ShowPopup);
    DEC_PYCALLBACK_VOID_(HidePopup);
    DEC_PYCALLBACK_VOID_(OnButtonClick);
    DEC_PYCALLBACK__RECTINT(DoShowPopup);
    DEC_PYCALLBACK_BOOL_RECTINT(AnimateShow);

    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyComboCtrl, wxComboCtrl);

IMP_PYCALLBACK_VOID_(wxPyComboCtrl, wxComboCtrl, ShowPopup);
IMP_PYCALLBACK_VOID_(wxPyComboCtrl, wxComboCtrl, HidePopup);
IMP_PYCALLBACK_VOID_(wxPyComboCtrl, wxComboCtrl, OnButtonClick);
IMP_PYCALLBACK__RECTINT(wxPyComboCtrl, wxComboCtrl, DoShowPopup);
IMP_PYCALLBACK_BOOL_RECTINT(wxPyComboCtrl, wxComboCtrl, AnimateShow);

%}



// Now declare wxPyComboCtrl for Python

DocStr(wxPyComboCtrl,
"A combo control is a generic combobox that allows for a totally custom
popup. In addition it has other customization features. For instance,
position and size of the dropdown button can be changed.

To specify what to use for the popup control you need to derive a
class from `wx.combo.ComboPopup` and pass it to the ComboCtrl with
`SetPopupControl`.  It doesn't derive from any widget class so it can
be used either as a mixin class combined with some standard or custom
widget, or you can use the derived ComboPopup to create and hold an
independent reference to the widget to be used for the popup.
", "
Window Styles
-------------
    ====================   ============================================
    wx.CB_READONLY         Text will not be editable.
    wx.CB_SORT             Sorts the entries in the list alphabetically.
    wx.TE_PROCESS_ENTER    The control will generate the event
                           EVT_TEXT_ENTER (otherwise pressing Enter key
                           is either processed internally by the control
                           or used for navigation between dialog controls).
    wx.CC_SPECIAL_DCLICK   Double-clicking triggers a call to popup's
                           OnComboDoubleClick. Actual behaviour is defined
                           by a derived class. For instance,
                           OwnerDrawnComboBox will cycle an item. This
                           style only applies if wx.CB_READONLY is used
                           as well.
    wx.CC_STD_BUTTON       Drop button will behave more like a standard
                           push button.
    ====================   ============================================
");

MustHaveApp(wxPyComboCtrl);
%rename(ComboCtrl) wxPyComboCtrl;

class wxPyComboCtrl : public wxControl
{
public:
    %pythonAppend wxPyComboCtrl      "self._setOORInfo(self);" setCallbackInfo(ComboCtrl)
    %pythonAppend wxPyComboCtrl()    "";

    DocCtorStr(
        wxPyComboCtrl(wxWindow *parent,
                      wxWindowID id = wxID_ANY,
                      const wxString& value = wxEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxPyComboBoxNameStr),
        "", "");

    DocCtorStrName(
        wxPyComboCtrl(),
        "", "",
        PreComboCtrl);


    void _setCallbackInfo(PyObject* self, PyObject* _class);

    DocDeclStr(
        virtual void , ShowPopup(),
        "Show the popup window.", "");

    DocDeclStr(
        virtual void , HidePopup(),
        "Dismisses the popup window.", "");


    // Override for totally custom combo action
    DocDeclStr(
        virtual void , OnButtonClick(),
        "Implement in a derived class to define what happens on dropdown button
click.  Default action is to show the popup. ", "");


    // return true if the popup is currently shown
    DocDeclStr(
        bool , IsPopupShown() const,
        "Returns true if the popup is currently shown.", "");


    %disownarg(wxPyComboPopup* popup);
    DocDeclStr(
        void , SetPopupControl( wxPyComboPopup* popup ),
        "Set popup interface class derived from `wx.combo.ComboPopup`. This
method should be called as soon as possible after the control has been
created, unless `OnButtonClick` has been overridden.", "");
    %cleardisown(wxPyComboPopup* popup);


    DocDeclStr(
        wxPyComboPopup* , GetPopupControl(),
        "Returns the current popup interface that has been set with
`SetPopupControl`.", "");


    DocDeclStr(
        wxWindow *, GetPopupWindow() const,
        "Returns the popup window containing the popup control.", "");


    DocDeclStr(
        wxTextCtrl *, GetTextCtrl() const,
        "Get the text control which is part of the combo control.", "");


   DocDeclStr(
        wxWindow *, GetButton() const,
        "Get the dropdown button which is part of the combobox.  Note: it's not
necessarily a wx.Button or wx.BitmapButton.", "");


//     // forward these methods to all subcontrols
//     virtual bool Enable(bool enable = true);
//     virtual bool Show(bool show = true);
//     virtual bool SetFont(const wxFont& font);

    // wxTextCtrl methods - for readonly combo they should return
    // without errors.

    DocDeclStr(
        virtual wxString , GetValue() const,
        "Returns text representation of the current value. For writable combo
control it always returns the value in the text field.", "");

    DocDeclStr(
        virtual void , SetValue(const wxString& value),
        "Sets the text for the combo control text field.  For a combo control
with wx.CB_READONLY style the string must be accepted by the popup (for
instance, exist in the dropdown list), otherwise the call to
SetValue is ignored.", "");

    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual long GetLastPosition() const;
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);
    virtual void Undo();

    %Rename(SetMark, void , SetSelection(long from, long to));


    DocDeclStr(
        void , SetText(const wxString& value),
        "Sets the text for the text field without affecting the popup. Thus,
unlike `SetValue`, it works equally well with combo control using
wx.CB_READONLY style.", "");


    DocDeclStr(
        void , SetValueWithEvent(const wxString& value, bool withEvent = true),
        "Same as `SetValue`, but also sends a EVT_TEXT event if withEvent is true.", "");


    //
    // Popup customization methods
    //

    DocDeclStr(
        void , SetPopupMinWidth( int width ),
        "Sets minimum width of the popup. If wider than combo control, it will
extend to the left.  A value of -1 indicates to use the default.  The
popup implementation may choose to ignore this.", "");


    DocDeclStr(
        void , SetPopupMaxHeight( int height ),
        "Sets preferred maximum height of the popup. A value of -1 indicates to
use the default.  The popup implementation may choose to ignore this.", "");


    DocDeclStr(
        void , SetPopupExtents( int extLeft, int extRight ),
        "Extends popup size horizontally, relative to the edges of the combo
control.  Values are given in pixels, and the defaults are zero.  It
is up to the popup to fully take these values into account.", "");


    DocDeclStr(
        void , SetCustomPaintWidth( int width ),
        "Set width, in pixels, of custom painted area in control without
wx.CB_READONLY style. In read-only OwnerDrawnComboBox, this is used
to indicate the area that is not covered by the focus rectangle.", "");

    int GetCustomPaintWidth() const;


    DocDeclStr(
        void , SetPopupAnchor( int anchorSide ),
        "Set side of the control to which the popup will align itself. Valid
values are wx.LEFT, wx.RIGHT and 0. The default value 0 means that the
most appropriate side is used (which, currently, is always wx.LEFT).", "");


    DocDeclStr(
        void , SetButtonPosition( int width = -1,
                                  int height = -1,
                                  int side = wxRIGHT,
                                  int spacingX = 0 ),
        "Set the position of the dropdown button.", "");


    DocDeclStr(
        wxSize , GetButtonSize(),
        "Returns current size of the dropdown button.", "");


    DocDeclStr(
        void , SetButtonBitmaps( const wxBitmap& bmpNormal,
                                 bool pushButtonBg = false,
                                 const wxBitmap& bmpPressed = wxNullBitmap,
                                 const wxBitmap& bmpHover = wxNullBitmap,
                                 const wxBitmap& bmpDisabled = wxNullBitmap ),
        "Sets custom dropdown button graphics.

    :param bmpNormal:  Default button image
    :param pushButtonBg: If ``True``, blank push button background is painted below the image.
    :param bmpPressed:  Depressed butotn image.
    :param bmpHover:  Button imate to use when the mouse hovers over it.
    :param bmpDisabled: Disabled button image.
", "");


    DocDeclStr(
        void , SetTextIndent( int indent ),
        "This will set the space in pixels between left edge of the control and
the text, regardless whether control is read-only or not. A value of -1 can
be given to indicate platform default.", "");


    DocDeclStr(
        wxCoord , GetTextIndent() const,
        "Returns actual indentation in pixels.", "");


    DocDeclStr(
        const wxRect& , GetTextRect() const,
        "Returns area covered by the text field (includes everything except
borders and the dropdown button).", "");


    DocDeclStr(
        void , UseAltPopupWindow( bool enable = true ),
        "Enable or disable usage of an alternative popup window, which
guarantees ability to focus the popup control, and allows common
native controls to function normally. This alternative popup window is
usually a wxDialog, and as such, when it is shown, its parent
top-level window will appear as if the focus has been lost from it.", "");


    DocDeclStr(
        void , EnablePopupAnimation( bool enable = true ),
        "Enables or disables popup animation, if any, depending on the value of
the argument.", "");


    //
    // Utilies needed by the popups or native implementations
    //

    DocDeclStr(
        virtual bool , IsKeyPopupToggle(const wxKeyEvent& event) const,
        "Returns true if given key combination should toggle the popup.", "");


    // Prepare background of combo control or an item in a dropdown list
    // in a way typical on platform. This includes painting the focus/disabled
    // background and setting the clipping region.
    // Unless you plan to paint your own focus indicator, you should always call this
    // in your wxComboPopup::PaintComboControl implementation.
    // In addition, it sets pen and text colour to what looks good and proper
    // against the background.
    // flags: wxRendererNative flags: wxCONTROL_ISSUBMENU: is drawing a list item instead of combo control
    //                                wxCONTROL_SELECTED: list item is selected
    //                                wxCONTROL_DISABLED: control/item is disabled
    DocDeclStr(
        virtual void , PrepareBackground( wxDC& dc, const wxRect& rect, int flags ) const,
        "Prepare background of combo control or an item in a dropdown list in a
way typical on platform. This includes painting the focus/disabled
background and setting the clipping region.  Unless you plan to paint
your own focus indicator, you should always call this in your
wxComboPopup::PaintComboControl implementation.  In addition, it sets
pen and text colour to what looks good and proper against the
background.

flags are the same as wx.RendererNative flags:

    ======================   ============================================
    wx.CONTROL_ISSUBMENU     drawing a list item instead of combo control
    wx.CONTROL_SELECTED      list item is selected
    wx.CONTROL_DISABLED       control/item is disabled
    ======================   ============================================
", "");



    DocDeclStr(
        bool , ShouldDrawFocus() const,
        "Returns true if focus indicator should be drawn in the control.", "");


    const wxBitmap& GetBitmapNormal() const;
    const wxBitmap& GetBitmapPressed() const;
    const wxBitmap& GetBitmapHover() const;
    const wxBitmap& GetBitmapDisabled() const;

    wxUint32 GetInternalFlags() const;

    DocDeclStr(
        bool , IsCreated() const,
        "Return true if Create has finished", "");


    DocDeclStr(
        void , OnPopupDismiss(),
        "Common code to be called on popup hide/dismiss", "");


    // PopupShown states
    enum
    {
        Hidden       = 0,
        //Closing      = 1,
        Animating    = 2,
        Visible      = 3
    };

    bool IsPopupWindowState( int state ) const;

    int GetPopupWindowState() const;

    // Set value returned by GetMainWindowOfCompositeControl
    void SetCtrlMainWnd( wxWindow* wnd );
    virtual wxWindow *GetMainWindowOfCompositeControl();

    DocDeclStr(
        static int , GetFeatures(),
        "Returns a bit-list of flags indicating which features of the ComboCtrl
functionality are implemented by this implemetation.  See
`wx.combo.ComboCtrlFeatures`.", "");



    // Flags for DoShowPopup and AnimateShow
    enum
    {
        ShowBelow       = 0x0000,  // Showing popup below the control
        ShowAbove       = 0x0001,  // Showing popup above the control
        CanDeferShow    = 0x0002  // Can only return true from AnimateShow if this is set
    };

    // Shows and positions the popup.
    DocDeclStr(
        virtual void , DoShowPopup( const wxRect& rect, int flags ),
        "Shows and positions the popup.

Flags:
    ============  =====================================================
    ShowBelow     Showing popup below the control
    ShowAbove     Showing popup above the control
    CanDeferShow  Can only return true from AnimateShow if this is set
    ============  =====================================================
", "");



    DocDeclStr(
        virtual bool , AnimateShow( const wxRect& rect, int flags ),
        "Implement in derived class to create a drop-down animation.  Return
``True`` if finished immediately. Otherwise the popup is only shown when the
derived class calls `DoShowPopup`.  Flags are same as for `DoShowPopup`.
", "");


    %property(PopupControl, GetPopupControl, SetPopupControl);
    %property(PopupWindow, GetPopupWindow);
    %property(TextCtrl, GetTextCtrl);
    %property(Button, GetButton);
    %property(Value, GetValue, SetValue);
    %property(InsertionPoint, GetInsertionPoint);
    %property(CustomPaintWidth, GetCustomPaintWidth, SetCustomPaintWidth);
    %property(ButtonSize, GetButtonSize);
    %property(TextIndent, GetTextIndent, SetTextIndent);
    %property(TextRect, GetTextRect);
    %property(BitmapNormal, GetBitmapNormal);
    %property(BitmapPressed, GetBitmapPressed);
    %property(BitmapHover, GetBitmapHover);
    %property(BitmapDisabled, GetBitmapDisabled);
    %property(PopupWindowState, GetPopupWindowState);

};

//---------------------------------------------------------------------------
%newgroup


// C++ implemetation of Python aware wxComboCtrl
%{
class wxPyComboPopup : public wxComboPopup
{
public:
    wxPyComboPopup() : wxComboPopup() {}
    ~wxPyComboPopup() {}


    DEC_PYCALLBACK_VOID_(Init);
    DEC_PYCALLBACK_BOOL_WXWIN_pure(Create);
    DEC_PYCALLBACK_VOID_(OnPopup);
    DEC_PYCALLBACK_VOID_(OnDismiss);
    DEC_PYCALLBACK__STRING(SetStringValue);
    DEC_PYCALLBACK_STRING__constpure(GetStringValue);
    DEC_PYCALLBACK_VOID_(OnComboDoubleClick);
    DEC_PYCALLBACK_BOOL_(LazyCreate);

    virtual wxWindow *GetControl()
    {
        wxWindow* rval = NULL;
        const char* errmsg = "GetControl should return an object derived from wx.Window.";
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "GetControl")) {
            PyObject* ro;
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));
            if (ro) {
                if (!wxPyConvertSwigPtr(ro, (void**)&rval, wxT("wxWindow")))
                    PyErr_SetString(PyExc_TypeError, errmsg);
                Py_DECREF(ro);
            }
        }
        else
            PyErr_SetString(PyExc_TypeError, errmsg);
        wxPyEndBlockThreads(blocked);
        return rval;
    }


    virtual void PaintComboControl( wxDC& dc, const wxRect& rect )
    {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "PaintComboControl"))) {
            PyObject* odc = wxPyMake_wxObject(&dc,false);
            PyObject* orect = wxPyConstructObject((void*)&rect, wxT("wxRect"), 0);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OO)", odc, orect));
            Py_DECREF(odc);
            Py_DECREF(orect);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxComboPopup::PaintComboControl(dc, rect);
    }


    virtual void OnComboKeyEvent( wxKeyEvent& event )
    {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "OnComboKeyEvent"))) {
            PyObject* oevt = wxPyConstructObject((void*)&event, wxT("wxKeyEvent"), 0);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", oevt));
            Py_DECREF(oevt);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxComboPopup::OnComboKeyEvent(event);
    }


    virtual wxSize GetAdjustedSize( int minWidth, int prefHeight, int maxHeight )
    {
        const char* errmsg = "GetAdjustedSize should return a wx.Size or a 2-tuple of integers.";
        bool found;
        wxSize rval(0,0);
        wxSize* rptr = &rval;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "GetAdjustedSize"))) {
            PyObject* ro;
            ro = wxPyCBH_callCallbackObj(
                m_myInst, Py_BuildValue("(iii)", minWidth, prefHeight, maxHeight));
            if (ro) {
                if (! wxSize_helper(ro, &rptr))
                    PyErr_SetString(PyExc_TypeError, errmsg);
                else
                    rval = *rptr;
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            rval = wxComboPopup::GetAdjustedSize(minWidth, prefHeight, maxHeight);
        return rval;
    }

    wxComboCtrl* GetCombo() { return (wxComboCtrl*)m_combo; }

    PYPRIVATE;
};


IMP_PYCALLBACK_VOID_(wxPyComboPopup, wxComboPopup, Init);
IMP_PYCALLBACK_BOOL_WXWIN_pure(wxPyComboPopup, wxComboPopup, Create);
IMP_PYCALLBACK_VOID_(wxPyComboPopup, wxComboPopup, OnPopup);
IMP_PYCALLBACK_VOID_(wxPyComboPopup, wxComboPopup, OnDismiss);
IMP_PYCALLBACK__STRING(wxPyComboPopup, wxComboPopup, SetStringValue);
IMP_PYCALLBACK_STRING__constpure(wxPyComboPopup, wxComboPopup, GetStringValue);
IMP_PYCALLBACK_VOID_(wxPyComboPopup, wxComboPopup, OnComboDoubleClick);
IMP_PYCALLBACK_BOOL_(wxPyComboPopup, wxComboPopup, LazyCreate);

%}



// Now declare wxPyComboPopup for Python
DocStr(wxPyComboPopup,
"In order to use a custom popup with `wx.combo.ComboCtrl` an interface
class derived from wx.combo.ComboPopup is used to manage the interface
between the popup control and the popup.  You can either derive a new
class from both the widget class and this ComboPopup class, or the
derived class can have a reference to the widget used for the popup.
In either case you simply need to return the widget from the
`GetControl` method to allow the ComboCtrl to interact with it.

Nearly all of the methods of this class are overridable in Python.", "");


MustHaveApp(wxPyComboPopup);
%rename(ComboPopup) wxPyComboPopup;

class wxPyComboPopup
{
public:
    %pythonAppend wxPyComboPopup  setCallbackInfo(ComboPopup);

    DocCtorStr(
        wxPyComboPopup(),
        "Constructor", "");

    DocCtorStr(
        ~wxPyComboPopup(),
        "Destructor", "");


    void _setCallbackInfo(PyObject* self, PyObject* _class);

    DocDeclStr(
        virtual void , Init(),
        "This method is called after the popup is contructed and has been
assigned to the ComboCtrl.  Derived classes can override this to do
extra inialization or whatever.", "");


    // Create the popup child control.
    // Return true for success.
    DocDeclStr(
        virtual bool , Create(wxWindow* parent),
        "The derived class must implement this method to create the popup
control.  It should be a child of the ``parent`` passed in, but other
than that there is much flexibility in what the widget can be, its
style, etc.  Return ``True`` for success, ``False`` otherwise.  (NOTE:
this return value is not currently checked...)", "");


    DocDeclStr(
        virtual wxWindow *, GetControl(),
        "The derived class must implement this method and it should return a
reference to the widget created in the `Create` method.  If the
derived class inherits from both the widget class and ComboPopup then
the return value is probably just ``self``.", "");


    DocDeclStr(
        virtual void , OnPopup(),
        "The derived class may implement this to do special processing when
popup is shown.", "");


    DocDeclStr(
        virtual void , OnDismiss(),
        "The derived class may implement this to do special processing when
popup is hidden.", "");


    DocDeclStr(
        virtual void , SetStringValue( const wxString& value ),
        "Called just prior to displaying the popup.  The derived class can
implement this to \"select\" the item in the popup that coresponds to
the passed in string value, if appropriate.  The default
implementation does nothing.", "");


    DocDeclStr(
        virtual wxString , GetStringValue() const,
        "Gets the string representation of the currently selected value to be
used to display in the combo widget.", "");


    DocDeclStr(
        virtual void , PaintComboControl( wxDC& dc, const wxRect& rect ),
        "This is called to custom paint in the combo control itself (ie. not
the popup).  Default implementation draws the current value as string.", "");


    DocDeclStr(
        virtual void , OnComboKeyEvent( wxKeyEvent& event ),
        "Receives key events from the parent ComboCtrl.  Events not handled
should be skipped, as usual.", "");


    DocDeclStr(
        virtual void , OnComboDoubleClick(),
        "Implement this method in the derived class if you need to support
special actions when the user double-clicks on the parent ComboCtrl.", "");


    DocDeclStr(
        virtual wxSize , GetAdjustedSize( int minWidth, int prefHeight, int maxHeight ),
        "The derived class may implement this method to return adjusted size
for the popup control, according to the variables given.  It is called
on every popup, just prior to `OnPopup`.

    :param minWidth:    Preferred minimum width.
    :param prefHeight:  Preferred height. May be -1 to indicate no preference.
    :maxWidth:          Max height for window, as limited by screen size, and
                        should only be rounded down, if necessary.
", "");


    DocDeclStr(
        virtual bool , LazyCreate(),
        "The derived class may implement this to return ``True`` if it wants to
delay the call to `Create` until the popup is shown for the first
time. It is more efficient, but on the other hand it is often more
convenient to have the control created immediately.  The default
implementation returns ``False``.", "");


    //
    // Utilies
    //


    DocDeclStr(
        void , Dismiss(),
        "Hides the popup", "");


    DocDeclStr(
        bool , IsCreated() const,
        "Returns true if `Create` has been called.", "");


    DocDeclStr(
        static void , DefaultPaintComboControl( wxComboCtrlBase* combo,
                                                wxDC& dc,
                                                const wxRect& rect ),
        "Default PaintComboControl behaviour", "");


    DocDeclStr(
        wxPyComboCtrl* , GetCombo(),
        "Returns a reference to the `wx.combo.ComboCtrl` this ComboPopup object
is associated with.", "");

};



//---------------------------------------------------------------------------
%newgroup


enum {
    wxODCB_DCLICK_CYCLES,
    wxODCB_STD_CONTROL_PAINT,
    wxODCB_PAINTING_CONTROL,
    wxODCB_PAINTING_SELECTED
};



// C++ implemetation of Python aware wxOwnerDrawnComboBox
%{
class wxPyOwnerDrawnComboBox : public wxOwnerDrawnComboBox
{
public:
    wxPyOwnerDrawnComboBox() : wxOwnerDrawnComboBox() {}
    wxPyOwnerDrawnComboBox(wxWindow *parent,
                           wxWindowID id,
                           const wxString& value,
                           const wxPoint& pos,
                           const wxSize& size,
                           const wxArrayString& choices,
                           long style,
                           const wxValidator& validator = wxDefaultValidator,
                           const wxString& name = wxPyComboBoxNameStr)
        : wxOwnerDrawnComboBox(parent, id, value, pos, size, choices, style,
                               validator, name)
    {}

    DEC_PYCALLBACK__DCRECTINTINT_const(OnDrawItem);
    DEC_PYCALLBACK_COORD_SIZET_const(OnMeasureItem);
    DEC_PYCALLBACK_COORD_SIZET_const(OnMeasureItemWidth);
    DEC_PYCALLBACK__DCRECTINTINT_const(OnDrawBackground);


    PYPRIVATE;
};

IMP_PYCALLBACK__DCRECTINTINT_const(wxPyOwnerDrawnComboBox, wxOwnerDrawnComboBox, OnDrawItem);
IMP_PYCALLBACK_COORD_SIZET_const(wxPyOwnerDrawnComboBox, wxOwnerDrawnComboBox, OnMeasureItem);
IMP_PYCALLBACK_COORD_SIZET_const(wxPyOwnerDrawnComboBox, wxOwnerDrawnComboBox, OnMeasureItemWidth);
IMP_PYCALLBACK__DCRECTINTINT_const(wxPyOwnerDrawnComboBox, wxOwnerDrawnComboBox, OnDrawBackground);

%}



// Now declare wxPyOwnerDrawnComboBox for Python

DocStr(wxPyOwnerDrawnComboBox,
"wx.combo.OwnerDrawnComboBox is a combobox with owner-drawn list
items. In essence, it is a `wx.combo.ComboCtrl` with a `wx.VListBox`
popup and a `wx.ControlWithItems` API.

Implementing item drawing and measuring is similar to wx.VListBox.
The application needs to subclass wx.combo.OwnerDrawnComboBox and
implement the `OnDrawItem`, `OnMeasureItem` and `OnMeasureItemWidth`
methods.", "");

MustHaveApp(wxPyOwnerDrawnComboBox);
%rename(OwnerDrawnComboBox) wxPyOwnerDrawnComboBox;

class wxPyOwnerDrawnComboBox : public wxPyComboCtrl,
                               public wxItemContainer
{
public:
    %pythonAppend wxPyOwnerDrawnComboBox      "self._setOORInfo(self);" setCallbackInfo(OwnerDrawnComboBox)
    %pythonAppend wxPyOwnerDrawnComboBox()    "";

    DocCtorStr(
        wxPyOwnerDrawnComboBox(wxWindow *parent,
                               wxWindowID id = -1,
                               const wxString& value = wxPyEmptyString,
                               const wxPoint& pos = wxDefaultPosition,
                               const wxSize& size = wxDefaultSize,
                               const wxArrayString& choices = wxPyEmptyStringArray,
                               long style = 0,
                               const wxValidator& validator = wxDefaultValidator,
                               const wxString& name = wxPyComboBoxNameStr),
        "Standard constructor.", "");

    DocCtorStrName(wxPyOwnerDrawnComboBox(),
                   "2-phase create constructor.", "",
                   PreOwnerDrawnComboBox);

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    
    DocDeclStr(        
        bool , Create(wxWindow *parent,
                      wxWindowID id = -1,
                      const wxString& value = wxPyEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      const wxArrayString& choices = wxPyEmptyStringArray,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxPyComboBoxNameStr),
        "Create the UI object, and other initialization.", "");
    

    DocDeclStr(
        virtual int , GetWidestItemWidth(),
        "Return the widest item width (recalculating it if necessary.)", "");
    

    DocDeclStr(
        virtual int , GetWidestItem(),
        "Return the index of the widest item (recalculating it if necessary.)", "");
    

    void SetSelection(int n);
    %Rename(SetMark, void , SetSelection(long from, long to));


    // Callback for drawing. Font, background and text colour have been
    // prepared according to selection, focus and such.
    // item: item index to be drawn, may be wxNOT_FOUND when painting combo control itself
    //       and there is no valid selection
    // flags: wxODCB_PAINTING_CONTROL is set if painting to combo control instead of list
    DocDeclStr(
        virtual void , OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const,
        "The derived class may implement this function to actually draw the
item with the given index on the provided DC. If this method is not
overridden, the item text is simply drawn as if the control was a
normal combobox.

   :param dc:    The device context to use for drawing.
   :param rect:  The bounding rectangle for the item being drawn, the
                 DC's clipping region is set to this rectangle before
                 calling this method.
   :param item:  The index of the item to be drawn.

   :param flags: ``wx.combo.ODCB_PAINTING_CONTROL`` (The Combo control itself
                  is being painted, instead of a list item.  The ``item``
                  parameter may be ``wx.NOT_FOUND`` in this case.
                  ``wx.combo.ODCB_PAINTING_SELECTED``  (An item with
                  selection background is being painted. The DC's text colour
                  should already be correct.
", "");
    

    DocDeclStr(
        virtual wxCoord , OnMeasureItem( size_t item ) const,
        "The derived class may implement this method to return the height of
the specified item (in pixels).  The default implementation returns
text height, as if this control was a normal combobox.", "");
    

    DocDeclStr(
        virtual wxCoord , OnMeasureItemWidth( size_t item ) const,
        "The derived class may implement this method to return the width of the
specified item (in pixels). If -1 is returned, then the item text
width is used.  The default implementation returns -1.", "");
    

    DocDeclStr(
        virtual void , OnDrawBackground( wxDC& dc, const wxRect& rect, int item, int flags ) const,
        "This method is used to draw the items background and, maybe, a border
around it.

The base class version implements a reasonable default behaviour which
consists in drawing the selected item with the standard background
colour and drawing a border around the item if it is either selected
or current.  ``flags`` has the sam meaning as with `OnDrawItem`.", "");
    

};

//---------------------------------------------------------------------------

%{
#include <wx/bmpcbox.h>
%}

DocStr(wxBitmapComboBox,
       "A combobox that displays a bitmap in front of the list items. It
currently only allows using bitmaps of one size, and resizes itself so
that a bitmap can be shown next to the text field.",
"
Window Styles
-------------
    ===================    ============================================
    wx.CB_READONLY         Creates a combobox without a text editor. On
                           some platforms the control may appear very
                           different when this style is used. 
    wx.CB_SORT             Sorts the entries in the list alphabetically. 
    wx.TE_PROCESS_ENTER    The control will generate the event
                           wx.EVT__TEXT_ENTER (otherwise pressing Enter
                           key is either processed internally by the
                           control or used for navigation between dialog
                           controls).
    ===================    ============================================
");

MustHaveApp(wxBitmapComboBox);

class wxBitmapComboBox : public wxPyOwnerDrawnComboBox
{
public:
    %pythonAppend wxBitmapComboBox      "self._setOORInfo(self);";
    %pythonAppend wxBitmapComboBox()    "";

    DocCtorStr(
        wxBitmapComboBox(wxWindow *parent,
                         wxWindowID id = -1,
                         const wxString& value = wxPyEmptyString,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         const wxArrayString& choices = wxPyEmptyStringArray,
                         long style = 0,
                         const wxValidator& validator = wxDefaultValidator,
                         const wxString& name = wxBitmapComboBoxNameStr),
        "Standard constructor", "");

    DocCtorStrName(wxBitmapComboBox(),
                   "2-phase create constructor.", "",
                   PreBitmapComboBox);

    DocDeclStr(
        bool , Create(wxWindow *parent,
                      wxWindowID id = -1,
                      const wxString& value = wxPyEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      const wxArrayString& choices = wxPyEmptyStringArray,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxBitmapComboBoxNameStr),
        "Create the UI object, and other initialization.", "");
    


    %extend {
        DocStr(Append,
               "Adds the item to the control, associating the given data with the item
if not None.  The return value is the index of the newly added item.", "");
        int Append(const wxString& item, const wxBitmap& bitmap = wxNullBitmap, PyObject* clientData=NULL) {
            if (clientData) {
                wxPyClientData* data = new wxPyClientData(clientData);
                return self->Append(item, bitmap, data);
            } else
                return self->Append(item, bitmap);
        }
    }
    
    
    DocDeclStr(
        virtual wxBitmap , GetItemBitmap(/*unsigned*/ int n) const,
        "Returns the image of the item with the given index.", "");

    
    %extend {
        DocStr(Insert,
               "Insert an item into the control before the item at the ``pos`` index,
optionally associating some data object with the item.", "");
        int Insert(const wxString& item, const wxBitmap& bitmap,
                   /*unsigned*/ int pos, PyObject* clientData=NULL) {
            if (clientData) {
                wxPyClientData* data = new wxPyClientData(clientData);
                return self->Insert(item, bitmap, pos, data);
            } else
                return self->Insert(item, bitmap, pos);
        }
    }

    
    DocDeclStr(
        virtual void , SetItemBitmap(/*unsigned*/ int n, const wxBitmap& bitmap),
        "Sets the image for the given item.", "");
    

    DocDeclStr(
        virtual wxSize , GetBitmapSize() const,
        "Returns size of the image used in list.", "");
    

};

//---------------------------------------------------------------------------

%init %{
    // Map renamed classes back to their common name for OOR
    wxPyPtrTypeMap_Add("wxComboCtrl", "wxPyComboCtrl");
    wxPyPtrTypeMap_Add("wxComboPopup", "wxPyComboPopup");
    wxPyPtrTypeMap_Add("wxOwnerDrawnComboBox", "wxPyOwnerDrawnComboBox");
%}
//---------------------------------------------------------------------------

