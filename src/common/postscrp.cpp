/////////////////////////////////////////////////////////////////////////////
// Name:        postscrp.cpp
// Purpose:     wxPostScriptDC implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "postscrp.h"
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if USE_POSTSCRIPT

#ifndef WX_PRECOMP
#include "wx/intl.h"
#include "wx/frame.h"
#include "wx/postscrp.h"
#include "wx/utils.h"
#include "wx/filedlg.h"
#include "wx/msgdlg.h"
#include "wx/app.h"
#include "wx/button.h"
#include "wx/radiobox.h"
#include "wx/textctrl.h"
#include "wx/stattext.h"
#include "wx/icon.h"
#include "wx/list.h"
#endif

#include "wx/dcmemory.h"

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#include <fstream.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#ifdef __WXGTK__

#include "gdk/gdkx.h"        // GDK_DISPLAY
#include "gdk/gdkprivate.h"  // XImage
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#endif

#ifdef __WXMSW__

#ifdef DrawText
#undef DrawText
#endif

#ifdef StartDoc
#undef StartDoc
#endif

#ifdef GetCharWidth
#undef GetCharWidth
#endif

#ifdef FindWindow
#undef FindWindow
#endif

#endif

// Declarations local to this file
#define YSCALE(y) (m_yOrigin / m_scaleFactor - (y))

// Determine the Default Postscript Previewer
// available on the platform
#if defined(__SUN__) && defined(__XVIEW__)
// OpenWindow/NeWS's Postscript Previewer
# define PS_VIEWER_PROG "pageview"
#elif defined(__VMS__)
#define PS_VIEWER_PROG "view/format=ps/select=x_display"
#elif defined(__SGI__)
// SGI's Display Postscript Previewer
//# define PS_VIEWER_PROG "dps"
# define PS_VIEWER_PROG "xpsview"
#elif defined(__X__) || defined(__WXGTK__)
// Front-end to ghostscript
# define PS_VIEWER_PROG "ghostview"
#else
// Windows ghostscript/ghostview
# define PS_VIEWER_PROG NULL
#endif

wxPrintSetupData *wxThePrintSetupData = (wxPrintSetupData *) NULL;

// these should move into wxPostscriptDC:
double UnderlinePosition = 0.0F;
double UnderlineThickness = 0.0F;

#define _MAXPATHLEN 500

/* See "wxspline.inc" and "xfspline.inc" */
#if USE_XFIG_SPLINE_CODE
static const char *wxPostScriptHeaderSpline = " \
/DrawSplineSection {\n\
	/y3 exch def\n\
	/x3 exch def\n\
	/y2 exch def\n\
	/x2 exch def\n\
	/y1 exch def\n\
	/x1 exch def\n\
	/xa x1 x2 x1 sub 0.666667 mul add def\n\
	/ya y1 y2 y1 sub 0.666667 mul add def\n\
	/xb x3 x2 x3 sub 0.666667 mul add def\n\
	/yb y3 y2 y3 sub 0.666667 mul add def\n\
	x1 y1 lineto\n\
	xa ya xb yb x3 y3 curveto\n\
	} def\n\
";
#else
// No extra PS header for this spline implementation.
static const char *wxPostScriptHeaderSpline = (char *) NULL;

#endif /* USE_XFIG_SPLINE_CODE */

// steve, 05.09.94
// VMS has a bug in the ofstream class.
// the buffering doesn't work correctly. therefore
// we will allocate (temporarily) a very big buffer (1MB), so
// that a buffer overflow will not occur.
#ifdef __VMS__
#define VMS_BUFSIZ (1024L*1024L)
static char *fileBuffer = NULL;
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxPostScriptDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxPrintSetupData, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPrintPaperType, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPrintPaperDatabase, wxList)
#endif

wxPostScriptDC::wxPostScriptDC (void)
{
//  m_yOrigin = 792;		            // For EPS output
  m_yOrigin = 842;		            // For A4 output

  m_minX = 1000;
  m_minY = 1000;
  m_maxX = -1000;
  m_maxY = -1000;
  m_title = "";

  m_pstream = (ofstream *) NULL;

#ifdef __WXMSW__
  // Can only send to file in Windows
  wxThePrintSetupData->SetPrinterMode(PS_FILE);
#endif

  m_currentRed = 255;
  m_currentGreen = 255;
  m_currentBlue = 0;
}

wxPostScriptDC::wxPostScriptDC (const wxString& file, bool interactive, wxWindow *parent)
{
  Create(file, interactive, parent);
}

bool wxPostScriptDC::Create(const wxString& file, bool interactive, wxWindow *parent)
{
  m_isInteractive = interactive;

  m_yOrigin = 792;		            // For EPS output
//  m_yOrigin = 842;		            // For A4 output

  m_minX = 1000;
  m_minY = 1000;
  m_maxX = -1000;
  m_maxY = -1000;
  m_title = "";
  m_filename = file;
  m_pstream = (ofstream *) NULL;

#ifdef __WXMSW__
  // Can only send to file in Windows
  wxThePrintSetupData->SetPrinterMode(PS_FILE);
#endif

  if (m_isInteractive)
  {
    if ((m_ok = PrinterDialog (parent) ) == FALSE) return FALSE;
  }
  else
    m_ok = TRUE;

  m_currentRed = 255;
  m_currentGreen = 255;
  m_currentBlue = 0;

  m_scaleFactor = 1.0;

  return m_ok;
}

wxPostScriptDC::~wxPostScriptDC (void)
{
  if (m_pstream)
    delete m_pstream;
}

bool wxPostScriptDC::PrinterDialog(wxWindow *parent)
{
  wxPostScriptPrintDialog dialog (parent, _("Printer Settings"), wxPoint(150, 150), wxSize(400, 400), wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL);
  m_ok = (dialog.ShowModal () == wxID_OK) ;

  if (!m_ok)
    return FALSE;

  if ((m_filename == "") && (wxThePrintSetupData->GetPrinterMode() == PS_PREVIEW  || wxThePrintSetupData->GetPrinterMode() == PS_PRINTER))
    {
// steve, 05.09.94
#ifdef __VMS__
      wxThePrintSetupData->SetPrinterFile("preview");
#else
      // For PS_PRINTER action this depends on a Unix-style print spooler
      // since the wx_printer_file can be destroyed during a session
      // @@@ TODO: a Windows-style answer for non-Unix
      char userId[256];
      wxGetUserId (userId, sizeof (userId) / sizeof (char));
      char tmp[256];
      strcpy (tmp, "/tmp/preview_");
      strcat (tmp, userId);
      wxThePrintSetupData->SetPrinterFile(tmp);
#endif
      char tmp2[256];
      strcpy(tmp2, wxThePrintSetupData->GetPrinterFile());
      strcat (tmp2, ".ps");
      wxThePrintSetupData->SetPrinterFile(tmp2);
      m_filename = tmp2;
    }
    else if ((m_filename == "") && (wxThePrintSetupData->GetPrinterMode() == PS_FILE))
    {
      char *file = wxSaveFileSelector (_("PostScript"), "ps");
      if (!file)
      {
        m_ok = FALSE;
        return FALSE;
      }
      wxThePrintSetupData->SetPrinterFile(file);
      m_filename = file;
      m_ok = TRUE;
    }

  return m_ok;
}

void wxPostScriptDC::SetClippingRegion (long cx, long cy, long cw, long ch)
{
  if (m_clipping)
    return;
  if (!m_pstream)
    return;

  m_clipping = TRUE;
  *m_pstream << "gsave\n";
  *m_pstream << "newpath\n";
  *m_pstream << cx << " " << YSCALE (cy) << " moveto\n";
  *m_pstream << cx + cw << " " << YSCALE (cy) << " lineto\n";
  *m_pstream << cx + cw << " " << YSCALE (cy + ch) << " lineto\n";
  *m_pstream << cx << " " << YSCALE (cy + ch) << " lineto\n";
  *m_pstream << "closepath clip newpath\n";
}

void wxPostScriptDC::DestroyClippingRegion (void)
{
  if (!m_pstream)
    return;
  if (m_clipping)
    {
      m_clipping = FALSE;
      *m_pstream << "grestore\n";
    }
}

void wxPostScriptDC::Clear (void)
{
}

void wxPostScriptDC::FloodFill (long WXUNUSED(x), long WXUNUSED(y), wxColour * WXUNUSED(col), int WXUNUSED(style))
{
}

bool wxPostScriptDC::GetPixel (long WXUNUSED(x), long WXUNUSED(y), wxColour * WXUNUSED(col)) const
{
  return FALSE;
}

void wxPostScriptDC::CrossHair (long WXUNUSED(x), long WXUNUSED(y))
{
}

void wxPostScriptDC::DrawLine (long x1, long y1, long x2, long y2)
{
  if (!m_pstream)
    return;
  if (m_pen.Ok())
    SetPen (m_pen);
  *m_pstream << "newpath\n";
  *m_pstream << x1 << " " << YSCALE (y1) << " moveto\n";
  *m_pstream << x2 << " " << YSCALE (y2) << " lineto\n";
  *m_pstream << "stroke\n";
  CalcBoundingBox (x1, (long)YSCALE (y1));
  CalcBoundingBox (x2, (long)YSCALE (y2));
}

#define RAD2DEG 57.29577951308

void wxPostScriptDC::DrawArc (long x1, long y1, long x2, long y2, long xc, long yc)
{
    if (!m_pstream)
		return;

    long dx = x1 - xc;
    long dy = y1 - yc;
    long radius = (long) sqrt(dx*dx+dy*dy);
    double alpha1, alpha2;

    if (x1 == x2 && y1 == y2) {
	alpha1 = 0.0;
	alpha2 = 360.0;
    } else if (radius == 0.0) {
	alpha1 = alpha2 = 0.0;
    } else {
	alpha1 = (x1 - xc == 0) ?
	    (y1 - yc < 0) ? 90.0 : -90.0 :
	    -atan2(double(y1-yc), double(x1-xc)) * RAD2DEG;
	alpha2 = (x2 - xc == 0) ?
	    (y2 - yc < 0) ? 90.0 : -90.0 :
	    -atan2(double(y2-yc), double(x2-xc)) * RAD2DEG;
    }
    while (alpha1 <= 0)   alpha1 += 360;
    while (alpha2 <= 0)   alpha2 += 360; // adjust angles to be between
    while (alpha1 > 360)  alpha1 -= 360; // 0 and 360 degree
    while (alpha2 > 360)  alpha2 -= 360;

    if (m_brush.Ok() && m_brush.GetStyle() != wxTRANSPARENT) {
	SetBrush(m_brush);
	*m_pstream << "newpath\n"
	         << xc      << " " << YSCALE(yc) << " "
	         << radius  << " " << radius     << " "
		 << alpha1  << " " << alpha2     << " ellipse\n"
		 << xc      << " " << YSCALE(yc) << " lineto\n"
		 << "closepath\n"
	         << "fill\n";
    }
    if (m_pen.Ok() && m_pen.GetStyle() != wxTRANSPARENT) {
	SetPen(m_pen);
	*m_pstream << "newpath\n"
	         << xc      << " " << YSCALE(yc) << " "
	         << radius  << " " << radius     << " "
		 << alpha1  << " " << alpha2     << " ellipse\n"
	         << "stroke\n";
    }
    CalcBoundingBox(x1, (long)YSCALE(y1));
    CalcBoundingBox(x2, (long)YSCALE(y2));
}

