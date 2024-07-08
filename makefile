
TARGET= watchman
CFLAGS= -Wall -g -pedantic -std=gnu99
PKGFLAG= `pkg-config --cflags --libs libnotify`


all: watchman

# The order compiling arguments is important. Source file which
# refer to libnotify should come before the 'pkg-config *** ' part.
# "undefined reference error" occurs when it takes other orders.

watchman:
	gcc ${CFLAGS} ${TARGET}.c -o build/${TARGET} ${PKGFLAG}


PHONY: clean

clean:
	rm -rf  build/${TARGET}*
