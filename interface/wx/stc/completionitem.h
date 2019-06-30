/////////////////////////////////////////////////////////////////////////////
// Name:        wx/stc/completionitem.h
// Purpose:     Class representing wxStyledTextCtrl Autocompletion single item
// Author:      Stefano Mtangoo
// Modified by:
// Created:     2019-06-18
// Copyright:   (c) Hosanna Higher Technologies Co. Ltd
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxCompletionItem

    wxCompletionItem represents single autocompletion object that make autocompletion list
    in the @link wxStyledTextCtrl::AutoCompShow @endlink
 */
class wxCompletionItem
{
public:
    /**
     * Constructor
     * @param name item name. This will be displayed when displaying @link wxStyledTextCtrl @endlink autocompletion list
     * @param icon the autocompletion icon that is associated with this item. The icon should be registered beforehand by @link wxStyledTextCtrl::RegisterImage @endlink
     * @param doc (optional) documentation of current item. Currently accepts plain text format only
     * @param data extra data associated with autocompletion item 
     * @see wxStyledTextCtrl::AutoCompShow(int, const wxVector< wxCompletionItem > &)
     */
    wxCompletionItem(const wxString& name, int icon, const wxString& doc = wxString(), wxClientData* data = NULL);
    ~wxCompletionItem();

    ///Get Item name
    const wxString& GetName();
    ///Get Item icon
    int GetIcon();
    ///Get Documentation text
    const wxString& GetDocumentation();
    ///Get custom Client data
    wxClientData* GetClientObject();

private:
    wxString m_name;
    int m_icon;
    wxString m_doc;
    wxClientData* m_data;
};