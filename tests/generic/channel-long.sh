#!/bin/sh

function do_test {
	local CHANNEL=33
	local RET
	local SYSFS

	# Create sysfs root + chip folder + channel folder
	test_sysfs_create ${DEFAULT_PWM_CHIP} ${CHANNEL} SYSFS

	${PWM_TEST_BIN} --channel=${CHANNEL} -d 10
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"
	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc