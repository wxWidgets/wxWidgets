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

// ====================
// wxDC / wxGCDC tests
// ====================

class CoordinatesDCTestCaseBase
{
public:
    CoordinatesDCTestCaseBase()
    {
        m_bmp.Create(s_dcSize);
        m_dc = nullptr;
    }

    virtual ~CoordinatesDCTestCaseBase()
    {
        m_bmp = wxNullBitmap;
    }

protected:
    wxBitmap m_bmp;
    wxDC* m_dc;
};

// ===========
// wxDC tests
// ===========

class CoordinatesDCTestCase : public CoordinatesDCTestCaseBase
{
public:
    CoordinatesDCTestCase()
    {
        m_mdc.SelectObject(m_bmp);
        m_dc = &m_mdc;
    }

    virtual ~CoordinatesDCTestCase()
    {
        m_mdc.SelectObject(wxNullBitmap);
    }

protected:
    wxMemoryDC m_mdc;
};

#if wxUSE_GRAPHICS_CONTEXT
// =============
// wxGCDC tests
// =============

class CoordinatesGCDCTestCase : public CoordinatesDCTestCase
{
public:
    CoordinatesGCDCTestCase()
    {
        m_gcdc = new wxGCDC(m_mdc);

        wxGraphicsContext* ctx = m_gcdc->GetGraphicsContext();
        ctx->SetAntialiasMode(wxANTIALIAS_NONE);
        ctx->DisableOffset();

        m_dc = m_gcdc;
    }

    virtual ~CoordinatesGCDCTestCase()
    {
        delete m_gcdc;
    }

protected:
    wxGCDC* m_gcdc;
};
#endif //  wxUSE_GRAPHICS_CONTEXT

// =====  Implementation  =====

