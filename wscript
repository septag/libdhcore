# 
import os, sys, platform, inspect, glob

# main global variables
VERSION = "1.0.1"
PROJNAME = "libdhcore"

ROOTDIR = os.path.dirname(inspect.getfile(inspect.currentframe()))

# setup variant builds (debug/release)
def init(ctx):
    from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext
    for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
        name = y.__name__.replace('Context', '').lower()
        class DebugCtx(y):
            cmd = name + '_debug'   # command names like 'build_debug', 'clean_debug', etc
            variant = 'debug'
        class ReleaseCtx(y):
            # if a 'cmd' is not given, the default 'build', 'clean',
            # etc actions will be redirected to the 'release' variant dir
            variant = 'release'

# options command
def options(opt):
    opt.load('compiler_c compiler_cxx')

    # fetch from environment variables as default
    opt.add_option('--enable-assert', action='store_true', default=False, dest='DASSERT',
        help='Enable assertions (debug build always have assertions)')
    opt.add_option('--prefix', action='store', default='', dest='PREFIX',
        help='Install path prefix')
    opt.add_option('--build-tests', action='store_true', default=False, dest='BUILD_TESTS',
        help='Build test programs')
    opt.add_option('--file-mon', action='store_true', default=False, dest='DFILEMON',
        help='Enable file monitoring (Needs efsw library)')
    opt.add_option('--platform', action='store', default='', dest='PLATFORM', 
        help='Define custom target platform')

# Compiler Helper
class Compiler:
    @staticmethod
    def check_symbol(conf, statement, header, symbol_name, required=False, lib=''):
        frag = '#include <{0}>\nint main() {{{1}; return 0;}}'.format(header, statement)
        r = conf.check_cc(fragment=frag, msg="Checking for symbol '" + symbol_name + "'",
            mandatory=required, lib=lib)
        if r:
            conf.define('HAVE_' + symbol_name.upper(), True)
        return r;

    @staticmethod
    def msvc(conf):
        cc = conf.env['CC'][0].lower()
        return ('cl.exe' in cc) or ('icl.exe' in cc)

    @staticmethod
    def icc(conf):
        cc = conf.env['CC'][0].lower()
        return 'icl.exe' in cc

    @staticmethod
    def clang(conf):
        cc = conf.env['CC'][0].lower()
        return 'clang' in cc

    @staticmethod
    def x64(conf):
        return (conf.env.DEST_CPU == 'x86_64' or conf.env.DEST_CPU == 'amd64')

    @staticmethod
    def arm(conf):
        return 'arm' in conf.env.DEST_CPU

    @staticmethod
    def platform(conf):
        return sys.platform if conf.options.PLATFORM=='' else conf.options.PLATFORM

def compiler_setup(conf):
    cflags = []
    cxxflags = []

    # general
    if Compiler.msvc(conf):
        cflags.extend(['/W3', '/fp:fast', '/FC', '/GS-', '/TP'])
        conf.env.append_unique('LINKFLAGS', ['/MANIFEST', '/NOLOGO'])
        conf.env['WINDOWS_EMBED_MANIFEST'] = True

        if Compiler.x64(conf):   conf.env.append_unique('LINKFLAGS', '/MACHINE:X64')
        else:                    conf.env.append_unique('LINKFLAGS', '/MACHINE:X86')

        conf.env.append_unique('DEFINES', ['_CRT_SECURE_NO_WARNINGS', '_CRT_NONSTDC_NO_DEPRECATE',
            '_SCL_SECURE_NO_WARNINGS', '_MBCS'])
        if compiler_is_icc(conf):
            cflags.extend(['/Qstd=c99', '/Qintel-extensions-', '/Qfast-transcendentals-',
                '/Qparallel-'])
    else:
        cflags.extend(['-std=gnu99', '-ffast-math', '-fPIC', '-Winline', '-Wall'])
        #conf.env.append_unique('DEFINES', ['_POSIX_C_SOURCE', '_GNU_SOURCE'])
        if not Compiler.arm(conf):
            cflags.extend(['-msse', '-msse2'])

    cxxflags.extend(cflags)

    if '-std=c99' in cxxflags:  del cxxflags[cxxflags.index('-std=c99')]
    if '/TP' in cxxflags:   del cxxflags[cxxflags.index('/TP')]

    conf.env.append_unique('CFLAGS', cflags)

    if not Compiler.arm(conf):
        conf.env.append_unique('DEFINES', '_SIMD_SSE_')
    conf.env.append_unique('DEFINES', 'HAVE_CONFIG_H')

    if conf.options.DASSERT: conf.env.append_unique('DEFINES', '_ENABLEASSERT_')
    conf.define('_VERSION_', VERSION)

    platform = Compiler.platform(conf)
    if platform == 'win32':
        conf.env.append_unique('DEFINES', ['WIN32', '_WIN_'])
        conf.env.append_unique('CXXFLAGS', cxxflags)
    elif platform.startswith('linux'):
        conf.env.append_unique('DEFINES', '_LINUX_')
    elif platform == 'darwin':
        conf.env.append_unique('DEFINES', '_OSX_')

    # general lib/include path
    conf.env.append_unique('INCLUDES', os.path.join(conf.env.ROOTDIR, 'include'))

    # build specific
    base_env = conf.env.derive()

    # debug
    cflags = []
    conf.setenv('debug', env=base_env.derive())
    if Compiler.msvc(conf):
        cflags.extend(['/Od', '/Z7', '/RTC1', '/MDd'])
        conf.env.append_unique('LINKFLAGS', '/DEBUG')
        conf.env.append_unique('CXXFLAGS', cflags)
    else:
        cflags.extend(['-g', '-O0'])
    conf.env.append_unique('CFLAGS', cflags)
    conf.env.append_unique('DEFINES', ['_DEBUG', '_DEBUG_', '_ENABLEASSERT_'])
    conf.env.SUFFIX = '-dbg'

    # release
    cflags = []
    conf.setenv('release', env=base_env.derive())
    if Compiler.msvc(conf):
        cflags.extend(['/O2', '/Oi', '/MD'])
        conf.env.append_unique('LINKFLAGS', '/RELEASE')
        conf.env.append_unique('CXXFLAGS', cflags)
    else:
        cflags.extend(['-O2', '-Wno-unused-result'])
    conf.env.append_unique('CFLAGS', cflags)
    conf.env.append_unique('DEFINES', 'NDEBUG')
    conf.env.SUFFIX = ''

