#!/bin/bash

source clk_framework_utils.sh

#Function to obtain the clocks controlled by the PRM, takes the following
#parameters:
#    $1: ABE_LP_CLK clock in Hz
#    $2: SYS_CLK clock in Hz
#    $3: FUNC_SYS_32K clock in Hz
#    $4: WKUPAON_ICLK Mux. CM_CLKSEL_WKUPAON[0] CLKSEL
#    $5: ABE_SYS_CLK and DSS_SYS_CLK Div. CM_CLKSEL_ABE_DSS_SYS[0]CLKSEL
#    $6: MPU_DPLL_CLK_ABE Div. CM_MPU_MPU_CLKCTRL[26]CLKSEL_ABE_DIV_MODE
#    $7: MPU_DPLL_CLK_EMIF Div. CM_MPU_MPU_CLKCTRL[25:24]CLKSEL_EMIF_DIV_MODE
#    $8: L3INSTR_TS_GCLK and L3INSTR_DLL_AGING_CLK Div. CM_L3INSTR_CTRL_MODULE_BANDGAP_CLKCTRL[25:24]CLKSEL
#Provides the following clock values in Hz:
#    ABE_SYS_CLK WKUPAON_ICLK L3INSTR_TS_GCLK L3INSTR_DLL_AGING_GCLK CUSTEFUSE_SYS_GFCLK DSS_SYS_GFCLK MPU_DPLL_CLK_EMIF MPU_DPLL_CLK_ABE
# The following clocks are gated by PRM but the source of the clk is not modified; hence, they are not provided by this function. The list is displayed as <source (clk list)>:
#    WKUPAON_ICLK (SR_MM_SYS_GFLK, SR_MPU_SYS_GFCLK, SR_CORE_SYS_GFCLK,
#                  WKUPAON_GCLK, WKUPAON_GICLK, CM_CORE_AON_SYS_CLK, 
#                  WKUPAON_PROFILING_GCLK)
#
#    SYS_CLK      (CORE_DPLL_CLK, PER_DPLL_CLK, DSP_DPLL_CLK, MPU_DPLL_CLK, 
#                  USB_DPLL_CLK, USB_OTG_SS_REF_CLK, SATA_REF_GFCLK,
#                  COREAON_IO_SRCOMP_GFCLK, WKUPAON_IO_SRCOMP_GFCLK, 
#                  EMU_SYS_GCLK)
#   
#    FUNC_SYS_32K (COREAON_32K_GFCLK, PER_32K_GFCLK, ABE_32K_CLK, 
#                  HDMI_CEC_GFCLK, MMC1_32K_GFCLK)
omap5_prm() {
  params_check $# 8
  local abe_lp_clk=$1
  local sys_clk=$2
  local func_sys_32k=$3
  local mux_wkupaon_iclk=$4
  local div_abe_sys_clk=$(($5+1))
  local div_mpu_dpll_clk_abe=$((($6+1)*8))
  local div_mpu_dpll_clk_emif=4
  if [ $7 -gt 1 ]
  then
    div_mpu_dpll_clk_emif=8
  fi
  local div_l3instr_ts_gclk_l3instr_dll_aging_gclk=$((2**($8+3)))
 
  if [ $mux_wkupaon_iclk -eq 1 ]
  then
    WKUPAON_ICLK=$abe_lp_clk
  else
    WKUPAON_ICLK=$sys_clk
  fi
  #This code was moved to omap_abe_dpll_input_clks
  #if [ $mux_abe_dpll_clk -eq 1 ]
  #then
    #ABE_DPLL_CLK=$func_sys_32k
  #else
    #ABE_DPLL_CLK=$sys_clk
  #fi
  #if [ $mux_abe_dpll_bypass_clk -eq 1 ]
  #then
    #ABE_DPLL_BYPASS_CLK=$func_sys_32k
  #else
    #ABE_DPLL_BYPASS_CLK=$sys_clk
  #fi
  MPU_DPLL_CLK=$sys_clk
  ABE_SYS_CLK=$((sys_clk/div_abe_sys_clk))
  DSS_SYS_GFCLK=$ABE_SYS_CLK
  MPU_DPLL_CLK_ABE=$((MPU_DPLL_CLK/div_mpu_dpll_clk_abe))
  MPU_DPLL_CLK_EMIF=$((MPU_DPLL_CLK/div_mpu_dpll_clk_emif))
  L3INSTR_TS_GCLK=$(($WKUPAON_ICLK/div_l3instr_ts_gclk_l3instr_dll_aging_gclk))
  L3INSTR_DLL_AGING_GCLK=$((WKUPAON_ICLK/div_l3instr_ts_gclk_l3instr_dll_aging_gclk))
  CUSTEFUSE_SYS_GFCLK=$((sys_clk1/2))
}

