set(SOURCE_FILE
    thread.cc
    thread_pool.cc
    )

add_library(momoko STATIC ${SOURCE_FILE})
target_link_libraries(momoko pthread)

install(TARGETS momoko DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include)
