cmake_minimum_required(VERSION 3.1)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.18.65.tar.gz"
    SHA1 "59cd3223f10ee630bc3596709a22f2fea4926a7c"
)

SET(VERSION_MAJOR "9")
SET(VERSION_MINOR "20")
SET(VERSION_PATCH "0")
SET(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

project(StormLib VERSION "${VERSION_STRING}")

set(LIBRARY_NAME stormlib)

set(SRC_FILES
    src/adpcm/adpcm.cpp
    src/huffman/huff.cpp
    src/jenkins/lookup3.c
    src/lzma/C/LzFind.c
    src/lzma/C/LzmaDec.c
    src/lzma/C/LzmaEnc.c
    src/pklib/explode.c
    src/pklib/implode.c
    src/sparse/sparse.cpp
    src/FileStream.cpp
    src/SBaseCommon.cpp
    src/SBaseDumpData.cpp
    src/SBaseFileTable.cpp
    src/SBaseSubTypes.cpp
    src/SCompression.cpp
    src/SFileAddFile.cpp
    src/SFileAttributes.cpp
    src/SFileCompactArchive.cpp
    src/SFileCreateArchive.cpp
    src/SFileExtractFile.cpp
    src/SFileFindFile.cpp
    src/SFileGetFileInfo.cpp
    src/SFileListFile.cpp
    src/SFileOpenArchive.cpp
    src/SFileOpenFileEx.cpp
    src/SFilePatchArchives.cpp
    src/SFileReadFile.cpp
    src/SFileVerify.cpp
   
    src/libtomcrypt/src/pk/rsa/rsa_verify_simple.c
    src/thirdparty_extra_funcs.h
)

set(PUB_HEADERS
    src/StormLib.h
    src/StormPort.h
)

set(TEST_SRC_FILES
           test/StormTest.cpp
)

add_definitions(-D_7ZIP_ST -DBZ_STRICT_ANSI)

if(WIN32)
    if(MSVC)
        message(STATUS "Using MSVC")
        add_definitions(-DWIN32)
    else()
        message(STATUS "Using mingw")
    endif()
    #set(SRC_ADDITIONAL_FILES ${ZLIB_BZIP2_FILES} ${TOMCRYPT_FILES} ${TOMMATH_FILES})
    set(LINK_LIBS wininet)
endif()

hunter_add_package(ZLIB)
hunter_add_package(BZip2)
hunter_add_package(tomcrypt)
hunter_add_package(tommath)

find_package(ZLIB CONFIG REQUIRED)
find_package(BZip2 CONFIG REQUIRED)
find_package(tomcrypt CONFIG REQUIRED)

set(LINK_LIBS ZLIB::zlib BZip2::bz2 tomcrypt::tomcrypt)

if(${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD)
    message(STATUS "Using FreeBSD port")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DO_LARGEFILE=0 -Dstat64=stat -Dlstat64=lstat -Dlseek64=lseek -Doff64_t=off_t -Dfstat64=fstat -Dftruncate64=ftruncate")
endif()

option(BUILD_SHARED_LIBS "Compile shared libraries" OFF)

add_library(${LIBRARY_NAME} ${SRC_FILES} ${PUB_HEADERS})
target_link_libraries(${LIBRARY_NAME} ${LINK_LIBS})
target_compile_definitions(${LIBRARY_NAME} PUBLIC -D__STORMLIB_SELF__) #CMake should take care of the linking
target_include_directories(${LIBRARY_NAME} PUBLIC src/)
set_target_properties(${LIBRARY_NAME} PROPERTIES PUBLIC_HEADER "src/StormLib.h;src/StormPort.h")
if(BUILD_SHARED_LIBS)
    if(APPLE)
        set_target_properties(${LIBRARY_NAME} PROPERTIES FRAMEWORK true)
        set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-framework Carbon")
    endif()
    if(UNIX)
        set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION "${VERSION_STRING}")
        set_target_properties(${LIBRARY_NAME} PROPERTIES SOVERSION "${VERSION_MAJOR}")
    endif()

endif()

option(WITH_TEST "Compile Test application" OFF)
if(WITH_TEST)
    add_executable(storm_test ${TEST_SRC_FILES})
    target_link_libraries(storm_test ${LIBRARY_NAME})
    install(TARGETS storm_test DESTINATION bin)
endif()


#######################################
#       cmake config-mode stuff       #
#######################################
set(PROJECT_NAME "${LIBRARY_NAME}")

set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated/cmake")

set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)

# Note: PROJECT_VERSION is used as a VERSION
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Note: variable 'targets_export_name' used
configure_file("Config.cmake.in" "${project_config}" @ONLY)


install(
    TARGETS "${PROJECT_NAME}"
    EXPORT "${targets_export_name}"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "${include_install_dir}"
)

install(
    FILES ${PUB_HEADERS}
    DESTINATION "${include_install_dir}"
)


install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)


install(
    EXPORT "${targets_export_name}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)