#Function to define the input clks to te ABE DPLL, it was place separately in
#order to start the clk definitons with the ABE DPLL which feeds the PRM, but
#whose input clks are controlled by PRM
#Takes the following parameters:
#    $1: SYS_CLK clock in Hz
#    $2: FUNC_SYS_32K clock in Hz
#    $3: ABE_DPLL_CLK Mux. CM_CLKSEL_ABE_PLL_REF[0] CLKSEL
#    $4: ABE_DPLL_BYPASS_CLK Mux. CM_CLKSEL_ABE_PLL_BYPAS[0] CLKSEL
#Provides the following clock values in Hz:
#    ABE_DPLL_CLK ABE_DPLL_BYPASS_CLK
omap5_abe_dpll_input_clks() {
  params_check $# 4
  local sys_clk=$1
  local func_sys_32k=$2
  local mux_abe_dpll_clk=$3
  local mux_abe_dpll_bypass_clk=$4

  if [ $mux_abe_dpll_clk -eq 1 ]
  then
    ABE_DPLL_CLK=$func_sys_32k
  else
    ABE_DPLL_CLK=$sys_clk
  fi
  if [ $mux_abe_dpll_bypass_clk -eq 1 ]
  then
    ABE_DPLL_BYPASS_CLK=$func_sys_32k
  else
    ABE_DPLL_BYPASS_CLK=$sys_clk
  fi
}

#Function to obtain the clocks controlled by the CM_CORE_AON, takes the 
#following parameters:
#    $1: DPLL Core CORE_X2_CLK in Hz
#    $2: DPLL ABE CORE_DPLL_HS_CLK in Hz
#    $3: DPLL PER FUNC_96M_AON_CLK in Hz
#    $4: Divider L3_ICLK CM_CLKSEL_CORE[4] CLKSEL_L3
#    $5: Divider L4_ROOT_CLK CM_CLKSEL_CORE[8] CLKSEL_L4
#    $6: Divider MPU_DPLL_HS_CLK CM_BYPCLK_DPLL_MPU[1:0] CLKSEL
#    $7: Divider IVA_DPLL_HS_CLK CM_BYPCLK_DPLL_IVA[1:0] CLKSEL
#Provides the following clock values in Hz:
#    L3_ICLK CM_CORE_AON_PROFILING_L3_GICLK L4_ROOT_CLK L4_ICLK CM_CORE_AON_PROFILING_L4_GICLK MPU_DPLL_HS_CLK IVA_DPLL_HS_CLK PER_DPLL_HS_CLK USB_DPLL_HS_CLK FUNC_24M_GFCLK 
omap5_cm_core_aon() {
  params_check $# 7
  local dpll_core_x2_clk=$1
  local core_dpll_hs_clk=$2
  local func_96m_aon_clk=$3
  local div_l3_iclk=$(($4+1))
  local div_l4_root_clk=$(($5+1))
  local div_mpu_dpll_hs_clk=$((2**$6))
  local div_iva_dpll_hs_clk=$((2**$7))

  L3_ICLK=$((dpll_core_x2_clk/div_l3_iclk))
  CM_CORE_AON_PROFILING_L3_GICLK=$L3_ICLK
  L4_ROOT_CLK=$((L3_ICLK/div_l4_root_clk))
  L4_ICLK=$L4_ROOT_CLK
  CM_CORE_AON_PROFILING_L4_GICLK=$L4_ROOT_CLK
  MPU_DPLL_HS_CLK=$((dpll_core_x2_clk/div_mpu_dpll_hs_clk))
  IVA_DPLL_HS_CLK=$((dpll_core_x2_clk/div_iva_dpll_hs_clk))
  PER_DPLL_HS_CLK=$((core_dpll_hs_clk/2))
  USB_DPLL_HS_CLK=$((core_dpll_hs_clk/3))
  FUNC_24M_GFCLK=$((func_96m_aon_clk/4))
} 

