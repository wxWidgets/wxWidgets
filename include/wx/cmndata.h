/////////////////////////////////////////////////////////////////////////////
// Name:        cmndata.h
// Purpose:     Common GDI data classes
// Author:      Julian Smart and others
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CMNDATA_H_BASE_
#define _WX_CMNDATA_H_BASE_

#ifdef __GNUG__
#pragma interface "cmndata.h"
#endif

#include "wx/font.h"
#include "wx/colour.h"

class WXDLLEXPORT wxColourData: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxColourData)
 public:
  wxColour dataColour;
  wxColour custColours[16];
  bool chooseFull;

  wxColourData(void);
  ~wxColourData(void);

  inline void SetChooseFull(bool flag) { chooseFull = flag; }
  inline bool GetChooseFull(void) { return chooseFull; }
  inline void SetColour(wxColour& colour) { dataColour = colour; }
  inline wxColour &GetColour(void) { return dataColour; } 

  // Array of 16 custom colours
  void SetCustomColour(int i, wxColour& colour);
  wxColour GetCustomColour(int i);

  void operator=(const wxColourData& data);
};

class WXDLLEXPORT wxFontData: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxFontData)
 public:
  wxColour fontColour;
  bool showHelp;
  bool allowSymbols;
  bool enableEffects;
  wxFont initialFont;
  wxFont chosenFont;
  int minSize;
  int maxSize;

  wxFontData(void);
  ~wxFontData(void);

  inline void SetAllowSymbols(bool flag) { allowSymbols = flag; }
  inline bool GetAllowSymbols(void) { return allowSymbols; }
  inline void SetColour(const wxColour& colour) { fontColour = colour; }
  inline wxColour &GetColour(void) { return fontColour; }
  inline void SetShowHelp(bool flag) { showHelp = flag; }
  inline bool GetShowHelp(void) { return showHelp; }
  inline void EnableEffects(bool flag) { enableEffects = flag; }
  inline bool GetEnableEffects(void) { return enableEffects; }
  inline void SetInitialFont(const wxFont& font) { initialFont = font; }
  inline wxFont GetInitialFont(void) { return initialFont; }
  inline void SetChosenFont(const wxFont& font) { chosenFont = font; }
  inline wxFont GetChosenFont(void) { return chosenFont; }
  inline void SetRange(int minRange, int maxRange) { minSize = minRange; maxSize = maxRange; }

  void operator=(const wxFontData& data);
};

/*
 * wxPrintData
 * Encapsulates information displayed and edited in the printer dialog box.
 */
 
class WXDLLEXPORT wxPrintData: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPrintData)

 public:
#ifdef __WXMSW__
  void *printData;
#endif
  int printFromPage;
  int printToPage;
  int printMinPage;
  int printMaxPage;
  int printNoCopies;
  int printOrientation;
  bool printAllPages;
  bool printCollate;
  bool printToFile;
  bool printEnableSelection;
  bool printEnablePageNumbers;
  bool printEnableHelp;
  bool printEnablePrintToFile;
  bool printSetupDialog;

  wxPrintData(void);
  ~wxPrintData(void);

  inline int GetFromPage(void) { return printFromPage; };
  inline int GetToPage(void) { return printToPage; };
  inline int GetMinPage(void) { return printMinPage; };
  inline int GetMaxPage(void) { return printMaxPage; };
  inline int GetNoCopies(void) { return printNoCopies; };
  inline bool GetAllPages(void) { return printAllPages; };
  inline bool GetCollate(void) { return printCollate; };
  inline bool GetPrintToFile(void) { return printToFile; };
  inline bool GetSetupDialog(void) { return printSetupDialog; };
  inline int  GetOrientation(void) { return printOrientation; };

  inline void SetFromPage(int v) { printFromPage = v; };
  inline void SetToPage(int v) { printToPage = v; };
  inline void SetMinPage(int v) { printMinPage = v; };
  inline void SetMaxPage(int v) { printMaxPage = v; };
  inline void SetNoCopies(int v) { printNoCopies = v; };
  inline void SetAllPages(bool flag) { printAllPages = flag; };
  inline void SetCollate(bool flag) { printCollate = flag; };
  inline void SetPrintToFile(bool flag) { printToFile = flag; };
  inline void SetSetupDialog(bool flag) { printSetupDialog = flag; };
  inline void SetOrientation(int orient) { printOrientation = orient; };

  inline void EnablePrintToFile(bool flag) { printEnablePrintToFile = flag; };
  inline void EnableSelection(bool flag) { printEnableSelection = flag; };
  inline void EnablePageNumbers(bool flag) { printEnablePageNumbers = flag; };
  inline void EnableHelp(bool flag) { printEnableHelp = flag; };

  inline bool GetEnablePrintToFile(void) { return printEnablePrintToFile; };
  inline bool GetEnableSelection(void) { return printEnableSelection; };
  inline bool GetEnablePageNumbers(void) { return printEnablePageNumbers; };
  inline bool GetEnableHelp(void) { return printEnableHelp; };

  void operator=(const wxPrintData& data);

