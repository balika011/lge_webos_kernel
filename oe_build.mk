#Just for verfiyication building in mtk side
ifeq "$(BUILD_CFG)" "debug"
DEFAULT_CONFIG := mt5882_A5LR_smp_mod_dbg_defconfig
else
DEFAULT_CONFIG := mt5882_A5LR_smp_mod_defconfig
endif 
all :
			make ARCH=arm $(DEFAULT_CONFIG)
			make ARCH=arm uImage -j64 CONFIG_CROSS_COMPILE=/mtkoss/gnuarm/arm-lg115x-linux-gnueabi-4.8-2014.08-1-x86_64/bin/arm-lg115x-linux-gnueabi-
			chmod +x kernel.sh
			chmod +x ./../boot/uboot/tools/mkepk
			./kernel.sh
clean:
			make clean
			rm -rf kernel.pak
			rm -rf uImage.bin_signed

