AC_DEFUN([AC_BAKEFILE_CREATE_FILE_DLLAR_SH],
[
dnl ===================== dllar.sh begins here =====================
dnl    (Created by merge-scripts.py from dllar.sh
dnl     file do not edit here!)
D='$'
cat <<EOF >dllar.sh
#!/bin/sh
#
# dllar - a tool to build both a .dll and an .a file
# from a set of object (.o) files for EMX/OS2.
#
#  Written by Andrew Zabolotny, bit@freya.etu.ru
#  Ported to Unix like shell by Stefan Neis, Stefan.Neis@t-online.de
#
#  This script will accept a set of files on the command line.
#  All the public symbols from the .o files will be exported into
#  a .DEF file, then linker will be run (through gcc) against them to
#  build a shared library consisting of all given .o files. All libraries
#  (.a) will be first decompressed into component .o files then act as
#  described above. You can optionally give a description (-d "description")
#  which will be put into .DLL. To see the list of accepted options (as well
#  as command-line format) simply run this program without options. The .DLL
#  is built to be imported by name (there is no guarantee that new versions
#  of the library you build will have same ordinals for same symbols).
#
#  dllar is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2, or (at your option)
#  any later version.
#
#  dllar is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with dllar; see the file COPYING.  If not, write to the Free
#  Software Foundation, 59 Temple Place - Suite 330, Boston, MA
#  02111-1307, USA.

# To successfuly run this program you will need:
#  - Current drive should have LFN support (HPFS, ext2, network, etc)
#    (Sometimes dllar generates filenames which won't fit 8.3 scheme)
#  - gcc
#    (used to build the .dll)
#  - emxexp
#    (used to create .def file from .o files)
#  - emximp
#    (used to create .a file from .def file)
#  - GNU text utilites (cat, sort, uniq)
#    used to process emxexp output
#  - GNU file utilities (mv, rm)
#  - GNU sed
#  - lxlite (optional, see flag below)
#    (used for general .dll cleanup)
#

flag_USE_LXLITE=1;

#
# helper functions
# basnam, variant of basename, which does _not_ remove the path, _iff_
#                              second argument (suffix to remove) is given
basnam(){
    case ${D}# in
    1)
        echo ${D}1 | sed 's/.*\\///' | sed 's/.*\\\\//'
        ;;
    2)
        echo ${D}1 | sed 's/'${D}2'${D}//'
        ;;
    *)
        echo "error in basnam ${D}*"
        exit 8
        ;;
    esac
}

