/////////////////////////////////////////////////////////////////////////////
// Purpose:     XML resources editor
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "preview.h"
#endif

#ifndef _PREVIEW_H_
#define _PREVIEW_H_



class WXDLLEXPORT wxXmlNode;
class WXDLLEXPORT wxScrolledWindow;
class WXDLLEXPORT wxTextCtrl;
class WXDLLEXPORT wxSplitterWindow;
class WXDLLEXPORT wxXmlResource;
class WXDLLEXPORT wxXmlDocument;
#include "wx/frame.h"


class PreviewFrame : public wxFrame
{
    public:
        PreviewFrame();
        ~PreviewFrame();
        
        void Preview(wxXmlNode *node);
      
        static PreviewFrame *Get();
        
    private:
        void PreviewMenu();
        void PreviewToolbar();
        void PreviewPanel();
    
    private:
        static PreviewFrame *ms_Instance;
        wxXmlNode *m_Node;
        wxScrolledWindow *m_ScrollWin;
        wxTextCtrl *m_LogCtrl;
        wxSplitterWindow *m_Splitter;
        
        wxXmlResource *m_RC;
        wxString m_TmpFile;
};


#endif
