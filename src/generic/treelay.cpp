///////////////////////////////////////////////////////////////////////////////
// Name:        treelay.h
// Purpose:     wxTreeLayout class
// Author:      Julian Smart
// Modified by: 
// Created:     7/4/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "wxtree.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if wxUSE_TREELAYOUT

#include <wx/treelay.h>

/*
 * Abstract tree
 *
 */

IMPLEMENT_ABSTRACT_CLASS(wxTreeLayout, wxObject)

wxTreeLayout::wxTreeLayout()
{
    m_xSpacing = 16;
    m_ySpacing = 20;
    m_topMargin = 5;
    m_leftMargin = 5;
    m_orientation = FALSE;
    m_parentNode = 0;
}

void wxTreeLayout::DoLayout(wxDC& dc, long topId)
{
    if (topId != -1)
        SetTopNode(topId);
    
    long actualTopId = GetTopNode();
    long id = actualTopId;
    while (id != -1)
    {
        SetNodeX(id, 0);
        SetNodeY(id, 0);
        ActivateNode(id, FALSE);
        id = GetNextNode(id);
    }
    m_lastY = m_topMargin;
    m_lastX = m_leftMargin;
    CalcLayout(actualTopId, 0, dc);
}

void wxTreeLayout::Draw(wxDC& dc)
{
    dc.Clear();
    DrawBranches(dc);
    DrawNodes(dc);
}

void wxTreeLayout::DrawNodes(wxDC& dc)
{  
    long id = GetTopNode();
    while (id != -1)
    {
        if (NodeActive(id))
            DrawNode(id, dc);
        id = GetNextNode(id);
    }
}

void wxTreeLayout::DrawBranches(wxDC& dc)
{  
    long id = GetTopNode();
    while (id != -1)
    {
        if (GetNodeParent(id) > -1)
        {
            long parent = GetNodeParent(id);
            if (NodeActive(parent))
                DrawBranch(parent, id, dc);
        }
        id = GetNextNode(id);
    }
}

void wxTreeLayout::DrawNode(long id, wxDC& dc)
{
    char buf[80];
    wxString name(GetNodeName(id));
    if (name != "")
        sprintf(buf, "%s", (const char*) name);
    else
        sprintf(buf, "<unnamed>");
    
    long x = 80;
    long y = 20;
    dc.GetTextExtent(buf, &x, &y);
    dc.DrawText(buf, GetNodeX(id), (long)(GetNodeY(id) - (y/2.0)));
}

void wxTreeLayout::DrawBranch(long from, long to, wxDC& dc)
{
    long w, h;
    GetNodeSize(from, &w, &h, dc);
    dc.DrawLine(GetNodeX(from)+w, GetNodeY(from),
        GetNodeX(to), GetNodeY(to));
}

void wxTreeLayout::Initialize(void)
{
}

void wxTreeLayout::GetNodeSize(long id, long *x, long *y, wxDC& dc)
{
    wxString name(GetNodeName(id));
    if (name != "")
        dc.GetTextExtent(name, x, y);
    else
    {
        *x = 70; *y = 20;
    }
}

void wxTreeLayout::CalcLayout(long nodeId, int level, wxDC& dc)
{
    wxList children;
    GetChildren(nodeId, children);
    int n = children.Number();
    
    if (m_orientation == FALSE)
    {
        // Left to right
        // X Calculations
        if (level == 0)
            SetNodeX(nodeId, m_leftMargin);
        else
        {
            long x = 0;
            long y = 0;
            long parentId = GetNodeParent(nodeId);
            if (parentId != -1)
                GetNodeSize(parentId, &x, &y, dc);
            SetNodeX(nodeId, (long)(GetNodeX(parentId) + m_xSpacing + x));
        }
        
        wxNode *node = children.First();
        while (node)
        {
            CalcLayout((long)node->Data(), level+1, dc);
            node = node->Next();
        }
        
        // Y Calculations
        long averageY;
        ActivateNode(nodeId, TRUE);
        
        if (n > 0)
        {
            averageY = 0;
            node = children.First();
            while (node)
            {
                averageY += GetNodeY((long)node->Data());
                node = node->Next();
            }
            averageY = averageY / n;
            SetNodeY(nodeId, averageY);
        }
        else
        {
            SetNodeY(nodeId, m_lastY);
            long x, y;
            GetNodeSize(nodeId, &x, &y, dc);
            
            m_lastY = m_lastY + y + m_ySpacing;
        }
    }
    else
    {
        // Top to bottom
        
        // Y Calculations
        if (level == 0)
            SetNodeY(nodeId, m_topMargin);
        else
        {
            long x = 0;
            long y = 0;
            long parentId = GetNodeParent(nodeId);
            if (parentId != -1)
                GetNodeSize(parentId, &x, &y, dc);
            SetNodeY(nodeId, (long)(GetNodeY(parentId) + m_ySpacing + y));
        }
        
        wxNode *node = children.First();
        while (node)
        {
            CalcLayout((long)node->Data(), level+1, dc);
            node = node->Next();
        }
        
        // X Calculations
        long averageX;
        ActivateNode(nodeId, TRUE);
        
        if (n > 0)
        {
            averageX = 0;
            node = children.First();
            while (node)
            {
                averageX += GetNodeX((long)node->Data());
                node = node->Next();
            }
            averageX = averageX / n;
            SetNodeX(nodeId, averageX);
        }
        else
        {
            SetNodeX(nodeId, m_lastX);
            long x, y;
            GetNodeSize(nodeId, &x, &y, dc);
            
            m_lastX = m_lastX + x + m_xSpacing;
        }
    }
}

