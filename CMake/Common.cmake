macro (add_boost_library BOOST_LIB)
    list(APPEND Boost_INCLUDE_DIRS ${YUME_BOOST_PATH})
    list(REMOVE_DUPLICATES Boost_INCLUDE_DIRS)

    list(APPEND Boost_LIBRARIES boost_${BOOST_LIB})
    list(REMOVE_DUPLICATES Boost_LIBRARIES)
    get_target_property(_BL_EXISTS boost_${BOOST_LIB} TYPE)
    if (NOT _BL_EXISTS)
        add_subdirectory(${YUME_BOOST_PATH}/libs/${BOOST_LIB} ${CMAKE_BINARY_DIR}/boost/libs/${BOOST_LIB})
    endif()

endmacro (add_boost_library)


macro (link_boost_library PROJECT BOOST_LIB)
    add_boost_library(${BOOST_LIB})
    target_link_libraries(${PROJECT} boost_${BOOST_LIB})
endmacro(link_boost_library)
