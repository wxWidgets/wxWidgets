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

#ifdef __WXMSW__
    #include "wx/display.h"
#endif

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
//      sub-directory "drawing/references" (by default the parent directory of
//      the directory of the test program is used)

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

namespace
{

struct ImageRegionStats
{
    size_t pixels = 0;
    size_t background = 0;
    size_t white = 0;
    size_t grey = 0;
    size_t dark = 0;
    size_t blue = 0;
};

bool IsNear(unsigned char value, unsigned char expected, unsigned char tolerance)
{
    return value >= expected - tolerance && value <= expected + tolerance;
}

ImageRegionStats GetImageRegionStats(const wxImage& image,
                                     const wxRect& region)
{
    ImageRegionStats stats;

    if (region.x < 0 || region.y < 0 ||
        region.GetRight() >= image.GetWidth() ||
        region.GetBottom() >= image.GetHeight())
    {
        return stats;
    }

    const unsigned char* const data = image.GetData();
    if (!data)
        return stats;

    for (int y = region.y; y <= region.GetBottom(); ++y)
    {
        for (int x = region.x; x <= region.GetRight(); ++x)
        {
            const size_t offset =
                3 * (static_cast<size_t>(y) * image.GetWidth() + x);
            const unsigned char red = data[offset];
            const unsigned char green = data[offset + 1];
            const unsigned char blue = data[offset + 2];

            ++stats.pixels;

            if (IsNear(red, 240, 4) &&
                IsNear(green, 240, 4) &&
                IsNear(blue, 240, 4))
            {
                ++stats.background;
            }
            if (red >= 251 && green >= 251 && blue >= 251)
                ++stats.white;
            if (IsNear(red, 192, 4) &&
                IsNear(green, 192, 4) &&
                IsNear(blue, 192, 4))
            {
                ++stats.grey;
            }
            if (red < 80 && green < 80 && blue < 80)
                ++stats.dark;
            if (blue >= 160 &&
                static_cast<int>(blue) - red >= 64 &&
                static_cast<int>(blue) - green >= 64)
            {
                ++stats.blue;
            }
        }
    }

    return stats;
}

bool HasAtLeast(size_t count,
                const ImageRegionStats& stats,
                size_t numerator,
                size_t denominator)
{
    return stats.pixels != 0 &&
           count * denominator >= stats.pixels * numerator;
}

} // anonymous namespace

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
        if (refFileName.FileExists())
        {
            WX_ASSERT_SAME_AS_IMAGE_FILE(fileName.GetFullPath(),
                                         refFileName.GetFullPath());
        }
        else
        {
            // Not every renderer/OS combination has an exact golden image.
            // Do not pretend that a different platform reference is
            // compatible: validate the deliberately drawn content instead.
            WX_ASSERT_MESSAGE(
                ("No compatible reference image \"%s\" exists and the "
                 "generated image \"%s\" failed semantic validation.",
                 refFileName.GetFullPath(), fileName.GetFullPath()),
                HasExpectedImageSemantics(fileName.GetFullPath(), testCase));
        }
    }
    else
    {
        WX_ASSERT_SAME_AS_FILE(fileName.GetFullPath(),
                               refFileName.GetFullPath());
    }
}

