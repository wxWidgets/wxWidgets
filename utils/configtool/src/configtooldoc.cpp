/////////////////////////////////////////////////////////////////////////////
// Name:        configtooldoc.h
// Purpose:     Document class
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "configtooldoc.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP

#include "wx/process.h"
#include "wx/mimetype.h"
#include "wx/process.h"

#endif

#include "wx/textfile.h"
#include "wx/wfstream.h"
#include "wx/config.h"
#include "configtooldoc.h"
#include "configtoolview.h"
#include "configtree.h"
#include "mainframe.h"
#include "utils.h"
#include "wxconfigtool.h"
#include "htmlparser.h"

IMPLEMENT_DYNAMIC_CLASS(ctConfigToolDoc, wxDocument)

// Ctor
ctConfigToolDoc::ctConfigToolDoc()
{
    m_topItem = NULL;
    m_clipboardItem = NULL;
}

// Dtor
ctConfigToolDoc::~ctConfigToolDoc()
{
    DeleteItems();
    ClearClipboard();
    if (GetCommandProcessor())
        GetCommandProcessor()->SetEditMenu(NULL);
}

// Delete all the items not already deleted
void ctConfigToolDoc::DeleteItems()
{
    if (m_topItem)
        delete m_topItem;
    m_topItem = NULL;
}

/// Clears the clipboard item.
void ctConfigToolDoc::ClearClipboard()
{
    if (m_clipboardItem)
        delete m_clipboardItem;
    m_clipboardItem = NULL;
}

/// Sets the clipboard item.
void ctConfigToolDoc::SetClipboardItem(ctConfigItem* item)
{
    if (m_clipboardItem)
        delete m_clipboardItem;
    m_clipboardItem = item;
}


// Closes and clears the document
bool ctConfigToolDoc::OnCloseDocument()
{
    if (wxDocument::OnCloseDocument())
    {
        ctConfigToolHint hint(NULL, ctClear);
        UpdateAllViews (NULL, & hint);

        DeleteItems();
        return true;
    }
    else
    {
        return false;
    }
}

// Saves the doc
bool ctConfigToolDoc::Save()
{
    if (!IsModified() && m_savedYet) return true;

    bool ret = (m_documentFile == wxT("") || !m_savedYet) ?
                 SaveAs() :
                 OnSaveDocument(m_documentFile);
    if ( ret )
        SetDocumentSaved(true);
    return ret;
}

// Create the document
bool ctConfigToolDoc::OnCreate(const wxString& path, long flags)
{
    GetCommandProcessor()->SetEditMenu(wxGetApp().GetMainFrame()->GetEditMenu());
    GetCommandProcessor()->Initialize();
    GetCommandProcessor()->ClearCommands();

    // wxGetApp().m_currentDoc = this;

    if (flags & wxDOC_NEW)
    {
        ctConfigItem* rootItem = new ctConfigItem(NULL, ctTypeGroup, _T("Configuration"));
        //rootItem->InitProperties();
        rootItem->GetProperties().AddProperty(
        new ctProperty(
        wxT("The item description."),
        wxVariant(wxT(""), wxT("description")),
        wxT("multiline")));

        rootItem->SetPropertyString(_T("description"),
            _T("<B>This is the top-level configuration item.</B>"));


        SetTopItem(rootItem);

        Modify(false);
        SetDocumentSaved(false);

        wxString rootName(wxT("untitled"));
        wxStripExtension(rootName);
        SetFilename(wxGetApp().GetSettings().GenerateFilename(rootName));
    }

    // Creates the view, so do any view updating after this
    bool success = wxDocument::OnCreate(path, flags);

    if (success)
    {
        if (flags & wxDOC_NEW)
        {
            wxBusyCursor wait;

            ctConfigToolHint hint(NULL, ctInitialUpdate);
            UpdateAllViews (NULL, & hint);

            SetFilename(GetFilename(), true);
        }
    }
    return success;
}

