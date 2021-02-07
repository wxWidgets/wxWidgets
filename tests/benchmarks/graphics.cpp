/////////////////////////////////////////////////////////////////////////////
// Name:        graphics.cpp
// Purpose:     Some benchmarks for measuring graphics operations performance
// Author:      Vadim Zeitlin
// Created:     2008-04-13
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/cmdline.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/dcgraph.h"
#include "wx/image.h"
#include "wx/rawbmp.h"
#include "wx/stopwatch.h"
#include "wx/crt.h"

#if wxUSE_GLCANVAS
    #include "wx/glcanvas.h"
    #ifdef _MSC_VER
        #pragma comment(lib, "opengl32")
    #endif
#endif // wxUSE_GLCANVAS

#if wxUSE_GLCANVAS

GLuint g_texture;
wxImage g_image;

void InitializeTexture(int w, int h)
{
    glGenTextures(1, &g_texture);
    glBindTexture(GL_TEXTURE_2D, g_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    g_image.Create(w, h, false /* don't clear */);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0,
                 GL_RGB, g_image.GetWidth(), g_image.GetHeight(), 0,
                 GL_RGB, GL_UNSIGNED_BYTE, g_image.GetData());
}
#endif // wxUSE_GLCANVAS

struct GraphicsBenchmarkOptions
{
    GraphicsBenchmarkOptions()
    {
        mapMode = 0;
        penWidth = 0;
        penStyle = wxPENSTYLE_INVALID;
        penQuality = wxPEN_QUALITY_DEFAULT;

        width = 800;
        height = 600;

        numIters = 1000;

        testBitmaps =
        testImages =
        testLines =
        testRawBitmaps =
        testRectangles =
        testCircles =
        testEllipses =
        testTextExtent =
        testMultiLineTextExtent =
        testPartialTextExtents = false;

        usePaint =
        useClient =
        useMemory = false;

        useDC =
        useGC =
        useGL = false;

        renderer = Default;
    }

    long mapMode,
         penWidth,
         width,
         height,
         numIters;

    wxPenStyle penStyle;
    wxPenQuality penQuality;

    bool testBitmaps,
         testImages,
         testLines,
         testRawBitmaps,
         testRectangles,
         testCircles,
         testEllipses,
         testTextExtent,
         testMultiLineTextExtent,
         testPartialTextExtents;

    bool usePaint,
         useClient,
         useMemory;

    bool useDC,
         useGC,
         useGL;

#ifdef __WXMSW__
    enum GraphicsRenderer { Default, GDIPlus, Direct2D, Cairo };
#else
    enum GraphicsRenderer { Default };
#endif // __WXMSW__ / !__WXMSW__
    GraphicsRenderer renderer;
} opts;

