#ifndef _WX_STC_COMPLETIONITEM_H_
#define _WX_STC_COMPLETIONITEM_H_

#include "wx/string.h"
#include "wx/clntdata.h"

class WXDLLIMPEXP_STC wxCompletionItem
{
public:
    wxCompletionItem(const wxString& name, int icon, const wxString& doc = wxString(), wxClientData* data = NULL)
    : m_name(name), m_icon(icon), m_doc(doc), m_data(data)
    {
    }
    ~wxCompletionItem(){}

    const wxString& GetName() const { return m_name; }
    int GetIcon() const { return m_icon; }
    const wxString& GetDocumentation() const { return m_doc; }
    wxClientData* GetClientObject(){ return m_data; }

private:
    wxString m_name;
    int m_icon;
    wxString m_doc;
    wxClientData* m_data;
};

#endif //_WX_STC_COMPLETIONITEM_H_