/////////////////////////////////////////////////////////////////////////////
// Name:        _pywindows.i
// Purpose:     SWIG interface for wxPyWindow, and etc.  These classes support
//              overriding many of wxWindow's virtual methods in Python derived
//              classes.
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
%newgroup

// TODO:  Redo these with SWIG directors?

// TODO:  Which (if any?) of these should be done also???
//      Destroy
//      DoCaptureMouse
//      DoClientToScreen
//      DoHitTest
//      DoMoveWindow
//      DoPopupMenu
//      DoReleaseMouse
//      DoScreenToClient
//      DoSetToolTip
//      Enable
//      Fit
//      GetCharHeight
//      GetCharWidth
//      GetClientAreaOrigin
//      GetDefaultItem
//      IsTopLevel
//      SetBackgroundColour
//      SetDefaultItem
//      SetFocus
//      SetFocusFromKbd
//      SetForegroundColour
//      SetSizeHints
//      SetVirtualSizeHints
//      Show


//---------------------------------------------------------------------------


%{ // C++ version of Python aware wxWindow
class wxPyWindow : public wxWindow
{
    DECLARE_DYNAMIC_CLASS(wxPyWindow)
public:
    wxPyWindow() : wxWindow() {}
    wxPyWindow(wxWindow* parent, const wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxPyPanelNameStr)
        : wxWindow(parent, id, pos, size, style, name) {}

    void SetBestSize(const wxSize& size) { wxWindow::SetBestSize(size); }

    DEC_PYCALLBACK_VOID_INT4(DoMoveWindow);
    DEC_PYCALLBACK_VOID_INT5(DoSetSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetClientSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetVirtualSize);

    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetClientSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetPosition);

    DEC_PYCALLBACK_SIZE_const(DoGetVirtualSize);
    DEC_PYCALLBACK_SIZE_const(DoGetBestSize);

    DEC_PYCALLBACK__(InitDialog);
    DEC_PYCALLBACK_BOOL_(TransferDataFromWindow);
    DEC_PYCALLBACK_BOOL_(TransferDataToWindow);
    DEC_PYCALLBACK_BOOL_(Validate);

    DEC_PYCALLBACK_BOOL_const(AcceptsFocus);
    DEC_PYCALLBACK_BOOL_const(AcceptsFocusFromKeyboard);
    DEC_PYCALLBACK_SIZE_const(GetMaxSize);

    DEC_PYCALLBACK_VOID_WXWINBASE(AddChild);
    DEC_PYCALLBACK_VOID_WXWINBASE(RemoveChild);

    DEC_PYCALLBACK_BOOL_const(ShouldInheritColours);
    DEC_PYCALLBACK__COLOUR(ApplyParentThemeBackground);
    DEC_PYCALLBACK_VIZATTR_(GetDefaultAttributes);
    
    DEC_PYCALLBACK_BOOL_(HasTransparentBackground);

    PYPRIVATE;
};

IMPLEMENT_DYNAMIC_CLASS(wxPyWindow, wxWindow);

IMP_PYCALLBACK_VOID_INT4(wxPyWindow, wxWindow, DoMoveWindow);
IMP_PYCALLBACK_VOID_INT5(wxPyWindow, wxWindow, DoSetSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyWindow, wxWindow, DoSetClientSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyWindow, wxWindow, DoSetVirtualSize);

IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyWindow, wxWindow, DoGetSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyWindow, wxWindow, DoGetClientSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyWindow, wxWindow, DoGetPosition);

IMP_PYCALLBACK_SIZE_const(wxPyWindow, wxWindow, DoGetVirtualSize);
IMP_PYCALLBACK_SIZE_const(wxPyWindow, wxWindow, DoGetBestSize);

IMP_PYCALLBACK__(wxPyWindow, wxWindow, InitDialog);
IMP_PYCALLBACK_BOOL_(wxPyWindow, wxWindow, TransferDataFromWindow);
IMP_PYCALLBACK_BOOL_(wxPyWindow, wxWindow, TransferDataToWindow);
IMP_PYCALLBACK_BOOL_(wxPyWindow, wxWindow, Validate);