#Function to obtain the clocks controlled by the CM_CORE_AON, takes the 
#following parameters:
#    $1: DPLL USB L3INIT_480M_GFCLK in Hz
#    $2: XCLK_60M_HSP1 in Hz
#    $3: XCLK_60M_HSP2 in Hz
#    $4: Divider L3_INIT_60M_FCLK CM_CLKSEL_USB_60MHZ[0] CLKSEL
#    $5: Mux UTMI_P1_GFCLK CM_L3INIT_USB_HOST_HS_CLKCTRL[24] CLKSEL_UTMI_P1
#    $6: Mux UTMI_P2_GFCLK CM_L3INIT_USB_HOST_HS_CLKCTRL[25] CLKSEL_UTMI_P2
#Provides the following clock values in Hz:
#    UTMI_P1_GFCLK UTMI_P2_GFCLK L3INIT_60M_FCLK
# The following clocks are gated by PRM but the source of the clk is not modified; hence, they are not provided by this function. The list is displayed as <source (clk list)>:
#    L3INIT_60M_FCLK (UTMI_P3_GFCLK, HSIC_P3_GFCLK, HSIC_P2_GFCLK, 
#                     HSIC_P1_GFCLK, L3INIT_60M_P1_GFCLK, L3INIT_60M_P2_GFCLK
#                     UTMI_ROOT_GFCLK, TLL_CH0_GFCLK, TLL_CH1_GFCLK, 
#                     TLL_CH2_GFCLK)
# 
#    L3INIT_480M_GFCLK (HSIC_P1_480M_GFCLK, HSIC_P2_480_GFCLK, 
#                       HSIC_P3_480M_GFCLK)
omap5_ckgen_usb() {
  params_check $# 6
  local l3init_480m_gfclk=$1
  local xclk_60m_hsp1=$2
  local xclk_60m_hsp2=$3
  local div_l3init_60m_fclk=$((8**$4))
  local mux_utmi_p1_gfclk=$5
  local mux_utmi_p2_gfclk=$6
  L3INIT_60M_FCLK=$((l3init_480m_gfclk/div_l3init_60m_fclk))
  if [ $mux_utmi_p1_gfclk -eq 1 ]
  then
    UTMI_P1_GFCLK=$xclk_60m_hsp1
  else
    UTMI_P1_GFCLK=$L3INIT_60M_FCLK
  fi
  if [ $mux_utmi_p2_gfclk -eq 1 ]
  then
    UTMI_P2_GFCLK=$xclk_60m_hsp2
  else
    UTMI_P2_GFCLK=$L3INIT_60M_FCLK
  fi
}


#Function used by omap5_ckgen_abe() to resolve the GFCLK of MCBSP1-3, MCASP, and
#DMIC, takes the following parameters as input:
#    $1: Mux GFCLK selection 
#    $2: Mux internal function clock selection
#    $3: ABE_24M_FCLK in Hz
#    $4: ABE_SYS_CLK in Hz
#    $5: FUNC_24M_GFCLK in Hz
#    $6: PADS_UCLKS in Hz
#    $7: SLIMBUS_UCLKS in Hz
#Provides the GFCLK for the MCxSP or DMIC specified as MCxSP_DMIC_GFCLK in Hz
omap5_mcxsp_dmic_clk() {
   params_check $# 7
   local gfclk_sel=$1
   local int_clk_sel=$2
   local int_mux_clks=( ${@:3:4} )
   local internal_func_clk=${int_mux_clks[$int_clk_sel]}
   local gfclks=( $internal_func_clk ${@:6} )
   MCxSP_DMIC_GFCLK=${gfclks[$gfclk_sel]}
}