// Save the document
bool ctConfigToolDoc::OnSaveDocument(const wxString& filename)
{
    wxBusyCursor cursor;

    const wxString strOldPath(GetFilename());

    // Do some backing up first

    // This is the backup filename
    wxString backupFilename(filename);
    backupFilename += wxT(".bak");
    
    // This is the temporary copy of the backup
    wxString tempFilename(filename);
    tempFilename += wxT(".tmp");
    if (wxFileExists(tempFilename))
        wxRemoveFile(tempFilename);

    bool leaveBackup = true;

    bool saved = DoSave(tempFilename);

    if (saved)
    {
        // Remove the old .bak file
        if (wxFileExists(backupFilename))
        {
            wxRemoveFile(backupFilename);
        }
        
        // Copy the old file to the .bak

        if (leaveBackup)
        {
            if (wxFileExists(filename))
            {
                if (!wxRenameFile(filename, backupFilename))
                {
                    wxCopyFile(filename, backupFilename);
                    wxRemoveFile(filename);
                }
            }
        }
        else
        {
            if (wxFileExists(filename))
                wxRemoveFile(filename);
        }
        
        // Finally, copy the temporary file to the proper filename
        if (!wxRenameFile(tempFilename, filename))
        {
            wxCopyFile(tempFilename, filename);
            wxRemoveFile(tempFilename);
        }        

        Modify(false);
        ((ctConfigToolView*)GetFirstView())->OnChangeFilename();
        SetDocumentSaved(true);
        SetFilename(filename);
        wxGetApp().GetSettings().m_lastFilename = filename;
    } else
    {
        SetFilename(strOldPath);
    }
    wxGetApp().GetMainFrame()->UpdateFrameTitle();
    return saved;
}

// Open the document
bool ctConfigToolDoc::OnOpenDocument(const wxString& filename)
{
    wxBusyCursor cursor;

    bool opened = DoOpen(filename);

    if (opened)
    {
        SetFilename(filename);
        wxGetApp().GetSettings().m_lastFilename = filename;

        ((ctConfigToolView*)GetFirstView())->OnChangeFilename();

        RefreshDependencies();

        // ctConfigToolHint hint(NULL, ctFilenameChanged);
        ctConfigToolHint hint(NULL, ctInitialUpdate);
        UpdateAllViews (NULL, & hint);
    }

    SetDocumentSaved(true); // Necessary or it will pop up the Save As dialog

    return opened;
}

/// Save the settings file
bool ctConfigToolDoc::DoSave(const wxString& filename)
{
    wxFileOutputStream stream(filename);
    if (!stream.Ok())
        return false;

    stream << wxT("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    stream << wxT("<settings xmlns=\"http://www.wxwidgets.org/wxs\" version=\"2.5.0.1\">");

    DoSave(m_topItem, stream, 1);

    stream << wxT("\n</settings>\n");

    return true;
}

inline static void OutputIndentation(wxOutputStream& stream, int indent)
{
    wxString str = wxT("\n");
    for (int i = 0; i < indent; i++)
        str << wxT(' ') << wxT(' ');
    stream << str ;
}

/// Recursive helper function for file saving
bool ctConfigToolDoc::DoSave(ctConfigItem* item, wxOutputStream& stream, int indent)
{
    OutputIndentation(stream, indent*2);

    wxString name(item->GetName());
    wxString s;
    wxString typeStr;
    if (item->GetType() == ctTypeGroup)
        typeStr = wxT("group");
    else if (item->GetType() == ctTypeCheckGroup)
        typeStr = wxT("check-group");
    else if (item->GetType() == ctTypeRadioGroup)
        typeStr = wxT("radio-group");
    else if (item->GetType() == ctTypeString)
        typeStr = wxT("string");
    else if (item->GetType() == ctTypeBoolCheck)
        typeStr = wxT("bool-check");
    else if (item->GetType() == ctTypeBoolRadio)
        typeStr = wxT("bool-radio");
    else if (item->GetType() == ctTypeInteger)
        typeStr = wxT("integer");
    else
        typeStr = wxT("unknown");

    stream << wxT("<setting type=\"") << typeStr << wxT("\">");

    indent ++;

    OutputIndentation(stream, indent*2);
    if (item->IsActive())
        stream << wxT("<active>1</active>");
    else
        stream << wxT("<active>0</active>");
    OutputIndentation(stream, indent*2);
    if (item->IsEnabled())
        stream << wxT("<enabled>1</enabled>");
    else
        stream << wxT("<enabled>0</enabled>");

    // Output properties
    wxObjectList::compatibility_iterator node = item->GetProperties().GetList().GetFirst();
    while (node)
    {
        ctProperty* prop = (ctProperty*) node->GetData();
        OutputIndentation(stream, indent*2);
        stream << wxT("<") << prop->GetName() ;
        
        if (prop->IsCustom())
        {
            stream << wxT(" custom=\"true\"");
            stream << wxT(" type=\"") << prop->GetVariant().GetType() << wxT("\"");
            stream << wxT(" editor-type=\"") << prop->GetEditorType() << wxT("\"");
            stream << wxT(" description=\"") << prop->GetDescription() << wxT("\"");
            if (prop->GetChoices().GetCount() > 0)
            {
                wxString choices;
                ctConfigItem::ArrayToString(prop->GetChoices(), choices);
                stream << wxT(" choices=\"") << choices << wxT("\"");
            }
        }
        
        stream << wxT(">");

        stream << ctEscapeHTMLCharacters(prop->GetVariant().GetString()) ;
        stream << wxT("</") << prop->GetName() << wxT(">");

        node = node->GetNext();
    }

    // Output children
    node = item->GetChildren().GetFirst();
    while (node)
    {
        ctConfigItem* child = (ctConfigItem*) node->GetData();
        DoSave(child, stream, indent);

        node = node->GetNext();
    }

    indent --;

    OutputIndentation(stream, indent*2);
    stream << wxT("</setting>");

    return true;
}

