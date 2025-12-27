add_executable(${PROJECT_NAME} WIN32)

target_link_libraries(${PROJECT_NAME} PRIVATE
  chomper::chomper-lib
)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
  "${CMAKE_CURRENT_BINARY_DIR}/include"
)

target_sources(${PROJECT_NAME} PRIVATE
  src/main.cpp
)

target_precompile_headers(${PROJECT_NAME} REUSE_FROM le2d)

klib_set_mainCRTStartup(${PROJECT_NAME})
