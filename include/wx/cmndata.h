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

#if (defined(__WXMOTIF__) || defined(__WXGTK__)) && wxUSE_POSTSCRIPT
class WXDLLEXPORT wxPrintSetupData;
#endif

class WXDLLEXPORT wxColourData: public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxColourData)
public:
    wxColourData();
    wxColourData(const wxColourData& data);
    ~wxColourData();
    
    inline void SetChooseFull(bool flag) { chooseFull = flag; }
    inline bool GetChooseFull() const { return chooseFull; }
    inline void SetColour(wxColour& colour) { dataColour = colour; }
    inline wxColour &GetColour() { return dataColour; }

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
    wxFontData(const wxFontData& fontData);
    ~wxFontData();

    inline void SetAllowSymbols(bool flag) { allowSymbols = flag; }
    inline bool GetAllowSymbols() const { return allowSymbols; }

    inline void SetColour(const wxColour& colour) { fontColour = colour; }
    inline wxColour &GetColour() { return fontColour; }

    inline void SetShowHelp(bool flag) { showHelp = flag; }
    inline bool GetShowHelp() const { return showHelp; }

    inline void EnableEffects(bool flag) { enableEffects = flag; }
    inline bool GetEnableEffects() const { return enableEffects; }

    inline void SetInitialFont(const wxFont& font) { initialFont = font; }
    inline wxFont GetInitialFont() const { return initialFont; }

    inline void SetChosenFont(const wxFont& font) { chosenFont = font; }
    inline wxFont GetChosenFont() const { return chosenFont; }

    inline void SetRange(int minRange, int maxRange) { minSize = minRange; maxSize = maxRange; }
    
    void operator=(const wxFontData& data);

public:
    wxColour        fontColour;
    bool            showHelp;
    bool            allowSymbols;
    bool            enableEffects;
    wxFont          initialFont;
    wxFont          chosenFont;
    int             minSize;
    int             maxSize;
};

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
    
    inline int GetNoCopies() const { return m_printNoCopies; };
    inline bool GetCollate() const { return m_printCollate; };
    inline int  GetOrientation() const { return m_printOrientation; };

    inline const wxString& GetPrinterName() const { return m_printerName; }
    inline bool GetColour() const { return m_colour; }
    inline wxDuplexMode GetDuplex() const { return m_duplexMode; }
    inline wxPaperSize GetPaperId() const { return m_paperId; }
    inline const wxSize& GetPaperSize() const { return m_paperSize; } // Not used yet: confusable with paper size
                                                                      // in wxPageSetupDialogData
    inline wxPrintQuality GetQuality() const { return m_printQuality; }

    inline void SetNoCopies(int v) { m_printNoCopies = v; };
    inline void SetCollate(bool flag) { m_printCollate = flag; };
    inline void SetOrientation(int orient) { m_printOrientation = orient; };

    inline void SetPrinterName(const wxString& name) { m_printerName = name; }
    inline void SetColour(bool colour) { m_colour = colour; }
    inline void SetDuplex(wxDuplexMode duplex) { m_duplexMode = duplex; }
    inline void SetPaperId(wxPaperSize sizeId) { m_paperId = sizeId; }
    inline void SetPaperSize(const wxSize& sz) { m_paperSize = sz; }
    inline void SetQuality(wxPrintQuality quality) { m_printQuality = quality; }

    // PostScript-specific data
    inline const wxString& GetPrinterCommand() const { return m_printerCommand; }
    inline const wxString& GetPrinterOptions() const { return m_printerOptions; }
    inline const wxString& GetPreviewCommand() const { return m_previewCommand; }
    inline const wxString& GetFilename() const { return m_filename; }
    inline const wxString& GetFontMetricPath() const { return m_afmPath; }
    inline double GetPrinterScaleX() const { return m_printerScaleX; }
    inline double GetPrinterScaleY() const { return m_printerScaleY; }
    inline long GetPrinterTranslateX() const { return m_printerTranslateX; }
    inline long GetPrinterTranslateY() const { return m_printerTranslateY; }
    inline wxPrintMode GetPrintMode() const { return m_printMode; }

    inline void SetPrinterCommand(const wxString& command) { m_printerCommand = command; }
    inline void SetPrinterOptions(const wxString& options) { m_printerOptions = options; }
    inline void SetPreviewCommand(const wxString& command) { m_previewCommand = command; }
    inline void SetFilename(const wxString& filename) { m_filename = filename; }
    inline void SetFontMetricPath(const wxString& path) { m_afmPath = path; }
    inline void SetPrinterScaleX(double x) { m_printerScaleX = x; }
    inline void SetPrinterScaleY(double y) { m_printerScaleY = y; }
    inline void SetPrinterScaling(double x, double y) { m_printerScaleX = x; m_printerScaleY = y; }
    inline void SetPrinterTranslateX(long x) { m_printerTranslateX = x; }
    inline void SetPrinterTranslateY(long y) { m_printerTranslateY = y; }
    inline void SetPrinterTranslation(long x, long y) { m_printerTranslateX = x; m_printerTranslateY = y; }
    inline void SetPrintMode(wxPrintMode printMode) { m_printMode = printMode; }

    void operator=(const wxPrintData& data);

    // For compatibility
#if (defined(__WXMOTIF__) || defined(__WXGTK__)) && wxUSE_POSTSCRIPT
    void operator=(const wxPrintSetupData& setupData);
#endif
    
#ifdef __WXMSW__
    // Convert to/from the DEVMODE structure
    void ConvertToNative();
    void ConvertFromNative();
    inline void* GetNativeData() const { return m_devMode; }
    inline void SetNativeData(void* data) { m_devMode = data; }
#endif
    
