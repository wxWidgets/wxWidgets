
import sys, os, string

from distutils.msvccompiler import MSVCCompiler

from distutils.errors import \
     DistutilsExecError, DistutilsPlatformError, \
     CompileError, LibError, LinkError
from distutils.ccompiler import \
     CCompiler, gen_preprocess_options, gen_lib_options

#----------------------------------------------------------------------

class MyMSVCCompiler(MSVCCompiler):

    ##------------------------------------------------------------
    ## Override the entire compile method just to add flags to the
    ## RC command.  There should be an easier way to do this from
    ## distutils directly or in a derived class...
    ##------------------------------------------------------------

    def compile (self,
                 sources,
                 output_dir=None,
                 macros=None,
                 include_dirs=None,
                 debug=0,
                 extra_preargs=None,
                 extra_postargs=None):

        (output_dir, macros, include_dirs) = \
            self._fix_compile_args (output_dir, macros, include_dirs)
        (objects, skip_sources) = self._prep_compile (sources, output_dir)

        if extra_postargs is None:
            extra_postargs = []

        pp_opts = gen_preprocess_options (macros, include_dirs)
        compile_opts = extra_preargs or []
        compile_opts.append ('/c')
        if debug:
            compile_opts.extend (self.compile_options_debug)
        else:
            compile_opts.extend (self.compile_options)

        for i in range (len (sources)):
            src = sources[i] ; obj = objects[i]
            ext = (os.path.splitext (src))[1]

            if skip_sources[src]:
                self.announce ("skipping %s (%s up-to-date)" % (src, obj))
            else:
                self.mkpath (os.path.dirname (obj))

                if ext in self._c_extensions:
                    input_opt = "/Tc" + os.path.abspath(src)
                elif ext in self._cpp_extensions:
                    input_opt = "/Tp" + os.path.abspath(src)
                elif ext in self._rc_extensions:
                    # compile .RC to .RES file
                    input_opt = src
                    output_opt = "/fo" + obj
                    try:
                        self.spawn ([self.rc] + pp_opts +         ### RPD changed this line only
                                    [output_opt] + [input_opt])
                    except DistutilsExecError, msg:
                        raise CompileError, msg
                    continue
                elif ext in self._mc_extensions:

                    # Compile .MC to .RC file to .RES file.
                    #   * '-h dir' specifies the directory for the
                    #     generated include file
                    #   * '-r dir' specifies the target directory of the
                    #     generated RC file and the binary message resource
                    #     it includes
                    #
                    # For now (since there are no options to change this),
                    # we use the source-directory for the include file and
                    # the build directory for the RC file and message
                    # resources. This works at least for win32all.

                    h_dir = os.path.dirname (src)
                    rc_dir = os.path.dirname (obj)
                    try:
                        # first compile .MC to .RC and .H file
                        self.spawn ([self.mc] +
                                    ['-h', h_dir, '-r', rc_dir] + [src])
                        base, _ = os.path.splitext (os.path.basename (src))
                        rc_file = os.path.join (rc_dir, base + '.rc')
                        # then compile .RC to .RES file
                        self.spawn ([self.rc] +
                                    ["/fo" + obj] + [rc_file])

                    except DistutilsExecError, msg:
                        raise CompileError, msg
                    continue
                else:
                    # how to handle this file?
                    raise CompileError (
                        "Don't know how to compile %s to %s" % \
                        (src, obj))

                output_opt = "/Fo" + obj
                try:
                    self.spawn ([self.cc] + compile_opts + pp_opts +
                                [input_opt, output_opt] +
                                extra_postargs)
                except DistutilsExecError, msg:
                    raise CompileError, msg

        return objects

    # compile ()





#----------------------------------------------------------------------
# Hack this module and class into the distutils...

from distutils import ccompiler

ccompiler.default_compiler['nt'] = 'my_msvc'
ccompiler.compiler_class['my_msvc'] = ('my_distutils',
                                       'MyMSVCCompiler',
                                       'My MSVC derived class')


# make it look like it is part of the package...
import my_distutils
sys.modules['distutils.my_distutils'] = my_distutils




#----------------------------------------------------------------------
# Run SWIG the way I want it done

def run_swig(files, dir, gendir, package, USE_SWIG, force, swig_args, swig_deps=[]):
    from distutils.file_util import copy_file
    from distutils.dep_util import newer
    from distutils.spawn import spawn

    sources = []

    for file in files:
        basefile = os.path.splitext(file)[0]
        i_file   = os.path.join(dir, file)
        py_file  = os.path.join(dir, gendir, basefile+'.py')
        cpp_file = os.path.join(dir, gendir, basefile+'.cpp')

        sources.append(cpp_file)

        if USE_SWIG:
            for dep in swig_deps:
                if newer(dep, py_file) or newer(dep, cpp_file):
                    force = 1
                    break

            if force or newer(i_file, py_file) or newer(i_file, cpp_file):
                # we need forward slashes here even on win32
                cpp_file = string.join(string.split(cpp_file, '\\'), '/')
                i_file = string.join(string.split(i_file, '\\'), '/')

                cmd = ['swig'] + swig_args + ['-I'+dir, '-c', '-o', cpp_file, i_file]
                spawn(cmd, verbose=1)

        # copy the generated python file to the package directory
        copy_file(py_file, package, update=not force, verbose=0)


    return sources



#----------------------------------------------------------------------
# Update local copies of wxWindows contrib files


def contrib_copy_tree(src, dest, verbose=0):
    from distutils.dir_util import mkpath, copy_tree

    mkpath(dest, verbose=verbose)
    copy_tree(src, dest, update=1, verbose=verbose)

