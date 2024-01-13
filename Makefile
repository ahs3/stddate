#
# Copyright (c) 2024, Al Stone <ahs3@ahs3.net>
# SPDX-License-Identifier: GPL-2.0-only
#

stddate: stddate.c
	gcc -O -o stddate stddate.c

debug:
	gcc -DDEBUG -g -o stddate stddate.c

clean:
	rm stddate
