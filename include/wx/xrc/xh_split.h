/////////////////////////////////////////////////////////////////////////////
// Name:        xh_split.h
// Purpose:     XRC resource for wxSplitterWindow
// Author:      panga@freemail.hu, Vaclav Slavik
// Created:     2003/01/26
// RCS-ID:      $Id$
// Copyright:   (c) 2003 panga@freemail.hu, Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_SPLIT_H_
#define _WX_XH_SPLIT_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "xh_split.h"
#endif

#include "wx/xrc/xmlres.h"

class WXDLLIMPEXP_XRC wxSplitterWindowXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxSplitterWindowXmlHandler)
public:
    wxSplitterWindowXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif // _WX_XH_SPLIT_H_
