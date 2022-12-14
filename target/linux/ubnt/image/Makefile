#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

PROFILE_LC:=ubntxn
IMGNAME:=$(BIN_DIR)/$(PROFILE_LC)

define imgname
$(IMGNAME)-$(2)-$(patsubst jffs2-%,jffs2,$(1))
endef

VMLINUX:=$(IMGNAME)-vmlinux
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	VMLINUX:=$(IMGNAME)-vmlinux-initramfs
endif

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(VMLINUX).elf
	cp $(KDIR)/vmlinux $(VMLINUX).bin
#	gzip -9 -c $(KDIR)/vmlinux > $(KDIR)/vmlinux.bin.gz
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.bin.l7
#	dd if=$(KDIR)/vmlinux.bin.l7 of=$(VMLINUX).lzma bs=65536 conv=sync
#	dd if=$(KDIR)/vmlinux.bin.gz of=$(VMLINUX).gz bs=65536 conv=sync
#	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux -lc1 -lp2 -pb2 $(KDIR)/vmlinux.lzma
	mkimage -A mips -O linux -T kernel -a 0x80002000 -C lzma -e \
		0x80002000 \
		-n 'MIPS Ubiquiti Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux.bin.l7 $(IMGNAME)-uImage.lzma
endef

define Image/Build/MyLoader
	-$(STAGING_DIR_HOST)/bin/mkmylofw -B $(2) \
		-p0x30000:0xd0000:ahl:0x80060000 \
		-p0x100000:0 \
		-b0x30000:0xd0000:h:$(KDIR)/vmlinux.lzma \
		-b0x100000:0::$(KDIR)/root.$(1) \
		$(call imgname,$(1),$(2)).img
endef

define Image/Build/Template/Compex
	$(call Image/Build/MyLoader,$(1),$(2))
endef

define Image/Build/Template/Compex/squashfs
	$(call Image/Build/Template/Compex,squashfs,$(1))
endef

define Image/Build/Profile/WP543
	$(call Image/Build/Template/Compex/$(1),wp543)
endef

define Image/Build/squashfs
	md5sum $(KDIR)/root.squashfs | cut -d ' ' -f 1 >> $(KDIR)/root.squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(IMGNAME)-root.$(1) bs=128k conv=sync

	cp ./files/$(PROFILE_LC)-fw-layout.txt $(BIN_DIR)/$(PROFILE_LC)-fw-layout.txt
	[ -e ./files/$(PROFILE_LC)-ubootreflash ] && cp ./files/$(PROFILE_LC)-ubootreflash $(BIN_DIR) || echo "ubootreflash not found. IGNORING..."
	sed -i -e 's,$$$$(PROFILE_LC),$(PROFILE_LC),g' $(BIN_DIR)/$(PROFILE_LC)-fw-layout.txt 
	[ -e ./files/$(PROFILE_LC)-u-boot.rom ] && cp ./files/$(PROFILE_LC)-u-boot.rom $(BIN_DIR) || echo "$(PROFILE_LC)-u-boot.rom not found. IGNORING..."
	cd $(BIN_DIR) && ubnt-mkfwimage -v `cat $(TARGET_DIR)/usr/lib/version` -o `sed -e 's/\([^\.]*\)\.[^\.]*\.\(.*\)/\1.\2/' $(TARGET_DIR)/usr/lib/version`.bin -i $(BIN_DIR)/$(PROFILE_LC)-fw-layout.txt \
	&& ln -sf `sed -e 's/\([^\.]*\)\.[^\.]*\.\(.*\)/\1.\2/' $(TARGET_DIR)/usr/lib/version`.bin latest.bin
endef


$(eval $(call BuildImage))
