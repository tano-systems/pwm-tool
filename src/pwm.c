/*
 * SPDX-License-Identifier: WTFPL
 * SPDX-FileCopyrightText: 2021 Anton Kikin <a.kikin@tano-systems.com>
 *
 * PWM tool
 * Copyright © 2021 Anton Kikin <a.kikin@tano-systems.com>
 *
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 */

/**
 * @file
 * @brief PWM tool source file
 *
 * @author Anton Kikin <a.kikin@tano-systems.com>
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>        /* isspace(), isdigit() */
#include <errno.h>        /* EINTR */
#include <time.h>         /* clock_nanosleep() */
#include <fcntl.h>        /* openat() */
#include <math.h>         /* roundf() */
#include <linux/limits.h> /* NAME_MAX */

#include "pwm.h"

/* ----------------------------------------------------------------------- */

#ifndef SYSFS_PWM_ROOT

/**
 * Root in sysfs for PWM control
 *
 * Full sysfs path for specified PWM channel is:
 * <code>
 * sprintf(path, SYSFS_PWM_ROOT"/"
 *               SYSFS_PWM_CHIP_FOLDER_FMT"/"
 *               SYSFS_PWM_CH_FOLDER_FMT,
 *               chip, ch);
 * </code>
 */
#define SYSFS_PWM_ROOT  "/sys/class/pwm"
#endif

#ifndef SYSFS_PWM_CHIP_FOLDER_FMT

/**
 * Format for PWM chip number subfolder in sysfs
 *
 * @see SYSFS_PWM_ROOT
 */
#define SYSFS_PWM_CHIP_FOLDER_FMT  "pwmchip%u"
#endif

#ifndef SYSFS_PWM_CH_FOLDER_FMT

/**
 * Format for PWM channel number subfolder in sysfs
 *
 * @see SYSFS_PWM_ROOT
 */
#define SYSFS_PWM_CH_FOLDER_FMT  "pwm%u"
#endif

#ifndef SYSFS_PWM_FILE_ENABLE

/** File name in sysfs for control enabled state of the PWM */
#define SYSFS_PWM_FILE_ENABLE  "enable"
#endif

#ifndef SYSFS_PWM_FILE_PERIOD

/** File name in sysfs for control period of the PWM */
#define SYSFS_PWM_FILE_PERIOD  "period"
#endif

#ifndef SYSFS_PWM_FILE_DUTY_CYCLE

/** File name in sysfs for control duty-cycle of the PWM */
#define SYSFS_PWM_FILE_DUTY_CYCLE  "duty_cycle"
#endif

/* ----------------------------------------------------------------------- */

pwm_status_t pwm_open(pwm_t *pwm, unsigned int chip, unsigned int channel)
{
	int pwm_root_fd;
	int pwm_chip_fd;
	int pwm_channel_fd;

	char filename[NAME_MAX];

	memset(pwm, 0, sizeof(pwm_t));

	pwm->chip = chip;
	pwm->channel = channel;

	/* Open sysfs root */
	pwm_root_fd = open(SYSFS_PWM_ROOT,
		O_PATH | O_DIRECTORY);

	if (pwm_root_fd < 0)
		return PWM_E_NO_SYSFS;

	/* Open PWM chip folder */
	snprintf(filename, sizeof(filename),
		SYSFS_PWM_CHIP_FOLDER_FMT, chip);
	pwm_chip_fd = openat(pwm_root_fd, filename,
		O_PATH | O_DIRECTORY);

	if (pwm_chip_fd < 0) {
		close(pwm_root_fd);
		return PWM_E_NO_CHIP;
	}

	close(pwm_root_fd);

	/* Open PWM channel folder */
	snprintf(filename, sizeof(filename),
		SYSFS_PWM_CH_FOLDER_FMT, channel);
	pwm_channel_fd = openat(pwm_chip_fd, filename,
		O_PATH | O_DIRECTORY);

	if (pwm_channel_fd < 0) {
		close(pwm_chip_fd);
		return PWM_E_NO_CHANNEL;
	}

	close(pwm_chip_fd);

	/* Open control files */
	pwm->fd_enable = openat(pwm_channel_fd,
		SYSFS_PWM_FILE_ENABLE, O_RDWR);

	if (pwm->fd_enable < 0)
		goto failed;

	pwm->fd_dutycycle = openat(pwm_channel_fd,
		SYSFS_PWM_FILE_DUTY_CYCLE, O_RDWR);

	if (pwm->fd_dutycycle < 0)
		goto failed;

	pwm->fd_period = openat(pwm_channel_fd,
		SYSFS_PWM_FILE_PERIOD, O_RDWR);

	if (pwm->fd_period < 0)
		goto failed;

	close(pwm_channel_fd);
	return PWM_E_OK;

failed:
	close(pwm_channel_fd);

	if (pwm->fd_enable > 0)
		close(pwm->fd_enable);

	if (pwm->fd_dutycycle > 0)
		close(pwm->fd_dutycycle);

	if (pwm->fd_period > 0)
		close(pwm->fd_period);

	return PWM_E_IO;
}

