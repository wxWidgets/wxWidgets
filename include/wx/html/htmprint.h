/////////////////////////////////////////////////////////////////////////////
// Name:        htmprint.h
// Purpose:     html printing classes
// Author:      Vaclav Slavik
// Created:     25/09/99
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HTMPRINT_H_
#define _WX_HTMPRINT_H_

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/defs.h>

#if wxUSE_HTML & wxUSE_PRINTING_ARCHITECTURE

#include "wx/html/htmlcell.h"
#include "wx/html/winpars.h"

#include "wx/print.h"
#include "wx/printdlg.h"

//--------------------------------------------------------------------------------
// wxHtmlDCRenderer
//                  This class is capable of rendering HTML into specified 
//                  portion of DC
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlDCRenderer : public wxObject
{
    public:
        wxHtmlDCRenderer();
        ~wxHtmlDCRenderer();
        
        // Following 3 methods *must* be called before any call to Render:
        void SetDC(wxDC *dc, int maxwidth);
                // asign DC to this render
                // maxwidth is width of area (on this DC) that is equivalent to screen's width, in pixels
                // (you should set it to page width minus margins)
                // Also see SetSize
        void SetSize(int width, int height);
                // sets size of output rectangle, in pixels. Note that you *can't* change
                // width of the rectangle between calls to Render! (You can freely change height.)
                // If you set width = maxwidth then HTML is rendered as if it were displayed in fullscreen.
                // If you set width = 1/2 maxwidth the it is rendered as if it covered half the screen
                // and so on..
        void SetHtmlText(const wxString& html, const wxString& basepath = wxEmptyString, bool isdir = TRUE);
                // sets the text to be displayed
                //
                // basepath is base directory (html string would be stored there if it was in
                // file). It is used to determine path for loading images, for example.
                // isdir is FALSE if basepath is filename, TRUE if it is directory name
                // (see wxFileSystem for detailed explanation)
                                
        int Render(int x, int y, int from = 0, int dont_render = FALSE);
                // [x,y] is position of upper-left corner of printing rectangle (see SetSize)
                // from is y-coordinate of the very first visible cell 
                // Returned value is y coordinate of first cell than didn't fit onto page.
                // Use this value as 'from' in next call to Render in order to print multiple pages
                // document
                // If dont_render is TRUE then nothing is rendered into DC and it only counts
                // pixels and return y coord of the next page
                //
                // CAUTION! Render() changes DC's user scale and does NOT restore it! 
                
        int GetTotalHeight();
                // returns total height of the html document
                // (compare Render's return value with this)
        
    private:
    
        wxDC *m_DC;
        wxHtmlWinParser *m_Parser;
        wxFileSystem *m_FS;
        wxHtmlContainerCell *m_Cells;
        int m_MaxWidth, m_Width, m_Height;
        double m_Scale;
};





enum {
    wxPAGE_ODD,
    wxPAGE_EVEN,
    wxPAGE_ALL
};



//--------------------------------------------------------------------------------
// wxHtmlPrintout
//                  This class is derived from standard wxWindows printout class
//                  and is used to print HTML documents.
//--------------------------------------------------------------------------------


class WXDLLEXPORT wxHtmlPrintout : public wxPrintout
{
    public:
        wxHtmlPrintout(const wxString& title = "Printout");
        ~wxHtmlPrintout();

        void SetHtmlText(const wxString& html, const wxString &basepath = wxEmptyString, bool isdir = TRUE); 
                // prepares the class for printing this html document.
                // Must be called before using the class, in fact just after constructor
                //
                // basepath is base directory (html string would be stored there if it was in
                // file). It is used to determine path for loading images, for example.
                // isdir is FALSE if basepath is filename, TRUE if it is directory name
                // (see wxFileSystem for detailed explanation)
                
        void SetHtmlFile(const wxString &htmlfile);
                // same as SetHtmlText except that it takes regular file as the parameter
                