#Function to obtain the clocks controlled by the CKGEN_ABE, takes the 
#following parameters:
#    $1: DPLL_ABE_X2_CLK in Hz
#    $2: FUNC_24M_GFCLK in Hz
#    $3: ABE_SYS_CLK in Hz
#    $4: ABE_32K_CLK in Hz
#    $5: PAD_UCLKS in Hz
#    $6: SLIMBUS_UCLKS in Hz
#    $7: Divider ABE_CLK CM_CLKSEL_ABE[1:0] CLKSEL_OPP
#    $8: Divider AESS_FCLK and ABE_GICLK CM_ABE_AESS_CLKCTRL[24] CLKSEL_AESS_FCLK
#    $9: Mux TIMER5_GFCLK CM_ABE_TIMER5_CLKCTRL[24] CLKSEL
#    $10: Mux TIMER6_GFCLK CM_ABE_TIMER6_CLKCTRL[24] CLKSEL
#    $11: Mux TIMER7_GFCLK CM_ABE_TIMER7_CLKCTRL[24] CLKSEL
#    $12: Mux TIMER8_GFCLK CM_ABE_TIMER8_CLKCTRL[24] CLKSEL
#    $13: Mux DMIC_GFCLK CM_ABE_DMIC_CLKCTRL[25:24] CLKSEL_SOURCE
#    $14: Mux DMIC internal functional clock CM_ABE_DMIC_CLKCTRL[27:26] CLKSEL_INTERNAL_SOURCE
#    $15: Mux MCASP internal functional clock CM_ABE_MCASP_CLKCTRL[27:26] CLKSEL_INTERNAL_SOURCE
#    $16: Mux MCASP_GFCLK CM_ABE_MCASP_CLKCTRL[25:24] CLKSEL_SOURCE
#    $17: Mux MCBSP1_GFCLK CM_ABE_MCBSP1_CLKCTRL[25:24] CLKSEL_SOURCE
#    $18: Mux MCBSP1 internal functional clock CM_ABE_MCBSP1_CLKCTRL[27:26]CLKSEL_INTERNAL_SOURCE
#    $19: Mux MCBSP2_GFCLK CM_ABE_MCBSP2_CLKCTRL[25:24]CLKSEL_SOURCE
#    $20: Mux MCBSP2 internal functional clock CM_ABE_MCBSP2_CLKCTRL[27:26]CLKSEL_INTERNAL_SOURCE
#    $21: Mux MCBSP3_GFCLK CM_ABE_MCBSP3_CLKCTRL[25:24]CLKSEL_SOURCE
#    $22: Mux MCBSP3 internal functional clock CM_ABE_MCBSP3_CLKCTRL[27:26]CLKSEL_INTERNAL_SOURCE
#Provides the following clocks in Hz:
#    ABE_LP_CLK ABE_CLK AESS_CLK ABE_GICLK ABE_24M_FCLK TIMER5_GFCLK TIMER6_GFCLK TIMER7_GFCLK TIMER8_GFCLK DMIC_GFCLK MCBSP1_GFCLK MCBSP2_GFCLK MCBSP3_GFCLK MCASP_GFCLK
omap5_ckgen_abe() {
  params_check $# 22
  local dpll_abe_x2_clk=$1
  local func_24m_gfclk=$2
  local abe_sys_clk=$3
  local abe_32k_clk=$4
  local pad_uclks=$5
  local slimbus_uclks=$6
  local div_abe_clk=$((2**$7))
  local div_aess_fclk_abe_giclk=$(($8+1))
  local mux_timer5_gfclk=$9
  local mux_timer6_gfclk=${10}
  local mux_timer7_gfclk=${11}
  local mux_timer8_gfclk=${12}
  local mux_dmic_gfclk=${13}
  local mux_dmic_int_func_clk=${14}
  local mux_mcasp_int_func_clk=${15}
  local mux_mcasp_gfclk=${16}
  local mux_mcbsp1_gfclk=${17}
  local mux_mcbsp1_int_func_clk=${18}
  local mux_mcbsp2_gfclk=${19}
  local mux_mcbsp2_int_func_clk=${20}
  local mux_mcbsp3_gfclk=${21}
  local mux_mcbsp3_int_func_clk=${22}
  
  ABE_LP_CLK=$((dpll_abe_x2_clk/16))
  ABE_CLK=$((dpll_abe_x2_clk/div_abe_clk))
  AESS_CLK=$((ABE_CLK/div_aess_fclk_abe_giclk))
  ABE_GICLK=$((ABE_CLK/2))
  ABE_24M_FCLK=$((dpll_abe_x2_clk/8)) 
  omap5_cd_prm_timerx $mux_timer5_gfclk $abe_sys_clk $abe_32k_clk
  TIMER5_GFCLK=$TIMERx_GFCLK
  omap5_cd_prm_timerx $mux_timer6_gfclk $abe_sys_clk $abe_32k_clk
  TIMER6_GFCLK=$TIMERx_GFCLK
  omap5_cd_prm_timerx $mux_timer7_gfclk $abe_sys_clk $abe_32k_clk
  TIMER7_GFCLK=$TIMERx_GFCLK
  omap5_cd_prm_timerx $mux_timer8_gfclk $abe_sys_clk $abe_32k_clk
  TIMER8_GFCLK=$TIMERx_GFCLK
  omap5_mcxsp_dmic_clk $mux_dmic_gfclk $mux_dmic_int_func_clk $ABE_24M_FCLK $abe_sys_clk $func_24m_gfclk $pad_uclks $slimbus_uclks
  DMIC_GFCLK=$MCxSP_DMIC_GFCLK
  omap5_mcxsp_dmic_clk $mux_mcbsp1_gfclk $mux_mcbsp1_int_func_clk $ABE_24M_FCLK $abe_sys_clk $func_24m_gfclk $pad_uclks $slimbus_uclks
  MCBSP1_GFCLK=$MCxSP_DMIC_GFCLK
  omap5_mcxsp_dmic_clk $mux_mcbsp2_gfclk $mux_mcbsp2_int_func_clk $ABE_24M_FCLK $abe_sys_clk $func_24m_gfclk $pad_uclks $slimbus_uclks
  MCBSP2_GFCLK=$MCxSP_DMIC_GFCLK
  omap5_mcxsp_dmic_clk $mux_mcbsp3_gfclk $mux_mcbsp3_int_func_clk $ABE_24M_FCLK $abe_sys_clk $func_24m_gfclk $pad_uclks $slimbus_uclks
  MCBSP3_GFCLK=$MCxSP_DMIC_GFCLK
  omap5_mcxsp_dmic_clk $mux_mcasp_gfclk $mux_mcasp_int_func_clk $ABE_24M_FCLK $abe_sys_clk $func_24m_gfclk $pad_uclks $slimbus_uclks
  MCASP_GFCLK=$MCxSP_DMIC_GFCLK
}

