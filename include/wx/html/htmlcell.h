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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "htmlcell.h"
#endif

#include "wx/defs.h"

#if wxUSE_HTML

#include "wx/html/htmltag.h"
#include "wx/html/htmldefs.h"
#include "wx/window.h"


class WXDLLEXPORT wxHtmlLinkInfo;
class WXDLLEXPORT wxHtmlCell;
class WXDLLEXPORT wxHtmlContainerCell;


// wxHtmlSelection is data holder with information about text selection.
// Selection is defined by two positions (beginning and end of the selection)
// and two leaf(!) cells at these positions.
class WXDLLEXPORT wxHtmlSelection
{
public:
    wxHtmlSelection() 
        : m_fromPos(wxDefaultPosition), m_toPos(wxDefaultPosition),
          m_fromCell(NULL), m_toCell(NULL) {}

    void Set(const wxPoint& fromPos, wxHtmlCell *fromCell,
             const wxPoint& toPos, wxHtmlCell *toCell)
    {
        m_fromCell = fromCell;
        m_toCell = toCell;
        m_fromPos = fromPos;
        m_toPos = toPos;
    }
    
    const wxHtmlCell *GetFromCell() const { return m_fromCell; }
    const wxHtmlCell *GetToCell() const { return m_toCell; }
    
    // these values are *relative* to From/To cell's origin:
    const wxPoint& GetFromPos() const { return m_fromPos; }
    const wxPoint& GetToPos() const { return m_toPos; }

    const bool IsEmpty() const 
        { return m_fromPos == wxDefaultPosition && 
                 m_toPos == wxDefaultPosition; }

private:
    wxPoint     m_fromPos,   m_toPos;
    wxHtmlCell *m_fromCell, *m_toCell;
};



enum wxHtmlSelectionState
{
    wxHTML_SEL_OUT,     // currently rendered cell is outside the selection
    wxHTML_SEL_IN,      // ... is inside selection
    wxHTML_SEL_CHANGING // ... is the cell on which selection state changes
};

// Selection state is passed to wxHtmlCell::Draw so that it can render itself
// differently e.g. when inside text selection or outside it.
class WXDLLEXPORT wxHtmlRenderingState
{
public:
    wxHtmlRenderingState(wxHtmlSelection *s)
        : m_selection(s), m_selState(wxHTML_SEL_OUT) {}
    wxHtmlSelection *GetSelection() const { return m_selection; }

    void SetSelectionState(wxHtmlSelectionState s) { m_selState = s; }  
    wxHtmlSelectionState GetSelectionState() const { return m_selState; }

    void SetFgColour(const wxColour& c) { m_fgColour = c; }
    const wxColour& GetFgColour() const { return m_fgColour; }
    void SetBgColour(const wxColour& c) { m_bgColour = c; }
    const wxColour& GetBgColour() const { return m_bgColour; }
    
private:
    wxHtmlSelection      *m_selection;
    wxHtmlSelectionState  m_selState;
    wxColour              m_fgColour, m_bgColour;
};

// Flags for wxHtmlCell::FindCellByPos
enum
{
    wxHTML_FIND_TERMINAL    = 0x0001,
    wxHTML_FIND_NONTERMINAL = 0x0002
};