#ifdef __WXMSW__
  // Convert to/from the PRINTDLG structure
  void ConvertToNative(void);
  void ConvertFromNative(void);
  void SetOwnerWindow(wxWindow* win);
  inline void* GetNativeData(void) { return printData; }
#endif
};

/*
 * This is the data used (and returned) by the wxPageSetupDialog.
 */

class WXDLLEXPORT wxPageSetupData: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPageSetupData)

 public:
#if defined(__WIN95__)
  void*     m_pageSetupData;
#endif
  wxPoint   m_paperSize;
  wxPoint   m_minMarginTopLeft;
  wxPoint   m_minMarginBottomRight;
  wxPoint   m_marginTopLeft;
  wxPoint   m_marginBottomRight;
  int       m_orientation;

  // Flags
  bool      m_defaultMinMargins;
  bool      m_enableMargins;
  bool      m_enableOrientation;
  bool      m_enablePaper;
  bool      m_enablePrinter;
  bool      m_getDefaultInfo; // Equiv. to PSD_RETURNDEFAULT
  bool      m_enableHelp;

  wxPageSetupData(void);
  ~wxPageSetupData(void);

  inline wxPoint GetPaperSize(void) { return m_paperSize; };
  inline wxPoint GetMinMarginTopLeft(void) { return m_minMarginTopLeft; };
  inline wxPoint GetMinMarginBottomRight(void) { return m_minMarginBottomRight; };
  inline wxPoint GetMarginTopLeft(void) { return m_marginTopLeft; };
  inline wxPoint GetMarginBottomRight(void) { return m_marginBottomRight; };
  inline int GetOrientation(void) { return m_orientation; };

  inline bool GetDefaultMinMargins(void) { return m_defaultMinMargins; };
  inline bool GetEnableMargins(void) { return m_enableMargins; };
  inline bool GetEnableOrientation(void) { return m_enableOrientation; };
  inline bool GetEnablePaper(void) { return m_enablePaper; };
  inline bool GetEnablePrinter(void) { return m_enablePrinter; };
  inline bool GetDefaultInfo(void) { return m_getDefaultInfo; };
  inline bool GetEnableHelp(void) { return m_enableHelp; };

  inline void SetPaperSize(const wxPoint& pt) { m_paperSize = pt; };
  inline void SetMinMarginTopLeft(const wxPoint& pt) { m_minMarginTopLeft = pt; };
  inline void SetMinMarginBottomRight(const wxPoint& pt) { m_minMarginBottomRight = pt; };
  inline void SetMarginTopLeft(const wxPoint& pt) { m_marginTopLeft = pt; };
  inline void SetMarginBottomRight(const wxPoint& pt) { m_marginBottomRight = pt; };
  inline void SetOrientation(int orient) { m_orientation = orient; };
  inline void SetDefaultMinMargins(bool flag) { m_defaultMinMargins = flag; };
  inline void SetDefaultInfo(bool flag) { m_getDefaultInfo = flag; };

  inline void EnableMargins(bool flag) { m_enableMargins = flag; };
  inline void EnableOrientation(bool flag) { m_enableOrientation = flag; };
  inline void EnablePaper(bool flag) { m_enablePaper = flag; };
  inline void EnablePrinter(bool flag) { m_enablePrinter = flag; };
  inline void EnableHelp(bool flag) { m_enableHelp = flag; };

#if defined(__WIN95__)
  // Convert to/from the PAGESETUPDLG structure
  void ConvertToNative(void);
  void ConvertFromNative(void);
  void SetOwnerWindow(wxWindow* win);
  inline void* GetNativeData(void) { return m_pageSetupData; }
#endif

  void operator=(const wxPageSetupData& data);
};


#endif
    // _WX_CMNDATA_H_BASE_
