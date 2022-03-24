///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/preferences.cpp
// Purpose:     Native OS X implementation of wxPreferencesEditor.
// Author:      Vaclav Slavik
// Created:     2013-02-19
// Copyright:   (c) 2013 Vaclav Slavik <vslavik@fastmail.fm>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_PREFERENCES_EDITOR

#include "wx/private/preferences.h"

#ifdef wxHAS_PREF_EDITOR_NATIVE

#include "wx/frame.h"
#include "wx/sharedptr.h"
#include "wx/toolbar.h"
#include "wx/vector.h"
#include "wx/weakref.h"
#include "wx/windowid.h"
#include "wx/osx/private.h"
#include "wx/private/bmpbndl.h"
#include "wx/osx/private/available.h"

#import <AppKit/NSWindow.h>


wxBitmapBundle wxStockPreferencesPage::GetIcon() const
{
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_16
    if ( WX_IS_MACOS_AVAILABLE(11, 0) )
    {
        switch ( m_kind )
        {
            case Kind_General:
                return wxOSXMakeBundleFromImage([NSImage imageWithSystemSymbolName:@"gearshape" accessibilityDescription:nil]);
            case Kind_Advanced:
                return wxOSXMakeBundleFromImage([NSImage imageWithSystemSymbolName:@"gearshape.2" accessibilityDescription:nil]);
        }
    }
#endif

    switch ( m_kind )
    {
        case Kind_General:
            return wxOSXMakeBundleFromImage([NSImage imageNamed:NSImageNamePreferencesGeneral]);
        case Kind_Advanced:
            return wxOSXMakeBundleFromImage([NSImage imageNamed:NSImageNameAdvanced]);
    }

    return wxBitmapBundle();
}


class wxCocoaPrefsWindow : public wxFrame
{
public:
    wxCocoaPrefsWindow(const wxString& title)
        : wxFrame(NULL, wxID_ANY, title,
                  wxDefaultPosition, wxDefaultSize,
                  wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX)),
          m_toolbarRealized(false),
          m_visiblePage(NULL)
    {
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_16
        if ( WX_IS_MACOS_AVAILABLE(11,0) )
        {
            NSWindow *win = GetWXWindow();
            [win setToolbarStyle:NSWindowToolbarStylePreference];
        }
#endif
        m_toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                  wxTB_FLAT | wxTB_TEXT);
        m_toolbar->SetToolBitmapSize(wxSize(32,32));
        m_toolbar->OSXSetSelectableTools(true);
        SetToolBar(m_toolbar);

        m_toolbar->Bind(wxEVT_TOOL,
                        &wxCocoaPrefsWindow::OnPageChanged, this);
        Bind(wxEVT_CLOSE_WINDOW, &wxCocoaPrefsWindow::OnClose, this);
    }

    void AddPage(wxPreferencesPage *page)
    {
        wxASSERT_MSG( !m_toolbarRealized,
                      "can't add more preferences pages after showing the window" );

        const wxString title = page->GetName();
        wxBitmapBundle bmp(page->GetIcon());
        wxASSERT_MSG( bmp.IsOk(), "OS X requires valid bitmap for preference page" );

        int toolId = wxIdManager::ReserveId();
        wxToolBarToolBase *tool = m_toolbar->AddTool(toolId, title, bmp);

        wxSharedPtr<PageInfo> info(new PageInfo(page));
        m_pages.push_back(info);

        tool->SetClientData(info.get());
    }

    virtual bool Show(bool show) wxOVERRIDE
    {
        if ( show && !m_toolbarRealized )
        {
            m_toolbar->Realize();
            m_toolbarRealized = true;

            const wxToolBarToolBase *first = m_toolbar->GetToolByPos(0);
            wxCHECK_MSG( first, false, "no preferences panels" );
            OnSelectPageForTool(first);
            m_toolbar->OSXSelectTool(first->GetId());
        }

        return wxFrame::Show(show);
    }

    virtual bool ShouldPreventAppExit() const wxOVERRIDE { return false; }

protected:
    // Native preferences windows resize when the selected panel changes and
    // the resizing is animated, so we need to override DoMoveWindow.
    virtual void DoMoveWindow(int x, int y, int width, int height) wxOVERRIDE
    {
        NSRect r = wxToNSRect(NULL, wxRect(x, y, width, height));
        NSWindow *win = (NSWindow*)GetWXWindow();
        [win setFrame:r display:YES animate:YES];
    }


