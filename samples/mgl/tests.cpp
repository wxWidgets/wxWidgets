
#include "tests.h"

#ifdef TEST_DC

void RenderTest1(wxDC& dc)
{
    dc.SetBackground(*wxLIGHT_GREY_BRUSH);
    dc.Clear();

    wxPen pen;
    dc.SetPen(*wxRED_PEN);
    dc.DrawLine(10, 10, SIZEX-11, 10);
    dc.SetPen(*wxBLACK_DASHED_PEN);
    dc.DrawLine(10, 20, SIZEX-11, 20);
    dc.DrawLine(0, 39, SIZEX-1, 39);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawLine(10, 30, SIZEX-11, 30);
    pen = *wxRED_PEN;
    pen.SetWidth(10);
    dc.SetPen(pen);
    dc.DrawLine(10, 40, SIZEX-11, 40);
    pen = *wxBLACK_DASHED_PEN;
    pen.SetWidth(5);
    dc.SetPen(pen);
    dc.DrawLine(10, 50, SIZEX-11, 50);
    dc.SetPen(wxPen("blue", 1, wxDOT_DASH));
    dc.CrossHair(300,200);

    // ---------
    
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxBLUE_BRUSH);
    dc.DrawRectangle(10, 100, 100, 100);
    dc.DrawRectangle(10, 200, 100, 100);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(100, 100, 100, 100);
    dc.DrawRectangle(100, 200, 100, 100);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(*wxGREY_BRUSH);
    dc.DrawRectangle(300, 100, 100, 100);
    dc.DrawRectangle(300, 100, 100, 100);
    
    dc.SetPen(wxPen("red", 5, wxSOLID));
    dc.SetBrush(*wxBLUE_BRUSH);
    dc.SetLogicalFunction(wxXOR);
    dc.DrawRectangle(150, 150, 100, 100);
    dc.SetLogicalFunction(wxCOPY);

    // ---------

    dc.SetDeviceOrigin(400, 400);
    dc.SetPen(wxPen("green", 1, wxLONG_DASH));
    dc.DrawRoundedRectangle(0, 0, 200, 100, 25);
    dc.SetPen(wxPen("green", 5, wxSOLID));
    dc.DrawArc(240, 40, 320, 120, 280, 80);
    dc.DrawRoundedRectangle(0, 0, 200, 100, 25);
    dc.DrawCheckMark(-20, -20, 15, 15);
    dc.SetPen(wxPen("green", 5, wxCROSSDIAG_HATCH));
    dc.DrawEllipse(0, 250, 300, 100);
    dc.DrawEllipticArc(320, 250, 300, 100, 13, 260);

    // ---------
    
    dc.SetPen(wxPen("black", 3, wxSOLID));
    wxPoint pts[] = {wxPoint(20, 20), wxPoint(100,200), wxPoint(10,167)}; 
    dc.DrawPolygon(3, pts);
    dc.DrawLines(3, pts, 100, 100);
    dc.SetUserScale(2.0, 2.0);
    dc.DrawLines(3, pts, 100, 100);
    dc.SetUserScale(1.0, 1.0);


    // ---------

    dc.SetDeviceOrigin(500, 0);
    dc.SetPen(*wxCYAN_PEN);
    dc.SetBrush(wxBrush(*wxBLACK, wxBDIAGONAL_HATCH));
    dc.DrawRectangle(0, 0, 100, 50);
    dc.SetBrush(wxBrush(*wxBLACK, wxCROSSDIAG_HATCH));
    dc.DrawRectangle(0, 50, 100, 50);
    dc.SetBrush(wxBrush(*wxBLACK, wxFDIAGONAL_HATCH));
    dc.DrawRectangle(0, 100, 100, 50);
    dc.SetBrush(wxBrush(*wxBLACK, wxCROSS_HATCH));
    dc.DrawRectangle(0, 150, 100, 50);
    dc.SetBrush(wxBrush(*wxBLACK, wxHORIZONTAL_HATCH));
    dc.DrawRectangle(0, 200, 100, 50);
    dc.SetBrush(wxBrush(*wxBLACK, wxVERTICAL_HATCH));
    dc.DrawRectangle(0, 250, 100, 50);

    dc.SetDeviceOrigin(0, 0);
}


void RenderTest2(wxDC& dc)
{
    wxRegion clip;
    clip.Union(256,0,512,1024);
    clip.Union(0,256,900,512);
    
    dc.SetClippingRegion(clip);
    RenderTest1(dc);
    
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    for (wxRegionIterator it(clip); it.HaveRects(); it++)
        dc.DrawRectangle(it.GetRect());
    
}

#endif


#ifdef TEST_BITMAP

