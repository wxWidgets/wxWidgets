/////////////////////////////////////////////////////////////////////////////
// Name:        canvas.cpp
// Author:      Robert Roebling
// Created:     XX/XX/XX
// Copyright:   2000 (c) Robert Roebling
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "canvas.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/canvas/canvas.h"
#include "wx/canvas/polygon.h"
#include "wx/canvas/liner.h"

#ifdef __WXGTK__
    #include <gtk/gtk.h>
    #include <gdk/gdkrgb.h>
    #include "wx/gtk/win_gtk.h"
#endif

#ifndef wxUSE_FREETYPE
    #define wxUSE_FREETYPE 0
#endif

#if wxUSE_FREETYPE
    #include <freetype/freetype.h>
#endif

//#define CANVASDEBUG

//----------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------

const double pi = 3.1415926535;

#if wxUSE_FREETYPE
FT_Library g_freetypeLibrary;
#endif

//----------------------------------------------------------------------------
// wxCanvasObject
//----------------------------------------------------------------------------

wxCanvasObject::wxCanvasObject()
{
    // the default event handler is just this object
    m_eventHandler=this;
    m_admin = NULL;
    m_isControl = FALSE;
    m_isVector = FALSE;
    m_isImage = FALSE;
    m_visible  = TRUE;
    m_dragmode = DRAG_ONTOP;
//  handy when debugging
//  m_dragmode = DRAG_RECTANGLE;
    m_dragable = TRUE;
}

bool wxCanvasObject::ProcessCanvasObjectEvent(wxEvent& event)
{
    return m_eventHandler->ProcessEvent(event);
}

void wxCanvasObject::PushEventHandler(wxEvtHandler *handler)
{
    handler->SetNextHandler(GetEventHandler());
    m_eventHandler=handler;
}

wxEvtHandler *wxCanvasObject::PopEventHandler(bool deleteHandler)
{
    wxEvtHandler *handlerA = m_eventHandler;
    if ( handlerA )
    {
        wxEvtHandler *handlerB = handlerA->GetNextHandler();
        handlerA->SetNextHandler((wxEvtHandler *)NULL);
        m_eventHandler=handlerB;
        if ( deleteHandler )
        {
            delete handlerA;
            handlerA = (wxEvtHandler *)NULL;
        }
    }

    return handlerA;
}

void wxCanvasObject::AppendEventHandler(wxEvtHandler *handler)
{
    GetEventHandler()->SetNextHandler(handler);
}

wxEvtHandler *wxCanvasObject::RemoveLastEventHandler(bool deleteHandler)
{
    //always the first in the row
    wxEvtHandler *handlerA = m_eventHandler;
    wxEvtHandler *handlerB=handlerA;
    //goto the end
    while ( handlerA->GetNextHandler() )
    {
        handlerB = handlerA;
        handlerA = handlerA->GetNextHandler();
    }

    handlerB->SetNextHandler((wxEvtHandler *)NULL);
    if ( deleteHandler )
    {
        delete handlerA;
    }

    return GetEventHandler();
}

wxRect wxCanvasObject::GetAbsoluteArea(const wxTransformMatrix& cworld)
{
    wxBoundingBox tmp=m_bbox;
    tmp.MapBbox(cworld);

    int x1 = m_admin->LogicalToDeviceX( tmp.GetMinX() );
    int y1 = m_admin->LogicalToDeviceY( tmp.GetMinY() );
    int x2 = m_admin->LogicalToDeviceX( tmp.GetMaxX() );
    int y2 = m_admin->LogicalToDeviceY( tmp.GetMaxY() );

    if (x1 > x2)
    {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2)
    {
        int tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    wxRect tmparea;
    tmparea.x = x1;
    tmparea.y = y1;
    tmparea.width = x2-x1;    // FIXME +1 ?
    tmparea.height = y2-y1;   // FIXME +1 ?

    return tmparea;
}

void wxCanvasObject::MoveAbsolute( double x, double y )
{
    //save old position of boundingbox
    double oldx = GetXMin();
    double oldy = GetYMin();
    double w = m_bbox.GetWidth();
    double h = m_bbox.GetHeight();

    SetPosXY(x,y);

    double newx=GetXMin();
    double newy=GetYMin();

    double leftu,rightu,bottomu,topu ;
    leftu   = wxMin (oldx, newx ) ;
    rightu  = wxMax (oldx + w, newx + w) ;
    topu    = wxMin (oldy, newy) ;
    bottomu = wxMax (oldy + h, newy + h) ;

    if ( rightu - leftu < 2*w && bottomu - topu < 2*h )
    {
        m_admin->Update( this,leftu, topu, rightu - leftu, bottomu - topu);
    }
    else
    {
        m_admin->Update( this, oldx, oldy, w, h );
        m_admin->Update( this, newx, newy, w, h );
    }
}

void wxCanvasObject::MoveRelative( double x, double y )
{
    //save old position of boundingbox
    double oldx = GetXMin();
    double oldy = GetYMin();
    double w = m_bbox.GetWidth();
    double h = m_bbox.GetHeight();

    TransLate(x,y);

    double newx=GetXMin();
    double newy=GetYMin();

    double leftu,rightu,bottomu,topu ;
    leftu   = wxMin (oldx, newx ) ;
    rightu  = wxMax (oldx + w, newx + w) ;
    topu    = wxMin (oldy, newy) ;
    bottomu = wxMax (oldy + h, newy + h) ;

    if ( rightu - leftu < 2*w && bottomu - topu < 2*h )
    {
        m_admin->Update( this,leftu, topu, rightu - leftu, bottomu - topu);
    }
    else
    {
        m_admin->Update( this, oldx, oldy, w, h );
        m_admin->Update( this, newx, newy, w, h );
    }
}


void wxCanvasObject::DragStart()
{
    if (m_dragmode == DRAG_RECTANGLE)
    {
        this->SetVisible(FALSE);
        wxTransformMatrix help;
        double x = GetXMin();
        double y = GetYMin();
        double w = m_bbox.GetWidth();
        double h = m_bbox.GetHeight();
        m_admin->Update( this, x, y, w, h );
        m_admin->UpdateNow();

        wxRect recold=GetAbsoluteArea(help);
        wxClientDC dc(m_admin->GetActive());
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetLogicalFunction(wxINVERT);
        dc.DrawRectangle(recold);
        dc.SetBrush(wxNullBrush);
        dc.SetPen(wxNullPen);
    }
    else
    {
        this->SetVisible(FALSE);
        wxTransformMatrix help;
        double x = GetXMin();
        double y = GetYMin();
        double w = m_bbox.GetWidth();
        double h = m_bbox.GetHeight();

        wxRect recnew=GetAbsoluteArea(help);

         //redraw in buffer what should be there without this object
        m_admin->Update( this, x, y, w, h );
        m_admin->GetActive()->Freeze();

        //save the drawing (without the object itself to a bitmap)
        m_atnewpos = wxBitmap(recnew.width,recnew.height);
        wxMemoryDC dcm;
        dcm.SelectObject(*m_admin->GetActive()->GetBuffer());
        wxMemoryDC tmp;
        tmp.SelectObject(m_atnewpos);
        tmp.Blit(0,0,recnew.width,recnew.height,&dcm,recnew.x,recnew.y,wxCOPY,FALSE);
        tmp.SelectObject(wxNullBitmap);
        dcm.SelectObject(wxNullBitmap);
    }
}


void wxCanvasObject::DragRelative( double x, double y)
{
    if (m_dragmode == DRAG_RECTANGLE)
    {
        wxTransformMatrix help;

        wxRect recold=GetAbsoluteArea(help);

        TransLate(x,y);

        wxRect recnew=GetAbsoluteArea(help);

        wxClientDC dc(m_admin->GetActive());
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetLogicalFunction(wxINVERT);
        dc.DrawRectangle(recold);
        dc.DrawRectangle(recnew);
        dc.SetBrush(wxNullBrush);
        dc.SetPen(wxNullPen);
    }
    else
    {
        wxClientDC dc(m_admin->GetActive());
        wxMemoryDC tmp;

        wxTransformMatrix help;
        wxRect recold=GetAbsoluteArea(help);

        //restore what was there (without the object itself)
        wxMemoryDC dcm;
        dcm.SelectObject(*m_admin->GetActive()->GetBuffer());
        tmp.SelectObject(m_atnewpos);
        dcm.Blit(recold.x,recold.y,recold.width,recold.height,&tmp,0,0,wxCOPY,FALSE);

        TransLate(x,y);

        wxRect recnew=GetAbsoluteArea(help);

        //save the contents of the buffer at the new position
        tmp.Blit(0,0,recnew.width,recnew.height,&dcm,recnew.x,recnew.y,wxCOPY,FALSE);
        tmp.SelectObject(wxNullBitmap);

        //m_atnewpos = m_admin->GetActive()->GetBuffer()->GetSubBitmap( recnew );

        this->SetVisible(TRUE);
        //redraw object into the buffer
        m_admin->GetActive()->SetDC(&dcm);
        Render(&help,recnew.x,recnew.y,recnew.width,recnew.height);

        //draw the union or seperate to the canvas
        double leftu,rightu,bottomu,topu ;
        leftu   = wxMin (recold.x, recnew.x ) ;
        rightu  = wxMax (recold.x + recold.width, recnew.x + recnew.width ) ;
        topu    = wxMin (recold.y, recnew.y) ;
        bottomu = wxMax (recold.y + recold.height, recnew.y + recnew.height) ;

        if ( rightu - leftu < 2*recold.width && bottomu - topu < 2*recold.height)
        {
            dc.Blit(leftu,topu,rightu - leftu,bottomu - topu,&dcm,leftu,topu,wxCOPY,FALSE);
        }
        else
        {
            //do them seperate
            //first redraw what should be at the old position in the canvas
            dc.Blit(recold.x,recold.y,recold.width,recold.height,&dcm,recold.x,recold.y,wxCOPY,FALSE);
            //blit the new position of the object to the canvas
            dc.Blit(recnew.x,recnew.y,recnew.width,recnew.height,&dcm,recnew.x,recnew.y,wxCOPY,FALSE);
        }
        dcm.SelectObject(wxNullBitmap);
        this->SetVisible(FALSE);
    }
}


void wxCanvasObject::DragEnd()
{
    m_atnewpos = wxBitmap(0,0);
    m_admin->GetActive()->Thaw();
    this->SetVisible(TRUE);
    double x = GetXMin();
    double y = GetYMin();
    double w = m_bbox.GetWidth();
    double h = m_bbox.GetHeight();
    m_admin->Update( this, x, y, w, h );
    m_admin->UpdateNow();
}

wxCanvasObject* wxCanvasObject::IsHitWorld( double x, double y, double margin )
{
    if ((x >= m_bbox.GetMinX()-margin) &&
        (x <= m_bbox.GetMaxX()+margin) &&
        (y >= m_bbox.GetMinY()-margin) &&
        (y <= m_bbox.GetMaxY()+margin))
    {
        return this;
    }
    
    return (wxCanvasObject*) NULL;
}

wxCanvasObject* wxCanvasObject::Contains( wxCanvasObject* obj )
{
    if (obj == this)
        return this;
        
    return (wxCanvasObject*) NULL;
}

void wxCanvasObject::CaptureMouse()
{
    m_admin->GetActive()->SetCaptureMouse( this );
}

void wxCanvasObject::ReleaseMouse()
{
    m_admin->GetActive()->SetCaptureMouse( NULL );
}

bool wxCanvasObject::IsCapturedMouse()
{
    return (m_admin->GetActive()->GetCaptured() == this);
}


void wxCanvasObject::Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
}

