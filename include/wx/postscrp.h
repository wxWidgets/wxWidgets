/////////////////////////////////////////////////////////////////////////////
// Name:        postscrp.h
// Purpose:     wxPostScriptDC class
// Author:      Julian Smart and others
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_POSTSCRPH__
#define _WX_POSTSCRPH__

#ifdef __GNUG__
#pragma interface "postscrp.h"
#endif

#include "wx/dc.h"
#include "wx/dialog.h"
#include "wx/module.h"

#if wxUSE_POSTSCRIPT

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

#if wxUSE_IOSTREAMH
#  include <fstream.h>
#else
#  include <fstream>
#  ifdef _MSC_VER
      using namespace std;
#  endif
#endif

#ifdef __WXGTK__

// wxGTK has its own wxPostscriptDC

#include "wx/gtk/dcps.h"

#else

class WXDLLEXPORT wxPostScriptDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxPostScriptDC)

 public:
  // Create a printer DC
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

  void DrawIcon( const wxIcon& icon, long x, long y );
  void DrawBitmap( const wxBitmap& bitmap, long x, long y, bool useMask=FALSE );
  void DrawText(const wxString& text, long x, long y, bool use16 = FALSE);

  void Clear();
  void SetFont(const wxFont& font);
  void SetPen(const wxPen& pen);
  void SetBrush(const wxBrush& brush);
  void SetLogicalFunction(int function);
  void SetBackground(const wxBrush& brush);
  
  void SetClippingRegion(long x, long y, long width, long height);
  void SetClippingRegion( const wxRegion &region );
  void DestroyClippingRegion();

  bool StartDoc(const wxString& message);
  void EndDoc();
  void StartPage();
  void EndPage();

  long GetCharHeight();
  long GetCharWidth();
  void GetTextExtent(const wxString& string, long *x, long *y,
                     long *descent = (long *) NULL,
                     long *externalLeading = (long *) NULL,
                     wxFont *theFont = (wxFont *) NULL, bool use16 = FALSE);
  virtual void SetLogicalOrigin(long x, long y);
  virtual void CalcBoundingBox(long x, long y);

  void SetMapMode(int mode);
  void SetUserScale(double x, double y);
  long DeviceToLogicalX(int x) const;
  long DeviceToLogicalY(int y) const;
  long DeviceToLogicalXRel(int x) const;
  long DeviceToLogicalYRel(int y) const;
  long LogicalToDeviceX(long x) const;
  long LogicalToDeviceY(long y) const;
  long LogicalToDeviceXRel(long x) const;
  long LogicalToDeviceYRel(long y) const;
  bool Blit(long xdest, long ydest, long width, long height,
            wxDC *source, long xsrc, long ysrc, int rop = wxCOPY, bool useMask = FALSE);
  inline bool CanGetTextExtent(void) const { return FALSE; }
  inline bool CanDrawBitmap(void) const { return FALSE; }

  void GetSize(int* width, int* height) const;
  void GetSizeMM(long *width, long *height) const;

  inline void SetBackgroundMode(int WXUNUSED(mode)) {};
  inline void SetPalette(const wxPalette& WXUNUSED(palette)) {}
  
  inline ofstream *GetStream(void) const { return m_pstream; }
  inline int GetYOrigin(void) const { return m_yOrigin; }

  void SetupCTM();

protected:
  int               m_yOrigin;          // For EPS
  ofstream *        m_pstream;    // PostScript output stream
  wxString          m_title;
  unsigned char     m_currentRed;
  unsigned char     m_currentGreen;
  unsigned char     m_currentBlue;
  double            m_scaleFactor;
};

#endif 
        // __WXGTK__
      
#define wxID_PRINTER_COMMAND        1
#define wxID_PRINTER_OPTIONS        2
#define wxID_PRINTER_ORIENTATION    3
#define wxID_PRINTER_MODES          4
#define wxID_PRINTER_X_SCALE        5
#define wxID_PRINTER_Y_SCALE        6
#define wxID_PRINTER_X_TRANS        7
#define wxID_PRINTER_Y_TRANS        8

class WXDLLEXPORT wxPostScriptPrintDialog: public wxDialog
{
DECLARE_CLASS(wxPostScriptPrintDialog)
public:
    wxPostScriptPrintDialog (wxWindow *parent, const wxString& title,
          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
          long style = wxDEFAULT_DIALOG_STYLE);

    virtual int ShowModal(void) ;
};


// Print Orientation (Should also add Left, Right)
enum {
  PS_PORTRAIT = 1,
  PS_LANDSCAPE = 2
};// ps_orientation = PS_PORTRAIT;

