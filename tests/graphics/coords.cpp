///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/coords.cpp
// Purpose:     Coordinates conversion unit tests
// Author:      Artur Wieczorek
// Created:     2020-09-25
// Copyright:   (c) 2020 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/bitmap.h"
#include "wx/dcgraph.h"
#include "wx/dcmemory.h"


// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

static const wxSize s_dcSize(100, 100);
static const wxPoint s_posDev(24, 57);
static const wxSize s_dimDev(40, 15);

static void InitialState(wxDC& dc)
{
    // Check initial state

    wxPoint origin = dc.GetDeviceOrigin();
    CHECK(origin == wxPoint(0,0));

    origin = dc.GetLogicalOrigin();
    CHECK(origin == wxPoint(0, 0));

    double sx, sy;
    dc.GetUserScale(&sx, &sy);
    CHECK(sx == 1.0);
    CHECK(sy == 1.0);

    dc.GetLogicalScale(&sx, &sy);
    CHECK(sx == 1.0);
    CHECK(sy == 1.0);

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc.CanUseTransformMatrix() )
    {
        wxAffineMatrix2D m = dc.GetTransformMatrix();
        CHECK(m.IsIdentity() == true);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void NoTransform(wxDC& dc)
{
    // No transformations

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog.x = dc.DeviceToLogicalX(s_posDev.x);
    posLog.y = dc.DeviceToLogicalY(s_posDev.y);
    CHECK(posLog == s_posDev);

    wxSize dimLog;
    dimLog.x = dc.DeviceToLogicalXRel(s_dimDev.x);
    dimLog.y = dc.DeviceToLogicalYRel(s_dimDev.y);
    CHECK(dimLog == s_dimDev);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev.x = dc.LogicalToDeviceX(posLog.x);
    posDev.y = dc.LogicalToDeviceY(posLog.y);
    CHECK(posDev == s_posDev);

    wxSize dimDev;
    dimDev.x = dc.LogicalToDeviceXRel(dimLog.x);
    dimDev.y = dc.LogicalToDeviceYRel(dimLog.y);
    CHECK(dimDev == s_dimDev);
}

static void NoTransformEx(wxDC& dc)
{
    // No transformations

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog = dc.DeviceToLogical(s_posDev);
    CHECK(posLog == s_posDev);

    wxSize dimLog;
    dimLog = dc.DeviceToLogicalRel(s_dimDev);
    CHECK(dimLog == s_dimDev);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev = dc.LogicalToDevice(posLog);
    CHECK(posDev == s_posDev);

    wxSize dimDev;
    dimDev = dc.LogicalToDeviceRel(dimLog);
    CHECK(dimDev == s_dimDev);
}

static void DeviceOriginChanged(wxDC& dc)
{
    // Only device origin is changed
    const wxCoord dx = 10;
    const wxCoord dy = 15;
    dc.SetDeviceOrigin(dx, dy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog.x = dc.DeviceToLogicalX(s_posDev.x);
    posLog.y = dc.DeviceToLogicalY(s_posDev.y);
    CHECK(posLog == wxPoint(s_posDev.x - dx, s_posDev.y - dy));

    wxSize dimLog;
    dimLog.x = dc.DeviceToLogicalXRel(s_dimDev.x);
    dimLog.y = dc.DeviceToLogicalYRel(s_dimDev.y);
    CHECK(dimLog == s_dimDev);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev.x = dc.LogicalToDeviceX(posLog.x);
    posDev.y = dc.LogicalToDeviceY(posLog.y);
    CHECK(posDev == s_posDev);

    wxSize dimDev;
    dimDev.x = dc.LogicalToDeviceXRel(dimLog.x);
    dimDev.y = dc.LogicalToDeviceYRel(dimLog.y);
    CHECK(dimDev == s_dimDev);
}

static void DeviceOriginChangedEx(wxDC& dc)
{
    // Only device origin is changed
    const wxCoord dx = 10;
    const wxCoord dy = 15;
    dc.SetDeviceOrigin(dx, dy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog = dc.DeviceToLogical(s_posDev);
    CHECK(posLog == wxPoint(s_posDev.x - dx, s_posDev.y - dy));

    wxSize dimLog;
    dimLog = dc.DeviceToLogicalRel(s_dimDev);
    CHECK(dimLog == s_dimDev);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev = dc.LogicalToDevice(posLog);
    CHECK(posDev == s_posDev);

    wxSize dimDev;
    dimDev = dc.LogicalToDeviceRel(dimLog);
    CHECK(dimDev == s_dimDev);
}

static void LogicalOriginChanged(wxDC& dc)
{
    // Only logical origin is changed
    const wxCoord dx = -15;
    const wxCoord dy = -20;
    dc.SetLogicalOrigin(dx, dy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog.x = dc.DeviceToLogicalX(s_posDev.x);
    posLog.y = dc.DeviceToLogicalY(s_posDev.y);
    CHECK(posLog == wxPoint(s_posDev.x + dx, s_posDev.y + dy));

    wxSize dimLog;
    dimLog.x = dc.DeviceToLogicalXRel(s_dimDev.x);
    dimLog.y = dc.DeviceToLogicalYRel(s_dimDev.y);
    CHECK(dimLog == s_dimDev);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev.x = dc.LogicalToDeviceX(posLog.x);
    posDev.y = dc.LogicalToDeviceY(posLog.y);
    CHECK(posDev == s_posDev);

    wxSize dimDev;
    dimDev.x = dc.LogicalToDeviceXRel(dimLog.x);
    dimDev.y = dc.LogicalToDeviceYRel(dimLog.y);
    CHECK(dimDev == s_dimDev);
}

static void LogicalOriginChangedEx(wxDC& dc)
{
    // Only logical origin is changed
    const wxCoord dx = -15;
    const wxCoord dy = -20;
    dc.SetLogicalOrigin(dx, dy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog = dc.DeviceToLogical(s_posDev);
    CHECK(posLog == wxPoint(s_posDev.x + dx, s_posDev.y + dy));

    wxSize dimLog;
    dimLog = dc.DeviceToLogicalRel(s_dimDev);
    CHECK(dimLog == s_dimDev);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev = dc.LogicalToDevice(posLog);
    CHECK(posDev == s_posDev);

    wxSize dimDev;
    dimDev = dc.LogicalToDeviceRel(dimLog);
    CHECK(dimDev == s_dimDev);
}

static void UserScaleChanged(wxDC& dc)
{
    // Only user scale is changed
    const double sx = 2.0;
    const double sy = 3.0;
    dc.SetUserScale(sx, sy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog.x = dc.DeviceToLogicalX(s_posDev.x);
    posLog.y = dc.DeviceToLogicalY(s_posDev.y);
    CHECK(posLog == wxPoint(wxRound(s_posDev.x / sx), wxRound(s_posDev.y / sy)));

    wxSize dimLog;
    dimLog.x = dc.DeviceToLogicalXRel(s_dimDev.x);
    dimLog.y = dc.DeviceToLogicalYRel(s_dimDev.y);
    CHECK(dimLog == wxSize(wxRound(s_dimDev.x / sx), wxRound(s_dimDev.y / sy)));

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev.x = dc.LogicalToDeviceX(posLog.x);
    posDev.y = dc.LogicalToDeviceY(posLog.y);
    CHECK(posDev == s_posDev);

    wxSize dimDev;
    dimDev.x = dc.LogicalToDeviceXRel(dimLog.x);
    dimDev.y = dc.LogicalToDeviceYRel(dimLog.y);
    CHECK(dimDev == s_dimDev);
}

static void UserScaleChangedEx(wxDC& dc)
{
    // Only user scale is changed
    const double sx = 2.0;
    const double sy = 3.0;
    dc.SetUserScale(sx, sy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog = dc.DeviceToLogical(s_posDev);
    CHECK(posLog == wxPoint(wxRound(s_posDev.x / sx), wxRound(s_posDev.y / sy)));

    wxSize dimLog;
    dimLog = dc.DeviceToLogicalRel(s_dimDev);
    CHECK(dimLog == wxSize(wxRound(s_dimDev.x / sx), wxRound(s_dimDev.y / sy)));

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev = dc.LogicalToDevice(posLog);
    CHECK(posDev == s_posDev);

    wxSize dimDev;
    dimDev = dc.LogicalToDeviceRel(dimLog);
    CHECK(dimDev == s_dimDev);
}

static void LogicalScaleChanged(wxDC& dc)
{
    // Only logical scale is changed
    const double sx = 2.0;
    const double sy = 3.0;
    dc.SetLogicalScale(sx, sy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog.x = dc.DeviceToLogicalX(s_posDev.x);
    posLog.y = dc.DeviceToLogicalY(s_posDev.y);
    CHECK(posLog == wxPoint(wxRound(s_posDev.x / sx), wxRound(s_posDev.y / sy)));

    wxSize dimLog;
    dimLog.x = dc.DeviceToLogicalXRel(s_dimDev.x);
    dimLog.y = dc.DeviceToLogicalYRel(s_dimDev.y);
    CHECK(dimLog == wxSize(wxRound(s_dimDev.x / sx), wxRound(s_dimDev.y / sy)));

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev.x = dc.LogicalToDeviceX(posLog.x);
    posDev.y = dc.LogicalToDeviceY(posLog.y);
    CHECK(posDev == s_posDev);

    wxSize dimDev;
    dimDev.x = dc.LogicalToDeviceXRel(dimLog.x);
    dimDev.y = dc.LogicalToDeviceYRel(dimLog.y);
    CHECK(dimDev == s_dimDev);
}

static void LogicalScaleChangedEx(wxDC& dc)
{
    // Only logical scale is changed
    const double sx = 2.0;
    const double sy = 3.0;
    dc.SetLogicalScale(sx, sy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog = dc.DeviceToLogical(s_posDev);
    CHECK(posLog == wxPoint(wxRound(s_posDev.x / sx), wxRound(s_posDev.y / sy)));

    wxSize dimLog;
    dimLog = dc.DeviceToLogicalRel(s_dimDev);
    CHECK(dimLog == wxSize(wxRound(s_dimDev.x / sx), wxRound(s_dimDev.y / sy)));

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev = dc.LogicalToDevice(posLog);
    CHECK(posDev == s_posDev);

    wxSize dimDev;
    dimDev = dc.LogicalToDeviceRel(dimLog);
    CHECK(dimDev == s_dimDev);
}

static void TransformedStd(wxDC& dc)
{
    // Apply all standardd transformations
    dc.SetDeviceOrigin(10, 15);
    dc.SetUserScale(0.5, 2.0);
    dc.SetLogicalScale(4.0, 1.5);
    dc.SetLogicalOrigin(-15, -20);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog.x = dc.DeviceToLogicalX(s_posDev.x);
    posLog.y = dc.DeviceToLogicalY(s_posDev.y);
    CHECK(posLog == wxPoint(-8, -6));

    wxSize dimLog;
    dimLog.x = dc.DeviceToLogicalXRel(s_dimDev.x);
    dimLog.y = dc.DeviceToLogicalYRel(s_dimDev.y);
    CHECK(dimLog == wxSize(20, 5));

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev.x = dc.LogicalToDeviceX(posLog.x);
    posDev.y = dc.LogicalToDeviceY(posLog.y);
    CHECK(posDev == s_posDev);

    wxSize dimDev;
    dimDev.x = dc.LogicalToDeviceXRel(dimLog.x);
    dimDev.y = dc.LogicalToDeviceYRel(dimLog.y);
    CHECK(dimDev == s_dimDev);
}

static void TransformedStdEx(wxDC& dc)
{
    // Apply all standardd transformations
    dc.SetDeviceOrigin(10, 15);
    dc.SetUserScale(0.5, 2.0);
    dc.SetLogicalScale(4.0, 1.5);
    dc.SetLogicalOrigin(-15, -20);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog = dc.DeviceToLogical(s_posDev);
    CHECK(posLog == wxPoint(-8, -6));

    wxSize dimLog;
    dimLog = dc.DeviceToLogicalRel(s_dimDev);
    CHECK(dimLog == wxSize(20, 5));

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev = dc.LogicalToDevice(posLog);
    CHECK(posDev == s_posDev);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev = dc.LogicalToDeviceRel(dimLog);
    CHECK(dimDev == s_dimDev);
}

static void TransformedWithMatrix(wxDC& dc)
{
    // Apply transformation matrix only
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc.CanUseTransformMatrix() )
    {
        // Apply translation and scaling only
        wxAffineMatrix2D m = dc.GetTransformMatrix();
        m.Translate(10, 15);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);

        // First convert from device to logical coordinates
        // Results should be nagative because legacy functions
        // don't take affine transformation into account.
        m.Invert();
        wxPoint2DDouble posLogRef = m.TransformPoint(wxPoint2DDouble(s_posDev));
        wxPoint posLog;
        posLog.x = dc.DeviceToLogicalX(s_posDev.x);
        posLog.y = dc.DeviceToLogicalY(s_posDev.y);
        CHECK_FALSE(posLog == wxPoint(wxRound(posLogRef.m_x), wxRound(posLogRef.m_y)));
        CHECK(posLog == s_posDev);

        wxPoint2DDouble dimLogRef = m.TransformDistance(wxPoint2DDouble(s_dimDev.x, s_dimDev.y));
        wxSize dimLog;
        dimLog.x = dc.DeviceToLogicalXRel(s_dimDev.x);
        dimLog.y = dc.DeviceToLogicalYRel(s_dimDev.y);
        CHECK_FALSE(dimLog == wxSize(wxRound(dimLogRef.m_x), wxRound(dimLogRef.m_y)));
        CHECK(dimLog == s_dimDev);

        // And next back from logical to device coordinates
        wxPoint posDev;
        posDev.x = dc.LogicalToDeviceX(posLog.x);
        posDev.y = dc.LogicalToDeviceY(posLog.y);
        CHECK(posDev == s_posDev);

        wxSize dimDev;
        dimDev.x = dc.LogicalToDeviceXRel(dimLog.x);
        dimDev.y = dc.LogicalToDeviceYRel(dimLog.y);
        CHECK(dimDev == s_dimDev);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void TransformedWithMatrixEx(wxDC& dc)
{
    // Apply transformation matrix only
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc.CanUseTransformMatrix() )
    {
        // Apply translation and scaling only
        wxAffineMatrix2D m = dc.GetTransformMatrix();
        m.Translate(10, 15);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);

        // First convert from device to logical coordinates
        m.Invert();
        wxPoint2DDouble posLogRef = m.TransformPoint(wxPoint2DDouble(s_posDev));
         wxPoint posLog;
        posLog = dc.DeviceToLogical(s_posDev);
        CHECK(posLog == wxPoint(wxRound(posLogRef.m_x), wxRound(posLogRef.m_y)));

        wxPoint2DDouble dimLogRef = m.TransformDistance(wxPoint2DDouble(s_dimDev.x, s_dimDev.y));
        wxSize dimLog;
        dimLog = dc.DeviceToLogicalRel(s_dimDev);
        CHECK(dimLog == wxSize(wxRound(dimLogRef.m_x), wxRound(dimLogRef.m_y)));

        // And next back from logical to device coordinates
        wxPoint posDev;
        posDev = dc.LogicalToDevice(posLog);
        CHECK(posDev == s_posDev);

        wxSize dimDev;
        dimDev = dc.LogicalToDeviceRel(dimLog);
        CHECK(dimDev == s_dimDev);
     }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void TransformedWithMatrixAndStd(wxDC& dc)
{
    // Apply combination of standard and matrix transformations
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc.CanUseTransformMatrix() )
    {
        dc.SetDeviceOrigin(10, 15);

        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);

        wxAffineMatrix2D m = dc.GetTransformMatrix();
        m.Translate(10, 18);
        m.Scale(2.0, 0.5);
        dc.SetTransformMatrix(m);

        // First convert from device to logical coordinates
        // Results should be nagative because legacy functions
        // don't take affine transformation into account.
        wxAffineMatrix2D m1;
        m1.Translate(10 - (-15) * (0.5 * 4.0), 15 - (-20) * (1.5 * 2.0));
        m1.Scale(0.5 * 4.0, 1.5 * 2.0);
        m1.Concat(m);
        m1.Invert();

        wxPoint2DDouble posLogRef = m1.TransformPoint(wxPoint2DDouble(s_posDev));
        wxPoint posLog;
        posLog.x = dc.DeviceToLogicalX(s_posDev.x);
        posLog.y = dc.DeviceToLogicalY(s_posDev.y);
        CHECK_FALSE(posLog == wxPoint(wxRound(posLogRef.m_x), wxRound(posLogRef.m_y)));

        wxPoint2DDouble dimLogRef = m1.TransformDistance(wxPoint2DDouble(s_dimDev.x, s_dimDev.y));
        wxSize dimLog;
        dimLog.x = dc.DeviceToLogicalXRel(s_dimDev.x);
        dimLog.y = dc.DeviceToLogicalYRel(s_dimDev.y);
        CHECK_FALSE(dimLog == wxSize(wxRound(dimLogRef.m_x), wxRound(dimLogRef.m_y)));

        // And next back from logical to device coordinates
        wxPoint posDev;
        posDev.x = dc.LogicalToDeviceX(posLog.x);
        posDev.y = dc.LogicalToDeviceY(posLog.y);
        CHECK(posDev == s_posDev);

        wxSize dimDev;
        dimDev.x = dc.LogicalToDeviceXRel(dimLog.x);
        dimDev.y = dc.LogicalToDeviceYRel(dimLog.y);
        CHECK(dimDev == s_dimDev);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void TransformedWithMatrixAndStdEx(wxDC& dc)
{
    // Apply combination of standard and matrix transformations
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc.CanUseTransformMatrix() )
    {
        dc.SetDeviceOrigin(10, 15);

        dc.SetUserScale(0.5, 1.5);
        dc.SetLogicalScale(4.0, 2.0);
        dc.SetLogicalOrigin(-15, -20);

        wxAffineMatrix2D m = dc.GetTransformMatrix();
        m.Translate(10, 18);
        m.Scale(2.0, 0.5);
        dc.SetTransformMatrix(m);

        // First convert from device to logical coordinates
        wxAffineMatrix2D m1;
        m1.Translate(10 - (-15) * (0.5 * 4.0), 15 - (-20) * (1.5 * 2.0));
        m1.Scale(0.5 * 4.0, 1.5 * 2.0);
        m1.Concat(m);
        m1.Invert();

        wxPoint2DDouble posLogRef = m1.TransformPoint(wxPoint2DDouble(s_posDev));
        wxPoint posLog;
        posLog = dc.DeviceToLogical(s_posDev);

        if ( wxIsRunningUnderWine() )
        {
            // Current versions of Wine seem to have a bug and return a value
            // which is one off from DPtoLP() used by wxDC::DeviceToLogical()
            // and there doesn't seem to be anything we can do about it, so
            // just tweak the result to pass this test.
            if ( posLog.x == posLogRef.m_x + 1 )
                posLog.x--;
            else
                WARN("Wine workaround might be not needed any longer");
        }

        CHECK(posLog == wxPoint(wxRound(posLogRef.m_x), wxRound(posLogRef.m_y)));

        wxPoint2DDouble dimLogRef = m1.TransformDistance(wxPoint2DDouble(s_dimDev.x, s_dimDev.y));
        wxSize dimLog;
        dimLog = dc.DeviceToLogicalRel(s_dimDev);
        CHECK(dimLog == wxSize(wxRound(dimLogRef.m_x), wxRound(dimLogRef.m_y)));

        // And next back from logical to device coordinates
        wxPoint posDev;
        posDev = dc.LogicalToDevice(posLog);
        CHECK(posDev == s_posDev);

        wxSize dimDev;
        dimDev = dc.LogicalToDeviceRel(dimLog);
        CHECK(dimDev == s_dimDev);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void RotatedWithMatrix(wxDC& dc)
{
    // Apply matrix transformations with rotation component
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc.CanUseTransformMatrix() )
    {
        wxAffineMatrix2D m = dc.GetTransformMatrix();
        m.Rotate(6 * M_PI / 180.0);
        m.Translate(10, 15);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);

        // First convert from device to logical coordinates
        // Results should be nagative because legacy functions
        // don't take affine transformation into account.
        m.Invert();
        wxPoint2DDouble posLogRef = m.TransformPoint(wxPoint2DDouble(s_posDev));
        wxPoint posLog;
        posLog.x = dc.DeviceToLogicalX(s_posDev.x);
        posLog.y = dc.DeviceToLogicalY(s_posDev.y);
        CHECK_FALSE(posLog == wxPoint(wxRound(posLogRef.m_x), wxRound(posLogRef.m_y)));
        CHECK(posLog == s_posDev);

        wxPoint2DDouble dimLogRef = m.TransformDistance(wxPoint2DDouble(s_dimDev.x, s_dimDev.y));
        wxSize dimLog;
        dimLog.x = dc.DeviceToLogicalXRel(s_dimDev.x);
        dimLog.y = dc.DeviceToLogicalYRel(s_dimDev.y);
        CHECK_FALSE(dimLog == wxSize(wxRound(dimLogRef.m_x), wxRound(dimLogRef.m_y)));
        CHECK(dimLog == s_dimDev);

        // And next back from logical to device coordinates
        wxPoint posDev;
        posDev.x = dc.LogicalToDeviceX(posLog.x);
        posDev.y = dc.LogicalToDeviceY(posLog.y);
        CHECK(posDev == s_posDev);

        wxSize dimDev;
        dimDev.x = dc.LogicalToDeviceXRel(dimLog.x);
        dimDev.y = dc.LogicalToDeviceYRel(dimLog.y);
        CHECK(dimDev == s_dimDev);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void RotatedWithMatrixEx(wxDC& dc)
{
    // Apply matrix transformations with rotation component
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc.CanUseTransformMatrix() )
    {
        wxAffineMatrix2D m = dc.GetTransformMatrix();
        m.Rotate(6 * M_PI / 180.0);
        m.Translate(10, 15);
        m.Scale(2.0, 3.0);
        dc.SetTransformMatrix(m);

        // First convert from device to logical coordinates
        m.Invert();
        wxPoint2DDouble posLogRef = m.TransformPoint(wxPoint2DDouble(s_posDev));
        wxPoint posLog;
        posLog = dc.DeviceToLogical(s_posDev);
        CHECK(posLog == wxPoint(wxRound(posLogRef.m_x), wxRound(posLogRef.m_y)) );

        wxPoint2DDouble dimLogRef = m.TransformDistance(wxPoint2DDouble(s_dimDev.x, s_dimDev.y));
        wxSize dimLog;
        dimLog = dc.DeviceToLogicalRel(s_dimDev);
        CHECK(dimLog == wxSize(wxRound(dimLogRef.m_x), wxRound(dimLogRef.m_y)));

        // And next back from logical to device coordinates
        wxPoint posDev;
        posDev = dc.LogicalToDevice(posLog);
        CHECK(Approx(posDev.x).margin(1) == s_posDev.x);
        CHECK(Approx(posDev.y).margin(1) == s_posDev.y);

        wxSize dimDev;
        dimDev = dc.LogicalToDeviceRel(dimLog);
        CHECK(Approx(dimDev.x).margin(1) == s_dimDev.x);
        CHECK(Approx(dimDev.y).margin(1) == s_dimDev.y);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

TEST_CASE("CoordinatesTestCase::wxDC", "[coordinates][dc]")
{
    wxBitmap bmp(s_dcSize);
    wxMemoryDC dc(bmp);

    SECTION("InitialState")
    {
        InitialState(dc);
    }

    SECTION("NoTransform")
    {
        NoTransform(dc);
    }

    SECTION("NoTransformEx")
    {
        NoTransformEx(dc);
    }

    SECTION("DeviceOriginChanged")
    {
        // Only device origin is changed
        DeviceOriginChanged(dc);
    }

    SECTION("DeviceOriginChangedEx")
    {
        // Only device origin is changed
        DeviceOriginChangedEx(dc);
    }

    SECTION("LogicalOriginChanged")
    {
        // Only logical origin is changed
        LogicalOriginChanged(dc);
    }

    SECTION("LogicalOriginChangedEx")
    {
        // Only logical origin is changed
        LogicalOriginChangedEx(dc);
    }

    SECTION("UserScaleChanged")
    {
        // Only user scale is changed
        UserScaleChanged(dc);
    }

    SECTION("UserScaleChangedEx")
    {
        // Only user scale is changed
        UserScaleChangedEx(dc);
    }

    SECTION("LogicalScaleChanged")
    {
        // Only logical scale is changed
        LogicalScaleChanged(dc);
    }

    SECTION("LogicalScaleChangedEx")
    {
        // Only logical scale is changed
        LogicalScaleChangedEx(dc);
    }

    SECTION("TransformedStd")
    {
        // Apply all standardd transformations
        TransformedStd(dc);
    }

    SECTION("TransformedStdEx")
    {
        // Apply all standardd transformations
        TransformedStdEx(dc);
    }

    SECTION("TransformedWithMatrix")
    {
        // Apply transformation matrix only
        TransformedWithMatrix(dc);
    }

    SECTION("TransformedWithMatrixEx")
    {
        // Apply transformation matrix only
        TransformedWithMatrixEx(dc);
    }

    SECTION("TransformedWithMatrixAndStd")
    {
        // Apply combination of standard and matrix transformations
        TransformedWithMatrixAndStd(dc);
    }

    SECTION("TransformedWithMatrixAndStdEx")
    {
        // Apply combination of standard and matrix transformations
        TransformedWithMatrixAndStdEx(dc);
    }

    SECTION("RotatedWithMatrix")
    {
        // Apply matrix transformations with rotation component
        RotatedWithMatrix(dc);
    }

    SECTION("RotatedWithMatrixEx")
    {
        // Apply matrix transformations with rotation component
        RotatedWithMatrixEx(dc);
    }
}

#if wxUSE_GRAPHICS_CONTEXT
TEST_CASE("CoordinatesTestCase::wxGCDC", "[coordinates][dc][gcdc]")
{
    wxBitmap bmp(s_dcSize);
    wxMemoryDC mdc(bmp);
    wxGCDC dc(mdc);
    dc.GetGraphicsContext()->SetAntialiasMode(wxANTIALIAS_NONE);
    dc.GetGraphicsContext()->DisableOffset();

    SECTION("InitialState")
    {
        InitialState(dc);
    }

    SECTION("NoTransform")
    {
        NoTransform(dc);
    }

    SECTION("NoTransformEx")
    {
        NoTransformEx(dc);
    }

    SECTION("DeviceOriginChanged")
    {
        // Only device origin is changed
        DeviceOriginChanged(dc);
    }

    SECTION("DeviceOriginChangedEx")
    {
        // Only device origin is changed
        DeviceOriginChangedEx(dc);
    }

    SECTION("LogicalOriginChanged")
    {
        // Only logical origin is changed
        LogicalOriginChanged(dc);
    }

    SECTION("LogicalOriginChangedEx")
    {
        // Only logical origin is changed
        LogicalOriginChangedEx(dc);
    }

    SECTION("UserScaleChanged")
    {
        // Only user scale is changed
        UserScaleChanged(dc);
    }

    SECTION("UserScaleChangedEx")
    {
        // Only user scale is changed
        UserScaleChangedEx(dc);
    }

    SECTION("LogicalScaleChanged")
    {
        // Only logical scale is changed
        LogicalScaleChanged(dc);
    }

    SECTION("LogicalScaleChangedEx")
    {
        // Only logical scale is changed
        LogicalScaleChangedEx(dc);
    }

    SECTION("TransformedStd")
    {
        // Apply all standardd transformations
        TransformedStd(dc);
    }

    SECTION("TransformedStdEx")
    {
        // Apply all standardd transformations
        TransformedStdEx(dc);
    }

    SECTION("TransformedWithMatrix")
    {
        // Apply transformation matrix only
        TransformedWithMatrix(dc);
    }

    SECTION("TransformedWithMatrixEx")
    {
        // Apply transformation matrix only
        TransformedWithMatrixEx(dc);
    }

    SECTION("TransformedWithMatrixAndStd")
    {
        // Apply combination of standard and matrix transformations
        TransformedWithMatrixAndStd(dc);
    }

    SECTION("TransformedWithMatrixAndStdEx")
    {
        // Apply combination of standard and matrix transformations
        TransformedWithMatrixAndStdEx(dc);
    }

    SECTION("RotatedWithMatrix")
    {
        // Apply matrix transformations with rotation component
        RotatedWithMatrix(dc);
    }

    SECTION("RotatedWithMatrixEx")
    {
        // Apply matrix transformations with rotation component
        RotatedWithMatrixEx(dc);
    }
}

#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
TEST_CASE("CoordinatesTestCase::wxGCDC(GDI+)", "[coordinates][dc][gcdc][gdiplus]")
{
    int depth = GENERATE(24, 32);

    wxBitmap bmp(s_dcSize, depth);
    wxMemoryDC mdc(bmp);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetGDIPlusRenderer();
    REQUIRE(rend);
    wxGraphicsContext* ctx = rend->CreateContext(mdc);
    ctx->SetAntialiasMode(wxANTIALIAS_NONE);
    ctx->DisableOffset();
    wxGCDC dc(ctx);

    SECTION("InitialState")
    {
        InitialState(dc);
    }

    SECTION("NoTransform")
    {
        NoTransform(dc);
    }

    SECTION("NoTransformEx")
    {
        NoTransformEx(dc);
    }

    SECTION("DeviceOriginChanged")
    {
        // Only device origin is changed
        DeviceOriginChanged(dc);
    }

    SECTION("DeviceOriginChangedEx")
    {
        // Only device origin is changed
        DeviceOriginChangedEx(dc);
    }

    SECTION("LogicalOriginChanged")
    {
        // Only logical origin is changed
        LogicalOriginChanged(dc);
    }

    SECTION("LogicalOriginChangedEx")
    {
        // Only logical origin is changed
        LogicalOriginChangedEx(dc);
    }

    SECTION("UserScaleChanged")
    {
        // Only user scale is changed
        UserScaleChanged(dc);
    }

    SECTION("UserScaleChangedEx")
    {
        // Only user scale is changed
        UserScaleChangedEx(dc);
    }

    SECTION("LogicalScaleChanged")
    {
        // Only logical scale is changed
        LogicalScaleChanged(dc);
    }

    SECTION("LogicalScaleChangedEx")
    {
        // Only logical scale is changed
        LogicalScaleChangedEx(dc);
    }

    SECTION("TransformedStd")
    {
        // Apply all standardd transformations
        TransformedStd(dc);
    }

    SECTION("TransformedStdEx")
    {
        // Apply all standardd transformations
        TransformedStdEx(dc);
    }

    SECTION("TransformedWithMatrix")
    {
        // Apply transformation matrix only
        TransformedWithMatrix(dc);
    }

    SECTION("TransformedWithMatrixEx")
    {
        // Apply transformation matrix only
        TransformedWithMatrixEx(dc);
    }

    SECTION("TransformedWithMatrixAndStd")
    {
        // Apply combination of standard and matrix transformations
        TransformedWithMatrixAndStd(dc);
    }

    SECTION("TransformedWithMatrixAndStdEx")
    {
        // Apply combination of standard and matrix transformations
        TransformedWithMatrixAndStdEx(dc);
    }

    SECTION("RotatedWithMatrix")
    {
        // Apply matrix transformations with rotation component
        RotatedWithMatrix(dc);
    }

    SECTION("RotatedWithMatrixEx")
    {
        // Apply matrix transformations with rotation component
        RotatedWithMatrixEx(dc);
    }
}
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
TEST_CASE("CoordinatesTestCase::wxGCDC(Direct2D)", "[coordinates][dc][gcdc][direct2d]")
{
    int depth = GENERATE(24, 32);

    wxBitmap bmp(s_dcSize, depth);
    wxMemoryDC mdc(bmp);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetDirect2DRenderer();
    REQUIRE(rend);
    wxGraphicsContext* ctx = rend->CreateContext(mdc);
    ctx->SetAntialiasMode(wxANTIALIAS_NONE);
    ctx->DisableOffset();
    wxGCDC dc(ctx);

    SECTION("InitialState")
    {
        InitialState(dc);
    }

    SECTION("NoTransform")
    {
        NoTransform(dc);
    }

    SECTION("NoTransformEx")
    {
        NoTransformEx(dc);
    }

    SECTION("DeviceOriginChanged")
    {
        // Only device origin is changed
        DeviceOriginChanged(dc);
    }

    SECTION("DeviceOriginChangedEx")
    {
        // Only device origin is changed
        DeviceOriginChangedEx(dc);
    }

    SECTION("LogicalOriginChanged")
    {
        // Only logical origin is changed
        LogicalOriginChanged(dc);
    }

    SECTION("LogicalOriginChangedEx")
    {
        // Only logical origin is changed
        LogicalOriginChangedEx(dc);
    }

    SECTION("UserScaleChanged")
    {
        // Only user scale is changed
        UserScaleChanged(dc);
    }

    SECTION("UserScaleChangedEx")
    {
        // Only user scale is changed
        UserScaleChangedEx(dc);
    }

    SECTION("LogicalScaleChanged")
    {
        // Only logical scale is changed
        LogicalScaleChanged(dc);
    }

    SECTION("LogicalScaleChangedEx")
    {
        // Only logical scale is changed
        LogicalScaleChangedEx(dc);
    }

    SECTION("TransformedStd")
    {
        // Apply all standardd transformations
        TransformedStd(dc);
    }

    SECTION("TransformedStdEx")
    {
        // Apply all standardd transformations
        TransformedStdEx(dc);
    }

    SECTION("TransformedWithMatrix")
    {
        // Apply transformation matrix only
        TransformedWithMatrix(dc);
    }

    SECTION("TransformedWithMatrixEx")
    {
        // Apply transformation matrix only
        TransformedWithMatrixEx(dc);
    }

    SECTION("TransformedWithMatrixAndStd")
    {
        // Apply combination of standard and matrix transformations
        TransformedWithMatrixAndStd(dc);
    }

    SECTION("TransformedWithMatrixAndStdEx")
    {
        // Apply combination of standard and matrix transformations
        TransformedWithMatrixAndStdEx(dc);
    }

    SECTION("RotatedWithMatrix")
    {
        // Apply matrix transformations with rotation component
        RotatedWithMatrix(dc);
    }

    SECTION("RotatedWithMatrixEx")
    {
        // Apply matrix transformations with rotation component
        RotatedWithMatrixEx(dc);
    }
}
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
TEST_CASE("CoordinatesTestCase::wxGCDC(Cairo)", "[coordinates][dc][gcdc][cairo]")
{
#ifdef __WXMSW__
    int depth = GENERATE(24, 32);

    wxBitmap bmp(s_dcSize, depth);
#else
    wxBitmap bmp(s_dcSize);
#endif
    wxMemoryDC mdc(bmp);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetCairoRenderer();
    REQUIRE(rend);
    wxGraphicsContext* ctx = rend->CreateContext(mdc);
    ctx->SetAntialiasMode(wxANTIALIAS_NONE);
    ctx->DisableOffset();
    wxGCDC dc(ctx);

    SECTION("InitialState")
    {
        InitialState(dc);
    }

    SECTION("NoTransform")
    {
        NoTransform(dc);
    }

    SECTION("NoTransformEx")
    {
        NoTransformEx(dc);
    }

    SECTION("DeviceOriginChanged")
    {
        // Only device origin is changed
        DeviceOriginChanged(dc);
    }

    SECTION("DeviceOriginChangedEx")
    {
        // Only device origin is changed
        DeviceOriginChangedEx(dc);
    }

    SECTION("LogicalOriginChanged")
    {
        // Only logical origin is changed
        LogicalOriginChanged(dc);
    }

    SECTION("LogicalOriginChangedEx")
    {
        // Only logical origin is changed
        LogicalOriginChangedEx(dc);
    }

    SECTION("UserScaleChanged")
    {
        // Only user scale is changed
        UserScaleChanged(dc);
    }

    SECTION("UserScaleChangedEx")
    {
        // Only user scale is changed
        UserScaleChangedEx(dc);
    }

    SECTION("LogicalScaleChanged")
    {
        // Only logical scale is changed
        LogicalScaleChanged(dc);
    }

    SECTION("LogicalScaleChangedEx")
    {
        // Only logical scale is changed
        LogicalScaleChangedEx(dc);
    }

    SECTION("TransformedStd")
    {
        // Apply all standardd transformations
        TransformedStd(dc);
    }

    SECTION("TransformedStdEx")
    {
        // Apply all standardd transformations
        TransformedStdEx(dc);
    }

    SECTION("TransformedWithMatrix")
    {
        // Apply transformation matrix only
        TransformedWithMatrix(dc);
    }

    SECTION("TransformedWithMatrixEx")
    {
        // Apply transformation matrix only
        TransformedWithMatrixEx(dc);
    }

    SECTION("TransformedWithMatrixAndStd")
    {
        // Apply combination of standard and matrix transformations
        TransformedWithMatrixAndStd(dc);
    }

    SECTION("TransformedWithMatrixAndStdEx")
    {
        // Apply combination of standard and matrix transformations
        TransformedWithMatrixAndStdEx(dc);
    }

    SECTION("RotatedWithMatrix")
    {
        // Apply matrix transformations with rotation component
        RotatedWithMatrix(dc);
    }

    SECTION("RotatedWithMatrixEx")
    {
        // Apply matrix transformations with rotation component
        RotatedWithMatrixEx(dc);
    }
}
#endif // wxUSE_CAIRO
#endif // wxUSE_GRAPHICS_CONTEXT

TEST_CASE("wxPoint2D", "[geometry]")
{
    wxPoint2DInt pi(3, 14);
    pi *= 3;
    CHECK(pi == wxPoint2DInt(9, 42));

    wxPoint2DDouble pd(3, 4);
    pd /= 4;
    CHECK(pd == wxPoint2DDouble(0.75, 1));
}
