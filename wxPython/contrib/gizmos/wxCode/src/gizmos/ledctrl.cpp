// ============================================================================
// headers
// ============================================================================

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif  //__BORLANDC__

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/intl.h"
#endif

#include "wx/gizmos/ledctrl.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// A LED digit is build up like this, with maximum 7 Lines :
//
// 111
// 6 2
// 777
// 5 3
// 444
//
// Each number contains combinations of the lines, and they are set up below.

const int LINE1 = 1;
const int LINE2 = 2;
const int LINE3 = 4;
const int LINE4 = 8;
const int LINE5 = 16;
const int LINE6 = 32;
const int LINE7 = 64;
const int DECIMALSIGN = 128;

const int DIGIT0 = LINE1 | LINE2 | LINE3 | LINE4 | LINE5 | LINE6;
const int DIGIT1 = LINE2 | LINE3;
const int DIGIT2 = LINE1 | LINE2 | LINE4 | LINE5 | LINE7;
const int DIGIT3 = LINE1 | LINE2 | LINE3 | LINE4 | LINE7;
const int DIGIT4 = LINE2 | LINE3 | LINE6 | LINE7;
const int DIGIT5 = LINE1 | LINE3 | LINE4 | LINE6 | LINE7;
const int DIGIT6 = LINE1 | LINE3 | LINE4 | LINE5 | LINE6 | LINE7;
const int DIGIT7 = LINE1 | LINE2 | LINE3;
const int DIGIT8 = LINE1 | LINE2 | LINE3 | LINE4 | LINE5 | LINE6 | LINE7;
const int DIGIT9 = LINE1 | LINE2 | LINE3 | LINE6 | LINE7;
const int DASH   = LINE7;

const int DIGITALL = -1;

// ============================================================================
// wxLEDNumberCtrl class implementation
// ============================================================================

wxLEDNumberCtrl::wxLEDNumberCtrl()
:   m_Alignment(wxLED_ALIGN_LEFT),
    m_LineMargin(-1),
    m_DigitMargin(-1),
    m_LineLength(-1),
    m_LineWidth(-1),
    m_DrawFaded(false),
    m_LeftStartPos(-1)
{
}


wxLEDNumberCtrl::wxLEDNumberCtrl(wxWindow *parent, wxWindowID id,
                                 const wxPoint& pos, const wxSize& size,
                                 long style)
:   m_Alignment(wxLED_ALIGN_LEFT),
    m_LineMargin(-1),
    m_DigitMargin(-1),
    m_LineLength(-1),
    m_LineWidth(-1),
    m_DrawFaded(false),
    m_LeftStartPos(-1)
{
    Create(parent, id, pos, size, style);
}


bool wxLEDNumberCtrl::Create(wxWindow *parent, wxWindowID id,
                                 const wxPoint& pos, const wxSize& size,
                                 long style)
{
    bool RetVal = wxControl::Create(parent, id, pos, size, style);

    if ((style & wxLED_DRAW_FADED) != 0)
        SetDrawFaded(true);
    if ((style & wxLED_ALIGN_MASK) != 0)
        SetAlignment((wxLEDValueAlign)(style & wxLED_ALIGN_MASK));

    SetBackgroundColour(*wxBLACK);
    SetForegroundColour(*wxGREEN);

    return RetVal;
}


void wxLEDNumberCtrl::SetAlignment(wxLEDValueAlign Alignment, bool Redraw)
{
    if (Alignment != m_Alignment)
    {
        m_Alignment = Alignment;
        RecalcInternals(GetClientSize());

        if (Redraw)
            Refresh(false);
    }
}


void wxLEDNumberCtrl::SetDrawFaded(bool DrawFaded, bool Redraw)
{
    if (DrawFaded != m_DrawFaded)
    {
        m_DrawFaded = DrawFaded;

        if (Redraw)
            Refresh(false);
    }
}


void wxLEDNumberCtrl::SetValue(wxString const &Value, bool Redraw)
{
    if (Value != m_Value)
    {
#ifdef __WXDEBUG__
        if (!Value.empty())
        {
            for(size_t i=0; i<Value.Length(); i++) {
                wxChar ch = Value[i];
                wxASSERT_MSG((ch>='0' && ch<='9') || ch=='-' || ch==' ' || ch=='.',
                             wxT("wxLEDNumberCtrl can only display numeric string values."));
            }
        }
#endif

        m_Value = Value;
        RecalcInternals(GetClientSize());

        if (Redraw)
            Refresh(false);
    }
}


