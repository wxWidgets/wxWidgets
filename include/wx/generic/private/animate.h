/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/private/animate.h
// Purpose:     wxAnimationGenericImpl
// Author:      Julian Smart and Guillermo Rodriguez Garcia
// Modified by: Francesco Montorsi
// Created:     13/8/99
// Copyright:   (c) Julian Smart and Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_PRIVATE_ANIMATEH__
#define _WX_GENERIC_PRIVATE_ANIMATEH__

#include "wx/private/animate.h"

// ----------------------------------------------------------------------------
// wxAnimationGenericImpl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxAnimationGenericImpl : public wxAnimationImpl
{
public:
    wxAnimationGenericImpl() : m_decoder(NULL) {}
    virtual ~wxAnimationGenericImpl() { UnRef(); }

    virtual bool IsOk() const wxOVERRIDE
        { return m_decoder != NULL; }
    virtual bool IsCompatibleWith(wxClassInfo* ci) const wxOVERRIDE;

    virtual unsigned int GetFrameCount() const wxOVERRIDE;
    virtual int GetDelay(unsigned int i) const wxOVERRIDE;
    virtual wxImage GetFrame(unsigned int i) const wxOVERRIDE;
    virtual wxSize GetSize() const wxOVERRIDE;

    virtual bool LoadFile(const wxString& filename,
                          wxAnimationType type = wxANIMATION_TYPE_ANY) wxOVERRIDE;
    virtual bool Load(wxInputStream& stream,
                      wxAnimationType type = wxANIMATION_TYPE_ANY) wxOVERRIDE;

    // extended interface used only by the generic implementation of wxAnimationCtrl
    virtual wxPoint GetFramePosition(unsigned int frame) const;
    virtual wxSize GetFrameSize(unsigned int frame) const;
    virtual wxAnimationDisposal GetDisposalMethod(unsigned int frame) const;
    virtual wxColour GetTransparentColour(unsigned int frame) const;
    virtual wxColour GetBackgroundColour() const;

private:
    void UnRef();

    wxAnimationDecoder* m_decoder;

    wxDECLARE_NO_COPY_CLASS(wxAnimationGenericImpl);
};


#endif // _WX_GENERIC_PRIVATE_ANIMATEH__
