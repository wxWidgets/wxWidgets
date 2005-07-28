/////////////////////////////////////////////////////////////////////////////
// Name:        captionbar.h
// Purpose:     wxFoldPanel
// Author:      Jorgen Bodde
// Modified by: ABX - 19/12/2004 : possibility of horizontal orientation
//                               : wxWidgets coding standards
// Created:     22/06/2004
// RCS-ID:      $Id$
// Copyright:   (c) Jorgen Bodde
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __FOLDPANELBAR_H__
#define __FOLDPANELBAR_H__

#ifdef WXMAKINGDLL_FOLDBAR
    #define WXDLLIMPEXP_FOLDBAR WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_FOLDBAR WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_FOLDBAR
#endif


#define wxFPB_EXTRA_X 10
#define wxFPB_EXTRA_Y 4
#define wxFPB_BMP_RIGHTSPACE 2    // pixels of the bmp to be aligned from the right filled with space

enum
{
    /** Specifies the bars as gradient vertical filled caption bars going from top to bottom. The gradient
        starts with first colour, and ends with second colour */
    wxCAPTIONBAR_GRADIENT_V = 1,
    /** Specifies the gradient going from left to right. The gradient starts with first colour, and
        ends with second colour on the right */
    wxCAPTIONBAR_GRADIENT_H,
    /** Fills the captionbar with a single colour. The first colour is used for this fill */
    wxCAPTIONBAR_SINGLE,
    /** Draws a rectangle only using the second colour. The first colour is not used*/
    wxCAPTIONBAR_RECTANGLE,
    /** Fills the captionbar with a single colour (first colour) and draws a rectangle around it
        using the second colour. */
    wxCAPTIONBAR_FILLED_RECTANGLE
};

/** \class wxCaptionBarStyle
    This class encapsulates the styles you wish to set for the wxCaptionBar (this is the part of the wxFoldPanel
    where the caption is displayed). It can either be applied at creation time be reapplied when styles need to
    be changed.

    At construction time, all styles are set to their default transparency. This means none of the styles will be
    applied to the wxCaptionBar in question, meaning it will be created using the default internals. When setting i.e
    the color, font or panel style, these styles become active to be used.
*/
class WXDLLIMPEXP_FOLDBAR wxCaptionBarStyle
{
private:
    // boolean flags for default transparency on styles
    bool m_firstColourUsed,
         m_secondColourUsed,
         m_textColourUsed,
         m_captionFontUsed,
         m_captionStyleUsed;

    wxFont m_captionFont;
    wxColour m_firstColour, m_secondColour, m_textColour;

    int m_captionStyle;

public:
    /** Default constructor for this class */
    wxCaptionBarStyle() {
        ResetDefaults();
    };

    ~wxCaptionBarStyle() {

    };

    void ResetDefaults() {
        m_firstColourUsed = false;
        m_secondColourUsed = false;
        m_textColourUsed = false;
        m_captionFontUsed = false;
        m_captionStyleUsed = false;
        m_captionStyle = wxCAPTIONBAR_GRADIENT_V;
    };

    /** Copy operator. Only the styles in use in the source object are being copied to the destination object. All other
        styles are not copied */
    void operator=(const wxCaptionBarStyle &s) {
        if(s.m_captionStyleUsed)
        {
            m_captionStyleUsed = true;
            m_captionStyle = s.m_captionStyle;
        }
        if(s.m_captionFontUsed)
        {
            m_captionFontUsed = true;
            m_captionFont = s.m_captionFont;
        }
        if(s.m_firstColourUsed)
        {
            m_firstColourUsed = true;
            m_firstColour = s.m_firstColour;
        }
        if(s.m_secondColourUsed)
        {
            m_secondColourUsed = true;
            m_secondColour = s.m_secondColour;
        }
        if(s.m_textColourUsed)
        {
            m_textColourUsed = true;
            m_textColour = s.m_textColour;
        }
    };

    // ------- CaptionBar Font -------

