/////////////////////////////////////////////////////////////////////////////
// Name:        htmlcell.h
// Purpose:     wxHtmlCell class is used by wxHtmlWindow/wxHtmlWinParser
//              as a basic visual element of HTML page
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_HTMLCELL_H_
#define _WX_HTMLCELL_H_

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_HTML

#include "wx/html/htmltag.h"
#include "wx/html/htmldefs.h"
#include "wx/window.h"


class wxHtmlLinkInfo;
class wxHtmlCell;
class wxHtmlContainerCell;

//--------------------------------------------------------------------------------
// wxHtmlCell
//                  Internal data structure. It represents fragments of parsed HTML
//                  page - a word, picture, table, horizontal line and so on.
//                  It is used by wxHtmlWindow to represent HTML page in memory.
//--------------------------------------------------------------------------------


class WXDLLEXPORT wxHtmlCell : public wxObject
{
    public:
        wxHtmlCell();
        virtual ~wxHtmlCell();

        void SetParent(wxHtmlContainerCell *p) {m_Parent = p;}
        wxHtmlContainerCell *GetParent() const {return m_Parent;}

        int GetPosX() const {return m_PosX;}
        int GetPosY() const {return m_PosY;}
        int GetWidth() const {return m_Width;}
        int GetHeight() const {return m_Height;}
        int GetDescent() const {return m_Descent;}
        virtual wxHtmlLinkInfo* GetLink(int WXUNUSED(x) = 0,
                                 int WXUNUSED(y) = 0) const
            { return m_Link; }
                // returns the link associated with this cell. The position is position within
                // the cell so it varies from 0 to m_Width, from 0 to m_Height
        wxHtmlCell *GetNext() const {return m_Next;}
                // members access methods

        virtual void SetPos(int x, int y) {m_PosX = x, m_PosY = y;}
        void SetLink(const wxHtmlLinkInfo& link);
        void SetNext(wxHtmlCell *cell) {m_Next = cell;}
                // members writin methods

        virtual void Layout(int w) {SetPos(0, 0); if (m_Next) m_Next -> Layout(w);};
                // 1. adjust cell's width according to the fact that maximal possible width is w.
                //    (this has sense when working with horizontal lines, tables etc.)
                // 2. prepare layout (=fill-in m_PosX, m_PosY (and sometime m_Height) members)
                //    = place items to fit window, according to the width w

        virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2) {if (m_Next) m_Next -> Draw(dc, x, y, view_y1, view_y2);}
                // renders the cell

        virtual void DrawInvisible(wxDC& dc, int x, int y) {if (m_Next) m_Next -> DrawInvisible(dc, x, y);};
                // proceed drawing actions in case the cell is not visible (scrolled out of screen).
                // This is needed to change fonts, colors and so on

        virtual const wxHtmlCell* Find(int condition, const void* param) const {if (m_Next) return m_Next -> Find(condition, param); else return NULL;}
                // This method returns pointer to the FIRST cell for that
                // the condition
                // is true. It first checks if the condition is true for this
                // cell and then calls m_Next -> Find(). (Note: it checks
                // all subcells if the cell is container)
                // Condition is unique condition identifier (see htmldefs.h)
                // (user-defined condition IDs should start from 10000)
                // and param is optional parameter
                // Example : m_Cell -> Find(wxHTML_COND_ISANCHOR, "news");
                //   returns pointer to anchor news

        virtual void OnMouseClick(wxWindow *parent, int x, int y, bool left, bool middle, bool right);
                // This function is called when mouse button is clicked over the cell.
                // left, middle, right are flags indicating whether the button was or wasn't
                // pressed.
                // Parent is pointer to wxHtmlWindow that generated the event
                // HINT: if this handling is not enough for you you should use
                //       wxHtmlBinderCell
                
        virtual bool AdjustPagebreak(int *pagebreak);
                // This method used to adjust pagebreak position. The parameter is
                // variable that contains y-coordinate of page break (= horizontal line that
                // should not be crossed by words, images etc.). If this cell cannot be divided
                // into two pieces (each one on another page) then it moves the pagebreak
                // few pixels up.
                //
                // Returned value : true if pagebreak was modified, false otherwise
                // Usage : while (container->AdjustPagebreak(&p)) {}
                
        void SetCanLiveOnPagebreak(bool can) {m_CanLiveOnPagebreak = can;}
                // Sets cell's behaviour on pagebreaks (see AdjustPagebreak). Default
                // is true - the cell can be split on two pages


    protected:
        wxHtmlCell *m_Next;
                // pointer to the next cell
        wxHtmlContainerCell *m_Parent;
            // pointer to parent cell
        long m_Width, m_Height, m_Descent;
                // dimensions of fragment
                // m_Descent is used to position text&images..
        long m_PosX, m_PosY;
                // position where the fragment is drawn
        wxHtmlLinkInfo *m_Link;
                // destination address if this fragment is hypertext link, "" otherwise
        bool m_CanLiveOnPagebreak;
                // true if this cell can be placed on pagebreak, false otherwise

};




