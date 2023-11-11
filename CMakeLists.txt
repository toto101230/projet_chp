cmake_minimum_required(VERSION 3.16)
project(projet_chp C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")


set(SOURCE_FILES main.c)
#        projet_base/multiattack.c
#        projet_base/my_sha_dict_generator.c
#        projet_base/my_sha_shadow_generator.c)

add_executable(projet_chp ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC ssl crypto)