void wxCanvasObject::CalcBoundingBox()
{
}

void wxCanvasObject::WriteSVG( wxTextOutputStream &stream )
{
}

//----------------------------------------------------------------------------
// wxCanvasObjectGroup
//----------------------------------------------------------------------------

wxCanvasObjectGroup::wxCanvasObjectGroup(double x, double y)
{
    lworld.Translate(x,y);
    //no objects make the bounding box the x,y and take care of it later
    m_bbox.Expand(x,y);
}

wxCanvasObjectGroup::~wxCanvasObjectGroup()
{
}

void wxCanvasObjectGroup::PushEventHandler(wxEvtHandler *handler)
{
    wxCanvasObject::PushEventHandler(handler);
    wxNode *node = m_objects.First();
    while (node)
    {
        wxCanvasObject *obj = (wxCanvasObject*) node->Data();

        obj->PushEventHandler(handler);

        node = node->Next();
    }
}

wxEvtHandler *wxCanvasObjectGroup::PopEventHandler(bool deleteHandler)
{
    wxNode *node = m_objects.First();
    while (node)
    {
        wxCanvasObject *obj = (wxCanvasObject*) node->Data();

        obj->PopEventHandler(deleteHandler);

        node = node->Next();
    }
    return wxCanvasObject::PopEventHandler(deleteHandler);
}

void wxCanvasObjectGroup::AppendEventHandler(wxEvtHandler *handler)
{
    wxCanvasObject::AppendEventHandler(handler);
    wxNode *node = m_objects.First();
    while (node)
    {
        wxCanvasObject *obj = (wxCanvasObject*) node->Data();

        obj->AppendEventHandler(handler);

        node = node->Next();
    }
}

wxEvtHandler *wxCanvasObjectGroup::RemoveLastEventHandler(bool deleteHandler)
{
    wxNode *node = m_objects.First();
    while (node)
    {
        wxCanvasObject *obj = (wxCanvasObject*) node->Data();

        obj->RemoveLastEventHandler(deleteHandler);

        node = node->Next();
    }
    return wxCanvasObject::RemoveLastEventHandler(deleteHandler);
}

void wxCanvasObjectGroup::TransLate( double x, double y )
{
    lworld.Translate(x,y);
    CalcBoundingBox();
}

void wxCanvasObjectGroup::SetAdmin(wxCanvasAdmin* admin)
{
    m_admin=admin;
    wxNode *node = m_objects.First();
    while (node)
    {
        wxCanvasObject *obj = (wxCanvasObject*) node->Data();

        obj->SetAdmin(admin);

        node = node->Next();
    }
}

void wxCanvasObjectGroup::DeleteContents( bool flag)
{
    m_objects.DeleteContents( flag );
    m_bbox.SetValid(FALSE);
    CalcBoundingBox();
}

void wxCanvasObjectGroup::Prepend( wxCanvasObject* obj )
{
    m_objects.Insert( obj );
    if (m_objects.First())
    {
        m_bbox.Expand(obj->GetBbox());
    }
    else
    {
        m_bbox.SetValid(FALSE);
        CalcBoundingBox();
    }
}

void wxCanvasObjectGroup::Append( wxCanvasObject* obj )
{
    m_objects.Append( obj );
    if (m_objects.First())
    {
        m_bbox.Expand(obj->GetBbox());
    }
    else
    {
        m_bbox.SetValid(FALSE);
        CalcBoundingBox();
    }
}

void wxCanvasObjectGroup::Insert( size_t before, wxCanvasObject* obj )
{
    m_objects.Insert( before, obj );
    m_bbox.SetValid(FALSE);
    if (m_objects.First())
    {
        m_bbox.Expand(obj->GetBbox());
    }
    else
    {
        m_bbox.SetValid(FALSE);
        CalcBoundingBox();
    }
}

void wxCanvasObjectGroup::Remove( wxCanvasObject* obj )
{
    m_objects.DeleteObject( obj );
    m_bbox.SetValid(FALSE);
    CalcBoundingBox();
}

void wxCanvasObjectGroup::CalcBoundingBox()
{
    m_bbox.SetValid(FALSE);
    wxNode *node = m_objects.First();
    while (node)
    {
        wxCanvasObject *obj = (wxCanvasObject*) node->Data();


        obj->CalcBoundingBox();
        wxBoundingBox tmp;
        tmp=obj->GetBbox();
        tmp.MapBbox(lworld);

        m_bbox.Expand( tmp );
        node = node->Next();
    }
}

void wxCanvasObjectGroup::Render(wxTransformMatrix* cworld, int x, int y, int width, int height )
{
    if (!m_visible) return;

    wxTransformMatrix backup = *cworld;
    *cworld *= lworld;

    wxNode *node = m_objects.First();

    if (!node) return;


#ifdef CANVASDEBUG
    wxRect absarea=GetAbsoluteArea(*cworld);
    wxDC *dc = m_admin->GetActive()->GetDC();
    dc->SetPen(*wxBLACK_PEN);
    dc->SetBrush(*wxTRANSPARENT_BRUSH);
    dc->DrawRectangle( absarea.x , absarea.y , absarea.width , absarea.height );
    dc->SetBrush(wxNullBrush);
    dc->SetPen(wxNullPen);
#endif
    //TODO the next will only work if all boundingboxes stay up to date (problem when mowing currently
    /*
    if (! ( wxMax(absarea.x, x) < wxMin(absarea.x + absarea.width , x + width ) &&
            wxMax(absarea.y, y) < wxMin(absarea.y + absarea.height , y + height )
          )
       )
        return;
    */

    // cycle through all objects
    while (node)
    {
        wxCanvasObject *obj = (wxCanvasObject*) node->Data();

        if (!obj->IsControl() && obj->GetVisible())
        {

            //get area at the absolute position
            wxRect absareaobject = obj->GetAbsoluteArea(*cworld);
            
            // If we have 10.000 objects, we will go through
            // this 10.000 times for each update, so we have
            // to optimise carefully.
            int clip_x = absareaobject.x;
            int clip_width = absareaobject.width;
            if (clip_x < x)
            {
                clip_width -= x-clip_x;
                clip_x = x;
            }
            if (clip_width > 0)
            {
                if (clip_x + clip_width > x + width)
                    clip_width = x+width-clip_x;

                if (clip_width > 0)
                {
                    int clip_y = absareaobject.y;
                    int clip_height = absareaobject.height;
                    if (clip_y < y)
                    {
                        clip_height -= y-clip_y;
                        clip_y = y;
                    }
                    if (clip_height > 0)
                    {
                        if (clip_y + clip_height > y + height)
                            clip_height = y+height-clip_y;

                        if (clip_height > 0)
                        {
                            obj->Render(cworld, clip_x, clip_y, clip_width, clip_height );
                        }
                    }
                }
            }
        }

        node = node->Next();
    }
    *cworld = backup;
}

void wxCanvasObjectGroup::WriteSVG( wxTextOutputStream &stream )
{
}

wxCanvasObject* wxCanvasObjectGroup::IsHitWorld( double x, double y, double margin )
{
    //KKKfirst check if within bbox
    //will only work if they are always uptodate
    //if (!m_bbox.PointInBox(x,y,margin))
    //    return (wxCanvasObject*) NULL;

    wxTransformMatrix inverse =  lworld;
    double xh,yh;
    inverse.Invert();
    inverse.TransformPoint(x,y,xh,yh);

    wxCanvasObject *obj=0;
    wxNode *node = m_objects.Last();
    while (node)
    {
        obj=(wxCanvasObject*) node->Data();

        if (!obj->IsControl() )
        {
            if (obj->IsHitWorld(x,y,margin))
            {
                return obj;
            }
        }
        node = node->Previous();
    }

    return (wxCanvasObject*) NULL;
}

wxCanvasObject* wxCanvasObjectGroup::Contains( wxCanvasObject* obj )
{
    wxCanvasObject* cobj;
    wxNode *node = m_objects.First();
    while (node)
    {
        cobj=(wxCanvasObject*) node->Data();

        if (cobj->Contains(obj))
        {
            return obj;
        }
        node = node->Next();
    }

    return (wxCanvasObject*) NULL;
}

int wxCanvasObjectGroup::IndexOf( wxCanvasObject* obj )
{
    return m_objects.IndexOf( obj );
}

//----------------------------------------------------------------------------
// wxCanvasObjectRef
//----------------------------------------------------------------------------

wxCanvasObjectRef::wxCanvasObjectRef(double x, double y, wxCanvasObject* obj)
   : wxCanvasObject()
{
    lworld.Translate(x,y);
    m_obj = obj;

    m_bbox.SetValid(FALSE);
    wxBoundingBox tmp;
    tmp=obj->GetBbox();
    tmp.MapBbox(lworld);
    m_bbox.Expand( tmp );
}

void wxCanvasObjectRef::PushEventHandler(wxEvtHandler *handler)
{
    wxCanvasObject::PushEventHandler(handler);
    m_obj->PushEventHandler(handler);
}

wxEvtHandler *wxCanvasObjectRef::PopEventHandler(bool deleteHandler)
{
    m_obj->PopEventHandler(deleteHandler);
    return wxCanvasObject::PopEventHandler(deleteHandler);
}

void wxCanvasObjectRef::AppendEventHandler(wxEvtHandler *handler)
{
    wxCanvasObject::AppendEventHandler(handler);
    m_obj->AppendEventHandler(handler);
}

wxEvtHandler *wxCanvasObjectRef::RemoveLastEventHandler(bool deleteHandler)
{
    m_obj->RemoveLastEventHandler(deleteHandler);
    return wxCanvasObject::RemoveLastEventHandler(deleteHandler);
}

void wxCanvasObjectRef::TransLate( double x, double y )
{
    lworld.Translate(x,y);
    CalcBoundingBox();
}

wxCanvasObject* wxCanvasObjectRef::Contains( wxCanvasObject* obj )
{
    if (obj == this || m_obj->Contains(obj))
        return this;

    return (wxCanvasObject*) NULL;
}


void wxCanvasObjectRef::SetRotation(double rotation)
{
    lworld.SetRotation(rotation);
    CalcBoundingBox();
}

void wxCanvasObjectRef::SetScale(double scalex,double scaley)
{
    lworld.Scale(scalex,scaley,lworld.GetValue(2,0),lworld.GetValue(2,1));
    CalcBoundingBox();
}

void wxCanvasObjectRef::SetAdmin(wxCanvasAdmin* admin)
{
    m_admin = admin;
    m_obj->SetAdmin(admin);
}

void wxCanvasObjectRef::CalcBoundingBox()
{
    m_bbox.SetValid(FALSE);
    m_obj->CalcBoundingBox();

    wxBoundingBox tmp;
    tmp=m_obj->GetBbox();
    tmp.MapBbox(lworld);
    m_bbox.Expand( tmp );
}