private:
    struct PageInfo : public wxObject
    {
        PageInfo(wxPreferencesPage *p) : page(p), win(NULL) {}

        wxSharedPtr<wxPreferencesPage> page;
        wxWindow *win;
    };

    typedef wxVector< wxSharedPtr<PageInfo> > Pages;

    wxWindow *GetPageWindow(PageInfo& info)
    {
        if ( !info.win )
        {
            info.win = info.page->CreateWindow(this);
            info.win->Hide();
            // fill the page with data using wxEVT_INIT_DIALOG/TransferDataToWindow:
            info.win->InitDialog();
        }

        return info.win;
    }

    int GetBiggestPageWidth()
    {
        int width = -1;
        for ( Pages::const_iterator p = m_pages.begin(); p != m_pages.end(); ++p )
        {
            wxWindow *win = GetPageWindow(**p);
            width = wxMax(width, win->GetBestSize().x);
        }

        return width;
    }

    void FitPageWindow(wxWindow *win)
    {
        // On macOS 11, preferences are resizable only vertically, because the
        // icons are centered and horizontal resizing would move them around.
        if ( WX_IS_MACOS_AVAILABLE(11,0) )
        {
            int width = GetBiggestPageWidth();
            if (width > win->GetBestSize().x)
            {
                wxSize minsize = win->GetMinSize();
                minsize.x = width;
                win->SetMinSize(minsize);
            }
        }

        win->Fit();
    }

    void OnSelectPageForTool(const wxToolBarToolBase *tool)
    {
        PageInfo *info = static_cast<PageInfo*>(tool->GetClientData());
        wxCHECK_RET( info, "toolbar item lacks client data" );

        wxWindow *win = GetPageWindow(*info);
        FitPageWindow(win);

        // When the page changes in a native preferences dialog, the sequence
        // of events is thus:

        // 1. the old page is hidden, only gray background remains
        if ( m_visiblePage )
            m_visiblePage->Hide();
        m_visiblePage = win;

        //   2. window is resized to fix the new page, with animation
        //      (in our case, using overriden DoMoveWindow())
        SetClientSize(win->GetSize());

        //   3. new page is shown and the title updated.
        win->Show();
        SetTitle(info->page->GetName());

        // Refresh the page to ensure everything is drawn in 10.14's dark mode;
        // without it, generic controls aren't shown at all
        win->Refresh();

        // TODO: Preferences window may have some pages resizeable and some
        //       non-resizable on OS X; the whole window is or is not resizable
        //       depending on which page is selected.
        //
        //       We'll need to add wxPreferencesPage::IsResizable() virtual
        //       method to implement this.
    }

    void OnPageChanged(wxCommandEvent& event)
    {
        wxToolBarToolBase *tool = m_toolbar->FindById(event.GetId());
        wxCHECK_RET( tool, "invalid tool ID" );
        OnSelectPageForTool(tool);
    }

    void OnClose(wxCloseEvent& WXUNUSED(e))
    {
        // Instead of destroying the window, just hide it, it could be
        // reused again by another invocation of the editor.
        Hide();
    }

private:
    // All pages. Use shared pointer to be able to get pointers to PageInfo structs
    Pages m_pages;

    wxToolBar *m_toolbar;
    bool       m_toolbarRealized;
    wxWindow  *m_visiblePage;
};


class wxCocoaPreferencesEditorImpl : public wxPreferencesEditorImpl
{
public:
    wxCocoaPreferencesEditorImpl(const wxString& title)
        : m_win(NULL), m_title(title)
    {
    }

    virtual ~wxCocoaPreferencesEditorImpl()
    {
        // m_win may already be destroyed if this destructor is called from
        // wxApp's destructor. In that case, all windows -- including this
        // one -- would already be destroyed by now.
        if ( m_win )
            m_win->Destroy();
    }

    virtual void AddPage(wxPreferencesPage* page) wxOVERRIDE
    {
        GetWin()->AddPage(page);
    }

    virtual void Show(wxWindow* WXUNUSED(parent)) wxOVERRIDE
    {
        // OS X preferences windows don't have parents, they are independent
        // windows, so we just ignore the 'parent' argument.
        wxWindow *win = GetWin();
        win->Show();
        win->Raise();
    }

    virtual void Dismiss() wxOVERRIDE
    {
        // Don't destroy the window, only hide it, because OS X preferences
        // window typically remember their state even when closed. Reopening
        // the window should show it in the exact same state the user left it.
        GetWin()->Hide();
    }

private:
    // Use this function to access m_win, so that the window is only created on
    // demand when actually needed.
    wxCocoaPrefsWindow* GetWin()
    {
        if ( !m_win )
        {
            if ( m_title.empty() )
                m_title = _("Preferences");

            m_win = new wxCocoaPrefsWindow(m_title);
        }

        return m_win;
    }

    wxWeakRef<wxCocoaPrefsWindow> m_win;

    wxString m_title;
};

/*static*/
wxPreferencesEditorImpl* wxPreferencesEditorImpl::Create(const wxString& title)
{
    return new wxCocoaPreferencesEditorImpl(title);
}

#endif // wxHAS_PREF_EDITOR_NATIVE

#endif // wxUSE_PREFERENCES_EDITOR
