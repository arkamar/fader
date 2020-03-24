# Fader

Fader is combination of `faderd` daemon with `fader` command line utility for backlight control in linux.
It still needs some touch and polishing but I already use it daily.
I will be glad if it works for you as well.

## Invocation

`faderd [-d PATH]`

* `-d PATH` sets the path to the backlight control directory. It is set to `/sys/class/backlight/intel_backlight` by default (my default).

This server runs forever, waits for commands in `/tmp/fader.sock` and changes brightness appropriately.

`fader [+|-]NUMBER[%]`

This is user utility for sending commands to the `faderd` through `/tmp/fader.sock`.
It accepts exactly one parameter with decimal `NUMBER` which may be prefixed with `+` or `-` and suffixed with `%`.
The usage should be intuitive, but here is few examples for clarification:

* `fader 10` - set the brightness to 10
* `fader 10%` - set the brightness to 10%
* `fader +10` - increase brightness by 10
* `fader -10%` - decrease brightness by 10%

## Compilation

Run `make` in source directory in order to compile `fader` and `faderd` programs.