void wxCanvasObjectRef::Render(wxTransformMatrix* cworld, int x, int y, int width, int height )
{
    if (!m_visible) return;

    //get the absolute area (without the local matrix included)
    //the boundingbox is relative to the parent.
    wxRect absarea=GetAbsoluteArea(*cworld);

    wxTransformMatrix backup = *cworld;
    *cworld *= lworld;

#ifdef CANVASDEBUG
    wxDC *dc = m_admin->GetActive()->GetDC();
    dc->SetPen(*wxBLACK_PEN);
    dc->SetBrush(*wxTRANSPARENT_BRUSH);
    dc->DrawRectangle( absarea.x , absarea.y , absarea.width , absarea.height );
    dc->SetBrush(wxNullBrush);
    dc->SetPen(wxNullPen);
#endif

    int clip_x = absarea.x;
    int clip_width = absarea.width;
    if (clip_x < x)
    {
        clip_width -= x-clip_x;
        clip_x = x;
    }
    if (clip_width > 0)
    {
        if (clip_x + clip_width > x + width)
            clip_width = x+width-clip_x;

        if (clip_width > 0)
        {
            int clip_y = absarea.y;
            int clip_height = absarea.height;
            if (clip_y < y)
            {
                clip_height -= y-clip_y;
                clip_y = y;
            }
            if (clip_height > 0)
            {
                if (clip_y + clip_height > y + height)
                    clip_height = y+height-clip_y;

                if (clip_height > 0)
                    m_obj->Render(cworld, clip_x, clip_y, clip_width, clip_height );
            }
        }
    }

    *cworld = backup;
}

void wxCanvasObjectRef::WriteSVG( wxTextOutputStream &stream )
{
}

wxCanvasObject* wxCanvasObjectRef::IsHitWorld( double x, double y, double margin )
{
    //KKKfirst check if within bbox
    //will only work if they are always uptodate
    //if (!m_bbox.PointInBox(x,y,margin))
    //    return (wxCanvasObject*) NULL;

    wxTransformMatrix inverse =  lworld;
    double xh,yh;
    inverse.Invert();
    inverse.TransformPoint(x,y,xh,yh);

    if (m_obj->IsHitWorld(xh,yh,margin))
        return this;

    return (wxCanvasObject*) NULL;
}



//----------------------------------------------------------------------------
// wxCanvasRect
//----------------------------------------------------------------------------

wxCanvasRect::wxCanvasRect( double x, double y, double w, double h , double radius )
   : wxCanvasObject()
{
    m_x = x;
    m_y = y;
    m_width = w;
    m_height = h;
    m_radius = radius;

    m_brush = *wxBLACK_BRUSH;
    m_pen = *wxTRANSPARENT_PEN;
    CalcBoundingBox();
}

void wxCanvasRect::TransLate( double x, double y )
{
    m_x += x;
    m_y += y;
    CalcBoundingBox();
}

void wxCanvasRect::CalcBoundingBox()
{
    m_bbox.SetMin( m_x , m_y);
    m_bbox.SetMax( m_x + m_width ,m_y + m_height );

    //include the pen width also
//KKK    m_bbox.EnLarge(m_pen.GetWidth()+m_radius);
    m_bbox.EnLarge(m_pen.GetWidth()/2);
}

void wxCanvasRect::Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
    if (!m_visible) return;

    if (cworld->GetRotation())
    {
        wxPoint *cpoints = new wxPoint[4];
        double x;
        double y;
        cworld->TransformPoint( m_x, m_y, x, y );
        cpoints[0].x = m_admin->LogicalToDeviceX(x);
        cpoints[0].y = m_admin->LogicalToDeviceY(y);
        cworld->TransformPoint( m_x , m_y + m_height, x, y );
        cpoints[1].x = m_admin->LogicalToDeviceX(x);
        cpoints[1].y = m_admin->LogicalToDeviceY(y);
        cworld->TransformPoint( m_x + m_width, m_y + m_height, x, y );
        cpoints[2].x = m_admin->LogicalToDeviceX(x);
        cpoints[2].y = m_admin->LogicalToDeviceY(y);
        cworld->TransformPoint( m_x + m_width, m_y , x, y );
        cpoints[3].x = m_admin->LogicalToDeviceX(x);
        cpoints[3].y = m_admin->LogicalToDeviceY(y);

        wxDC *dc = m_admin->GetActive()->GetDC();
        dc->SetClippingRegion(clip_x, clip_y, clip_width, clip_height);
        dc->SetBrush(m_brush);
        int pw=m_pen.GetWidth();
        m_pen.SetWidth(m_admin->LogicalToDeviceXRel(pw));
        dc->SetPen(m_pen);
        dc->DrawPolygon(4, cpoints, 0,0,wxWINDING_RULE);
        delete [] cpoints;
        dc->SetBrush(wxNullBrush);
        dc->SetPen(wxNullPen);
        dc->DestroyClippingRegion();
        m_pen.SetWidth(pw);
    }
    else
    {
        wxDC *dc = m_admin->GetActive()->GetDC();
        dc->SetClippingRegion(clip_x, clip_y, clip_width, clip_height);
        dc->SetBrush(m_brush);
        int pw=m_pen.GetWidth();
        m_pen.SetWidth(m_admin->LogicalToDeviceXRel(pw));
        dc->SetPen(m_pen);
        //yes the whole not only the clipping region, because we have a pen also
        int x = m_admin->LogicalToDeviceX(cworld->GetValue(2,0) + m_x );
        int y = m_admin->LogicalToDeviceY(cworld->GetValue(2,1) + m_y );
        int w = m_admin->LogicalToDeviceXRel( m_width );
        int h = m_admin->LogicalToDeviceYRel( m_height );
        int r = m_admin->LogicalToDeviceYRel( m_radius );
        if (w > 0 && w < 1) w=1;
        if (w < 0 && w > -1) w=-1;
        if (h > 0 && h < 1) h=1;
        if (h < 0 && h > -1) h=-1;
        if (m_radius)
            dc->DrawRoundedRectangle( x,y,w,h,r);
        else
            dc->DrawRectangle( x,y,w,h);
        dc->SetBrush(wxNullBrush);
        dc->SetPen(wxNullPen);
        dc->DestroyClippingRegion();
        m_pen.SetWidth(pw);
    }
}

void wxCanvasRect::WriteSVG( wxTextOutputStream &stream )
{
}

//----------------------------------------------------------------------------
// wxCanvasCircle
//----------------------------------------------------------------------------

wxCanvasCircle::wxCanvasCircle( double x, double y, double radius )
   : wxCanvasObject()
{
    m_x = x;
    m_y = y;
    m_radius = radius;

    m_brush = *wxBLACK_BRUSH;
    m_pen = *wxTRANSPARENT_PEN;
    CalcBoundingBox();
}

void wxCanvasCircle::TransLate( double x, double y )
{
    m_x += x;
    m_y += y;
    CalcBoundingBox();
}

void wxCanvasCircle::CalcBoundingBox()
{
    m_bbox.SetMin( m_x-m_radius , m_y-m_radius );
    m_bbox.SetMax( m_x+m_radius , m_y+m_radius );

    //include the pen width also
    m_bbox.EnLarge(m_pen.GetWidth()/2);
}

void wxCanvasCircle::Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
    if (!m_visible) return;

    wxDC *dc = m_admin->GetActive()->GetDC();
    dc->SetClippingRegion(clip_x, clip_y, clip_width, clip_height);
    dc->SetBrush(m_brush);
    int pw=m_pen.GetWidth();
    m_pen.SetWidth(m_admin->LogicalToDeviceXRel(pw));
    dc->SetPen(m_pen);
    //yes the whole not only the clipping region, because we have a pen also
    //and rotation on a circle is not important so only a shift with cworld
    int x = m_admin->LogicalToDeviceX(cworld->GetValue(2,0) + m_x );
    int y = m_admin->LogicalToDeviceY(cworld->GetValue(2,1) + m_y );
    int radius = m_admin->LogicalToDeviceXRel( m_radius );
    if (radius < 1) radius=1;
    dc->DrawCircle( x,y,radius);
    dc->SetBrush(wxNullBrush);
    dc->SetPen(wxNullPen);
    dc->DestroyClippingRegion();
    m_pen.SetWidth(pw);
}

void wxCanvasCircle::WriteSVG( wxTextOutputStream &stream )
{
}

wxCanvasObject* wxCanvasCircle::IsHitWorld( double x, double y, double margin )
{
    if ((x >= m_bbox.GetMinX()-margin) &&
        (x <= m_bbox.GetMaxX()+margin) &&
        (y >= m_bbox.GetMinY()-margin) &&
        (y <= m_bbox.GetMaxY()+margin)
       )
    {
        if (m_radius+m_pen.GetWidth()/2+margin > sqrt(pow(m_x-x,2)+pow(m_y-y,2)))
            return this;
        else
            return (wxCanvasObject*) NULL;
    }
    return (wxCanvasObject*) NULL;
}

//----------------------------------------------------------------------------
// wxCanvasEllipse
//----------------------------------------------------------------------------

wxCanvasEllipse::wxCanvasEllipse( double x, double y, double width, double height )
   : wxCanvasObject()
{
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;

    m_brush = *wxBLACK_BRUSH;
    m_pen = *wxTRANSPARENT_PEN;
    CalcBoundingBox();
}

void wxCanvasEllipse::TransLate( double x, double y )
{
    m_x += x;
    m_y += y;
    CalcBoundingBox();
}

void wxCanvasEllipse::CalcBoundingBox()
{
    m_bbox.SetMin( m_x, m_y );
    m_bbox.SetMax( m_x+m_width , m_y+m_height );

    //include the pen width also
    m_bbox.EnLarge(m_pen.GetWidth()/2);
}

void wxCanvasEllipse::Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
    if (!m_visible) return;

    wxDC *dc = m_admin->GetActive()->GetDC();
    dc->SetClippingRegion(clip_x, clip_y, clip_width, clip_height);
    dc->SetBrush(m_brush);
    int pw=m_pen.GetWidth();
    m_pen.SetWidth(m_admin->LogicalToDeviceXRel(pw));
    dc->SetPen(m_pen);
    int x = m_admin->LogicalToDeviceX(cworld->GetValue(2,0) + m_x );
    int y = m_admin->LogicalToDeviceY(cworld->GetValue(2,1) + m_y );
    int w = m_admin->LogicalToDeviceXRel( m_width );
    int h = m_admin->LogicalToDeviceYRel( m_height );
    if (w > 0 && w < 1) w=1;
    if (w < 0 && w > -1) w=-1;
    if (h > 0 && h < 1) h=1;
    if (h < 0 && h > -1) h=-1;
    dc->DrawEllipse( x,y,w,h);
    dc->SetBrush(wxNullBrush);
    dc->SetPen(wxNullPen);
    dc->DestroyClippingRegion();
    m_pen.SetWidth(pw);
}

void wxCanvasEllipse::WriteSVG( wxTextOutputStream &stream )
{
}

