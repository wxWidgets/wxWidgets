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
//include "wx/wxprec.h"

#undef DEBUG
#include <Python.h>
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pseudodc.h"

// wxList based class definitions
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(pdcOpList);
WX_DEFINE_LIST(pdcObjectList);

//----------------------------------------------------------------------------
// Helper functions used for drawing greyed out versions of objects
//----------------------------------------------------------------------------
wxColour &MakeColourGrey(const wxColour &c)
{
    static wxColour rval;
    rval.Set(byte((230-c.Red())*0.7+c.Red()),
             byte((230-c.Green())*0.7+c.Green()),
             byte((230-c.Blue())*0.7+c.Blue()));
    return rval;
}
wxBrush &GetGreyBrush(wxBrush &brush)
{
	static wxBrush b;
	wxColour c;
	b = brush;
	c = MakeColourGrey(brush.GetColour());
	b.SetColour(c);
	return b;
}

wxPen &GetGreyPen(wxPen &pen)
{
	static wxPen p;
	wxColour c;
	p = pen;
	c = MakeColourGrey(pen.GetColour());
	p.SetColour(c);
	return p;
}

void GreyOutImage(wxImage &img)
{
    unsigned char *data = img.GetData();
    unsigned char r,g,b;
    unsigned char mr,mg,mb;
    int i, tst;
    int len = img.GetHeight()*img.GetWidth()*3;
    if (img.HasMask())
    {
        mr = img.GetMaskRed();
        mg = img.GetMaskGreen();
        mb = img.GetMaskBlue();
    }
    tst=0;
    for (i=0;i<len;i+=3)
    {
        r=data[i]; g=data[i+1]; b=data[i+2];
        if (!img.HasMask() || 
            r!=mr || g!=mg || b!=mb)
        {
            if (!tst)
            {
                tst=1;
            }
            r = (unsigned char)((230.0-r)*0.7+r);
            g = (unsigned char)((230.0-g)*0.7+g);
            b = (unsigned char)((230.0-b)*0.7+b);
            data[i]=r; data[i+1]=g; data[i+2]=b;
        }
    }
}

wxIcon &GetGreyIcon(wxIcon &icon)
{
    wxBitmap bmp;
    bmp.CopyFromIcon(icon);
    wxImage img = bmp.ConvertToImage();
    GreyOutImage(img);
    wxBitmap bmp2(img,32);
    static wxIcon rval;
    rval.CopyFromBitmap(bmp2);
    return rval;
}

wxBitmap &GetGreyBitmap(wxBitmap &bmp)
{
    wxImage img = bmp.ConvertToImage();
    GreyOutImage(img);
    static wxBitmap rval(img,32);
    return rval;
}

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
        node->GetData()->DrawToDC(dc, m_greyedout);
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

