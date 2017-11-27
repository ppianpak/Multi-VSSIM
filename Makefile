SUBDIRS = VSSIM1 VSSIM2

all:
	$(MAKE) -C MONITOR/SSD_MONITOR_PM
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir/QEMU; \
	done

clean:
	$(MAKE) clean -C MONITOR/SSD_MONITOR_PM
	for dir in $(SUBDIRS); do \
		$(MAKE) clean -C $$dir/QEMU; \
	done

.PHONY: $(SUBDIRS) clean