bool GraphicsContextDrawingTestCase::HasExpectedImageSemantics(
    const wxString& fileName,
    const DrawingTestCase& testCase) const
{
    wxImage image(fileName);
    if (!image.IsOk() ||
        image.GetSize() != wxSize(testCase.width, testCase.height) ||
        !IsImageFullyOpaque(image))
    {
        return false;
    }

    const unsigned char* const data = image.GetData();
    if (!data)
        return false;

    const size_t pixelsCount =
        static_cast<size_t>(testCase.width) * testCase.height;

    if (testCase.m_drawingF ==
            &GraphicsContextDrawingTestCase::DoBasicDrawings)
    {
        for (size_t n = 0; n < pixelsCount; ++n)
        {
            const unsigned char* const pixel = data + 3*n;
            if (pixel[0] != 255 || pixel[1] != 255 || pixel[2] != 255)
                return false;
        }

        return true;
    }

    if (testCase.m_drawingF ==
            &GraphicsContextDrawingTestCase::DoFontDrawings)
    {
        const ImageRegionStats whole =
            GetImageRegionStats(image, wxRect(0, 0, 800, 600));
        const ImageRegionStats emptyBottomRight =
            GetImageRegionStats(image, wxRect(720, 520, 80, 80));
        const ImageRegionStats horizontalGreyText =
            GetImageRegionStats(image, wxRect(100, 5, 140, 30));
        const ImageRegionStats rotatedGreyText =
            GetImageRegionStats(image, wxRect(0, 0, 110, 110));
        const ImageRegionStats swissHeading =
            GetImageRegionStats(image, wxRect(95, 35, 300, 35));
        const ImageRegionStats dimensionsText =
            GetImageRegionStats(image, wxRect(105, 75, 690, 35));
        const ImageRegionStats logicalFunction =
            GetImageRegionStats(image, wxRect(105, 140, 300, 100));
        const ImageRegionStats multilineText =
            GetImageRegionStats(image, wxRect(105, 225, 180, 145));
        const ImageRegionStats blueMultilineText =
            GetImageRegionStats(image, wxRect(105, 335, 190, 140));

        const ImageRegionStats rotatedNorth =
            GetImageRegionStats(image, wxRect(350, 250, 100, 100));
        const ImageRegionStats rotatedEast =
            GetImageRegionStats(image, wxRect(450, 350, 100, 100));
        const ImageRegionStats rotatedSouth =
            GetImageRegionStats(image, wxRect(350, 450, 100, 100));
        const ImageRegionStats rotatedWest =
            GetImageRegionStats(image, wxRect(250, 350, 100, 100));

        // Check both content and placement. These regions correspond to the
        // independent drawing operations in DoFontDrawings(): two grey text
        // backgrounds, the heading and metrics line, the white logical-op
        // rectangles, black and blue multiline text, and text distributed
        // around all four sides of the rotation centre. This deliberately
        // tolerates font rasterizer differences while still detecting blank,
        // recoloured, misplaced or substantially incomplete output.
        return
            HasAtLeast(whole.background, whole, 4, 5) &&
            HasAtLeast(emptyBottomRight.background,
                       emptyBottomRight, 99, 100) &&
            HasAtLeast(horizontalGreyText.grey,
                       horizontalGreyText, 1, 5) &&
            HasAtLeast(horizontalGreyText.dark,
                       horizontalGreyText, 1, 50) &&
            HasAtLeast(rotatedGreyText.grey, rotatedGreyText, 1, 20) &&
            HasAtLeast(rotatedGreyText.dark, rotatedGreyText, 1, 200) &&
            HasAtLeast(swissHeading.dark, swissHeading, 1, 50) &&
            HasAtLeast(dimensionsText.dark, dimensionsText, 3, 100) &&
            HasAtLeast(logicalFunction.white, logicalFunction, 1, 10) &&
            HasAtLeast(logicalFunction.dark, logicalFunction, 1, 50) &&
            HasAtLeast(multilineText.dark, multilineText, 1, 100) &&
            HasAtLeast(blueMultilineText.blue,
                       blueMultilineText, 1, 20) &&
            HasAtLeast(rotatedNorth.dark, rotatedNorth, 1, 200) &&
            HasAtLeast(rotatedEast.dark, rotatedEast, 1, 200) &&
            HasAtLeast(rotatedSouth.dark, rotatedSouth, 1, 200) &&
            HasAtLeast(rotatedWest.dark, rotatedWest, 1, 200);
    }

    // New drawing cases must either provide an exact reference or explicitly
    // define their semantic fallback above.
    return false;
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

    wxGraphicsContext* const gc = wxGraphicsContext::Create(*m_image);

#ifdef __WXMSW__
    // GDI+ image contexts inherit the desktop DPI even though their wxImage
    // backing store uses one logical unit per pixel. Keep this test-only
    // normalization limited to the built-in MSW GDI+ image context: changing
    // the scale of other renderers would change their established semantics.
    if (gc && gc->GetRenderer() &&
        gc->GetRenderer()->GetName().CmpNoCase("gdiplus") == 0)
    {
        wxDouble dpiY = 0;
        gc->GetDPI(nullptr, &dpiY);
        if (dpiY > 0)
        {
            const double standardDpi = wxDisplay::GetStdPPI().y;
            gc->SetContentScaleFactor(
                gc->GetContentScaleFactor() * standardDpi / dpiY);
        }
    }
#endif // __WXMSW__

    return gc;
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
