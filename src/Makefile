ifeq ($(CC),cc)
    CC = clang
endif
ifeq ($(CC),gcc)
    CC = clang
endif
CC     ?= clang

ifeq ($(V),1)
    Q =
else
    Q = @
endif

MAKEFILE_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SDN_SENSOR_BASE ?= $(shell dirname $(MAKEFILE_DIR))
RTE_SDK ?= $(SDN_SENSOR_BASE)/external/dpdk

# XXX: -Wunused-but-set-variable doesn't work
# See: http://llvm.org/bugs/show_bug.cgi?id=9824
# -fcolor-diagnostics
# -O2 -Os
#
WARNINGS = \
-Wall -Wextra -Weverything \
-Wno-language-extension-token -Wno-disabled-macro-expansion \
-Wno-cast-align -Wno-bad-function-cast -Wno-padded -Wno-packed \
-Wno-unused-parameter -Wno-unused-label -Wno-switch-enum -Wno-extra-semi \
-Wno-gcc-compat -Wno-documentation-unknown-command

SS_VERSION := $(shell echo `date "+%Y%m%d-%H%M%S"`-`git rev-parse --short HEAD`)

FLAGS          = -fPIC -O0 -g -fno-strict-aliasing -pthread -m64 -march=native -msse4 -std=gnu11 -ferror-limit=5
DEFINES        = -D__SSE3__ -D__SSSE3__ -D__SSE4_1__ -D__SSE4_2__ -DSS_VERSION="\"$(SS_VERSION)\"" -DSS_IOC_BACKEND_RAM
INCLUDE_FILES  = -include $(RTE_SDK)/build/include/rte_config.h
INCLUDE_PATHS  = -isystem$(RTE_SDK)/build/include -I$(SDN_SENSOR_BASE)/external/spcdns/src -isystem/usr/local/jemalloc/include -I/usr/local/lmdb/include
CPROTO_PATHS   = $(subst -isystem,-I,$(INCLUDE_PATHS))
CFLAGS        := $(WARNINGS) $(FLAGS) $(INCLUDE_FILES) $(INCLUDE_PATHS) $(CFLAGS) $(DEFINES)

# XXX: work around bugs in iwyu utility
IWYU_PATHS = -I/usr/local/include -I/usr/lib/llvm-3.4/lib/clang/3.4/include -I/usr/lib/gcc/x86_64-linux-gnu/4.8/include -I/usr/include/x86_64-linux-gnu -I/usr/include

ifneq ($(strip $(LESS)),)
    CFLAGS += -fcolor-diagnostics
endif

#DPDK_LINK = \
#-Wl,--whole-archive -Wl,--start-group \
#-lethdev \
#-lrte_acl \
#-lrte_cfgfile \
#-lrte_cmdline \
#-lrte_distributor \
#-lrte_eal \
#-lrte_hash \
#-lrte_ip_frag \
#-lrte_kvargs \
#-lrte_lpm \
#-lrte_malloc \
#-lrte_mbuf \
#-lrte_mempool \
#-lrte_meter \
#-lrte_pipeline \
#-lrte_pmd_bond \
#-lrte_pmd_e1000 \
#-lrte_pmd_pcap \
#-lrte_pmd_ring \
#-lrte_pmd_virtio_uio \
#-lrte_pmd_vmxnet3_uio \
#-lrte_port \
#-lrte_power \
#-lrte_ring \
#-lrte_sched \
#-lrte_table \
#-lrte_timer \
#-Wl,--end-group -Wl,--no-whole-archive

DPDK_LINK = -Wl,--whole-archive -Wl,--start-group -lintel_dpdk -Wl,--end-group -Wl,--no-whole-archive
STATIC_LINK = -Wl,-Bstatic -lbsd -lcre2 -lre2 -llzma -ljson-c -llmdb -lnanomsg -lanl -lpcap -lpcre -lspcdns -lspcdnsmisc -Wl,-Bdynamic

LDFLAGS = -L$(RTE_SDK)/build/lib -Wl,-rpath,$(RTE_SDK)/build/lib -L$(SDN_SENSOR_BASE)/external/spcdns/built -L/usr/local/jemalloc/lib -Wl,-rpath,/usr/local/jemalloc/lib -L/usr/local/lmdb/lib -Wl,-rpath,/usr/local/jemalloc/lib

HEADERS = $(wildcard *.h)
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))
DEPENDS = $(patsubst %.c,%.d,$(wildcard *.c))

.PHONY: clean cproto iwyu

sdn_sensor: $(OBJECTS)
	@echo 'Linking sdn_sensor...'
	$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(DPDK_LINK) $(STATIC_LINK) -ljemalloc -lunwind -ldl -lm -lpthread -lrt -lstdc++

#$(OBJECTS): $(DEPENDS)

%.o: %.d

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.o: %.c
	@echo "CC $<"
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

-include $(DEPENDS)

clean:
	@echo 'Cleaning sdn_sensor...'
	@rm -f sdn_sensor *.d *.o *.h.bak

cproto:
	$(PWD)/../scripts/update-header-file.pl $(DEFINES) $(CPROTO_PATHS) -- $(HEADERS)

iwyu: $(HEADERS) $(SOURCES)
	@echo 'Checking includes in sdn_sensor...'
	for file in $^; do iwyu ${CFLAGS} ${IWYU_PATHS} $$file; done;
