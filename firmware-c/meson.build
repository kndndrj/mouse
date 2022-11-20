project(
  'mouse',
  'c',
  meson_version: '>=0.60.1',
  version: '0.1.0',
)

#
# Flag presets (provided by drivers)
#
# Compiler
cargs = []
# Linker
linkargs = []
# Dependencies
deps = []

#
# Drivers
#
# Driver is specified in crossfile
_fallback = 'none'
driver = meson.get_external_property('driver', _fallback)
if driver == _fallback
  error('No driver specified!')
endif

driver = 'drivers/' + driver

fs = import('fs')
if not fs.is_dir(driver)
  error('Driver ' + driver + ' does not exist!')
endif

# Source the appropriate driver
subdir(driver)

#
# Sources
#
# Includes
includes = include_directories(
  './inc',
)

sources = [
  'src/main.c',
  'src/delay.c',
  'src/pmw3360.c',
]

# Targets
exe = executable(
  meson.project_name(),
  sources,
  name_suffix: 'elf',
  c_args: cargs,
  link_args: linkargs,
  include_directories : includes,
  dependencies: deps,
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
