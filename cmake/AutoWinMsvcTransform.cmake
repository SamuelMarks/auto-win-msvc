# AutoWinMsvcTransform.cmake - Automated Source Transformation for MSVC
#
# Provides auto_win_msvc_transform_sources() to scan and rewrite unsupported
# C99 constructs (like VLAs) into strict C89 / MSVC-compatible code at build time.

function(auto_win_msvc_transform_sources)
    cmake_parse_arguments(TRANSFORM "TRANSFORM_HEADERS" "OUTPUT_DIR;OUTPUT_VAR" "SOURCES;INCLUDE_DIRS" ${ARGN})

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

    # Detect upstream include directory containing platform.h / libbb.h
    set(_inc_gen_dir "${TRANSFORM_OUTPUT_DIR}/include")
    set(_has_inc_gen FALSE)
    foreach(_inc_dir IN LISTS TRANSFORM_INCLUDE_DIRS)
        if(EXISTS "${_inc_dir}")
            file(MAKE_DIRECTORY "${_inc_gen_dir}")
            file(GLOB _repo_hdrs "${_inc_dir}/*.h")
            foreach(_hdr_path IN LISTS _repo_hdrs)
                get_filename_component(_hdr "${_hdr_path}" NAME)
                execute_process(
                    COMMAND "${_python_bin}" "${_transform_script}" "${_hdr_path}" "${_inc_gen_dir}/${_hdr}"
                    RESULT_VARIABLE _hdr_res
                )
            endforeach()
            set(_has_inc_gen TRUE)
        endif()
    endforeach()
    foreach(src IN LISTS TRANSFORM_SOURCES)
        get_filename_component(src_abs "${src}" ABSOLUTE)
        get_filename_component(src_dir "${src_abs}" DIRECTORY)
        get_filename_component(repo_dir "${src_dir}" DIRECTORY)
        if(EXISTS "${repo_dir}/include/platform.h")
            file(MAKE_DIRECTORY "${_inc_gen_dir}")
            file(GLOB _repo_hdrs "${repo_dir}/include/*.h")
            foreach(_hdr_path IN LISTS _repo_hdrs)
                get_filename_component(_hdr "${_hdr_path}" NAME)
                execute_process(
                    COMMAND "${_python_bin}" "${_transform_script}" "${_hdr_path}" "${_inc_gen_dir}/${_hdr}"
                    RESULT_VARIABLE _hdr_res
                )
            endforeach()
            set(_has_inc_gen TRUE)
            break()
        endif()
    endforeach()

    if(_has_inc_gen)
        include_directories(BEFORE "${_inc_gen_dir}")
    endif()

    foreach(src IN LISTS TRANSFORM_SOURCES)
        get_filename_component(src_abs "${src}" ABSOLUTE)
        get_filename_component(src_name "${src}" NAME)
        get_filename_component(src_dir "${src_abs}" DIRECTORY)
        get_filename_component(parent_name "${src_dir}" NAME)

        set(out_src "${TRANSFORM_OUTPUT_DIR}/${src_name}")
        if(DEFINED _seen_target_${src_name})
            set(out_src "${TRANSFORM_OUTPUT_DIR}/${parent_name}_${src_name}")
        endif()
        set(_seen_target_${src_name} 1)

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
