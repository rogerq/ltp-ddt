#!/bin/bash
source st_log.sh
source am335x_clk_framework_utils.sh

am335x_CLK_TREE=()
add_to_tree() {
  am335x_CLK_TREE[${#am335x_CLK_TREE[*]}]=$1=$2
}

#A file that defines the following variables (commented variables only) and 
#sources this file must be created to obtain a clk tree.

#MASTER clocks sources
CLK_M_OSC=24000000 #From crystal
CLK_RC32K=32768 #From On-Chip 32.768 KHz Osc
CLK_32K_RTC=32768 #From external 32.768 KHz Osc

#Core PLL settings
#CORE_PLL_M=125 
#CORE_PLL_N=2
#CORE_PLL_REGXM4=0
#CORE_HSDIV_M4=10
#CORE_HSDIV_M5=8
#CORE_HSDIV_M6=4
#CORE_PLL_PRCM_MUXA=0
#CORE_PLL_PRCM_DIVB=0
#CORE_PLL_PRCM_MUXC=0
#CORE_PLL_PRCM_MUXD=1
CORE_PLL_MHZ_50_DIV=5
#DISP_CLKOUT=0

#Peripheral PLL settings
#PER_PLL_M=40
#PER_PLL_N=0
#PER_PLL_M2=5
PER_PLL_DIV_MUX=0

#MPU PLL settings
#MPU_PLL_M=550
#MPU_PLL_N=23
#MPU_PLL_M2=1
#MPU_PLL_REGXM4=0

#DDR PLL settings
#DDR_PLL_M=266
#DDR_PLL_N=23
#DDR_PLL_M2=1
#DDR_PLL_REGXM4=0

#Display PLL settings
#DISP_PLL_M=200
#DISP_PLL_N=23
#DISP_PLL_M2=1
#DISP_PLL_REGXM4=0 
#DISP_PLL_PRCM_MUX=0

#Timers setting
TCLKIN=120000000
#TIMER1_MUX=0
#TIMER2_MUX=1
#TIMER3_MUX=1
#TIMER4_MUX=1
#TIMER5_MUX=1
#TIMER6_MUX=1
#TIMER7_MUX=1

#End of fields definition


#Variable to define the xstal clk used in the board
src_clk=virt_${CLK_M_OSC}_ck

#test_print_trc "Processing 32.768KHz clocks"
add_to_tree clk_32768_ck $CLK_32K_RTC
add_to_tree clk_32768_ck/sysclkout_pre_ck $CLK_32K_RTC
add_to_tree clk_32768_ck/sysclkout_pre_ck/clkout2_div_ck $CLK_32K_RTC
add_to_tree clk_32768_ck/sysclkout_pre_ck/clkout2_div_ck/clkout2_ck $CLK_32K_RTC
add_to_tree clk_rc32k_ck $CLK_RC32K
#add_to_tree clk_rc32k_ck/wdt1_fck $CLK_RC32K 
add_to_tree clk_rc32k_ck/gpio0_dbclk_mux_ck $CLK_RC32K 
add_to_tree clk_rc32k_ck/gpio0_dbclk_mux_ck/gpio0_dbclk $CLK_RC32K
#test_print_trc "Processing Crystal"
add_to_tree virt_19200000_ck 19200000
add_to_tree virt_24000000_ck 24000000
add_to_tree ${src_clk}/sys_clkin_ck $CLK_M_OSC
#test_print_trc "Processing Core PLL"
am335x_per_pll $CLK_M_OSC $PER_PLL_M $PER_PLL_N $PER_PLL_M2 $PER_PLL_DIV_MUX
PER_PLL_CLKDCOLDO=$ADPLLS_CLKDCOLDO
am335x_core_pll $CLK_M_OSC $CORE_PLL_M $CORE_PLL_N $CORE_PLL_REGXM4 $CORE_HSDIV_M4 $CORE_HSDIV_M5 $CORE_HSDIV_M6 $CORE_PLL_PRCM_MUXA $CORE_PLL_PRCM_DIVB $CORE_PLL_PRCM_MUXC $CORE_PLL_PRCM_MUXD $PER_CLKOUTM2 $PER_CLKOUTM2 $CORE_PLL_MHZ_50_DIV
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck $((ADPLLS_CLKDCOLDO/2))

#add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_x2_ck

add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck $ADPLLS_CLKDCOLDO
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck $CORE_CLKOUTM4
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/mmu_fck $((CORE_CLKOUTM4/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/l4_rtc_gclk $((CORE_CLKOUTM4/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/l3_gclk $CORE_CLKOUTM4
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/l3_gclk/pruss_ocp_gclk $PRUICSSOCP_CLK
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/dpll_core_m4_div2_ck $L3S_L4PER_L4WKUP_CLK
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/dpll_core_m4_div2_ck/ieee5000_fck $((CORE_CLKOUTM4/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/dpll_core_m4_div2_ck/l3s_gclk $((CORE_CLKOUTM4/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/dpll_core_m4_div2_ck/l4fw_gclk $((CORE_CLKOUTM4/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/dpll_core_m4_div2_ck/l4ls_gclk $((CORE_CLKOUTM4/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/l4hs_gclk $CORE_CLKOUTM4
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/sysclk_div_ck $CORE_CLKOUTM4
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/gfx_fclk_clksel_ck $SGX_CLKSEL
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/gfx_fclk_clksel_ck/gfx_fck_div_ck $SGX_CORECLK
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/dbg_clka_ck $CORE_CLKOUTM4
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/dbg_clka_ck/stm_pmd_clock_mux_ck $CORE_CLKOUTM4
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/dbg_clka_ck/stm_pmd_clock_mux_ck/stm_clk_div_ck $((CORE_CLKOUTM4/4))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/dbg_clka_ck/trace_pmd_clk_mux_ck $CORE_CLKOUTM4
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/dbg_clka_ck/trace_pmd_clk_mux_ck/trace_clk_div_ck $((CORE_CLKOUTM4/4))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/ehrpwm0_tbclk $((CORE_CLKOUTM4/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/ehrpwm1_tbclk $((CORE_CLKOUTM4/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m4_ck/ehrpwm2_tbclk $((CORE_CLKOUTM4/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m5_ck $CORE_CLKOUTM5
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m5_ck/cpsw_125mhz_gclk $MHZ_125_CLK
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m5_ck/cpsw_cpts_rft_clk $CPTSRFT_CLK
add_to_tree ${src_clk}/sys_clkin_ck/dpll_core_ck/dpll_core_dcoldo_ck/dpll_core_m6_ck $CORE_CLKOUTM6

#test_print_trc "Processing MPU PLL"
am335x_mpu_pll $CLK_M_OSC $MPU_PLL_M $MPU_PLL_N $MPU_PLL_M2 $MPU_PLL_REGXM4 
add_to_tree ${src_clk}/sys_clkin_ck/dpll_mpu_ck $((ADPLLS_CLKDCOLDO/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_mpu_ck/dpll_mpu_m2_ck $MPU_CLK

#test_print_trc "Process DDR PLL"
am335x_ddr_pll $CLK_M_OSC $DDR_PLL_M $DDR_PLL_N $DDR_PLL_M2 $DDR_PLL_REGXM4 
add_to_tree ${src_clk}/sys_clkin_ck/dpll_ddr_ck $((ADPLLS_CLKDCOLDO/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_ddr_ck/dpll_ddr_m2_ck $ADPLLS_CLKOUT
add_to_tree ${src_clk}/sys_clkin_ck/dpll_ddr_ck/dpll_ddr_m2_ck/dpll_ddr_m2_div2_ck $EMIF_M_CLK

#test_print_trc "Processing Display PLL"
am335x_display_pll $CLK_M_OSC $DISP_PLL_M $DISP_PLL_N $DISP_PLL_M2 $DISP_PLL_REGXM4 $CORE_CLKOUTM5 $PER_CLKOUTM2 $DISP_PLL_PRCM_MUX
add_to_tree ${src_clk}/sys_clkin_ck/dpll_disp_ck $((ADPLLS_CLKDCOLDO/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_disp_ck/dpll_disp_m2_ck $LCD_CLK
add_to_tree ${src_clk}/sys_clkin_ck/dpll_disp_ck/dpll_disp_m2_ck/lcd_gclk $LCD_CLK

#test_print_trc "Process Peripheral PLL"
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck $((PER_PLL_CLKDCOLDO/2))
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck/dpll_per_m2_ck $PER_CLKOUTM2
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck/dpll_per_m2_ck/dpll_per_m2_div4_wkupdm_ck $PER_CLK48
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck/dpll_per_m2_ck/dpll_per_m2_div4_ck $PER_CLK48
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck/dpll_per_m2_ck/clk_24mhz $PER_CLK24
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck/dpll_per_m2_ck/clk_24mhz/clkdiv32k_ck $CLK_32KHZ
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck/dpll_per_m2_ck/clk_24mhz/clkdiv32k_ck/clkdiv32k_ick $CLK_32KHZ
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck/dpll_per_m2_ck/clk_24mhz/clkdiv32k_ck/clkdiv32k_ick/gpio1_dbclk $CLK_32KHZ
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck/dpll_per_m2_ck/clk_24mhz/clkdiv32k_ck/clkdiv32k_ick/gpio2_dbclk $CLK_32KHZ
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck/dpll_per_m2_ck/clk_24mhz/clkdiv32k_ck/clkdiv32k_ick/gpio3_dbclk $CLK_32KHZ
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck/dpll_per_m2_ck/clk_24mhz/clkdiv32k_ck/clkdiv32k_ick/wdt1_fck $CLK_32KHZ 
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck/dpll_per_m2_ck/mmc_clk $MMC_CLK
add_to_tree ${src_clk}/sys_clkin_ck/dpll_per_ck/usbotg_fck $USB_PHY_CLK
add_to_tree ${src_clk}/sys_clkin_ck/adc_tsc_fck $CLK_M_OSC
#CEFUSE taken from linux clk tree need to research value
add_to_tree ${src_clk}/sys_clkin_ck/cefuse_fck $CLK_M_OSC
add_to_tree ${src_clk}/sys_clkin_ck/dcan0_fck $CLK_M_OSC
add_to_tree ${src_clk}/sys_clkin_ck/dcan1_fck $CLK_M_OSC
add_to_tree ${src_clk}/sys_clkin_ck/mcasp0_fck $CLK_M_OSC
add_to_tree ${src_clk}/sys_clkin_ck/mcasp1_fck $CLK_M_OSC
add_to_tree ${src_clk}/sys_clkin_ck/smartreflex0_fck $L3S_L4PER_L4WKUP_CLK
add_to_tree ${src_clk}/sys_clkin_ck/smartreflex1_fck $L3S_L4PER_L4WKUP_CLK
add_to_tree ${src_clk}/sys_clkin_ck/sha0_fck $CLK_M_OSC
add_to_tree ${src_clk}/sys_clkin_ck/aes0_fck $CLK_M_OSC
am335x_timer_clkin $TIMER1_MUX 
add_to_tree ${src_clk}/sys_clkin_ck/timer1_fck $TIMER_CLK
am335x_timer_clkin $TIMER2_MUX
add_to_tree ${src_clk}/sys_clkin_ck/timer2_fck $TIMER_CLK
am335x_timer_clkin $TIMER4_MUX
add_to_tree ${src_clk}/sys_clkin_ck/timer4_fck $TIMER_CLK
am335x_timer_clkin $TIMER5_MUX
add_to_tree ${src_clk}/sys_clkin_ck/timer5_fck $TIMER_CLK
am335x_timer_clkin $TIMER7_MUX
add_to_tree ${src_clk}/sys_clkin_ck/timer7_fck $TIMER_CLK
add_to_tree ${src_clk}/sys_clkin_ck/dbg_sysclk_ck $CLK_M_OSC
am335x_timer_clkin $TIMER3_MUX
add_to_tree ${src_clk}/sys_clkin_ck/timer3_fck $TIMER_CLK
am335x_timer_clkin $TIMER6_MUX
add_to_tree ${src_clk}/sys_clkin_ck/timer6_fck $TIMER_CLK
add_to_tree virt_25000000_ck 25000000
add_to_tree virt_26000000_ck 26000000
add_to_tree tclkin_ck $TCLKIN
#add_to_tree orphans

echo ${am335x_CLK_TREE[@]}

