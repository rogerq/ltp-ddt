#!/bin/bash

source clk_framework_utils.sh

#Function to define the input clks to te ABE DPLL, it was place separately in
#order to start the clk definitons with the ABE DPLL which feeds the PRM, but
#whose input clks are controlled by PRM
#Takes the following parameters:
#    $1: SYS_CLK1 clock in Hz
#    $2: SYS_CLK2 clock in Hz
#    $3: FUNC_SYS_32K clock in Hz
#    $4: ABE_DPLL_CLK Mux. CM_CLKSEL_ABE_PLL_REF[0] CLKSEL
#    $5: ABE_DPLL_BYPASS_CLK Mux. CM_CLKSEL_ABE_PLL_BYPAS[0] CLKSEL
#    $6: ABE_DPLL_SYS_CLK Mux. CM_CLKSEL_ABE_PLL_SYS[0] CLKSEL
#Provides the following clock values in Hz:
#    ABE_DPLL_CLK ABE_DPLL_BYPASS_CLK ABE_DPLL_SYS_CLK
dra7xx_abe_dpll_input_clks() {
  params_check $# 6
  local sys_clk1=$1
  local sys_clk2=$2
  local func_sys_32k=$3
  local mux_abe_dpll_clk=$4
  local mux_abe_dpll_bypass_clk=$5
  local mux_abe_dpll_sys_clk=$6
  
  if [ $mux_abe_dpll_sys_clk -eq 1 ]
  then
    ABE_DPLL_SYS_CLK=$sys_clk2
  else
    ABE_DPLL_SYS_CLK=$sys_clk1
  fi
  if [ $mux_abe_dpll_clk -eq 1 ]
  then
    ABE_DPLL_CLK=$func_sys_32k
  else
    ABE_DPLL_CLK=$ABE_DPLL_SYS_CLK
  fi
  if [ $mux_abe_dpll_bypass_clk -eq 1 ]
  then
    ABE_DPLL_BYPASS_CLK=$func_sys_32k
  else
    ABE_DPLL_BYPASS_CLK=$ABE_DPLL_SYS_CLK
  fi
}


#Function to obtain the clocks controlled by the PRM, takes the following
#parameters:
#    $1: ABE_LP_CLK clock in Hz
#    $2: SYS_CLK1 clock in Hz
#    $3: SYS_CLK2 clock in Hz
#    $4: FUNC_SYS_32K clock in Hz
#    $5: WKUPAON_ICLK Mux. CM_CLKSEL_WKUPAON[0] CLKSEL
#    $6: DCAN1_SYS_CLK Mux. CM_WKUPAON_DCAN1_CLKCTRL[24] CLKSEL
#    $7: VIDEO1_DPLL_CLK Mux. CM_CLKSEL_VIDEO1_PLL_SYS[0] CLKSEL
#    $8: VIDEO2_DPLL_CLK Mux. CM_CLKSEL_VIDEO2_PLL_SYS[0] CLKSEL
#    $9: HDMI_DPLL_CLK Mux. CM_CLKSEL_HDMI_PLL_SYS[0] CLKSEL
#    $10: ABE_SYS_CLK Div. CM_CLKSEL_ABE_SYS[0]CLKSEL
#    $11: TIMER_SYS_CLK Div. CM_CLKSEL_TIMER_SYS[0]CLKSEL
#    $12: MPU_DPLL_CLK_ABE Div. CM_MPU_MPU_CLKCTRL[26]CLKSEL_ABE_DIV_MODE
#    $13: MPU_DPLL_CLK_EMIF Div. CM_MPU_MPU_CLKCTRL[25:24]CLKSEL_EMIF_DIV_MODE
#    $14: L3INSTR_TS_GCLK and L3INSTR_DLL_AGING_CLK Div. CM_L3INSTR_CTRL_MODULE_BANDGAP_CLKCTRL[25:24]CLKSEL
#Provides the following clock values in Hz:
#    WKUPAON_ICLK DCAN1_SYS_CLK VIDEO1_DPLL_CLK VIDEO2_DPLL_CLK HDMI_DPLL_CLK MPU_DPLL_CLK ABE_SYS_CLK TIMER_SYS_CLK MPU_DPLL_CLK_ABE MPU_DPLL_CLK_EMIF L3INSTR_TS_GCLK L3INSTR_DLL_AGING_GCLK CUSTEFUSE_SYS_GFCLK
# The following clocks are gated by PRM but the source of the clk is not modified; hence, they are not provided by this function. The list is displayed as <source (clk list)>:
#    WKUPAON_ICLK (SR_GPU_SYS_GFLK, SR_DSPEVE_SYS_GFCLK, SR_IVA_SYS_GFCLK,
#                  SR_MPU_SYS_GFCLK, SR_CORE_SYS_GFCLK, WKUPAON_GCLK, 
#                  WKUPAON_GICLK, CM_CORE_AON_SYS_CLK, WKUPAON_PROFILING_GCLK)
#
#    SYS_CLK1     (CORE_DPLL_CLK, PER_DPLL_CLK, DSP_DPLL_CLK, MPU_DPLL_CLK, 
#                  GPU_DPLL_CLK, IVA_DPLL_CLK, EVE_DPLL_CLK, USB_DPLL_CLK,
#                  DDR_DPLL_CLK, PCIE_DPLL_CLK, GMAC_DPLL_CLK, PCIE_SYS_CLK,
#                  DCAN2_SYS_CLK, USB_OTG_SS_REF_CLK, SATA_REF_GFCLK)
#   
#    FUNC_SYS_32K (COREAON_32K_GFCLK, L4PER_32K_GFCLK, L3INT_32K_GFCLK, 
#                  PCIE_32K_GFCLK, HDMI_CEC_GFCLK)
dra7xx_prm() {
  params_check $# 14
  local abe_lp_clk=$1
  local sys_clk1=$2
  local sys_clk2=$3
  local func_sys_32k=$4
  local mux_wkupaon_iclk=$5
  local mux_dcan1_sys_clk=$6
  local mux_video1_dpll_clk=$7
  local mux_video2_dpll_clk=$8
  local mux_hdmi_dpll_clk=$9
  local div_abe_sys_clk=$((${10}+1))
  local div_timer_sys_clk=$((${11}+1))
  local div_mpu_dpll_clk_abe=$(((${12}+1)*8))
  local div_mpu_dpll_clk_emif=4
  if [ ${13} -gt 1 ]
  then
    div_mpu_dpll_clk_emif=8
  fi
  local div_l3instr_ts_gclk_l3instr_dll_aging_gclk=$((2**(${14}+3)))
 
  if [ $mux_wkupaon_iclk -eq 1 ]
  then
    WKUPAON_ICLK=$abe_lp_clk
  else
    WKUPAON_ICLK=$sys_clk1
  fi
  if [ $mux_dcan1_sys_clk -eq 1 ]
  then
    DCAN1_SYS_CLK=$sys_clk2
  else
    DCAN1_SYS_CLK=$sys_clk1
  fi
  if [ $mux_video1_dpll_clk -eq 1 ]
  then
    VIDEO1_DPLL_CLK=$sys_clk2
  else
    VIDEO1_DPLL_CLK=$sys_clk1
  fi
  if [ $mux_video2_dpll_clk -eq 1 ]
  then
    VIDEO2_DPLL_CLK=$sys_clk2
  else
    VIDEO2_DPLL_CLK=$sys_clk1
  fi
  if [ $mux_hdmi_dpll_clk -eq 1 ]
  then
    HDMI_DPLL_CLK=$sys_clk2
  else
    HDMI_DPLL_CLK=$sys_clk1
  fi
  MPU_DPLL_CLK=$sys_clk1
  ABE_SYS_CLK=$((sys_clk1/div_abe_sys_clk))
  TIMER_SYS_CLK=$((sys_clk1/div_timer_sys_clk))
  MPU_DPLL_CLK_ABE=$(($MPU_DPLL_CLK/div_mpu_dpll_clk_abe))
  MPU_DPLL_CLK_EMIF=$(($MPU_DPLL_CLK/div_mpu_dpll_clk_emif))
  L3INSTR_TS_GCLK=$(($WKUPAON_ICLK/div_l3instr_ts_gclk_l3instr_dll_aging_gclk))
  L3INSTR_DLL_AGING_GCLK=$((WKUPAON_ICLK/div_l3instr_ts_gclk_l3instr_dll_aging_gclk))
  CUSTEFUSE_SYS_GFCLK=$((sys_clk1/2))
}

