#ifndef _WX_STC_COMPLETIONITEM_H_
#define _WX_STC_COMPLETIONITEM_H_

#include "wx/string.h"
#include "wx/clntdata.h"

class WXDLLIMPEXP_STC wxCompletionItem
{
public:
    wxCompletionItem(const wxString& name, int icon, const wxString& doc = wxString(), wxClientData* data = NULL);
    virtual ~wxCompletionItem();

    const wxString& GetName();
    int GetIcon();
    const wxString& GetDocumentation();
    wxClientData* GetClientObject();

protected:
    wxString m_name;
    int m_icon;
    wxString m_doc;
    wxClientData* m_data;
};

#endif //_WX_STC_COMPLETIONITEM_H_