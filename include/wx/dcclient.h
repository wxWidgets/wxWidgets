/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dcclient.h
// Purpose:     wxClientDC base header
// Author:      Julian Smart
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCCLIENT_H_BASE_
#define _WX_DCCLIENT_H_BASE_

#include "wx/dc.h"

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowDC : public wxDC
{
public:
    wxWindowDC(wxWindow *win);

protected:
    wxWindowDC(wxDCImpl *impl) : wxDC(impl) { }

private:
    wxDECLARE_ABSTRACT_CLASS(wxWindowDC);
};

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxClientDC : public wxWindowDC
{
public:
    wxClientDC(wxWindow *win);

    static bool CanBeUsedForDrawing(const wxWindow* window);

protected:
    wxClientDC(wxDCImpl *impl) : wxWindowDC(impl) { }

private:
    wxDECLARE_ABSTRACT_CLASS(wxClientDC);
};

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPaintDC : public wxClientDC
{
public:
    wxPaintDC(wxWindow *win);

protected:
    wxPaintDC(wxDCImpl *impl) : wxClientDC(impl) { }

private:
    wxDECLARE_ABSTRACT_CLASS(wxPaintDC);
};

//-----------------------------------------------------------------------------
// wxInfoDC
//-----------------------------------------------------------------------------

// A very limited wxDC-like class which can be only used to query the device
// context, but not to draw anything on it: it doesn't inherit from wxDC but
// can be implicitly converted to wxReadOnlyDC and makes all of its functions
// available.
class WXDLLIMPEXP_CORE wxInfoDC
{
public:
    explicit wxInfoDC(wxWindow* win) : m_dcClient(win) { }

    // Implicit conversions to wxReadOnlyDC allow passing objects of this class
    // to the functions taking this type.
    operator wxReadOnlyDC&() { return m_dcClient; }
    operator const wxReadOnlyDC&() const { return m_dcClient; }

    wxReadOnlyDC* operator&() { return &m_dcClient; }
    const wxReadOnlyDC* operator&() const { return &m_dcClient; }

    // wxReadOnlyDC re-exposed here.
    wxWindow* GetWindow() const { return m_dcClient.GetWindow(); }
    bool IsOk() const { return m_dcClient.IsOk(); }

    bool CanDrawBitmap() const { return m_dcClient.CanDrawBitmap(); }
    bool CanGetTextExtent() const { return m_dcClient.CanGetTextExtent(); }

    void GetSize(int *width, int *height) const { m_dcClient.GetSize(width, height); }
    wxSize GetSize() const { return m_dcClient.GetSize(); }
    void GetSizeMM(int* width, int* height) const { m_dcClient.GetSizeMM(width, height); }
    wxSize GetSizeMM() const { return m_dcClient.GetSizeMM(); }

    int GetDepth() const { return m_dcClient.GetDepth(); }
    wxSize GetPPI() const { return m_dcClient.GetPPI(); }

    int GetResolution() const { return m_dcClient.GetResolution(); }
    double GetContentScaleFactor() const { return m_dcClient.GetContentScaleFactor(); }

    wxSize FromDIP(const wxSize& sz) const { return m_dcClient.FromDIP(sz); }
    wxPoint FromDIP(const wxPoint& pt) const { return m_dcClient.FromDIP(pt); }
    int FromDIP(int d) const { return m_dcClient.FromDIP(d); }

    wxSize ToDIP(const wxSize & sz) const { return m_dcClient.ToDIP(sz); }
    wxPoint ToDIP(const wxPoint & pt) const { return m_dcClient.ToDIP(pt); }
    int ToDIP(int d) const { return m_dcClient.ToDIP(d); }

    void SetLayoutDirection(wxLayoutDirection dir)
        { m_dcClient.SetLayoutDirection( dir ); }
    wxLayoutDirection GetLayoutDirection() const
        { return m_dcClient.GetLayoutDirection(); }

    void SetFont(const wxFont& font) { m_dcClient.SetFont(font); }
    const wxFont& GetFont() const { return m_dcClient.GetFont(); }

    wxCoord GetCharHeight() const { return m_dcClient.GetCharHeight(); }
    wxCoord GetCharWidth() const { return m_dcClient.GetCharWidth(); }

    wxFontMetrics GetFontMetrics() const { return m_dcClient.GetFontMetrics(); }
    void GetTextExtent(const wxString& string,
                       wxCoord *x, wxCoord *y,
                       wxCoord *descent = nullptr,
                       wxCoord *externalLeading = nullptr,
                       const wxFont *theFont = nullptr) const
        { return m_dcClient.GetTextExtent(string, x, y, descent, externalLeading, theFont); }

    wxSize GetTextExtent(const wxString& string) const
    {
        return m_dcClient.GetTextExtent(string);
    }

    void GetMultiLineTextExtent(const wxString& string,
                                        wxCoord *width,
                                        wxCoord *height,
                                        wxCoord *heightLine = nullptr,
                                        const wxFont *font = nullptr) const
        { return m_dcClient.GetMultiLineTextExtent(string, width, height, heightLine, font); }

