/////////////////////////////////////////////////////////////////////////////
// Program:     wxWindows Widgets Sample
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

class WXDLLEXPORT wxNotebook;
class WXDLLEXPORT wxSizer;
class WXDLLEXPORT wxTextCtrl;

class WXDLLEXPORT WidgetsPageInfo;

// all source files use wxImageList
#include "wx/imaglist.h"

// ----------------------------------------------------------------------------
// WidgetsPage: a notebook page demonstrating some widget
// ----------------------------------------------------------------------------

class WidgetsPage : public wxPanel
{
public:
    WidgetsPage(wxNotebook *notebook);

protected:
    // several helper functions for page creation

    // create a horz sizer containing the given control and the text ctrl
    // (pointer to which will be saved in the provided variable if not NULL)
    // with the specified id
    wxSizer *CreateSizerWithText(wxControl *control,
                                 wxWindowID id = -1,
                                 wxTextCtrl **ppText = NULL);

    // create a sizer containing a label and a text ctrl
    wxSizer *CreateSizerWithTextAndLabel(const wxString& label,
                                         wxWindowID id = -1,
                                         wxTextCtrl **ppText = NULL);

    // create a sizer containing a button and a text ctrl
    wxSizer *CreateSizerWithTextAndButton(wxWindowID idBtn,
                                          const wxString& labelBtn,
                                          wxWindowID id = -1,
                                          wxTextCtrl **ppText = NULL);

public:
    // the head of the linked list containinginfo about all pages
    static WidgetsPageInfo *ms_widgetPages;
};

// ----------------------------------------------------------------------------
// dynamic WidgetsPage creation helpers
// ----------------------------------------------------------------------------

class WXDLLEXPORT WidgetsPageInfo
{
public:
    typedef WidgetsPage *(*Constructor)(wxNotebook *notebook,
                                        wxImageList *imaglist);

    // our ctor
    WidgetsPageInfo(Constructor ctor, const wxChar *label);

    // accessors
    const wxString& GetLabel() const { return m_label; }
    Constructor GetCtor() const { return m_ctor; }
    WidgetsPageInfo *GetNext() const { return m_next; }

private:
    // the label of the page
    wxString m_label;

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
#define IMPLEMENT_WIDGETS_PAGE(classname, label)                            \
    WidgetsPage *wxCtorFor##classname(wxNotebook *notebook,                 \
                                      wxImageList *imaglist)                \
        { return new classname(notebook, imaglist); }                       \
    WidgetsPageInfo classname::                                             \
        ms_info##classname(wxCtorFor##classname, label)

#endif // _WX_SAMPLE_WIDGETS_H_
