/////////////////////////////////////////////////////////////////////////////
// Name:        module.h
// Purpose:     interface of wxModule
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxModule

    The module system is a very simple mechanism to allow applications (and parts
    of wxWidgets itself) to define initialization and cleanup functions that are
    automatically called on wxWidgets startup and exit.

    To define a new kind of module, derive a class from wxModule, override the
    wxModule::OnInit and wxModule::OnExit
    functions, and add the DECLARE_DYNAMIC_CLASS and IMPLEMENT_DYNAMIC_CLASS to
    header and implementation files (which can be the same file). On
    initialization, wxWidgets will find all classes derived from wxModule, create
    an instance of each, and call each OnInit function. On exit, wxWidgets will
    call the OnExit function for each module instance.

    Note that your module class does not have to be in a header file.

    For example:

    @code
    // A module to allow DDE initialization/cleanup
      // without calling these functions from app.cpp or from
      // the user's application.
      class wxDDEModule: public wxModule
      {
      public:
          wxDDEModule() { }
          virtual bool OnInit() { wxDDEInitialize(); return true; };
          virtual void OnExit() { wxDDECleanUp(); };

      private:
          DECLARE_DYNAMIC_CLASS(wxDDEModule)
      };

      IMPLEMENT_DYNAMIC_CLASS(wxDDEModule, wxModule)

      // Another module which uses DDE in its OnInit()
      class MyModule: public wxModule
      {
      public:
          MyModule() { AddDependency(CLASSINFO(wxDDEModule)); }
          virtual bool OnInit() { ... code using DDE ... }
          virtual void OnExit() { ... }

      private:
          DECLARE_DYNAMIC_CLASS(MyModule)
      };

      IMPLEMENT_DYNAMIC_CLASS(MyModule, wxModule)

      // Another module which uses DDE in its OnInit()
      // but uses a named dependency
      class MyModule2: public wxModule
      {
      public:
          MyModule2() { AddDependency("wxDDEModule"); }
          virtual bool OnInit() { ... code using DDE ... }
          virtual void OnExit() { ... }

      private:
          DECLARE_DYNAMIC_CLASS(MyModule2)
      };

      IMPLEMENT_DYNAMIC_CLASS(MyModule2, wxModule)
    @endcode

    @library{wxbase}
    @category{FIXME}
*/
class wxModule : public wxObject
{
public:
    /**
        Constructs a wxModule object.
    */
    wxModule();

    /**
        Destructor.
    */
    virtual ~wxModule();

    //@{
    /**
        Call this function from the constructor of the derived class. @a dep must be
        the CLASSINFO() of a wxModule-derived class and the
        corresponding module will be loaded before and unloaded after
        this module.
        The second version of this function allows a dependency to be added by
        name without access to the class info.  This is useful when a module is
        declared entirely in a source file and there is no header for the declaration
        of the module needed by CLASSINFO(), however errors are
        not detected until run-time, instead of compile-time, then.
        Note that circular dependencies are detected and result in a fatal error.

        @param dep
            The class information object for the dependent module.
        @param classname
            The class name of the dependent module.
    */
    void AddDependency(wxClassInfo* dep);
    void AddDependency(const char* classname);
    //@}

    /**
        Provide this function with appropriate cleanup for your module.
    */
    virtual void OnExit();

    /**
        Provide this function with appropriate initialization for your module. If the
        function
        returns @false, wxWidgets will exit immediately.
    */
    virtual bool OnInit();
};

