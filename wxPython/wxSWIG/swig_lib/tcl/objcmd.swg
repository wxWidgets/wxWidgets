/* objcmd.swg : Tcl object creation */

static int Tcl@CLASS@Cmd(ClientData clientData, Tcl_Interp *interp, int argc, char **argv) {
    void (*del)(ClientData) = 0;
    char *name = 0;
    int (*cmd)(ClientData, Tcl_Interp *, int, char **) = 0;
    @CLASSTYPE@ newObj = 0;
    int firstarg = 0;
    int thisarg = 0;
    if (argc == 1) {
        cmd = @TCLCONSTRUCTOR@;
    } else {
        if (strcmp(argv[1],"-this") == 0) thisarg = 2;
        else if (strcmp(argv[1],"-args") == 0) {
            firstarg = 1;
            cmd = @TCLCONSTRUCTOR@;
        } else if (argc == 2) {
            firstarg = 1;
            name = argv[1];
            cmd = @TCLCONSTRUCTOR@;
        } else if (argc >= 3) {
            name = argv[1];
            if (strcmp(argv[2],"-this") == 0) thisarg = 3;
            else {
                firstarg = 1;
                cmd = @TCLCONSTRUCTOR@;
            }
        }
    }
    if (cmd) {
        int result;
        result = (*cmd)(clientData,interp,argc-firstarg,&argv[firstarg]);
        if (result == TCL_OK) {
            SWIG_GetPtr(interp->result,(void **) &newObj,"@CLASSMANGLE@");
        } else { return result; }
        if (!name) name = interp->result;
        del = @TCLDESTRUCTOR@;
    } else if (thisarg > 0) { 
        if (thisarg < argc) {
            char *r;
            r = SWIG_GetPtr(argv[thisarg],(void **) &newObj,"@CLASSMANGLE@");
            if (r) {
                interp->result = "Type error. not a @CLASS@ object.";
                return TCL_ERROR;
            }
            if (!name) name = argv[thisarg];
	    /* Return value is same as pointer value */
	    Tcl_SetResult(interp,argv[thisarg],TCL_VOLATILE);
        } else {
            interp->result = "wrong # args.";
            return TCL_ERROR;
        }
    } else {
        interp->result = "No constructor available.";
        return TCL_ERROR;
    }
    {
      Tcl_CmdInfo dummy;
      if (!Tcl_GetCommandInfo(interp,name,&dummy)) {
	Tcl_CreateCommand(interp,name, Tcl@CLASS@MethodCmd, (ClientData) newObj, del);
	return TCL_OK;
      } else {
	Tcl_SetResult(interp,"",TCL_VOLATILE);
	Tcl_AppendResult(interp,"Object ", name, " already exists!", (char *) NULL);
	return TCL_ERROR;
      }
    }
}