void wxPostScriptDC::DrawEllipticArc(long x,long y,long w,long h,double sa,double ea)
{
  if (!m_pstream)
    return;

  if (sa>=360 || sa<=-360) sa=sa-int(sa/360)*360;
  if (ea>=360 || ea<=-360) ea=ea-int(ea/360)*360;
  if (sa<0) sa+=360;
  if (ea<0) ea+=360;
  if (sa==ea)
  {
    DrawEllipse(x,y,w,h);
    return;
  }

  if (m_brush.Ok() && m_brush.GetStyle () != wxTRANSPARENT)
    {
      SetBrush (m_brush);

      *m_pstream << 
         "newpath\n" <<
         x+w/2 << " " << YSCALE (y+h/2) << " " <<
         w/2 << " " << h/2 << " " <<
         int(sa) <<" "<< int(ea)<<" true ellipticarc\n";

      CalcBoundingBox (x , (long)YSCALE (y ));
      CalcBoundingBox (x+w,(long)YSCALE(y+h));
    }
  if (m_pen.Ok() && m_pen.GetStyle () != wxTRANSPARENT)
    {
      SetPen (m_pen);

      *m_pstream << 
         "newpath\n" <<
         x+w/2 << " " << YSCALE (y+h/2) << " " <<
         w/2 << " " << h/2 << " " <<
         int(sa) <<" "<< int(ea)<<" false ellipticarc\n";

      CalcBoundingBox (x , (long)YSCALE (y ));
      CalcBoundingBox (x+w,(long)YSCALE(y+h));
    }
}

void wxPostScriptDC::DrawPoint (long x, long y)
{
  if (!m_pstream)
    return;
  if (m_pen.Ok())
    SetPen (m_pen);
  *m_pstream << "newpath\n";
  *m_pstream << x << " " << YSCALE (y) << " moveto\n";
  *m_pstream << (x+1) << " " << YSCALE (y) << " lineto\n";
  *m_pstream << "stroke\n";
  CalcBoundingBox (x, (long)YSCALE (y));
}

void wxPostScriptDC::DrawPolygon (int n, wxPoint points[], long xoffset, long yoffset, int WXUNUSED(fillStyle))
{
  if (!m_pstream)
    return;
  if (n > 0)
    {
      if (m_brush.Ok() && m_brush.GetStyle () != wxTRANSPARENT)
	{
	  SetBrush (m_brush);
	  *m_pstream << "newpath\n";

	  long xx = points[0].x + xoffset;
	  long yy = (long) YSCALE (points[0].y + yoffset);
	  *m_pstream << xx << " " << yy << " moveto\n";
	  CalcBoundingBox (xx, yy);

	  int i;
	  for (i = 1; i < n; i++)
	    {
	      xx = points[i].x + xoffset;
	      yy = (long) YSCALE (points[i].y + yoffset);
	      *m_pstream << xx << " " << yy << " lineto\n";
	      CalcBoundingBox (xx, yy);
	    }
	  *m_pstream << "fill\n";
	}

      if (m_pen.Ok() && m_pen.GetStyle () != wxTRANSPARENT)
	{
	  SetPen (m_pen);
	  *m_pstream << "newpath\n";

	  long xx = points[0].x + xoffset;
	  long yy = (long) YSCALE (points[0].y + yoffset);
	  *m_pstream << xx << " " << yy << " moveto\n";
	  CalcBoundingBox (xx, yy);

	  int i;
	  for (i = 1; i < n; i++)
	    {
	      xx = points[i].x + xoffset;
	      yy = (long) YSCALE (points[i].y + yoffset);
	      *m_pstream << xx << " " << yy << " lineto\n";
	      CalcBoundingBox (xx, yy);
	    }

	  // Close the polygon
	  xx = points[0].x + xoffset;
	  yy = (long) YSCALE (points[0].y + yoffset);
	  *m_pstream << xx << " " << yy << " lineto\n";

	  // Output the line
	  *m_pstream << "stroke\n";
	}
    }
}

void wxPostScriptDC::DrawLines (int n, wxPoint points[], long xoffset, long yoffset)
{
  if (!m_pstream)
    return;
  if (n > 0)
    {
      if (m_pen.Ok())
		SetPen (m_pen);

      *m_pstream << "newpath\n";

      long xx = points[0].x + xoffset;
      long yy = (long) YSCALE (points[0].y + yoffset);
      *m_pstream << xx << " " << yy << " moveto\n";
      CalcBoundingBox (xx, yy);

      int i;
      for (i = 1; i < n; i++)
	{
	  xx = points[i].x + xoffset;
	  yy = (long) YSCALE (points[i].y + yoffset);
	  *m_pstream << xx << " " << yy << " lineto\n";
	  CalcBoundingBox (xx, yy);
	}
      *m_pstream << "stroke\n";
    }
}

void wxPostScriptDC::DrawRectangle (long x, long y, long width, long height)
{
  if (!m_pstream)
    return;
  if (m_brush.Ok() && m_brush.GetStyle () != wxTRANSPARENT)
    {
      SetBrush (m_brush);

      *m_pstream << "newpath\n";
      *m_pstream << x << " " << YSCALE (y) << " moveto\n";
      *m_pstream << x + width << " " << YSCALE (y) << " lineto\n";
      *m_pstream << x + width << " " << YSCALE (y + height) << " lineto\n";
      *m_pstream << x << " " << YSCALE (y + height) << " lineto\n";
      *m_pstream << "closepath\n";
      *m_pstream << "fill\n";

      CalcBoundingBox (x, (long)YSCALE (y));
      CalcBoundingBox (x + width, (long)YSCALE (y + height));
    }
  if (m_pen.Ok() && m_pen.GetStyle () != wxTRANSPARENT)
    {
      SetPen (m_pen);

      *m_pstream << "newpath\n";
      *m_pstream << x << " " << YSCALE (y) << " moveto\n";
      *m_pstream << x + width << " " << YSCALE (y) << " lineto\n";
      *m_pstream << x + width << " " << YSCALE (y + height) << " lineto\n";
      *m_pstream << x << " " << YSCALE (y + height) << " lineto\n";
      *m_pstream << "closepath\n";
      *m_pstream << "stroke\n";

      CalcBoundingBox (x, (long)YSCALE (y));
      CalcBoundingBox (x + width, (long)YSCALE (y + height));
    }
}

void wxPostScriptDC::DrawRoundedRectangle (long x, long y, long width, long height, double radius)
{
  if (!m_pstream)
    return;

  if (radius < 0.0)
    {
      // Now, a negative radius is interpreted to mean
      // 'the proportion of the smallest X or Y dimension'
      double smallest = 0.0;
      if (width < height)
	smallest = width;
      else
	smallest = height;
      radius =  (-radius * smallest);
    }

  if (m_brush.Ok() && m_brush.GetStyle () != wxTRANSPARENT)
    {
      SetBrush (m_brush);
      // Draw rectangle anticlockwise
      *m_pstream << "newpath\n";
      *m_pstream << x + radius << " " << YSCALE (y + radius) << " " << radius << " 90 180 arc\n";

      *m_pstream << x << " " << YSCALE (y + radius) << " moveto\n";

      *m_pstream << x + radius << " " << YSCALE (y + height - radius) << " " << radius << " 180 270 arc\n";
      *m_pstream << x + width - radius << " " << YSCALE (y + height) << " lineto\n";

      *m_pstream << x + width - radius << " " << YSCALE (y + height - radius) << " " << radius << " 270 0 arc\n";
      *m_pstream << x + width << " " << YSCALE (y + radius) << " lineto\n";

      *m_pstream << x + width - radius << " " << YSCALE (y + radius) << " " << radius << " 0 90 arc\n";

      *m_pstream << x + radius << " " << YSCALE (y) << " lineto\n";

      *m_pstream << "closepath\n";

      *m_pstream << "fill\n";

      CalcBoundingBox (x, (long)YSCALE (y));
      CalcBoundingBox (x + width, (long)YSCALE (y + height));
    }
  if (m_pen.Ok() && m_pen.GetStyle () != wxTRANSPARENT)
    {
      SetPen (m_pen);
      // Draw rectangle anticlockwise
      *m_pstream << "newpath\n";
      *m_pstream << x + radius << " " << YSCALE (y + radius) << " " << radius << " 90 180 arc\n";

      *m_pstream << x << " " << YSCALE (y + height - radius) << " lineto\n";

      *m_pstream << x + radius << " " << YSCALE (y + height - radius) << " " << radius << " 180 270 arc\n";
      *m_pstream << x + width - radius << " " << YSCALE (y + height) << " lineto\n";

      *m_pstream << x + width - radius << " " << YSCALE (y + height - radius) << " " << radius << " 270 0 arc\n";
      *m_pstream << x + width << " " << YSCALE (y + radius) << " lineto\n";

      *m_pstream << x + width - radius << " " << YSCALE (y + radius) << " " << radius << " 0 90 arc\n";

      *m_pstream << x + radius << " " << YSCALE (y) << " lineto\n";

      *m_pstream << "closepath\n";

      *m_pstream << "stroke\n";

      CalcBoundingBox (x, (long)YSCALE (y));
      CalcBoundingBox (x + width, (long)YSCALE (y + height));
    }
}

void wxPostScriptDC::DrawEllipse (long x, long y, long width, long height)
{
  if (!m_pstream)
    return;
  if (m_brush.Ok() && m_brush.GetStyle () != wxTRANSPARENT)
    {
      SetBrush (m_brush);

      *m_pstream << "newpath\n";
      *m_pstream << x + width / 2 << " " << YSCALE (y + height / 2) << " ";
      *m_pstream << width / 2 << " " << height / 2 << " 0 360 ellipse\n";
      *m_pstream << "fill\n";

      CalcBoundingBox (x - width, (long)YSCALE (y - height));
      CalcBoundingBox (x + width, (long)YSCALE (y + height));
    }
  if (m_pen.Ok() && m_pen.GetStyle () != wxTRANSPARENT)
    {
      SetPen (m_pen);

      *m_pstream << "newpath\n";
      *m_pstream << x + width / 2 << " " << YSCALE (y + height / 2) << " ";
      *m_pstream << width / 2 << " " << height / 2 << " 0 360 ellipse\n";
      *m_pstream << "stroke\n";

      CalcBoundingBox (x - width, (long)YSCALE (y - height));
      CalcBoundingBox (x + width, (long)YSCALE (y + height));
    }
}