    /** Set font for the caption bar. If this is not set, the font property is undefined
        and will not be used. Use CaptionFontUsed() to check if this style is used */
    void SetCaptionFont(const wxFont &font) {
        m_captionFont = font;
        m_captionFontUsed = true;
    };

    /** Checks if the caption bar font is set */
    bool CaptionFontUsed() const {
        return m_captionFontUsed;
    };

    /** Returns the font for the caption bar. Please be warned this will result in an assertion failure when
        this property is not previously set
        \sa SetCaptionFont(), CaptionFontUsed() */
    wxFont GetCaptionFont() const {
        wxASSERT(m_captionFontUsed);
        return m_captionFont;
    };

    // ------- FirstColour -------

    /** Set first colour for the caption bar. If this is not set, the colour property is
        undefined and will not be used. Use FirstColourUsed() to check if this
        style is used */
    void SetFirstColour(const wxColour &col) {
        m_firstColour = col;
        m_firstColourUsed = true;
    };

    /** Checks if the first colour of the caption bar is set */
    bool FirstColourUsed() const {
        return m_firstColourUsed;
    };

    /** Returns the first colour for the caption bar. Please be warned this will
        result in an assertion failure when this property is not previously set.
        \sa SetCaptionFirstColour(), CaptionFirstColourUsed() */
    wxColour GetFirstColour() const {
        wxASSERT(m_firstColourUsed);
        return m_firstColour;
    };

    // ------- SecondColour -------

    /** Set second colour for the caption bar. If this is not set, the colour property is undefined and
        will not be used. Use SecondColourUsed() to check if this style is used */
    void SetSecondColour(const wxColour &col) {
        m_secondColour = col;
        m_secondColourUsed = true;
    };

    /** Checks if the second colour of the caption bar is set */
    bool SecondColourUsed() const {
        return m_secondColourUsed;
    };

    /** Returns the second colour for the caption bar. Please be warned this will result in
        an assertion failure when this property is not previously set.
        \sa SetSecondColour(), SecondColourUsed() */
    wxColour GetSecondColour() const {
        wxASSERT(m_secondColourUsed);
        return m_secondColour;
    };

    // ------- Caption Text Colour -------

    /** Set caption colour for the caption bar. If this is not set, the colour property is
        undefined and will not be used. Use CaptionColourUsed() to check if this style is used */
    void SetCaptionColour(const wxColour &col) {
        m_textColour = col;
        m_textColourUsed = true;
    };

    /** Checks if the caption colour of the caption bar is set */
    bool CaptionColourUsed() const {
        return m_textColourUsed;
    };

    /** Returns the caption colour for the caption bar. Please be warned this will
        result in an assertion failure when this property is not previously set.
        \sa SetCaptionColour(), CaptionColourUsed() */
    wxColour GetCaptionColour() const {
        wxASSERT(m_textColourUsed);
        return m_textColour;
    };

    // ------- CaptionStyle  -------

    /** Set caption style for the caption bar. If this is not set, the property is
        undefined and will not be used. Use CaptionStyleUsed() to check if this style is used.
        The following styles can be applied:
        - wxCAPTIONBAR_GRADIENT_V: Draws a vertical gradient from top to bottom
        - wxCAPTIONBAR_GRADIENT_H: Draws a horizontal gradient from left to right
        - wxCAPTIONBAR_SINGLE: Draws a single filled rectangle to draw the caption
        - wxCAPTIONBAR_RECTANGLE: Draws a single colour with a rectangle around the caption
        - wxCAPTIONBAR_FILLED_RECTANGLE: Draws a filled rectangle and a border around it
    */
    void SetCaptionStyle(int style) {
        m_captionStyle = style;
        m_captionStyleUsed = true;
    };

    /** Checks if the caption style of the caption bar is set */
    bool CaptionStyleUsed() const {
        return m_captionStyleUsed;
    };

    /** Returns the caption style for the caption bar. Please be warned this will
        result in an assertion failure when this property is not previously set.
        \sa SetCaptionStyle(), CaptionStyleUsed() */
    int GetCaptionStyle() const {
        wxASSERT(m_captionStyleUsed);
        return m_captionStyle;
    };
};