wxCanvasObject* wxCanvasEllipse::IsHitWorld( double x, double y, double margin )
{
    if ((x >= m_bbox.GetMinX()-margin) &&
        (x <= m_bbox.GetMaxX()+margin) &&
        (y >= m_bbox.GetMinY()-margin) &&
        (y <= m_bbox.GetMaxY()+margin)
       )
    {
        double a=(m_width+m_pen.GetWidth())/2+margin ;
        double b=(m_height+m_pen.GetWidth())/2+margin;
        double c=pow((m_x+m_width/2-x)/a,2)+pow((m_y+m_height/2-y)/b,2);
        if ( 1 > c)
            return this;
        else
            return (wxCanvasObject*) NULL;
    }
    return (wxCanvasObject*) NULL;
}

//----------------------------------------------------------------------------
// wxCanvasEllipticArc
//----------------------------------------------------------------------------

wxCanvasEllipticArc::wxCanvasEllipticArc( double x, double y, double width, double height, double start, double end )
   : wxCanvasObject()
{
    m_x = x;
    m_y = y;
    m_width  = width;
    m_height = height;
    m_start  = start;
    m_end    = end;

    m_brush = *wxBLACK_BRUSH;
    m_pen = *wxTRANSPARENT_PEN;
    CalcBoundingBox();
}

void wxCanvasEllipticArc::TransLate( double x, double y )
{
    m_x += x;
    m_y += y;
    CalcBoundingBox();
}

void wxCanvasEllipticArc::CalcBoundingBox()
{
    m_bbox.SetMin( m_x, m_y );
    m_bbox.SetMax( m_x+m_width , m_y+m_height );

    //include the pen width also
    m_bbox.EnLarge(m_pen.GetWidth()/2);
}

void wxCanvasEllipticArc::Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
    if (!m_visible) return;

    wxDC *dc = m_admin->GetActive()->GetDC();
    dc->SetClippingRegion(clip_x, clip_y, clip_width, clip_height);
    dc->SetBrush(m_brush);
    int pw=m_pen.GetWidth();
    m_pen.SetWidth(m_admin->LogicalToDeviceXRel(pw));
    dc->SetPen(m_pen);
    int x = m_admin->LogicalToDeviceX(cworld->GetValue(2,0) + m_x );
    int y = m_admin->LogicalToDeviceY(cworld->GetValue(2,1) + m_y );
    int w = m_admin->LogicalToDeviceXRel( m_width );
    int h = m_admin->LogicalToDeviceYRel( m_height );
    if (w > 0 && w < 1) w=1;
    if (w < 0 && w > -1) w=-1;
    if (h > 0 && h < 1) h=1;
    if (h < 0 && h > -1) h=-1;
    if (m_admin->GetActive()->GetYaxis())
        dc->DrawEllipticArc( x,y,w,h,-m_end,-m_start);
    else
        dc->DrawEllipticArc( x,y,w,h,m_start,m_end);
    dc->SetBrush(wxNullBrush);
    dc->SetPen(wxNullPen);
    dc->DestroyClippingRegion();
    m_pen.SetWidth(pw);
}

void wxCanvasEllipticArc::WriteSVG( wxTextOutputStream &stream )
{
}

wxCanvasObject* wxCanvasEllipticArc::IsHitWorld( double x, double y, double margin )
{
    if ((x >= m_bbox.GetMinX()-margin) &&
        (x <= m_bbox.GetMaxX()+margin) &&
        (y >= m_bbox.GetMinY()-margin) &&
        (y <= m_bbox.GetMaxY()+margin)
       )
    {
        double a=(m_width+m_pen.GetWidth())/2+margin ;
        double b=(m_height+m_pen.GetWidth())/2+margin;
        double c=pow((m_x+m_width/2-x)/a,2)+pow((m_y+m_height/2-y)/b,2);
        if ( 1 > c)
            return this;
        else
            return (wxCanvasObject*) NULL;
    }
    return (wxCanvasObject*) NULL;
}

//----------------------------------------------------------------------------
// wxCanvasLine
//----------------------------------------------------------------------------

wxCanvasLine::wxCanvasLine( double x1, double y1, double x2, double y2 )
   : wxCanvasObject()
{
    m_x1 = x1;
    m_y1 = y1;
    m_x2 = x2;
    m_y2 = y2;

    m_pen = *wxBLACK_PEN;
    CalcBoundingBox();
}

void wxCanvasLine::TransLate( double x, double y )
{
    m_x1 += x;
    m_y1 += y;
    m_x2 += x;
    m_y2 += y;
    CalcBoundingBox();
}

void wxCanvasLine::CalcBoundingBox()
{
    m_bbox.SetMin( m_x1 , m_y1);
    m_bbox.SetMax( m_x2 , m_y2);

    //include the pen width also
    m_bbox.EnLarge(m_pen.GetWidth()/2);
}

void wxCanvasLine::Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
    if (!m_visible) return;

    double x1,y1,x2,y2;
    cworld->TransformPoint( m_x1, m_y1, x1, y1 );
    cworld->TransformPoint( m_x2, m_y2, x2, y2 );
    x1 = m_admin->LogicalToDeviceX( x1 );
    y1 = m_admin->LogicalToDeviceY( y1 );
    x2 = m_admin->LogicalToDeviceX( x2 );
    y2 = m_admin->LogicalToDeviceY( y2 );

    wxDC *dc = m_admin->GetActive()->GetDC();
    dc->SetClippingRegion( clip_x, clip_y, clip_width, clip_height );
    int pw=m_pen.GetWidth();
    m_pen.SetWidth(m_admin->LogicalToDeviceXRel(pw));
    dc->SetPen( m_pen );
    dc->DrawLine( x1, y1, x2, y2 );

    dc->DestroyClippingRegion();
    m_pen.SetWidth(pw);
}

void wxCanvasLine::WriteSVG( wxTextOutputStream &stream )
{
    // no idea
}

wxCanvasObject* wxCanvasLine::IsHitWorld( double x, double y, double margin )
{
    if ((x >= m_bbox.GetMinX()-margin) &&
        (x <= m_bbox.GetMaxX()+margin) &&
        (y >= m_bbox.GetMinY()-margin) &&
        (y <= m_bbox.GetMaxY()+margin)
       )
    {
        wxLine line1(m_x1,m_y1,m_x2,m_y2);
        wxPoint2DDouble P=wxPoint2DDouble(x,y);
        double distance;
        if (line1.PointInLine(P,distance,m_pen.GetWidth()/2+margin) == R_IN_AREA)
            return this;
        else
            return (wxCanvasObject*) NULL;
    }
    return (wxCanvasObject*) NULL;
}

//----------------------------------------------------------------------------
// wxCanvasImage
//----------------------------------------------------------------------------

wxCanvasImage::wxCanvasImage( const wxImage &image, double x, double y, double w, double h )
   : wxCanvasObject()
{
    m_x = x;
    m_y = y;
    m_width = w;
    m_height = h;

    m_image = image;

    m_orgw = m_image.GetWidth();
    m_orgh = m_image.GetHeight();

    m_isImage = TRUE;
    CalcBoundingBox();
}

void wxCanvasImage::TransLate( double x, double y )
{
    m_x += x;
    m_y += y;
    CalcBoundingBox();
}

void wxCanvasImage::CalcBoundingBox()
{
    m_bbox.SetMin( m_x, m_y );
    m_bbox.SetMax( m_x + m_width, m_y + m_height );
}

void wxCanvasImage::Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
    if (!m_visible) return;

    wxRect tmparea;

    tmparea.x = m_admin->LogicalToDeviceXRel( m_bbox.GetMinX());
    tmparea.y = m_admin->LogicalToDeviceYRel( m_bbox.GetMinY());
    tmparea.width = m_admin->LogicalToDeviceXRel( m_bbox.GetWidth() );
    tmparea.height = m_admin->LogicalToDeviceYRel( m_bbox.GetHeight() );

    double x;
    double y;
    cworld->TransformPoint( m_x, m_y, x, y );
    x = m_admin->LogicalToDeviceX(x);
    y = m_admin->LogicalToDeviceY(y);


    if (  m_orgw*5 < m_admin->LogicalToDeviceXRel( m_bbox.GetWidth()  ) ||
          m_orgw/5 > m_admin->LogicalToDeviceXRel( m_bbox.GetWidth()  ) ||
          m_orgh*5 < m_admin->LogicalToDeviceYRel( m_bbox.GetHeight() ) ||
          m_orgh/5 > m_admin->LogicalToDeviceYRel( m_bbox.GetHeight() )
       )
    {
        wxDC *dc = m_admin->GetActive()->GetDC();
        dc->SetClippingRegion(clip_x, clip_y, clip_width, clip_height);
        dc->SetBrush(*wxTRANSPARENT_BRUSH);
        dc->SetPen(*wxBLACK_PEN);
        //yes the whole not only the clipping region, because we have a pen also
        int x = m_admin->LogicalToDeviceX(cworld->GetValue(2,0) + m_x );
        int y = m_admin->LogicalToDeviceY(cworld->GetValue(2,1) + m_y );
        int w = m_admin->LogicalToDeviceXRel( m_width );
        int h = m_admin->LogicalToDeviceYRel( m_height );
        if (w < 1) w=1;
        if (h < 1) h=1;
        dc->DrawRectangle( x,y,w,h);
        dc->SetBrush(wxNullBrush);
        dc->SetPen(wxNullPen);
        dc->DestroyClippingRegion();
        return;
    }

    if ((m_admin->LogicalToDeviceXRel( m_bbox.GetWidth() ) == m_image.GetWidth()) &&
        (m_admin->LogicalToDeviceYRel( m_bbox.GetHeight() ) == m_image.GetHeight()))
    {
        m_tmp = m_image;
    }
    else
    {
        m_tmp = m_image.Scale( m_admin->LogicalToDeviceXRel( m_bbox.GetWidth()),
                               m_admin->LogicalToDeviceYRel( m_bbox.GetHeight()) );
    }

    wxBitmap bmp;
//    wxPoint centr(m_admin->LogicalToDeviceX(m_x),m_admin->LogicalToDeviceY(m_y));
    wxPoint centr(0,0);

    if (cworld->GetRotation())
    {
        bmp=m_tmp.Rotate(-cworld->GetRotation()/180.0 * pi,centr, TRUE,  NULL).ConvertToBitmap();
    }
    else
    {
        bmp = m_tmp.ConvertToBitmap();
    }

    wxDC *dc = m_admin->GetActive()->GetDC();

    wxPoint centr2;
    if (cworld->GetRotation()> 0)
    {
        centr2.x= (int) (x+m_height*sin(-cworld->GetRotation()/180.0 * pi));
        centr2.y= (int) y;
    }
    else
    {
        centr2.x= (int) x;
        centr2.y= (int) (y-m_width*sin(-cworld->GetRotation()/180.0 * pi));
    }

    if (cworld->GetRotation() != 0)
    {
        //TODO clipping not right
            dc->DrawBitmap(bmp,centr2,TRUE );
//        dc->DrawPoint(centr2);
//        dc->DrawPoint(x,y);
    }
    else
    {
        //TODO clipping not right
//        dc->DrawPoint(centr2);
//        dc->DrawPoint(x,y);

        if ((clip_x == x) &&
            (clip_y == y) &&
            (clip_width == tmparea.width) &&
            (clip_height == tmparea.height))
        {
            dc->DrawBitmap( m_tmp, clip_x, clip_y, TRUE );
        }
        else
        {
            int start_x = clip_x - (int)x;
            int start_y = clip_y - (int)y;

            //dc->DrawBitmap( bmp, x, y, TRUE );
            wxMemoryDC dcm;
            dcm.SelectObject(bmp);
            dc->Blit(clip_x, clip_y,clip_width, clip_height,&dcm,start_x,start_y,wxCOPY,TRUE);
            dcm.SelectObject(wxNullBitmap);
        }
    }
}

