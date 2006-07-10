/////////////////////////////////////////////////////////////////////////////
// Name:        common/pseudodc.cpp
// Purpose:     Implementation of the wxPseudoDC Class
// Author:      Paul Lanier
// Modified by:
// Created:     05/25/06
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/wxPython/pseudodc.h"
#include <stdio.h>

// wxList based class definitions
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(pdcOpList);
WX_DEFINE_LIST(pdcObjectList);

// ============================================================================
// various pdcOp class implementation methods
// ============================================================================

// ----------------------------------------------------------------------------
// pdcDrawPolyPolygonOp constructor
// ----------------------------------------------------------------------------
pdcDrawPolyPolygonOp::pdcDrawPolyPolygonOp(int n, int count[], wxPoint points[],
                 wxCoord xoffset, wxCoord yoffset, int fillStyle) 
{
    m_n=n; m_xoffset=xoffset; m_yoffset=yoffset; m_fillStyle=fillStyle;
    int total_n=0;
    if (n)
    {
        m_count = new int[n];
        for(int i=0; i<n; i++) 
        {
            total_n+=count[i];
            m_count[i]=count[i];
        }
        if (total_n)
        {
            m_points = new wxPoint[total_n];
            for(int j=0; j<total_n; j++)
                m_points[j] = points[j];
        }
        else m_points=NULL;
    }
    else
    {
        m_points=NULL;
        m_count=NULL;
    }
    m_totaln = total_n;
}

// ----------------------------------------------------------------------------
// pdcDrawPolyPolygonOp destructor
// ----------------------------------------------------------------------------
pdcDrawPolyPolygonOp::~pdcDrawPolyPolygonOp()
{
    if (m_points) delete m_points;
    if (m_count) delete m_count;
    m_points=NULL;
    m_count=NULL;
}
        
// ----------------------------------------------------------------------------
// pdcDrawLinesOp constructor
// ----------------------------------------------------------------------------
pdcDrawLinesOp::pdcDrawLinesOp(int n, wxPoint points[],
             wxCoord xoffset, wxCoord yoffset)
{
    m_n=n; m_xoffset=xoffset; m_yoffset=yoffset;
    if (n)
    {
        m_points = new wxPoint[n];
        for (int i=0; i<n; i++)
            m_points[i] = points[i];
    }
    else m_points=NULL;
}

// ----------------------------------------------------------------------------
// pdcDrawLinesOp destructor
// ----------------------------------------------------------------------------
pdcDrawLinesOp::~pdcDrawLinesOp()
{
    if (m_points) delete m_points;
    m_points=NULL;
}
        
// ----------------------------------------------------------------------------
// pdcDrawPolygonOp constructor
// ----------------------------------------------------------------------------
pdcDrawPolygonOp::pdcDrawPolygonOp(int n, wxPoint points[],
             wxCoord xoffset, wxCoord yoffset, int fillStyle)
{
    m_n=n; m_xoffset=xoffset; m_yoffset=yoffset; m_fillStyle=fillStyle;
    if (n)
    {
        m_points = new wxPoint[n];
        for (int i=0; i<n; i++)
            m_points[i] = points[i];
    }
    else m_points=NULL;
}

// ----------------------------------------------------------------------------
// pdcDrawPolygonOp destructor
// ----------------------------------------------------------------------------
pdcDrawPolygonOp::~pdcDrawPolygonOp()
{
    if (m_points) delete m_points;
    m_points=NULL;
}

#if wxUSE_SPLINES
// ----------------------------------------------------------------------------
// pdcDrawSplineOp constructor
// ----------------------------------------------------------------------------
pdcDrawSplineOp::pdcDrawSplineOp(int n, wxPoint points[])
{
    m_n=n;
    if (n)
    {
        m_points = new wxPoint[n];
        for(int i=0; i<n; i++)
            m_points[i] = points[i];
    }
    else m_points=NULL;
}

// ----------------------------------------------------------------------------
// pdcDrawSplineOp destructor
// ----------------------------------------------------------------------------
pdcDrawSplineOp::~pdcDrawSplineOp()
{
    if (m_points) delete m_points;
    m_points=NULL;
}
#endif // wxUSE_SPLINES

// ============================================================================
// pdcObject implementation
// ============================================================================
// ----------------------------------------------------------------------------
// DrawToDC - play back the op list to the DC 
// ----------------------------------------------------------------------------
void pdcObject::DrawToDC(wxDC *dc)
{
    pdcOpList::Node *node = m_oplist.GetFirst(); 
    while(node)
    {
        node->GetData()->DrawToDC(dc);
        node = node->GetNext();
    }
}

// ----------------------------------------------------------------------------
// Translate - translate all the operations by some dx,dy
// ----------------------------------------------------------------------------
void pdcObject::Translate(wxCoord dx, wxCoord dy)
{
    pdcOpList::Node *node = m_oplist.GetFirst(); 
    while(node)
    {
        node->GetData()->Translate(dx,dy);
        node = node->GetNext();
    }
	if (m_bounded) 
	{
		m_bounds.x += dx;
		m_bounds.y += dy;
	}
}

// ============================================================================
// wxPseudoDC implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
wxPseudoDC::~wxPseudoDC()
{
    // delete all the nodes in the list
	RemoveAll();
	
}

// ----------------------------------------------------------------------------
// ClearAll - remove all nodes from list
// ----------------------------------------------------------------------------
void wxPseudoDC::RemoveAll(void)
{
    m_objectlist.Clear();
    m_currId = -1;
	m_lastObjNode = NULL;
}