        void SetHeader(const wxString& header, int pg = wxPAGE_ALL);
        void SetFooter(const wxString& footer, int pg = wxPAGE_ALL);
                // sets header/footer for the document. The argument is interpreted as HTML document.
                // You can use macros in it:
                //   @PAGENUM@ is replaced by page number
                //   @PAGESCNT@ is replaced by total number of pages
                //
                // pg is one of wxPAGE_ODD, wxPAGE_EVEN and wx_PAGE_ALL constants.
                // You can set different header/footer for odd and even pages

        void SetMargins(float top = 25.2, float bottom = 25.2, float left = 25.2, float right = 25.2, 
                        float spaces = 5);
                // sets margins in milimeters. Defaults to 1 inch for margins and 0.5cm for space
                // between text and header and/or footer

        // wxPrintout stuff:        
        bool OnPrintPage(int page);
        bool HasPage(int page);
        void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);
        void OnBeginPrinting();
        
    private:

        void RenderPage(wxDC *dc, int page);
                // renders one page into dc
        wxString TranslateHeader(const wxString& instr, int page);
                // substitute @PAGENUM@ and @PAGESCNT@ by real values
        void CountPages();
                // counts pages and fills m_NumPages and m_PageBreaks
        
        
    private:
        int m_NumPages;
        int m_PageBreaks[wxHTML_PRINT_MAX_PAGES];

        wxString m_Document, m_BasePath;
        bool m_BasePathIsDir;
        wxString m_Headers[2], m_Footers[2];

        int m_HeaderHeight, m_FooterHeight;
        wxHtmlDCRenderer *m_Renderer, *m_RendererHdr;
        float m_MarginTop, m_MarginBottom, m_MarginLeft, m_MarginRight, m_MarginSpace;
};





//--------------------------------------------------------------------------------
// wxHtmlEasyPrinting
//                  This class provides very simple interface to printing 
//                  architecture. It allows you to print HTML documents only
//                  with very few commands. 
//
//                  Note : do not create this class on stack only.
//                         You should create an instance on app startup and 
//                         use this instance for all printing. Why? The class
//                         stores page&printer settings in it.
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlEasyPrinting : public wxObject
{
    public:

        wxHtmlEasyPrinting(const wxString& name = "Printing", wxFrame *parent_frame = NULL);
        ~wxHtmlEasyPrinting();
        
        void PreviewFile(const wxString &htmlfile);
        void PreviewText(const wxString &htmltext, const wxString& basepath = wxEmptyString);
                // Preview file / html-text for printing
                // (and offers printing)
                // basepath is base directory for opening subsequent files (e.g. from <img> tag)
                
        void PrintFile(const wxString &htmlfile);
        void PrintText(const wxString &htmltext, const wxString& basepath = wxEmptyString);
                // Print file / html-text w/o preview
                
        void PrinterSetup();
        void PageSetup();
                // pop up printer or page setup dialog

        void SetHeader(const wxString& header, int pg = wxPAGE_ALL);
        void SetFooter(const wxString& footer, int pg = wxPAGE_ALL);
                // sets header/footer for the document. The argument is interpreted as HTML document.
                // You can use macros in it:
                //   @PAGENUM@ is replaced by page number
                //   @PAGESCNT@ is replaced by total number of pages
                //
                // pg is one of wxPAGE_ODD, wxPAGE_EVEN and wx_PAGE_ALL constants.
                // You can set different header/footer for odd and even pages
                
        wxPrintData *GetPrintData() {return m_PrintData;}
        wxPageSetupDialogData *GetPageSetupData() {return m_PageSetupData;}
                // return page setting data objects. 
                // (You can set their parameters.)
                
    private:
    
        wxHtmlPrintout *CreatePrintout();
        void DoPreview(wxHtmlPrintout *printout1, wxHtmlPrintout *printout2);
        void DoPrint(wxHtmlPrintout *printout);
    
        wxPrintData *m_PrintData;
        wxPageSetupDialogData *m_PageSetupData;
        wxString m_Name;
        wxString m_Headers[2], m_Footers[2];
        wxFrame *m_Frame;
};




#endif  // wxUSE_HTML & wxUSE_PRINTING_ARCHITECTURE

#endif // _WX_HTMPRINT_H_