void wxCanvasImage::WriteSVG( wxTextOutputStream &stream )
{
    // no idea
}

//----------------------------------------------------------------------------
// wxCanvasCtrl
//----------------------------------------------------------------------------

wxCanvasControl::wxCanvasControl( wxWindow *control )
   : wxCanvasObject()
{
    m_isControl = TRUE;
    m_control = control;
    CalcBoundingBox();
}

double wxCanvasControl::GetPosX()
{
    int x,y ;
    m_control->GetPosition( &x, &y );
    return m_admin->DeviceToLogicalX(x);
}

double wxCanvasControl::GetPosY()
{
    int x,y ;
    m_control->GetPosition( &x, &y );
    return m_admin->DeviceToLogicalY(y);
}

void wxCanvasControl::SetPosXY( double x, double y)
{
    int xd = m_admin->LogicalToDeviceX(x);
    int yd = m_admin->LogicalToDeviceY(y);
    m_control->Move(xd,yd);
}


void wxCanvasControl::TransLate( double x, double y )
{
    int xdo,ydo;
    m_control->GetPosition( &xdo, &ydo );
    int xd = m_admin->LogicalToDeviceX(x)-xdo;
    int yd = m_admin->LogicalToDeviceY(y)-ydo;
    m_control->Move(xd,yd);
    CalcBoundingBox();
}

wxCanvasControl::~wxCanvasControl()
{
    m_control->Destroy();
}

void wxCanvasControl::CalcBoundingBox()
{
    wxRect tmparea;

    m_control->GetSize( &tmparea.width, &tmparea.height );
    m_control->GetPosition( &tmparea.x, &tmparea.y );

    m_bbox.SetMin( tmparea.x , tmparea.y);
    m_bbox.SetMax( tmparea.x + tmparea.width , tmparea.y + tmparea.height);

}

void wxCanvasControl::MoveRelative( double x, double y )
{
    m_control->Move( m_admin->LogicalToDeviceX(x),  m_admin->LogicalToDeviceX(y) );
}

//----------------------------------------------------------------------------
// wxCanvasText
//----------------------------------------------------------------------------

class wxFaceData
{
public:
#if wxUSE_FREETYPE
     FT_Face   m_face;
#else
     void     *m_dummy;
#endif
};

wxCanvasText::wxCanvasText( const wxString &text, double x, double y, const wxString &fontFile, int size )
   : wxCanvasObject()
{
    m_text = text;
    m_fontFileName = fontFile;
    m_size = size;

    m_red = 0;
    m_green = 0;
    m_blue = 0;

    m_alpha = NULL;

    m_x = x;
    m_y = y;

#if wxUSE_FREETYPE
    wxFaceData *data = new wxFaceData;
    m_faceData = data;

    int error = FT_New_Face( g_freetypeLibrary,
                             m_fontFileName,
                             0,
                             &(data->m_face) );

    error = FT_Set_Char_Size( data->m_face,
                              0,
                              m_size*64,
                              96,    // screen dpi
                              96 );
#endif
    CalcBoundingBox();
}

wxCanvasText::~wxCanvasText()
{
#if wxUSE_FREETYPE
    wxFaceData *data = (wxFaceData*) m_faceData;
    delete data;
#endif

    if (m_alpha) delete [] m_alpha;
}

void wxCanvasText::SetRGB( unsigned char red, unsigned char green, unsigned char blue )
{
    m_red = red;
    m_green = green;
    m_blue = blue;
}

void wxCanvasText::SetFlag( int flag )
{
    m_flag = flag;
}

void wxCanvasText::Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
    if (!m_visible) return;

    wxRect tmparea;
    tmparea.x = m_admin->LogicalToDeviceX( m_bbox.GetMinX());
    tmparea.y = m_admin->LogicalToDeviceY( m_bbox.GetMinY());
    tmparea.width = m_admin->LogicalToDeviceXRel( m_bbox.GetWidth() );
    tmparea.height = m_admin->LogicalToDeviceYRel( m_bbox.GetHeight() );

    m_alpha = new unsigned char[tmparea.width*tmparea.height];
    memset( m_alpha, 0, tmparea.width*tmparea.height );

    if (!m_alpha) return;

#if wxUSE_FREETYPE
    FT_Face face = ((wxFaceData*)m_faceData)->m_face;
    FT_GlyphSlot slot = face->glyph;
    int pen_x = 0;
    int pen_y = m_size;

    for (int n = 0; n < (int)m_text.Len(); n++)
    {
        FT_UInt index = FT_Get_Char_Index( face, m_text[(unsigned int)n] );

        int error = FT_Load_Glyph( face, index, FT_LOAD_DEFAULT );
        if (error) continue;

        error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );
        if (error) continue;

        FT_Bitmap *bitmap = &slot->bitmap;
        unsigned char* buffer = bitmap->buffer;
        for (int y = 0; y < bitmap->rows; y++)
            for (int x = 0; x < bitmap->width; x++)
            {
                unsigned char alpha = buffer[ y*bitmap->pitch + x ];
                if (alpha == 0) continue;

                int xx = pen_x + slot->bitmap_left + x;
                int yy = pen_y - slot->bitmap_top + y;
                m_alpha[ yy * tmparea.width + xx ] = alpha;
            }

        pen_x += slot->advance.x >> 6;
        pen_y += slot->advance.y >> 6;
    }
#endif

    wxBitmap *bitmap = m_admin->GetActive()->GetBuffer();
    wxRect sub_rect( clip_x, clip_y, clip_width, clip_height );
    wxBitmap sub_bitmap( bitmap->GetSubBitmap( sub_rect ) );

    wxImage image( sub_bitmap );

    // local coordinates
    int start_x = clip_x - tmparea.x;
    int end_x = clip_width + start_x;
    int start_y = clip_y - tmparea.y;
    int end_y = clip_height + start_y;

    for (int y = start_y; y < end_y; y++)
        for (int x = start_x; x < end_x; x++)
        {
            int alpha = m_alpha[y*tmparea.width + x];
            if (alpha)
            {
                int image_x = x - start_x;
                int image_y = y - start_y;
                if (alpha == 255)
                {
                    image.SetRGB( image_x, image_y, m_red, m_green, m_blue );
                    continue;
                }
                int red1 = (m_red * alpha) / 255;
                int green1 = (m_green * alpha) / 255;
                int blue1 = (m_blue * alpha) / 255;

                alpha = 255-alpha;
                int red2 = image.GetRed( image_x, image_y );
                int green2 = image.GetGreen( image_x, image_y );
                int blue2 = image.GetBlue( image_x, image_y );
                red2 = (red2 * alpha) / 255;
                green2 = (green2 * alpha) / 255;
                blue2 = (blue2 * alpha) / 255;

                image.SetRGB( image_x, image_y, red1+red2, green1+green2, blue1+blue2 );
            }
        }

   sub_bitmap = image.ConvertToBitmap();

   wxDC *dc = m_admin->GetActive()->GetDC();
   dc->DrawBitmap( sub_bitmap, clip_x, clip_y );
}

void wxCanvasText::WriteSVG( wxTextOutputStream &stream )
{
}

void wxCanvasText::TransLate( double x, double y )
{
    m_x += x;
    m_y += y;
    CalcBoundingBox();
}

void wxCanvasText::CalcBoundingBox()
{
    if (m_alpha) delete [] m_alpha;

    m_bbox.SetMin( m_x , m_y);
    m_bbox.SetMax( m_x + 100 , m_y + m_size + (m_size/2));


}

//----------------------------------------------------------------------------
// wxCanvas
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxCanvas,wxScrolledWindow)

BEGIN_EVENT_TABLE(wxCanvas,wxScrolledWindow)
    EVT_PAINT( wxCanvas::OnPaint )
    EVT_IDLE( wxCanvas::OnIdle )
    EVT_SIZE( wxCanvas::OnSize )
    EVT_MOUSE_EVENTS( wxCanvas::OnMouse )
    EVT_SET_FOCUS( wxCanvas::OnSetFocus )
    EVT_KILL_FOCUS( wxCanvas::OnKillFocus )
    EVT_ERASE_BACKGROUND( wxCanvas::OnEraseBackground )
END_EVENT_TABLE()

