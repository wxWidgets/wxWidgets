/* delcmd.swg : Tcl object deletion method */

static void TclDelete@CLASS@(ClientData clientData) {
    @DESTRUCTOR@((@CLASSTYPE@) clientData);
}