public:
#ifdef __WXMSW__
    void*           m_devMode;
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
    
    inline int GetFromPage() const { return m_printFromPage; };
    inline int GetToPage() const { return m_printToPage; };
    inline int GetMinPage() const { return m_printMinPage; };
    inline int GetMaxPage() const { return m_printMaxPage; };
    inline int GetNoCopies() const { return m_printNoCopies; };
    inline bool GetAllPages() const { return m_printAllPages; };
    inline bool GetCollate() const { return m_printCollate; };
    inline bool GetPrintToFile() const { return m_printToFile; };
    inline bool GetSetupDialog() const { return m_printSetupDialog; };

    inline void SetFromPage(int v) { m_printFromPage = v; };
    inline void SetToPage(int v) { m_printToPage = v; };
    inline void SetMinPage(int v) { m_printMinPage = v; };
    inline void SetMaxPage(int v) { m_printMaxPage = v; };
    inline void SetNoCopies(int v) { m_printNoCopies = v; };
    inline void SetAllPages(bool flag) { m_printAllPages = flag; };
    inline void SetCollate(bool flag) { m_printCollate = flag; };
    inline void SetPrintToFile(bool flag) { m_printToFile = flag; };
    inline void SetSetupDialog(bool flag) { m_printSetupDialog = flag; };

    inline void EnablePrintToFile(bool flag) { m_printEnablePrintToFile = flag; };
    inline void EnableSelection(bool flag) { m_printEnableSelection = flag; };
    inline void EnablePageNumbers(bool flag) { m_printEnablePageNumbers = flag; };
    inline void EnableHelp(bool flag) { m_printEnableHelp = flag; };

    inline bool GetEnablePrintToFile() const { return m_printEnablePrintToFile; };
    inline bool GetEnableSelection() const { return m_printEnableSelection; };
    inline bool GetEnablePageNumbers() const { return m_printEnablePageNumbers; };
    inline bool GetEnableHelp() const { return m_printEnableHelp; };

    inline wxPrintData& GetPrintData() { return m_printData; }
    inline void SetPrintData(const wxPrintData& printData) { m_printData = printData; }

    void operator=(const wxPrintDialogData& data);
    void operator=(const wxPrintData& data); // Sets internal m_printData member
    
#ifdef __WXMSW__
    // Convert to/from the PRINTDLG structure
    void ConvertToNative();
    void ConvertFromNative();
    void SetOwnerWindow(wxWindow* win);
    inline void* GetNativeData() const { return m_printDlgData; }
#endif
    
#ifdef __WXMSW__
    void*           m_printDlgData;
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

    inline wxSize GetPaperSize() const { return m_paperSize; };
    inline wxPaperSize GetPaperId() const { return m_printData.GetPaperId(); };
    inline wxPoint GetMinMarginTopLeft() const { return m_minMarginTopLeft; };
    inline wxPoint GetMinMarginBottomRight() const { return m_minMarginBottomRight; };
    inline wxPoint GetMarginTopLeft() const { return m_marginTopLeft; };
    inline wxPoint GetMarginBottomRight() const { return m_marginBottomRight; };

    inline bool GetDefaultMinMargins() const { return m_defaultMinMargins; };
    inline bool GetEnableMargins() const { return m_enableMargins; };
    inline bool GetEnableOrientation() const { return m_enableOrientation; };
    inline bool GetEnablePaper() const { return m_enablePaper; };
    inline bool GetEnablePrinter() const { return m_enablePrinter; };
    inline bool GetDefaultInfo() const { return m_getDefaultInfo; };
    inline bool GetEnableHelp() const { return m_enableHelp; };

    // If a corresponding paper type is found in the paper database, will set the m_printData
    // paper size id member as well.
    void SetPaperSize(const wxSize& sz);

    // Sets the wxPrintData id, plus the paper width/height if found in the paper database.
    void SetPaperSize(wxPaperSize id);

    inline void SetMinMarginTopLeft(const wxPoint& pt) { m_minMarginTopLeft = pt; };
    inline void SetMinMarginBottomRight(const wxPoint& pt) { m_minMarginBottomRight = pt; };
    inline void SetMarginTopLeft(const wxPoint& pt) { m_marginTopLeft = pt; };
    inline void SetMarginBottomRight(const wxPoint& pt) { m_marginBottomRight = pt; };
    inline void SetDefaultMinMargins(bool flag) { m_defaultMinMargins = flag; };
    inline void SetDefaultInfo(bool flag) { m_getDefaultInfo = flag; };
    
    inline void EnableMargins(bool flag) { m_enableMargins = flag; };
    inline void EnableOrientation(bool flag) { m_enableOrientation = flag; };
    inline void EnablePaper(bool flag) { m_enablePaper = flag; };
    inline void EnablePrinter(bool flag) { m_enablePrinter = flag; };
    inline void EnableHelp(bool flag) { m_enableHelp = flag; };
    
#if defined(__WIN95__)
    // Convert to/from the PAGESETUPDLG structure
    void ConvertToNative();
    void ConvertFromNative();
    void SetOwnerWindow(wxWindow* win);
    inline void* GetNativeData() const { return m_pageSetupData; }
#endif

    // Use paper size defined in this object to set the wxPrintData
    // paper id
    void CalculateIdFromPaperSize();
    
    // Use paper id in wxPrintData to set this object's paper size
    void CalculatePaperSizeFromId();
    
    void operator=(const wxPageSetupData& data);
    void operator=(const wxPrintData& data);

    inline wxPrintData& GetPrintData() { return m_printData; }
    inline void SetPrintData(const wxPrintData& printData) { m_printData = printData; }

#if defined(__WIN95__)
    void*           m_pageSetupData;
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

#endif
// _WX_CMNDATA_H_BASE_
