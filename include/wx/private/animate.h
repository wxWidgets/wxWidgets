/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/animate.h
// Purpose:     wxAnimationImpl declaration
// Author:      Robin Dunn, Vadim Zeitlin
// Created:     2020-04-06
// Copyright:   (c) 2020 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_ANIMATEH__
#define _WX_PRIVATE_ANIMATEH__

// ----------------------------------------------------------------------------
// wxAnimationImpl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxAnimationImpl : public wxRefCounter
{
public:
    wxAnimationImpl() {}
    virtual ~wxAnimationImpl() {}

    virtual bool IsOk() const = 0;
    virtual bool IsCompatibleWith(wxClassInfo* ci) const = 0;

    // can be -1
    virtual int GetDelay(unsigned int frame) const = 0;

    virtual unsigned int GetFrameCount() const = 0;
    virtual wxImage GetFrame(unsigned int frame) const = 0;
    virtual wxSize GetSize() const = 0;

    virtual bool LoadFile(const wxString& name,
                          wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;
    virtual bool Load(wxInputStream& stream,
                      wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;

    // This function creates the default implementation for this platform:
    // currently it's wxAnimationGTKImpl under wxGTK and wxAnimationGenericImpl
    // under all the other platforms.
    static wxAnimationImpl *CreateDefault();
};

#endif // _WX_PRIVATE_ANIMATEH__
