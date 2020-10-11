function(using_library_at library_name library_path)
  foreach (module_path ${CMAKE_MODULE_PATH})
    if (EXISTS "${module_path}/${library_path}")
      MESSAGE("Using custom library '${library_name}'")
      add_subdirectory("${module_path}/${library_path}")
      return()
    endif()
  endforeach()
  if (EXISTS "${MIDIUTILS_DEPENDENCY_DIR}/${library_path}")
    add_subdirectory("${MIDIUTILS_DEPENDENCY_DIR}/${library_path}" ${library_name})
    MESSAGE("Using dependant library '${library_name}'")
    return()
  endif()
  MESSAGE("Finding library '${library_name}'")
  find_package(${library_name} REQUIRED)
endfunction()

function(using_library library_name)
  using_library_at(${library_name} ${library_name})
endfunction()