# hi6402 dsp device driver
EXTRA_CFLAGS += -I../vendor/hisi/audiodsp/custom/hi6402_hifi/include/med/

ifneq ($(TARGET_BUILD_VARIANT),user)
	EXTRA_CFLAGS += -DENABLE_HI6402_HIFI_DEBUG
endif
obj-$(CONFIG_HI6402_HIFI_MISC) += hi6402_hifi_misc.o
obj-$(CONFIG_HI6402_HIFI_MISC) += hi6402es_hifi_misc.o
