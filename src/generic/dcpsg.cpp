/////////////////////////////////////////////////////////////////////////////
// Name:        dcpsg.cpp
// Purpose:     Generic wxPostScriptDC implementation
// Author:      Julian Smart, Robert Roebling, Markus Holzhem
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/setup.h"
    #include "wx/defs.h"
#endif // WX_PRECOMP

#if wxUSE_PRINTING_ARCHITECTURE

#if wxUSE_POSTSCRIPT

#include "wx/window.h"
#include "wx/dcmemory.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/filedlg.h"
#include "wx/app.h"
#include "wx/msgdlg.h"
#include "wx/image.h"
#include "wx/log.h"
#include "wx/generic/dcpsg.h"
#include "wx/generic/prntdlgg.h"
#include "wx/button.h"
#include "wx/stattext.h"
#include "wx/radiobox.h"
#include "wx/textctrl.h"
#include "wx/prntbase.h"
#include "wx/paper.h"

#include <math.h>

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

//-----------------------------------------------------------------------------
// start and end of document/page
//-----------------------------------------------------------------------------

static const char *wxPostScriptHeaderEllipse = "\
/ellipsedict 8 dict def\n\
ellipsedict /mtrx matrix put\n\
/ellipse {\n\
    ellipsedict begin\n\
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

static const char *wxPostScriptHeaderSpline = "\
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

static const char *wxPostScriptHeaderColourImage = "\
%% define 'colorimage' if it isn't defined\n\
%%   ('colortogray' and 'mergeprocs' come from xwd2ps\n\
%%     via xgrab)\n\
/colorimage where   %% do we know about 'colorimage'?\n\
  { pop }           %% yes: pop off the 'dict' returned\n\
  {                 %% no:  define one\n\
    /colortogray {  %% define an RGB->I function\n\
      /rgbdata exch store    %% call input 'rgbdata'\n\
      rgbdata length 3 idiv\n\
      /npixls exch store\n\
      /rgbindx 0 store\n\
      0 1 npixls 1 sub {\n\
        grays exch\n\
        rgbdata rgbindx       get 20 mul    %% Red\n\
        rgbdata rgbindx 1 add get 32 mul    %% Green\n\
        rgbdata rgbindx 2 add get 12 mul    %% Blue\n\
        add add 64 idiv      %% I = .5G + .31R + .18B\n\
        put\n\
        /rgbindx rgbindx 3 add store\n\
      } for\n\
      grays 0 npixls getinterval\n\
    } bind def\n\
\n\
    %% Utility procedure for colorimage operator.\n\
    %% This procedure takes two procedures off the\n\
    %% stack and merges them into a single procedure.\n\
\n\
    /mergeprocs { %% def\n\
      dup length\n\
      3 -1 roll\n\
      dup\n\
      length\n\
      dup\n\
      5 1 roll\n\
      3 -1 roll\n\
      add\n\
      array cvx\n\
      dup\n\
      3 -1 roll\n\
      0 exch\n\
      putinterval\n\
      dup\n\
      4 2 roll\n\
      putinterval\n\
    } bind def\n\
\n\
    /colorimage { %% def\n\
      pop pop     %% remove 'false 3' operands\n\
      {colortogray} mergeprocs\n\
      image\n\
    } bind def\n\
  } ifelse          %% end of 'false' case\n\
";

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

//-------------------------------------------------------------------------------
// wxPostScriptDC
//-------------------------------------------------------------------------------

wxPostScriptDC::wxPostScriptDC ()
{
    m_pstream = (FILE*) NULL;

    m_currentRed = 0;
    m_currentGreen = 0;
    m_currentBlue = 0;

    m_pageNumber = 0;

    m_clipping = FALSE;

    m_underlinePosition = 0.0;
    m_underlineThickness = 0.0;

    m_signX =  1;  // default x-axis left to right
    m_signY = -1;  // default y-axis bottom up -> top down

    // Compatibility only
    // HH: Doesn't seem to work for wxMSW...
    #ifndef __WXMSW__
    m_printData = * wxThePrintSetupData;
    #endif
}

wxPostScriptDC::wxPostScriptDC (const wxString& file, bool interactive, wxWindow *parent)
{
    m_pstream = (FILE*) NULL;

    m_currentRed = 0;
    m_currentGreen = 0;
    m_currentBlue = 0;

    m_pageNumber = 0;

    m_clipping = FALSE;

    m_underlinePosition = 0.0;
    m_underlineThickness = 0.0;

    m_signX =  1;  // default x-axis left to right
    m_signY = -1;  // default y-axis bottom up -> top down

    Create(file, interactive, parent);
}

bool wxPostScriptDC::Create(const wxString& file, bool interactive, wxWindow *parent)
{
    m_isInteractive = interactive;

    m_title = "";
    m_printData.SetFilename(file);

#ifdef __WXMSW__
    // Can only send to file in Windows
    m_printData.SetPrintMode(wxPRINT_MODE_FILE);
#endif

    if (m_isInteractive)
    {
        if ((m_ok = PrinterDialog (parent) ) == FALSE) return FALSE;
    }
    else
    {
        m_ok = TRUE;
    }

    return m_ok;
}

wxPostScriptDC::wxPostScriptDC (const wxPrintData& printData)
{
    m_pstream = (FILE*) NULL;

    m_currentRed = 0;
    m_currentGreen = 0;
    m_currentBlue = 0;

    m_pageNumber = 0;

    m_clipping = FALSE;

    m_underlinePosition = 0.0;
    m_underlineThickness = 0.0;

    m_signX =  1;  // default x-axis left to right
    m_signY = -1;  // default y-axis bottom up -> top down

    m_printData = printData;

    m_ok = TRUE;
}

wxPostScriptDC::~wxPostScriptDC ()
{
    if (m_pstream)
    {
        fclose( m_pstream );
	m_pstream = (FILE*) NULL;
    }
}

bool wxPostScriptDC::Ok() const
{
  return m_ok;
}

// This dialog is deprecated now: use wxGenericPrintDialog or the printing framework
bool wxPostScriptDC::PrinterDialog(wxWindow *parent)
{
    wxPostScriptPrintDialog dialog( parent, _("Printer Settings"), wxPoint(150, 150), wxSize(400, 400),
                                    wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL );
    m_ok = (dialog.ShowModal () == wxID_OK);

    if (!m_ok) return FALSE;

    if ((m_printData.GetFilename() == "") &&
        (m_printData.GetPrintMode() == wxPRINT_MODE_PREVIEW  ||
         m_printData.GetPrintMode() == wxPRINT_MODE_PRINTER))
    {
// steve, 05.09.94
#ifdef __VMS__
      m_printData.SetFilename("preview");
#else
      // For PS_PRINTER action this depends on a Unix-style print spooler
      // since the wx_printer_file can be destroyed during a session
      // @@@ TODO: a Windows-style answer for non-Unix
      wxChar userId[256];
      wxGetUserId (userId, sizeof (userId) / sizeof (char));
      wxChar tmp[256];
      wxStrcpy (tmp, T("/tmp/preview_"));
      wxStrcat (tmp, userId);
      m_printData.SetFilename(tmp);
#endif
      wxChar tmp2[256];
      wxStrcpy(tmp2, m_printData.GetFilename());
      wxStrcat (tmp2, T(".ps"));
      m_printData.SetFilename(tmp2);
    }
    else if ((m_printData.GetFilename() == T("")) && (m_printData.GetPrintMode() == wxPRINT_MODE_FILE))
    {
      wxString file = wxSaveFileSelector (_("PostScript"), T("ps"));
      if ( file.IsEmpty() )
      {
        m_ok = FALSE;
        return FALSE;
      }

      m_printData.SetFilename(file);
      m_ok = TRUE;
    }

    return m_ok;
}

void wxPostScriptDC::SetClippingRegion (long x, long y, long w, long h)
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if (m_clipping) return;

    wxDC::SetClippingRegion( x, y, w, h );

    m_clipping = TRUE;
    fprintf( m_pstream, 
            "gsave\n newpath\n"
	    "%ld %ld moveto\n"
	    "%ld %ld lineto\n"
	    "%ld %ld lineto\n"
	    "%ld %ld lineto\n"
	    "closepath clip newpath\n",
	    XLOG2DEV(x),   YLOG2DEV(y), 
	    XLOG2DEV(x+w), YLOG2DEV(y), 
	    XLOG2DEV(x+w), YLOG2DEV(y+h),
	    XLOG2DEV(x),   YLOG2DEV(y+h) );
}

void wxPostScriptDC::SetClippingRegion( const wxRegion &WXUNUSED(region) )
{
    wxFAIL_MSG( T("wxPostScriptDC::SetClippingRegion not implemented.") );
}

