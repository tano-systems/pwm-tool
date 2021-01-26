#!/bin/sh
#
# SPDX-License-Identifier: WTFPL
# SPDX-FileCopyrightText: 2021 Anton Kikin <a.kikin@tano-systems.com>
#

# Must be synced with defines in pwm.c
SYSFS_PWM_ROOT="./pwmroot"
SYSFS_PWM_CHIP_FOLDER_FMT="pwmchip%u"
SYSFS_PWM_CH_FOLDER_FMT="pwm%u"
SYSFS_PWM_FILE_ENABLE="enable"
SYSFS_PWM_FILE_PERIOD="period"
SYSFS_PWM_FILE_DUTY_CYCLE="duty_cycle"

# Must be synced with defines in main.c
DEFAULT_PWM_CHIP="0"
DEFAULT_PWM_CHANNEL="0"
DEFAULT_PWM_FREQUENCY_HZ="1000"
DEFAULT_PWM_DURATION_MS="250"

# Must be synced with defines in pwm.h
PWM_E_OK="0"
PWM_E_IO="1"
PWM_E_NO_SYSFS="2"
PWM_E_NO_CHIP="3"
PWM_E_NO_CHANNEL="4"
PWM_E_INVALID_FREQ="5"
PWM_E_INVALID_DURATION="6"
PWM_E_INVALID_COMMAND="7"
PWM_E_INTR="8"
PWM_E_FAILED="9"

function test_passed() {
	exit 0
}

function test_failed() {
	[ -n "$1" ] && echo "TEST FAILED: $1"
	exit 1
}

function test_cleanup() {
	rm -rf "${SYSFS_PWM_ROOT}"
}

function test_init() {
	test_cleanup
}

#
# $1 - chip number
# $2 - channel number
# $3 - sysfs control directory (output)
#
function test_sysfs_create() {
	local _CHIP=$1
	local _CHANNEL=$2

	# Create sysfs root + chip folder + channel folder
	local _DIR="${SYSFS_PWM_ROOT}/$(printf ${SYSFS_PWM_CHIP_FOLDER_FMT} ${_CHIP})/$(printf ${SYSFS_PWM_CH_FOLDER_FMT} ${_CHANNEL})"

	rm -rf ${_DIR}
	mkdir -p ${_DIR}

	touch ${_DIR}/${SYSFS_PWM_FILE_ENABLE}
	touch ${_DIR}/${SYSFS_PWM_FILE_PERIOD}
	touch ${_DIR}/${SYSFS_PWM_FILE_DUTY_CYCLE}

	export -- $3="${_DIR}"
}

#
# $1 - sysfs control dir
# $2 - var to store enable contents
# $3 - var to store period contents
# $4 - var to store duty_cycle contents
#
function test_sysfs_read() {
	local _SYSFS="$1"
	local _ENABLE="$(cat ${_SYSFS}/${SYSFS_PWM_FILE_ENABLE})"
	local _PERIOD="$(cat ${_SYSFS}/${SYSFS_PWM_FILE_PERIOD})"
	local _DUTY_CYCLE="$(cat ${_SYSFS}/${SYSFS_PWM_FILE_DUTY_CYCLE})"

	echo "${_SYSFS}/enable = '${_ENABLE}'"
	echo "${_SYSFS}/period = '${_PERIOD}'"
	echo "${_SYSFS}/duty_cycle = '${_DUTY_CYCLE}'"

	export -- $2="${_ENABLE}"
	export -- $3="${_PERIOD}"
	export -- $4="${_DUTY_CYCLE}"
}

function test_assert_eq() {
	[ "$1" != "$2" ] && {
		if [ "$3" = "" ]; then
			echo "ASSERTION: '$1' != '$2'"
		else
			echo "ASSERTION: '$1' != '$2' ($3)"
		fi
		test_failed
	}
}

#
# $1 - test value
# $2 - min value
# $3 - max value
# $4 - optional message
#
function test_assert_range() {
	local VAL=$1
	local VAL_MIN=$2
	local VAL_MAX=$3

	if [ $VAL -lt $VAL_MIN -o $VAL -gt $VAL_MAX ]; then
		if [ "$4" = "" ]; then
			echo "ASSERTION: ${VAL_MIN} <= $1 <= ${VAL_MAX}"
		else
			echo "ASSERTION: ${VAL_MIN} <= $1 <= ${VAL_MAX} ($4)"
		fi
		test_failed
	fi
}

#
# $1 - test value
# $2 - reference value
# $3 - range min percent
# $4 - range max percent
# $5 - optional message
#
function test_assert_range_percent() {
	local VAL=$1
	local VAL_REF=$2
	local VAL_MIN_PERCENT=$3
	local VAL_MAX_PERCENT=$4

	local VMINP=$((${VAL_MIN_PERCENT} * ${VAL_REF} / 100))
	local VMAXP=$((${VAL_MAX_PERCENT} * ${VAL_REF} / 100))

	local VAL_MIN=$((${VAL_REF} - ${VMINP}))
	local VAL_MAX=$((${VAL_REF} + ${VMAXP}))

	test_assert_range ${VAL} ${VAL_MIN} ${VAL_MAX} "$5"
}

#
# $1 - D2 seconds
# $2 - D2 nanoseconds
# $3 - D1 seconds
# $4 - D1 nanoseconds
#
function date_diff_ms {
	local D1S=$(expr $1 + 0)
	local D2NS=$(expr $2 + 0)
	local D2S=$(expr $3 + 0)
	local D1NS=$(expr $4 + 0)

	D2S=$((${D1S}-${D2S}))
	D1S=0

	local D2MS=$((1000*${D2S}+${D2NS}/1000000))
	local D1MS=$((1000*${D1S}+${D1NS}/1000000))

	local DMS=$((${D2MS}-${D1MS}))

	echo ${DMS}
}

test_init
do_test
test_failed