#Function to obtain the clocks controlled by the CM_CORE_AON, takes the 
#following parameters:
#    $1: DPLL Core CORE_X2_CLK in Hz
#    $2: DPLL ABE DPLL_ABE_X2_CLK in Hz
#    $3: DPLL ABE CORE_DPLL_HS_CLK in Hz
#    $4  DPLL PER FUNC_96M_AON_CLK in Hz
#    $5: Divider L3_ICLK CM_CLKSEL_CORE[4] CLKSEL_L3
#    $6: Divider L4_ROOT_CLK CM_CLKSEL_CORE[8] CLKSEL_L4
#    $7: Divider MPU_DPLL_HS_CLK CM_BYPCLK_DPLL_MPU[1:0] CLKSEL
#    $8: Divider IVA_DPLL_HS_CLK CM_BYPCLK_DPLL_IVA[1:0] CLKSEL
#    $9: Divider EVE_DPLL_HS_CLK CM_BYPCLK_DPLL_EVE[1:0] CLKSEL
#    $10: Divider DSP_DPLL_HS_CLK CM_BYPCLK_DPLL_DSP[1:0] CLKSEL
#    $11: Divider ABE_LP_CLK CM_CLKSEL_ABE_LP_CLK[1:0] CLKSEL
#    $12: Divider ABE_24M_FCLK CM_CLKSEL_ABE_24M[1:0] CLKSEL
#Provides the following clock values in Hz:
#    L3_ICLK CM_CORE_AON_PROFILING_L3_GICLK L4_ROOT_CLK L4_ICLK CM_CORE_AON_PROFILING_L4_GICLK MPU_DPLL_HS_CLK IVA_DPLL_HS_CLK EVE_DPLL_HS_CLK DSP_DPLL_HS_CLK ABE_LP_CLK ABE_24M_GFCLK PER_DPLL_HS_CLK USB_DPLL_HS_CLK FUNC_24M_GFCLK 
dra7xx_cm_core_aon_a() {
  params_check $# 12
  local dpll_core_x2_clk=$1
  local dpll_abe_x2_clk=$2
  local core_dpll_hs_clk=$3
  local func_96m_aon_clk=$4
  local div_l3_iclk=$(($5+1))
  local div_l4_root_clk=$(($6+1))
  local div_mpu_dpll_hs_clk=$((2**$7)) #This is a mux in TRM not a div
  local div_iva_dpll_hs_clk=$((2**$8)) #This is a mux in TRM not a div
  local div_eve_dpll_hs_clk=$((2**$9)) #This is a mux in TRM not a div
  local div_dsp_dpll_hs_clk=$((2**${10})) #This is a mux in TRM not a div
  local div_abe_lp_clk=$((2**(4+${11})))
  local div_abe_24m_fclk=$((2**(3+${12})))

  L3_ICLK=$((dpll_core_x2_clk/div_l3_iclk))
  CM_CORE_AON_PROFILING_L3_GICLK=$L3_ICLK
  L4_ROOT_CLK=$((L3_ICLK/div_l4_root_clk))
  L4_ICLK=$L4_ROOT_CLK
  CM_CORE_AON_PROFILING_L4_GICLK=$L4_ROOT_CLK
  MPU_DPLL_HS_CLK=$((dpll_core_x2_clk/div_mpu_dpll_hs_clk))
  IVA_DPLL_HS_CLK=$((dpll_core_x2_clk/div_iva_dpll_hs_clk))
  EVE_DPLL_HS_CLK=$((dpll_core_x2_clk/div_eve_dpll_hs_clk))
  DSP_DPLL_HS_CLK=$((dpll_core_x2_clk/div_dsp_dpll_hs_clk))
  ABE_LP_CLK=$((dpll_abe_x2_clk/div_abe_lp_clk))
  ABE_24M_GFCLK=$((dpll_abe_x2_clk/div_abe_24m_fclk))
  PER_DPLL_HS_CLK=$((core_dpll_hs_clk/2))
  USB_DPLL_HS_CLK=$((core_dpll_hs_clk/3))
  FUNC_24M_GFCLK=$((func_96m_aon_clk/4))
} 

