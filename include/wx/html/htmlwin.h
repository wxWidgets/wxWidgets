/////////////////////////////////////////////////////////////////////////////
// Name:        htmlwin.h
// Purpose:     wxHtmlWindow class for parsing & displaying HTML
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_HTMLWIN_H_
#define _WX_HTMLWIN_H_

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#if wxUSE_HTML

#include "wx/window.h"
#include "wx/scrolwin.h"
#include "wx/config.h"
#include "wx/treectrl.h"
#include "wx/html/winpars.h"
#include "wx/html/htmlcell.h"
#include "wx/filesys.h"
#include "wx/html/htmlfilt.h"


//--------------------------------------------------------------------------------
// wxHtmlWindow
//                  (This is probably the only class you will directly use.)
//                  Purpose of this class is to display HTML page (either local
//                  file or downloaded via HTTP protocol) in a window. Width
//                  of window is constant - given in constructor - virtual height
//                  is changed dynamicly depending on page size.
//                  Once the window is created you can set it's content by calling
//                  SetPage(text) or LoadPage(filename).
//--------------------------------------------------------------------------------


// item of history list
class WXDLLEXPORT HtmlHistoryItem : public wxObject
{
    public:
        HtmlHistoryItem(const wxString& p, const wxString& a) {m_Page = p, m_Anchor = a, m_Pos = 0;}
        int GetPos() const {return m_Pos;}
        void SetPos(int p) {m_Pos = p;}
        const wxString& GetPage() const {return m_Page;}
        const wxString& GetAnchor() const {return m_Anchor;}

    private:
        wxString m_Page;
        wxString m_Anchor;
        int m_Pos;
};


WX_DECLARE_EXPORTED_OBJARRAY(HtmlHistoryItem, HtmlHistoryArray);



class WXDLLEXPORT wxHtmlWindow : public wxScrolledWindow
{
    DECLARE_DYNAMIC_CLASS(wxHtmlWindow)

    public:
        wxHtmlWindow() : wxScrolledWindow() {};
        wxHtmlWindow(wxWindow *parent, wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                     long style = wxHW_SCROLLBAR_AUTO,
                     const wxString& name = "htmlWindow");
        ~wxHtmlWindow();

        bool SetPage(const wxString& source);
                // Set HTML page and display it. !! source is HTML document itself,
                // it is NOT address/filename of HTML document. If you want to
                // specify document location, use LoadPage() istead
                // Return value : FALSE if an error occured, TRUE otherwise

        bool LoadPage(const wxString& location);
                // Load HTML page from given location. Location can be either
                // a) /usr/wxGTK2/docs/html/wx.htm
                // b) http://www.somewhere.uk/document.htm
                // c) ftp://ftp.somesite.cz/pub/something.htm
                // In case there is no prefix (http:,ftp:), the method
                // will try to find it itself (1. local file, then http or ftp)
                // After the page is loaded, the method calls SetPage() to display it.
                // Note : you can also use path relative to previously loaded page
                // Return value : same as SetPage

        wxString GetOpenedPage() const {return m_OpenedPage;}
                // Returns full location of opened page
        wxString GetOpenedAnchor() const {return m_OpenedAnchor;}
                // Returns anchor within opened page
        wxString GetOpenedPageTitle() const {return m_OpenedPageTitle;}
                // Returns <TITLE> of opened page or empty string otherwise

        void SetRelatedFrame(wxFrame* frame, const wxString& format);
                // sets frame in which page title will  be displayed. Format is format of
                // frame title, e.g. "HtmlHelp : %s". It must contain exactly one %s
        wxFrame* GetRelatedFrame() const {return m_RelatedFrame;}

        void SetRelatedStatusBar(int bar);
                // after(!) calling SetRelatedFrame, this sets statusbar slot where messages
                // will be displayed. Default is -1 = no messages.

        void SetFonts(wxString normal_face, wxString fixed_face, const int *sizes);
                // sets fonts to be used when displaying HTML page.

