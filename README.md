# LIFX
Some C code fragments to control [LIFX light bulbs](http://www.lifx.com)

* [https://community.lifx.com](https://community.lifx.com)
* [https://lan.developer.lifx.com/docs](https://lan.developer.lifx.com/docs)
* [https://github.com/LIFX/lifx-protocol-docs](https://github.com/LIFX/lifx-protocol-docs)

### This code was inspired by this source:

  [https://community.lifx.com/t/recvfrom-doesnt-answer-the-right-infos/912](https://community.lifx.com/t/recvfrom-doesnt-answer-the-right-infos/912)

1. Compile with: `clang lifx.c lifx-lib.c -o lifx` or use the supplied `Makefile`

2. The Wireshark directory contains a `lifx.lua` file that can be put in the Wireshark plugin directory in order to dissect LIFX packets.