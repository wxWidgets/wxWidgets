#include "wx/wx.h"

class MyGestureFrame : public wxFrame
{
public:
    MyGestureFrame();

    void OnQuit(wxCloseEvent& event);
};
 
class MyGesturePanel : public wxPanel
{
public:
    MyGesturePanel(MyGestureFrame* parent);
 
    void OnPaint(wxPaintEvent& event);
    void OnPan(wxPanGestureEvent& event);
    void OnZoom(wxZoomGestureEvent& event);
    void OnRotate(wxRotateGestureEvent& event);
 
private:
    wxBitmap m_bitmap;
};
