/*
 * File:	xfspline.cc
 * Purpose:	
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id$
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

// Must be a comment as this file is #include\'d by wb_dc.cc
/* static const char sccsid[] = "@(#)xfspline.cc	1.3 5/9/94"; */

/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 */


#if wxUSE_SPLINES

////#define     wx_round(a)    (int)((a)+.5)
//#define     wx_round(a)    (a)

class wxSpline: public wxObject
{
 public:
  int type;
  wxList *points;

  wxSpline(wxList *list);
  void DeletePoints(void);

  // Doesn't delete points
  ~wxSpline(void);
};

void wx_draw_open_spline(wxDC *dc, wxSpline *spline);

#if wxUSE_POSTSCRIPT
void wx_draw_open_spline_ps(wxPostScriptDC *dc, wxSpline *s);
#endif

void wx_quadratic_spline(double a1, double b1, double a2, double b2,
                         double a3, double b3, double a4, double b4);
void wx_clear_stack(void);
int wx_spline_pop(double *x1, double *y1, double *x2, double *y2, double *x3,
        double *y3, double *x4, double *y4);
void wx_spline_push(double x1, double y1, double x2, double y2, double x3, double y3,
          double x4, double y4);
static bool wx_spline_add_point(double x, double y);
static void wx_spline_draw_point_array(wxDC *dc);
wxSpline *wx_make_spline(int x1, int y1, int x2, int y2, int x3, int y3);

void wxDC::DrawSpline(int n, wxPoint points[])
{
  wxList list;
  int i;
  for (i =0; i < n; i++)
    list.Append((wxObject*)&points[i]);
  DrawSpline((wxList *)&list);
}

void wxDC::DrawSpline(wxList *list)
{
  wxSpline spline(list);

  wx_draw_open_spline(this, &spline);
}


wxList wx_spline_point_list;

void wx_draw_open_spline(wxDC *dc, wxSpline *spline)
{
    wxPoint *p;
    double           cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    double           x1, y1, x2, y2;

    wxNode *node = spline->points->First();
    p = (wxPoint *)node->Data();

    x1 = p->x;
    y1 = p->y;

    node = node->Next();
    p = (wxPoint *)node->Data();

    x2 = p->x;
    y2 = p->y;
    cx1 = (double)((x1 + x2) / 2);
    cy1 = (double)((y1 + y2) / 2);
    cx2 = (double)((cx1 + x2) / 2);
    cy2 = (double)((cy1 + y2) / 2);

    wx_spline_add_point(x1, y1);

    while ((node = node->Next()) != NULL)
    {
        p = (wxPoint *)node->Data();
	x1 = x2;
	y1 = y2;
	x2 = p->x;
	y2 = p->y;
        cx4 = (double)(x1 + x2) / 2;
        cy4 = (double)(y1 + y2) / 2;
        cx3 = (double)(x1 + cx4) / 2;
        cy3 = (double)(y1 + cy4) / 2;

        wx_quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4);

	cx1 = cx4;
	cy1 = cy4;
        cx2 = (double)(cx1 + x2) / 2;
        cy2 = (double)(cy1 + y2) / 2;
    }

    wx_spline_add_point((double)wx_round(cx1), (double)wx_round(cy1));
    wx_spline_add_point(x2, y2);

    wx_spline_draw_point_array(dc);

}

#if wxUSE_POSTSCRIPT
void wx_draw_open_spline_ps(wxPostScriptDC *dc, wxSpline *s)
{
	double		a, b, c, d, x1, y1, x2, y2, x3, y3;
        wxPoint *p, *q;
        wxNode *node = s->points->First();
        p = (wxPoint *)node->Data();

	x1 = p->x; y1 = p->y;

        node = node->Next();
        p = (wxPoint *)node->Data();
	c = p->x; d = p->y;
        x3 = a = (double)(x1 + c) / 2;
        y3 = b = (double)(y1 + d) / 2;

        *(dc->GetStream()) << "newpath " << x1 << " " << dc->GetYOrigin() - y1 << " moveto " << x3 << " " << dc->GetYOrigin() - y3;
        *(dc->GetStream()) << " lineto\n";
        dc->CalcBoundingBox((double)x1, (double)(dc->GetYOrigin() - y1));
        dc->CalcBoundingBox((double)x3, (double)(dc->GetYOrigin() - y3));

        while ((node = node->Next()) != NULL)
	{
          q = (wxPoint *)node->Data();

	  x1 = x3; y1 = y3;
	  x2 = c;  y2 = d;
	  c = q->x; d = q->y;
          x3 = (double)(x2 + c) / 2;
          y3 = (double)(y2 + d) / 2;
          *(dc->GetStream()) << x1 << " " << dc->GetYOrigin() - y1 << " " << x2 << " " << dc->GetYOrigin() - y2 << " ";
          *(dc->GetStream()) << x3 << " " << dc->GetYOrigin() - y3 << " DrawSplineSection\n";

          dc->CalcBoundingBox((double)x1, (double)(dc->GetYOrigin() - y1));
          dc->CalcBoundingBox((double)x3, (double)(dc->GetYOrigin() - y3));
        }
	/*
	* At this point, (x2,y2) and (c,d) are the position of the 
	* next-to-last and last point respectively, in the point list
	*/
        *(dc->GetStream()) << c << " " << dc->GetYOrigin() - d << " lineto stroke\n";
}
#endif

