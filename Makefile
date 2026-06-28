.PHONY: all firmware software ISP_tests clean

all: firmware software ISP_tests

firmware:
	$(MAKE) -C firmware

software:
	$(MAKE) -C software

ISP_tests:
	$(MAKE) -C ISP_tests

clean:
	$(MAKE) -C firmware clean
	$(MAKE) -C software clean
	$(MAKE) -C ISP_tests clean