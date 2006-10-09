/////////////////////////////////////////////////////////////////////////////
// Name:        _about.i
// Purpose:     SWIG interface for wxAboutDialog
//
// Author:      Robin Dunn
//
// Created:     08-Oct-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module

%{
#include <wx/aboutdlg.h>
%}

//---------------------------------------------------------------------------
%newgroup


DocStr(wxAboutDialogInfo,
"`wx.AboutDialogInfo contains information shown in the standard About
dialog displayed by the `wx.AboutBox` function.  This class contains
the general information about the program, such as its name, version,
copyright and so on, as well as lists of the program developers,
documentation writers, artists and translators.

While all the main platforms have a native implementation of the about
dialog, they are often more limited than the generic version provided
by wxWidgets and so the generic version is used if
`wx.AboutDialogInfo` has any fields not supported by the native
version. Currently GTK+ version supports all the possible fields
natively but MSW and Mac versions don't support URLs, licence text nor
custom icons in the about dialog and if either of those is used,
wxAboutBox() will automatically use the generic version so you should
avoid specifying these fields to achieve more native look and feel.
", "");

class wxAboutDialogInfo
{
public:
    wxAboutDialogInfo();
    ~wxAboutDialogInfo();

    DocDeclStr(
        void , SetName(const wxString& name),
        "Set the name of the program. If this method is not called, the string
returned by `wx.App.GetAppName` will be shown in the dialog.", "");
    
    DocDeclStr(
        wxString , GetName() const,
        "Returns the program name.", "");
    
    %property(Name, GetName, SetName);

    DocDeclStr(
        void , SetVersion(const wxString& version),
        "Set the version of the program. The version is in free format,
i.e. not necessarily in the x.y.z form but it shouldn't contain the
\"version\" word.", "");
    
    DocDeclStr(
        bool , HasVersion() const,
        "Returns ``True`` if the version property has been set.", "");
    
    DocDeclStr(
        wxString , GetVersion() const,
        "Returns the version value.", "");
    
    %property(Version, GetVersion, SetVersion);
    

    DocDeclStr(
        void , SetDescription(const wxString& desc),
        "Set brief, but possibly multiline, description of the program.", "");
    
    DocDeclStr(
        bool , HasDescription() const,
        "Returns ``True`` if the description property has been set.", "");
    
    DocDeclStr(
        wxString , GetDescription() const,
        "Returns the description value.", "");
    
    %property(Description, GetDescription, SetDescription);


    DocDeclStr(
        void , SetCopyright(const wxString& copyright),
        "Set the short string containing the program copyright
information. Notice that any occurrences of \"(C)\" in ``copyright``
will be replaced by the copyright symbol (circled C) automatically,
which means that you can avoid using this symbol in the program source
code which can be problematic.", "");
    
    DocDeclStr(
        bool , HasCopyright() const,
        "Returns ``True`` if the copyright property has been set.", "");
    
    DocDeclStr(
        wxString , GetCopyright() const,
        "Returns the copyright value.", "");
    
    %property(Copyright, GetCopyright, SetCopyright);


    DocDeclStr(
        void , SetLicence(const wxString& licence),
        "Set the long, multiline string containing the text of the program
licence.

Only GTK+ version supports showing the licence text in the native
about dialog currently so the generic version will be used under all
the other platforms if this method is called. To preserve the native
look and feel it is advised that you do not call this method but
provide a separate menu item in the \"Help\" menu for displaying the
text of your program licence.
", "");
    
    DocDeclStr(
        void , SetLicense(const wxString& licence),
        "This is the same as `SetLicence`.", "");
    
    DocDeclStr(
        bool , HasLicence() const,
        "Returns ``True`` if the licence property has been set.", "");
    
    DocDeclStr(
        wxString , GetLicence() const,
        "Returns the licence value.", "");
    
    %property(Licence, GetLicence, SetLicence);
    %pythoncode { License = Licence }


    DocDeclStr(
        void , SetIcon(const wxIcon& icon),
        "Set the icon to be shown in the dialog. By default the icon of the
main frame will be shown if the native about dialog supports custom
icons. If it doesn't but a valid icon is specified using this method,
the generic about dialog is used instead so you should avoid calling
this function for maximally native look and feel.", "");
    
    DocDeclStr(
        bool , HasIcon() const,
        "Returns ``True`` if the icon property has been set.", "");
    
    DocDeclStr(
        wxIcon , GetIcon() const,
        "Return the current icon value.", "");
    
    %property(Icon, GetIcon, SetIcon);    

    
    // web site for the program and its description (defaults to URL itself if
    // empty)
    %Rename(_SetWebSite,
            void , SetWebSite(const wxString& url, const wxString& desc = wxEmptyString));
    %Rename(_GetWebSiteURL,
            wxString , GetWebSiteURL() const);
    %Rename(_GetWebSiteDescription,
            wxString , GetWebSiteDescription() const);
    bool HasWebSite() const;

    %pythoncode {
        def SetWebSite(self, args):
            """
            SetWebSite(self, URL, [Description])

            Set the web site property.  The ``args`` parameter can
            either be a single string for the URL, to a 2-tuple of
            (URL, Description) strings.
            """
            if type(args) in [str, unicode]:
                self._SetWebSite(args)
            else:
                self._SetWebSite(args[0], args[1])
                    
        def GetWebSite(self):
            """
            GetWebSite(self) --> (URL, Description)
            """
            return (self._GetWebSiteURL(), self._GetWebSiteDescription())
    }
    %property(WebSite, GetWebSite, SetWebSite)


    
    DocDeclAStr(
        void , SetDevelopers(const wxArrayString& developers),
        "SetDevelopers(self, list developers)",
        "Set the list of the developers of the program.", "");
    
    DocDeclStr(
        void , AddDeveloper(const wxString& developer),
        "Add a string to the list of developers.", "");
    
    DocDeclStr(
        bool , HasDevelopers() const,
        "Returns ``True if any developers have been set.", "");
    
    DocDeclAStr(
        const wxArrayString& , GetDevelopers() const,
        "GetDevelopers(self) --> list",
        "Returns the list of developers.", "");
    
    %property(Developers, GetDevelopers, SetDevelopers);
    

    DocDeclAStr(
        void , SetDocWriters(const wxArrayString& docwriters),
        "SetDocWriters(self, list docwriters)",
        "Set the list of the documentation writers.", "");
    
    DocDeclStr(
        void , AddDocWriter(const wxString& docwriter),
        "Add a string to the list of documentation writers.", "");
    
    DocDeclStr(
        bool , HasDocWriters() const,
        "Returns ``True if any documentation writers have been set.", "");
    
    DocDeclAStr(
        const wxArrayString& , GetDocWriters() const,
        "GetDocWriters(self) --> list",
        "Returns the list of documentation writers.", "");

    %property(DocWriters, GetDocWriters, SetDocWriters);
    

    DocDeclAStr(
        void , SetArtists(const wxArrayString& artists),
        "SetArtists(self, list artists)", 
        "Set the list of artists for the program.", "");
    
    DocDeclStr(
        void , AddArtist(const wxString& artist),
        "Add a string to the list of artists.", "");
    
    DocDeclStr(
        bool , HasArtists() const,
        "Returns ``True`` if any artists have been set.", "");
    
    DocDeclAStr(
        const wxArrayString& , GetArtists() const,
        "GetArtists(self) --> list",
        "Returns the list od artists.", "");
    
    %property(Artists, GetArtists, SetArtists);

    
    
    DocDeclAStr(
        void , SetTranslators(const wxArrayString& translators),
        "SetTranslators(self, list translators)",
        "Sets the list of program translators.", "");
    
    DocDeclStr(
        void , AddTranslator(const wxString& translator),
        "Add a string to the list of translators.", "");
    
    DocDeclStr(
        bool , HasTranslators() const,
        "Returns ``True`` if any translators have been set.", "");
    
    DocDeclAStr(
        const wxArrayString& , GetTranslators() const,
        "GetTranslators(self) --> list",
        "Returns the list of program translators.", "");
    
    %property(Translators, GetTranslators, SetTranslators);


    // implementation only
    // -------------------

    // "simple" about dialog shows only textual information (with possibly
    // default icon but without hyperlink nor any long texts such as the
    // licence text)
    bool IsSimple() const;

    // get the description and credits (i.e. all of developers, doc writers,
    // artists and translators) as a one long multiline string
    wxString GetDescriptionAndCredits() const;

};



DocStr(wxAboutBox,
       "This function shows the standard about dialog containing the
information specified in ``info``. If the current platform has a
native about dialog which is capable of showing all the fields in
``info``, the native dialog is used, otherwise the function falls back
to the generic wxWidgets version of the dialog.", "");

void wxAboutBox(const wxAboutDialogInfo& info);


//---------------------------------------------------------------------------
