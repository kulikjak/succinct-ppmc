
all: test

test:
	$(MAKE) default -C tests

clean:
	$(MAKE) clean -C tests

.PHONY: all test clean
