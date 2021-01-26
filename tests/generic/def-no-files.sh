#!/bin/sh
#
# SPDX-License-Identifier: WTFPL
# SPDX-FileCopyrightText: 2021 Anton Kikin <a.kikin@tano-systems.com>
#

function do_test {
	local RET

	# Create sysfs root + chip folder + channel folder
	mkdir -p ${SYSFS_PWM_ROOT}/$(printf ${SYSFS_PWM_CHIP_FOLDER_FMT} ${DEFAULT_PWM_CHIP})/$(printf ${SYSFS_PWM_CH_FOLDER_FMT} ${DEFAULT_PWM_CHANNEL})

	${PWM_TEST_BIN}
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_IO}" "return code"
	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
