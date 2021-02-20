#
# SPDX-License-Identifier: WTFPL
# SPDX-FileCopyrightText: 2021 Anton Kikin <a.kikin@tano-systems.com>
#
file(GLOB_RECURSE TEST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.sh)

set(PWM_TEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Works only for CMake 3.17+
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

foreach(FILE ${TEST_FILES})
	set(NAME ${FILE})
	set(COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
	string(REPLACE ".sh" "" NAME ${NAME})
	add_test(NAME ${NAME} COMMAND ${COMMAND})
	set_property(TEST ${NAME}
		PROPERTY ENVIRONMENT
			PWM_VERSION=${PWM_VERSION}
			PWM_TEST_BIN=${PWM_TEST_BIN}
			PWM_TEST_ROOT=${PWM_TEST_ROOT}
	)
endforeach(FILE ${TEST_FILES})

add_custom_target(build_and_test
	COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
	DEPENDS ${PWM_TEST_NAME}
)