void wxPostScriptDC::DrawIcon (const wxIcon& icon, long x, long y)
{
#if defined(__X__) || defined(__WXGTK__)
  wxMemoryDC memDC;
  memDC.SelectObject(icon);
  Blit(x, y, icon.GetWidth(), icon.GetHeight(), &memDC, 0, 0);
#endif
}

void wxPostScriptDC::SetFont (const wxFont& the_font)
{
  if (!m_pstream)
    return;

  if (m_font == the_font)
    return;

  m_font = the_font;

  if ( !m_font.Ok() )
	return;

  char buf[100];
  const char *name;
  const char *style = "";
  int Style = m_font.GetStyle ();
  int Weight = m_font.GetWeight ();

  switch (m_font.GetFamily ())
    {
    case wxTELETYPE:
    case wxMODERN:
      name = "/Courier";
      break;
    case wxSWISS:
      name = "/Helvetica";
      break;
    case wxROMAN:
//      name = "/Times-Roman";
      name = "/Times"; // Altered by EDZ
      break;
    case wxSCRIPT:
      name = "/Zapf-Chancery-MediumItalic";
      Style  = wxNORMAL;
      Weight = wxNORMAL;
      break;
    default:
    case wxDEFAULT: // Sans Serif Font
      name = "/LucidaSans";
    }

  if (Style == wxNORMAL && (Weight == wxNORMAL || Weight == wxLIGHT))
    {
      if (m_font.GetFamily () == wxROMAN)
	style = "-Roman";
      else
	style = "";
    }
  else if (Style == wxNORMAL && Weight == wxBOLD)
    style = "-Bold";

  else if (Style == wxITALIC && (Weight == wxNORMAL || Weight == wxLIGHT))
    {
      if (m_font.GetFamily () == wxROMAN)
	style = "-Italic";
      else
	style = "-Oblique";
    }
  else if (Style == wxITALIC && Weight == wxBOLD)
    {
      if (m_font.GetFamily () == wxROMAN)
	style = "-BoldItalic";
      else
	style = "-BoldOblique";
    }
  else if (Style == wxSLANT && (Weight == wxNORMAL || Weight == wxLIGHT))
    {
      if (m_font.GetFamily () == wxROMAN)
	style = "-Italic";
      else
	style = "-Oblique";
    }
  else if (Style == wxSLANT && Weight == wxBOLD)
    {
      if (m_font.GetFamily () == wxROMAN)
	style = "-BoldItalic";
      else
	style = "-BoldOblique";
    }
  else
    style = "";

  strcpy (buf, name);
  strcat (buf, style);
  *m_pstream << buf << " findfont\n";
  *m_pstream << m_font.GetPointSize() * m_scaleFactor << " scalefont setfont\n";
}

void wxPostScriptDC::SetPen (const wxPen& pen)
{
  if (!m_pstream)
    return;

  int oldStyle = m_pen.GetStyle();

  m_pen = pen;

  if (!m_pen.Ok())
    return;

  // Line width
  *m_pstream << m_pen.GetWidth () << " setlinewidth\n";

  // Line style - WRONG: 2nd arg is OFFSET
  /*
     Here, I'm afraid you do not conceive meaning of parameters of 'setdash'
     operator correctly. You should look-up this in the Red Book: the 2nd parame-
     ter is not number of values in the array of the first one, but an offset
     into this description of the pattern. I mean a real *offset* not index
     into array. I.e. If the command is [3 4] 1 setdash   is used, then there
     will be first black line *2* units long, then space 4 units, then the
     pattern of *3* units black, 4 units space will be repeated.
   */
  static const char *dotted = "[2 5] 2";
  static const char *short_dashed = "[4 4] 2";
  static const char *long_dashed = "[4 8] 2";
  static const char *dotted_dashed = "[6 6 2 6] 4";

  const char *psdash = (char *) NULL;
  switch (m_pen.GetStyle ())
    {
    case wxDOT:
      psdash = dotted;
      break;
    case wxSHORT_DASH:
      psdash = short_dashed;
      break;
    case wxLONG_DASH:
      psdash = long_dashed;
      break;
    case wxDOT_DASH:
      psdash = dotted_dashed;
      break;
    case wxSOLID:
    case wxTRANSPARENT:
    default:
      psdash = "[] 0";
      break;
    }
  if (oldStyle != m_pen.GetStyle())
    *m_pstream << psdash << " setdash\n";

  // Line colour
  unsigned char red = m_pen.GetColour ().Red ();
  unsigned char blue = m_pen.GetColour ().Blue ();
  unsigned char green = m_pen.GetColour ().Green ();

  if (!m_colour)
    {
      // Anything not white is black
      if (!(red == (unsigned char) 255 && blue == (unsigned char) 255
	    && green == (unsigned char) 255))
	{
	  red = (unsigned char) 0;
	  green = (unsigned char) 0;
	  blue = (unsigned char) 0;
	}
    }

  if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue))
  {
    long redPS = (long) (((int) red) / 255.0);
    long bluePS = (long) (((int) blue) / 255.0);
    long greenPS = (long) (((int) green) / 255.0);

    *m_pstream << redPS << " " << greenPS << " " << bluePS << " setrgbcolor\n";

    m_currentRed = red;
    m_currentBlue = blue;
    m_currentGreen = green;
  }
}

void wxPostScriptDC::SetBrush (const wxBrush& brush)
{
  if (!m_pstream)
    return;

  m_brush = brush;

  if ( !m_brush.Ok() )
	return;

  // Brush colour
  unsigned char red = m_brush.GetColour ().Red ();
  unsigned char blue = m_brush.GetColour ().Blue ();
  unsigned char green = m_brush.GetColour ().Green ();

  if (!m_colour)
    {
      // Anything not black is white
      if (!(red == (unsigned char) 0 && blue == (unsigned char) 0
	    && green == (unsigned char) 0))
	{
	  red = (unsigned char) 255;
	  green = (unsigned char) 255;
	  blue = (unsigned char) 255;
	}
    }

  if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue))
  {
    long redPS = (long) (((int) red) / 255.0);
    long bluePS = (long) (((int) blue) / 255.0);
    long greenPS = (long) (((int) green) / 255.0);
    *m_pstream << redPS << " " << greenPS << " " << bluePS << " setrgbcolor\n";
    m_currentRed = red;
    m_currentBlue = blue;
    m_currentGreen = green;
  }
}

void wxPostScriptDC::DrawText (const wxString& text, long x, long y, bool WXUNUSED(use16bit))
{
  if (!m_pstream)
    return;

  // TODO: SetFont checks for identity so this will always be a NULL operation
  if (m_font.Ok())
    SetFont (m_font);

  if (m_textForegroundColour.Ok ())
    {
      unsigned char red = m_textForegroundColour.Red ();
      unsigned char blue = m_textForegroundColour.Blue ();
      unsigned char green = m_textForegroundColour.Green ();

      if (!m_colour)
	{
	  // Anything not white is black
	  if (!(red == (unsigned char) 255 && blue == (unsigned char) 255
		&& green == (unsigned char) 255))
	    {
	      red = (unsigned char) 0;
	      green = (unsigned char) 0;
	      blue = (unsigned char) 0;
	    }
	}
      if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue))
      {
        long redPS = (long) (((int) red) / 255.0);
        long bluePS = (long) (((int) blue) / 255.0);
        long greenPS = (long) (((int) green) / 255.0);
        *m_pstream << redPS << " " << greenPS << " " << bluePS << " setrgbcolor\n";

        m_currentRed = red;
        m_currentBlue = blue;
        m_currentGreen = green;
      }
    }

  int size = 10;
  if (m_font.Ok())
    size = m_font.GetPointSize ();

  *m_pstream << x << " " << YSCALE (y + size) << " moveto\n";

//  *m_pstream << "(" << text << ")" << " show\n";
  *m_pstream << "(";
  int len = strlen ((char *)(const char *)text);
  int i;
  for (i = 0; i < len; i++)
    {
/*
      char ch = text[i];
      if (ch == ')' || ch == '(' || ch == '\\')
	*m_pstream << "\\";
      *m_pstream << ch;
*/
      int c = (unsigned char) text[i];
      if ( c == ')' || c == '(' || c == '\\')
      {
        *m_pstream << "\\" << (char) c;
      }
      else if ( c >= 128 )
      {
        // Cope with character codes > 127
        char tmp[5];
        sprintf(tmp, "\\%o", c);
        *m_pstream << tmp;
      }
      else
        *m_pstream << (char) c;
    }

  *m_pstream << ")" << " show\n";

  if (m_font.GetUnderlined())
  {
      long w, h;
      GetTextExtent(text, &w, &h);
      *m_pstream << "gsave " << x << " " << YSCALE (y + size - UnderlinePosition)
               << " moveto\n"
               << UnderlineThickness << " setlinewidth "
               << x + w << " " << YSCALE (y + size - UnderlinePosition)
               << " lineto stroke grestore\n";
  }
  
  CalcBoundingBox (x, (long)YSCALE (y + size));
  CalcBoundingBox (x + size * strlen ((char *)(const char *)text), (long)YSCALE (y));
}


void wxPostScriptDC::SetBackground (const wxBrush& brush)
{
  m_backgroundBrush = brush;
}

void wxPostScriptDC::SetLogicalFunction (int WXUNUSED(function))
{
}

static const char *wxPostScriptHeaderEllipse = "\
/ellipsedict 8 dict def\n\
ellipsedict /mtrx matrix put\n\
/ellipse\n\
{ ellipsedict begin\n\
  /endangle exch def\n\
  /startangle exch def\n\
  /yrad exch def\n\
  /xrad exch def\n\
  /y exch def\n\
  /x exch def\n\
  /savematrix mtrx currentmatrix def\n\
  x y translate\n\
  xrad yrad scale\n\
  0 0 1 startangle endangle arc\n\
  savematrix setmatrix\n\
  end\n\
  } def\n\
";

static const char *wxPostScriptHeaderEllipticArc= "\
/ellipticarcdict 8 dict def\n\
ellipticarcdict /mtrx matrix put\n\
/ellipticarc\n\
{ ellipticarcdict begin\n\
  /do_fill exch def\n\
  /endangle exch def\n\
  /startangle exch def\n\
  /yrad exch def\n\
  /xrad exch def \n\
  /y exch def\n\
  /x exch def\n\
  /savematrix mtrx currentmatrix def\n\
  x y translate\n\
  xrad yrad scale\n\
  do_fill { 0 0 moveto } if\n\
  0 0 1 startangle endangle arc\n\
  savematrix setmatrix\n\
  do_fill { fill }{ stroke } ifelse\n\
  end\n\
} def\n";

