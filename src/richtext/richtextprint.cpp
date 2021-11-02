/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextprint.cpp
// Purpose:     Rich text printing classes
// Author:      Julian Smart
// Created:     2006-10-24
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if wxUSE_RICHTEXT && wxUSE_PRINTING_ARCHITECTURE && wxUSE_STREAMS

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/dc.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
#endif

#include "wx/datetime.h"
#include "wx/print.h"
#include "wx/printdlg.h"
#include "wx/richtext/richtextprint.h"
#include "wx/wfstream.h"

/*!
 * wxRichTextPrintout
 */

wxRichTextPrintout::wxRichTextPrintout(const wxString& title) : wxPrintout(title)
{
    m_numPages = wxRICHTEXT_PRINT_MAX_PAGES;

    SetMargins(); // to default values
}

wxRichTextPrintout::~wxRichTextPrintout()
{
}

void wxRichTextPrintout::OnPreparePrinting()
{
    wxBusyCursor wait;

    m_numPages = 1;

    m_pageBreaksStart.Clear();
    m_pageBreaksEnd.Clear();
    m_pageYOffsets.Clear();

    int lastStartPos = 0;

    wxRect rect, headerRect, footerRect;

    /// Sets the DC scaling and returns important page rectangles
    CalculateScaling(GetDC(), rect, headerRect, footerRect);

    if (GetRichTextBuffer())
    {
        GetRichTextBuffer()->Invalidate(wxRICHTEXT_ALL);

        wxRichTextDrawingContext context(GetRichTextBuffer());
        GetRichTextBuffer()->Layout(*GetDC(), context, rect, rect, wxRICHTEXT_FIXED_WIDTH|wxRICHTEXT_VARIABLE_HEIGHT);

        // Now calculate the page breaks

        int yOffset = 0;

        wxRichTextLine* lastLine = NULL;

        wxRichTextObjectList::compatibility_iterator node = GetRichTextBuffer()->GetChildren().GetFirst();
        while (node)
        {
            // child is a paragraph
            wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
            wxASSERT (child != NULL);
            if (child)
            {
                wxRichTextLineVector::const_iterator it = child->GetLines().begin();
                while (it != child->GetLines().end())
                {
                    wxRichTextLine* line = *it;

                    int lineY = child->GetPosition().y + line->GetPosition().y - yOffset;
                    bool hasHardPageBreak = ((it == child->GetLines().begin()) && child->GetAttributes().HasPageBreak());

                    // Break the page if either we're going off the bottom, or this paragraph specifies
                    // an explicit page break

                    if (((lineY + line->GetSize().y) > rect.GetBottom()) || hasHardPageBreak)
                    {
                        // Only if we're not at the start of the document, and
                        // even then, only if either it's a hard break or if the object
                        // can fit in a whole page (otherwise there's no point in making
                        // the rest of this page blank).
                        if (lastLine && (hasHardPageBreak || (line->GetSize().y <= rect.GetHeight())))
                        {
                            // New page starting at this line
                            int newY = rect.y;

                            // We increase the offset by the difference between new and old positions

                            int increaseOffsetBy = lineY - newY;
                            yOffset += increaseOffsetBy;

                            m_pageBreaksStart.Add(lastStartPos);
                            m_pageBreaksEnd.Add(lastLine->GetAbsoluteRange().GetEnd());
                            m_pageYOffsets.Add(yOffset);

                            lastStartPos = line->GetAbsoluteRange().GetStart();
                            m_numPages ++;
                        }

                        lastLine = line;

                        // Now create page breaks for the rest of the line, if it's larger than the page height
                        int contentLeft = line->GetSize().y - rect.GetHeight();
                        while (contentLeft >= 0)
                        {
                            yOffset += rect.GetHeight();
                            contentLeft -= rect.GetHeight();

                            m_pageBreaksStart.Add(lastStartPos);
                            m_pageBreaksEnd.Add(lastLine->GetAbsoluteRange().GetEnd());
                            m_pageYOffsets.Add(yOffset);

                            m_numPages ++;
                        }
                    }

                    lastLine = line;

                    ++it;
                }
            }

            node = node->GetNext();
        }

        // Closing page break
        m_pageBreaksStart.Add(lastStartPos);
        m_pageBreaksEnd.Add(GetRichTextBuffer()->GetOwnRange().GetEnd());
        m_pageYOffsets.Add(yOffset);
    }
}