# Cleanup temporary files and output
CleanUp() {
    cd ${D}curDir
    for i in ${D}inputFiles ; do
        case ${D}i in
        *!)
            rm -rf \`basnam ${D}i !\`
            ;;
        *)
            ;;
        esac
    done

    # Kill result in case of failure as there is just to many stupid make/nmake
    # things out there which doesn't do this.
    if @<:@ ${D}# -eq 0 @:>@; then
        rm -f ${D}arcFile ${D}arcFile2 ${D}defFile ${D}dllFile
    fi
}

# Print usage and exit script with rc=1.
PrintHelp() {
 echo 'Usage: dllar.sh @<:@-o@<:@utput@:>@ output_file@:>@ @<:@-i@<:@mport@:>@ importlib_name@:>@'
 echo '       @<:@-name-mangler-script script.sh@:>@'
 echo '       @<:@-d@<:@escription@:>@ "dll descrption"@:>@ @<:@-cc "CC"@:>@ @<:@-f@<:@lags@:>@ "CFLAGS"@:>@'
 echo '       @<:@-ord@<:@inals@:>@@:>@ -ex@<:@clude@:>@ "symbol(s)"'
 echo '       @<:@-libf@<:@lags@:>@ "{INIT|TERM}{GLOBAL|INSTANCE}"@:>@ @<:@-nocrt@<:@dll@:>@@:>@ @<:@-nolxl@<:@ite@:>@@:>@'
 echo '       @<:@*.o@:>@ @<:@*.a@:>@'
 echo '*> "output_file" should have no extension.'
 echo '   If it has the .o, .a or .dll extension, it is automatically removed.'
 echo '   The import library name is derived from this and is set to "name".a,'
 echo '   unless overridden by -import'
 echo '*> "importlib_name" should have no extension.'
 echo '   If it has the .o, or .a extension, it is automatically removed.'
 echo '   This name is used as the import library name and may be longer and'
 echo '   more descriptive than the DLL name which has to follow the old '
 echo '   8.3 convention of FAT.'
 echo '*> "script.sh may be given to override the output_file name by a'
 echo '   different name. It is mainly useful if the regular make process'
 echo '   of some package does not take into account OS/2 restriction of'
 echo '   DLL name lengths. It takes the importlib name as input and is'
 echo '   supposed to procude a shorter name as output. The script should'
 echo '   expect to get importlib_name without extension and should produce'
 echo '   a (max.) 8 letter name without extension.'
 echo '*> "cc" is used to use another GCC executable.   (default: gcc.exe)'
 echo '*> "flags" should be any set of valid GCC flags. (default: -s -Zcrtdll)'
 echo '   These flags will be put at the start of GCC command line.'
 echo '*> -ord@<:@inals@:>@ tells dllar to export entries by ordinals. Be careful.'
 echo '*> -ex@<:@clude@:>@ defines symbols which will not be exported. You can define'
 echo '   multiple symbols, for example -ex "myfunc yourfunc _GLOBAL*".'
 echo '   If the last character of a symbol is "*", all symbols beginning'
 echo '   with the prefix before "*" will be exclude, (see _GLOBAL* above).'
 echo '*> -libf@<:@lags@:>@ can be used to add INITGLOBAL/INITINSTANCE and/or'
 echo '   TERMGLOBAL/TERMINSTANCE flags to the dynamically-linked library.'
 echo '*> -nocrt@<:@dll@:>@ switch will disable linking the library against emx''s'
 echo '   C runtime DLLs.'
 echo '*> -nolxl@<:@ite@:>@ switch will disable running lxlite on the resulting DLL.'
 echo '*> All other switches (for example -L./ or -lmylib) will be passed'
 echo '   unchanged to GCC at the end of command line.'
 echo '*> If you create a DLL from a library and you do not specify -o,'
 echo '   the basename for DLL and import library will be set to library name,'
 echo '   the initial library will be renamed to 'name'_s.a (_s for static)'
 echo '   i.e. "dllar gcc.a" will create gcc.dll and gcc.a, and the initial'
 echo '   library will be renamed into gcc_s.a.'
 echo '--------'
 echo 'Example:'
 echo '   dllar -o gcc290.dll libgcc.a -d "GNU C runtime library" -ord'
 echo '    -ex "__main __ctordtor*" -libf "INITINSTANCE TERMINSTANCE"'
 CleanUp
 exit 1
}

# Execute a command.
# If exit code of the commnad <> 0 CleanUp() is called and we'll exit the script.
# @Uses    Whatever CleanUp() uses.
doCommand() {
    echo "${D}*"
    eval ${D}*
    rcCmd=${D}?

    if @<:@ ${D}rcCmd -ne 0 @:>@; then
        echo "command failed, exit code="${D}rcCmd
        CleanUp
        exit ${D}rcCmd
    fi
}

# main routine
# setup globals
cmdLine=${D}*
outFile=""
outimpFile=""
inputFiles=""
renameScript=""
description=""
CC=gcc.exe
CFLAGS="-s -Zcrtdll"
EXTRA_CFLAGS=""
EXPORT_BY_ORDINALS=0
exclude_symbols=""
library_flags=""
curDir=\`pwd\`
curDirS=curDir
case ${D}curDirS in
*/)
  ;;
*)
  curDirS=${D}{curDirS}"/"
  ;;
esac
# Parse commandline
libsToLink=0
omfLinking=0
while @<:@ ${D}1 @:>@; do
    case ${D}1 in
    -ord*)
        EXPORT_BY_ORDINALS=1;
        ;;
    -o*)
	shift
        outFile=${D}1
	;;
    -i*)
        shift
        outimpFile=${D}1
        ;;
    -name-mangler-script)
        shift
        renameScript=${D}1
        ;;
    -d*)
        shift
        description=${D}1
        ;;
    -f*)
        shift
        CFLAGS=${D}1
        ;;
    -c*)
        shift
        CC=${D}1
        ;;
    -h*)
        PrintHelp
        ;;
    -ex*)
        shift
        exclude_symbols=${D}{exclude_symbols}${D}1" "
        ;;
    -libf*)
        shift
        library_flags=${D}{library_flags}${D}1" "
        ;;
    -nocrt*)
        CFLAGS="-s"
        ;;
    -nolxl*)
        flag_USE_LXLITE=0
        ;;
    -* | /*)
        case ${D}1 in
        -L* | -l*)
            libsToLink=1
            ;;
        -Zomf)
            omfLinking=1
            ;;
        *)
            ;;
        esac
        EXTRA_CFLAGS=${D}{EXTRA_CFLAGS}" "${D}1
        ;;
    *.dll)
        EXTRA_CFLAGS="${D}{EXTRA_CFLAGS} \`basnam ${D}1 .dll\`"
        if @<:@ ${D}omfLinking -eq 1 @:>@; then
            EXTRA_CFLAGS="${D}{EXTRA_CFLAGS}.lib"
	else
            EXTRA_CFLAGS="${D}{EXTRA_CFLAGS}.a"
        fi
        ;;
    *)
        found=0;
        if @<:@ ${D}libsToLink -ne 0 @:>@; then
            EXTRA_CFLAGS=${D}{EXTRA_CFLAGS}" "${D}1
        else
            for file in ${D}1 ; do
                if @<:@ -f ${D}file @:>@; then
                    inputFiles="${D}{inputFiles} ${D}file"
                    found=1
                fi
            done
            if @<:@ ${D}found -eq 0 @:>@; then
                echo "ERROR: No file(s) found: "${D}1
                exit 8
            fi
        fi
      ;;
    esac
    shift
done # iterate cmdline words

#
if @<:@ -z "${D}inputFiles" @:>@; then
    echo "dllar: no input files"
    PrintHelp
fi

# Now extract all .o files from .a files
newInputFiles=""
for file in ${D}inputFiles ; do
    case ${D}file in
    *.a | *.lib)
        case ${D}file in
        *.a)
            suffix=".a"
            AR="ar"
            ;;
        *.lib)
            suffix=".lib"
            AR="emxomfar"
            EXTRA_CFLAGS="${D}EXTRA_CFLAGS -Zomf"
            ;;
        *)
            ;;
        esac
        dirname=\`basnam ${D}file ${D}suffix\`"_%"
        mkdir ${D}dirname
        if @<:@ ${D}? -ne 0 @:>@; then
            echo "Failed to create subdirectory ./${D}dirname"
            CleanUp
            exit 8;
        fi
        # Append '!' to indicate archive
        newInputFiles="${D}newInputFiles ${D}{dirname}!"
        doCommand "cd ${D}dirname; ${D}AR x ../${D}file"
        cd ${D}curDir
        found=0;
        for subfile in ${D}dirname/*.o* ; do
            if @<:@ -f ${D}subfile @:>@; then
                found=1
                if @<:@ -s ${D}subfile @:>@; then
	            # FIXME: This should be: is file size > 32 byte, _not_ > 0!
                    newInputFiles="${D}newInputFiles ${D}subfile"
                fi
            fi
        done
        if @<:@ ${D}found -eq 0 @:>@; then
            echo "WARNING: there are no files in archive \\'${D}file\\'"
        fi
        ;;
    *)
        newInputFiles="${D}{newInputFiles} ${D}file"
        ;;
    esac
done
inputFiles="${D}newInputFiles"

# Output filename(s).
do_backup=0;
if @<:@ -z ${D}outFile @:>@; then
    do_backup=1;
    set outFile ${D}inputFiles; outFile=${D}2
fi

# If it is an archive, remove the '!' and the '_%' suffixes
case ${D}outFile in
*_%!)
    outFile=\`basnam ${D}outFile _%!\`
    ;;
*)
    ;;
esac
case ${D}outFile in
*.dll)
    outFile=\`basnam ${D}outFile .dll\`
    ;;
*.DLL)
    outFile=\`basnam ${D}outFile .DLL\`
    ;;
*.o)
    outFile=\`basnam ${D}outFile .o\`
    ;;
*.obj)
    outFile=\`basnam ${D}outFile .obj\`
    ;;
*.a)
    outFile=\`basnam ${D}outFile .a\`
    ;;
*.lib)
    outFile=\`basnam ${D}outFile .lib\`
    ;;
*)
    ;;
esac
case ${D}outimpFile in
*.a)
    outimpFile=\`basnam ${D}outimpFile .a\`
    ;;
*.lib)
    outimpFile=\`basnam ${D}outimpFile .lib\`
    ;;
*)
    ;;
esac
if @<:@ -z ${D}outimpFile @:>@; then
    outimpFile=${D}outFile
fi
defFile="${D}{outFile}.def"
arcFile="${D}{outimpFile}.a"
arcFile2="${D}{outimpFile}.lib"

#create ${D}dllFile as something matching 8.3 restrictions,
if @<:@ -z ${D}renameScript @:>@ ; then
    dllFile="${D}outFile"
else
    dllFile=\`${D}renameScript ${D}outimpFile\`
fi

if @<:@ ${D}do_backup -ne 0 @:>@ ; then
    if @<:@ -f ${D}arcFile @:>@ ; then
        doCommand "mv ${D}arcFile ${D}{outFile}_s.a"
    fi
    if @<:@ -f ${D}arcFile2 @:>@ ; then
        doCommand "mv ${D}arcFile2 ${D}{outFile}_s.lib"
    fi
fi

# Extract public symbols from all the object files.
tmpdefFile=${D}{defFile}_%
rm -f ${D}tmpdefFile
for file in ${D}inputFiles ; do
    case ${D}file in
    *!)
        ;;
    *)
        doCommand "emxexp -u ${D}file >> ${D}tmpdefFile"
        ;;
    esac
done

# Create the def file.
rm -f ${D}defFile
echo "LIBRARY \`basnam ${D}dllFile\` ${D}library_flags" >> ${D}defFile
dllFile="${D}{dllFile}.dll"
if @<:@ ! -z ${D}description @:>@; then
    echo "DESCRIPTION  \\"${D}{description}\\"" >> ${D}defFile
fi
echo "EXPORTS" >> ${D}defFile

doCommand "cat ${D}tmpdefFile | sort.exe | uniq.exe > ${D}{tmpdefFile}%"
grep -v "^ *;" < ${D}{tmpdefFile}% | grep -v "^ *${D}" >${D}tmpdefFile

# Checks if the export is ok or not.
for word in ${D}exclude_symbols; do
    grep -v ${D}word < ${D}tmpdefFile >${D}{tmpdefFile}%
    mv ${D}{tmpdefFile}% ${D}tmpdefFile
done


if @<:@ ${D}EXPORT_BY_ORDINALS -ne 0 @:>@; then
    sed "=" < ${D}tmpdefFile | \\
    sed '
      N
      : loop
      s/^\\(@<:@0-9@:>@\\+\\)\\(@<:@^;@:>@*\\)\\(;.*\\)\\?/\\2 @\\1 NONAME/
      t loop
    ' > ${D}{tmpdefFile}%
    grep -v "^ *${D}" < ${D}{tmpdefFile}% > ${D}tmpdefFile
else
    rm -f ${D}{tmpdefFile}%
fi
cat ${D}tmpdefFile >> ${D}defFile
rm -f ${D}tmpdefFile

# Do linking, create implib, and apply lxlite.
gccCmdl="";
for file in ${D}inputFiles ; do
    case ${D}file in
    *!)
        ;;
    *)
        gccCmdl="${D}gccCmdl ${D}file"
        ;;
    esac
done
doCommand "${D}CC ${D}CFLAGS -Zdll -o ${D}dllFile ${D}defFile ${D}gccCmdl ${D}EXTRA_CFLAGS"
touch "${D}{outFile}.dll"

doCommand "emximp -o ${D}arcFile ${D}defFile"
if @<:@ ${D}flag_USE_LXLITE -ne 0 @:>@; then
    add_flags="";
    if @<:@ ${D}EXPORT_BY_ORDINALS -ne 0 @:>@; then
        add_flags="-ynd"
    fi
    doCommand "lxlite -cs -t: -mrn -mln ${D}add_flags ${D}dllFile"
fi
doCommand "emxomf -s -l ${D}arcFile"

# Successful exit.
CleanUp 1
exit 0
EOF
dnl ===================== dllar.sh ends here =====================
])