BEGIN_EVENT_TABLE(wxLEDNumberCtrl, wxControl)
    EVT_ERASE_BACKGROUND(wxLEDNumberCtrl::OnEraseBackground)
    EVT_PAINT(wxLEDNumberCtrl::OnPaint)
    EVT_SIZE(wxLEDNumberCtrl::OnSize)
END_EVENT_TABLE()


void wxLEDNumberCtrl::OnEraseBackground(wxEraseEvent &WXUNUSED(event))
{
}


void wxLEDNumberCtrl::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC Dc(this);

    int Width, Height;
    GetClientSize(&Width, &Height);

    wxBitmap *pMemoryBitmap = new wxBitmap(Width, Height);
    wxMemoryDC MemDc;

    MemDc.SelectObject(*pMemoryBitmap);

    // Draw background.
    MemDc.SetBrush(wxBrush(GetBackgroundColour(), wxSOLID));
    MemDc.DrawRectangle(wxRect(0, 0, Width, Height));
    MemDc.SetBrush(wxNullBrush);

    // Iterate each digit in the value, and draw.
    const int DigitCount = m_Value.Len();
    for (int offset=0, i = 0; offset < DigitCount; ++offset, ++i)
    {
        wxChar c = m_Value.GetChar(offset);

        // Draw faded lines if wanted.
        if (m_DrawFaded && (c != _T('.')))
            DrawDigit(MemDc, DIGITALL, i);

        // Draw the digits.
        switch (c)
        {
            case _T('0') :
                DrawDigit(MemDc, DIGIT0, i);
                break;
            case _T('1') :
                DrawDigit(MemDc, DIGIT1, i);
                break;
            case _T('2') :
                DrawDigit(MemDc, DIGIT2, i);
                break;
            case _T('3') :
                DrawDigit(MemDc, DIGIT3, i);
                break;
            case _T('4') :
                DrawDigit(MemDc, DIGIT4, i);
                break;
            case _T('5') :
                DrawDigit(MemDc, DIGIT5, i);
                break;
            case _T('6') :
                DrawDigit(MemDc, DIGIT6, i);
                break;
            case _T('7') :
                DrawDigit(MemDc, DIGIT7, i);
                break;
            case _T('8') :
                DrawDigit(MemDc, DIGIT8, i);
                break;
            case _T('9') :
                DrawDigit(MemDc, DIGIT9, i);
                break;
            case _T('-') :
                DrawDigit(MemDc, DASH, i);
                break;
            case _T('.') :
                // Display the decimal in the previous segment
                i--;
                DrawDigit(MemDc, DECIMALSIGN, i);
                break;
            case _T(' ') :
                // just skip it
                break;
            default :
                wxFAIL_MSG(wxT("Unknown digit value"));
                break;
        }
    }

    // Blit the memory dc to screen.
    Dc.Blit(0, 0, Width, Height, &MemDc, 0, 0, wxCOPY);
    delete pMemoryBitmap;
}