class GraphicsBenchmarkFrame : public wxFrame
{
public:
    GraphicsBenchmarkFrame()
        : wxFrame(NULL, wxID_ANY, "wxWidgets Graphics Benchmark")
    {
        SetClientSize(opts.width, opts.height);

#if wxUSE_GLCANVAS
        m_glCanvas = NULL;
        m_glContext = NULL;

        if ( opts.useGL )
        {
            m_glCanvas = new wxGLCanvas(this, wxID_ANY, NULL,
                                        wxPoint(0, 0),
                                        wxSize(opts.width, opts.height));
            m_glContext = new wxGLContext(m_glCanvas);
            m_glContext->SetCurrent(*m_glCanvas);

            glViewport(0, 0, opts.width, opts.height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-1, 1, -1, 1, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            InitializeTexture(opts.width, opts.height);

            m_glCanvas->Connect(
                wxEVT_PAINT,
                wxPaintEventHandler(GraphicsBenchmarkFrame::OnGLRender),
                NULL,
                this
            );
        }
        else // Not using OpenGL
#endif // wxUSE_GLCANVAS
        {
            Connect(wxEVT_PAINT,
                    wxPaintEventHandler(GraphicsBenchmarkFrame::OnPaint));
        }

        Connect(wxEVT_SIZE, wxSizeEventHandler(GraphicsBenchmarkFrame::OnSize));

        m_bitmapARGB.Create(64, 64, 32);
#if defined(__WXMSW__) || defined(__WXOSX__)
        m_bitmapARGB.UseAlpha(true);
#endif // __WXMSW__ || _WXOSX__
        m_bitmapRGB.Create(64, 64, 24);

        wxBitmap bmpMask(64, 64, 1);
        {
            wxMemoryDC dc(bmpMask);
            dc.SetBackground(*wxBLACK_BRUSH);
            dc.Clear();
        }
        m_bitmapARGBwithMask.Create(64, 64, 32);
#if defined(__WXMSW__) || defined(__WXOSX__)
        m_bitmapARGBwithMask.UseAlpha(true);
#endif // __WXMSW__ || __WXOSX__
        m_bitmapARGBwithMask.SetMask(new wxMask(bmpMask));

        m_bitmapRGBwithMask.Create(64, 64, 24);
        m_bitmapRGBwithMask.SetMask(new wxMask(bmpMask));

        m_renderer = NULL;
        if ( opts.useGC )
        {
#ifdef __WXMSW__
            if ( opts.renderer == GraphicsBenchmarkOptions::GDIPlus )
                m_renderer = wxGraphicsRenderer::GetGDIPlusRenderer();
            else if ( opts.renderer == GraphicsBenchmarkOptions::Direct2D )
                m_renderer = wxGraphicsRenderer::GetDirect2DRenderer();
            else if ( opts.renderer == GraphicsBenchmarkOptions::Cairo )
                m_renderer = wxGraphicsRenderer::GetCairoRenderer();
            // Check if selected renderer is operational.
            if ( m_renderer )
            {
                wxBitmap bmp(16, 16);
                wxMemoryDC memDC(bmp);
                wxGraphicsContext* gc = m_renderer->CreateContext(memDC);
                if ( !gc )
                {
                    wxPrintf("Couldn't initialize '%s' graphics renderer.\n", m_renderer->GetName().c_str());
                    m_renderer = NULL;
                }
                delete gc;
            }
#endif // __WXMSW__

            if( !m_renderer )
                m_renderer = wxGraphicsRenderer::GetDefaultRenderer();
        }

        Show();
    }

#if wxUSE_GLCANVAS
    virtual ~GraphicsBenchmarkFrame()
    {
        delete m_glContext;
    }
#endif // wxUSE_GLCANVAS

private:
    // Just change the image in some (quick) way to show that it's really being
    // updated on screen.
    void UpdateRGB(unsigned char* data, int n)
    {
        for ( int y = 0; y < opts.height; ++y )
        {
            memset(data, n % 256, 3*opts.width);

            data += 3*opts.width;
            n++;
        }
    }

#if wxUSE_GLCANVAS
    void OnGLRender(wxPaintEvent& WXUNUSED(event))
    {
        m_glContext->SetCurrent(*m_glCanvas);
        glEnable(GL_TEXTURE_2D);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        wxPrintf("Benchmarking %s: ", "OpenGL images");
        fflush(stdout);

        wxStopWatch sw;
        for ( int n = 0; n < opts.numIters; n++ )
        {
            UpdateRGB(g_image.GetData(), n);

            glTexSubImage2D(GL_TEXTURE_2D, 0,
                            0, 0, opts.width, opts.height,
                            GL_RGB, GL_UNSIGNED_BYTE, g_image.GetData());
            glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex2f(-1.0, -1.0);

                glTexCoord2f(0, 1);
                glVertex2f(-1.0, 1.0);

                glTexCoord2f(1, 1);
                glVertex2f(1.0, 1.0);

                glTexCoord2f(1, 0);
                glVertex2f(1.0, -1.0);
            glEnd();

            m_glCanvas->SwapBuffers();
        }

        const long t = sw.Time();

        wxPrintf("%ld images done in %ldms = %gus/image or %ld FPS\n",
                 opts.numIters, t, (1000. * t)/opts.numIters,
                 (1000*opts.numIters + t - 1)/t);

        wxTheApp->ExitMainLoop();
    }
#endif // wxUSE_GLCANVAS

    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        if ( opts.usePaint )
        {
            {
                wxPaintDC dc(this);
                wxGCDC gcdc;
                if ( m_renderer )
                {
                    wxGraphicsContext* gc = m_renderer->CreateContext(dc);
                    gcdc.SetGraphicsContext(gc);
                }
                BenchmarkDCAndGC("paint", dc, gcdc);
            }
            // Since some renderers use back buffers and hence
            // drawing results are not displayed when the test
            // is running then wait a second after graphics
            // contents is commited to DC to present the output.
            wxSleep(1);
        }

        if ( opts.useClient )
        {
            {
                wxClientDC dc(this);
                wxGCDC gcdc;
                if ( m_renderer )
                {
                    wxGraphicsContext* gc = m_renderer->CreateContext(dc);
                    gcdc.SetGraphicsContext(gc);
                }
                BenchmarkDCAndGC("client", dc, gcdc);
            }
            // Since some renderers use back buffers and hence
            // drawing results are not displayed when the test
            // is running then wait a second after graphics
            // contents is commited to DC to present the output.
            wxSleep(1);
        }

        if ( opts.useMemory )
        {
            {
                wxBitmap bmp(opts.width, opts.height);
                wxMemoryDC dc(bmp);
                wxGCDC gcdc;
                if ( m_renderer )
                {
                    wxGraphicsContext* gc = m_renderer->CreateContext(dc);
                    gcdc.SetGraphicsContext(gc);
                }
                BenchmarkDCAndGC("default memory", dc, gcdc);
            }
            {
                wxBitmap bmp(opts.width, opts.height, 24);
                wxMemoryDC dc(bmp);
                wxGCDC gcdc;
                if ( m_renderer )
                {
                    wxGraphicsContext* gc = m_renderer->CreateContext(dc);
                    gcdc.SetGraphicsContext(gc);
                }
                BenchmarkDCAndGC("RGB memory", dc, gcdc);
            }
            {
#if defined(__WXMSW__) || defined(__WXOSX__)
                wxBitmap bmp(opts.width, opts.height, 32);
                bmp.UseAlpha(false);
                wxMemoryDC dc(bmp);
                wxGCDC gcdc;
                if ( m_renderer )
                {
                    wxGraphicsContext* gc = m_renderer->CreateContext(dc);
                    gcdc.SetGraphicsContext(gc);
                }
                BenchmarkDCAndGC("0RGB memory", dc, gcdc);
#endif // __WXMSW__ ||__WXOSX__
            }
            {
                wxBitmap bmp(opts.width, opts.height, 32);
#if defined(__WXMSW__) || defined(__WXOSX__)
                bmp.UseAlpha(true);
#endif // __WXMSW__ || __WXOSX__
                wxMemoryDC dc(bmp);
                wxGCDC gcdc;
                if ( m_renderer )
                {
                    wxGraphicsContext* gc = m_renderer->CreateContext(dc);
                    gcdc.SetGraphicsContext(gc);
                }
                BenchmarkDCAndGC("ARGB memory", dc, gcdc);
            }

        }

        wxTheApp->ExitMainLoop();
    }