bool wxPostScriptDC::StartDoc (const wxString& message)
{
    if (m_filename == "")
	{
#ifdef __VMS__
          m_filename = "wxtmp.ps";
#else
          m_filename = wxGetTempFileName("ps");
#endif
	  wxThePrintSetupData->SetPrinterFile((char *)(const char *)m_filename);
	  m_ok = TRUE;
	}
      else
	wxThePrintSetupData->SetPrinterFile((char *)(const char *)m_filename);

#ifdef __VMS__
      // steve, 05.09.94
      // VMS is sh*t!
      m_pstream = new ofstream;
      if(fileBuffer) delete[] fileBuffer;
      fileBuffer = new char[VMS_BUFSIZ]; 
      m_pstream->setbuf(fileBuffer,VMS_BUFSIZ);
      m_pstream->open(wxThePrintSetupData->GetPrinterFile());
#else
      m_pstream = new ofstream (wxThePrintSetupData->GetPrinterFile());
#endif
      if (!m_pstream || !m_pstream->good())
	{
	  wxMessageBox (_("Cannot open file!"), _("Error"), wxOK);
	  m_ok = FALSE;
	  return FALSE;
	}
  m_ok = TRUE;

  SetBrush (*wxBLACK_BRUSH);
  SetPen (*wxBLACK_PEN);

  wxPageNumber = 1;
  m_title = message;
  return TRUE;
}


void wxPostScriptDC::EndDoc (void)
{
    static char wxPostScriptHeaderReencodeISO1[] =
    "\n/reencodeISO {\n"
"dup dup findfont dup length dict begin\n"
"{ 1 index /FID ne { def }{ pop pop } ifelse } forall\n"
"/Encoding ISOLatin1Encoding def\n"
"currentdict end definefont\n"
"} def\n"
"/ISOLatin1Encoding [\n"
"/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
"/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
"/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
"/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
"/space/exclam/quotedbl/numbersign/dollar/percent/ampersand/quoteright\n"
"/parenleft/parenright/asterisk/plus/comma/minus/period/slash\n"
"/zero/one/two/three/four/five/six/seven/eight/nine/colon/semicolon\n"
"/less/equal/greater/question/at/A/B/C/D/E/F/G/H/I/J/K/L/M/N\n"
"/O/P/Q/R/S/T/U/V/W/X/Y/Z/bracketleft/backslash/bracketright\n"
"/asciicircum/underscore/quoteleft/a/b/c/d/e/f/g/h/i/j/k/l/m\n"
"/n/o/p/q/r/s/t/u/v/w/x/y/z/braceleft/bar/braceright/asciitilde\n"
"/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
"/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
"/.notdef/dotlessi/grave/acute/circumflex/tilde/macron/breve\n"
"/dotaccent/dieresis/.notdef/ring/cedilla/.notdef/hungarumlaut\n";

    static char wxPostScriptHeaderReencodeISO2[] =
"/ogonek/caron/space/exclamdown/cent/sterling/currency/yen/brokenbar\n"
"/section/dieresis/copyright/ordfeminine/guillemotleft/logicalnot\n"
"/hyphen/registered/macron/degree/plusminus/twosuperior/threesuperior\n"
"/acute/mu/paragraph/periodcentered/cedilla/onesuperior/ordmasculine\n"
"/guillemotright/onequarter/onehalf/threequarters/questiondown\n"
"/Agrave/Aacute/Acircumflex/Atilde/Adieresis/Aring/AE/Ccedilla\n"
"/Egrave/Eacute/Ecircumflex/Edieresis/Igrave/Iacute/Icircumflex\n"
"/Idieresis/Eth/Ntilde/Ograve/Oacute/Ocircumflex/Otilde/Odieresis\n"
"/multiply/Oslash/Ugrave/Uacute/Ucircumflex/Udieresis/Yacute\n"
"/Thorn/germandbls/agrave/aacute/acircumflex/atilde/adieresis\n"
"/aring/ae/ccedilla/egrave/eacute/ecircumflex/edieresis/igrave\n"
"/iacute/icircumflex/idieresis/eth/ntilde/ograve/oacute/ocircumflex\n"
"/otilde/odieresis/divide/oslash/ugrave/uacute/ucircumflex/udieresis\n"
"/yacute/thorn/ydieresis\n"
        "] def\n\n";

    if (!m_pstream)
    	return;
  if (m_clipping)
    {
      m_clipping = FALSE;
      *m_pstream << "grestore\n";
    }

  // Will reuse m_pstream for header
#ifdef __VMS__
  // see the definition of fileBuffer for explanation
  m_pstream->close(); // steve, 05.09.94
  if(fileBuffer) delete[] fileBuffer;
#endif
  if (m_pstream)
    {
      delete m_pstream;
      m_pstream = (ofstream *) NULL;
    }

  // Write header now
// steve, 05.09.94
#ifdef __VMS__
  char *header_file = "header.ps";
#else
  char *header_file = wxGetTempFileName("ps");
#endif
  m_pstream = new ofstream (header_file);

  *m_pstream << "%!PS-Adobe-2.0\n";	/* PostScript magic strings */
  *m_pstream << "%%Title: " << (const char *) m_title << "\n";
  *m_pstream << "%%Creator: " << wxTheApp->argv[0] << "\n";
//  time_t when; time (&when);
//  *m_pstream << "%%CreationDate: " << ctime (&when);
  *m_pstream << "%%CreationDate: " << wxNow() << "\n";

  // User Id information
  char userID[256];
  if ( wxGetEmailAddress(userID, sizeof(userID)) )
  {
    *m_pstream << "%%For: " << (char *)userID;
    char userName[245];
    if (wxGetUserName(userName, sizeof(userName)))
      *m_pstream << " (" << (char *)userName << ")";
    *m_pstream << "\n";
  }
  else if ( wxGetUserName(userID, sizeof(userID)) )
  {
    *m_pstream << "%%For: " << (char *)userID << "\n";
  }

  // THE FOLLOWING HAS BEEN CONTRIBUTED BY Andy Fyfe <andy@hyperparallel.com>

  long wx_printer_translate_x, wx_printer_translate_y;
  double wx_printer_scale_x, wx_printer_scale_y;
  wxThePrintSetupData->GetPrinterTranslation(&wx_printer_translate_x, &wx_printer_translate_y);
  wxThePrintSetupData->GetPrinterScaling(&wx_printer_scale_x, &wx_printer_scale_y);

	if (wxThePrintSetupData->GetPrinterOrientation() == PS_LANDSCAPE)
	{
		*m_pstream << "%%Orientation: Landscape\n";
	}
	else
	{
		*m_pstream << "%%Orientation: Portrait\n";
	}

  // Compute the bounding box.  Note that it is in the default user
  // coordinate system, thus we have to convert the values.
  long llx = (long) ((m_minX+wx_printer_translate_x)*wx_printer_scale_x);
  long lly = (long) ((m_minY+wx_printer_translate_y)*wx_printer_scale_y);
  long urx = (long) ((m_maxX+wx_printer_translate_x)*wx_printer_scale_x);
  long ury = (long) ((m_maxY+wx_printer_translate_y)*wx_printer_scale_y);

#if 0
  // If we're landscape, our sense of "x" and "y" is reversed.
  if (wxThePrintSetupData->GetPrinterOrientation() == PS_LANDSCAPE)
    {
      double tmp;
      tmp = llx; llx = lly; lly = tmp;
      tmp = urx; urx = ury; ury = tmp;

      // We need either the two lines that follow, or we need to subtract
      // min_x from real_translate_y, which is commented out below.
      llx = llx - m_minX*wx_printer_scale_y;
      urx = urx - m_minX*wx_printer_scale_y;
    }
#endif

  // The Adobe specifications call for integers; we round as to make
  // the bounding larger.
  *m_pstream << "%%BoundingBox: "
      << floor(llx) << " " << floor(lly) << " "
      << ceil(urx)  << " " << ceil(ury)  << "\n";
  *m_pstream << "%%Pages: " << wxPageNumber - 1 << "\n";
  *m_pstream << "%%EndComments\n\n";

  // To check the correctness of the bounding box, postscript commands
  // to draw a box corresponding to the bounding box are generated below.
  // But since we typically don't want to print such a box, the postscript
  // commands are generated within comments.  These lines appear before any
  // adjustment of scale, rotation, or translation, and hence are in the
  // default user coordinates.
  *m_pstream << "% newpath\n";
  *m_pstream << "% " << llx << " " << lly << " moveto\n";
  *m_pstream << "% " << urx << " " << lly << " lineto\n";
  *m_pstream << "% " << urx << " " << ury << " lineto\n";
  *m_pstream << "% " << llx << " " << ury << " lineto closepath stroke\n";

#if 0
  // Output scaling
  long real_translate_y = wx_printer_translate_y;
  if (wxThePrintSetupData->GetPrinterOrientation() == PS_LANDSCAPE)
    {
      real_translate_y -= m_maxY;
      // The following line can be used instead of the adjustment to 
      // llx and urx above.
      // real_translate_y -= m_minX;
      *m_pstream << "90 rotate\n";
    }

/* Probably don't want this now we have it in EndPage, below.
 * We should rationalise the scaling code to one place. JACS, October 1995
 * Do we take the next 2 lines out or not?
 */
 
  *m_pstream << wx_printer_scale_x << " " << wx_printer_scale_y << " scale\n";
  *m_pstream << wx_printer_translate_x << " " << real_translate_y << " translate\n";
#endif

  *m_pstream << "%%BeginProlog\n";
  *m_pstream << wxPostScriptHeaderEllipse;
  *m_pstream << wxPostScriptHeaderEllipticArc;
  *m_pstream << wxPostScriptHeaderReencodeISO1;
  *m_pstream << wxPostScriptHeaderReencodeISO2;

  if (wxPostScriptHeaderSpline)
    *m_pstream << wxPostScriptHeaderSpline;
  *m_pstream << "%%EndProlog\n";

  delete m_pstream;
  m_pstream = (ofstream *) NULL;

#ifdef __VMS__
  char *tmp_file = "tmp.ps";
#else
  char *tmp_file = wxGetTempFileName("ps");
#endif

  // Paste header Before wx_printer_file
  wxConcatFiles (header_file, wxThePrintSetupData->GetPrinterFile(), tmp_file);
  wxRemoveFile (header_file);
  wxRemoveFile (wxThePrintSetupData->GetPrinterFile());
  wxRenameFile(tmp_file, wxThePrintSetupData->GetPrinterFile());

#if defined(__X__) || defined(__WXGTK__)
  if (m_ok)
    {
      switch (wxThePrintSetupData->GetPrinterMode()) {
	case PS_PREVIEW:
	{
          char *argv[3];
          argv[0] = wxThePrintSetupData->GetPrintPreviewCommand();
          argv[1] = wxThePrintSetupData->GetPrinterFile();
          argv[2] = (char *) NULL;
	  wxExecute (argv, TRUE);
          wxRemoveFile(wxThePrintSetupData->GetPrinterFile());
	}
	break;

	case PS_PRINTER:
	{
          char *argv[4];
          int argc = 0;
          argv[argc++] = wxThePrintSetupData->GetPrinterCommand();

          // !SM! If we simply assign to argv[1] here, if printer options
          // are blank, we get an annoying and confusing message from lpr.
          char * opts = wxThePrintSetupData->GetPrinterOptions();
          if (opts && *opts)
              argv[argc++] = opts;
              
          argv[argc++] = wxThePrintSetupData->GetPrinterFile();
          argv[argc++] = (char *) NULL;
	  wxExecute (argv, TRUE);
          wxRemoveFile(wxThePrintSetupData->GetPrinterFile());
	}
	break;

	case PS_FILE:
	  break;
	}
    }
#endif
}

