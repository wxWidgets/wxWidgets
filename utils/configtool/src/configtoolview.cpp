/////////////////////////////////////////////////////////////////////////////
// Name:        configtoolview.cpp
// Purpose:     View class
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "configtoolview.h"
#endif

// Includes other headers for precompiled compilation
#include "wx/wx.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wfstream.h"

#include "configtoolview.h"
#include "configtooldoc.h"
#include "configtree.h"
#include "wxconfigtool.h"
#include "mainframe.h"
#include "propeditor.h"
#include "utils.h"
#include "custompropertydialog.h"

IMPLEMENT_DYNAMIC_CLASS(ctConfigToolView, wxView)

BEGIN_EVENT_TABLE(ctConfigToolView, wxView)
    EVT_MENU(ctID_ADD_ITEM_CHECKBOX, ctConfigToolView::OnAddCheckBoxItem)
    EVT_MENU(ctID_ADD_ITEM_RADIOBUTTON, ctConfigToolView::OnAddRadioButtonItem)
    EVT_MENU(ctID_ADD_ITEM_GROUP, ctConfigToolView::OnAddGroupItem)
    EVT_MENU(ctID_ADD_ITEM_CHECK_GROUP, ctConfigToolView::OnAddCheckGroupItem)
    EVT_MENU(ctID_ADD_ITEM_RADIO_GROUP, ctConfigToolView::OnAddRadioGroupItem)
    EVT_MENU(ctID_ADD_ITEM_STRING, ctConfigToolView::OnAddStringItem)
    EVT_MENU(ctID_RENAME_ITEM, ctConfigToolView::OnRenameItem)
    EVT_MENU(ctID_DELETE_ITEM, ctConfigToolView::OnDeleteItem)

    EVT_MENU(wxID_COPY, ctConfigToolView::OnCopy)
    EVT_MENU(wxID_CUT, ctConfigToolView::OnCut)
    EVT_MENU(wxID_PASTE, ctConfigToolView::OnPaste)

    EVT_UPDATE_UI(wxID_COPY, ctConfigToolView::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_CUT, ctConfigToolView::OnUpdateCut)
    EVT_UPDATE_UI(wxID_PASTE, ctConfigToolView::OnUpdatePaste)

    EVT_MENU(ctID_ITEM_HELP,  ctConfigToolView::OnItemHelp)

    EVT_UPDATE_UI(ctID_ADD_ITEM_CHECKBOX, ctConfigToolView::OnUpdateAddItem)
    EVT_UPDATE_UI(ctID_ADD_ITEM_RADIOBUTTON, ctConfigToolView::OnUpdateAddItem)
    EVT_UPDATE_UI(ctID_ADD_ITEM_GROUP, ctConfigToolView::OnUpdateAddItem)
    EVT_UPDATE_UI(ctID_ADD_ITEM_CHECK_GROUP, ctConfigToolView::OnUpdateAddItem)
    EVT_UPDATE_UI(ctID_ADD_ITEM_RADIO_GROUP, ctConfigToolView::OnUpdateAddItem)
    EVT_UPDATE_UI(ctID_ADD_ITEM_STRING, ctConfigToolView::OnUpdateAddItem)
    EVT_UPDATE_UI(ctID_RENAME_ITEM, ctConfigToolView::OnUpdateCut)
    EVT_UPDATE_UI(ctID_DELETE_ITEM, ctConfigToolView::OnUpdateCut)
    EVT_UPDATE_UI(ctID_ITEM_HELP,  ctConfigToolView::OnUpdateItemHelp)

    EVT_MENU(ctID_TREE_COPY,  ctConfigToolView::OnContextCopy)
    EVT_MENU(ctID_TREE_CUT,  ctConfigToolView::OnContextCut)
    EVT_MENU(ctID_TREE_PASTE_BEFORE,  ctConfigToolView::OnContextPasteBefore)
    EVT_MENU(ctID_TREE_PASTE_AFTER,  ctConfigToolView::OnContextPasteAfter)
    EVT_MENU(ctID_TREE_PASTE_AS_CHILD,  ctConfigToolView::OnContextPasteAsChild)
    EVT_UPDATE_UI(ctID_TREE_COPY,  ctConfigToolView::OnUpdateContextCopy)
    EVT_UPDATE_UI(ctID_TREE_CUT,  ctConfigToolView::OnUpdateContextCut)
    EVT_UPDATE_UI(ctID_TREE_PASTE_BEFORE,  ctConfigToolView::OnUpdateContextPasteBefore)
    EVT_UPDATE_UI(ctID_TREE_PASTE_AFTER,  ctConfigToolView::OnUpdateContextPasteAfter)
    EVT_UPDATE_UI(ctID_TREE_PASTE_AS_CHILD,  ctConfigToolView::OnUpdateContextPasteAsChild)

    EVT_MENU(ctID_ADD_CUSTOM_PROPERTY, ctConfigToolView::OnAddCustomProperty)
    EVT_MENU(ctID_EDIT_CUSTOM_PROPERTY, ctConfigToolView::OnEditCustomProperty)
    EVT_MENU(ctID_DELETE_CUSTOM_PROPERTY, ctConfigToolView::OnDeleteCustomProperty)
    EVT_UPDATE_UI(ctID_ADD_CUSTOM_PROPERTY, ctConfigToolView::OnUpdateAddCustomProperty)
    EVT_UPDATE_UI(ctID_EDIT_CUSTOM_PROPERTY, ctConfigToolView::OnUpdateEditCustomProperty)
    EVT_UPDATE_UI(ctID_DELETE_CUSTOM_PROPERTY, ctConfigToolView::OnUpdateDeleteCustomProperty)

    EVT_NOTEBOOK_PAGE_CHANGED(-1, ctConfigToolView::OnTabSelect)

    EVT_MENU(ctID_SAVE_SETUP_FILE, ctConfigToolView::OnSaveSetupFile)
    EVT_MENU(ctID_SAVE_CONFIGURE_COMMAND, ctConfigToolView::OnSaveConfigureCommand)
    EVT_UPDATE_UI(ctID_SAVE_SETUP_FILE, ctConfigToolView::OnUpdateSaveSetupFile)
    EVT_UPDATE_UI(ctID_SAVE_CONFIGURE_COMMAND, ctConfigToolView::OnUpdateSaveConfigureCommand)
