#!/bin/sh
#
# SPDX-License-Identifier: WTFPL
# SPDX-FileCopyrightText: 2021 Anton Kikin <a.kikin@tano-systems.com>
#

function do_test {
	local CHANNEL=55
	local RET
	local SYSFS

	# Create sysfs root + chip folder + channel folder
	test_sysfs_create ${DEFAULT_PWM_CHIP} ${CHANNEL} SYSFS

	${PWM_TEST_BIN} -c ${CHANNEL} -d 10
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"
	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
