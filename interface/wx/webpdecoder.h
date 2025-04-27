/////////////////////////////////////////////////////////////////////////////
// Name:        webpdecoder.h
// Purpose:     interface of wxWebPDecoder
// Author:      Maarten Bent
// Created:     2025-04-05
// Copyright:   (c) Maarten Bent
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
   @class wxWebPDecoder

    @since 3.3.0

   An animation decoder supporting animated WebP files.
*/
class  wxWebPDecoder : public wxAnimationDecoder
{
public:
    wxWebPDecoder();
    ~wxWebPDecoder();

    virtual wxSize GetFrameSize(unsigned int frame) const;
    virtual wxPoint GetFramePosition(unsigned int frame) const;
    virtual wxAnimationDisposal GetDisposalMethod(unsigned int frame) const;
    virtual long GetDelay(unsigned int frame) const;
    virtual wxColour GetTransparentColour(unsigned int frame) const;
    virtual bool Load(wxInputStream& stream);
    virtual bool ConvertToImage(unsigned int frame, wxImage* image) const;
    virtual wxAnimationDecoder* Clone() const;
    virtual wxAnimationType GetType() const;

protected:
    virtual bool DoCanRead(wxInputStream& stream) const;
};