static pwm_status_t pwm_enable_ext(
	pwm_t *pwm,
	unsigned int period,
	unsigned int duty)
{
	ssize_t len;
	char buf[32];

	/*
	 * Temporarily set a minimum duty-cycle to be able
	 * to set any period, even one that is larger than
	 * the current set duty-cycle.
	 */
	if (write(pwm->fd_dutycycle, "0", 1) != 1)
		return PWM_E_IO;

	len = snprintf(buf, sizeof(buf), "%u", period);
	if (write(pwm->fd_period, buf, len) != len)
		return PWM_E_IO;

	/* Set specified duty-cycle */
	len = snprintf(buf, sizeof(buf), "%u", duty);
	if (write(pwm->fd_dutycycle, buf, len) != len)
		return PWM_E_IO;

	if (write(pwm->fd_enable, "1", 1) != 1)
		return PWM_E_IO;

	return PWM_E_OK;
}

pwm_status_t pwm_enable(pwm_t *pwm, unsigned int freq)
{
	float fresult;
	unsigned int period;
	unsigned int duty;

	if ((freq < 1) || (freq > 500000000))
		return PWM_E_INVALID_FREQ;

	fresult = 1000000000 / (float)freq;
	period  = (unsigned int)(roundf(fresult));
	duty    = (unsigned int)(roundf(period / 2.0f));

	return pwm_enable_ext(pwm, period, duty);
}

pwm_status_t pwm_disable(pwm_t *pwm)
{
	if (write(pwm->fd_enable, "0", 1) != 1)
		return PWM_E_IO;

	return PWM_E_OK;
}

pwm_status_t pwm_close(pwm_t *pwm)
{
	if (pwm->fd_enable > 0)
		close(pwm->fd_enable);

	if (pwm->fd_dutycycle > 0)
		close(pwm->fd_dutycycle);

	if (pwm->fd_period > 0)
		close(pwm->fd_period);

	return PWM_E_OK;
}

static pwm_status_t pwm_delay_abs_time(
	pwm_t *pwm,
	const struct timespec *ts,
	struct timespec *remain
)
{
	int ret;

	ret = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, ts, remain);
	if (ret == 0)
		return PWM_E_OK;

	if (ret == EINTR)
		return PWM_E_INTR;
	else
		return PWM_E_FAILED;
}

const char *pwm_strstatus(const pwm_status_t status)
{
	switch (status) {
		case PWM_E_OK:
			return "Ok";

		case PWM_E_NO_SYSFS:
			return "PWM sysfs interface is not available";

		case PWM_E_NO_CHIP:
			return "PWM chip is not available";

		case PWM_E_NO_CHANNEL:
			return "PWM channel is not available";

		case PWM_E_INVALID_FREQ:
			return "Invalid frequency";

		case PWM_E_INVALID_DURATION:
			return "Invalid duration";

		case PWM_E_INTR:
			return "Interrupted";

		case PWM_E_FAILED:
			return "General failure";

		default:
			return "Unknown";
	}
}

/* ----------------------------------------------------------------------- */

/**
 * PWM command data structure
 */
typedef struct {
	/** Frequency */
	unsigned int frequency_hz;

	/** Duration */
	unsigned int duration_ms;

	/** Keep enabled after command executed */
	int keep_enabled;

	/** Timestamp when command must be finished */
	struct timespec ts_end;

} pwm_cmd_t;

/**
 * PWM commands fetcher data structure
 */
typedef struct {
	const char *script;
	const char *pos;

	/** Default frequency (used if not specified in command) */
	unsigned int frequency_hz;

	/** Default duration (used if not specified in command) */
	unsigned int duration_ms;

} pwm_cmd_fetcher_t;

/**
 * Initialize PWM commands fetcher
 */
