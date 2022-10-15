///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/drawing.cpp
// Purpose:     Tests for wxGraphicsContent general drawing
// Author:      Armel Asselin
// Created:     2014-02-21
// Copyright:   (c) 2014 Ellié Computing <opensource@elliecomputing.com>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#include "drawing.h"

#if wxUSE_TEST_GC_DRAWING

#include "wx/image.h"
#include "wx/wfstream.h"
#include "wx/stdpaths.h"
#include "wx/scopeguard.h"

#include "testimagefile.h"

#include <stdexcept>

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

//// ORGANIZATION /////////////////////////////////////////////////////////////

// This test suite is organized around two axes:
// - drawing test cases
// - drawing contexts life cycle
// => each drawing test case represent a serie of drawing primitives to execute
//  for whichever context
// => each drawing context life cycle represent a particular class of
//  wxGraphicsContext and a way to create, dispose of and save it so that it is
//  possible to compare it with a reference file

// A plugin system is implemented to let developers of contributed libraries
// test their library without the need to impact the test system and its
// dependencies. See RunPluginsDrawingCase.

// The crossing of drawing case and life cycles is implemented by
// RunIndividualDrawingCase

// The CPPUNIT test case class present a test per drawing case per life cycle
// so that it is easy to run a particular test

// The test requires reference files and must produce them when an
// implementation changed and new good references are known to be produced.
// Environment variables control where reference files are located and when to
// produce them:
//  - WX_TEST_SUITE_BUILD_REFERENCE must be "1" to request production of
//      reference files (by default only testing is done)
//  - WX_TEST_SUITE_REFERENCE_DIR must be a path to a directory containing the
//      sub-directory "gcdrawing-references" (by default the parent directory
//      of the directory of the test program is used)

//// WRITING NEW TEST CASES

// - add a new function to realize the drawing in the "cases functions" section
// - add a case structure declaration for it in the "test cases" section
// - use drawingbasic.cpp as a sample to add your own test case implementation

//// WRITING NEW FACTORIES

// - if the wxGraphicsContext is a class built-in wxWidgets, add a
//      DrawingTestGCFactory derived sub-class in drawing.h header
//      together with a declaration for it and its implementation
//      can be placed in drawing.cpp
//      Once this is done duplicate all the CPP UNIT test functions
//      and entries "DrawToImage_YYY" to your new GC "DrawTo<newGc>_YYYY"
//
// - if it is not built-in (contributed library/wxCode...), make a plugin for it
//      test.bkl contains a sample "test_drawingplugin" target, you can use
//      drawingplgsample.cpp as a start, see RunPluginsDrawingCase declaration
//      for information about how to run the tests

wxString GraphicsContextDrawingTestCase::ms_referenceDirectory;
bool GraphicsContextDrawingTestCase::ms_buildReference;
bool GraphicsContextDrawingTestCase::ms_buildReferenceDetermined;
GraphicsContextDrawingTestCase::ImageGraphicsContextLifeCycle
    GraphicsContextDrawingTestCase::ms_imageLifeCycle;

#if wxUSE_SVG
    GraphicsContextDrawingTestCase::SvgGraphicsContextLifeCycle
        GraphicsContextDrawingTestCase::ms_svgLifeCycle;