END_EVENT_TABLE()

ctConfigToolView::ctConfigToolView()
{
}

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool ctConfigToolView::OnCreate(wxDocument *doc, long WXUNUSED(flags) )
{
    wxGetApp().GetDocManager()->ActivateView(this, TRUE);
    wxGetApp().GetMainFrame()->SetDocument((ctConfigToolDoc*) doc);
    wxGetApp().GetMainFrame()->GetSetupPage()->SetDocument((ctConfigToolDoc*) doc) ;
    wxGetApp().GetMainFrame()->GetConfigurePage()->SetDocument((ctConfigToolDoc*) doc) ;

    return TRUE;
}

void ctConfigToolView::OnDraw(wxDC *dc)
{
}

void ctConfigToolView::OnUpdate(wxView *WXUNUSED(sender), wxObject *hintObj)
{
    ctConfigToolDoc* doc = wxDynamicCast(GetDocument(), ctConfigToolDoc);
    ctConfigTreeCtrl* treeCtrl = wxGetApp().GetMainFrame()->GetConfigTreeCtrl();
    if (!treeCtrl)
        return;
    
    wxASSERT (doc != NULL);
    
    ctConfigItem* selItem = NULL;
    
    wxTreeItemId sel = treeCtrl->GetSelection();
    if (sel.IsOk())
    {
        ctTreeItemData* data = (ctTreeItemData*) treeCtrl->GetItemData(sel);
        if (data)
            selItem = data->GetConfigItem() ;
    }
    
    ctConfigToolHint* hint = (ctConfigToolHint*) hintObj;
    int hintOp = ctNoHint;
    if (hint)
        hintOp = hint->m_op;
    
    switch (hintOp)
    {
    case ctInitialUpdate:
        {
            if (doc && doc->GetTopItem())
            {
                wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->DeleteAllItems();
                AddItems(wxGetApp().GetMainFrame()->GetConfigTreeCtrl(), doc->GetTopItem());
                wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->Expand(doc->GetTopItem()->GetTreeItemId());
            }
        }
        break;
    case ctSelChanged:        
        {
            if (selItem)
            {
                wxGetApp().GetMainFrame()->GetPropertyEditor()->ShowItem(selItem);
            }            
        }
        break;
    case ctAllSaved:
        {
            // TODO: update windows and frame title
        }
        break;
    case ctFilenameChanged:
        {
            wxGetApp().GetMainFrame()->UpdateFrameTitle();
        }
        break;
    case ctClear:
        {
            // TODO: update windows
            treeCtrl->DeleteAllItems();
            wxGetApp().GetMainFrame()->GetPropertyEditor()->ShowItem(NULL);
            break;
        }        
    case ctValueChanged:
        {
            // ctConfigItem& ti = *(ctConfigItem *)hint->m_item;
            wxGetApp().GetMainFrame()->GetPropertyEditor()->ShowItem(selItem);
        }		        
        break;
        
    default:
        break;
    }
}

