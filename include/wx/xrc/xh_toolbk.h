/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_toolbk.h
// Purpose:     XML resource handler for wxToolbook
// Author:      Andrea Zanellato
// Created:     2009/12/12
// Copyright:   (c) 2010 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_TOOLBK_H_
#define _WX_XH_TOOLBK_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC && wxUSE_TOOLBOOK

class WXDLLIMPEXP_FWD_CORE wxToolbook;

class WXDLLIMPEXP_XRC wxToolbookXmlHandler : public wxXmlResourceHandler
{
public:
    wxToolbookXmlHandler();

    virtual wxObject *DoCreateResource() wxOVERRIDE;
    virtual bool CanHandle(wxXmlNode *node) wxOVERRIDE;

private:
    struct newPage
    {
        wxWindow* wnd = NULL;
        wxString label;
        bool selected = false;
        int imgId = -1; // index in image list
        int bmpId = -1; // index in wxVector<wxBitmapBundle>
    };

    bool m_isInside;
    wxToolbook *m_toolbook;
    wxVector<newPage> m_bookPages;
    wxVector<wxBitmapBundle> m_bookImages;

    wxDECLARE_DYNAMIC_CLASS(wxToolbookXmlHandler);
};

#endif // wxUSE_XRC && wxUSE_TOOLBOOK

#endif // _WX_XH_TOOLBK_H_
