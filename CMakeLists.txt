cmake_minimum_required(VERSION 3.16)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(ArduFocuser CXX)

set(ARDUFOCUSER_VERSION_MAJOR 0)
set(ARDUFOCUSER_VERSION_MINOR 1)

# Where to find our custom Find*.cmake modules (e.g. FindINDI.cmake)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

# Standard INDI install paths
set(BIN_INSTALL_DIR     "${CMAKE_INSTALL_PREFIX}/bin")
set(DATA_INSTALL_DIR    "${CMAKE_INSTALL_PREFIX}/share/indi/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Dependencies ──────────────────────────────────────────────────────────────

find_package(INDI REQUIRED)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}          # for generated ardufocuser_config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/driver
    ${INDI_INCLUDE_DIR}
)

# ── Sources ───────────────────────────────────────────────────────────────────

set(ARDUFOCUSER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/ardufocuser.cpp
)

# ── Target ────────────────────────────────────────────────────────────────────

add_executable(indi_ardufocuser ${ARDUFOCUSER_SRCS})
target_link_libraries(indi_ardufocuser indidriver)

# ── Generated files ───────────────────────────────────────────────────────────

# Generates ardufocuser_config.h in the build tree from the .in template
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/ardufocuser_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/ardufocuser_config.h
)

# Generates the INDI XML descriptor in the build tree
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/indi_ardufocuser.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/indi_ardufocuser.xml
)

# ── Install ───────────────────────────────────────────────────────────────────

install(TARGETS indi_ardufocuser RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/indi_ardufocuser.xml
        DESTINATION ${DATA_INSTALL_DIR})