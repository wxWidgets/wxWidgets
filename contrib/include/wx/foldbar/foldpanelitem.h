/////////////////////////////////////////////////////////////////////////////
// Name:        foldpanelitem.h
// Purpose:     wxFoldPanel
// Author:      Jorgen Bodde
// Modified by:
// Created:     22/06/2004
// RCS-ID:      $Id$
// Copyright:   (c) Jorgen Bodde
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXFOLDPANELITEM_H__
#define __WXFOLDPANELITEM_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "captionbar.h"

#define wxFPB_ALIGN_LEFT  0 
#define wxFPB_ALIGN_WIDTH 1

#define wxFPB_DEFAULT_LEFTSPACING  5
#define wxFPB_DEFAULT_RIGHTSPACING 10
#define wxFPB_DEFAULT_YSPACING     8

#define wxFPB_DEFAULT_LEFTLINESPACING  2
#define wxFPB_DEFAULT_RIGHTLINESPACING 2

class wxFoldWindowItem
{
private:
    wxWindow *_wnd;
    int _type, _flags;
    int _leftSpacing,
        _rightSpacing,
        _ySpacing;
    int _lineWidth, _lineY;
    wxColour _sepLineColour;

public:
    enum 
    {
        WINDOW = 0,
        SEPARATOR
    };

    // wxWindow constructor. This initialises the class as a wxWindow type
    wxFoldWindowItem(wxWindow *wnd, int flags = wxFPB_ALIGN_WIDTH, int ySpacing = wxFPB_DEFAULT_YSPACING, 
                     int leftSpacing = wxFPB_DEFAULT_LEFTSPACING, int rightSpacing = wxFPB_DEFAULT_RIGHTSPACING) 
        : _wnd(wnd)
        , _type(WINDOW)
        , _flags(flags)
        , _leftSpacing(leftSpacing)
        , _rightSpacing(rightSpacing)
        , _ySpacing(ySpacing)
        , _lineWidth(0)
        , _lineY(0)
    {
    };

    // separator constructor. This initialises the class as a separator type
    wxFoldWindowItem(int y, const wxColour &lineColor = *wxBLACK, int ySpacing = wxFPB_DEFAULT_YSPACING, 
                     int leftSpacing = wxFPB_DEFAULT_LEFTLINESPACING, 
                     int rightSpacing = wxFPB_DEFAULT_RIGHTLINESPACING) 
        : _wnd(0)
        , _type(SEPARATOR)
        , _flags(wxFPB_ALIGN_WIDTH)
        , _leftSpacing(leftSpacing)
        , _rightSpacing(rightSpacing)
        , _ySpacing(ySpacing)
        , _lineWidth(0)
        , _lineY(y)
        , _sepLineColour(lineColor)
    {
    };

    // TODO: Make a c'tor for a captioned splitter

    int GetType() const {
        return _type;
    };

    int GetLineY() const {
        return _lineY;
    };

    int GetLineWidth() const {
        return _lineWidth;
    };

    const wxColour &GetLineColour() const {
        return _sepLineColour;
    };

    int GetLeftSpacing() const {
        return _leftSpacing;
    };

    int GetRightSpacing() const {
        return _rightSpacing;
    };

    int GetYSpacing() const {
        return _ySpacing;
    };

    // returns the window height if type is wxFoldWindowItem::WINDOW
    // and returns the total size plus the extra spacing

    int GetWindowHeight() const {
        int value = 0;
        if(_type == WINDOW)
        {
            wxCHECK(_wnd, 0);
            wxSize size = _wnd->GetSize();
            value = size.GetHeight() + _ySpacing;
        }
        else if(_type == SEPARATOR)
            value = 1 + _ySpacing;

        return value;
    };

    // resize the element, whatever it is. A separator or
    // line will be always alligned by width

