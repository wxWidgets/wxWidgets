/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_styledtextctrl.h
// Purpose:     XML resource handler for wxStyledTextCtrl
// Author:      Alexander Koshelev
// Created:     2021-09-22
// Copyright:   (c) 2021 TT-Solutions SARL
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_STYLEDTEXTCTRL_H_
#define _WX_XH_STYLEDTEXTCTRL_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC && wxUSE_STC

class WXDLLIMPEXP_STC wxStyledTextCtrlXmlHandler : public wxXmlResourceHandler
{
    wxDECLARE_DYNAMIC_CLASS(wxStyledTextCtrlXmlHandler);

public:
    wxStyledTextCtrlXmlHandler();
    virtual wxObject *DoCreateResource() override;
    virtual bool CanHandle(wxXmlNode *node) override;
};

#endif // wxUSE_XRC && wxUSE_STC

#endif // _WX_XH_STYLEDTEXTCTRL_H_
