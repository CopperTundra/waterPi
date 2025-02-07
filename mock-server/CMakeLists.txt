set(SOURCES_MOCK
main.cpp
)

add_executable(mock-server
    ${SOURCES_MOCK}
)

target_link_libraries(mock-server PRIVATE httplib)