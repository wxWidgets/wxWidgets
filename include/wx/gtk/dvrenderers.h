///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/dvrenderers.h
// Purpose:     All GTK wxDataViewCtrl renderer classes
// Author:      Robert Roebling, Vadim Zeitlin
// Created:     2009-11-07 (extracted from wx/gtk/dataview.h)
// Copyright:   (c) 2006 Robert Roebling
//              (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_DVRENDERERS_H_
#define _WX_GTK_DVRENDERERS_H_

#ifdef __WXGTK3__
    typedef struct _cairo_rectangle_int cairo_rectangle_int_t;
    typedef cairo_rectangle_int_t GdkRectangle;
#else
    typedef struct _GdkRectangle GdkRectangle;
#endif

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

#if wxUSE_MARKUP
    void EnableMarkup(bool enable = true);
#endif // wxUSE_MARKUP

    virtual bool SetValue( const wxVariant &value ) override
    {
        return SetTextValue(value);
    }

    virtual bool GetValue( wxVariant &value ) const override
    {
        wxString str;
        if ( !GetTextValue(str) )
            return false;

        value = str;

        return true;
    }

    virtual void GtkUpdateAlignment() override;

    virtual GtkCellRendererText *GtkGetTextRenderer() const override;

protected:
    virtual void SetAttr(const wxDataViewItemAttr& attr) override;

    // implementation of Set/GetValue()
    bool SetTextValue(const wxString& str);
    bool GetTextValue(wxString& str) const;

    // Return the name of the GtkCellRendererText property to use: "text" or
    // "markup".
    const char* GetTextPropertyName() const;

#if wxUSE_MARKUP
    // True if we should interpret markup in our text.
    bool m_useMarkup;
#endif // wxUSE_MARKUP

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewTextRenderer);
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

    bool SetValue( const wxVariant &value ) override;
    bool GetValue( wxVariant &value ) const override;

    virtual
    bool IsCompatibleVariantType(const wxString& variantType) const override;

protected:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewBitmapRenderer);
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

    void ShowAsRadio();

    bool SetValue( const wxVariant &value ) override;
    bool GetValue( wxVariant &value ) const override;

protected:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewToggleRenderer);
};

// ---------------------------------------------------------
// wxDataViewCustomRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCustomRenderer: public wxDataViewCustomRendererBase
{
public:
    static wxString GetDefaultType() { return wxS("string"); }

    wxDataViewCustomRenderer( const wxString &varianttype = GetDefaultType(),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT,
                              bool no_init = false );
    virtual ~wxDataViewCustomRenderer();


    // Create DC on request
    virtual wxDC *GetDC() override;

    // override the base class function to use GTK text cell renderer
    virtual void RenderText(const wxString& text,
                            int xoffset,
                            wxRect cell,
                            wxDC *dc,
                            int state) override;

    struct GTKRenderParams;

    // store GTK render call parameters for possible later use
    void GTKSetRenderParams(GTKRenderParams* renderParams)
    {
        m_renderParams = renderParams;
    }

    virtual GtkCellRendererText *GtkGetTextRenderer() const override;
    virtual GtkWidget* GtkGetEditorWidget() const override;

    virtual void GtkUpdateAlignment() override;

private:
    bool Init(wxDataViewCellMode mode, int align);

    // Called from GtkGetTextRenderer() to really create the renderer if
    // necessary.
    void GtkInitTextRenderer();

    wxDC        *m_dc;

    GtkCellRendererText      *m_text_renderer;

    // parameters of the original render() call stored so that we could pass
    // them forward to m_text_renderer if our RenderText() is called
    GTKRenderParams* m_renderParams;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCustomRenderer);
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

    bool SetValue( const wxVariant &value ) override;
    bool GetValue( wxVariant &value ) const override;

    virtual bool Render( wxRect cell, wxDC *dc, int state ) override;
    virtual wxSize GetSize() const override;

private:
    void GTKSetLabel();

    wxString    m_label;
    int         m_value;

protected:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewProgressRenderer);
};

// ---------------------------------------------------------
// wxDataViewIconTextRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewIconTextRenderer: public wxDataViewTextRenderer
{
    typedef wxDataViewTextRenderer BaseType;
public:
    static wxString GetDefaultType() { return wxS("wxDataViewIconText"); }

    wxDataViewIconTextRenderer( const wxString &varianttype = GetDefaultType(),
                                wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                int align = wxDVR_DEFAULT_ALIGNMENT );
    virtual ~wxDataViewIconTextRenderer();

    bool SetValue( const wxVariant &value ) override;
    bool GetValue( wxVariant &value ) const override;

    virtual void GtkPackIntoColumn(GtkTreeViewColumn *column) override;

protected:
    virtual void SetAttr(const wxDataViewItemAttr& attr) override;
    virtual wxVariant GtkGetValueFromString(const wxString& str) const override;

private:
    wxDataViewIconText   m_value;

    // we use the base class m_renderer for the text and this one for the icon
    GtkCellRenderer *m_rendererIcon;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewIconTextRenderer);
};

// -------------------------------------
// wxDataViewChoiceRenderer
// -------------------------------------

class WXDLLIMPEXP_ADV wxDataViewChoiceRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewChoiceRenderer(const wxArrayString &choices,
                             wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                             int alignment = wxDVR_DEFAULT_ALIGNMENT );
    virtual bool Render( wxRect rect, wxDC *dc, int state ) override;
    virtual wxSize GetSize() const override;
    virtual bool SetValue( const wxVariant &value ) override;
    virtual bool GetValue( wxVariant &value ) const override;

    virtual void GtkUpdateAlignment() override;

    wxString GetChoice(size_t index) const { return m_choices[index]; }
    const wxArrayString& GetChoices() const { return m_choices; }

private:
    wxArrayString m_choices;
    wxString      m_data;
};

// ----------------------------------------------------------------------------
// wxDataViewChoiceByIndexRenderer
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewChoiceByIndexRenderer: public wxDataViewChoiceRenderer
{
public:
    wxDataViewChoiceByIndexRenderer( const wxArrayString &choices,
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                              int alignment = wxDVR_DEFAULT_ALIGNMENT );

    virtual bool SetValue( const wxVariant &value ) override;
    virtual bool GetValue( wxVariant &value ) const override;

private:
    virtual wxVariant GtkGetValueFromString(const wxString& str) const override;
};



#endif // _WX_GTK_DVRENDERERS_H_