IMP_PYCALLBACK_BOOL_const(wxPyWindow, wxWindow, AcceptsFocus);
IMP_PYCALLBACK_BOOL_const(wxPyWindow, wxWindow, AcceptsFocusFromKeyboard);
IMP_PYCALLBACK_SIZE_const(wxPyWindow, wxWindow, GetMaxSize);

IMP_PYCALLBACK_VOID_WXWINBASE(wxPyWindow, wxWindow, AddChild);
IMP_PYCALLBACK_VOID_WXWINBASE(wxPyWindow, wxWindow, RemoveChild);

IMP_PYCALLBACK_BOOL_const(wxPyWindow, wxWindow, ShouldInheritColours);
IMP_PYCALLBACK__COLOUR(wxPyWindow, wxWindow, ApplyParentThemeBackground);
IMP_PYCALLBACK_VIZATTR_(wxPyWindow, wxWindow, GetDefaultAttributes);

IMP_PYCALLBACK_BOOL_(wxPyWindow, wxWindow, HasTransparentBackground);
%}

// And now the one for SWIG to see
MustHaveApp(wxPyWindow);
class wxPyWindow : public wxWindow
{
public:
    %pythonAppend wxPyWindow         "self._setOORInfo(self); self._setCallbackInfo(self, PyWindow)"
    %pythonAppend wxPyWindow()       ""

    wxPyWindow(wxWindow* parent, const wxWindowID id=-1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxPyPanelNameStr);

    %RenameCtor(PrePyWindow, wxPyWindow());
    
    void _setCallbackInfo(PyObject* self, PyObject* _class);


    void SetBestSize(const wxSize& size);

    void base_DoMoveWindow(int x, int y, int width, int height);
    void base_DoSetSize(int x, int y, int width, int height,
                        int sizeFlags = wxSIZE_AUTO);
    void base_DoSetClientSize(int width, int height);
    void base_DoSetVirtualSize( int x, int y );

    DocDeclA(
        void, base_DoGetSize( int *OUTPUT, int *OUTPUT ) const,
        "base_DoGetSize() -> (width, height)");
    DocDeclA(
        void, base_DoGetClientSize( int *OUTPUT, int *OUTPUT ) const,
        "base_DoGetClientSize() -> (width, height)");
    DocDeclA(
        void, base_DoGetPosition( int *OUTPUT, int *OUTPUT ) const,
        "base_DoGetPosition() -> (x,y)");

    wxSize base_DoGetVirtualSize() const;
    wxSize base_DoGetBestSize() const;

    void base_InitDialog();
    bool base_TransferDataToWindow();
    bool base_TransferDataFromWindow();
    bool base_Validate();

    bool base_AcceptsFocus() const;
    bool base_AcceptsFocusFromKeyboard() const;
    wxSize base_GetMaxSize() const;

    void base_AddChild(wxWindow* child);
    void base_RemoveChild(wxWindow* child);

    bool base_ShouldInheritColours() const;
    void base_ApplyParentThemeBackground(const wxColour& c);
    wxVisualAttributes base_GetDefaultAttributes();
};

//---------------------------------------------------------------------------
// Do the same thing for wxControl

// ** See _pycontrol.i, it was moved there because of dependency on wxControl


//---------------------------------------------------------------------------
// and for wxPanel

