# hifi dsp device driver

ifeq ($(CONFIG_SEATTLE),y)
EXTRA_CFLAGS += -DPLATFORM_HI3XXX
endif
ifeq ($(CONFIG_HISILICON_PLATFORM_HI6XXX),y)
EXTRA_CFLAGS += -DPLATFORM_HI6XXX
endif

ifneq ($(TARGET_BUILD_VARIANT),user)
EXTRA_CFLAGS += -DENABLE_HIFI_DEBUG
endif

obj-$(CONFIG_HISILICON_PLATFORM_HIFI_DSP)	+= hifi_lpp.o
obj-$(CONFIG_HISILICON_PLATFORM_HIFI_DSP)	+= hifi_om.o

ifeq ($(CONFIG_SEATTLE),y)
EXTRA_CFLAGS += -Idrivers/hisi/hifi_mailbox/mailbox
EXTRA_CFLAGS += -Idrivers/hisi/hifi_mailbox/ipcm
endif

