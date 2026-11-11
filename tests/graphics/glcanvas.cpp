///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/glcanvas.cpp
// Purpose:     wxGLCanvas Wayland scale-change regression test
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/glcanvas.h"
#include "wx/sizer.h"
#include "wx/stattext.h"

#include "waitfor.h"

#include <cstdio>

namespace
{

int GetTestSetting(const char* name, int fallback)
{
    wxString value;
    long number;
    return wxGetEnv(name, &value) && value.ToLong(&number) ? number : fallback;
}

} // anonymous namespace

// The external runner changes the output scale after the "initialised" line.
// Keep this hidden and in its own executable: a Wayland protocol error exits
// the entire process instead of producing a recoverable Catch assertion.
TEST_CASE("wxGLCanvas::WaylandScaleChange", "[glcanvas][wayland][.]")
{
    INFO("Run this test using tests/graphics/wayland/run.py");
    REQUIRE( IsRunningUnderWayland() );

    const int width = GetTestSetting("WX_GL_TEST_WIDTH", 1012);
    const int height = GetTestSetting("WX_GL_TEST_HEIGHT", 743);
    const int initialScale = GetTestSetting("WX_GL_TEST_INITIAL_SCALE", 1);
    const int finalScale = GetTestSetting("WX_GL_TEST_FINAL_SCALE", 2);
    const bool lateCurrent = GetTestSetting("WX_GL_TEST_LATE_CURRENT", 0) != 0;

    auto frame = make_unique<wxFrame>(nullptr, wxID_ANY, "Wayland scale test");
    // Keep some GTK content visible beside the EGL subsurface.
    auto* label = new wxStaticText(frame.get(), wxID_ANY, "wxGLCanvas scale test");
    label->SetMinSize(wxSize(-1, 30));
    const int attributes[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 24, 0};
    auto* canvas = new wxGLCanvas(frame.get(), wxID_ANY, attributes);
    auto context = make_unique<wxGLContext>(canvas);
    REQUIRE( context->IsOK() );
    canvas->Bind(wxEVT_PAINT, [canvas](wxPaintEvent&) {
        wxPaintDC dc(canvas);
    });
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(label, 0, wxEXPAND);
    sizer->Add(canvas, 1, wxEXPAND);
    frame->SetSizer(sizer);
    frame->SetClientSize(width, height + 30);
    frame->Show();
    REQUIRE( canvas->GetContentScaleFactor() == initialScale );

    if ( !lateCurrent )
    {
        REQUIRE( canvas->SetCurrent(*context) );
        // Allocate at the initial scale without presenting yet.
        glClear(GL_COLOR_BUFFER_BIT);
    }
    std::fprintf(stderr, "initialised wxWidgets=%s canvas=%dx%d scale=%.2f\n",
                 wxVERSION_NUM_DOT_STRING, width, height,
                 canvas->GetContentScaleFactor());

    REQUIRE( WaitFor("output scale change", [=]() {
        return canvas->GetContentScaleFactor() == finalScale;
    }, 5000) );
    REQUIRE( canvas->GetClientSize() == wxSize(width, height) );
    REQUIRE( canvas->SetCurrent(*context) );
    glClear(GL_COLOR_BUFFER_BIT);
    REQUIRE( WaitFor("first frame", [=]() { return canvas->SwapBuffers(); }, 5000) );

    // Dispatch the compositor's response, then present another frame. Returning
    // true from the first SwapBuffers() alone doesn't prove it was accepted.
    REQUIRE( WaitFor("second frame", [&]() {
        if ( !canvas->SetCurrent(*context) )
            return false;
        glClear(GL_COLOR_BUFFER_BIT);
        return canvas->SwapBuffers();
    }, 5000) );
    YieldForAWhile();
    std::fprintf(stderr, "completed canvas=%dx%d scale=%.2f\n",
                 canvas->GetClientSize().x, canvas->GetClientSize().y,
                 canvas->GetContentScaleFactor());
}
