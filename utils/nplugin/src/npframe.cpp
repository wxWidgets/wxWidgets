/*
 * File:	NPFrame.cc
 * Purpose:	wxPluginFrame implementation
 * Author:	Julian Smart
 * Created:	1997
 * Updated:
 * Copyright:   (c) Julian Smart
 */

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/frame.h"
#endif

#include "wx/dcprint.h"

#include "NPFrame.h"
#include "NPApp.h"

#include <windows.h>

extern wxList wxModelessWindows;
extern char wxFrameClassName[];

IMPLEMENT_DYNAMIC_CLASS(wxPluginFrame, wxFrame)

wxPluginFrame::wxPluginFrame(void)
{
    m_npWindow = NULL;
    m_npInstance = NULL;
    m_nAttributes = 0;
    m_names = NULL;
    m_values = NULL;
}

bool wxPluginFrame::Create(const wxPluginData& data)
{
    SetName("pluginFrame");

    m_npWindow = NULL;
    m_npInstance = NULL;
    m_nAttributes = 0;
    m_names = NULL;
    m_values = NULL;
    m_npWindow = data.m_window;
    m_npInstance = data.m_instance;

    SetAttributeValues(data.m_argc, data.m_argn, data.m_argv);
    SetNPWindow(data.m_window);

    wxModelessWindows.Append(this);

    if (wxTheApp->IsKindOf(CLASSINFO(wxPluginApp)))
    {
        ((wxPluginApp *)wxTheApp)->AddFrame(this);
    }
    return TRUE;
}

wxPluginFrame::~wxPluginFrame(void)
{
    if (wxTheApp->IsKindOf(CLASSINFO(wxPluginApp)))
    {
        ((wxPluginApp *)wxTheApp)->RemoveFrame(this);
    }

    if ( GetHWND() )
        UnsubclassWin();
    m_hWnd = 0;

    if ( m_names )
        delete[] m_names;
    if ( m_values )
        delete[] m_values;
}

// Get size *available for subwindows* i.e. excluding menu bar etc.
// For XView, this is the same as GetSize
void wxPluginFrame::GetClientSize(int *x, int *y) const
{
    if ( !m_hWnd )
    {
        *x = 0; *y = 0;
        return;
    }
    wxFrame::GetClientSize(x, y);
}

// Set the client size (i.e. leave the calculation of borders etc.
// to wxWindows)
void wxPluginFrame::SetClientSize(const int width, const int height)
{
    if ( !m_hWnd )
        return ;

    wxFrame::SetClientSize(width, height);
}

void wxPluginFrame::GetSize(int *width, int *height) const
{
    if ( !m_hWnd )
    {
        *width = 0; *height = 0;
        return;
    }
    wxFrame::GetSize(width, height);
}

void wxPluginFrame::GetPosition(int *x, int *y) const
{
    if ( !m_hWnd )
    {
        *x = 0; *y = 0;
        return;
    }
    wxFrame::GetPosition(x, y);
}

void wxPluginFrame::SetAttributeValues(const int n, const char *argn[], const char *argv[])
{
    if ( m_names )
        delete[] m_names;
    if ( m_values )
        delete[] m_values;

    m_nAttributes = n;

    m_names = new wxString[n];
    m_values = new wxString[n];
    int i;
    for ( i = 0; i < n ; i ++)
    {
        m_names[i] = argn[i];
        m_values[i] = argv[i];
    }
}

void wxPluginFrame::SetAttributeValues(const int n, const wxString* argn, const wxString* argv)
{
    if ( m_names )
        delete[] m_names;
    if ( m_values )
        delete[] m_values;

    m_nAttributes = n;

    m_names = new wxString[n];
    m_values = new wxString[n];
    int i;
    for ( i = 0; i < n ; i ++)
    {
        m_names[i] = argn[i];
        m_values[i] = argv[i];
    }
}

void wxPluginFrame::SetSize(const int x, const int y, const int width, const int height, const int sizeFlags)
{
    // Can't allow app to set the size.
    return;
}

