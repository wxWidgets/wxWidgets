#include <wx/wx.h>

// wxApp is the application object. It controls app startup and shutdown.
class MyApp : public wxApp
{
public:
    // OnInit() runs once when the app starts.
    // We create and show the main window here.
    bool OnInit() override
    {
        // Create a top-level window (wxFrame).
        // Why these parameters?
        // - nullptr: no parent -> this is a main window.
        // - wxID_ANY: let wxWidgets choose an internal ID.
        // - "Hello wxWidgets": text shown in the title bar.
        // - wxDefaultPosition: let the OS choose initial position.
        // - wxSize(640, 480): initial width and height.
        // - wxDEFAULT_FRAME_STYLE: normal frame buttons/border.
        // - "MyFrame": internal name (useful for framework internals/debugging).
        auto* frame = new wxFrame(
            nullptr,
            wxID_ANY,
            "Hello wxWidgets",
            wxDefaultPosition,
            wxSize(640, 480),
            wxDEFAULT_FRAME_STYLE,
            "MyFrame"
        );

        // Add a small status bar at the bottom of the window.
        frame->CreateStatusBar();
        // Show a short message in the status bar.
        frame->SetStatusText("Ready.");
        // Put the window in the center of the screen.
        frame->Centre();
        // Make the window visible. Without this, the user sees nothing.
        frame->Show(true);

        // Return true so the app continues running.
        return true;
    }
};

// Creates the platform-specific entry point (main/WinMain) for this app.
wxIMPLEMENT_APP(MyApp);