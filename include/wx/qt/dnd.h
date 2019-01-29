/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dnd.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DND_H_
#define _WX_QT_DND_H_

#define wxDROP_ICON(name)   wxICON(name)

class QMimeData;

class WXDLLIMPEXP_CORE wxDropTarget : public wxDropTargetBase
{
public:
    wxDropTarget(wxDataObject *dataObject = NULL);
    
    virtual bool OnDrop(wxCoord x, wxCoord y) wxOVERRIDE;
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def) wxOVERRIDE;
    virtual bool GetData() wxOVERRIDE;

    wxDataFormat GetMatchingPair();

    void OnQtEnter(QEvent* event);
    void OnQtLeave(QEvent* event);
    void OnQtMove(QEvent* event);
    void OnQtDrop(QEvent* event);

private:
    class PendingMimeDataSetter;
    friend class PendingMimeDataSetter;

    const QMimeData* m_pendingMimeData;
};

class WXDLLIMPEXP_CORE wxDropSource: public wxDropSourceBase
{
public:
    wxDropSource( wxWindow *win = NULL,
                  const wxIcon &copy = wxNullIcon,
                  const wxIcon &move = wxNullIcon,
                  const wxIcon &none = wxNullIcon);

    wxDropSource( wxDataObject& data,
                  wxWindow *win,
                  const wxIcon &copy = wxNullIcon,
                  const wxIcon &move = wxNullIcon,
                  const wxIcon &none = wxNullIcon);
    
    virtual wxDragResult DoDragDrop(int flags = wxDrag_CopyOnly);

private:
    wxWindow* m_parentWindow;
};
#endif // _WX_QT_DND_H_
