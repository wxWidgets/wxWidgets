/////////////////////////////////////////////////////////////////////////////
// Name:        dc.cpp
// Purpose:     wxDC Class
// Author:      Brian Macy
// Modified by:
// Created:     05/25/99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
   #include "wx/window.h"
#endif

#ifdef __WXMSW__
   #include "wx/msw/private.h"
#endif

#include "wx/dc.h"

void wxDCBase::DrawLines(const wxList *list, long xoffset, long yoffset)
{
    int n = list->Number();
    wxPoint *points = new wxPoint[n];

    int i = 0;
    for ( wxNode *node = list->First(); node; node = node->Next(), i++ )
    {
        wxPoint *point = (wxPoint *)node->Data();
        points[i].x = point->x;
        points[i].y = point->y;
    }

    DoDrawLines(n, points, xoffset, yoffset);

    delete [] points;
}


void wxDCBase::DrawPolygon(const wxList *list,
                           long xoffset, long yoffset,
                           int fillStyle)
{
    int n = list->Number();
    wxPoint *points = new wxPoint[n];

    int i = 0;
    for ( wxNode *node = list->First(); node; node = node->Next(), i++ )
    {
        wxPoint *point = (wxPoint *)node->Data();
        points[i].x = point->x;
        points[i].y = point->y;
    }

    DoDrawPolygon(n, points, xoffset, yoffset, fillStyle);

    delete [] points;
}


#if wxUSE_SPLINES

// TODO: this API needs fixing (wxPointList, why (!const) "wxList *"?)
void wxDCBase::DrawSpline(long x1, long y1, long x2, long y2, long x3, long y3)
{
    wxList point_list;

    wxPoint *point1 = new wxPoint;
    point1->x = x1; point1->y = y1;
    point_list.Append((wxObject*)point1);

    wxPoint *point2 = new wxPoint;
    point2->x = x2; point2->y = y2;
    point_list.Append((wxObject*)point2);

    wxPoint *point3 = new wxPoint;
    point3->x = x3; point3->y = y3;
    point_list.Append((wxObject*)point3);

    DrawSpline(&point_list);

    for( wxNode *node = point_list.First(); node; node = node->Next() )
    {
        wxPoint *p = (wxPoint *)node->Data();
        delete p;
    }
}

void wxDCBase::DrawSpline(int n, wxPoint points[])
{
    wxList list;
    for (int i =0; i < n; i++)
    {
        list.Append((wxObject*)&points[i]);
    }

    DrawSpline(&list);
}

#endif // wxUSE_SPLINES
