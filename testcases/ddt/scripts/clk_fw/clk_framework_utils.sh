#!/bin/bash
#Clock Framework Utilities

#Function to obtain the Clock framework tree rooted at CLK_FRAMEWORK_ROOT
#including the clock values
get_clk_tree_values() {

  get_clk_tree

  for clock in $clock_tree;
  do
    echo $clock
    for clock_data in `find $clock -type f -maxdepth 1`;
    do
      echo $clock_data
      cat $clock_data
    done
  done
}

#Function to obtain the clocks provided by a ADPLLS takes the following
#parameters:
#       $1: input clock in Hz
#       $2: ADPLL multiplier M
#       $3: ADPLL divider N
#       $4: ADPLL divider M2
#       $5: ADPLL REGM4XEN M4
#       $6: ADPLL CLKINPHIF if any
#       $7: ADPLL divider M3
#Provides the following clock values in Hz:
#       ADPLLS_CLKOUT ADPLLS_CLKOUTX2 ADPLLS_CLKDCOLDO ADPLLS_CLKOUTHIF
adplls() {
  params_check $# 7
  local CLKINP=$1
  local M=$2
  local N=$3
  local M2=$4
  local M4=$5
  local CLKINPHIF=$6
  local M3=$7
  local common=$((CLKINP*(1+3*M4)*M/(N+1)))  
  ADPLLS_CLKDCOLDO=$((2*common))
  ADPLLS_CLKOUT=$((common/M2))
  ADPLLS_CLKOUTX2=$((ADPLLS_CLKDCOLDO/M2))
  if [ $CLKINPHIF -ne 0 ]
  then
    ADPLLS_CLKOUTHIF=$((CLKINPHIF/M3)) 
  else
    ADPLLS_CLKOUTHIF=$((ADPLLS_CLKDCOLDO/M3))
  fi
}

#Function to obtain the clocks provided by a ADPLLJ takes the following
#parameters:
#       $1: input clock in Hz
#       $2: ADPLL multiplier M
#       $3: ADPLL divider N
#       $4: ADPLL divider M2
#Provides the following clock values in Hz:
#       ADPLLJ_CLKOUT ADPLLJ_CLKOUTLDO ADPLLJ_CLKDCOLDO
adpllj() {
  params_check $# 4
  local CLKINP=$1
  local M=$2
  local N=$3
  local M2=$4
  ADPLLJ_CLKDCOLDO=$((CLKINP*M/(N+1)))
  ADPLLJ_CLKOUT=$((ADPLLJ_CLKDCOLDO/M2))
  ADPLLJ_CLKOUTLDO=$ADPLLJ_CLKOUT
}

#Function to read a register and provide the value of the specified register 
#fields, takes the following parameters:
#       $1: the register address in Hex
#       $2 to $N: one or more strings of syntax 
#          <high field bit>[-<low field bit>]
#Return: An array register_values with the values of the fields specified by
#        parameters $2 to $N. The array is zero based and the value are
#        indexed in the same order as parameter $2 to $N 
register_read() {
  local register_string=`devmem2 $1 w`
  if [ $? -ne "0" ]
  then
    echo "register_read: devmem2 call failed"
    exit 1
  fi
  local register=${register_string: -10}
  register_values=()
  for bit_field in ${@:2}
  do
    local bits=( $(echo $bit_field | tr '\-' " ") )
    if [ ${#bits[@]} -gt 1 ]
    then
      field_mask=$((0xFFFFFFFF >> (31+${bits[1]}-${bits[0]})))
      field_value=$(((register >> ${bits[1]}) & field_mask))
    else
      field_value=$(((register >> bits) & 1))
    fi
    register_values[${#register_values[*]}]=$field_value
  done
}

set_tree_input() {
  local regs=("${!1}")
  local current_reg
  for idx in $(seq 0 $((${#regs[@]}-1)))
  do
     current_reg=( ${regs[$idx]} )
     register_read ${current_reg[@]:1}
     eval ${current_reg[0]}=${register_values[0]}
  done
}

#Function to obtain the clocks provided by a type A of Type B DPLL, takes the
#following parameters:
#    $1: input clock in Hz
#    $2: DPLL multiplier M
#    $3: DPLL divider N
#    $4: DPLL divider M2
#    $5: DPLL REGM4XEN M4
#    $6: DPLL BYP_CLK_M3 in Hz (or 0 in non-bypass)
#    $7: DPLL divider M3
#    $8-11: (optionals) H1x dividers
#    $12-15: (optionals) H2x dividers
#Provides the following clock values in Hz:
#    CLKOUT_M2 CLKOUTX2_M2 CLK_DCO_LDO CLKOUTX2_M3 
#    CLKOUTX2_H1x (x from 1 to 4) CLKOUTX2_H2x (x from 1 to 4)
ab_dpll() {
  local dpll_params=( ${@:1:7} )
  adplls ${dpll_params[@]}
  local hsdivs=( ${@:8:8} )
  for (( idx=${#hsdivs[@]}; idx<8; idx++ ))
  do
    hsdivs[$idx]=0
  done
  CLKOUT_M2=$ADPLLS_CLKOUT
  CLKOUTX2_M2=$ADPLLS_CLKOUTX2
  CLK_DCO_LDO=$ADPLLS_CLKDCOLDO
  CLKOUTX2_M3=$ADPLLS_CLKOUTHIF
  for (( idx=0; idx<${#hsdivs[@]}; idx++ ))
  do
    if [ $idx -lt 4 ]
    then
      eval "CLKOUTX2_H1$((idx+1))=$((ADPLLS_CLKDCOLDO/(${hsdivs[$idx]}+1)))"
    else
      eval "CLKOUTX2_H2$((idx-3))=$((ADPLLS_CLKDCOLDO/(${hsdivs[$idx]}+1)))"
    fi
  done
}

#Function to verify the number of parameters passed to a function, takes the
# following parameters
#    $1: Number of parameters passed to the function i.e $#
#    $2: Number of parameters required by the function
params_check() {
  if [ $1 -ne $2 ]
  then
    if [ $# -eq 3 ]
    then
      echo $3
    else
      echo "Function requires $2 parameters, $1 parameters specified"
    fi
    i=0
    while caller $i
    do
      i=$((i+1))
    done 
    exit 2
  fi
}
