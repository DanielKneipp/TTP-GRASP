file(GLOB GRASP_HEADERS "*.hpp")
file(GLOB GRASP_SOURCES "*.cpp")

source_group("GRASP" FILES
    ${GRASP_SOURCES}
    ${GRASP_HEADERS}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(libgrasp STATIC ${GRASP_SOURCES} ${GRASP_HEADERS})
target_link_libraries(libgrasp libttp)