wxCanvas::wxCanvas( wxCanvasAdmin* admin, wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxScrolledWindow( parent, id, position, size, style )
{
    // These are unused in wxVectorCanvas
    m_bufferX = 0;
    m_bufferY = 0;
    
    m_admin = admin;
    m_admin->Append( this );
    
    m_needUpdate = FALSE;
    m_background = *wxWHITE;
    m_lastMouse = (wxCanvasObject*)NULL;
    m_captureMouse = (wxCanvasObject*)NULL;
    m_frozen = FALSE;
    m_oldDeviceX = 0;
    m_oldDeviceY = 0;
    m_root = (wxCanvasObjectGroup*)NULL;
}

wxCanvas::~wxCanvas()
{
    wxNode *node = m_updateRects.First();
    while (node)
    {
        wxRect *rect = (wxRect*) node->Data();
        delete rect;
        m_updateRects.DeleteNode( node );
        node = m_updateRects.First();
    }
}

double wxCanvas::GetMinX() const
{
    return 0.0;
}

double wxCanvas::GetMinY() const
{
    return 0.0;
}

double wxCanvas::GetMaxX() const
{
   int width;
   GetVirtualSize( &width, NULL );
   return width;
}

double wxCanvas::GetMaxY() const
{
   int height;
   GetVirtualSize( NULL, &height );
   return height;
}

void wxCanvas::SetColour( const wxColour& background )
{
    m_background = background;
    SetBackgroundColour( m_background );

    if (m_frozen) return;

    wxMemoryDC dc;
    dc.SelectObject( m_buffer );
    dc.SetPen( *wxTRANSPARENT_PEN );
    wxBrush brush( m_background, wxSOLID );
    dc.SetBrush( brush );
    dc.DrawRectangle( 0, 0, m_buffer.GetWidth(), m_buffer.GetHeight() );
    dc.SelectObject( wxNullBitmap );
}

void wxCanvas::SetCaptureMouse( wxCanvasObject *obj )
{
    if (obj)
    {
        wxWindow::CaptureMouse();
        m_captureMouse = obj;
    }
    else
    {
        wxWindow::ReleaseMouse();
        m_captureMouse = NULL;
    }
}

void wxCanvas::Freeze()
{
    m_frozen = TRUE;
}

void wxCanvas::Thaw()
{
    wxNode *node = m_updateRects.First();
    while (node)
    {
        wxRect *rect = (wxRect*) node->Data();
        delete rect;
        m_updateRects.DeleteNode( node );
        node = m_updateRects.First();
    }

    m_frozen = FALSE;

    if (m_buffer.Ok())
        Update( m_bufferX, m_bufferY, m_buffer.GetWidth(), m_buffer.GetHeight() );
}

void wxCanvas::Update( int x, int y, int width, int height, bool blit )
{
    CalcScrolledPosition( 0, 0, &m_oldDeviceX, &m_oldDeviceY );
    
    m_admin->SetActive(this);

    if (!m_root) return;

    if (m_frozen) return;

    // clip to buffer
    if (x < m_bufferX)
    {
        width -= m_bufferX-x;
        x = m_bufferX;
    }
    if (width <= 0) return;

    if (y < m_bufferY)
    {
        height -= m_bufferY-y;
        y = m_bufferY;
    }
    if (height <= 0) return;

    if (x+width > m_bufferX+m_buffer.GetWidth())
    {
        width = m_bufferX+m_buffer.GetWidth() - x;
    }
    if (width <= 0) return;

    if (y+height > m_bufferY+m_buffer.GetHeight())
    {
        height = m_bufferY+m_buffer.GetHeight() - y;
    }
    if (height <= 0) return;

    // update is within the buffer
    m_needUpdate = TRUE;

    // has to be blitted to screen later
    if (blit)
    {
        m_updateRects.Append(
            (wxObject*) new wxRect( x,y,width,height ) );
    }

    wxTransformMatrix cworld;

    wxMemoryDC dc;
    dc.SelectObject( m_buffer );

    dc.SetPen( *wxTRANSPARENT_PEN );
    wxBrush brush( m_background, wxSOLID );
    dc.SetBrush( brush );
    dc.SetLogicalFunction(wxCOPY);

#if 0
    if (width != m_buffer.GetWidth() && height != m_buffer.GetHeight())
    {
        dc.SetClippingRegion(x,y,width,height);
        dc.DrawRectangle(x-2,y-2,width+4,height+4);
        dc.DestroyClippingRegion();
    }
    else
    {
        dc.Clear();
        dc.DrawRectangle(0,0,m_buffer.GetWidth(),m_buffer.GetHeight());
    }
#else
    // No idea, what the code up there does.
    dc.DrawRectangle( x-m_bufferX, y-m_bufferY, width, height );
#endif    
   
    dc.SetBrush(wxNullBrush);
    dc.SetPen(wxNullPen);

    dc.SetDeviceOrigin( m_oldDeviceX, m_oldDeviceY );
    m_renderDC = &dc;

    m_root->Render( &cworld, x, y, width, height );

    m_renderDC = NULL;
    dc.SelectObject( wxNullBitmap );
}

void wxCanvas::BlitBuffer( wxDC &dc )
{
    wxNode *node = m_updateRects.First();
    while (node)
    {
        wxRect *rect = (wxRect*) node->Data();
        
        wxMemoryDC mdc;
        mdc.SelectObject( m_buffer );
        dc.Blit( rect->x,
                 rect->y,
                 rect->width,
                 rect->height,
                 &mdc,
                 rect->x - m_bufferX,
                 rect->y - m_bufferY ); 
        mdc.SelectObject( wxNullBitmap );

        delete rect;
        m_updateRects.DeleteNode( node );
        node = m_updateRects.First();
    }

    m_needUpdate = FALSE;
}

void wxCanvas::UpdateNow()
{
    if (m_frozen) return;

    if (!m_needUpdate) return;

    wxClientDC dc( this );
    PrepareDC( dc );

    BlitBuffer( dc );
}

void wxCanvas::OnSize(wxSizeEvent &event)
{
    int w,h;
    GetClientSize( &w, &h );
    m_buffer = wxBitmap( w, h );

    CalcUnscrolledPosition( 0, 0, &m_bufferX, &m_bufferY );

    wxNode *node = m_updateRects.First();
    while (node)
    {
        wxRect *rect = (wxRect*) node->Data();
        delete rect;
        m_updateRects.DeleteNode( node );
        node = m_updateRects.First();
    }

    m_frozen = FALSE;

    Update( m_bufferX, m_bufferY, m_buffer.GetWidth(), m_buffer.GetHeight(), FALSE );

    event.Skip();
}

void wxCanvas::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    PrepareDC( dc );

    if (!m_buffer.Ok()) return;

    if (m_frozen) return;

    m_needUpdate = TRUE;

    wxRegionIterator it( GetUpdateRegion() );
    while (it)
    {
        int x = it.GetX();
        int y = it.GetY();

        int w = it.GetWidth();
        int h = it.GetHeight();

        if (x+w > m_buffer.GetWidth())
            w = m_buffer.GetWidth() - x;
        if (y+h > m_buffer.GetHeight())
            h = m_buffer.GetHeight() - y;

        if ((w > 0) && (h > 0))
        {
            x += m_bufferX;
            y += m_bufferY;
            m_updateRects.Append( (wxObject*) new wxRect( x, y, w, h ) );
        }

        it++;
    }

    BlitBuffer( dc );
}

void wxCanvas::ScrollWindow( int dx, int dy, const wxRect* rect )
{
    // If any updates are pending, do them now since they will
    // expect the previous m_bufferX and m_bufferY as well as
    // the previous device origin values.
    wxClientDC dc( this );
    dc.SetDeviceOrigin( m_oldDeviceX, m_oldDeviceY );
    BlitBuffer( dc );

    // The buffer always starts at the top left corner of the
    // client area. Indeed, it is the client area.
    CalcUnscrolledPosition( 0, 0, &m_bufferX, &m_bufferY );
    
    // Update everything.
    Update( m_bufferX, m_bufferY, m_buffer.GetWidth(), m_buffer.GetHeight(), FALSE );

    // Scroll, actually.
    wxWindow::ScrollWindow( dx, dy, rect );
}

void wxCanvas::OnMouse(wxMouseEvent &event)
{
    m_admin->SetActive(this);
    if (!m_root)
    {
        event.Skip();
        return;
    }

    int x = event.GetX();
    int y = event.GetY();

    //to world coordinates to do hit test in world coordinates
    double xw = DeviceToLogicalX( x );
    double yw = DeviceToLogicalY( y );

    //make a select margin of 2 pixels, so also zero line thickness will be hit
    double margin = DeviceToLogicalXRel( 2 );

    if (event.GetEventType() == wxEVT_MOTION)
    {
        if (m_captureMouse) //no matter what go to this one
        {
            wxMouseEvent child_event( wxEVT_MOTION );
            child_event.SetEventObject(m_captureMouse);
            child_event.m_x = x;
            child_event.m_y = y;
            child_event.m_leftDown = event.m_leftDown;
            child_event.m_rightDown = event.m_rightDown;
            child_event.m_middleDown = event.m_middleDown;
            child_event.m_controlDown = event.m_controlDown;
            child_event.m_shiftDown = event.m_shiftDown;
            child_event.m_altDown = event.m_altDown;
            child_event.m_metaDown = event.m_metaDown;

            m_captureMouse->ProcessCanvasObjectEvent( child_event );
            return;
        }
        else
        {
            wxCanvasObject *obj = m_root->IsHitWorld(xw,yw,margin);

            if (obj && !obj->IsControl())
            {
                wxMouseEvent child_event( wxEVT_MOTION );
                child_event.SetEventObject( obj );
                child_event.m_x = x;
                child_event.m_y = y;
                child_event.m_leftDown = event.m_leftDown;
                child_event.m_rightDown = event.m_rightDown;
                child_event.m_middleDown = event.m_middleDown;
                child_event.m_controlDown = event.m_controlDown;
                child_event.m_shiftDown = event.m_shiftDown;
                child_event.m_altDown = event.m_altDown;
                child_event.m_metaDown = event.m_metaDown;

                if ((obj != m_lastMouse) && (m_lastMouse != NULL))
                {
                    child_event.SetEventType( wxEVT_LEAVE_WINDOW );
                    child_event.SetEventObject( m_lastMouse );
                    child_event.m_x = x;
                    child_event.m_y = y;
                    m_lastMouse->ProcessCanvasObjectEvent( child_event );

                    m_lastMouse = obj;
                    child_event.SetEventType( wxEVT_ENTER_WINDOW );
                    child_event.SetEventObject( m_lastMouse );
                    child_event.m_x = x;
                    child_event.m_y = y;
                    m_lastMouse->ProcessCanvasObjectEvent( child_event );

                    child_event.SetEventType( wxEVT_MOTION );
                    child_event.SetEventObject( obj );
                }

                obj->ProcessCanvasObjectEvent( child_event );
                return;
            }
        }
        if (m_lastMouse)
        {
            wxMouseEvent child_event( wxEVT_LEAVE_WINDOW );
            child_event.SetEventObject( m_lastMouse );
            child_event.m_x = x;
            child_event.m_y = y;
            child_event.m_leftDown = event.m_leftDown;
            child_event.m_rightDown = event.m_rightDown;
            child_event.m_middleDown = event.m_middleDown;
            child_event.m_controlDown = event.m_controlDown;
            child_event.m_shiftDown = event.m_shiftDown;
            child_event.m_altDown = event.m_altDown;
            child_event.m_metaDown = event.m_metaDown;
            m_lastMouse->ProcessCanvasObjectEvent( child_event );

            m_lastMouse = (wxCanvasObject*) NULL;
            return;
        }
    }
    else
    {
        if (m_captureMouse) //no matter what go to this one
        {
            wxMouseEvent child_event( event.GetEventType() );
            child_event.SetEventObject(m_captureMouse);
            child_event.m_x = x;
            child_event.m_y = y;
            child_event.m_leftDown = event.m_leftDown;
            child_event.m_rightDown = event.m_rightDown;
            child_event.m_middleDown = event.m_middleDown;
            child_event.m_controlDown = event.m_controlDown;
            child_event.m_shiftDown = event.m_shiftDown;
            child_event.m_altDown = event.m_altDown;
            child_event.m_metaDown = event.m_metaDown;
            m_captureMouse->ProcessCanvasObjectEvent( child_event );
        }
        else
        {
            wxCanvasObject *obj = m_root->IsHitWorld(xw,yw,margin);

            if (obj && !obj->IsControl())
            {
                wxMouseEvent child_event( event.GetEventType() );
                child_event.SetEventObject( obj );
                child_event.m_x = x;
                child_event.m_y = y;
                child_event.m_leftDown = event.m_leftDown;
                child_event.m_rightDown = event.m_rightDown;
                child_event.m_middleDown = event.m_middleDown;
                child_event.m_controlDown = event.m_controlDown;
                child_event.m_shiftDown = event.m_shiftDown;
                child_event.m_altDown = event.m_altDown;
                child_event.m_metaDown = event.m_metaDown;

                obj->ProcessCanvasObjectEvent( child_event );
                return;
            }
        }
    }

    event.Skip();
}

void wxCanvas::OnIdle(wxIdleEvent &event)
{
    m_admin->SetActive(this);
    UpdateNow();
    event.Skip();
}

void wxCanvas::OnSetFocus(wxFocusEvent &event)
{
    m_admin->SetActive(this);
}

void wxCanvas::OnKillFocus(wxFocusEvent &event)
{
}


void wxCanvas::OnEraseBackground(wxEraseEvent &event)
{
}

