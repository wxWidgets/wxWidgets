/* methodcmd.swg : Tcl method invocation */

static int Tcl@CLASS@MethodCmd(ClientData clientData, Tcl_Interp *interp, int argc, char **argv) {
    int (*cmd)(ClientData, Tcl_Interp *, int, char **) = 0;
    char temp[256], *oldarg;
    int rcode;
    int length;
    char c;

    if (argc < 2) {
        Tcl_SetResult(interp,"@CLASS@ methods : { @METHODLIST@ }",TCL_STATIC);
        return TCL_ERROR;
    }
    c = argv[1][0];
    length = strlen(argv[1]);
    SWIG_MakePtr(temp,(void *) clientData, "@CLASSMANGLE@");
    if (0);
    @METHODS@
    else if ((c == 'c') && (strncmp(argv[1],"configure",length) == 0) && (length >= 2)) {
      int i = 2;
      cmd = 0;
      while (i+1 < argc) {
	@CONFIGMETHODS@
	   if (cmd) {
	     oldarg = argv[i];
	     argv[i] = &temp[0];
	     rcode = (*cmd)(clientData,interp,3,&argv[i-1]);
	     argv[i] = oldarg;
	     if (rcode == TCL_ERROR) return rcode;
	     cmd = 0;
	   } else {
	     Tcl_SetResult(interp,"Invalid configure option. Must be { @CONFIGLIST@ }",TCL_STATIC);
	     return TCL_ERROR;
	   }
	i+=2;
      }
      if ((i < argc) || (i == 2)) {
	Tcl_SetResult(interp,"{ @CONFIGLIST@ }",TCL_STATIC);
	return TCL_ERROR;
      }
      return TCL_OK;
    } else if ((c == 'c') && (strncmp(argv[1],"cget",length) == 0) && (length >= 2)) {
            if (argc == 3) {
	      if (0) {}
	      @CGETMETHODS@
		 else if (strcmp(argv[2],"-this") == 0) {
                   SWIG_MakePtr(interp->result,(void *) clientData, "@CLASSMANGLE@");
                   return TCL_OK;
		 }
	      if (cmd) {
		oldarg = argv[2];
		argv[2] = &temp[0];
		rcode = (*cmd)(clientData,interp,argc-1,&argv[1]);
		argv[2] = oldarg;
		return rcode;
	      } else {
		Tcl_SetResult(interp,"Invalid cget option. Must be { -this @CGETLIST@ }",TCL_STATIC);
		return TCL_ERROR;
	      }
	    } else {
	      Tcl_SetResult(interp,"{ -this @CGETLIST@ }", TCL_STATIC);
	      return TCL_ERROR;
	    }
    }
    if (!cmd) {
        Tcl_SetResult(interp,"Invalid Method. Must be { @METHODLIST@}",TCL_STATIC);
        return TCL_ERROR;
    }
    oldarg = argv[1];
    argv[1] = &temp[0];
    rcode = (*cmd)(clientData,interp,argc,argv);
    argv[1] = oldarg;
    return rcode;
}
