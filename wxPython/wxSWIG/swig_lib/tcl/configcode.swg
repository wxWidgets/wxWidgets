/* configcode.swg */

    else if ((*(argv[1]) == 'c') && (strncmp(argv[1],"configure") == 0) && (argv[1][1])) {
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
    }