void wxPostScriptDC::DestroyClippingRegion()
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    wxDC::DestroyClippingRegion();

    if (m_clipping)
    {
        m_clipping = FALSE;
	fprintf( m_pstream, "grestore\n" );
    }
}

void wxPostScriptDC::Clear()
{
    wxFAIL_MSG( T("wxPostScriptDC::Clear not implemented.") );
}

void wxPostScriptDC::DoFloodFill (long WXUNUSED(x), long WXUNUSED(y), const wxColour &WXUNUSED(col), int WXUNUSED(style))
{
    wxFAIL_MSG( T("wxPostScriptDC::FloodFill not implemented.") );
}

bool wxPostScriptDC::DoGetPixel (long WXUNUSED(x), long WXUNUSED(y), wxColour * WXUNUSED(col)) const
{
    wxFAIL_MSG( T("wxPostScriptDC::GetPixel not implemented.") );
    return FALSE;
}

void wxPostScriptDC::DoCrossHair (long WXUNUSED(x), long WXUNUSED(y))
{
    wxFAIL_MSG( T("wxPostScriptDC::CrossHair not implemented.") );
}

void wxPostScriptDC::DoDrawLine (long x1, long y1, long x2, long y2)
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if  (m_pen.GetStyle() == wxTRANSPARENT) return;

    SetPen( m_pen );

    fprintf( m_pstream,
            "newpath\n"
	    "%ld %ld moveto\n"
	    "%ld %ld lineto\n"
	    "stroke\n",
	    XLOG2DEV(x1), YLOG2DEV(y1),
	    XLOG2DEV(x2), YLOG2DEV (y2) );

    CalcBoundingBox( x1, y1 );
    CalcBoundingBox( x2, y2 );
}

#define RAD2DEG 57.29577951308

void wxPostScriptDC::DoDrawArc (long x1, long y1, long x2, long y2, long xc, long yc)
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    long dx = x1 - xc;
    long dy = y1 - yc;
    long radius = (long) sqrt(dx*dx+dy*dy);
    double alpha1, alpha2;

    if (x1 == x2 && y1 == y2)
    {
        alpha1 = 0.0;
        alpha2 = 360.0;
    } 
    else if (radius == 0.0)
    {
        alpha1 = alpha2 = 0.0;
    } 
    else
    {
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

    if (m_brush.GetStyle() != wxTRANSPARENT)
    {
        SetBrush( m_brush );
	
	fprintf( m_pstream,
	        "newpath\n"
		"%ld %ld %ld %ld %ld %ld ellipse\n"
		"%ld %ld lineto\n"
		"closepath\n"
		"fill\n",
		XLOG2DEV(xc), YLOG2DEV(yc), XLOG2DEVREL(radius), YLOG2DEVREL(radius), (long)alpha1, (long) alpha2,
		XLOG2DEV(xc), YLOG2DEV(yc) );

        CalcBoundingBox( xc-radius, yc-radius );
        CalcBoundingBox( xc+radius, yc+radius );
    }

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        SetPen( m_pen );
	
	fprintf( m_pstream,
	        "newpath\n"
		"%ld %ld %ld %ld %ld %ld ellipse\n"
		"%ld %ld lineto\n"
		"stroke\n"
		"fill\n",
		XLOG2DEV(xc), YLOG2DEV(yc), XLOG2DEVREL(radius), YLOG2DEVREL(radius), (long)alpha1, (long) alpha2,
		XLOG2DEV(xc), YLOG2DEV(yc) );

        CalcBoundingBox( xc-radius, yc-radius );
        CalcBoundingBox( xc+radius, yc+radius );
    }
}

void wxPostScriptDC::DoDrawEllipticArc(long x,long y,long w,long h,double sa,double ea)
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if (sa>=360 || sa<=-360) sa=sa-int(sa/360)*360;
    if (ea>=360 || ea<=-360) ea=ea-int(ea/360)*360;
    if (sa<0) sa+=360;
    if (ea<0) ea+=360;

    if (sa==ea)
    {
        DrawEllipse(x,y,w,h);
        return;
    }

    if (m_brush.GetStyle () != wxTRANSPARENT)
    {
        SetBrush( m_brush );
	
	fprintf( m_pstream,
	        "newpath\n"
		"%ld %ld %ld %ld %ld %ld true ellipticarc\n",
		XLOG2DEV(x+w/2), YLOG2DEV(y+h/2), XLOG2DEVREL(w/2), YLOG2DEVREL(h/2), (long)sa, (long)ea );
    
        CalcBoundingBox( x ,y );
        CalcBoundingBox( x+w, y+h );
    }	
    
    if (m_pen.GetStyle () != wxTRANSPARENT)
    {
        SetPen( m_pen );

	fprintf( m_pstream,
	        "newpath\n"
		"%ld %ld %ld %ld %ld %ld false ellipticarc\n",
		XLOG2DEV(x+w/2), YLOG2DEV(y+h/2), XLOG2DEVREL(w/2), YLOG2DEVREL(h/2), (long)sa, (long)ea );
    
        CalcBoundingBox( x ,y );
        CalcBoundingBox( x+w, y+h );
    }
}

void wxPostScriptDC::DoDrawPoint (long x, long y)
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if (m_pen.GetStyle() == wxTRANSPARENT) return;

    SetPen (m_pen);

    fprintf( m_pstream,
            "newpath\n"
	    "%ld %ld moveto\n"
	    "%ld %ld lineto\n"
	    "stroke\n",
	    XLOG2DEV(x),   YLOG2DEV(y),
	    XLOG2DEV(x+1), YLOG2DEV(y) );

    CalcBoundingBox( x, y );
}

void wxPostScriptDC::DoDrawPolygon (int n, wxPoint points[], long xoffset, long yoffset, int WXUNUSED(fillStyle))
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if (n <= 0) return;

    if (m_brush.GetStyle () != wxTRANSPARENT)
    {
        SetBrush( m_brush );

        fprintf( m_pstream, "newpath\n" );

        long xx = XLOG2DEV(points[0].x + xoffset);
        long yy = YLOG2DEV(points[0].y + yoffset);
	
	fprintf( m_pstream, "%ld %ld moveto\n", xx, yy );
		
        CalcBoundingBox( points[0].x + xoffset, points[0].y + yoffset );

        for (int i = 1; i < n; i++)
        {
            xx = XLOG2DEV(points[i].x + xoffset);
            yy = YLOG2DEV(points[i].y + yoffset);
	    
	    fprintf( m_pstream, "%ld %ld lineto\n", xx, yy );

            CalcBoundingBox( points[i].x + xoffset, points[i].y + yoffset);
        }
	
	fprintf( m_pstream, "fill\n" );
    }

    if (m_pen.GetStyle () != wxTRANSPARENT)
    {
        SetPen( m_pen );
	
        fprintf( m_pstream, "newpath\n" );

        long xx = XLOG2DEV(points[0].x + xoffset);
        long yy = YLOG2DEV(points[0].y + yoffset);
	
	fprintf( m_pstream, "%ld %ld moveto\n", xx, yy );
	
        CalcBoundingBox( points[0].x + xoffset, points[0].y + yoffset );

        for (int i = 1; i < n; i++)
        {
            xx = XLOG2DEV(points[i].x + xoffset);
            yy = YLOG2DEV(points[i].y + yoffset);
	    
	    fprintf( m_pstream, "%ld %ld lineto\n", xx, yy );
	    
            CalcBoundingBox( points[i].x + xoffset, points[i].y + yoffset);
        }

	fprintf( m_pstream, "fill\n" );
    }
}

void wxPostScriptDC::DoDrawLines (int n, wxPoint points[], long xoffset, long yoffset)
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if (m_pen.GetStyle() == wxTRANSPARENT) return;
	
    if (n <= 0) return;

    SetPen (m_pen);

    int i;
    for ( i =0; i<n ; i++ )
    {
        CalcBoundingBox( XLOG2DEV(points[i].x+xoffset), YLOG2DEV(points[i].y+yoffset));
    }

    fprintf( m_pstream,
            "newpath\n"
	    "%ld %ld moveto\n",
	    XLOG2DEV(points[0].x+xoffset), YLOG2DEV(points[0].y+yoffset) );

    for (i = 1; i < n; i++)
    {
        fprintf( m_pstream,
	        "%ld %ld lineto\n",
	        XLOG2DEV(points[i].x+xoffset), YLOG2DEV(points[i].y+yoffset) );
    }

    fprintf( m_pstream, "stroke\n" );
}

