project(
  'mouse',
  'c',
  meson_version: '>=0.60.1',
  version: '0.1.0',
)

compiler = meson.get_compiler('c')

# Specific
cpu_type = 'cortex-m3'
cpu_define = 'STM32F1'
libopencm3_path = './libopencm3' # relative path
libopencm3_libname = 'opencm3_stm32f1'

# Includes
includes = include_directories(
  './include',
)

# Sources
sources = [
  'src/main.c',
  'src/delay.c',
  'src/PMW3360.c',
]

# libopencm3
message('Running make for libopencm3, this might take a while...')
libopencm3_make = run_command('make', '-C', libopencm3_path)
if libopencm3_make.returncode() == 0
  message('Succesfully built libopencm3')
else
  error('Failed to build libopencm3')
endif

libopencm3 = declare_dependency(
  link_args : [
    '-L' + meson.current_source_dir() + '/' + libopencm3_path.strip('../').strip('./') + '/lib',
    '-l' + libopencm3_libname
  ],
  include_directories : include_directories(libopencm3_path + '/include'),
)

# Compile arguments
cargs = [
  '-Os',
  '-std=c99',
  '-mcpu=' + cpu_type,
  '-mthumb',
  '-msoft-float',
  '-Wextra',
  '-Wshadow',
  '-Wimplicit-function-declaration',
  '-Wredundant-decls',
  '-Wmissing-prototypes',
  '-Wstrict-prototypes',
  '-fno-common',
  '-ffunction-sections',
  '-fdata-sections',
  '-Wundef',
  '-D' + cpu_define,
]

# Linker arguments
linkargs = [
  '--static',
  '-nostartfiles',
  '-T' + meson.current_source_dir() + '/' + meson.project_name() + '.ld',
  '-mcpu=' + cpu_type,
  '-mthumb',
  '-msoft-float',
  '-Wl,-Map=' + meson.project_name() + '.map',
  '-Wl,--cref',
  '-Wl,--gc-sections',
  '-lc',
  '-lgcc',
  '-lnosys',
]

# Debug specific options
if get_option('debug') == true
  cargs += '-ggdb3'
  linkargs += '-ggdb3'
endif

# Targets
exe = executable(
  meson.project_name(),
  sources,
  name_suffix: 'elf',
  c_args: cargs,
  link_args: linkargs,
  include_directories : includes,
  dependencies: [ libopencm3 ],
  build_by_default: true,
)

bin = custom_target(
  'bin',
  input: exe,
  output: meson.project_name() + '.bin',
  command: [ 'objcopy', '-Obinary', '@INPUT@', '@OUTPUT@' ],
  build_by_default: true,
  depends: exe,
)
