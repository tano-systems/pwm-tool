/*
 * PWM tool
 * Copyright © 2021 Anton Kikin <a.kikin@tano-systems.com>
 *
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 */

/**
 * @file
 * @brief PWM tool header file
 *
 * @author Anton Kikin <a.kikin@tano-systems.com>
 */

#ifndef PWM_H_INCLUDED
#define PWM_H_INCLUDED

/* ----------------------------------------------------------------------- */

/**
 * PWM status codes
 */
typedef enum {
	PWM_E_OK = 0,
	PWM_E_IO,
	PWM_E_NO_SYSFS,
	PWM_E_NO_CHIP,
	PWM_E_NO_CHANNEL,
	PWM_E_INVALID_FREQ,
	PWM_E_INVALID_DURATION,
	PWM_E_INVALID_COMMAND,
	PWM_E_INTR,
	PWM_E_FAILED,
} pwm_status_t;

/**
 * PWM handle structure
 */
typedef struct {
	/** File handle to control enabled state */
	int fd_enable;

	/** File handle to control duty cycle */
	int fd_dutycycle;

	/** File handle to control period */
	int fd_period;

	/** PWM chip number */
	unsigned int chip;

	/** PWM channel number */
	unsigned int channel;

} pwm_t;

/**
 * Try to open PWM channel.
 *
 * @param[out] pwm     Pointer to the PWM handle structure
 * @param[in]  chip    PWM chip number
 * @param[in]  channel PWM channel number
 *
 * @return PWM_E_OK Success
 * @return PWM_E_NO_SYSFS No access to sysfs
 * @return PWM_E_NO_CHIP Specified chip number is not exits
 * @return PWM_E_NO_CHANNEL Specified channel number is not exits
 * @return PWM_E_IO Can't open channel
 */
pwm_status_t pwm_open(pwm_t *pwm, unsigned int chip, unsigned int channel);

/**
 * Enable PWM with specified frequency
 *
 * @param[in] pwm  Pointer to the PWM handle structure
 * @param[in] freq Frequency in Hz
 *
 * @return PWM_E_OK Success
 * @return PWM_E_INVALID_FREQ Invalid frequency
 * @return PWM_E_IO Can't enable channel
 */
pwm_status_t pwm_enable(pwm_t *pwm, unsigned int freq);

/**
 * Delay for specified duration
 *
 * @param[in]  pwm      Pointer to the PWM handle structure
 * @param[in]  duration Duration in milliseconds
 * @param[out] remain   Remaining duration in case of interrupted
 *                      delay (PWM_E_INTR). Can be NULL.
 *
 * @return PWM_E_OK Success
 * @return PWM_E_INVALID_DURATION Invalid duration
 * @return PWM_E_INTR Delay is interrupted by signal
 * @return PWM_E_FAILED Delay failed
 */
pwm_status_t pwm_delay(pwm_t *pwm, unsigned int duration,
	unsigned int *remain);

/**
 * Disable PWM
 *
 * @param[in] pwm Pointer to the PWM handle structure
 *
 * @return PWM_E_OK Success
 * @return PWM_E_IO Can't disable channel
 */
pwm_status_t pwm_disable(pwm_t *pwm);

/**
 * Close PWM channel
 *
 * @param[in] pwm Pointer to the PWM handle structure
 *
 * @return PWM_E_OK Success
 * @return PWM_E_IO Can't close PWM channel
 */
pwm_status_t pwm_close(pwm_t *pwm);

/**
 * Get description for specified PWM status.
 *
 * @param[in] status PWM status value.
 *
 * @return Pointer to the null-terminated string with
 *         short status description.
 */
const char *pwm_strstatus(const pwm_status_t status);

/**
 * PWM commands script execution configuration
 * structure
 */
typedef struct {
	/**
	 * Script
	 *
	 * The script consists of commands separated by one or more
	 * spaces. Each command consists of one or more operations,
	 * which are stored one after the other without spaces.
	 * The operation is represented by a single character of the
	 * alphabet. Operation can have one numeric argument, which
	 * must be written immediately after the operation character.
	 *
	 * Operations:
	 *
	 * - `f[hz]`:
	 *   Set frequency from optional argument. If no argument
	 *   is specified, the current default frequency will be used.
	 *
	 * - `F[hz]`:
	 *   Same as `f[hz]`, but if an argument is specified, the
	 *   value will then be used as the default frequency for
	 *   subsequent commands.
	 *
	 * - `d[ms]`:
	 *   Set duration from optional argument. If no argument
	 *   is specified, the current default duration will be used.
	 *
	 * - `D[ms]`:
	 *   Same as `d[ms]`, but if an argument is specified, the
	 *   value will then be used as the default duration for
	 *   subsequent commands.
	 *
	 * - `k`:
	 *   Keep the PWM enabled when the command is completed.
	 *
	 * If no `f` or `F` operation is specified in a command, then
	 * a frequency of 0 will be used for that command, i.e. such
	 * commands can be used to delay script execution.
	 *
	 * If no `d` or `D` operation is specified in a command, the
	 * current default duration will be used for that command.
	 * Changing the default duration can either be done through
	 * the configuration structure or directly at runtime with
	 * the `D[ms]` operation.
	 *
	 * Examples:
	 *
	 * Script for three short beeps with a frequency of 1000 Hz
	 * (duration 100 ms with 50 ms delay between):
	 * <code>
	 *     F1000D100 d50 f d50 f
	 * </code>
	 */
	const char *script;

	/** Default frequency in Hz */
	unsigned int default_frequency_hz;

	/** Default duration in milliseconds */
	unsigned int default_duration_ms;

	/** Pointer to the external stop flag */
	volatile int *stop_flag;

} pwm_execute_config_t;

/**
 * Execute commands script for specified PWM.
 *
 * @param[in] pwm    Pointer to the PWM handle structure
 * @param[in] config Pointer to the PWM commands script exectution
 *                   configuration structure
 *
 * @return PWM_E_OK Script successfully executed
 * @return PWM_E_INTR Script execution has been
 *     interrupted by signal
 * @return PWM_E_IO Execution failure (sysfs I/O error).
 * @return PWM_E_FAILED Execution failure (syntax error,
 *     unknown command, invalid config, etc).
 */
pwm_status_t pwm_execute(
	pwm_t *pwm,
	const pwm_execute_config_t *config
);

/* ----------------------------------------------------------------------- */

#endif /* PWM_H_INCLUDED */