bool wxRichTextPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage)) return false;

    return true;
}

bool wxRichTextPrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc)
    {
        if (HasPage(page))
            RenderPage(dc, page);
        return true;
    }
    else return false;
}

void wxRichTextPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    *maxPage = m_numPages;
    *selPageFrom = 1;
    *selPageTo = m_numPages;
}

bool wxRichTextPrintout::HasPage(int pageNum)
{
    return pageNum > 0 && pageNum <= m_numPages;
}

void wxRichTextPrintout::RenderPage(wxDC *dc, int page)
{
    if (!GetRichTextBuffer())
        return;

    wxBusyCursor wait;

    wxRect textRect, headerRect, footerRect;

    /// Sets the DC scaling and returns important page rectangles
    CalculateScaling(dc, textRect, headerRect, footerRect);

    if (page > 1 || m_headerFooterData.GetShowOnFirstPage())
    {
        if (m_headerFooterData.GetFont().IsOk())
            dc->SetFont(m_headerFooterData.GetFont());
        else
            dc->SetFont(*wxNORMAL_FONT);
        if (m_headerFooterData.GetTextColour().IsOk())
            dc->SetTextForeground(m_headerFooterData.GetTextColour());
        else
            dc->SetTextForeground(*wxBLACK);
        dc->SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

        // Draw header, if any
        wxRichTextOddEvenPage oddEven = ((page % 2) == 1) ? wxRICHTEXT_PAGE_ODD : wxRICHTEXT_PAGE_EVEN;

        wxString headerTextCentre = m_headerFooterData.GetHeaderText(oddEven, wxRICHTEXT_PAGE_CENTRE);
        wxString headerTextLeft = m_headerFooterData.GetHeaderText(oddEven, wxRICHTEXT_PAGE_LEFT);
        wxString headerTextRight = m_headerFooterData.GetHeaderText(oddEven, wxRICHTEXT_PAGE_RIGHT);

        if (!headerTextLeft.IsEmpty())
        {
            SubstituteKeywords(headerTextLeft, GetTitle(), page, m_numPages);

            //int tx, ty;
            //dc->GetTextExtent(headerTextLeft, & tx, & ty);

            int x = headerRect.GetLeft();
            int y = headerRect.GetX();
            dc->DrawText(headerTextLeft, x, y);
        }
        if (!headerTextCentre.IsEmpty())
        {
            SubstituteKeywords(headerTextCentre, GetTitle(), page, m_numPages);

            int tx, ty;
            dc->GetTextExtent(headerTextCentre, & tx, & ty);

            int x = headerRect.GetWidth()/2 - tx/2 + headerRect.GetLeft();
            int y = headerRect.GetY();
            dc->DrawText(headerTextCentre, x, y);
        }
        if (!headerTextRight.IsEmpty())
        {
            SubstituteKeywords(headerTextRight, GetTitle(), page, m_numPages);

            int tx, ty;
            dc->GetTextExtent(headerTextRight, & tx, & ty);

            int x = headerRect.GetRight() - tx;
            int y = headerRect.GetY();
            dc->DrawText(headerTextRight, x, y);
        }

        // Draw footer, if any
        wxString footerTextCentre = m_headerFooterData.GetFooterText(oddEven, wxRICHTEXT_PAGE_CENTRE);
        wxString footerTextLeft = m_headerFooterData.GetFooterText(oddEven, wxRICHTEXT_PAGE_LEFT);
        wxString footerTextRight = m_headerFooterData.GetFooterText(oddEven, wxRICHTEXT_PAGE_RIGHT);

        if (!footerTextLeft.IsEmpty())
        {
            SubstituteKeywords(footerTextLeft, GetTitle(), page, m_numPages);

            int tx, ty;
            dc->GetTextExtent(footerTextLeft, & tx, & ty);

            int x = footerRect.GetLeft();
            int y = footerRect.GetBottom() - ty;
            dc->DrawText(footerTextLeft, x, y);
        }
        if (!footerTextCentre.IsEmpty())
        {
            SubstituteKeywords(footerTextCentre, GetTitle(), page, m_numPages);

            int tx, ty;
            dc->GetTextExtent(footerTextCentre, & tx, & ty);

            int x = footerRect.GetWidth()/2 - tx/2 + footerRect.GetLeft();
            int y = footerRect.GetBottom() - ty;
            dc->DrawText(footerTextCentre, x, y);
        }
        if (!footerTextRight.IsEmpty())
        {
            SubstituteKeywords(footerTextRight, GetTitle(), page, m_numPages);

            int tx, ty;
            dc->GetTextExtent(footerTextRight, & tx, & ty);

            int x = footerRect.GetRight() - tx;
            int y = footerRect.GetBottom() - ty;
            dc->DrawText(footerTextRight, x, y);
        }
    }

    wxRichTextRange rangeToDraw(m_pageBreaksStart[page-1], m_pageBreaksEnd[page-1]);

    wxPoint oldOrigin = dc->GetLogicalOrigin();
    double scaleX, scaleY;
    dc->GetUserScale(& scaleX, & scaleY);

    int yOffset = 0;
    if (page > 1)
        yOffset = m_pageYOffsets[page-2];

    if (yOffset != oldOrigin.y)
        dc->SetLogicalOrigin(oldOrigin.x, oldOrigin.y + yOffset);

    dc->SetClippingRegion(wxRect(textRect.x, textRect.y + yOffset, textRect.width, textRect.height));

    wxRichTextDrawingContext context(GetRichTextBuffer());
    GetRichTextBuffer()->Draw(*dc, context, rangeToDraw, wxRichTextSelection(), textRect, 0 /* descent */, wxRICHTEXT_DRAW_IGNORE_CACHE|wxRICHTEXT_DRAW_PRINT /* flags */);

    dc->DestroyClippingRegion();

    if (yOffset != oldOrigin.y)
        dc->SetLogicalOrigin(oldOrigin.x, oldOrigin.y);
}