// ----------------------------------------------------------------------------
// SetGreyedOut - set the greyout member and cache grey versions of everything
// if greyout is true
// ----------------------------------------------------------------------------
void pdcObject::SetGreyedOut(bool greyout) 
{
	m_greyedout=greyout;
	if (greyout)
	{
		pdcOpList::Node *node = m_oplist.GetFirst(); 
        pdcOp *obj;
		while(node)
		{

			obj = node->GetData();
            obj->CacheGrey();
			node = node->GetNext();
		}
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
// SetIdGreyedOut - Set the greyedout member of id
// ----------------------------------------------------------------------------
void wxPseudoDC::SetIdGreyedOut(int id, bool greyout)
{
    pdcObjectList::Node *pt = FindObjNode(id);
    if (pt) pt->GetData()->SetGreyedOut(greyout);
}

// ----------------------------------------------------------------------------
// GetIdGreyedOut - Get the greyedout member of id
// ----------------------------------------------------------------------------
bool wxPseudoDC::GetIdGreyedOut(int id)
{
    pdcObjectList::Node *pt = FindObjNode(id);
    if (pt) return pt->GetData()->GetGreyedOut();
	else return false;
}

// ----------------------------------------------------------------------------
// FindObjectsByBBox - Return a list of all the ids whose bounding boxes
//                     contain (x,y)
// ----------------------------------------------------------------------------
PyObject *wxPseudoDC::FindObjectsByBBox(wxCoord x, wxCoord y)
{
    //wxPyBlock_t blocked = wxPyBeginBlockThreads();
    pdcObjectList::Node *pt = m_objectlist.GetFirst();
    pdcObject *obj;
    PyObject* pyList = NULL;
    pyList = PyList_New(0);
    wxRect r;
    while (pt) 
    {
        obj = pt->GetData();
        r = obj->GetBounds();
        if (obj->IsBounded() && r.Contains(x,y))
        {
            PyObject* pyObj = PyInt_FromLong((long)obj->GetId());
            PyList_Insert(pyList, 0, pyObj);
            Py_DECREF(pyObj);
        }
        pt = pt->GetNext();
    }
    //wxPyEndBlockThreads(blocked);
    return pyList;
}

// ----------------------------------------------------------------------------
// FindObjects - Return a list of all the ids that draw to (x,y)
// ----------------------------------------------------------------------------
PyObject *wxPseudoDC::FindObjects(wxCoord x, wxCoord y, 
                                  wxCoord radius, const wxColor& bg)
{
    //wxPyBlock_t blocked = wxPyBeginBlockThreads();
    pdcObjectList::Node *pt = m_objectlist.GetFirst();
    pdcObject *obj;
    PyObject* pyList = NULL;
    pyList = PyList_New(0);
    wxBrush bgbrush(bg);
    wxPen bgpen(bg);
    // special case radius = 0
    if (radius == 0)
    {
        wxBitmap bmp(4,4,24);
        wxMemoryDC memdc;
        wxColor pix;
        wxRect viewrect(x-2,y-2,4,4);
        // setup the memdc for rendering
        memdc.SelectObject(bmp);
        memdc.SetBackground(bgbrush);
        memdc.Clear();
        memdc.SetDeviceOrigin(2-x,2-y);
        while (pt) 
        {
            obj = pt->GetData();
            if (obj->IsBounded() && obj->GetBounds().Contains(x,y))
            {
                // start clean
                memdc.SetBrush(bgbrush);
                memdc.SetPen(bgpen);
                memdc.DrawRectangle(viewrect);
                // draw the object
                obj->DrawToDC(&memdc);
                memdc.GetPixel(x,y,&pix);
                // clear and update rgn2
                if (pix != bg)
                {
                    PyObject* pyObj = PyInt_FromLong((long)obj->GetId());
                    PyList_Insert(pyList, 0, pyObj);
                    Py_DECREF(pyObj);
                }
            }
            pt = pt->GetNext();
        }
        memdc.SelectObject(wxNullBitmap);
    }
    else
    {
        wxRect viewrect(x-radius,y-radius,2*radius,2*radius);
        wxBitmap maskbmp(2*radius,2*radius,24);
        wxMemoryDC maskdc;
        // create bitmap with circle for masking
        maskdc.SelectObject(maskbmp);
        maskdc.SetBackground(*wxBLACK_BRUSH);
        maskdc.Clear();
        maskdc.SetBrush(*wxWHITE_BRUSH);
        maskdc.SetPen(*wxWHITE_PEN);
        maskdc.DrawCircle(radius,radius,radius);
        // now setup a memdc for rendering our object
        wxBitmap bmp(2*radius,2*radius,24);
        wxMemoryDC memdc;
        memdc.SelectObject(bmp);
        // set the origin so (x,y) is in the bmp center
        memdc.SetDeviceOrigin(radius-x,radius-y);
        // a region will be used to see if the result is empty
        wxRegion rgn2;
        while (pt) 
        {
            obj = pt->GetData();
            if (obj->IsBounded() && viewrect.Intersects(obj->GetBounds()))
            {
                // start clean
                //memdc.Clear();
                memdc.SetBrush(bgbrush);
                memdc.SetPen(bgpen);
                memdc.DrawRectangle(viewrect);
                // draw the object
                obj->DrawToDC(&memdc);
                // remove background color
                memdc.SetLogicalFunction(wxXOR);
                memdc.SetBrush(bgbrush);
                memdc.SetPen(bgpen);
                memdc.DrawRectangle(viewrect);
                memdc.SetLogicalFunction(wxCOPY);
                // AND with circle bitmap
                memdc.Blit(x-radius,y-radius,2*radius,2*radius,&maskdc,0,0,wxAND);
                // clear and update rgn2
                memdc.SelectObject(wxNullBitmap);
                rgn2.Clear();
                rgn2.Union(bmp, *wxBLACK);
                //rgn2.Intersect(rgn);
                memdc.SelectObject(bmp);
                if (!rgn2.IsEmpty())
                {
                    PyObject* pyObj = PyInt_FromLong((long)obj->GetId());
                    PyList_Insert(pyList, 0, pyObj);
                    Py_DECREF(pyObj);
                }
            }
            pt = pt->GetNext();
        }
        maskdc.SelectObject(wxNullBitmap);
        memdc.SelectObject(wxNullBitmap);
    }
    //wxPyEndBlockThreads(blocked);
    return pyList;
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
        
