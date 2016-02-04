/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        native.cpp
// Purpose:     Part of the widgets sample showing native control integration
// Author:      Vadim Zeitlin
// Created:     2015-07-30
// Copyright:   (c) 2015 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// this file is included from native.mm which ensures that it is compiled as
// Objective C++, but it's also still compiled by the makefiles directly as C++
// source because we can't easily exclude it, so check for this and only
// compile the rest of this file once
#if !defined(__WXOSX_COCOA__) || defined(__OBJC__)

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/checkbox.h"
    #include "wx/log.h"
    #include "wx/menu.h"
    #include "wx/sizer.h"
#endif // !WX_PRECOMP

#include "wx/nativewin.h"

#ifdef wxHAS_NATIVE_WINDOW

#include "widgets.h"

#include "icons/native.xpm"

// Helper to create a menu to be shown in our button.
//
// The menu is supposed to be initially empty, don't call this more than once
// for the same object.
void BuildTestMenu(wxMenu *menu)
{
    menu->Append(wxID_NEW, "Do it with a new file");
    menu->Append(wxID_OPEN, "Do it with an existing file");
    menu->AppendSeparator();
    menu->Append(wxID_ABOUT);
}

// Create the native control in a -- necessarily -- platform-specific way.
#ifdef __WXMSW__

#include "wx/msw/wrapcctl.h"    // for BS_SPLITBUTTON

#ifndef BS_SPLITBUTTON
    #define BS_SPLITBUTTON 0x000c
#endif // BS_SPLITBUTTON

class NativeWindow : public wxNativeWindow
{
public:
    explicit NativeWindow(wxWindow* parent)
        : wxNativeWindow()
    {
        // When creating the native window, we must specify the valid parent
        // and while we don't have to specify any position if it's going to be
        // laid out by sizers, we do need the size.
        const wxSize size = FromDIP(wxSize(140, 30));

        HWND hwnd = ::CreateWindow
                      (
                        TEXT("BUTTON"),
                        TEXT("Press me to do it"),
                        WS_CHILD | WS_VISIBLE | BS_SPLITBUTTON,
                        0, 0, size.x, size.y,
                        (HWND)parent->GetHWND(), 0, NULL, NULL
                      );
        if ( !hwnd )
        {
            wxLogError("Creating split button failed.");
            return;
        }

        (void)Create(parent, wxID_ANY, hwnd);
    }

    virtual ~NativeWindow()
    {
        // If you don't call this, you need to call DestroyWindow() later.
        //
        // Also notice that a HWND can't continue to exist under MSW if its
        // parent its destroyed, so you may also want to reparent it under some
        // other window if the parent of this window is also getting destroyed.
        Disown();
    }

protected:
    // This code requires NMBCDROPDOWN to work, we don't really want to
    // reproduce its definition here for very old compilers not having it.
#ifdef BCN_DROPDOWN
    // Split buttons under MSW don't show the menu on their own, unlike their
    // equivalents under the other platforms, so do it manually here. This also
    // shows how to handle a native event in MSW (for the specific case of
    // WM_NOTIFY, more generally MSWHandleMessage() could be overridden).
    virtual bool
    MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result) wxOVERRIDE
    {
        const NMHDR* hdr = reinterpret_cast<NMHDR*>(lParam);
        if ( hdr->code != BCN_DROPDOWN )
            return wxNativeWindow::MSWOnNotify(idCtrl, lParam, result);

        const NMBCDROPDOWN* dd = reinterpret_cast<NMBCDROPDOWN*>(lParam);

        wxMenu menu;
        BuildTestMenu(&menu);
        PopupMenu(&menu, wxPoint(dd->rcButton.right, dd->rcButton.bottom));

        return true;
    }
#endif // defined(BCN_DROPDOWN)
};

#elif defined(__WXGTK__)

class NativeWindow : public wxNativeWindow
{
public:
    explicit NativeWindow(wxWindow* parent)
        : wxNativeWindow()
    {
#if GTK_CHECK_VERSION(3,6,0)
        BuildTestMenu(&m_menu);

        GtkWidget* const widget = gtk_menu_button_new();
        gtk_menu_button_set_popup(GTK_MENU_BUTTON(widget), m_menu.m_menu);
#else // GTK+ < 3.6
        // Menu buttons are only available since GTK+ 3.6, so use something
        // even simpler for earlier versions (including GTK+ 2) just as a
        // placeholder.
        GtkWidget* const widget = gtk_label_new("");
        gtk_label_set_markup
        (
            GTK_LABEL(widget),
            "<b>Sorry</b>, but your GTK+ is too old to have menu buttons."
        );
#endif // GTK+ 3.6/earlier

        g_object_ref_sink(widget);

        (void)Create(parent, wxID_ANY, widget);
    }

