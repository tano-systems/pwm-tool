#!/bin/sh

function do_test {
	local SYSFS
	local ENABLE
	local PERIOD
	local DUTY_CYCLE
	local RET
	local D1
	local D2

	# Create sysfs root + chip folder + channel folder
	test_sysfs_create ${DEFAULT_PWM_CHIP} ${DEFAULT_PWM_CHANNEL} SYSFS

	D1=$(date "+%s %N")
	${PWM_TEST_BIN}
	RET=$?
	D2=$(date "+%s %N")

	local DMS=$(date_diff_ms ${D2} ${D1})

	test_assert_range \
		${DMS} \
		${DEFAULT_PWM_DURATION_MS} \
		$((${DEFAULT_PWM_DURATION_MS} + 100)) \
		"execution duration"

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"

	test_sysfs_read ${SYSFS} ENABLE PERIOD DUTY_CYCLE

	test_assert_eq "${ENABLE}" "10" "enable data check"
	test_assert_eq "${PERIOD}" "1000000" "period data check"
	test_assert_eq "${DUTY_CYCLE}" "0500000" "duty_cycle data check"

	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