// Clean up windows used for displaying the view.
bool ctConfigToolView::OnClose(bool deleteWindow)
{
    if (!GetDocument()->Close())
        return FALSE;

    ctConfigToolHint hint(NULL, ctClear);
    GetDocument()->UpdateAllViews (NULL, & hint);

    wxGetApp().GetDocManager()->ActivateView(this, FALSE);

    Activate(FALSE);
    
    wxGetApp().GetMainFrame()->SetDocument(NULL);
    wxGetApp().GetMainFrame()->GetSetupPage()->SetDocument(NULL) ;
    wxGetApp().GetMainFrame()->GetConfigurePage()->SetDocument(NULL) ;

    return TRUE;
}

void ctConfigToolView::OnChangeFilename()
{
    if (wxGetApp().GetTopWindow() && GetDocument())
    {
        wxString docTitle(wxFileNameFromPath(GetDocument()->GetFilename()));
        wxStripExtension(docTitle);
        GetDocument()->SetTitle(docTitle);

        wxString name(GetDocument()->GetFilename());
        wxStripExtension(name);

        wxString title;

        wxString modifiedMarker;
        if (GetDocument()->IsModified())
            modifiedMarker = wxT("*");

        title = docTitle + modifiedMarker + wxString(wxT(" - ")) + wxGetApp().GetSettings().GetAppName() ;

        ((wxFrame*) wxGetApp().GetTopWindow())->SetTitle(title);
    }
}

// General disabler
void ctConfigToolView::OnUpdateDisable(wxUpdateUIEvent& event)
{
    event.Enable( FALSE );
}

void ctConfigToolView::OnUpdateAddItem(wxUpdateUIEvent& event)
{
    event.Enable( TRUE );
}

/// Add item and its children to the tree
void ctConfigToolView::AddItems(ctConfigTreeCtrl* treeControl, ctConfigItem* item)
{
    SyncItem(treeControl, item);

    int count = item->GetChildCount();
    int i;
    for (i = 0; i < count; i++)
    {
        ctConfigItem* child = item->GetChild(i);
        AddItems(treeControl, child);
    }
}

