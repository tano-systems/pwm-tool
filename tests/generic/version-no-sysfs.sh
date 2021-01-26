#!/bin/sh

function do_test {
	local RET
	local STDOUT

	STDOUT=$(${PWM_TEST_BIN} --version)
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_OK}" "return code"
	test_assert_eq "${STDOUT}" "${PWM_VERSION}" "stdout contents"
	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
