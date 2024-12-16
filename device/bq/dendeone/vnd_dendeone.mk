MTK_TARGET_PROJECT := dendeone
MTK_BASE_PROJECT := dendeone
ifndef MTK_TARGET_PROJECT_FOLDER
MTK_TARGET_PROJECT_FOLDER := $(LOCAL_PATH)
endif
MTK_PROJECT_FOLDER := $(MTK_TARGET_PROJECT_FOLDER)

include $(MTK_TARGET_PROJECT_FOLDER)/ProjectConfig.mk
include $(wildcard $(MTK_TARGET_PROJECT_FOLDER)/RuntimeSwitchConfig.mk)
$(call inherit-product, $(MTK_TARGET_PROJECT_FOLDER)/device.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/handheld_vendor.mk)
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.zygote=zygote32

ifndef SYS_TARGET_PROJECT
PRODUCT_BUILD_SYSTEM_IMAGE := false
PRODUCT_BUILD_PRODUCT_IMAGE := false
endif
PRODUCT_LOCALES := en_US es_ES ca_ES ru_RU fr_FR de_DE tr_TR in_ID it_IT ar_EG hi_IN ur_PK pt_PT nl_NL hu_HU ro_RO cs_CZ ko_KR pl_PL bg_BG lt_LT sk_SK uk_UA da_DK fi_FI nb_NO sv_SE
PRODUCT_MANUFACTURER := bq
PRODUCT_NAME := vnd_dendeone_bsp
PRODUCT_DEVICE := $(strip $(MTK_BASE_PROJECT))
PRODUCT_MODEL := Aquaris_A4.5
PRODUCT_POLICY := android.policy_phone
PRODUCT_BRAND := bq
BUILD_FINGERPRINT := bq/Aquaris_A45/Aquaris_A45_sprout:11/RP1A.200720.011/20241010:user/release-keys

ifeq ($(TARGET_BUILD_VARIANT), eng)
KERNEL_DEFCONFIG ?= dendeone_debug_defconfig
else
KERNEL_DEFCONFIG ?= dendeone_defconfig
endif
PRELOADER_TARGET_PRODUCT ?= dendeone
LK_PROJECT ?= dendeone
TRUSTY_PROJECT ?= dendeone