//--------------------------------------------------------------------------------
// Inherited cells:
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
// wxHtmlWordCell
//                  Single word in input stream.
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlWordCell : public wxHtmlCell
{
    protected:
        wxString m_Word;

    public:
        wxHtmlWordCell(const wxString& word, wxDC& dc);
        void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2);
};





//--------------------------------------------------------------------------------
// wxHtmlContainerCell
//                  Container - it contains other cells. Basic of layout algorithm.
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlContainerCell : public wxHtmlCell
{
    protected:
        int m_IndentLeft, m_IndentRight, m_IndentTop, m_IndentBottom;
                // indentation of subcells. There is always m_Indent pixels
                // big space between given border of the container and the subcells
                // it m_Indent < 0 it is in PERCENTS, otherwise it is in pixels
        int m_MinHeight, m_MinHeightAlign;
            // minimal height.
        int m_MaxLineWidth;
            // maximal widht of line. Filled during Layout()
        wxHtmlCell *m_Cells, *m_LastCell;
                // internal cells, m_Cells points to the first of them, m_LastCell to the last one.
                // (LastCell is needed only to speed-up InsertCell)
        int m_AlignHor, m_AlignVer;
                // alignment horizontal and vertical (left, center, right)
        int m_WidthFloat, m_WidthFloatUnits;
                // width float is used in adjustWidth
        bool m_UseBkColour;
        wxColour m_BkColour;
                // background color of this container
        bool m_UseBorder;
        wxColour m_BorderColour1, m_BorderColour2;
                // borders color of this container

    public:
        wxHtmlContainerCell(wxHtmlContainerCell *parent);
        ~wxHtmlContainerCell() {if (m_Cells) delete m_Cells;}

        virtual void Layout(int w);
        virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2);
        virtual void DrawInvisible(wxDC& dc, int x, int y);
        virtual bool AdjustPagebreak(int *pagebreak);

        void InsertCell(wxHtmlCell *cell);
                // insert cell at the end of m_Cells list
        void SetAlignHor(int al) {m_AlignHor = al;}
        int GetAlignHor() const {return m_AlignHor;}
        void SetAlignVer(int al) {m_AlignVer = al;}
                // sets horizontal/vertical alignment
        int GetAlignVer() const {return m_AlignVer;}
        void SetIndent(int i, int what, int units = wxHTML_UNITS_PIXELS);
                // sets left-border indentation. units is one of wxHTML_UNITS_* constants
                // what is combination of wxHTML_INDENT_*
        int GetIndent(int ind) const;
                // returns the indentation. ind is one of wxHTML_INDENT_* constants
        int GetIndentUnits(int ind) const;
                // returns type of value returned by GetIndent(ind)
        void SetAlign(const wxHtmlTag& tag);
                // sets alignment info based on given tag's params
        void SetWidthFloat(int w, int units) {m_WidthFloat = w; m_WidthFloatUnits = units;}
        void SetWidthFloat(const wxHtmlTag& tag, double pixel_scale = 1.0);
                // sets floating width adjustment
                // (examples : 32 percent of parent container,
                // -15 pixels percent (this means 100 % - 15 pixels)
        void SetMinHeight(int h, int align = wxHTML_ALIGN_TOP) {m_MinHeight = h; m_MinHeightAlign = align;}
                // sets minimal height of this container.
        int GetMaxLineWidth() const {return m_MaxLineWidth;}
            // returns maximal line width in this container.
            // Call to this method is valid only after calling
            // Layout()
        void SetBackgroundColour(const wxColour& clr) {m_UseBkColour = TRUE; m_BkColour = clr;}
        void SetBorder(const wxColour& clr1, const wxColour& clr2) {m_UseBorder = TRUE; m_BorderColour1 = clr1, m_BorderColour2 = clr2;}
        virtual wxHtmlLinkInfo* GetLink(int x = 0, int y = 0) const;
        virtual const wxHtmlCell* Find(int condition, const void* param) const;
        virtual void OnMouseClick(wxWindow *parent, int x, int y, bool left, bool middle, bool right);

        wxHtmlCell* GetFirstCell() {return m_Cells;}
                // returns pointer to the first cell in container or NULL
};