void wxPostScriptDC::DoDrawRectangle (long x, long y, long width, long height)
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if (m_brush.GetStyle () != wxTRANSPARENT)
    {
        SetBrush( m_brush );
	
	fprintf( m_pstream,
	        "newpath\n"
		"%ld %ld moveto\n"
		"%ld %ld lineto\n"
		"%ld %ld lineto\n"
		"%ld %ld lineto\n"
                "closepath\n"
                "fill\n",
		XLOG2DEV(x),         YLOG2DEV(y),
		XLOG2DEV(x + width), YLOG2DEV(y),
		XLOG2DEV(x + width), YLOG2DEV(y + height),
		XLOG2DEV(x),         YLOG2DEV(y + height) );

        CalcBoundingBox( x, y );
        CalcBoundingBox( x + width, y + height );
    }

    if (m_pen.GetStyle () != wxTRANSPARENT)
    {
        SetPen (m_pen);

	fprintf( m_pstream,
	        "newpath\n"
		"%ld %ld moveto\n"
		"%ld %ld lineto\n"
		"%ld %ld lineto\n"
		"%ld %ld lineto\n"
                "closepath\n"
                "stroke\n",
		XLOG2DEV(x),         YLOG2DEV(y),
		XLOG2DEV(x + width), YLOG2DEV(y),
		XLOG2DEV(x + width), YLOG2DEV(y + height),
		XLOG2DEV(x),         YLOG2DEV(y + height) );
		
        CalcBoundingBox( x, y );
        CalcBoundingBox( x + width, y + height );
    }
}

void wxPostScriptDC::DoDrawRoundedRectangle (long x, long y, long width, long height, double radius)
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

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

    long rad = (long) radius;

    if (m_brush.GetStyle () != wxTRANSPARENT)
    {
        SetBrush( m_brush );

        /* Draw rectangle anticlockwise */
	fprintf( m_pstream,
	        "newpath\n"
		"%ld %ld %ld 90 180 arc\n"
		"%ld %ld moveto\n"
		"%ld %ld %ld 180 270 arc\n"
		"%ld %ld lineto\n"
		"%ld %ld %ld 270 0 arc\n"
		"%ld %ld lineto\n"
		"%ld %ld %ld 0 90 arc\n"
		"%ld %ld lineto\n"
                "closepath\n"
                "fill\n",
		XLOG2DEV(x + rad), YLOG2DEV(y + rad), XLOG2DEVREL(rad),
		XLOG2DEV(x), YLOG2DEV(y + rad),
                XLOG2DEV(x + rad), YLOG2DEV(y + height - rad), XLOG2DEVREL(rad),
                XLOG2DEV(x + width - rad), YLOG2DEV(y + height),
		XLOG2DEV(x + width - rad), YLOG2DEV(y + height - rad), XLOG2DEVREL(rad),
		XLOG2DEV(x + width), YLOG2DEV(y + rad),
		XLOG2DEV(x + width - rad), YLOG2DEV(y + rad), XLOG2DEVREL(rad),
		XLOG2DEV(x + rad), YLOG2DEV(y) );

        CalcBoundingBox( x, y );
        CalcBoundingBox( x + width, y + height );
    }

    if (m_pen.GetStyle () != wxTRANSPARENT)
    {
        SetPen (m_pen);

        /* Draw rectangle anticlockwise */
	fprintf( m_pstream,
	        "newpath\n"
		"%ld %ld %ld 90 180 arc\n"
		"%ld %ld moveto\n"
		"%ld %ld %ld 180 270 arc\n"
		"%ld %ld lineto\n"
		"%ld %ld %ld 270 0 arc\n"
		"%ld %ld lineto\n"
		"%ld %ld %ld 0 90 arc\n"
		"%ld %ld lineto\n"
                "closepath\n"
                "stroke\n",
		XLOG2DEV(x + rad), YLOG2DEV(y + rad), XLOG2DEVREL(rad),
		XLOG2DEV(x), YLOG2DEV(y + rad),
                XLOG2DEV(x + rad), YLOG2DEV(y + height - rad), XLOG2DEVREL(rad),
                XLOG2DEV(x + width - rad), YLOG2DEV(y + height),
		XLOG2DEV(x + width - rad), YLOG2DEV(y + height - rad), XLOG2DEVREL(rad),
		XLOG2DEV(x + width), YLOG2DEV(y + rad),
		XLOG2DEV(x + width - rad), YLOG2DEV(y + rad), XLOG2DEVREL(rad),
		XLOG2DEV(x + rad), YLOG2DEV(y) );

        CalcBoundingBox( x, y );
        CalcBoundingBox( x + width, y + height );
    }
}

void wxPostScriptDC::DoDrawEllipse (long x, long y, long width, long height)
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if (m_brush.GetStyle () != wxTRANSPARENT)
    {
        SetBrush (m_brush);

        fprintf( m_pstream, 
	        "newpath\n"
		"%ld %ld %ld %ld 0 360 ellipse\n"
		"fill\n",
		XLOG2DEV(x + width / 2), YLOG2DEV(y + height / 2), 
		XLOG2DEV(width / 2), YLOG2DEVREL(height / 2) );

        CalcBoundingBox( x - width, y - height );
        CalcBoundingBox( x + width, y + height );
    }

    if (m_pen.GetStyle () != wxTRANSPARENT)
    {
        SetPen (m_pen);

        fprintf( m_pstream, 
	        "newpath\n"
		"%ld %ld %ld %ld 0 360 ellipse\n"
		"stroke\n",
		XLOG2DEV(x + width / 2), YLOG2DEV(y + height / 2), 
		XLOG2DEV(width / 2), YLOG2DEVREL(height / 2) );

        CalcBoundingBox( x - width, y - height );
        CalcBoundingBox( x + width, y + height );
    }
}

void wxPostScriptDC::DoDrawIcon( const wxIcon& icon, long x, long y )
{
    DrawBitmap( icon, x, y, TRUE );
}

/* this has to be char, not wxChar */
static char hexArray[] = "0123456789ABCDEF";
static void LocalDecToHex( int dec, char *buf )
{
    int firstDigit = (int)(dec/16.0);
    int secondDigit = (int)(dec - (firstDigit*16.0));
    buf[0] = hexArray[firstDigit];
    buf[1] = hexArray[secondDigit];
    buf[2] = 0;
}

void wxPostScriptDC::DoDrawBitmap( const wxBitmap& bitmap, long x, long y, bool WXUNUSED(useMask) )
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if (!bitmap.Ok()) return;

    wxImage image( bitmap );

    if (!image.Ok()) return;

    long ww = XLOG2DEVREL(image.GetWidth());
    long hh = YLOG2DEVREL(image.GetHeight());

    image = image.Scale( ww, hh );

    if (!image.Ok()) return;

    long xx = XLOG2DEV(x);
    long yy = YLOG2DEV(y + bitmap.GetHeight());

    fprintf( m_pstream,
            "/origstate save def\n"
	    "20 dict begin\n"
	    "/pix %ld string def\n"
	    "/grays %ld string def\n"
            "/npixels 0 def\n"
            "/rgbindx 0 def\n"
	    "%ld %ld translate\n"
	    "%ld %ld scale\n"
	    "%ld %ld 8\n"
	    "[%ld 0 0 %ld 0 %ld]\n"
	    "{currentfile pix readhexstring pop}\n"
	    "false 3 colorimage\n",
	    ww, ww, xx, yy, ww, hh, ww, hh, ww, -hh, hh );

    for (int j = 0; j < hh; j++)
    {
        for (int i = 0; i < ww; i++)
        {
            char buffer[5];
            LocalDecToHex( image.GetRed(i,j), buffer );
	    fprintf( m_pstream, buffer );
            LocalDecToHex( image.GetGreen(i,j), buffer );
	    fprintf( m_pstream, buffer );
	    LocalDecToHex( image.GetBlue(i,j), buffer );
	    fprintf( m_pstream, buffer );
        }
        fprintf( m_pstream, "\n" );
    }

    fprintf( m_pstream, "end\n" );
    fprintf( m_pstream, "origstate restore\n" );
}

void wxPostScriptDC::SetFont( const wxFont& font )
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if (!font.Ok())  return;

    m_font = font;

    const char *name;
    const char *style = "";
    int Style = m_font.GetStyle();
    int Weight = m_font.GetWeight();

    switch (m_font.GetFamily())
    {
        case wxTELETYPE:
        case wxMODERN:
            name = "/Courier";
            break;
        case wxSWISS:
            name = "/Helvetica";
            break;
        case wxROMAN:
//          name = "/Times-Roman";
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
    {
        style = "-Bold";
    }
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
    {
        style = "";
    }

    char buffer[100];
    strcpy( buffer, name );
    strcat( buffer, style );

    fprintf( m_pstream, buffer );
    fprintf( m_pstream, " reencodeISO def\n" );
    fprintf( m_pstream, buffer );
    fprintf( m_pstream, " findfont\n" );
    #ifdef __WXMSW__
    fprintf( m_pstream, "%d scalefont setfont\n", YLOG2DEVREL(m_font.GetPointSize()) );
    #else
    fprintf( m_pstream, "%ld scalefont setfont\n", YLOG2DEVREL(m_font.GetPointSize()) );
    #endif
}