#ifndef _NO_CAPTIONBAR_

/** \class wxCaptionBar
    This class is a graphical caption component that consists of a caption and a clickable arrow.

    The wxCaptionBar fires an event EVT_CAPTIONBAR which is a wxCaptionBarEvent. This event can be caught
    and the parent window can act upon the collapsed or expanded state of the bar (which is actually just
    the icon which changed). The parent panel can reduce size or expand again.
*/

#include <wx/imaglist.h>

/** Defines an empty captionbar style */
#define wxEmptyCaptionBarStyle wxCaptionBarStyle()

class WXDLLIMPEXP_FOLDBAR wxCaptionBar: public wxWindow
{
private:
    wxString m_caption;
    wxImageList *m_foldIcons;
    wxSize m_oldSize;
    //wxFont m_captionFont;
    int m_rightIndent;
    int m_iconWidth, m_iconHeight;
    //int m_captionStyle;

    //wxColour m_firstColour, m_secondColour, m_textColour;

    /** True when the caption is in collapsed state (means at the bottom of the wxFoldPanel */
    bool m_collapsed;

    wxCaptionBarStyle m_captionStyle;

    /** Fills the background of the caption with either a gradient, or a solid color */
    void FillCaptionBackground(wxPaintDC &dc);

    /* Draw methods */
    void DrawHorizontalGradient(wxDC &dc, const wxRect &rect );
    void DrawVerticalGradient(wxDC &dc, const wxRect &rect );
    void DrawSingleColour(wxDC &dc, const wxRect &rect );
    void DrawSingleRectangle(wxDC &dc, const wxRect &rect );

    void RedrawIconBitmap();

    void ApplyCaptionStyle(const wxCaptionBarStyle &cbstyle, bool applyDefault);

public:
    /** Constructor of wxCaptionBar. To create a wxCaptionBar with the arrow images, simply pass an image list
        which contains at least two bitmaps. The bitmaps contain the expanded and collapsed icons needed to
        represent it's state. If you don't want images, simply pass a null pointer and the bitmap is disabled. */
    wxCaptionBar(wxWindow* parent, const wxString &caption, wxImageList *images,
                 wxWindowID id = wxID_ANY, const wxCaptionBarStyle &cbstyle = wxEmptyCaptionBarStyle,
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER);

    ~wxCaptionBar();

    /** Set wxCaptionBar styles with wxCapionBarSyle class. All styles that are actually set, are applied. If you
        set applyDefault to true, all other (not defined) styles will be set to default. If it is false,
        the styles which are not set in the wxCaptionBarStyle will be ignored */
    void SetCaptionStyle(bool applyDefault, wxCaptionBarStyle style = wxEmptyCaptionBarStyle) {
        ApplyCaptionStyle(style, applyDefault);
        Refresh();
    };

    /** Returns the current style of the captionbar in a wxCaptionBarStyle class. This can be used to change and set back the
        changes. */
    wxCaptionBarStyle GetCaptionStyle() {
        return m_captionStyle;
    };

    bool IsVertical() const;

#if 0
    /** Sets a pointer to an image list resource (a non owned pointer) to the collapsed and expand icon bitmap.
        The reason why it will be assigned a pointer is that it is very likely that multiple caption bars will
        be used and if they all have their own bitmap resources it will eat up more memory then needed. It will
        also ease the use of shared icon change, when there is any need to.

        If no wxImageList is assigned, there will be no fold icons and only the doubleclick on the panel
        will work to collapse / expand.

        The image list must contain 2 bitmaps. Index 0 will be the expanded state, and index 1 will be the
        collapsed state of the bitmap. The size of the bitmap is taken in account when the minimal height and
        widht is calculated.

        The bitmaps must be the second thing to be done before using it (SetRightIndent should be the first thing),
        make sure if the icons are larger than the font, that the parent of this window gets a Fit call to resize
        all the windows accordingly */

