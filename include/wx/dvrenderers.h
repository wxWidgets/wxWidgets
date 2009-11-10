///////////////////////////////////////////////////////////////////////////////
// Name:        wx/dvrenderers.h
// Purpose:     Declare all wxDataViewCtrl classes
// Author:      Robert Roebling, Vadim Zeitlin
// Created:     2009-11-08 (extracted from wx/dataview.h)
// RCS-ID:      $Id: wxhead.h,v 1.11 2009-06-29 10:23:04 zeitlin Exp $
// Copyright:   (c) 2006 Robert Roebling
//              (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DVRENDERERS_H_
#define _WX_DVRENDERERS_H_

/*
    Note about the structure of various headers: they're organized in a more
    complicated way than usual because of the various dependencies which are
    different for different ports. In any case the only public header, i.e. the
    one which can be included directly is wx/dataview.h. It, in turn, includes
    this one to define all the renderer classes.

    We define the base wxDataViewRendererBase class first and then include a
    port-dependent wx/xxx/dvrenderer.h which defines wxDataViewRenderer itself.
    After this we can define wxDataViewRendererCustomBase (and maybe in the
    future base classes for other renderers if the need arises, i.e. if there
    is any non-trivial code or API which it makes sense to keep in common code)
    and include wx/xxx/dvrenderers.h (notice the plural) which defines all the
    rest of the renderer classes.
 */

class WXDLLIMPEXP_FWD_ADV wxDataViewCustomRenderer;

// ----------------------------------------------------------------------------
// wxDataViewIconText: helper class used by wxDataViewIconTextRenderer
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewIconText : public wxObject
{
public:
    wxDataViewIconText( const wxString &text = wxEmptyString,
                        const wxIcon& icon = wxNullIcon )
        : m_text(text),
          m_icon(icon)
    { }

    wxDataViewIconText( const wxDataViewIconText &other )
        : wxObject(),
          m_text(other.m_text),
          m_icon(other.m_icon)
    { }

    void SetText( const wxString &text ) { m_text = text; }
    wxString GetText() const             { return m_text; }
    void SetIcon( const wxIcon &icon )   { m_icon = icon; }
    const wxIcon &GetIcon() const        { return m_icon; }

private:
    wxString    m_text;
    wxIcon      m_icon;

    DECLARE_DYNAMIC_CLASS(wxDataViewIconText)
};

inline
bool operator==(const wxDataViewIconText& left, const wxDataViewIconText& right)
{
    return left.GetText() == right.GetText() &&
             left.GetIcon().IsSameAs(right.GetIcon());
}

inline
bool operator!=(const wxDataViewIconText& left, const wxDataViewIconText& right)
{
    return !(left == right);
}

DECLARE_VARIANT_OBJECT_EXPORTED(wxDataViewIconText, WXDLLIMPEXP_ADV)

// ----------------------------------------------------------------------------
// wxDataViewRendererBase
// ----------------------------------------------------------------------------

enum wxDataViewCellMode
{
    wxDATAVIEW_CELL_INERT,
    wxDATAVIEW_CELL_ACTIVATABLE,
    wxDATAVIEW_CELL_EDITABLE
};

enum wxDataViewCellRenderState
{
    wxDATAVIEW_CELL_SELECTED    = 1,
    wxDATAVIEW_CELL_PRELIT      = 2,
    wxDATAVIEW_CELL_INSENSITIVE = 4,
    wxDATAVIEW_CELL_FOCUSED     = 8
};

class WXDLLIMPEXP_ADV wxDataViewRendererBase: public wxObject
{
public:
    wxDataViewRendererBase( const wxString &varianttype,
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                            int alignment = wxDVR_DEFAULT_ALIGNMENT );
    virtual ~wxDataViewRendererBase();

    virtual bool Validate( wxVariant& WXUNUSED(value) )
        { return true; }

    void SetOwner( wxDataViewColumn *owner )    { m_owner = owner; }
    wxDataViewColumn* GetOwner() const          { return m_owner; }

    // renderer properties:

    virtual bool SetValue( const wxVariant& WXUNUSED(value) ) = 0;
    virtual bool GetValue( wxVariant& WXUNUSED(value) ) const = 0;

    wxString GetVariantType() const             { return m_variantType; }

    virtual void SetMode( wxDataViewCellMode mode ) = 0;
    virtual wxDataViewCellMode GetMode() const = 0;

    // NOTE: Set/GetAlignment do not take/return a wxAlignment enum but
    //       rather an "int"; that's because for rendering cells it's allowed
    //       to combine alignment flags (e.g. wxALIGN_LEFT|wxALIGN_BOTTOM)
    virtual void SetAlignment( int align ) = 0;
    virtual int GetAlignment() const = 0;

    // enable or disable (if called with wxELLIPSIZE_NONE) replacing parts of
    // the item text (hence this only makes sense for renderers showing
    // text...) with ellipsis in order to make it fit the column width
    virtual void EnableEllipsize(wxEllipsizeMode mode = wxELLIPSIZE_MIDDLE) = 0;
    void DisableEllipsize() { EnableEllipsize(wxELLIPSIZE_NONE); }

    virtual wxEllipsizeMode GetEllipsizeMode() const = 0;

    // in-place editing
    virtual bool HasEditorCtrl() const
        { return false; }
    virtual wxControl* CreateEditorCtrl(wxWindow * WXUNUSED(parent),
                                        wxRect WXUNUSED(labelRect),
                                        const wxVariant& WXUNUSED(value))
        { return NULL; }
    virtual bool GetValueFromEditorCtrl(wxControl * WXUNUSED(editor),
                                        wxVariant& WXUNUSED(value))
        { return false; }

