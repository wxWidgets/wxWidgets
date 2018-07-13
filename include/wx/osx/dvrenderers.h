///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/dvrenderers.h
// Purpose:     All OS X wxDataViewCtrl renderer classes
// Author:      Vadim Zeitlin
// Created:     2009-11-07 (extracted from wx/osx/dataview.h)
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_DVRENDERERS_H_
#define _WX_OSX_DVRENDERERS_H_

// ---------------------------------------------------------
// wxDataViewCustomRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCustomRenderer : public wxDataViewCustomRendererBase
{
public:
    static wxString GetDefaultType() { return wxS("string"); }

    wxDataViewCustomRenderer(const wxString& varianttype = GetDefaultType(),
                             wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                             int align = wxDVR_DEFAULT_ALIGNMENT);

    virtual ~wxDataViewCustomRenderer();


    // implementation only
    // -------------------

    virtual bool MacRender();

    virtual wxDC* GetDC(); // creates a device context and keeps it
    void SetDC(wxDC* newDCPtr); // this method takes ownership of the pointer

private:
    wxControl* m_editorCtrlPtr; // pointer to an in-place editor control

    wxDC* m_DCPtr;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCustomRenderer);
};

// ---------------------------------------------------------------------------
// This is a Mac-specific class that should be used as the base class for the
// renderers that should be disabled when they're inert, to prevent the user
// from editing them.
// ---------------------------------------------------------------------------

class wxOSXDataViewDisabledInertRenderer : public wxDataViewRenderer
{
protected:
    wxOSXDataViewDisabledInertRenderer(const wxString& varianttype,
                                       wxDataViewCellMode mode,
                                       int alignment)
        : wxDataViewRenderer(varianttype, mode, alignment)
    {
    }

    virtual void SetEnabled(bool enabled) wxOVERRIDE
    {
        wxDataViewRenderer::SetEnabled(enabled &&
                                        GetMode() != wxDATAVIEW_CELL_INERT);
    }
};

// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewTextRenderer: public wxDataViewRenderer
{
public:
    static wxString GetDefaultType() { return wxS("string"); }

    wxDataViewTextRenderer(const wxString& varianttype = GetDefaultType(),
                           wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                           int align = wxDVR_DEFAULT_ALIGNMENT);

#if wxUSE_MARKUP && wxOSX_USE_COCOA
    void EnableMarkup(bool enable = true);
#endif // wxUSE_MARKUP && Cocoa

    virtual bool MacRender();

    virtual void OSXOnCellChanged(NSObject *value,
                                  const wxDataViewItem& item,
                                  unsigned col);

private:
#if wxUSE_MARKUP && wxOSX_USE_COCOA
    // True if we should interpret markup in our text.
    bool m_useMarkup;
#endif // wxUSE_MARKUP && Cocoa

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewTextRenderer);
};

// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewBitmapRenderer: public wxDataViewRenderer
{
public:
    static wxString GetDefaultType() { return wxS("wxBitmap"); }

    wxDataViewBitmapRenderer(const wxString& varianttype = GetDefaultType(),
                             wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                             int align = wxDVR_DEFAULT_ALIGNMENT);

    virtual bool MacRender();

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewBitmapRenderer);
};

// -------------------------------------
// wxDataViewChoiceRenderer
// -------------------------------------

class WXDLLIMPEXP_ADV wxDataViewChoiceRenderer
    : public wxOSXDataViewDisabledInertRenderer
{
public:
    wxDataViewChoiceRenderer(const wxArrayString& choices,
                             wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                             int alignment = wxDVR_DEFAULT_ALIGNMENT );

    virtual bool MacRender();

    wxString GetChoice(size_t index) const { return m_choices[index]; }
    const wxArrayString& GetChoices() const { return m_choices; }

    virtual void OSXOnCellChanged(NSObject *value,
                                  const wxDataViewItem& item,
                                  unsigned col);

private:
    wxArrayString m_choices;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewChoiceRenderer);
};

// ----------------------------------------------------------------------------
// wxDataViewChoiceByIndexRenderer
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewChoiceByIndexRenderer: public wxDataViewChoiceRenderer
{
public:
    wxDataViewChoiceByIndexRenderer(const wxArrayString& choices,
                                    wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                                    int alignment = wxDVR_DEFAULT_ALIGNMENT);

    virtual bool SetValue(const wxVariant& value) wxOVERRIDE;
    virtual bool GetValue(wxVariant& value) const wxOVERRIDE;

    virtual void OSXOnCellChanged(NSObject *value,
                                  const wxDataViewItem& item,
                                  unsigned col) wxOVERRIDE;
};

// ---------------------------------------------------------
// wxDataViewIconTextRenderer
// ---------------------------------------------------------
class WXDLLIMPEXP_ADV wxDataViewIconTextRenderer: public wxDataViewRenderer
{
public:
    static wxString GetDefaultType() { return wxS("wxDataViewIconText"); }

    wxDataViewIconTextRenderer(const wxString& varianttype = GetDefaultType(),
                               wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                               int align = wxDVR_DEFAULT_ALIGNMENT);

    virtual bool MacRender();

    virtual void OSXOnCellChanged(NSObject *value,
                                  const wxDataViewItem& item,
                                  unsigned col);

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewIconTextRenderer);
};

// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewToggleRenderer
    : public wxOSXDataViewDisabledInertRenderer
{
public:
    static wxString GetDefaultType() { return wxS("bool"); }

    wxDataViewToggleRenderer(const wxString& varianttype = GetDefaultType(),
                             wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                             int align = wxDVR_DEFAULT_ALIGNMENT);

    void ShowAsRadio();

    virtual bool MacRender();

    virtual void OSXOnCellChanged(NSObject *value,
                                  const wxDataViewItem& item,
                                  unsigned col);

private:
    void DoInitButtonCell(int buttonType);

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewToggleRenderer);
};

// ---------------------------------------------------------
// wxDataViewProgressRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewProgressRenderer: public wxDataViewRenderer
{
public:
    static wxString GetDefaultType() { return wxS("long"); }

    wxDataViewProgressRenderer(const wxString& label = wxEmptyString,
                               const wxString& varianttype = GetDefaultType(),
                               wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                               int align = wxDVR_DEFAULT_ALIGNMENT);

    virtual bool MacRender();

    virtual void OSXOnCellChanged(NSObject *value,
                                  const wxDataViewItem& item,
                                  unsigned col);

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewProgressRenderer);
};

// ---------------------------------------------------------
// wxDataViewDateRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewDateRenderer: public wxDataViewRenderer
{
public:
    static wxString GetDefaultType() { return wxS("datetime"); }

    wxDataViewDateRenderer(const wxString& varianttype = GetDefaultType(),
                           wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE,
                           int align = wxDVR_DEFAULT_ALIGNMENT);

    virtual bool MacRender();

    virtual void OSXOnCellChanged(NSObject *value,
                                  const wxDataViewItem& item,
                                  unsigned col);

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewDateRenderer);
};

#endif // _WX_OSX_DVRENDERERS_H_

