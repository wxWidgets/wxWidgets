/////////////////////////////////////////////////////////////////////////////
// Name:        dcps.h
// Purpose:     wxPostScriptDC class
// Author:      Julian Smart and others
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Robert Roebling and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDCPSH__
#define __GTKDCPSNH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/dc.h"
#include "wx/dialog.h"
#include "wx/module.h"
#include <fstream.h>

#if wxUSE_POSTSCRIPT

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxPostScriptDC;

//-----------------------------------------------------------------------------
// wxPostScriptDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPostScriptDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxPostScriptDC)

public:

  wxPostScriptDC();
  wxPostScriptDC(const wxString& output, bool interactive = TRUE, wxWindow *parent = (wxWindow *) NULL);

  ~wxPostScriptDC();

  bool Create(const wxString& output, bool interactive = TRUE, wxWindow *parent = (wxWindow *) NULL);

  virtual bool PrinterDialog(wxWindow *parent = (wxWindow *) NULL);

  inline virtual void BeginDrawing(void) {} ;
  inline virtual void EndDrawing(void) {} ;

  void FloodFill(long x1, long y1, const wxColour &col, int style=wxFLOOD_SURFACE) ;
  bool GetPixel(long x1, long y1, wxColour *col) const;

  void DrawLine(long x1, long y1, long x2, long y2);
  void CrossHair(long x, long y) ;
  void DrawArc(long x1,long y1,long x2,long y2,long xc,long yc);
  void DrawEllipticArc(long x,long y,long w,long h,double sa,double ea);
  void DrawPoint(long x, long y);
  // Avoid compiler warning
  void DrawPoint(wxPoint& point) { wxDC::DrawPoint(point); }
  void DrawLines(int n, wxPoint points[], long xoffset = 0, long yoffset = 0);
  // Avoid compiler warning
  void DrawLines(wxList *lines, long xoffset = 0, long yoffset = 0)
  { wxDC::DrawLines(lines, xoffset, yoffset); }
  void DrawPolygon(int n, wxPoint points[], long xoffset = 0, long yoffset = 0, int fillStyle=wxODDEVEN_RULE);
  // Avoid compiler warning
  void DrawPolygon(wxList *lines, long xoffset = 0, long yoffset = 0, int fillStyle=wxODDEVEN_RULE)
  { wxDC::DrawPolygon(lines, xoffset, yoffset, fillStyle); }
  void DrawRectangle(long x, long y, long width, long height);
  void DrawRoundedRectangle(long x, long y, long width, long height, double radius = 20);
  void DrawEllipse(long x, long y, long width, long height);

  void DrawSpline(wxList *points);

  bool Blit(long xdest, long ydest, long width, long height,
            wxDC *source, long xsrc, long ysrc, int rop = wxCOPY, bool useMask = FALSE);
  inline bool CanDrawBitmap(void) const { return TRUE; }

  void DrawIcon( const wxIcon& icon, long x, long y );
  void DrawBitmap( const wxBitmap& bitmap, long x, long y, bool useMask=FALSE );
  
  
  void DrawText(const wxString& text, long x, long y, bool use16 = FALSE);

  void Clear();
  void SetFont( const wxFont& font );
  void SetPen( const wxPen& pen );
  void SetBrush( const wxBrush& brush );
  void SetLogicalFunction( int function );
  void SetBackground( const wxBrush& brush );
  
  void SetClippingRegion(long x, long y, long width, long height);
  void SetClippingRegion( const wxRegion &region );
  void DestroyClippingRegion();

  bool StartDoc(const wxString& message);
  void EndDoc();
  void StartPage();
  void EndPage();

  long GetCharHeight();
  long GetCharWidth();
  inline bool CanGetTextExtent(void) const { return FALSE; }
  void GetTextExtent(const wxString& string, long *x, long *y,
                     long *descent = (long *) NULL,
                     long *externalLeading = (long *) NULL,
                     wxFont *theFont = (wxFont *) NULL, bool use16 = FALSE);
		     
  void GetSize(int* width, int* height) const;
  void GetSizeMM(long *width, long *height) const;

  void SetAxisOrientation( bool xLeftRight, bool yBottomUp );
  void SetDeviceOrigin( long x, long y );
  
  inline void SetBackgroundMode(int WXUNUSED(mode)) {};
  inline void SetPalette(const wxPalette& WXUNUSED(palette)) {}
  
  inline ofstream *GetStream(void) const { return m_pstream; }

protected:

  ofstream *        m_pstream;    // PostScript output stream
  wxString          m_title;
  unsigned char     m_currentRed;
  unsigned char     m_currentGreen;
  unsigned char     m_currentBlue;
  int               m_pageNumber;
  bool              m_clipping;
  double            m_underlinePosition;
  double            m_underlineThickness;
};

#endif 
    // wxUSE_POSTSCRIPT

#endif
        // __GTKDCPSH__
