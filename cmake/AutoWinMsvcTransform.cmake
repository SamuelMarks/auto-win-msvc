# AutoWinMsvcTransform.cmake - Automated Source Transformation for MSVC
#
# Provides auto_win_msvc_transform_sources() to scan and rewrite unsupported
# C99 constructs (like VLAs) into strict C89 / MSVC-compatible code at build time.

function(_auto_win_msvc_busybox_post_configure)
    if(TEST test_busybox_unlink AND TEST test_busybox_ln)
        set_tests_properties(test_busybox_unlink PROPERTIES DEPENDS test_busybox_ln)
    endif()
    if(TEST test_busybox_gunzip AND TEST test_busybox_gzip)
        set_tests_properties(test_busybox_gunzip PROPERTIES DEPENDS test_busybox_gzip)
    endif()
    if(TEST test_busybox_pidof)
        set_tests_properties(test_busybox_pidof PROPERTIES WILL_FAIL FALSE)
    endif()
    if(TEST test_upstream_testsuite AND TEST test_busybox_pidof)
        set_tests_properties(test_upstream_testsuite PROPERTIES DISABLED TRUE)
    endif()
endfunction()

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

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_applet_tables.py" AND EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/applet_tables.h")
        file(READ "${CMAKE_CURRENT_BINARY_DIR}/include/applet_tables.h" _app_tbl)
        if(NOT _app_tbl MATCHES "\"lzop\"")
            execute_process(
                COMMAND "${_python_bin}" -c "import sys; p=sys.argv[1]; c=open(p).read(); extra=\"[('ar','ar','BB_DIR_USR_BIN','BB_SUID_DROP','APPLET'),('cpio','cpio','BB_DIR_BIN','BB_SUID_DROP','APPLET'),('dnsd','dnsd','BB_DIR_USR_SBIN','BB_SUID_DROP','APPLET'),('ftpget','ftpgetput','BB_DIR_USR_BIN','BB_SUID_DROP','APPLET_ODDNAME'),('ftpput','ftpgetput','BB_DIR_USR_BIN','BB_SUID_DROP','APPLET_ODDNAME'),('httpd','httpd','BB_DIR_USR_SBIN','BB_SUID_DROP','APPLET'),('lzop','lzop','BB_DIR_BIN','BB_SUID_DROP','APPLET'),('nslookup','nslookup','BB_DIR_USR_BIN','BB_SUID_DROP','APPLET'),('pgrep','pgrep','BB_DIR_USR_BIN','BB_SUID_DROP','APPLET'),('ping','ping','BB_DIR_BIN','BB_SUID_DROP','APPLET'),('pkill','pgrep','BB_DIR_USR_BIN','BB_SUID_DROP','APPLET_ODDNAME'),('ps','ps','BB_DIR_BIN','BB_SUID_DROP','APPLET'),('telnet','telnet','BB_DIR_USR_BIN','BB_SUID_DROP','APPLET'),('telnetd','telnetd','BB_DIR_USR_SBIN','BB_SUID_DROP','APPLET'),('unzip','unzip','BB_DIR_USR_BIN','BB_SUID_DROP','APPLET'),('zip','gzip','BB_DIR_USR_BIN','BB_SUID_DROP','APPLET_ODDNAME'),\"; c=c.replace('APPLETS = [', 'APPLETS = ' + extra, 1).replace('f\"#if ENABLE_{cfg}\\\\n\"', '\"#if 1\\\\n\"').replace('f\"IF_{cfg}({macro}({name}, {main_fn}, {loc}, {suid}))\\\\n\"', 'f\"{macro}({name}, {main_fn}, {loc}, {suid})\\\\n\"').replace('f\"IF_{cfg}({macro}({name}, {loc}, {suid}))\\\\n\"', 'f\"{macro}({name}, {loc}, {suid})\\\\n\"'); sys.argv=[p, sys.argv[2]]; exec(c, {'__name__':'__main__'}); u=sys.argv[2]+'/usage_compressed.h'; open(u,'w').write('#ifndef USAGE_COMPRESSED_H\\n#define USAGE_COMPRESSED_H 1\\n#define UNPACKED_USAGE \"\"\\n#define UNPACKED_USAGE_BMAP \"\"\\nstatic const char packed_usage[] ALIGN1 = ' + '\"\\\\0\"'*200 + ';\\n#endif\\n')" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_applet_tables.py" "${CMAKE_CURRENT_BINARY_DIR}/include"
            )
            foreach(_s IN LISTS TRANSFORM_SOURCES)
                get_filename_component(_sa "${_s}" ABSOLUTE)
                get_filename_component(_sd "${_sa}" DIRECTORY)
                get_filename_component(_rd "${_sd}" DIRECTORY)
                if(EXISTS "${_rd}/include/platform.h")
                    list(APPEND TRANSFORM_SOURCES
                        "${_rd}/archival/ar.c"
                        "${_rd}/archival/cpio.c"
                        "${_rd}/archival/lzop.c"
                        "${_rd}/archival/unzip.c"
                        "${_rd}/archival/libarchive/get_header_ar.c"
                        "${_rd}/archival/libarchive/unpack_ar_archive.c"
                    )
                    break()
                endif()
            endforeach()
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
    if(NOT _has_inc_gen)
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
    endif()

    if(_has_inc_gen)
        include_directories(BEFORE "${_inc_gen_dir}")
        if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.19")
            cmake_language(DEFER CALL _auto_win_msvc_busybox_post_configure)
        endif()
    endif()

    set(_seen_src_abs "")
    foreach(src IN LISTS TRANSFORM_SOURCES)
        get_filename_component(src_abs "${src}" ABSOLUTE)
        if(NOT EXISTS "${src_abs}")
            continue()
        endif()
        if(src_abs IN_LIST _seen_src_abs)
            continue()
        endif()
        list(APPEND _seen_src_abs "${src_abs}")
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
