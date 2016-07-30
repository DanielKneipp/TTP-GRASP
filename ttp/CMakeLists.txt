file(GLOB TTP_HEADERS "*.hpp")
file(GLOB TTP_SOURCES "*.cpp")

source_group("TTP" FILES
    ${TTP_SOURCES}
    ${TTP_HEADERS}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(libttp STATIC ${TTP_SOURCES} ${TTP_HEADERS})
