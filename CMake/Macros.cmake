macro (add_boost_library BOOST_LIB)

    if (WITH_SYSTEM_BOOST)
        list(APPEND Boost_COMPONENTS ${BOOST_LIB})
        list(REMOVE_DUPLICATES Boost_COMPONENTS)
        find_package(Boost COMPONENTS "${Boost_COMPONENTS}" REQUIRED)
    else()
        list(APPEND Boost_INCLUDE_DIRS ${YUME_BOOST_PATH})
        list(REMOVE_DUPLICATES Boost_INCLUDE_DIRS)

        list(APPEND Boost_LIBRARIES boost_${BOOST_LIB})
        list(REMOVE_DUPLICATES Boost_LIBRARIES)
        get_target_property(_BL_EXISTS boost_${BOOST_LIB} TYPE)
        if (NOT _BL_EXISTS)
            add_subdirectory(${YUME_BOOST_PATH}/libs/${BOOST_LIB} ${CMAKE_BINARY_DIR}/boost/libs/${BOOST_LIB})
        endif()
    endif()

endmacro (add_boost_library)


macro (link_boost_library PROJECT BOOST_LIB)
    add_boost_library(${BOOST_LIB})
    if (WITH_SYSTEM_BOOST)
        string ( TOUPPER ${BOOST_LIB} BOOST_LIB_UPPER )
        target_link_libraries ( ${PROJECT} ${Boost_${BOOST_LIB_UPPER}_LIBRARY} )
    else ()
        target_link_libraries(${PROJECT} boost_${BOOST_LIB})
    endif()
endmacro(link_boost_library)

function (yume_check_boost)
    message(${YUME_BOOST_PATH})
    message(${YUME_ROOT_DIR})
    if (NOT WITH_SYSTEM_BOOST)
        if (NOT EXISTS ${YUME_BOOST_PATH})
            file(MAKE_DIRECTORY ${YUME_BOOST_PATH})
        endif()
        if (NOT EXISTS ${YUME_BOOST_PATH}/boost/)
            set (FB_URL "https://github.com/firebreath/firebreath-boost/tarball/1.50.0")
            message("Boost not found; downloading latest FireBreath-boost from GitHub (http://github.com/firebreath/firebreath-boost)")
            find_program(GIT git
                PATHS
                )
            if (FIREBREATH_AUTO_GIT AND EXISTS ${YUME_ROOT_DIR}/.git AND NOT ${GIT} MATCHES "GIT-NOTFOUND")
                message("Using git")
                execute_process(
                    COMMAND ${GIT}
                    submodule update --recursive --init
                    WORKING_DIRECTORY "${YUME_ROOT_DIR}")
            else()
                message("Downloading...")
                find_program(CURL curl)
                find_program(WGET wget PATHS "${YUME_ROOT_DIR}/Cmake/")
                proxy_setup()
                if (NOT ${WGET} MATCHES "WGET-NOTFOUND")
                    execute_process(
                        COMMAND ${WGET}
                        --no-check-certificate
                        -O "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz"
                        "${FB_URL}"
                        )
                elseif (NOT ${CURL} MATCHES "CURL-NOTFOUND")
                    execute_process(
                        COMMAND ${CURL}
                        -L "${FB_URL}"
                        -k
                        OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz"
                        )
                else()
                    file (DOWNLOAD "${FB_URL}" "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz" STATUS DL_STATUS SHOW_PROGRESS)
                    message("Result: ${DL_STATUS}")
                endif()
                if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz")
                    message("Error downloading firebreath-boost. Please get firebreath-boost from ")
                    message("http://github.com/firebreath/firebreath-boost and install it into")
                    message("the src/3rdParty/boost directory.  When properly installed, you should have:")
                    message("    src/3rdParty/boost/boost/")
                    message("    src/3rdParty/boost/libs/")
                    message(FATAL_ERROR "firebreath-boost could not be installed. Please install manually.")
                endif()
                find_program(TAR tar NO_DEFAULT_PATHS)
                find_program(GZIP gzip NO_DEFAULT_PATHS)
                if (WIN32)
                    message("Using 7-zip to extract the archive")
                    find_program(SEVZIP 7za PATHS "${YUME_ROOT_DIR}/CMake/")
                    execute_process(
                        COMMAND ${SEVZIP}
                        e "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz"
                        WORKING_DIRECTORY "${YUME_BOOST_PATH}"
                        )
                    file (GLOB TAR_FILE
                        ${YUME_BOOST_PATH}/*.tar
                        )
                    execute_process(
                        COMMAND ${SEVZIP}
                        x "${TAR_FILE}"
                        WORKING_DIRECTORY "${YUME_BOOST_PATH}"
                        OUTPUT_QUIET
                        )
                    file(REMOVE ${TAR_FILE})
                elseif (NOT ${TAR} MATCHES "TAR-NOTFOUND" AND NOT ${GZIP} MATCHES "GZIP-NOTFOUND")
                    message("Using tar xzf to extract the archive")
                    execute_process(
                        COMMAND ${TAR}
                        xzf "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz"
                        WORKING_DIRECTORY "${YUME_BOOST_PATH}"
                        OUTPUT_QUIET
                        )
                else()
                    message("Please extract ${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz and ")
                    message("move the boost/ and libs/ subdirectories ")
                    message("to ${YUME_BOOST_PATH}/boost and ${YUME_BOOST_PATH}/libs")
                    message(FATAL_ERROR "Firebreath-boost not installed! (follow above directions to resolve)")
                endif()
                #file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz)
                message("Installing firebreath-boost...")
                file (GLOB _BOOST_FILES
                    ${YUME_BOOST_PATH}/firebreath-firebreath-boost*/*)
                foreach (_CUR_FILE ${_BOOST_FILES})
                    get_filename_component (_CUR_FILENAME ${_CUR_FILE} NAME)
                    file(RENAME ${_CUR_FILE} ${YUME_BOOST_PATH}/${_CUR_FILENAME})
                endforeach()
                message("Cleaning up firebreath-boost temp install files...")
                file (GLOB _BOOST_TMPFILES
                    ${YUME_BOOST_PATH}/firebreath*
                    ${YUME_BOOST_PATH}/pax*)
                file (REMOVE_RECURSE ${_BOOST_TMPFILES})
            endif()
        endif()
    endif()
endfunction()

function(proxy_setup)
    SET( HTTP_PROXY $ENV{HTTP_PROXY} )
    SET( HTTPS_PROXY $ENV{HTTPS_PROXY} )
    SET( http_proxy $ENV{http_proxy} )
    SET( https_proxy $ENV{https_proxy} )
    if(HTTP_PROXY AND NOT HTTPS_PROXY)
        message("!!!! WARNING: HTTP_PROXY env var set, but we need HTTPS_PROXY. Attempting to use HTTP_PROXY FOR HTTPS_PROXY")
        set(ENV{HTTPS_PROXY} ${HTTP_PROXY})
    endif()

    if(http_proxy AND NOT https_proxy)
        message("!!!! WARNING: http_proxy env var set, but we need https_proxy. Attempting to use http_proxy FOR https_proxy")
        set(ENV{https_proxy} ${http_proxy})
    endif()
    #message($ENV{https_proxy})
    #message($ENV{HTTPS_PROXY})
endfunction(proxy_setup)