static void InitialState(wxDC* dc)
{
    // Check initial state

    wxPoint origin = dc->GetDeviceOrigin();
    CHECK(origin.x == 0);
    CHECK(origin.y == 0);

    origin = dc->GetLogicalOrigin();
    CHECK(origin.x == 0);
    CHECK(origin.y == 0);

    double sx, sy;
    dc->GetUserScale(&sx, &sy);
    CHECK(sx == 1.0);
    CHECK(sy == 1.0);

    dc->GetLogicalScale(&sx, &sy);
    CHECK(sx == 1.0);
    CHECK(sy == 1.0);

#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc->CanUseTransformMatrix() )
    {
        wxAffineMatrix2D m = dc->GetTransformMatrix();
        CHECK(m.IsIdentity() == true);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void NoTransform(wxDC *dc)
{
    // No transformations

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog.x = dc->DeviceToLogicalX(s_posDev.x);
    posLog.y = dc->DeviceToLogicalY(s_posDev.y);
    CHECK(posLog.x == s_posDev.x);
    CHECK(posLog.y == s_posDev.y);

    wxSize dimLog;
    dimLog.x = dc->DeviceToLogicalXRel(s_dimDev.x);
    dimLog.y = dc->DeviceToLogicalYRel(s_dimDev.y);
    CHECK(dimLog.x == s_dimDev.x);
    CHECK(dimLog.y == s_dimDev.y);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev.x = dc->LogicalToDeviceX(posLog.x);
    posDev.y = dc->LogicalToDeviceY(posLog.y);
    CHECK(posDev.x == s_posDev.x);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev.x = dc->LogicalToDeviceXRel(dimLog.x);
    dimDev.y = dc->LogicalToDeviceYRel(dimLog.y);
    CHECK(dimDev.x == s_dimDev.x);
    CHECK(dimDev.y == s_dimDev.y);
}

static void NoTransformEx(wxDC * dc)
{
    // No transformations

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog = dc->DeviceToLogical(s_posDev);
    CHECK(posLog.x == s_posDev.x);
    CHECK(posLog.y == s_posDev.y);

    wxSize dimLog;
    dimLog = dc->DeviceToLogicalRel(s_dimDev);
    CHECK(dimLog.x == s_dimDev.x);
    CHECK(dimLog.y == s_dimDev.y);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev = dc->LogicalToDevice(posLog);
    CHECK(posDev.x == s_posDev.x);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev = dc->LogicalToDeviceRel(dimLog);
    CHECK(dimDev.x == s_dimDev.x);
    CHECK(dimDev.y == s_dimDev.y);
}

static void DeviceOriginChanged(wxDC* dc)
{
    // Only device origin is changed
    const wxCoord dx = 10;
    const wxCoord dy = 15;
    dc->SetDeviceOrigin(dx, dy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog.x = dc->DeviceToLogicalX(s_posDev.x);
    posLog.y = dc->DeviceToLogicalY(s_posDev.y);
    CHECK(posLog.x == s_posDev.x - dx);
    CHECK(posLog.y == s_posDev.y - dy);

    wxSize dimLog;
    dimLog.x = dc->DeviceToLogicalXRel(s_dimDev.x);
    dimLog.y = dc->DeviceToLogicalYRel(s_dimDev.y);
    CHECK(dimLog.x == s_dimDev.x);
    CHECK(dimLog.y == s_dimDev.y);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev.x = dc->LogicalToDeviceX(posLog.x);
    posDev.y = dc->LogicalToDeviceY(posLog.y);
    CHECK(posDev.x == s_posDev.x);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev.x = dc->LogicalToDeviceXRel(dimLog.x);
    dimDev.y = dc->LogicalToDeviceYRel(dimLog.y);
    CHECK(dimDev.x == s_dimDev.x);
    CHECK(dimDev.y == s_dimDev.y);
}

static void DeviceOriginChangedEx(wxDC * dc)
{
    // Only device origin is changed
    const wxCoord dx = 10;
    const wxCoord dy = 15;
    dc->SetDeviceOrigin(dx, dy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog = dc->DeviceToLogical(s_posDev);
    CHECK(posLog.x == s_posDev.x - dx);
    CHECK(posLog.y == s_posDev.y - dy);

    wxSize dimLog;
    dimLog = dc->DeviceToLogicalRel(s_dimDev);
    CHECK(dimLog.x == s_dimDev.x);
    CHECK(dimLog.y == s_dimDev.y);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev = dc->LogicalToDevice(posLog);
    CHECK(posDev.x == s_posDev.x);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev = dc->LogicalToDeviceRel(dimLog);
    CHECK(dimDev.x == s_dimDev.x);
    CHECK(dimDev.y == s_dimDev.y);
}

static void LogicalOriginChanged(wxDC* dc)
{
    // Only logical origin is changed
    const wxCoord dx = -15;
    const wxCoord dy = -20;
    dc->SetLogicalOrigin(dx, dy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog.x = dc->DeviceToLogicalX(s_posDev.x);
    posLog.y = dc->DeviceToLogicalY(s_posDev.y);
    CHECK(posLog.x == s_posDev.x + dx);
    CHECK(posLog.y == s_posDev.y + dy);

    wxSize dimLog;
    dimLog.x = dc->DeviceToLogicalXRel(s_dimDev.x);
    dimLog.y = dc->DeviceToLogicalYRel(s_dimDev.y);
    CHECK(dimLog.x == s_dimDev.x);
    CHECK(dimLog.y == s_dimDev.y);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev.x = dc->LogicalToDeviceX(posLog.x);
    posDev.y = dc->LogicalToDeviceY(posLog.y);
    CHECK(posDev.x == s_posDev.x);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev.x = dc->LogicalToDeviceXRel(dimLog.x);
    dimDev.y = dc->LogicalToDeviceYRel(dimLog.y);
    CHECK(dimDev.x == s_dimDev.x);
    CHECK(dimDev.y == s_dimDev.y);
}

static void LogicalOriginChangedEx(wxDC * dc)
{
    // Only logical origin is changed
    const wxCoord dx = -15;
    const wxCoord dy = -20;
    dc->SetLogicalOrigin(dx, dy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog = dc->DeviceToLogical(s_posDev);
    CHECK(posLog.x == s_posDev.x + dx);
    CHECK(posLog.y == s_posDev.y + dy);

    wxSize dimLog;
    dimLog = dc->DeviceToLogicalRel(s_dimDev);
    CHECK(dimLog.x == s_dimDev.x);
    CHECK(dimLog.y == s_dimDev.y);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev = dc->LogicalToDevice(posLog);
    CHECK(posDev.x == s_posDev.x);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev = dc->LogicalToDeviceRel(dimLog);
    CHECK(dimDev.x == s_dimDev.x);
    CHECK(dimDev.y == s_dimDev.y);
}

static void UserScaleChanged(wxDC* dc)
{
    // Only user scale is changed
    const double sx = 2.0;
    const double sy = 3.0;
    dc->SetUserScale(sx, sy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog.x = dc->DeviceToLogicalX(s_posDev.x);
    posLog.y = dc->DeviceToLogicalY(s_posDev.y);
    CHECK(posLog.x == wxRound(s_posDev.x / sx));
    CHECK(posLog.y == wxRound(s_posDev.y / sy));

    wxSize dimLog;
    dimLog.x = dc->DeviceToLogicalXRel(s_dimDev.x);
    dimLog.y = dc->DeviceToLogicalYRel(s_dimDev.y);
    CHECK(dimLog.x == wxRound(s_dimDev.x / sx));
    CHECK(dimLog.y == wxRound(s_dimDev.y / sy));

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev.x = dc->LogicalToDeviceX(posLog.x);
    posDev.y = dc->LogicalToDeviceY(posLog.y);
    CHECK(posDev.x == s_posDev.x);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev.x = dc->LogicalToDeviceXRel(dimLog.x);
    dimDev.y = dc->LogicalToDeviceYRel(dimLog.y);
    CHECK(dimDev.x == s_dimDev.x);
    CHECK(dimDev.y == s_dimDev.y);
}

static void UserScaleChangedEx(wxDC * dc)
{
    // Only user scale is changed
    const double sx = 2.0;
    const double sy = 3.0;
    dc->SetUserScale(sx, sy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog = dc->DeviceToLogical(s_posDev);
    CHECK(posLog.x == wxRound(s_posDev.x / sx));
    CHECK(posLog.y == wxRound(s_posDev.y / sy));

    wxSize dimLog;
    dimLog = dc->DeviceToLogicalRel(s_dimDev);
    CHECK(dimLog.x == wxRound(s_dimDev.x / sx));
    CHECK(dimLog.y == wxRound(s_dimDev.y / sy));

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev = dc->LogicalToDevice(posLog);
    CHECK(posDev.x == s_posDev.x);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev = dc->LogicalToDeviceRel(dimLog);
    CHECK(dimDev.x == s_dimDev.x);
    CHECK(dimDev.y == s_dimDev.y);
}

static void LogicalScaleChanged(wxDC* dc)
{
    // Only logical scale is changed
    const double sx = 2.0;
    const double sy = 3.0;
    dc->SetLogicalScale(sx, sy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog.x = dc->DeviceToLogicalX(s_posDev.x);
    posLog.y = dc->DeviceToLogicalY(s_posDev.y);
    CHECK(posLog.x == wxRound(s_posDev.x / sx));
    CHECK(posLog.y == wxRound(s_posDev.y / sy));

    wxSize dimLog;
    dimLog.x = dc->DeviceToLogicalXRel(s_dimDev.x);
    dimLog.y = dc->DeviceToLogicalYRel(s_dimDev.y);
    CHECK(dimLog.x == wxRound(s_dimDev.x / sx));
    CHECK(dimLog.y == wxRound(s_dimDev.y / sy));

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev.x = dc->LogicalToDeviceX(posLog.x);
    posDev.y = dc->LogicalToDeviceY(posLog.y);
    CHECK(posDev.x == s_posDev.x);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev.x = dc->LogicalToDeviceXRel(dimLog.x);
    dimDev.y = dc->LogicalToDeviceYRel(dimLog.y);
    CHECK(dimDev.x == s_dimDev.x);
    CHECK(dimDev.y == s_dimDev.y);
}

static void LogicalScaleChangedEx(wxDC * dc)
{
    // Only logical scale is changed
    const double sx = 2.0;
    const double sy = 3.0;
    dc->SetLogicalScale(sx, sy);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog = dc->DeviceToLogical(s_posDev);
    CHECK(posLog.x == wxRound(s_posDev.x / sx));
    CHECK(posLog.y == wxRound(s_posDev.y / sy));

    wxSize dimLog;
    dimLog = dc->DeviceToLogicalRel(s_dimDev);
    CHECK(dimLog.x == wxRound(s_dimDev.x / sx));
    CHECK(dimLog.y == wxRound(s_dimDev.y / sy));

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev = dc->LogicalToDevice(posLog);
    CHECK(posDev.x == s_posDev.x);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev = dc->LogicalToDeviceRel(dimLog);
    CHECK(dimDev.x == s_dimDev.x);
    CHECK(dimDev.y == s_dimDev.y);
}

static void TransformedStd(wxDC* dc)
{
    // Apply all standardd transformations
    dc->SetDeviceOrigin(10, 15);
    dc->SetUserScale(0.5, 2.0);
    dc->SetLogicalScale(4.0, 1.5);
    dc->SetLogicalOrigin(-15, -20);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog.x = dc->DeviceToLogicalX(s_posDev.x);
    posLog.y = dc->DeviceToLogicalY(s_posDev.y);
    CHECK(posLog.x == -8);
    CHECK(posLog.y == -6);

    wxSize dimLog;
    dimLog.x = dc->DeviceToLogicalXRel(s_dimDev.x);
    dimLog.y = dc->DeviceToLogicalYRel(s_dimDev.y);
    CHECK(dimLog.x == 20);
    CHECK(dimLog.y == 5);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev.x = dc->LogicalToDeviceX(posLog.x);
    posDev.y = dc->LogicalToDeviceY(posLog.y);
    CHECK(posDev.x == s_posDev.x);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev.x = dc->LogicalToDeviceXRel(dimLog.x);
    dimDev.y = dc->LogicalToDeviceYRel(dimLog.y);
    CHECK(dimDev.x == s_dimDev.x);
    CHECK(dimDev.y == s_dimDev.y);
}

static void TransformedStdEx(wxDC * dc)
{
    // Apply all standardd transformations
    dc->SetDeviceOrigin(10, 15);
    dc->SetUserScale(0.5, 2.0);
    dc->SetLogicalScale(4.0, 1.5);
    dc->SetLogicalOrigin(-15, -20);

    // First convert from device to logical coordinates
    wxPoint posLog;
    posLog = dc->DeviceToLogical(s_posDev);
    CHECK(posLog.x == -8);
    CHECK(posLog.y == -6);

    wxSize dimLog;
    dimLog = dc->DeviceToLogicalRel(s_dimDev);
    CHECK(dimLog.x == 20);
    CHECK(dimLog.y == 5);

    // And next back from logical to device coordinates
    wxPoint posDev;
    posDev = dc->LogicalToDevice(posLog);
    CHECK(posDev.x == s_posDev.x);
    CHECK(posDev.y == s_posDev.y);

    wxSize dimDev;
    dimDev = dc->LogicalToDeviceRel(dimLog);
    CHECK(dimDev.x == s_dimDev.x);
    CHECK(dimDev.y == s_dimDev.y);
}

static void TransformedWithMatrix(wxDC* dc)
{
    // Apply transformation matrix only
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc->CanUseTransformMatrix() )
    {
        // Apply translation and scaling only
        wxAffineMatrix2D m = dc->GetTransformMatrix();
        m.Translate(10, 15);
        m.Scale(2.0, 3.0);
        dc->SetTransformMatrix(m);

        // First convert from device to logical coordinates
        // Results should be nagative because legacy functions
        // don't take affine transformation into account.
        m.Invert();
        wxPoint2DDouble posLogRef = m.TransformPoint(wxPoint2DDouble(s_posDev));
        wxPoint posLog;
        posLog.x = dc->DeviceToLogicalX(s_posDev.x);
        posLog.y = dc->DeviceToLogicalY(s_posDev.y);
        CHECK_FALSE(posLog.x == wxRound(posLogRef.m_x));
        CHECK_FALSE(posLog.y == wxRound(posLogRef.m_y));
        CHECK(posLog.x == s_posDev.x);
        CHECK(posLog.y == s_posDev.y);

        wxPoint2DDouble dimLogRef = m.TransformDistance(wxPoint2DDouble(s_dimDev.x, s_dimDev.y));
        wxSize dimLog;
        dimLog.x = dc->DeviceToLogicalXRel(s_dimDev.x);
        dimLog.y = dc->DeviceToLogicalYRel(s_dimDev.y);
        CHECK_FALSE(dimLog.x == wxRound(dimLogRef.m_x));
        CHECK_FALSE(dimLog.y == wxRound(dimLogRef.m_y));
        CHECK(dimLog.x == s_dimDev.x);
        CHECK(dimLog.y == s_dimDev.y);

        // And next back from logical to device coordinates
        wxPoint posDev;
        posDev.x = dc->LogicalToDeviceX(posLog.x);
        posDev.y = dc->LogicalToDeviceY(posLog.y);
        CHECK(posDev.x == s_posDev.x);
        CHECK(posDev.y == s_posDev.y);

        wxSize dimDev;
        dimDev.x = dc->LogicalToDeviceXRel(dimLog.x);
        dimDev.y = dc->LogicalToDeviceYRel(dimLog.y);
        CHECK(dimDev.x == s_dimDev.x);
        CHECK(dimDev.y == s_dimDev.y);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void TransformedWithMatrixEx(wxDC * dc)
{
    // Apply transformation matrix only
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc->CanUseTransformMatrix() )
    {
        // Apply translation and scaling only
        wxAffineMatrix2D m = dc->GetTransformMatrix();
        m.Translate(10, 15);
        m.Scale(2.0, 3.0);
        dc->SetTransformMatrix(m);

        // First convert from device to logical coordinates
        m.Invert();
        wxPoint2DDouble posLogRef = m.TransformPoint(wxPoint2DDouble(s_posDev));
        wxPoint posLog;
        posLog = dc->DeviceToLogical(s_posDev);
        CHECK(posLog.x == wxRound(posLogRef.m_x));
        CHECK(posLog.y == wxRound(posLogRef.m_y));

        wxPoint2DDouble dimLogRef = m.TransformDistance(wxPoint2DDouble(s_dimDev.x, s_dimDev.y));
        wxSize dimLog;
        dimLog = dc->DeviceToLogicalRel(s_dimDev);
        CHECK(dimLog.x == wxRound(dimLogRef.m_x));
        CHECK(dimLog.y == wxRound(dimLogRef.m_y));

        // And next back from logical to device coordinates
        wxPoint posDev;
        posDev = dc->LogicalToDevice(posLog);
        CHECK(posDev.x == s_posDev.x);
        CHECK(posDev.y == s_posDev.y);

        wxSize dimDev;
        dimDev = dc->LogicalToDeviceRel(dimLog);
        CHECK(dimDev.x == s_dimDev.x);
        CHECK(dimDev.y == s_dimDev.y);
     }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void TransformedWithMatrixAndStd(wxDC* dc)
{
    // Apply combination of standard and matrix transformations
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc->CanUseTransformMatrix() )
    {
        dc->SetDeviceOrigin(10, 15);

        dc->SetUserScale(0.5, 1.5);
        dc->SetLogicalScale(4.0, 2.0);
        dc->SetLogicalOrigin(-15, -20);

        wxAffineMatrix2D m = dc->GetTransformMatrix();
        m.Translate(10, 18);
        m.Scale(2.0, 0.5);
        dc->SetTransformMatrix(m);

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
        posLog.x = dc->DeviceToLogicalX(s_posDev.x);
        posLog.y = dc->DeviceToLogicalY(s_posDev.y);
        CHECK_FALSE(posLog.x == wxRound(posLogRef.m_x));
        CHECK_FALSE(posLog.y == wxRound(posLogRef.m_y));

        wxPoint2DDouble dimLogRef = m1.TransformDistance(wxPoint2DDouble(s_dimDev.x, s_dimDev.y));
        wxSize dimLog;
        dimLog.x = dc->DeviceToLogicalXRel(s_dimDev.x);
        dimLog.y = dc->DeviceToLogicalYRel(s_dimDev.y);
        CHECK_FALSE(dimLog.x == wxRound(dimLogRef.m_x));
        CHECK_FALSE(dimLog.y == wxRound(dimLogRef.m_y));

        // And next back from logical to device coordinates
        wxPoint posDev;
        posDev.x = dc->LogicalToDeviceX(posLog.x);
        posDev.y = dc->LogicalToDeviceY(posLog.y);
        CHECK(posDev.x == s_posDev.x);
        CHECK(posDev.y == s_posDev.y);

        wxSize dimDev;
        dimDev.x = dc->LogicalToDeviceXRel(dimLog.x);
        dimDev.y = dc->LogicalToDeviceYRel(dimLog.y);
        CHECK(dimDev.x == s_dimDev.x);
        CHECK(dimDev.y == s_dimDev.y);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void TransformedWithMatrixAndStdEx(wxDC * dc)
{
    // Apply combination of standard and matrix transformations
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc->CanUseTransformMatrix() )
    {
        dc->SetDeviceOrigin(10, 15);

        dc->SetUserScale(0.5, 1.5);
        dc->SetLogicalScale(4.0, 2.0);
        dc->SetLogicalOrigin(-15, -20);

        wxAffineMatrix2D m = dc->GetTransformMatrix();
        m.Translate(10, 18);
        m.Scale(2.0, 0.5);
        dc->SetTransformMatrix(m);

        // First convert from device to logical coordinates
        wxAffineMatrix2D m1;
        m1.Translate(10 - (-15) * (0.5 * 4.0), 15 - (-20) * (1.5 * 2.0));
        m1.Scale(0.5 * 4.0, 1.5 * 2.0);
        m1.Concat(m);
        m1.Invert();

        wxPoint2DDouble posLogRef = m1.TransformPoint(wxPoint2DDouble(s_posDev));
        wxPoint posLog;
        posLog = dc->DeviceToLogical(s_posDev);

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

        CHECK(posLog.x == wxRound(posLogRef.m_x));
        CHECK(posLog.y == wxRound(posLogRef.m_y));

        wxPoint2DDouble dimLogRef = m1.TransformDistance(wxPoint2DDouble(s_dimDev.x, s_dimDev.y));
        wxSize dimLog;
        dimLog = dc->DeviceToLogicalRel(s_dimDev);
        CHECK(dimLog.x == wxRound(dimLogRef.m_x));
        CHECK(dimLog.y == wxRound(dimLogRef.m_y));

        // And next back from logical to device coordinates
        wxPoint posDev;
        posDev = dc->LogicalToDevice(posLog);
        CHECK(posDev.x == s_posDev.x);
        CHECK(posDev.y == s_posDev.y);

        wxSize dimDev;
        dimDev = dc->LogicalToDeviceRel(dimLog);
        CHECK(dimDev.x == s_dimDev.x);
        CHECK(dimDev.y == s_dimDev.y);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void RotatedWithMatrix(wxDC* dc)
{
    // Apply matrix transformations with rotation component
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc->CanUseTransformMatrix() )
    {
        wxAffineMatrix2D m = dc->GetTransformMatrix();
        m.Rotate(6 * M_PI / 180.0);
        m.Translate(10, 15);
        m.Scale(2.0, 3.0);
        dc->SetTransformMatrix(m);

        // First convert from device to logical coordinates
        // Results should be nagative because legacy functions
        // don't take affine transformation into account.
        m.Invert();
        wxPoint2DDouble posLogRef = m.TransformPoint(wxPoint2DDouble(s_posDev));
        wxPoint posLog;
        posLog.x = dc->DeviceToLogicalX(s_posDev.x);
        posLog.y = dc->DeviceToLogicalY(s_posDev.y);
        CHECK_FALSE(posLog.x == wxRound(posLogRef.m_x));
        CHECK_FALSE(posLog.y == wxRound(posLogRef.m_y));
        CHECK(posLog.x == s_posDev.x);
        CHECK(posLog.y == s_posDev.y);

        wxPoint2DDouble dimLogRef = m.TransformDistance(wxPoint2DDouble(s_dimDev.x, s_dimDev.y));
        wxSize dimLog;
        dimLog.x = dc->DeviceToLogicalXRel(s_dimDev.x);
        dimLog.y = dc->DeviceToLogicalYRel(s_dimDev.y);
        CHECK_FALSE(dimLog.x == wxRound(dimLogRef.m_x));
        CHECK_FALSE(dimLog.y == wxRound(dimLogRef.m_y));
        CHECK(dimLog.x == s_dimDev.x);
        CHECK(dimLog.y == s_dimDev.y);

        // And next back from logical to device coordinates
        wxPoint posDev;
        posDev.x = dc->LogicalToDeviceX(posLog.x);
        posDev.y = dc->LogicalToDeviceY(posLog.y);
        CHECK(posDev.x == s_posDev.x);
        CHECK(posDev.y == s_posDev.y);

        wxSize dimDev;
        dimDev.x = dc->LogicalToDeviceXRel(dimLog.x);
        dimDev.y = dc->LogicalToDeviceYRel(dimLog.y);
        CHECK(dimDev.x == s_dimDev.x);
        CHECK(dimDev.y == s_dimDev.y);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

static void RotatedWithMatrixEx(wxDC * dc)
{
    // Apply matrix transformations with rotation component
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc->CanUseTransformMatrix() )
    {
        wxAffineMatrix2D m = dc->GetTransformMatrix();
        m.Rotate(6 * M_PI / 180.0);
        m.Translate(10, 15);
        m.Scale(2.0, 3.0);
        dc->SetTransformMatrix(m);

        // First convert from device to logical coordinates
        m.Invert();
        wxPoint2DDouble posLogRef = m.TransformPoint(wxPoint2DDouble(s_posDev));
        wxPoint posLog;
        posLog = dc->DeviceToLogical(s_posDev);
        CHECK(posLog.x == wxRound(posLogRef.m_x));
        CHECK(posLog.y == wxRound(posLogRef.m_y));

        wxPoint2DDouble dimLogRef = m.TransformDistance(wxPoint2DDouble(s_dimDev.x, s_dimDev.y));
        wxSize dimLog;
        dimLog = dc->DeviceToLogicalRel(s_dimDev);
        CHECK(dimLog.x == wxRound(dimLogRef.m_x));
        CHECK(dimLog.y == wxRound(dimLogRef.m_y));

        // And next back from logical to device coordinates
        wxPoint posDev;
        posDev = dc->LogicalToDevice(posLog);
        CHECK(Approx(posDev.x).margin(1) == s_posDev.x);
        CHECK(Approx(posDev.y).margin(1) == s_posDev.y);

        wxSize dimDev;
        dimDev = dc->LogicalToDeviceRel(dimLog);
        CHECK(Approx(dimDev.x).margin(1) == s_dimDev.x);
        CHECK(Approx(dimDev.y).margin(1) == s_dimDev.y);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

// For GTK+ 3 and OSX wxDC is equivalent to wxGCDC
// so it doesn't need to be tested individually.
#if !defined(__WXGTK3__) && !defined(__WXOSX__)
TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::InitialState", "[coordinates]")
{
    // Check initial state
    InitialState(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::NoTransform", "[coordinates]")
{
    // No transformations
    NoTransform(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::NoTransformEx", "[coordinates]")
{
    // No transformations
    NoTransformEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::DeviceOriginChanged", "[coordinates]")
{
    // Only device origin is changed
    DeviceOriginChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::DeviceOriginChangedEx", "[coordinates]")
{
    // Only device origin is changed
    DeviceOriginChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::LogicalOriginChanged", "[coordinates]")
{
    // Only logical origin is changed
    LogicalOriginChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::LogicalOriginChangedEx", "[coordinates]")
{
    // Only logical origin is changed
    LogicalOriginChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::UserScaleChanged", "[coordinates]")
{
    // Only user scale is changed
    UserScaleChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::UserScaleChangedEx", "[coordinates]")
{
    // Only user scale is changed
    UserScaleChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::LogicalScaleChanged", "[coordinates]")
{
    // Only logical scale is changed
    LogicalScaleChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::LogicalScaleChangedEx", "[coordinates]")
{
    // Only logical scale is changed
    LogicalScaleChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::TransformedStd", "[coordinates]")
{
    // Apply all standardd transformations
    TransformedStd(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::TransformedStdEx", "[coordinates]")
{
    // Apply all standardd transformations
    TransformedStdEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::TransformedWithMatrix", "[coordinates]")
{
    // Apply transformation matrix only
    TransformedWithMatrix(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::TransformedWithMatrixEx", "[coordinates]")
{
    // Apply transformation matrix only
    TransformedWithMatrixEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::TransformedWithMatrixAndStd", "[coordinates]")
{
    // Apply combination of standard and matrix transformations
    TransformedWithMatrixAndStd(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::TransformedWithMatrixAndStdEx", "[coordinates]")
{
    // Apply combination of standard and matrix transformations
    TransformedWithMatrixAndStdEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::RotatedWithMatrix", "[coordinates]")
{
    // Apply matrix transformations with rotation component
    RotatedWithMatrix(m_dc);
}

TEST_CASE_METHOD(CoordinatesDCTestCase, "CoordinatesDC::RotatedWithMatrixEx", "[coordinates]")
{
    // Apply matrix transformations with rotation component
    RotatedWithMatrixEx(m_dc);
}
#endif // !__WXGTK3__ && !_WXOSX__

#if wxUSE_GRAPHICS_CONTEXT
// For MSW we have individual test cases for each graphics renderer
// so we don't need to test wxGCDC with default renderer.
#ifndef __WXMSW__
TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::InitialState", "[coordinates]")
{
    // Check initial state
    InitialState(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::NoTransform", "[coordinates]")
{
    // No transformations
    NoTransform(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::NoTransformEx", "[coordinates]")
{
    // No transformations
    NoTransformEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::DeviceOriginChanged", "[coordinates]")
{
    // Only device origin is changed
    DeviceOriginChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::DeviceOriginChangedEx", "[coordinates]")
{
    // Only device origin is changed
    DeviceOriginChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::LogicalOriginChanged", "[coordinates]")
{
    // Only logical origin is changed
    LogicalOriginChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::LogicalOriginChangedEx", "[coordinates]")
{
    // Only logical origin is changed
    LogicalOriginChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::UserScaleChanged", "[coordinates]")
{
    // Only user scale is changed
    UserScaleChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::UserScaleChangedEx", "[coordinates]")
{
    // Only user scale is changed
    UserScaleChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::LogicalScaleChanged", "[coordinates]")
{
    // Only logical scale is changed
    LogicalScaleChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::LogicalScaleChangedEx", "[coordinates]")
{
    // Only logical scale is changed
    LogicalScaleChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::TransformedStd", "[coordinates]")
{
    // Apply all standardd transformations
    TransformedStd(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::TransformedStdEx", "[coordinates]")
{
    // Apply all standardd transformations
    TransformedStdEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::TransformedWithMatrix", "[coordinates]")
{
    // Apply transformation matrix only
    TransformedWithMatrix(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::TransformedWithMatrixEx", "[coordinates]")
{
    // Apply transformation matrix only
    TransformedWithMatrixEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::TransformedWithMatrixAndStd", "[coordinates]")
{
    // Apply combination of standard and matrix transformations
    TransformedWithMatrixAndStd(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::TransformedWithMatrixAndStdEx", "[coordinates]")
{
    // Apply combination of standard and matrix transformations
    TransformedWithMatrixAndStdEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::RotatedWithMatrix", "[coordinates]")
{
    // Apply matrix transformations with rotation component
    RotatedWithMatrix(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCTestCase, "CoordinatesGCDC::RotatedWithMatrixEx", "[coordinates]")
{
    // Apply matrix transformations with rotation component
    RotatedWithMatrixEx(m_dc);
}
#else// GDI+ and Direct2D are available only under MSW.
#if wxUSE_GRAPHICS_GDIPLUS
class CoordinatesGCDCGDIPlusTestCase : public CoordinatesGCDCTestCase
{
public:
    CoordinatesGCDCGDIPlusTestCase()
    {
        wxGraphicsRenderer* rend = wxGraphicsRenderer::GetGDIPlusRenderer();
        wxGraphicsContext* ctx = rend->CreateContext(m_mdc);
        REQUIRE(ctx != nullptr);
        m_gcdc->SetGraphicsContext(ctx);
    }

    virtual ~CoordinatesGCDCGDIPlusTestCase() {}
};

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::InitialState", "[coordinates]")
{
    // Check initial state
    InitialState(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::NoTransform", "[coordinates]")
{
    // No transformations
    NoTransform(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::NoTransformEx", "[coordinates]")
{
    // No transformations
    NoTransformEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::DeviceOriginChanged", "[coordinates]")
{
    // Only device origin is changed
    DeviceOriginChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::DeviceOriginChangedEx", "[coordinates]")
{
    // Only device origin is changed
    DeviceOriginChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::LogicalOriginChanged", "[coordinates]")
{
    // Only logical origin is changed
    LogicalOriginChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::LogicalOriginChangedEx", "[coordinates]")
{
    // Only logical origin is changed
    LogicalOriginChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::UserScaleChanged", "[coordinates]")
{
    // Only user scale is changed
    UserScaleChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::UserScaleChangedEx", "[coordinates]")
{
    // Only user scale is changed
    UserScaleChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::LogicalScaleChanged", "[coordinates]")
{
    // Only logical scale is changed
    LogicalScaleChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::LogicalScaleChangedex", "[coordinates]")
{
    // Only logical scale is changed
    LogicalScaleChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::TransformedStd", "[coordinates]")
{
    // Apply all standardd transformations
    TransformedStd(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::TransformedStdEx", "[coordinates]")
{
    // Apply all standardd transformations
    TransformedStdEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::TransformedWithMatrix", "[coordinates]")
{
    // Apply transformation matrix only
    TransformedWithMatrix(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::TransformedWithMatrixEx", "[coordinates]")
{
    // Apply transformation matrix only
    TransformedWithMatrixEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::TransformedWithMatrixAndStd", "[coordinates]")
{
    // Apply combination of standard and matrix transformations
    TransformedWithMatrixAndStd(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::TransformedWithMatrixAndStdEx", "[coordinates]")
{
    // Apply combination of standard and matrix transformations
    TransformedWithMatrixAndStdEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::RotatedWithMatrix", "[coordinates]")
{
    // Apply matrix transformations with rotation component
    RotatedWithMatrix(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCGDIPlusTestCase, "CoordinatesGCDCGDIPlus::RotatedWithMatrixEx", "[coordinates]")
{
    // Apply matrix transformations with rotation component
    RotatedWithMatrixEx(m_dc);
}
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
class CoordinatesGCDCDirect2DTestCase : public CoordinatesGCDCTestCase
{
public:
    CoordinatesGCDCDirect2DTestCase()
    {
        wxGraphicsRenderer* rend = wxGraphicsRenderer::GetDirect2DRenderer();
        wxGraphicsContext* ctx = rend->CreateContext(m_mdc);
        REQUIRE(ctx != nullptr);
        m_gcdc->SetGraphicsContext(ctx);
    }

    virtual ~CoordinatesGCDCDirect2DTestCase() {}
};

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::InitialState", "[coordinates]")
{
    // Check initial state
    InitialState(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::NoTransform", "[coordinates]")
{
    // No transformations
    NoTransform(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::NoTransformEx", "[coordinates]")
{
    // No transformations
    NoTransformEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::DeviceOriginChanged", "[coordinates]")
{
    // Only device origin is changed
    DeviceOriginChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::DeviceOriginChangedEx", "[coordinates]")
{
    // Only device origin is changed
    DeviceOriginChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::LogicalOriginChanged", "[coordinates]")
{
    // Only logical origin is changed
    LogicalOriginChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::LogicalOriginChangedEx", "[coordinates]")
{
    // Only logical origin is changed
    LogicalOriginChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::UserScaleChanged", "[coordinates]")
{
    // Only user scale is changed
    UserScaleChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::UserScaleChangedEx", "[coordinates]")
{
    // Only user scale is changed
    UserScaleChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::LogicalScaleChanged", "[coordinates]")
{
    // Only logical scale is changed
    LogicalScaleChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::LogicalScaleChangedEx", "[coordinates]")
{
    // Only logical scale is changed
    LogicalScaleChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::TransformedStd", "[coordinates]")
{
    // Apply all standardd transformations
    TransformedStd(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::TransformedStdEx", "[coordinates]")
{
    // Apply all standardd transformations
    TransformedStdEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::TransformedWithMatrix", "[coordinates]")
{
    // Apply transformation matrix only
    TransformedWithMatrix(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::TransformedWithMatrixEx", "[coordinates]")
{
    // Apply transformation matrix only
    TransformedWithMatrixEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::TransformedWithMatrixAndStd", "[coordinates]")
{
    // Apply combination of standard and matrix transformations
    TransformedWithMatrixAndStd(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::TransformedWithMatrixAndStdEx", "[coordinates]")
{
    // Apply combination of standard and matrix transformations
    TransformedWithMatrixAndStdEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::RotatedWithMatrix", "[coordinates]")
{
    // Apply matrix transformations with rotation component
    RotatedWithMatrix(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCDirect2DTestCase, "CoordinatesGCDCDirect2D::RotatedWithMatrixEx", "[coordinates]")
{
    // Apply matrix transformations with rotation component
    RotatedWithMatrixEx(m_dc);
}
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__/!__WXMSW__

#if wxUSE_CAIRO
class CoordinatesGCDCCairoTestCase : public CoordinatesGCDCTestCase
{
public:
    CoordinatesGCDCCairoTestCase()
    {
        wxGraphicsRenderer* rend = wxGraphicsRenderer::GetCairoRenderer();
        wxGraphicsContext* ctx = rend->CreateContext(m_mdc);
        REQUIRE(ctx != nullptr);
        m_gcdc->SetGraphicsContext(ctx);
    }

    virtual ~CoordinatesGCDCCairoTestCase() {}
};

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::InitialState", "[coordinates]")
{
    // Check initial state
    InitialState(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::NoTransform", "[coordinates]")
{
    // No transformations
    NoTransform(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::NoTransformEx", "[coordinates]")
{
    // No transformations
    NoTransformEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::DeviceOriginChanged", "[coordinates]")
{
    // Only device origin is changed
    DeviceOriginChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::DeviceOriginChangedEx", "[coordinates]")
{
    // Only device origin is changed
    DeviceOriginChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::LogicalOriginChanged", "[coordinates]")
{
    // Only logical origin is changed
    LogicalOriginChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::LogicalOriginChangedEx", "[coordinates]")
{
    // Only logical origin is changed
    LogicalOriginChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::UserScaleChanged", "[coordinates]")
{
    // Only user scale is changed
    UserScaleChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::UserScaleChangedEx", "[coordinates]")
{
    // Only user scale is changed
    UserScaleChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::LogicalScaleChanged", "[coordinates]")
{
    // Only logical scale is changed
    LogicalScaleChanged(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::LogicalScaleChangedEx", "[coordinates]")
{
    // Only logical scale is changed
    LogicalScaleChangedEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::TransformedStd", "[coordinates]")
{
    // Apply all standardd transformations
    TransformedStd(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::TransformedStdEx", "[coordinates]")
{
    // Apply all standardd transformations
    TransformedStdEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::TransformedWithMatrix", "[coordinates]")
{
    // Apply transformation matrix only
    TransformedWithMatrix(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::TransformedWithMatrixEx", "[coordinates]")
{
    // Apply transformation matrix only
    TransformedWithMatrixEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::TransformedWithMatrixAndStd", "[coordinates]")
{
    // Apply combination of standard and matrix transformations
    TransformedWithMatrixAndStd(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::TransformedWithMatrixAndStdEx", "[coordinates]")
{
    // Apply combination of standard and matrix transformations
    TransformedWithMatrixAndStdEx(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::RotatedWithMatrix", "[coordinates]")
{
    // Apply matrix transformations with rotation component
    RotatedWithMatrix(m_dc);
}

TEST_CASE_METHOD(CoordinatesGCDCCairoTestCase, "CoordinatesGCDCCairo::RotatedWithMatrixEx", "[coordinates]")
{
    // Apply matrix transformations with rotation component
    RotatedWithMatrixEx(m_dc);
}
#endif // wxUSE_CAIRO
#endif // wxUSE_GRAPHICS_CONTEXT