/// Gets the tree item in sync with the item
void ctConfigToolView::SyncItem(ctConfigTreeCtrl* treeControl, ctConfigItem* item)
{
    if (!item->GetTreeItemId().IsOk())
    {
        if (!item->GetParent())
        {
            item->SetTreeItem(treeControl->AddRoot(_(""), -1, -1, new ctTreeItemData(item)));
        }
        else
        {
            // Find the item to insert the new item after.
            ctConfigItem* previousItem = item->FindPreviousSibling();
            if (previousItem && previousItem->GetTreeItemId().IsOk())
            {
                item->SetTreeItem(treeControl->InsertItem(item->GetParent()->GetTreeItemId(),
                    previousItem->GetTreeItemId(),
                    item->GetName(), -1, -1, new ctTreeItemData(item)));
            }
            else if (!previousItem && item->GetParent()->GetChildCount() > 1
                     )
            {
                // Insert at the beginning
                item->SetTreeItem(treeControl->InsertItem(item->GetParent()->GetTreeItemId(),
                    (size_t) 0, // Insert at first position
                    item->GetName(), -1, -1, new ctTreeItemData(item)));
            }
            else
            {
                item->SetTreeItem(treeControl->AppendItem(item->GetParent()->GetTreeItemId(),
                    item->GetName(), -1, -1, new ctTreeItemData(item)));
            }
        }
    }

    if (item->GetTreeItemId().IsOk())
    {
        treeControl->SetItemText(item->GetTreeItemId(), item->GetName());

        int iconId = 0;

        if (item->GetType() == ctTypeGroup)
        {
            iconId = treeControl->GetIconTable().GetIconId(wxT("Group"), 0, item->IsActive());
        }
        else if (item->GetType() == ctTypeCheckGroup)
        {
            iconId = treeControl->GetIconTable().GetIconId(wxT("CheckGroup"), item->IsEnabled() ? 0 : 1, item->IsActive());
        }
        else if (item->GetType() == ctTypeRadioGroup)
        {
            iconId = treeControl->GetIconTable().GetIconId(wxT("RadioGroup"), item->IsEnabled() ? 0 : 1, item->IsActive());
        }
        else if (item->GetType() == ctTypeBoolCheck)
        {
            iconId = treeControl->GetIconTable().GetIconId(wxT("Checkbox"), item->IsEnabled() ? 0 : 1, item->IsActive());
        }
        else if (item->GetType() == ctTypeBoolRadio)
        {
            iconId = treeControl->GetIconTable().GetIconId(wxT("Radiobutton"), item->IsEnabled() ? 0 : 1, item->IsActive());
        }
        treeControl->SetItemImage(item->GetTreeItemId(), iconId, wxTreeItemIcon_Normal);
        treeControl->SetItemImage(item->GetTreeItemId(), iconId, wxTreeItemIcon_Selected);

        if (treeControl->GetSelection() == item->GetTreeItemId())
        {
            if (wxGetApp().GetMainFrame()->GetPropertyEditor()->GetItem())
                wxGetApp().GetMainFrame()->GetPropertyEditor()->UpdateTitle();
        }
    }
}

/// Clicked an icon
void ctConfigToolView::OnIconLeftDown(ctConfigTreeCtrl* treeControl, ctConfigItem* item)
{
    if (!item->IsActive())
        return;

    if (item->GetType() == ctTypeCheckGroup ||
        item->GetType() == ctTypeBoolCheck ||
        item->GetType() == ctTypeBoolRadio ||
        item->GetType() == ctTypeRadioGroup
        )
    {
        // Don't toggle a radio button that's already
        // enabled.
        if ((item->GetType() == ctTypeBoolRadio || item->GetType() == ctTypeRadioGroup)
             && item->IsEnabled())
            return;

        item->Enable(!item->IsEnabled());

        GetDocument()->Modify(TRUE);
        OnChangeFilename();

        SyncItem(treeControl, item);

        wxList considered;
        item->PropagateChange(considered);
        if ((item->GetType() == ctTypeBoolRadio || item->GetType() == ctTypeRadioGroup) && item->IsEnabled())
        {
            item->PropagateRadioButton(considered);
        }
    }
}

/// Returns the selected config item, if any.
ctConfigItem* ctConfigToolView::GetSelection()
{
    wxTreeCtrl* treeCtrl = wxGetApp().GetMainFrame()->GetConfigTreeCtrl();
    if (!treeCtrl)
        return NULL;

    wxTreeItemId sel = treeCtrl->GetSelection();
    if (sel.IsOk())
    {
        ctTreeItemData* data = (ctTreeItemData*) treeCtrl->GetItemData(sel);
        if (data)
            return data->GetConfigItem() ;
    }
    return NULL;
}

/// Add a checkbox item
void ctConfigToolView::OnAddCheckBoxItem(wxCommandEvent& event)
{
    AddItem(ctTypeBoolCheck, _("New checkbox item"));
}

/// Add a radiobutton item
void ctConfigToolView::OnAddRadioButtonItem(wxCommandEvent& event)
{
    AddItem(ctTypeBoolRadio, _("New radio button item"));
}

/// Add a group item
void ctConfigToolView::OnAddGroupItem(wxCommandEvent& event)
{
    AddItem(ctTypeGroup, _("New group item"));
}

/// Add a group option item
void ctConfigToolView::OnAddCheckGroupItem(wxCommandEvent& event)
{
    AddItem(ctTypeCheckGroup, _("New check group item"));
}

/// Add a group option item
void ctConfigToolView::OnAddRadioGroupItem(wxCommandEvent& event)
{
    AddItem(ctTypeRadioGroup, _("New radio group item"));
}