#Function to obtain the clocks controlled by the CM_CORE_AON, takes the 
#following parameters:
#    $1: DPLL PER PER_GPU_CLK in Hz
#    $2: DPLL PER FUNC_256M_CLK in Hz
#    $3: DPLL PER FUNC_192M_CLK in Hz
#    $4: DPLL Core CORE_GPU_CLK in Hz
#    $5: CM_CORE_AON L3_ICLK in Hz
#    $6: Mux MMC1_GFCLK CM_L3INIT_MMC1_CLKCTRL[24] CLKSEL_SOURCE
#    $7: Divider MMC1_GFCLK CM_L3INIT_MMC1_CLKCTRL[25] CLKSEL_DIV
#    $8: Mux MMC2_GFCLK CM_L3INIT_MMC2_CLKCTRL[24] CLKSEL_SOURCE
#    $9: Divider MMC2_GFCLK CM_L3INIT_MMC2_CLKCTRL[25] CLKSEL_DIV
#    $10: Mux GPU_HYD_GCLK CM_GPU_GPU_CLKCTRL[25] CLKSEL_GPU_HYD_CLK
#    $11: Mux GPU_CORE_GCLK CM_GPU_GPU_CLKCTRL[24] CLKSEL_GPU_CORE_CLK
#    $12: Divider FDIF_GFCLK CM_CAM_FDIF_CLKCTRL[24] CLKSEL_FCLK
#    $13: Divider HSI_GFCLK CM_L3INIT_HSI_CLKCTRL[24] CLKSEL
#    $14: Divider GPU_L3_GICLK CM_GPU_GPU_CLKCTRL[26] CLKSEL_GPU_SYS_CLK
#Provides the following clock values in Hz:
#    GPU_HYD_GCLK GPU_CORE_GCLK FUNC_128M_CLK FUNC_48M_FCLK FUNC_96M_FCLK FDIF_GFCLK HSI_GFCLK FUNC_12M_FCLK PER_12M_GFCLK GPU_L3_ICLK GPU_L3_GICLK MMC1_GFCLK MMC2_GFCLK PER_96M_GFCLK CSI_PHY_GFCLK PER_48M_GFCLK L3INIT_48M_GFCLK HDMI_PHY_GFCLK
omap5_cm_core() {
  params_check $# 14
  local dpll_per_gpu_clk=$1
  local dpll_per_func_256m_clk=$2
  local dpll_per_func_192m_clk=$3
  local dpll_core_gpu_clk=$4
  local cm_core_aon_l3_iclk=$5
  local mux_mmc1_gfclk=$6
  local div_mmc1_gfclk=$((2**$7))
  local mux_mmc2_gfclk=$8
  local div_mmc2_gfclk=$((2**$9))
  local mux_gpu_hyd_gclk=${10}
  local mux_gpu_core_gclk=${11}
  local div_fdif_gfclk=$((2**${12}))
  local div_hsi_gfclk=$((2**${13}))
  local div_gpu_l3_giclk=$((2**${14}))

  case $mux_gpu_hyd_gclk in
    0)GPU_HYD_GCLK=$dpll_core_gpu_clk;;
    1)GPU_HYD_GCLK=$dpll_per_gpu_clk;;
  esac

  case $mux_gpu_core_gclk in
    0)GPU_CORE_GCLK=$dpll_core_gpu_clk;;
    1)GPU_CORE_GCLK=$dpll_per_gpu_clk;;
  esac

  FUNC_128M_CLK=$((dpll_per_func_256m_clk/2))
  FUNC_48M_FCLK=$((dpll_per_func_192m_clk/4))
  FUNC_96M_FCLK=$((dpll_per_func_192m_clk/2))
  FDIF_GFCLK=$((dpll_per_func_256m_clk/div_fdif_gfclk))
  HSI_GFCLK=$((dpll_per_func_192m_clk/div_hsi_gfclk))
  FUNC_12M_FCLK=$((dpll_per_func_192m_clk/16))
  PER_12M_GFCLK=$FUNC_12M_FCLK
  GPU_L3_ICLK=$((cm_core_aon_l3_iclk/div_gpu_l3_giclk))
  GPU_L3_GICLK=$GPU_L3_ICLK

  if [ $mux_mmc1_gfclk -eq 1 ]
  then
    MMC1_GFCLK=$((dpll_per_func_192m_clk/div_mmc1_gfclk))
  else
    MMC1_GFCLK=$((FUNC_128M_CLK/div_mmc1_gfclk))
  fi
  if [ $mux_mmc2_gfclk -eq 1 ]
  then
    MMC2_GFCLK=$((dpll_per_func_192m_clk/div_mmc2_gfclk))
  else
    MMC2_GFCLK=$((FUNC_128M_CLK/div_mmc2_gfclk))
  fi
  
  PER_96M_GFCLK=$FUNC_96M_FCLK
  CSI_PHY_GFCLK=$FUNC_96M_FCLK
  PER_48M_GFCLK=$FUNC_48M_FCLK
  L3INIT_48M_GFCLK=$FUNC_48M_FCLK
  HDMI_PHY_GFCLK=$FUNC_48M_FCLK
}  