    void BenchmarkDCAndGC(const char* dckind, wxDC& dc, wxGCDC& gcdc)
    {
        if ( opts.useDC )
        {
            BenchmarkAll(wxString::Format("%6s DC", dckind), dc);
        }
        else if ( opts.useGC && gcdc.IsOk() )
        {
            wxString rendName = gcdc.GetGraphicsContext()->GetRenderer()->GetName();
            BenchmarkAll(wxString::Format("%6s GC (%s)", dckind, rendName.c_str()), gcdc);
        }
    }

    void BenchmarkAll(const wxString& msg, wxDC& dc)
    {
        BenchmarkBitmaps(msg, dc);
        BenchmarkImages(msg, dc);
        BenchmarkLines(msg, dc);
        BenchmarkRawBitmaps(msg, dc);
        BenchmarkRectangles(msg, dc);
        BenchmarkRoundedRectangles(msg, dc);
        BenchmarkCircles(msg, dc);
        BenchmarkEllipses(msg, dc);
        BenchmarkTextExtent(msg, dc);
        BenchmarkPartialTextExtents(msg, dc);
    }

    void SetupDC(wxDC& dc)
    {
        if ( opts.mapMode != 0 )
            dc.SetMapMode((wxMappingMode)opts.mapMode);

        bool setPen = false;
        wxPenInfo penInfo(*wxWHITE);
        if ( opts.penWidth != 0 )
        {
            penInfo.Width(opts.penWidth);
            setPen = true;
        }

        if ( opts.penStyle != wxPENSTYLE_INVALID )
        {
            penInfo.Style(opts.penStyle);
            setPen = true;
        }

        if ( opts.penQuality != wxPEN_QUALITY_DEFAULT )
        {
            penInfo.Quality(opts.penQuality);
            setPen = true;
        }

        if ( setPen )
            dc.SetPen(penInfo);
    }