// Print Actions
enum {
  PS_PRINTER,
  PS_FILE,
  PS_PREVIEW
};// ps_action = PS_PREVIEW;

// PostScript printer settings
WXDLLEXPORT void wxSetPrinterCommand(const char *cmd);
WXDLLEXPORT void wxSetPrintPreviewCommand(const char *cmd);
WXDLLEXPORT void wxSetPrinterOptions(const char *flags);
WXDLLEXPORT void wxSetPrinterOrientation(int orientation);
WXDLLEXPORT void wxSetPrinterScaling(double x, double y);
WXDLLEXPORT void wxSetPrinterTranslation(long x, long y);
WXDLLEXPORT void wxSetPrinterMode(int mode);
WXDLLEXPORT void wxSetPrinterFile(const char *f);
WXDLLEXPORT void wxSetAFMPath(const char *f);

// Get current values
WXDLLEXPORT char* wxGetPrinterCommand();
WXDLLEXPORT char* wxGetPrintPreviewCommand();
WXDLLEXPORT char* wxGetPrinterOptions();
WXDLLEXPORT int wxGetPrinterOrientation();
WXDLLEXPORT void wxGetPrinterScaling(double* x, double* y);
WXDLLEXPORT void wxGetPrinterTranslation(long *x, long *y);
WXDLLEXPORT int wxGetPrinterMode();
WXDLLEXPORT char* wxGetPrinterFile();
WXDLLEXPORT char* wxGetAFMPath();

/*
 * PostScript print setup information
 */

class WXDLLEXPORT wxPrintSetupData: public wxObject
{
public:
    char *printerCommand;
    char *previewCommand;
    char *printerFlags;
    char *printerFile;
    int printerOrient;
    double printerScaleX;
    double printerScaleY;
    long printerTranslateX;
    long printerTranslateY;
    // 1 = Preview, 2 = print to file, 3 = send to printer
    int printerMode;
    char *afmPath;
    // A name in the paper database (see wx_print.h: the printing framework)
    char *paperName;
    bool printColour;

public:
    wxPrintSetupData();
    ~wxPrintSetupData();

    void SetPrinterCommand(const char *cmd);
    void SetPaperName(const char *paper);
    void SetPrintPreviewCommand(const char *cmd);
    void SetPrinterOptions(const char *flags);
    void SetPrinterFile(const char *f);
    void SetPrinterOrientation(int orient);
    void SetPrinterScaling(double x, double y);
    void SetPrinterTranslation(long x, long y);
    // 1 = Preview, 2 = print to file, 3 = send to printer
    void SetPrinterMode(int mode);
    void SetAFMPath(const char *f);
    void SetColour(bool col);

    // Get current values
    char *GetPrinterCommand();
    char *GetPrintPreviewCommand();
    char *GetPrinterOptions();
    char *GetPrinterFile();
    char *GetPaperName();
    int GetPrinterOrientation();
    void GetPrinterScaling(double* x, double* y);
    void GetPrinterTranslation(long *x, long *y);
    int GetPrinterMode();
    char *GetAFMPath();
    bool GetColour();

    void operator=(wxPrintSetupData& data);

private:
  DECLARE_DYNAMIC_CLASS(wxPrintSetupData)
};

WXDLLEXPORT_DATA(extern wxPrintSetupData*) wxThePrintSetupData;
WXDLLEXPORT extern void wxInitializePrintSetupData(bool init = TRUE);

/*
 * Again, this only really needed for non-Windows platforms
 * or if you want to test the PostScript printing under Windows.
 */

class WXDLLEXPORT wxPrintPaperType: public wxObject
{
public:
    wxPrintPaperType(const char *name = (const char *) NULL, int wmm = 0, int hmm = 0, int wp = 0, int hp = 0);
    ~wxPrintPaperType();

public:
    int widthMM;
    int heightMM;
    int widthPixels;
    int heightPixels;
    char *pageName;

private:
    DECLARE_DYNAMIC_CLASS(wxPrintPaperType)
};

class WXDLLEXPORT wxPrintPaperDatabase: public wxList
{
public:
    wxPrintPaperDatabase();
    ~wxPrintPaperDatabase();

    void CreateDatabase();
    void ClearDatabase();

    void AddPaperType(const char *name, int wmm, int hmm, int wp, int hp);
    wxPrintPaperType *FindPaperType(const char *name);

private:
    DECLARE_DYNAMIC_CLASS(wxPrintPaperDatabase)
};

WXDLLEXPORT_DATA(extern wxPrintPaperDatabase*) wxThePrintPaperDatabase;

#endif 
      // wxUSE_POSTSCRIPT

#endif
        // _WX_POSTSCRPH__
	
	