//--------------------------------------------------------------------------------
// wxHtmlColourCell
//                  Color changer.
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlColourCell : public wxHtmlCell
{
    public:
        wxColour m_Colour;
        unsigned m_Flags;

        wxHtmlColourCell(wxColour clr, int flags = wxHTML_CLR_FOREGROUND) : wxHtmlCell() {m_Colour = clr; m_Flags = flags;}
        virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2);
        virtual void DrawInvisible(wxDC& dc, int x, int y);
};




//--------------------------------------------------------------------------------
// wxHtmlFontCell
//                  Sets actual font used for text rendering
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlFontCell : public wxHtmlCell
{
    public:
        wxFont m_Font;

        wxHtmlFontCell(wxFont *font) : wxHtmlCell() { m_Font = (*font); }
        virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2);
        virtual void DrawInvisible(wxDC& dc, int x, int y);
};






//--------------------------------------------------------------------------------
// wxHtmlwidgetCell
//                  This cell is connected with wxWindow object
//                  You can use it to insert windows into HTML page
//                  (buttons, input boxes etc.)
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlWidgetCell : public wxHtmlCell
{
    protected:
        wxWindow* m_Wnd;
        int m_WidthFloat;
                // width float is used in adjustWidth (it is in percents)

    public:
        wxHtmlWidgetCell(wxWindow *wnd, int w = 0);
                // !!! wnd must have correct parent!
                // if w != 0 then the m_Wnd has 'floating' width - it adjust
                // it's width according to parent container's width
                // (w is percent of parent's width)
        ~wxHtmlWidgetCell() {if (m_Wnd) m_Wnd -> Destroy(); }
        virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2);
        virtual void DrawInvisible(wxDC& dc, int x, int y);
        virtual void Layout(int w);
};



//--------------------------------------------------------------------------------
// wxHtmlLinkInfo
//                  Internal data structure. It represents hypertext link
//--------------------------------------------------------------------------------

class wxHtmlLinkInfo : public wxObject
{
    public:
        wxHtmlLinkInfo() : wxObject()
                { m_Href = m_Target = wxEmptyString; }
        wxHtmlLinkInfo(const wxString& href, const wxString& target = wxEmptyString) : wxObject()
                { m_Href = href; m_Target = target; }    
        wxHtmlLinkInfo(const wxHtmlLinkInfo& l) 
                { m_Href = l.m_Href, m_Target = l.m_Target; }
        wxHtmlLinkInfo& operator=(const wxHtmlLinkInfo& l) 
                { m_Href = l.m_Href, m_Target = l.m_Target; return *this; }

        wxString GetHref() const { return m_Href; }
        wxString GetTarget() const { return m_Target; }
        
    private:
        wxString m_Href, m_Target;
};




#endif // wxUSE_HTML

#endif // _WX_HTMLCELL_H_

