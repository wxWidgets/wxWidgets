/* objcmd8.swg : Tcl 8.x object creation */

static int Tcl@CLASS@Cmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
    void (*del)(ClientData) = 0;
    char *name = 0;
    int (*cmd)(ClientData, Tcl_Interp *, int, Tcl_Obj *CONST*) = 0;
    @CLASSTYPE@ newObj = 0;
    int firstarg = 0;
    int thisarg = 0;
    int length;
    char *_str;
    Tcl_Obj *tcl_result;

    tcl_result = Tcl_GetObjResult(interp);
    if (objc == 1) {
        cmd = @TCLCONSTRUCTOR@;
    } else {
      _str = Tcl_GetStringFromObj(objv[1],&length);
      if (strcmp(_str,"-this") == 0) thisarg = 2;
      else if (strcmp(_str,"-args") == 0) {
	firstarg = 1;
	cmd = @TCLCONSTRUCTOR@;
      } else if (objc == 2) {
	firstarg = 1;
	name = _str;
	cmd = @TCLCONSTRUCTOR@;
      } else if (objc >= 3) {
	name = _str;
	_str = Tcl_GetStringFromObj(objv[2],&length);
	if (strcmp(_str,"-this") == 0) thisarg = 3;
	else {
	  firstarg = 1;
	  cmd = @TCLCONSTRUCTOR@;
	}
      }
    }
    if (cmd) {
        int result;
        result = (*cmd)(clientData,interp,objc-firstarg,&objv[firstarg]);
        if (result == TCL_OK) {
            SWIG_GetPointerObj(interp,tcl_result,(void **) &newObj,"@CLASSMANGLE@");
        } else { return result; }
        if (!name) name = Tcl_GetStringFromObj(tcl_result,&length);
        del = @TCLDESTRUCTOR@;
    } else if (thisarg > 0) { 
        if (thisarg < objc) {
            char *r;
            r = SWIG_GetPointerObj(interp,objv[thisarg],(void **) &newObj,"@CLASSMANGLE@");
            if (r) {
	      Tcl_SetStringObj(tcl_result,"Type error. not a @CLASS@ object.",-1);
	      return TCL_ERROR;
            }
        if (!name) name = Tcl_GetStringFromObj(objv[thisarg],&length);
	Tcl_SetStringObj(tcl_result,name,-1);
        } else {
            Tcl_SetStringObj(tcl_result,"wrong # args.",-1);
            return TCL_ERROR;
        }
    } else {
        Tcl_SetStringObj(tcl_result,"No constructor available.",-1);
        return TCL_ERROR;
    }
    {
      Tcl_CmdInfo dummy;
      if (!Tcl_GetCommandInfo(interp,name,&dummy)) {
	Tcl_CreateObjCommand(interp,name, Tcl@CLASS@MethodCmd, (ClientData) newObj, del);
	return TCL_OK;
      } else {
	Tcl_SetStringObj(tcl_result,"Object name already exists!",-1);
	return TCL_ERROR;
      }
    }
}