void wxPostScriptDC::StartPage (void)
{
  if (!m_pstream)
    return;
  *m_pstream << "%%Page: " << wxPageNumber++ << "\n";
//  *m_pstream << "matrix currentmatrix\n";

    // Added by Chris Breeze

	// Each page starts with an "initgraphics" which resets the
	// transformation and so we need to reset the origin
	// (and rotate the page for landscape printing)
	m_scaleFactor = 1.0;
	m_logicalOriginX = 0;
	m_logicalOriginY = 0;

	// Output scaling
	long translate_x, translate_y;
	double scale_x, scale_y;
	wxThePrintSetupData->GetPrinterTranslation(&translate_x, &translate_y);
	wxThePrintSetupData->GetPrinterScaling(&scale_x, &scale_y);

	if (wxThePrintSetupData->GetPrinterOrientation() == PS_LANDSCAPE)
	{
		translate_y -= GetYOrigin();
		*m_pstream << "90 rotate\n";
	}

	*m_pstream << scale_x << " " << scale_y << " scale\n";
	*m_pstream << translate_x << " " << translate_y << " translate\n";
}

void wxPostScriptDC::EndPage (void)
{
  if (!m_pstream)
    return;
  *m_pstream << "showpage\n";

  // Removed by	Chris Breeze
#if 0
  *m_pstream << "setmatrix\n";

  // THE FOLLOWING HAS BEEN CONTRIBUTED BY Andy Fyfe <andy@hyperparallel.com>

  long wx_printer_translate_x, wx_printer_translate_y;
  double wx_printer_scale_x, wx_printer_scale_y;
  wxThePrintSetupData->GetPrinterTranslation(&wx_printer_translate_x, &wx_printer_translate_y);
  wxThePrintSetupData->GetPrinterScaling(&wx_printer_scale_x, &wx_printer_scale_y);

  // Compute the bounding box.  Note that it is in the default user
  // coordinate system, thus we have to convert the values.
  long llx = (m_minX+wx_printer_translate_x)*wx_printer_scale_x;
  long lly = (m_minY+wx_printer_translate_y)*wx_printer_scale_y;
  long urx = (m_maxX+wx_printer_translate_x)*wx_printer_scale_x;
  long ury = (m_maxY+wx_printer_translate_y)*wx_printer_scale_y;

  // If we're landscape, our sense of "x" and "y" is reversed.
  if (wxThePrintSetupData->GetPrinterOrientation() == PS_LANDSCAPE)
    {
      long tmp;
      tmp = llx; llx = lly; lly = tmp;
      tmp = urx; urx = ury; ury = tmp;

      // We need either the two lines that follow, or we need to subtract
      // m_minX from real_translate_y, which is commented out below.
      llx = llx - m_minX*wx_printer_scale_y;
      urx = urx - m_minX*wx_printer_scale_y;
    }

  // Output scaling
  long real_translate_y = wx_printer_translate_y;
  if (wxThePrintSetupData->GetPrinterOrientation() == PS_LANDSCAPE)
    {
      real_translate_y -= m_maxY;
      // The following line can be used instead of the adjustment to
      // llx and urx above.
      // real_translate_y -= m_minX;
      *m_pstream << "90 rotate\n";
    }

  *m_pstream << wx_printer_scale_x << " " << wx_printer_scale_y << " scale\n";
  *m_pstream << wx_printer_translate_x << " " << real_translate_y << " translate\n";
#endif
}

/* MATTHEW: Implement Blit: */
/* MATTHEW: [4] Re-wrote to use colormap */
bool wxPostScriptDC::
Blit (long xdest, long ydest, long fwidth, long fheight,
      wxDC *source, long xsrc, long ysrc, int WXUNUSED(rop), bool WXUNUSED(useMask))
{
  long width, height, x, y;

#if !defined(__X__) && !defined(__WXGTK__)
  return FALSE;
#endif

  if (!source->IsKindOf(CLASSINFO(wxPaintDC))) return FALSE;

  width = (long)floor(fwidth);
  height = (long)floor(fheight);
  x = (long)floor(xsrc);
  y = (long)floor(ysrc);

  /* PostScript setup: */
  *m_pstream << "gsave\n";
  *m_pstream << xdest << " " << YSCALE(ydest + fheight) << " translate\n";
  *m_pstream << fwidth << " " << fheight << " scale\n";
  *m_pstream << "/DataString " << width << " string def\n";
  *m_pstream << width << " " << height << " 8 [ ";
  *m_pstream << width << " 0 0 " << (-height) << " 0 " << height;
  *m_pstream << " ]\n{\n";
  *m_pstream << "  currentfile DataString readhexstring pop\n";
  *m_pstream << "} bind image\n";

#if defined(__X__) || defined(__WXGTK__)

  /* Output data as hex digits: */
  Display *d;
  Colormap cm;
  XImage *image;
  long j, i;
  char s[3];
  
#ifdef __WXGTK__

  d = gdk_display;
  cm = ((GdkColormapPrivate*)gdk_colormap_get_system())->xcolormap;
  GdkWindow *gwin = ((wxClientDC*)source)->GetWindow();
  image = XGetImage(d, ((GdkWindowPrivate*)gwin)->xwindow, x, y, width, height, AllPlanes, ZPixmap);

#else  

#ifdef __WXMOTIF__
  d = source->display;
#else
  d = wxGetDisplay();
#endif

  cm = wxGetMainColormap(d);
  image = XGetImage(d, source->pixmap, x, y, width, height, AllPlanes, ZPixmap);
  
#endif


  s[2] = 0;

#define CM_CACHE_SIZE 256
  unsigned long cachesrc[CM_CACHE_SIZE];
  int cachedest[CM_CACHE_SIZE], cache_pos = 0, all_cache = FALSE;

  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      XColor xcol;
      unsigned long spixel;
      int pixel, k;
      const unsigned short MAX_COLOR = 0xFFFF;

      spixel = XGetPixel(image, i, j);

      for (k = cache_pos; k--; )
	if (cachesrc[k] == spixel) {
	  pixel = cachedest[k];
	  goto install;
	}
      if (all_cache)
	for (k = CM_CACHE_SIZE; k-- > cache_pos; )
	  if (cachesrc[k] == spixel) {
	    pixel = cachedest[k];
	    goto install;
	  }
      
      cachesrc[cache_pos] = xcol.pixel = spixel;
      XQueryColor(d, cm, &xcol);
      
      long r, g, b;

      r = (long)((double)(xcol.red) / MAX_COLOR);
      g = (long)((double)(xcol.green) / MAX_COLOR);
      b = (long)((double)(xcol.blue) / MAX_COLOR);

      pixel = (int)(255 * sqrt(((r * r) + (g * g) + (b * b)) / 3));

      cachedest[cache_pos] = pixel;
      
      if (++cache_pos >= CM_CACHE_SIZE) {
	cache_pos = 0;
	all_cache = TRUE;
      }

    install:      
      int h, l;

      h = (pixel >> 4) & 0xF;
      l = pixel & 0xF;

      if (h <= 9)
	s[0] = '0' + h;
      else
	s[0] = 'a' + (h - 10);
      if (l <= 9)
	s[1] = '0' + l;
      else
	s[1] = 'a' + (l - 10);
      
      *m_pstream << s;
    }
    *m_pstream << "\n";
  }

  XDestroyImage(image);
#endif

  *m_pstream << "grestore\n";

  CalcBoundingBox(xdest, (long)YSCALE(ydest));
  CalcBoundingBox(xdest + fwidth, (long)YSCALE(ydest + fheight));

  return TRUE;
}

long wxPostScriptDC::GetCharHeight (void)
{
  if (m_font.Ok())
    return  m_font.GetPointSize ();
  else
    return 12;
}

