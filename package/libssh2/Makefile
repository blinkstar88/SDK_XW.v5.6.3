#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libssh2
PKG_VERSION:=1.8.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.libssh2.org/download
PKG_MD5SUM:=3d1147cae66e2959ea5441b183de1b1c

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libssh2
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=SSH2 library
  URL:=http://www.libssh2.org/
  DEPENDS:=+libopenssl +zlib
endef

define Package/libssh2/description
	libssh2 is a client-side C library implementing the SSH2 protocol
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--disable-examples-build \
	--with-openssl=$(STAGING_DIR)/usr \
	--with-libssl-prefix=$(STAGING_DIR)/usr

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libssh2.so* $(1)/usr/lib/
endef

define Package/libssh2/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libssh2.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libssh2))