/// Add a string item
void ctConfigToolView::OnAddStringItem(wxCommandEvent& event)
{
    AddItem(ctTypeString, _("New string item"));
}

/// Add an item
void ctConfigToolView::AddItem(ctConfigType type, const wxString& msg)
{
    ctConfigItem* sel = GetSelection();
    if (sel)
    {
        wxString name = wxGetTextFromUser(_("Please enter a name for the new item."),
            msg, wxT(""));
        if (!name.IsEmpty())
        {
            ctConfigItem* parent ;
            ctConfigItem* insertBefore ;
            if (sel->CanHaveChildren())
            {
                parent = sel;
                insertBefore = NULL;
            }
            else
            {
                parent = sel->GetParent();
                insertBefore = sel->FindNextSibling();
            }
            
            ctConfigItem* newItem = new ctConfigItem(NULL, type, name);
            newItem->InitProperties();

            newItem->GetDocument()->GetCommandProcessor()->Submit(
                new ctConfigCommand(msg, ctCMD_NEW_ELEMENT, NULL, newItem,
                        parent, insertBefore));
        }
    }
}

/// Delete an item
void ctConfigToolView::OnDeleteItem(wxCommandEvent& event)
{
    ctConfigItem* sel = GetSelection();
    if (sel)
    {
        wxString name(sel->GetName());
        wxString msg;
        msg.Printf(_("Delete %s?"), (const wxChar*) name);
        if (wxYES == wxMessageBox(msg, _("Delete item"), wxICON_QUESTION|wxYES_NO))
        {
            wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->Delete(sel->GetTreeItemId());
        }
    }
}

/// Rename an item
void ctConfigToolView::OnRenameItem(wxCommandEvent& event)
{
    ctConfigItem* sel = GetSelection();
    if (sel)
    {
        wxString name = wxGetTextFromUser(_("Please enter a new name for the item."),
            _("Rename item"), sel->GetName());
        if (!name.IsEmpty())
        {
            sel->SetName(name);
            SyncItem(wxGetApp().GetMainFrame()->GetConfigTreeCtrl(), sel);

            ctConfigToolHint hint(NULL, ctSelChanged);
            GetDocument()->UpdateAllViews (NULL, & hint);
        }
    }
}

/// Copy an item to the clipboard
void ctConfigToolView::OnCopy(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = GetSelection();
    if (sel)
    {
        doc->SetClipboardItem(sel->DeepClone());
    }
}

/// Copy an item to the clipboard and cut the item
void ctConfigToolView::OnCut(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = GetSelection();
    if (sel)
    {
        {
            ctConfigCommand* cmd = new ctConfigCommand(wxT("Cut Config Item"), ctCMD_CUT,
                sel, (ctConfigItem*) NULL);
            doc->GetCommandProcessor()->Submit(cmd);
        }
    }
}

/// Paste an item from the clipboard to the tree
void ctConfigToolView::OnPaste(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = GetSelection();
    if (sel && doc->GetClipboardItem())
    {
        ctConfigItem* parent ;
        ctConfigItem* insertBefore ;
        if (sel->CanHaveChildren())
        {
            parent = sel;
            insertBefore = NULL;
        }
        else
        {
            parent = sel->GetParent();
            insertBefore = sel->FindNextSibling();
        }

        ctConfigItem* newItem = doc->GetClipboardItem()->DeepClone();
        ctConfigCommand* cmd = new ctConfigCommand(wxT("Paste Config Item"), ctCMD_PASTE,
            NULL, newItem, parent, insertBefore);
        doc->GetCommandProcessor()->Submit(cmd);
    }
}

/// Update for copy command
void ctConfigToolView::OnUpdateCopy(wxUpdateUIEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    event.Enable( doc && GetSelection() && GetSelection()->GetParent() );
}

/// Update for cut
void ctConfigToolView::OnUpdateCut(wxUpdateUIEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    event.Enable( doc && GetSelection() && GetSelection()->GetParent() );
}

/// Update for paste
void ctConfigToolView::OnUpdatePaste(wxUpdateUIEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    event.Enable( doc && doc->GetClipboardItem() && GetSelection() );
}

/// Copy an item to the clipboard
void ctConfigToolView::OnContextCopy(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->GetContextItem();
    if (doc && sel)
    {
        doc->SetClipboardItem(sel->DeepClone());
    }
}

