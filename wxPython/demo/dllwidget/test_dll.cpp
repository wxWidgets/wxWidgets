
#include <wx/defs.h>
#include <wx/window.h>
#include <wx/msgdlg.h>

#include "../../contrib/dllwidget/dllwidget.h"

class TestWindow : public wxWindow
{
public:
    TestWindow(wxWindow *parent, long style)
        : wxWindow(parent, wxID_ANY)
    {
        SetBackgroundColour(wxColour("white"));
    }

    int HandleCommand(int cmd, const wxString& param)
    {
        if (cmd == 1)
        {
            SetBackgroundColour(wxColour("red"));
            Refresh();
        }

        if (cmd == 2)
        {
            SetBackgroundColour(wxColour(param));
            Refresh();
        }

        else if (cmd == 3)
        {
            wxMessageBox("Message from embedded widget:\n\n" + param);
        }

        return 0;
    }
private:
    DECLARE_ABSTRACT_CLASS(TestWindow)
};

IMPLEMENT_ABSTRACT_CLASS(TestWindow, wxWindow)


//DECLARE_DLL_WIDGET(TestWindow)
static int SendCommandToTestWindow(wxWindow *wnd, int cmd, const wxString& param)
{
    return wxStaticCast(wnd, TestWindow)->HandleCommand(cmd, param);
}


BEGIN_WIDGET_LIBRARY()
    REGISTER_WIDGET(TestWindow)
END_WIDGET_LIBRARY()