/********************* CURVES FOR SPLINES *****************************

	The following spline drawing routine is from

	"An Algorithm for High-Speed Curve Generation"
	by George Merrill Chaikin,
	Computer Graphics and Image Processing, 3, Academic Press,
	1974, 346-349.

	and

	"On Chaikin's Algorithm" by R. F. Riesenfeld,
	Computer Graphics and Image Processing, 4, Academic Press,
	1975, 304-310.

***********************************************************************/

#define		half(z1, z2)	((z1+z2)/2.0)
#define		THRESHOLD	5

/* iterative version */

void wx_quadratic_spline(double a1, double b1, double a2, double b2, double a3, double b3, double a4,
                 double b4)
{
    register double  xmid, ymid;
    double           x1, y1, x2, y2, x3, y3, x4, y4;

    wx_clear_stack();
    wx_spline_push(a1, b1, a2, b2, a3, b3, a4, b4);

    while (wx_spline_pop(&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4)) {
        xmid = (double)half(x2, x3);
        ymid = (double)half(y2, y3);
	if (fabs(x1 - xmid) < THRESHOLD && fabs(y1 - ymid) < THRESHOLD &&
	    fabs(xmid - x4) < THRESHOLD && fabs(ymid - y4) < THRESHOLD) {
            wx_spline_add_point((double)wx_round(x1), (double)wx_round(y1));
            wx_spline_add_point((double)wx_round(xmid), (double)wx_round(ymid));
	} else {
            wx_spline_push(xmid, ymid, (double)half(xmid, x3), (double)half(ymid, y3),
                 (double)half(x3, x4), (double)half(y3, y4), x4, y4);
            wx_spline_push(x1, y1, (double)half(x1, x2), (double)half(y1, y2),
                 (double)half(x2, xmid), (double)half(y2, ymid), xmid, ymid);
	}
    }
}


/* utilities used by spline drawing routines */


typedef struct wx_spline_stack_struct {
    double           x1, y1, x2, y2, x3, y3, x4, y4;
}
                Stack;

#define         SPLINE_STACK_DEPTH             20
static Stack    wx_spline_stack[SPLINE_STACK_DEPTH];
static Stack   *wx_stack_top;
static int      wx_stack_count;

void wx_clear_stack(void)
{
    wx_stack_top = wx_spline_stack;
    wx_stack_count = 0;
}

void wx_spline_push(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    wx_stack_top->x1 = x1;
    wx_stack_top->y1 = y1;
    wx_stack_top->x2 = x2;
    wx_stack_top->y2 = y2;
    wx_stack_top->x3 = x3;
    wx_stack_top->y3 = y3;
    wx_stack_top->x4 = x4;
    wx_stack_top->y4 = y4;
    wx_stack_top++;
    wx_stack_count++;
}

int wx_spline_pop(double *x1, double *y1, double *x2, double *y2,
                  double *x3, double *y3, double *x4, double *y4)
{
    if (wx_stack_count == 0)
	return (0);
    wx_stack_top--;
    wx_stack_count--;
    *x1 = wx_stack_top->x1;
    *y1 = wx_stack_top->y1;
    *x2 = wx_stack_top->x2;
    *y2 = wx_stack_top->y2;
    *x3 = wx_stack_top->x3;
    *y3 = wx_stack_top->y3;
    *x4 = wx_stack_top->x4;
    *y4 = wx_stack_top->y4;
    return (1);
}

static bool wx_spline_add_point(double x, double y)
{
  wxPoint *point = new wxPoint ;
  point->x = (int) x;
  point->y = (int) y;
  wx_spline_point_list.Append((wxObject*)point);
  return TRUE;
}

static void wx_spline_draw_point_array(wxDC *dc)
{
  dc->DrawLines(&wx_spline_point_list, (double)0.0, (double)0.0);
  wxNode *node = wx_spline_point_list.First();
  while (node)
  {
    wxPoint *point = (wxPoint *)node->Data();
    delete point;
    delete node;
    node = wx_spline_point_list.First();
  }
}

wxSpline::wxSpline(wxList *list)
{
  points = list;
}

wxSpline::~wxSpline(void)
{
}

void wxSpline::DeletePoints(void)
{
  for(wxNode *node = points->First(); node; node = points->First())
  {
    wxPoint *point = (wxPoint *)node->Data();
    delete point;
    delete node;
  }
  delete points;
}

#if wxUSE_POSTSCRIPT

// Make a 3-point spline
void wxPostScriptDC::DrawSpline(long x1, long y1, long x2, long y2, long x3, long y3)
{
  wxList *point_list = new wxList;

  wxPoint *point1 = new wxPoint;
  point1->x = x1; point1->y = y1;
  point_list->Append((wxObject*)point1);

  wxPoint *point2 = new wxPoint;
  point2->x = x2; point2->y = y2;
  point_list->Append((wxObject*)point2);

  wxPoint *point3 = new wxPoint;
  point3->x = x3; point3->y = y3;
  point_list->Append((wxObject*)point3);

  wxSpline spline(point_list);

  wx_draw_open_spline_ps(this, &spline);
  spline.DeletePoints();
}

void wxPostScriptDC::DrawSpline(wxList *list)
{
  wxSpline spline(list);

  wx_draw_open_spline_ps(this, &spline);
}

void wxPostScriptDC::DrawSpline(int n, wxPoint points[])
{
  wxList list;
  int i;
  for (i =0; i < n; i++)
    list.Append((wxObject*)&points[i]);
  DrawSpline((wxList *)&list);
}


#endif // wxUSE_POSTSCRIPT

#endif // wxUSE_SPLINES