%{ // C++ version of Python aware wxPanel
class wxPyPanel : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxPyPanel)
public:
    wxPyPanel() : wxPanel() {}
    wxPyPanel(wxWindow* parent, const wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxPyPanelNameStr)
        : wxPanel(parent, id, pos, size, style, name) {}

    void SetBestSize(const wxSize& size) { wxPanel::SetBestSize(size); }
    

    DEC_PYCALLBACK_VOID_INT4(DoMoveWindow);
    DEC_PYCALLBACK_VOID_INT5(DoSetSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetClientSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetVirtualSize);

    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetClientSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetPosition);

    DEC_PYCALLBACK_SIZE_const(DoGetVirtualSize);
    DEC_PYCALLBACK_SIZE_const(DoGetBestSize);

    DEC_PYCALLBACK__(InitDialog);
    DEC_PYCALLBACK_BOOL_(TransferDataFromWindow);
    DEC_PYCALLBACK_BOOL_(TransferDataToWindow);
    DEC_PYCALLBACK_BOOL_(Validate);

    DEC_PYCALLBACK_BOOL_const(AcceptsFocus);
    DEC_PYCALLBACK_BOOL_const(AcceptsFocusFromKeyboard);
    DEC_PYCALLBACK_SIZE_const(GetMaxSize);

    DEC_PYCALLBACK_VOID_WXWINBASE(AddChild);
    DEC_PYCALLBACK_VOID_WXWINBASE(RemoveChild);

    DEC_PYCALLBACK_BOOL_const(ShouldInheritColours);
    DEC_PYCALLBACK__COLOUR(ApplyParentThemeBackground);
    DEC_PYCALLBACK_VIZATTR_(GetDefaultAttributes);
    
    DEC_PYCALLBACK_BOOL_(HasTransparentBackground);

    PYPRIVATE;
};

IMPLEMENT_DYNAMIC_CLASS(wxPyPanel, wxPanel);

IMP_PYCALLBACK_VOID_INT4(wxPyPanel, wxPanel, DoMoveWindow);
IMP_PYCALLBACK_VOID_INT5(wxPyPanel, wxPanel, DoSetSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyPanel, wxPanel, DoSetClientSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyPanel, wxPanel, DoSetVirtualSize);

IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyPanel, wxPanel, DoGetSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyPanel, wxPanel, DoGetClientSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyPanel, wxPanel, DoGetPosition);

IMP_PYCALLBACK_SIZE_const(wxPyPanel, wxPanel, DoGetVirtualSize);
IMP_PYCALLBACK_SIZE_const(wxPyPanel, wxPanel, DoGetBestSize);

IMP_PYCALLBACK__(wxPyPanel, wxPanel, InitDialog);
IMP_PYCALLBACK_BOOL_(wxPyPanel, wxPanel, TransferDataFromWindow);
IMP_PYCALLBACK_BOOL_(wxPyPanel, wxPanel, TransferDataToWindow);
IMP_PYCALLBACK_BOOL_(wxPyPanel, wxPanel, Validate);

IMP_PYCALLBACK_BOOL_const(wxPyPanel, wxPanel, AcceptsFocus);
IMP_PYCALLBACK_BOOL_const(wxPyPanel, wxPanel, AcceptsFocusFromKeyboard);
IMP_PYCALLBACK_SIZE_const(wxPyPanel, wxPanel, GetMaxSize);

IMP_PYCALLBACK_VOID_WXWINBASE(wxPyPanel, wxPanel, AddChild);
IMP_PYCALLBACK_VOID_WXWINBASE(wxPyPanel, wxPanel, RemoveChild);

IMP_PYCALLBACK_BOOL_const(wxPyPanel, wxPanel, ShouldInheritColours);
IMP_PYCALLBACK__COLOUR(wxPyPanel, wxPanel, ApplyParentThemeBackground);
IMP_PYCALLBACK_VIZATTR_(wxPyPanel, wxPanel, GetDefaultAttributes);

IMP_PYCALLBACK_BOOL_(wxPyPanel, wxPanel, HasTransparentBackground);
%}

// And now the one for SWIG to see
MustHaveApp(wxPyPanel);
class wxPyPanel : public wxPanel
{
public:
    %pythonAppend wxPyPanel         "self._setOORInfo(self); self._setCallbackInfo(self, PyPanel)"
    %pythonAppend wxPyPanel()       ""

    wxPyPanel(wxWindow* parent, const wxWindowID id=-1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxPyPanelNameStr);

    %RenameCtor(PrePyPanel,  wxPyPanel());

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    void SetBestSize(const wxSize& size);

    void base_DoMoveWindow(int x, int y, int width, int height);
    void base_DoSetSize(int x, int y, int width, int height,
                        int sizeFlags = wxSIZE_AUTO);
    void base_DoSetClientSize(int width, int height);
    void base_DoSetVirtualSize( int x, int y );

