#!/bin/sh

function do_test {
	local RET

	# Create sysfs root + chip folder
	mkdir -p ${SYSFS_PWM_ROOT}/$(printf ${SYSFS_PWM_CHIP_FOLDER_FMT} ${DEFAULT_PWM_CHIP})

	${PWM_TEST_BIN}
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_NO_CHANNEL}" "return code"
	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
