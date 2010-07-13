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
#include "wx/stopwatch.h"

struct GraphicsBenchmarkOptions
{
    GraphicsBenchmarkOptions()
    {
        mapMode = 0;
        penWidth = 0;

        width = 800;
        height = 600;

        numLines = 10000;
    }

    long mapMode,
         penWidth,
         width,
         height,
         numLines;
} opts;

class GraphicsBenchmarkFrame : public wxFrame
{
public:
    GraphicsBenchmarkFrame()
        : wxFrame(NULL, wxID_ANY, "wxWidgets Graphics Benchmark")
    {
        Connect(wxEVT_PAINT,
                wxPaintEventHandler(GraphicsBenchmarkFrame::OnPaint));

        Show();
        SetClientSize(opts.width, opts.height);
        wxClientDC dc(this);
        BenchmarkLines("client", dc);

        wxBitmap bmp(opts.width, opts.height);
        wxMemoryDC dc2(bmp);
        BenchmarkLines("memory", dc2);
    }

protected:
    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc(this);

        BenchmarkLines(" paint", dc);

        wxTheApp->ExitMainLoop();
    }

    void BenchmarkLines(const char *msg, wxDC& dc)
    {
        if ( opts.mapMode != 0 )
            dc.SetMapMode(opts.mapMode);
        if ( opts.penWidth != 0 )
            dc.SetPen(wxPen(*wxWHITE, opts.penWidth));

        wxPrintf("Benchmarking %s DC: ", msg);

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

        wxPrintf("%d lines done in %lums = %gus/line\n",
                 opts.numLines, t, (1000. * t)/opts.numLines);
    }
};

class GraphicsBenchmarkApp : public wxApp
{
public:
    virtual void OnInitCmdLine(wxCmdLineParser& parser)
    {
        static const wxCmdLineEntryDesc desc[] =
        {
            { wxCMD_LINE_OPTION, "m", "map-mode", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "p", "pen-width", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "w", "width", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "h", "height", "", wxCMD_LINE_VAL_NUMBER },
            { wxCMD_LINE_OPTION, "L", "lines", "", wxCMD_LINE_VAL_NUMBER },
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
