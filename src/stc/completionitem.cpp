#include "wx/stc/completionitem.h"

wxCompletionItem::wxCompletionItem(const wxString& name, int icon, const wxString& doc, wxClientData* data)
    : m_name(name), m_icon(icon), m_doc(doc), m_data(data)
{
}
wxCompletionItem::~wxCompletionItem() {}
const wxString& wxCompletionItem::GetName() { return m_name; }
int wxCompletionItem::GetIcon() { return m_icon; }
const wxString& wxCompletionItem::GetDocumentation() { return m_doc; }
wxClientData* wxCompletionItem::GetClientObject() { return m_data; }