void wxPostScriptDC::GetTextExtent (const wxString& string, long *x, long *y,
	       long *descent, long *externalLeading, wxFont *theFont,
				    bool WXUNUSED(use16))
{
  wxFont *fontToUse = theFont;
  if (!fontToUse)
    fontToUse = (wxFont*) &m_font;

  if (!m_pstream)
    return;
#if !USE_AFM_FOR_POSTSCRIPT
  // Provide a VERY rough estimate (avoid using it)
	// Chris Breeze 5/11/97: produces accurate results for mono-spaced
	// font such as Courier (aka wxMODERN)
	int height = 12;
	if (fontToUse)
	{
		height = fontToUse->GetPointSize();
	}
	*x = strlen (string) * height * 72 / 120;
	*y = (long) (height * 1.32);	// allow for descender

  if (descent)
    *descent = 0;
  if (externalLeading)
    *externalLeading = 0;
#else
  // +++++ start of contributed code +++++
  
  // ************************************************************
  // method for calculating string widths in postscript:
  // read in the AFM (adobe font metrics) file for the
  // actual font, parse it and extract the character widths
  // and also the descender. this may be improved, but for now
  // it works well. the AFM file is only read in if the
  // font is changed. this may be chached in the future.
  // calls to GetTextExtent with the font unchanged are rather
  // efficient!!!
  //
  // for each font and style used there is an AFM file necessary.
  // currently i have only files for the roman font family.
  // i try to get files for the other ones!
  //
  // CAVE: the size of the string is currently always calculated
  //       in 'points' (1/72 of an inch). this should later on be
  //       changed to depend on the mapping mode.
  // CAVE: the path to the AFM files must be set before calling this
  //       function. this is usually done by a call like the following:
  //       wxSetAFMPath("d:\\wxw161\\afm\\");
  //
  // example:
  //
  //    wxPostScriptDC dc(NULL, TRUE);
  //    if (dc.Ok()){
  //      wxSetAFMPath("d:\\wxw161\\afm\\");
  //      dc.StartDoc("Test");
  //      dc.StartPage();
  //      long w,h;
  //      dc.SetFont(new wxFont(10, wxROMAN, wxNORMAL, wxNORMAL));
  //      dc.GetTextExtent("Hallo",&w,&h);
  //      dc.EndPage();
  //      dc.EndDoc();
  //    }
  //
  // by steve (stefan.hammes@urz.uni-heidelberg.de)
  // created: 10.09.94
  // updated: 14.05.95

  assert(fontToUse && "void wxPostScriptDC::GetTextExtent: no font defined");
  assert(x && "void wxPostScriptDC::GetTextExtent: x == NULL");
  assert(y && "void wxPostScriptDC::GetTextExtent: y == NULL");

  // these static vars are for storing the state between calls
  static int lastFamily= INT_MIN;
  static int lastSize= INT_MIN;
  static int lastStyle= INT_MIN;
  static int lastWeight= INT_MIN;
  static int lastDescender = INT_MIN;
  static int lastWidths[256]; // widths of the characters

  // get actual parameters
  const int Family = fontToUse->GetFamily();
  const int Size =   fontToUse->GetPointSize();
  const int Style =  fontToUse->GetStyle();
  const int Weight = fontToUse->GetWeight();

  // if we have another font, read the font-metrics
  if(Family!=lastFamily||Size!=lastSize||Style!=lastStyle||Weight!=lastWeight){
    // store actual values
    lastFamily = Family;
    lastSize =   Size;
    lastStyle =  Style;
    lastWeight = Weight;

    // read in new font metrics **************************************

    // 1. construct filename ******************************************
    /* MATTHEW: [2] Use wxTheFontNameDirectory */
    const char *name;

	// Julian - we'll need to do this a different way now we've removed the
	// font directory system. Must find Stefan's original code.

    name = wxTheFontNameDirectory->GetAFMName(Family, Weight, Style);
    if (!name)
      name = "unknown";

    // get the directory of the AFM files
    char afmName[256];
    afmName[0] = 0;
    if (wxGetAFMPath())
      strcpy(afmName,wxGetAFMPath());

    // 2. open and process the file **********************************

    // a short explanation of the AFM format:
    // we have for each character a line, which gives its size
    // e.g.:
    //
    //   C 63 ; WX 444 ; N question ; B 49 -14 395 676 ;
    //
    // that means, we have a character with ascii code 63, and width 
    // (444/1000 * fontSize) points.
    // the other data is ignored for now!
    //
    // when the font has changed, we read in the right AFM file and store the
    // character widths in an array, which is processed below (see point 3.).
    
        // new elements JC Sun Aug 25 23:21:44 MET DST 1996

    
    strcat(afmName,name);
    strcat(afmName,".afm");
    FILE *afmFile = fopen(afmName,"r");
    if(afmFile==NULL){
      wxDebugMsg("GetTextExtent: can't open AFM file '%s'\n",afmName);
      wxDebugMsg("               using approximate values\n");
      int i;
      for (i=0; i<256; i++) lastWidths[i] = 500; // an approximate value
      lastDescender = -150; // dito.
    }else{
      int i;
      // init the widths array
      for(i=0; i<256; i++) lastWidths[i]= INT_MIN;
      // some variables for holding parts of a line
      char cString[10],semiString[10],WXString[10],descString[20];
      char upString[30], utString[30], encString[50];
      char line[256];
      int ascii,cWidth;
      // read in the file and parse it
      while(fgets(line,sizeof(line),afmFile)!=NULL){
        // A.) check for descender definition
        if(strncmp(line,"Descender",9)==0){
          if((sscanf(line,"%s%d",descString,&lastDescender)!=2)
	     || (strcmp(descString,"Descender")!=0)) {
	    wxDebugMsg("AFM-file '%s': line '%s' has error (bad descender)\n",
		       afmName,line);
          }
        }
            // JC 1.) check for UnderlinePosition
        else if(strncmp(line,"UnderlinePosition",17)==0){
          if((sscanf(line,"%s%lf",upString,&UnderlinePosition)!=2)
	     || (strcmp(upString,"UnderlinePosition")!=0)) {
	    wxDebugMsg("AFM-file '%s': line '%s' has error (bad UnderlinePosition)\n",
		       afmName,line);
          }
        }
	// JC 2.) check for UnderlineThickness
        else if(strncmp(line,"UnderlineThickness",18)==0){
           if((sscanf(line,"%s%lf",utString,&UnderlineThickness)!=2)
	     || (strcmp(utString,"UnderlineThickness")!=0)) {
	    wxDebugMsg("AFM-file '%s': line '%s' has error (bad UnderlineThickness)\n",
		       afmName,line);
          }
        }
	// JC 3.) check for EncodingScheme
        else if(strncmp(line,"EncodingScheme",14)==0){
          if((sscanf(line,"%s%s",utString,encString)!=2)
	     || (strcmp(utString,"EncodingScheme")!=0)) {
	    wxDebugMsg("AFM-file '%s': line '%s' has error (bad EncodingScheme)\n",
		       afmName,line);
          }
          else if (strncmp(encString, "AdobeStandardEncoding", 21))
          {
	    wxDebugMsg("AFM-file '%s': line '%s' has error (unsupported EncodingScheme %s)\n",
		       afmName,line, encString);
          }
        }
            // B.) check for char-width
        else if(strncmp(line,"C ",2)==0){
          if(sscanf(line,"%s%d%s%s%d",
              cString,&ascii,semiString,WXString,&cWidth)!=5){
             wxDebugMsg("AFM-file '%s': line '%s' has an error (bad character width)\n",afmName,line);
          }
          if(strcmp(cString,"C")!=0 || strcmp(semiString,";")!=0 ||
             strcmp(WXString,"WX")!=0){
             wxDebugMsg("AFM-file '%s': line '%s' has a format error\n",afmName,line);
          }
          //printf("            char '%c'=%d has width '%d'\n",ascii,ascii,cWidth);
          if(ascii>=0 && ascii<256){
            lastWidths[ascii] = cWidth; // store width
          }else{
	    /* MATTHEW: this happens a lot; don't print an error */
            // wxDebugMsg("AFM-file '%s': ASCII value %d out of range\n",afmName,ascii);
          }
        }
        // C.) ignore other entries.
      }
      fclose(afmFile);
    }
        // hack to compute correct values for german 'Umlaute'
        // the correct way would be to map the character names
        // like 'adieresis' to corresp. positions of ISOEnc and read
        // these values from AFM files, too. Maybe later ...
    lastWidths[196] = lastWidths['A'];  // Ä
    lastWidths[228] = lastWidths['a'];  // ä
    lastWidths[214] = lastWidths['O'];  // Ö
    lastWidths[246] = lastWidths['o'];  // ö
    lastWidths[220] = lastWidths['U'];  // Ü
    lastWidths[252] = lastWidths['u'];  // ü
    lastWidths[223] = lastWidths[251];  // ß
  }

      // JC: calculate UnderlineThickness/UnderlinePosition
  UnderlinePosition = UnderlinePosition * fontToUse->GetPointSize() / 1000.0f;
  UnderlineThickness = UnderlineThickness * fontToUse->GetPointSize() / 1000.0f * m_scaleFactor;

  // 3. now the font metrics are read in, calc size *******************
  // this is done by adding the widths of the characters in the
  // string. they are given in 1/1000 of the size!

  long widthSum=0;
  long height=Size; // by default
  unsigned char *p;
  for(p=(unsigned char *)(const char *)string; *p; p++){
    if(lastWidths[*p]== INT_MIN){
      wxDebugMsg("GetTextExtent: undefined width for character '%c' (%d)\n",
                 *p,*p);
      widthSum += (long)(lastWidths[' ']/1000.0F * Size); // assume space
    }else{
      widthSum += (long)((lastWidths[*p]/1000.0F)*Size);
    }
  }
  // add descender to height (it is usually a negative value)
  if(lastDescender!=INT_MIN){
    height += (long)(((-lastDescender)/1000.0F) * Size); /* MATTHEW: forgot scale */
  }
  
  // return size values
  *x = widthSum;
  *y = height;

  // return other parameters
  if (descent){
    if(lastDescender!=INT_MIN){
      *descent = (long)(((-lastDescender)/1000.0F) * Size); /* MATTHEW: forgot scale */
    }else{
      *descent = 0;
    }
  }

  // currently no idea how to calculate this!
  // if (externalLeading) *externalLeading = 0;
  if (externalLeading)
    *externalLeading = 0;

  // ----- end of contributed code -----
#endif
}

void wxPostScriptDC::DrawOpenSpline( wxList *points )
{
	double		a, b, c, d, x1, y1, x2, y2, x3, y3;
        wxPoint *p, *q;

        wxNode *node = points->First();
        p = (wxPoint *)node->Data();

	x1 = p->x; y1 = p->y;

        node = node->Next();
        p = (wxPoint *)node->Data();
	c = p->x; d = p->y;
        x3 = a = (double)(x1 + c) / 2;
        y3 = b = (double)(y1 + d) / 2;

        *(GetStream()) << "newpath " << x1 << " " << GetYOrigin() - y1 << " moveto " << x3 << " " << GetYOrigin() - y3;
        *(GetStream()) << " lineto\n";
        CalcBoundingBox( (long)x1, (long)(GetYOrigin() - y1));
        CalcBoundingBox( (long)x3, (long)(GetYOrigin() - y3));

        while ((node = node->Next()) != NULL)
	{
          q = (wxPoint *)node->Data();

	  x1 = x3; y1 = y3;
	  x2 = c;  y2 = d;
	  c = q->x; d = q->y;
          x3 = (double)(x2 + c) / 2;
          y3 = (double)(y2 + d) / 2;
          *(GetStream()) << x1 << " " << GetYOrigin() - y1 << " " << x2 << " " << GetYOrigin() - y2 << " ";
          *(GetStream()) << x3 << " " << GetYOrigin() - y3 << " DrawSplineSection\n";

          CalcBoundingBox( (long)x1, (long)(GetYOrigin() - y1));
          CalcBoundingBox( (long)x3, (long)(GetYOrigin() - y3));
        }
	/*
	* At this point, (x2,y2) and (c,d) are the position of the 
	* next-to-last and last point respectively, in the point list
	*/
        *(GetStream()) << c << " " << GetYOrigin() - d << " lineto stroke\n";
}

long wxPostScriptDC::GetCharWidth (void)
{
	// Chris Breeze: reasonable approximation using wxMODERN/Courier
	return (long) (GetCharHeight() * 72.0 / 120.0);
}


void wxPostScriptDC::SetMapMode (int mode)
{
  	m_mappingMode = mode;
	SetLogicalOrigin(0, 0);
	if(m_mappingMode == MM_METRIC)
	{
		SetUserScale(72.0f / 25.4f, 72.0f / 25.4f);
	}
	else
	{
		SetUserScale(1.0, 1.0);
	}
  return;
}

/*
 * Set the logical origin.
 * Actually we are setting the printer's origin and since
 * postscript transformations are cumulative we need to reset
 * the previous origin. We also need to allow for the user scale
 * factor (which affects printer coords but not logical)
 */
void wxPostScriptDC::SetLogicalOrigin(long x, long y)
{
	if (m_scaleFactor)
	{
		long xOffset = (long) ((m_logicalOriginX - x) / m_scaleFactor);
		long yOffset = (long) ((y - m_logicalOriginY) / m_scaleFactor);

		if (m_pstream)
		{
			*m_pstream << xOffset << " " << yOffset << " translate\n";
		}
	}
  	m_logicalOriginX = x;
  	m_logicalOriginY = y;
}