    void SetFoldIcons(wxImageList *images) {
        m_foldIcons = images;
        m_iconWidth = m_iconHeight = 0;
        if(m_foldIcons)
            m_foldIcons->GetSize(0, m_iconWidth, m_iconHeight);

        Refresh();
    };

#endif

    /** Returns wether the status of the bar is expanded or collapsed */
    bool IsCollapsed() const {
        return m_collapsed;
    };

    /** Sets the amount of pixels on the right from which the bitmap is trailing. If this is 0, it will be
        drawn all the way to the right, default is equal to wxFPB_BMP_RIGHTSPACE. Assign this before
        assigning an image list to prevent a redraw */

    void SetRightIndent(int pixels) {
        wxCHECK2(pixels >= 0, return);
        m_rightIndent = pixels;
        // issue a refresh (if we have a bmp)
        if(m_foldIcons)
            Refresh();
    };


    /** Return the best size for this panel, based upon the font assigned to this window, and the
        caption string */
    wxSize DoGetBestSize() const;

    /** This sets the internal state / representation to collapsed. This does not trigger a wxCaptionBarEvent
        to be sent to the parent */
    void Collapse() {
        m_collapsed = true;
        RedrawIconBitmap();
    };

    /** This sets the internal state / representation to expanded. This does not trigger a wxCaptionBarEvent
        to be sent to the parent */
    void Expand() {
        m_collapsed = false;
        RedrawIconBitmap();
    };

    void SetBoldFont() {
        GetFont().SetWeight(wxBOLD);
    };

    void SetNormalFont() {
        GetFont().SetWeight(wxNORMAL);
    };


private:
    /** The paint event for flat or gradient fill */
    void OnPaint(wxPaintEvent& event);

    /** For clicking the icon, the mouse event must be intercepted */
    void OnMouseEvent(wxMouseEvent& event);

    /** Maybe when focus (don't know how yet) a cursor left or backspace will collapse or expand */
    void OnChar(wxKeyEvent& event);

    void OnSize(wxSizeEvent &event);


protected:
    DECLARE_NO_COPY_CLASS(wxCaptionBar)
    DECLARE_EVENT_TABLE()
};

/***********************************************************************************************************/

/** \class wxCaptionBarEvent
    This event will be sent when a EVT_CAPTIONBAR is mapped in the parent. It is to notify the parent
    that the bar is now in collapsed or expanded state. The parent should re-arrange the associated
    windows accordingly */

class WXDLLIMPEXP_FOLDBAR wxCaptionBarEvent : public wxCommandEvent
{

private:
    bool m_collapsed;
    wxCaptionBar *m_captionBar;
    void *m_tag;

public:
    wxCaptionBarEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
        , m_collapsed(false)
        , m_captionBar(NULL)
        , m_tag(0)
    { }

    /** Constructor for clone copy */
    wxCaptionBarEvent(const wxCaptionBarEvent &event);

    /** Clone function */
    virtual wxEvent *Clone() const {
        return new wxCaptionBarEvent(*this);
    };

    /** Returns wether the bar is expanded or collapsed. True means expanded */
    bool GetFoldStatus() const {
        wxCHECK(m_captionBar, false);
        return !m_captionBar->IsCollapsed();
    };

    /** Returns the bar associated with this event */
    wxCaptionBar *GetCaptionBar() const {
        return m_captionBar;
    };

    void SetTag(void *tag) {
        m_tag = tag;
    };

    void *GetTag() const {
        return m_tag;
    };

    /** Sets the bar associated with this event, should not used
        by any other then the originator of the event */
    void SetCaptionBar(wxCaptionBar *bar) {
        m_captionBar = bar;
    };

    DECLARE_DYNAMIC_CLASS(wxCaptionBarEvent)

};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_FOLDBAR, wxEVT_CAPTIONBAR, 7777)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxCaptionBarEventFunction)(wxCaptionBarEvent&);

#define EVT_CAPTIONBAR(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_CAPTIONBAR, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent(wxCaptionBarEventFunction, & fn), \
        (wxObject *) NULL \
    ),

#endif // _NO_CAPTIONBAR_

#endif