/// Open the settings file
bool ctConfigToolDoc::DoOpen(const wxString& filename)
{
    wxSimpleHtmlParser parser;
    if (parser.ParseFile(filename))
    {
        ctConfigToolHint hint(NULL, ctClear);
        UpdateAllViews (NULL, & hint);
        m_topItem = NULL;

        if (parser.GetTopLevelTag()->GetChildren())
        {
            wxSimpleHtmlTag* settingsTag = parser.GetTopLevelTag()->GetChildren()->FindTag(wxT("settings"));
            if (settingsTag && settingsTag->GetChildren())
            {
                wxSimpleHtmlTag* firstSettingTag = settingsTag->GetChildren();
                if (firstSettingTag)
                    DoOpen(firstSettingTag, NULL);
                return true;
            }
            return true;
        }
    }
    return false;
}

static bool GetHtmlBoolValue(const wxString& value)
{
    if (value == wxT("true") || value == wxT("TRUE") || value == wxT("1"))
        return true;
     else
         return false;
}

static int GetHtmlIntegerValue(const wxString& value)
{
    return wxAtoi(value);
}

static double GetHtmlDoubleValue(const wxString& value)
{
    return wxAtof(value);
}

bool ctConfigToolDoc::DoOpen(wxSimpleHtmlTag* tag, ctConfigItem* parent)
{
    ctConfigItem* newItem = NULL;
    if (tag->NameIs(wxT("setting")))
    {
        wxSimpleHtmlAttribute* attr = tag->FindAttribute(wxT("type"));
        if (attr)
        {
            ctConfigType type = ctTypeUnknown;
            wxString typeStr(attr->GetValue());
            if (typeStr == wxT("group"))
                type = ctTypeGroup;
            else if (typeStr == wxT("option-group") || typeStr == wxT("check-group"))
                type = ctTypeCheckGroup;
            else if (typeStr == wxT("radio-group"))
                type = ctTypeRadioGroup;
            else if (typeStr == wxT("bool-check"))
                type = ctTypeBoolCheck;
            else if (typeStr == wxT("bool-radio"))
                type = ctTypeBoolRadio;
            else if (typeStr == wxT("string"))
                type = ctTypeString;
            else if (typeStr == wxT("integer"))
                type = ctTypeInteger;
            else
            {
                wxLogError(wxT("Unknown type %s"), (const wxChar*) typeStr);
            }
            if (type != ctTypeUnknown)
            {
                newItem = new ctConfigItem(parent, type, wxT(""));
                newItem->InitProperties();
                if (!parent)
                    SetTopItem(newItem);
            }
        }
    }
    wxSimpleHtmlTag* childTag = tag->GetChildren();

    while (childTag)
    {
        if (childTag->GetType() == wxSimpleHtmlTag_Open)
        {
            if (childTag->GetName() == wxT("setting"))
            {
                DoOpen(childTag, newItem);
            }
            else if (childTag->GetName() == wxT("name"))
            {
                if (newItem)
                {
                    wxString name(childTag->GetNext()->GetTagText());
                    newItem->SetName(name);
                }
            }
            else if (childTag->GetName() == wxT("active"))
            {
                if (newItem)
                    newItem->SetActive(GetHtmlBoolValue(childTag->GetNext()->GetTagText()));
            }
            else if (childTag->GetName() == wxT("enabled"))
            {
                if (newItem)
                    newItem->Enable(GetHtmlBoolValue(childTag->GetNext()->GetTagText()));
            }
            else
            {
                if (newItem)
                {
                    ctProperty* prop = newItem->GetProperties().FindProperty(childTag->GetName());
                    if (!prop)
                    {
                        // A custom property, else an obsolete
                        // property that we should ignore.
                        wxString isCustom;
                        if (childTag->GetAttributeValue(isCustom, wxT("custom")) &&
                            isCustom == wxT("true"))
                        {
                            prop = new ctProperty;

                            wxString name(childTag->GetName());
                            wxString type(wxT("string"));
                            wxString choices;
                            wxString editorType(wxT("string"));
                            wxString description(wxT(""));
                            childTag->GetAttributeValue(type, wxT("type"));
                            childTag->GetAttributeValue(type, wxT("editor-type"));
                            childTag->GetAttributeValue(type, wxT("choices"));
                            childTag->GetAttributeValue(description, wxT("description"));

                            if (type == wxT("bool"))
                                prop->GetVariant() = wxVariant(false, name);
                            else if (type == wxT("double"))
                                prop->GetVariant() = wxVariant((double) 0.0, name);
                            else if (type == wxT("long"))
                                prop->GetVariant() = wxVariant((long) 0, name);
                            else
                                prop->GetVariant() = wxVariant(wxT(""), name);
                            prop->SetDescription(description);
                            prop->SetCustom(true);
                            prop->SetEditorType(editorType);
                            if (!choices.IsEmpty())
                            {
                                wxArrayString arr;
                                ctConfigItem::StringToArray(choices, arr);
                                prop->SetChoices(arr);
                            }
                            newItem->GetProperties().AddProperty(prop);
                        }
                    }
                    if (prop)
                    {
                        if (prop->GetVariant().GetType() == wxT("string"))
                            prop->GetVariant() = childTag->GetNext()->GetTagText();
                        else if (prop->GetVariant().GetType() == wxT("long"))
                            prop->GetVariant() = (long) GetHtmlIntegerValue(childTag->GetNext()->GetTagText());
                        else if (prop->GetVariant().GetType() == wxT("bool"))
                            prop->GetVariant() = GetHtmlBoolValue(childTag->GetNext()->GetTagText());
                        else if (prop->GetVariant().GetType() == wxT("double"))
                            prop->GetVariant() = (double) GetHtmlDoubleValue(childTag->GetNext()->GetTagText());
                    }
                }
            }
        }
        childTag = childTag->GetNext();
    }
    return true;
}

