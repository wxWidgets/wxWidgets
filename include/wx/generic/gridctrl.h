///////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/gridctrl.h
// Purpose:     wxGrid controls
// Author:      Paul Gammans, Roger Gammans
// Created:     11/04/2001
// Copyright:   (c) The Computer Surgery (paul@compsurg.co.uk)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_GRIDCTRL_H_
#define _WX_GENERIC_GRIDCTRL_H_

#include "wx/grid.h"

#if wxUSE_GRID

#define wxGRID_VALUE_CHOICEINT    wxT("choiceint")
#define wxGRID_VALUE_DATETIME     wxT("datetime")


// the default renderer for the cells containing string data
class WXDLLIMPEXP_ADV wxGridCellStringRenderer : public wxGridCellRenderer
{
public:
    wxGridCellStringRenderer()
        : wxGridCellRenderer()
    {
    }

    wxGridCellStringRenderer(const wxGridCellStringRenderer& other)
        : wxGridCellRenderer(other)
    {
    }

    // draw the string
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected) override;

    // return the string extent
    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col) override;

    wxNODISCARD virtual wxGridCellRenderer *Clone() const override
        { return new wxGridCellStringRenderer(*this); }

protected:
    // calc the string extent for given string/font
    wxSize DoGetBestSize(const wxGridCellAttr& attr,
                         wxReadOnlyDC& dc,
                         const wxString& text);
};

// the default renderer for the cells containing numeric (long) data
class WXDLLIMPEXP_ADV wxGridCellNumberRenderer : public wxGridCellStringRenderer
{
public:
    explicit wxGridCellNumberRenderer(long minValue = LONG_MIN,
                                      long maxValue = LONG_MAX)
        : wxGridCellStringRenderer(),
          m_minValue(minValue),
          m_maxValue(maxValue)
    {
    }

    wxGridCellNumberRenderer(const wxGridCellNumberRenderer& other)
        : wxGridCellStringRenderer(other),
          m_minValue(other.m_minValue),
          m_maxValue(other.m_maxValue)
    {
    }

    // draw the string right aligned
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected) override;

    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col) override;

    virtual wxSize GetMaxBestSize(wxGrid& grid,
                                  wxGridCellAttr& attr,
                                  wxDC& dc) override;

    // Optional parameters for this renderer are "<min>,<max>".
    virtual void SetParameters(const wxString& params) override;

    wxNODISCARD virtual wxGridCellRenderer *Clone() const override
        { return new wxGridCellNumberRenderer(*this); }

protected:
    wxString GetString(const wxGrid& grid, int row, int col);

    long m_minValue,
         m_maxValue;
};

class WXDLLIMPEXP_ADV wxGridCellFloatRenderer : public wxGridCellStringRenderer
{
public:
    wxGridCellFloatRenderer(int width = -1,
                            int precision = -1,
                            int format = wxGRID_FLOAT_FORMAT_DEFAULT);

    wxGridCellFloatRenderer(const wxGridCellFloatRenderer& other)
        : wxGridCellStringRenderer(other),
          m_width(other.m_width),
          m_precision(other.m_precision),
          m_style(other.m_style),
          m_format(other.m_format)
    {
    }

    // get/change formatting parameters
    int GetWidth() const { return m_width; }
    void SetWidth(int width) { m_width = width; m_format.clear(); }
    int GetPrecision() const { return m_precision; }
    void SetPrecision(int precision) { m_precision = precision; m_format.clear(); }
    int GetFormat() const { return m_style; }
    void SetFormat(int format) { m_style = format; m_format.clear(); }

    // draw the string right aligned with given width/precision
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected) override;

    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col) override;

    // parameters string format is "width[,precision[,format]]"
    // with format being one of f|e|g|E|F|G
    virtual void SetParameters(const wxString& params) override;

    wxNODISCARD virtual wxGridCellRenderer *Clone() const override
        { return new wxGridCellFloatRenderer(*this); }

protected:
    wxString GetString(const wxGrid& grid, int row, int col);

private:
    // formatting parameters
    int m_width,
        m_precision;

    int m_style;
    wxString m_format;
};

// renderer for boolean fields
class WXDLLIMPEXP_ADV wxGridCellBoolRenderer : public wxGridCellRenderer
{
public:
    wxGridCellBoolRenderer()
        : wxGridCellRenderer()
    {
    }

    wxGridCellBoolRenderer(const wxGridCellBoolRenderer& other)
        : wxGridCellRenderer(other)
    {
    }

    // draw a check mark or nothing
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected) override;

    // return the checkmark size
    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col) override;

    virtual wxSize GetMaxBestSize(wxGrid& grid,
                                  wxGridCellAttr& attr,
                                  wxDC& dc) override;

    wxNODISCARD virtual wxGridCellRenderer *Clone() const override
        { return new wxGridCellBoolRenderer(*this); }
};


#if wxUSE_DATETIME

#include "wx/datetime.h"

namespace wxGridPrivate { class DateParseParams; }

// renderer for the cells containing dates only, without time component
class WXDLLIMPEXP_ADV wxGridCellDateRenderer : public wxGridCellStringRenderer
{
public:
    explicit wxGridCellDateRenderer(const wxString& outformat = wxString());

    wxGridCellDateRenderer(const wxGridCellDateRenderer& other)
        : wxGridCellStringRenderer(other),
          m_oformat(other.m_oformat),
          m_tz(other.m_tz)
    {
    }

