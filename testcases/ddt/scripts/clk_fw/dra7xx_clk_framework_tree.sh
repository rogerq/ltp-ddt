#!/bin/bash
source st_log.sh
source dra7xx_clk_framework_utils.sh

dra7xx_CLK_TREE=()
add_to_tree() {
  dra7xx_CLK_TREE[${#dra7xx_CLK_TREE[*]}]=$1=$2
}

#A file that defines the following variables (commented variables only) and 
#sources this file must be created to obtain a clk tree.

#MASTER clocks sources
SYS_32K=32768
SYS_CLK1=20000000
SYS_CLK2=22579200
XREF_CLK0=0
XREF_CLK1=0
XREF_CLK2=0
XREF_CLK3=0
RMII_CLK=50000000
#Setting ATL_CLKs to 0 until needed
ATL_CLK0=0
ATL_CLK1=0
ATL_CLK2=0
ATL_CLK3=0
#Settings MLB and MLBP clks to 0 until needed
MLB_CLK=0
MLBP_CLK=0
#PCIE APLL input clk
PCIESREF_ACS_CLK=100000000 

#PER DPLL settings
#PER_PLL_M=
#PER_PLL_N=
#PER_PLL_M2=
#PER_PLL_H11=
#PER_PLL_H12=
#PER_PLL_H13= 
#PER_PLL_H14=

#Core DPLL Settings
#CORE_PLL_M=
#CORE_PLL_N=
#CORE_PLL_M2=
#CORE_PLL_H12=
#CORE_PLL_H13=
#CORE_PLL_H14=
#CORE_PLL_H22=
#CORE_PLL_H23=
#CORE_PLL_H24=

#ABE DPLL Settings
#Info missing from the TRM
#ABE_PLL_M=
#ABE_PLL_N=
#ABE_PLL_M2=
#ABE_PLL_REGM4XEN=
#ABE_PLL_M3=

#MPU DPLL Settings
#MPU_PLL_M=
#MPU_PLL_N=
#MPU_PLL_M2=

#IVA DPLL Settings
#IVA_PLL_M=
#IVA_PLL_N=
#IVA_PLL_M2=

#USB DPLL Settings
#USB_PLL_M=
#USB_PLL_N=
#USB_PLL_M2=

#EVE DPLL Settings
#EVE_PLL_M=
#EVE_PLL_N=
#EVE_PLL_M2=

#DSP DPLL Settings
#DSP_PLL_M=
#DSP_PLL_N=
#DSP_PLL_M2=
#DSP_PLL_M3=

#GMAC DPLL Settings
#GMAC_PLL_M=
#GMAC_PLL_N=
#GMAC_PLL_M2=
#GMAC_PLL_M3= #Set to 1 for now until TRM is finalized
#GMAC_PLL_H11= #Set to 9 for now until TRM is finalized
#GMAC_PLL_H12=
#GMAC_PLL_H13= #Set to 0 for now until TRM is finalized

#GPU DPLL Settings
#GPU_PLL_M=
#GPU_PLL_N=
#GPU_PLL_M2=

#DDR DPLL Settings
#DDR_PLL_M=
#DDR_PLL_N=
#DDR_PLL_M2=
#DDR_PLL_H11= #Set to 0 for now until TRM is finalized

#PCIE Ref PLL Settings
#PCIE_REF_PLL_M=
#PCIE_REF_PLL_N=
#PCIE_REF_PLL_M2= #Set to 1 until TRM is finalized

#PCIE APLL Settings
#PCIE_APLL_M2= #Set to 1 until TRM is finalized
#PCIE_APLL_DIV= #Either 0 (div 1) or 1 (div 0), Set to 0 until TRM is finalized

#USB OTG SS DPLL Settings
#USB_OTG_SS_PLL_M=125
#USB_OTG_SS_PLL_N=0

#USB OTG SS DPLL Settings
#SATA_PLL_M=75
#SATA_PLL_N=0

#DEBUGSS DPLL Settings
#DEBUGSS_PLL_M=95
#DEBUGSS_PLL_N=1
#DEBUGSS_PLL_M6_HSDIV=2

#PRM Settings
#PRM_ABE_PLL_CLK_MUX=
#PRM_WKUPAON_ICLK_MUX=
#PRM_ABE_DPLL_BYPASS_CLK_MUX=
#PRM_DCAN1_SYS_CLK_MUX=
#PRM_ABE_DPLL_SYS_CLK_MUX=
#PRM_VIDEO1_DPLL_CLK_MUX=
#PRM_VIDEO2_DPLL_CLK_MUX=
#PRM_HDMI_DPLL_CLK_MUX=
#PRM_ABE_SYS_CLK_DIV=
#PRM_TIMER_SYS_CLK_DIV=
#PRM_MPU_DPLL_CLK_ABE_DIV=
#PRM_MPU_DPLL_CLK_EMIF_DIV= # 0 or 1 (div 4); 2 or 3 (div 8)
#PRM_L3INSTR_TS_GCLK_AND_DLL_AGING_CLK_DIV= # 0 (div 8); 1 (div 16); 2 (div 32)

#CM_CORE_AON part a Settings
#CM_CORE_AON_L3_ICLK_DIV= # 0 or 1 maps to 2^n
#CM_CORE_AON_L4_ROOT_CLK_DIV=i # 0 or 1 maps to 2^n
#CM_CORE_AON_MPU_DPLL_HS_CLK_DIV= #0, 1, 2, or 3 map to 2^n
#CM_CORE_AON_IVA_DPLL_HS_CLK_DIV= #0, 1, 2, or 3 map to 2^n
#CM_CORE_AON_EVE_DPLL_HS_CLK_DIV= #0, 1, 2, or 3 map to 2^n
#CM_CORE_AON_DSP_DPLL_HS_CLK_DIV= #0, 1, 2, or 3 map to 2^n
#CM_CORE_AON_ABE_LP_CLK_DIV=  #TRM table 3-39 refers to bits 1:0,only bit 0 is used according to register
#CM_CORE_AON_ABE_24M_FCLK_DIV=  #TRM table 3-39 refers to bits 1:0,only bit 0 is used according to register

##CM_CORE_AON part b Settings
#CM_CORE_AON_MMC1_GFCLK_MUX=
#CM_CORE_AON_MMC1_GFCLK_DIV= # 0, 1, or 2 maps to 2^n
#CM_CORE_AON_MMC2_GFCLK_MUX=
#CM_CORE_AON_MMC2_GFCLK_DIV= # 0, 1, or 2 maps to 2^n
#CM_CORE_AON_MMC3_FCLK_MUX=
#CM_CORE_AON_MMC3_FCLK_DIV= # 0, 1, or 2 maps to 2^n
#CM_CORE_AON_MMC4_FCLK_MUX=
#CM_CORE_AON_MMC4_FCLK_DIV= # 0, 1, or 2 maps to 2^n
#CM_CORE_AON_GPU_HYD_GCLK_MUX=
#CM_CORE_AON_GPU_CORE_GCLK_MUX=
#CM_CORE_AON_QSPI_GFCLK_MUX=
#CM_CORE_AON_QSPI_GFCLK_DIV= # 0, 1, or 2 maps to 2^n
#CM_CORE_AON_VIP1_GCLK_MUX=
#CM_CORE_AON_VIP2_GCLK_MUX=
#CM_CORE_AON_VIP3_GCLK_MUX=
#CM_CORE_AON_ABE_CLK_DIV= # 0, 1, 2, 3, or 4 maps to 2^n
#CM_CORE_AON_AESS_FCLK_DIV= # 0 or 1 maps to 2^n
#CM_CORE_AON_ABE_GICLK_DIV= # 0 or 1 maps to 2^n
#CM_CORE_AON_IPU1_GFCLK_MUX=
#CM_CORE_AON_RGMII_50M_CLK_MUX=
#CM_CORE_AON_ATL_GFCLK_MUX=
#CM_CORE_AON_ATL_SOURCE1_MUX=
#CM_CORE_AON_GMAC_RFT_CLK_MUX= #TRM CM_GMAC_GMAC_CLKTCTRL register info does not show sources
#CM_CORE_AON_EVE_CLK_MUX=
#CM_CORE_AON_EVE_DCLK_DIV= # 0, 1, 2, 3, 4, or 5 maps to 2^n

#VIDEO1 PLL Settings
#VIDEO1_PLL_M=
#VIDEO1_PLL_N=
#VIDEO1_M4_HSDIV=
#VIDEO1_M5_HSDIV=38 #TRM is missing M5 div register info
#VIDEO1_M6_HSDIV=

#VIDEO2 PLL Settings
#VIDEO2_PLL_M=
#VIDEO2_PLL_N=
#VIDEO2_M4_HSDIV=
#VIDEO2_M5_HSDIV=10 #TRM is missing M5 div register info
#VIDEO2_M6_HSDIV=

#HDMI PLL Settings
#HDMI_PLL_M=
#HDMI_PLL_N=
#HDMI_PLL_M2=

#CM_CORE_AON_CLKOUTMUX Mux Settings
#All dividers here follow # 0, 1, 2, 3, 4, or 5 maps to 2^n
#CLKOUTMUX0_CLK_MUX=
#CLKOUTMUX_SYS_CLK1_DCLK_DIV=
#CLKOUTMUX_SYS_CLK2_DCLK_DIV=
#CLKOUTMUX_PER_ABE_X1_DCLK_DIV=
#CLKOUTMUX_MPU_DCLK_DIV=
#CLKOUTMUX_DSP_DCLK_DIV=
#CLKOUTMUX_IVA_DCLK_DIV=
#CLKOUTMUX_GPU_DCLK_DIV=
#CLKOUTMUX_CORE_DPLL_OUT_DCLK_DIV=
#CLKOUTMUX_EMIF_PHY_DCLK_DIV=
#CLKOUTMUX_GMAC_250M_DCLK_DIV=
#CLKOUTMUX_VIDEO2_DCLK_DIV=
#CLKOUTMUX_VIDEO1_DCLK_DIV=
#CLKOUTMUX_HDMI_DCLK_DIV=
#CLKOUTMUX_FUNC_96M_AON_DCLK_DIV=
#CLKOUTMUX_L3INIT_480M_DCLK_DIV=
#CLKOUTMUX_USB_OTG_DCLK_DIV=
#CLKOUTMUX_SATA_DCLK_DIV=
#CLKOUTMUX_PCIE2_DCLK_DIV=
#CLKOUTMUX_PCIE1_DCLK_DIV=
#CLKOUTMUX_EMU_DCLK_DIV=

#TIMERs Settings
#TIMER_VIDEO1_CLK_DIV= # 0, 1, 2, 3 and 4 map to 2^n; 5 div 22; 6 div 32
#TIMER_VIDEO2_CLK_DIV= # 0, 1, 2, 3 and 4 map to 2^n; 5 div 22; 6 div 32
#TIMER_HDMI_CLK_DIV= # 0, 1, 2, 3 and 4 map to 2^n; 5 div 22; 6 div 32
#TIMER1_GFCLK_MUX=
#TIMER2_GFCLK_MUX=
#TIMER3_GFCLK_MUX=
#TIMER4_GFCLK_MUX=
#TIMER5_GFCLK_MUX=
#TIMER6_GFCLK_MUX=
#TIMER7_GFCLK_MUX=
#TIMER8_GFCLK_MUX=
#TIMER9_GFCLK_MUX=
#TIMER10_GFCLK_MUX=
#TIMER11_GFCLK_MUX=
#TIMER13_GFCLK_MUX=
#TIMER14_GFCLK_MUX=
#TIMER15_GFCLK_MUX=
#TIMER16_GFCLK_MUX=

#CM_CORE_AON MCASPs Settings
#MCASP AUX_GFCLK Settings
#MCASP1_AUX_GFCLK_MUX=
#MCASP2_AUX_GFCLK_MUX=
#MCASP3_AUX_GFCLK_MUX=
#MCASP4_AUX_GFCLK_MUX=
#MCASP5_AUX_GFCLK_MUX=
#MCASP6_AUX_GFCLK_MUX=
#MCASP7_AUX_GFCLK_MUX=
#MCASP8_AUX_GFCLK_MUX=
#MCASP_VIDEO1_CLK_DIV= # 0, 1, 2, 3 and 4 map to 2^n
#MCASP_VIDEO2_CLK_DIV= # 0, 1, 2, 3 and 4 map to 2^n
#MCASP_HDMI_CLK_DIV= # 0, 1, 2, 3 and 4 map to 2^n
#MCASP_PER_ABE_X1_GFCLK_DIV= # 0, 1, 2, 3 and 4 map to 2^n
#MCASP AHCLKX and AHCLKR Settings
#MCASP1_AHCLKX_MUX=
#MCASP1_AHCLKR_MUX=
#MCASP2_AHCLKX_MUX=
#MCASP2_AHCLKR_MUX=
#MCASP_MLB_CLK_DIV= # 0, 1, 2, 3 and 4 map to 2^n
#MCASP_MLBP_CLK_DIV= # 0, 1, 2, 3 and 4 map to 2^n
#MCASP3_AHCLKX_MUX=
#MCASP4_AHCLKX_MUX=
#MCASP5_AHCLKX_MUX=
#MCASP6_AHCLKX_MUX=
#MCASP7_AHCLKX_MUX=
#MCASP8_AHCLKX_MUX=

#################### End of variables definitions ###########################

dra7xx_per_dpll $SYS_CLK1 $PER_PLL_M $PER_PLL_N $PER_PLL_M2 $PER_PLL_H11 $PER_PLL_H12 $PER_PLL_H13 $PER_PLL_H14
#echo $FUNC_96M_AON_CLK $FUNC_192M_CLK $FUNC_256M_CLK $DSS_GFCLK $PER_QSPI_CLK $PER_GPU_CLK
PER_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_core_dpll $SYS_CLK1 $CORE_PLL_M $CORE_PLL_N $CORE_PLL_M2 $CORE_PLL_H12 $CORE_PLL_H13 $CORE_PLL_H14 $CORE_PLL_H22 $CORE_PLL_H23 $CORE_PLL_H24
#echo $CORE_DPLL_OUT_CLK $CORE_X2_CLK $CORE_USB_OTG_SS_LFPS_TX_CLK $CORE_GPU_CLK $CORE_IPU_ISS_BOOST_CLK $CORE_ISS_MAIN_CLK $BB2D_GFCLK
CORE_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_mpu_dpll $SYS_CLK1 $MPU_PLL_M $MPU_PLL_N $MPU_PLL_M2
#echo $MPU_GCLK $MA_EOCP_GICLK
MPU_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_iva_dpll $SYS_CLK1 $IVA_PLL_M $IVA_PLL_N $IVA_PLL_M2
#echo $IVA_GCLK 
IVA_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_usb_dpll $SYS_CLK1 $USB_PLL_M $USB_PLL_N $USB_PLL_M2
#echo $L3INIT_480M_GFCLK $L3INIT_960M_GFCLK
USB_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_eve_dpll $SYS_CLK1 $EVE_PLL_M $EVE_PLL_N $EVE_PLL_M2
#echo $EVE_GFCLK
EVE_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_dsp_dpll $SYS_CLK1 $DSP_PLL_M $DSP_PLL_N $DSP_PLL_M2 $DSP_PLL_M3
#echo $DSP_GFCLK $EVE_GCLK
DSP_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_gmac_dpll $SYS_CLK1 $GMAC_PLL_M $GMAC_PLL_N $GMAC_PLL_M2 $GMAC_PLL_M3 $GMAC_PLL_H11 $GMAC_PLL_H12 $GMAC_PLL_H13
#echo $GMAC_250M_CLK $ICSS_IEP_CLK $GMAC_RMII_HS_CLK $GMII_250MHZ_CLK $ICSS_CLK
GMAC_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_gpu_dpll $SYS_CLK1 $GPU_PLL_M $GPU_PLL_N $GPU_PLL_M2
#echo $GPU_GCLK
GPU_DPLL_DCOLDO=$CLK_DCO_LDO
 
dra7xx_ddr_dpll $SYS_CLK1 $DDR_PLL_M $DDR_PLL_N $DDR_PLL_M2 $DDR_PLL_H11
#echo $EMIF_PHY_GCLK $EMIF_DLL_GCLK 
DDR_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_pcie_ref_dpll $SYS_CLK1 $PCIE_REF_PLL_M $PCIE_REF_PLL_N $PCIE_REF_PLL_M2 
#echo $PCIE_M2_CLK $PCIE_REF_CLK
PCIE_REF_DPLL_DCOLDO=$CLK_DCO_LDO
 
dra7xx_pcie_apll $PCIE_APLL_M2 $PCIE_APLL_DIV
#echo $PCIE_PHY_CLK $PCIE_DIV_GCLK $APLL_PCIE_M2_CLK

dra7xx_abe_dpll_input_clks $SYS_CLK1 $SYS_CLK2 $SYS_32K $PRM_ABE_PLL_CLK_MUX $PRM_ABE_DPLL_BYPASS_CLK_MUX $PRM_ABE_DPLL_SYS_CLK_MUX
#echo $ABE_DPLL_CLK $ABE_DPLL_BYPASS_CLK $ABE_DPLL_SYS_CLK
dra7xx_abe_dpll $ABE_DPLL_CLK $ABE_PLL_M $ABE_PLL_N $ABE_PLL_M2 $ABE_PLL_REGM4XEN $ABE_PLL_M3
#echo $PER_ABE_X1_GFCLK $DPLL_ABE_X2_FCLK $CORE_DPLL_HS_CLK
ABE_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_cm_core_aon_a $CORE_X2_CLK $DPLL_ABE_X2_FCLK $CORE_DPLL_HS_CLK $FUNC_96M_AON_CLK $CM_CORE_AON_L3_ICLK_DIV $CM_CORE_AON_L4_ROOT_CLK_DIV $CM_CORE_AON_MPU_DPLL_HS_CLK_DIV $CM_CORE_AON_IVA_DPLL_HS_CLK_DIV $CM_CORE_AON_EVE_DPLL_HS_CLK_DIV $CM_CORE_AON_DSP_DPLL_HS_CLK_DIV $CM_CORE_AON_ABE_LP_CLK_DIV $CM_CORE_AON_ABE_24M_FCLK_DIV
#echo $L3_ICLK $CM_CORE_AON_PROFILING_L3_GICLK $L4_ROOT_CLK $L4_ICLK $CM_CORE_AON_PROFILING_L4_GICLK $MPU_DPLL_HS_CLK $IVA_DPLL_HS_CLK $EVE_DPLL_HS_CLK $DSP_DPLL_HS_CLK $ABE_LP_CLK $ABE_24M_GFCLK $PER_DPLL_HS_CLK $USB_DPLL_HS_CLK $FUNC_24M_GFCLK

dra7xx_prm $ABE_LP_CLK $SYS_CLK1 $SYS_CLK2 $SYS_32K $PRM_WKUPAON_ICLK_MUX $PRM_DCAN1_SYS_CLK_MUX $PRM_VIDEO1_DPLL_CLK_MUX $PRM_VIDEO2_DPLL_CLK_MUX $PRM_HDMI_DPLL_CLK_MUX $PRM_ABE_SYS_CLK_DIV $PRM_TIMER_SYS_CLK_DIV $PRM_MPU_DPLL_CLK_ABE_DIV $PRM_MPU_DPLL_CLK_EMIF_DIV $PRM_L3INSTR_TS_GCLK_AND_DLL_AGING_CLK_DIV
#echo $WKUPAON_ICLK $DCAN1_SYS_CLK $VIDEO1_DPLL_CLK $VIDEO2_DPLL_CLK $HDMI_DPLL_CLK $MPU_DPLL_CLK $ABE_SYS_CLK $TIMER_SYS_CLK $MPU_DPLL_CLK_ABE $MPU_DPLL_CLK_EMIF $L3INSTR_TS_GCLK $L3INSTR_DLL_AGING_GCLK $CUSTEFUSE_SYS_GFCLK


dra7xx_dss_dpll_videox $VIDEO1_DPLL_CLK $VIDEO1_PLL_M $VIDEO1_PLL_N $VIDEO1_M4_HSDIV $VIDEO1_M5_HSDIV $VIDEO1_M6_HSDIV
VIDEO1_CLK=$DPLL_VIDEO_CLKOUTM6
VIDEO1_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_dss_dpll_videox $VIDEO2_DPLL_CLK $VIDEO2_PLL_M $VIDEO2_PLL_N $VIDEO2_M4_HSDIV $VIDEO2_M5_HSDIV $VIDEO2_M6_HSDIV
VIDEO2_CLK=$DPLL_VIDEO_CLKOUTM6
VIDEO2_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_hdmi_pll $HDMI_DPLL_CLK $HDMI_PLL_M $HDMI_PLL_N $HDMI_PLL_M2
HDMI_CLK=$DPLL_HDMI_CLK1
HDMI_DPLL_DCOLDO=$CLK_DCO_LDO

dra7xx_cm_core_aon_b $GPU_GCLK $PER_GPU_CLK $FUNC_256M_CLK $PER_QSPI_CLK $FUNC_192M_CLK $CORE_GPU_CLK $CORE_IPU_ISS_BOOST_CLK $CORE_ISS_MAIN_CLK $L3_ICLK $DPLL_ABE_X2_FCLK $RMII_CLK $GMAC_250M_CLK $GMAC_RMII_HS_CLK $EVE_GFCLK $EVE_GCLK $SYS_32K $PER_ABE_X1_GFCLK $VIDEO1_CLK $VIDEO2_CLK $DPLL_HDMI_CLK1 $CM_CORE_AON_MMC1_GFCLK_MUX $CM_CORE_AON_MMC1_GFCLK_DIV $CM_CORE_AON_MMC2_GFCLK_MUX $CM_CORE_AON_MMC2_GFCLK_DIV $CM_CORE_AON_MMC3_FCLK_MUX $CM_CORE_AON_MMC3_FCLK_DIV $CM_CORE_AON_MMC4_FCLK_MUX $CM_CORE_AON_MMC4_FCLK_DIV $CM_CORE_AON_GPU_HYD_GCLK_MUX $CM_CORE_AON_GPU_CORE_GCLK_MUX $CM_CORE_AON_QSPI_GFCLK_MUX $CM_CORE_AON_QSPI_GFCLK_DIV $CM_CORE_AON_VIP1_GCLK_MUX $CM_CORE_AON_VIP2_GCLK_MUX $CM_CORE_AON_VIP3_GCLK_MUX $CM_CORE_AON_ABE_CLK_DIV $CM_CORE_AON_AESS_FCLK_DIV $CM_CORE_AON_ABE_GICLK_DIV $CM_CORE_AON_IPU1_GFCLK_MUX $CM_CORE_AON_RGMII_50M_CLK_MUX $CM_CORE_AON_ATL_GFCLK_MUX $CM_CORE_AON_ATL_SOURCE1_MUX $CM_CORE_AON_GMAC_RFT_CLK_MUX $CM_CORE_AON_EVE_CLK_MUX $CM_CORE_AON_EVE_DCLK_DIV
#echo $GPU_HYD_GCLK $GPU_CORE_GCLK $FUNC_128M_CLK $FUNC_48M_FCLK $QSPI_GFCLK $MMC1_FCLK $MMC2_FCLK $MMC3_FCLK $MMC4_FCLK $PER_48M_GFCLK $L3INIT_48M_GFCLK $HDMI_PHY_GFCLK $PER_12M_GFCLK $IPU1_GFCLK $VIP1_GCLK $VIP2_GCLK $VIP3_GCLK $ABE_CLK $AESS_FCLK $ABE_GICLK $GMAC_MAIN_CLK $GMII_M_CLK_SINK $RGMII_50M_CLK $RGMII_5M_CLK $EVE_DCLK $EVE_CLK $ATL_GFCLK $GMAC_RFT_CLK

dra7xx_usb_otg_ss_dpll $SYS_CLK1 $USB_OTG_SS_PLL_M $USB_OTG_SS_PLL_N
USB_OTG_SS_DPLL_DCOLDO=$CLK_DCO_LDO
dra7xx_sata_dpll $SYS_CLK1 $SATA_PLL_M $SATA_PLL_N
USB_OTG_SS_DPLL_DCOLDO=$CLK_DCO_LDO
dra7xx_debugss_dpll $SYS_CLK1 $DEBUGSS_PLL_M $DEBUGSS_PLL_N $DEBUGSS_PLL_M6_HSDIV
DEBUGSS_DPLL_DCOLDO=$CLK_DCO_LDO
#echo $USB_OTG_CLKIN $SATA_CLKIN $EMU_CLKIN

dra7xx_cm_core_aon_clkoutmux $CLKOUTMUX0_CLK_MUX $CLKOUTMUX_SYS_CLK1_DCLK_DIV $CLKOUTMUX_SYS_CLK2_DCLK_DIV $CLKOUTMUX_PER_ABE_X1_DCLK_DIV $CLKOUTMUX_MPU_DCLK_DIV $CLKOUTMUX_DSP_DCLK_DIV $CLKOUTMUX_IVA_DCLK_DIV $CLKOUTMUX_GPU_DCLK_DIV $CLKOUTMUX_CORE_DPLL_OUT_DCLK_DIV $CLKOUTMUX_EMIF_PHY_DCLK_DIV $CLKOUTMUX_GMAC_250M_DCLK_DIV $CLKOUTMUX_VIDEO2_DCLK_DIV $CLKOUTMUX_VIDEO1_DCLK_DIV $CLKOUTMUX_HDMI_DCLK_DIV $CLKOUTMUX_FUNC_96M_AON_DCLK_DIV $CLKOUTMUX_L3INIT_480M_DCLK_DIV $CLKOUTMUX_USB_OTG_DCLK_DIV $CLKOUTMUX_SATA_DCLK_DIV $CLKOUTMUX_PCIE2_DCLK_DIV $CLKOUTMUX_PCIE1_DCLK_DIV $CLKOUTMUX_EMU_DCLK_DIV
CLKOUTMUX0_CLK=$CLKOUTMUX_CLK
#echo $CLKOUTMUX0_CLK

dra7xx_cm_core_aon_clkoutmux $CLKOUTMUX1_CLK_MUX $CLKOUTMUX_SYS_CLK1_DCLK_DIV $CLKOUTMUX_SYS_CLK2_DCLK_DIV $CLKOUTMUX_PER_ABE_X1_DCLK_DIV $CLKOUTMUX_MPU_DCLK_DIV $CLKOUTMUX_DSP_DCLK_DIV $CLKOUTMUX_IVA_DCLK_DIV $CLKOUTMUX_GPU_DCLK_DIV $CLKOUTMUX_CORE_DPLL_OUT_DCLK_DIV $CLKOUTMUX_EMIF_PHY_DCLK_DIV $CLKOUTMUX_GMAC_250M_DCLK_DIV $CLKOUTMUX_VIDEO2_DCLK_DIV $CLKOUTMUX_VIDEO1_DCLK_DIV $CLKOUTMUX_HDMI_DCLK_DIV $CLKOUTMUX_FUNC_96M_AON_DCLK_DIV $CLKOUTMUX_L3INIT_480M_DCLK_DIV $CLKOUTMUX_USB_OTG_DCLK_DIV $CLKOUTMUX_SATA_DCLK_DIV $CLKOUTMUX_PCIE2_DCLK_DIV $CLKOUTMUX_PCIE1_DCLK_DIV $CLKOUTMUX_EMU_DCLK_DIV
CLKOUTMUX1_CLK=$CLKOUTMUX_CLK
#echo $CLKOUTMUX1_CLK

dra7xx_cm_core_aon_clkoutmux $CLKOUTMUX2_CLK_MUX $CLKOUTMUX_SYS_CLK1_DCLK_DIV $CLKOUTMUX_SYS_CLK2_DCLK_DIV $CLKOUTMUX_PER_ABE_X1_DCLK_DIV $CLKOUTMUX_MPU_DCLK_DIV $CLKOUTMUX_DSP_DCLK_DIV $CLKOUTMUX_IVA_DCLK_DIV $CLKOUTMUX_GPU_DCLK_DIV $CLKOUTMUX_CORE_DPLL_OUT_DCLK_DIV $CLKOUTMUX_EMIF_PHY_DCLK_DIV $CLKOUTMUX_GMAC_250M_DCLK_DIV $CLKOUTMUX_VIDEO2_DCLK_DIV $CLKOUTMUX_VIDEO1_DCLK_DIV $CLKOUTMUX_HDMI_DCLK_DIV $CLKOUTMUX_FUNC_96M_AON_DCLK_DIV $CLKOUTMUX_L3INIT_480M_DCLK_DIV $CLKOUTMUX_USB_OTG_DCLK_DIV $CLKOUTMUX_SATA_DCLK_DIV $CLKOUTMUX_PCIE2_DCLK_DIV $CLKOUTMUX_PCIE1_DCLK_DIV $CLKOUTMUX_EMU_DCLK_DIV
CLKOUTMUX2_CLK=$CLKOUTMUX_CLK
#echo $CLKOUTMUX2_CLK

dra7xx_cm_core_aon_timerx $TIMER1_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER1_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timerx $TIMER2_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER2_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timerx $TIMER3_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER3_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timerx $TIMER4_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER4_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timerx $TIMER9_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER9_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timerx $TIMER10_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER10_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timerx $TIMER11_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER11_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timerx $TIMER13_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER13_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timerx $TIMER14_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER14_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timerx $TIMER15_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER15_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timerx $TIMER16_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER16_GFCLK=$TIMERx_GFCLK

dra7xx_cm_core_aon_timer1 $TIMER5_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER5_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timer1 $TIMER6_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER6_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timer1 $TIMER7_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER7_GFCLK=$TIMERx_GFCLK
dra7xx_cm_core_aon_timer1 $TIMER8_GFCLK_MUX $TIMER_VIDEO1_CLK_DIV $TIMER_VIDEO2_CLK_DIV $TIMER_HDMI_CLK_DIV
TIMER8_GFCLK=$TIMERx_GFCLK

dra7xx_cm_core_aon_mcasp_aux_gfclk $MCASP1_AUX_GFCLK_MUX $MCASP_PER_ABE_X1_GFCLK_DIV $MCASP_VIDEO1_CLK_DIV $MCASP_VIDEO2_CLK_DIV $MCASP_HDMI_CLK_DIV
MCASP1_AUX_GFCLK=$MCASPx_AUX_GFCLK
dra7xx_cm_core_aon_mcasp_aux_gfclk $MCASP2_AUX_GFCLK_MUX $MCASP_PER_ABE_X1_GFCLK_DIV $MCASP_VIDEO1_CLK_DIV $MCASP_VIDEO2_CLK_DIV $MCASP_HDMI_CLK_DIV
MCASP2_AUX_GFCLK=$MCASPx_AUX_GFCLK
dra7xx_cm_core_aon_mcasp_aux_gfclk $MCASP3_AUX_GFCLK_MUX $MCASP_PER_ABE_X1_GFCLK_DIV $MCASP_VIDEO1_CLK_DIV $MCASP_VIDEO2_CLK_DIV $MCASP_HDMI_CLK_DIV
MCASP3_AUX_GFCLK=$MCASPx_AUX_GFCLK
dra7xx_cm_core_aon_mcasp_aux_gfclk $MCASP4_AUX_GFCLK_MUX $MCASP_PER_ABE_X1_GFCLK_DIV $MCASP_VIDEO1_CLK_DIV $MCASP_VIDEO2_CLK_DIV $MCASP_HDMI_CLK_DIV
MCASP4_AUX_GFCLK=$MCASPx_AUX_GFCLK
dra7xx_cm_core_aon_mcasp_aux_gfclk $MCASP5_AUX_GFCLK_MUX $MCASP_PER_ABE_X1_GFCLK_DIV $MCASP_VIDEO1_CLK_DIV $MCASP_VIDEO2_CLK_DIV $MCASP_HDMI_CLK_DIV
MCASP5_AUX_GFCLK=$MCASPx_AUX_GFCLK
dra7xx_cm_core_aon_mcasp_aux_gfclk $MCASP6_AUX_GFCLK_MUX $MCASP_PER_ABE_X1_GFCLK_DIV $MCASP_VIDEO1_CLK_DIV $MCASP_VIDEO2_CLK_DIV $MCASP_HDMI_CLK_DIV
MCASP6_AUX_GFCLK=$MCASPx_AUX_GFCLK
dra7xx_cm_core_aon_mcasp_aux_gfclk $MCASP7_AUX_GFCLK_MUX $MCASP_PER_ABE_X1_GFCLK_DIV $MCASP_VIDEO1_CLK_DIV $MCASP_VIDEO2_CLK_DIV $MCASP_HDMI_CLK_DIV
MCASP7_AUX_GFCLK=$MCASPx_AUX_GFCLK
dra7xx_cm_core_aon_mcasp_aux_gfclk $MCASP8_AUX_GFCLK_MUX $MCASP_PER_ABE_X1_GFCLK_DIV $MCASP_VIDEO1_CLK_DIV $MCASP_VIDEO2_CLK_DIV $MCASP_HDMI_CLK_DIV
MCASP8_AUX_GFCLK=$MCASPx_AUX_GFCLK

dra7xx_cm_core_aon_mcasp_ahclkrx $MCASP1_AHCLKX_MUX $MCASP_MLB_CLK_DIV $MCASP_MLBP_CLK_DIV
MCASP1_AHCLKX=$MCASPx_AHCLKx
dra7xx_cm_core_aon_mcasp_ahclkrx $MCASP1_AHCLKR_MUX $MCASP_MLB_CLK_DIV $MCASP_MLBP_CLK_DIV
MCASP1_AHCLKR=$MCASPx_AHCLKx
dra7xx_cm_core_aon_mcasp_ahclkrx $MCASP2_AHCLKX_MUX $MCASP_MLB_CLK_DIV $MCASP_MLBP_CLK_DIV
MCASP2_AHCLKX=$MCASPx_AHCLKx
dra7xx_cm_core_aon_mcasp_ahclkrx $MCASP2_AHCLKR_MUX $MCASP_MLB_CLK_DIV $MCASP_MLBP_CLK_DIV
MCASP2_AHCLKR=$MCASPx_AHCLKx

dra7xx_cm_core_aon_mcasp_ahclkrx $MCASP3_AHCLKX_MUX $MCASP_MLB_CLK_DIV $MCASP_MLBP_CLK_DIV
MCASP3_AHCLKX=$MCASPx_AHCLKx
dra7xx_cm_core_aon_mcasp_ahclkrx $MCASP4_AHCLKX_MUX $MCASP_MLB_CLK_DIV $MCASP_MLBP_CLK_DIV
MCASP4_AHCLKX=$MCASPx_AHCLKx
dra7xx_cm_core_aon_mcasp_ahclkrx $MCASP5_AHCLKX_MUX $MCASP_MLB_CLK_DIV $MCASP_MLBP_CLK_DIV
MCASP5_AHCLKX=$MCASPx_AHCLKx
dra7xx_cm_core_aon_mcasp_ahclkrx $MCASP6_AHCLKX_MUX $MCASP_MLB_CLK_DIV $MCASP_MLBP_CLK_DIV
MCASP6_AHCLKX=$MCASPx_AHCLKx
dra7xx_cm_core_aon_mcasp_ahclkrx $MCASP7_AHCLKX_MUX $MCASP_MLB_CLK_DIV $MCASP_MLBP_CLK_DIV
MCASP7_AHCLKX=$MCASPx_AHCLKx
dra7xx_cm_core_aon_mcasp_ahclkrx $MCASP8_AHCLKX_MUX $MCASP_MLB_CLK_DIV $MCASP_MLBP_CLK_DIV
MCASP8_AHCLKX=$MCASPx_AHCLKx

clk_src=$SYS_CLK1

#test_print_trc "Processing external clks"
add_to_tree atl_clkin0_ck $ATL_CLK0 
add_to_tree atl_clkin1_ck $ATL_CLK1
add_to_tree atl_clkin2_ck $ATL_CLK2
add_to_tree atl_clkin3_ck $ATL_CLK3
add_to_tree hdmi_clkin_ck $HDMI_CLK
add_to_tree mlb_clkin_ck $MLB_CLK
add_to_tree mlb_clkin_ck/mlb_clk $MLB_CLK
add_to_tree mlbp_clkin_ck $MLBP_CLK
add_to_tree mlbp_clkin_ck/mlbp_clk $MLBP_CLK
add_to_tree pciesref_acs_clk_ck $PCIESREF_ACS_CLK 
add_to_tree ref_clkin0_ck $XREF_CLK0
add_to_tree ref_clkin1_ck $XREF_CLK1
add_to_tree ref_clkin2_ck $XREF_CLK2
add_to_tree ref_clkin3_ck $XREF_CLK3
add_to_tree rmii_clk_ck $RMII_CLK
add_to_tree sdvenc_clkin_ck 0
#test_print_trc "Processing 32K clk"
add_to_tree secure_32k_clk_src_ck $SYS_32K
add_to_tree secure_32k_clk_src_ck/secure_32k_dclk_div $SYS_32K
add_to_tree sys_32k_ck $SYS_32K
add_to_tree sys_32k_ck/dss_32khz_clk $SYS_32K 
add_to_tree sys_32k_ck/gpio1_dbclk $SYS_32K
add_to_tree sys_32k_ck/gpio2_dbclk $SYS_32K
add_to_tree sys_32k_ck/gpio3_dbclk $SYS_32K
add_to_tree sys_32k_ck/gpio4_dbclk $SYS_32K
add_to_tree sys_32k_ck/gpio5_dbclk $SYS_32K
add_to_tree sys_32k_ck/gpio6_dbclk $SYS_32K
add_to_tree sys_32k_ck/gpio7_dbclk $SYS_32K
add_to_tree sys_32k_ck/gpio8_dbclk $SYS_32K
add_to_tree sys_32k_ck/mmc1_clk32k $SYS_32K
add_to_tree sys_32k_ck/mmc2_clk32k $SYS_32K
add_to_tree sys_32k_ck/mmc3_clk32k $SYS_32K
add_to_tree sys_32k_ck/mmc4_clk32k $SYS_32K
add_to_tree sys_32k_ck/usb_phy1_always_on_clk32k $SYS_32K
add_to_tree sys_32k_ck/usb_phy2_always_on_clk32k $SYS_32K
add_to_tree sys_32k_ck/usb_phy3_always_on_clk32k $SYS_32K
add_to_tree sys_32k_ck/atl_dpll_clk_mux $SYS_32K
add_to_tree sys_32k_ck/timer1_gfclk_mux $SYS_32K
add_to_tree virt_12000000_ck 12000000
#add_to_tree virt_13000000_ck 13000000
add_to_tree virt_16800000_ck 16800000
add_to_tree virt_19200000_ck 19200000
add_to_tree virt_20000000_ck 20000000
#test_print_trc "Processing PRM clks"
add_to_tree virt_${clk_src}_ck/sys_clkin1 $clk_src
add_to_tree virt_${clk_src}_ck/sys_clkin1/abe_sys_clk_div $ABE_SYS_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/abe_gfclk_mux $ABE_DPLL_SYS_CLK
#test_print_trc "Processing PCIE REF DPLL"
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_pcie_ref_ck $((PCIE_REF_DPLL_DCOLDO/2)) 
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_pcie_ref_ck/dpll_pcie_ref_m2ldo_ck $PCIE_REF_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_pcie_ref_ck/dpll_pcie_ref_m2_ck $PCIE_M2_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_pcie_ref_ck/dpll_pcie_ref_m2_ck/pcie2_dclk_div $((PCIE_M2_CLK/(2**$CLKOUTMUX_PCIE2_DCLK_DIV)))
#test_print_trc "Processing CLKOUTMUXs"
add_to_tree virt_${clk_src}_ck/sys_clkin1/sys_clk1_dclk_div $((SYS_CLK1/(2**CLKOUTMUX_SYS_CLK1_DCLK_DIV)))
add_to_tree virt_${clk_src}_ck/sys_clkin1/sys_clk1_dclk_div/clkoutmux0_clk_mux $CLKOUTMUX0_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/sys_clk1_dclk_div/clkoutmux1_clk_mux $CLKOUTMUX1_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/sys_clk1_dclk_div/clkoutmux2_clk_mux $CLKOUTMUX2_CLK
#test_print_trc "Processing CORE DPLL"
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck $((CORE_DPLL_DCOLDO/2))
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck $CORE_DPLL_DCOLDO
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck $CORE_X2_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/mpu_dpll_hs_clk_div $MPU_DPLL_HS_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/dsp_dpll_hs_clk_div $DSP_DPLL_HS_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/iva_dpll_hs_clk_div $IVA_DPLL_HS_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/eve_dpll_hs_clk_div $EVE_DPLL_HS_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/l3_iclk_div $L3_ICLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/l3_iclk_div/l4_root_clk_div $L4_ROOT_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/l3_iclk_div/atl_gfclk_mux $ATL_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/l3_iclk_div/gmac_rft_clk_mux $GMAC_RFT_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/l3_iclk_div/vip1_gclk_mux $VIP1_GCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/l3_iclk_div/vip2_gclk_mux $VIP2_GCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h12x2_ck/l3_iclk_div/vip3_gclk_mux $VIP3_GCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h13x2_ck $CORE_USB_OTG_SS_LFPS_TX_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h14x2_ck $CORE_GPU_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h14x2_ck/gpu_core_gclk_mux $GPU_CORE_GCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h14x2_ck/gpu_hyd_gclk_mux $GPU_HYD_GCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h22x2_ck $CORE_IPU_ISS_BOOST_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h23x2_ck $CORE_ISS_MAIN_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_x2_ck/dpll_core_h24x2_ck $BB2D_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_m2_ck $CORE_DPLL_OUT_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_core_ck/dpll_core_m2_ck/core_dpll_out_dclk_div $((CORE_DPLL_OUT_CLK/(2**CLKOUTMUX_CORE_DPLL_OUT_DCLK_DIV)))
#test_print_trc "Processing MPU DPLL"
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_mpu_ck $((MPU_DPLL_DCOLDO/2))
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_mpu_ck/dpll_mpu_m2_ck $MPU_GCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_mpu_ck/dpll_mpu_m2_ck/mpu_dclk_div $((MPU_GCLK/(2**CLKOUTMUX_MPU_DCLK_DIV)))
#test_print_trc "Processing DSP DPLL"
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_dsp_ck $((DSP_DPLL_DCOLDO/2))
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_dsp_ck/dpll_dsp_m2_ck $DSP_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_dsp_ck/dpll_dsp_m2_ck/dsp_gclk_div $((DSP_GFCLK/(2**CLKOUTMUX_DSP_DCLK_DIV)))
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_dsp_ck/dpll_dsp_x2_ck $DSP_DPLL_DCOLDO
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_dsp_ck/dpll_dsp_x2_ck/dpll_dsp_m3x2_ck $EVE_GCLK
#test_print_trc "Processing IVA DPLL"
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_iva_ck $((IVA_DPLL_DCOLDO/2))
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_iva_ck/dpll_iva_m2_ck $IVA_GCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_iva_ck/dpll_iva_m2_ck/iva_dclk $((IVA_GCLK/(2**CLKOUTMUX_IVA_DCLK_DIV)))
#test_print_trc "Processing GPU DPLL"
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gpu_ck $((GPU_DPLL_DCOLDO/2))
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gpu_ck/dpll_gpu_m2_ck $GPU_GCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gpu_ck/dpll_gpu_m2_ck/gpu_dclk $((GPU_CLK/(2**CLKOUTMUX_GPU_DCLK_DIV)))
#test_print_trc "Processing DDR DPLL"
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_ddr_ck $((DDR_DPLL_DCOLDO/2))
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_ddr_ck/dpll_ddr_m2_ck $EMIF_PHY_GCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_ddr_ck/dpll_ddr_m2_ck/emif_phy_dclk_div $((EMIF_PHY_GCLK/(2**CLKOUTMUX_EMIF_PHY_DCLK_DIV)))
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_ddr_ck/dpll_ddr_x2_ck $DDR_DPLL_DCOLDO
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_ddr_ck/dpll_ddr_x2_ck/dpll_ddr_h11x2_ck $EMIF_DLL_GCLK
#test_print_trc "Processing PER DPLL"
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck $((PER_DPLL_DCOLDO/2))
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_m2_ck $FUNC_96M_AON_CLK 
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_m2_ck/func_96m_aon_dclk_div $((FUNC_96M_AON_CLK/(2**CLKOUTMUX_FUNC_96M_AON_DCLK_DIV)))
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_m2_ck/func_24m_clk $FUNC_24M_GFCLK 
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck $PER_DPLL_DCOLDO
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_h11x2_ck $FUNC_256M_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_h11x2_ck/func_128m_clk $FUNC_128M_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_h12x2_ck $DSS_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_h12x2_ck/dss_dss_clk $DSS_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_h13x2_ck $PER_QSPI_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_h13x2_ck/qspi_gfclk_mux $PER_QSPI_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_h13x2_ck/qspi_gfclk_mux/qspi_gfclk_div $QSPI_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_h14x2_ck $PER_GPU_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck $FUNC_192M_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_12m_fclk $PER_12M_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk $FUNC_48M_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/dss_48mhz_clk $HDMI_PHY_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/mmc3_gfclk_mux $FUNC_48M_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/mmc3_gfclk_mux/mmc3_gfclk_div $MMC3_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/mmc4_gfclk_mux $FUNC_48M_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/mmc4_gfclk_mux/mmc4_gfclk_div $MMC4_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/uart10_gfclk_mux $FUNC_48M_FCLK 
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/uart1_gfclk_mux $FUNC_48M_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/uart2_gfclk_mux $FUNC_48M_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/uart3_gfclk_mux $FUNC_48M_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/uart4_gfclk_mux $FUNC_48M_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/uart5_gfclk_mux $FUNC_48M_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/uart6_gfclk_mux $FUNC_48M_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/uart7_gfclk_mux $FUNC_48M_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/uart8_gfclk_mux $FUNC_48M_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_48m_fclk/uart9_gfclk_mux $FUNC_48M_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/func_96m_fclk $FUNC_96M_AON_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/mmc1_fclk_mux $FUNC_128M_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/mmc1_fclk_mux/mmc1_fclk_div $MMC1_FCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/mmc2_fclk_mux $FUNC_128M_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_per_ck/dpll_per_x2_ck/dpll_per_m2x2_ck/mmc2_fclk_mux/mmc2_fclk_div $MMC2_FCLK
#test_print_trc "Processing USB DPLL"
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_usb_ck $((USB_DPLL_DCOLDO/2)) 
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_usb_ck/dpll_usb_m2_ck $L3INIT_480M_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_usb_ck/dpll_usb_m2_ck/l3init_480m_dclk_div $((L3INIT_480M_GFCLK/(2**CLKOUTMUX_L3INIT_480M_DCLK_DIV)))
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_usb_ck/dpll_usb_m2_ck/l3init_60m_fclk 60000000 #$L3INIT_60M_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_usb_ck/dpll_usb_clkdcoldo $USB_DPLL_DCOLDO
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_usb_ck/dpll_usb_clkdcoldo/usb_otg_ss1_refclk960m $L3INIT_960M_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_usb_ck/dpll_usb_clkdcoldo/usb_otg_ss2_refclk960m $L3INIT_960M_GFCLK
#test_print_trc "Processing SATA and EMU"
add_to_tree virt_${clk_src}_ck/sys_clkin1/sata_dclk_div $((SATA_CLKIN/(2**CLKOUTMUX_SATA_DCLK_DIV)))
add_to_tree virt_${clk_src}_ck/sys_clkin1/emu_dclk_div $((EMU_CLKIN/(2**CLKOUTMUX_SATA_DCLK_DIV)))
#test_print_trc "Processing EVE DPLL"
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_eve_ck $((EVE_DPLL_DCOLDO/2))
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_eve_ck/dpll_eve_m2_ck $EVE_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_eve_ck/dpll_eve_m2_ck/eve_dclk_div $EVE_DCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_eve_ck/dpll_eve_m2_ck/eve_clk $EVE_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/custefuse_sys_gfclk_div $CUSTEFUSE_SYS_GFCLK
#test_print_trc "Processing HDMI DPLL"
add_to_tree virt_${clk_src}_ck/sys_clkin1/hdmi_dpll_clk_mux $SYS_CLK1 
add_to_tree virt_${clk_src}_ck/sys_clkin1/hdmi_dpll_clk_mux/dss_hdmi_clk $HDMI_DPLL_CLK
#test_print_trc "Processing TIMERs"
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div $TIMER_SYS_CLK 
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer10_gfclk_mux $TIMER10_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer11_gfclk_mux $TIMER11_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer13_gfclk_mux $TIMER13_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer14_gfclk_mux $TIMER14_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer15_gfclk_mux $TIMER15_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer16_gfclk_mux $TIMER16_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer2_gfclk_mux $TIMER2_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer3_gfclk_mux $TIMER3_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer4_gfclk_mux $TIMER4_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer5_gfclk_mux $TIMER5_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer6_gfclk_mux $TIMER6_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer7_gfclk_mux $TIMER7_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer8_gfclk_mux $TIMER8_GFCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/timer_sys_clk_div/timer9_gfclk_mux $TIMER9_GFCLK
#test_print_trc "Processing VIDEO DPLLs"
add_to_tree virt_${clk_src}_ck/sys_clkin1/video1_dpll_clk_mux $VIDEO1_DPLL_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/video1_dpll_clk_mux/dss_video1_clk $VIDEO1_DPLL_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/video2_dpll_clk_mux $VIDEO2_DPLL_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/video2_dpll_clk_mux/dss_video2_clk $VIDEO2_DPLL_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/wkupaon_iclk_mux $WKUPAON_ICLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/wkupaon_iclk_mux/l3instr_ts_gclk_div $L3INSTR_TS_GCLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/sata_ref_clk $SYS_CLK1
add_to_tree virt_${clk_src}_ck/sys_clkin1/dcan1_sys_clk_mux $SYS_CLK1
#test_print_trc "Processing GMAC DPLL"
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gmac_ck $((GMAC_DPLL_DCOLDO/2)) 
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gmac_ck/dpll_gmac_m2_ck $GMAC_250M_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gmac_ck/dpll_gmac_m2_ck/gmac_250m_dclk_div $GMAC_MAIN_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gmac_ck/dpll_gmac_m2_ck/gmac_gmii_ref_clk_div $GMAC_MAIN_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gmac_ck/dpll_gmac_x2_ck $GMAC_DPLL_DCOLDO
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gmac_ck/dpll_gmac_x2_ck/dpll_gmac_h11x2_ck $GMAC_RMII_HS_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gmac_ck/dpll_gmac_x2_ck/dpll_gmac_h11x2_ck/gmii_m_clk_div $GMII_M_CLK_SINK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gmac_ck/dpll_gmac_x2_ck/dpll_gmac_h12x2_ck $GMII_250MHZ_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gmac_ck/dpll_gmac_x2_ck/dpll_gmac_h13x2_ck $ICSS_CLK
add_to_tree virt_${clk_src}_ck/sys_clkin1/dpll_gmac_ck/dpll_gmac_x2_ck/dpll_gmac_m3x2_ck $ICSS_IEP_CLK
add_to_tree virt_26000000_ck 26000000
#add_to_tree virt_27000000_ck 27000000
add_to_tree virt_38400000_ck 38400000
add_to_tree sys_clkin2 $SYS_CLK2
add_to_tree sys_clkin2/sys_clk2_dclk_div $((SYS_CLK2/2**CLKOUTMUX_SYS_CLK2_DCLK_DIV))
#test_print_trc "Processing ABE DPLL"
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux $ABE_DPLL_SYS_CLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_bypass_clk_mux $ABE_DPLL_BYPASS_CLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux $ABE_DPLL_CLK 
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck $((ABE_DPLL_DCOLDO/2))
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck $ABE_DPLL_DCOLDO
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck $DPLL_ABE_X2_FCLK 
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk $ABE_24M_GFCLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcasp1_ahclkr_mux $MCASP1_AHCLKR 
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcasp1_ahclkx_mux $MCASP1_AHCLKX
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcasp2_ahclkr_mux $MCASP2_AHCLKR
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcasp2_ahclkx_mux $MCASP2_AHCLKX
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcasp3_ahclkx_mux $MCASP3_AHCLKX
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcasp4_ahclkx_mux $MCASP4_AHCLKX
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcasp5_ahclkx_mux $MCASP5_AHCLKX
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcasp6_ahclkx_mux $MCASP6_AHCLKX
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcasp7_ahclkx_mux $MCASP7_AHCLKX
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_24m_fclk/mcasp8_ahclk_mux $MCASP8_AHCLKX
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_clk $ABE_CLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_clk/aess_fclk $AESS_FCLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_clk/aess_fclk/abe_giclk_div $ABE_GICLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/abe_lp_clk_div $ABE_LP_CLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m2x2_ck/ipu1_gfclk_mux $IPU1_GFCLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m3x2_ck $CORE_DPLL_HS_CLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m3x2_ck/per_dpll_hs_clk_div $PER_DPLL_HS_CLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_x2_ck/dpll_abe_m3x2_ck/usb_dpll_hs_clk_div $USB_DPLL_HS_CLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_m2_ck $PER_ABE_X1_GFCLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_m2_ck/per_abe_x1_dclk_div $((PER_ABE_X1_GFCLK/(2**CLKOUTMUX_PER_ABE_X1_DCLK_DIV)))
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_m2_ck/per_abe_x1_gfclk2_div $((PER_ABE_X1_GFCLK/(2**MCASP_PER_ABE_X1_GFCLK_DIV)))
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_m2_ck/per_abe_x1_gfclk2_div/mcasp1_aux_gfclk_mux $MCASP1_AUX_GFCLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_m2_ck/per_abe_x1_gfclk2_div/mcasp2_aux_gfclk_mux $MCASP2_AUX_GFCLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_m2_ck/per_abe_x1_gfclk2_div/mcasp3_aux_gfclk_mux $MCASP3_AUX_GFCLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_m2_ck/per_abe_x1_gfclk2_div/mcasp4_aux_gfclk_mux $MCASP4_AUX_GFCLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_m2_ck/per_abe_x1_gfclk2_div/mcasp5_aux_gfclk_mux $MCASP5_AUX_GFCLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_m2_ck/per_abe_x1_gfclk2_div/mcasp6_aux_gfclk_mux $MCASP6_AUX_GFCLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_m2_ck/per_abe_x1_gfclk2_div/mcasp7_aux_gfclk_mux $MCASP7_AUX_GFCLK
add_to_tree sys_clkin2/abe_dpll_sys_clk_mux/abe_dpll_clk_mux/dpll_abe_ck/dpll_abe_m2_ck/per_abe_x1_gfclk2_div/mcasp8_aux_gfclk_mux $MCASP8_AUX_GFCLK
#test_print_trc "Processing other clks"
add_to_tree usb_otg_clkin_ck $((USB_OTG_SS_DPLL_DCOLDO/2)) 
add_to_tree usb_otg_clkin_ck/usb_otg_dclk_div $((USB_OTG_SS_DPLL_DCOLDO/(2**CLKOUTMUX_USB_OTG_DCLK_DIV)))
add_to_tree video1_clkin_ck $((VIDEO1_DPLL_DCOLDO/2))
add_to_tree video1_m2_clkin_ck $((VIDEO1_DPLL_DCOLDO/32)) 
add_to_tree video2_clkin_ck $((VIDEO2_DPLL_DCOLDO/2))
add_to_tree video2_m2_clkin_ck $((VIDEO2_DPLL_DCOLDO/10))
#add_to_tree orphans
add_to_tree orphans/apll_pcie_ck $((PCIE_PHY_CLK/2))
add_to_tree orphans/apll_pcie_ck/apll_pcie_clkvcoldo $PCIE_PHY_CLK
add_to_tree orphans/apll_pcie_ck/apll_pcie_clkvcoldo_div $PCIE_DIV_GCLK
add_to_tree orphans/apll_pcie_ck/apll_pcie_m2_ck $APLL_PCIE_M2_CLK
add_to_tree orphans/apll_pcie_ck/apll_pcie_m2_ck/pcie_dclk_div $((APLL_PCIE_M2_CLK/(2**CLKOUTMUX_PCIE1_DCLK_DIV)))
add_to_tree orphans/video2_dclk_div $((VIDEO2_CLK/(2**CLKOUTMUX_VIDEO2_DCLK_DIV)))
add_to_tree orphans/video1_dclk_div $((VIDEO1_CLK/(2**CLKOUTMUX_VIDEO1_DCLK_DIV)))
add_to_tree orphans/hdmi_dclk_div  $((HDMI_CLK/(2**CLKOUTMUX_HDMI_DCLK_DIV)))
add_to_tree orphans/hdmi_clk2_div $((HDMI_CLK/(2**MCASP_HDMI_CLK_DIV)))
add_to_tree orphans/hdmi_div_clk $((HDMI_CLK/(2**TIMER_HDMI_CLK_DIV)))
add_to_tree orphans/video1_clk2_div $((VIDEO1_CLK/(2**MCASP_VIDEO1_CLK_DIV)))
add_to_tree orphans/video1_div_clk $((VIDEO1_CLK/(2**TIMER_VIDEO1_CLK_DIV)))
add_to_tree orphans/video2_clk2_div $((VIDEO2_CLK/(2**MCASP_VIDEO2_CLK_DIV)))
add_to_tree orphans/video2_div_clk $((VIDEO2_CLK/(2**TIMER_VIDEO2_CLK_DIV)))
add_to_tree orphans/dummy_clk 0

echo ${dra7xx_CLK_TREE[@]}
