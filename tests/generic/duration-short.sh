#!/bin/sh
#
# SPDX-License-Identifier: WTFPL
# SPDX-FileCopyrightText: 2021 Anton Kikin <a.kikin@tano-systems.com>
#

function duration_test() {
	local RET
	local D1
	local D2
	local DURATION=$1
	local DELTA=$2

	D1=$(date "+%s %N")
	${PWM_TEST_BIN} -d ${DURATION}
	RET=$?
	D2=$(date "+%s %N")

	local DMS=$(date_diff_ms ${D2} ${D1})

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"
	test_assert_range \
		${DMS} \
		${DURATION} \
		$((${DURATION} + ${DELTA})) \
		"execution duration"
}

function do_test {
	local SYSFS

	# Create sysfs root + chip folder + channel folder
	test_sysfs_create ${DEFAULT_PWM_CHIP} ${DEFAULT_PWM_CHANNEL} SYSFS

	duration_test 50   250
	duration_test 125  250
	duration_test 333  250
	duration_test 1253 250

	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