void wxPostScriptDC::SetPen( const wxPen& pen )
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if (!pen.Ok()) return;

    int oldStyle = m_pen.GetStyle();

    m_pen = pen;

    #ifdef __WXMSW__
    fprintf( m_pstream, "%d setlinewidth\n", XLOG2DEVREL(m_pen.GetWidth()) );
    #else
    fprintf( m_pstream, "%ld setlinewidth\n", XLOG2DEVREL(m_pen.GetWidth()) );
    #endif
/*
     Line style - WRONG: 2nd arg is OFFSET

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
    switch (m_pen.GetStyle())
    {
        case wxDOT:           psdash = dotted;         break;
        case wxSHORT_DASH:    psdash = short_dashed;   break;
        case wxLONG_DASH:     psdash = long_dashed;    break;
        case wxDOT_DASH:      psdash = dotted_dashed;  break;
        case wxSOLID:
        case wxTRANSPARENT:
        default:              psdash = "[] 0";         break;
    }

    if (oldStyle != m_pen.GetStyle())
    {
        fprintf( m_pstream, psdash );
	fprintf( m_pstream," setdash\n" );
    }

    // Line colour
    unsigned char red = m_pen.GetColour().Red();
    unsigned char blue = m_pen.GetColour().Blue();
    unsigned char green = m_pen.GetColour().Green();

    if (!m_colour)
    {
        // Anything not white is black
        if (! (red == (unsigned char) 255 && 
	       blue == (unsigned char) 255 &&
               green == (unsigned char) 255) )
        {
            red = (unsigned char) 0;
            green = (unsigned char) 0;
            blue = (unsigned char) 0;
        } 
        // setgray here ?
    }

    if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue))
    {
        double redPS = (double)(red) / 255.0;
        double bluePS = (double)(blue) / 255.0;
        double greenPS = (double)(green) / 255.0;
	
	fprintf( m_pstream, 
	        "%.8f %.8f %.8f setrgbcolor\n",
		redPS, greenPS, bluePS );
		
        m_currentRed = red;
        m_currentBlue = blue;
        m_currentGreen = green;
    }
}

void wxPostScriptDC::SetBrush( const wxBrush& brush )
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if (!brush.Ok()) return;

    m_brush = brush;

    // Brush colour
    unsigned char red = m_brush.GetColour().Red();
    unsigned char blue = m_brush.GetColour().Blue();
    unsigned char green = m_brush.GetColour().Green();

    if (!m_colour)
    {
        // Anything not white is black
        if (! (red == (unsigned char) 255 && 
	       blue == (unsigned char) 255 &&
               green == (unsigned char) 255) )
        {
            red = (unsigned char) 0;
            green = (unsigned char) 0;
            blue = (unsigned char) 0;
        } 
        // setgray here ?
    }
    
    if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue))
    {
        double redPS = (double)(red) / 255.0;
        double bluePS = (double)(blue) / 255.0;
        double greenPS = (double)(green) / 255.0;
	
	fprintf( m_pstream, 
	        "%.8f %.8f %.8f setrgbcolor\n",
		redPS, greenPS, bluePS );
		
        m_currentRed = red;
        m_currentBlue = blue;
        m_currentGreen = green;
    }
}

void wxPostScriptDC::DoDrawText( const wxString& text, long x, long y )
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    SetFont( m_font );

    if (m_textForegroundColour.Ok())
    {
        unsigned char red = m_textForegroundColour.Red();
        unsigned char blue = m_textForegroundColour.Blue();
        unsigned char green = m_textForegroundColour.Green();

        if (!m_colour)
        {
            // Anything not white is black
            if (! (red == (unsigned char) 255 && 
	           blue == (unsigned char) 255 &&
		   green == (unsigned char) 255))
            {
                red = (unsigned char) 0;
                green = (unsigned char) 0;
                blue = (unsigned char) 0;
            }
        }

        // maybe setgray here ?
        if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue))
        {
            double redPS = (double)(red) / 255.0;
            double bluePS = (double)(blue) / 255.0;
            double greenPS = (double)(green) / 255.0;
	
	    fprintf( m_pstream, 
	            "%.8f %.8f %.8f setrgbcolor\n",
		    redPS, greenPS, bluePS );
		
            m_currentRed = red;
            m_currentBlue = blue;
            m_currentGreen = green;
        }
    }

    int size = m_font.GetPointSize();

    long by = y + (long)floor( double(size) * 2.0 / 3.0 ); // approximate baseline
    fprintf( m_pstream, "%ld %ld moveto\n", XLOG2DEV(x), YLOG2DEV(by) );

    /* I don't know how to write char to a stream, so I use a mini string */
    char tmpbuf[2];
    tmpbuf[1] = 0;
    
    fprintf( m_pstream, "(" );
    const wxWX2MBbuf textbuf = text.mb_str();
    int len = strlen(textbuf);
    int i;
    for (i = 0; i < len; i++)
    {
        int c = (unsigned char) textbuf[i];
        if (c == ')' || c == '(' || c == '\\')
        {
            /* Cope with special characters */
	    fprintf( m_pstream, "\\" );
	    tmpbuf[0] = (char) c;
	    fprintf( m_pstream, tmpbuf );
        }
        else if ( c >= 128 )
        {
            /* Cope with character codes > 127 */
	    fprintf(m_pstream, "\\%o", c);
        }
        else
	{
	    tmpbuf[0] = (char) c;
	    fprintf( m_pstream, tmpbuf );
	}
    }
    
    fprintf( m_pstream, ") show\n" );

    if (m_font.GetUnderlined())
    {
        long uy = (long)(y + size - m_underlinePosition);
        long w, h;
        GetTextExtent(text, &w, &h);

        fprintf( m_pstream, 
	        "gsave\n"
		"%ld %ld moveto\n"		
		"%ld setlinewidth\n"
		"%ld %ld lineto\n"
		"stroke\n"
		"grestore\n", 
                XLOG2DEV(x), YLOG2DEV(uy),
		(long)m_underlineThickness,
		XLOG2DEV(x + w), YLOG2DEV(uy) );
    }

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + size * text.Length() * 2/3 , y );
}


void wxPostScriptDC::SetBackground (const wxBrush& brush)
{
    m_backgroundBrush = brush;
}

void wxPostScriptDC::SetLogicalFunction (int WXUNUSED(function))
{
    wxFAIL_MSG( T("wxPostScriptDC::SetLogicalFunction not implemented.") );
}

void wxPostScriptDC::DoDrawSpline( wxList *points )
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    SetPen( m_pen );

    double a, b, c, d, x1, y1, x2, y2, x3, y3;
    wxPoint *p, *q;

    wxNode *node = points->First();
    p = (wxPoint *)node->Data();
    x1 = p->x;
    y1 = p->y;

    node = node->Next();
    p = (wxPoint *)node->Data();
    c = p->x;
    d = p->y;
    x3 = a = (double)(x1 + c) / 2;
    y3 = b = (double)(y1 + d) / 2;

    fprintf( m_pstream, 
            "newpath\n"
	    "%ld %ld moveto\n"
	    "%ld %ld lineto\n",
            XLOG2DEV((long)x1), YLOG2DEV((long)y1),
            XLOG2DEV((long)x3), YLOG2DEV((long)y3) );

    CalcBoundingBox( (long)x1, (long)y1 );
    CalcBoundingBox( (long)x3, (long)y3 );

    while ((node = node->Next()) != NULL)
    {
        q = (wxPoint *)node->Data();

        x1 = x3;
        y1 = y3;
        x2 = c;
        y2 = d;
        c = q->x;
        d = q->y;
        x3 = (double)(x2 + c) / 2;
        y3 = (double)(y2 + d) / 2;
	
	fprintf( m_pstream,
	        "%ld %ld %ld %ld %ld %ld DrawSplineSection\n",
		XLOG2DEV((long)x1), YLOG2DEV((long)y1),
		XLOG2DEV((long)x2), YLOG2DEV((long)y2),
		XLOG2DEV((long)x3), YLOG2DEV((long)y3) );

        CalcBoundingBox( (long)x1, (long)y1 );
        CalcBoundingBox( (long)x3, (long)y3 );
    }

