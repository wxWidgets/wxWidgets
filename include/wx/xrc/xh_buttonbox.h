/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_buttonbox.h
// Purpose:     XML resource handler for wxButtonBox
// Author:      Wlodzimierz ABX Skiba, Assisted-by: Claude Sonnet 5.5
// Created:     2026-11-14
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_BUTTONBOX_H_
#define _WX_XH_BUTTONBOX_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC && wxUSE_BUTTONBOX

// Include the full definition, not just a forward declaration, because
// wxButtonBox may be a macro for wxGenericButtonBox (see wx/buttonbox.h).
#include "wx/buttonbox.h"

class WXDLLIMPEXP_XRC wxButtonBoxXmlHandler : public wxXmlResourceHandler
{
    wxDECLARE_DYNAMIC_CLASS(wxButtonBoxXmlHandler);

public:
    wxButtonBoxXmlHandler();

    virtual wxObject *DoCreateResource() override;
    virtual bool CanHandle(wxXmlNode *node) override;

private:
    // True while the "segment" children of a wxButtonBox are being created.
    bool m_isInside = false;
    wxButtonBox *m_buttonBox = nullptr;

    wxDECLARE_NO_COPY_CLASS(wxButtonBoxXmlHandler);
};

#endif // wxUSE_XRC && wxUSE_BUTTONBOX

#endif // _WX_XH_BUTTONBOX_H_