/// Clear dependencies
void ctConfigToolDoc::ClearDependencies(ctConfigItem* item)
{
    if (!item)
        item = GetTopItem();

    item->GetDependents().Clear();
    for ( wxObjectList::compatibility_iterator node = item->GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        ctConfigItem* child = (ctConfigItem*) node->GetData();
        ClearDependencies(child);
    }
}

/// Refresh dependencies
void ctConfigToolDoc::RefreshDependencies()
{
    ClearDependencies(GetTopItem());
    RefreshDependencies(GetTopItem());
}

/// Refresh dependencies
void ctConfigToolDoc::RefreshDependencies(ctConfigItem* item)
{
    wxArrayString requiresArr;
    wxString requires = item->GetPropertyString(wxT("requires"));
    wxString precludes = item->GetPropertyString(wxT("precludes"));
    wxString enabledIf = item->GetPropertyString(wxT("enabled-if"));
    wxString enabledIfNot = item->GetPropertyString(wxT("enabled-if-not"));
    wxString indeterminateIf = item->GetPropertyString(wxT("indeterminate-if"));
    wxString context = item->GetPropertyString(wxT("context"));

    if (!requires.IsEmpty())
        item->StringToArray(requires, requiresArr);

    if (!precludes.IsEmpty())
        item->StringToArray(precludes, requiresArr);

    if (!enabledIfNot.IsEmpty())
        item->StringToArray(enabledIfNot, requiresArr);

    if (!enabledIf.IsEmpty())
        item->StringToArray(enabledIf, requiresArr);

    if (!indeterminateIf.IsEmpty())
        item->StringToArray(indeterminateIf, requiresArr);

    // Add the parent to the list of dependencies, if the
    // parent is a check or radio group.
    ctConfigItem* parent = item->GetParent();
    if (parent &&
        (parent->GetType() == ctTypeCheckGroup || 
        parent->GetType() == ctTypeRadioGroup))
        requiresArr.Add(parent->GetName());

    // Also look in 'context' since these items
    // are another kind of dependency (switching to
    // a different platform may cause the dependencies
    // to be evaluated differently).
    if (!context.IsEmpty())
        item->StringToArray(context, requiresArr);

    size_t i;
    for (i = 0; i < requiresArr.GetCount(); i++)
    {
        wxString itemName(requiresArr[i]);
        ctConfigItem* otherItem = GetTopItem()->FindItem(itemName);
        if (otherItem && !otherItem->GetDependents().Member(item))
        {
            otherItem->GetDependents().Append(item);
        }
    }
    for ( wxObjectList::compatibility_iterator node = item->GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        ctConfigItem* child = (ctConfigItem*) node->GetData();
        RefreshDependencies(child);
    }
}

