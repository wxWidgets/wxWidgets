///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/textmeasure.h
// Purpose:     wxMSW-specific declaration of wxTextMeasure class
// Author:      Manuel Martin
// Created:     2012-10-05
// Copyright:   (c) 1997-2012 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_TEXTMEASURE_H_
#define _WX_MSW_PRIVATE_TEXTMEASURE_H_

#include "wx/msw/wrapwin.h"

// ----------------------------------------------------------------------------
// wxTextMeasure for MSW.
// ----------------------------------------------------------------------------

class wxTextMeasure : public wxTextMeasureBase
{
public:
    explicit wxTextMeasure(const wxDC *dc, const wxFont *font = nullptr)
        : wxTextMeasureBase(dc, font)
    {
        Init();
    }

    explicit wxTextMeasure(const wxWindow *win, const wxFont *font = nullptr)
        : wxTextMeasureBase(win, font)
    {
        Init();
    }

protected:
    void Init();

    virtual void BeginMeasuring() override;
    virtual void EndMeasuring() override;

    virtual void DoGetTextExtent(const wxString& string,
                               wxCoord *width,
                               wxCoord *height,
                               wxCoord *descent = nullptr,
                               wxCoord *externalLeading = nullptr) override;

    virtual bool DoGetPartialTextExtents(const wxString& text,
                                         wxArrayInt& widths,
                                         double scaleX) override;



    // We use either the HDC of the provided wxDC or an HDC created for our
    // window.
    HDC m_hdc;

    // If we change the font in BeginMeasuring(), we restore it to the old one
    // in EndMeasuring().
    HFONT m_hfontOld;

    wxDECLARE_NO_COPY_CLASS(wxTextMeasure);
};

#endif // _WX_MSW_PRIVATE_TEXTMEASURE_H_
