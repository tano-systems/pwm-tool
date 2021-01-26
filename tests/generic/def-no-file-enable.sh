#!/bin/sh
#
# SPDX-License-Identifier: WTFPL
# SPDX-FileCopyrightText: 2021 Anton Kikin <a.kikin@tano-systems.com>
#

function do_test {
	local DIR
	local RET

	# Create sysfs root + chip folder + channel folder
	DIR=${SYSFS_PWM_ROOT}/$(printf ${SYSFS_PWM_CHIP_FOLDER_FMT} ${DEFAULT_PWM_CHIP})/$(printf ${SYSFS_PWM_CH_FOLDER_FMT} ${DEFAULT_PWM_CHANNEL})
	mkdir -p ${DIR}

	#touch ${DIR}/${SYSFS_PWM_FILE_ENABLE}
	touch ${DIR}/${SYSFS_PWM_FILE_PERIOD}
	touch ${DIR}/${SYSFS_PWM_FILE_DUTY_CYCLE}

	${PWM_TEST_BIN}
	RET=$?

	test_assert_eq "${RET}" "${PWM_E_IO}" "return code"
	test_passed
}

. ${PWM_TEST_ROOT}/pwm-test-common.sh.inc