#endif // wxUSE_SVG

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( GraphicsContextDrawingTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( GraphicsContextDrawingTestCase,
    "GraphicsContextDrawingTestCase" );

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void GraphicsContextDrawingTestCase::RunIndividualDrawingCase (
    DrawingTestGCFactory& gcFactory,
    const DrawingTestCase & testCase)
{
    wxFileName fileName, refFileName;
    wxString testsOutputDirectory = wxStandardPaths::Get().GetTempDir();
    wxString refOutputDirectory = GetTestsReferenceDirectory();

    wxString platformTag;
    if (!testCase.platformIndependent && !gcFactory.PlatformIndependent())
        platformTag = wxString::Format("_%s", GetPlatformTag());

    fileName.Assign (testsOutputDirectory,
        wxString::Format("image_test_%s%s_%d", gcFactory.GetIdForFileName(),
                         platformTag, testCase.caseNumber),
        gcFactory.GetExtensionForFileName());

    refFileName.Assign (refOutputDirectory,
        wxString::Format("image_test_%s%s_%d_ref", gcFactory.GetIdForFileName(),
                        platformTag, testCase.caseNumber),
        gcFactory.GetExtensionForFileName());

    {
        wxGraphicsContext *gc = nullptr;

        wxON_BLOCK_EXIT_OBJ1(gcFactory, DrawingTestGCFactory::CleanUp, gc);

        gc = gcFactory.BuildNewContext(
            wxSize(testCase.width, testCase.height),
            testCase.pointsPerInch, fileName);

        (this->*testCase.m_drawingF)(gc);

        gcFactory.SaveBuiltContext(gc);
    }

    if (GetBuildReference())
    {
         WX_ASSERT_MESSAGE(
             ("Cannot copy file \"%s\" to \"%s\".",
            fileName.GetFullPath(), refFileName.GetFullPath()),
            wxCopyFile (fileName.GetFullPath(),
                        refFileName.GetFullPath(), true));
    }
    else if (gcFactory.UseImageComparison())
    {
        WX_ASSERT_SAME_AS_IMAGE_FILE(fileName.GetFullPath(),
                                     refFileName.GetFullPath());
    }
    else
    {
        WX_ASSERT_SAME_AS_FILE(fileName.GetFullPath(),
                               refFileName.GetFullPath());
    }
}


bool GraphicsContextDrawingTestCase::GetBuildReference() const
{
    if (!ms_buildReferenceDetermined)
    {
        wxString value;
        if (wxGetEnv("WX_TEST_SUITE_BUILD_REFERENCE", &value))
            ms_buildReference = value == "1";
    }
    return ms_buildReference;
}

wxString GraphicsContextDrawingTestCase::GetTestsReferenceDirectory() const
{
    if (ms_referenceDirectory.empty())
    {
        wxFileName refDir;

        if ( !wxGetEnv("WX_TEST_SUITE_REFERENCE_DIR",
                        &ms_referenceDirectory) )
        {
            refDir = wxFileName(wxStandardPaths::Get().GetExecutablePath());
            refDir.RemoveLastDir();
        }
        else
        {
            refDir = wxFileName(ms_referenceDirectory, wxT(""));
        }
        refDir.AppendDir ("drawing");
        refDir.AppendDir ("references");
        ms_referenceDirectory = refDir.GetPath();
    }
    return ms_referenceDirectory;
}

wxString GraphicsContextDrawingTestCase::GetPlatformTag() const
{
    // We consider that the platform tag is the kind of default renderer plus
    // its major/minor versions.
    // The reason why including major/minor version is important, is that the
    // rendering engine typically evolves somewhat between two version
    // (i.e. font rendering is not the same in Windows XP and Windows 8)
    int major, minor;
    const wxGraphicsRenderer *defaultRenderer = wxGraphicsRenderer::GetDefaultRenderer();
    wxString rendererName = defaultRenderer->GetName();
    defaultRenderer->GetVersion (&major, &minor);

    return wxString::Format("%s-%d.%d", rendererName, major, minor);
}

///////////////////////////////////////////////////////////////////////////////
// BUILT-IN FACTORIES  ////////////////////////////////////////////////////////

wxGraphicsContext *
GraphicsContextDrawingTestCase::ImageGraphicsContextLifeCycle::
    BuildNewContext (wxSize expectedSize, double WXUNUSED(pointsPerInch),
    const wxFileName &targetFileName)
{
    m_image = new wxImage (expectedSize);
    m_image->InitAlpha();

    m_targetFileName = targetFileName.GetFullPath();

    // we should probably pass the number of points per inches somewhere...
    //  but I don't see where yet...
    return wxGraphicsContext::Create(*m_image);
}

void
GraphicsContextDrawingTestCase::ImageGraphicsContextLifeCycle::
    SaveBuiltContext (wxGraphicsContext *&gc)
{
    wxDELETE(gc);

    m_image->SaveFile (m_targetFileName);
}

void
GraphicsContextDrawingTestCase::ImageGraphicsContextLifeCycle::
    CleanUp (wxGraphicsContext *gc)
{
    delete gc;
    m_targetFileName.clear();
    wxDELETE(m_image);
}

#if wxUSE_SVG
wxGraphicsContext *
GraphicsContextDrawingTestCase::SvgGraphicsContextLifeCycle::
    BuildNewContext (wxSize WXUNUSED(expectedSize),
    double WXUNUSED(pointsPerInch),
    const wxFileName &WXUNUSED(targetFileName))
{
    m_svgFileDc = nullptr;
    //m_svg_file_dc = new wxSVGFileDC (target_file_name.GetFullPath(),
    //    expected_size.GetWidth(), expected_size.GetHeight(), points_per_inch);

    // unfortunately cannot make GC over a DC yet :(
    throw std::runtime_error("SVG as no wxGC interface yet");
}

void
GraphicsContextDrawingTestCase::SvgGraphicsContextLifeCycle::
    SaveBuiltContext (wxGraphicsContext *&WXUNUSED(gc))
{
}

void
GraphicsContextDrawingTestCase::SvgGraphicsContextLifeCycle::
    CleanUp (wxGraphicsContext *WXUNUSED(gc))
{
    wxDELETE (m_svgFileDc);
}
#endif // wxUSE_SVG

#endif // wxUSE_TEST_GC_DRAWING
