EXCEPT := 00-common/. FreeRTOS-Kernel/.

SUBDIRS := $(patsubst %/.,%,$(filter-out $(EXCEPT), $(wildcard */.)))
SUBDIRS_CLEAN := $(addsuffix .clean, $(SUBDIRS))

.PHONY: all $(SUBDIRS) $(SUBDIRS_CLEAN)

all: $(SUBDIRS)
clean: $(SUBDIRS_CLEAN)

$(SUBDIRS):
	$(MAKE) -C $@

$(SUBDIRS_CLEAN):
	$(MAKE) -C $(basename $@) clean