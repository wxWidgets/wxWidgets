/////////////////////////////////////////////////////////////////////////////
// Name:        cmndata.h
// Purpose:     Common GDI data classes
// Author:      Julian Smart and others
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CMNDATA_H_BASE_
#define _WX_CMNDATA_H_BASE_

#ifdef __GNUG__
#pragma interface "cmndata.h"
#endif

#include "wx/window.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/gdicmn.h"

#if (defined(__WXMOTIF__) || defined(__WXGTK__) || defined(__WXPM__)) && wxUSE_POSTSCRIPT
class WXDLLEXPORT wxPrintSetupData;
#endif

class WXDLLEXPORT wxColourData: public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxColourData)
public:
    wxColourData();
    wxColourData(const wxColourData& data);
    ~wxColourData();

    void SetChooseFull(bool flag) { chooseFull = flag; }
    bool GetChooseFull() const { return chooseFull; }
    void SetColour(wxColour& colour) { dataColour = colour; }
    wxColour &GetColour() { return dataColour; }

    // Array of 16 custom colours
    void SetCustomColour(int i, wxColour& colour);
    wxColour GetCustomColour(int i);

    void operator=(const wxColourData& data);

public:
    wxColour        dataColour;
    wxColour        custColours[16];
    bool            chooseFull;
};

class WXDLLEXPORT wxFontData: public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxFontData)
public:
    wxFontData();
    ~wxFontData();

    void SetAllowSymbols(bool flag) { allowSymbols = flag; }
    bool GetAllowSymbols() const { return allowSymbols; }

    void SetColour(const wxColour& colour) { fontColour = colour; }
    wxColour &GetColour() { return fontColour; }

    void SetShowHelp(bool flag) { showHelp = flag; }
    bool GetShowHelp() const { return showHelp; }

    void EnableEffects(bool flag) { enableEffects = flag; }
    bool GetEnableEffects() const { return enableEffects; }

    void SetInitialFont(const wxFont& font) { initialFont = font; }
    wxFont GetInitialFont() const { return initialFont; }

    void SetChosenFont(const wxFont& font) { chosenFont = font; }
    wxFont GetChosenFont() const { return chosenFont; }

    void SetRange(int minRange, int maxRange) { minSize = minRange; maxSize = maxRange; }

    // encoding info is split into 2 parts: the logical wxWin encoding
    // (wxFontEncoding) and a structure containing the native parameters for
    // it (wxNativeEncodingInfo)
    wxFontEncoding GetEncoding() const { return m_encoding; }
    void SetEncoding(wxFontEncoding encoding) { m_encoding = encoding; }

    wxNativeEncodingInfo& EncodingInfo() { return m_encodingInfo; }

public:
    wxColour        fontColour;
    bool            showHelp;
    bool            allowSymbols;
    bool            enableEffects;
    wxFont          initialFont;
    wxFont          chosenFont;
    int             minSize;
    int             maxSize;

private:
    wxFontEncoding       m_encoding;
    wxNativeEncodingInfo m_encodingInfo;
};

#if wxUSE_PRINTING_ARCHITECTURE
/*
 * wxPrintData
 * Encapsulates printer information (not printer dialog information)
 */

