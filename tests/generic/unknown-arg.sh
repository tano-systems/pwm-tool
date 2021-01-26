#!/bin/sh

function do_test {
	${PWM_TEST_BIN} -x
	RET=$?

	# EINVAL = 22
	test_assert_eq "${RET}" "22" "return code"
	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