/// Copy an item to the clipboard and cut the item
void ctConfigToolView::OnContextCut(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->GetContextItem();
    if (sel)
    {
        {
            ctConfigCommand* cmd = new ctConfigCommand(wxT("Cut Config Item"), ctCMD_CUT,
                sel, (ctConfigItem*) NULL);
            doc->GetCommandProcessor()->Submit(cmd);
        }
    }
}

/// Paste an item from the clipboard to the tree
void ctConfigToolView::OnContextPasteBefore(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->GetContextItem();
    if (sel && doc->GetClipboardItem())
    {
        ctConfigItem* parent = sel->GetParent();
        ctConfigItem* insertBefore = sel;

        ctConfigItem* newItem = doc->GetClipboardItem()->DeepClone();
        ctConfigCommand* cmd = new ctConfigCommand(wxT("Paste Config Item"), ctCMD_PASTE,
            NULL, newItem, parent, insertBefore);
        doc->GetCommandProcessor()->Submit(cmd);
    }
}

/// Paste an item from the clipboard to the tree
void ctConfigToolView::OnContextPasteAfter(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->GetContextItem();
    if (sel && doc->GetClipboardItem())
    {
        ctConfigItem* parent = sel->GetParent();
        ctConfigItem* insertBefore = sel->FindNextSibling();

        ctConfigItem* newItem = doc->GetClipboardItem()->DeepClone();
        ctConfigCommand* cmd = new ctConfigCommand(wxT("Paste Config Item"), ctCMD_PASTE,
            NULL, newItem, parent, insertBefore);
        doc->GetCommandProcessor()->Submit(cmd);
    }
}

/// Paste an item from the clipboard to the tree
void ctConfigToolView::OnContextPasteAsChild(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->GetContextItem();
    if (sel && doc->GetClipboardItem())
    {
        if (sel->CanHaveChildren())
        {
            ctConfigItem* parent = sel;
            ctConfigItem* insertBefore = NULL;
            
            ctConfigItem* newItem = doc->GetClipboardItem()->DeepClone();
            ctConfigCommand* cmd = new ctConfigCommand(wxT("Paste Config Item"), ctCMD_PASTE,
                NULL, newItem, parent, insertBefore);
            doc->GetCommandProcessor()->Submit(cmd);
        }
    }
}

/// Copy an item to the clipboard
void ctConfigToolView::OnUpdateContextCopy(wxUpdateUIEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->GetContextItem();
    event.Enable( doc && sel && sel->GetParent() );
}

/// Copy an item to the clipboard and cut the item
void ctConfigToolView::OnUpdateContextCut(wxUpdateUIEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->GetContextItem();
    event.Enable( doc && sel && sel->GetParent() );
}

/// Paste an item from the clipboard to the tree
void ctConfigToolView::OnUpdateContextPasteBefore(wxUpdateUIEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->GetContextItem();
    event.Enable( doc && sel && sel->GetParent() && doc->GetClipboardItem() );
}

/// Paste an item from the clipboard to the tree
void ctConfigToolView::OnUpdateContextPasteAfter(wxUpdateUIEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->GetContextItem();
    event.Enable( doc && sel && sel->GetParent() && doc->GetClipboardItem() );
}

/// Paste an item from the clipboard to the tree
void ctConfigToolView::OnUpdateContextPasteAsChild(wxUpdateUIEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->GetContextItem();
    event.Enable( doc && sel && sel->CanHaveChildren() && doc->GetClipboardItem() );
}

/// Item help
void ctConfigToolView::OnItemHelp(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    if ( doc && GetSelection() )
    {
        wxString helpTopic = GetSelection()->GetPropertyString(wxT("help-topic"));
        if (!helpTopic.IsEmpty())
        {
            wxGetApp().GetReferenceHelpController().DisplaySection(helpTopic);
        }
    }
}

/// Item help update
void ctConfigToolView::OnUpdateItemHelp(wxUpdateUIEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    event.Enable( doc && GetSelection() );
}

