/////////////////////////////////////////////////////////////////////////////
// Name:        xh_notbk.h
// Purpose:     XML resource handler for wxNotebook
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_NOTBK_H_
#define _WX_XH_NOTBK_H_

#ifdef __GNUG__
#pragma interface "xh_notbk.h"
#endif

#include "wx/xml/xmlres.h"

#if wxUSE_NOTEBOOK

class WXDLLEXPORT wxNotebook;

class WXDLLEXPORT wxNotebookXmlHandler : public wxXmlResourceHandler
{
    public:
        wxNotebookXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
        
    private:
        bool m_IsInside;
        wxNotebook *m_Notebook;
};

#endif

#endif // _WX_XH_NOTBK_H_