// ----------------------------------------------------------------------------
// GetLen - return the number of operations in the current op list
// ----------------------------------------------------------------------------
int wxPseudoDC::GetLen(void)
{
    pdcObjectList::Node *pt = m_objectlist.GetFirst();
    int len=0;
    while (pt) 
    {
        len += pt->GetData()->GetLen();
        pt = pt->GetNext();
    }
    return len;
}

// ----------------------------------------------------------------------------
// FindObjNode - find and return an object node by id.  If node doesn't exist
//               and create is true then create one and return it.  Otherwise
//               return NULL.
// ----------------------------------------------------------------------------
pdcObjectList::Node *wxPseudoDC::FindObjNode(int id, bool create)
{
    // see if last operation was for same id
    if (m_lastObjNode && m_lastObjNode->GetData()->GetId() == id)
        return m_lastObjNode;
    // if not then search for it
    pdcObjectList::Node *pt = m_objectlist.GetFirst();
    while (pt) 
    {
        if (pt->GetData()->GetId() == id)
		{
        
            // cache this node for future operations
            m_lastObjNode = pt;
            return pt;
        }
        pt = pt->GetNext();
    }
    // if create then create and return a new node
    if (create) 
    {
        // cache this node for future operations
        m_lastObjNode = m_objectlist.Append(new pdcObject(id));
        return m_lastObjNode;
    }
    // otherwise just return NULL
    return NULL;
}

// ----------------------------------------------------------------------------
// AddToList - Add a node to the list at the end (preserve draw order)
// ----------------------------------------------------------------------------
void wxPseudoDC::AddToList(pdcOp *newOp)
{
    pdcObjectList::Node *pt = FindObjNode(m_currId, true);
    pt->GetData()->AddOp(newOp);
}

// ----------------------------------------------------------------------------
// ClearID - remove all the operations associated with a single ID
// ----------------------------------------------------------------------------
void wxPseudoDC::ClearId(int id)
{
    pdcObjectList::Node *pt = FindObjNode(id);
    if (pt) pt->GetData()->Clear();
}

// ----------------------------------------------------------------------------
// RemoveID - Remove the object node (and all operations) associated with an id
// ----------------------------------------------------------------------------
void wxPseudoDC::RemoveId(int id)
{
    pdcObjectList::Node *pt = FindObjNode(id);
    if (pt) 
    {
        if (m_lastObjNode == pt)
            m_lastObjNode = NULL;
        m_objectlist.DeleteNode(pt);
    }
}

// ----------------------------------------------------------------------------
// SetIdBounds - Set the bounding rect for a given id
// ----------------------------------------------------------------------------
void wxPseudoDC::SetIdBounds(int id, wxRect& rect)
{
    pdcObjectList::Node *pt = FindObjNode(id, true);
    pt->GetData()->SetBounds(rect);
}

// ----------------------------------------------------------------------------
// GetIdBounds - Get the bounding rect for a given id
// ----------------------------------------------------------------------------
void wxPseudoDC::GetIdBounds(int id, wxRect& rect)
{
    pdcObjectList::Node *pt = FindObjNode(id);
	if (pt && pt->GetData()->IsBounded())
		rect = pt->GetData()->GetBounds();
	else
		rect.x = rect.y = rect.width = rect.height = 0;
}

// ----------------------------------------------------------------------------
// TranslateId - Translate all the operations of a single id
// ----------------------------------------------------------------------------
void wxPseudoDC::TranslateId(int id, wxCoord dx, wxCoord dy)
{
    pdcObjectList::Node *pt = FindObjNode(id);
    if (pt) pt->GetData()->Translate(dx,dy);
}

// ----------------------------------------------------------------------------
// DrawIdToDC - Draw a specific id to the dc passed in
// ----------------------------------------------------------------------------
void wxPseudoDC::DrawIdToDC(int id, wxDC *dc)
{
    pdcObjectList::Node *pt = FindObjNode(id);
    if (pt) pt->GetData()->DrawToDC(dc);
}

// ----------------------------------------------------------------------------
// DrawToDCClipped - play back the op list to the DC but clip any objects
//                   known to be not in rect.  This is a coarse level of 
//                   clipping to speed things up when lots of objects are off 
//                   screen and doesn't affect the dc level clipping
// ----------------------------------------------------------------------------
void wxPseudoDC::DrawToDCClipped(wxDC *dc, const wxRect& rect)
{
    pdcObjectList::Node *pt = m_objectlist.GetFirst();
    pdcObject *obj;
    while (pt) 
    {
        obj = pt->GetData();
        if (!obj->IsBounded() || rect.Intersects(obj->GetBounds()))
            obj->DrawToDC(dc);
        pt = pt->GetNext();
    }
}
void wxPseudoDC::DrawToDCClippedRgn(wxDC *dc, const wxRegion& region)
{
    pdcObjectList::Node *pt = m_objectlist.GetFirst();
    pdcObject *obj;
    while (pt) 
    {
        obj = pt->GetData();
        if (!obj->IsBounded() || 
		    (region.Contains(obj->GetBounds()) != wxOutRegion))
            obj->DrawToDC(dc);
        pt = pt->GetNext();
    }
}

// ----------------------------------------------------------------------------
// DrawToDC - play back the op list to the DC 
// ----------------------------------------------------------------------------
void wxPseudoDC::DrawToDC(wxDC *dc)
{
    pdcObjectList::Node *pt = m_objectlist.GetFirst();
    while (pt) 
    {
        pt->GetData()->DrawToDC(dc);
        pt = pt->GetNext();
    }
}
        