/// Add a custom property
void ctConfigToolView::OnAddCustomProperty(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = GetSelection();
    ctPropertyEditor* editor = wxGetApp().GetMainFrame()->GetPropertyEditor();
    if (doc && sel && editor)
    {
        ctCustomPropertyDialog dialog(wxGetApp().GetMainFrame(),
            -1, _("Add a custom property"));
        if (dialog.ShowModal() == wxID_OK)
        {
            wxString name = dialog.GetPropertyName();
            wxString type = dialog.GetPropertyType();
            wxString descr = dialog.GetPropertyDescription();
            wxString editorType = dialog.GetEditorType();
            wxArrayString choices = dialog.GetChoices();

            if (sel->GetProperties().FindProperty(name))
            {
                wxMessageBox(_("Sorry, this name already exists."), _T("Add custom property"),
                    wxOK|wxICON_INFORMATION);
                return;
            }
            ctProperty* property = new ctProperty;
            if (type == wxT("bool"))
                property->GetVariant() = wxVariant((bool) FALSE, name);
            else if (type == wxT("double"))
                property->GetVariant() = wxVariant((double) 0.0, name);
            else if (type == wxT("long"))
                property->GetVariant() = wxVariant((long) 0, name);
            else
                property->GetVariant() = wxVariant(wxT(""), name);
            property->SetCustom(TRUE);
            property->SetDescription(descr);
            property->SetChoices(choices);
            property->SetEditorType(editorType);

            sel->GetProperties().AddProperty(property);
            editor->ShowItem(sel);
            OnChangeFilename();
        }
    }
}

/// Edit a custom property
void ctConfigToolView::OnEditCustomProperty(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = GetSelection();
    ctPropertyEditor* editor = wxGetApp().GetMainFrame()->GetPropertyEditor();
    if (doc && sel && editor)
    {
        int row;
        ctProperty* property = editor->FindSelectedProperty(row) ;
        if (property && property->IsCustom())
        {
            wxString oldName = property->GetName();
            wxString oldDescription = property->GetDescription();
            wxString oldType = property->GetVariant().GetType();
            wxString oldEditorType = property->GetEditorType();
            wxArrayString oldChoices = property->GetChoices();
            
            ctCustomPropertyDialog dialog(wxGetApp().GetMainFrame(),
                -1, _("Edit custom property"));
            dialog.SetPropertyName(oldName);
            dialog.SetPropertyType(oldType);
            dialog.SetPropertyDescription(oldDescription);
            if (dialog.ShowModal() == wxID_OK)
            {
                wxString name = dialog.GetPropertyName();
                wxString type = dialog.GetPropertyType();
                wxString editorType = dialog.GetEditorType();
                wxArrayString choices = dialog.GetChoices();
                wxString descr = dialog.GetPropertyDescription();
                
                if (name != oldName && sel->GetProperties().FindProperty(name))
                {
                    wxMessageBox(_("Sorry, this name already exists."), _T("Add custom property"),
                        wxOK|wxICON_INFORMATION);
                    return;
                }
                if (type != oldType)
                {
                    if (type == wxT("bool"))
                        property->GetVariant() = wxVariant((bool) FALSE, name);
                    else if (type == wxT("double"))
                        property->GetVariant() = wxVariant((double) 0.0, name);
                    else if (type == wxT("long"))
                        property->GetVariant() = wxVariant((long) 0, name);
                    else
                        property->GetVariant() = wxVariant(wxT(""), name);
                }
                if (name != oldName)
                    property->GetVariant().SetName(name);

                if (choices != oldChoices)
                    property->SetChoices(choices);

                if (editorType != oldEditorType)
                    property->SetEditorType(editorType);

                if (name != oldName)
                    property->GetVariant().SetName(name);

                property->SetCustom(TRUE);

                if (descr != oldDescription)
                    property->SetDescription(descr);
                
                editor->ShowItem(sel);
                OnChangeFilename();
            }
        }
    }
}

/// Delete a custom property
void ctConfigToolView::OnDeleteCustomProperty(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = GetSelection();
    ctPropertyEditor* editor = wxGetApp().GetMainFrame()->GetPropertyEditor();
    if (doc && sel && editor)
    {
        int row;
        ctProperty* property = editor->FindSelectedProperty(row) ;
        if (property && property->IsCustom())
        {
            wxString name(property->GetName());
            wxString msg;
            msg.Printf(_("Delete custom property '%s'?"), (const wxChar*) name);
            if (wxYES == wxMessageBox(msg, _("Delete property"), wxICON_EXCLAMATION|wxYES_NO))
            {
                sel->GetProperties().RemoveProperty(property);
                editor->ShowItem(sel);
                delete property;
                OnChangeFilename();
            }
        }
    }
}

