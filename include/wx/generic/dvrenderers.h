///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/dvrenderers.h
// Purpose:     All generic wxDataViewCtrl renderer classes
// Author:      Robert Roebling, Vadim Zeitlin
// Created:     2009-11-07 (extracted from wx/generic/dataview.h)
// RCS-ID:      $Id: wxhead.h,v 1.11 2009-06-29 10:23:04 zeitlin Exp $
// Copyright:   (c) 2006 Robert Roebling
//              (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_DVRENDERERS_H_
#define _WX_GENERIC_DVRENDERERS_H_

// ---------------------------------------------------------
// wxDataViewCustomRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCustomRenderer: public wxDataViewRenderer
{
public:
    wxDataViewCustomRenderer( const wxString &varianttype = wxT("string"),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT );

    virtual wxDataViewCustomRenderer *WXGetAsCustom() { return this; }

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCustomRenderer)
};


// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewTextRenderer: public wxDataViewRenderer
{
public:
    wxDataViewTextRenderer( const wxString &varianttype = wxT("string"),
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                            int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    virtual bool RenderWithAttr(wxDC& dc,
                                const wxRect& rect,
                                int align,
                                const wxDataViewItemAttr *attr,
                                int state);
    virtual bool Render(wxRect cell, wxDC *dc, int state)
    {
        return DummyRender(cell, dc, state);
    }

    wxSize GetSize() const;

    // in-place editing
    virtual bool HasEditorCtrl() const;
    virtual wxControl* CreateEditorCtrl( wxWindow *parent, wxRect labelRect,
                                         const wxVariant &value );
    virtual bool GetValueFromEditorCtrl( wxControl* editor, wxVariant &value );

protected:
    wxString   m_text;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewTextRenderer)
};

// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewBitmapRenderer: public wxDataViewRenderer
{
public:
    wxDataViewBitmapRenderer( const wxString &varianttype = wxT("wxBitmap"),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    bool Render( wxRect cell, wxDC *dc, int state );
    wxSize GetSize() const;

private:
    wxIcon m_icon;
    wxBitmap m_bitmap;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewBitmapRenderer)
};

// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewToggleRenderer: public wxDataViewRenderer
{
public:
    wxDataViewToggleRenderer( const wxString &varianttype = wxT("bool"),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    bool Render( wxRect cell, wxDC *dc, int state );
    bool Activate( wxRect cell, wxDataViewModel *model, const wxDataViewItem & item,
                            unsigned int col );
    wxSize GetSize() const;

private:
    bool    m_toggle;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewToggleRenderer)
};

// ---------------------------------------------------------
// wxDataViewProgressRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewProgressRenderer: public wxDataViewRenderer
{
public:
    wxDataViewProgressRenderer( const wxString &label = wxEmptyString,
                                const wxString &varianttype = wxT("long"),
                                wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant& value ) const;

    virtual bool RenderWithAttr(wxDC& dc,
                                const wxRect& rect,
                                int align,
                                const wxDataViewItemAttr *attr,
                                int state);
    virtual bool Render(wxRect cell, wxDC *dc, int state)
    {
        return DummyRender(cell, dc, state);
    }
    virtual wxSize GetSize() const;

private:
    wxString    m_label;
    int         m_value;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewProgressRenderer)
};

// ---------------------------------------------------------
// wxDataViewIconTextRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewIconTextRenderer: public wxDataViewRenderer
{
public:
    wxDataViewIconTextRenderer( const wxString &varianttype = wxT("wxDataViewIconText"),
                                wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    virtual bool RenderWithAttr(wxDC& dc,
                                const wxRect& rect,
                                int align,
                                const wxDataViewItemAttr *attr,
                                int state);
    virtual bool Render(wxRect cell, wxDC *dc, int state)
    {
        return DummyRender(cell, dc, state);
    }
    virtual wxSize GetSize() const;

    virtual bool HasEditorCtrl() const { return true; }
    virtual wxControl* CreateEditorCtrl( wxWindow *parent, wxRect labelRect,
                                         const wxVariant &value );
    virtual bool GetValueFromEditorCtrl( wxControl* editor, wxVariant &value );

private:
    wxDataViewIconText   m_value;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewIconTextRenderer)
};

// ---------------------------------------------------------
// wxDataViewDateRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewDateRenderer: public wxDataViewRenderer
{
public:
    wxDataViewDateRenderer( const wxString &varianttype = wxT("datetime"),
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE,
                            int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant& value ) const;

    virtual bool Render( wxRect cell, wxDC *dc, int state );
    virtual wxSize GetSize() const;
    virtual bool Activate( wxRect cell,
                           wxDataViewModel *model,
                           const wxDataViewItem& item,
                           unsigned int col );

private:
    wxDateTime    m_date;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewDateRenderer)
};


#endif // _WX_GENERIC_DVRENDERERS_H_

