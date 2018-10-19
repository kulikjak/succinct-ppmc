CXX = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic

ARCD_ROOT = arcd-master
ARCD_ARCHIVE = $(ARCD_ROOT).zip

all: test

download: .downloaded-arcd
.downloaded-arcd:
	#wget -O $(ARCD_ARCHIVE) https://github.com/wonder-mice/arcd/archive/master.zip
	unzip $(ARCD_ARCHIVE)
	cp $(ARCD_ROOT)/arcd src/arcd
	touch .downloaded-arcd

test:
	$(MAKE) default -C tests

dnagen: dnagen.c
	$(CXX) $(CFLAGS) $^ -o $@

clean:
	$(MAKE) clean -C tests
	rm -f dnagen

purge: clean
	$(MAKE) purge -C tests
	rm -rf $(ARCD_ARCHIVE) $(ARCD_ROOT) .downloaded-arcd


.PHONY: all test clean download
