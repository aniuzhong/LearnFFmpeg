project(Step1)

add_executable(Step1 src/main.c)
target_link_directories(Step1 PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(Step1 ${FFMPEG_LIBRARIES} ${SDL2_LIBRARY})
install(TARGETS Step1 RUNTIME DESTINATION ${BIN_DIR})

