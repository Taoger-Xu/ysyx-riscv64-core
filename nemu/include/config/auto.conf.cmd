deps_config := \
	src/device/Kconfig \
	src/memory/Kconfig \
	/home/xujintao/projects/ysyx-riscv64-core/nemu/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
