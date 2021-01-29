#!/bin/sh
#
# SPDX-License-Identifier: WTFPL
# SPDX-FileCopyrightText: 2021 Anton Kikin <a.kikin@tano-systems.com>
#

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
	${PWM_TEST_BIN} --script="f d fk F1000D100 f d fk F1200 D20 fk d fk d f"
	RET=$?
	D2=$(date "+%s %N")

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"

	local DMS=$(date_diff_ms ${D2} ${D1})

	# expected duration
	local EXP_DURATION=$((${DEFAULT_PWM_DURATION_MS} * 3))
	local EXP_DURATION=$((${EXP_DURATION} + 5*100))
	local EXP_DURATION=$((${EXP_DURATION} + 6*20))

	test_assert_range ${DMS} ${EXP_DURATION} $((${EXP_DURATION} + 100)) "execution duration"

	test_sysfs_read ${SYSFS} ENABLE PERIOD DUTY_CYCLE

	local EXP_ENABLE="1001101001100101010"
	local EXP_PERIOD="10000001000000100000010000001000000833333833333833333833333"
	local EXP_DUTY_CYCLE="50000005000000500000050000005000000416667041666704166670416667"

	test_assert_eq "${ENABLE}" "${EXP_ENABLE}" "enable data check"
	test_assert_eq "${PERIOD}" "${EXP_PERIOD}" "period data check"
	test_assert_eq "${DUTY_CYCLE}" "${EXP_DUTY_CYCLE}" "duty_cycle data check"

	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
