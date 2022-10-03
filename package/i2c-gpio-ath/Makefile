include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=i2c-gpio-ath

include $(INCLUDE_DIR)/package.mk

define KernelPackage/i2c-gpio-ath
  SUBMENU:=I2C support
  TITLE:=I2C over Atheros Radio GPIO
  DEPENDS:=@GPIO_SUPPORT kmod-i2c-core +kmod-i2c-algo-bit
  FILES:=$(PKG_BUILD_DIR)/i2c-gpio-ath.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,97,i2c-gpio-ath)
endef

define KernelPackage/i2c-gpio-ath/description
 I2C over Atheros Radio GPIO
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		KERNELDIR=$(LINUX_DIR) \
		CC="$(TARGET_CC)" \
		modules
endef

$(eval $(call KernelPackage,i2c-gpio-ath))