    void BenchmarkLines(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testLines )
            return;

        SetupDC(dc);

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        wxStopWatch sw;
        int x0 = 0,
            y0 = 0;
        for ( int n = 0; n < opts.numIters; n++ )
        {
            int x1 = rand() % opts.width,
                y1 = rand() % opts.height;

            dc.DrawLine(x0, y0, x1, y1);

            x0 = x1;
            y0 = y1;
        }

        const long t = sw.Time();

        wxPrintf("%ld lines done in %ldms = %gus/line\n",
                 opts.numIters, t, (1000. * t)/opts.numIters);

        // Horizontal lines
        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        sw.Start();
        x0 = 0;
        for ( int n = 0; n < opts.numIters; n++ )
        {
            int x1 = rand() % opts.width;
            int y = rand() % opts.height;

            dc.DrawLine(x0, y, x1, y);

            x0 = x1;
        }

        const long t2 = sw.Time();

        wxPrintf("%ld horizontal lines done in %ldms = %gus/line\n",
            opts.numIters, t2, (1000. * t2) / opts.numIters);

        // Vertical lines
        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        sw.Start();
        y0 = 0;
        for ( int n = 0; n < opts.numIters; n++ )
        {
            int x = rand() % opts.width;
            int y1 = rand() % opts.height;

            dc.DrawLine(x, y0, x, y1);

            y0 = y1;
        }

        const long t3 = sw.Time();

        wxPrintf("%ld vertical lines done in %ldms = %gus/line\n",
            opts.numIters, t3, (1000. * t3) / opts.numIters);

        // Cross hair
        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        sw.Start();
        for ( int n = 0; n < opts.numIters; n++ )
        {
            x0 = rand() % opts.width;
            y0 = rand() % opts.height;

            dc.CrossHair(x0, y0);
        }
        const long t4 = sw.Time();

