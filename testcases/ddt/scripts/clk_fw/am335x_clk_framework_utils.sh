#!/bin/bash
#AM335x Clock Framework Utilities

source clk_framework_utils.sh

#Function to obtain the clocks provided by the Core ADPLLS, takes the following
#parameters:
#       $1: input clock in Hz
#       $2: ADPLL multiplier M
#       $3: ADPLL divider N
#       $4: ADPLL REGXM4REG
#       $5: HSDIVIDER M4
#       $6: HSDIVIDER M5
#       $7: HSDIVIDER M6
#       $8: PRCM Mux A
#       $9: PRCM Divider B (0|1)
#       $10: PRCM Mux C
#       $11: PRCM Mux D
#       $12: PER_CLKOUTM2 in Hz
#       $13: DISP_CLKOUT in Hz
#       $14: MHZ 50 Divider
#Provides the following clock values in Hz:
#       CORE_CLKOUTM6 SGX_CORECLK L3S_L4PER_L4WKUP_CLK CORE_CLKOUTM4 
#       PRUICSSOCP_CLK CPTSRFT_CLK MHZ_250_CLK MHZ_125_CLK MHZ_50_CLK 
#       MHZ_5_CLK CORE_CLKOUTM5
am335x_core_pll() {
  params_check $# 14
  local adplls_params=( ${@:1:3} 1 $4 0 1 )
  local M4=$5
  local M5=$6
  local M6=$7
  local MUXA=$8
  local DIVB=$(($9+1))
  local MUXC=${10}
  local MUXD=${11}
  local per_clkoutm2=${12}
  local disp_clkout=${13}
  local mhz_50_div=${14}
  adplls ${adplls_params[@]}
  CORE_CLKOUTM6=$((ADPLLS_CLKDCOLDO/M6))
  CORE_CLKOUTM4=$((ADPLLS_CLKDCOLDO/M4))
  CORE_CLKOUTM5=$((ADPLLS_CLKDCOLDO/M5))
  if [ $MUXA -eq 1 ]
  then
    SGX_CLKSEL=$per_clkoutm2
    SGX_CORECLK=$((per_clkoutm2/DIVB))
  else
    SGX_CLKSEL=$CORE_CLKOUTM4
    SGX_CORECLK=$((CORE_CLKOUTM4/DIVB))
  fi
  L3S_L4PER_L4WKUP_CLK=$((CORE_CLKOUTM4/2))
  if [ $MUXC -eq 1 ]
  then
    PRUICSSOCP_CLK=$disp_clkout
  else
    PRUICSSOCP_CLK=$CORE_CLKOUTM4
  fi
  if [ $MUXD -eq 1 ]
  then
    CPTSRFT_CLK=$CORE_CLKOUTM5
  else
    CPTSRFT_CLK=$CORE_CLKOUTM4
  fi
  MHZ_250_CLK=$CORE_CLKOUTM5
  MHZ_125_CLK=$((CORE_CLKOUTM5/2))
  MHZ_50_CLK=$((CORE_CLKOUTM5/mhz_50_div))
  MHZ_5_CLK=$((MHZ_50_CLK/10))
}

#Function to obtain the clocks provided by the PER ADPLLJ, takes the following
#parameters:
#       $1: input clock in Hz
#       $2: ADPLL multiplier M
#       $3: ADPLL divider N
#       $4: ADPLL divider M2
#       $5: DIV Mux (0|1)
#Provides the following clock values in Hz:                              
#       PER_CLKOUTM2 USB_PHY_CLK PRU_ICCS_UART_CLK MMC_CLK PER_CLK48 PER_CLK24
#       CLK_32KHZ
am335x_per_pll() {
  params_check $# 5
  adpllj ${@:1:4}
  USB_PHY_CLK=$ADPLLJ_CLKDCOLDO
  PER_CLKOUTM2=$ADPLLJ_CLKOUT
  PRU_ICSS_UART_CLK=$PER_CLKOUTM2
  MMC_CLK=$((PER_CLKOUTM2/2))
  PER_CLK48=$((PER_CLKOUTM2/4))
  PER_CLK24=$((PER_CLKOUTM2/8))
  if [ $5 -eq 1 ]
  then
    CLK_32KHZ=$((PER_CLK24*10000/3662109))
  else
    CLK_32KHZ=$((PER_CLK24*10000/7324218))
  fi
}

#Function to obtain the clocks provided by the MPU PLL, takes the following
#parameters:
#       $1: input clock in Hz
#       $2: ADPLL multiplier M
#       $3: ADPLL divider N
#       $4: ADPLL divider M2
#       $5: ADPLL REGXM4EN
#Provides the following clock values in Hz:
#       MPU_CLK
am335x_mpu_pll() {
  params_check $# 5
  local adplls_params=( $@ 0 1 )
  adplls ${adplls_params[@]}
  MPU_CLK=$ADPLLS_CLKOUT
}

#Function to obtain the clocks provided by the Display PLL, takes the following
#parameters:
#       $1: ADPLL input clock in Hz
#       $2: ADPLL multiplier M
#       $3: ADPLL divider N
#       $4: ADPLL divider M2
#       $5: ADPLL REGXM4EN
#       $6: CORE_CLKOUTM5 in Hz
#       $7: PER_CLKOUTM2 in Hz
#       $8: PRCM MUX selection
#Provides the following clock values in Hz:
#       LCD_CLK
am335x_display_pll() {
  params_check $# 8
  local adplls_params=( ${@:1:5} 0 1 )
  local prcm_mux=$8
  local core_clkoutm5=$6
  local per_clkoutm2=$7
  adplls ${adplls_params[@]}
  case $prcm_mux in
    0) LCD_CLK=$ADPLLS_CLKOUT;;
    1) LCD_CLK=$core_clkoutm5;;
    2) LCD_CLK=$per_clkoutm2;;
  esac
}

#Function to obtain the clocks provided by the DDR PLL, takes the following
#parameters:
#       $1: input clock in Hz
#       $2: ADPLL multiplier M
#       $3: ADPLL divider N
#       $4: ADPLL divider M2
#       $5: ADPLL REGXM4EN
#Provides the following clock values in Hz:
#       EMIF_M_CLK
am335x_ddr_pll() {
  params_check $# 5
  local adplls_params=( $@ 0 1 )
  adplls ${adplls_params[@]}
  EMIF_M_CLK=$((ADPLLS_CLKOUT/2))
}

am335x_timer_clkin() {
  params_check $# 1
  local timer_mux=$1
  case $timer_mux in
    0)TIMER_CLK=$TCLKIN;;
    1)TIMER_CLK=$CLK_M_OSC;;
    2)TIMER_CLK=$CLK_32KHZ;;
  esac
}