    virtual ~NativeWindow()
    {
        // If you don't call this, you need to call g_object_unref() on the
        // widget from elsewhere later.
        Disown();
    }

private:
    wxMenu m_menu;
};

#elif defined(__WXOSX_COCOA__)

#import <Cocoa/Cocoa.h>

class NativeWindow : public wxNativeWindow
{
public:
    explicit NativeWindow(wxWindow* parent)
        : wxNativeWindow()
    {
        // Neither the position nor the size really matter: the former because
        // the window will be positioned by the sizers, the latter because its
        // "fitting" size will be used as the best size by default.
        NSPopUpButton* const v = [[NSPopUpButton alloc]
                                    initWithFrame:NSMakeRect(0, 0, 0, 0)
                                    pullsDown:YES];

        wxMenu menu;
        BuildTestMenu(&menu);
        [v setMenu:(NSMenu*)menu.GetHMenu()];

        // In a pull down (but not pop up) buttons, items start with 1 and the
        // 0-th one is used as title.
        [v insertItemWithTitle:@"Press me to do it" atIndex:0];

        // By default the control would disable the button title because it
        // doesn't appear in the list of the menu items, prevent this from
        // happening.
        [v setAutoenablesItems:NO];

        (void)Create(parent, wxID_ANY, v);
    }

    virtual ~NativeWindow()
    {
        // If you don't call this, you need to call -release: on the button
        // from elsewhere later.
        Disown();
    }
};

#else // some other platform

// The sample should be updated if wxNativeCtrl is implemented for some new
// platform in wx/nativectrl.h.
#error "Native control creation not implemented for this platform"

#endif // platforms

// ----------------------------------------------------------------------------
// NativeWidgetsPage
// ----------------------------------------------------------------------------

class NativeWidgetsPage : public WidgetsPage
{
public:
    NativeWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_nativeWindow; }
    virtual void RecreateWidget() wxOVERRIDE;

    // lazy creation of the content
    virtual void CreateContent() wxOVERRIDE;

private:
    void OnCheckExpand(wxCommandEvent& event);

    wxCheckBox* m_chkExpand;

    wxNativeWindow* m_nativeWindow;
    wxSizer* m_sizerCtrl;

    DECLARE_WIDGETS_PAGE(NativeWidgetsPage)
};

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(NativeWidgetsPage, wxT("Native"), NATIVE_CTRLS);

NativeWidgetsPage::NativeWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist)
                 : WidgetsPage(book, imaglist, native_xpm)
{
    m_nativeWindow = NULL;
}

void NativeWidgetsPage::CreateContent()
{
    wxSizer* const sizerTop = new wxBoxSizer(wxHORIZONTAL);

    wxSizer* const sizerLeft = new wxBoxSizer(wxVERTICAL);
    m_chkExpand = new wxCheckBox(this, wxID_ANY, "&Expand to all available size");
    m_chkExpand->Bind(wxEVT_CHECKBOX, &NativeWidgetsPage::OnCheckExpand, this);
    sizerLeft->Add(m_chkExpand, wxSizerFlags().Border());
    sizerTop->Add(sizerLeft);

    m_sizerCtrl = new wxBoxSizer(wxHORIZONTAL);

    RecreateWidget();

    sizerTop->Add(m_sizerCtrl, wxSizerFlags(1).Expand());
    SetSizer(sizerTop);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void NativeWidgetsPage::RecreateWidget()
{
    delete m_nativeWindow;
    m_nativeWindow = new NativeWindow(this);

    m_sizerCtrl->Clear();
    if ( m_chkExpand->IsChecked() )
    {
        m_sizerCtrl->Add(m_nativeWindow, wxSizerFlags(1).Expand().Border());
    }
    else
    {
        m_sizerCtrl->AddStretchSpacer();
        m_sizerCtrl->Add(m_nativeWindow, wxSizerFlags().Centre());
        m_sizerCtrl->AddStretchSpacer();
    }

    m_sizerCtrl->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void NativeWidgetsPage::OnCheckExpand(wxCommandEvent& WXUNUSED(event))
{
    RecreateWidget();
}

#endif // wxHAS_NATIVE_WINDOW

#endif // !OSX/Cocoa compilation as Objective C source file
