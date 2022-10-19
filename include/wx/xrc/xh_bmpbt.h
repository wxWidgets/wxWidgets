/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_bmpbt.h
// Purpose:     XML resource handler for bitmap buttons
// Author:      Brian Gavin
// Created:     2000/03/05
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_BMPBT_H_
#define _WX_XH_BMPBT_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC && wxUSE_BMPBUTTON

#include "wx/bmpbuttn.h"

class WXDLLIMPEXP_XRC wxBitmapButtonXmlHandler : public wxXmlResourceHandler
{
    wxDECLARE_DYNAMIC_CLASS(wxBitmapButtonXmlHandler);

public:
    wxBitmapButtonXmlHandler();
    virtual wxObject *DoCreateResource() override;
    virtual bool CanHandle(wxXmlNode *node) override;

private:
    typedef void (wxBitmapButton::*BitmapSetter)(const wxBitmapBundle&);

    void SetBitmapIfSpecified(wxBitmapButton* button,
                              BitmapSetter setter,
                              const char* paramName,
                              const char* paramNameAlt = nullptr);
};

#endif // wxUSE_XRC && wxUSE_BMPBUTTON

#endif // _WX_XH_BMPBT_H_
