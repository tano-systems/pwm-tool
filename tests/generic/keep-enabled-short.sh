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

	${PWM_TEST_BIN} -k
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"

	test_sysfs_read ${SYSFS} ENABLE PERIOD DUTY_CYCLE

	test_assert_eq "${ENABLE}" "1" "enable data check"
	test_assert_eq "${PERIOD}" "1000000" "period data check"
	test_assert_eq "${DUTY_CYCLE}" "500000" "duty_cycle data check"

	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