#Function to obtain the clocks controlled by the CM_CORE_AON, takes the 
#following parameters:
#    $1: DPLL GPU GPU_CLK in Hz
#    $2: DPLL PER PER_GPU_CLK in Hz
#    $3: DPLL PER FUNC_256M_CLK in Hz
#    $4: DPLL PER PER_QSPI_CLK in Hz
#    $5: DPLL PER FUNC_192M_CLK in Hz
#    $6: DPLL Core CORE_GPU_CLK in Hz
#    $7: DPLL Core CORE_IPU_ISS_BOOST_CLK in Hz
#    $8: DPLL Core CORE_ISS_MAIN_CLK in Hz
#    $9: CM_CORE_AON L3_ICLK in Hz
#    $10: DPLL ABE DPLL_ABE_X2_CLK in Hz
#    $11: RMII clkock in Hz
#    $12: DPLL GMAC GMAC_250M_CLK in Hz
#    $13: DPLL GMAC GMAC_RMII_HS_CLK in Hz
#    $14: DPLL EVE EVE_GFCLK in Hz
#    $15: DPLL EVE EVE_GCLK in Hz
#    $16: PRM FUNC_32K_CLK in Hz
#    $17: DPLL ABE PER_ABE_X1_GFCLK in Hz
#    $18: Video1 VIDEO1_CLK in Hz
#    $19: Video2 VIDEO2_CLK in Hz
#    $20: HDMI HDMI_CLKIN in Hz
#    $21: Mux MMC1_GFCLK CM_L3INIT_MMC1_CLKCTRL[24] CLKSEL_SOURCE
#    $22: Divider MMC1_GFCLK CM_L3INIT_MMC1_CLKCTRL[26:25] CLKSEL_DIV
#    $23: Mux MMC2_GFCLK CM_L3INIT_MMC2_CLKCTRL[24] CLKSEL_SOURCE
#    $24: Divider MMC2_GFCLK CM_L3INIT_MMC2_CLKCTRL[26:25] CLKSEL_DIV
#    $25: Mux MMC3_FCLK CM_L4PER_MMC3_CLKCTRL[24] CLKSEL_MUX
#    $26: Divider MMC3_FCLK CM_L4PER_MMC3_CLKCTRL[26:25] CLKSEL_DIV
#    $27: Mux MMC4_FCLK CM_L4PER_MMC4_CLKCTRL[24] CLKSEL_MUX
#    $28: Divider MMC4_FCLK CM_L4PER_MMC4_CLKCTRL[26:25] CLKSEL_DIV
#    $29: Mux GPU_HYD_GCLK CM_GPU_GPU_CLKCTRL[27:26] CLKSEL_HYD_CLK
#    $30: Mux GPU_CORE_GCLK CM_GPU_GPU_CLKCTRL[25:24] CLKSEL_CORE_CLK
#    $31: Mux QSPI_GFCLK CM_L4PER2_QSPI_CLKCTRL[24] CLKSEL_SOURCE
#    $32: Divider QSPI_GFCLK CM_L4PER2_QSPI_CLKCTRL[26:25] CLKSEL_DIV
#    $33: Mux VIP1_GCLK CM_CAM_VIP1_CLKCTRL[24] CLKSEL
#    $34: Mux VIP2_GCLK CM_CAM_VIP2_CLKCTRL[24] CLKSEL
#    $35: Mux VIP3_GCLK CM_CAM_VIP3_CLKCTRL[24] CLKSEL
#    $36: Divider ABE_CLK CM_CLKSEL_ABE_CLK_DIV[2:0] CLKSEL
#    $37: Divider AESS_FCLK CM_CLKSEL_AESS_FCLK_DIV[0] CLKSEL
#    $38: Divider ABE_GICLK CM_CLKSEL_ABE_GICLK_DIV[0] CLKSEL
#    $39: Mux IPU1_GFCLK CM_IPU1_IPU1_CLKCTRL[24] CLKSEL
#    $40: Mux RGMII_50M_CLK CM_GMAC_GMAC_CLKCTRL[24] CLKSEL_REF
#    $41: Mux ATL_GFCLK CM_ATL_ATL_CLKCTRL[27:26] CLKSEL_SOURCE2
#    $42: Mux ATL_SOURCE1 CM_ATL_ATL_CLKCTRL[25:24] CLKSEL_SOURCE1
#    $43: Mux GMAC_RFT_CLK CM_GMAC_GMAC_CLKCTRL[27:25] CLKSEL_RFT
#    $44: Mux EVE_CLK CM_CLKSEL_EVE_CLK[0] CLKSEL
#    $45: Divider EVE_DCLK CM_CLKSEL_EVE_GFCLK_CLKOUTMUX[2:0] CLKSEL
#Provides the following clock values in Hz:
#    GPU_HYD_GCLK GPU_CORE_GCLK FUNC_128M_CLK FUNC_48M_FCLK QSPI_GFCLK MMC1_FCLK MMC2_FCLK MMC3_FCLK MMC4_FCLK PER_48M_GFCLK L3INIT_48M_GFCLK HDMI_PHY_GFCLK PER_12M_GFCLK IPU1_GFCLK VIP1_GCLK VIP2_GCLK VIP3_GCLK ABE_CLK AESS_FCLK ABE_GICLK GMAC_MAIN_CLK GMII_M_CLK_SINK RGMII_50M_CLK RGMII_5M_CLK EVE_DCLK EVE_CLK ATL_GFCLK GMAC_RFT_CLK
dra7xx_cm_core_aon_b() {
  params_check $# 45
  local dpll_gpu_clk=$1
  local dpll_per_gpu_clk=$2
  local dpll_per_func_256m_clk=$3
  local dpll_per_qspi_clk=$4
  local dpll_per_func_192m_clk=$5
  local dpll_core_gpu_clk=$6
  local dpll_core_ipu_iss_boost_clk=$7
  local dpll_core_iss_main_clk=$8
  local cm_core_aon_l3_iclk=$9
  local dpll_abe_x2_clk=${10}
  local rmii_clk=${11}
  local dpll_gmag_250m_clk=${12}
  local dpll_gmag_rmii_hs_clk=${13}
  local dpll_eve_gfclk=${14}
  local dpll_dsp_eve_gclk=${15}
  local func_32k_clk=${16}
  local dpll_abe_per_abe_x1_gfclk=${17}
  local video1_clkin=${18}
  local video2_clkin=${19}
  local hdmi_clkin=${20}
  local mux_mmc1_gfclk=${21}
  local div_mmc1_gfclk=$((2**${22}))
  local mux_mmc2_gfclk=${23}
  local div_mmc2_gfclk=$((2**${24}))
  local mux_mmc3_gfclk=${25}
  local div_mmc3_gfclk=$((2**${26}))
  local mux_mmc4_gfclk=${27}
  local div_mmc4_gfclk=$((2**${28}))
  local mux_gpu_hyd_gclk=${29}
  local mux_gpu_core_gclk=${30}
  local mux_qspi_gfclk=${31}
  local div_qspi_gfclk=$((2**${32}))
  local mux_vip1_gclk=${33}
  local mux_vip2_gclk=${34}
  local mux_vip3_gclk=${35}
  local div_abe_clk=$((2**${36}))
  local div_aess_fclk=$((${37}+1))
  local div_abe_giclk=$((${38}+1))
  local mux_ipu1_gfclk=${39}
  local mux_rgmii_50m_clk=${40}
  local mux_atl_gfclk=${41}
  local mux_atl_source1=${42}
  local mux_gmac_rft_clk=${43}
  local mux_eve_clk=${44}
  local div_eve_dclk=$((2**${45}))

  case $mux_gpu_hyd_gclk in
    0)GPU_HYD_GCLK=$dpll_core_gpu_clk;;
    1)GPU_HYD_GCLK=$dpll_per_gpu_clk;;
    2)GPU_HYD_GCLK=$dpll_gpu_clk;;
  esac

  case $mux_gpu_core_gclk in
    0)GPU_CORE_GCLK=$dpll_core_gpu_clk;;
    1)GPU_CORE_GCLK=$dpll_per_gpu_clk;;
    2)GPU_CORE_GCLK=$dpll_gpu_clk;;
  esac

  FUNC_128M_CLK=$((dpll_per_func_256m_clk/2))
  FUNC_48M_FCLK=$((dpll_per_func_192m_clk/4))
  if [ $mux_qspi_gfclk -eq 1 ]
  then 
    QSPI_GFCLK=$((dpll_per_qspi_clk/div_qspi_gfclk))
  else
    QSPI_GFCLK=$((FUNC_128M_CLK/div_qspi_gfclk))
  fi
  if [ $mux_mmc1_gfclk -eq 1 ]
  then
    MMC1_FCLK=$((dpll_per_func_192m_clk/div_mmc1_gfclk))
  else
    MMC1_FCLK=$((FUNC_128M_CLK/div_mmc1_gfclk))
  fi
  if [ $mux_mmc2_gfclk -eq 1 ]
  then
    MMC2_FCLK=$((dpll_per_func_192m_clk/div_mmc2_gfclk))
  else
    MMC2_FCLK=$((FUNC_128M_CLK/div_mmc2_gfclk))
  fi
  if [ $mux_mmc3_gfclk -eq 1 ]
  then
    MMC3_FCLK=$((dpll_per_func_192m_clk/div_mmc3_gfclk))
  else
    MMC3_FCLK=$((FUNC_48M_FCLK/div_mmc3_gfclk))
  fi
  if [ $mux_mmc4_gfclk -eq 1 ]
  then
    MMC4_FCLK=$((dpll_per_func_192m_clk/div_mmc4_gfclk))
  else
    MMC4_FCLK=$((FUNC_48M_FCLK/div_mmc4_gfclk))
  fi
  PER_48M_GFCLK=$FUNC_48M_FCLK
  L3INIT_48M_GFCLK=$FUNC_48M_FCLK
  HDMI_PHY_GFCLK=$FUNC_48M_FCLK
  PER_12M_GFCLK=$((dpll_per_func_192m_clk/16))
  if [ $mux_ipu1_gfclk -eq 1 ]
  then
    IPU1_GFCLK=$dpll_core_ipu_iss_boost_clk
  else
    IPU1_GFCLK=$dpll_abe_x2_clk
  fi
  if [ $mux_vip1_gclk -eq 1 ]
  then
    VIP1_GCLK=$dpll_core_iss_main_clk
  else
    VIP1_GCLK=$cm_core_aon_l3_iclk
  fi  
  if [ $mux_vip2_gclk -eq 1 ]
  then
    VIP2_GCLK=$dpll_core_iss_main_clk
  else
    VIP2_GCLK=$cm_core_aon_l3_iclk
  fi
  if [ $mux_vip3_gclk -eq 1 ]
  then
    VIP3_GCLK=$dpll_core_iss_main_clk
  else
    VIP3_GCLK=$cm_core_aon_l3_iclk
  fi
  ABE_CLK=$((dpll_abe_x2_clk/div_abe_clk))
  AESS_FCLK=$((ABE_CLK/div_aess_fclk))
  ABE_GICLK=$((AESS_FCLK/div_abe_giclk))
  GMAC_MAIN_CLK=$((dpll_gmag_250m_clk/2))
  GMII_M_CLK_SINK=$((dpll_gmag_rmii_hs_clk/2))
  if [ $mux_rgmii_50m_clk -eq 1 ]
  then
    RGMII_50M_CLK=$rmii_clk
  else
    RGMII_50M_CLK=$dpll_gmag_rmii_hs_clk
  fi
  RGMII_5M_CLK=$((RGMII_50M_CLK/10))
  EVE_DCLK=$((dpll_eve_gfclk/div_eve_dclk))
  if [ $mux_eve_clk -eq 1 ]
  then
    EVE_CLK=$dpll_dsp_eve_gclk
  else
    EVE_CLK=$dpll_eve_gfclk
  fi
  local atl_source1
  case $mux_atl_source1 in
    0)atl_source1=$func_32k_clk;;
    1)atl_source1=$video1_clkin;;
    2)atl_source1=$video2_clkin;;
    3)atl_source1=$hdmi_clkin;;
  esac
  case $mux_atl_gfclk in
    0)ATL_GFCLK=$cm_core_aon_l3_iclk;;
    1)ATL_GFCLK=$dpll_abe_per_abe_x1_gfclk;;
    2)ATL_GFCLK=$atl_source1;;
  esac
  case $mux_gmac_rft_clk in
    0)GMAC_RFT_CLK=$video1_clkin;;
    1)GMAC_RFT_CLK=$video2_clkin;;
    2)GMAC_RFT_CLK=$dpll_abe_per_abe_x1_gfclk;;
    3)GMAC_RFT_CLK=$hdmi_clkin;;
    4)GMAC_RFT_CLK=$cm_core_aon_l3_iclk;;
  esac
}  

