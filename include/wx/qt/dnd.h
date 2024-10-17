/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dnd.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DND_H_
#define _WX_QT_DND_H_

#ifdef __WINDOWS__
#define wxDROP_ICON(name)   wxCursor(#name)
#else
#define wxDROP_ICON(name)   wxCursor(name##_xpm)
#endif

class WXDLLIMPEXP_CORE wxDropTarget : public wxDropTargetBase
{
public:
    wxDropTarget(wxDataObject *dataObject = nullptr);
    virtual ~wxDropTarget();

    virtual bool OnDrop(wxCoord x, wxCoord y) override;
    virtual wxDragResult OnData(wxCoord x,
                                wxCoord y,
                                wxDragResult def) override;
    virtual bool GetData() override;

    wxDataFormat GetMatchingPair();

    void ConnectTo(QWidget* widget);
    void Disconnect();

private:
    class Impl;
    Impl* m_pImpl;
};

class WXDLLIMPEXP_CORE wxDropSource: public wxDropSourceBase
{
public:
    wxDropSource(wxWindow *win = nullptr,
                 const wxCursor &copy = wxNullCursor,
                 const wxCursor &move = wxNullCursor,
                 const wxCursor &none = wxNullCursor);

    wxDropSource(wxDataObject& data,
                 wxWindow *win,
                 const wxCursor &copy = wxNullCursor,
                 const wxCursor &move = wxNullCursor,
                 const wxCursor &none = wxNullCursor);

    virtual wxDragResult DoDragDrop(int flags = wxDrag_CopyOnly) override;

private:
    wxWindow* m_parentWindow;
};
#endif // _WX_QT_DND_H_
