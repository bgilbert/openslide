openslide_common_sources = files(
  'openslide-common-cmdline.c',
  'openslide-common-fail.c',
  'openslide-common-fd.c',
  'openslide-common-file.c',
)

openslide_common = static_library(
  'openslide-common',
  openslide_common_sources,
  dependencies : [openslide_dep, glib_dep, valgrind_dep],
  include_directories : config_h_include,
)

openslide_common_dep = declare_dependency(
  include_directories : include_directories('.'),
  link_with : openslide_common,
)
