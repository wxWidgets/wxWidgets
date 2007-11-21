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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/dc.h"
#include "wx/event.h"
#endif

#include "wx/deprecated/setup.h"

#if wxUSE_TREELAYOUT

#include "wx/deprecated/treelay.h"

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
    m_orientation = false;
    m_parentNode = 0;
}

void wxTreeLayout::DoLayout(wxDC& dc, long topId)
{
    if (topId != wxID_ANY)
        SetTopNode(topId);

    long actualTopId = GetTopNode();
    long id = actualTopId;
    while (id != wxID_ANY)
    {
        SetNodeX(id, 0);
        SetNodeY(id, 0);
        ActivateNode(id, false);
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
    while (id != wxID_ANY)
    {
        if (NodeActive(id))
            DrawNode(id, dc);
        id = GetNextNode(id);
    }
}

void wxTreeLayout::DrawBranches(wxDC& dc)
{
    long id = GetTopNode();
    while (id != wxID_ANY)
    {
        if (GetNodeParent(id) != wxID_ANY)
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
    wxChar buf[80];
    wxString name(GetNodeName(id));
    if (name != wxT(""))
        wxSprintf(buf, wxT("%s"), (const wxChar*) name);
    else
        wxSprintf(buf, wxT("<unnamed>"));

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
    if (name != wxT(""))
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
    int n = children.GetCount();

    if (m_orientation == false)
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
            if (parentId != wxID_ANY)
                GetNodeSize(parentId, &x, &y, dc);
            SetNodeX(nodeId, (long)(GetNodeX(parentId) + m_xSpacing + x));
        }

        wxList::compatibility_iterator node = children.GetFirst();
        while (node)
        {
            CalcLayout((long)node->GetData(), level+1, dc);
            node = node->GetNext();
        }

        // Y Calculations
        long averageY;
        ActivateNode(nodeId, true);

        if (n > 0)
        {
            averageY = 0;
            node = children.GetFirst();
            while (node)
            {
                averageY += GetNodeY((long)node->GetData());
                node = node->GetNext();
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
            if (parentId != wxID_ANY)
                GetNodeSize(parentId, &x, &y, dc);
            SetNodeY(nodeId, (long)(GetNodeY(parentId) + m_ySpacing + y));
        }

        wxList::compatibility_iterator node = children.GetFirst();
        while (node)
        {
            CalcLayout((long)node->GetData(), level+1, dc);
            node = node->GetNext();
        }

        // X Calculations
        long averageX;
        ActivateNode(nodeId, true);

        if (n > 0)
        {
            averageX = 0;
            node = children.GetFirst();
            while (node)
            {
                averageX += GetNodeX((long)node->GetData());
                node = node->GetNext();
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

IMPLEMENT_DYNAMIC_CLASS(wxTreeLayoutStored, wxTreeLayout)

wxTreeLayoutStored::wxTreeLayoutStored(int n):wxTreeLayout()
{
    m_nodes = NULL;
    m_maxNodes = 0;
    Initialize(n);
}

wxTreeLayoutStored::~wxTreeLayoutStored(void)
{
    if (m_nodes)
        delete[] m_nodes;
}

void wxTreeLayoutStored::Initialize(int n)
{
    m_maxNodes = n;
    wxTreeLayout::Initialize();
    if (m_nodes) delete[] m_nodes;
    m_nodes = new wxStoredNode[m_maxNodes];
    int i;
    for (i = 0; i < n; i++)
    {
        m_nodes[i].m_name = wxT("");
        m_nodes[i].m_active = false;
        m_nodes[i].m_parentId = wxID_ANY;
        m_nodes[i].m_x = 0;
        m_nodes[i].m_y = 0;
    }
    m_num = 0;
}

long wxTreeLayoutStored::AddChild(const wxString& name, const wxString& parent)
{
    if (m_num < (m_maxNodes -1 ))
    {
        long i = -1;
        if (parent != wxT(""))
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

long wxTreeLayoutStored::AddChild(const wxString& name, long parent)
{
    if (m_num < (m_maxNodes -1 ) && parent < m_num)
    {
        long i = -1;
        if (parent != -1)
        {
          i = parent;
        }
        else
        {
          m_parentNode = m_num;
        }

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

long wxTreeLayoutStored::NameToId(const wxString& name)
{
    long i;
    for (i = 0; i < m_num; i++)
        if (name == m_nodes[i].m_name)
            return i;
        return -1;
}

void wxTreeLayoutStored::GetChildren(long id, wxList& list)
{
    long currentId = GetTopNode();
    while (currentId != wxID_ANY)
    {
        if (id == GetNodeParent(currentId))
            list.Append((wxObject *)currentId);
        currentId = GetNextNode(currentId);
    }
}

wxStoredNode* wxTreeLayoutStored::GetNode(long idx) const
{
    wxASSERT(idx < m_num);

    return &m_nodes[idx];
};

long wxTreeLayoutStored::GetNodeX(long id)
{
    wxASSERT(id < m_num);

    return (long)m_nodes[id].m_x;
}

long wxTreeLayoutStored::GetNodeY(long id)
{
    wxASSERT(id < m_num);

    return (long)m_nodes[id].m_y;
}

void wxTreeLayoutStored::SetNodeX(long id, long x)
{
    wxASSERT(id < m_num);

    m_nodes[id].m_x = (int)x;
}

void wxTreeLayoutStored::SetNodeY(long id, long y)
{
    wxASSERT(id < m_num);

    m_nodes[id].m_y = (int)y;
}

void wxTreeLayoutStored::SetNodeName(long id, const wxString& name)
{
    wxASSERT(id < m_num);

    m_nodes[id].m_name = name;
}

wxString wxTreeLayoutStored::GetNodeName(long id)
{
    wxASSERT(id < m_num);

    return m_nodes[id].m_name;
}

long wxTreeLayoutStored::GetNodeParent(long id)
{
    if (id != wxID_ANY)
    {
        wxASSERT(id < m_num);

        return m_nodes[id].m_parentId;
    }
    else
        return wxNOT_FOUND;
}

long wxTreeLayoutStored::GetNextNode(long id)
{
    wxASSERT(id < m_num);

    if ((id != wxID_ANY) && (id < (m_num - 1)))
        return id + 1;
    else
        return wxNOT_FOUND;
}

void wxTreeLayoutStored::SetClientData(long id, long clientData)
{
    wxASSERT(id < m_num);

    m_nodes[id].m_clientData = clientData;
}

long wxTreeLayoutStored::GetClientData(long id) const
{
    wxASSERT(id < m_num);

    return m_nodes[id].m_clientData;
}

void wxTreeLayoutStored::ActivateNode(long id, bool active)
{
    wxASSERT(id < m_num);

    m_nodes[id].m_active = active;
}

bool wxTreeLayoutStored::NodeActive(long id)
{
    wxASSERT(id < m_num);

    return m_nodes[id].m_active;
}

wxString wxTreeLayoutStored::HitTest(wxMouseEvent& event, wxDC& dc)
{
    wxPoint pt = event.GetPosition();
    wxCoord x = pt.x;
    wxCoord y = pt.y;

    int i;
    for (i = 0; i < m_maxNodes; i++)
    {
        long width, height;
        dc.GetTextExtent(m_nodes[i].m_name, &width, &height);

        if ( (x >= (m_nodes[i].m_x-10)) && (x < (m_nodes[i].m_x + width+10)) &&
            (y >= m_nodes[i].m_y-10) && (y < (m_nodes[i].m_y + height+10)) )
        {
            return m_nodes[i].m_name;
        }
    }

    return wxString( wxT("") );
}

#endif
    // wxUSE_TREELAYOUT