/// Generate the text of a setup.h
wxString ctConfigToolDoc::GenerateSetup()
{
    wxString str;
    str << wxT("/*\n * setup.h\n * Generated by wxConfigTool\n *\n */\n\n");

    GenerateSetup(GetTopItem(), str);

    return str;
}

/// Helper function
void ctConfigToolDoc::GenerateSetup(ctConfigItem* item, wxString& str)
{
    // Generate the setup.h entries for this item
    wxString name = item->GetName();

    // We don't process the platform choice
    if (item->GetName() == wxT("Target"))
        return;

    if (item->IsInActiveContext() &&
        (item->GetType() == ctTypeCheckGroup ||
        item->GetType() == ctTypeRadioGroup ||
        item->GetType() == ctTypeBoolCheck ||
        item->GetType() == ctTypeBoolRadio))
    {
        // TODO: write description
        wxString name = item->GetName();
        if (name.Left(6) == wxT("wxUSE_") ||
            name == wxT("REMOVE_UNUSED_ARG") || // Hack alert: change to wxUSE_UNUSED_ARG_REMOVAL
            name.Find(wxT("COMPATIBILITY")) != wxNOT_FOUND)
        {
            str << wxT("#define ") << name ;
            if (item->IsEnabled())
                str << wxT(" 1");
            else
                str << wxT(" 0");
            str << wxT("\n\n");
        }
    }

    for ( wxObjectList::compatibility_iterator node = item->GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        ctConfigItem* child = (ctConfigItem*) node->GetData();
        GenerateSetup(child, str);
    }
}


/// Generate a configure command
wxString ctConfigToolDoc::GenerateConfigureCommand()
{
    wxString str;
    str << wxT("# configurewx\n# Generated by wxConfigTool\n\n");

    wxString path = GetFrameworkDir(true);
    bool makeUnix = true;
    if (!path.IsEmpty())
    {
        if (makeUnix)
            path += wxT("/");
        else
            path += wxFILE_SEP_PATH ;
    }

    str << path << wxT("configure");

    // Find the target to use
    ctConfigItem* platformsFolder = GetTopItem()->FindItem(wxT("Target"));
    if (platformsFolder)
    {
        for ( wxObjectList::compatibility_iterator node = platformsFolder->GetChildren().GetFirst(); node; node = node->GetNext() )
        {
            ctConfigItem* child = (ctConfigItem*) node->GetData();
            if (child->GetType() == ctTypeBoolRadio && child->IsEnabled())
            {
                wxString configureCommand = child->GetPropertyString(wxT("configure-command"));
                if (!configureCommand.IsEmpty())
                    str << wxT(" ") << configureCommand;
            }
        }
    }

    GenerateConfigureCommand(GetTopItem(), str);
    return str;
}

/// Helper function
void ctConfigToolDoc::GenerateConfigureCommand(ctConfigItem* item, wxString& str)
{
    // We don't process the platform group, since we've
    // already done so.
    if (item->GetName() == wxT("Target"))
        return;

    if (item->IsInActiveContext() &&
        (item->GetType() == ctTypeCheckGroup ||
        item->GetType() == ctTypeRadioGroup ||
        item->GetType() == ctTypeBoolCheck ||
        item->GetType() == ctTypeBoolRadio))
    {
        wxString name = item->GetName();
        wxString configureCommand = item->GetPropertyString(wxT("configure-command"));
        if (!configureCommand.IsEmpty())
        {
            if (!item->IsEnabled())
            {
                // Replace 'enable' with 'disable' if this
                // option is off.
                configureCommand.Replace(wxT("--enable-"), wxT("--disable-"));
                configureCommand.Replace(wxT("--with-"), wxT("--without-"));
            }
            ctProperty* prop = item->GetProperties().FindProperty(wxT("builtin"));
            if (prop && prop->GetVariant().GetType() == wxT("bool"))
            {
                bool builtin = prop->GetVariant().GetBool();
                str << wxT(" ") << configureCommand;
                if (builtin)
                    str << wxT("=builtin");
                else
                    str << wxT("=sys");
            }
            else
            {
                ctProperty* prop = item->GetProperties().FindProperty(wxT("value"));
                if (prop && prop->GetVariant().GetType() == wxT("string"))
                {
                    wxString val = prop->GetVariant().GetString();
                    if (item->IsEnabled() && !val.IsEmpty())
                    {
                        str << wxT(" ") << configureCommand;
                        str << wxT("=\"") << val << wxT("\"");
                    }
                    // If the string is empty, ignore this parameter,
                    // since it's obviously intended to be supplied
                    // only if there's a string to use and it's enabled.
                }
                else
                {
                    str << wxT(" ") << configureCommand;
                }
            }            
        }
    }

    for ( wxObjectList::compatibility_iterator node = item->GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        ctConfigItem* child = (ctConfigItem*) node->GetData();
        GenerateConfigureCommand(child, str);
    }
}

