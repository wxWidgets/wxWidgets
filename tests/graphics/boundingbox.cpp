///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/boundingbox.cpp
// Purpose:     wxGCDC bounding box unit tests
// Author:      Vadim Zeitlin / Maarten Spoek / Toni Ruža
// Created:     2011-01-36
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
//              (c) 2014 Toni Ruža <toni.ruza@gmail.com>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#if wxUSE_GRAPHICS_CONTEXT

#include "wx/bitmap.h"
#include "wx/dcmemory.h"
#include "wx/dcgraph.h"
#include "wx/icon.h"
#include "wx/colour.h"
#include "wx/gdicmn.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class GCDCBoundingBoxTestCase
{
public:
    GCDCBoundingBoxTestCase()
    {
        m_bmp.Create(100, 100);
        m_dc.SelectObject(m_bmp);
        m_gcdc = wxGCDC(m_dc);
        m_gcdc.ResetBoundingBox();
    }

    ~GCDCBoundingBoxTestCase()
    {
        m_dc.SelectObject(wxNullBitmap);
        m_bmp = wxNullBitmap;
    }

protected:
    wxBitmap m_bmp;
    wxMemoryDC m_dc;

    wxGCDC m_gcdc;

    void AssertBox(int minX, int minY, int width, int height, int margin = 0)
    {
        int maxX = minX + width;
        int maxY = minY + height;

        // Allow for a margin of error due to different implementation
        // specificities regarding drawing paths.
        if ( margin )
        {
            #define WX_ASSERT_CLOSE(expected, actual, delta) \
                CHECK(actual == Approx(expected).margin(delta))

            WX_ASSERT_CLOSE(minX, m_gcdc.MinX(), margin);
            WX_ASSERT_CLOSE(minY, m_gcdc.MinY(), margin);
            WX_ASSERT_CLOSE(maxX, m_gcdc.MaxX(), margin);
            WX_ASSERT_CLOSE(maxY, m_gcdc.MaxY(), margin);

            #undef WX_ASSERT_CLOSE
        }
        else
        {
            CHECK(m_gcdc.MinX() == minX);
            CHECK(m_gcdc.MinY() == minY);
            CHECK(m_gcdc.MaxX() == maxX);
            CHECK(m_gcdc.MaxY() == maxY);
        }
    }

    wxDECLARE_NO_COPY_CLASS(GCDCBoundingBoxTestCase);
};

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawBitmap", "[boundingbox]")
{
    wxBitmap bitmap;
    bitmap.Create(12, 12);

    m_gcdc.DrawBitmap(bitmap, 5, 5);
    AssertBox(5, 5, 12, 12);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawIcon", "[boundingbox]")
{
    wxBitmap bitmap;
    bitmap.Create(16, 16);
    wxIcon icon;
    icon.CopyFromBitmap(bitmap);

    m_gcdc.DrawIcon(icon, 42, 42);
    AssertBox(42, 42, 16, 16);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawLine", "[boundingbox]")
{
    m_gcdc.DrawLine(10, 10, 20, 15);
    AssertBox(10, 10, 10, 5);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::CrossHair", "[boundingbox]")
{
    int w, h;
    m_gcdc.GetSize(&w, &h);

    m_gcdc.CrossHair(33, 33);
    AssertBox(0, 0, w, h);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawArc", "[boundingbox]")
{
    m_gcdc.DrawArc(25, 30, 15, 40, 25, 40);  // quarter circle
    AssertBox(15, 30, 10, 10, 3);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawEllipticArc", "[boundingbox]")
{
    m_gcdc.DrawEllipticArc(40, 50, 30, 20, 0, 180);  // half circle
    AssertBox(40, 50, 30, 10, 3);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawPoint", "[boundingbox]")
{
    m_gcdc.DrawPoint(20, 20);
    AssertBox(20, 20, 0, 0);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawLines", "[boundingbox]")
{
    wxPoint points[4];
    points[0] = wxPoint(10, 20);
    points[1] = wxPoint(20, 10);
    points[2] = wxPoint(30, 20);
    points[3] = wxPoint(20, 30);

    m_gcdc.DrawLines(4, points, 7, 8);
    AssertBox(17, 18, 20, 20);
}

#if wxUSE_SPLINES
TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawSpline", "[boundingbox]")
{
    wxPoint points[3];
    points[0] = wxPoint(10, 30);
    points[1] = wxPoint(20, 20);
    points[2] = wxPoint(40, 50);

    m_gcdc.DrawSpline(3, points);
    AssertBox(10, 20, 30, 30, 5);
}
#endif  // wxUSE_SPLINES

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawPolygon", "[boundingbox]")
{
    wxPoint points[3];
    points[0] = wxPoint(10, 30);
    points[1] = wxPoint(20, 10);
    points[2] = wxPoint(30, 30);

    m_gcdc.DrawPolygon(3, points, -5, -7);
    AssertBox(5, 3, 20, 20);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawPolyPolygon", "[boundingbox]")
{
    int lenghts[2];
    lenghts[0] = 3;
    lenghts[1] = 3;
    wxPoint points[6];
    points[0] = wxPoint(10, 30);
    points[1] = wxPoint(20, 10);
    points[2] = wxPoint(30, 30);
    points[3] = wxPoint(20, 60);
    points[4] = wxPoint(30, 40);
    points[5] = wxPoint(40, 60);

    m_gcdc.DrawPolyPolygon(2, lenghts, points, 12, 5);
    AssertBox(22, 15, 30, 50, 4);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawRectangle", "[boundingbox]")
{
    m_gcdc.DrawRectangle(2, 2, 12, 12);
    AssertBox(2, 2, 12, 12);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawRoundedRectangle", "[boundingbox]")
{
    m_gcdc.DrawRoundedRectangle(27, 27, 12, 12, 2);
    AssertBox(27, 27, 12, 12);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawEllipse", "[boundingbox]")
{
    m_gcdc.DrawEllipse(54, 45, 23, 12);
    AssertBox(54, 45, 23, 12);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::Blit", "[boundingbox]")
{
    wxBitmap bitmap;
    bitmap.Create(20, 20);
    wxMemoryDC dc(bitmap);

    m_gcdc.Blit(20, 10, 12, 7, &dc, 0, 0);
    AssertBox(20, 10, 12, 7);

    dc.SelectObject(wxNullBitmap);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::StretchBlit", "[boundingbox]")
{
    wxBitmap bitmap;
    bitmap.Create(20, 20);
    wxMemoryDC dc(bitmap);

    m_gcdc.StretchBlit(30, 50, 5, 5, &dc, 0, 0, 12, 4);
    AssertBox(30, 50, 5, 5);

    dc.SelectObject(wxNullBitmap);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawRotatedText", "[boundingbox]")
{
    wxString text("vertical");
    wxCoord w, h;
    m_gcdc.GetTextExtent(text, &w, &h);

    m_gcdc.DrawRotatedText(text, 43, 22, -90);
    AssertBox(43 - h, 22, h, w, 3);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawText", "[boundingbox]")
{
    wxString text("H");
    wxCoord w, h;
    m_gcdc.GetTextExtent(text, &w, &h);

    m_gcdc.DrawText(text, 3, 3);
    AssertBox(3, 3, w, h, 3);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::GradientFillLinear", "[boundingbox]")
{
    wxRect rect(16, 16, 30, 40);
    m_gcdc.GradientFillLinear(rect, *wxWHITE, *wxBLACK, wxNORTH);
    AssertBox(16, 16, 30, 40);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::GradientFillConcentric", "[boundingbox]")
{
    wxRect rect(6, 6, 30, 40);
    m_gcdc.GradientFillConcentric(rect, *wxWHITE, *wxBLACK, wxPoint(10, 10));
    AssertBox(6, 6, 30, 40);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawCheckMark", "[boundingbox]")
{
    m_gcdc.DrawCheckMark(32, 24, 16, 16);
    AssertBox(32, 24, 16, 16);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawRectangleAndReset", "[boundingbox]")
{
    m_gcdc.DrawRectangle(2, 2, 12, 12);
    m_gcdc.ResetBoundingBox();
    AssertBox(0, 0, 0, 0);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawTwoRectangles", "[boundingbox]")
{
    m_gcdc.DrawRectangle(10, 15, 50, 30);
    m_gcdc.DrawRectangle(15, 20, 55, 35);
    AssertBox(10, 15, 60, 40);
}

TEST_CASE_METHOD(GCDCBoundingBoxTestCase, "BoundingBox::DrawRectsOnTransformedDC", "[boundingbox]")
{
    m_gcdc.DrawRectangle(10, 15, 50, 30);
    m_gcdc.SetDeviceOrigin(15, 20);
    m_gcdc.DrawRectangle(15, 20, 45, 35);
    m_gcdc.SetDeviceOrigin(5, 10);
    AssertBox(5, 5, 65, 60);
}

#endif // wxUSE_GRAPHICS_CONTEXT