// Sets and subclasses the platform-specific window handle
bool wxPluginFrame::SetNPWindow(NPWindow *window)
{
    if ( !window || !window->window)
    {
        if ( m_hWnd )
        {
            wxMessageBox("Unsubclassing window prematurely");
            UnsubclassWin();
            m_hWnd = 0;
        }
        m_npWindow = NULL;
    }
    else
    {
        if ( m_hWnd )
        {
            if ( m_hWnd == (WXHWND) window->window )
            {
                // Does this mean a resize?
                return TRUE;
            }
        }

        m_npWindow = window;
        m_hWnd = (WXHWND) window->window;
        SubclassWin(m_hWnd);
        m_windowId = ::GetWindowLong((HWND) m_hWnd, GWL_ID);
    }
    return TRUE;
}

NPError wxPluginFrame::OnNPNewStream(NPMIMEType type, NPStream *stream, bool seekable, uint16* stype)
{
    *stype = NP_ASFILE;
    return NPERR_NO_ERROR;
}

void wxPluginFrame::OnNPNewFile(NPStream *stream, const wxString& fname)
{
}

void wxPluginFrame::OnNPPrint(NPPrint* printInfo)
{
        if (printInfo->mode == NP_FULL)
        {
            //
            // *Developers*: If your plugin would like to take over
            // printing completely when it is in full-screen mode,
            // set printInfo->pluginPrinted to TRUE and print your
            // plugin as you see fit.  If your plugin wants Netscape
            // to handle printing in this case, set printInfo->pluginPrinted
            // to FALSE (the default) and do nothing.  If you do want
            // to handle printing yourself, printOne is true if the
            // print button (as opposed to the print menu) was clicked.
            // On the Macintosh, platformPrint is a THPrint; on Windows,
            // platformPrint is a structure (defined in npapi.h) containing
            // the printer name, port, etc.
            //
            void* platformPrint = printInfo->print.fullPrint.platformPrint;
            NPBool printOne = printInfo->print.fullPrint.printOne;
            
            printInfo->print.fullPrint.pluginPrinted = FALSE; // Do the default
        
        }
        else    // If not fullscreen, we must be embedded
        {
            //
            // *Developers*: If your plugin is embedded, or is full-screen
            // but you returned false in pluginPrinted above, NPP_Print
            // will be called with mode == NP_EMBED.  The NPWindow
            // in the printInfo gives the location and dimensions of
            // the embedded plugin on the printed page.  On the Macintosh,
            // platformPrint is the printer port; on Windows, platformPrint
            // is the handle to the printing device context.
            //
            NPWindow* printWindow = &(printInfo->print.embedPrint.window);
            void* platformPrint = printInfo->print.embedPrint.platformPrint;

            HDC hDC = (HDC) platformPrint;
            wxRect rect;
            rect.x = printWindow->x;
            rect.y = printWindow->y;
            rect.width = printWindow->width;
            rect.height = printWindow->height;

			int saveIt = ::SaveDC(hDC);

            wxPrinterDC *printerDC = new wxPrinterDC((WXHDC) hDC);

            OnPrint(*printerDC, rect);

            printerDC->SetHDC(0);
			delete printerDC;

			::RestoreDC(hDC, saveIt);
        }
 }

void wxPluginFrame::OnPrint(wxPrinterDC& dc, wxRect& rect)
{
    // We must do some transformations here
	RECT winRect;
/*
	winRect.left = rect.x;
	winRect.top = rect.y;
	winRect.right = rect.x + rect.right;
	winRect.bottom = rect.y + rect.height;
*/
	POINT winPoint[2];
	winPoint[0].x = rect.x;
	winPoint[0].y = rect.y;
	winPoint[1].x = rect.x + rect.width;
	winPoint[1].y = rect.y + rect.height;

	if (!LPtoDP((HDC) dc.GetHDC(), winPoint, 2))
		wxMessageBox("LPtoDP failed.");

    OnDraw(dc);
}

void wxPluginFrame::OnDraw(wxDC& dc)
{
}

