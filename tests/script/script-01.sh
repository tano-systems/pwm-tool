#!/bin/sh

function do_test {
	local RET
	local SYSFS
	local ENABLE
	local PERIOD
	local DUTY_CYCLE
	local D1
	local D2

	# Create sysfs root + chip folder + channel folder
	test_sysfs_create ${DEFAULT_PWM_CHIP} ${DEFAULT_PWM_CHANNEL} SYSFS

	D1=$(date "+%s %N")
	${PWM_TEST_BIN} --script="F1000D100 d50 f d50 f"
	RET=$?
	D2=$(date "+%s %N")

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"

	local DMS=$(date_diff_ms ${D2} ${D1})

	# expected duration is 400 ms (3 x 100 + 2 x 50)
	test_assert_range ${DMS} 400 500 "execution duration"

	test_sysfs_read ${SYSFS} ENABLE PERIOD DUTY_CYCLE

	local EXP_ENABLE="10010010"
	local EXP_PERIOD="100000010000001000000"
	local EXP_DUTY_CYCLE="050000005000000500000"

	test_assert_eq "${ENABLE}" "${EXP_ENABLE}" "enable data check"
	test_assert_eq "${PERIOD}" "${EXP_PERIOD}" "period data check"
	test_assert_eq "${DUTY_CYCLE}" "${EXP_DUTY_CYCLE}" "duty_cycle data check"

	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
