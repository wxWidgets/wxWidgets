/* methodcmd8.swg : Tcl8.x method invocation */

static int Tcl@CLASS@MethodCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST _objv[]) {
    int (*cmd)(ClientData, Tcl_Interp *, int, Tcl_Obj *CONST*) = 0;
    char *_str;
    int rcode;
    Tcl_Obj **objv;
    Tcl_Obj *oldarg,*tcl_result,*obj;
    int length;
    char c;

    tcl_result = Tcl_GetObjResult(interp);
    objv = (Tcl_Obj **) _objv; 
    if (objc < 2) {
        Tcl_SetStringObj(tcl_result,"@CLASS@ methods : { @METHODLIST@ }",-1);
        return TCL_ERROR;
    }
    obj = Tcl_NewObj();
    SWIG_SetPointerObj(obj,(void *) clientData,"@CLASSMANGLE@");
    _str = Tcl_GetStringFromObj(objv[1],&length);
    c = *_str;
    if (0);
@METHODS@
    else if ((c == 'c') && (strncmp(_str,"configure",length) == 0) && (length >= 2)) {
      int i = 2;
      cmd = 0;
      while (i+1 < objc) {
	_str = Tcl_GetStringFromObj(objv[i],&length);
@CONFIGMETHODS@
	   if (cmd) {
	     oldarg = objv[i];
	     objv[i] = obj;
	     rcode = (*cmd)(clientData,interp,3,&objv[i-1]);
	     objv[i] = oldarg;
	     if (rcode == TCL_ERROR) {
		Tcl_DecrRefCount(obj);
		return rcode;
	     }
	     cmd = 0;
	   } else {
	     Tcl_SetStringObj(tcl_result,"Invalid configure option. Must be { @CONFIGLIST@ }",-1);
	     Tcl_DecrRefCount(obj);
	     return TCL_ERROR;
	   }
	i+=2;
      }
      if ((i < objc) || (i == 2)) {
	Tcl_SetStringObj(tcl_result,"{ @CONFIGLIST@ }",-1);
	Tcl_DecrRefCount(obj);
	return TCL_ERROR;
      }
      Tcl_DecrRefCount(obj);
      return TCL_OK;
    } else if ((c == 'c') && (strncmp(_str,"cget",length) == 0) && (length >= 2)) {
            if (objc == 3) {
	      _str = Tcl_GetStringFromObj(objv[2],&length);
	      if (0) {}
@CGETMETHODS@
	      else if (strcmp(_str,"-this") == 0) {
                   SWIG_SetPointerObj(tcl_result,(void *) clientData, "@CLASSMANGLE@");
		   Tcl_DecrRefCount(obj);
                   return TCL_OK;
	      }
	      if (cmd) {
		oldarg = objv[2];
		objv[2] = obj;
		rcode = (*cmd)(clientData,interp,objc-1,&objv[1]);
		objv[2] = oldarg;
		Tcl_DecrRefCount(obj);
		return rcode;
	      } else {
		Tcl_SetStringObj(tcl_result,"Invalid cget option. Must be { -this @CGETLIST@ }",-1);
		Tcl_DecrRefCount(obj);
		return TCL_ERROR;
	      }
	    } else {
	      Tcl_SetStringObj(tcl_result,"{ -this @CGETLIST@ }", -1);
	      Tcl_DecrRefCount(obj);
	      return TCL_ERROR;
	    }
    }
    if (!cmd) {
        Tcl_SetStringObj(tcl_result,"Invalid Method. Must be { @METHODLIST@}",-1);
	Tcl_DecrRefCount(obj);
        return TCL_ERROR;
    }
    oldarg = objv[1];
    objv[1] = obj;
    rcode = (*cmd)(clientData,interp,objc,objv);
    objv[1] = oldarg;
    Tcl_DecrRefCount(obj);
    return rcode;
}