void wxRichTextPrintout::SetMargins(int top, int bottom, int left, int right)
{
    m_marginTop = top;
    m_marginBottom = bottom;
    m_marginLeft = left;
    m_marginRight = right;
}

/// Calculate scaling and rectangles, setting the device context scaling
void wxRichTextPrintout::CalculateScaling(wxDC* dc, wxRect& textRect, wxRect& headerRect, wxRect& footerRect)
{
    // Get the logical pixels per inch of screen and printer
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

    // This scales the DC so that the printout roughly represents the
    // the screen scaling.
    const double scale = double(ppiPrinterX) / ppiScreenX;

    // Now we have to check in case our real page size is reduced
    // (e.g. because we're drawing to a print preview memory DC)
    int pageWidth, pageHeight;
    int w, h;
    dc->GetSize(&w, &h);
    GetPageSizePixels(&pageWidth, &pageHeight);

    // If printer pageWidth == current DC width, then this doesn't
    // change. But w might be the preview bitmap width, so scale down.
    const double previewScale = double(w) / pageWidth;
    const double overallScale = scale * previewScale;

    // The dimensions used for indentation etc. have to be unscaled
    // during printing to be correct when scaling is applied.
    // Also, correct the conversions in wxRTC using DC instead of print DC.
    m_richTextBuffer->SetScale(scale * dc->GetPPI().x / ppiPrinterX);

    // Calculate margins
    int marginLeft = wxRichTextObject::ConvertTenthsMMToPixels(ppiPrinterX, m_marginLeft);
    int marginTop = wxRichTextObject::ConvertTenthsMMToPixels(ppiPrinterX, m_marginTop);
    int marginRight = wxRichTextObject::ConvertTenthsMMToPixels(ppiPrinterX, m_marginRight);
    int marginBottom = wxRichTextObject::ConvertTenthsMMToPixels(ppiPrinterX, m_marginBottom);

    // Header and footer margins
    int headerMargin = wxRichTextObject::ConvertTenthsMMToPixels(ppiPrinterX, m_headerFooterData.GetHeaderMargin());
    int footerMargin = wxRichTextObject::ConvertTenthsMMToPixels(ppiPrinterX, m_headerFooterData.GetFooterMargin());

    dc->SetUserScale(overallScale, overallScale);

    wxRect rect((int) (marginLeft/scale), (int) (marginTop/scale),
                (int) ((pageWidth - marginLeft - marginRight)/scale), (int)((pageHeight - marginTop - marginBottom)/scale));

    headerRect = wxRect(0, 0, 0, 0);

    if (!m_headerFooterData.GetHeaderText(wxRICHTEXT_PAGE_ODD, wxRICHTEXT_PAGE_LEFT).IsEmpty() ||
        !m_headerFooterData.GetHeaderText(wxRICHTEXT_PAGE_ODD, wxRICHTEXT_PAGE_CENTRE).IsEmpty() ||
        !m_headerFooterData.GetHeaderText(wxRICHTEXT_PAGE_ODD, wxRICHTEXT_PAGE_RIGHT).IsEmpty() ||

        !m_headerFooterData.GetHeaderText(wxRICHTEXT_PAGE_EVEN, wxRICHTEXT_PAGE_LEFT).IsEmpty() ||
        !m_headerFooterData.GetHeaderText(wxRICHTEXT_PAGE_EVEN, wxRICHTEXT_PAGE_CENTRE).IsEmpty() ||
        !m_headerFooterData.GetHeaderText(wxRICHTEXT_PAGE_EVEN, wxRICHTEXT_PAGE_RIGHT).IsEmpty())
    {
        if (m_headerFooterData.GetFont().IsOk())
            dc->SetFont(m_headerFooterData.GetFont());
        else
            dc->SetFont(*wxNORMAL_FONT);

        int charHeight = dc->GetCharHeight();

        int headerHeight = (int) (charHeight + headerMargin/scale);

        headerRect = wxRect(rect.x, rect.y, rect.width, headerHeight);

        rect.y += headerHeight;
        rect.height -= headerHeight;
    }

    footerRect = wxRect(0, 0, 0, 0);

    if (!m_headerFooterData.GetFooterText(wxRICHTEXT_PAGE_ODD, wxRICHTEXT_PAGE_LEFT).IsEmpty() ||
        !m_headerFooterData.GetFooterText(wxRICHTEXT_PAGE_ODD, wxRICHTEXT_PAGE_CENTRE).IsEmpty() ||
        !m_headerFooterData.GetFooterText(wxRICHTEXT_PAGE_ODD, wxRICHTEXT_PAGE_RIGHT).IsEmpty() ||

        !m_headerFooterData.GetFooterText(wxRICHTEXT_PAGE_EVEN, wxRICHTEXT_PAGE_LEFT).IsEmpty() ||
        !m_headerFooterData.GetFooterText(wxRICHTEXT_PAGE_EVEN, wxRICHTEXT_PAGE_CENTRE).IsEmpty() ||
        !m_headerFooterData.GetFooterText(wxRICHTEXT_PAGE_EVEN, wxRICHTEXT_PAGE_RIGHT).IsEmpty())
    {
        if (m_headerFooterData.GetFont().IsOk())
            dc->SetFont(m_headerFooterData.GetFont());
        else
            dc->SetFont(*wxNORMAL_FONT);

        int charHeight = dc->GetCharHeight();

        int footerHeight = (int) (charHeight + footerMargin/scale);

        footerRect = wxRect(rect.x, rect.y + rect.height, rect.width, footerHeight);

        rect.height -= footerHeight;
    }

    textRect = rect;
}

