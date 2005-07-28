/////////////////////////////////////////////////////////////////////////////
// Name:        foldpanelbar.h
// Purpose:     wxFoldPanel
// Author:      Jorgen Bodde
// Modified by: ABX - 19/12/2004 : possibility of horizontal orientation
//                               : wxWidgets coding standards
// Created:     22/06/2004
// RCS-ID:      $Id$
// Copyright:   (c) Jorgen Bodde
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXFOLDPANELBAR_H__
#define __WXFOLDPANELBAR_H__

/** Not yet supported but added for future reference. Single fold forces other panels to close when
    they are open, and only opens the current panel. This will allow the open panel to gain the full
    size left in the client area */
#define wxFPB_SINGLE_FOLD           0x0001

/** All panels are stacked to the bottom. When they are expanded again they show up at the top */
#define wxFPB_COLLAPSE_TO_BOTTOM    0x0002

/** Orientation flag **/
#define wxFPB_HORIZONTAL            wxHORIZONTAL /*  0x0004 */
#define wxFPB_VERTICAL              wxVERTICAL   /*  0x0008 */

/** Not yet supported, but added for future reference. Single fold plus panels will be stacked at the bottom */
#define wxFPB_EXCLUSIVE_FOLD        wxFPB_SINGLE_FOLD | wxFPB_COLLAPSE_TO_BOTTOM

/** Default style of the wxFoldPanelBar */
#define wxFPB_DEFAULT_EXTRASTYLE    0

#define wxFPB_DEFAULT_STYLE         wxTAB_TRAVERSAL | wxNO_BORDER

#include "wx/foldbar/foldpanelitem.h"

/** \class wxFoldPanel
    This class is used to return a reference to the fold panel that is added by wxFoldPanelBar::AddFoldPanel(). Use
    wxFoldPanel::IsOk() to check wether the result is ok to be used in further operations. Use wxFoldPanel::GetItem()
    to obtain a parent window reference to create the controls on you want to add with wxFoldPanelBar::AddFoldPanelWindow().<br><br>
*/

class wxFoldPanel
{
private:
    wxFoldPanelItem *m_item;

public:
    /** Constructor, usually not directly used by the developer. */
    wxFoldPanel(wxFoldPanelItem *item)
        : m_item(item)
    {
    }

    /** Returns true if this is a valid wxFoldPanelItem reference. */
    bool IsOk() const {
        return (m_item != 0);
    };

    /** Copy operator to assign one instance to the other, this is needed because these classes are passed
        as instance not by reference. */
    virtual void operator=(const wxFoldPanel &item) {
        m_item = item.m_item;
    };

#ifndef _NO_DOXYGEN_
    // not allowed to be seen by doxygen
    wxFoldPanelItem *GetItem() const {
        return m_item;
    };
#endif

    /** Use this method to obtain the wxPanel derived class to which you need to add your components. For example;<br>

    \code
        wxFoldPanel item = m_pnl->AddFoldPanel(wxT("Test me"), false);
        m_pnl->AddFoldPanelWindow(item, new wxButton(item.GetParent(), wxID_ANY, wxT("Press Me")));
    \endcode
    */
    wxFoldPanelItem *GetParent() const {
        wxASSERT(m_item);
        return m_item;
    };


};

#include <wx/dynarray.h>
WX_DEFINE_ARRAY_WITH_DECL_PTR(wxFoldPanelItem *, wxFoldPanelItemArray, class WXDLLIMPEXP_FOLDBAR);

/** \class wxFoldPanelBar
    The wxFoldPanelBar is a class which can maintain a list of collapsable panels. Once a panel is collapsed, only
    it's panel bar is visible to the user. This will provide more space for the other panels, or allow the user to
    close panels which are not used often to get the most out of the work area.

    This control is easy to use. Simply create it as a child for a panel or sash window, and populate panels with
    wxFoldPanelBar::AddFoldPanel(). Then use the wxFoldPanelBar::AddFoldPanelWindow() to add wxWindow derived controls
    to the current fold panel. Use wxFoldPanelBar::AddFoldPanelSeparator() to put separators between the groups of
    controls that need a visual separator to group them together. After all is constructed, the user can fold
    the panels by doubleclicking on the bar or single click on the arrow, which will indicate the collapsed or
    expanded state.
*/

class WXDLLIMPEXP_FOLDBAR wxFoldPanelBar: public wxPanel
{
private:
    DECLARE_DYNAMIC_CLASS( wxFoldPanelBar )
    DECLARE_EVENT_TABLE()

