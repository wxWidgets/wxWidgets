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
#include "wx/stopwatch.h"
#include "wx/crt.h"

struct GraphicsBenchmarkOptions
{
    GraphicsBenchmarkOptions()
    {
        mapMode = 0;
        penWidth = 0;

        width = 800;
        height = 600;

        numLines = 10000;

        testBitmaps =
        testLines =
        testRectangles = false;
    }

    long mapMode,
         penWidth,
         width,
         height,
         numLines;

    bool testBitmaps,
         testLines,
         testRectangles;
} opts;

class GraphicsBenchmarkFrame : public wxFrame
{
public:
    GraphicsBenchmarkFrame()
        : wxFrame(NULL, wxID_ANY, "wxWidgets Graphics Benchmark")
    {
        Connect(wxEVT_PAINT,
                wxPaintEventHandler(GraphicsBenchmarkFrame::OnPaint));

        m_bitmap.Create(64, 64, 32);

        Show();
        SetClientSize(opts.width, opts.height);
    }

private:
    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        {
            wxPaintDC dc(this);
            wxGCDC gcdc(dc);
            BenchmarkDCAndGC("paint", dc, gcdc);
        }

        {
            wxClientDC dc(this);
            wxGCDC gcdc(dc);
            BenchmarkDCAndGC("client", dc, gcdc);
        }

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
        BenchmarkAll(wxString::Format("%6s DC", dckind), dc);
        BenchmarkAll(wxString::Format("%6s GC", dckind), gcdc);
    }

    void BenchmarkAll(const wxString& msg, wxDC& dc)
    {
        BenchmarkLines(msg, dc);
        BenchmarkRectangles(msg, dc);
        BenchmarkBitmaps(msg, dc);
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

        wxStopWatch sw;
        int x = 0,
            y = 0;
        for ( int n = 0; n < opts.numLines; n++ )
        {
            int x1 = rand() % opts.width,
                y1 = rand() % opts.height;

            dc.DrawLine(x, y, x1, y1);

            x = x1;
            y = y1;
        }

        const long t = sw.Time();

        wxPrintf("%ld lines done in %ldms = %gus/line\n",
                 opts.numLines, t, (1000. * t)/opts.numLines);
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

        wxStopWatch sw;
        for ( int n = 0; n < opts.numLines; n++ )
        {
            int x = rand() % opts.width,
                y = rand() % opts.height;

            dc.DrawRectangle(x, y, 32, 32);
        }

        const long t = sw.Time();

        wxPrintf("%ld rects done in %ldms = %gus/rect\n",
                 opts.numLines, t, (1000. * t)/opts.numLines);
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

        wxStopWatch sw;
        for ( int n = 0; n < opts.numLines; n++ )
        {
            int x = rand() % opts.width,
                y = rand() % opts.height;

            dc.DrawBitmap(m_bitmap, x, y, true);
        }

        const long t = sw.Time();

        wxPrintf("%ld bitmaps done in %ldms = %gus/bitmap\n",
                 opts.numLines, t, (1000. * t)/opts.numLines);
    }


    wxBitmap m_bitmap;
};

class GraphicsBenchmarkApp : public wxApp
{
public:
    virtual void OnInitCmdLine(wxCmdLineParser& parser)
    {
        static const wxCmdLineEntryDesc desc[] =
        {
            { wxCMD_LINE_SWITCH, "",  "bitmaps" },
            { wxCMD_LINE_SWITCH, "",  "lines" },
            { wxCMD_LINE_SWITCH, "",  "rectangles" },
            { wxCMD_LINE_OPTION, "m", "map-mode", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "p", "pen-width", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "w", "width", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "h", "height", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "L", "lines", "", wxCMD_LINE_VAL_NUMBER },
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
        if ( parser.Found("L", &opts.numLines) && opts.numLines < 1 )
            return false;

        opts.testBitmaps = parser.Found("bitmaps");
        opts.testLines = parser.Found("lines");
        opts.testRectangles = parser.Found("rectangles");
        if ( !(opts.testBitmaps || opts.testLines || opts.testRectangles) )
        {
            // Do everything by default.
            opts.testBitmaps =
            opts.testLines =
            opts.testRectangles = true;
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
