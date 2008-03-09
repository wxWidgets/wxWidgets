/////////////////////////////////////////////////////////////////////////////
// Name:        cmdproc.h
// Purpose:     documentation for wxCommand class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxCommand
    @wxheader{cmdproc.h}

    wxCommand is a base class for modelling an application command,
    which is an action usually performed by selecting a menu item, pressing
    a toolbar button or any other means provided by the application to
    change the data or view.

    @library{wxcore}
    @category{FIXME}

    @seealso
    Overview
*/
class wxCommand : public wxObject
{
public:
    /**
        Constructor. wxCommand is an abstract class, so you will need to derive
        a new class and call this constructor from your own constructor.
        @a canUndo tells the command processor whether this command is undo-able. You
        can achieve the same functionality by overriding the CanUndo member function
        (if for example
        the criteria for undoability is context-dependent).
        @a name must be supplied for the command processor to display the command name
        in the application's edit menu.
    */
    wxCommand(bool canUndo = false, const wxString& name = NULL);

    /**
        Destructor.
    */
    ~wxCommand();

    /**
        Returns @true if the command can be undone, @false otherwise.
    */
    bool CanUndo();

    /**
        Override this member function to execute the appropriate action when called.
        Return @true to indicate that the action has taken place, @false otherwise.
        Returning @false will indicate to the command processor that the action is
        not undoable and should not be added to the command history.
    */
    bool Do();

    /**
        Returns the command name.
    */
    wxString GetName();

    /**
        Override this member function to un-execute a previous Do.
        Return @true to indicate that the action has taken place, @false otherwise.
        Returning @false will indicate to the command processor that the action is
        not redoable and no change should be made to the command history.
        How you implement this command is totally application dependent, but typical
        strategies include:
         Perform an inverse operation on the last modified piece of
        data in the document. When redone, a copy of data stored in command
        is pasted back or some operation reapplied. This relies on the fact that
        you know the ordering of Undos; the user can never Undo at an arbitrary position
        in the command history.
         Restore the entire document state (perhaps using document transactioning).
        Potentially very inefficient, but possibly easier to code if the user interface
        and data are complex, and an 'inverse execute' operation is hard to write.
        The docview sample uses the first method, to remove or restore segments
        in the drawing.
    */
    bool Undo();
};


/**
    @class wxCommandProcessor
    @wxheader{cmdproc.h}

    wxCommandProcessor is a class that maintains a history of wxCommands,
    with undo/redo functionality built-in. Derive a new class from this
    if you want different behaviour.

    @library{wxcore}
    @category{FIXME}

    @seealso
    @ref overview_wxcommandprocessoroverview "wxCommandProcessor overview",
    wxCommand
*/
class wxCommandProcessor : public wxObject
{
public:
    /**
        Constructor.
        @a maxCommands may be set to a positive integer to limit the number of
        commands stored to it, otherwise (and by default) the list of commands can grow
        arbitrarily.
    */
    wxCommandProcessor(int maxCommands = -1);

    /**
        Destructor.
    */
    ~wxCommandProcessor();

    /**
        Returns @true if the currently-active command can be undone, @false otherwise.
    */
    virtual bool CanUndo();

    /**
        Deletes all commands in the list and sets the current command pointer to @c
        @NULL.
    */
    virtual void ClearCommands();

    /**
        Returns the list of commands.
    */
    wxList GetCommands() const;

    /**
        Returns the edit menu associated with the command processor.
    */
    wxMenu* GetEditMenu() const;

    /**
        Returns the maximum number of commands that the command processor stores.
    */
    int GetMaxCommands() const;

    /**
        Returns the string that will be appended to the Redo menu item.
    */
    const wxString GetRedoAccelerator() const;

    /**
        Returns the string that will be shown for the redo menu item.
    */
    wxString GetRedoMenuLabel() const;

    /**
        Returns the string that will be appended to the Undo menu item.
    */
    const wxString GetUndoAccelerator() const;

    /**
        Returns the string that will be shown for the undo menu item.
    */
    wxString GetUndoMenuLabel() const;

    /**
        Initializes the command processor, setting the current command to the
        last in the list (if any), and updating the edit menu (if one has been
        specified).
    */
    virtual void Initialize();

    /**
        Returns a boolean value that indicates if changes have been made since
        the last save operation. This only works if
        MarkAsSaved()
        is called whenever the project is saved.
    */
    virtual bool IsDirty();

    /**
        You must call this method whenever the project is saved if you plan to use
        IsDirty().
    */
    virtual void MarkAsSaved();

    /**
        Executes (redoes) the current command (the command that has just been undone if
        any).
    */
    virtual bool Redo();

    /**
        Tells the command processor to update the Undo and Redo items on this
        menu as appropriate. Set this to @NULL if the menu is about to be
        destroyed and command operations may still be performed, or the command
        processor may try to access an invalid pointer.
    */
    void SetEditMenu(wxMenu* menu);

    /**
        Sets the menu labels according to the currently set menu and the current
        command state.
    */
    void SetMenuStrings();

    /**
        Sets the string that will be appended to the Redo menu item.
    */
    void SetRedoAccelerator(const wxString& accel);

    /**
        Sets the string that will be appended to the Undo menu item.
    */
    void SetUndoAccelerator(const wxString& accel);

    /**
        Submits a new command to the command processor. The command processor
        calls wxCommand::Do to execute the command; if it succeeds, the command
        is stored in the history list, and the associated edit menu (if any) updated
        appropriately. If it fails, the command is deleted
        immediately. Once Submit has been called, the passed command should not
        be deleted directly by the application.
        @a storeIt indicates whether the successful command should be stored
        in the history list.
    */
    virtual bool Submit(wxCommand* command, bool storeIt = true);

    /**
        Undoes the command just executed.
    */
    virtual bool Undo();
};
