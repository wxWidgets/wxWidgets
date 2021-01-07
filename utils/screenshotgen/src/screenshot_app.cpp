/////////////////////////////////////////////////////////////////////////////
// Name:        screenshot_app.cpp
// Purpose:     Implement Application Class
// Author:      Utensil Candel (UtensilCandel@@gmail.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// for all others, include the necessary headers wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/richtext/richtextxml.h"

#include "screenshot_app.h"
#include "screenshot_main.h"


// ----------------------------------------------------------------------------
// ScreenshotApp
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(ScreenshotApp);

bool ScreenshotApp::OnInit()
{
    // Init all Image handlers
    wxInitAllImageHandlers();

    // Add richtext extra handlers (plain text is automatically added)
    wxRichTextBuffer::AddHandler(new wxRichTextXMLHandler);

    ScreenshotFrame* frame = new ScreenshotFrame(0L);
    frame->Show();

    return true;
}
