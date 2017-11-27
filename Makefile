SUBDIRS = VSSIM1 VSSIM2

all:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir/../MONITOR/SSD_MONITOR_PM; \
		$(MAKE) -C $$dir/QEMU; \
	done

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) clean -C $$dir/../MONITOR/SSD_MONITOR_PM; \
		$(MAKE) clean -C $$dir/QEMU; \
	done

.PHONY: $(SUBDIRS) clean