/*
     At this point, (x2,y2) and (c,d) are the position of the
     next-to-last and last point respectively, in the point list
*/

    fprintf( m_pstream,
            "%ld %ld lineto\n"
	    "stroke\n",
	    XLOG2DEV((long)c), YLOG2DEV((long)d) );
}

long wxPostScriptDC::GetCharWidth() const
{
    // Chris Breeze: reasonable approximation using wxMODERN/Courier
    return (long) (GetCharHeight() * 72.0 / 120.0);
}


void wxPostScriptDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    m_signX = (xLeftRight ? 1 : -1);
    m_signY = (yBottomUp  ? 1 : -1);

    // FIXME there is no such function in MSW
#ifndef __WXMSW__
    ComputeScaleAndOrigin();
#endif
}

void wxPostScriptDC::SetDeviceOrigin( long x, long y )
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    int h = 0;
    int w = 0;
    GetSize( &w, &h );

    wxDC::SetDeviceOrigin( x, h-y );
}

void wxPostScriptDC::DoGetSize(int* width, int* height) const
{
    wxPaperSize id = m_printData.GetPaperId();

    wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(id);

    if (!paper) paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);

    int w = 595;
    int h = 842;
    if (paper)
    {
        w = paper->GetSizeDeviceUnits().x;
        h = paper->GetSizeDeviceUnits().y;
    }
    
    if (m_printData.GetOrientation() == wxLANDSCAPE)
    {
        int tmp = w;
	w = h;
	h = tmp;
    }
    
    if (width) *width = w;
    if (height) *height = h;
}

void wxPostScriptDC::DoGetSizeMM(int *width, int *height) const
{
    wxPaperSize id = m_printData.GetPaperId();

    wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(id);

    if (!paper) paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);

    int w = 210;
    int h = 297;
    if (paper)
    {
        w = paper->GetWidth() / 10;
        h = paper->GetHeight() / 10;
    }
    
    if (m_printData.GetOrientation() == wxLANDSCAPE)
    {
        int tmp = w;
	w = h;
	h = tmp;
    }
    
    if (width) *width = w;
    if (height) *height = h;
}

// Resolution in pixels per logical inch
wxSize wxPostScriptDC::GetPPI(void) const
{
    return wxSize(72, 72);
}


bool wxPostScriptDC::StartDoc( const wxString& message )
{
    wxCHECK_MSG( m_ok, FALSE, T("invalid postscript dc") );

    if (m_printData.GetFilename() == "")
    {
        wxString filename = wxGetTempFileName("ps");
        m_printData.SetFilename(filename);
    }
    
    m_pstream = fopen( m_printData.GetFilename().fn_str(), "w+" );

    if (!m_pstream)
    {
        wxMessageBox( _("Cannot open file for PostScript printing!"), _("Error"), wxOK );
        m_ok = FALSE;
        return FALSE;
    }

    m_ok = TRUE;

    fprintf( m_pstream, "%%%%BeginProlog\n" );
    fprintf( m_pstream, wxPostScriptHeaderEllipse );
    fprintf( m_pstream, wxPostScriptHeaderEllipticArc );
    fprintf( m_pstream, wxPostScriptHeaderColourImage );
    fprintf( m_pstream, wxPostScriptHeaderReencodeISO1 );
    fprintf( m_pstream, wxPostScriptHeaderReencodeISO2 );
    if (wxPostScriptHeaderSpline)
        fprintf( m_pstream, wxPostScriptHeaderSpline );
    fprintf( m_pstream, "%%%%EndProlog\n" );

    SetBrush( *wxBLACK_BRUSH );
    SetPen( *wxBLACK_PEN );
    SetBackground( *wxWHITE_BRUSH );
    SetTextForeground( *wxBLACK );

    // set origin according to paper size
    SetDeviceOrigin( 0,0 );

    wxPageNumber = 1;
    m_pageNumber = 1;
    m_title = message;
    return TRUE;
}

void wxPostScriptDC::EndDoc ()
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    if (m_clipping)
    {
        m_clipping = FALSE;
	fprintf( m_pstream, "grestore\n" );
    }

    fclose( m_pstream );
    m_pstream = (FILE *) NULL;

    wxChar *header_file = wxGetTempFileName("ps");

    m_pstream = fopen( wxConvFile.cWX2MB(header_file) , "w+" );

    fprintf( m_pstream, "%%!PS-Adobe-2.0\n" );                     // PostScript magic strings
    fprintf( m_pstream, "%%%%Title: %s\n", (const char *)m_title.mb_str() );
    fprintf( m_pstream, "%%%%Creator: %s\n", (const char*)wxConvCurrent->cWX2MB(wxTheApp->argv[0]) );
    fprintf( m_pstream, "%%%%CreationDate: %s\n", (const char *)wxNow().mb_str() );

    wxChar userID[256];
    if ( wxGetEmailAddress(userID, sizeof(userID)) )
    {
        fprintf( m_pstream, "%%%%For: %s ", wxMBSTRINGCAST wxConvCurrent->cWX2MB(userID) );
        wxChar userName[245];
        if (wxGetUserName(userName, sizeof(userName)))
            fprintf( m_pstream, " (%s)", wxMBSTRINGCAST wxConvCurrent->cWX2MB(userName) );
	fprintf( m_pstream, "\n" );
    }
    else if ( wxGetUserName(userID, sizeof(userID)) )
    {
        fprintf( m_pstream, "%%%%For: %s\n", wxMBSTRINGCAST wxConvCurrent->cWX2MB(userID) );;
    }

    // THE FOLLOWING HAS BEEN CONTRIBUTED BY Andy Fyfe <andy@hyperparallel.com>

    long wx_printer_translate_x, wx_printer_translate_y;
    double wx_printer_scale_x, wx_printer_scale_y;

    wx_printer_translate_x = m_printData.GetPrinterTranslateX();
    wx_printer_translate_y = m_printData.GetPrinterTranslateY();

    wx_printer_scale_x = m_printData.GetPrinterScaleX();
    wx_printer_scale_y = m_printData.GetPrinterScaleY();

    if (m_printData.GetOrientation() == wxLANDSCAPE)
        fprintf( m_pstream, "%%%%Orientation: Landscape\n" );
    else
        fprintf( m_pstream, "%%%%Orientation: Portrait\n" );

    // Compute the bounding box.  Note that it is in the default user
    // coordinate system, thus we have to convert the values.
    long llx = (long) ((XLOG2DEV(m_minX)+wx_printer_translate_x)*wx_printer_scale_x);
    long lly = (long) ((YLOG2DEV(m_minY)+wx_printer_translate_y)*wx_printer_scale_y);
    long urx = (long) ((XLOG2DEV(m_maxX)+wx_printer_translate_x)*wx_printer_scale_x);
    long ury = (long) ((YLOG2DEV(m_maxY)+wx_printer_translate_y)*wx_printer_scale_y);

    // If we're landscape, our sense of "x" and "y" is reversed.
    if (m_printData.GetOrientation() == wxLANDSCAPE)
    {
        long tmp;
        tmp = llx; llx = lly; lly = tmp;
        tmp = urx; urx = ury; ury = tmp;

        // We need either the two lines that follow, or we need to subtract
        // min_x from real_translate_y, which is commented out below.
        llx = llx - (long)(m_minX*wx_printer_scale_y);
        urx = urx - (long)(m_minX*wx_printer_scale_y);
    }

    // The Adobe specifications call for integers; we round as to make
    // the bounding larger.
    fprintf( m_pstream, 
            "%%%%BoundingBox: %ld %ld %ld %ld\n",
	    (long)floor((double)llx), (long)floor((double)lly),
	    (long)ceil((double)urx), (long)ceil((double)ury) );
    fprintf( m_pstream, "%%%%Pages: %d\n", (wxPageNumber - 1) );
    fprintf( m_pstream, "%%%%EndComments\n\n" );

    // To check the correctness of the bounding box, postscript commands
    // to draw a box corresponding to the bounding box are generated below.
    // But since we typically don't want to print such a box, the postscript
    // commands are generated within comments.  These lines appear before any
    // adjustment of scale, rotation, or translation, and hence are in the
    // default user coordinates.
    fprintf( m_pstream, "%% newpath\n" );
    fprintf( m_pstream, "%% %ld %ld moveto\n", llx, lly );
    fprintf( m_pstream, "%% %ld %ld lineto\n", urx, lly );
    fprintf( m_pstream, "%% %ld %ld lineto\n", urx, ury );
    fprintf( m_pstream, "%% %ld %ld lineto closepath stroke\n", llx, ury );

    fclose( m_pstream );
    m_pstream = (FILE*) NULL;

    wxChar *tmp_file = wxGetTempFileName("ps");

    // Paste header Before wx_printer_file
    wxConcatFiles (header_file, m_printData.GetFilename(), tmp_file );
    wxRemoveFile( header_file );
    wxRemoveFile( m_printData.GetFilename() );
    wxRenameFile( tmp_file, m_printData.GetFilename() );

