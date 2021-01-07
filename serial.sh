#!/bin/sh
stty -F /dev/ttyACM0 9600 -brkint -imaxbel icrnl opost icanon onlcr isig iexten echo echoe echok echoctl echoke
