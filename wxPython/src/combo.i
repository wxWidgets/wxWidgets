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


// Namespace for wxComboCtrl feature flags
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

    enum
    {
        ShowBelow       = 0x0000,  // Showing popup below the control
        ShowAbove       = 0x0001,  // Showing popup above the control
        CanDeferShow    = 0x0002  // Can only return true from AnimateShow if this is set
    };


    DEC_PYCALLBACK_VOID_(OnButtonClick);
    DEC_PYCALLBACK__RECTINT(DoShowPopup);
    DEC_PYCALLBACK_BOOL_RECTINT(AnimateShow);

    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyComboCtrl, wxComboCtrl);

IMP_PYCALLBACK_VOID_(wxPyComboCtrl, wxComboCtrl, OnButtonClick);
IMP_PYCALLBACK__RECTINT(wxPyComboCtrl, wxComboCtrl, DoShowPopup);
IMP_PYCALLBACK_BOOL_RECTINT(wxPyComboCtrl, wxComboCtrl, AnimateShow);

%}



// Now declare wxPyComboCtrl for Python
MustHaveApp(wxPyComboCtrl);
%rename(ComboCtrl) wxPyComboCtrl;

class wxPyComboCtrl : public wxControl
{
public:
    %pythonAppend wxPyComboCtrl      "self._setOORInfo(self);" setCallbackInfo(ComboCtrl)
    %pythonAppend wxPyComboCtrl()    "";

    wxPyComboCtrl(wxWindow *parent,
                  wxWindowID id = wxID_ANY,
                  const wxString& value = wxEmptyString,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxPyComboBoxNameStr);

    %RenameCtor(PreComboCtrl, wxPyComboCtrl());

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    // show/hide popup window
    virtual void ShowPopup();
    virtual void HidePopup();

    // Override for totally custom combo action
    virtual void OnButtonClick();

    // return true if the popup is currently shown
    bool IsPopupShown() const;


    // set interface class instance derived from wxComboPopup
    // NULL popup can be used to indicate default in a derived class
    %disownarg(wxPyComboPopup* popup);
    void SetPopupControl( wxPyComboPopup* popup );
    %cleardisown(wxPyComboPopup* popup);


    // get interface class instance derived from wxComboPopup
    wxPyComboPopup* GetPopupControl();

    // get the popup window containing the popup control
    wxWindow *GetPopupWindow() const;

    // Get the text control which is part of the combobox.
    wxTextCtrl *GetTextCtrl() const;

    // get the dropdown button which is part of the combobox
    // note: its not necessarily a wxButton or wxBitmapButton
    wxWindow *GetButton() const;

    // forward these methods to all subcontrols
    virtual bool Enable(bool enable = true);
    virtual bool Show(bool show = true);
    virtual bool SetFont(const wxFont& font);

