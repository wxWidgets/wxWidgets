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

#ifndef _WX_XH_PROPGRID_H_
#define _WX_XH_PROPGRID_H_

/*

  NOTE: relevant source file, xh_propgrid.cpp is *not* included in the
    wxPropertyGrid library (to prevent xrc-lib dependency). To use this
    code, you will need to separately add src/xh_propgrid.cpp to your
    application.

*/

#include "wx/xrc/xmlres.h"

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/manager.h"

#if wxUSE_XRC

class wxPropertyGridXmlHandler : public wxXmlResourceHandler
{
    friend class wxPropertyGridXrcPopulator;
    DECLARE_DYNAMIC_CLASS(wxPropertyGridXmlHandler)

public:
    wxPropertyGridXmlHandler();
    wxObject *DoCreateResource() override;
    bool CanHandle(wxXmlNode *node) override;

    void InitPopulator();
    void PopulatePage( wxPropertyGridState* state );
    void DonePopulator();

    void HandlePropertyGridParams();

private:
    wxPropertyGridManager*      m_manager;
    wxPropertyGrid*             m_pg;
    wxPropertyGridPopulator*    m_populator;
};

#endif // wxUSE_XRC

#endif // _WX_XH_PROPGRID_H_
