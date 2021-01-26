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

	${PWM_TEST_BIN} --frequency ${FREQ} -d 10
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

	freq_test 440   "2272727" "01136364"
	freq_test 2213  "4518757" "02259384"
	freq_test 8192  "1220707" "06103584"
	freq_test 16000 "6250007" "03125084"
	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
