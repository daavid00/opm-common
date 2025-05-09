# - Turn on warnings when compiling

include (AddOptions)
include (UseCompVer)
is_compiler_gcc_compatible ()

if (CXX_COMPAT_GCC)
  # default warnings flags, if not set by user
  set_default_option (CXX _warn_flag "-Wall -Wextra -Wshadow" "(^|\ )-W")
  if (_warn_flag)
	message (STATUS "All warnings enabled: ${_warn_flag}")
	add_options (ALL_LANGUAGES ALL_BUILDS "${_warn_flag}")
  endif (_warn_flag)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13)
    add_options (CXX ALL_BUILDS -Wno-dangling-reference)
  endif()
endif ()

option(SILENCE_EXTERNAL_WARNINGS "Disable some warnings from external packages (requires GCC 4.6 or newer)" OFF)
if(SILENCE_EXTERNAL_WARNINGS AND CXX_COMPAT_GCC)
  add_definitions(-DSILENCE_EXTERNAL_WARNINGS)
endif()
