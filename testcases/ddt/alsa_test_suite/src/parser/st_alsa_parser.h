/*
 * st_alsa_parser.h
 *
 * This file contains command line option definions
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/  
    
#ifndef _ST_alsa_PARSER_H_
#define _ST_alsa_PARSER_H_
    
/*
 * Define Options here, so that it would be easy to add new features/options.
 */ 
    
/* 
 *access type
 */ 
    
#define OPTION_ACCESS	1
    
/* 
 *Number of channesl
 */ 
#define OPTION_CHANNEL	2
    
/* 
 *Device name
 */ 
#define OPTION_DEVICE	3
    
/* 
 * Format 
 */ 
#define OPTION_FORMAT 4
    
/* 
 *Name of file
 */ 
#define OPTION_FILE	5
    
/* 
 * Period size
 */ 
#define OPTION_PERIODSIZE	6
    
/* 
 * Total size
 */ 
    
#define OPTION_TOTALSIZE	7
    
/* 
 *sampling rate
 */ 
#define OPTION_RATE	8
    
/* 
 *Playback
 */ 
#define OPTION_PLAYBACK	9
    
/* 
 * Record
 */ 
#define OPTION_RECORD 10
    
/* 
 * Loopback
 */ 
#define OPTION_LOOPBACK 11
    
/* 
 *Non block mode 
 */ 
#define OPTION_NONBLOCK	12
    
/* 
 * Test case id string for logging purpose 
 */ 
#define OPTION_TESTCASE_ID	13
    
/* 
 * Enables performance calculation
 */ 
#define OPTION_PERFORMANCE 14
    
/*
 *Displays the version of LFTB package 
 */ 
#define OPTION_VERSION	'v'
    
/* 
 *Displays the help
 */ 
#define OPTION_HELP	'h'
    
#define CPULOAD "cpuload"
    
#define THROUGHPUT "throughput"
    
#endif	/* #ifndef _ST_alsa_PARSER_H_ */
    
