/////////////////////////////////////////////////////////////////////////////
// Name:        tests/testwin32.cpp
// Purpose:     Test including Windows headers before wxWidgets headers
// Author:      Maarten Bent
// Created:     2025-11-15
// Copyright:   (c) 2025 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32
    #error This is a Windows/win32 specific test
#endif

// When including Windows headers before wx headers, the functions declared
// in wx/msw/winundef.h need to match the ANSI/WIDE variant in the Windows
// headers. By default the WIDE variant is declared, but if ANSI Windows is
// used, define wxNO_WIN32_W.
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <Windows.h>
#ifndef UNICODE
    #define wxNO_WIN32_W
#endif

#include "wx/wxprec.h"

// include all headers, so incorrect usage of ANSI/WIDE win32 API in public
// headers is detected
#include "allheaders.h"

#ifdef wxHAS_NATIVE_WINDOW

class NativeWindow : public wxNativeWindow
{
public:
    explicit NativeWindow(wxWindow* parent)
        : wxNativeWindow()
    {
        const wxSize size = FromDIP(wxSize(140, 30));

        HWND hwnd = ::CreateWindow
                      (
                        TEXT("BUTTON"),
                        TEXT("Press me to do it"),
                        WS_CHILD | WS_VISIBLE | BS_SPLITBUTTON,
                        0, 0, size.x, size.y,
                        (HWND)parent->GetHWND(), 0, nullptr, nullptr
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
        Disown();
    }

protected:
    virtual bool
    MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result) override
    {
        const NMHDR* hdr = reinterpret_cast<NMHDR*>(lParam);
        if ( hdr->code != BCN_DROPDOWN )
            return wxNativeWindow::MSWOnNotify(idCtrl, lParam, result);

        const NMBCDROPDOWN* dd = reinterpret_cast<NMBCDROPDOWN*>(lParam);

        wxMenu menu;
        menu.Append(wxID_NEW);
        menu.AppendSeparator();
        menu.Append(wxID_ABOUT);
        PopupMenu(&menu, wxPoint(dd->rcButton.right, dd->rcButton.bottom));

        return true;
    }
};

#endif // wxHAS_NATIVE_WINDOW

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title)
    {
        wxSizer* m_sizerCtrl = new wxBoxSizer(wxHORIZONTAL);
#ifdef wxHAS_NATIVE_WINDOW
        wxNativeWindow* m_nativeWindow = new NativeWindow(this);
        m_sizerCtrl->Add(m_nativeWindow, wxSizerFlags(1).Expand().Border());
#endif
        SetSizer(m_sizerCtrl);

        CallAfter([&]() { Close(true); });
    }

};

class MyApp : public wxApp
{
public:
    virtual bool OnInit() override
    {
        if (!wxApp::OnInit())
            return false;

        MyFrame* frame = new MyFrame("win32 test");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
