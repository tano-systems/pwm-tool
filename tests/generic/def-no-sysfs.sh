#!/bin/sh

function do_test {
	local RET

	${PWM_TEST_BIN}
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_NO_SYSFS}" "return code"
	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