bool wxRichTextPrintout::SubstituteKeywords(wxString& str, const wxString& title, int pageNum, int pageCount)
{
    wxString num;

    num.Printf(wxT("%i"), pageNum);
    str.Replace(wxT("@PAGENUM@"), num);

    num.Printf(wxT("%lu"), (unsigned long) pageCount);
    str.Replace(wxT("@PAGESCNT@"), num);

#if wxUSE_DATETIME
    wxDateTime now = wxDateTime::Now();

    str.Replace(wxT("@DATE@"), now.FormatDate());
    str.Replace(wxT("@TIME@"), now.FormatTime());
#else
    str.Replace(wxT("@DATE@"), wxEmptyString);
    str.Replace(wxT("@TIME@"), wxEmptyString);
#endif

    str.Replace(wxT("@TITLE@"), title);

    return true;
}

/*!
 * wxRichTextPrinting
 */

wxRichTextPrinting::wxRichTextPrinting(const wxString& name, wxWindow *parentWindow)
    : m_title(name)
    , m_previewRect(100, 100, 800, 800)
{
    m_richTextBufferPrinting = NULL;
    m_richTextBufferPreview = NULL;

    m_parentWindow = parentWindow;
    m_printData = NULL;

    m_pageSetupData = new wxPageSetupDialogData;
    m_pageSetupData->EnableMargins(true);
    m_pageSetupData->SetMarginTopLeft(wxPoint(25, 25));
    m_pageSetupData->SetMarginBottomRight(wxPoint(25, 25));
}

