///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/dvrenderer.h
// Purpose:     wxDataViewRenderer for GTK wxDataViewCtrl implementation
// Author:      Robert Roebling, Vadim Zeitlin
// Created:     2009-11-07 (extracted from wx/gtk/dataview.h)
// RCS-ID:      $Id: wxhead.h,v 1.11 2009-06-29 10:23:04 zeitlin Exp $
// Copyright:   (c) 2006 Robert Roebling
//              (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_DVRENDERER_H_
#define _WX_GTK_DVRENDERER_H_

typedef struct _GtkTreeViewColumn GtkTreeViewColumn;

// ----------------------------------------------------------------------------
// wxDataViewRenderer
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewRenderer: public wxDataViewRendererBase
{
public:
    wxDataViewRenderer( const wxString &varianttype,
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                        int align = wxDVR_DEFAULT_ALIGNMENT );

    virtual void SetMode( wxDataViewCellMode mode );
    virtual wxDataViewCellMode GetMode() const;

    virtual void SetAlignment( int align );
    virtual int GetAlignment() const;

    virtual void EnableEllipsize(wxEllipsizeMode mode = wxELLIPSIZE_MIDDLE);
    virtual wxEllipsizeMode GetEllipsizeMode() const;

    // GTK-specific implementation
    // ---------------------------

    // pack the GTK cell renderers used by this renderer to the given column
    //
    // by default only a single m_renderer is used but some renderers use more
    // than one GTK cell renderer
    virtual void GtkPackIntoColumn(GtkTreeViewColumn *column);

    // called when the cell value was edited by user with the new value
    //
    // it validates the new value and notifies the model about the change by
    // calling GtkOnCellChanged() if it was accepted
    void GtkOnTextEdited(const gchar *itempath, const wxString& value);

    GtkCellRenderer* GetGtkHandle() { return m_renderer; }
    void GtkInitHandlers();
    void GtkUpdateAlignment();

    bool GtkIsUsingDefaultAttrs() const { return m_usingDefaultAttrs; }
    void GtkSetUsingDefaultAttrs(bool def) { m_usingDefaultAttrs = def; }

protected:
    virtual void GtkOnCellChanged(const wxVariant& value,
                                  const wxDataViewItem& item,
                                  unsigned col);


    GtkCellRenderer   *m_renderer;
    int                m_alignment;

    // true if we hadn't changed any visual attributes or restored them since
    // doing this
    bool m_usingDefaultAttrs;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewRenderer)
};

#endif // _WX_GTK_DVRENDERER_H_