    DocDeclA(
        void, base_DoGetSize( int *OUTPUT, int *OUTPUT ) const,
        "base_DoGetSize() -> (width, height)");
    DocDeclA(
        void, base_DoGetClientSize( int *OUTPUT, int *OUTPUT ) const,
        "base_DoGetClientSize() -> (width, height)");
    DocDeclA(
        void, base_DoGetPosition( int *OUTPUT, int *OUTPUT ) const,
        "base_DoGetPosition() -> (x,y)");

    wxSize base_DoGetVirtualSize() const;
    wxSize base_DoGetBestSize() const;

    void base_InitDialog();
    bool base_TransferDataToWindow();
    bool base_TransferDataFromWindow();
    bool base_Validate();

    bool base_AcceptsFocus() const;
    bool base_AcceptsFocusFromKeyboard() const;
    wxSize base_GetMaxSize() const;

    void base_AddChild(wxWindow* child);
    void base_RemoveChild(wxWindow* child);

    bool base_ShouldInheritColours() const ;
    void base_ApplyParentThemeBackground(const wxColour& c);
    wxVisualAttributes base_GetDefaultAttributes();
};

//---------------------------------------------------------------------------
// and for wxScrolledWindow

%{ // C++ version of Python aware wxScrolledWindow
class wxPyScrolledWindow : public wxScrolledWindow
{
    DECLARE_DYNAMIC_CLASS(wxPyScrolledWindow)
public:
    wxPyScrolledWindow() : wxScrolledWindow() {}
    wxPyScrolledWindow(wxWindow* parent, const wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxPyPanelNameStr)
        : wxScrolledWindow(parent, id, pos, size, style, name) {}

    void SetBestSize(const wxSize& size) { wxScrolledWindow::SetBestSize(size); }

    DEC_PYCALLBACK_VOID_INT4(DoMoveWindow);
    DEC_PYCALLBACK_VOID_INT5(DoSetSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetClientSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetVirtualSize);

    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetClientSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetPosition);

    DEC_PYCALLBACK_SIZE_const(DoGetVirtualSize);
    DEC_PYCALLBACK_SIZE_const(DoGetBestSize);

    DEC_PYCALLBACK__(InitDialog);
    DEC_PYCALLBACK_BOOL_(TransferDataFromWindow);
    DEC_PYCALLBACK_BOOL_(TransferDataToWindow);
    DEC_PYCALLBACK_BOOL_(Validate);

    DEC_PYCALLBACK_BOOL_const(AcceptsFocus);
    DEC_PYCALLBACK_BOOL_const(AcceptsFocusFromKeyboard);
    DEC_PYCALLBACK_SIZE_const(GetMaxSize);

    DEC_PYCALLBACK_VOID_WXWINBASE(AddChild);
    DEC_PYCALLBACK_VOID_WXWINBASE(RemoveChild);

    DEC_PYCALLBACK_BOOL_const(ShouldInheritColours);
    DEC_PYCALLBACK__COLOUR(ApplyParentThemeBackground);
    DEC_PYCALLBACK_VIZATTR_(GetDefaultAttributes);
    
    DEC_PYCALLBACK_BOOL_(HasTransparentBackground);

    PYPRIVATE;
};

IMPLEMENT_DYNAMIC_CLASS(wxPyScrolledWindow, wxScrolledWindow);

IMP_PYCALLBACK_VOID_INT4(wxPyScrolledWindow, wxScrolledWindow, DoMoveWindow);
IMP_PYCALLBACK_VOID_INT5(wxPyScrolledWindow, wxScrolledWindow, DoSetSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyScrolledWindow, wxScrolledWindow, DoSetClientSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyScrolledWindow, wxScrolledWindow, DoSetVirtualSize);

IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyScrolledWindow, wxScrolledWindow, DoGetSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyScrolledWindow, wxScrolledWindow, DoGetClientSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyScrolledWindow, wxScrolledWindow, DoGetPosition);

