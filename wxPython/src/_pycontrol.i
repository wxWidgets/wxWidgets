/////////////////////////////////////////////////////////////////////////////
// Name:        _pywindows.i
// Purpose:     SWIG interface for wxPyControl, See also _pywindows.i
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



//---------------------------------------------------------------------------

%{ // C++ version of Python aware wxControl
class wxPyControl : public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxPyControl)
public:
    wxPyControl() : wxControl() {}
    wxPyControl(wxWindow* parent, const wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator=wxDefaultValidator,
                const wxString& name = wxPyControlNameStr)
        : wxControl(parent, id, pos, size, style, validator, name) {}

    void SetBestSize(const wxSize& size) { wxControl::SetBestSize(size); }

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
    
    PYPRIVATE;
};

IMPLEMENT_DYNAMIC_CLASS(wxPyControl, wxControl);

IMP_PYCALLBACK_VOID_INT4(wxPyControl, wxControl, DoMoveWindow);
IMP_PYCALLBACK_VOID_INT5(wxPyControl, wxControl, DoSetSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyControl, wxControl, DoSetClientSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyControl, wxControl, DoSetVirtualSize);

IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyControl, wxControl, DoGetSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyControl, wxControl, DoGetClientSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyControl, wxControl, DoGetPosition);

IMP_PYCALLBACK_SIZE_const(wxPyControl, wxControl, DoGetVirtualSize);
IMP_PYCALLBACK_SIZE_const(wxPyControl, wxControl, DoGetBestSize);

IMP_PYCALLBACK__(wxPyControl, wxControl, InitDialog);
IMP_PYCALLBACK_BOOL_(wxPyControl, wxControl, TransferDataFromWindow);
IMP_PYCALLBACK_BOOL_(wxPyControl, wxControl, TransferDataToWindow);
IMP_PYCALLBACK_BOOL_(wxPyControl, wxControl, Validate);

IMP_PYCALLBACK_BOOL_const(wxPyControl, wxControl, AcceptsFocus);
IMP_PYCALLBACK_BOOL_const(wxPyControl, wxControl, AcceptsFocusFromKeyboard);
IMP_PYCALLBACK_SIZE_const(wxPyControl, wxControl, GetMaxSize);

IMP_PYCALLBACK_VOID_WXWINBASE(wxPyControl, wxControl, AddChild);
IMP_PYCALLBACK_VOID_WXWINBASE(wxPyControl, wxControl, RemoveChild);

IMP_PYCALLBACK_BOOL_const(wxPyControl, wxControl, ShouldInheritColours);
IMP_PYCALLBACK__COLOUR(wxPyControl, wxControl, ApplyParentThemeBackground);
IMP_PYCALLBACK_VIZATTR_(wxPyControl, wxControl, GetDefaultAttributes);
%}

// And now the one for SWIG to see
MustHaveApp(wxPyControl);
class wxPyControl : public wxControl
{
public:
    %pythonAppend wxPyControl         "self._setOORInfo(self); self._setCallbackInfo(self, PyControl)"
    %pythonAppend wxPyControl()       ""

    wxPyControl(wxWindow* parent, const wxWindowID id=-1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator=wxDefaultValidator,
                const wxString& name = wxPyControlNameStr);

    %RenameCtor(PrePyControl,  wxPyControl());
    
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
