#!/bin/bash
source st_log.sh
source omap5_clk_framework_utils.sh

omap5_CLK_TREE=()
add_to_tree() {
  omap5_CLK_TREE[${#omap5_CLK_TREE[*]}]=$1=$2
}

#A file that defines the following variables (commented variables only) and 
#sources this file must be created to obtain a clk tree.

#MASTER clocks sources
FUNC_32K_CLK=32768
SYS_CLK=19200000  #Any of 12, 16.8, 19.2, 26, or 38.4 MHz
FUNC_SYS_32K=$FUNC_32K_CLK
XCLK_60M_HSP1=60000000
XCLK_60M_HSP2=60000000
PAD_UCLKS=12000000 #Set to 12MHz for now, need to research
SLIMBUS_UCLKS=12000000 #Set to 12MHz for now, need to research
FREF_CLK0_AUX=0
FREF_CLK1_AUX=0
FREF_CLK2_AUX=0

#PER DPLL settings
#case $SYS_CLK in
#   38400000) PER_PLL_M=10
#             PER_PLL_N=0;;
#   26000000) PER_PLL_M=192
#             PER_PLL_N=12;;
#   19200000) PER_PLL_M=20
#             PER_PLL_N=0;;
#   16800000) PER_PLL_M=260
#             PER_PLL_N=6;;
#   12000000) PER_PLL_M=32
#             PER_PLL_N=0;;
#esac
#PER_PLL_M2=4
#PER_PLL_M3=3
#PER_PLL_H11=2
#PER_PLL_H12=3
#OPP_NOM Settings
#PER_PLL_H14=1
#OPP_LOW Settings
#PER_PLL_H14=3

#Core DPLL Settings
#case $SYS_CLK in
#   38400000) CORE_PLL_M=665
#             CORE_PLL_N=23;;
#   26000000) CORE_PLL_M=532
#             CORE_PLL_N=12;;
#   19200000) CORE_PLL_M=665
#             CORE_PLL_N=11;;
#   16800000) CORE_PLL_M=570
#             CORE_PLL_N=8;;
#   12000000) CORE_PLL_M=266
#             CORE_PLL_N=2;;
#esac
#CORE_PLL_M3=8
#CORE_PLL_H11=7
#CORE_PLL_H12=3
#CORE_PLL_H13=61
#CORE_PLL_H22=9
#CORE_PLL_H23=13
#CORE_PLL_H24=11
#OPP_NOM Settings
#CORE_PLL_M2=2 #DDR3 used can do DDR1600, but it is set to work as DDR1066
#CORE_PLL_H14=4
#OPP_LOW Settings
#CORE_PLL_M2=4 #DDR3 used can do DDR1600, but it is set to work as DDR1066
#CORE_PLL_H14=9
#OPP_OD Settings
#CORE_PLL_H14=3

#ABE DPLL Settings
#Info missing from the TRM
#ABE_PLL_M=750
#ABE_PLL_N=0
#ABE_PLL_M2=1
#ABE_PLL_REGM4=0
#ABE_PLL_M3=1
#The previous settings are the ones in the TRM but they do not provide the
#196.608MHz clk stated in the TRM so going with the following instead for now
#case $SYS_CLK in
#   38400000) ABE_PLL_M=64
#             ABE_PLL_N=24
#             ABE_PLL_M2=1
#             ABE_PLL_M3=1;;
#   26000000) ABE_PLL_M=64 #Not exactly 196.608, actually 195.764705
#             ABE_PLL_N=16
#             ABE_PLL_M2=1
#             ABE_PLL_M3=1;;
#   19200000) ABE_PLL_M=128
#             ABE_PLL_N=24
#             ABE_PLL_M3=1;;
#   16800000) ABE_PLL_M=1024
#             ABE_PLL_N=34
#             ABE_PLL_M2=5
#             ABE_PLL_M3=5;;
#   12000000) ABE_PLL_M=1024
#             ABE_PLL_N=124
#             ABE_PLL_M2=1
#             ABE_PLL_M3=1;;
#esac
#ABE_PLL_REGM4=0



#MPU DPLL Settings
#OPP_LOW and OPP_NOM Settings
#case $SYS_CLK in
#   38400000) MPU_PLL_M=125
#             MPU_PLL_N=3;;
#   26000000) MPU_PLL_M=600
#             MPU_PLL_N=12;;
#   19200000) MPU_PLL_M=625
#             MPU_PLL_N=9;;
#   16800000) MPU_PLL_M=1500
#             MPU_PLL_N=20;;
#   12000000) MPU_PLL_M=100 
#             MPU_PLL_N=0;;
#esac
#OPP_HIGH Settings
#case $SYS_CLK in
#   38400000) MPU_PLL_M=2125 
#             MPU_PLL_N=95;;
#   26000000) MPU_PLL_M=425 
#             MPU_PLL_N=12;;
#   19200000) MPU_PLL_M=2125 
#             MPU_PLL_N=47;;
#   16800000) MPU_PLL_M=2125
#             MPU_PLL_N=41;;
#   12000000) MPU_PLL_M=425
#             MPU_PLL_N=5;;
#esac
#OPP_SPEEDBIN Settings
#case $SYS_CLK in
#   38400000) MPU_PLL_M=625 
#             MPU_PLL_N=23;;
#   26000000) MPU_PLL_M=500 
#             MPU_PLL_N=12;;
#   19200000) MPU_PLL_M=625
#             MPU_PLL_N=11
#   16800000) MPU_PLL_M=1250
#             MPU_PLL_N=20;;
#   12000000) MPU_PLL_M=250
#             MPU_PLL_N=2;;
#esac
#OPP_LOW M2
#MPU_PLL_M2=2;;
#OPP_NOM, OPP_HIGH, OPP_SPEEDBIN M2
#MPU_PLL_M2=1;;

#IVA DPLL Settings
#OPP_LOW and OPP_NOM Settings
#case $SYS_CLK in
#   38400000) IVA_PLL_M=1456 #Not exactly 2330MHz actually 2329.6
#             IVA_PLL_N=47;;
#   26000000) IVA_PLL_M=1165
#             IVA_PLL_N=25;;
#   19200000) IVA_PLL_M=1456 #Not exactly 2330MHz actually 2329.6
#             IVA_PLL_N=23;;
#   16800000) IVA_PLL_M=971 #Not exactly 2330MHz actually 2330.4
#             IVA_PLL_N=13;;
#   12000000) IVA_PLL_M=1165
#             IVA_PLL_N=11;;
#esac
#OPP_OD Settings
#case $SYS_CLK in
#   38400000) IVA_PLL_M=665
#             IVA_PLL_N=47;;
#   26000000) IVA_PLL_M=266
#             IVA_PLL_N=12;;
#   19200000) IVA_PLL_M=665
#             IVA_PLL_N=23;;
#   16800000) IVA_PLL_M=285
#             IVA_PLL_N=8;;
#   12000000) IVA_PLL_M=133
#             IVA_PLL_N=2;;
#esac
#OPP_LOW H1x Settings
#IVA_PLL_H11=9
#IVA_PLL_H12=11
#OPP_NOM H1x Settings
#IVA_PLL_H11=4
#IVA_PLL_H12=5
#OPP_OD H1x Settings
#IVA_PLL_H11=1
#IVA_PLL_H12=1

#USB DPLL Settings
#case $SYS_CLK in
#   38400000) USB_PLL_M=25
#             USB_PLL_N=1;;
#   26000000) USB_PLL_M=240
#             USB_PLL_N=12;;
#   19200000) USB_PLL_M=25
#             USB_PLL_N=0;;
#   16800000) USB_PLL_M=200
#             USB_PLL_N=6;;
#   12000000) USB_PLL_M=40
#             USB_PLL_N=0;;
#esac
#USB_PLL_M2=1

#PRM Settings
#PRM_ABE_DPLL_CLK_MUX=0
#PRM_WKUPAON_ICLK_MUX=0
#PRM_ABE_DPLL_BYPASS_CLK_MUX=0
#PRM_ABE_SYS_CLK_DIV=0
#PRM_MPU_DPLL_CLK_ABE_DIV=0
#PRM_MPU_DPLL_CLK_EMIF_DIV=0 # 0 or 1 (div 4); 2 or 3 (div 8)
#PRM_L3INSTR_TS_GCLK_AND_DLL_AGING_CLK_DIV=2 # 0 (div 8); 1 (div 16); 2 (div 32)

#CM_CORE_AON Settings
#CM_CORE_AON_L3_ICLK_DIV=1 # 0 or 1 maps to 2^n
#CM_CORE_AON_L4_ROOT_CLK_DIV=1 # 0 or 1 maps to 2^n
#CM_CORE_AON_MPU_DPLL_HS_CLK_DIV=0 # 0, 1, 2, or 3 maps to 2^n
#CM_CORE_AON_IVA_DPLL_HS_CLK_DIV=0 # 0, 1, 2, or 3 maps to 2^n

#CKGEN_USB Settings
#CKGEN_USB_L3_INIT_60M_FCLK_DIV=1
#CKGEN_USB_UTMI_P1_GFCLK_MUX=0
#CKGEN_USB_UTMI_P2_GFCLK_MUX=0

#CKGEN_ABE Settings
#CKGEN_ABE_CLK_DIV=0 #0 (OPP_NOM), 1 (OPP_LOW), or 2 maps to 2^n 
#CKGEN_AESS_FCLK_ABE_GICLK_DIV=0 #0 or 1 maps to 2^n
#CKGEN_ABE_TIMER5_GFCLK_MUX=0
#CKGEN_ABE_TIMER6_GFCLK_MUX=0
#CKGEN_ABE_TIMER7_GFCLK_MUX=0
#CKGEN_ABE_TIMER8_GFCLK_MUX=0
#CKGEN_ABE_DMIC_GFCLK_MUX=0
#CKGEN_ABE_DMIC_INT_MUX=0
#CKGEN_ABE_MCASP_INT_MUX=0
#CKGEN_ABE_MCASP_GFCLK_MUX=0
#CKGEN_ABE_MCBSP1_GFCLK_MUX=0
#CKGEN_ABE_MCBSP1_INT_MUX=0
#CKGEN_ABE_MCBSP2_GFCLK_MUX=0
#CKGEN_ABE_MCBSP2_INT_MUX=0
#CKGEN_ABE_MCBSP3_GFCLK_MUX=0
#CKGEN_ABE_MCBSP3_INT_MUX=0

#CM_CORE Settings
#CM_CORE_MMC1_GFCLK_MUX=1
#CM_CORE_MMC1_GFCLK_DIV=0 # 0 (OPP_NOM) or 1 map to 2^n
#CM_CORE_MMC2_GFCLK_MUX=1
#CM_CORE_MMC2_GFCLK_DIV=0 # 0 (OPP_NOM) or 1 map to 2^n
#CM_CORE_GPU_HYD_GCLK_MUX=0
#CM_CORE_GPU_CORE_GCLK_MUX=0
#CM_CORE_FDIF_GFCLK_DIV=0 # 0 (OPP_NOM), 1 (OPP_LOW) maps to 2^n
#CM_CORE_HSI_GFCLK_DIV=0 # 0 (OPP_NOM), 1 maps to 2^n
#CM_CORE_GPU_L3_DIV=0 # 0, 1 maps to 2^n

#TIMERs Settings
#TIMER1_GFCLK_MUX=1
#TIMER2_GFCLK_MUX=0
#TIMER3_GFCLK_MUX=0
#TIMER4_GFCLK_MUX=0
#TIMER9_GFCLK_MUX=0
#TIMER10_GFCLK_MUX=0
#TIMER11_GFCLK_MUX=0

#The following dpll settings are not needed at the moment they are placed 
#here in case they are added in the future to the clk framework
#HDMI PLL Settings
#HDMI_PLL_M=
#HDMI_PLL_N=
#HDMI_PLL_M2=

#DSI1_A PLL Settings
#DSI1_A_PLL_M=
#DSI1_A_PLL_N=
#DSI1_A_PLL_M4_DIV=
#DSI1_A_PLL_M5_DIV=

#DSI1_C PLL Settings
#DSI1_C_PLL_M=
#DSI1_C_PLL_N=
#DSI1_C_PLL_M4_DIV=
#DSI1_C_PLL_M5_DIV=


#################### End of variables definitions ###########################

omap5_abe_dpll_input_clks $SYS_CLK $FUNC_32K_CLK $PRM_ABE_DPLL_CLK_MUX $PRM_ABE_DPLL_BYPASS_CLK_MUX
#echo $ABE_DPLL_CLK $ABE_DPLL_BYPASS_CLK
omap5_abe_dpll $ABE_DPLL_CLK $ABE_PLL_M $ABE_PLL_N $ABE_PLL_M2 $ABE_PLL_REGM4 $ABE_PLL_M3
#echo $DPLL_ABE_X2_FCLK $CORE_DPLL_HS_CLK
ABE_DPLL_DCOLDO=$CLK_DCO_LDO

omap5_prm $((DPLL_ABE_X2_FCLK/16)) $SYS_CLK $FUNC_32K_CLK $PRM_WKUPAON_ICLK_MUX $PRM_ABE_SYS_CLK_DIV $PRM_MPU_DPLL_CLK_ABE_DIV $PRM_MPU_DPLL_CLK_EMIF_DIV $PRM_L3INSTR_TS_GCLK_AND_DLL_AGING_CLK_DIV
#echo $ABE_SYS_CLK $WKUPAON_ICLK $L3INSTR_TS_GCLK $L3INSTR_DLL_AGING_GCLK $CUSTEFUSE_SYS_GFCLK $DSS_SYS_GFCLK $MPU_DPLL_CLK_EMIF $MPU_DPLL_CLK_ABE

omap5_per_dpll $SYS_CLK $PER_PLL_M $PER_PLL_N $PER_PLL_M2 $PER_PLL_M3 $PER_PLL_H11 $PER_PLL_H12 $PER_PLL_H14
#echo $FUNC_96M_AON_CLK $FUNC_192M_FCLK $FUNC_256M_FCLK $DSS_GFCLK $PER_DPLL_SCRM_CLK $PER_GPU_CLK
PER_DPLL_DCOLDO=$CLK_DCO_LDO

omap5_core_dpll $SYS_CLK $CORE_PLL_M $CORE_PLL_N $CORE_PLL_M2 $CORE_PLL_M3 $CORE_PLL_H11 $CORE_PLL_H12 $CORE_PLL_H13 $CORE_PLL_H14 $CORE_PLL_H22 $CORE_PLL_H23 $CORE_PLL_H24
#echo $EMIF_PHY_GCLK $CORE_DPLL_SCRM_CLK $CORE_DLL_GCLK $CORE_X2_CLK $CORE_USB_OTG_SS_LFPS_TX_CLK $CORE_GPU_CLK $CORE_IPU_ISS_BOOST_CLK $CORE_ISS_MAIN_CLK $BB2D_GFCLK
CORE_DPLL_DCOLDO=$CLK_DCO_LDO

omap5_mpu_dpll $SYS_CLK $MPU_PLL_M $MPU_PLL_N $MPU_PLL_M2
#echo $MPU_GCLK $MA_EOCP_GICLK
MPU_DPLL_DCOLDO=$CLK_DCO_LDO

omap5_iva_dpll $SYS_CLK $IVA_PLL_M $IVA_PLL_N $IVA_PLL_H11 $IVA_PLL_H12
#echo $DSP_GCLK $IVA_GCLK
IVA_DPLL_DCOLDO=$CLK_DCO_LDO

omap5_usb_dpll $SYS_CLK $USB_PLL_M $USB_PLL_N $USB_PLL_M2
#echo $L3INIT_480M_GFCLK $L3INIT_960M_GFCLK
USB_DPLL_DCOLDO=$CLK_DCO_LDO

omap5_cd_prm_timerx $TIMER1_GFCLK_MUX
TIMER1_GFCLK=$TIMERx_GFCLK
omap5_cd_prm_timerx $TIMER2_GFCLK_MUX
TIMER2_GFCLK=$TIMERx_GFCLK
omap5_cd_prm_timerx $TIMER3_GFCLK_MUX
TIMER3_GFCLK=$TIMERx_GFCLK
omap5_cd_prm_timerx $TIMER4_GFCLK_MUX
TIMER4_GFCLK=$TIMERx_GFCLK
omap5_cd_prm_timerx $TIMER9_GFCLK_MUX
TIMER9_GFCLK=$TIMERx_GFCLK
omap5_cd_prm_timerx $TIMER10_GFCLK_MUX
TIMER10_GFCLK=$TIMERx_GFCLK
omap5_cd_prm_timerx $TIMER11_GFCLK_MUX
TIMER11_GFCLK=$TIMERx_GFCLK

omap5_cm_core_aon $CORE_X2_CLK $CORE_DPLL_HS_CLK $FUNC_96M_AON_CLK $CM_CORE_AON_L3_ICLK_DIV $CM_CORE_AON_L4_ROOT_CLK_DIV $CM_CORE_AON_MPU_DPLL_HS_CLK_DIV $CM_CORE_AON_IVA_DPLL_HS_CLK_DIV
#echo $L3_ICLK $CM_CORE_AON_PROFILING_L3_GICLK $L4_ROOT_CLK $L4_ICLK $CM_CORE_AON_PROFILING_L4_GICLK $MPU_DPLL_HS_CLK $IVA_DPLL_HS_CLK $PER_DPLL_HS_CLK $USB_DPLL_HS_CLK $FUNC_24M_GFCLK

omap5_ckgen_usb $L3INIT_480M_GFCLK $XCLK_60M_HSP1 $XCLK_60M_HSP2 $CKGEN_USB_L3_INIT_60M_FCLK_DIV $CKGEN_USB_UTMI_P1_GFCLK_MUX $CKGEN_USB_UTMI_P2_GFCLK_MUX
#echo $UTMI_P1_GFCLK $UTMI_P2_GFCLK $L3INIT_60M_FCLK

omap5_ckgen_abe $DPLL_ABE_X2_FCLK $FUNC_24M_GFCLK $ABE_SYS_CLK $FUNC_SYS_32K $PAD_UCLKS $SLIMBUS_UCLKS $CKGEN_ABE_CLK_DIV $CKGEN_AESS_FCLK_ABE_GICLK_DIV $CKGEN_ABE_TIMER5_GFCLK_MUX $CKGEN_ABE_TIMER6_GFCLK_MUX $CKGEN_ABE_TIMER7_GFCLK_MUX $CKGEN_ABE_TIMER8_GFCLK_MUX $CKGEN_ABE_DMIC_GFCLK_MUX $CKGEN_ABE_DMIC_INT_MUX $CKGEN_ABE_MCASP_INT_MUX $CKGEN_ABE_MCASP_GFCLK_MUX $CKGEN_ABE_MCBSP1_GFCLK_MUX $CKGEN_ABE_MCBSP1_INT_MUX $CKGEN_ABE_MCBSP2_GFCLK_MUX $CKGEN_ABE_MCBSP2_INT_MUX $CKGEN_ABE_MCBSP3_GFCLK_MUX $CKGEN_ABE_MCBSP3_INT_MUX
#echo $ABE_LP_CLK $ABE_CLK $AESS_CLK $ABE_GICLK $ABE_24M_FCLK $TIMER5_GFCLK $TIMER6_GFCLK $TIMER7_GFCLK $TIMER8_GFCLK $DMIC_GFCLK $MCBSP1_GFCLK $MCBSP2_GFCLK $MCBSP3_GFCLK $MCASP_GFCLK

omap5_cm_core $PER_GPU_CLK $FUNC_256M_FCLK $FUNC_192M_FCLK $CORE_GPU_CLK $L3_ICLK $CM_CORE_MMC1_GFCLK_MUX $CM_CORE_MMC1_GFCLK_DIV $CM_CORE_MMC2_GFCLK_MUX $CM_CORE_MMC2_GFCLK_DIV $CM_CORE_GPU_HYD_GCLK_MUX $CM_CORE_GPU_CORE_GCLK_MUX $CM_CORE_FDIF_GFCLK_DIV $CM_CORE_HSI_GFCLK_DIV $CM_CORE_GPU_L3_DIV
#echo $GPU_HYD_GCLK $GPU_CORE_GCLK $FUNC_128M_CLK $FUNC_48M_FCLK $FUNC_96M_FCLK $FDIF_GFCLK $HSI_GFCLK $FUNC_12M_FCLK $PER_12M_GFCLK $GPU_L3_ICLK $GPU_L3_GICLK $MMC1_GFCLK $MMC2_GFCLK $PER_96M_GFCLK $CSI_PHY_GFCLK $PER_48M_GFCLK $L3INIT_48M_GFCLK $HDMI_PHY_GFCLK

src_clk=virt_${SYS_CLK}_ck

add_to_tree pad_clks_src_ck $PAD_UCLKS 
add_to_tree pad_clks_src_ck/pad_clks_ck $PAD_UCLKS
#Not sure if secure is supported in this board
add_to_tree secure_32k_clk_src_ck $FUNC_32K_CLK
add_to_tree slimbus_src_clk $SLIMBUS_UCLKS
add_to_tree slimbus_src_clk/slimbus_clk $SLIMBUS_UCLKS
add_to_tree slimbus_src_clk/slimbus_clk/slimbus1_slimbus_clk $SLIMBUS_UCLKS
#test_print_trc "Processing 32K source"
add_to_tree sys_32k_ck $FUNC_32K_CLK
add_to_tree sys_32k_ck/dss_32khz_clk $FUNC_32K_CLK
add_to_tree sys_32k_ck/gpio1_dbclk $FUNC_32K_CLK
add_to_tree sys_32k_ck/gpio2_dbclk $FUNC_32K_CLK
add_to_tree sys_32k_ck/gpio3_dbclk $FUNC_32K_CLK
add_to_tree sys_32k_ck/gpio4_dbclk $FUNC_32K_CLK
add_to_tree sys_32k_ck/gpio5_dbclk $FUNC_32K_CLK
add_to_tree sys_32k_ck/gpio6_dbclk $FUNC_32K_CLK
add_to_tree sys_32k_ck/gpio7_dbclk $FUNC_32K_CLK
add_to_tree sys_32k_ck/gpio8_dbclk $FUNC_32K_CLK
add_to_tree sys_32k_ck/mmc1_32khz_clk $FUNC_32K_CLK
add_to_tree sys_32k_ck/usb_phy_cm_clk32k $FUNC_32K_CLK
add_to_tree sys_32k_ck/timer1_gfclk_mux $FUNC_32K_CLK
add_to_tree virt_12000000_ck 12000000
add_to_tree virt_16800000_ck 16800000
add_to_tree virt_19200000_ck 19200000
add_to_tree ${src_clk}/sys_clkin $SYS_CLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_bypass_clk_mux $ABE_DPLL_BYPASS_CLK
#test_print_trc "Processing DPLL_CORE clks"
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck $((CORE_DPLL_DCOLDO/2))
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck $CORE_DPLL_DCOLDO
#No dpll core h21 clk in TRM
#add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h21x2_ck
#add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h21x2_ck/c2c_fclk
#add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h21x2_ck/c2c_fclk/c2c_iclk
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h11x2_ck $CORE_DLL_GCLK
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck $CORE_X2_CLK
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/iva_dpll_hs_clk_div $IVA_DPLL_HS_CLK 
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/mpu_dpll_hs_clk_div $MPU_DPLL_HS_CLK 
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/l3_iclk_div $L3_ICLK
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/l3_iclk_div/gpu_l3_iclk $GPU_L3_GICLK
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/l3_iclk_div/l4_root_clk_div $L4_ROOT_CLK
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h13x2_ck $CORE_USB_OTG_SS_LFPS_TX_CLK
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h14x2_ck $CORE_GPU_CLK
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h14x2_ck/gpu_core_gclk_mux $GPU_CORE_GCLK 
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h14x2_ck/gpu_hyd_gclk_mux $GPU_HYD_GCLK
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h22x2_ck $CORE_IPU_ISS_BOOST_CLK
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h23x2_ck $CORE_ISS_MAIN_CLK
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_h24x2_ck $BB2D_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_m3x2_ck  $CORE_DPLL_SCRM_CLK
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_x2_ck/dpll_core_m3x2_ck/dpll_core_m3x2_opt_ck $CORE_DPLL_SCRM_CLK 
add_to_tree ${src_clk}/sys_clkin/dpll_core_ck/dpll_core_m2_ck $EMIF_PHY_GCLK
add_to_tree ${src_clk}/sys_clkin/custefuse_sys_gfclk_div $CUSTEFUSE_SYS_GFCLK 
#test_print_trc 'Processing DPLL_IVA clks'
add_to_tree ${src_clk}/sys_clkin/dpll_iva_ck $((IVA_DPLL_DCOLDO/2))
add_to_tree ${src_clk}/sys_clkin/dpll_iva_ck/dpll_iva_x2_ck $IVA_DPLL_DCOLDO
add_to_tree ${src_clk}/sys_clkin/dpll_iva_ck/dpll_iva_x2_ck/dpll_iva_h11x2_ck $DSP_GCLK
add_to_tree ${src_clk}/sys_clkin/dpll_iva_ck/dpll_iva_x2_ck/dpll_iva_h12x2_ck $IVA_GCLK
#test_print_trc 'Processing DPLL_PER clks'
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck $((PER_DPLL_DCOLDO/2))
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck $PER_DPLL_DCOLDO
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_h11x2_ck $FUNC_256M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_h11x2_ck/func_128m_clk $FUNC_128M_CLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_h11x2_ck/fdif_fclk $FDIF_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_h12x2_ck $DSS_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_h12x2_ck/dss_dss_clk $DSS_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_h14x2_ck $PER_GPU_CLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck $FUNC_192M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_12m_fclk $FUNC_12M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk $FUNC_48M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/dss_48mhz_clk $FUNC_48M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_96m_fclk $FUNC_96M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_96m_fclk/iss_ctrlclk $FUNC_96M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/hsi_fclk $HSI_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/mmc1_fclk_mux $MMC1_GFCLK 
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/mmc1_fclk_mux/mmc1_fclk $MMC1_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/mmc2_fclk_mux $MMC2_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/mmc2_fclk_mux/mmc2_fclk $MMC2_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_m2_ck $FUNC_96M_AON_CLK
add_to_tree ${src_clk}/sys_clkin/dpll_per_ck/dpll_per_m2_ck/func_24m_clk $FUNC_24M_GFCLK
#No unipro in TRM
#add_to_tree ${src_clk}/sys_clkin/dpll_unipro1_ck
#add_to_tree ${src_clk}/sys_clkin/dpll_unipro1_ck/dpll_unipro1_clkdcoldo
#add_to_tree ${src_clk}/sys_clkin/dpll_unipro1_ck/dpll_unipro1_clkdcoldo/lli_txphy_clk
#add_to_tree ${src_clk}/sys_clkin/dpll_unipro1_ck/dpll_unipro1_m2_ck
#add_to_tree ${src_clk}/sys_clkin/dpll_unipro1_ck/dpll_unipro1_m2_ck/lli_txphy_ls_clk
#add_to_tree ${src_clk}/sys_clkin/dpll_unipro2_ck
#add_to_tree ${src_clk}/sys_clkin/dpll_unipro2_ck/dpll_unipro2_clkdcoldo
#add_to_tree ${src_clk}/sys_clkin/dpll_unipro2_ck/dpll_unipro2_m2_ck
#add_to_tree ${src_clk}/sys_clkin/dss_syc_gfclk_div $DSS_SYS_GFCLK
#test_print_trc 'Processing CKGEN_ABE clks'
add_to_tree ${src_clk}/sys_clkin/abe_sys_clk_div $ABE_SYS_CLK
add_to_tree ${src_clk}/sys_clkin/abe_sys_clk_div/dss_sys_gfclk $DSS_SYS_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_sys_clk_div/timer5_gfclk_mux $TIMER5_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_syc_clk_div/timer6_gfclk_mux $TIMER6_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_sys_clk_div/timer7_gfclk_mux $TIMER7_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_sys_clk_div/timer8_gfclk_mux $TIMER8_GFCLK
#test_print_trc 'Processing PRM clks'
add_to_tree ${src_clk}/sys_clkin/wkupaon_iclk_mux $WKUPAON_ICLK
add_to_tree ${src_clk}/sys_clkin/wkupaon_iclk_mux/l3instr_ts_gclk_div $L3INSTR_TS_GCLK
add_to_tree ${src_clk}/sys_clkin/sata_ref_clk $SYS_CLK 
add_to_tree ${src_clk}/sys_clkin/timer10_gfclk_mux $TIMER10_GFCLK
add_to_tree ${src_clk}/sys_clkin/timer11_gfclk_mux $TIMER11_GFCLK
add_to_tree ${src_clk}/sys_clkin/timer2_gfclk_mux $TIMER2_GFCLK
add_to_tree ${src_clk}/sys_clkin/timer3_gfclk_mux $TIMER3_GFCLK
add_to_tree ${src_clk}/sys_clkin/timer4_gfclk_mux $TIMER4_GFCLK
add_to_tree ${src_clk}/sys_clkin/timer9_gfclk_mux $TIMER9_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux $ABE_DPLL_CLK
#test_print_trc 'Processing DPLL_ABE clks'
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck $((ABE_DPLL_DCOLDO/2)) 
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck $ABE_DPLL_DCOLDO
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck $DPLL_ABE_X2_FCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk $ABE_24M_FCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/dmic_sync_mux_ck $DMIC_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/dmic_sync_mux_ck/dmic_gfclk $DMIC_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcasp_sync_mux_ck $MCASP_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcasp_sync_mux_ck/mcasp_gfclk $MCASP_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcbsp1_sync_mux_ck $MCBSP1_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcbsp1_sync_mux_ck/mcbsp1_gfclk $MCBSP1_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcbsp2_sync_mux_ck $MCBSP2_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcbsp2_sync_mux_ck/mcbsp2_gfclk $MCBSP2_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcbsp3_sync_mux_ck $MCBSP3_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcbsp3_sync_mux_ck/mcbsp3_gfclk $MCBSP3_GFCLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_clk $ABE_CLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_clk/abe_iclk $ABE_GICLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_clk/aess_fclk $AESS_CLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_lp_clk_div $ABE_LP_CLK 
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m3x2_ck $CORE_DPLL_HS_CLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m3x2_ck/per_dpll_hs_clk_div $PER_DPLL_HS_CLK
add_to_tree ${src_clk}/sys_clkin/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m3x2_ck/usb_dpll_hs_clk_div $USB_DPLL_HS_CLK
#test_print_trc 'Processing DPLL_USB clks'
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck $((USB_DPLL_DCOLDO/2))
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_clkdcoldo $L3INIT_960M_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_clkdcoldo/usb_otg_ss_refclk960m $L3INIT_960M_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck $L3INIT_480M_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk $L3INIT_60M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk/usb_host_hs_hsic60m_p1_clk $L3INIT_60M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk/usb_host_hs_hsic60m_p2_clk $L3INIT_60M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk/usb_host_hs_hsic60m_p3_clk $L3INIT_60M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk/usb_host_hs_utmi_p3_clk $L3INIT_60M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk/usb_tll_hs_usb_ch0_clk $L3INIT_60M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk/usb_tll_hs_usb_ch1_clk $L3INIT_60M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk/usb_tll_hs_usb_ch2_clk $L3INIT_60M_FCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk/utmi_p1_gfclk $UTMI_P1_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk/utmi_p1_gfclk/usb_host_hs_utmi_p1_clk $UTMI_P1_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk/utmi_p2_gfclk $UTMI_P2_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk/utmi_p2_gfclk/usb_host_hs_utmi_p2_clk $UTMI_P2_GFCLK 
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/usb_host_hs_hsic480m_p1_clk $L3INIT_480M_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/usb_host_hs_hsic480m_p2_clk $L3INIT_480M_GFCLK
add_to_tree ${src_clk}/sys_clkin/dpll_usb_ck/dpll_usb_m2_ck/usb_host_hs_hsic480m_p3_clk $L3INIT_480M_GFCLK
#test_print_trc 'Processing DPLL_MPU clks'
add_to_tree ${src_clk}/sys_clkin/dpll_mpu_ck $((MPU_DPLL_DCOLDO/2))
add_to_tree ${src_clk}/sys_clkin/dpll_mpu_ck/dpll_mpu_m2_ck $MPU_GCLK
#test_print_trc 'Processing other clks'
add_to_tree virt_26000000_ck 26000000
add_to_tree virt_38400000_ck 38400000
add_to_tree xclk60mhsp1_ck $XCLK_60M_HSP1
add_to_tree xclk60mhsp2_ck $XCLK_60M_HSP2
add_to_tree orphans
add_to_tree orphans/dpll_per_m3x2_opt_ck $PER_DPLL_SCRM_CLK
add_to_tree orphans/auxclk0_src_ck $FREF_CLK0_AUX
add_to_tree orphans/auxclk0_src_ck/auxclk0_ck $FREF_CLK0_AUX
add_to_tree orphans/auxclk0_src_ck/auxclk0_ck/auxclkreq0_ck $FREF_CLK0_AUX
add_to_tree orphans/auxclk1_src_ck $FREF_CLK1_AUX
add_to_tree orphans/auxclk1_src_ck/auxclk1_ck $FREF_CLK1_AUX
add_to_tree orphans/auxclk1_src_ck/auxclk1_ck/auxclkreq1_ck $FREF_CLK1_AUX
add_to_tree orphans/auxclk2_src_ck $FREF_CLK2_AUX
add_to_tree orphans/auxclk2_src_ck/auxclk2_ck $FREF_CLK2_AUX
add_to_tree orphans/auxclk2_src_ck/auxclk2_ck/auxclkreq2_ck $FREF_CLK2_AUX
#fref_clk3_out is not mapped
#add_to_tree orphans/auxclk3_src_ck
#add_to_tree orphans/auxclk3_src_ck/auxclk3_ck
#add_to_tree orphans/auxclk3_src_ck/auxclk3_ck/auxclkreq3_ck
add_to_tree orphans/dummy_clk 0

echo ${omap5_CLK_TREE[@]}