#if defined(__X__) || defined(__WXGTK__)
    if (m_ok)
    {
        wxString previewCommand(m_printData.GetPreviewCommand());
        wxString printerCommand(m_printData.GetPrinterCommand());
        wxString printerOptions(m_printData.GetPrinterOptions());
        wxString filename(m_printData.GetFilename());

        switch (m_printData.GetPrintMode()) 
	{
            case wxPRINT_MODE_PREVIEW:
            {
                wxChar *argv[3];
                argv[0] = WXSTRINGCAST previewCommand;
                argv[1] = WXSTRINGCAST filename;
                argv[2] = (wxChar*) NULL;
                wxExecute( argv, TRUE );
                wxRemoveFile( m_printData.GetFilename() );
            }
            break;
            case wxPRINT_MODE_PRINTER:
            {
                wxChar *argv[4];
                int argc = 0;
                argv[argc++] = WXSTRINGCAST printerCommand;

                // !SM! If we simply assign to argv[1] here, if printer options
                // are blank, we get an annoying and confusing message from lpr.
                wxChar *opts = WXSTRINGCAST printerOptions;
                if (opts && *opts)
                    argv[argc++] = opts;

                argv[argc++] = WXSTRINGCAST filename;
                argv[argc++] = (wxChar *) NULL;
                wxExecute( argv, TRUE );
                wxRemoveFile( filename );
            }
            break;
            case wxPRINT_MODE_FILE:
            case wxPRINT_MODE_NONE:
            break;
        }
    }
#endif
}

void wxPostScriptDC::StartPage()
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    fprintf( m_pstream, "%%%%Page: %d\n", wxPageNumber++ );

    //  What is this one supposed to do? RR.
//  *m_pstream << "matrix currentmatrix\n";

    // Added by Chris Breeze

    // Each page starts with an "initgraphics" which resets the
    // transformation and so we need to reset the origin
    // (and rotate the page for landscape printing)

    // Output scaling
    long translate_x, translate_y;
    double scale_x, scale_y;

    translate_x = m_printData.GetPrinterTranslateX();
    translate_y = m_printData.GetPrinterTranslateY();

    scale_x = m_printData.GetPrinterScaleX();
    scale_y = m_printData.GetPrinterScaleY();

    if (m_printData.GetOrientation() == wxLANDSCAPE)
    {
        int h;
        GetSize( (int*) NULL, &h );
        translate_y -= h;
        fprintf( m_pstream, "90 rotate\n" );
	
        // I copied this one from a PostScript tutorial, but to no avail. RR.
//	fprintf( m_pstream, "90 rotate llx neg ury nef translate\n" );
    }

    fprintf( m_pstream, "%.8f %.8f scale\n", scale_x, scale_y );
    fprintf( m_pstream, "%ld %ld translate\n", translate_x, translate_y );
}

void wxPostScriptDC::EndPage ()
{
    wxCHECK_RET( m_ok && m_pstream, T("invalid postscript dc") );

    fprintf( m_pstream, "showpage\n" );
}

bool wxPostScriptDC::DoBlit( long xdest, long ydest,
                           long fwidth, long fheight,
                           wxDC *source,
                           long xsrc, long ysrc,
                           int rop, bool WXUNUSED(useMask) )
{
    wxCHECK_MSG( m_ok && m_pstream, FALSE, T("invalid postscript dc") );

    wxCHECK_MSG( source, FALSE, T("invalid source dc") );

    /* blit into a bitmap */
    wxBitmap bitmap( (int)fwidth, (int)fheight );
    wxMemoryDC memDC;
    memDC.SelectObject(bitmap);
    memDC.Blit(0, 0, fwidth, fheight, source, xsrc, ysrc, rop); /* TODO: Blit transparently? */
    memDC.SelectObject(wxNullBitmap);

    /* draw bitmap. scaling and positioning is done there */
    DrawBitmap( bitmap, xdest, ydest );

    return TRUE;
}

long wxPostScriptDC::GetCharHeight() const
{
    if (m_font.Ok())
        return  m_font.GetPointSize();
    else
        return 12;
}

