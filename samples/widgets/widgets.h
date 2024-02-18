/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        widgets.h
// Purpose:     Common stuff for all widgets project files
// Author:      Vadim Zeitlin
// Created:     27.03.01
// Copyright:   (c) 2001 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLE_WIDGETS_H_
#define _WX_SAMPLE_WIDGETS_H_

#if wxUSE_TREEBOOK
    #include "wx/treebook.h"
    #define USE_TREEBOOK 1
    #define WidgetsBookCtrl wxTreebook
    #define WidgetsBookCtrlEvent wxTreebookEvent
    #define EVT_WIDGETS_PAGE_CHANGING(id,func) EVT_TREEBOOK_PAGE_CHANGING(id,func)
    #define wxEVT_COMMAND_WIDGETS_PAGE_CHANGED wxEVT_TREEBOOK_PAGE_CHANGED
    #define wxWidgetsbookEventHandler(func) wxTreebookEventHandler(func)
#else
    #include "wx/bookctrl.h"
    #define USE_TREEBOOK 0
    #define WidgetsBookCtrl wxBookCtrl
    #define WidgetsBookCtrlEvent wxBookCtrlEvent
    #define EVT_WIDGETS_PAGE_CHANGING(id,func) EVT_BOOKCTRL_PAGE_CHANGING(id,func)
    #define wxEVT_COMMAND_WIDGETS_PAGE_CHANGED wxEVT_BOOKCTRL_PAGE_CHANGED
    #define wxWidgetsbookEventHandler(func) wxBookCtrlEventHandler(func)
#endif

#if wxUSE_LOG
    #define USE_LOG 1
#else
    #define USE_LOG 0
#endif

#define ICON_SIZE         16

class WXDLLIMPEXP_FWD_CORE wxCheckBox;
class WXDLLIMPEXP_FWD_CORE wxSizer;
class WXDLLIMPEXP_FWD_CORE wxImageList;
class WXDLLIMPEXP_FWD_CORE wxTextCtrl;
class WXDLLIMPEXP_FWD_CORE wxTextEntryBase;
class WXDLLIMPEXP_FWD_CORE WidgetsBookCtrl;

class WidgetsPageInfo;

#include "wx/scrolwin.h"
#include "wx/vector.h"

// INTRODUCING NEW PAGES DON'T FORGET TO ADD ENTRIES TO 'WidgetsCategories'
enum
{
    // In wxUniversal-based builds 'native' means 'made with wxUniv port
    // renderer'
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

typedef wxVector<wxWindow *> Widgets;

// ----------------------------------------------------------------------------
// WidgetsPage: a book page demonstrating some widget
// ----------------------------------------------------------------------------

// struct to store common widget attributes
struct WidgetAttributes
{
    WidgetAttributes()
    {
#if wxUSE_TOOLTIPS
        m_tooltip = "This is a tooltip";
#endif // wxUSE_TOOLTIPS
        m_enabled = true;
        m_show = true;
        m_dir = wxLayout_Default;
        m_variant = wxWINDOW_VARIANT_NORMAL;
        m_cursor = wxNullCursor;
        m_defaultFlags = wxBORDER_DEFAULT;
    }

#if wxUSE_TOOLTIPS
   wxString m_tooltip;
#endif // wxUSE_TOOLTIPS
#if wxUSE_FONTDLG
    wxFont m_font;
#endif // wxUSE_FONTDLG
    wxColour m_colFg;
    wxColour m_colBg;
    wxColour m_colPageBg;
    bool m_enabled;
    bool m_show;
    wxLayoutDirection m_dir;
    wxWindowVariant m_variant;
    wxCursor m_cursor;
    // the default flags, currently only contains border flags
    long m_defaultFlags;
};

class WidgetsPage : public wxScrolledWindow
{
public:
    WidgetsPage(WidgetsBookCtrl *book,
                wxImageList *imaglist,
                const char *const icon[]);

    // return the control shown by this page
    virtual wxWindow *GetWidget() const = 0;

    // return the control shown by this page, if it supports text entry interface
    virtual wxTextEntryBase *GetTextEntry() const { return nullptr; }

    // lazy creation of the content
    virtual void CreateContent() = 0;

    // some pages show additional controls, in this case override this one to
    // return all of them (including the one returned by GetWidget())
    virtual Widgets GetWidgets() const
    {
        Widgets widgets;
        widgets.push_back(GetWidget());
        return widgets;
    }

    // recreate the control shown by this page
    //
    // this is currently used only to take into account the border flags
    virtual void RecreateWidget() = 0;

    // notify the main window about the widget recreation if it didn't happen
    // due to a call to RecreateWidget()
    void NotifyWidgetRecreation(wxWindow* widget);

    // enable notifications about the widget recreation disabled initially
    void EnableRecreationNotifications() { m_notifyRecreate = true; }

    // apply current attributes to the widget(s)
    void SetUpWidget();

    // the default attributes for the widget
    static WidgetAttributes& GetAttrs();

    // return true if we're showing logs in the log window (always the case
    // except during startup and shutdown)
    static bool IsUsingLogWindow();

protected:
    // several helper functions for page creation

    // create a horz sizer containing the given control and the text ctrl
    // (pointer to which will be saved in the provided variable if not null)
    // with the specified id
    wxSizer *CreateSizerWithText(wxControl *control,
                                 wxWindowID id = wxID_ANY,
                                 wxTextCtrl **ppText = nullptr);

    // create a sizer containing a label and a text ctrl
    wxSizer *CreateSizerWithTextAndLabel(const wxString& label,
                                         wxWindowID id = wxID_ANY,
                                         wxTextCtrl **ppText = nullptr,
                                         wxWindow* statBoxParent = nullptr);

    // create a sizer containing a button and a text ctrl
    wxSizer *CreateSizerWithTextAndButton(wxWindowID idBtn,
                                          const wxString& labelBtn,
                                          wxWindowID id = wxID_ANY,
                                          wxTextCtrl **ppText = nullptr,
                                          wxWindow* statBoxParent = nullptr);

    // create a checkbox and add it to the sizer
    wxCheckBox *CreateCheckBoxAndAddToSizer(wxSizer *sizer,
                                            const wxString& label,
                                            wxWindowID id = wxID_ANY,
                                            wxWindow* statBoxParent = nullptr);

public:
    // the head of the linked list containinginfo about all pages
    static WidgetsPageInfo *ms_widgetPages;

private:
    bool m_notifyRecreate = false;
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
    WidgetsPageInfo(Constructor ctor, const wxString& label, int categories);

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

    // next node in the linked list or nullptr
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