        wxPrintf("%ld cross hairs done in %ldms = %gus/line\n",
            opts.numIters, t4, (1000. * t4) / (2*opts.numIters));
    }


    void BenchmarkRectangles(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testRectangles )
            return;

        SetupDC(dc);

        dc.SetBrush( *wxRED_BRUSH );

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        wxStopWatch sw;
        for ( int n = 0; n < opts.numIters; n++ )
        {
            int x = rand() % opts.width,
                y = rand() % opts.height;

            dc.DrawRectangle(x, y, 32, 32);
        }

        const long t = sw.Time();

        wxPrintf("%ld rects done in %ldms = %gus/rect\n",
                 opts.numIters, t, (1000. * t)/opts.numIters);
    }

    void BenchmarkRoundedRectangles(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testRectangles )
            return;

        SetupDC(dc);

        dc.SetBrush( *wxCYAN_BRUSH );

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        wxStopWatch sw;
        for ( int n = 0; n < opts.numIters; n++ )
        {
            int x = rand() % opts.width,
                y = rand() % opts.height;

            dc.DrawRoundedRectangle(x, y, 48, 32, 8);
        }

        const long t = sw.Time();

        wxPrintf("%ld rounded rects done in %ldms = %gus/rect\n",
                 opts.numIters, t, (1000. * t)/opts.numIters);
    }

    void BenchmarkCircles(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testCircles )
            return;

        SetupDC(dc);

        dc.SetBrush( *wxGREEN_BRUSH );

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        wxStopWatch sw;
        for ( long n = 0; n < opts.numIters; n++ )
        {
            int x = rand() % opts.width,
                y = rand() % opts.height;

            dc.DrawCircle(x, y, 32);
        }

        const long t = sw.Time();

        wxPrintf("%ld circles done in %ldms = %gus/circle\n",
                 opts.numIters, t, (1000. * t)/opts.numIters);
    }

    void BenchmarkEllipses(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testEllipses )
            return;

        SetupDC(dc);

        dc.SetBrush( *wxBLUE_BRUSH );

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        wxStopWatch sw;
        for ( long n = 0; n < opts.numIters; n++ )
        {
            int x = rand() % opts.width,
                y = rand() % opts.height;

            dc.DrawEllipse(x, y, 48, 32);
        }

        const long t = sw.Time();

        wxPrintf("%ld ellipses done in %ldms = %gus/ellipse\n",
                 opts.numIters, t, (1000. * t)/opts.numIters);
    }

    void BenchmarkTextExtent(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testTextExtent )
            return;

        SetupDC(dc);

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        const wxString str("The quick brown fox jumps over the lazy dog");
        wxSize size;

        wxStopWatch sw;
        for ( long n = 0; n < opts.numIters; n++ )
        {
            if ( opts.testMultiLineTextExtent )
                size += dc.GetMultiLineTextExtent(str);
            else
                size += dc.GetTextExtent(str);
        }

        const long t = sw.Time();

        wxPrintf("%ld text extent measures done in %ldms = %gus/call\n",
                 opts.numIters, t, (1000. * t)/opts.numIters);
    }

    void BenchmarkPartialTextExtents(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testPartialTextExtents )
            return;

        SetupDC(dc);

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        const wxString str("The quick brown fox jumps over the lazy dog");
        wxArrayInt widths;

        wxStopWatch sw;
        for ( long n = 0; n < opts.numIters; n++ )
        {
            if ( !dc.GetPartialTextExtents(str, widths) )
            {
                wxPrintf("ERROR: GetPartialTextExtents() failed\n");
                return;
            }
        }

        const long t = sw.Time();

        wxPrintf("%ld partial text extents measures done in %ldms = %gus/call\n",
                 opts.numIters, t, (1000. * t)/opts.numIters);
    }

    void BenchmarkBitmaps(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testBitmaps )
            return;

        SetupDC(dc);

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        wxStopWatch sw;
        for ( int n = 0; n < opts.numIters; n++ )
        {
            int x = rand() % opts.width,
                y = rand() % opts.height;

            dc.DrawBitmap(m_bitmapARGB, x, y, true);
        }

        const long t = sw.Time();

        wxPrintf("%ld ARGB bitmaps done in %ldms = %gus/bitmap\n",
                 opts.numIters, t, (1000. * t)/opts.numIters);

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        sw.Start();
        for ( int n = 0; n < opts.numIters; n++ )
        {
            int x = rand() % opts.width,
                y = rand() % opts.height;

            dc.DrawBitmap(m_bitmapRGB, x, y, true);
        }
        const long t2 = sw.Time();

        wxPrintf("%ld RGB bitmaps done in %ldms = %gus/bitmap\n",
                 opts.numIters, t2, (1000. * t2)/opts.numIters);

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        sw.Start();
        for ( int n = 0; n < opts.numIters; n++ )
        {
            int x = rand() % opts.width,
                y = rand() % opts.height;

            dc.DrawBitmap(m_bitmapARGBwithMask, x, y, true);
        }
        const long t3 = sw.Time();

        wxPrintf("%ld ARGB bitmaps with mask done in %ldms = %gus/bitmap\n",
            opts.numIters, t3, (1000. * t3) / opts.numIters);

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        sw.Start();
        for ( int n = 0; n < opts.numIters; n++ )
        {
            int x = rand() % opts.width,
                y = rand() % opts.height;

            dc.DrawBitmap(m_bitmapRGBwithMask, x, y, true);
        }
        const long t4 = sw.Time();

        wxPrintf("%ld RGB bitmaps with mask done in %ldms = %gus/bitmap\n",
            opts.numIters, t4, (1000. * t4) / opts.numIters);
    }

    void BenchmarkImages(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testImages )
            return;

        if ( opts.mapMode != 0 )
            dc.SetMapMode((wxMappingMode)opts.mapMode);

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        wxImage image(wxSize(opts.width, opts.height), false /* don't clear */);

        wxStopWatch sw;
        for ( int n = 0; n < opts.numIters; n++ )
        {
            UpdateRGB(image.GetData(), n);
            dc.DrawBitmap(image, 0, 0);
        }

        const long t = sw.Time();

        wxPrintf("%ld images done in %ldms = %gus/image or %ld FPS\n",
                 opts.numIters, t, (1000. * t)/opts.numIters,
                 (1000*opts.numIters + t - 1)/t);
    }

    void BenchmarkRawBitmaps(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testRawBitmaps )
            return;

        if ( opts.mapMode != 0 )
            dc.SetMapMode((wxMappingMode)opts.mapMode);

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        wxBitmap bitmap(opts.width, opts.height, 24);
        wxNativePixelData data(bitmap);

        wxStopWatch sw;
        for ( int n = 0; n < opts.numIters; n++ )
        {
            unsigned char c = n % 256;
            {
                wxNativePixelData::Iterator p(data);
                for ( int y = 0; y < opts.height; ++y )
                {
                    wxNativePixelData::Iterator rowStart = p;

                    for ( int x = 0; x < opts.width; ++x )
                    {
                        p.Red() =
                        p.Green() =
                        p.Blue() = c;
                        ++p;
                    }

                    p = rowStart;
                    p.OffsetY(data, 1);
                    c++;
                }
            }

            dc.DrawBitmap(bitmap, 0, 0);
        }

        const long t = sw.Time();

        wxPrintf("%ld raw bitmaps done in %ldms = %gus/bitmap or %ld FPS\n",
                 opts.numIters, t, (1000. * t)/opts.numIters,
                 (1000*opts.numIters + t - 1)/t);
    }


    wxBitmap m_bitmapARGB;
    wxBitmap m_bitmapRGB;
    wxBitmap m_bitmapARGBwithMask;
    wxBitmap m_bitmapRGBwithMask;