void wxPostScriptDC::GetTextExtent( const wxString& string, long *x, long *y,
                                    long *descent, long *externalLeading, wxFont *theFont ) const
{
    wxFont *fontToUse = theFont;

    if (!fontToUse) fontToUse = (wxFont*) &m_font;

    wxCHECK_RET( fontToUse, T("GetTextExtent: no font defined") );
    wxCHECK_RET( x, T("GetTextExtent: x == NULL") );
    wxCHECK_RET( y, T("GetTextExtent: y == NULL") );

    const wxWX2MBbuf strbuf = string.mb_str();

#if !wxUSE_AFM_FOR_POSTSCRIPT
    /* Provide a VERY rough estimate (avoid using it).
     * Produces accurate results for mono-spaced font
     * such as Courier (aka wxMODERN) */

    int height = 12;
    if (fontToUse)
    {
        height = fontToUse->GetPointSize();
    }
    *x = strlen (strbuf) * height * 72 / 120;
    *y = (long) (height * 1.32);    /* allow for descender */
    if (descent) *descent = 0;
    if (externalLeading) *externalLeading = 0;
#else

    /* method for calculating string widths in postscript:
    /  read in the AFM (adobe font metrics) file for the
    /  actual font, parse it and extract the character widths
    /  and also the descender. this may be improved, but for now
    /  it works well. the AFM file is only read in if the
    /  font is changed. this may be chached in the future.
    /  calls to GetTextExtent with the font unchanged are rather
    /  efficient!!!
    /
    /  for each font and style used there is an AFM file necessary.
    /  currently i have only files for the roman font family.
    /  I try to get files for the other ones!
    /
    /  CAVE: the size of the string is currently always calculated
    /        in 'points' (1/72 of an inch). this should later on be
    /        changed to depend on the mapping mode.
    /  CAVE: the path to the AFM files must be set before calling this
    /        function. this is usually done by a call like the following:
    /        wxSetAFMPath("d:\\wxw161\\afm\\");
    /
    /  example:
    /
    /    wxPostScriptDC dc(NULL, TRUE);
    /    if (dc.Ok()){
    /      wxSetAFMPath("d:\\wxw161\\afm\\");
    /      dc.StartDoc("Test");
    /      dc.StartPage();
    /      long w,h;
    /      dc.SetFont(new wxFont(10, wxROMAN, wxNORMAL, wxNORMAL));
    /      dc.GetTextExtent("Hallo",&w,&h);
    /      dc.EndPage();
    /      dc.EndDoc();
    /    }
    /
    /  by steve (stefan.hammes@urz.uni-heidelberg.de)
    /  created: 10.09.94
    /  updated: 14.05.95 */

    /* these static vars are for storing the state between calls */
    static int lastFamily= INT_MIN;
    static int lastSize= INT_MIN;
    static int lastStyle= INT_MIN;
    static int lastWeight= INT_MIN;
    static int lastDescender = INT_MIN;
    static int lastWidths[256]; /* widths of the characters */
    
    double UnderlinePosition = 0.0;
    double UnderlineThickness = 0.0;

    /* get actual parameters */
    const int Family = fontToUse->GetFamily();
    const int Size =   fontToUse->GetPointSize();
    const int Style =  fontToUse->GetStyle();
    const int Weight = fontToUse->GetWeight();

    /* if we have another font, read the font-metrics */
    if (Family!=lastFamily || Size!=lastSize || Style!=lastStyle || Weight!=lastWeight)
    {
        /* store actual values */
        lastFamily = Family;
        lastSize =   Size;
        lastStyle =  Style;
        lastWeight = Weight;

        char *name = (char*) NULL;

        switch (Family)
        {
            case wxMODERN:
                {
                    if ((Style == wxITALIC) && (Weight == wxBOLD)) name = "CourBoO";
                    else if ((Style != wxITALIC) && (Weight == wxBOLD)) name = "CourBo";
                    else if ((Style == wxITALIC) && (Weight != wxBOLD)) name = "Cour0";
                    else name = "Cour";
                }
                break;
            case wxROMAN:
                {
                    if ((Style == wxITALIC) && (Weight == wxBOLD)) name = "TimesBoO";
                    else if ((Style != wxITALIC) && (Weight == wxBOLD)) name = "TimesBo";
                    else if ((Style == wxITALIC) && (Weight != wxBOLD)) name = "TimesO";
                    else name = "TimesRo"; 
                }
                break;
            default:
                {
                    if ((Style == wxITALIC) && (Weight == wxBOLD)) name = "HelvBoO";
                    else if ((Style != wxITALIC) && (Weight == wxBOLD)) name = "HelvBo";
                    else if ((Style == wxITALIC) && (Weight != wxBOLD)) name = "Helv0";
                    else name = "Helv";
                }
                break;
        }

        /* get the directory of the AFM files */
        char afmName[256];
        afmName[0] = 0;
	
	if (!m_printData.GetFontMetricPath().IsEmpty())
	{
	    strcpy( afmName, m_printData.GetFontMetricPath().fn_str() );
	}

        /* 2. open and process the file
           /  a short explanation of the AFM format:
           /  we have for each character a line, which gives its size
           /  e.g.:
           /
           /    C 63 ; WX 444 ; N question ; B 49 -14 395 676 ;
           /
           /  that means, we have a character with ascii code 63, and width
           /  (444/1000 * fontSize) points.
           /  the other data is ignored for now!
           /
           /  when the font has changed, we read in the right AFM file and store the
           /  character widths in an array, which is processed below (see point 3.). */

        /* new elements JC Sun Aug 25 23:21:44 MET DST 1996 */

        strcat(afmName,name);
        strcat(afmName,".afm");
        FILE *afmFile = fopen(afmName,"r");
	
#ifdef __UNIX__
	     if (afmFile==NULL)
	     {
	         strcpy( afmName, "/usr/local/share/wx/afm/" );
             strcat(afmName,name);
             strcat(afmName,".afm");
             afmFile = fopen(afmName,"r");
	     }

	     if (afmFile==NULL)
	     {
	         strcpy( afmName, "/usr/share/wx/afm/" );
             strcat(afmName,name);
             strcat(afmName,".afm");
             afmFile = fopen(afmName,"r");
	     }
#endif
	
        if (afmFile==NULL)
        {
            wxLogDebug( "GetTextExtent: can't open AFM file '%s'\n", afmName );
            wxLogDebug( "               using approximate values\n");
            for (int i=0; i<256; i++) lastWidths[i] = 500; /* an approximate value */
            lastDescender = -150; /* dito. */
        }
        else
        {
            /* init the widths array */
            for(int i=0; i<256; i++) lastWidths[i] = INT_MIN;
            /* some variables for holding parts of a line */
            char cString[10],semiString[10],WXString[10],descString[20];
            char upString[30], utString[30], encString[50];
            char line[256];
            int ascii,cWidth;
            /* read in the file and parse it */
            while(fgets(line,sizeof(line),afmFile)!=NULL)
            {
                /* A.) check for descender definition */
                if (strncmp(line,"Descender",9)==0)
                {
                    if ((sscanf(line,"%s%d",descString,&lastDescender)!=2) ||
                            (strcmp(descString,"Descender")!=0))
                    {
                        wxLogDebug( "AFM-file '%s': line '%s' has error (bad descender)\n", afmName,line );
                    }
                }
                /* JC 1.) check for UnderlinePosition */
                else if(strncmp(line,"UnderlinePosition",17)==0)
                {
                    if ((sscanf(line,"%s%lf",upString,&UnderlinePosition)!=2) ||
                            (strcmp(upString,"UnderlinePosition")!=0))
                    {
                        wxLogDebug( "AFM-file '%s': line '%s' has error (bad UnderlinePosition)\n", afmName, line );
                    }
                }
                /* JC 2.) check for UnderlineThickness */
                else if(strncmp(line,"UnderlineThickness",18)==0)
                {
                    if ((sscanf(line,"%s%lf",utString,&UnderlineThickness)!=2) ||
                            (strcmp(utString,"UnderlineThickness")!=0))
                    {
                        wxLogDebug( "AFM-file '%s': line '%s' has error (bad UnderlineThickness)\n", afmName, line );
                    }
                }
                /* JC 3.) check for EncodingScheme */
                else if(strncmp(line,"EncodingScheme",14)==0)
                {
                    if ((sscanf(line,"%s%s",utString,encString)!=2) ||
                            (strcmp(utString,"EncodingScheme")!=0))
                    {
                        wxLogDebug("AFM-file '%s': line '%s' has error (bad EncodingScheme)\n", afmName, line );
                    }
                    else if (strncmp(encString, "AdobeStandardEncoding", 21))
                    {
                        wxLogDebug( "AFM-file '%s': line '%s' has error (unsupported EncodingScheme %s)\n",
                                afmName,line, encString);
                    }
                }
                /* B.) check for char-width */
                else if(strncmp(line,"C ",2)==0)
                {
                    if (sscanf(line,"%s%d%s%s%d",cString,&ascii,semiString,WXString,&cWidth)!=5)
                    {
                        wxLogDebug("AFM-file '%s': line '%s' has an error (bad character width)\n",afmName,line);
                    }
                    if(strcmp(cString,"C")!=0 || strcmp(semiString,";")!=0 || strcmp(WXString,"WX")!=0)
                    {
                        wxLogDebug("AFM-file '%s': line '%s' has a format error\n",afmName,line);
                    }
                    /* printf("            char '%c'=%d has width '%d'\n",ascii,ascii,cWidth); */
                    if (ascii>=0 && ascii<256)
                    {
                        lastWidths[ascii] = cWidth; /* store width */
                    }
                    else
                    {
                        /* MATTHEW: this happens a lot; don't print an error */
                        /* wxLogDebug("AFM-file '%s': ASCII value %d out of range\n",afmName,ascii); */
                    }
                }
                /* C.) ignore other entries. */
            }
            fclose(afmFile);
        }
        /* hack to compute correct values for german 'Umlaute'
           /  the correct way would be to map the character names
           /  like 'adieresis' to corresp. positions of ISOEnc and read
           /  these values from AFM files, too. Maybe later ... */
        lastWidths[196] = lastWidths['A'];  // Ä
        lastWidths[228] = lastWidths['a'];  // ä
        lastWidths[214] = lastWidths['O'];  // Ö
        lastWidths[246] = lastWidths['o'];  // ö
        lastWidths[220] = lastWidths['U'];  // Ü
        lastWidths[252] = lastWidths['u'];  // ü
        lastWidths[223] = lastWidths[251];  // ß
    }

    /* JC: calculate UnderlineThickness/UnderlinePosition */
    {
        // VS: dirty, but is there any better solution?
        double *pt;
        pt = (double*) &m_underlinePosition;
        *pt = UnderlinePosition * fontToUse->GetPointSize() / 1000.0f;
        pt = (double*) &m_underlineThickness;
        *pt = UnderlineThickness * fontToUse->GetPointSize() / 1000.0f * m_scaleFactor;
    }

    /* 3. now the font metrics are read in, calc size this
       /  is done by adding the widths of the characters in the
       /  string. they are given in 1/1000 of the size! */

    double widthSum=0;
    long height=Size; /* by default */
    unsigned char *p;
    for(p=(unsigned char *)strbuf; *p; p++)
    {
        if(lastWidths[*p]== INT_MIN)
        {
            wxLogDebug("GetTextExtent: undefined width for character '%c' (%d)\n", *p,*p);
            widthSum += /*(long)*/(lastWidths[' ']/1000.0F * Size); /* assume space */
        }
        else
        {
            widthSum += /*(long)*/((lastWidths[*p]/1000.0F)*Size);
        }
    }

    /* add descender to height (it is usually a negative value) */
    if (lastDescender != INT_MIN)
    {
        height += (long)(((-lastDescender)/1000.0F) * Size); /* MATTHEW: forgot scale */
    }

    /* return size values */
    *x = (long)widthSum;
    *y = height;

    /* return other parameters */
    if (descent)
    {
        if(lastDescender!=INT_MIN)
        {
            *descent = (long)(((-lastDescender)/1000.0F) * Size); /* MATTHEW: forgot scale */
        }
        else
        {
            *descent = 0;
        }
    }

    /* currently no idea how to calculate this! */
    if (externalLeading) *externalLeading = 0;

#endif
}

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
# define PS_VIEWER_PROG "gsview"
#endif

wxPrintSetupData *wxThePrintSetupData = (wxPrintSetupData *) NULL;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxPostScriptDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxPrintSetupData, wxObject)
#endif

// Redundant now I think
#if 1
IMPLEMENT_CLASS(wxPostScriptPrintDialog, wxDialog)