#Function to obtain the clocks provided by the videos plls takes the following
#parameters:
#    $1: input clock in Hz
#    $2: ADPLL multiplier M
#    $3: ADPLL divider N
#    $4: HSDIVIDER M4 value
#    $5: HSDIVIDER M5 value
#    $6: HSDIVIDER M6 value
# Reference clock control is enabled with PLL_CONFIGURATION2[13] PLL_REFEN bit
dra7xx_dss_dpll_videox() {
  params_check $# 6
  local dpll_params=( ${@:1:3} 1 0 0 1 )
  local m4_div=$4
  local m5_div=$5
  local m6_div=$6

  adplls ${dpll_params[@]}

  VIDEOx_CLK=$((ADPLLS_CLKDCOLDO/m5_div))
  DPLL_VIDEO_CLKOUTM4=$((ADPLLS_CLKDCOLDO/m4_div))
  DPLL_VIDEO_CLKOUTM6=$((ADPLLS_CLKDCOLDO/m6_div))
}
#DPLL_VIDEOx information
#Register Name                                    Offset DPLL_VIDEO1 DPLL_VIDEO1
#                                                          L3_MAIN     L4_CFG 
#                                                          Physical    Physical
#                                                          Address     Address
#DPLL_VIDEO1 Registers
#PLL_CONFIGURATION1(M4_div:25-21,M:20-9,N:8-1)  0x0000000C 0x5800430C 0x4A0A400C
#PLL_CONFIGURATION3(M6_div:4-0)                 0x00000014 0x58004314 0x4A0A4014
#DPLL_VIDEO2 Registers
#PLL_CONFIGURATION1(M4_div:25-21,M:20-9,N:8-1)  0x0000000C 0x5800530C 0x4A0A500C
#PLL_CONFIGURATION3(M6_div:4-0)                 0x00000014 0x58005314 0x4A0A5014
#Note: M5 divider value is missing from the TRM
#End of DPLL_VIDEOx information

#Function to obtain the clocks provided byt he HDMI PLL, takes the following
#parameters:
#    $1: input clock in Hz
#    $2: ADPLL multiplier M PLLCTRL_HDMI_CONFIGURATION1[20:9] PLL_REGM
#    $3: ADPLL divider N PLLCTRL_HDMI_CONFIGURATION1[8:1] PLL_REGN
#    $4: ADPLL divider M2 PLLCTRL_HDMI_CONFIGURATION4[24:18] PLL_REGM2
#Provides the following clock values in Hz:
#    HDMI_PHY DPLL_HDMI_CLK1 CLKDCOLDO
dra7xx_hdmi_pll() {
  params_check $# 4
  adpllj $@
  CLKDCOLDO=$ADPLLJ_CLKDCOLDO
  HDMI_PHY=$ADPLLJ_CLKOUTLDO
  DPLL_HDMI_CLK1=$ADPLLJ_CLKOUT
}

