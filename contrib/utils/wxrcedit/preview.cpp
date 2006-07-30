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
#include "wx/xrc/xmlres.h"
#include "wx/config.h"
#include "wx/log.h"
#include "wx/splitter.h"
#include "preview.h"
#include "xmlhelpr.h"
#include "editor.h"

#include "wx/xrc/xh_menu.h"

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
        ms_Instance->Show(true);
    }
    return ms_Instance;
}

PreviewFrame::PreviewFrame()
    : wxFrame(EditorFrame::Get(), wxID_ANY, _("Preview"),
              wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR
#ifdef __WXMSW__
              | wxFRAME_TOOL_WINDOW
#endif
              )
{
    m_Dirty = false;
    ms_Instance = this;
    m_Node = NULL;

    SetMenuBar(new wxMenuBar());

    m_RC = NULL;
    m_TmpFile = wxGetTempFileName(_T("wxrcedit"));
    ResetResource();

    wxConfigBase *cfg = wxConfigBase::Get();
    SetSize(wxRect(wxPoint(cfg->Read(_T("previewframe_x"), wxDefaultCoord), cfg->Read(_T("previewframe_y"), wxDefaultCoord)),
            wxSize(cfg->Read(_T("previewframe_w"), 400), cfg->Read(_T("previewframe_h"), 400))));

    m_Splitter = new wxSplitterWindow(this, wxID_ANY);
#if wxUSE_LOG
    m_LogCtrl = new wxTextCtrl(m_Splitter, wxID_ANY, wxEmptyString, wxDefaultPosition,
                               wxDefaultSize, wxTE_MULTILINE);
#endif // wxUSE_LOG
    m_ScrollWin = new wxScrolledWindow(m_Splitter, wxID_ANY);
    m_ScrollWin->SetBackgroundColour(_T("light steel blue"));
#if wxUSE_LOG
    m_Splitter->SplitHorizontally(m_ScrollWin, m_LogCtrl, cfg->Read(_T("previewframe_sash"), 300));
#endif // wxUSE_LOG

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

#ifdef __WXMSW__
    SendSizeEvent(); // force resize for WXMSW
#endif
}



void PreviewFrame::ResetResource()
{
    delete m_RC;
    m_RC = new wxXmlResource;
    // these handlers take precedence over std. ones:
    m_RC->AddHandler(new MyMenubarHandler(GetMenuBar()));
    // std handlers:
    m_RC->InitAllHandlers();
    wxRemoveFile(m_TmpFile);
    m_RC->Load(m_TmpFile);
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
    m_Dirty = true;
#if wxUSE_LOG
    m_LogCtrl->Clear();
    m_LogCtrl->SetValue(_("Resource modified.\nMove mouse cursor over the preview window to refresh it."));
#endif // wxUSE_LOG
}



void PreviewFrame::Preview(wxXmlNode *node, wxXmlDocument *orig_doc)
{
   wxString version = orig_doc->GetRoot()->GetPropVal(wxT("version"), wxT("0.0.0.0"));

   while (node->GetParent()->GetParent() != NULL) node = node->GetParent();

   {
       wxXmlDocument doc;
       wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE, _T("resource"));
       root->AddProperty(new wxXmlProperty(wxT("version"),version,NULL));
       doc.SetRoot(root);
       doc.GetRoot()->AddChild(new wxXmlNode(*node));
       doc.SetFileEncoding(orig_doc->GetFileEncoding());

       if (XmlGetClass(doc.GetRoot()->GetChildren()) == _T("wxDialog"))
           XmlSetClass(doc.GetRoot()->GetChildren(), _T("wxPanel"));

       if (XmlGetClass(doc.GetRoot()->GetChildren()) == _T("wxFrame"))
           XmlSetClass(doc.GetRoot()->GetChildren(), _T("wxPanel"));

       doc.Save(m_TmpFile);
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
   m_Doc = orig_doc;

#if wxUSE_LOG
   m_LogCtrl->Clear();
   wxLogTextCtrl mylog(m_LogCtrl);
   wxLog *oldlog = wxLog::SetActiveTarget(&mylog);
#endif // wxUSE_LOG

   wxString oldcwd = wxGetCwd();
   wxSetWorkingDirectory(wxPathOnly(EditorFrame::Get()->GetFileName()));

   if (XmlGetClass(node) == _T("wxMenuBar") || XmlGetClass(node) == _T("wxMenu"))
       PreviewMenu();
   else if (XmlGetClass(node) == _T("wxToolBar"))
       PreviewToolbar();
   else if (XmlGetClass(node) == _T("wxPanel") || XmlGetClass(node) == _T("wxDialog"))
       PreviewPanel();
   else if (XmlGetClass(node) == _T("wxFrame"))
       PreviewWXFrame();

   wxSetWorkingDirectory(oldcwd);
#if wxUSE_LOG
   wxLog::SetActiveTarget(oldlog);
#endif // wxUSE_LOG

   m_Dirty = false;
}



void PreviewFrame::PreviewMenu()
{
    wxMenuBar *mbar = NULL;

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
                                   0, 0, true);
    }
}

void PreviewFrame::PreviewWXFrame()
{
    //for this to work the frame MUST have a child panel!

    wxXmlNode* child = m_Node;
    wxString name;

    while( child != NULL)
    {
        name = child->GetPropVal(_T("name"), _T("-1"));

        if(name != _T("-1"))
        {
            wxXmlNode* parent = child->GetParent();
            if(parent->GetPropVal(_T("class"),_T("-1")) == _T("wxPanel"))
                break;
        }
        child = child->GetNext();
    }

    wxPanel *panel = m_RC->LoadPanel(m_ScrollWin, name);

    if (panel == NULL)
        wxLogError(_("Cannot preview the panel -- XML resource corrupted."));
    else
    {
        m_ScrollWin->SetScrollbars(1, 1, panel->GetSize().x, panel->GetSize().y,
                                   0, 0, true);
    }

}

BEGIN_EVENT_TABLE(PreviewFrame, wxFrame)
    EVT_ENTER_WINDOW(PreviewFrame::OnMouseEnter)
END_EVENT_TABLE()

void PreviewFrame::OnMouseEnter(wxMouseEvent& WXUNUSED(event))
{
    if (m_Dirty) Preview(m_Node,m_Doc);
}