class WXDLLEXPORT wxPrintData: public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxPrintData)

    wxPrintData();
    wxPrintData(const wxPrintData& printData);
    ~wxPrintData();

    int GetNoCopies() const { return m_printNoCopies; };
    bool GetCollate() const { return m_printCollate; };
    int  GetOrientation() const { return m_printOrientation; };

    const wxString& GetPrinterName() const { return m_printerName; }
    bool GetColour() const { return m_colour; }
    wxDuplexMode GetDuplex() const { return m_duplexMode; }
    wxPaperSize GetPaperId() const { return m_paperId; }
    const wxSize& GetPaperSize() const { return m_paperSize; } // Not used yet: confusable with paper size
                                                                      // in wxPageSetupDialogData
    wxPrintQuality GetQuality() const { return m_printQuality; }

    void SetNoCopies(int v) { m_printNoCopies = v; };
    void SetCollate(bool flag) { m_printCollate = flag; };
    void SetOrientation(int orient) { m_printOrientation = orient; };

    void SetPrinterName(const wxString& name) { m_printerName = name; }
    void SetColour(bool colour) { m_colour = colour; }
    void SetDuplex(wxDuplexMode duplex) { m_duplexMode = duplex; }
    void SetPaperId(wxPaperSize sizeId) { m_paperId = sizeId; }
    void SetPaperSize(const wxSize& sz) { m_paperSize = sz; }
    void SetQuality(wxPrintQuality quality) { m_printQuality = quality; }

    // PostScript-specific data
    const wxString& GetPrinterCommand() const { return m_printerCommand; }
    const wxString& GetPrinterOptions() const { return m_printerOptions; }
    const wxString& GetPreviewCommand() const { return m_previewCommand; }
    const wxString& GetFilename() const { return m_filename; }
    const wxString& GetFontMetricPath() const { return m_afmPath; }
    double GetPrinterScaleX() const { return m_printerScaleX; }
    double GetPrinterScaleY() const { return m_printerScaleY; }
    long GetPrinterTranslateX() const { return m_printerTranslateX; }
    long GetPrinterTranslateY() const { return m_printerTranslateY; }
    wxPrintMode GetPrintMode() const { return m_printMode; }

    void SetPrinterCommand(const wxString& command) { m_printerCommand = command; }
    void SetPrinterOptions(const wxString& options) { m_printerOptions = options; }
    void SetPreviewCommand(const wxString& command) { m_previewCommand = command; }
    void SetFilename(const wxString& filename) { m_filename = filename; }
    void SetFontMetricPath(const wxString& path) { m_afmPath = path; }
    void SetPrinterScaleX(double x) { m_printerScaleX = x; }
    void SetPrinterScaleY(double y) { m_printerScaleY = y; }
    void SetPrinterScaling(double x, double y) { m_printerScaleX = x; m_printerScaleY = y; }
    void SetPrinterTranslateX(long x) { m_printerTranslateX = x; }
    void SetPrinterTranslateY(long y) { m_printerTranslateY = y; }
    void SetPrinterTranslation(long x, long y) { m_printerTranslateX = x; m_printerTranslateY = y; }
    void SetPrintMode(wxPrintMode printMode) { m_printMode = printMode; }

    void operator=(const wxPrintData& data);

    // For compatibility
#if (defined(__WXMOTIF__) || defined(__WXGTK__) || defined(__WXPM__)) && wxUSE_POSTSCRIPT
    void operator=(const wxPrintSetupData& setupData);
#endif

#if defined(__WXMSW__)
    // Convert to/from the DEVMODE structure
    void ConvertToNative();
    void ConvertFromNative();
    void* GetNativeData() const { return m_devMode; }
    void SetNativeData(void* data) { m_devMode = data; }
    void* GetNativeDataDevNames() const { return m_devNames; }
    void SetNativeDataDevNames(void* data) { m_devNames = data; }
#elif defined( __WXMAC__)
  void ConvertToNative();
  void ConvertFromNative();
#endif

public:
#ifdef __WXMSW__
    void*           m_devMode;
    void*           m_devNames;
#elif defined( __WXMAC__  )
	THPrint 		m_macPrintInfo ;
#endif

private:

    int             m_printNoCopies;
    int             m_printOrientation;
    bool            m_printCollate;

    // New members, 24/3/99
    wxString        m_printerName;
    bool            m_colour;
    wxDuplexMode    m_duplexMode;
    wxPrintQuality  m_printQuality;
    wxPaperSize     m_paperId;
    wxSize          m_paperSize;

    // PostScript-specific data
    wxString        m_printerCommand;
    wxString        m_previewCommand;
    wxString        m_printerOptions;
    wxString        m_filename;
    wxString        m_afmPath;
    double          m_printerScaleX;
    double          m_printerScaleY;
    long            m_printerTranslateX;
    long            m_printerTranslateY;
    wxPrintMode     m_printMode;
};

