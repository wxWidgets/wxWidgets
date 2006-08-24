/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        widgets.h
// Purpose:     Common stuff for all widgets project files
// Author:      Vadim Zeitlin
// Created:     27.03.01
// Id:          $Id$
// Copyright:   (c) 2001 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLE_WIDGETS_H_
#define _WX_SAMPLE_WIDGETS_H_

#if wxUSE_TREEBOOK && !defined(__WXHANDHELD__)
    #include "wx/treebook.h"
    #define USE_TREEBOOK 1
    #define WidgetsBookCtrl wxTreebook
    #define WidgetsBookCtrlEvent wxTreebookEvent
    #define EVT_WIDGETS_PAGE_CHANGING(id,func) EVT_TREEBOOK_PAGE_CHANGING(id,func)
    #define EVT_WIDGETS_PAGE_CHANGED(id,func) EVT_TREEBOOK_PAGE_CHANGED(id,func)
#else
    #include "wx/bookctrl.h"
    #define USE_TREEBOOK 0
    #define WidgetsBookCtrl wxBookCtrl
    #define WidgetsBookCtrlEvent wxBookCtrlEvent
    #define EVT_WIDGETS_PAGE_CHANGING(id,func) EVT_BOOKCTRL_PAGE_CHANGING(id,func)
    #define EVT_WIDGETS_PAGE_CHANGED(id,func) EVT_BOOKCTRL_PAGE_CHANGED(id,func)
#endif

#if wxUSE_LOG && !defined(__WXHANDHELD__)
    #define USE_LOG 1
#else
    #define USE_LOG 0
#endif

#if defined(__WXHANDHELD__)
    #define USE_ICONS_IN_BOOK 0
#else
    #define USE_ICONS_IN_BOOK 1
#endif

class WXDLLEXPORT wxCheckBox;
class WXDLLEXPORT wxSizer;
class WXDLLEXPORT wxImageList;
class WXDLLEXPORT wxTextCtrl;
class WXDLLEXPORT WidgetsBookCtrl;

class WidgetsPageInfo;

#include "wx/panel.h"

// INTRODUCING NEW PAGES DON'T FORGET TO ADD ENTRIES TO 'WidgetsCategories'
enum
{
    // On wxUniversal-based build (wxX11, wxMGL, wxMSWUniv, wxGTKUniv, etc.)
    // 'native' means 'made with wxUniv port renderer'
    NATIVE_PAGE = 0,
    UNIVERSAL_PAGE = NATIVE_PAGE,
    GENERIC_PAGE,
    PICKER_PAGE,
    COMBO_PAGE,
    WITH_ITEMS_PAGE,
    EDITABLE_PAGE,
    BOOK_PAGE,
    ALL_PAGE,
    MAX_PAGES
};

enum
{
    NATIVE_CTRLS     = 1 << NATIVE_PAGE,
    UNIVERSAL_CTRLS  = NATIVE_CTRLS,
    GENERIC_CTRLS    = 1 << GENERIC_PAGE,
    PICKER_CTRLS     = 1 << PICKER_PAGE,
    COMBO_CTRLS      = 1 << COMBO_PAGE,
    WITH_ITEMS_CTRLS = 1 << WITH_ITEMS_PAGE,
    EDITABLE_CTRLS   = 1 << EDITABLE_PAGE,
    BOOK_CTRLS       = 1 << BOOK_PAGE,
    ALL_CTRLS        = 1 << ALL_PAGE
};

// ----------------------------------------------------------------------------
// WidgetsPage: a book page demonstrating some widget
// ----------------------------------------------------------------------------

class WidgetsPage : public wxPanel
{
public:
    WidgetsPage(WidgetsBookCtrl *book,
                wxImageList *imaglist,
                char* icon[]);

    // return the control shown by this page
    virtual wxControl *GetWidget() const = 0;

    // lazy creation of the content
    virtual void CreateContent() = 0;

    // some pages show 2 controls, in this case override this one as well
    virtual wxControl *GetWidget2() const { return NULL; }

    // recreate the control shown by this page
    //
    // this is currently used only to take into account the border flags
    virtual void RecreateWidget() = 0;

    // the default flags for the widget, currently only contains border flags
    static int ms_defaultFlags;

protected:
    // several helper functions for page creation

    // create a horz sizer containing the given control and the text ctrl
    // (pointer to which will be saved in the provided variable if not NULL)
    // with the specified id
    wxSizer *CreateSizerWithText(wxControl *control,
                                 wxWindowID id = wxID_ANY,
                                 wxTextCtrl **ppText = NULL);

    // create a sizer containing a label and a text ctrl
    wxSizer *CreateSizerWithTextAndLabel(const wxString& label,
                                         wxWindowID id = wxID_ANY,
                                         wxTextCtrl **ppText = NULL);

    // create a sizer containing a button and a text ctrl
    wxSizer *CreateSizerWithTextAndButton(wxWindowID idBtn,
                                          const wxString& labelBtn,
                                          wxWindowID id = wxID_ANY,
                                          wxTextCtrl **ppText = NULL);

    // create a checkbox and add it to the sizer
    wxCheckBox *CreateCheckBoxAndAddToSizer(wxSizer *sizer,
                                            const wxString& label,
                                            wxWindowID id = wxID_ANY);

public:
    // the head of the linked list containinginfo about all pages
    static WidgetsPageInfo *ms_widgetPages;
};

// ----------------------------------------------------------------------------
// dynamic WidgetsPage creation helpers
// ----------------------------------------------------------------------------

class WidgetsPageInfo
{
public:
    typedef WidgetsPage *(*Constructor)(WidgetsBookCtrl *book,
                                        wxImageList *imaglist);

    // our ctor
    WidgetsPageInfo(Constructor ctor, const wxChar *label, int categories);

    // accessors
    const wxString& GetLabel() const { return m_label; }
    int GetCategories() const { return m_categories; }
    Constructor GetCtor() const { return m_ctor; }
    WidgetsPageInfo *GetNext() const { return m_next; }

    void SetNext(WidgetsPageInfo *next) { m_next = next; }

private:
    // the label of the page
    wxString m_label;

    // the list (flags) for sharing page between categories
    int m_categories;

    // the function to create this page
    Constructor m_ctor;

    // next node in the linked list or NULL
    WidgetsPageInfo *m_next;
};

// to declare a page, this macro must be used in the class declaration
#define DECLARE_WIDGETS_PAGE(classname)                                     \
    private:                                                                \
        static WidgetsPageInfo ms_info##classname;                          \
    public:                                                                 \
        const WidgetsPageInfo *GetPageInfo() const                          \
            { return &ms_info##classname; }

// and this one must be inserted somewhere in the source file
#define IMPLEMENT_WIDGETS_PAGE(classname, label, categories)                \
    WidgetsPage *wxCtorFor##classname(WidgetsBookCtrl *book,                \
                                      wxImageList *imaglist)                \
        { return new classname(book, imaglist); }                           \
    WidgetsPageInfo classname::                                             \
        ms_info##classname(wxCtorFor##classname, label, ALL_CTRLS | categories)

#endif // _WX_SAMPLE_WIDGETS_H_