#if wxUSE_GLCANVAS
    wxGLCanvas* m_glCanvas;
    wxGLContext* m_glContext;
#endif // wxUSE_GLCANVAS
    wxGraphicsRenderer* m_renderer;
};

class GraphicsBenchmarkApp : public wxApp
{
public:
    virtual void OnInitCmdLine(wxCmdLineParser& parser)
    {
        static const wxCmdLineEntryDesc desc[] =
        {
            { wxCMD_LINE_SWITCH, "",  "bitmaps" },
            { wxCMD_LINE_SWITCH, "",  "images" },
            { wxCMD_LINE_SWITCH, "",  "lines" },
            { wxCMD_LINE_SWITCH, "",  "rawbmp" },
            { wxCMD_LINE_SWITCH, "",  "rectangles" },
            { wxCMD_LINE_SWITCH, "",  "circles" },
            { wxCMD_LINE_SWITCH, "",  "ellipses" },
            { wxCMD_LINE_SWITCH, "",  "textextent" },
            { wxCMD_LINE_SWITCH, "",  "multilinetextextent" },
            { wxCMD_LINE_SWITCH, "",  "partialtextextents" },
            { wxCMD_LINE_SWITCH, "",  "paint" },
            { wxCMD_LINE_SWITCH, "",  "client" },
            { wxCMD_LINE_SWITCH, "",  "memory" },
            { wxCMD_LINE_SWITCH, "",  "dc" },
            { wxCMD_LINE_SWITCH, "",  "gc" },
#if wxUSE_GLCANVAS
            { wxCMD_LINE_SWITCH, "",  "gl" },
#endif // wxUSE_GLCANVAS
            { wxCMD_LINE_OPTION, "m", "map-mode", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "p", "pen-width", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "s", "pen-style", "solid | dot | long_dash | short_dash", wxCMD_LINE_VAL_STRING },
            { wxCMD_LINE_OPTION, "",  "pen-quality", "default | low | high", wxCMD_LINE_VAL_STRING },
            { wxCMD_LINE_OPTION, "w", "width", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "h", "height", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "I", "images", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "N", "number-of-iterations", "", wxCMD_LINE_VAL_NUMBER },
#ifdef __WXMSW__
            { wxCMD_LINE_OPTION, "r", "renderer", "gdiplus | direct2d | cairo", wxCMD_LINE_VAL_STRING },
#endif // __WXMSW__
            { wxCMD_LINE_NONE },
        };

        parser.SetDesc(desc);
    }

