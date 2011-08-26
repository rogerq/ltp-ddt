/*
 * st_wdt_parser.h
 *
 * This file contains command line option definions
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
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

#ifndef _ST_WDT_PARSER_H_
#define _ST_WDT_PARSER_H_

/*
 * Define Options here, so that it would be easy to add new features/options.
 */
/* Device name on which wdt test is to be run */
#define	OPTION_DEVICE_NAME	1

/* execute the IOCTL test case */
#define OPTION_IOCTL		2

/* IOCTL name to execute */
#define OPTION_IOCTL_ARG	3

/* Loop Count i.e how many time to execute the particular test */
#define OPTION_LOOP		4

/* execute the write test case */
#define OPTION_WRITE		5

/* Test case id string for logging purpose */
#define OPTION_TESTCASE_ID	6

/* Displays the version of LFTB package */
#define OPTION_VERSION		'v'
/* Displays the help */
#define OPTION_HELP		'h'

#define ST_DEFAULT_OPTIONS	WDIOS_ENABLECARD
#define ST_DEFAULT_TIMEOUT	60
#define ST_DEFAULT_PRETIMEOUT	50
#define DEFAULT_LOOP_COUNT	1
#define DEFAULT_IOCTL_NAME	"keepalive"

/* Table which holds the ioctl info */
struct wdt_ioctl_table ioctl_table[] = {
	{"keepalive", WDIOC_KEEPALIVE, 0, "WDIOC_KEEPALIVE"},
	{"settimeout", WDIOC_SETTIMEOUT, ST_DEFAULT_TIMEOUT,
	 "WDIOC_SETTIMEOUT"},
	{"gettimeout", WDIOC_GETTIMEOUT, 0, "WDIOC_GETTIMEOUT"},
	{"getsupport", WDIOC_GETSUPPORT, 0, ""},
	{"setoptions", WDIOC_SETOPTIONS, ST_DEFAULT_OPTIONS,
	 "WDIOC_SETOPTIONS"},
	{"getstatus", WDIOC_GETSTATUS, 0, "WDIOC_GETSTATUS"},
	{"gettimeleft", WDIOC_GETTIMELEFT, 0, "WDIOC_GETTIMELEFT"},
	{"getbootstatus", WDIOC_GETBOOTSTATUS, 0, "WDIOC_GETBOOTSTATUS"},
	{"getpretimeout", WDIOC_GETPRETIMEOUT, 0, "WDIOC_GETPRETIMEOUT"},
	{"setpretimeout", WDIOC_SETPRETIMEOUT, ST_DEFAULT_PRETIMEOUT,
	 "WDIOC_SETPRETIMEOUT"},
	{"gettemp", WDIOC_GETTEMP, 0, "WDIOC_GETTEMP"},
	{NULL, 0, 0, NULL}
};

/* Table which holds the support info of the WDT*/
struct supportinfo_table supportinfo_table[] = {
	{WDIOF_OVERHEAT, "Reset due to CPU overheat"},
	{WDIOF_FANFAULT, "Fan Failure"},
	{WDIOF_EXTERN1, "External Relay 1"},
	{WDIOF_EXTERN2, "External Relay 2"},
	{WDIOF_POWERUNDER, "Power Bad/Power Fault"},
	{WDIOF_CARDRESET, "Card Previously Reset The CPU"},
	{WDIOF_POWEROVER, "Power Over Voltage"},
	{WDIOF_SETTIMEOUT, "Set Timeout"},
	{WDIOF_MAGICCLOSE, "Supports Magic Close Char"},
	{WDIOF_PRETIMEOUT, "Pretimeout Get/Set"},
	{WDIOF_KEEPALIVEPING, "Keep Alive Ping Reply"},
	{0, NULL}
};

#endif /* #ifndef _ST_WDT_PARSER_H_ */