#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/image.h"
#include "wx/dcmemory.h"

#if defined(__WXMSW__)
#define FULLNAME(x) (x "_msw.png")
#elif defined(__WXGTK__)
#define FULLNAME(x) (x "_gtk.png")
#else
#define FULLNAME(x) (x ".png")
#endif



unsigned char g_XBM[] = {0x55,0x55,0xF0,0xF0,0xCC,0xCC,0x39,0x39,0xFF,0xFF,
                         0x55,0x55,0xF0,0xF0,0xCC,0xCC,0x39,0x39,0xFF,0xFF};
                                               /* - - - - - - - -*/;
                                               /*    ----    ----*/;
                                               /*--  --  --  --  */;
                                               /*  ---  -  ---  -*/;        
                                               /*----------------*/;        
                                               /* - - - - - - - -*/;
                                               /*    ----    ----*/;
                                               /*--  --  --  --  */;
                                               /*  ---  -  ---  -*/;        
                                               /*----------------*/;        

void TestBitmap()
{
    wxBitmap myBmp(800, 600);

    wxBitmap garfield("garfield.png", wxBITMAP_TYPE_PNG);
    wxBitmap bmask1("mask1.png", wxBITMAP_TYPE_PNG);
    wxBitmap bmask2("mask2.png", wxBITMAP_TYPE_PNG);
    wxBitmap blender("blender.png", wxBITMAP_TYPE_PNG);
    wxIcon blender_icon;
    blender_icon.CopyFromBitmap(blender);
    
    wxMemoryDC dc;
    dc.SelectObject(myBmp);
    
    dc.SetUserScale(2.0, 3.1);
    dc.DrawBitmap(garfield, 0, 0);
    dc.SetUserScale(1.0, 1.0);
    garfield.SetMask(new wxMask(bmask2, wxColour(0,0,0)));
    dc.DrawBitmap(garfield, 50, 50, TRUE);
    garfield.SaveFile(FULLNAME("test_garfield"), wxBITMAP_TYPE_PNG);
    garfield.SetMask(NULL);
    dc.DrawIcon(blender_icon, 10, 10);
    dc.DrawBitmap(blender, 80, 10);
    dc.DrawBitmap(blender, 160, 10, TRUE);
    dc.SetUserScale(3.0, 3.0);
    dc.DrawIcon(blender_icon, 10, 30);
    dc.SetUserScale(1.0, 1.0);
    
    const int ROPs[] = {wxAND_REVERSE, wxNOR, wxNAND, wxCLEAR, wxXOR, wxINVERT, 
                        wxOR_REVERSE, wxCOPY, wxAND, wxAND_INVERT, wxNO_OP, 
                        wxEQUIV, wxSRC_INVERT, wxOR_INVERT, wxOR, wxSET};
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {        
            dc.DrawBitmap(garfield, 200+130*x, 130*y);
            dc.SetLogicalFunction(ROPs[y*4+x]);
            dc.DrawBitmap(blender, 200+130*x+50, 130*y+50, TRUE);
            dc.SetLogicalFunction(wxCOPY);
        }
    }

    wxBitmap stipple1("stipple1.png", wxBITMAP_TYPE_PNG);
    wxBitmap stipple2("stipple2.png", wxBITMAP_TYPE_PNG);
    
    dc.DrawBitmap(stipple1, 10, 10, TRUE);
    dc.DrawBitmap(stipple2, 20, 10, TRUE);
    
#ifndef __WXGTK__
    dc.SetPen(wxPen(stipple2, 10));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);    
    dc.DrawRectangle(50, 50, 100, 100);
#endif    
    dc.SetPen(wxPen("black", 10, wxSOLID));
    dc.SetTextForeground("red");
    dc.SetTextBackground("blue");
    dc.SetBrush(wxBrush(stipple2));
    dc.DrawRectangle(50, 200, 100, 100);
    dc.SetBrush(wxBrush(stipple1));
    dc.DrawRectangle(50, 350, 100, 100);
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    
    wxBitmap xbm((const char*)g_XBM, 16, 10);
    dc.DrawBitmap(xbm, 700, 500);
    dc.SetUserScale(5.0, 5.0);
    dc.SetTextForeground("green");
    dc.SetTextBackground("red");
    dc.DrawBitmap(xbm, 720/5, 520/5);
    dc.SetUserScale(1.0, 1.0);

    dc.DrawBitmap(garfield.GetSubBitmap(wxRect(32, 32, 64, 64)), 720, 10);
    
#ifdef __WXMGL__
    dc.DrawIcon(wxICON(folder1), 10, 80);
#endif

    myBmp.SaveFile(FULLNAME("test_bmp1"), wxBITMAP_TYPE_PNG);
}


#endif