/// Gets the current framework directory
wxString ctConfigToolDoc::GetFrameworkDir(bool makeUnix)
{
    wxString path = wxGetApp().GetSettings().m_frameworkDir;
    if (wxGetApp().GetSettings().m_useEnvironmentVariable)
    {
        // Should probably allow other variables
        // to be used, and maybe expand variables within m_frameworkDir
        wxString pathEnv(wxGetenv(wxT("WXWIN")));
        path = pathEnv;
#ifdef __WXMSW__
        if (makeUnix)
            path.Replace(wxT("\\"), wxT("/"));
#endif
    }
    return path;
}

/// Finds the next item in the tree
ctConfigItem* ctConfigToolDoc::FindNextItem(ctConfigItem* item, bool wrap)
{
    if (!item)
        return GetTopItem();

    // First, try to find the first child
    if (item->GetChildCount() > 0)
    {
        return item->GetChild(0);
    }
    else
    {
        ctConfigItem* p = item;
        while (p)
        {
            ctConfigItem* toFind = FindNextSibling(p);
            if (toFind)
                return toFind;
            p = p->GetParent();
        }
    }

    // Finally, wrap around to the root.
    if (wrap)
        return GetTopItem();
    else
        return NULL;
}

/// Finds the next sibling in the tree
ctConfigItem* ctConfigToolDoc::FindNextSibling(ctConfigItem* item)
{
    if (item->GetParent())
    {
        wxObjectList::compatibility_iterator node = item->GetParent()->GetChildren().Member(item);
        if (node && node->GetNext())
        {
            ctConfigItem* nextItem = (ctConfigItem*) node->GetNext()->GetData();
            return nextItem;
        }
    }
    return NULL;
}


/*
 * Implements a document editing command.
 */

ctConfigCommand::ctConfigCommand(const wxString& name, int cmdId,
        ctConfigItem* activeState, ctConfigItem* savedState,
        ctConfigItem* parent, ctConfigItem* insertBefore,
        bool ignoreFirstTime): wxCommand(true, name)
{
    m_activeState = activeState;
    m_savedState = savedState;
    m_ignoreThis = ignoreFirstTime;
    m_cmdId = cmdId;
    m_properties = NULL;
    m_parent = parent;
    m_insertBefore = insertBefore;
}

ctConfigCommand::ctConfigCommand(const wxString& name, int cmdId,
        ctConfigItem* activeState,  ctProperties* properties,
        bool ignoreFirstTime): wxCommand(true, name)
{
    m_activeState = activeState;
    m_savedState = NULL;
    m_properties = properties;
    m_ignoreThis = ignoreFirstTime;
    m_cmdId = cmdId;
    m_properties = properties;
    m_parent = NULL;
    m_insertBefore = NULL;
}

ctConfigCommand::~ctConfigCommand()
{
    if (m_savedState)
        delete m_savedState;
    if (m_properties)
        delete m_properties;
}

bool ctConfigCommand::Do()
{
    return DoAndUndo(true);
}

bool ctConfigCommand::Undo()
{
    return DoAndUndo(false);
}

