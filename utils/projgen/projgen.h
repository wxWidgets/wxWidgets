/////////////////////////////////////////////////////////////////////////////
// Name:        projgen.h
// Purpose:     Project generator classes.
// Author:      Julian Smart
// Modified by:
// Created:     04/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*

Description
-----------

The top-level object is wxProjectDatabase, which maintains a list of wxProjectGenerator
objects. Each wxProjectGenerator object contains a list of wxGeneratorConfiguration objects,
and each of these in turn stores a list of variants which represent compiler-specific options in that
configuration. wxProjectDatabase also stores a list of generic options (again variants),
which may cause compiler-specific options to be stored in configurations.

The usage is like this. The IDE (or other calling application) adds a number of project generators
at initialization, one for each kind of compiler. For a new project, the app should call InitializeGenerators
in order to call OnInitializeDefaults for each generator, which will call back into the wxProjectDatabase
to get user-settable defaults.

The app can then set generic options. When a generic option (such as 'use debug info')
is set for a particular configuration, all generator objects are notified via OnSetGenericOption and they
translate the generic option into a specific one (for that configuration).

The wxProjectDatabase object can also be used to set compiler-specific options directly if required,
but normally this would not be required. Each wxProjectGenerator should also have the opportunity
to set initial defaults. These defaults should be editable by the user.

Each wxProjectGenerator can access the parent wxProjectDatabase object at any time, since it
may need to make a judgement about several generic settings in order to know what specific
compiler options should be set.

TODO: make a list of generic compiler options that each generator should recognise.

*/

#ifndef _PROJGEN_H_
#define _PROJGEN_H_

#ifdef __GNUG__
#pragma interface "projgen.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/hash.h"
#include "wx/variant.h"

typedef enum {
  wxPROJECT_CAP_NONE = 0,
  wxPROJECT_CAP_MAKEFILE = 1,
  wxPROJECT_CAP_PROJECT = 2,
} wxProjectCapability;

class wxProjectGenerator;
class wxGeneratorConfiguration;

/*
 * wxProjectDatabase
 * This class maintains a list to all wxProjectGenerator objects, one for
 * each compiler.
 * Setting a generic option in wxProjectDatabase causes the individual wxProjectGenerator
 * objects to set their compiler-specific options for later generation.
 */

class wxProjectDatabase: public wxObject
{
DECLARE_CLASS(wxProjectDatabase)
public:
    wxProjectDatabase();
    ~wxProjectDatabase();

// Operations
    // Generate project or makefile for a named compiler. Give an optional compiler version.
    virtual bool GenerateProject(const wxString& compiler, const wxString& filename, bool isMakefile, int compilerVersion = 0);

    // This calls each wxProjectGenerator's OnInitializeDefaults function to fill out the
    // defaults for each configuration. The generators will probably call back into the wxProjectDatabase
    // to get the defaults from a file (see GetDefaultCompilerOptions below).
    virtual bool InitializeGenerators();

// Accessors
    // Get the capability: can it generate projects, or makefiles, or neither?
    virtual wxProjectCapability GetCapability(const wxString& compiler) const ;

    // Override this for your app so that when the wxProjectGenerator initializes its defaults, it
    // can call this to get specific option values that may be setup by the user.
    virtual wxVariant GetDefaultCompilerOption(const wxString& compiler, const wxString& config, const wxString& option) const ;

    // Gets all the default options for the named compiler/config. Likewise, override this to provide
    // a list of defaults to the calling wxProjectGenerator.
    virtual wxStringList GetDefaultCompilerOptionList(const wxString& compiler, const wxString& config) const ;

// Compiler/configuration-specific options
    // Get a compiler-specific option value from the name.
    virtual wxVariant GetCompilerOption(const wxString& compiler, const wxString& config, const wxString& name) const;

    // Set the compiler-specific option
    virtual void SetCompilerOption(const wxString& compiler, const wxString& config, const wxString& name, const wxVariant& value);

    // Removes the compiler-specific option
    virtual void RemoveCompilerOption(const wxString& compiler, const wxString& config, const wxString& name);

    // Does this option exist?
    virtual bool HasCompilerOption(const wxString& compiler, const wxString& config, const wxString& name) const;

// Generic options
    // Get a generic option value from the name.
    virtual wxVariant GetGenericOption(const wxString& config, const wxString& name) const;

    // Set the generic option value. This calls SetGenericOption for each wxProjectGenerator,
    // which will cause compiler-specific values to be placed in the relevant config
    virtual void SetGenericOption(const wxString& config, const wxString& name, const wxVariant& value);

    // Removes the generic option.
    virtual void RemoveGenericOption(const wxString& config, const wxString& name);

