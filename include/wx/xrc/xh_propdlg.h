/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_propdlg.h
// Purpose:     XML resource handler for wxPropertySheetDialog
// Author:      Sander Berents
// Created:     2007/07/12
// Copyright:   (c) 2007 Sander Berents
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_PROPDLG_H_
#define _WX_XH_PROPDLG_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC && wxUSE_BOOKCTRL

class WXDLLIMPEXP_FWD_CORE wxPropertySheetDialog;

class WXDLLIMPEXP_XRC wxPropertySheetDialogXmlHandler : public wxXmlResourceHandler
{
    wxDECLARE_DYNAMIC_CLASS(wxPropertySheetDialogXmlHandler);

public:
    wxPropertySheetDialogXmlHandler();
    virtual wxObject *DoCreateResource() wxOVERRIDE;
    virtual bool CanHandle(wxXmlNode *node) wxOVERRIDE;

private:
    bool m_isInside;
    wxPropertySheetDialog *m_dialog;
    wxVector<wxBitmapBundle> m_bookImages;
    wxVector<size_t> m_bookImagesIdx;
};

#endif // wxUSE_XRC && wxUSE_BOOKCTRL

#endif // _WX_XH_PROPDLG_H_