    // wxTextCtrl methods - for readonly combo they should return
    // without errors.
    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value);
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

    // This method sets the text without affecting list selection
    // (ie. wxComboPopup::SetStringValue doesn't get called).
    void SetText(const wxString& value);

    // This method sets value and also optionally sends EVT_TEXT
    // (needed by combo popups)
    void SetValueWithEvent(const wxString& value, bool withEvent = true);

    //
    // Popup customization methods
    //

    // Sets minimum width of the popup. If wider than combo control, it will
    // extend to the left.
    // Remarks:
    // * Value -1 indicates the default.
    // * Custom popup may choose to ignore this (wxOwnerDrawnComboBox does not).
    void SetPopupMinWidth( int width );

    // Sets preferred maximum height of the popup.
    // Remarks:
    // * Value -1 indicates the default.
    // * Custom popup may choose to ignore this (wxOwnerDrawnComboBox does not).
    void SetPopupMaxHeight( int height );

    // Extends popup size horizontally, relative to the edges of the combo control.
    // Remarks:
    // * Popup minimum width may override extLeft (ie. it has higher precedence).
    // * Values 0 indicate default.
    // * Custom popup may not take this fully into account (wxOwnerDrawnComboBox takes).
    void SetPopupExtents( int extLeft, int extRight );

    // Set width, in pixels, of custom paint area in writable combo.
    // In read-only, used to indicate area that is not covered by the
    // focus rectangle (which may or may not be drawn, depending on the
    // popup type).
    void SetCustomPaintWidth( int width );
    int GetCustomPaintWidth() const;

    // Set side of the control to which the popup will align itself.
    // Valid values are wxLEFT, wxRIGHT and 0. The default value 0 means
    // that the side of the button will be used.
    void SetPopupAnchor( int anchorSide );

    // Set position of dropdown button.
    //   width: button width. <= 0 for default.
    //   height: button height. <= 0 for default.
    //   side: wxLEFT or wxRIGHT, indicates on which side the button will be placed.
    //   spacingX: empty space on sides of the button. Default is 0.
    // Remarks:
    //   There is no spacingY - the button will be centered vertically.
    void SetButtonPosition( int width = -1,
                            int height = -1,
                            int side = wxRIGHT,
                            int spacingX = 0 );

    // Returns current size of the dropdown button.
    wxSize GetButtonSize();

    //
    // Sets dropbutton to be drawn with custom bitmaps.
    //
    //  bmpNormal: drawn when cursor is not on button
    //  pushButtonBg: Draw push button background below the image.
    //                NOTE! This is usually only properly supported on platforms with appropriate
    //                      method in wxRendererNative.
    //  bmpPressed: drawn when button is depressed
    //  bmpHover: drawn when cursor hovers on button. This is ignored on platforms
    //            that do not generally display hover differently.
    //  bmpDisabled: drawn when combobox is disabled.
    void SetButtonBitmaps( const wxBitmap& bmpNormal,
                           bool pushButtonBg = false,
                           const wxBitmap& bmpPressed = wxNullBitmap,
                           const wxBitmap& bmpHover = wxNullBitmap,
                           const wxBitmap& bmpDisabled = wxNullBitmap );

    //
    // This will set the space in pixels between left edge of the control and the
    // text, regardless whether control is read-only (ie. no wxTextCtrl) or not.
    // Platform-specific default can be set with value-1.
    // Remarks
    // * This method may do nothing on some native implementations.
    void SetTextIndent( int indent );

    // Returns actual indentation in pixels.
    wxCoord GetTextIndent() const;

    // Returns area covered by the text field.
    const wxRect& GetTextRect() const;

    // Call with enable as true to use a type of popup window that guarantees ability
    // to focus the popup control, and normal function of common native controls.
    // This alternative popup window is usually a wxDialog, and as such it's parent
    // frame will appear as if the focus has been lost from it.
    void UseAltPopupWindow( bool enable = true );

    // Call with false to disable popup animation, if any.
    void EnablePopupAnimation( bool enable = true );

    //
    // Utilies needed by the popups or native implementations
    //

    // Returns true if given key combination should toggle the popup.
    // NB: This is a separate from other keyboard handling because:
    //     1) Replaceability.
    //     2) Centralized code (otherwise it'd be split up between
    //        wxComboCtrl key handler and wxVListBoxComboPopup's
    //        key handler).
    virtual bool IsKeyPopupToggle(const wxKeyEvent& event) const;

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
    virtual void PrepareBackground( wxDC& dc, const wxRect& rect, int flags ) const;

    // Returns true if focus indicator should be drawn in the control.
    bool ShouldDrawFocus() const;

    // These methods return references to appropriate dropbutton bitmaps
    const wxBitmap& GetBitmapNormal() const;
    const wxBitmap& GetBitmapPressed() const;
    const wxBitmap& GetBitmapHover() const;
    const wxBitmap& GetBitmapDisabled() const;

    // Return internal flags
    wxUint32 GetInternalFlags() const;

    // Return true if Create has finished
    bool IsCreated() const;

    // common code to be called on popup hide/dismiss
    void OnPopupDismiss();

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

    static int GetFeatures();


    // Flags for DoShowPopup and AnimateShow
    enum
    {
        ShowBelow       = 0x0000,  // Showing popup below the control
        ShowAbove       = 0x0001,  // Showing popup above the control
        CanDeferShow    = 0x0002  // Can only return true from AnimateShow if this is set
    };

    // Shows and positions the popup.
    virtual void DoShowPopup( const wxRect& rect, int flags );

    // Implement in derived class to create a drop-down animation.
    // Return true if finished immediately. Otherwise popup is only
    // shown when the derived class call DoShowPopup.
    // Flags are same as for DoShowPopup.
    virtual bool AnimateShow( const wxRect& rect, int flags );

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