void wxLEDNumberCtrl::DrawDigit(wxDC &Dc, int Digit, int Column)
{
    wxColour LineColor(GetForegroundColour());

    if (Digit == DIGITALL)
    {
        const unsigned char R = (unsigned char)(LineColor.Red() / 16);
        const unsigned char G = (unsigned char)(LineColor.Green() / 16);
        const unsigned char B = (unsigned char)(LineColor.Blue() / 16);

        LineColor.Set(R, G, B);
    }

    int XPos = m_LeftStartPos + Column * (m_LineLength + m_DigitMargin);

    // Create a pen and draw the lines.
    wxPen Pen(LineColor, m_LineWidth, wxSOLID);
    Dc.SetPen(Pen);

    if ((Digit & LINE1))
    {
        Dc.DrawLine(XPos + m_LineMargin*2, m_LineMargin,
            XPos + m_LineLength + m_LineMargin*2, m_LineMargin);
    }

    if (Digit & LINE2)
    {
        Dc.DrawLine(XPos + m_LineLength + m_LineMargin*3, m_LineMargin*2,
            XPos + m_LineLength + m_LineMargin*3, m_LineLength + (m_LineMargin*2));
    }

    if (Digit & LINE3)
    {
        Dc.DrawLine(XPos + m_LineLength + m_LineMargin*3, m_LineLength + (m_LineMargin*4),
            XPos + m_LineLength + m_LineMargin*3, m_LineLength*2 + (m_LineMargin*4));
    }

    if (Digit & LINE4)
    {
        Dc.DrawLine(XPos + m_LineMargin*2, m_LineLength*2 + (m_LineMargin*5),
            XPos + m_LineLength + m_LineMargin*2, m_LineLength*2 + (m_LineMargin*5));
    }

    if (Digit & LINE5)
    {
        Dc.DrawLine(XPos + m_LineMargin, m_LineLength + (m_LineMargin*4),
            XPos + m_LineMargin, m_LineLength*2 + (m_LineMargin*4));
    }

    if (Digit & LINE6)
    {
        Dc.DrawLine(XPos + m_LineMargin, m_LineMargin*2,
            XPos + m_LineMargin, m_LineLength + (m_LineMargin*2));
    }

    if (Digit & LINE7)
    {
        Dc.DrawLine(XPos + m_LineMargin*2, m_LineLength + (m_LineMargin*3),
            XPos + m_LineMargin*2 + m_LineLength, m_LineLength + (m_LineMargin*3));
    }

    if (Digit & DECIMALSIGN)
    {
        Dc.DrawLine(XPos + m_LineLength + m_LineMargin*4, m_LineLength*2 + (m_LineMargin*5),
            XPos + m_LineLength + m_LineMargin*4, m_LineLength*2 + (m_LineMargin*5));
    }

    Dc.SetPen(wxNullPen);
}


void wxLEDNumberCtrl::RecalcInternals(const wxSize &CurrentSize)
{
    // Dimensions of LED segments
    //
    // Size of character is based on the HEIGH of the widget, NOT the width.
    // Segment height is calculated as follows:
    // Each segment is m_LineLength pixels long.
    // There is m_LineMargin pixels at the top and bottom of each line segment
    // There is m_LineMargin pixels at the top and bottom of each digit
    //
    //  Therefore, the heigth of each character is:
    //  m_LineMargin                            : Top digit boarder
    //  m_LineMargin+m_LineLength+m_LineMargin  : Top half of segment
    //  m_LineMargin+m_LineLength+m_LineMargin  : Bottom half of segment
    //  m_LineMargin                            : Bottom digit boarder
    //  ----------------------
    //  m_LineMargin*6 + m_LineLength*2 == Total height of digit.
    //  Therefore, (m_LineMargin*6 + m_LineLength*2) must equal Height
    //
    //  Spacing between characters can then be calculated as follows:
    //  m_LineMargin                            : before the digit,
    //  m_LineMargin+m_LineLength+m_LineMargin  : for the digit width
    //  m_LineMargin                            : after the digit
    //  = m_LineMargin*4 + m_LineLength
    const int Height = CurrentSize.GetHeight();

    if ((Height * 0.075) < 1)
        m_LineMargin = 1;
    else
        m_LineMargin = (int)(Height * 0.075);

    if ((Height * 0.275) < 1)
        m_LineLength = 1;
    else
        m_LineLength = (int)(Height * 0.275);

    m_LineWidth = m_LineMargin;

    m_DigitMargin = m_LineMargin * 4;

    // Count the number of characters in the string; '.' characters are not
    // included because they do not take up space in the display
    int count = 0;
    for (unsigned int i = 0; i < m_Value.Len(); i++)
        if (m_Value.GetChar(i) != '.')
            count++;
    const int ValueWidth = (m_LineLength + m_DigitMargin) * count;
    const int ClientWidth = CurrentSize.GetWidth();

    switch (m_Alignment)
    {
        case wxLED_ALIGN_LEFT :
            m_LeftStartPos = m_LineMargin;
            break;
        case wxLED_ALIGN_RIGHT :
            m_LeftStartPos = ClientWidth - ValueWidth - m_LineMargin;
            break;
        case wxLED_ALIGN_CENTER :
            m_LeftStartPos = (ClientWidth - ValueWidth) / 2;
            break;
        default :
            wxFAIL_MSG(wxT("Unknown alignent value for wxLEDNumberCtrl."));
            break;
    }
}


void wxLEDNumberCtrl::OnSize(wxSizeEvent &Event)
{
    RecalcInternals(Event.GetSize());

    Event.Skip();
}