    wxSize GetMultiLineTextExtent(const wxString& string) const
    {
        return m_dcClient.GetMultiLineTextExtent(string);
    }

    bool GetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
        { return m_dcClient.GetPartialTextExtents(text, widths); }

    wxCoord DeviceToLogicalX(wxCoord x) const
        { return m_dcClient.DeviceToLogicalX(x); }
    wxCoord DeviceToLogicalY(wxCoord y) const
        { return m_dcClient.DeviceToLogicalY(y); }
    wxCoord DeviceToLogicalXRel(wxCoord x) const
        { return m_dcClient.DeviceToLogicalXRel(x); }
    wxCoord DeviceToLogicalYRel(wxCoord y) const
        { return m_dcClient.DeviceToLogicalYRel(y); }
    wxPoint DeviceToLogical(const wxPoint& pt) const
        { return m_dcClient.DeviceToLogical(pt.x, pt.y); }
    wxPoint DeviceToLogical(wxCoord x, wxCoord y) const
        { return m_dcClient.DeviceToLogical(x, y); }
    wxSize DeviceToLogicalRel(const wxSize& dim) const
        { return m_dcClient.DeviceToLogicalRel(dim.x, dim.y); }
    wxSize DeviceToLogicalRel(int x, int y) const
        { return m_dcClient.DeviceToLogicalRel(x, y); }
    wxCoord LogicalToDeviceX(wxCoord x) const
        { return m_dcClient.LogicalToDeviceX(x); }
    wxCoord LogicalToDeviceY(wxCoord y) const
        { return m_dcClient.LogicalToDeviceY(y); }
    wxCoord LogicalToDeviceXRel(wxCoord x) const
        { return m_dcClient.LogicalToDeviceXRel(x); }
    wxCoord LogicalToDeviceYRel(wxCoord y) const
        { return m_dcClient.LogicalToDeviceYRel(y); }
    wxPoint LogicalToDevice(const wxPoint& pt) const
        { return m_dcClient.LogicalToDevice(pt.x, pt.y); }
    wxPoint LogicalToDevice(wxCoord x, wxCoord y) const
        { return m_dcClient.LogicalToDevice(x, y); }
    wxSize LogicalToDeviceRel(const wxSize& dim) const
        { return m_dcClient.LogicalToDeviceRel(dim.x, dim.y); }
    wxSize LogicalToDeviceRel(int x, int y) const
        { return m_dcClient.LogicalToDeviceRel(x, y); }

    void SetMapMode(wxMappingMode mode)
        { m_dcClient.SetMapMode(mode); }
    wxMappingMode GetMapMode() const
        { return m_dcClient.GetMapMode(); }

    void SetUserScale(double x, double y)
        { m_dcClient.SetUserScale(x,y); }
    void GetUserScale(double *x, double *y) const
        { m_dcClient.GetUserScale( x, y ); }

    void SetLogicalScale(double x, double y)
        { m_dcClient.SetLogicalScale( x, y ); }
    void GetLogicalScale(double *x, double *y) const
        { m_dcClient.GetLogicalScale( x, y ); }

    void SetLogicalOrigin(wxCoord x, wxCoord y)
        { m_dcClient.SetLogicalOrigin(x,y); }
    void GetLogicalOrigin(wxCoord *x, wxCoord *y) const
        { m_dcClient.GetLogicalOrigin(x, y); }
    wxPoint GetLogicalOrigin() const
        { return m_dcClient.GetLogicalOrigin(); }

    void SetDeviceOrigin(wxCoord x, wxCoord y)
        { m_dcClient.SetDeviceOrigin( x, y); }
    void GetDeviceOrigin(wxCoord *x, wxCoord *y) const
        { m_dcClient.GetDeviceOrigin(x, y); }
    wxPoint GetDeviceOrigin() const
        { return m_dcClient.GetDeviceOrigin(); }

    void SetAxisOrientation(bool xLeftRight, bool yBottomUp)
        { m_dcClient.SetAxisOrientation(xLeftRight, yBottomUp); }

#if wxUSE_DC_TRANSFORM_MATRIX
    bool CanUseTransformMatrix() const
        { return m_dcClient.CanUseTransformMatrix(); }

    bool SetTransformMatrix(const wxAffineMatrix2D &matrix)
        { return m_dcClient.SetTransformMatrix(matrix); }

    wxAffineMatrix2D GetTransformMatrix() const
        { return m_dcClient.GetTransformMatrix(); }

    void ResetTransformMatrix()
        { m_dcClient.ResetTransformMatrix(); }
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // mostly internal
    void SetDeviceLocalOrigin( wxCoord x, wxCoord y )
        { m_dcClient.SetDeviceLocalOrigin( x, y ); }
private:
    wxClientDC m_dcClient;
};

// Define this to indicate that wxInfoDC is available to allow application code
// to fall back to using wxClientDC if it isn't.
#define wxHAS_INFO_DC

#endif // _WX_DCCLIENT_H_BASE_