// Combine Do and Undo into one
bool ctConfigCommand::DoAndUndo(bool doCmd)
{
    switch (m_cmdId)
    {
    case ctCMD_CUT:
        {
            if (doCmd)
            {
                wxASSERT(m_savedState == NULL);
                wxASSERT(m_activeState != NULL);

                ctConfigItem* newItem = m_activeState->DeepClone();
                ctConfigToolDoc* doc = m_activeState->GetDocument();

                // This will delete the old clipboard contents, if any.
                doc->SetClipboardItem(newItem);
                
                m_parent = m_activeState->GetParent();
                m_insertBefore = m_activeState->FindNextSibling();

                m_activeState->Detach();
                m_savedState = m_activeState;
                m_activeState = NULL;

                m_savedState->GetDocument()->Modify(true);
                ctConfigToolView* view = (ctConfigToolView*) m_savedState->GetDocument()->GetFirstView();
                view->OnChangeFilename();
            }
            else
            {
                wxASSERT(m_savedState != NULL);
                wxASSERT(m_activeState == NULL);

                m_savedState->GetDocument()->Modify(true);
                m_savedState->Attach(m_parent, m_insertBefore);
                ctConfigToolView* view = (ctConfigToolView*) m_savedState->GetDocument()->GetFirstView();
                view->AddItems(wxGetApp().GetMainFrame()->GetConfigTreeCtrl(), m_savedState);
                m_activeState = m_savedState;
                m_savedState = NULL;
                m_parent = NULL;
                m_insertBefore = NULL;
                view->OnChangeFilename();
            }
            break;
        }
    case ctCMD_PASTE:
        {
            if (doCmd)
            {
                wxASSERT(m_savedState != NULL);
                wxASSERT(m_activeState == NULL);

                m_savedState->GetDocument()->Modify(true);
                m_savedState->Attach(m_parent, m_insertBefore);
                ctConfigToolView* view = (ctConfigToolView*) m_savedState->GetDocument()->GetFirstView();
                view->AddItems(wxGetApp().GetMainFrame()->GetConfigTreeCtrl(), m_savedState);
                wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->SelectItem(m_savedState->GetTreeItemId());
                m_activeState = m_savedState;
                m_savedState = NULL;
                view->OnChangeFilename();
            }
            else
            {
                wxASSERT(m_savedState == NULL);
                wxASSERT(m_activeState != NULL);

                m_activeState->GetDocument()->Modify(true);
                ctConfigToolView* view = (ctConfigToolView*) m_activeState->GetDocument()->GetFirstView();
                m_activeState->Detach();
                m_savedState = m_activeState;
                m_activeState = NULL;
                view->OnChangeFilename();
            }
            break;
        }
    case ctCMD_NEW_ELEMENT:
        {
            if (doCmd)
            {
                wxASSERT(m_savedState != NULL);
                wxASSERT(m_activeState == NULL);

                m_savedState->GetDocument()->Modify(true);
                m_savedState->Attach(m_parent, m_insertBefore);
                ctConfigToolView* view = (ctConfigToolView*) m_savedState->GetDocument()->GetFirstView();
                view->AddItems(wxGetApp().GetMainFrame()->GetConfigTreeCtrl(), m_savedState);
                wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->SelectItem(m_savedState->GetTreeItemId());

                m_activeState = m_savedState;
                m_savedState = NULL;
            }
            else
            {
                wxASSERT(m_savedState == NULL);
                wxASSERT(m_activeState != NULL);

                m_activeState->GetDocument()->Modify(true);
                m_activeState->Detach();
                m_savedState = m_activeState;
                m_activeState = NULL;
            }
            break;
        }
    case ctCMD_APPLY_PROPERTY:
        {
            wxASSERT(m_properties != NULL);
            wxASSERT(m_activeState != NULL);

            // Don't update the properties editor first time
            // around since it will be done one property at a time
            // initially (and no property editor update required)
            if (!m_ignoreThis)
            {
                // Just swap the saved and current properties.
                ctProperties propsTemp = m_activeState->GetProperties() ;
                m_activeState->GetProperties() = (* m_properties);
                (* m_properties) = propsTemp;

                // Apply only those that need applying
                // (those properties in activeState that are not in propsTemp)
                wxObjectList::compatibility_iterator node = m_activeState->GetProperties().GetList().GetFirst();
                while (node)
                {
                    ctProperty* prop = (ctProperty*) node->GetData();
                    ctProperty* otherProp = propsTemp.FindProperty(prop->GetName());
                    if (otherProp && ((*prop) != (*otherProp)))
                    {
                        m_activeState->ApplyProperty(prop, otherProp->GetVariant());
                    }
                    node = node->GetNext();
                }
                m_activeState->GetDocument()->Modify(true);
                ctConfigToolView* view = (ctConfigToolView*) m_activeState->GetDocument()->GetFirstView();
                if (view)
                {
                    ctConfigToolHint hint(NULL, ctValueChanged);
                    m_activeState->GetDocument()->UpdateAllViews (NULL, & hint);
                }
            }
            m_ignoreThis = false;

            break;
        }
    }
    return true;
}

IMPLEMENT_CLASS(ctConfiguration, wxObject)

ctConfiguration::ctConfiguration()
{
    m_treeItemId = wxTreeItemId();
    m_parent = NULL;
    m_topItem = NULL;
}

ctConfiguration::ctConfiguration(ctConfiguration* parent, const wxString& name)
{
    m_treeItemId = wxTreeItemId();
    SetName(name);
    m_parent = parent;
    if (parent)
        parent->AddChild(this);
}

