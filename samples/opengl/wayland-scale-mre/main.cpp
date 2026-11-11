// Minimal wxGTK/EGL startup reproducer. wxWindows licence, see ../../../docs/licence.txt.
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <cstdlib>
#include <cstdio>
#include <memory>

class App : public wxApp
{
public:
    bool OnInit() override
    {
        SetAppName("wx-wayland-scale-mre");
        const int width = std::getenv("MRE_WIDTH") ? std::atoi(std::getenv("MRE_WIDTH")) : 1012;
        const int height = std::getenv("MRE_HEIGHT") ? std::atoi(std::getenv("MRE_HEIGHT")) : 743;
        auto* frame = new wxFrame(nullptr, wxID_ANY, "wx Wayland scale MRE");
        const int attributes[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 24, 0};
        auto* canvas = new wxGLCanvas(frame, wxID_ANY, attributes);
        auto context = std::make_shared<wxGLContext>(canvas);
        auto render = [canvas, context]() {
            const auto size = canvas->GetClientSize();
            std::fprintf(stderr, "paint canvas=%dx%d scale=%.2f\n",
                         size.x, size.y, canvas->GetContentScaleFactor());
            // Let the runner change the output scale before the first swap.
            if (std::getenv("MRE_WAIT_SCALE") &&
                canvas->GetContentScaleFactor() < std::atoi(std::getenv("MRE_WAIT_SCALE")))
                return;
            if (!canvas->SetCurrent(*context))
                return;
            glClearColor(0.15f, 0.35f, 0.65f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            if (canvas->SwapBuffers())
                std::fprintf(stderr, "frame presented\n");
        };
        canvas->Bind(wxEVT_PAINT, [canvas, render](wxPaintEvent&) {
            wxPaintDC dc(canvas);
            render();
        });
        auto* sizer = new wxBoxSizer(wxVERTICAL);
        // Keep part of the GTK parent visible beside the EGL subsurface.
        auto* label = new wxStaticText(frame, wxID_ANY, "Wayland / EGL scale reproducer");
        label->SetMinSize(wxSize(-1, 30));
        sizer->Add(label, 0, wxEXPAND);
        sizer->Add(canvas, 1, wxEXPAND);
        frame->SetSizer(sizer);
        frame->SetClientSize(width, height + 30);
        frame->Show();
        if (!std::getenv("MRE_LATE_CURRENT"))
        {
            // Allocate the first back buffer before GTK receives the new scale.
            canvas->SetCurrent(*context);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        std::fprintf(stderr, "initialised wxWidgets=%s requested=%dx%d scale=%.2f EGL=%d\n",
                     wxVERSION_NUM_DOT_STRING, width, height,
                     canvas->GetContentScaleFactor(), wxUSE_GLCANVAS_EGL);
        return true;
    }
};

wxIMPLEMENT_APP(App);
