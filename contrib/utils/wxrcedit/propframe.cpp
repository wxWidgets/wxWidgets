/////////////////////////////////////////////////////////////////////////////
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "propframe.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/config.h"
#include "splittree.h"
#include "xmlhelpr.h"
#include "propframe.h"
#include "nodehnd.h"
#include "propedit.h"
#include "pe_basic.h"
#include "pe_adv.h"
#include "editor.h"

// ------------- support classes --------


class PropsTree: public wxRemotelyScrolledTreeCtrl
{
    public:
        PropsTree(wxWindow* parent, wxWindowID id, const wxPoint& pt = wxDefaultPosition,
            const wxSize& sz = wxDefaultSize, long style = wxTR_HAS_BUTTONS)
            : wxRemotelyScrolledTreeCtrl(parent, id, pt, sz, style),
              m_EditCtrl(NULL) {}

	    void OnPaint(wxPaintEvent& event)
        {
	        wxPaintDC dc(this);

	        wxTreeCtrl::OnPaint(event);

            // Reset the device origin since it may have been set
            dc.SetDeviceOrigin(0, 0);

	        wxPen pen(wxColour(_T("BLACK")), 1, wxSOLID);
	        dc.SetPen(pen);
            
	        dc.SetBrush(* wxTRANSPARENT_BRUSH);

            wxSize clientSize = GetClientSize();
	        wxRect itemRect;
	        int cy=0;
	        wxTreeItemId h, lastH;
	        for(h=GetFirstVisibleItem();h;h=GetNextVisible(h))
	        {
		        if (h.IsOk() && GetBoundingRect(h, itemRect))
		        {
			        cy = itemRect.GetTop();
			        dc.DrawLine(0, cy, clientSize.x, cy);
			        lastH = h;
		        }
	        }
	        if (lastH.IsOk() && GetBoundingRect(lastH, itemRect))
	        {
		        cy = itemRect.GetBottom();
		        dc.DrawLine(0, cy, clientSize.x, cy);
	        }
        }
        
        void OnSelChange(wxTreeEvent& event)
        {
            if (m_EditCtrl != NULL)
            {
                m_EditCtrl->EndEdit();
                m_EditCtrl = NULL;
            }
        
            wxTreeItemId item = event.GetItem();
            PETreeData *dt = (PETreeData*)GetItemData(item);
            if (dt != NULL)
            {
                wxRect bounding;
                GetBoundingRect(item, bounding);
                bounding.SetX(0);
                bounding.SetWidth(GetCompanionWindow()->GetSize().x); 
                dt->EditCtrl->BeginEdit(bounding, item);
                m_EditCtrl = dt->EditCtrl;
            }
        }

        void ClearProps()
        {
            DeleteAllItems();
            AddRoot(_("Properties"));
            if (m_EditCtrl)
            {
                m_EditCtrl->EndEdit();
                m_EditCtrl = NULL;
            }
        }

        void OnScroll(wxScrollWinEvent& event)
        {
            event.Skip();
            if (event.GetOrientation() == wxHORIZONTAL) return;
            if (!m_EditCtrl) return;
            
            wxTreeItemId id = GetSelection();
            wxRect bounding;
            GetBoundingRect(id, bounding);
            
            m_EditCtrl->Move(-1, bounding.y);
        }
        
        PropEditCtrl *m_EditCtrl;
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(PropsTree, wxRemotelyScrolledTreeCtrl)
	EVT_PAINT(PropsTree::OnPaint)
    EVT_TREE_SEL_CHANGED(-1, PropsTree::OnSelChange)
    EVT_SCROLLWIN(PropsTree::OnScroll)
END_EVENT_TABLE()


class PropsValueWindow: public wxTreeCompanionWindow
{
    public:
        PropsValueWindow(wxWindow* parent, wxWindowID id = -1,
          const wxPoint& pos = wxDefaultPosition,
          const wxSize& sz = wxDefaultSize,
          long style = 0)
           : wxTreeCompanionWindow(parent, id, pos, sz, style) {}
           
        virtual void DrawItem(wxDC& dc, wxTreeItemId id, const wxRect& rect)
        {
	        if (m_treeCtrl)
	        {
                PETreeData *data = (PETreeData*)m_treeCtrl->GetItemData(id);
		        wxString text;
                if (data != NULL) text = data->EditCtrl->GetValueAsText(id);
                dc.SetBrush(wxBrush(GetBackgroundColour(), wxSOLID));
                dc.DrawRectangle(rect);
		        dc.SetTextForeground(* wxBLACK);
		        dc.SetBackgroundMode(wxTRANSPARENT);

		        int textW, textH;
		        dc.GetTextExtent(text, & textW, & textH);

		        int x = 5;
		        int y = rect.GetY() + wxMax(0, (rect.GetHeight() - textH) / 2);

		        dc.DrawText(text, x, y);
	        }
        }
        