#Function to obtain the clocks provided byt he HDMI PLL, takes the following
#parameters:
#    $1: input clock in Hz
#    $2: PLL multiplier M PLLCTRL_HDMI_CONFIGURATION1[20:9] PLL_REGM
#    $3: PLL divider N PLLCTRL_HDMI_CONFIGURATION1[8:1] PLL_REGN
#    $4: PLL divider M2 PLLCTRL_HDMI_CONFIGURATION4[24:18] PLL_REGM2
#Provides the following clock values in Hz:
#    HDMI_PHY CLKDCOLDO
omap5_hdmi_pll() {
  params_check $# 4
  adpllj $@
  CLKDCOLDO=$ADPLLJ_CLKDCOLDO
  HDMI_PHY=$ADPLLJ_CLKOUTLDO
}

#Function to obtain the clocks provided by the DSI PLLs, takes the following
#parameters:
#    $1: input clock in Hz
#    $2: PLL multiplier M DSI_PLL_CONFIGURATION1[20:9] PLL_REGM
#    $3: PLL divider N DSI_PLL_CONFIGURATION1[8:1] PLL_REGN
#    $4: PLL HS divider M4 DSI_PLL_CONFIGURATION1[25:21] M4_CLOCK_DIV
#    $5: PLL HS divider M5 DSI_PLL_CONFIGURATION1[30:26] M5_CLOCK_DIV
#Provides the following clock values in Hz:
#    CLKDCOLDO DSI_PHY_DSS DPLL_DSI1_CLK1 DPLL_DSI1_CLK2
omap5_dsi_pll() {
  params_check $# 5
  local pll_params=( ${@:1:3} 1 0 0 1 )
  adplls ${pll_params[@]}
  local div_m4=$4
  local div_m5=$5
  CLKDCOLDO=$ADPLLJ_CLKDCOLDO
  DSI_PHY_DSS=$ADPLLJ_CLKOUTLDO
  DPLL_DSI1_CLK1=$((CLKDCOLDO/div_m4))
  DPLL_DSI1_CLK2=$((CLKDCOLDO/div_m5))
}