/// Add a custom property: update event
void ctConfigToolView::OnUpdateAddCustomProperty(wxUpdateUIEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    event.Enable( doc && GetSelection() && GetSelection()->GetParent() );
}

/// Edit a custom property: update event
void ctConfigToolView::OnUpdateEditCustomProperty(wxUpdateUIEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = GetSelection();
    ctPropertyEditor* editor = wxGetApp().GetMainFrame()->GetPropertyEditor();
    int row;
    event.Enable( doc && sel && sel->GetParent() && editor &&
                  editor->FindSelectedProperty(row) &&
                  editor->FindSelectedProperty(row)->IsCustom() );
}

/// Delete a custom property: update event
void ctConfigToolView::OnUpdateDeleteCustomProperty(wxUpdateUIEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctConfigItem* sel = GetSelection();
    ctPropertyEditor* editor = wxGetApp().GetMainFrame()->GetPropertyEditor();
    int row;
    event.Enable( doc && sel && sel->GetParent() && editor &&
                  editor->FindSelectedProperty(row) &&
                  editor->FindSelectedProperty(row)->IsCustom() );
}

/// Regenerate setup.h and configure command
void ctConfigToolView::RegenerateSetup()
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    ctOutputWindow* setupPage = wxGetApp().GetMainFrame()->GetSetupPage() ;
    ctOutputWindow* configurePage = wxGetApp().GetMainFrame()->GetConfigurePage() ;

    wxString setupStr = doc->GenerateSetup();
    wxString configureStr = doc->GenerateConfigureCommand();

    setupPage->SetText(setupStr);
    configurePage->SetText(configureStr);
}

/// Regenerate if selected a tab
void ctConfigToolView::OnTabSelect(wxNotebookEvent& event)
{
    if (wxGetApp().GetMainFrame()->GetMainNotebook() != event.GetEventObject())
    {
        event.Skip();
        return;
    }

    if (event.GetSelection() > 0)
    {
        RegenerateSetup();
    }
}

void ctConfigToolView::OnSaveSetupFile(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    wxString setupStr = doc->GenerateSetup();

    wxString filename = _T("setup.h");
    wxString path = doc->GetFrameworkDir(FALSE);
    wxString wildcard = _T("Header files (*.h)|*.h|All files (*.*)|*.*");
    
    wxFileDialog dialog(wxTheApp->GetTopWindow(),
        _("Save Setup File As"),
        path, filename ,
        wildcard, wxSAVE|wxOVERWRITE_PROMPT);
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString fullPath = dialog.GetPath();

        // TODO: save last saved path in settings.

        wxFileOutputStream stream(fullPath);
        if (!stream.Ok())
        {
            wxMessageBox(_("Sorry, could not save this file."), _("Save Setup File"), wxICON_EXCLAMATION|wxOK);
            return;
        }

        stream << setupStr;
    }    
}

void ctConfigToolView::OnSaveConfigureCommand(wxCommandEvent& event)
{
    ctConfigToolDoc* doc = (ctConfigToolDoc*) GetDocument();
    wxString configureStr = doc->GenerateConfigureCommand();

    wxString filename = _T("configurewx.sh");
    wxString path = doc->GetFrameworkDir(FALSE);
    wxString wildcard = _T("Shell script files (*.sh)|*.sh|All files (*.*)|*.*");
    
    wxFileDialog dialog(wxTheApp->GetTopWindow(),
        _("Save Configure Command File As"),
        path, filename ,
        wildcard, wxSAVE|wxOVERWRITE_PROMPT);
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString fullPath = dialog.GetPath();

        // TODO: save last saved path in settings.

        wxFileOutputStream stream(fullPath);
        if (!stream.Ok())
        {
            wxMessageBox(_("Sorry, could not save this file."), _("Save Configure Command File"), wxICON_EXCLAMATION|wxOK);
            return;
        }

        stream << configureStr;
    }    
}

void ctConfigToolView::OnUpdateSaveSetupFile(wxUpdateUIEvent& event)
{
    event.Enable(TRUE);
}

void ctConfigToolView::OnUpdateSaveConfigureCommand(wxUpdateUIEvent& event)
{
    event.Enable(TRUE);
}
