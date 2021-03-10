# hue

Terminal tool for NZXT Hue RGB controller

## Quick start

    make all
    ./bin/hue-static init

With debug

    make DEBUG=y all
    ./bin/hue-static init

# Usage

```
Usage: ./bin/hue-static
options
        -h, --help              print this help
        --serial SERIAL         set serial device
        -s, --speed VALUE       speed value between 0..4
        -g, --group VALUE       group value between 0..3
        -r, --reverse           reverse direction
        -m, --moving            enable movement

arguments
    init                        reset leds
    mode                        set led colors
        fixed <color>           set static color
        fading <colors>         set fading colors 2..8
                                options: speed
        spectrum                set spectrum
                                options: speed, reverse
        marquee <color>         set marquee
                                options: speed, group, reverse
        cmarquee <color>        set cover marquee 2..8
                                options: speed, reverse
        alternating <colors>    set alternating between 2 colors
                                options: speed, moving, group, reverse
        pulse <colors>          set pulse 1..8 colors
                                options: speed
        breathing <colors>      set breathing 1..8 colors
                                options: speed
        alert <colors>          set alert 2 colors
                                group changes blinking interval
                                using max speed causes fast flickering
                                options: group, speed
        candle <color>          set candle
                                options: speed
        wings <color>           set wings
                                options: speed
        wave <color>            set wave
        direct <color>          same as fixed

examples
    ./bin/hue-static init
    ./bin/hue-static mode fixed 550055
    ./bin/hue-static mode fixed 505

```