/*
 * Obsolete - now performed by SetUserScale() and SetLogicalOrigin()
 */
void
wxPostScriptDC::SetupCTM()
{
}

/*
 * Set the user scale.
 * NB: Postscript transformations are cumulative and so we
 * need to take into account the current scale. Also, this
 * affects the logical origin
 */
void wxPostScriptDC::SetUserScale (double x, double y)
{
	// reset logical origin
	long xOrg = m_logicalOriginX;
	long yOrg = m_logicalOriginY;
	SetLogicalOrigin(0, 0);

	// set new scale factor
	double factor = x;
	if (m_scaleFactor)
	{
		factor /= m_scaleFactor;
	}
	if (m_pstream)
	{
		*m_pstream << factor << " " << factor << " scale\n";
	}

	// set logical origin at new scale
	SetLogicalOrigin(xOrg, yOrg);

  	m_userScaleX = x;
  	m_userScaleY = y;

  	m_scaleFactor = m_userScaleX;
}

long wxPostScriptDC::DeviceToLogicalX (int x) const
{
  return  x;
}

long wxPostScriptDC::DeviceToLogicalXRel (int x) const
{
  return  x;
}

long wxPostScriptDC::DeviceToLogicalY (int y) const
{
  return  y;
}

long wxPostScriptDC::DeviceToLogicalYRel (int y) const
{
  return  y;
}

long wxPostScriptDC::LogicalToDeviceX (long x) const
{
  return x;
}

long wxPostScriptDC::LogicalToDeviceXRel (long x) const
{
  return x;
}

long wxPostScriptDC::LogicalToDeviceY (long y) const
{
  return y;
}

long wxPostScriptDC::LogicalToDeviceYRel (long y) const
{
  return y;
}

void wxPostScriptDC::GetSize(int* width, int* height) const
{
  const char *paperType = wxThePrintSetupData->GetPaperName();
  if (!paperType)
    paperType = _("A4 210 x 297 mm");

  wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(paperType);
  if (!paper)
    paper = wxThePrintPaperDatabase->FindPaperType(_("A4 210 x 297 mm"));
  if (paper)
  {
    *width = paper->widthPixels;
    *height = paper->heightPixels;
  }
  else
  {
    *width = 1000;
    *height = 1000;
  }
}

void wxPostScriptDC::GetSizeMM(long *width, long *height) const
{
  const char *paperType = wxThePrintSetupData->GetPaperName();
  if (!paperType)
    paperType = _("A4 210 x 297 mm");

  wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(paperType);
  if (!paper)
    paper = wxThePrintPaperDatabase->FindPaperType(_("A4 210 x 297 mm"));
  if (paper)
  {
    *width = paper->widthMM;
    *height = paper->heightMM;
  }
  else
  {
    *width = 1000;
    *height = 1000;
  }
}

void wxPostScriptDC::CalcBoundingBox(long x, long y)
{
    long device_x = (long) (x - m_logicalOriginX * m_scaleFactor);
    long device_y = (long) (y + m_logicalOriginY * m_scaleFactor);

    if (device_x < m_minX) m_minX = device_x;
    if (device_y < m_minY) m_minY = device_y;
    if (device_x > m_maxX) m_maxX = device_x;
    if (device_y > m_maxY) m_maxY = device_y;
}

IMPLEMENT_CLASS(wxPostScriptPrintDialog, wxDialog)

wxPostScriptPrintDialog::wxPostScriptPrintDialog (wxWindow *parent, const wxString& title,
		    const wxPoint& pos, const wxSize& size, long style):
wxDialog(parent, -1, title, pos, size, style)
{
  wxBeginBusyCursor();
  char buf[100];

  wxButton *okBut = new wxButton (this, wxID_OK, _("OK"), wxPoint(5, 5));
  (void) new wxButton (this, wxID_CANCEL, _("Cancel"), wxPoint(40, 5));
  okBut->SetDefault();

  int yPos = 40;

#if defined(__WXGTK__) || defined (__WXMOTIF__)
  (void) new wxStaticText( this, -1, _("Printer Command: "), 
                           wxPoint(5, yPos) );
  (void) new wxTextCtrl( this, wxID_PRINTER_COMMAND, wxThePrintSetupData->GetPrinterCommand(), 
                         wxPoint(100, yPos), wxSize(100, -1) );
  
  (void) new wxStaticText( this, -1, _("Printer Options: "), 
                           wxPoint(210, yPos) );
  (void) new wxTextCtrl( this, wxID_PRINTER_OPTIONS, wxThePrintSetupData->GetPrinterOptions(), 
                         wxPoint(305, yPos), wxSize(150, -1) );

  yPos += 40;
#endif

  wxString orientation[2];
  orientation[0] = _("Portrait");
  orientation[1] = _("Landscape");

  wxRadioBox *radio0 = new wxRadioBox(this, wxID_PRINTER_ORIENTATION, "Orientation: ", wxPoint(5, yPos), wxSize(-1,-1),
  		2,orientation,2,0);
  radio0->SetSelection((int)wxThePrintSetupData->GetPrinterOrientation() - 1);

  // @@@ Configuration hook
  if (wxThePrintSetupData->GetPrintPreviewCommand() == NULL)
    wxThePrintSetupData->SetPrintPreviewCommand(PS_VIEWER_PROG);

  wxGetResource ("wxWindows", "PSView", &wxThePrintSetupData->previewCommand);

  wxString print_modes[3];
  print_modes[0] = _("Send to Printer");
  print_modes[1] = _("Print to File");
  print_modes[2] = _("Preview Only");

  int features = (wxThePrintSetupData->GetPrintPreviewCommand() && *wxThePrintSetupData->GetPrintPreviewCommand()) ? 3 : 2;
  wxRadioBox *radio1 = new wxRadioBox(this, wxID_PRINTER_MODES, _("PostScript:"),
    wxPoint(150, yPos), wxSize(-1,-1), features, print_modes, features, 0);

#ifdef __WXMSW__
  radio1->Enable(0, FALSE);
  if (wxThePrintSetupData->GetPrintPreviewCommand() && *wxThePrintSetupData->GetPrintPreviewCommand())
    radio1->Enable(2, FALSE);
#endif

  radio1->SetSelection((int)wxThePrintSetupData->GetPrinterMode());
  
  long wx_printer_translate_x, wx_printer_translate_y;
  double wx_printer_scale_x, wx_printer_scale_y;
  wxThePrintSetupData->GetPrinterTranslation(&wx_printer_translate_x, &wx_printer_translate_y);
  wxThePrintSetupData->GetPrinterScaling(&wx_printer_scale_x, &wx_printer_scale_y);

  sprintf (buf, "%.2f", wx_printer_scale_x);

  yPos += 90;

  (void) new wxStaticText(this, -1, _("X Scaling"), wxPoint(5, yPos));
  /* wxTextCtrl *text1 = */ (void) new wxTextCtrl(this, wxID_PRINTER_X_SCALE, buf, wxPoint(100, yPos), wxSize(100, -1));

  sprintf (buf, "%.2f", wx_printer_scale_y);
  (void) new wxStaticText(this, -1, _("Y Scaling"), wxPoint(220, yPos));
  /* wxTextCtrl *text2 = */ (void) new wxTextCtrl(this, wxID_PRINTER_Y_SCALE, buf, wxPoint(320, yPos), wxSize(100, -1));

  yPos += 25;

  (void) new wxStaticText(this, -1, _("X Translation"), wxPoint(5, yPos));
  sprintf (buf, "%.2ld", wx_printer_translate_x);
  /* wxTextCtrl *text3 = */ (void) new wxTextCtrl(this, wxID_PRINTER_X_TRANS, buf, wxPoint(100, yPos), wxSize(100, -1));

  (void) new wxStaticText(this, -1, _("Y Translation"), wxPoint(220, yPos));
  sprintf (buf, "%.2ld", wx_printer_translate_y);
  /* wxTextCtrl *text4 = */ (void) new wxTextCtrl(this, wxID_PRINTER_Y_TRANS, buf, wxPoint(320, yPos), wxSize(100, -1));

  Fit ();

  wxEndBusyCursor();
}

int wxPostScriptPrintDialog::ShowModal (void)
{
  if ( wxDialog::ShowModal() == wxID_OK )
  {
//	  wxTextCtrl *text0 = (wxTextCtrl *)FindWindow(wxID_PRINTER_OPTIONS);
	  wxTextCtrl *text1 = (wxTextCtrl *)FindWindow(wxID_PRINTER_X_SCALE);
	  wxTextCtrl *text2 = (wxTextCtrl *)FindWindow(wxID_PRINTER_Y_SCALE);
	  wxTextCtrl *text3 = (wxTextCtrl *)FindWindow(wxID_PRINTER_X_TRANS);
	  wxTextCtrl *text4 = (wxTextCtrl *)FindWindow(wxID_PRINTER_Y_TRANS);
//	  wxTextCtrl *text_prt = (wxTextCtrl *)FindWindow(wxID_PRINTER_COMMAND);
	  wxRadioBox *radio0 = (wxRadioBox *)FindWindow(wxID_PRINTER_ORIENTATION);
	  wxRadioBox *radio1 = (wxRadioBox *)FindWindow(wxID_PRINTER_MODES);

      StringToDouble (WXSTRINGCAST text1->GetValue (), &wxThePrintSetupData->printerScaleX);
      StringToDouble (WXSTRINGCAST text2->GetValue (), &wxThePrintSetupData->printerScaleY);
      StringToLong (WXSTRINGCAST text3->GetValue (), &wxThePrintSetupData->printerTranslateX);
      StringToLong (WXSTRINGCAST text4->GetValue (), &wxThePrintSetupData->printerTranslateY);

#ifdef __X__
      wxThePrintSetupData->SetPrinterOptions(WXSTRINGCAST text0->GetValue ());
      wxThePrintSetupData->SetPrinterCommand(WXSTRINGCAST text_prt->GetValue ());
#endif

      wxThePrintSetupData->SetPrinterOrientation((radio0->GetSelection() == 1 ? PS_LANDSCAPE : PS_PORTRAIT));

      // C++ wants this
      switch ( radio1->GetSelection() ) {
		case 0: wxThePrintSetupData->SetPrinterMode(PS_PRINTER); break;
		case 1:  wxThePrintSetupData->SetPrinterMode(PS_FILE); break;
		case 2: wxThePrintSetupData->SetPrinterMode(PS_PREVIEW); break;
      }
	  return wxID_OK;
  }
  return wxID_CANCEL;
}

// PostScript printer settings
// RETAINED FOR BACKWARD COMPATIBILITY
void wxSetPrinterCommand(const char *cmd)
{
  wxThePrintSetupData->SetPrinterCommand(cmd);
}

