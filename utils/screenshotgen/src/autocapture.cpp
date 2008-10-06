/////////////////////////////////////////////////////////////////////////////
// Name:        autocapture.cpp
// Purpose:     Implement wxCtrlMaskOut class
// Author:      Utensil Candel (UtensilCandel@@gmail.com)
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/filename.h>

#include "autocapture.h"


wxBitmap Capture(int x, int y, int width, int height)
{
    //Somehow wxScreenDC.Blit() doesn't work under Mac for now. Here is a trick.
    #ifdef __WXMAC__

    //wxExecute(_T("screencapture -x ") + tempfile, wxEXEC_SYNC);

    system("screencapture -x /tmp/wx_screen_capture.png");

    wxBitmap fullscreen;

    do
    {
        fullscreen = wxBitmap(_T("/tmp/wx_screen_capture.png"), wxBITMAP_TYPE_PNG);
    }
    while(!fullscreen.IsOk());

    wxBitmap screenshot = fullscreen.GetSubBitmap(wxRect(x,y,width,height));

    #else //Under other paltforms, take a real screenshot

    //Create a DC for the whole screen area
    wxScreenDC dcScreen;

    //Create a Bitmap that will later on hold the screenshot image
    //Note that the Bitmap must have a size big enough to hold the screenshot
    //-1 means using the current default colour depth
    wxBitmap screenshot(width, height, -1);

    //Create a memory DC that will be used for actually taking the screenshot
    wxMemoryDC memDC;
    //Tell the memory DC to use our Bitmap
    //all drawing action on the memory DC will go to the Bitmap now
    memDC.SelectObject(screenshot);
    //Blit (in this case copy) the actual screen on the memory DC
    //and thus the Bitmap
    memDC.Blit( 0, //Copy to this X coordinate
                0, //Copy to this Y coordinate
                width, //Copy this width
                height, //Copy this height
                &dcScreen, //From where do we copy?
                x, //What's the X offset in the original DC?
                y  //What's the Y offset in the original DC?
                    );
    //Select the Bitmap out of the memory DC by selecting a new
    //uninitialized Bitmap
    memDC.SelectObject(wxNullBitmap);
    #endif //#ifdef __WXMAC__

//    wxMessageBox(_(""),_(""));

    return screenshot;

}

wxBitmap Capture(wxRect rect)
{
    wxPoint origin = rect.GetPosition();
    return Capture(origin.x, origin.y, rect.GetWidth(), rect.GetHeight());
}

void AutoCaptureMechanism::Save(wxBitmap screenshot, wxString fileName)
{
    //Check if m_defaultDir already existed
    if(!wxDirExists(m_dir))
        wxMkdir(m_dir);

    wxString fullFileName = m_dir + wxFileName::GetPathSeparator() + fileName;

    //to prvent overwritten
    while(wxFileName::FileExists(fullFileName + _T(".png"))) fullFileName += _T("_");

    //Our Bitmap now has the screenshot, so let's save it as an png
    //The filename itself is without extension.
    screenshot.SaveFile(fullFileName + _T(".png"), wxBITMAP_TYPE_PNG);
}

wxRect AutoCaptureMechanism::GetRect(wxWindow* ctrl, int flag)
{
    if(flag & AJ_RegionAdjust)
    {
        wxWindow * parent = ctrl->GetParent();
        wxSizer * sizer = parent->GetSizer();

        if(sizer)
        {
            sizer->Detach(ctrl);

            /*
            +---------+-----------+---------+
            |    0    |   label   |    1    |
            +---------+-----------+---------+
            |  label  |    ctrl   |  label  |
            +---------+-----------+---------+
            |    2    |   label   |    3    |
            +---------+-----------+---------+
           */

            m_grid = new wxFlexGridSizer(3, 3, m_border, m_border);

            wxStaticText* l[4];

            for(int i = 0; i < 4; ++i)
                l[i] = new wxStaticText(parent, wxID_ANY, wxT(" "));

            m_grid->Add(l[0]);
            m_grid->Add(new wxStaticText(parent, wxID_ANY, wxT(" ")));
            m_grid->Add(l[1]);
            m_grid->Add(new wxStaticText(parent, wxID_ANY, wxT(" ")));
            m_grid->Add(ctrl);
            m_grid->Add(new wxStaticText(parent, wxID_ANY, wxT(" ")));
            m_grid->Add(l[2]);
            m_grid->Add(new wxStaticText(parent, wxID_ANY, wxT(" ")));
            m_grid->Add(l[3]);

            sizer->Add(m_grid);
            parent->SetSizer(sizer);
            parent->Layout();

            parent->Refresh();
            wxYield();

            return wxRect(l[0]->GetScreenRect().GetBottomRight(),
                    l[3]->GetScreenRect().GetTopLeft());

        }
        else  //Actually it won't get here working with the current guiframe.h/guiframe.cpp
        {
            return ctrl->GetScreenRect().Inflate(m_border);
        }
    }
    else
    {
       return ctrl->GetScreenRect().Inflate(m_border);
    }
}

void AutoCaptureMechanism::PutBack(wxWindow * ctrl)
{
    m_grid->Detach(ctrl);

    wxSizerItemList children = m_grid->GetChildren();

    for(wxSizerItemList::iterator it = children.begin(); it != children.end(); ++it)
    {
        wxSizerItem* item = *it;
        if(item->IsWindow()) delete (*it)->GetWindow();
    }

    wxSizer * sizer = ctrl->GetParent()->GetSizer();
    sizer->Detach(m_grid);
    delete m_grid;
    sizer->Add(ctrl);
}