/*
 * Tree with storage
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxLayoutTreeStored, wxTreeLayout)

wxLayoutTreeStored::wxLayoutTreeStored(int n):wxTreeLayout()
{
    m_nodes = NULL;
    m_maxNodes = 0;
    Initialize(n);
}

wxLayoutTreeStored::~wxLayoutTreeStored(void)
{
    if (m_nodes)
        delete[] m_nodes;
}

void wxLayoutTreeStored::Initialize(int n)
{
    m_maxNodes = n;
    wxTreeLayout::Initialize();
    if (m_nodes) delete[] m_nodes;
    m_nodes = new wxStoredNode[m_maxNodes];
    int i;
    for (i = 0; i < n; i++)
    {
        m_nodes[i].m_name = "";
        m_nodes[i].m_active = FALSE;
        m_nodes[i].m_parentId = -1;
        m_nodes[i].m_x = 0;
        m_nodes[i].m_y = 0;
    }
    m_num = 0;
}

long wxLayoutTreeStored::AddChild(const wxString& name, const wxString& parent)
{
    if (m_num < (m_maxNodes -1 ))
    {
        long i = -1;
        if (parent != "")
            i = NameToId(parent);
        else m_parentNode = m_num;
        
        m_nodes[m_num].m_parentId = i;
        m_nodes[m_num].m_name = name;
        m_nodes[m_num].m_x = m_nodes[m_num].m_y = 0;
        m_nodes[m_num].m_clientData = 0;
        m_num ++;
        
        return (m_num - 1);
    }
    else
        return -1;
}

long wxLayoutTreeStored::NameToId(const wxString& name)
{
    long i;
    for (i = 0; i < m_num; i++)
        if (name == m_nodes[i].m_name)
            return i;
        return -1;
}

void wxLayoutTreeStored::GetChildren(long id, wxList& list)
{
    long currentId = GetTopNode();
    while (currentId != -1)
    {
        if (id == GetNodeParent(currentId))
            list.Append((wxObject *)currentId);
        currentId = GetNextNode(currentId);
    }
}

wxStoredNode* wxLayoutTreeStored::GetNode(long idx) const
{
    wxASSERT(idx < m_num);
    
    return &m_nodes[idx];
};

long wxLayoutTreeStored::GetNodeX(long id)
{
    wxASSERT(id < m_num);
    
    return (long)m_nodes[id].m_x;
}

long wxLayoutTreeStored::GetNodeY(long id)
{
    wxASSERT(id < m_num);
    
    return (long)m_nodes[id].m_y;
}

void wxLayoutTreeStored::SetNodeX(long id, long x)
{
    wxASSERT(id < m_num);
    
    m_nodes[id].m_x = (int)x;
}

void wxLayoutTreeStored::SetNodeY(long id, long y)
{
    wxASSERT(id < m_num);
    
    m_nodes[id].m_y = (int)y;
}

void wxLayoutTreeStored::SetNodeName(long id, const wxString& name)
{
    wxASSERT(id < m_num);
    
    m_nodes[id].m_name = name;
}

wxString wxLayoutTreeStored::GetNodeName(long id)
{
    wxASSERT(id < m_num);
    
    return m_nodes[id].m_name;
}

long wxLayoutTreeStored::GetNodeParent(long id)
{
    if (id != -1)
    {
        wxASSERT(id < m_num);
        
        return m_nodes[id].m_parentId;
    }
    else
        return -1;
}

long wxLayoutTreeStored::GetNextNode(long id)
{
    wxASSERT(id < m_num);
    
    if ((id != -1) && (id < (m_num - 1)))
        return id + 1;
    else
        return -1;
}

void wxLayoutTreeStored::SetClientData(long id, long clientData)
{
    wxASSERT(id < m_num);
    
    m_nodes[id].m_clientData = clientData;
}

long wxLayoutTreeStored::GetClientData(long id) const
{
    wxASSERT(id < m_num);
    
    return m_nodes[id].m_clientData;
}

void wxLayoutTreeStored::ActivateNode(long id, bool active)
{
    wxASSERT(id < m_num);
    
    m_nodes[id].m_active = active;
}

bool wxLayoutTreeStored::NodeActive(long id)
{
    wxASSERT(id < m_num);
    
    return m_nodes[id].m_active;
}

wxString wxLayoutTreeStored::HitTest(wxMouseEvent& event, wxDC& dc)
{
    wxPoint pt = event.GetPosition();
    wxCoord x = pt.x;
    wxCoord y = pt.y;
    
    int i;
    for (i = 0; i < m_maxNodes; i++)
    {
        wxStoredNode* item = &m_nodes[i];
        
        long width, height;
        dc.GetTextExtent(m_nodes[i].m_name, &width, &height);
        
        if ( (x >= (m_nodes[i].m_x-10)) && (x < (m_nodes[i].m_x + width+10)) &&
            (y >= m_nodes[i].m_y-10) && (y < (m_nodes[i].m_y + height+10)) )
        {
            return m_nodes[i].m_name;
        }
    }
    
    return wxString("");
}

#endif
    // wxUSE_TREELAYOUT