wxRichTextPrinting::~wxRichTextPrinting()
{
    delete m_printData;
    delete m_pageSetupData;
    delete m_richTextBufferPrinting;
    delete m_richTextBufferPreview;
}

wxPrintData *wxRichTextPrinting::GetPrintData()
{
    if (m_printData == NULL)
        m_printData = new wxPrintData();
    return m_printData;
}

/// Set print and page setup data
void wxRichTextPrinting::SetPrintData(const wxPrintData& printData)
{
    (*GetPrintData()) = printData;
}

void wxRichTextPrinting::SetPageSetupData(const wxPageSetupDialogData& pageSetupData)
{
    (*GetPageSetupData()) = pageSetupData;
}

/// Set the rich text buffer pointer, deleting the existing object if present
void wxRichTextPrinting::SetRichTextBufferPrinting(wxRichTextBuffer* buf)
{
    if (m_richTextBufferPrinting)
    {
        delete m_richTextBufferPrinting;
        m_richTextBufferPrinting = NULL;
    }
    m_richTextBufferPrinting = buf;
}

void wxRichTextPrinting::SetRichTextBufferPreview(wxRichTextBuffer* buf)
{
    if (m_richTextBufferPreview)
    {
        delete m_richTextBufferPreview;
        m_richTextBufferPreview = NULL;
    }
    m_richTextBufferPreview = buf;
}

#if wxUSE_FFILE && wxUSE_STREAMS
bool wxRichTextPrinting::PreviewFile(const wxString& richTextFile)
{
    SetRichTextBufferPreview(new wxRichTextBuffer);

    if (!m_richTextBufferPreview->LoadFile(richTextFile))
    {
        SetRichTextBufferPreview(NULL);
        return false;
    }
    else
        SetRichTextBufferPrinting(new wxRichTextBuffer(*m_richTextBufferPreview));

    wxRichTextPrintout *p1 = CreatePrintout();
    p1->SetRichTextBuffer(m_richTextBufferPreview);

    wxRichTextPrintout *p2 = CreatePrintout();
    p2->SetRichTextBuffer(m_richTextBufferPrinting);
    return DoPreview(p1, p2);
}
#endif // wxUSE_FFILE && wxUSE_STREAMS

