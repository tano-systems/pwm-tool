#!/bin/sh
#
# SPDX-License-Identifier: WTFPL
# SPDX-FileCopyrightText: 2021 Anton Kikin <a.kikin@tano-systems.com>
#

function freq_test {
	local ENABLE
	local PERIOD
	local DUTY_CYCLE
	local RET

	local FREQ="$1"
	local EXP_PERIOD="$2"
	local EXP_DUTY_CYCLE="$3"

	# Create sysfs root + chip folder + channel folder
	test_sysfs_create ${DEFAULT_PWM_CHIP} ${DEFAULT_PWM_CHANNEL} SYSFS

	${PWM_TEST_BIN} -f ${FREQ} -d 10
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"

	test_sysfs_read ${SYSFS} ENABLE PERIOD DUTY_CYCLE
	test_assert_eq "${ENABLE}" "10" "enable data check"
	test_assert_eq "${PERIOD}" "${EXP_PERIOD}" "period data check"
	test_assert_eq "${DUTY_CYCLE}" "${EXP_DUTY_CYCLE}" "duty_cycle data check"
}

function do_test {
	local SYSFS

	# Create sysfs root + chip folder + channel folder
	test_sysfs_create ${DEFAULT_PWM_CHIP} ${DEFAULT_PWM_CHANNEL} SYSFS

	freq_test 440   "2272727" "1136364"
	freq_test 2213  "451875"  "225938"
	freq_test 8192  "122070"  "61035"
	freq_test 16000 "62500"   "31250"
	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