    virtual bool OnCmdLineParsed(wxCmdLineParser& parser)
    {
        if ( parser.Found("m", &opts.mapMode) &&
                (opts.mapMode < 1 || opts.mapMode > wxMM_METRIC) )
            return false;
        if ( parser.Found("p", &opts.penWidth) && opts.penWidth < 1 )
            return false;
        wxString penStyle;
        if ( parser.Found("pen-style", &penStyle) )
        {
            if ( !penStyle.empty() )
            {
                if ( penStyle == wxS("solid") )
                {
                    opts.penStyle = wxPENSTYLE_SOLID;
                }
                else if ( penStyle == wxS("dot") )
                {
                    opts.penStyle = wxPENSTYLE_DOT;
                }
                else if ( penStyle == wxS("long_dash") )
                {
                    opts.penStyle = wxPENSTYLE_LONG_DASH;
                }
                else if ( penStyle == wxS("short_dash") )
                {
                    opts.penStyle = wxPENSTYLE_SHORT_DASH;
                }
                else
                {
                    wxLogError(wxS("Unsupported pen style."));
                    return false;
                }
            }
        }
        wxString penQuality;
        if ( parser.Found("pen-quality", &penQuality) )
        {
            if ( penQuality == "low" )
                opts.penQuality = wxPEN_QUALITY_LOW;
            else if ( penQuality == "high" )
                opts.penQuality = wxPEN_QUALITY_HIGH;
            else if ( penQuality != "default" )
            {
                wxLogError("Unsupported pen quality.");
                return false;
            }
        }
        if ( parser.Found("w", &opts.width) && opts.width < 1 )
            return false;
        if ( parser.Found("h", &opts.height) && opts.height < 1 )
            return false;
        if ( parser.Found("N", &opts.numIters) && opts.numIters < 1 )
            return false;

        opts.testBitmaps = parser.Found("bitmaps");
        opts.testImages = parser.Found("images");
        opts.testLines = parser.Found("lines");
        opts.testRawBitmaps = parser.Found("rawbmp");
        opts.testRectangles = parser.Found("rectangles");
        opts.testCircles = parser.Found("circles");
        opts.testEllipses = parser.Found("ellipses");
        opts.testTextExtent = parser.Found("textextent");
        opts.testMultiLineTextExtent = parser.Found("multilinetextextent");
        opts.testPartialTextExtents = parser.Found("partialtextextents");
        if ( !(opts.testBitmaps || opts.testImages || opts.testLines
                    || opts.testRawBitmaps || opts.testRectangles
                    || opts.testCircles || opts.testEllipses
                    || opts.testTextExtent || opts.testPartialTextExtents) )
        {
            // Do everything by default.
            opts.testBitmaps =
            opts.testImages =
            opts.testLines =
            opts.testRawBitmaps =
            opts.testRectangles =
            opts.testCircles =
            opts.testEllipses =
            opts.testTextExtent =
            opts.testPartialTextExtents = true;
        }

        opts.usePaint = parser.Found("paint");
        opts.useClient = parser.Found("client");
        opts.useMemory = parser.Found("memory");
        if ( !(opts.usePaint || opts.useClient || opts.useMemory) )
        {
            opts.usePaint =
            opts.useClient =
            opts.useMemory = true;
        }

        opts.useDC = parser.Found("dc");
        opts.useGC = parser.Found("gc");
#if wxUSE_GLCANVAS
        opts.useGL = parser.Found("gl");
        if ( opts.useGL )
        {
            if ( opts.useDC || opts.useGC )
            {
                wxLogError("Can't use both OpenGL and normal graphics.");
                return false;
            }
        }
        else // Not using OpenGL
#endif // wxUSE_GLCANVAS
        {
            if ( !(opts.useDC || opts.useGC) )
            {
                opts.useDC =
                opts.useGC = true;
            }
        }

        opts.renderer = GraphicsBenchmarkOptions::Default;
#ifdef __WXMSW__
        wxString rendererName;
        if ( parser.Found("renderer", &rendererName) )
        {
            if ( !opts.useGC )
            {
                wxLogError("Renderer can be specified only when using graphics.");
                return false;
            }
            if ( !rendererName.empty() )
            {
                if ( rendererName == wxS("gdiplus") )
                {
                    opts.renderer = GraphicsBenchmarkOptions::GDIPlus;
                }
                else if ( rendererName == wxS("direct2d") )
                {
                    opts.renderer = GraphicsBenchmarkOptions::Direct2D;
                }
                else if ( rendererName == wxS("cairo") )
                {
                    opts.renderer = GraphicsBenchmarkOptions::Cairo;
                }
                else
                {
                    wxLogError( wxS("Unknown renderer name.") );
                    return false;
                }
            }
        }
#endif // __WXMSW__

        return true;
    }

    virtual bool OnInit()
    {
        if ( !wxApp::OnInit() )
            return false;

        new GraphicsBenchmarkFrame;

        return true;
    }
};

wxIMPLEMENT_APP_CONSOLE(GraphicsBenchmarkApp);
