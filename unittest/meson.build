##################
# Subdirectories #
##################

libblasr_unittest_sources = []
subdir('alignment')
subdir('hdf')
subdir('pbdata')

###########
# Sources #
###########

# find GoogleTest
libblasr_gtest_dep = dependency(
  'gtest',
  main : true,
  fallback : ['gtest', 'gtest_main_dep'])

libblasr_unittest = executable(
  'libblasr_unittest', [
    libblasr_libconfig_h,
    libblasr_unittest_sources],
  dependencies : [
    libblasr_deps,
    libblasr_gtest_dep],
  include_directories : libblasr_include_directories,
  link_with : libblasr_lib,
  cpp_args : libblasr_warning_flags,
  install : false)

#########
# tests #
#########

test(
  'libblasr formatting check',
  libblasr_clang_formatter,
  args : [
    '--all'],
  workdir : meson.source_root())

test(
  'libblasr gtest unittests',
  libblasr_unittest,
  args : [
    '--gtest_output=xml:' + join_paths(meson.build_root(), 'libblasr-gtest-unittests.xml')],
  env : [
    'ARGS=-V',
    'VERBOSE=1'],
  timeout : 3600)