/*
 * wxPrintDialogData
 * Encapsulates information displayed and edited in the printer dialog box.
 * Contains a wxPrintData object which is filled in according to the values retrieved
 * from the dialog.
 */

class WXDLLEXPORT wxPrintDialogData: public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxPrintDialogData)

    wxPrintDialogData();
    wxPrintDialogData(const wxPrintDialogData& dialogData);
    wxPrintDialogData(const wxPrintData& printData);
    ~wxPrintDialogData();

    int GetFromPage() const { return m_printFromPage; };
    int GetToPage() const { return m_printToPage; };
    int GetMinPage() const { return m_printMinPage; };
    int GetMaxPage() const { return m_printMaxPage; };
    int GetNoCopies() const { return m_printNoCopies; };
    bool GetAllPages() const { return m_printAllPages; };
    bool GetSelection() const { return m_printSelection; };
    bool GetCollate() const { return m_printCollate; };
    bool GetPrintToFile() const { return m_printToFile; };
    bool GetSetupDialog() const { return m_printSetupDialog; };

    void SetFromPage(int v) { m_printFromPage = v; };
    void SetToPage(int v) { m_printToPage = v; };
    void SetMinPage(int v) { m_printMinPage = v; };
    void SetMaxPage(int v) { m_printMaxPage = v; };
    void SetNoCopies(int v) { m_printNoCopies = v; };
    void SetAllPages(bool flag) { m_printAllPages = flag; };
    void SetSelection(bool flag) { m_printSelection = flag; };
    void SetCollate(bool flag) { m_printCollate = flag; };
    void SetPrintToFile(bool flag) { m_printToFile = flag; };
    void SetSetupDialog(bool flag) { m_printSetupDialog = flag; };

    void EnablePrintToFile(bool flag) { m_printEnablePrintToFile = flag; };
    void EnableSelection(bool flag) { m_printEnableSelection = flag; };
    void EnablePageNumbers(bool flag) { m_printEnablePageNumbers = flag; };
    void EnableHelp(bool flag) { m_printEnableHelp = flag; };

    bool GetEnablePrintToFile() const { return m_printEnablePrintToFile; };
    bool GetEnableSelection() const { return m_printEnableSelection; };
    bool GetEnablePageNumbers() const { return m_printEnablePageNumbers; };
    bool GetEnableHelp() const { return m_printEnableHelp; };

    wxPrintData& GetPrintData() { return m_printData; }
    void SetPrintData(const wxPrintData& printData) { m_printData = printData; }

    void operator=(const wxPrintDialogData& data);
    void operator=(const wxPrintData& data); // Sets internal m_printData member

#ifdef __WXMSW__
    // Convert to/from the PRINTDLG structure
    void ConvertToNative();
    void ConvertFromNative();
    void SetOwnerWindow(wxWindow* win);
    void* GetNativeData() const { return m_printDlgData; }
#elif defined( __WXMAC__)
  void ConvertToNative();
  void ConvertFromNative();
#endif

#ifdef __WXMSW__
    void*           m_printDlgData;
#elif defined( __WXMAC__  )
	THPrint 		m_macPrintInfo ;
#endif

private:

    int             m_printFromPage;
    int             m_printToPage;
    int             m_printMinPage;
    int             m_printMaxPage;
    int             m_printNoCopies;
    bool            m_printAllPages;
    bool            m_printCollate;
    bool            m_printToFile;
    bool            m_printSelection;
    bool            m_printEnableSelection;
    bool            m_printEnablePageNumbers;
    bool            m_printEnableHelp;
    bool            m_printEnablePrintToFile;
    bool            m_printSetupDialog;

    wxPrintData     m_printData;
};

/*
* This is the data used (and returned) by the wxPageSetupDialog.
*/

// Compatibility with old name
#define wxPageSetupData wxPageSetupDialogData

class WXDLLEXPORT wxPageSetupDialogData: public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxPageSetupDialogData)