ctConfiguration::~ctConfiguration()
{
/*
    ctConfigTreeCtrl* treeCtrl = wxGetApp().GetMainFrame()->GetConfigTreeCtrl();
    if (m_treeItemId.IsOk() && treeCtrl)
    {
        ctTreeItemData* data = (ctTreeItemData*) treeCtrl->GetItemData(m_treeItemId);
        if (data)
            data->SetConfigItem(NULL);
    }
    if (GetParent())
        GetParent()->RemoveChild(this);
    else
    {
        if (wxGetApp().GetMainFrame()->GetDocument() &&
            wxGetApp().GetMainFrame()->GetDocument()->GetTopItem() == this)
            wxGetApp().GetMainFrame()->GetDocument()->SetTopItem(NULL);
    }
*/
    
    Clear();
}

/// Assignment operator.
void ctConfiguration::operator= (const ctConfiguration& configuration)
{
    m_name = configuration.m_name;
    m_description = configuration.m_description;
}

/// Clear children
void ctConfiguration::Clear()
{
    wxObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxObjectList::compatibility_iterator next = node->GetNext();
        ctConfiguration* child = (ctConfiguration*) node->GetData();

        // This should delete 'node' too, assuming
        // child's m_parent points to 'this'. If not,
        // it'll be cleaned up by m_children.Clear().
        delete child;

        node = next;
    }
    m_children.Clear();
}

// Get the nth child
ctConfiguration* ctConfiguration::GetChild(int n) const
{
    wxASSERT ( n < GetChildCount() && n > -1 );

    if ( n < GetChildCount() && n > -1 )
    {
        ctConfiguration* child = wxDynamicCast(m_children.Item(n)->GetData(), ctConfiguration);
        return child;
    }
    else
        return NULL;
}

// Get the child count
int ctConfiguration::GetChildCount() const
{
    return m_children.GetCount();
}

/// Add a child
void ctConfiguration::AddChild(ctConfiguration* configuration)
{
    m_children.Append(configuration);
    configuration->SetParent(this);
}

/// Remove (but don't delete) a child
void ctConfiguration::RemoveChild(ctConfiguration* configuration)
{
    m_children.DeleteObject(configuration);
    configuration->SetParent(NULL);
}

/// Get the associated document (currently, assumes
/// there's only ever one document active)
ctConfigToolDoc* ctConfiguration::GetDocument()
{
    ctConfigToolDoc* doc = wxGetApp().GetMainFrame()->GetDocument();
    return doc;
}

/// Find an item in this hierarchy
// TODO: ensure that names are unique, somehow.
ctConfiguration* ctConfiguration::FindConfiguration(const wxString& name)
{
    if (GetName() == name)
        return this;

    for ( wxObjectList::compatibility_iterator node = GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        ctConfiguration* child = (ctConfiguration*) node->GetData();
        ctConfiguration* found = child->FindConfiguration(name);
        if (found)
            return found;
    }
    return NULL;
}

/// Find the next sibling
ctConfiguration* ctConfiguration::FindNextSibling()
{
    if (!GetParent())
        return NULL;
    wxObjectList::compatibility_iterator node = GetParent()->GetChildren().Member(this);
    if (node && node->GetNext())
    {
        return (ctConfiguration*) node->GetNext()->GetData();
    }
    return NULL;
}

/// Find the previous sibling
ctConfiguration* ctConfiguration::FindPreviousSibling()
{
    if (!GetParent())
        return NULL;
    wxObjectList::compatibility_iterator node = GetParent()->GetChildren().Member(this);
    if (node && node->GetPrevious())
    {
        return (ctConfiguration*) node->GetPrevious()->GetData();
    }
    return NULL;
}

/// Create a clone of this and children
ctConfiguration* ctConfiguration::DeepClone()
{
    ctConfiguration* newItem = Clone();

    for ( wxObjectList::compatibility_iterator node = GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        ctConfiguration* child = (ctConfiguration*) node->GetData();
        ctConfiguration* newChild = child->DeepClone();
        newItem->AddChild(newChild);
    }
    return newItem;
}

/// Detach: remove from parent, and remove tree items
void ctConfiguration::Detach()
{
    // TODO
    if (GetParent())
        GetParent()->RemoveChild(this);
    else
        GetDocument()->SetTopItem(NULL);
    SetParent(NULL);

/*
    wxTreeItemId treeItem = GetTreeItemId();

    DetachFromTree();

    // Will delete the branch, but not the config items.
    wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->Delete(treeItem);
*/
}

/// Hide from tree: make sure tree deletions won't delete
/// the config items
void ctConfiguration::DetachFromTree()
{
/*
    wxTreeItemId item = GetTreeItemId();

    // TODO
    ctTreeItemData* data = (ctTreeItemData*) wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->GetItemData(item);
    data->SetConfigItem(NULL);
    m_treeItemId = wxTreeItemId();

    for ( wxNode* node = GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        ctConfiguration* child = (ctConfiguration*) node->GetData();
        child->DetachFromTree();
    }
*/
}

        
