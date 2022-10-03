#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

PROFILE_LC:=$(shell tr '[:upper:]' '[:lower:]' <<< $(PROFILE))
IMGNAME:=$(BIN_DIR)/$(PROFILE_LC)
ROOTFS_CS_DIR:=/rootfs_cs
ROOTFS_CPU400_DIR:=/rootfs_cpu400
ROOTFS_CPU400_CS_DIR:=/rootfs_cpu400_cs
CONFIG_UBNT_FIRMWARE_VERSION:=$(shell cat $(TARGET_DIR)/usr/lib/version | awk -F "." '{ORS=""}{for (i=1;i<NF-3;i++)print $$i"."}{print $$(NF-3)}')

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

define Image/Build/squashfs/cpu400
	rm -rf $(1)
	cp -r $(TARGET_DIR) $(1)
	cat $(1)/usr/lib/version | \
	awk -F "." '{ORS=""}{for (i=1;i<NF-3;i++)print $$$$i"."}{print $$$$(NF-3)"-cpu400."$$$$(NF-2)"."$$$$(NF-1)"."$$$$(NF)}' > \
	$(1)/usr/lib/version_tmp
	mv $(1)/usr/lib/version_tmp $(1)/usr/lib/version
	$(STAGING_DIR_HOST)/bin/mksquashfs4 $(1) $(KDIR)/400-root.squashfs -nopad -noappend -root-owned -comp lzma -processors 1
	md5sum $(KDIR)/400-root.squashfs | cut -d ' ' -f 1 >> $(KDIR)/400-root.squashfs
	$(call prepare_generic_squashfs,$(KDIR)/400-root.squashfs)
	dd if=$(KDIR)/400-root.squashfs of=$(IMGNAME)-400-root.squashfs bs=128k conv=sync
endef

define Image/Build/cpu400
	[ -e ./files/$(PROFILE_LC)-400-u-boot.rom ] && cp ./files/$(PROFILE_LC)-400-fw-layout.txt $(BIN_DIR)/$(PROFILE_LC)-400-fw-layout.txt
	[ -e ./files/$(PROFILE_LC)-400-ubootreflash ] && cp ./files/$(PROFILE_LC)-400-ubootreflash $(BIN_DIR)
	[ -e $(BIN_DIR)/$(PROFILE_LC)-400-fw-layout.txt ] && \
	sed -i -e 's,$$$$(PROFILE_LC),$(PROFILE_LC),g' $(BIN_DIR)/$(PROFILE_LC)-400-fw-layout.txt && \
	sed -i -e 's,$(PROFILE_LC)-u-boot,$(PROFILE_LC)-400-u-boot,g' $(BIN_DIR)/$(PROFILE_LC)-400-fw-layout.txt && \
	sed -i -e 's,$(PROFILE_LC)-ubootreflash,$(PROFILE_LC)-400-ubootreflash,g' $(BIN_DIR)/$(PROFILE_LC)-400-fw-layout.txt && \
	sed -i -e 's,$(PROFILE_LC)-root.squashfs,$(PROFILE_LC)-400-root.squashfs,g' $(BIN_DIR)/$(PROFILE_LC)-400-fw-layout.txt
	[ -e ./files/$(PROFILE_LC)-400-u-boot.rom ] && 	cp ./files/$(PROFILE_LC)-400-u-boot.rom $(BIN_DIR) && \
	cd $(BIN_DIR) && ubnt-mkfwimage -v `cat $(1)/usr/lib/version` -o `sed -e 's/\([^\.]*\)\.[^\.]*\.\(.*\)/\1.\2/' $(1)/usr/lib/version`.bin -i $(BIN_DIR)/$(PROFILE_LC)-400-fw-layout.txt
	rm -rf $(1)
endef

define patch_scripts
	cat $(1)/usr/lib/version | \
	awk -F "." '{ORS=""}{for (i=1;i<NF-3;i++)print $$$$i"."}{print $$$$(NF-3)"-cs."$$$$(NF-2)"."$$$$(NF-1)"."$$$$(NF)}' > $(1)/usr/lib/version_tmp
	mv $(1)/usr/lib/version_tmp $(1)/usr/lib/version
	@touch $(1)/usr/lib/cs
endef

define Image/Build/squashfs/custom_scripts
	rm -rf $(1)
	cp -r $(TARGET_DIR) $(1)
	$(call patch_scripts, $(1))
	$(STAGING_DIR_HOST)/bin/mksquashfs4 $(1) $(KDIR)/cs-root.squashfs -nopad -noappend -root-owned -comp lzma -processors 1
	md5sum $(KDIR)/cs-root.squashfs | cut -d ' ' -f 1 >> $(KDIR)/cs-root.squashfs
	$(call prepare_generic_squashfs,$(KDIR)/cs-root.squashfs)
	dd if=$(KDIR)/cs-root.squashfs of=$(IMGNAME)-cs-root.squashfs bs=128k conv=sync