IMP_PYCALLBACK_SIZE_const(wxPyScrolledWindow, wxScrolledWindow, DoGetVirtualSize);
IMP_PYCALLBACK_SIZE_const(wxPyScrolledWindow, wxScrolledWindow, DoGetBestSize);

IMP_PYCALLBACK__(wxPyScrolledWindow, wxScrolledWindow, InitDialog);
IMP_PYCALLBACK_BOOL_(wxPyScrolledWindow, wxScrolledWindow, TransferDataFromWindow);
IMP_PYCALLBACK_BOOL_(wxPyScrolledWindow, wxScrolledWindow, TransferDataToWindow);
IMP_PYCALLBACK_BOOL_(wxPyScrolledWindow, wxScrolledWindow, Validate);

IMP_PYCALLBACK_BOOL_const(wxPyScrolledWindow, wxScrolledWindow, AcceptsFocus);
IMP_PYCALLBACK_BOOL_const(wxPyScrolledWindow, wxScrolledWindow, AcceptsFocusFromKeyboard);
IMP_PYCALLBACK_SIZE_const(wxPyScrolledWindow, wxScrolledWindow, GetMaxSize);

IMP_PYCALLBACK_VOID_WXWINBASE(wxPyScrolledWindow, wxScrolledWindow, AddChild);
IMP_PYCALLBACK_VOID_WXWINBASE(wxPyScrolledWindow, wxScrolledWindow, RemoveChild);

IMP_PYCALLBACK_BOOL_const(wxPyScrolledWindow, wxScrolledWindow, ShouldInheritColours);
IMP_PYCALLBACK__COLOUR(wxPyScrolledWindow, wxScrolledWindow, ApplyParentThemeBackground);
IMP_PYCALLBACK_VIZATTR_(wxPyScrolledWindow, wxScrolledWindow, GetDefaultAttributes);

IMP_PYCALLBACK_BOOL_(wxPyScrolledWindow, wxScrolledWindow, HasTransparentBackground);
%}

// And now the one for SWIG to see
MustHaveApp(wxPyScrolledWindow);
class wxPyScrolledWindow : public wxScrolledWindow
{
public:
    %pythonAppend wxPyScrolledWindow         "self._setOORInfo(self); self._setCallbackInfo(self, PyPanel)"
    %pythonAppend wxPyScrolledWindow()       ""

    wxPyScrolledWindow(wxWindow* parent, const wxWindowID id=-1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxPyPanelNameStr);

    %RenameCtor(PrePyScrolledWindow,  wxPyScrolledWindow());

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    void SetBestSize(const wxSize& size);

    void base_DoMoveWindow(int x, int y, int width, int height);
    void base_DoSetSize(int x, int y, int width, int height,
                        int sizeFlags = wxSIZE_AUTO);
    void base_DoSetClientSize(int width, int height);
    void base_DoSetVirtualSize( int x, int y );

    DocDeclA(
        void, base_DoGetSize( int *OUTPUT, int *OUTPUT ) const,
        "base_DoGetSize() -> (width, height)");
    DocDeclA(
        void, base_DoGetClientSize( int *OUTPUT, int *OUTPUT ) const,
        "base_DoGetClientSize() -> (width, height)");
    DocDeclA(
        void, base_DoGetPosition( int *OUTPUT, int *OUTPUT ) const,
        "base_DoGetPosition() -> (x,y)");

    wxSize base_DoGetVirtualSize() const;
    wxSize base_DoGetBestSize() const;

    void base_InitDialog();
    bool base_TransferDataToWindow();
    bool base_TransferDataFromWindow();
    bool base_Validate();

    bool base_AcceptsFocus() const;
    bool base_AcceptsFocusFromKeyboard() const;
    wxSize base_GetMaxSize() const;

    void base_AddChild(wxWindow* child);
    void base_RemoveChild(wxWindow* child);

    bool base_ShouldInheritColours() const;
    void base_ApplyParentThemeBackground(const wxColour& c);
    wxVisualAttributes base_GetDefaultAttributes();
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