#Function to obtain the clock provided by the CM_CORE_AON Mux, requires definition of clocks: SYS_CLK1, SYS_CLK2, PER_ABE_X1_GFCLK, MPU_GCLK, DSP_GFCLK, IVA_GCLK, GPU_GCLK, CORE_DPLL_OUT_CLK, EMIF_PHY_GCLK, GMAC_250M_CLK, VIDEO2_M2_CLKIN, VIDEO1_M2_CLKIN, HDMI_CLKIN, FUNC_96M_AON_CLK, L3_INIT_480M_GFCLK, USB_OTG_CLKIN, SATA_CLKIN, PCIE_M2_CLK, APLL_PCIE_M2_CLK, EMU_CLKIN, and EVE_DCLK. Takes the 
#following parameters:
#    $1: Mux input selection CLKOUTMUX0_CLK CM_CLKSEL_CLKOUTMUX0[4:0] CLKSEL
#    $2: Divider SYS_CLK1_DCLK CM_CLKSEL_SYS_CLK1_CLKOUTMUX[2:0] CLKSEL
#    $3: Divider SYS_CLK2_DCLK CM_CLKSEL_SYS_CLK2_CLKOUTMUX[2:0] CLKSEL
#    $4: Divider PER_ABE_X1_DCLK CM_CLKSEL_PER_ABE_X1_CLK_CLKOUTMUX[2:0] CLKSEL
#    $5: Divider MPU_DCLK CM_CLKSEL_MPU_GCLK_CLKOUTMUX[2:0] CLKSEL
#    $6: Divider DSP_DCLK CM_CLKSEL_DSP_GFCLK_CLKOUTMUX[2:0] CLKSEL
#    $7: Divider IVA_DCLK CM_CLKSEL_IVA_GCLK_CLKOUTMUX[2:0] CLKSEL
#    $8: Divider GPU_DCLK CM_CLKSEL_GPU_GCLK_CLKOUTMUX[2:0] CLKSEL
#    $9: Divider CORE_DPLL_OUT_DCLK CM_CLKSEL_CORE_DPLL_OUT_CLK_CLKOUTMUX[2:0] CLKSEL
#    $10: Divider EMIF_PHY_DCLK CM_CLKSEL_EMIF_PHY_GCLK_CLKOUTMUX[2:0] CLKSEL
#    $11: Divider GMAC_250M_DCLK CM_CLKSEL_GMAC_250M_CLK_CLKOUTMUX[2:0] CLKSEL
#    $12: Divider VIDEO2_DCLK CM_CLKSEL_VIDEO2_CLK_CLKOUTMUX[2:0] CLKSEL
#    $13: Divider VIDEO1_DCLK CM_CLKSEL_VIDEO1_CLK_CLKOUTMUX[2:0] CLKSEL
#    $14: Divider HDMI_DCLK CM_CLKSEL_HDMI_CLK_CLKOUTMUX[2:0] CLKSEL
#    $15: Divider FUNC_96M_AON_DCLK CM_CLKSEL_FUNC_96M_AON_CLK_CLKOUTMUX[2:0] CLKSEL
#    $16: Divider L3INIT_480M_DCLK CM_CLKSEL_L3INIT_480M_GFCLK_CLKOUTMUX[2:0] CLKSEL
#    $17: Divider USB_OTG_DCLK CM_CLKSEL_USB_OTG_CLK_CLKOUTMUX[2:0] CLKSEL
#    $18: Divider SATA_DCLK CM_CLKSEL_SATA_CLK_CLKOUTMUX[2:0] CLKSEL
#    $19: Divider PCIE2_DCLK CM_CLKSEL_PCIE2_CLK_CLKOUTMUX[2:0] CLKSEL
#    $20: Divider PCIE1_DCLK CM_CLKSEL_PCIE1_CLK_CLKOUTMUX[2:0] CLKSEL
#    $21: Divider EMU_DCLK CM_CLKSEL_EMU_CLK_CLKOUTMUX[2:0] CLKSEL
dra7xx_cm_core_aon_clkoutmux() {
  local mux_signals=( $SYS_CLK1 $SYS_CLK2 $PER_ABE_X1_GFCLK $MPU_GCLK $DSP_GFCLK $IVA_GCLK $GPU_GCLK $CORE_DPLL_OUT_CLK $EMIF_PHY_GCLK $GMAC_250M_CLK $VIDEO2_CLK $VIDEO1_CLK $DPLL_HDMI_CLK1 $FUNC_96M_AON_CLK $L3_INIT_480_GFCLK $USB_OTG_CLKIN $SATA_CLKIN $PCIE_M2_CLK $APLL_PCIE_M2_CLK $EMU_CLKIN $EVE_DCLK )
  params_check ${#mux_signals[@]} 20 "Requires definition of 20 clks, ${#mux_signals[@]} clks defined"
  params_check $# 21
  local cm_clksel_clkoutmux0=$1
  local cm_clk_div=( ${@:2} 0 )
  CLKOUTMUX_CLK=$((${mux_signals[$cm_clksel_clkoutmux0]}/(2**${cm_clk_div[$cm_clksel_clkoutmux0]})))
}

#Function to obtain the clock provided to a timer by the CM_CORE_AON, requires definition of clocks: TIMER_SYS_CLK, FUNC_32K_CLK, SYS_CLK2, XREF_CLK0, XREF_CLK1, XREF_CLK2, XREF_CLK3, ABE_GICLK, VIDEO1_CLK, VIDEO2_CLK, HDMI_CLK, and  CLKCLKOUTMUX0_CLK. Takes the following parameters:
#    $1: Mux input selection (see Timers register information comment below)
#    $2: Divider VIDEO1_CLK CM_CLKSEL_VIDEO1_TIMER[2:0] CLKSEL
#    $3: Divider VIDEO2_CLK CM_CLKSEL_VIDEO2_TIMER[2:0] CLKSEL
#    $4: Divider HDMI_CLK CM_CLKSEL_HDMI_TIMER[2:0] CLKSEL
#Provides the GFCLK clock in Hz, this funcion can be used for TIMERs 1-4, 9-11,
#    and 13-16
dra7xx_cm_core_aon_timerx() {
  local mux_signals=( $TIMER_SYS_CLK $SYS_32K $SYS_CLK2 $XREF_CLK0 $XREF_CLK1 $XREF_CLK2 $XREF_CLK3 $ABE_GICLK $VIDEO1_CLK $VIDEO2_CLK $HDMI_CLK )
  params_check ${#mux_signals[@]} 11 "Requires definition of 11 clks, only ${#mux_signals[@]} have been defined"
  params_check $# 4
  local cm_timer_clksel=$1
  local cm_clk_div_pow=( 0 0 0 0 0 0 0 0 ${@:2} )
  local pow=${cm_clk_div_pow[$cm_timer_clksel]}
  local cm_clk_div=$((2**pow))
  case $pow in
    5)cm_clk_div=22;;
    6)cm_clk_div=32;;
  esac
  TIMERx_GFCLK=$((${mux_signals[$cm_timer_clksel]}/cm_clk_div))
}

#Timers register information
#Mux TIMER1_GFCLK CM_WKUPAON_TIMER1_CLKCTRL[27:24] CLKSEL
#Mux TIMER2_GFCLK CM_L4PER_TIMER2_CLKCTRL[27:24] CLKSEL
#Mux TIMER3_GFCLK CM_L4PER_TIMER3_CLKCTRL[27:24] CLKSEL
#Mux TIMER4_GFCLK CM_L4PER_TIMER4_CLKCTRL[27:24] CLKSEL
#Mux TIMER5_GFCLK CM_IPU_TIMER5_CLKCTRL[27:24] CLKSEL
#Mux TIMER6_GFCLK CM_IPU_TIMER6_CLKCTRL[27:24] CLKSEL
#Mux TIMER7_GFCLK CM_IPU_TIMER7_CLKCTRL[27:24] CLKSEL
#Mux TIMER8_GFCLK CM_IPU_TIMER8_CLKCTRL[27:24] CLKSEL
#Divider VIDEO1_CLK CM_CLKSEL_VIDEO1_TIMER[2:0] CLKSEL
#Divider VIDEO2_CLK CM_CLKSEL_VIDEO2_TIMER[2:0] CLKSEL
#Divider HDMI_CLK CM_CLKSEL_HDMI_TIMER[2:0] CLKSEL
#Mux TIMER9_GFCLK CM_L4PER_TIMER9_CLKCTRL[27:24] CLKSEL
#Mux TIMER10_GFCLK CM_L4PER_TIMER10_CLKCTRL[27:24] CLKSEL
#Mux TIMER11_GFCLK CM_L4PER_TIMER11_CLKCTRL[27:24] CLKSEL
#Mux TIMER13_GFCLK CM_L4PER3_TIMER13_CLKCTRL[27:24] CLKSEL
#Mux TIMER14_GFCLK CM_L4PER3_TIMER14_CLKCTRL[27:24] CLKSEL
#Mux TIMER15_GFCLK CM_L4PER3_TIMER15_CLKCTRL[27:24] CLKSEL
#Mux TIMER16_GFCLK CM_L4PER3_TIMER16_CLKCTRL[27:24] CLKSEL
#End of Timers register information

#Function to obtain the clock provided to a timer by the CM_CORE_AON, requires definition of clocks: TIMER_SYS_CLK, FUNC_32K_CLK, SYS_CLK2, XREF_CLK0, XREF_CLK1, XREF_CLK2, XREF_CLK3, ABE_GICLK, VIDEO1_CLK, VIDEO2_CLK, HDMI_CLK, and  CLKCLKOUTMUX0_CLK. Takes the following parameters:
#    $1: Mux input selection (see Timers register information comment below)
#    $2: Divider VIDEO1_CLK CM_CLKSEL_VIDEO1_TIMER[2:0] CLKSEL
#    $3: Divider VIDEO2_CLK CM_CLKSEL_VIDEO2_TIMER[2:0] CLKSEL
#    $4: Divider HDMI_CLK CM_CLKSEL_HDMI_TIMER[2:0] CLKSEL
#Provides the GFCLK clock in Hz, this function can be used for TIMERs 5,6,7,8
dra7xx_cm_core_aon_timer1() {
  params_check $# 4
  if [ $1 -eq 11 ]
  then
    TIMERx_GFCLK=$CLKCLKOUTMUX0_CLK
  else
    dra7xx_cm_core_aon_timerx $@
  fi
}