static void pwm_cmd_fetch_init(
	pwm_cmd_fetcher_t *f,
	const char *script,
	unsigned int frequency_hz,
	unsigned int duration_ms
)
{
	f->script = f->pos = script;

	f->frequency_hz = frequency_hz;
	f->duration_ms  = duration_ms;
}

/**
 * Fetch single PWM command
 *
 * @return -1 Command successufully fetched.
 * @return  0 All commands are fetched, no more commands available.
 * @return <0 Syntax error or invalid command.
 */
static int pwm_cmd_fetch(pwm_cmd_fetcher_t *f, pwm_cmd_t *cmd)
{
	/* Skip whitespaces */
	while (*(f->pos) && isspace(*(f->pos)))
		f->pos++;

	if (!*(f->pos))
		return 0;

	cmd->keep_enabled = 0;
	cmd->frequency_hz = 0;
	cmd->duration_ms  = f->duration_ms;

	/* Parse operations */
	while (f->pos[0] && !isspace(f->pos[0])) {
		char op = f->pos[0];
		switch(op) {
			case 'k':
				cmd->keep_enabled = 1;
				f->pos++;
				break;

			case 'f': /* fallthrough */
			case 'F':
				if (isdigit(f->pos[1])) {
					cmd->frequency_hz =
						(unsigned int)strtoul(f->pos + 1, (char **)&f->pos, 10);

					if (op == 'F')
						f->frequency_hz = cmd->frequency_hz;
				}
				else {
					cmd->frequency_hz = f->frequency_hz;
					f->pos++;
				}
				break;

			case 'd': /* fallthrough */
			case 'D':
				if (isdigit(f->pos[1])) {
					cmd->duration_ms =
						(unsigned int)strtoul(f->pos + 1, (char **)&f->pos, 10);

					if (op == 'D')
						f->duration_ms = cmd->duration_ms;
				}
				else {
					cmd->duration_ms = f->duration_ms;
					f->pos++;
				}
				break;

			default:
				fprintf(stderr,
					"ERROR: Unknown command '%c' in script at position %u\n",
					f->pos[0],
					(unsigned int)(f->pos - f->script) + 1);

				return -1;
		}
	}

	return 1;
}

/**
 * Execute single PWM command
 */
static int pwm_cmd_execute(pwm_t *pwm, pwm_cmd_t *cmd)
{
	pwm_status_t ret = PWM_E_OK;

	if (cmd->frequency_hz) {
		ret = pwm_enable(pwm, cmd->frequency_hz);
		if (ret != PWM_E_OK) {
			fprintf(stderr,
				"ERROR: Can't enable PWM channel %u of chip %u: %s\n",
				pwm->channel, pwm->chip, pwm_strstatus(ret));
			return ret;
		}
	}
	else {
		ret = pwm_disable(pwm);
	}

	/* Sleep for specified duration */
	if (cmd->duration_ms)
		ret = pwm_delay_abs_time(pwm, &cmd->ts_end, NULL);

	if (!cmd->keep_enabled && cmd->frequency_hz)
		ret = pwm_disable(pwm);

	return ret;
}

pwm_status_t pwm_execute(
	pwm_t *pwm,
	const pwm_execute_config_t *config)
{
	pwm_status_t ret = PWM_E_OK;
	pwm_cmd_t cmd;
	pwm_cmd_fetcher_t fetcher;
	int fetched;

	pwm_cmd_fetch_init(
		&fetcher,
		config->script,
		config->default_frequency_hz,
		config->default_duration_ms
	);

	clock_gettime(CLOCK_MONOTONIC, &cmd.ts_end);

	while (1) {
		if (config->stop_flag && *(config->stop_flag))
			break;

		fetched = pwm_cmd_fetch(&fetcher, &cmd);
		if (fetched < 0) {
			pwm_disable(pwm);
			return PWM_E_FAILED;
		}
		
		if (!fetched)
			return PWM_E_OK;

		cmd.ts_end.tv_sec  += cmd.duration_ms / 1000;
		cmd.ts_end.tv_nsec += (cmd.duration_ms % 1000) * 1000000L;

		if (cmd.ts_end.tv_nsec >= 1000000000L) {
			cmd.ts_end.tv_nsec -= 1000000000L;
			cmd.ts_end.tv_sec++;
		}

		ret = pwm_cmd_execute(pwm, &cmd);
		if (ret != PWM_E_OK)
			break;
	}

	return ret;
}