    wxImageList *m_images;
    wxFoldPanelItemArray m_panels;
    wxBoxSizer* m_panelSizer;
    wxPanel *m_foldPanel, *m_bottomPanel;
    wxFlexGridSizer* m_mainSizer;
    bool m_controlCreated;
    wxBitmap *m_moreBmp;
    int m_extraStyle;

private:
    /** Refreshes all the panels from given index down to last one */
    void RefreshPanelsFrom(size_t i);

    /** Refreshes all the panels from given pointer down to last one in the list */
    void RefreshPanelsFrom(wxFoldPanelItem *item);

    /** Returns the length of the panels that are expanded and collapsed. This is useful to determine
        quickly what size is used to display, and what is left at the bottom (right) to align
        the collapsed panels. */
    int GetPanelsLength(int &collapsed, int &expanded);

    /** Reposition all the collapsed panels to the bottom. When it is not possible to
        align them to the bottom, stick them behind the visible panels. The Rect holds the
        slack area left between last repositioned panel and the bottom panels. This needs to
        get a refresh */
    wxRect RepositionCollapsedToBottom();

public:
    /** Two step constructor used for XRC. Use wxFoldPanelBar::Create() to create the panel. Do not call
        any other methods before the control is fully created! */
    wxFoldPanelBar();
    /** One step creation. Look at wxPanel for the argument and style flags. The extraStyle flags are
        - wxFPB_DEFAULT_EXTRASTYLE : Takes default styles.
        - wxFPB_COLLAPSE_TO_BOTTOM : When panels are collapsed, they are put at the bottom of the area. */
    wxFoldPanelBar( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = wxFPB_DEFAULT_STYLE,
                    long extraStyle = wxFPB_DEFAULT_EXTRASTYLE);

    /** wxFoldPanelBar destructor */
    virtual ~wxFoldPanelBar();

    /** Two step create call. Use this when the control is not created using the wxPanel derived constructor.
        WARNING: Do not create this component more then once! */
    virtual void Create( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize, long style = wxFPB_DEFAULT_STYLE,
                         long extraStyle = wxFPB_DEFAULT_EXTRASTYLE);

    /** Adds a fold panel to the list of panels. If the flag collapsedInitially is set to true, the panel
        is collapsed initially. The wxFoldPanel item which is returned, can be used as a reference to
        perform actions upon the fold panel like collapsing it, expanding it, or deleting it from the list.

        Use this foldpanel to add windows to it. Please consult wxFoldPanelBar::AddFoldPanelWindow() and
        wxFoldPanelBar::AddFoldPanelSeparator() how to add wxWindow items to the panels. */
    wxFoldPanel AddFoldPanel(const wxString &caption, bool collapsedInitially = false,
                             const wxCaptionBarStyle &style = wxEmptyCaptionBarStyle);


    /** Adds a wxWindow derived class to the referenced wxFoldPanel. IMPORTANT: Make the to be created window,
        child of the wxFoldPanel. See example that follows. The flags to be used are:
        - wxFPB_ALIGN_WIDTH: Which means the wxWindow to be added will be aligned to fit the width of the
          wxFoldPanel when it is resized. Very handy for sizer items, buttons and text boxes.
        - wxFPB_ALIGN_LEFT: Aligns left instead of fitting the width of the child window to be added. Use either
          this one or wxFPB_ALIGN_WIDTH.

        The wxWindow to be added can be slightly indented from left and right so it is more visibly placed
        in the wxFoldPanel. Use ySpacing > 0 to give the control an y offset from the previous wxWindow added,
        use leftSpacing to give it a slight indent from the left, and rightSpacing also reserves a little space
        on the right so the wxWindow can be properly placed in the wxFoldPanel.

        The following example adds a wxFoldPanel to the wxFoldPanelBar and adds two wxWindow derived controls
        to the wxFoldPanel:

        \code

            // create the wxFoldPanelBar
            m_pnl = new wxFoldPanelBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFPB_DEFAULT_STYLE, wxFPB_COLLAPSE_TO_BOTTOM);

            // add a foldpanel to the control. "Test me" is the caption and it is initially not collapsed.
            wxFoldPanel item = m_pnl->AddFoldPanel(wxT("Test me"), false);

            // now add a button to the fold panel. Mind that the button should be made child of the
            // wxFoldPanel and not of the main form.
            m_pnl->AddFoldPanelWindow(item, new wxButton(item.GetParent(), ID_COLLAPSEME, wxT("Collapse Me")));

            // add a separator between the two controls. This is purely a visual line that can have a certain
            // color and also the indents and width alligning like a control.
            m_pnl->AddFoldPanelSeperator(item);

            // now add a text ctrl. Also very easy. Align this on width so that when the control gets wider
            // the text control also sizes along.
            m_pnl->AddFoldPanelWindow(item, new wxTextCtrl(item.GetParent(), wxID_ANY, wxT("Comment")), wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_SPACING, 20);

        \endcode
    */
    int AddFoldPanelWindow(const wxFoldPanel &panel, wxWindow *window, int flags = wxFPB_ALIGN_WIDTH,
                           int Spacing = wxFPB_DEFAULT_SPACING, int leftSpacing = wxFPB_DEFAULT_LEFTSPACING,
                           int rightSpacing = wxFPB_DEFAULT_RIGHTSPACING);

