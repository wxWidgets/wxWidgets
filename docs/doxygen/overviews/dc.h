/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_dc Device Contexts

A wxDC is a @e device context onto which graphics and text can be drawn.
The device context is intended to represent a number of output devices in a
generic way, with the same API being used throughout.

Objects of wxDC class itself can't be created, instead you should create
objects of the following classes:

@li wxPaintDC for painting on a window from within a paint event handler. This
    is the most common device context to use.
@li wxMemoryDC for painting off-screen, i.e. to a bitmap.
@li wxPrinterDC for printing.
@li wxInfoDC for obtaining information about the device context without drawing
    on it.

To draw on a window, you need to create a wxPaintDC object in the paint event
handler:
@code
void MyWindow::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    dc.DrawText("Hello, world!", 20, 20);
}
@endcode

To obtain information about a device context associated with a window outside
of its paint event handler, you need to use wxInfoDC, e.g.
the window as argument, for example:

@code
void MyFrame::SomeFunction()
{
    wxInfoDC dc(this);

    // Create control with the width just big enough for the given string.
    auto* text = new wxStaticText
                     (
                        this, wxID_ANY, "",
                        wxPoint(),
                        dc.GetTextExtent("String of max length"),
                        wxST_NO_AUTORESIZE
                     );
}
@endcode

When writing drawing code, it is recommended to extract it into a function
taking wxDC as an argument. This allows you to reuse the same code for drawing
and printing, by calling it with either wxPaintDC or wxPrinterDC.

Please note that other device context classes that could previously be used for
painting on screen cannot be used any more due to the architecture of the
modern graphics systems. In particular, wxClientDC, wxWindowDC and wxScreenDC
are not guaranteed to work any longer.

@see @ref group_class_dc

*/