#Function to obtain the MCSASPx_AHCLKX or MCSASPx_AHCLKR by the CM_CORE_AON, requires definition of clocks: ABE_24M_GFCLK ABE_SYS_CLK FUNC_24M_GFCLK ATL_CLK0 ATL_CLK1 ATL_CLK2 ATL_CLK3 SYS_CLK2 XREF_CLK0 XREF_CLK1 XREF_CLK2 XREF_CLK3 MLB_CLK MLBP_CLK. Takes the following parameters:
#    $1: Mux input selection (see MCASPs register information comment below)
#    $2: Divider MLB_CLK CM_CLKSEL_MLB_MCASP[2:0] CLKSEL
#    $3: Divider MLBP_CLK CM_CLKSEL_MLBP_MCASP[2:0] CLKSEL
#Provides the AHCLK or AHCLKX in Hz for MCASP 1-8 
dra7xx_cm_core_aon_mcasp_ahclkrx() {
  local mux_signals=( $ABE_24M_GFCLK $ABE_SYS_CLK $FUNC_24M_GFCLK $ATL_CLK0 $ATL_CLK1 $ATL_CLK2 $ATL_CLK3 $SYS_CLK2 $XREF_CLK0 $XREF_CLK1 $XREF_CLK2 $XREF_CLK3 $MLB_CLK $MLBP_CLK )
  local cm_clksel=$1
  local cm_clk_div=( 0 0 0 0 0 0 0 0 0 0 0 0 ${@:2} )
  params_check ${#mux_signals[@]} 14 "Requires definition of 14 clks, ${#mux_signals[@]} clks defined"
  params_check $# 3
  MCASPx_AHCLKx=$((${mux_signals[$cm_clksel]}/(2**${cm_clk_div[$cm_clksel]})))
  MLB_DIV_CLK=$((MLB_CLK/(2**$2)))
  MLBP_DIV_CLK=$((MLBP_CLK/(2**$3)))
}

# This function is commented since the TRM seems to have an error it does not reference a register to divide neither MLB_DIV_CLK nor MLBP_DIV_CLK
#Function to obtain the MCSASPx_AHCLKX by the CM_CORE_AON, requires definition of clocks: $ABE_24M_GFCLK $ABE_SYS_CLK $FUNC_24M_GFCLK $ATL_CLK0 $ATL_CLK1 $ATL_CLK2 $ATL_CLK3 $SYS_CLK2 $XREF_CLK0 $XREF_CLK1 $XREF_CLK2 $XREF_CLK3 $MLB_DIV_CLK $MLBP_DIV_CLK. Takes the following parameters:
#    $1: Mux input selection (see MCASPs register information comment below)
#    $2: Divider MLB_CLK CM_CLKSEL_MLB_MCASP[2:0] CLKSEL
#    $3: Divider MLBP_CLK CM_CLKSEL_MLBP_MCASP[2:0] CLKSEL
#dra7xx_cm_core_aon_mcasp3() {
#  local mux_signals=( $ABE_24M_GFCLK $ABE_SYS_CLK $FUNC_24M_GFCLK $ATL_CLK0 $ATL_CLK1 $ATL_CLK2 $ATL_CLK3 $SYS_CLK2 $XREF_CLK0 $XREF_CLK1 $XREF_CLK2 $XREF_CLK3 $MLB_CLK $MLBP_CLK )
#  local cm_clksel=$1
#  local cm_clk_div=( ${@:2} )
#  MCASPx_AHCLKx=$((${mux_signals[$cm_clksel]}/${cm_clk_div[$cm_clksel]}))
#}

# MCASPs register information
# Mux MCASP1_AUX_GFCLK CM_IPU_MCASP1_CLKCTRL[23:22] CLKSEL_AUX_CLK
# Mux MCASP2_AUX_GFCLK CM_L4PER2_MCASP2_CLKCTRL[23:22] CLKSEL_AUX_CLK
# Mux MCASP3_AUX_GFCLK CM_L4PER2_MCASP3_CLKCTRL[23:22] CLKSEL_AUX_CLK
# Mux MCASP4_AUX_GFCLK CM_L4PER2_MCASP4_CLKCTRL[23:22] CLKSEL_AUX_CLK
# Mux MCASP5_AUX_GFCLK CM_L4PER2_MCASP5_CLKCTRL[23:22] CLKSEL_AUX_CLK
# Mux MCASP6_AUX_GFCLK CM_L4PER2_MCASP6_CLKCTRL[23:22] CLKSEL_AUX_CLK
# Mux MCASP7_AUX_GFCLK CM_L4PER2_MCASP7_CLKCTRL[23:22] CLKSEL_AUX_CLK
# Mux MCASP8_AUX_GFCLK CM_L4PER2_MCASP8_CLKCTRL[23:22] CLKSEL_AUX_CLK
# Divider VIDEO1_CLK CM_CLKSEL_VIDEO1_MCASP_AUX[2:0] CLKSEL
# Divider VIDEO2_CLK CM_CLKSEL_VIDEO2_MCASP_AUX[2:0] CLKSEL
# Divider HDMI_CLK CM_CLKSEL_HDMI_MCASP_AUX[2:0] CLKSEL
# Divider PER_ABE_X1_GFCLK CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX[2:0] CLKSEL
# Mux MCASP1_AHCLKX CM_IPU_MCASP1_CLKCTRL[27:24] CLKSEL_AHCLKX
# Mux MCASP1_AHCLKR CM_IPU_MCASP1_CLKCTRL[31:28] CLKSEL_AHCLKR
# Mux MCASP2_AHCLKX CM_L4PER2_MCASP2_CLKCTRL[27:24] CLKSEL_AHCLKX
# Mux MCASP2_AHCLKR CM_L4PER2_MCASP2_CLKCTRL[31:28] CLKSEL_AHCLKR
# Divider MLB_CLK CM_CLKSEL_MLB_MCASP[2:0] CLKSEL
# Divider MLBP_CLK CM_CLKSEL_MLBP_MCASP[2:0] CLKSEL
# Mux MCASP3_AHCLKX CM_L4PER2_MCASP3_CLKCTRL[27:24] CLKSEL_AHCLKX
# Mux MCASP4_AHCLKX CM_L4PER2_MCASP4_CLKCTRL[27:24] CLKSEL_AHCLKX
# Mux MCASP5_AHCLKX CM_L4PER2_MCASP5_CLKCTRL[27:24] CLKSEL_AHCLKX
# Mux MCASP6_AHCLKX CM_L4PER2_MCASP6_CLKCTRL[27:24] CLKSEL_AHCLKX
# Mux MCASP7_AHCLKX CM_L4PER2_MCASP7_CLKCTRL[27:24] CLKSEL_AHCLKX
# Mux MCASP8_AHCLKX CM_L4PER2_MCASP8_CLKCTRL[27:24] CLKSEL_AHCLKX
# End of MCASPs register information


#Function to obtain the MCSASPx_AUX_GFCLK controlled by the CM_CORE_AON, requires definition of clocks: PER_ABE_X1_GFCLK, VIDEO1_CLK, VIDEO2_CLK, and HDMI_CLK. Takes the following parameters:
#    $1: Mux (See MACSPs register information above)
#    $2: Divider PER_ABE_X1_GFCLK CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX[2:0] CLKSEL
#    $3: Divider VIDEO1_CLK CM_CLKSEL_VIDEO1_TIMER[2:0] CLKSEL
#    $4: Divider VIDEO2_CLK CM_CLKSEL_VIDEO2_TIMER[2:0] CLKSEL
#    $5: Divider HDMI_CLK CM_CLKSEL_HDMI_TIMER[2:0] CLKSEL
#Provides the GFCLK in Hz for MCASP 1-8
dra7xx_cm_core_aon_mcasp_aux_gfclk() {
  params_check $# 5
  local mux_signals=( $PER_ABE_X1_GFCLK $VIDEO1_CLK $VIDEO2_CLK $HDMI_CLK )
  local cm_clksel=$1
  local cm_clk_div=( ${@:2} )
  MCASPx_AUX_GFCLK=$((${mux_signals[$cm_clksel]}/(2**${cm_clk_div[$cm_clksel]})))

}

#Wrapper function for the most commong DPLL configuration of PLLs in
#DRA7x boards
dra7xx_dpll_m2() {
  local dpll_m2_params=( ${@:1:4} 0 0 1 ${@:5:8} )
  ab_dpll ${dpll_m2_params[@]}
}

#Function to obtain the clocks provided by a type A of Type B DPLL, takes the
#following parameters:
#    $1: input clock in Hz PER_DPLL_CLK
#    $2: DPLL multiplier M CM_CLKSEL_DPLL_PER[18:8] DPLL_MULT
#    $3: DPLL divider N CM_CLKSEL_DPLL_PER[6:0] DPLL_DIV
#    $4: DPLL divider M2 CM_DIV_M2_DPLL_PER[4:0] DIVHS
#    $5: H11 divider CM_DIV_H11_DPLL_PER[5:0] DIVHS
#    $6: H12 divider CM_DIV_H12_DPLL_PER[5:0] DIVHS
#    $7: H13 divider CM_DIV_H13_DPLL_PER[5:0] DIVHS
#    $8: H14 divider CM_DIV_H14_DPLL_PER[5:0] DIVHS
#Provides the following clock values in Hz:
#    FUNC_96M_AON_CLK FUNC_192M_CLK FUNC_256M_CLK DSS_GFCLK PER_QSPI_CLK 
#    PER_GPU_CLK
dra7xx_per_dpll() {
  params_check $# 8
  dra7xx_dpll_m2 $@
  FUNC_96M_AON_CLK=$CLKOUT_M2
  FUNC_192M_CLK=$CLKOUTX2_M2
  FUNC_256M_CLK=$CLKOUTX2_H11
  DSS_GFCLK=$CLKOUTX2_H12
  PER_QSPI_CLK=$CLKOUTX2_H13
  PER_GPU_CLK=$CLKOUTX2_H14
}

#Function to obtain the clocks provided by a type A of Type B DPLL, takes the
#following parameters:
#    $1: input clock in Hz CORE_DPLL_CLK
#    $2: DPLL multiplier M CM_CLKSEL_DPLL_CORE[18:8] DPLL_MULT
#    $3: DPLL divider N CM_CLKSEL_DPLL_CORE[6:0] DPLL_DIV
#    $4: DPLL divider M2 CM_DIV_M2_DPLL_CORE[4:0] DIVHS
#    $5: H12 divider CM_DIV_H12_DPLL_CORE[5:0] DIVHS
#    $6: H13 divider CM_DIV_H13_DPLL_CORE[5:0] DIVHS
#    $7: H14 divider CM_DIV_H14_DPLL_CORE[5:0] DIVHS
#    $8: H22 divider CM_DIV_H22_DPLL_CORE[5:0] DIVHS
#    $9: H23 divider CM_DIV_H23_DPLL_CORE[5:0] DIVHS
#    $10: H24 divider CM_DIV_H24_DPLL_CORE[5:0] DIVHS
#Provides the following clock values in Hz:
#    CORE_DPLL_OUT_CLK CORE_X2_CLK CORE_USB_OTG_SS_LFPS_TX_CLK CORE_GPU_CLK 
#    CORE_IPU_ISS_BOOST_CLK CORE_ISS_MAIN_CLK BB2D_GFCLK=$CLKOUTX2_H24
dra7xx_core_dpll() {
  params_check $# 10
  local dpll_params=( ${@:1:4} 1 ${@:5:3} 1 ${@:8:3} )
  dra7xx_dpll_m2 ${dpll_params[@]}
  CORE_DPLL_OUT_CLK=$CLKOUT_M2
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
#    PER_ABE_X1_GFCLK DPLL_ABE_X2_FCLK CORE_DPLL_HS_CLK
dra7xx_abe_dpll() {
  params_check $# 6
  local pll_params=( ${@:1:5} 0 $6 )
  ab_dpll ${pll_params[@]}
  PER_ABE_X1_GFCLK=$CLKOUT_M2
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
dra7xx_mpu_dpll() {
  params_check $# 4
  dra7xx_dpll_m2 $@
  MPU_GCLK=$CLKOUT_M2
  MA_EOCP_GICLK=$((CLKOUT_M2/4)) 
}

#Function to obtain the clocks provided by the IVA PLL takes the following
#parameters:
#    $1: IVA_DPLL_CLK input clock in Hz
#    $2: IVA PLL multiplier M CM_CLKSEL_DPLL_IVA[18:8] DPLL_MULT
#    $3: IVA PLL divider N CM_CLKSEL_DPLL_IVA[6:0] DPLL_DIV
#    $4: IVA PLL divider M2 CM_DIV_M2_DPLL_IVA[4:0] DIVHS
#Provides the following clock values in Hz:
#    IVA_GCLK
dra7xx_iva_dpll() {
  params_check $# 4
  dra7xx_dpll_m2 $@
  IVA_GCLK=$CLKOUTX2_M2  
}

#Function to obtain the clocks provided by the USB PLL takes the following
#parameters:
#    $1: USB_DPLL_CLK input clock in Hz
#    $2: USB PLL multiplier M CM_CLKSEL_DPLL_USB[19:8] DPLL_MULT
#    $3: USB PLL divider N CM_CLKSEL_DPLL_USB[7:0] DPLL_DIV
#    $4: USB PLL divider M2 CM_DIV_M2_DPLL_USB[6:0] DIVHS
#Provides the following clock values in Hz:
#    L3INIT_480M_GFCLK L3INIT_960M_GFCLK
dra7xx_usb_dpll() {
  params_check $# 4
  dra7xx_dpll_m2 $@
  L3INIT_480M_GFCLK=$CLKOUT_M2
  L3INIT_960M_GFCLK=$CLK_DCO_LDO
}

#Function to obtain the clocks provided by the EVE PLL takes the following
#parameters:
#    $1: EVE_DPLL_CLK input clock in Hz
#    $2: EVE PLL multiplier M CM_CLKSEL_DPLL_EVE[18:8] DPLL_MULT
#    $3: EVE PLL divider N CM_CLKSEL_DPLL_EVE[6:0] DPLL_DIV
#    $4: EVE PLL divider M2 CM_DIV_M2_DPLL_EVE[4:0] DIVHS
#Provides the following clock values in Hz:
#    EVE_GFCLK
dra7xx_eve_dpll() {
  params_check $# 4
  dra7xx_dpll_m2 $@
  EVE_GFCLK=$CLKOUTX2_M2
}

#Function to obtain the clocks provided by the ABE DPLL takes the following
#parameters:
#    $1: DSP_DPLL_CLK input clock in Hz
#    $2: DSP PLL multiplier M CM_CLKSEL_DPLL_DSP[18:8] DPLL_MULT
#    $3: DSP PLL divider N CM_CLKSEL_DPLL_DSP[6:0] DPLL_DIV
#    $4: DSP PLL divider M2 CM_DIV_M2_DPLL_DSP[4:0] DIVHS
#    $5: DSP PLL divider M3 CM_DIV_M3_DPLL_DSP[4:0] DIVHS
#Provides the following clock values in Hz:
#    DSP_GFCLK EVE_GCLK
dra7xx_dsp_dpll() {
  params_check $# 5
  local pll_params=( ${@:1:4} 0 0 $5 )
  ab_dpll ${pll_params[@]}
  DSP_GFCLK=$CLKOUT_M2
  EVE_GCLK=$CLKOUTX2_M3
}

#Function to obtain the clocks provided by the GMAC DPLL takes the following
#parameters:
#    $1: GMAC_DPLL_CLK input clock in Hz
#    $2: GMAC PLL multiplier M CM_CLKSEL_DPLL_GMAC[18:8] DPLL_MULT
#    $3: GMAC PLL divider N CM_CLKSEL_DPLL_GMAC[6:0] DPLL_DIV
#    $4: GMAC PLL divider M2 CM_DIV_M2_DPLL_GMAC[4:0] DIVHS
#    $5: GMAC PLL divider M3 CM_DIV_M3_DPLL_GMAC[4:0] DIVHS
#    $6: H11 divider CM_DIV_H11_DPLL_GMAC[5:0] DIVHS
#    $7: H12 divider CM_DIV_H12_DPLL_GMAC[5:0] DIVHS
#    $8: H13 divider CM_DIV_H13_DPLL_GMAC[5:0] DIVHS
#Provides the following clock values in Hz:
#    GMAC_250M_CLK ICSS_IEP_CLK GMAC_RMII_HS_CLK GMII_250MHZ_CLK ICSS_CLK
dra7xx_gmac_dpll() {
  params_check $# 8
  local pll_params=( ${@:1:4} 0 0 ${@:5} )
  ab_dpll ${pll_params[@]}
  GMAC_250M_CLK=$CLKOUT_M2
  ICSS_IEP_CLK=$CLKOUTX2_M3
  GMAC_RMII_HS_CLK=$CLKOUTX2_H11
  GMII_250MHZ_CLK=$CLKOUTX2_H12
  ICSS_CLK=$CLKOUTX2_H13
}

#Function to obtain the clocks provided by the GPU PLL takes the following
#parameters:
#    $1: GPU_DPLL_CLK input clock in Hz
#    $2: GPU PLL multiplier M CM_CLKSEL_DPLL_GPU[18:8] DPLL_MULT
#    $3: GPU PLL divider N CM_CLKSEL_DPLL_GPU[6:0] DPLL_DIV
#    $4: GPU PLL divider M2 CM_DIV_M2_DPLL_GPU[4:0] DIVHS
#Provides the following clock values in Hz:
#    GPU_GCLK
dra7xx_gpu_dpll() {
  params_check $# 4
  dra7xx_dpll_m2 $@
  GPU_GCLK=$CLKOUTX2_M2
}

#Function to obtain the clocks provided by the DDR DPLL takes the following
#parameters:
#    $1: DDR_DPLL_CLK input clock in Hz
#    $2: DDR PLL multiplier M CM_CLKSEL_DPLL_DDR[18:8] DPLL_MULT
#    $3: DDR PLL divider N CM_CLKSEL_DPLL_DDR[6:0] DPLL_DIV
#    $4: DDR PLL divider M2 CM_DIV_M2_DPLL_DDR[4:0] DIVHS
#    $5: H11 divider CM_DIV_H11_DPLL_DDR[5:0] DIVHS
#Provides the following clock values in Hz:
#    EMIF_PHY_GCLK EMIF_DLL_GCLK
dra7xx_ddr_dpll() {
  params_check $# 5
  dra7xx_dpll_m2 $@
  EMIF_PHY_GCLK=$CLKOUTX2_M2
  EMIF_DLL_GCLK=$CLKOUTX2_H11
}

#Function to obtain the clocks provided by the PCI REF DPLL takes the following
#parameters:
#    $1: PCIE_DPLL_CLK input clock in Hz
#    $2: PCIE PLL multiplier M CM_CLKSEL_DPLL_PCIE_REF[19:8] DPLL_MULT
#    $3: PCIE PLL divider N CM_CLKSEL_DPLL_PCIE_REF[7:0] DPLL_DIV
#    $4: PCIE PLL divider M2 CM_DIV_M2_DPLL_PCIE_REF[6:0] DIVHS
#Provides the following clock values in Hz:
#    PCIE_M2_CLK PCIE_REF_CLK
dra7xx_pcie_ref_dpll() {
  params_check $# 4
  dra7xx_dpll_m2 $@
  PCIE_M2_CLK=$CLKOUTX2_M2
  PCIE_REF_CLK=$CLK_DCO_LDO 
}

#Function to obtain the clocks provided by the PCIE APLL takes the following
#parameters:
#    $1: PCIE APLL M2 Divisor CM_DIV_M2_APLL_PCIE[6:0] DIVHS
#    $2: VCO LDO divisor either 0 (div by 1) or 1 (div by 2)
#Provides the following clock values in Hz:
#    PCIE_PHY_CLK PCIE_DIV_GCLK APLL_PCIE_M2_CLK
dra7xx_pcie_apll() {
  params_check $# 2
  #local pciesref_acs_clk=$1 #var not used right now, TRM does not show tx func
  #local pcie_ref_clk=$2 # var not used rigth now, TRM does not show Tx func
  local m2_div=$1  #CM_DIV_M2_APLL_PCIE[6:0] DIVHS
  local vco_ldo_div=$(($2+1)) # either 1 or 2
  PCIE_PHY_CLK=2500000000
  PCIE_DIV_GCLK=$((PCIE_PHY_CLK/vco_ldo_div))
  APLL_PCIE_M2_CLK=$((PCIE_PHY_CLK/(2*m2_div)))
}

#Function to obtain the clocks provided by the USB OTG SS DPLL takes the following
#parameters:
#    $1: USB_OTG_SS_REF_CLK input clock in Hz
#    $2: DPLLCTRL_USB_OTG_SS.PLL_CONFIGURATION1[20:9], PLL_REGM
#    $3: DPLLCTRL_USB_OTG_SS.PLL_CONFIGURATION1[8:1], , PLL_REGN
dra7xx_usb_otg_ss_dpll() {
  params_check $# 3
  local pll_params=( $@ 25 ) 
  dra7xx_dpll_m2 ${pll_params[@]}
  USB_OTG_CLKIN=$CLKOUT_M2
  USB_PHY_PLL_CLK=$CLK_DCO_LDO
}

#Function to obtain the clocks provided by the SATA DPLL takes the following
#parameters:
#    $1: SATA_REF_GFCLK input clock in Hz
#    $2: DPLLCTRL_SATA.PLL_CONFIGURATION1[20:9], PLL_REGM
#    $3: DPLLCTRL_SATA.PLL_CONFIGURATION1[8:1], , PLL_REGN
dra7xx_sata_dpll() {
  params_check $# 3
  local pll_params=( $@ 15 )
  dra7xx_dpll_m2 ${pll_params[@]}
  SATA_CLKIN=$CLKOUT_M2
  SATA_PHY_PLL_CLK=$CLK_DCO_LDO
}

#Function to obtain the clocks provided by the SATA DPLL takes the following
#parameters:
#    $1: EMU_SYSCLK input clock in Hz
#    $2: , PLL_REGM
#    $3: , PLL_REGN
#    $4: M6 Divider
dra7xx_debugss_dpll() {
  params_check $# 4
  local dpll_params=( ${@:1:3} 1 0 0 1 )
  local m6_div=$4

  adplls ${dpll_params[@]}

  EMU_CLKIN=$((ADPLLS_CLKDCOLDO/m6_div))
}

