#!/bin/sh
#
# SPDX-License-Identifier: WTFPL
# SPDX-FileCopyrightText: 2021 Anton Kikin <a.kikin@tano-systems.com>
#

function do_test {
	local RET

	${PWM_TEST_BIN}
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_NO_SYSFS}" "return code"
	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
