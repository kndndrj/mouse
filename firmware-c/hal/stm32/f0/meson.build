# Presets
libopencm3_path = '../../../external/libopencm3'
libopencm3_libname = 'opencm3_stm32f0'
libopencm3_cpu = 'STM32F0'
cpu = host_machine.cpu() == 'cortex-m0+' ? 'cortex-m0plus' : host_machine.cpu()

# Build libopencm3
message('Running make for libopencm3, this might take a while...')
run_command('make', '-C', libopencm3_path, check: true)
message('Succesfully built libopencm3')

# Declare libopencm3 as a dependency
libopencm3 = declare_dependency(
  link_args : [
    '-L' + meson.project_source_root() + '/' + libopencm3_path.strip('../').strip('./') + '/lib',
    '-l' + libopencm3_libname,
  ],
  include_directories : include_directories(libopencm3_path + '/include'),
)

dep = declare_dependency(
  include_directories: [
    include_directories('../../'),
    include_directories('../common'),
    ],
  sources: [
    'spi.c',
    'usb.c',
    'delay.c',
    'gpio.c',
  ],
)

deps += [libopencm3, dep]

# Compiler
cargs += [
  '-Os',
  '-std=c99',
  '-mcpu=' + cpu,
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
  '-D' + libopencm3_cpu,
]

# Linker
linkargs += [
  '--static',
  '-nostartfiles',
  '-T' + meson.current_source_dir() + '/' + 'f0.ld',
  '-mcpu=' + cpu,
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

