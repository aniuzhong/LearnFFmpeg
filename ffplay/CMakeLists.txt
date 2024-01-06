add_executable(step1 src/step1.c)
target_link_directories(step1 PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(step1 ${FFMPEG_LIBRARIES} ${SDL2_LIBRARY})
install(TARGETS step1 RUNTIME DESTINATION ${BIN_DIR})

