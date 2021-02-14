/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/textentry.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TEXTENTRY_H_
#define _WX_QT_TEXTENTRY_H_

class WXDLLIMPEXP_CORE wxTextEntry : public wxTextEntryBase
{
public:
    wxTextEntry();

    virtual void WriteText(const wxString& text) wxOVERRIDE;

    virtual void Remove(long from, long to) wxOVERRIDE;

    virtual void Copy() wxOVERRIDE;
    virtual void Cut() wxOVERRIDE;
    virtual void Paste() wxOVERRIDE;

    virtual void Undo() wxOVERRIDE;
    virtual void Redo() wxOVERRIDE;
    virtual bool CanUndo() const wxOVERRIDE;
    virtual bool CanRedo() const wxOVERRIDE;

    virtual void SetInsertionPoint(long pos) wxOVERRIDE;
    virtual long GetInsertionPoint() const wxOVERRIDE;
    virtual long GetLastPosition() const wxOVERRIDE;

    virtual void SetSelection(long from, long to) wxOVERRIDE;
    virtual void GetSelection(long *from, long *to) const wxOVERRIDE;

    virtual bool IsEditable() const wxOVERRIDE;
    virtual void SetEditable(bool editable) wxOVERRIDE;

protected:
    virtual wxString DoGetValue() const wxOVERRIDE;
    virtual void DoSetValue(const wxString& value, int flags=0) wxOVERRIDE;

    virtual wxWindow *GetEditableWindow() wxOVERRIDE;
};

#endif // _WX_QT_TEXTENTRY_H_
