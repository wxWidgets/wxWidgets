/////////////////////////////////////////////////////////////////////////////
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "preview.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/xml/xmlres.h"
#include "wx/config.h"
#include "wx/log.h"
#include "wx/splitter.h"
#include "preview.h"
#include "xmlhelpr.h"
#include "editor.h"

#include "wx/xml/xh_menu.h"

class MyMenubarHandler : public wxMenuBarXmlHandler
{
    public:
        MyMenubarHandler(wxMenuBar *mbar) : m_MenuBar(mbar) {}

        wxObject *DoCreateResource()
        {
            //wxMenuBar *menubar = new wxMenuBar(GetStyle());            
            CreateChildren(m_MenuBar);
            return m_MenuBar;
        }
        
    private:
        wxMenuBar *m_MenuBar;
};




PreviewFrame* PreviewFrame::ms_Instance = NULL;

PreviewFrame *PreviewFrame::Get()
{
    if (ms_Instance == NULL) 
    {
        (void)new PreviewFrame;
        ms_Instance->Show(TRUE);
    }
    return ms_Instance;
}

PreviewFrame::PreviewFrame()
    : wxFrame(NULL, -1, _("Preview"))
{
    m_Dirty = FALSE;
    ms_Instance = this;
    m_Node = NULL;
    
    SetMenuBar(new wxMenuBar());
    
    m_RC = new wxXmlResource;
    // these handlers take precedence over std. ones:
    m_RC->AddHandler(new MyMenubarHandler(GetMenuBar()));
    // std handlers:
    m_RC->InitAllHandlers();
    m_TmpFile = wxGetTempFileName(_T("wxrcedit"));
    m_RC->Load(m_TmpFile);

    wxConfigBase *cfg = wxConfigBase::Get();
    SetSize(wxRect(wxPoint(cfg->Read(_T("previewframe_x"), -1), cfg->Read(_T("previewframe_y"), -1)),
            wxSize(cfg->Read(_T("previewframe_w"), 400), cfg->Read(_T("previewframe_h"), 400))));

    m_Splitter = new wxSplitterWindow(this, -1);
    m_LogCtrl = new wxTextCtrl(m_Splitter, -1, wxEmptyString, wxDefaultPosition, 
                               wxDefaultSize, wxTE_MULTILINE);
    m_ScrollWin = new wxScrolledWindow(m_Splitter, -1);
    m_ScrollWin->SetBackgroundColour(_T("light steel blue"));
    m_Splitter->SplitHorizontally(m_ScrollWin, m_LogCtrl, cfg->Read(_T("previewframe_sash"), 300));

    CreateStatusBar();
}



PreviewFrame::~PreviewFrame()
{
    wxConfigBase *cfg = wxConfigBase::Get();  
    cfg->Write(_T("previewframe_x"), (long)GetPosition().x);
    cfg->Write(_T("previewframe_y"), (long)GetPosition().y);
    cfg->Write(_T("previewframe_w"), (long)GetSize().x);
    cfg->Write(_T("previewframe_h"), (long)GetSize().y);
    cfg->Write(_T("previewframe_sash"), (long)m_Splitter->GetSashPosition());
    
    ms_Instance = NULL;
    
    delete m_RC;
    wxRemoveFile(m_TmpFile);
}


        
void PreviewFrame::MakeDirty()
{
    if (m_Node == NULL) return;
    if (m_Dirty) return;
    m_Dirty = TRUE;
    m_LogCtrl->Clear();
    m_LogCtrl->SetValue(_("Resource modified.\n"
                   "Move mouse cursor over the preview window to refresh it."));
}



void PreviewFrame::Preview(wxXmlNode *node)
{
   while (node->GetParent()->GetParent() != NULL) node = node->GetParent();

   {
       wxXmlDocument doc;
       doc.SetRoot(new wxXmlNode(wxXML_ELEMENT_NODE, _T("resource")));
       doc.GetRoot()->AddChild(new wxXmlNode(*node));

       if (XmlGetClass(doc.GetRoot()->GetChildren()) == _T("wxDialog")) 
           XmlSetClass(doc.GetRoot()->GetChildren(), _T("wxPanel"));   

       doc.Save(m_TmpFile, wxXML_IO_BIN);
       // wxXmlResource will detect change automatically
   }


   //if (m_Node != node)
   {
       SetToolBar(NULL);
       for (int i = (int)(GetMenuBar()->GetMenuCount())-1; i >= 0; i--)
           delete GetMenuBar()->Remove(i);
       m_ScrollWin->DestroyChildren();
   }

   m_Node = node;

   m_LogCtrl->Clear();
   wxLogTextCtrl mylog(m_LogCtrl);
   wxLog *oldlog = wxLog::SetActiveTarget(&mylog);
   
   wxString oldcwd = wxGetCwd();
   wxSetWorkingDirectory(wxPathOnly(EditorFrame::Get()->GetFileName()));
   
   if (XmlGetClass(node) == _T("wxMenuBar") || XmlGetClass(node) == _T("wxMenu"))
       PreviewMenu();
   else if (XmlGetClass(node) == _T("wxToolBar"))
       PreviewToolbar();
   else if (XmlGetClass(node) == _T("wxPanel") || XmlGetClass(node) == _T("wxDialog"))
       PreviewPanel();
   
   wxSetWorkingDirectory(oldcwd);
   wxLog::SetActiveTarget(oldlog);
   
   m_Dirty = FALSE;
}



void PreviewFrame::PreviewMenu()
{
    wxMenuBar *mbar;

    if (XmlGetClass(m_Node) == _T("wxMenuBar"))
        mbar = m_RC->LoadMenuBar(m_Node->GetPropVal(_T("name"), _T("-1")));
    else
    {
        wxMenu *m = m_RC->LoadMenu(m_Node->GetPropVal(_T("name"), _T("-1")));
        if (m != NULL) GetMenuBar()->Append(m, _("(menu)"));
    }

    if (mbar == NULL)
        wxLogError(_("Cannot preview the menu -- XML resource corrupted."));
}



void PreviewFrame::PreviewToolbar()
{
    SetToolBar(m_RC->LoadToolBar(this, m_Node->GetPropVal(_T("name"), _T("-1"))));
}



void PreviewFrame::PreviewPanel()
{
    wxPanel *panel = m_RC->LoadPanel(m_ScrollWin, m_Node->GetPropVal(_T("name"), _T("-1")));
    
    if (panel == NULL)
        wxLogError(_("Cannot preview the panel -- XML resource corrupted."));
    else
    {
        m_ScrollWin->SetScrollbars(1, 1, panel->GetSize().x, panel->GetSize().y,
                                   0, 0, TRUE);
    }
}



BEGIN_EVENT_TABLE(PreviewFrame, wxFrame)
    EVT_ENTER_WINDOW(PreviewFrame::OnMouseEnter)
END_EVENT_TABLE()

void PreviewFrame::OnMouseEnter(wxMouseEvent& event)
{
    if (m_Dirty) Preview(m_Node);
}