public:
    wxPageSetupDialogData();
    wxPageSetupDialogData(const wxPageSetupDialogData& dialogData);
    wxPageSetupDialogData(const wxPrintData& printData);
    ~wxPageSetupDialogData();

    wxSize GetPaperSize() const { return m_paperSize; };
    wxPaperSize GetPaperId() const { return m_printData.GetPaperId(); };
    wxPoint GetMinMarginTopLeft() const { return m_minMarginTopLeft; };
    wxPoint GetMinMarginBottomRight() const { return m_minMarginBottomRight; };
    wxPoint GetMarginTopLeft() const { return m_marginTopLeft; };
    wxPoint GetMarginBottomRight() const { return m_marginBottomRight; };

    bool GetDefaultMinMargins() const { return m_defaultMinMargins; };
    bool GetEnableMargins() const { return m_enableMargins; };
    bool GetEnableOrientation() const { return m_enableOrientation; };
    bool GetEnablePaper() const { return m_enablePaper; };
    bool GetEnablePrinter() const { return m_enablePrinter; };
    bool GetDefaultInfo() const { return m_getDefaultInfo; };
    bool GetEnableHelp() const { return m_enableHelp; };

    // If a corresponding paper type is found in the paper database, will set the m_printData
    // paper size id member as well.
    void SetPaperSize(const wxSize& sz);

    void SetPaperId(wxPaperSize& id) { m_printData.SetPaperId(id); };

    // Sets the wxPrintData id, plus the paper width/height if found in the paper database.
    void SetPaperSize(wxPaperSize id);

    void SetMinMarginTopLeft(const wxPoint& pt) { m_minMarginTopLeft = pt; };
    void SetMinMarginBottomRight(const wxPoint& pt) { m_minMarginBottomRight = pt; };
    void SetMarginTopLeft(const wxPoint& pt) { m_marginTopLeft = pt; };
    void SetMarginBottomRight(const wxPoint& pt) { m_marginBottomRight = pt; };
    void SetDefaultMinMargins(bool flag) { m_defaultMinMargins = flag; };
    void SetDefaultInfo(bool flag) { m_getDefaultInfo = flag; };

    void EnableMargins(bool flag) { m_enableMargins = flag; };
    void EnableOrientation(bool flag) { m_enableOrientation = flag; };
    void EnablePaper(bool flag) { m_enablePaper = flag; };
    void EnablePrinter(bool flag) { m_enablePrinter = flag; };
    void EnableHelp(bool flag) { m_enableHelp = flag; };

#if defined(__WIN95__)
    // Convert to/from the PAGESETUPDLG structure
    void ConvertToNative();
    void ConvertFromNative();
    void SetOwnerWindow(wxWindow* win);
    void* GetNativeData() const { return m_pageSetupData; }
#elif defined( __WXMAC__)
  void ConvertToNative();
  void ConvertFromNative();
#endif

    // Use paper size defined in this object to set the wxPrintData
    // paper id
    void CalculateIdFromPaperSize();

    // Use paper id in wxPrintData to set this object's paper size
    void CalculatePaperSizeFromId();

    void operator=(const wxPageSetupData& data);
    void operator=(const wxPrintData& data);

    wxPrintData& GetPrintData() { return m_printData; }
    void SetPrintData(const wxPrintData& printData) { m_printData = printData; }

#if defined(__WIN95__)
    void*           m_pageSetupData;
#elif defined( __WXMAC__  )
	THPrint 	m_macPageSetupInfo ;
#endif

private:

    wxSize          m_paperSize; // The dimensions selected by the user (on return, same as in wxPrintData?)
    wxPoint         m_minMarginTopLeft;
    wxPoint         m_minMarginBottomRight;
    wxPoint         m_marginTopLeft;
    wxPoint         m_marginBottomRight;

    // Flags
    bool            m_defaultMinMargins;
    bool            m_enableMargins;
    bool            m_enableOrientation;
    bool            m_enablePaper;
    bool            m_enablePrinter;
    bool            m_getDefaultInfo; // Equiv. to PSD_RETURNDEFAULT
    bool            m_enableHelp;

    wxPrintData     m_printData;
};

#endif // wxUSE_PRINTING_ARCHITECTURE

#endif
// _WX_CMNDATA_H_BASE_
