
#include "wx/dc.h"


//#define TEST_DC
#define TEST_BITMAP



#define SIZEX 1024
#define SIZEY 1024

#ifdef TEST_DC
extern void RenderTest1(wxDC& dc);
extern void RenderTest2(wxDC& dc);
#endif

#ifdef TEST_BITMAP
extern void TestBitmap();
#endif
