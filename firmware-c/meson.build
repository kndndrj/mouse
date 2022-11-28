project(
  'mouse',
  'c',
  meson_version: '>=0.60.1',
  version: '0.1.0',
)

##
## Flag presets (provided by HAL drivers)
##
# Compiler
cargs = []
# Linker
linkargs = []
# Dependencies
deps = []

##
## HAL drivers
##
# HAL driver is specified in crossfile
_fallback = 'none'
hal = meson.get_external_property('hal_driver', _fallback)
if hal == _fallback
  error('No HAL driver specified!')
endif

hal = 'hal/' + hal

fs = import('fs')
if not fs.is_dir(hal)
  error('HAL driver ' + hal + ' does not exist!')
endif

# Source the appropriate hal driver
subdir(hal)

##
## Drivers (external chips etc.)
##
subdir('drivers')


##
## Sources
##
sources = [
  'src/main.c',
]

##
## Targets
##
exe = executable(
  meson.project_name(),
  sources,
  name_suffix: 'elf',
  c_args: cargs,
  link_args: linkargs,
  # include_directories : includes,
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
