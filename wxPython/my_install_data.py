"""my_install_data.py

Provides a more sophisticated facility to install data files
than distutils' install_data does.
You can specify your files as a template like in MANIFEST.in
and you have more control over the copy process.

"""

# created 2000/08/01, Rene Liebscher <R.Liebscher@gmx.de>

###########################################################################
# import some modules we need

import os,sys,string
from types import StringType,TupleType,ListType
from distutils.util import change_root
from distutils.filelist import FileList
from distutils.command.install_data import install_data

###########################################################################
# a container class for our more sophisticated install mechanism

class Data_Files:
    """ container for list of data files.
        supports alternate base_dirs e.g. 'install_lib','install_header',...
        supports a directory where to copy files
        supports templates as in MANIFEST.in
        supports preserving of paths in filenames
            eg. foo/xyz is copied to base_dir/foo/xyz
        supports stripping of leading dirs of source paths
            eg. foo/bar1/xyz, foo/bar2/abc can be copied to bar1/xyz, bar2/abc
    """

    def __init__(self,base_dir=None,files=None,copy_to=None,template=None,preserve_path=0,strip_dirs=0):
        self.base_dir = base_dir
        self.files = files
        self.copy_to = copy_to
        self.template = template
        self.preserve_path = preserve_path
        self.strip_dirs = strip_dirs
        self.finalized = 0

    def warn (self, msg):
        sys.stderr.write ("warning: %s: %s\n" %
                          ("install_data", msg))

    def debug_print (self, msg):
        """Print 'msg' to stdout if the global DEBUG (taken from the
        DISTUTILS_DEBUG environment variable) flag is true.
        """
        from distutils.core import DEBUG
        if DEBUG:
            print msg


    def finalize(self):
        """ complete the files list by processing the given template """
        if self.finalized:
            return
        if self.files == None:
            self.files = []
        if self.template != None:
            if type(self.template) == StringType:
                self.template = string.split(self.template,";")
            filelist = FileList(self.warn,self.debug_print)
            for line in self.template:
                filelist.process_template_line(string.strip(line))
            filelist.sort()
            filelist.remove_duplicates()
            self.files.extend(filelist.files)
        self.finalized = 1

# end class Data_Files

###########################################################################
# a more sophisticated install routine than distutils install_data

class my_install_data (install_data):

    def check_data(self,d):
        """ check if data are in new format, if not create a suitable object.
            returns finalized data object
        """
        if not isinstance(d, Data_Files):
            self.warn(("old-style data files list found "
                        "-- please convert to Data_Files instance"))
            if type(d) is TupleType:
                if len(d) != 2 or  not (type(d[1]) is ListType):
                        raise DistutilsSetupError, \
                          ("each element of 'data_files' option must be an "
                            "Data File instance, a string or 2-tuple (string,[strings])")
                d = Data_Files(copy_to=d[0],files=d[1])
            else:
                if not (type(d) is StringType):
                        raise DistutilsSetupError, \
                          ("each element of 'data_files' option must be an "
                           "Data File instance, a string or 2-tuple (string,[strings])")
                d = Data_Files(files=[d])
        d.finalize()
        return d

    def run(self):
        self.outfiles = []
        install_cmd = self.get_finalized_command('install')

        for d in self.data_files:
            d = self.check_data(d)

            install_dir = self.install_dir
            # alternative base dir given => overwrite install_dir
            if d.base_dir != None:
                install_dir = getattr(install_cmd,d.base_dir)

            # copy to an other directory
            if d.copy_to != None:
                if not os.path.isabs(d.copy_to):
                    # relatiev path to install_dir
                    dir = os.path.join(install_dir, d.copy_to)
                elif install_cmd.root:
                    # absolute path and alternative root set
                    dir = change_root(self.root,d.copy_to)
                else:
                    # absolute path
                    dir = d.copy_to
            else:
                # simply copy to install_dir
                dir = install_dir
                # warn if necceassary
                self.warn("setup script did not provide a directory to copy files to "
                          " -- installing right in '%s'" % install_dir)

            dir=os.path.normpath(dir)
            # create path
            self.mkpath(dir)

            # copy all files
            for src in d.files:
                if d.strip_dirs > 0:
                    dst = string.join(string.split(os.path.normcase(src),os.sep)[d.strip_dirs:],os.sep)
                else:
                    dst = src
                if d.preserve_path:
                    # preserve path in filename
                    self.mkpath(os.path.dirname(os.path.join(dir,dst)))
                    out = self.copy_file(src, os.path.join(dir,dst))
                else:
                    out = self.copy_file(src, dir)
                if type(out) is TupleType:
                    out = out[0]
                self.outfiles.append(out)

        return self.outfiles

    def get_inputs (self):
        inputs = []
        for d in self.data_files:
            d = self.check_data(d)
            inputs.append(d.files)
        return inputs

    def get_outputs (self):
         return self.outfiles


###########################################################################

