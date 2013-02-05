/////////////////////////////////////////////////////////////////////////////
// Name:        graphics.cpp
// Purpose:     Some benchmarks for measuring graphics operations performance
// Author:      Vadim Zeitlin
// Created:     2008-04-13
// RCS-ID:      $Id$
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

        width = 800;
        height = 600;

        numIters = 1000;

        testBitmaps =
        testImages =
        testLines =
        testRawBitmaps =
        testRectangles = false;

        usePaint =
        useClient =
        useMemory = false;

        useDC =
        useGC =
        useGL = false;
    }

    long mapMode,
         penWidth,
         width,
         height,
         numIters;

    bool testBitmaps,
         testImages,
         testLines,
         testRawBitmaps,
         testRectangles;

    bool usePaint,
         useClient,
         useMemory;

    bool useDC,
         useGC,
         useGL;
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

        m_bitmap.Create(64, 64, 32);

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

        wxPrintf("%ld images done in %ldms = %gus/image or %d FPS\n",
                 opts.numIters, t, (1000. * t)/opts.numIters,
                 (1000*opts.numIters + t - 1)/t);

        wxTheApp->ExitMainLoop();
    }
#endif // wxUSE_GLCANVAS

    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        if ( opts.usePaint )
        {
            wxPaintDC dc(this);
            wxGCDC gcdc(dc);
            BenchmarkDCAndGC("paint", dc, gcdc);
        }

        if ( opts.useClient )
        {
            wxClientDC dc(this);
            wxGCDC gcdc(dc);
            BenchmarkDCAndGC("client", dc, gcdc);
        }

        if ( opts.useMemory )
        {
            wxBitmap bmp(opts.width, opts.height);
            wxMemoryDC dc(bmp);
            wxGCDC gcdc(dc);
            BenchmarkDCAndGC("memory", dc, gcdc);
        }

        wxTheApp->ExitMainLoop();
    }

    void BenchmarkDCAndGC(const char* dckind, wxDC& dc, wxGCDC& gcdc)
    {
        if ( opts.useDC )
            BenchmarkAll(wxString::Format("%6s DC", dckind), dc);
        if ( opts.useGC )
            BenchmarkAll(wxString::Format("%6s GC", dckind), gcdc);
    }

    void BenchmarkAll(const wxString& msg, wxDC& dc)
    {
        BenchmarkBitmaps(msg, dc);
        BenchmarkImages(msg, dc);
        BenchmarkLines(msg, dc);
        BenchmarkRawBitmaps(msg, dc);
        BenchmarkRectangles(msg, dc);
    }

    void BenchmarkLines(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testLines )
            return;

        if ( opts.mapMode != 0 )
            dc.SetMapMode((wxMappingMode)opts.mapMode);
        if ( opts.penWidth != 0 )
            dc.SetPen(wxPen(*wxWHITE, opts.penWidth));

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        wxStopWatch sw;
        int x = 0,
            y = 0;
        for ( int n = 0; n < opts.numIters; n++ )
        {
            int x1 = rand() % opts.width,
                y1 = rand() % opts.height;

            dc.DrawLine(x, y, x1, y1);

            x = x1;
            y = y1;
        }

        const long t = sw.Time();

        wxPrintf("%ld lines done in %ldms = %gus/line\n",
                 opts.numIters, t, (1000. * t)/opts.numIters);
    }


    void BenchmarkRectangles(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testRectangles )
            return;

        if ( opts.mapMode != 0 )
            dc.SetMapMode((wxMappingMode)opts.mapMode);
        if ( opts.penWidth != 0 )
            dc.SetPen(wxPen(*wxWHITE, opts.penWidth));

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

    void BenchmarkBitmaps(const wxString& msg, wxDC& dc)
    {
        if ( !opts.testBitmaps )
            return;

        if ( opts.mapMode != 0 )
            dc.SetMapMode((wxMappingMode)opts.mapMode);
        if ( opts.penWidth != 0 )
            dc.SetPen(wxPen(*wxWHITE, opts.penWidth));

        wxPrintf("Benchmarking %s: ", msg);
        fflush(stdout);

        wxStopWatch sw;
        for ( int n = 0; n < opts.numIters; n++ )
        {
            int x = rand() % opts.width,
                y = rand() % opts.height;

            dc.DrawBitmap(m_bitmap, x, y, true);
        }

        const long t = sw.Time();

        wxPrintf("%ld bitmaps done in %ldms = %gus/bitmap\n",
                 opts.numIters, t, (1000. * t)/opts.numIters);
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

        wxPrintf("%ld images done in %ldms = %gus/image or %d FPS\n",
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

        wxPrintf("%ld raw bitmaps done in %ldms = %gus/bitmap or %d FPS\n",
                 opts.numIters, t, (1000. * t)/opts.numIters,
                 (1000*opts.numIters + t - 1)/t);
    }


    wxBitmap m_bitmap;
#if wxUSE_GLCANVAS
    wxGLCanvas* m_glCanvas;
    wxGLContext* m_glContext;
#endif // wxUSE_GLCANVAS
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
            { wxCMD_LINE_OPTION, "w", "width", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "h", "height", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "I", "images", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "N", "number-of-iterations", "", wxCMD_LINE_VAL_NUMBER },
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
        if ( !(opts.testBitmaps || opts.testImages || opts.testLines
                    || opts.testRawBitmaps || opts.testRectangles) )
        {
            // Do everything by default.
            opts.testBitmaps =
            opts.testImages =
            opts.testLines =
            opts.testRawBitmaps =
            opts.testRectangles = true;
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

IMPLEMENT_APP_CONSOLE(GraphicsBenchmarkApp)
