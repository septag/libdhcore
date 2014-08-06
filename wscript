# 
import os, sys, platform, inspect, glob

# main global variables
VERSION = "0.5.1"
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
    if sys.platform != 'win32':
        opt.load('compiler_c')
    else:
        opt.load('compiler_c compiler_cxx')

    # fetch from environment variables as default
    opt.add_option('--enable-assert', action='store_true', default=False, dest='DASSERT',
        help='Enable assertions (debug build always have assertions)')
    opt.add_option('--retail-build', action='store_true', default=False, dest='DRETAIL',
        help='Retail build (full optimization)')
    opt.add_option('--cross-compile', action='store', default='', dest='CROSS_COMPILE',
        help='Use a cross-compiler, must provide compiler executable name/path')
    opt.add_option('--prefix', action='store', default='', dest='PREFIX',
        help='Install path prefix')
    opt.add_option('--build-tests', action='store_true', default=False, dest='BUILD_TESTS',
        help='Build test programs')
    opt.add_option('--file-mon', action='store_true', default=False, dest='DFILEMON',
        help='Enable file monitoring (needs efsw library)')

def compiler_load(conf):
    if conf.options.CROSS_COMPILE != '':
        conf.env['CC'] = conf.options.CROSS_COMPILE

def compiler_check_symbol(conf, statement, header, symbol_name, required=False, lib=''):
    frag = '#include <{0}>\nint main() {{{1}; return 0;}}'.format(header, statement)
    r = conf.check_cc(fragment=frag, msg="Checking for symbol '" + symbol_name + "'",
        mandatory=required, lib=lib)
    if r:
        conf.define('HAVE_' + symbol_name.upper(), True)
    return r;

def compiler_is_msvc(conf):
    cc = conf.env['CC'][0].lower()
    if 'cl.exe' in cc or 'icl.exe' in cc:
        return True
    else:
        return False

def compiler_is_icc(conf):
    cc = conf.env['CC'][0].lower()
    if 'icl.exe' in cc:
        return True
    else:
        return False

def compiler_is_clang(conf):
    cc = conf.env['CC'][0].lower()
    if 'clang' in cc:
        return True
    else:
        return False

def compiler_is_x64(conf):
    return (conf.env.DEST_CPU == 'x86_64' or conf.env.DEST_CPU == 'amd64')

def compiler_is_arm(conf):
    return 'arm' in conf.env.DEST_CPU

def compiler_setup(conf):
    cflags = []
    cxxflags = []

    # general
    if compiler_is_msvc(conf):
        cflags.extend(['/W3', '/fp:fast', '/FC', '/GS-', '/TP'])
        conf.env.append_unique('LINKFLAGS', ['/MANIFEST', '/NOLOGO'])
        conf.env['WINDOWS_EMBED_MANIFEST'] = True

        if compiler_is_x64(conf):   conf.env.append_unique('LINKFLAGS', '/MACHINE:X64')
        else:                       conf.env.append_unique('LINKFLAGS', '/MACHINE:X86')

        conf.env.append_unique('DEFINES', ['_CRT_SECURE_NO_WARNINGS', '_CRT_NONSTDC_NO_DEPRECATE',
            '_SCL_SECURE_NO_WARNINGS', '_MBCS'])
        if compiler_is_icc(conf):
            cflags.extend(['/Qstd=c99', '/Qintel-extensions-', '/Qfast-transcendentals-',
                '/Qparallel-'])
    else:
        cflags.extend(['-std=c99', '-ffast-math', '-fPIC', '-Winline', '-Wall'])
        conf.env.append_unique('DEFINES', ['_POSIX_C_SOURCE', '_GNU_SOURCE'])
        if not compiler_is_arm(conf):
            if compiler_is_clang(conf):
                cflags.extend(['-msse', '-msse2', '-msse4.1'])
            else:    
                cflags.extend(['-msse', '-msse2', '-msse4.1', '-malign-double', 
                    '-fvisibility=internal'])
        if compiler_is_clang(conf):
            cflags.extend(['-fms-extensions'])

    cxxflags.extend(cflags)

    if '-std=c99' in cxxflags:  del cxxflags[cxxflags.index('-std=c99')]
    if '/TP' in cxxflags:   del cxxflags[cxxflags.index('/TP')]

    if not compiler_is_clang(conf):
        if '-fvisibility=internal' in cxxflags:  
            del cxxflags[cxxflags.index('-fvisibility=internal')]

    conf.env.append_unique('CFLAGS', cflags)

    if not compiler_is_arm(conf):
        conf.env.append_unique('DEFINES', '_SIMD_SSE_')
    conf.env.append_unique('DEFINES', 'HAVE_CONFIG_H')

    if conf.options.DASSERT: conf.env.append_unique('DEFINES', '_ENABLEASSERT_')
    if conf.options.DRETAIL: conf.env.append_unique('DEFINES', '_RETAIL_')
    conf.define('_VERSION_', VERSION)

    if sys.platform == 'win32':
        conf.env.append_unique('DEFINES', ['WIN32', '_WIN_'])
        conf.env.append_unique('CXXFLAGS', cxxflags)
    elif sys.platform.startswith('linux'):
        conf.env.append_unique('DEFINES', '_LINUX_')
    elif sys.platform == 'darwin':
        conf.env.append_unique('DEFINES', '_OSX_')

    # general lib/include path
    conf.env.append_unique('INCLUDES', os.path.join(conf.env.ROOTDIR, 'include'))

    # build specific
    base_env = conf.env.derive()

    # debug
    cflags = []
    conf.setenv('debug', env=base_env.derive())
    if compiler_is_msvc(conf):
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
    if compiler_is_msvc(conf):
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

    if compiler_is_msvc(conf):
        conf.start_msg('CXX flags:')
        conf.end_msg(str.join(' ', conf.env.CXXFLAGS))

    conf.start_msg('Linker flags:')
    conf.end_msg(str.join(' ', conf.env.LINKFLAGS))

    conf.start_msg('Defines:')
    conf.end_msg(str.join(', ',  conf.env.DEFINES))