def compiler_print_opts(conf):
    conf.start_msg('CC flags:')
    conf.end_msg(str.join(' ', conf.env.CFLAGS))

    if Compiler.msvc(conf):
        conf.start_msg('CXX flags:')
        conf.end_msg(str.join(' ', conf.env.CXXFLAGS))

    conf.start_msg('Linker flags:')
    conf.end_msg(str.join(' ', conf.env.LINKFLAGS))

    conf.start_msg('Defines:')
    conf.end_msg(str.join(', ',  conf.env.DEFINES))

def compiler_setup_config(conf):
    # backup current env
    base_env = conf.env.derive()

    have_malloc_h = conf.check_cc(header_name='malloc.h', mandatory=False)
    have_alloca_h = conf.check_cc(header_name='alloca.h', mandatory=False)
    have_alloca = False
    if have_alloca_h:
        have_alloca = Compiler.check_symbol(conf, 'alloca(0);', 'alloca.h', 'alloca')
    elif have_malloc_h:
        have_alloca = Compiler.check_symbol(conf, 'alloca(0);', 'malloc.h', 'alloca')
    if not have_alloca:
        Compiler.check_symbol(conf, '_alloca(0);', 'malloc.h', '_alloca', required=True)
        conf.define('alloca', '_alloca', quote=False)

    Compiler.check_symbol(conf, 'lrintf(0.0);', 'math.h', 'lrintf', lib='m')

    if not Compiler.arm(conf):
        sse_flags = ''
        if not Compiler.msvc(conf):
            sse_flags = '-msse -msse2'
        conf.check_cc(header_name='xmmintrin.h', cflags=sse_flags, define_ret=False)
        conf.check_cc(header_name='emmintrin.h', cflags=sse_flags, define_ret=False)

    platform = Compiler.platform(conf)
    if platform == 'win32':
        conf.check_cc(header_name='windows.h', define_ret=False)
    else:
        conf.check_cc(header_name='pthread.h', define_ret=False)

    if conf.options.DFILEMON:
        conf.check_cc(header_name='efsw/efsw.h')

    conf.write_config_header()
    conf.set_env(base_env)

def configure(conf):
    conf.check_waf_version(mini='1.7.11')
    conf.load('compiler_c compiler_cxx')

    compiler_setup_config(conf)

    # Set remaining option variables
    conf.env.ROOTDIR = ROOTDIR
    conf.env.PREFIX = os.path.abspath(conf.options.PREFIX)
    conf.env.VERSION = VERSION
    conf.env.BUILD_TESTS = conf.options.BUILD_TESTS
    conf.env.DFILEMON = conf.options.DFILEMON
    conf.env.PLATFORM = Compiler.platform(conf)

    conf.start_msg('Processor')
    conf.end_msg(conf.env.DEST_CPU)

    ## Install directory
    conf.start_msg('Install directory')
    conf.end_msg(conf.env.PREFIX)

    # Platform
    conf.start_msg('Platform')
    conf.end_msg(conf.env.PLATFORM)

    compiler_setup(conf)

def build(bld):
    bld.recurse('src')

    # Install headers
    if bld.env.PREFIX != bld.env.ROOTDIR:
        bld.install_files('${PREFIX}', bld.path.ant_glob('include/**/*.h'), relative_trick=True)
