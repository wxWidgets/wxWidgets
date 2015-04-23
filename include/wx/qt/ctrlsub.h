/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/ctrlsub.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CTRLSUB_H_
#define _WX_QT_CTRLSUB_H_

class WXDLLIMPEXP_CORE wxControlWithItems : public wxControlWithItemsBase
{
public:
    wxControlWithItems();

protected:

private:
    wxDECLARE_ABSTRACT_CLASS(wxControlWithItems);
};

#endif // _WX_QT_CTRLSUB_H_
