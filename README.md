# PWM Tool

This is a simple Linux command line tool designed to control the available PWM channels via the sysfs interface. The main purpose of this utility is to control a buzzer device connected to the corresponding PWM channel.

## Build and Install

Clone tool sources (for example, to `~/pwm-tool` directory):

```shell
$ git clone https://github.com/tano-systems/pwm-tool.git ~/pwm-tool
```

Jump to the directory with sources:

```shell
$ cd ~/pwm-tool
```

Create a `build` subdirectory and jump to it:

```shell
$ mkdir build
$ cd build
```

Run CMake:

```shell
$ cmake ../
```

Run Make:

```shell
$ make
```

This will produce binary `pwm` in the `build` directory. To install this binary to `/usr/bin` run command as `root` user:

```shell
# make install
```

## Tests

To run unit tests use following command:

```shell
$ make build_and_test
```

## Usage

Usage syntax:

```shell
pwm [options]
```

*   `[options]` is a one or more additional optional options that are described in the "[Options](#options)" section.

### Options

| Short Form         | Long Form                  | Default Value | Description                                                  |
| ------------------ | -------------------------- | ------------- | ------------------------------------------------------------ |
| `-h`               | `--help`                   | -             | Display help and usage text.                                 |
| `-p <chip>`        | `--chip=<chip>`            | `0`           | Set PWM chip number to `<chip>`                              |
| `-c <channel>`     | `--channel=<channel>`      | `0`           | Set PWM channel number to `<channel>`                        |
| `-f <freq_hz>`     | `--frequency=<freq_hz>`    | `1000`        | Set PWM frequency in Hz. If the specified frequency is `0`, the PWM will not be enabled. |
| `-d <duration_ms>` | `--duration=<duration_ms>` | `250`         | Set PWM enabled state duration in milliseconds.              |
| `-k`               | `--keep-enabled`           | -             | If specified, PWM will remain enabled on exit.               |
| `-s <script>`      | `--script=<script>`        | -             | Run PWM commands script. See details in "[Scripts Syntax](#scripts-syntax)" section. |
| -                  | `--version`                | -             | Display PWM tool version.                                    |

### Scripts Syntax

The script consists of commands separated by one or more spaces:
```
<command1> <command2> ... <commandN>
```

Each command consists of one or more operations, which are stored one after the other without spaces:
```
<op1><op2>...<opN>
```

The operation is represented by a single character of the alphabet. Operation can have only one optional numeric argument, which must be written immediately after the operation character:
```
O[argument]
```

For example, the operation `F` with the argument 1250 should be written as follows:
```
F1250
```

And a command consisting of two operations, `F` with argument 1250 and `d` with argument 50, as follows:
```
F1250d50
```

Examples of how to write multiple commands can be seen in the "[Examples](#examples)" section.

Available PWM command operations:

| Operation | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `f[hz]`   | Set frequency from optional argument. If no argument is specified, the current default frequency will be used. |
| `F[hz]`   | Same as `f[hz]`, but if an argument is specified, the value will then be used as the default frequency for subsequent commands. |
| `d[ms]`   | Set duration from optional argument. If no argument is specified, the current default duration will be used. |
| `D[ms]`   | Same as `d[ms]`, but if an argument is specified, the value will then be used as the default duration for subsequent commands. |
| `k`       | Keep the PWM enabled when the command is completed.          |

If no `f` or `F` operation is specified in a command, then a frequency of 0 will be used for that command, i.e. such commands can be used to delay script execution.

If no `d` or `D` operation is specified in a command, the current default duration will be used for that command. Changing the default duration can either be done through the configuration structure or directly at runtime with the `D[ms]` operation.


## Examples

Three short beeps with a frequency of 1000 Hz (duration 100 ms with 50 ms delay between):

```shell
$ pwm -f 1000 -d 100
$ pwm -f 0 -d 50
$ pwm -f 1000 -d 100
$ pwm -f 0 -d 50
$ pwm -f 1000 -d 100
```

Same for PWM commands script mode:
```shell
$ pwm -s "F1000D100 d50 f d50 f"
```

## Changelog

See [CHANGELOG.md](CHANGELOG.md).

## License

This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See <http://www.wtfpl.net/> for more details.

## Maintainers

*   Anton Kikin <mailto:a.kikin@tano-systems.com>
