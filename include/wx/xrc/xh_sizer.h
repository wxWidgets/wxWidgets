/////////////////////////////////////////////////////////////////////////////
// Name:        xh_sizer.h
// Purpose:     XML resource handler for wxBoxSizer
// Author:      Vaclav Slavik
// Created:     2000/04/24
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_SIZER_H_
#define _WX_XH_SIZER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "xh_sizer.h"
#endif

#include "wx/xrc/xmlres.h"
#include "wx/sizer.h"
#include "wx/gbsizer.h"


class WXDLLEXPORT wxSizer;

class WXDLLIMPEXP_XRC wxSizerXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxSizerXmlHandler)
public:
    wxSizerXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);

private:
    bool m_isInside;
    bool m_isGBS;

    wxSizer *m_parentSizer;

    bool IsSizerNode(wxXmlNode *node);

    wxObject* Handle_sizeritem();
    wxObject* Handle_spacer();
    wxObject* Handle_sizer();
    wxSizer*  Handle_wxBoxSizer();
    wxSizer*  Handle_wxStaticBoxSizer();
    wxSizer*  Handle_wxGridSizer();
    wxSizer*  Handle_wxFlexGridSizer();
    wxSizer*  Handle_wxGridBagSizer();

    void SetGrowables(wxFlexGridSizer* fsizer, const wxChar* param, bool rows);
    wxGBPosition GetGBPos(const wxString& param);
    wxGBSpan GetGBSpan(const wxString& param);
    wxSizerItem* MakeSizerItem();
    void SetSizerItemAttributes(wxSizerItem* sitem);
    void AddSizerItem(wxSizerItem* sitem);
};


#endif // _WX_XH_BOXSIZER_H_
