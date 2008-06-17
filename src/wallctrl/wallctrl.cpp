#include "wx/wx.h"


#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif


#include "wx/Wallctrl/WallCtrl.h"


// Declare the event table
BEGIN_EVENT_TABLE(wxWallCtrl, wxGLCanvas)
    EVT_SIZE(wxWallCtrl::OnSize)
EVT_PAINT(wxWallCtrl::OnPaint)
   EVT_ERASE_BACKGROUND(wxWallCtrl::OnEraseBackground)
//	EVT_KEY_DOWN( wxWallCtrl::OnKeyDown )
//    EVT_KEY_UP( wxWallCtrl::OnKeyUp )
	EVT_ENTER_WINDOW( wxWallCtrl::OnEnterWindow )
END_EVENT_TABLE()