wxPostScriptPrintDialog::wxPostScriptPrintDialog (wxWindow *parent, const wxString& title,
            const wxPoint& pos, const wxSize& size, long style):
   wxDialog(parent, -1, title, pos, size, style)
{
   wxBeginBusyCursor();

   char buf[100];
   int yPos = 40;
   wxString
      *orientation = new wxString[2],
      *print_modes = new wxString[3];
   int features;
   long wx_printer_translate_x, wx_printer_translate_y;
   double wx_printer_scale_x, wx_printer_scale_y;

   orientation[0] = _("Portrait");
   orientation[1] = _("Landscape");

   print_modes[0] = _("Send to Printer");
   print_modes[1] = _("Print to File");
   print_modes[2] = _("Preview Only");



   wxButton *okBut = new wxButton (this, wxID_OK, _("OK"), wxPoint(5, 5));
   (void) new wxButton (this, wxID_CANCEL, _("Cancel"), wxPoint(40, 5));
   okBut->SetDefault();


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


   wxRadioBox *radio0 = new wxRadioBox(this, wxID_PRINTER_ORIENTATION, "Orientation: ", wxPoint(5, yPos), wxSize(-1,-1),
                                       2,orientation,2,wxRA_SPECIFY_ROWS);
   radio0->SetSelection((int)wxThePrintSetupData->GetPrinterOrientation() - 1);

  // @@@ Configuration hook
   if ( !wxThePrintSetupData->GetPrintPreviewCommand() )
      wxThePrintSetupData->SetPrintPreviewCommand(PS_VIEWER_PROG);

   //   wxGetResource ("wxWindows", "PSView", &wxThePrintSetupData->m_previewCommand);

   features = (wxThePrintSetupData->GetPrintPreviewCommand() &&
               *wxThePrintSetupData->GetPrintPreviewCommand()) ? 3 : 2;

   wxRadioBox *radio1 = new wxRadioBox(this, wxID_PRINTER_MODES, _("PostScript:"),
                                       wxPoint(150, yPos),
                                       wxSize(-1,-1), features,
                                       print_modes, features, wxRA_SPECIFY_ROWS);

#ifdef __WXMSW__
   radio1->Enable(0, FALSE);
   if (wxThePrintSetupData->GetPrintPreviewCommand() && *wxThePrintSetupData->GetPrintPreviewCommand())
      radio1->Enable(2, FALSE);
#endif

   radio1->SetSelection((int)wxThePrintSetupData->GetPrinterMode());
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

   delete[] orientation;
   delete[] print_modes;

   wxEndBusyCursor();
}

int wxPostScriptPrintDialog::ShowModal ()
{
  if ( wxDialog::ShowModal() == wxID_OK )
  {
//    wxTextCtrl *text0 = (wxTextCtrl *)FindWindow(wxID_PRINTER_OPTIONS);
      wxTextCtrl *text1 = (wxTextCtrl *)FindWindow(wxID_PRINTER_X_SCALE);
      wxTextCtrl *text2 = (wxTextCtrl *)FindWindow(wxID_PRINTER_Y_SCALE);
      wxTextCtrl *text3 = (wxTextCtrl *)FindWindow(wxID_PRINTER_X_TRANS);
      wxTextCtrl *text4 = (wxTextCtrl *)FindWindow(wxID_PRINTER_Y_TRANS);
//    wxTextCtrl *text_prt = (wxTextCtrl *)FindWindow(wxID_PRINTER_COMMAND);
      wxRadioBox *radio0 = (wxRadioBox *)FindWindow(wxID_PRINTER_ORIENTATION);
      wxRadioBox *radio1 = (wxRadioBox *)FindWindow(wxID_PRINTER_MODES);

      StringToDouble (WXSTRINGCAST text1->GetValue (), &wxThePrintSetupData->m_printerScaleX);
      StringToDouble (WXSTRINGCAST text2->GetValue (), &wxThePrintSetupData->m_printerScaleY);
      StringToLong (WXSTRINGCAST text3->GetValue (), &wxThePrintSetupData->m_printerTranslateX);
      StringToLong (WXSTRINGCAST text4->GetValue (), &wxThePrintSetupData->m_printerTranslateY);

#ifdef __X__
      //      wxThePrintSetupData->SetPrinterOptions(WXSTRINGCAST text0->GetValue ());
      //      wxThePrintSetupData->SetPrinterCommand(WXSTRINGCAST text_prt->GetValue ());
#endif

      wxThePrintSetupData->SetPrinterOrientation((radio0->GetSelection() == 1 ? wxLANDSCAPE : wxPORTRAIT));

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
#endif
 // 0 (redundant)

// PostScript printer settings
// RETAINED FOR BACKWARD COMPATIBILITY
void wxSetPrinterCommand(const wxString& cmd)
{
  wxThePrintSetupData->SetPrinterCommand(cmd);
}

void wxSetPrintPreviewCommand(const wxString& cmd)
{
  wxThePrintSetupData->SetPrintPreviewCommand(cmd);
}

void wxSetPrinterOptions(const wxString& flags)
{
  wxThePrintSetupData->SetPrinterOptions(flags);
}

void wxSetPrinterFile(const wxString& f)
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

void wxSetAFMPath(const wxString& f)
{
  wxThePrintSetupData->SetAFMPath(f);
}

// Get current values
wxString wxGetPrinterCommand()
{
  return wxThePrintSetupData->GetPrinterCommand();
}

wxString wxGetPrintPreviewCommand()
{
  return wxThePrintSetupData->GetPrintPreviewCommand();
}

wxString wxGetPrinterOptions()
{
  return wxThePrintSetupData->GetPrinterOptions();
}

wxString wxGetPrinterFile()
{
  return wxThePrintSetupData->GetPrinterFile();
}

int wxGetPrinterOrientation()
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

int wxGetPrinterMode()
{
  return wxThePrintSetupData->GetPrinterMode();
}

wxString wxGetAFMPath()
{
  return wxThePrintSetupData->GetAFMPath();
}

/*
 * Print setup data
 */

wxPrintSetupData::wxPrintSetupData()
{
  m_printerOrient = wxPORTRAIT;
  m_printerScaleX = (double)1.0;
  m_printerScaleY = (double)1.0;
  m_printerTranslateX = 0;
  m_printerTranslateY = 0;
  m_printerMode = wxPRINT_MODE_FILE;
  m_printColour = TRUE;
}

wxPrintSetupData::~wxPrintSetupData()
{
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

// Initialize from a wxPrintData object (wxPrintData should now be used instead of wxPrintSetupData).
void wxPrintSetupData::operator=(const wxPrintData& data)
{
    SetPrinterCommand(data.GetPrinterCommand());
    SetPrintPreviewCommand(data.GetPreviewCommand());
    SetPrinterOptions(data.GetPrinterOptions());
    SetPrinterTranslation(data.GetPrinterTranslateX(), data.GetPrinterTranslateY());
    SetPrinterScaling(data.GetPrinterScaleX(), data.GetPrinterScaleY());
    SetPrinterOrientation(data.GetOrientation());
    SetPrinterMode((int) data.GetPrintMode());
    SetAFMPath(data.GetFontMetricPath());
    SetPaperName(wxThePrintPaperDatabase->ConvertIdToName(data.GetPaperId()));
    SetColour(data.GetColour());
    SetPrinterFile(data.GetFilename());
}

void wxInitializePrintSetupData(bool init)
{
  if (init)
  {
    wxThePrintSetupData = new wxPrintSetupData;

    wxThePrintSetupData->SetPrintPreviewCommand(PS_VIEWER_PROG);
    wxThePrintSetupData->SetPrinterOrientation(wxPORTRAIT);
    wxThePrintSetupData->SetPrinterMode(wxPRINT_MODE_PREVIEW);
    wxThePrintSetupData->SetPaperName(_("A4 sheet, 210 x 297 mm"));

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
    wxThePrintSetupData->SetPrinterOptions("");
#endif
#if !defined(__VMS__) && !defined(__WXMSW__)
    wxThePrintSetupData->SetPrinterCommand("lpr");
    wxThePrintSetupData->SetPrinterOptions("");
    wxThePrintSetupData->SetAFMPath("");
#endif
  }
  else
  {
    if (wxThePrintSetupData)
      delete wxThePrintSetupData;
     wxThePrintSetupData = (wxPrintSetupData *) NULL;
  }
}

// A module to allow initialization/cleanup of PostScript-related
// things without calling these functions from app.cpp.

class WXDLLEXPORT wxPostScriptModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxPostScriptModule)
public:
    wxPostScriptModule() {}
    bool OnInit();
    void OnExit();
};

IMPLEMENT_DYNAMIC_CLASS(wxPostScriptModule, wxModule)

/*
 * Initialization/cleanup module
 */

bool wxPostScriptModule::OnInit()
{
    wxInitializePrintSetupData();

    return TRUE;
}

void wxPostScriptModule::OnExit()
{
    wxInitializePrintSetupData(FALSE);
}

#endif
  // wxUSE_POSTSCRIPT

#endif 
  // wxUSE_PRINTING_ARCHITECTURE
