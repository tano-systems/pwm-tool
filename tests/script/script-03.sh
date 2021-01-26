#!/bin/sh
# Test long durations (exceeding 2^32 in nanoseconds) in script

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
	${PWM_TEST_BIN} --script="F1000D100 d50 f d50 f d5000 F2000 d50 f d50 f"
	RET=$?
	D2=$(date "+%s %N")

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"

	local DMS=$(date_diff_ms ${D2} ${D1})

	# expected duration
	local EXP_DURATION=$((100*6 + 50*4 + 5000))

	test_assert_range ${DMS} ${EXP_DURATION} $((${EXP_DURATION} + 100)) "execution duration"

	test_sysfs_read ${SYSFS} ENABLE PERIOD DUTY_CYCLE

	local EXP_ENABLE="10010010010010010"
	local EXP_PERIOD="100000010000001000000500000500000500000"
	local EXP_DUTY_CYCLE="050000005000000500000025000002500000250000"

	test_assert_eq "${ENABLE}" "${EXP_ENABLE}" "enable data check"
	test_assert_eq "${PERIOD}" "${EXP_PERIOD}" "period data check"
	test_assert_eq "${DUTY_CYCLE}" "${EXP_DUTY_CYCLE}" "duty_cycle data check"

	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
