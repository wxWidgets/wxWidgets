///////////////////////////////////////////////////////////////////////////////
// Name:        tests/drawing/drawing.h
// Purpose:     Drawing test case header
// Author:      Armel Asselin
// Created:     2014-02-26
// Copyright:   (c) 2014 Ellié Computing <opensource@elliecomputing.com>
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_DRAWING_H_
#define _WX_TESTS_DRAWING_H_

#include "gcfactory.h"

#if wxUSE_TEST_GC_DRAWING

#include "wx/math.h"

#if wxUSE_SVG
#include "wx/dcsvg.h"
#endif

class WXDLLIMPEXP_FWD_BASE wxDynamicLibrary;

class GraphicsContextDrawingTestCase : public CppUnit::TestCase
{
public:
    GraphicsContextDrawingTestCase()
        : m_drawingPluginsLoaded(false)
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
        CPPUNIT_TEST( DrawToPlugins_Basics );

        // FIXME: Reference data files are currently not found when using Unix
        // build system, so these tests are failing there, fix this and remove
        // this ifdef.
#ifdef __WINDOWS__
        CPPUNIT_TEST( DrawToImage_Fonts );
#if wxUSE_SVG
//        CPPUNIT_TEST( DrawToSVG_Fonts );
#endif
        CPPUNIT_TEST( DrawToPlugins_Fonts );
#endif // __WINDOWS__
   CPPUNIT_TEST_SUITE_END();

    class ImageGraphicsContextLifeCycle: public DrawingTestGCFactory
    {
    public:
        virtual ~ImageGraphicsContextLifeCycle() {}
        virtual wxString GetIdForFileName () const wxOVERRIDE { return "image"; }
        virtual wxString GetExtensionForFileName () const wxOVERRIDE { return "png"; }
        virtual bool UseImageComparison() const wxOVERRIDE { return true; }
        virtual bool PlatformIndependent() const wxOVERRIDE { return false; }
        virtual wxGraphicsContext *BuildNewContext (wxSize expectedSize,
            double pointsPerInch, const wxFileName &targetFileName) wxOVERRIDE;
        virtual void SaveBuiltContext (wxGraphicsContext *&gc) wxOVERRIDE;
        virtual void CleanUp (wxGraphicsContext *gc) wxOVERRIDE;
    private:
        wxImage *m_image;
        wxString m_targetFileName;
    };

    static ImageGraphicsContextLifeCycle ms_imageLifeCycle;

#if wxUSE_SVG
    class SvgGraphicsContextLifeCycle: public DrawingTestGCFactory
    {
    public:
        virtual wxString GetIdForFileName () const wxOVERRIDE { return "svg"; }
        virtual wxString GetExtensionForFileName () const wxOVERRIDE { return "svg"; }
        virtual bool UseImageComparison() const wxOVERRIDE { return false; }
        virtual bool PlatformIndependent() const wxOVERRIDE { return true; }
        virtual wxGraphicsContext *BuildNewContext (wxSize expectedSize,
            double pointsPerInch, const wxFileName &targetFileName) wxOVERRIDE;
        virtual void SaveBuiltContext (wxGraphicsContext *&gc) wxOVERRIDE;
        virtual void CleanUp (wxGraphicsContext *gc) wxOVERRIDE;

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

    // enumerates the dll names as specified in WX_TEST_SUITE_GC_DRAWING_PLUGINS
    //  (coma separated list of DLL to load and test)
    // each DLL should have these procedures:
    //  - DrawingTestGCFactory* CreateDrawingTestLifeCycle(),
    //          to create the life cycle object
    //  - DestroyDrawingTestLifeCycle(DrawingTestGCFactory *),
    //          to destroy the life cycle object
    void RunPluginsDrawingCase (const DrawingTestCase & testCase);

    void DrawToImage_Basics() {
        RunIndividualDrawingCase (ms_imageLifeCycle, ms_drawingBasicTc);
    }
#if wxUSE_SVG
    void DrawToSVG_Basics() {
        RunIndividualDrawingCase (ms_svgLifeCycle, ms_drawingBasicTc);
    }
#endif
    void DrawToPlugins_Basics() {
        RunPluginsDrawingCase (ms_drawingBasicTc);
    }
    void DrawToImage_Fonts() {
        RunIndividualDrawingCase (ms_imageLifeCycle, ms_drawingFontTc);
    }
#if wxUSE_SVG
    void DrawToSVG_Fonts() {
        RunIndividualDrawingCase (ms_svgLifeCycle, ms_drawingFontTc);
    }
#endif
    void DrawToPlugins_Fonts() {
        RunPluginsDrawingCase (ms_drawingFontTc);
    }

    bool GetBuildReference() const;

    wxString GetTestsReferenceDirectory() const;

    wxString GetPlatformTag() const;

    static wxString ms_referenceDirectory;
    static bool ms_buildReference, ms_buildReferenceDetermined;
    bool m_drawingPluginsLoaded;

    struct PluginInfo {
        PluginInfo();
        ~PluginInfo();

        wxDynamicLibrary* library;

        DrawingTestGCFactory *gcFactory;
        DestroyDrawingTestLifeCycleFunction destroy;
    };

    wxVector<PluginInfo> m_drawingPlugins;

    wxDECLARE_NO_COPY_CLASS(GraphicsContextDrawingTestCase);
};


#endif // wxUSE_TEST_GC_DRAWING

#endif // _WX_TESTS_DRAWING_H_
