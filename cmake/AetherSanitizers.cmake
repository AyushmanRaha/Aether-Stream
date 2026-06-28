include_guard(GLOBAL)

function(aether_enable_sanitizers target_name)
  if(NOT AETHER_ENABLE_ASAN AND NOT AETHER_ENABLE_UBSAN AND NOT AETHER_ENABLE_TSAN)
    return()
  endif()

  if(AETHER_ENABLE_ASAN AND AETHER_ENABLE_TSAN)
    message(FATAL_ERROR "AETHER_ENABLE_ASAN and AETHER_ENABLE_TSAN cannot be enabled together")
  endif()

  if(MSVC)
    message(WARNING "Aether-Stream sanitizer flags are not configured for MSVC")
    return()
  endif()

  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    message(WARNING "Aether-Stream sanitizer flags are only configured for Clang and GCC")
    return()
  endif()

  set(AETHER_SANITIZER_FLAGS)

  if(AETHER_ENABLE_ASAN)
    list(APPEND AETHER_SANITIZER_FLAGS -fsanitize=address)
  endif()

  if(AETHER_ENABLE_UBSAN)
    list(APPEND AETHER_SANITIZER_FLAGS -fsanitize=undefined)
  endif()

  if(AETHER_ENABLE_TSAN)
    list(APPEND AETHER_SANITIZER_FLAGS -fsanitize=thread)
  endif()

  target_compile_options(${target_name} PRIVATE ${AETHER_SANITIZER_FLAGS} -fno-omit-frame-pointer)
  target_link_options(${target_name} PRIVATE ${AETHER_SANITIZER_FLAGS})
endfunction()
