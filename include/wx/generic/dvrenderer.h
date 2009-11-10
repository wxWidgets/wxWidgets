///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/dvrenderer.h
// Purpose:     wxDataViewRenderer for generic wxDataViewCtrl implementation
// Author:      Robert Roebling, Vadim Zeitlin
// Created:     2009-11-07 (extracted from wx/generic/dataview.h)
// RCS-ID:      $Id: wxhead.h,v 1.11 2009-06-29 10:23:04 zeitlin Exp $
// Copyright:   (c) 2006 Robert Roebling
//              (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_DVRENDERER_H_
#define _WX_GENERIC_DVRENDERER_H_

// ----------------------------------------------------------------------------
// wxDataViewRenderer
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewRenderer: public wxDataViewCustomRendererBase
{
public:
    wxDataViewRenderer( const wxString &varianttype,
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                        int align = wxDVR_DEFAULT_ALIGNMENT );
    virtual ~wxDataViewRenderer();

    // these methods are used to draw the cell contents, Render() doesn't care
    // about the attributes while RenderWithAttr() does -- override it if you
    // want to take the attributes defined for this cell into account, otherwise
    // overriding Render() is enough
    virtual bool Render( wxRect cell, wxDC *dc, int state ) = 0;

    // NB: RenderWithAttr() also has more standard parameter order and types
    virtual bool
    RenderWithAttr(wxDC& dc,
                   const wxRect& rect,
                   int align,   // combination of horizontal and vertical
                   const wxDataViewItemAttr *attr, // may be NULL if none
                   int state);

    virtual wxSize GetSize() const = 0;
    virtual wxDC *GetDC();

    // Draw the text using the provided attributes
    void RenderText(wxDC& dc,
                    const wxRect& rect,
                    int align,
                    const wxString& text,
                    const wxDataViewItemAttr *attr, // may be NULL if none
                    int state,
                    int xoffset = 0);

    // Overload using standard attributes
    void RenderText(const wxString& text,
                    int xoffset,
                    wxRect cell,
                    wxDC *dc,
                    int state)
    {
        RenderText(*dc, cell, wxALIGN_NOT, text, NULL, state, xoffset);
    }


    virtual void SetAlignment( int align );
    virtual int GetAlignment() const;

    virtual void EnableEllipsize(wxEllipsizeMode mode = wxELLIPSIZE_MIDDLE)
        { m_ellipsizeMode = mode; }
    virtual wxEllipsizeMode GetEllipsizeMode() const
        { return m_ellipsizeMode; }

    virtual void SetMode( wxDataViewCellMode mode )
        { m_mode = mode; }
    virtual wxDataViewCellMode GetMode() const
        { return m_mode; }

    // implementation
    int CalculateAlignment() const;

    // this is a replacement for dynamic_cast<wxDataViewCustomRenderer> in the
    // code checking whether the renderer is interested in mouse events, it's
    // overridden in wxDataViewCustomRenderer to return the object itself but
    // intentionally returns NULL for all the other renderer classes as the
    // user should _not_ be able to override Activate/LeftClick() when deriving
    // from them for consistency with the other ports and while we can't
    // prevent this from working at compile-time because all renderer are
    // custom renderers in the generic implementation, we at least make sure
    // that it doesn't work at run-time because Activate/LeftClick() would
    // never be called
    virtual wxDataViewCustomRenderer *WXGetAsCustom() { return NULL; }

protected:
    // This is just a convenience for the derived classes overriding
    // RenderWithAttr() to avoid repeating the same wxFAIL_MSG() in all of them
    bool DummyRender(wxRect WXUNUSED(cell),
                     wxDC * WXUNUSED(dc),
                     int WXUNUSED(state))
    {
        wxFAIL_MSG("shouldn't be called at all, use RenderWithAttr() instead");

        return false;
    }

private:
    int                          m_align;
    wxDataViewCellMode           m_mode;

    wxEllipsizeMode m_ellipsizeMode;

    wxDC *m_dc;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewRenderer)
};

#endif // _WX_GENERIC_DVRENDERER_H_