bool wxRichTextPrinting::PreviewBuffer(const wxRichTextBuffer& buffer)
{
    SetRichTextBufferPreview(new wxRichTextBuffer(buffer));
    SetRichTextBufferPrinting(new wxRichTextBuffer(buffer));

    wxRichTextPrintout *p1 = CreatePrintout();
    p1->SetRichTextBuffer(m_richTextBufferPreview);

    wxRichTextPrintout *p2 = CreatePrintout();
    p2->SetRichTextBuffer(m_richTextBufferPrinting);

    return DoPreview(p1, p2);
}

#if wxUSE_FFILE && wxUSE_STREAMS
bool wxRichTextPrinting::PrintFile(const wxString& richTextFile, bool showPrintDialog)
{
    SetRichTextBufferPrinting(new wxRichTextBuffer);

    if (!m_richTextBufferPrinting->LoadFile(richTextFile))
    {
        SetRichTextBufferPrinting(NULL);
        return false;
    }

    wxRichTextPrintout *p = CreatePrintout();
    p->SetRichTextBuffer(m_richTextBufferPrinting);

    bool ret = DoPrint(p, showPrintDialog);
    delete p;
    return ret;
}
#endif // wxUSE_FFILE && wxUSE_STREAMS

bool wxRichTextPrinting::PrintBuffer(const wxRichTextBuffer& buffer, bool showPrintDialog)
{
    SetRichTextBufferPrinting(new wxRichTextBuffer(buffer));

    wxRichTextPrintout *p = CreatePrintout();
    p->SetRichTextBuffer(m_richTextBufferPrinting);

    bool ret = DoPrint(p, showPrintDialog);
    delete p;
    return ret;
}

bool wxRichTextPrinting::DoPreview(wxRichTextPrintout *printout1, wxRichTextPrintout *printout2)
{
    // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(*GetPrintData());
    wxPrintPreview *preview = new wxPrintPreview(printout1, printout2, &printDialogData);
    if (!preview->IsOk())
    {
        delete preview;
        return false;
    }

    wxPreviewFrame *frame = new wxPreviewFrame(preview, m_parentWindow,
                                               m_title + _(" Preview"),
                                               m_previewRect.GetPosition(), m_previewRect.GetSize());
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(true);
    return true;
}

bool wxRichTextPrinting::DoPrint(wxRichTextPrintout *printout, bool showPrintDialog)
{
    wxPrintDialogData printDialogData(*GetPrintData());
    wxPrinter printer(&printDialogData);

    if (!printer.Print(m_parentWindow, printout, showPrintDialog))
    {
        return false;
    }

    (*GetPrintData()) = printer.GetPrintDialogData().GetPrintData();
    return true;
}

void wxRichTextPrinting::PageSetup()
{
    if (!GetPrintData()->IsOk())
    {
        wxLogError(_("There was a problem during page setup: you may need to set a default printer."));
        return;
    }

    m_pageSetupData->SetPrintData(*GetPrintData());
    wxPageSetupDialog pageSetupDialog(m_parentWindow, m_pageSetupData);

    if (pageSetupDialog.ShowModal() == wxID_OK)
    {
        (*GetPrintData()) = pageSetupDialog.GetPageSetupData().GetPrintData();
        (*m_pageSetupData) = pageSetupDialog.GetPageSetupData();
    }
}

wxRichTextPrintout *wxRichTextPrinting::CreatePrintout()
{
    wxRichTextPrintout *p = new wxRichTextPrintout(m_title);

    p->SetHeaderFooterData(GetHeaderFooterData());
    p->SetMargins(10*m_pageSetupData->GetMarginTopLeft().y,
                    10*m_pageSetupData->GetMarginBottomRight().y,
                    10*m_pageSetupData->GetMarginTopLeft().x,
                    10*m_pageSetupData->GetMarginBottomRight().x);

    return p;
}

/// Set/get header text, e.g. wxRICHTEXT_PAGE_ODD, wxRICHTEXT_PAGE_LEFT
void wxRichTextPrinting::SetHeaderText(const wxString& text, wxRichTextOddEvenPage page, wxRichTextPageLocation location)
{
    m_headerFooterData.SetHeaderText(text, page, location);
}