    /** Adds a separator line to the current wxFoldPanel. The separator is a simple line which is drawn and is no
        real component. It can be used to separate groups of controls which belong to each other. The colour is
        adjustable, and it takes the same ySpacing, leftSpacing and rightSpacing as AddFoldPanelWindow(). */
    int AddFoldPanelSeperator(const wxFoldPanel &panel, const wxColour &color = wxColour(167,167,167),
                           int Spacing = wxFPB_DEFAULT_SPACING, int leftSpacing = wxFPB_DEFAULT_LEFTLINESPACING,
                           int rightSpacing = wxFPB_DEFAULT_RIGHTLINESPACING);

    /** Returns the number of panels currently present in the wxFoldPanelBar. This is independent if they are
        visible or hidden. */
    size_t GetCount() const {
        return m_panels.GetCount();
    };

    inline bool IsVertical() const
        {
            return HasFlag(wxFPB_VERTICAL);
        }

    /** Returns the wxFoldPanel reference belonging to the current index. An empty panel is returned when the
        index is out of bounds. Use GetCount() to get the amount of panels present. Collapsing and folding the
        panel does not change the order in which they are indexed. So it is safe enough to keep a reference
        to the panel by number. */
    wxFoldPanel Item(size_t i) {
        wxCHECK((int)i >= 0 && i < GetCount(), wxFoldPanel(0));
        return wxFoldPanel(m_panels.Item(i));
    };

    /** Collapses the given wxFoldPanel reference, and updates the foldpanel bar. In the wxFPB_COLLAPSE_TO_BOTTOM
        style, all collapsed captions are put at the bottom of the control. In the normal mode, they stay where
        they are */
    void Collapse(const wxFoldPanel &item) {
        wxCHECK2(item.IsOk(), return);
        item.GetItem()->Collapse();

        RefreshPanelsFrom(item.GetItem());
    };

    /** Expands the given wxFoldPanel reference, and updates the foldpanel bar. In the wxFPB_COLLAPSE_TO_BOTTOM
       they will be removed from the bottom and the order where the panel originally was placed is restored.  */
    void Expand(const wxFoldPanel &item) {
        wxCHECK2(item.IsOk(), return);
        item.GetItem()->Expand();

        RefreshPanelsFrom(item.GetItem());
    };

 /** Sets the style of the caption bar (called wxCaptionBar) of the wxFoldPanel. The changes are applied immediately.
        All styles not set in the wxCaptionBarStyle class are not applied. Use the wxCaptionBar reference to indicate
        what captionbar you want to apply the style to. To apply one style to all wxCaptionBar items, use
        ApplyCaptionStyleAll() */
    void ApplyCaptionStyle(wxFoldPanel &fp, const wxCaptionBarStyle &style) {
        wxCHECK2(fp.IsOk(), return);
        fp.GetItem()->ApplyCaptionStyle(style);
    };

    /** Sets the style of all the caption bars of the wxFoldPanel. The changes are applied immediately */
    void ApplyCaptionStyleAll(const wxCaptionBarStyle &style) {
        for(size_t i = 0; i < GetCount(); i++)
        {
            wxFoldPanel item = Item(i);
            ApplyCaptionStyle(item, style);
        }
    };

    /** Returns the currently used caption style for the wxFoldPanel. It is returned as a wxCaptionBarStyle class.
        after modifying it, it can be set again */
    wxCaptionBarStyle GetCaptionStyle(wxFoldPanel &fp) const {
        wxCHECK2(fp.IsOk(), return wxEmptyCaptionBarStyle);
        return fp.GetItem()->GetCaptionStyle();
    };

private:
    void OnPressCaption(wxCaptionBarEvent &event);
    void OnSizePanel(wxSizeEvent &event);

    /** Resize the fold panels so they match the width */
    void RedisplayFoldPanelItems();

    void OnPaint(wxPaintEvent &event);
};

#endif // __WXFOLDPANELBAR_H__
