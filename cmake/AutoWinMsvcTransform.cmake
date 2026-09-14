# AutoWinMsvcTransform.cmake - Automated Source Transformation for MSVC
#
# Provides auto_win_msvc_transform_sources() to scan and rewrite unsupported
# C99 constructs (like VLAs) into strict C89 / MSVC-compatible code at build time.

function(auto_win_msvc_transform_sources)
    cmake_parse_arguments(TRANSFORM "" "OUTPUT_DIR;OUTPUT_VAR" "SOURCES" ${ARGN})

    if(NOT TRANSFORM_OUTPUT_DIR)
        set(TRANSFORM_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/src_gen")
    endif()
    if(NOT TRANSFORM_OUTPUT_VAR)
        set(TRANSFORM_OUTPUT_VAR "TRANSFORMED_SOURCES")
    endif()

    if(Python3_EXECUTABLE)
        set(_python_bin "${Python3_EXECUTABLE}")
    elseif(PYTHON_EXECUTABLE)
        set(_python_bin "${PYTHON_EXECUTABLE}")
    else()
        find_package(Python3 COMPONENTS Interpreter QUIET)
        if(Python3_EXECUTABLE)
            set(_python_bin "${Python3_EXECUTABLE}")
        else()
            set(_python_bin "python")
        endif()
    endif()

    get_filename_component(_transform_script "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../scripts/transform_sources.py" ABSOLUTE)

    set(_transformed_list "")
    file(MAKE_DIRECTORY "${TRANSFORM_OUTPUT_DIR}")

    foreach(src IN LISTS TRANSFORM_SOURCES)
        get_filename_component(src_abs "${src}" ABSOLUTE)
        get_filename_component(src_name "${src}" NAME)
        set(out_src "${TRANSFORM_OUTPUT_DIR}/${src_name}")

        # Transform immediately at configure time so headers/sources exist for tooling
        execute_process(
            COMMAND "${_python_bin}" "${_transform_script}" "${src_abs}" "${out_src}"
            RESULT_VARIABLE _res
        )

        # Register build-time dependency for automated rebuilds if source changes
        add_custom_command(
            OUTPUT "${out_src}"
            COMMAND "${_python_bin}" "${_transform_script}" "${src_abs}" "${out_src}"
            DEPENDS "${src_abs}" "${_transform_script}"
            COMMENT "Transforming ${src_name} for MSVC compatibility"
            VERBATIM
        )
        list(APPEND _transformed_list "${out_src}")
    endforeach()

    set(${TRANSFORM_OUTPUT_VAR} "${_transformed_list}" PARENT_SCOPE)
endfunction()
