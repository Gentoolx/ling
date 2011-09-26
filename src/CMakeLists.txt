project(WntrLingLib)

file(GLOB WNTRLING_SOURCES "*.cpp")
file(GLOB WNTRLING_HEADERS "*.hpp")
QT4_WRAP_CPP(WNTRLING_SOURCES ${WNTRLING_HEADERS})

add_library(wntrling SHARED
    ${WNTRLING_SOURCES})

set_target_properties(wntrling PROPERTIES
    VERSION 0.0.1
    SOVERSION 0)

target_link_libraries(wntrling ${WNTRLING_LIBRARIES})

install(TARGETS wntrling
    EXPORT WntrLingLibraryDepends
    LIBRARY DESTINATION "${WNTRLING_LIB_DIR}")

install(EXPORT WntrLingLibraryDepends
    DESTINATION "${WNTRLING_CMAKE_DIR}")

install(FILES ${WNTRLING_HEADERS}
    DESTINATION "${WNTRLING_INCLUDE_DIR}"
    COMPONENT dev)
