///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dvrenderers.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DVRENDERERS_H_
#define _WX_QT_DVRENDERERS_H_

// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewTextRenderer: public wxDataViewRenderer
{
public:
    static wxString GetDefaultType() { return wxS("string"); }

    wxDataViewTextRenderer( const wxString &varianttype = GetDefaultType(),
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                            int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    void SetAlignment( int align );
};

// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewBitmapRenderer: public wxDataViewRenderer
{
public:
    static wxString GetDefaultType() { return wxS("wxBitmapBundle"); }

    wxDataViewBitmapRenderer( const wxString &varianttype = GetDefaultType(),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    bool IsCompatibleVariantType(const wxString& variantType) const;
};

// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewToggleRenderer: public wxDataViewRenderer
{
public:
    static wxString GetDefaultType() { return wxS("bool"); }

    wxDataViewToggleRenderer( const wxString &varianttype = GetDefaultType(),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;
};

// ---------------------------------------------------------
// wxDataViewCustomRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCustomRenderer: public wxDataViewRenderer
{
public:
    static wxString GetDefaultType() { return wxS("string"); }

    wxDataViewCustomRenderer( const wxString &varianttype = GetDefaultType(),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT,
                              bool no_init = false );
    virtual ~wxDataViewCustomRenderer();


    virtual bool Render( wxRect cell, wxDC *dc, int state ) = 0;

    void RenderText( const wxString &text, int xoffset,  wxRect cell, wxDC *dc, int state );

    virtual wxSize GetSize() const = 0;

    virtual bool Activate( wxRect WXUNUSED(cell),
                           wxDataViewModel *WXUNUSED(model), const wxDataViewItem &WXUNUSED(item), unsigned int WXUNUSED(col) )
                           { return false; }

    virtual bool LeftClick( wxPoint WXUNUSED(cursor), wxRect WXUNUSED(cell),
                           wxDataViewModel *WXUNUSED(model), const wxDataViewItem &WXUNUSED(item), unsigned int WXUNUSED(col) )
                           { return false; }
    virtual bool StartDrag( wxPoint WXUNUSED(cursor), wxRect WXUNUSED(cell),
                           wxDataViewModel *WXUNUSED(model), const wxDataViewItem &WXUNUSED(item), unsigned int WXUNUSED(col) )
                           { return false; }

    // Create DC on request
    virtual wxDC *GetDC();
};

// ---------------------------------------------------------
// wxDataViewProgressRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewProgressRenderer: public wxDataViewCustomRenderer
{
public:
    static wxString GetDefaultType() { return wxS("long"); }

    wxDataViewProgressRenderer( const wxString &label = wxEmptyString,
                                const wxString &varianttype = GetDefaultType(),
                                wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                int align = wxDVR_DEFAULT_ALIGNMENT );
    virtual ~wxDataViewProgressRenderer();

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    virtual bool Render( wxRect cell, wxDC *dc, int state );
    virtual wxSize GetSize() const;
};

// ---------------------------------------------------------
// wxDataViewIconTextRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewIconTextRenderer: public wxDataViewCustomRenderer
{
public:
    static wxString GetDefaultType() { return wxS("wxDataViewIconText"); }

    wxDataViewIconTextRenderer( const wxString &varianttype = GetDefaultType(),
                                wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                int align = wxDVR_DEFAULT_ALIGNMENT );
    virtual ~wxDataViewIconTextRenderer();

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    virtual bool Render( wxRect cell, wxDC *dc, int state );
    virtual wxSize GetSize() const;

    virtual bool HasEditorCtrl() const { return true; }
    virtual wxControl* CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value );
    virtual bool GetValueFromEditorCtrl( wxControl* editor, wxVariant &value );
};

// ---------------------------------------------------------
// wxDataViewDateRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewDateRenderer: public wxDataViewCustomRenderer
{
public:
    static wxString GetDefaultType() { return wxS("datetime"); }

    wxDataViewDateRenderer( const wxString &varianttype = GetDefaultType(),
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE,
                            int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    virtual bool Render( wxRect cell, wxDC *dc, int state );
    virtual wxSize GetSize() const;
    virtual bool Activate( wxRect cell,
                           wxDataViewModel *model, const wxDataViewItem &item, unsigned int col );

};

// -------------------------------------
// wxDataViewChoiceRenderer
// -------------------------------------

class WXDLLIMPEXP_ADV wxDataViewChoiceRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewChoiceRenderer( const wxArrayString &choices,
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                            int alignment = wxDVR_DEFAULT_ALIGNMENT );
    virtual bool Render( wxRect rect, wxDC *dc, int state );
    virtual wxSize GetSize() const;
    virtual bool SetValue( const wxVariant &value );
    virtual bool GetValue( wxVariant &value ) const;

    void SetAlignment( int align );
};

#endif // _WX_QT_DVRENDERERS_H_