// Now declare wxPyComboCtrl for Python
MustHaveApp(wxPyComboPopup);
%rename(ComboPopup) wxPyComboPopup;

class wxPyComboPopup
{
public:
    %pythonAppend wxPyComboPopup  setCallbackInfo(ComboPopup);

    wxPyComboPopup();
    ~wxPyComboPopup();

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    // This is called immediately after construction finishes. m_combo member
    // variable has been initialized before the call.
    // NOTE: It is not in constructor so the derived class doesn't need to redefine
    //       a default constructor of its own.
    virtual void Init();

    // Create the popup child control.
    // Return true for success.
    virtual bool Create(wxWindow* parent);

    // We must have an associated control which is subclassed by the combobox.
    virtual wxWindow *GetControl();

    // Called immediately after the popup is shown
    virtual void OnPopup();

    // Called when popup is dismissed
    virtual void OnDismiss();

    // Called just prior to displaying popup.
    // Default implementation does nothing.
    virtual void SetStringValue( const wxString& value );

    // Gets displayed string representation of the value.
    virtual wxString GetStringValue() const;

    // This is called to custom paint in the combo control itself (ie. not the popup).
    // Default implementation draws value as string.
    virtual void PaintComboControl( wxDC& dc, const wxRect& rect );

    // Receives key events from the parent wxComboCtrl.
    // Events not handled should be skipped, as usual.
    virtual void OnComboKeyEvent( wxKeyEvent& event );

    // Implement if you need to support special action when user
    // double-clicks on the parent wxComboCtrl.
    virtual void OnComboDoubleClick();

    // Return final size of popup. Called on every popup, just prior to OnPopup.
    // minWidth = preferred minimum width for window
    // prefHeight = preferred height. Only applies if > 0,
    // maxHeight = max height for window, as limited by screen size
    //   and should only be rounded down, if necessary.
    virtual wxSize GetAdjustedSize( int minWidth, int prefHeight, int maxHeight );

    // Return true if you want delay call to Create until the popup is shown
    // for the first time. It is more efficient, but note that it is often
    // more convenient to have the control created immediately.
    // Default returns false.
    virtual bool LazyCreate();

    //
    // Utilies
    //

    // Hides the popup
    void Dismiss();

    // Returns true if Create has been called.
    bool IsCreated() const;

    // Default PaintComboControl behaviour
    static void DefaultPaintComboControl( wxComboCtrlBase* combo,
                                          wxDC& dc,
                                          const wxRect& rect );

    wxPyComboCtrl* GetCombo();

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



// Now declare wxPyComboCtrl for Python
MustHaveApp(wxPyOwnerDrawnComboBox);
%rename(OwnerDrawnComboBox) wxPyOwnerDrawnComboBox;

class wxPyOwnerDrawnComboBox : public wxPyComboCtrl,
                               public wxItemContainer
{
public:
    %pythonAppend wxPyOwnerDrawnComboBox      "self._setOORInfo(self);" setCallbackInfo(OwnerDrawnComboBox)
    %pythonAppend wxPyOwnerDrawnComboBox()    "";

    wxPyOwnerDrawnComboBox(wxWindow *parent,
                           wxWindowID id = -1,
                           const wxString& value = wxPyEmptyString,
                           const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize,
                           const wxArrayString& choices = wxPyEmptyStringArray,
                           long style = 0,
                           const wxValidator& validator = wxDefaultValidator,
                           const wxString& name = wxPyComboBoxNameStr);

    %RenameCtor(PreOwnerDrawnComboBox, wxPyOwnerDrawnComboBox());


    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxString& value = wxPyEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxArrayString& choices = wxPyEmptyStringArray,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyComboBoxNameStr);

    // Return the widest item width (recalculating it if necessary)
    virtual int GetWidestItemWidth();

    // Return the index of the widest item (recalculating it if necessary)
    virtual int GetWidestItem();

    void SetSelection(int n);
    %Rename(SetMark, void , SetSelection(long from, long to));
};


//---------------------------------------------------------------------------

%init %{
    // Map renamed classes back to their common name for OOR
    wxPyPtrTypeMap_Add("wxComboCtrl", "wxPyComboCtrl");
    wxPyPtrTypeMap_Add("wxComboPopup", "wxPyComboPopup");
%}
//---------------------------------------------------------------------------

