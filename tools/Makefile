# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the host tools
#
curdir:=tools

# subdirectories to descend into
$(curdir)/builddirs := sed sstrip ipkg-utils genext2fs lzma squashfs lzma-new squashfs4 mtd-utils mkimage firmware-utils patch-cmdline pkg-config automake $(if $(CONFIG_CCACHE),ccache) bison $(if $(CONFIG_powerpc),dtc) lua quilt autoconf ubnt-mkfwimage

# builddir dependencies
$(curdir)/automake/compile := $(curdir)/autoconf/install
$(curdir)/squashfs/compile := $(curdir)/lzma/install
$(curdir)/squashfs4/compile := $(curdir)/lzma-new/install $(curdir)/squashfs/install
$(curdir)/quilt/compile := $(curdir)/sed/install

# preparatory work
$(STAGING_DIR)/.prepared: $(TMP_DIR)/.build
	@for dir in $(STAGING_DIR) $(STAGING_DIR_HOST); do ( \
		set -x; \
		mkdir -p "$$dir"; \
		cd "$$dir"; \
		mkdir -p bin lib include stamp; \
	); done
	mkdir -p $(BUILD_DIR_HOST)/stamp $(BUILD_DIR)/stamp
	$(INSTALL_DATA) $(TOPDIR)/tools/include/*.h $(STAGING_DIR_HOST)/include/
	touch $@

define PrepareCommand
$(STAGING_DIR_HOST)/bin/$(1): $(STAGING_DIR)/.prepared
	@mkdir -p "$$(dir $$@)"; rm -f "$$@"
	@export FILE="$$$$(which $(2) 2>/dev/null | grep -v 'not found' | head -n1)"; [ -n "$$$$FILE" ] || { \
		echo "Command $(1) not found."; false; \
	}; ln -s "$$$$FILE" "$$@"

endef

$(eval $(call PrepareCommand,find,gfind find))
$(eval $(call PrepareCommand,md5sum,md5sum $(SCRIPT_DIR)/md5sum))
$(eval $(call PrepareCommand,cp,gcp cp))
$(eval $(call PrepareCommand,stat,gstat stat))

$(curdir)/cmddeps = $(patsubst %,$(STAGING_DIR_HOST)/bin/%,find md5sum cp stat)
$(curdir)//prepare = $(STAGING_DIR)/.prepared $($(curdir)/cmddeps)
$(curdir)//compile = $(STAGING_DIR)/.prepared $($(curdir)/cmddeps)

# prerequisites for the individual targets
$(curdir)/ := .config prereq
$(curdir)//install = $(1)/compile

$(eval $(call stampfile,$(curdir),tools,install))
$(eval $(call subdir,$(curdir)))
