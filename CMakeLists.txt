cmake_minimum_required(VERSION 3.0)

project(pwm-tool C)
include(GNUInstallDirs)
set(PWM_VERSION 1.0.0)

add_definitions(-Os -Wall -Werror --std=gnu99 -D_GNU_SOURCE)
add_definitions(-DPWM_VERSION="${PWM_VERSION}")

include_directories(
	src
)

set(SOURCES src/main.c src/pwm.c)
set(LIBS m)

add_executable(pwm ${SOURCES})
target_link_libraries(pwm ${LIBS})
install(TARGETS pwm RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(PWM_TEST_NAME pwm-test)
set(PWM_TEST_BIN ${CMAKE_CURRENT_BINARY_DIR}/${PWM_TEST_NAME})

add_executable(${PWM_TEST_NAME} EXCLUDE_FROM_ALL ${SOURCES})
target_link_libraries(${PWM_TEST_NAME} ${LIBS})
add_dependencies(${PWM_TEST_NAME} pwm)

target_compile_definitions(${PWM_TEST_NAME}
	PRIVATE
	TESTS=1
	SYSFS_PWM_ROOT="./pwmroot"
)

enable_testing()
add_subdirectory(tests)