    // draw the string right aligned
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected) override;

    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col) override;

    virtual wxSize GetMaxBestSize(wxGrid& grid,
                                  wxGridCellAttr& attr,
                                  wxDC& dc) override;

    wxNODISCARD virtual wxGridCellRenderer *Clone() const override
        { return new wxGridCellDateRenderer(*this); }

    // output strptime()-like format string
    virtual void SetParameters(const wxString& params) override;

protected:
    wxString GetString(const wxGrid& grid, int row, int col);

    // This is overridden in wxGridCellDateTimeRenderer which uses a separate
    // input format and forbids fallback to ParseDate().
    virtual void
    GetDateParseParams(wxGridPrivate::DateParseParams& params) const;

    wxString m_oformat;
    wxDateTime::TimeZone m_tz;
};

// the default renderer for the cells containing times and dates
class WXDLLIMPEXP_ADV wxGridCellDateTimeRenderer : public wxGridCellDateRenderer
{
public:
    wxGridCellDateTimeRenderer(const wxString& outformat = wxASCII_STR(wxDefaultDateTimeFormat),
                               const wxString& informat = wxASCII_STR(wxDefaultDateTimeFormat));

    wxGridCellDateTimeRenderer(const wxGridCellDateTimeRenderer& other)
        : wxGridCellDateRenderer(other),
          m_iformat(other.m_iformat)
    {
    }

    wxNODISCARD virtual wxGridCellRenderer *Clone() const override
        { return new wxGridCellDateTimeRenderer(*this); }

protected:
    virtual void
    GetDateParseParams(wxGridPrivate::DateParseParams& params) const override;

    wxString m_iformat;
};

#endif // wxUSE_DATETIME

// Renderer for fields taking one of a limited set of values: this is the same
// as the renderer for strings, except that it can implement GetMaxBestSize().
class WXDLLIMPEXP_ADV wxGridCellChoiceRenderer : public wxGridCellStringRenderer
{
public:
    explicit wxGridCellChoiceRenderer(const wxString& choices = wxString());

    wxGridCellChoiceRenderer(const wxGridCellChoiceRenderer& other);

    virtual wxSize GetMaxBestSize(wxGrid& grid,
                                  wxGridCellAttr& attr,
                                  wxDC& dc) override;

    // Parameters string is a comma-separated list of values.
    virtual void SetParameters(const wxString& params) override;

    wxNODISCARD virtual wxGridCellRenderer *Clone() const override
    {
        return new wxGridCellChoiceRenderer(*this);
    }

protected:

    wxArrayString m_choices;
};


// renders a number using the corresponding text string
class WXDLLIMPEXP_ADV wxGridCellEnumRenderer : public wxGridCellChoiceRenderer
{
public:
    explicit wxGridCellEnumRenderer(const wxString& choices = wxString())
        : wxGridCellChoiceRenderer(choices)
    {
    }

    wxGridCellEnumRenderer(const wxGridCellEnumRenderer& other)
        : wxGridCellChoiceRenderer(other)
    {
    }

    // draw the string right aligned
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected) override;

    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col) override;

    wxNODISCARD virtual wxGridCellRenderer *Clone() const override
        { return new wxGridCellEnumRenderer(*this); }

protected:
    wxString GetString(const wxGrid& grid, int row, int col);
};


class WXDLLIMPEXP_ADV wxGridCellAutoWrapStringRenderer : public wxGridCellStringRenderer
{
public:
    wxGridCellAutoWrapStringRenderer()
        : wxGridCellStringRenderer()
    {
    }

    wxGridCellAutoWrapStringRenderer(const wxGridCellAutoWrapStringRenderer& other)
        : wxGridCellStringRenderer(other)
    {
    }

    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected) override;

    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col) override;

    virtual int GetBestHeight(wxGrid& grid,
                              wxGridCellAttr& attr,
                              wxDC& dc,
                              int row, int col,
                              int width) override;

    virtual int GetBestWidth(wxGrid& grid,
                              wxGridCellAttr& attr,
                              wxDC& dc,
                              int row, int col,
                              int height) override;

    wxNODISCARD virtual wxGridCellRenderer *Clone() const override
        { return new wxGridCellAutoWrapStringRenderer(*this); }

private:
    wxArrayString GetTextLines( wxGrid& grid,
                                wxReadOnlyDC& dc,
                                const wxGridCellAttr& attr,
                                const wxRect& rect,
                                int row, int col);

    // Helper methods of GetTextLines()

    // Break a single logical line of text into several physical lines, all of
    // which are added to the lines array. The lines are broken at maxWidth and
    // the dc is used for measuring text extent only.
    void BreakLine(wxReadOnlyDC& dc,
                   const wxString& logicalLine,
                   wxCoord maxWidth,
                   wxArrayString& lines);

    // Break a word, which is supposed to be wider than maxWidth, into several
    // lines, which are added to lines array and the last, incomplete, of which
    // is returned in line output parameter.
    //
    // Returns the width of the last line.
    wxCoord BreakWord(wxReadOnlyDC& dc,
                      const wxString& word,
                      wxCoord maxWidth,
                      wxArrayString& lines,
                      wxString& line);


};

#endif  // wxUSE_GRID
#endif // _WX_GENERIC_GRIDCTRL_H_
