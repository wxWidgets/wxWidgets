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

void wxDCBase::DoDrawCheckMark(wxCoord x1, wxCoord y1,
                               wxCoord width, wxCoord height)
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord x2 = x1 + width,
            y2 = y1 + height;

    // this is to yield width of 3 for width == height == 10
    SetPen(wxPen(GetTextForeground(), (width + height + 1) / 7, wxSOLID));

    // we're drawing a scaled version of wx/generic/tick.xpm here
    wxCoord x3 = x1 + (4*width) / 10,   // x of the tick bottom
            y3 = y1 + height / 2;       // y of the left tick branch
    DoDrawLine(x1, y3, x3, y2);
    DoDrawLine(x3, y2, x2, y1);

    CalcBoundingBox(x1, y1);
    CalcBoundingBox(x2, y2);
}

void wxDCBase::DrawLines(const wxList *list, wxCoord xoffset, wxCoord yoffset)
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
                           wxCoord xoffset, wxCoord yoffset,
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
void wxDCBase::DrawSpline(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord x3, wxCoord y3)
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