        virtual void OnSetTitle(const wxString& title);
                // Sets the title of the window
                // (depending on the information passed to SetRelatedFrame() method)

        void SetBorders(int b) {m_Borders = b;}
                // Sets space between text and window borders.

        virtual void ReadCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
                // saves custom settings into cfg config. it will use the path 'path'
                // if given, otherwise it will save info into currently selected path.
                // saved values : things set by SetFonts, SetBorders.
        virtual void WriteCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
                // ...

        bool HistoryBack();
        bool HistoryForward();
                // Goes to previous/next page (in browsing history)
                // Returns TRUE if successful, FALSE otherwise
        void HistoryClear();
                // Resets history

        wxHtmlContainerCell* GetInternalRepresentation() const {return m_Cell;}
                // Returns pointer to conteiners/cells structure.
                // It should be used ONLY when printing

        static void AddFilter(wxHtmlFilter *filter);
                // Adds input filter

        virtual void OnLinkClicked(const wxHtmlLinkInfo& link);
                // called when users clicked on hypertext link. Default behavior is to
                // call LoadPage(loc)

        static void CleanUpStatics();
	        // cleans static variables

        wxHtmlWinParser *GetParser() const { return m_Parser; }
                // return a pointer to the parser.

    protected:
        bool ScrollToAnchor(const wxString& anchor);
                // Scrolls to anchor of this name. (Anchor is #news
                // or #features etc. it is part of address sometimes:
                // http://www.ms.mff.cuni.cz/~vsla8348/wxhtml/index.html#news)
                // Return value : TRUE if anchor exists, FALSE otherwise

        void CreateLayout();
                // prepare layout (= fill m_PosX, m_PosY for fragments) based on actual size of
                // window. This method also setup scrollbars

        void OnDraw(wxDC& dc);
        void OnSize(wxSizeEvent& event);
        void OnMouseEvent(wxMouseEvent& event);
        void OnIdle(wxIdleEvent& event);
        void OnKeyDown(wxKeyEvent& event);

        virtual wxHtmlFilter *GetDefaultFilter() {return new wxHtmlFilterPlainText;}
	        // returns new filter (will be stored into m_DefaultFilter variable)


    protected:
        wxHtmlContainerCell *m_Cell;
                // This is pointer to the first cell in parsed data.
                // (Note: the first cell is usually top one = all other cells are sub-cells of this one)
        wxHtmlWinParser *m_Parser;
                // parser which is used to parse HTML input.
                // Each wxHtmlWindow has it's own parser because sharing one global
                // parser would be problematic (because of reentrancy)
        wxString m_OpenedPage;
                // contains name of actualy opened page or empty string if no page opened
        wxString m_OpenedAnchor;
                // contains name of current anchor within m_OpenedPage
        wxString m_OpenedPageTitle;
                // contains title of actualy opened page or empty string if no <TITLE> tag
        wxFileSystem* m_FS;
                // class for opening files (file system)

        wxFrame *m_RelatedFrame;
        wxString m_TitleFormat;
        int m_RelatedStatusBar;
                // frame in which page title should be displayed & number of it's statusbar
                // reserved for usage with this html window

        int m_Borders;
                // borders (free space between text and window borders)
                // defaults to 10 pixels.

        int m_Style;

    private:
        bool m_tmpMouseMoved;
                // a flag indicated if mouse moved
                // (if TRUE we will try to change cursor in last call to OnIdle)
        wxHtmlLinkInfo *m_tmpLastLink;
                // contains last link name
        int m_tmpCanDrawLocks;
                // if >0 contents of the window is not redrawn
                // (in order to avoid ugly bliking)

        static wxList m_Filters;
                // list of HTML filters
        static wxHtmlFilter *m_DefaultFilter;
                // this filter is used when no filter is able to read some file

        HtmlHistoryArray m_History;
        int m_HistoryPos;
                // browser history
        bool m_HistoryOn;
                // if this FLAG is false, items are not added to history


    DECLARE_EVENT_TABLE()
};


#endif

#endif // _WX_HTMLWIN_H_
