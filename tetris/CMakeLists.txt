cmake_minimum_required(VERSION 3.8)

project(tetris LANGUAGES C VERSION 1)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_executable(tetris
	tet.c
	graphics.c
	utils.c
	audio.c
	font.c
	input.c
	timer.c
        mtimer.h
        main.c
)
target_link_libraries(tetris SDL2::SDL2 OpenGL::OpenGL GLEW::GLEW m)
