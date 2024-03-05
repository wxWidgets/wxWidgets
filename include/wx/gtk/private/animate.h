/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/animate.h
// Purpose:     Animation classes
// Author:      Julian Smart and Guillermo Rodriguez Garcia
// Modified by: Francesco Montorsi
// Created:     13/8/99
// Copyright:   (c) Julian Smart and Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_ANIMATEH__
#define _WX_GTK_PRIVATE_ANIMATEH__

#include "wx/private/animate.h"

typedef struct _GdkPixbufAnimation GdkPixbufAnimation;
typedef struct _GdkPixbufAnimationIter GdkPixbufAnimationIter;

// ----------------------------------------------------------------------------
// wxAnimationGTKImpl
// Unlike the generic wxAnimation object we won't use directly
// wxAnimationDecoders as gdk-pixbuf already provides the concept of decoder and
// will automatically use the available handlers.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxAnimationGTKImpl : public wxAnimationImpl
{
public:
    wxAnimationGTKImpl()
        : m_pixbuf(nullptr) {}
    ~wxAnimationGTKImpl() { UnRef(); }

    virtual bool IsOk() const override
        { return m_pixbuf != nullptr; }
    virtual bool IsCompatibleWith(wxClassInfo* ci) const override;


    // unfortunately GdkPixbufAnimation does not expose these info:

    virtual unsigned int GetFrameCount() const override { return 0; }
    virtual wxImage GetFrame(unsigned int frame) const override;

    // we can retrieve the delay for a frame only after building
    // a GdkPixbufAnimationIter...
    virtual int GetDelay(unsigned int WXUNUSED(frame)) const override { return 0; }
    virtual wxSize GetSize() const override;

    virtual bool LoadFile(const wxString &name, wxAnimationType type = wxANIMATION_TYPE_ANY) override;
    virtual bool Load(wxInputStream &stream, wxAnimationType type = wxANIMATION_TYPE_ANY) override;

    // Implementation
public:     // used by GTK callbacks

    GdkPixbufAnimation *GetPixbuf() const
        { return m_pixbuf; }
    void SetPixbuf(GdkPixbufAnimation* p);

protected:
    GdkPixbufAnimation *m_pixbuf;

private:
    void UnRef();

    typedef wxAnimationImpl base_type;
    wxDECLARE_NO_COPY_CLASS(wxAnimationGTKImpl);
};


#endif // _WX_GTK_PRIVATE_ANIMATEH__
