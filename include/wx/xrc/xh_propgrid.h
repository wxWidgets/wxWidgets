/////////////////////////////////////////////////////////////////////////////
// Name:        xh_propgrid.h
// Purpose:     XML resource handler for wxPropertyGrid
// Author:      Jaakko Salli
// Modified by:
// Created:     May-16-2007
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XRC_XH_PROPGRID_H_
#define _WX_XRC_XH_PROPGRID_H_

/*

  NOTE: relevant source file, xh_propgrid.cpp is *not* included in the
    wxPropertyGrid library (to prevent xrc-lib dependency). To use this
    code, you will need to separately add src/xh_propgrid.cpp to your
    application.

*/

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC && wxUSE_PROPGRID

class WXDLLIMPEXP_FWD_PROPGRID wxPropertyGrid;
class WXDLLIMPEXP_FWD_PROPGRID wxPropertyGridManager;
class WXDLLIMPEXP_FWD_PROPGRID wxPropertyGridPageState;
class WXDLLIMPEXP_FWD_PROPGRID wxPropertyGridPopulator;

class WXDLLIMPEXP_PROPGRID wxPropertyGridXmlHandler : public wxXmlResourceHandler
{
    friend class wxPropertyGridXrcPopulator;
    DECLARE_DYNAMIC_CLASS(wxPropertyGridXmlHandler)

public:
    wxPropertyGridXmlHandler();
    wxObject *DoCreateResource() override;
    bool CanHandle(wxXmlNode *node) override;

    void InitPopulator();
    void PopulatePage( wxPropertyGridPageState* state );
    void DonePopulator();

    void HandlePropertyGridParams();

private:
    wxPropertyGridManager*      m_manager = nullptr;
    wxPropertyGrid*             m_pg = nullptr;
    wxPropertyGridPopulator*    m_populator = nullptr;
};

#endif // wxUSE_XRC && wxUSE_PROPGRID

#endif // _WX_XRC_XH_PROPGRID_H_
