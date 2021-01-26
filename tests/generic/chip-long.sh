#!/bin/sh
#
# SPDX-License-Identifier: WTFPL
# SPDX-FileCopyrightText: 2021 Anton Kikin <a.kikin@tano-systems.com>
#

function do_test {
	local CHIP=155
	local RET
	local SYSFS

	# Create sysfs root + chip folder + channel folder
	test_sysfs_create ${CHIP} ${DEFAULT_PWM_CHANNEL} SYSFS

	${PWM_TEST_BIN} --chip=${CHIP} -d 10
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"
	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
