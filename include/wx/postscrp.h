/////////////////////////////////////////////////////////////////////////////
// Name:        postscrp.h
// Purpose:     wxPostScriptDC class
// Author:      Julian Smart and others
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __POSTSCRPH__
#define __POSTSCRPH__

#ifdef __GNUG__
#pragma interface "postscrp.h"
#endif

#include "wx/dc.h"
#include "wx/dialog.h"

#if USE_POSTSCRIPT

class WXDLLIMPORT ofstream;
class WXDLLEXPORT wxPostScriptDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxPostScriptDC)

 public:
  // Create a printer DC
  wxPostScriptDC(void);
  wxPostScriptDC(const wxString& output, bool interactive = TRUE, wxWindow *parent = NULL);

  ~wxPostScriptDC(void);

  bool Create(const wxString& output, bool interactive = TRUE, wxWindow *parent = NULL);

  virtual bool PrinterDialog(wxWindow *parent = NULL);

  inline virtual void BeginDrawing(void) {} ;
  inline virtual void EndDrawing(void) {} ;

  void FloodFill(long x1, long y1, wxColour *col, int style=wxFLOOD_SURFACE) ;
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

  // RR: I define these in wxDC, after all they all do the same everywhere

#ifdef __WINDOWS__
  // Splines
  // 3-point spline
  void DrawSpline(long x1, long y1, long x2, long y2, long x3, long y3);
  // Any number of control points - a list of pointers to wxPoints
  void DrawSpline(wxList *points);
  void DrawSpline(int n, wxPoint points[]);
#endif

  void DrawOpenSpline(wxList *points);

  void DrawIcon(const wxIcon& icon, long x, long y);
  void DrawText(const wxString& text, long x, long y, bool use16 = FALSE);

  void Clear(void);
  void SetFont(const wxFont& font);
  void SetPen(const wxPen& pen);
  void SetBrush(const wxBrush& brush);
  void SetLogicalFunction(int function);
  void SetBackground(const wxBrush& brush);
  void SetClippingRegion(long x, long y, long width, long height);
  void DestroyClippingRegion(void);

  bool StartDoc(const wxString& message);
  void EndDoc(void);
  void StartPage(void);
  void EndPage(void);

  long GetCharHeight(void);
  long GetCharWidth(void);
  void GetTextExtent(const wxString& string, long *x, long *y,
                     long *descent = NULL, long *externalLeading = NULL,
                     wxFont *theFont = NULL, bool use16 = FALSE);
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
void WXDLLEXPORT wxSetPrinterCommand(char *cmd);
void WXDLLEXPORT wxSetPrintPreviewCommand(char *cmd);
void WXDLLEXPORT wxSetPrinterOptions(char *flags);
void WXDLLEXPORT wxSetPrinterOrientation(int orientation);
void WXDLLEXPORT wxSetPrinterScaling(double x, double y);
void WXDLLEXPORT wxSetPrinterTranslation(long x, long y);
void WXDLLEXPORT wxSetPrinterMode(int mode);
void WXDLLEXPORT wxSetPrinterFile(char *f);
void WXDLLEXPORT wxSetAFMPath(char *f);

// Get current values
char* WXDLLEXPORT wxGetPrinterCommand(void);
char* WXDLLEXPORT wxGetPrintPreviewCommand(void);
char* WXDLLEXPORT wxGetPrinterOptions(void);
int WXDLLEXPORT wxGetPrinterOrientation(void);
void WXDLLEXPORT wxGetPrinterScaling(double* x, double* y);
void WXDLLEXPORT wxGetPrinterTranslation(long *x, long *y);
int WXDLLEXPORT wxGetPrinterMode(void);
char* WXDLLEXPORT wxGetPrinterFile(void);
char* WXDLLEXPORT wxGetAFMPath(void);

/*
 * PostScript print setup information
 */

class WXDLLEXPORT wxPrintSetupData: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPrintSetupData)

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
  wxPrintSetupData(void);
  ~wxPrintSetupData(void);

  void SetPrinterCommand(char *cmd);
  void SetPaperName(char *paper);
  void SetPrintPreviewCommand(char *cmd);
  void SetPrinterOptions(char *flags);
  void SetPrinterFile(char *f);
  void SetPrinterOrientation(int orient);
  void SetPrinterScaling(double x, double y);
  void SetPrinterTranslation(long x, long y);
  // 1 = Preview, 2 = print to file, 3 = send to printer
  void SetPrinterMode(int mode);
  void SetAFMPath(char *f);
  void SetColour(bool col);

  // Get current values
  char *GetPrinterCommand(void);
  char *GetPrintPreviewCommand(void);
  char *GetPrinterOptions(void);
  char *GetPrinterFile(void);
  char *GetPaperName(void);
  int GetPrinterOrientation(void);
  void GetPrinterScaling(double* x, double* y);
  void GetPrinterTranslation(long *x, long *y);
  int GetPrinterMode(void);
  char *GetAFMPath(void);
  bool GetColour(void);

  void operator=(wxPrintSetupData& data);
};

extern wxPrintSetupData* WXDLLEXPORT wxThePrintSetupData;
extern void WXDLLEXPORT wxInitializePrintSetupData(bool init = TRUE);

/*
 * Again, this only really needed for non-Windows platforms
 * or if you want to test the PostScript printing under Windows.
 */

class WXDLLEXPORT wxPrintPaperType: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPrintPaperType)

 public:
  int widthMM;
  int heightMM;
  int widthPixels;
  int heightPixels;
  char *pageName;

  wxPrintPaperType(char *name = NULL, int wmm = 0, int hmm = 0, int wp = 0, int hp = 0);
  ~wxPrintPaperType(void);
};

class WXDLLEXPORT wxPrintPaperDatabase: public wxList
{
  DECLARE_DYNAMIC_CLASS(wxPrintPaperDatabase)

 public:
  wxPrintPaperDatabase(void);
  ~wxPrintPaperDatabase(void);

  void CreateDatabase(void);
  void ClearDatabase(void);

  void AddPaperType(char *name, int wmm, int hmm, int wp, int hp);
  wxPrintPaperType *FindPaperType(char *name);
};

WXDLLEXPORT_DATA(extern wxPrintPaperDatabase*) wxThePrintPaperDatabase;

#endif // USE_POSTSCRIPT
#endif
        // __POSTSCRPH__
