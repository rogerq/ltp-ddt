#! /bin/sh
# 
# Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
#  
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as 
# published by the Free Software Foundation version 2.
# 
# This program is distributed "as is" WITHOUT ANY WARRANTY of any
# kind, whether express or implied; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# @desc Get module config option and module name for a driver.
# @params 
#        Input: device type like "nand", "mmc", "spi", "sound", "graphics" 
# @returns Config Option and Top-level .ko file (Module) name to modprobe for
#          this driver separated by colon ':'. If one device requires multiple 
#          top-level .ko files then separate the CONFIG_OPTION:KO_FILE_NAME pairs
#          by space. 
#          Ex: "CONFIG_JFFS2_FS:jffs2 CONFIG_MTD_NAND_OMAP2:omap2_nand"
# @history 2011-03-22: First version

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################

################################ CLI Params ####################################
if [ $# -ne 1 ]; then
  echo "Error: Invalid Argument Count"
  echo "Syntax: $0 <device_type like 'nand', 'mmc', 'spi', 'rtc', 'graphics'>"
  exit 1
fi
DEVICE_TYPE=$1
############################ USER-DEFINED Params ###############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically. ARCH, DRIVER, SOC and MACHINE are 
# initilized and exported by runltp script based on platform option (-P)
case $ARCH in
esac
IFS_ORIG=$IFS
IFS=","
for DRIVER in $DRIVERS
do
  case $DRIVER in
    *omap2-nand)
      nand="CONFIG_JFFS2_FS:jffs2 CONFIG_MTD_NAND_OMAP2:omap2";; 
    *davinci-nand)
      nand=":davinci_nand";;

    *ahci)
      sata="CONFIG_SATA_AHCI_PLATFORM:ahci_platform";;

    *mmci-omap-hs)
      mmc="CONFIG_MMC_OMAP_HS:omap_hsmmc";;
    *davinci-mmc)
      mmc="CONFIG_MMC_DAVINCI:davinci_mmc";;

    *rtc-s35390a)
      rtc="CONFIG_RTC_DRV_S35390A:rtc-s35390a";; #not sure this is the right one.
    *omap_rtc)
      rtc="CONFIG_RTC_DRV_OMAP:rtc-omap";;	
    *rtc_twl)
      rtc="CONFIG_RTC_DRV_TWL4030:rtc_twl";;

    *davinci_spi)
      spi="CONFIG_SPI_DAVINCI:davinci_spi";;
    *omap2_mcspi)
      spi="CONFIG_SPI_OMAP24XX:omap2_mcspi";;
    *spi-omap2-mcspi)
      spi="CONFIG_SPI_OMAP24XX:spi-omap2-mcspi";; #Not sure if Ne/Ce is the same as SA.

    *watchdog)
      wdt="CONFIG_DAVINCI_WATCHDOG:davinci_wdt";;
    *omap_wdt)
      wdt="CONFIG_OMAP_WATCHDOG:omap_wdt";;

    *omapfb)
      graphics="CONFIG_FB_OMAP2:omapfb";;
    *DA8xx_FB_Drv)
      graphics="CONFIG_FB_DA8XX:da8xx-fb";;


    *soc-audio)
      sound=':snd-soc-evm';; #covered in below switch case for MACHINE

    *musb_ti81xx)
      usb="CONFIG_USB_MUSB_TI81XX_GLUE:ti81xx";;
    *musb_davinci)
      usb="CONFIG_USB_MUSB_DAVINCI_GLUE:davinci";;
    *musb_da8xx)
      usb="CONFIG_USB_MUSB_da8xx_GLUE:da8xx";;
    *musb_omap2430)
      usb="CONFIG_USB_MUSB_OMAP2PLUS_GLUE:omap2430";;



  esac
done
IFS=$IFS_ORIG

# In case if the module name is not depending on driver, you may overwrite the 
# CONFIG_OPTION:KO_FILE_NAME pairs based on SOC or MACHINE name below.
case $SOC in
esac
case $MACHINE in
	am387x-evm|am389x-evm)
                sound='CONFIG_SND_SOC:snd-soc-evm CONFIG_SND_DAVINCI_SOC_MCASP:snd-soc-davinci-mcasp CONFIG_SND_TI81XX_SOC:snd-soc-davinci ';;
	omap3evm)
		sound='CONFIG_SND_OMAP_SOC_OMAP3EVM:snd_soc_omap3evm';;
	am3517-evm)
		sound='CONFIG_SND_OMAP_SOC_AM3517EVM:snd-soc-am3517evm';;
	am335x-evm)
	        sound='CONFIG_SND_SOC_TLV320AIC3X:snd-soc-tlv320aic3x CONFIG_SND_AM335X_SOC_EVM:snd-soc-evm CONFIG_SND_DAVINCI_SOC_MCASP:snd-soc-davinci-mcasp CONFIG_SND_AM33XX_SOC:snd-soc-davinci';;	
	am180x-evm|am181x-evm|da850-omapl138-evm)
		sound='CONFIG_SND_DA850_SOC_EVM:snd-soc-evm';;
	dm6467-evm)
		sound='CONFIG_SND_DM6467_SOC_EVM:snd-soc-evm';;
esac

# Define default values for variables being overriden
#: ${nand:="davinci_nand"}

########################### DYNAMICALLY-DEFINED Params #########################
# Try to use /sys and /proc information to determine values dynamically.
# Alternatively you should check if there is an existing script to get the
# value you want

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.

# Avoid using echo. Instead use print functions provided by st_log.sh

# Use do_cmd() (imported from common.sh) to execute your test steps.
# do_cmd() will check return code and fail the test is return code is non-zero.
#echo ${!DEVICE_TYPE}
eval MODULE_NAME=\$$DEVICE_TYPE
if [ -z "$MODULE_NAME" ]; then
	die 'Config Option and Module name not found'
fi
echo "$MODULE_NAME"
