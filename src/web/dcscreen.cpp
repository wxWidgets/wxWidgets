// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcscreen.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/window.h"
    #include "wx/frame.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC,wxPaintDC)

wxScreenDC::wxScreenDC()
{
    m_ok = false;
}

wxScreenDC::~wxScreenDC()
{
}