wxString wxRichTextPrinting::GetHeaderText(wxRichTextOddEvenPage page, wxRichTextPageLocation location) const
{
    return m_headerFooterData.GetHeaderText(page, location);
}

/// Set/get footer text, e.g. wxRICHTEXT_PAGE_ODD, wxRICHTEXT_PAGE_LEFT
void wxRichTextPrinting::SetFooterText(const wxString& text, wxRichTextOddEvenPage page, wxRichTextPageLocation location)
{
    m_headerFooterData.SetFooterText(text, page, location);
}

wxString wxRichTextPrinting::GetFooterText(wxRichTextOddEvenPage page, wxRichTextPageLocation location) const
{
    return m_headerFooterData.GetFooterText(page, location);
}

/*!
 * Header/footer data
 */

wxIMPLEMENT_CLASS(wxRichTextHeaderFooterData, wxObject);

/// Copy
void wxRichTextHeaderFooterData::Copy(const wxRichTextHeaderFooterData& data)
{
    int i;
    for (i = 0; i < 12; i++)
        m_text[i] = data.m_text[i];
    m_font = data.m_font;
    m_colour = data.m_colour;
    m_headerMargin = data.m_headerMargin;
    m_footerMargin = data.m_footerMargin;
    m_showOnFirstPage = data.m_showOnFirstPage;
}

/// Set/get text
void wxRichTextHeaderFooterData::SetText(const wxString& text, int headerFooter, wxRichTextOddEvenPage page, wxRichTextPageLocation location)
{
    int idx = headerFooter + (2 * (int) page) + (4 * (int) location);
    wxASSERT( idx >= 0 && idx < 12 );

    if (idx >= 0 && idx < 12)
        m_text[idx] = text;
}

wxString wxRichTextHeaderFooterData::GetText(int headerFooter, wxRichTextOddEvenPage page, wxRichTextPageLocation location) const
{
    int idx = headerFooter + (2 * (int) page) + (4 * (int) location);
    wxASSERT( idx >= 0 && idx < 12 );

    if (idx >= 0 && idx < 12)
        return m_text[idx];
    else
        return wxEmptyString;
}

/// Set/get header text, e.g. wxRICHTEXT_PAGE_ODD, wxRICHTEXT_PAGE_LEFT
void wxRichTextHeaderFooterData::SetHeaderText(const wxString& text, wxRichTextOddEvenPage page, wxRichTextPageLocation location)
{
    if (page == wxRICHTEXT_PAGE_ALL)
    {
        SetText(text, 0, wxRICHTEXT_PAGE_ODD, location);
        SetText(text, 0, wxRICHTEXT_PAGE_EVEN, location);
    }
    else
        SetText(text, 0, page, location);
}

wxString wxRichTextHeaderFooterData::GetHeaderText(wxRichTextOddEvenPage page, wxRichTextPageLocation location) const
{
    return GetText(0, page, location);
}

/// Set/get footer text, e.g. wxRICHTEXT_PAGE_ODD, wxRICHTEXT_PAGE_LEFT
void wxRichTextHeaderFooterData::SetFooterText(const wxString& text, wxRichTextOddEvenPage page, wxRichTextPageLocation location)
{
    if (page == wxRICHTEXT_PAGE_ALL)
    {
        SetText(text, 1, wxRICHTEXT_PAGE_ODD, location);
        SetText(text, 1, wxRICHTEXT_PAGE_EVEN, location);
    }
    else
        SetText(text, 1, page, location);
}

wxString wxRichTextHeaderFooterData::GetFooterText(wxRichTextOddEvenPage page, wxRichTextPageLocation location) const
{
    return GetText(1, page, location);
}

/// Clear all text
void wxRichTextHeaderFooterData::Clear()
{
    int i;
    for (i = 0; i < 12; i++)
        m_text[i].clear();
}

#endif // wxUSE_RICHTEXT & wxUSE_PRINTING_ARCHITECTURE

