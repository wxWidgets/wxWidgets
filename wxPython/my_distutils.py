
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
                    input_opt = "/Tc" + src
                elif ext in self._cpp_extensions:
                    input_opt = "/Tp" + src
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



    ##------------------------------------------------------------
    ## Now override the link() method to change where the import
    ## library is placed.  Hopefully distutils will be updated
    ## someday to make this configurable...
    ##------------------------------------------------------------

    def link (self,
              target_desc,
              objects,
              output_filename,
              output_dir=None,
              libraries=None,
              library_dirs=None,
              runtime_library_dirs=None,
              export_symbols=None,
              debug=0,
              extra_preargs=None,
              extra_postargs=None,
              build_temp=None):

        (objects, output_dir) = self._fix_object_args (objects, output_dir)
        (libraries, library_dirs, runtime_library_dirs) = \
            self._fix_lib_args (libraries, library_dirs, runtime_library_dirs)

        if runtime_library_dirs:
            self.warn ("I don't know what to do with 'runtime_library_dirs': "
                       + str (runtime_library_dirs))

        lib_opts = gen_lib_options (self,
                                    library_dirs, runtime_library_dirs,
                                    libraries)
        if output_dir is not None:
            output_filename = os.path.join (output_dir, output_filename)

        if self._need_link (objects, output_filename):

            if target_desc == CCompiler.EXECUTABLE:
                if debug:
                    ldflags = self.ldflags_shared_debug[1:]
                else:
                    ldflags = self.ldflags_shared[1:]
            else:
                if debug:
                    ldflags = self.ldflags_shared_debug
                else:
                    ldflags = self.ldflags_shared

            export_opts = []
            for sym in (export_symbols or []):
                export_opts.append("/EXPORT:" + sym)

            ld_args = (ldflags + lib_opts + export_opts +
                       objects + ['/OUT:' + output_filename])

            # The MSVC linker generates .lib and .exp files, which cannot be
            # suppressed by any linker switches. The .lib files may even be
            # needed! Make sure they are generated in the temporary build
            # directory. Since they have different names for debug and release
            # builds, they can go into the same directory.
            if export_symbols is not None:
                (dll_name, dll_ext) = os.path.splitext(
                    os.path.basename(output_filename))

                ## The old code
                ##implib_file = os.path.join(
                ##    os.path.dirname(objects[0]),
                ##    self.library_filename(dll_name))

                ## The new
                implib_file = os.path.join('build', 'ilib',
                                           self.library_filename(dll_name))
                self.mkpath(os.path.dirname(implib_file))

                ld_args.append ('/IMPLIB:' + implib_file)

            if extra_preargs:
                ld_args[:0] = extra_preargs
            if extra_postargs:
                ld_args.extend(extra_postargs)

            self.mkpath (os.path.dirname (output_filename))
            try:
                self.spawn ([self.linker] + ld_args)
            except DistutilsExecError, msg:
                raise LinkError, msg

        else:
            self.announce ("skipping %s (up-to-date)" % output_filename)

    # link ()



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

                cmd = ['swig'] + swig_args + ['-c', '-o', cpp_file, i_file]
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