    virtual bool StartEditing( const wxDataViewItem &item, wxRect labelRect );
    virtual void CancelEditing();
    virtual bool FinishEditing();

    wxControl *GetEditorCtrl() { return m_editorCtrl; }

protected:
    wxString                m_variantType;
    wxDataViewColumn       *m_owner;
    wxWeakRef<wxControl>    m_editorCtrl;
    wxDataViewItem          m_item; // for m_editorCtrl

    // internal utility:
    const wxDataViewCtrl* GetView() const;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewRendererBase)
};

// include the real wxDataViewRenderer declaration for the native ports
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    // in the generic implementation there is no real wxDataViewRenderer, all
    // renderers are custom so it's the same as wxDataViewCustomRenderer and
    // wxDataViewCustomRendererBase derives from wxDataViewRendererBase directly
    //
    // this is a rather ugly hack but unfortunately it just doesn't seem to be
    // possible to have the same class hierarchy in all ports and avoid
    // duplicating the entire wxDataViewCustomRendererBase in the generic
    // wxDataViewRenderer class (well, we could use a mix-in but this would
    // make classes hierarchy non linear and arguably even more complex)
    #define wxDataViewCustomRendererRealBase wxDataViewRendererBase
#else
    #if defined(__WXGTK20__)
        #include "wx/gtk/dvrenderer.h"
    #elif defined(__WXMAC__)
        #include "wx/osx/dvrenderer.h"
    #else
        #error "unknown native wxDataViewCtrl implementation"
    #endif
    #define wxDataViewCustomRendererRealBase wxDataViewRenderer
#endif

// ----------------------------------------------------------------------------
// wxDataViewCustomRendererBase
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCustomRendererBase
    : public wxDataViewCustomRendererRealBase
{
public:
    // Constructor must specify the usual renderer parameters which we simply
    // pass to the base class
    wxDataViewCustomRendererBase(const wxString& varianttype = "string",
                                 wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                 int align = wxDVR_DEFAULT_ALIGNMENT)
        : wxDataViewCustomRendererRealBase(varianttype, mode, align)
    {
    }


    // Define virtual function which are called when the item is activated
    // (double-clicked or Enter is pressed on it), clicked or the user starts
    // to drag it: by default they all simply return false indicating that the
    // events are not handled

    virtual bool Activate(wxRect WXUNUSED(cell),
                          wxDataViewModel *WXUNUSED(model),
                          const wxDataViewItem & WXUNUSED(item),
                          unsigned int WXUNUSED(col))
        { return false; }

    virtual bool LeftClick(wxPoint WXUNUSED(cursor),
                           wxRect WXUNUSED(cell),
                           wxDataViewModel *WXUNUSED(model),
                           const wxDataViewItem & WXUNUSED(item),
                           unsigned int WXUNUSED(col) )
        { return false; }

    virtual bool StartDrag(wxPoint WXUNUSED(cursor),
                           wxRect WXUNUSED(cell),
                           wxDataViewModel *WXUNUSED(model),
                           const wxDataViewItem & WXUNUSED(item),
                           unsigned int WXUNUSED(col) )
        { return false; }


private:
    wxDECLARE_NO_COPY_CLASS(wxDataViewCustomRendererBase);
};

// include the declaration of all the other renderers to get the real
// wxDataViewCustomRenderer from which we need to inherit below
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    // because of the different renderer classes hierarchy in the generic
    // version, as explained above, we can include the header defining
    // wxDataViewRenderer only here and not before wxDataViewCustomRendererBase
    // declaration as for the native ports
    #include "wx/generic/dvrenderer.h"
    #include "wx/generic/dvrenderers.h"
#elif defined(__WXGTK20__)
    #include "wx/gtk/dvrenderers.h"
#elif defined(__WXMAC__)
    #include "wx/osx/dvrenderers.h"
#else
    #error "unknown native wxDataViewCtrl implementation"
#endif

// ----------------------------------------------------------------------------
// wxDataViewSpinRenderer
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewSpinRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewSpinRenderer( int min, int max,
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                            int alignment = wxDVR_DEFAULT_ALIGNMENT );
    virtual bool HasEditorCtrl() const { return true; }
    virtual wxControl* CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value );
    virtual bool GetValueFromEditorCtrl( wxControl* editor, wxVariant &value );
    virtual bool Render( wxRect rect, wxDC *dc, int state );
    virtual wxSize GetSize() const;
    virtual bool SetValue( const wxVariant &value );
    virtual bool GetValue( wxVariant &value ) const;

private:
    long    m_data;
    long    m_min,m_max;
};

#if defined(wxHAS_GENERIC_DATAVIEWCTRL) || defined(__WXOSX_CARBON__)

// ----------------------------------------------------------------------------
// wxDataViewChoiceRenderer
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewChoiceRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewChoiceRenderer( const wxArrayString &choices,
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                            int alignment = wxDVR_DEFAULT_ALIGNMENT );
    virtual bool HasEditorCtrl() const { return true; }
    virtual wxControl* CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value );
    virtual bool GetValueFromEditorCtrl( wxControl* editor, wxVariant &value );
    virtual bool Render( wxRect rect, wxDC *dc, int state );
    virtual wxSize GetSize() const;
    virtual bool SetValue( const wxVariant &value );
    virtual bool GetValue( wxVariant &value ) const;

private:
    wxArrayString m_choices;
    wxString      m_data;
};

#endif // generic or Carbon versions

// this class is obsolete, its functionality was merged in
// wxDataViewTextRenderer itself now, don't use it any more
#define wxDataViewTextRendererAttr wxDataViewTextRenderer

#endif // _WX_DVRENDERERS_H_