// ---------------------------------------------------------------------------
// wxHtmlCell
//                  Internal data structure. It represents fragments of parsed
//                  HTML page - a word, picture, table, horizontal line and so
//                  on.  It is used by wxHtmlWindow to represent HTML page in
//                  memory.
// ---------------------------------------------------------------------------


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

    const wxString& GetId() const { return m_id; }
    void SetId(const wxString& id) { m_id = id; }

    // returns the link associated with this cell. The position is position
    // within the cell so it varies from 0 to m_Width, from 0 to m_Height
    virtual wxHtmlLinkInfo* GetLink(int WXUNUSED(x) = 0, int WXUNUSED(y) = 0) const
        { return m_Link; }

    // members access methods
    wxHtmlCell *GetNext() const {return m_Next;}

    // members writing methods
    virtual void SetPos(int x, int y) {m_PosX = x, m_PosY = y;}
    void SetLink(const wxHtmlLinkInfo& link);
    void SetNext(wxHtmlCell *cell) {m_Next = cell;}

    // 1. adjust cell's width according to the fact that maximal possible width
    //    is w.  (this has sense when working with horizontal lines, tables
    //    etc.)
    // 2. prepare layout (=fill-in m_PosX, m_PosY (and sometime m_Height)
    //    members) = place items to fit window, according to the width w
    virtual void Layout(int w);

    // renders the cell
    virtual void Draw(wxDC& WXUNUSED(dc),
                      int WXUNUSED(x), int WXUNUSED(y),
                      int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                      wxHtmlRenderingState& WXUNUSED(state)) {}

    // proceed drawing actions in case the cell is not visible (scrolled out of
    // screen).  This is needed to change fonts, colors and so on.
    virtual void DrawInvisible(wxDC& WXUNUSED(dc),
                               int WXUNUSED(x), int WXUNUSED(y),
                               wxHtmlRenderingState& WXUNUSED(state)) {}

    // This method returns pointer to the FIRST cell for that
    // the condition
    // is true. It first checks if the condition is true for this
    // cell and then calls m_Next->Find(). (Note: it checks
    // all subcells if the cell is container)
    // Condition is unique condition identifier (see htmldefs.h)
    // (user-defined condition IDs should start from 10000)
    // and param is optional parameter
    // Example : m_Cell->Find(wxHTML_COND_ISANCHOR, "news");
    //   returns pointer to anchor news
    virtual const wxHtmlCell* Find(int condition, const void* param) const;

    // This function is called when mouse button is clicked over the cell.
    //
    // Parent is pointer to wxHtmlWindow that generated the event
    // HINT: if this handling is not enough for you you should use
    //       wxHtmlWidgetCell
    virtual void OnMouseClick(wxWindow *parent, int x, int y, const wxMouseEvent& event);

    // This method used to adjust pagebreak position. The parameter is variable
    // that contains y-coordinate of page break (= horizontal line that should
    // not be crossed by words, images etc.). If this cell cannot be divided
    // into two pieces (each one on another page) then it moves the pagebreak
    // few pixels up.
    //
    // Returned value : true if pagebreak was modified, false otherwise
    // Usage : while (container->AdjustPagebreak(&p)) {}
    virtual bool AdjustPagebreak(int *pagebreak,
                                 int *known_pagebreaks = NULL,
                                 int number_of_pages = 0) const;

    // Sets cell's behaviour on pagebreaks (see AdjustPagebreak). Default
    // is true - the cell can be split on two pages
    void SetCanLiveOnPagebreak(bool can) { m_CanLiveOnPagebreak = can; }

    // Returns y-coordinates that contraint the cell, i.e. left is highest
    // and right lowest coordinate such that the cell lays between then.
    // Note: this method does not return meaningful values if you haven't
    //       called Layout() before!
    virtual void GetHorizontalConstraints(int *left, int *right) const;

    // Returns true for simple == terminal cells, i.e. not composite ones.
    // This if for internal usage only and may disappear in future versions!
    virtual bool IsTerminalCell() const { return TRUE; }

    // Find a cell inside this cell positioned at the given coordinates
    // (relative to this's positions). Returns NULL if no such cell exists.
    // The flag can be used to specify whether to look for terminal or
    // nonterminal cells or both. In either case, returned cell is deepest
    // cell in cells tree that contains [x,y].
    virtual wxHtmlCell *FindCellByPos(wxCoord x, wxCoord y,
                                  unsigned flags = wxHTML_FIND_TERMINAL) const;

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
            // destination address if this fragment is hypertext link, NULL otherwise
    bool m_CanLiveOnPagebreak;
            // true if this cell can be placed on pagebreak, false otherwise
    wxString m_id;
            // unique identifier of the cell, generated from "id" property of tags

    DECLARE_NO_COPY_CLASS(wxHtmlCell)
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
public:
    wxHtmlWordCell(const wxString& word, wxDC& dc);
    void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
              wxHtmlRenderingState& state);

