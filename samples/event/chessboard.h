/////////////////////////////////////////////////////////////////////////////
// Name:        samples/event/chessboard.h
// Purpose:     Part of wxWidgets event sample, shows how to create a new wxEvent class
// Author:      PB <pbfordev@gmail.com>
// Created:     2019-10-29
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHESSBOARD_H_

#include "wx/wx.h"

// A chessboard is organized in columns (called files)
// and rows (called ranks). In a 2D view,
// files are labeled from left to right 'a' to 'h'
// while ranks are numbered from bottom to top 1 to 8.
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

// ChessBoardEvent tells us something happened on
// the chessboard. Either a square was clicked
// (EVT_CHESSBOARD_CLICKED) or the mouse cursor was dragged
// from one square to another (EVT_CHESSBOARD_DRAGGED).
class ChessBoardEvent : public wxCommandEvent
{
public:
    ChessBoardEvent(wxEventType commandEventType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandEventType, id),
          m_file('a'), m_rank(1), m_fileTo('a'), m_rankTo(1)
    {}

    // EVT_CHESSBOARD_CLICKED: Returns File/Rank for the square clicked.
    // EVT_CHESSBOARD_DRAGGED: Returns File/Rank for the square
    //                         the mouse cursor was dragged from.
    char GetFile() const    { return m_file; }
    wxUint8 GetRank() const { return m_rank; }

    // Valid only for EVT_CHESSBOARD_DRAGGED,
    // returns File/Rank for the square where the mouse cursor was dragged to.
    char GetFileTo() const    { return m_fileTo; }
    wxUint8 GetRankTo() const { return m_rankTo; }

    void SetFile(char file)    { m_file = file; }
    void SetRank(wxUint8 rank) { m_rank = rank; }

    void SetFileTo(char file)    { m_fileTo = file; }
    void SetRankTo(wxUint8 rank) { m_rankTo = rank; }

    virtual wxEvent* Clone() const wxOVERRIDE { return new ChessBoardEvent(*this); }
private:
    char  m_file;
    wxUint8 m_rank;
    char m_fileTo;
    wxUint8 m_rankTo;
};

// Define new event types associated with ChessBoardEvent
wxDEFINE_EVENT(EVT_CHESSBOARD_CLICKED, ChessBoardEvent);
wxDEFINE_EVENT(EVT_CHESSBOARD_DRAGGED, ChessBoardEvent);

// @TODO: what to write here, the info in the docs seem to be incorrect?
typedef void (wxEvtHandler::* ChessBoardEventFunction)(ChessBoardEvent&);
#define ChessBoardEventHandler(func) wxEVENT_HANDLER_CAST(ChessBoardEventFunction, func)

// These defines are needed only if the event is to be used with event tables,
// i.e., when only Bind() is going to be used you do not need them.
#define etEVT_CHESSBOARD_CLICKED(id, func) \
    wx__DECLARE_EVT1(EVT_CHESSBOARD_CLICKED, id, ChessBoardEventHandler(func))
#define etEVT_CHESSBOARD_DRAGGED(id, func) \
    wx__DECLARE_EVT1(EVT_CHESSBOARD_DRAGGED, id, ChessBoardEventHandler(func))

// wxDECLARE_EVENT is needed only when ChessBoardEvent
// is going to be exported from the DLL.
wxDECLARE_EVENT(EVT_CHESSBOARD_CLICKED, ChessBoardEvent);
wxDECLARE_EVENT(EVT_CHESSBOARD_DRAGGED, ChessBoardEvent);


// Dialog displaying the chessboard
// and handling ChessBoardEvents
class MyChessBoardDialog : public wxDialog
{
public:
    MyChessBoardDialog(wxWindow* parent);
private:
    void OnChessBoardClicked(ChessBoardEvent& event);
    void OnChessBoardDragged(ChessBoardEvent& event);

    // Just for demonstration, we will use an event table
    // for EVT_CHESSBOARD_CLICKED.
    wxDECLARE_EVENT_TABLE();
};

#endif // #ifndef _WX_CHESSBOARD_H_
