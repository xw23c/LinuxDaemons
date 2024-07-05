# CFLAGS= -Wall -pedantic -std=gnu99
#
# all: watchman
#
# watchman:
# 	gcc $(CFLAGS) `pkg-config --cflags --libs libnotify` watchman.c -o build/watchmand
#
#



all:watchman

# The order compiling arguments is important. Source file which
# refer to libnotify should come before the 'pkg-config *** ' part.
# "undefined reference error" occurs when it takes other orders.
watchman:
	gcc -Wall -g watchman.c -o build/watchmand `pkg-config --cflags --libs libnotify`
