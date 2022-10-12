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

    virtual void WriteText(const wxString& text) override;

    virtual void Remove(long from, long to) override;

    virtual void Copy() override;
    virtual void Cut() override;
    virtual void Paste() override;

    virtual void Undo() override;
    virtual void Redo() override;
    virtual bool CanUndo() const override;
    virtual bool CanRedo() const override;

    virtual void SetInsertionPoint(long pos) override;
    virtual long GetInsertionPoint() const override;
    virtual long GetLastPosition() const override;

    virtual void SetSelection(long from, long to) override;
    virtual void GetSelection(long *from, long *to) const override;

    virtual bool IsEditable() const override;
    virtual void SetEditable(bool editable) override;

protected:
    virtual wxString DoGetValue() const override;
    virtual void DoSetValue(const wxString& value, int flags=0) override;

    virtual wxWindow *GetEditableWindow() override;
};

#endif // _WX_QT_TEXTENTRY_H_
