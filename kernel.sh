TOOL_DIR=../boot/uboot/tools
MKEPK=$TOOL_DIR/mkepk
SIGN_DIR=$TOOL_DIR/sign_dir
SIGN_TOOL_DIR=$SIGN_DIR/tool
SIGN_TOOL=$SIGN_TOOL_DIR/sign_tool
SIGGEN_KEY1=$SIGN_TOOL_DIR/customer_pub_mtka5lr_key  
SIGGEN_KEY2=$SIGN_TOOL_DIR/customer_priv_mtka5lr_key

$SIGN_TOOL	arch/arm/boot/uImage uImage.bin_signed 1 131072 32 $SIGGEN_KEY1 $SIGGEN_KEY2
$MKEPK -c kernel.pak  uImage.bin_signed kern MTK_KER 1.00.03 12/16/18 0x00 RELEASE
