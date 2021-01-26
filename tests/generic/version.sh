#!/bin/sh

function do_test {
	local RET
	local SYSFS
	local ENABLE
	local PERIOD
	local DUTY_CYCLE

	# Create sysfs root + chip folder + channel folder
	test_sysfs_create ${DEFAULT_PWM_CHIP} ${DEFAULT_PWM_CHANNEL} SYSFS

	STDOUT=$(${PWM_TEST_BIN} --version)
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"
	test_assert_eq "${STDOUT}" "${PWM_VERSION}" "stdout contents"

	test_sysfs_read ${SYSFS} ENABLE PERIOD DUTY_CYCLE

	test_assert_eq "${ENABLE}" "" "enable data check"
	test_assert_eq "${PERIOD}" "" "period data check"
	test_assert_eq "${DUTY_CYCLE}" "" "duty_cycle data check"

	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