// coordinates conversions
// -----------------------
double wxCanvas::DeviceToLogicalX(int x) const
{
    return (double)(x);
}

double wxCanvas::DeviceToLogicalY(int y) const
{
    return (double)(y);
}

double wxCanvas::DeviceToLogicalXRel(int x) const
{
    return (double)x;
}

double wxCanvas::DeviceToLogicalYRel(int y) const
{
    return (double)y;
}

int wxCanvas::LogicalToDeviceX(double x) const
{
    return (int)(x + 0.5);
}

int wxCanvas::LogicalToDeviceY(double y) const
{
    return (int)(y + 0.5);
}

int wxCanvas::LogicalToDeviceXRel(double x) const
{
    return (int)(x + 0.5);
}

int wxCanvas::LogicalToDeviceYRel(double y) const
{
    return (int)(y + 0.5);
}

//----------------------------------------------------------------------------
// wxVectorCanvas
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxVectorCanvas,wxCanvas)

BEGIN_EVENT_TABLE(wxVectorCanvas,wxCanvas)
    EVT_SCROLLWIN( wxVectorCanvas::OnScroll )
    EVT_CHAR( wxVectorCanvas::OnChar )
    EVT_SIZE( wxVectorCanvas::OnSize )
END_EVENT_TABLE()

wxVectorCanvas::wxVectorCanvas( wxCanvasAdmin* admin, wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxCanvas( admin, parent, id, position, size, style )
{
    m_scrolled = FALSE;
    m_yaxis = FALSE;
}

double wxVectorCanvas::GetMinX() const
{
    return m_virt_minX;
}

double wxVectorCanvas::GetMinY() const
{
    return m_virt_minY;
}

double wxVectorCanvas::GetMaxX() const
{
   return m_virt_maxX;
}

double wxVectorCanvas::GetMaxY() const
{
    return m_virt_maxY;
}

void wxVectorCanvas::ScrollWindow( int dx, int dy, const wxRect* rect )
{
    // If any updates are pending, do them now since they will
    // expect the previous m_bufferX and m_bufferY as well as
    // the previous device origin values.
    wxClientDC dc( this );
    dc.SetDeviceOrigin( m_oldDeviceX, m_oldDeviceY );
    BlitBuffer( dc );

    if (dy != 0)
    {
        double dyv=DeviceToLogicalYRel(dy);
        m_virt_minY=m_virt_minY-dyv;
        m_virt_maxY=m_virt_maxY-dyv;
    }
    if (dx != 0)
    {
        double dxv=DeviceToLogicalXRel(dx);
        m_virt_minX=m_virt_minX-dxv;
        m_virt_maxX=m_virt_maxX-dxv;
    }

    m_admin->SetActive(this);
    SetMappingScroll(m_virt_minX,m_virt_minY,m_virt_maxX,m_virt_maxY,FALSE);


    if (dy != 0)
    {
        if (dy > 0 && dy < m_buffer.GetHeight())
        {
            wxRect rect( 0, 0, m_buffer.GetWidth(), m_buffer.GetHeight()-dy);
            wxBitmap sub_bitmap( m_buffer.GetSubBitmap( rect ) );
            wxMemoryDC dcm;
            dcm.SelectObject( m_buffer );
            dcm.DrawBitmap( sub_bitmap, 0, dy, TRUE );
            dcm.SelectObject( wxNullBitmap );

            Update( 0, 0, m_buffer.GetWidth(), dy, TRUE );
        }
        else  if (dy < 0 && dy > -m_buffer.GetHeight())
        {
            wxRect rect( 0, -dy, m_buffer.GetWidth(), m_buffer.GetHeight()+dy);
            wxBitmap sub_bitmap( m_buffer.GetSubBitmap( rect ) );
            wxMemoryDC dcm;
            dcm.SelectObject( m_buffer );
            dcm.DrawBitmap( sub_bitmap, 0, 0, TRUE );
            dcm.SelectObject( wxNullBitmap );

            Update( 0, m_buffer.GetHeight()+dy, m_buffer.GetWidth(), m_buffer.GetHeight(), TRUE );
        }
        else
            Update( 0, 0, m_buffer.GetWidth(), m_buffer.GetHeight(), TRUE );
    }

    if (dx != 0)
    {
        if (dx > 0 && dx < m_buffer.GetWidth())
        {
            wxRect rect( 0, 0, m_buffer.GetWidth()-dx, m_buffer.GetHeight());
            wxBitmap sub_bitmap( m_buffer.GetSubBitmap( rect ) );
            wxMemoryDC dcm;
            dcm.SelectObject( m_buffer );
            dcm.DrawBitmap( sub_bitmap, dx, 0, TRUE );
            dcm.SelectObject( wxNullBitmap );

            Update( 0, 0, dx, m_buffer.GetHeight(), TRUE );
        }
        else if (dx < 0 && dx > -m_buffer.GetWidth())
        {
            wxRect rect( -dx, 0, m_buffer.GetWidth()+dx, m_buffer.GetHeight());
            wxBitmap sub_bitmap( m_buffer.GetSubBitmap( rect ) );
            wxMemoryDC dcm;
            dcm.SelectObject( m_buffer );
            dcm.DrawBitmap( sub_bitmap, 0, 0, TRUE );
            dcm.SelectObject( wxNullBitmap );

            Update( m_buffer.GetWidth()+dx, 0, m_buffer.GetWidth(), m_buffer.GetHeight(), TRUE );
        }
        else
            Update( 0,0, m_buffer.GetWidth(), m_buffer.GetHeight(), TRUE );
    }
    
    wxWindow::ScrollWindow( dx, dy, rect );

    //must be done now because quick repeated scrolling will prevent wxPaint
    //from doing it properly
    UpdateNow();
}

void wxVectorCanvas::OnSize(wxSizeEvent &event)
{
    int w,h;

    GetClientSize( &w, &h );

    wxMemoryDC dc;
    m_buffer = wxBitmap( w, h );
    dc.SelectObject( m_buffer );
    dc.SetPen( *wxTRANSPARENT_PEN );
    wxBrush brush( m_background , wxSOLID );
    dc.SetBrush( brush );
    dc.DrawRectangle( 0, 0, m_buffer.GetWidth(), m_buffer.GetHeight() );
    dc.SelectObject( wxNullBitmap );

    wxNode *node = m_updateRects.First();
    while (node)
    {
        wxRect *rect = (wxRect*) node->Data();
        delete rect;
        m_updateRects.DeleteNode( node );
        node = m_updateRects.First();
    }

    m_frozen = FALSE;

    m_admin->SetActive(this);
    SetMappingScroll(m_virt_minX,m_virt_minY,m_virt_maxX,m_virt_maxY,FALSE);

    Update( 0,0, m_buffer.GetWidth(), m_buffer.GetHeight(), FALSE );

//    event.Skip();
}

// maps the virtual window (Real drawing to the window coordinates
// also used for zooming
void wxVectorCanvas::SetMappingScroll( double vx1,    double vy1, double vx2, double vy2, bool border)
{
    int dwxi,dwyi;
    GetClientSize(&dwxi,&dwyi);

    if (vx2==vx1) vx2=vx1+100000;
    if (vy2==vy1) vy2=vy1+100000;
    m_virt_minX=vx1;
    m_virt_minY=vy1;
    m_virt_maxX=vx2;
    m_virt_maxY=vy2;

    double dwx=dwxi;
    double dwy=dwyi;
    if (dwx==0) dwx=1;
    if (dwy==0) dwy=1;

    double dvx = m_virt_maxX - m_virt_minX;
    double dvy = m_virt_maxY - m_virt_minY;

    // calculate the scaling factor for the virtual window
    double temp_x=0;
    double temp_y=0;
    if ((dvy / dvx) < (dwy / dwx))
    {
        dvy = dvx * (dwy / dwx);
        // calculate the change in the coordinates
        temp_y = (dvy - (m_virt_maxY - m_virt_minY) )/ 2.0;
    }
    else
    {
        dvx = dvy * (dwx / dwy);
        // calculate the change in the coordinates
        temp_x = (dvx - (m_virt_maxX - m_virt_minX) )/ 2.0;
    }

    // add or substract the change from the original coordinates
    m_virt_minX=m_virt_minX-temp_x;
    m_virt_minY=m_virt_minY-temp_y;

    m_virt_maxX=m_virt_maxX+temp_x;
    m_virt_maxY=m_virt_maxY+temp_y;

    // initialize the mapping_matrix used for mapping the
    // virtual windows to the drawing window

     // make mappingmatrix
    m_mapping_matrix.Identity();
    if (!border)
    {
        // translate the drawing to 0,0
        if (m_yaxis)
            m_mapping_matrix.Translate(-m_virt_minX,-m_virt_maxY);
        else
            m_mapping_matrix.Translate(-m_virt_minX,-m_virt_minY);
    }
    else
    {
        // make a small white border around the drawing
        m_virt_minX=m_virt_minX- 0.05 * dvx;
        m_virt_minY=m_virt_minY- 0.05 * dvy;

        m_virt_maxX=m_virt_maxX+ 0.05 * dvx;
        m_virt_maxY=m_virt_maxY+ 0.05 * dvy;

        // translate the drawing to 0,0
        if (m_yaxis)
            m_mapping_matrix.Translate(-m_virt_minX,-m_virt_maxY);
        else
            m_mapping_matrix.Translate(-m_virt_minX,-m_virt_minY);
    }

    double scalefactor_x = dwx;
    scalefactor_x /= (m_virt_maxX - m_virt_minX);

    double scalefactor_y = dwy;
    scalefactor_y /= (m_virt_maxY - m_virt_minY);

    // scale the drawing so it fit's in the window
    m_mapping_matrix.Scale(scalefactor_x, scalefactor_y, 0, 0);

    // because of coordinate change mirror over X
    // 0,0 in graphic computerscreens: upperleft corner
    // 0,0 in cartesian: lowerleft corner
    if (m_yaxis)
    {
        m_mapping_matrix.Mirror();
    }
    // make inverse of mapping matrix
    // this is to set coordinates in the statusbar
    // and the calculate screencoordinates to world coordinates used
    // in zooming
    m_inverse_mapping=m_mapping_matrix;
    m_inverse_mapping.Invert();

    if (m_scrolled)
        SetScroll(m_virtm_minX,m_virtm_minY,m_virtm_maxX,m_virtm_maxY);

    int dx2,dy2;
    GetClientSize(&dx2,&dy2);
    if ( dwxi != dx2 || dwyi  != dy2) //scrollbar is/became empty
        SetScroll(m_virtm_minX,m_virtm_minY,m_virtm_maxX,m_virtm_maxY);
}


void wxVectorCanvas::SetScroll(double vx1,double vy1,double vx2,double vy2)
{
    m_virtm_minX=vx1;
    m_virtm_minY=vy1;
    m_virtm_maxX=vx2;
    m_virtm_maxY=vy2;

    double dvx = m_virt_maxX - m_virt_minX;
    double dvy = m_virt_maxY - m_virt_minY;
    double dmvx = m_virtm_maxX - m_virtm_minX;
    double dmvy = m_virtm_maxY - m_virtm_minY;

    SetScrollbar(wxHORIZONTAL,(m_virt_minX-m_virtm_minX)/dmvx *1000,dvx/dmvx *1000,1000,FALSE);
    if (m_yaxis)
    {
        SetScrollbar(wxVERTICAL,(m_virtm_maxY-m_virt_maxY)/dmvy *1000,dvy/dmvy *1000,1000,FALSE);
    }
    else
    {
        SetScrollbar(wxVERTICAL,(m_virt_minY-m_virtm_minY)/dmvy *1000,dvy/dmvy *1000,1000,FALSE);
    }

    m_scrolled=TRUE;
}

// coordinates conversions
// -----------------------
double wxVectorCanvas::DeviceToLogicalX(int x) const
{
    return m_inverse_mapping.GetValue(0,0) * x + m_inverse_mapping.GetValue(2,0);
}

double wxVectorCanvas::DeviceToLogicalY(int y) const
{
    return m_inverse_mapping.GetValue(1,1) * y + m_inverse_mapping.GetValue(2,1);
}

double wxVectorCanvas::DeviceToLogicalXRel(int x) const
{
    return x*m_inverse_mapping.GetValue(0,0);
}

double wxVectorCanvas::DeviceToLogicalYRel(int y) const
{
    return y*m_inverse_mapping.GetValue(1,1);
}

int wxVectorCanvas::LogicalToDeviceX(double x) const
{
    return (int) (m_mapping_matrix.GetValue(0,0) * x + m_mapping_matrix.GetValue(2,0) + 0.5);
}

int wxVectorCanvas::LogicalToDeviceY(double y) const
{
    return (int) (m_mapping_matrix.GetValue(1,1) * y + m_mapping_matrix.GetValue(2,1) + 0.5);
}

int wxVectorCanvas::LogicalToDeviceXRel(double x) const
{
    return (int) (x*m_mapping_matrix.GetValue(0,0) + 0.5);
}

int wxVectorCanvas::LogicalToDeviceYRel(double y) const
{
    return (int) (y*m_mapping_matrix.GetValue(1,1) + 0.5);
}


// return the inverse mapping matrix for zooming or coordinates
wxTransformMatrix wxVectorCanvas::GetInverseMappingMatrix()
{
    return m_inverse_mapping;
}

wxTransformMatrix wxVectorCanvas::GetMappingMatrix()
{
    return m_mapping_matrix;
}


// ----------------------------------------------------------------------------
// scrolling behaviour
// ----------------------------------------------------------------------------

void wxVectorCanvas::OnScroll(wxScrollWinEvent& event)
{
    if (event.GetEventType()==wxEVT_SCROLLWIN_THUMBRELEASE)
    {
        if (event.GetOrientation()==wxHORIZONTAL)
        {
          double x=m_virtm_minX+event.GetPosition()/1000.0*(m_virtm_maxX-m_virtm_minX);
          x=LogicalToDeviceXRel(x-m_virt_minX);
          ScrollWindow(-x, 0, (const wxRect *) NULL);
        }
        else
        {
          double y=m_virtm_minY+event.GetPosition()/1000.0*(m_virtm_maxY-m_virtm_minY);
          y=LogicalToDeviceYRel(y-m_virt_minY);
          ScrollWindow(0, -y, (const wxRect *) NULL);
        }
    }
    else if (event.GetEventType()==wxEVT_SCROLLWIN_PAGEUP)
    {
        if (event.GetOrientation()==wxHORIZONTAL)
        {
          double x=GetBufferWidth();
          ScrollWindow(x, 0, (const wxRect *) NULL);
        }
        else
        {
          double y=GetBufferHeight();
          ScrollWindow(0, y, (const wxRect *) NULL);
        }
    }
    else if (event.GetEventType()==wxEVT_SCROLLWIN_PAGEDOWN)
    {
        if (event.GetOrientation()==wxHORIZONTAL)
        {
          double x=-GetBufferWidth();
          ScrollWindow(x, 0, (const wxRect *) NULL);
        }
        else
        {
          double y=-GetBufferHeight();
          ScrollWindow(0, y, (const wxRect *) NULL);
        }
    }
    else if (event.GetEventType()==wxEVT_SCROLLWIN_LINEUP)
    {
        if (event.GetOrientation()==wxHORIZONTAL)
        {
          int x=GetBufferWidth()/10;
          ScrollWindow(x, 0, (const wxRect *) NULL);
        }
        else
        {
          int y=GetBufferHeight()/10;
          ScrollWindow(0, y, (const wxRect *) NULL);
        }
    }
    else if (event.GetEventType()==wxEVT_SCROLLWIN_LINEDOWN)
    {
        if (event.GetOrientation()==wxHORIZONTAL)
        {
          int x=-GetBufferWidth()/10;
          ScrollWindow(x, 0, (const wxRect *) NULL);
        }
        else
        {
          int y=-GetBufferHeight()/10;
          ScrollWindow(0, y, (const wxRect *) NULL);
        }
    }

}

void wxVectorCanvas::OnChar(wxKeyEvent& event)
{
    switch ( event.KeyCode() )
    {
        case WXK_PAGEUP:
        case WXK_PRIOR:
            {
              double y=GetBufferHeight();
              ScrollWindow(0, y, (const wxRect *) NULL);
            }
            break;
        case WXK_PAGEDOWN:
        case WXK_NEXT:
            {
              double y=-GetBufferHeight();
              ScrollWindow(0, y, (const wxRect *) NULL);
            }
            break;
        case WXK_HOME:
            {
                double y=m_virtm_minY;
                y=LogicalToDeviceYRel(y-m_virt_minY);
                ScrollWindow(0, -y, (const wxRect *) NULL);
            }
            break;
        case WXK_END:
            {
                double y=m_virtm_minY+(m_virtm_maxY-m_virtm_minY);
                y=LogicalToDeviceYRel(y-m_virt_minY);
                ScrollWindow(0, -y, (const wxRect *) NULL);
            }
            break;
        case WXK_UP:
            {
                int y;
                if  (!event.ControlDown())
                    y=GetBufferHeight()/10;
                else
                    y=GetBufferHeight();
                ScrollWindow(0, y, (const wxRect *) NULL);
            }
            break;

        case WXK_DOWN:
            {
                int y;
                if  (!event.ControlDown())
                    y=-GetBufferHeight()/10;
                else
                    y=-GetBufferHeight();
                ScrollWindow(0, y, (const wxRect *) NULL);
            }
            break;

        case WXK_LEFT:
            {
                int x;
                if  (!event.ControlDown())
                    x=GetBufferWidth()/10;
                else
                    x=GetBufferWidth();
                ScrollWindow(x, 0, (const wxRect *) NULL);
            }
            break;
        case WXK_RIGHT:
            {
                int x;
                if  (!event.ControlDown())
                    x=-GetBufferWidth()/10;
                else
                    x=-GetBufferWidth();
                ScrollWindow(x, 0, (const wxRect *) NULL);
            }
            break;
        default:
            // not for us
            event.Skip();
    }
}


//----------------------------------------------------------------------------
// wxCanvasAdmin
//----------------------------------------------------------------------------

wxCanvasAdmin::wxCanvasAdmin()
{

}

wxCanvasAdmin::~wxCanvasAdmin()
{
}


void wxCanvasAdmin::Append( wxCanvas* canvas )
{
    m_canvaslist.Append( canvas );
}

void wxCanvasAdmin::Remove( wxCanvas* canvas )
{
    m_canvaslist.DeleteObject( canvas );
}

void wxCanvasAdmin::Update(wxCanvasObject* obj, double x, double y, double width, double height)
{
    wxNode *node = m_canvaslist.First();
    while (node)
    {

        wxCanvas *canvas = (wxCanvas*) node->Data();

        if (m_active == canvas)
        {
            int xi = canvas->LogicalToDeviceX( x);
            int yi = canvas->LogicalToDeviceY( y);
            int wi = canvas->LogicalToDeviceXRel( width );
            int hi = canvas->LogicalToDeviceYRel( height);
            //update a little more then is strictly needed,
            //to get rid of the 1 bit bugs
            if (canvas->GetYaxis())
                canvas->Update( xi-2, yi+hi-2, wi+4, -hi+4);
            else
                canvas->Update( xi-2, yi-2, wi+4, hi+4);
        }
        else
        {   wxCanvasObject* topobj=canvas->GetRoot()->Contains(obj);
            if (topobj)
            {
                wxCanvas* tcanvas = m_active;
                SetActive(canvas);

                /*
                //KKK TODO somehow the next does not work for update i do not know why
                canvas->GetRoot()->CalcBoundingBox();
                int xi = topobj->GetX();
                int yi = topobj->GetY();
                int wi = topobj->GetWidth();
                int hi = topobj->GetHeight();
                */
                canvas->Update( 0,0, canvas->GetBufferWidth(),canvas->GetBufferHeight());
                SetActive(tcanvas);
            }
        }

        node = node->Next();
    }
}

void wxCanvasAdmin::UpdateNow()
{
    wxNode *node = m_canvaslist.First();
    while (node)
    {
        wxCanvas *canvas = (wxCanvas*) node->Data();

        canvas->UpdateNow();
        node = node->Next();
    }
}

// coordinates conversions
// -----------------------
double wxCanvasAdmin::DeviceToLogicalX(int x) const
{
    return m_active->DeviceToLogicalX(x);
}

double wxCanvasAdmin::DeviceToLogicalY(int y) const
{
    return m_active->DeviceToLogicalY(y);
}

double wxCanvasAdmin::DeviceToLogicalXRel(int x) const
{
    return m_active->DeviceToLogicalXRel(x);
}

double wxCanvasAdmin::DeviceToLogicalYRel(int y) const
{
    return m_active->DeviceToLogicalYRel(y);
}

int wxCanvasAdmin::LogicalToDeviceX(double x) const
{
    return m_active->LogicalToDeviceX(x);
}

int wxCanvasAdmin::LogicalToDeviceY(double y) const
{
    return m_active->LogicalToDeviceY(y);
}

int wxCanvasAdmin::LogicalToDeviceXRel(double x) const
{
    return m_active->LogicalToDeviceXRel(x);
}

int wxCanvasAdmin::LogicalToDeviceYRel(double y) const
{
    return m_active->LogicalToDeviceYRel(y);
}

void wxCanvasAdmin::SetActive(wxCanvas* activate)
{
    wxNode *node = m_canvaslist.First();
    while (node)
    {
        wxCanvas *canvas = (wxCanvas*) node->Data();

        if (activate == canvas)
        {
            m_active=canvas;
            break;
        }
        node = node->Next();
    }
}
//--------------------------------------------------------------------
// wxCanvasModule
//--------------------------------------------------------------------

class wxCanvasModule : public wxModule
{
public:
    virtual bool OnInit();
    virtual void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxCanvasModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxCanvasModule, wxModule)

bool wxCanvasModule::OnInit()
{
#if wxUSE_FREETYPE
    int error = FT_Init_FreeType( &g_freetypeLibrary );
    if (error) return FALSE;
#endif

    return TRUE;
}

void wxCanvasModule::OnExit()
{
#if wxUSE_FREETYPE
    FT_Done_FreeType( g_freetypeLibrary );
#endif
}




