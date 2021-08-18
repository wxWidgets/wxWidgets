/////////////////////////////////////////////////////////////////////////////
// Name:        samples/event/chessboard.cpp
// Purpose:     Part of wxWidgets event sample, shows how to create a new wxEvent class
// Author:      PB <pbfordev@gmail.com>
// Created:     2019-10-29
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "chessboard.h"


//
// ChessBoardEvent
//

// Define new event types associated with ChessBoardEvent
wxDEFINE_EVENT(myEVT_CHESSBOARD_CLICKED, ChessBoardEvent);
wxDEFINE_EVENT(myEVT_CHESSBOARD_DRAGGED, ChessBoardEvent);


//
// ChessBoard
//
class ChessBoard : public wxPanel
{
public:
    ChessBoard(wxWindow* parent);

private:
    static const wxUint8 SquaresPerSide = 8;
    static const int SquareSizeInDIPs   = 40;

    // square on which mouse left button was pressed
    char m_fileLeftMDown;
    wxUint8 m_rankLeftMDown;

    bool ConvertMousePosToFileAndRank(const wxPoint& pos,
                                      char& file, wxUint8& rank) const;

    void OnPaint(wxPaintEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);

    virtual wxSize DoGetBestClientSize() const wxOVERRIDE;
};

ChessBoard::ChessBoard(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED),
      m_fileLeftMDown('a'), m_rankLeftMDown(1)
{
    SetCursor(wxCursor(wxCURSOR_HAND));
    SetBackgroundColour(*wxWHITE);

    Bind(wxEVT_PAINT, &ChessBoard::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &ChessBoard::OnMouseLeftDown, this);
    Bind(wxEVT_LEFT_UP, &ChessBoard::OnMouseLeftUp, this);
}

 bool ChessBoard::ConvertMousePosToFileAndRank(const wxPoint& pos,
                                               char& file, wxUint8& rank) const
{
    static const char FileLetters[SquaresPerSide] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

    const int squareSizeInPx = FromDIP(SquareSizeInDIPs);
    const int colIdx = pos.x / squareSizeInPx;
    const int rowIdx = pos.y / squareSizeInPx;

    if ( colIdx < 0 || colIdx >= SquaresPerSide
         || rowIdx < 0 || rowIdx >= SquaresPerSide )
    {
        return false; // not on a square
    }

    file = FileLetters[colIdx],
    // convert rowIdx (top to down 0 to 7) to rank (top to down 8 to 1)
    rank = static_cast<wxUint8>(SquaresPerSide - rowIdx);

    return true;
}

// Background is already painted white,
// so we need to draw only the black squares over it.
void ChessBoard::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    const int squareSizeInPx = FromDIP(SquareSizeInDIPs);

    wxPaintDC dc(this);
    wxDCPenChanger penChanger(dc, *wxTRANSPARENT_PEN);
    wxDCBrushChanger brushChanger(dc, *wxBLACK_BRUSH);
    wxRect squareRect(wxSize(squareSizeInPx, squareSizeInPx));

    // draw from top to down, left to right
    for ( size_t rank = SquaresPerSide; rank >= 1; --rank )
    {
        if ( rank % 2 )
            squareRect.SetLeft(0); // odd ranks start with a black square
        else
            squareRect.SetLeft(squareSizeInPx); // even ranks start with a white square

        for ( size_t file = 0; file < SquaresPerSide / 2; ++file )
        {
            dc.DrawRectangle(squareRect);
            squareRect.Offset(2 * squareSizeInPx, 0);
        }

        squareRect.Offset(0, squareSizeInPx);
    }
}

void ChessBoard::OnMouseLeftDown(wxMouseEvent& event)
{
    char file;
    wxUint8 rank;

    event.Skip();

    if ( !ConvertMousePosToFileAndRank(event.GetPosition(), file, rank) )
        return; // mouse cursor not on a square

    m_fileLeftMDown = file;
    m_rankLeftMDown = rank;
    CaptureMouse();
}

void ChessBoard::OnMouseLeftUp(wxMouseEvent& event)
{
    event.Skip();

    if ( !HasCapture() )
        return; // capture lost or mouse left down was not on a square

    const wxRect clientRect = GetClientRect();
    char file;
    wxUint8 rank;

    ReleaseMouse();

    if (  !clientRect.Contains(event.GetPosition()) ||
          !ConvertMousePosToFileAndRank(event.GetPosition(), file, rank) )
    {
        return; // mouse cursor not on a square
    }

    ChessBoardEvent chessBoardEvent;

    chessBoardEvent.SetId(GetId());
    chessBoardEvent.SetEventObject(this);
    chessBoardEvent.SetFile(m_fileLeftMDown);
    chessBoardEvent.SetRank(m_rankLeftMDown);

    if ( file == m_fileLeftMDown && rank == m_rankLeftMDown )
    {   // mouse left and down on the same square, consider it a click
        chessBoardEvent.SetEventType(myEVT_CHESSBOARD_CLICKED);
    }
    else
    {   // mouse dragged from one square to another
        chessBoardEvent.SetEventType(myEVT_CHESSBOARD_DRAGGED);
        chessBoardEvent.SetFileTo(file);
        chessBoardEvent.SetRankTo(rank);
    }

    ProcessWindowEvent(chessBoardEvent);
}

wxSize ChessBoard::DoGetBestClientSize() const
{
    const int boardSideSizeInPx = SquaresPerSide * FromDIP(SquareSizeInDIPs);

    return wxSize(boardSideSizeInPx, boardSideSizeInPx);
}

//
// MyChessBoardDialog implementation
//

// For demonstration, event table is used for myEVT_CHESSBOARD_CLICKED
// while Bind() is used for myEVT_CHESSBOARD_DRAGGED.
wxBEGIN_EVENT_TABLE(MyChessBoardDialog, wxDialog)
    EVT_CHESSBOARD_CLICKED(wxID_ANY, MyChessBoardDialog::OnChessBoardClicked)
wxEND_EVENT_TABLE()

MyChessBoardDialog::MyChessBoardDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "New event class demonstration")
{
    wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(
        new wxStaticText(this, wxID_ANY, "Click or drag the mouse over the chessboard"),
        wxSizerFlags().Border().CenterHorizontal());

    mainSizer->Add(new ChessBoard(this),
        wxSizerFlags().TripleBorder().CenterHorizontal());

    mainSizer->Add(CreateStdDialogButtonSizer(wxCLOSE),
        wxSizerFlags().Expand().Border());

    SetSizerAndFit(mainSizer);

    Bind(myEVT_CHESSBOARD_DRAGGED, &MyChessBoardDialog::OnChessBoardDragged, this);
}

void MyChessBoardDialog::OnChessBoardClicked(ChessBoardEvent& event)
{
    wxLogMessage("myEVT_CHESSBOARD_CLICKED: %c%hhu",
        event.GetFile(), event.GetRank());
}

void MyChessBoardDialog::OnChessBoardDragged(ChessBoardEvent& event)
{
    wxLogMessage("myEVT_CHESSBOARD_DRAGGED: from %c%hhu to %c%hhu",
        event.GetFile(), event.GetRank(),
        event.GetFileTo(), event.GetRankTo());
}