endef

define Image/Build/custom_scripts
	cp ./files/$(PROFILE_LC)-fw-layout.txt $(BIN_DIR)/$(PROFILE_LC)-cs-fw-layout.txt
	[ -e ./files/$(PROFILE_LC)-ubootreflash ] && cp ./files/$(PROFILE_LC)-ubootreflash $(BIN_DIR)
	[ -e ./files/$(PROFILE_LC)-u-boot.rom ] && cp ./files/$(PROFILE_LC)-u-boot.rom $(BIN_DIR)
	[ -e $(BIN_DIR)/$(PROFILE_LC)-cs-fw-layout.txt ] && \
	sed -i -e 's,$$$$(PROFILE_LC),$(PROFILE_LC),g' $(BIN_DIR)/$(PROFILE_LC)-cs-fw-layout.txt && \
	sed -i -e 's,$(PROFILE_LC)-root.squashfs,$(PROFILE_LC)-cs-root.squashfs,g' $(BIN_DIR)/$(PROFILE_LC)-cs-fw-layout.txt
	cd $(BIN_DIR) && ubnt-mkfwimage -v `cat $(1)/usr/lib/version` -o `sed -e 's/\([^\.]*\)\.[^\.]*\.\(.*\)/\1.\2/' $(1)/usr/lib/version`.bin -i $(BIN_DIR)/$(PROFILE_LC)-cs-fw-layout.txt
	rm -rf $(1)
endef

define Image/Build/squashfs
	md5sum $(KDIR)/root.squashfs | cut -d ' ' -f 1 >> $(KDIR)/root.squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(IMGNAME)-root.$(1) bs=128k conv=sync
ifneq ($(findstring -devel,$(CONFIG_UBNT_FIRMWARE_VERSION)),-devel)
	@echo "Tag detected. Building custom firmware..."
	$(call Image/Build/cpu400, $(BIN_DIR)$(ROOTFS_CPU400_DIR))
	$(call Image/Build/cpu400, $(BIN_DIR)$(ROOTFS_CPU400_CS_DIR))
	$(call Image/Build/custom_scripts, $(BIN_DIR)$(ROOTFS_CS_DIR))
endif
	cp ./files/$(PROFILE_LC)-fw-layout.txt $(BIN_DIR)/$(PROFILE_LC)-fw-layout.txt
	[ -e ./files/$(PROFILE_LC)-ubootreflash ] && cp ./files/$(PROFILE_LC)-ubootreflash $(BIN_DIR)
	sed -i -e 's,$$$$(PROFILE_LC),$(PROFILE_LC),g' $(BIN_DIR)/$(PROFILE_LC)-fw-layout.txt 
	[ -e ./files/$(PROFILE_LC)-u-boot.rom ] && cp ./files/$(PROFILE_LC)-u-boot.rom $(BIN_DIR) || echo "$(PROFILE_LC)-u-boot.rom not found. IGNORING..."
	cd $(BIN_DIR) && ubnt-mkfwimage -v `cat $(TARGET_DIR)/usr/lib/version` -o `sed -e 's/\([^\.]*\)\.[^\.]*\.\(.*\)/\1.\2/' $(TARGET_DIR)/usr/lib/version`.bin -i $(BIN_DIR)/$(PROFILE_LC)-fw-layout.txt \
	&& ln -sf `sed -e 's/\([^\.]*\)\.[^\.]*\.\(.*\)/\1.\2/' $(TARGET_DIR)/usr/lib/version`.bin latest.bin
endef

define Image/mkfs/prepare

	$(call Image/mkfs/prepare/default)
ifneq ($(findstring -devel,$(CONFIG_UBNT_FIRMWARE_VERSION)),-devel)
	@echo "Tag detected. Building custom filesystem..."
	$(call Image/Build/squashfs/cpu400, $(BIN_DIR)$(ROOTFS_CPU400_DIR))
	$(call Image/Build/squashfs/cpu400, $(BIN_DIR)$(ROOTFS_CPU400_CS_DIR))
	$(call patch_scripts, $(BIN_DIR)$(ROOTFS_CPU400_CS_DIR))
	$(call Image/Build/squashfs/custom_scripts, $(BIN_DIR)$(ROOTFS_CS_DIR))
else
	@echo "No tag detected. Enabling custom scripts..."
	$(call patch_scripts, $(TARGET_DIR))
endif
endef

$(eval $(call BuildImage))