#Function to obtain the clocks provided to a timer, takes the following parameters:
#    $1: Mux input selection (see Timers register information comment below)
#    $2: (optional) clock associated with Mux selection 0 in Hz. Defaults to SYS_CLK if not specified
#    $3: (optional) clock associated with Mux selection 1 in Hz. Defaults to SYS_32K if not specified
#Provides the GFCLK clock in Hz, this funcion can be used for TIMERs 1-4 and 9-11. Timers 5-8 clocks are managed by GKGEN_ABE
omap5_cd_prm_timerx() {
  local mux_signals=( $SYS_CLK $FUNC_SYS_32K )
  if [ $# -gt 1 ]
  then
    mux_signals=( $2 $3 )
  fi
  params_check $((${#mux_signals[@]}+1)) 3 
  local cm_timer_clksel=$1
  TIMERx_GFCLK=${mux_signals[$cm_timer_clksel]}
}

#Timers register information
#Mux TIMER1_GFCLK CM_WKUPAON_TIMER1_CLKCTRL[24] CLKSEL
#Mux TIMER2_GFCLK CM_L4PER_TIMER2_CLKCTRL[24] CLKSEL
#Mux TIMER3_GFCLK CM_L4PER_TIMER3_CLKCTRL[24] CLKSEL
#Mux TIMER4_GFCLK CM_L4PER_TIMER4_CLKCTRL[24] CLKSEL
#Mux TIMER5_GFCLK CM_ABE_TIMER5_CLKCTRL[24] CLKSEL, provided by omap5_ckgen_abe
#Mux TIMER6_GFCLK CM_ABE_TIMER6_CLKCTRL[24] CLKSEL, provided by omap5_ckgen_abe
#Mux TIMER7_GFCLK CM_ABE_TIMER7_CLKCTRL[24] CLKSEL, provided by omap5_ckgen_abe
#Mux TIMER8_GFCLK CM_ABE_TIMER8_CLKCTRL[24] CLKSEL, provided by omap5_ckgen_abe
#Mux TIMER9_GFCLK CM_L4PER_TIMER9_CLKCTRL[24] CLKSEL
#Mux TIMER10_GFCLK CM_L4PER_TIMER10_CLKCTRL[24] CLKSEL
#Mux TIMER11_GFCLK CM_L4PER_TIMER11_CLKCTRL[24] CLKSEL
#End of Timers register information

#Function to obtain the clocks provided by the PER DPLL, takes the
#following parameters:
#    $1: input clock in Hz PER_DPLL_CLK
#    $2: DPLL multiplier M CM_CLKSEL_DPLL_PER[18:8] DPLL_MULT
#    $3: DPLL divider N CM_CLKSEL_DPLL_PER[6:0] DPLL_DIV
#    $4: DPLL divider M2 CM_DIV_M2_DPLL_PER[4:0] DIVHS
#    $5: DPLL divider M3 CM_DIV_M3_DPLL_PER[4:0] DIVHS
#    $6: H11 divider CM_DIV_H11_DPLL_PER[5:0] DIVHS
#    $7: H12 divider CM_DIV_H12_DPLL_PER[5:0] DIVHS
#    $8: H14 divider CM_DIV_H14_DPLL_PER[5:0] DIVHS
#Provides the following clock values in Hz:
#    FUNC_96M_AON_CLK FUNC_192M_FCLK FUNC_256M_FCLK DSS_GFCLK PER_DPLL_SCRM_CLK 
#    PER_GPU_CLK
omap5_per_dpll() {
  params_check $# 8
  local pll_params=( ${@:1:4} 0 0 ${@:5:3} 0 $8 )
  ab_dpll ${pll_params[@]}
  FUNC_96M_AON_CLK=$CLKOUT_M2
  FUNC_192M_FCLK=$CLKOUTX2_M2
  PER_DPLL_SCRM_CLK=$CLKOUTX2_M3
  FUNC_256M_FCLK=$CLKOUTX2_H11
  DSS_GFCLK=$CLKOUTX2_H12
  PER_GPU_CLK=$CLKOUTX2_H14
}

#Function to obtain the clocks provided by a type A of Type B DPLL, takes the
#following parameters:
#    $1: input clock in Hz CORE_DPLL_CLK
#    $2: DPLL multiplier M CM_CLKSEL_DPLL_CORE[18:8] DPLL_MULT
#    $3: DPLL divider N CM_CLKSEL_DPLL_CORE[6:0] DPLL_DIV
#    $4: DPLL divider M2 CM_DIV_M2_DPLL_CORE[4:0] DIVHS
#    $5: DPLL divider M3 CM_DIV_M3_DPLL_CORE[4:0] DIVHS
#    $6: H11 divider CM_DIV_H11_DPLL_CORE[5:0] DIVHS
#    $7: H12 divider CM_DIV_H12_DPLL_CORE[5:0] DIVHS
#    $8: H13 divider CM_DIV_H13_DPLL_CORE[5:0] DIVHS
#    $9: H14 divider CM_DIV_H14_DPLL_CORE[5:0] DIVHS
#    $10: H22 divider CM_DIV_H22_DPLL_CORE[5:0] DIVHS
#    $11: H23 divider CM_DIV_H23_DPLL_CORE[5:0] DIVHS
#    $12: H24 divider CM_DIV_H24_DPLL_CORE[5:0] DIVHS
#Provides the following clock values in Hz:
#    EMIF_PHY_GCLK CORE_DPLL_SCRM_CLK CORE_DLL_GCLK CORE_X2_CLK 
#    CORE_USB_OTG_SS_LFPS_TX_CLK CORE_GPU_CLK CORE_IPU_ISS_BOOST_CLK
#    CORE_ISS_MAIN_CLK BB2D_GFCLK
omap5_core_dpll() {
  params_check $# 12
  local dpll_params=( ${@:1:4} 0 0 ${@:5:5} 1 ${@:${10}:3} )
  ab_dpll ${dpll_params[@]}
  EMIF_PHY_GCLK=$CLKOUT_M2
  CORE_DPLL_SCRM_CLK=$CLKOUTX2_M3
  CORE_DLL_GCLK=$CLKOUTX2_H11
  CORE_X2_CLK=$CLKOUTX2_H12
  CORE_USB_OTG_SS_LFPS_TX_CLK=$CLKOUTX2_H13
  CORE_GPU_CLK=$CLKOUTX2_H14
  CORE_IPU_ISS_BOOST_CLK=$CLKOUTX2_H22
  CORE_ISS_MAIN_CLK=$CLKOUTX2_H23
  BB2D_GFCLK=$CLKOUTX2_H24
}

#Function to obtain the clocks provided by the ABE DPLL takes the following
#parameters:
#    $1: ABE_DPLL_CLK input clock in Hz
#    $2: ABE PLL multiplier M CM_CLKSEL_DPLL_ABE[18:8] DPLL_MULT
#    $3: ABE PLL divider N CM_CLKSEL_DPLL_ABE[6:0] DPLL_DIV
#    $4: ABE PLL divider M2 CM_DIV_M2_DPLL_ABE[4:0] DIVHS
#    $5: ABE PLL REGM4XEN M4 CM_CLKMODE_DPLL_ABE[11] DPLL_REGM4XEN
#    $6: ABE PLL divider M3 CM_DIV_M3_DPLL_ABE [4:0] DIVHS
#Provides the following clock values in Hz:
#    DPLL_ABE_X2_FCLK CORE_DPLL_HS_CLK
omap5_abe_dpll() {
  params_check $# 6
  local pll_params=( ${@:1:5} 0 $6 )
  ab_dpll ${pll_params[@]}
  DPLL_ABE_X2_FCLK=$CLKOUTX2_M2
  CORE_DPLL_HS_CLK=$CLKOUTX2_M3
}

#Function to obtain the clocks provided by the MPU PLL takes the following
#parameters:
#    $1: MPU_DPLL_CLK input clock in Hz
#    $2: MPU PLL multiplier M CM_CLKSEL_DPLL_MPU[18:8] DPLL_MULT
#    $3: MPU PLL divider N CM_CLKSEL_DPLL_MPU[6:0] DPLL_DIV
#    $4: MPU PLL divider M2 CM_DIV_M2_DPLL_MPU[4:0] DIVHS
#Provides the following clock values in Hz:
#    MPU_GCLK MA_EOCP_GICLK
omap5_mpu_dpll() {
  params_check $# 4
  local pll_params=( $@ 0 0 1 )
  ab_dpll ${pll_params[@]}
  MPU_GCLK=$CLKOUT_M2
  MA_EOCP_GICLK=$((CLKOUT_M2/4)) 
}

#Function to obtain the clocks provided by the IVA PLL takes the following
#parameters:
#    $1: DSP_DPLL_CLK input clock in Hz
#    $2: IVA PLL multiplier M CM_CLKSEL_DPLL_IVA[18:8] DPLL_MULT
#    $3: IVA PLL divider N CM_CLKSEL_DPLL_IVA[6:0] DPLL_DIV
#    $4: IVA PLL H11 divider CM_DIV_H11_DPLL_IVA[5:0] DIVHS
#    $5: IVA PLL H12 divider CM_DIV_H12_DPLL_IVA[5:0] DIVHS 
#Provides the following clock values in Hz:
#    DSP_GCLK IVA_GCLK
omap5_iva_dpll() {
  params_check $# 5
  local pll_params=( ${@:1:3} 1 0 0 1 ${@:4} )
  ab_dpll ${pll_params[@]}
  DSP_GCLK=$CLKOUTX2_H11
  IVA_GCLK=$CLKOUTX2_H12
    
}

#Function to obtain the clocks provided by the USB PLL takes the following
#parameters:
#    $1: USB_DPLL_CLK input clock in Hz
#    $2: USB PLL multiplier M CM_CLKSEL_DPLL_USB[19:8] DPLL_MULT
#    $3: USB PLL divider N CM_CLKSEL_DPLL_USB[7:0] DPLL_DIV
#    $4: USB PLL divider M2 CM_DIV_M2_DPLL_USB[6:0] DIVHS
#Provides the following clock values in Hz:
#    L3INIT_480M_GFCLK L3INIT_960M_GFCLK
omap5_usb_dpll() {
  params_check $# 4
  local pll_params=( $@ 0 0 1 )
  ab_dpll ${pll_params[@]}
  L3INIT_480M_GFCLK=$CLKOUT_M2
  L3INIT_960M_GFCLK=$CLK_DCO_LDO
}

