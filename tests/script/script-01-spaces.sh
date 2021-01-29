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

	# Create sysfs root + chip folder + channel folder
	test_sysfs_create ${DEFAULT_PWM_CHIP} ${DEFAULT_PWM_CHANNEL} SYSFS

	${PWM_TEST_BIN} --script="    	F1000D100  	 	 d50	   	  f 	   d50   f  "
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"

	test_sysfs_read ${SYSFS} ENABLE PERIOD DUTY_CYCLE

	local EXP_ENABLE="10010010"
	local EXP_PERIOD="100000010000001000000"
	local EXP_DUTY_CYCLE="50000005000000500000"

	test_assert_eq "${ENABLE}" "${EXP_ENABLE}" "enable data check"
	test_assert_eq "${PERIOD}" "${EXP_PERIOD}" "period data check"
	test_assert_eq "${DUTY_CYCLE}" "${EXP_DUTY_CYCLE}" "duty_cycle data check"

	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