    // Does this option exist?
    virtual bool HasGenericOption(const wxString& config, const wxString& name) const;

// Project path
    inline void SetProjectPath(const wxString& path) { m_projectPath = path; };
    inline wxString GetProjectPath() const { return m_projectPath; };

// Project name
    inline void SetProjectName(const wxString& name) { m_projectName = name; };
    inline wxString GetProjectName() const { return m_projectName; };

// The source files in the project
    // Add a file to the project. Normally this will be relative to the project path.
    // TODO: Files are managed within the wxProjectDatabase, but what about extra files
    // for specific platforms? Well, let's assume that even on Unix, you'd create a .rc
    // file, even if this isn't used in the resulting project/makefile on Unix.
    virtual void AddFile(const wxString& filename);
    virtual void RemoveFile(const wxString& filename);
    virtual bool FileExists(const wxString& filename) const;

// TODO: management of include paths, library paths, libraries

// Generator management
    virtual void AddGenerator(wxProjectGenerator* generator) ;
    virtual void RemoveGenerator(wxProjectGenerator* generator) ; // Doesn't delete it, just removes it
    virtual wxProjectGenerator* FindGenerator(const wxString& compiler) const ;
    virtual void ClearGenerators();

protected:
    // List of wxProjectGenerator objects
    wxList          m_generators;

    // List of compiler-independent configurations, such as "debug".
    wxList          m_genericConfigurations;

    // List of source files
    wxStringList    m_sourceFiles;

    // List of library paths
    wxStringList    m_libraryPaths;

    // List of libraries: TODO this should be compiler-specific, surely?
    wxStringList    m_libraries;

    // List of include paths
    wxStringList    m_includePaths;

    // Project path
    wxString        m_projectPath;

    // Project name
    wxString        m_projectName;
};

/*
 * wxGeneratorConfiguration
 * A configuration, e.g. "debug", "release"
 */

class wxGeneratorConfiguration: public wxObject
{
DECLARE_CLASS(wxGeneratorConfiguration)
public:
    wxGeneratorConfiguration(const wxString& name);
    ~wxGeneratorConfiguration();

    // Does this option exist?
    virtual bool HasOption(const wxString& name) const;

    // Find option: returns NULL if there is no such option.
    wxVariant* FindOption(const wxString& name) const;

    // Get an option value
    virtual wxVariant GetOption(const wxString& name) const;

    // Set the option
    virtual void SetOption(const wxString& name, const wxVariant& value);

    // Remove the option
    virtual void RemoveOption(const wxString& name);

    // Does this option exist?
    virtual bool HasOption(const wxString& name) const;

    // Get the list of options
    inline const wxList& GetOptions() const { return m_options; }

    inline void SetName(const wxString& name) { m_name = name; }
    inline wxString GetName() const { return m_name; }

protected:
    // Configuration name
    wxString    m_name;

    // List of wxVariants
    wxList      m_options;
};

/*
 * wxProjectGenerator.
 * Only derived classes can be instantiated.
 */

class wxProjectGenerator: public wxObject
{
DECLARE_CLASS(wxProjectGenerator)
public:
    wxProjectGenerator(const wxString& name, wxProjectDatabase* topLevel);
    ~wxProjectGenerator();

// Operations
    // Generate project or makefile. Give an optional compiler version.
    virtual bool GenerateProject(bool isMakefile, int compilerVersion = 0) = 0;

    // Called when the defaults should be initialized.
    // It would recognise e.g. the "Debug" configuration name and set specific defaults, possibly
    // reading them from a database to allow for tailoring.
    // It is likely to call wxProjectDatabase::GetDefaultCompilerOption.
    virtual bool OnInitializeDefaults(const wxString& config) = 0;

    // This is called by wxProjectDatabase::SetGenericOption, and it tells this object
    // to translate it to a specific option. Then this object will (probably) call SetOption.
    virtual bool OnSetGenericOption(const wxString& config, const wxString& name, const wxVariant& value) = 0;

// Accessors
    // Get the capability: can it generate projects, or makefiles, or neither?
    virtual wxProjectCapability GetCapability() const = 0;

    // Name
    inline void SetName(const wxString& name) { m_name = name; }
    inline wxString GetName() const { return m_name; }

    // Top-level wxProjectDatabase object
    inline void SetTopLevel(wxProjectDatabase* topLevel) { m_topLevel = topLevel; }
    inline wxProjectDatabase* GetTopLevel() const { return m_topLevel; }

// Options
    // Get an option value
    virtual wxVariant GetOption(const wxString& config, const wxString& name) const;

    // Set the option
    virtual void SetOption(const wxString& config, const wxString& name, const wxVariant& value);

    // Remove the option
    virtual void RemoveOption(const wxString& config, const wxString& name);

    // Does this option exist?
    virtual bool HasOption(const wxString& name) const;

    // Get the list of options
    inline const wxList& GetConfigurations() const { return m_configs; }

// Configuration management
    wxGeneratorConfiguation* FindConfiguration(const wxString& config) const ;
    void AddConfiguration(wxGeneratorConfiguration* config) ;
    void RemoveConfiguration(wxGeneratorConfiguration* config) ;
    void ClearConfigurations() ;

protected:
    // List of wxGeneratorConfiguration objects
    wxList                  m_configs;

    // Compiler name
    wxString                m_name;

    // Top-level object
    wxProjectDatabase*      m_topLevel;
};

#endif
    // projgen.h