protected:
    wxString m_Word;
};





// Container contains other cells, thus forming tree structure of rendering
// elements. Basic code of layout algorithm is contained in this class.
class WXDLLEXPORT wxHtmlContainerCell : public wxHtmlCell
{
public:
    wxHtmlContainerCell(wxHtmlContainerCell *parent);
    ~wxHtmlContainerCell();

    virtual void Layout(int w);
    virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
                      wxHtmlRenderingState& state);
    virtual void DrawInvisible(wxDC& dc, int x, int y,
                               wxHtmlRenderingState& state);
    virtual bool AdjustPagebreak(int *pagebreak, int *known_pagebreaks = NULL, int number_of_pages = 0) const;

    // insert cell at the end of m_Cells list
    void InsertCell(wxHtmlCell *cell);

    // sets horizontal/vertical alignment
    void SetAlignHor(int al) {m_AlignHor = al; m_LastLayout = -1;}
    int GetAlignHor() const {return m_AlignHor;}
    void SetAlignVer(int al) {m_AlignVer = al; m_LastLayout = -1;}
    int GetAlignVer() const {return m_AlignVer;}

    // sets left-border indentation. units is one of wxHTML_UNITS_* constants
    // what is combination of wxHTML_INDENT_*
    void SetIndent(int i, int what, int units = wxHTML_UNITS_PIXELS);
    // returns the indentation. ind is one of wxHTML_INDENT_* constants
    int GetIndent(int ind) const;
    // returns type of value returned by GetIndent(ind)
    int GetIndentUnits(int ind) const;

    // sets alignment info based on given tag's params
    void SetAlign(const wxHtmlTag& tag);
    // sets floating width adjustment
    // (examples : 32 percent of parent container,
    // -15 pixels percent (this means 100 % - 15 pixels)
    void SetWidthFloat(int w, int units) {m_WidthFloat = w; m_WidthFloatUnits = units; m_LastLayout = -1;}
    void SetWidthFloat(const wxHtmlTag& tag, double pixel_scale = 1.0);
    // sets minimal height of this container.
    void SetMinHeight(int h, int align = wxHTML_ALIGN_TOP) {m_MinHeight = h; m_MinHeightAlign = align; m_LastLayout = -1;}

    void SetBackgroundColour(const wxColour& clr) {m_UseBkColour = TRUE; m_BkColour = clr;}
    // returns background colour (of wxNullColour if none set), so that widgets can
    // adapt to it:
    wxColour GetBackgroundColour();
    void SetBorder(const wxColour& clr1, const wxColour& clr2) {m_UseBorder = TRUE; m_BorderColour1 = clr1, m_BorderColour2 = clr2;}
    virtual wxHtmlLinkInfo* GetLink(int x = 0, int y = 0) const;
    virtual const wxHtmlCell* Find(int condition, const void* param) const;
    virtual void OnMouseClick(wxWindow *parent, int x, int y, const wxMouseEvent& event);
    virtual void GetHorizontalConstraints(int *left, int *right) const;

    // returns pointer to the first cell in container or NULL
    wxHtmlCell* GetFirstCell() const {return m_Cells;}

    // see comment in wxHtmlCell about this method
    virtual bool IsTerminalCell() const { return FALSE; }

    virtual wxHtmlCell *FindCellByPos(wxCoord x, wxCoord y,
                                  unsigned flags = wxHTML_FIND_TERMINAL) const;

protected:
    void UpdateRenderingStatePre(wxHtmlRenderingState& state,
                                 wxHtmlCell *cell) const;
    void UpdateRenderingStatePost(wxHtmlRenderingState& state,
                                  wxHtmlCell *cell) const;
    