    void ResizeItem(int width) {
        if((_flags & wxFPB_ALIGN_WIDTH))
        {
            // allign by taking full width
            int myWidth = width - _leftSpacing - _rightSpacing;

            if(myWidth < 0) 
                myWidth = 10; // can't have negative width

            if(_type == SEPARATOR)
                _lineWidth = myWidth;
            else
            {
                wxCHECK2(_wnd, return);
                _wnd->SetSize(wxSize(myWidth, wxDefaultCoord));
            }
        }
    };

};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(wxFoldWindowItem, wxFoldWindowItemArray);

#ifndef _NO_DOXYGEN_

/** \wxFoldPanelItem
    This class is a child sibling of the wxFoldPanelBar class. It will be containing a wxCaptionBar class
    for receiving of events, and a the rest of the area can be populated by a wxPanel derived class. 
*/

class WXDLLIMPEXP_FOLDBAR wxFoldPanelItem: public wxPanel
{
private:
    wxCaptionBar *_captionBar;

    bool _controlCreated;
    int _yUserSize,
        _yPanelSize,
        _yLastInsertPos;
    int _yPos;
    bool _userSized;

private:
    DECLARE_CLASS( wxFoldPanelItem )
    DECLARE_EVENT_TABLE()
    
private:
    wxFoldWindowItemArray _items;

    void OnSize(wxSizeEvent &event);
    void OnPressCaption(wxCaptionBarEvent &event);
    void OnPaint(wxPaintEvent &event);

public:
    // constructors and destructors
    wxFoldPanelItem( wxWindow *parent, const wxString &caption, wxImageList *icons = 0, bool collapsedInitially = false, 
                     const wxCaptionBarStyle &style = wxEmptyCaptionBarStyle);
    virtual ~wxFoldPanelItem();
    
    /** Add a window item to the list of items on this panel. The flags are wxFPB_ALIGN_LEFT for a non sizing
        window element, and wxFPB_ALIGN_WIDTH for a width alligned item. The ySpacing parameter reserves a number
        of pixels before the window element, and leftSpacing is an indent. rightSpacing is only relevant when the
        style wxFPB_ALIGN_WIDTH is chosen. */
    void AddWindow(wxWindow *window, int flags, int ySpacing, int leftSpacing, int rightSpacing);
    
    void AddSeparator(const wxColour &color, int ySpacing, int leftSpacing, int rightSpacing);

    /** Repositions this wxFoldPanelBar and reports the height occupied for the next wxFoldPanelBar in the 
        list */
    int Reposition(int y);

    void ResizePanel();

    /** Return expanded or collapsed status. If the panel is expanded, true is returned */
    bool IsExpanded() const {
        return !_captionBar->IsCollapsed();
    };

    /** Return Y pos */

    int GetY() const {
        return _yPos;
    };

    // this should not be called by the user, because it doesn't trigger the parent 
    // to tell it that we are collapsed or expanded, it only changes visual state
    void Collapse() {
        _captionBar->Collapse();
        ResizePanel();
    };

    // this should not be called by the user, because it doesn't trigger the parent 
    // to tell it that we are collapsed or expanded, it only changes visual state
    void Expand() {
        _captionBar->Expand();
        ResizePanel();
    };

    /* Return size of panel */

    int GetPanelHeight() const {
        if(_captionBar->IsCollapsed())
            return GetCaptionHeight();
        else if(_userSized)
            return _yUserSize;
        return _yPanelSize;
    };

    // returns height of caption only. This is for folding calulation
    // purposes

    int GetCaptionHeight() const {
        wxSize size = _captionBar->GetSize();
        return size.GetHeight();
    };

    void ApplyCaptionStyle(const wxCaptionBarStyle &style) {
        wxCHECK2(_captionBar, return);
        _captionBar->SetCaptionStyle(false, style);
    };

    wxCaptionBarStyle GetCaptionStyle() {
        wxCHECK(_captionBar, wxEmptyCaptionBarStyle);
        return _captionBar->GetCaptionStyle();
    };
};


#endif // _NO_DOXYGEN_

#endif // __WXFOLDPANELITEM_H__
