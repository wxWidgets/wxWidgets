#!/bin/sh

necho() {
   if [ "`echo -n`" = "-n" ]; then
      echo "${@}\c"
   else
      echo -n "${@}"
   fi
}

# Script that attempts to produce different run-time libraries

TARGET='perl_lib perl_shared py_lib py_shared tcl_lib tcl_shared tcl8_lib tcl8_shared'

echo "Building the SWIG runtime libraries..."
echo ""
echo "*** Note : Some builds may fail due to uninstalled packages or"
echo "unsupported features such as dynamic loading (don't panic)."
echo ""

for i in ${TARGET}; do 
necho "    Building ${i}"; 
if make ${i} >/dev/null 2>&1; then 
# See if SWIG generated any errors at all
	echo " ....... OK.";
else 
    echo " ....... failed.";
fi;
done