void wxSetPrintPreviewCommand(const char *cmd)
{
  wxThePrintSetupData->SetPrintPreviewCommand(cmd);
}

void wxSetPrinterOptions(const char *flags)
{
  wxThePrintSetupData->SetPrinterOptions(flags);
}

void wxSetPrinterFile(const char *f)
{
  wxThePrintSetupData->SetPrinterFile(f);
}

void wxSetPrinterOrientation(int orient)
{
  wxThePrintSetupData->SetPrinterOrientation(orient);
}

void wxSetPrinterScaling(double x, double y)
{
  wxThePrintSetupData->SetPrinterScaling(x, y);
}

void wxSetPrinterTranslation(long x, long y)
{
  wxThePrintSetupData->SetPrinterTranslation(x, y);
}

// 1 = Preview, 2 = print to file, 3 = send to printer
void wxSetPrinterMode(int mode)
{
  wxThePrintSetupData->SetPrinterMode(mode);
}

void wxSetAFMPath(const char *f)
{
  wxThePrintSetupData->SetAFMPath(f);
}

// Get current values
char *wxGetPrinterCommand(void)
{
  return wxThePrintSetupData->GetPrinterCommand();
}

char *wxGetPrintPreviewCommand(void)
{
  return wxThePrintSetupData->GetPrintPreviewCommand();
}

char *wxGetPrinterOptions(void)
{
  return wxThePrintSetupData->GetPrinterOptions();
}

char *wxGetPrinterFile(void)
{
  return wxThePrintSetupData->GetPrinterFile();
}

int wxGetPrinterOrientation(void)
{
  return wxThePrintSetupData->GetPrinterOrientation();
}

void wxGetPrinterScaling(double* x, double* y)
{
  wxThePrintSetupData->GetPrinterScaling(x, y);
}

void wxGetPrinterTranslation(long *x, long *y)
{
  wxThePrintSetupData->GetPrinterTranslation(x, y);
}

int wxGetPrinterMode(void)
{
  return wxThePrintSetupData->GetPrinterMode();
}

char *wxGetAFMPath(void)
{
  return wxThePrintSetupData->GetAFMPath();
}

/*
 * Print setup data
 */

wxPrintSetupData::wxPrintSetupData(void)
{
  printerCommand = (char *) NULL;
  previewCommand = (char *) NULL;
  printerFlags = (char *) NULL;
  printerOrient = PS_PORTRAIT;
  printerScaleX = (double)1.0;
  printerScaleY = (double)1.0;
  printerTranslateX = 0;
  printerTranslateY = 0;
  // 1 = Preview, 2 = print to file, 3 = send to printer
  printerMode = 3;
  afmPath = (char *) NULL;
  paperName = (char *) NULL;
  printColour = TRUE;
  printerFile = (char *) NULL;
}

wxPrintSetupData::~wxPrintSetupData(void)
{
  if (printerCommand)
    delete[] printerCommand;
  if (previewCommand)
    delete[] previewCommand;
  if (printerFlags)
    delete[] printerFlags;
  if (afmPath)
    delete[] afmPath;
  if (paperName)
    delete[] paperName;
  if (printerFile)
    delete[] printerFile;
}

void wxPrintSetupData::SetPrinterCommand(const char *cmd)
{
  if (cmd == printerCommand)
    return;

  if (printerCommand)
    delete[] printerCommand;
  if (cmd)
    printerCommand = copystring(cmd);
  else
    printerCommand = (char *) NULL;
}

void wxPrintSetupData::SetPrintPreviewCommand(const char *cmd)
{
  if (cmd == previewCommand)
    return;
    
  if (previewCommand)
    delete[] previewCommand;
  if (cmd)
    previewCommand = copystring(cmd);
  else
    previewCommand = (char *) NULL;
}

void wxPrintSetupData::SetPaperName(const char *name)
{
  if (name == paperName)
    return;

  if (paperName)
    delete[] paperName;
  if (name)
    paperName = copystring(name);
  else
    paperName = (char *) NULL;
}

void wxPrintSetupData::SetPrinterOptions(const char *flags)
{
  if (printerFlags == flags)
    return;
   
  if (printerFlags)
    delete[] printerFlags;
  if (flags)
    printerFlags = copystring(flags);
  else
    printerFlags = (char *) NULL;
}

void wxPrintSetupData::SetPrinterFile(const char *f)
{
  if (f == printerFile)
    return;
    
  if (printerFile)
    delete[] printerFile;
  if (f)
    printerFile = copystring(f);
  else
    printerFile = (char *) NULL;
}

void wxPrintSetupData::SetPrinterOrientation(int orient)
{
  printerOrient = orient;
}

void wxPrintSetupData::SetPrinterScaling(double x, double y)
{
  printerScaleX = x;
  printerScaleY = y;
}

void wxPrintSetupData::SetPrinterTranslation(long x, long y)
{
  printerTranslateX = x;
  printerTranslateY = y;
}

// 1 = Preview, 2 = print to file, 3 = send to printer
void wxPrintSetupData::SetPrinterMode(int mode)
{
  printerMode = mode;
}

void wxPrintSetupData::SetAFMPath(const char *f)
{
  if (f == afmPath)
    return;
    
  if (afmPath)
    delete[] afmPath;
  if (f)
    afmPath = copystring(f);
  else
    afmPath = (char *) NULL;
}

void wxPrintSetupData::SetColour(bool col)
{
  printColour = col;
}

// Get current values
char *wxPrintSetupData::GetPrinterCommand(void)
{
  return printerCommand;
}

char *wxPrintSetupData::GetPrintPreviewCommand(void)
{
  return previewCommand;
}

char *wxPrintSetupData::GetPrinterOptions(void)
{
  return printerFlags;
}

char *wxPrintSetupData::GetPrinterFile(void)
{
  return printerFile;
}

char *wxPrintSetupData::GetPaperName(void)
{
  return paperName;
}

int wxPrintSetupData::GetPrinterOrientation(void)
{
  return printerOrient;
}

void wxPrintSetupData::GetPrinterScaling(double *x, double *y)
{
  *x = printerScaleX;
  *y = printerScaleY;
}

void wxPrintSetupData::GetPrinterTranslation(long *x, long *y)
{
  *x = printerTranslateX;
  *y = printerTranslateY;
}

int wxPrintSetupData::GetPrinterMode(void)
{
  return printerMode;
}

char *wxPrintSetupData::GetAFMPath(void)
{
  return afmPath;
}

bool wxPrintSetupData::GetColour(void)
{
  return printColour;
}

void wxPrintSetupData::operator=(wxPrintSetupData& data)
{
  SetPrinterCommand(data.GetPrinterCommand());
  SetPrintPreviewCommand(data.GetPrintPreviewCommand());
  SetPrinterOptions(data.GetPrinterOptions());
  long x, y;
  data.GetPrinterTranslation(&x, &y);
  SetPrinterTranslation(x, y);

  double x1, y1;
  data.GetPrinterScaling(&x1, &y1);
  SetPrinterScaling(x1, y1);

  SetPrinterOrientation(data.GetPrinterOrientation());
  SetPrinterMode(data.GetPrinterMode());
  SetAFMPath(data.GetAFMPath());
  SetPaperName(data.GetPaperName());
  SetColour(data.GetColour());
}

void wxInitializePrintSetupData(bool init)
{
  if (init)
  {
    wxThePrintSetupData = new wxPrintSetupData;

    wxThePrintSetupData->SetPrintPreviewCommand(PS_VIEWER_PROG);
    wxThePrintSetupData->SetPrinterOrientation(PS_PORTRAIT);
    wxThePrintSetupData->SetPrinterMode(PS_PREVIEW);
    wxThePrintSetupData->SetPaperName(_("A4 210 x 297 mm"));

    // Could have a .ini file to read in some defaults
    // - and/or use environment variables, e.g. WXWIN
#ifdef __VMS__
    wxThePrintSetupData->SetPrinterCommand("print");
    wxThePrintSetupData->SetPrinterOptions("/nonotify/queue=psqueue");
    wxThePrintSetupData->SetAFMPath("sys$ps_font_metrics:");
#endif
#ifdef __WXMSW__
    wxThePrintSetupData->SetPrinterCommand("print");
    wxThePrintSetupData->SetAFMPath("c:\\windows\\system\\");
    wxThePrintSetupData->SetPrinterOptions(NULL);
#endif
#if !defined(__VMS__) && !defined(__WXMSW__)
    wxThePrintSetupData->SetPrinterCommand("lpr");
    wxThePrintSetupData->SetPrinterOptions((char *) NULL);
    wxThePrintSetupData->SetAFMPath((char *) NULL);
#endif
  }
  else
  {
    if (wxThePrintSetupData)
      delete wxThePrintSetupData;
     wxThePrintSetupData = (wxPrintSetupData *) NULL;
  }
}

/*
 * Paper size database for PostScript
 */

wxPrintPaperType::wxPrintPaperType(const char *name, int wmm, int hmm, int wp, int hp)
{
  widthMM = wmm;
  heightMM = hmm;
  widthPixels = wp;
  heightPixels = hp;
  pageName = copystring(name);
}

wxPrintPaperType::~wxPrintPaperType(void)
{
  delete[] pageName;
}

wxPrintPaperDatabase::wxPrintPaperDatabase(void):wxList(wxKEY_STRING)
{
  DeleteContents(TRUE);
}

wxPrintPaperDatabase::~wxPrintPaperDatabase(void)
{
}

void wxPrintPaperDatabase::CreateDatabase(void)
{
  // Need correct values for page size in pixels.
  // Each unit is one 'point' = 1/72 of an inch.
  // NOTE: WE NEED ALSO TO MAKE ADJUSTMENTS WHEN TRANSLATING
  // in wxPostScriptDC code, so we can start from top left.
  // So access this database and translate by appropriate number
  // of points for this paper size. OR IS IT OK ALREADY?
  // Can't remember where the PostScript origin is by default.
  // Heck, someone will know how to make it hunky-dory...
  // JACS 25/5/95
  
  AddPaperType(_("A4 210 x 297 mm"), 210, 297,         595, 842);
  AddPaperType(_("A3 297 x 420 mm"), 297, 420,         842, 1191);
  AddPaperType(_("Letter 8 1/2 x 11 in"), 216, 279,    612, 791);
  AddPaperType(_("Legal 8 1/2 x 14 in"), 216, 356,     612, 1009);
}

void wxPrintPaperDatabase::ClearDatabase(void)
{
  Clear();
}

void wxPrintPaperDatabase::AddPaperType(const char *name, int wmm, int hmm, int wp, int hp)
{
  Append(name, new wxPrintPaperType(name, wmm, hmm, wp, hp));
}

wxPrintPaperType *wxPrintPaperDatabase::FindPaperType(const char *name)
{
  wxNode *node = Find(name);
  if (node)
    return (wxPrintPaperType *)node->Data();
  else
    return (wxPrintPaperType *) NULL;
}

#endif
