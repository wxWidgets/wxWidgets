///////////////////////////////////////////////////////////////////////////////
// Name:        tests/drawing/drawing.h
// Purpose:     Drawing test case header
// Author:      Armel Asselin
// Created:     2014-02-26
// Copyright:   (c) 2014 Ellié Computing <opensource@elliecomputing.com>
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_DRAWING_H_
#define _WX_TESTS_DRAWING_H_

// wxCairoRenderer::CreateMeasuringContext() is not implemented for wxX11
#if wxUSE_GRAPHICS_CONTEXT && !defined(__WXX11__)
    #define wxUSE_TEST_GC_DRAWING 1
#else
    #define wxUSE_TEST_GC_DRAWING 0
#endif

#if wxUSE_TEST_GC_DRAWING

#include "wx/filename.h"
#include "wx/graphics.h"
#include "wx/math.h"

#if wxUSE_SVG
#include "wx/dcsvg.h"
#endif

// Implement a class derived from this one to let the drawing test create
// graphics context, save them and destroy them.
class DrawingTestGCFactory
{
public:
    virtual ~DrawingTestGCFactory() {}

    virtual wxString GetIdForFileName () const = 0;
    virtual wxString GetExtensionForFileName () const = 0;
    virtual bool UseImageComparison() const = 0;
    virtual bool PlatformIndependent() const = 0;
    virtual wxGraphicsContext *BuildNewContext (wxSize expectedSize,
        double pointsPerInch, const wxFileName &targetFileName) = 0;
    virtual void SaveBuiltContext (wxGraphicsContext *&gc) = 0;
    virtual void CleanUp (wxGraphicsContext *gc) = 0;
};

class GraphicsContextDrawingTestCase : public CppUnit::TestCase
{
public:
    GraphicsContextDrawingTestCase()
    {
        wxImage::AddHandler (new wxPNGHandler());
        wxImage::AddHandler (new wxBMPHandler());
    }
    ~GraphicsContextDrawingTestCase() {
        ms_referenceDirectory.clear();
    }

private:

    // NB: individual test cases launchers are copied/pasted so that the CppUnit
    //  test case selection can be used

    CPPUNIT_TEST_SUITE( GraphicsContextDrawingTestCase );
        CPPUNIT_TEST( DrawToImage_Basics );
#if wxUSE_SVG
//        CPPUNIT_TEST( DrawToSVG_Basics );
#endif
        // FIXME: Reference data files are currently not found when using Unix
        // build system, so these tests are failing there, fix this and remove
        // this ifdef.
#ifdef __WINDOWS__
        CPPUNIT_TEST( DrawToImage_Fonts );
#if wxUSE_SVG
//        CPPUNIT_TEST( DrawToSVG_Fonts );
#endif
#endif // __WINDOWS__
   CPPUNIT_TEST_SUITE_END();

    class ImageGraphicsContextLifeCycle: public DrawingTestGCFactory
    {
    public:
        virtual ~ImageGraphicsContextLifeCycle() {}
        virtual wxString GetIdForFileName () const override { return "image"; }
        virtual wxString GetExtensionForFileName () const override { return "png"; }
        virtual bool UseImageComparison() const override { return true; }
        virtual bool PlatformIndependent() const override { return false; }
        virtual wxGraphicsContext *BuildNewContext (wxSize expectedSize,
            double pointsPerInch, const wxFileName &targetFileName) override;
        virtual void SaveBuiltContext (wxGraphicsContext *&gc) override;
        virtual void CleanUp (wxGraphicsContext *gc) override;
    private:
        wxImage *m_image;
        wxString m_targetFileName;
    };

    static ImageGraphicsContextLifeCycle ms_imageLifeCycle;

#if wxUSE_SVG
    class SvgGraphicsContextLifeCycle: public DrawingTestGCFactory
    {
    public:
        virtual wxString GetIdForFileName () const override { return "svg"; }
        virtual wxString GetExtensionForFileName () const override { return "svg"; }
        virtual bool UseImageComparison() const override { return false; }
        virtual bool PlatformIndependent() const override { return true; }
        virtual wxGraphicsContext *BuildNewContext (wxSize expectedSize,
            double pointsPerInch, const wxFileName &targetFileName) override;
        virtual void SaveBuiltContext (wxGraphicsContext *&gc) override;
        virtual void CleanUp (wxGraphicsContext *gc) override;

    private:
        wxSVGFileDC *m_svgFileDc;
    };

    static SvgGraphicsContextLifeCycle ms_svgLifeCycle;
#endif // wxUSE_SVG

    struct DrawingTestCase {
        unsigned caseNumber;
        void (GraphicsContextDrawingTestCase::*m_drawingF) (
            wxGraphicsContext *gc);
        unsigned width, height;
        double pointsPerInch;

        // if true, it means that this test case is "simple" enough to be
        //  platform independent even for targets that normally are dependent
        //  on the platform
        bool platformIndependent;
    };

    // test cases
    static const DrawingTestCase ms_drawingBasicTc;
    static const DrawingTestCase ms_drawingFontTc;

    // cases functions
    void DoBasicDrawings (wxGraphicsContext *gc);
    void DoFontDrawings (wxGraphicsContext *gc);

    void RunIndividualDrawingCase (
        DrawingTestGCFactory& gcFactory,
        const DrawingTestCase & testCase);

    void DrawToImage_Basics() {
        RunIndividualDrawingCase (ms_imageLifeCycle, ms_drawingBasicTc);
    }
#if wxUSE_SVG
    void DrawToSVG_Basics() {
        RunIndividualDrawingCase (ms_svgLifeCycle, ms_drawingBasicTc);
    }
#endif
    void DrawToImage_Fonts() {
        RunIndividualDrawingCase (ms_imageLifeCycle, ms_drawingFontTc);
    }
#if wxUSE_SVG
    void DrawToSVG_Fonts() {
        RunIndividualDrawingCase (ms_svgLifeCycle, ms_drawingFontTc);
    }
#endif

    bool GetBuildReference() const;

    wxString GetTestsReferenceDirectory() const;

    wxString GetPlatformTag() const;

    static wxString ms_referenceDirectory;
    static bool ms_buildReference, ms_buildReferenceDetermined;

    wxDECLARE_NO_COPY_CLASS(GraphicsContextDrawingTestCase);
};


#endif // wxUSE_TEST_GC_DRAWING

#endif // _WX_TESTS_DRAWING_H_