        void OnClick(wxMouseEvent& event)
        {
            int flags;
            wxTreeItemId item = GetTreeCtrl()->HitTest(wxPoint(1, event.GetY()), flags);
            if (item.IsOk())
            {
                GetTreeCtrl()->ScrollTo(item);
                GetTreeCtrl()->SelectItem(item);
            }
        }

        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(PropsValueWindow, wxTreeCompanionWindow)
	EVT_LEFT_DOWN(PropsValueWindow::OnClick)
END_EVENT_TABLE()



// ------------- properties frame --------



PropertiesFrame* PropertiesFrame::ms_Instance = NULL;

PropertiesFrame *PropertiesFrame::Get()
{
    if (ms_Instance == NULL) 
    {
        (void)new PropertiesFrame;
        ms_Instance->Show(TRUE);
    }
    return ms_Instance;
}

PropertiesFrame::PropertiesFrame()
    : wxFrame(EditorFrame::Get(), -1, _("Properties"),
              wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW)
{
    ms_Instance = this;
    m_Node = NULL;

    m_scrolledWindow = new wxSplitterScrolledWindow(this, -1, wxDefaultPosition,
		wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN | wxVSCROLL);
    m_splitter = new wxThinSplitterWindow(m_scrolledWindow, -1, wxDefaultPosition,
		wxDefaultSize, wxSP_3DBORDER | wxCLIP_CHILDREN /* | wxSP_LIVE_UPDATE */);
	m_splitter->SetSashSize(2);
    m_tree = new PropsTree(m_splitter, -1, wxDefaultPosition,
		wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_NO_LINES | wxNO_BORDER );
    m_tree->SetIndent(2);

    m_valueWindow = new PropsValueWindow(m_splitter, -1, wxDefaultPosition,
		wxDefaultSize, wxNO_BORDER);
    m_valueWindow->SetBackgroundColour(m_tree->GetBackgroundColour());
    m_splitter->SplitVertically(m_tree, m_valueWindow, wxConfig::Get()->Read(_T("propertiesframe_sash"), 100));
	//m_splitter->AdjustScrollbars();
	m_scrolledWindow->SetTargetWindow(m_tree);

	m_scrolledWindow->EnableScrolling(FALSE, FALSE);

	// Let the two controls know about each other
	m_valueWindow->SetTreeCtrl(m_tree);
	m_tree->SetCompanionWindow(m_valueWindow);

    wxConfigBase *cfg = wxConfigBase::Get();
    SetSize(wxRect(wxPoint(cfg->Read(_T("propertiesframe_x"), -1), cfg->Read(_T("propertiesframe_y"), -1)),
            wxSize(cfg->Read(_T("propertiesframe_w"), 200), cfg->Read(_T("propertiesframe_h"), 200))));
            

    m_EditCtrls.DeleteContents(TRUE);
    m_EditCtrls.Put(_T("bool"), new PropEditCtrlBool(this));
    m_EditCtrls.Put(_T("coord"), new PropEditCtrlCoord(this));
    m_EditCtrls.Put(_T("color"), new PropEditCtrlColor(this));
    m_EditCtrls.Put(_T("dimension"), new PropEditCtrlDim(this));
    m_EditCtrls.Put(_T("flags"), new PropEditCtrlFlags(this));
    m_EditCtrls.Put(_T("integer"), new PropEditCtrlInt(this));
    m_EditCtrls.Put(_T("not_implemented"), new PropEditCtrlNull(this));
    m_EditCtrls.Put(_T("text"), new PropEditCtrlTxt(this));
    m_EditCtrls.Put(_T("xmlid"), new PropEditCtrlXMLID(this));
    m_EditCtrls.Put(_T("font"), new PropEditCtrlFont(this));
    m_EditCtrls.Put(_T("choice"), new PropEditCtrlChoice(this));
    m_EditCtrls.Put(_T("file"), new PropEditCtrlFile(this));
    m_EditCtrls.Put(_T("imagefile"), new PropEditCtrlImageFile(this));
    m_EditCtrls.Put(_T(""), new PropEditCtrlNull(this));
            
    ClearProps();
}



PropertiesFrame::~PropertiesFrame()
{
    wxConfigBase *cfg = wxConfigBase::Get();  
    cfg->Write(_T("propertiesframe_x"), (long)GetPosition().x);
    cfg->Write(_T("propertiesframe_y"), (long)GetPosition().y);
    cfg->Write(_T("propertiesframe_w"), (long)GetSize().x);
    cfg->Write(_T("propertiesframe_h"), (long)GetSize().y);
    cfg->Write(_T("propertiesframe_sash"), (long)m_splitter->GetSashPosition());
    
    ms_Instance = NULL;
}


        
void PropertiesFrame::ShowProps(wxXmlNode *node)
{
   m_Node = node;
   
   ClearProps();
   AddSingleProp(PropertyInfo(_T("xmlid"), _T("XMLID"), wxEmptyString));
   AddProps(NodeHandler::Find(node)->GetPropsList(node));
   
   m_tree->Expand(m_tree->GetRootItem());
   m_valueWindow->Refresh();
}



void PropertiesFrame::ClearProps()
{
    ((PropsTree*)m_tree)->ClearProps();
}



void PropertiesFrame::AddProps(PropertyInfoArray& plist)
{
    for (size_t i = 0; i < plist.GetCount(); i++)
    {
        AddSingleProp(plist[i]);
    }
}



void PropertiesFrame::AddSingleProp(const PropertyInfo& pinfo, wxTreeItemId *root)
{
    PropEditCtrl *pec = (PropEditCtrl*)m_EditCtrls.Get(pinfo.Type);
    wxTreeItemId tid;
    if (root != NULL) tid = *root;
    else tid = m_tree->GetRootItem();
    
    if (pec == NULL)
        wxLogError(_("Unknown property type '%s'!"), pinfo.Type.c_str());
    else
        pec->CreateTreeEntry(tid, pinfo);
}