def compiler_setup_deps(conf):
    # backup current env
    base_env = conf.env.derive()

    have_malloc_h = conf.check_cc(header_name='malloc.h', mandatory=False)
    have_alloca_h = conf.check_cc(header_name='alloca.h', mandatory=False)
    have_alloca = False
    if have_alloca_h:
        have_alloca = compiler_check_symbol(conf, 'alloca(0);', 'alloca.h', 'alloca')
    elif have_malloc_h:
        have_alloca = compiler_check_symbol(conf, 'alloca(0);', 'malloc.h', 'alloca')
    if not have_alloca:
        compiler_check_symbol(conf, '_alloca(0);', 'malloc.h', '_alloca', required=True)
        conf.define('alloca', '_alloca', quote=False)

    compiler_check_symbol(conf, 'lrintf(0.0);', 'math.h', 'lrintf', lib='m')

    if not compiler_is_arm(conf):
        sse_flags = ''
        if not compiler_is_msvc(conf):
            sse_flags = '-msse -msse2 -msse4.1'
        conf.check_cc(header_name='xmmintrin.h', cflags=sse_flags, define_ret=False)
        conf.check_cc(header_name='emmintrin.h', cflags=sse_flags, define_ret=False)
        conf.check_cc(header_name='smmintrin.h', cflags=sse_flags, define_ret=False)

    if sys.platform == 'win32':
        conf.check_cc(header_name='windows.h', define_ret=False)
    else:
        conf.check_cc(header_name='pthread.h', define_ret=False)

    # check for deps and SDKs
    prefix = os.path.abspath(conf.options.PREFIX)
    if prefix != ROOTDIR:
        conf.define('SHARE_DIR', os.path.join(prefix, 'share', PROJNAME).replace('\\', '\\\\'))
    else:
        conf.define('SHARE_DIR', prefix.replace('\\', '\\\\'))

    if conf.options.DFILEMON:
        conf.check_cc(header_name='efsw/efsw.h')

    conf.write_config_header()
    conf.set_env(base_env)

def configure(conf):
    conf.check_waf_version(mini='1.7.11')
    compiler_load(conf)
    if sys.platform != 'win32':
        conf.load('compiler_c')
    else:
        conf.load('compiler_c compiler_cxx')

    compiler_setup_deps(conf)

    # set remaining option variables
    conf.env.ROOTDIR = ROOTDIR
    conf.env.PREFIX = os.path.abspath(conf.options.PREFIX)
    conf.env.VERSION = VERSION
    conf.env.BUILD_TESTS = conf.options.BUILD_TESTS
    conf.env.DFILEMON = conf.options.DFILEMON

    conf.start_msg('Processor')
    conf.end_msg(conf.env.DEST_CPU)

    ## install directory
    conf.start_msg('Install directory')
    conf.end_msg(conf.env.PREFIX)

    compiler_setup(conf)

def build(bld):
    bld.recurse('src')

    # install headers
    if bld.env.PREFIX != bld.env.ROOTDIR:
        bld.install_files('${PREFIX}', bld.path.ant_glob('include/**/*.h'), relative_trick=True)
