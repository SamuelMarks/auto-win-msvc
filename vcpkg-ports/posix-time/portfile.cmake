vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO youruser/auto-win-msvc
    REF main
    SHA512 0  # TODO: update
    HEAD_REF main
)
vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}/posix-time")
vcpkg_cmake_install()
vcpkg_cmake_config_fixup()
file(INSTALL "${SOURCE_PATH}/posix-time/README.md" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