protected:
    int m_IndentLeft, m_IndentRight, m_IndentTop, m_IndentBottom;
            // indentation of subcells. There is always m_Indent pixels
            // big space between given border of the container and the subcells
            // it m_Indent < 0 it is in PERCENTS, otherwise it is in pixels
    int m_MinHeight, m_MinHeightAlign;
        // minimal height.
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
    int m_LastLayout;
            // if != -1 then call to Layout may be no-op
            // if previous call to Layout has same argument

    DECLARE_NO_COPY_CLASS(wxHtmlContainerCell)
};





//--------------------------------------------------------------------------------
// wxHtmlColourCell
//                  Color changer.
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlColourCell : public wxHtmlCell
{
public:
    wxHtmlColourCell(const wxColour& clr, int flags = wxHTML_CLR_FOREGROUND) : wxHtmlCell() {m_Colour = clr; m_Flags = flags;}
    virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
                      wxHtmlRenderingState& state);
    virtual void DrawInvisible(wxDC& dc, int x, int y,
                               wxHtmlRenderingState& state);

protected:
    wxColour m_Colour;
    unsigned m_Flags;
};




//--------------------------------------------------------------------------------
// wxHtmlFontCell
//                  Sets actual font used for text rendering
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlFontCell : public wxHtmlCell
{
public:
    wxHtmlFontCell(wxFont *font) : wxHtmlCell() { m_Font = (*font); }
    virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
                      wxHtmlRenderingState& state);
    virtual void DrawInvisible(wxDC& dc, int x, int y,
                               wxHtmlRenderingState& state);

protected:
    wxFont m_Font;
};






//--------------------------------------------------------------------------------
// wxHtmlwidgetCell
//                  This cell is connected with wxWindow object
//                  You can use it to insert windows into HTML page
//                  (buttons, input boxes etc.)
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlWidgetCell : public wxHtmlCell
{
public:
    // !!! wnd must have correct parent!
    // if w != 0 then the m_Wnd has 'floating' width - it adjust
    // it's width according to parent container's width
    // (w is percent of parent's width)
    wxHtmlWidgetCell(wxWindow *wnd, int w = 0);
    ~wxHtmlWidgetCell() { m_Wnd->Destroy(); }
    virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
                      wxHtmlRenderingState& state);
    virtual void DrawInvisible(wxDC& dc, int x, int y,
                               wxHtmlRenderingState& state);
    virtual void Layout(int w);

protected:
    wxWindow* m_Wnd;
    int m_WidthFloat;
            // width float is used in adjustWidth (it is in percents)

    DECLARE_NO_COPY_CLASS(wxHtmlWidgetCell)
};



//--------------------------------------------------------------------------------
// wxHtmlLinkInfo
//                  Internal data structure. It represents hypertext link
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlLinkInfo : public wxObject
{
public:
    wxHtmlLinkInfo() : wxObject()
          { m_Href = m_Target = wxEmptyString; m_Event = NULL, m_Cell = NULL; }
    wxHtmlLinkInfo(const wxString& href, const wxString& target = wxEmptyString) : wxObject()
          { m_Href = href; m_Target = target; m_Event = NULL, m_Cell = NULL; }
    wxHtmlLinkInfo(const wxHtmlLinkInfo& l) : wxObject()
          { m_Href = l.m_Href, m_Target = l.m_Target, m_Event = l.m_Event;
            m_Cell = l.m_Cell; }
    wxHtmlLinkInfo& operator=(const wxHtmlLinkInfo& l)
          { m_Href = l.m_Href, m_Target = l.m_Target, m_Event = l.m_Event;
            m_Cell = l.m_Cell; return *this; }

    void SetEvent(const wxMouseEvent *e) { m_Event = e; }
    void SetHtmlCell(const wxHtmlCell *e) { m_Cell = e; }

    wxString GetHref() const { return m_Href; }
    wxString GetTarget() const { return m_Target; }
    const wxMouseEvent* GetEvent() const { return m_Event; }
    const wxHtmlCell* GetHtmlCell() const { return m_Cell; }

private:
    wxString m_Href, m_Target;
    const wxMouseEvent *m_Event;
    const wxHtmlCell *m_Cell;
};




#endif // wxUSE_HTML

#endif // _WX_HTMLCELL_H_

