
#include <mgraph.hpp>
#include "wx/dc.h"
#include "wx/image.h"
#include "tests.h"


pixel_format_t myformat =
	{0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0,0x00,0x00,0}; 	//RGB 24bpp

extern MGLDevCtx *g_displayDC;

#ifdef TEST_DC

void TestDC_1()
{
    wxDC dc;
    MGLMemoryDC *mgldc = new MGLMemoryDC(SIZEX, SIZEY, 24, &myformat);
    dc.SetMGLDC(mgldc, TRUE);
    
    RenderTest1(dc);
    
    mgldc->savePNGFromDC("test1.png", 0, 0, SIZEX, SIZEY);
}

void TestDC_2()
{
    wxDC dc;
    MGLMemoryDC *mgldc = new MGLMemoryDC(SIZEX, SIZEY, 24, &myformat);
    dc.SetMGLDC(mgldc, TRUE);
    
    RenderTest2(dc);
    
    mgldc->savePNGFromDC("test1_clipped.png", 0, 0, SIZEX, SIZEY);
}

#endif


extern bool wxMGL_Initialize();

int main()
{
    if (MGL_init(".", NULL) == 0)
       MGL_fatalError(MGL_errorMsg(MGL_result()));
    printf("MGL up and running\n");

    wxMGL_Initialize();

    g_displayDC = new MGLMemoryDC(640, 480, 24, &myformat);

#ifdef TEST_DC
    TestDC_1();
    TestDC_2();
#endif

#ifdef TEST_BITMAP
    TestBitmap();
#endif

    delete g_displayDC;
    MGL_exit();
    return 0;
}
