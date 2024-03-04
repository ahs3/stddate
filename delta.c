/*
 * delta: print the delta between SRT epoch start and Linux epoch start,
 *        in seconds
 *
 * Copyright (c) 2024, Al Stone <ahs3@ahs3.net>
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * No arguments => stddate right now
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#if defined(DEBUG)
bool DO_DEBUG = true;
#else
bool DO_DEBUG = false;
#endif

const char *VERSION = "0.0.1";

/* 
 * Date and time Neil Armstrong stepped on the moon:
 * July 16, 1969 13:32:00 UTC (Universal Coordinated Time)
 *
 */
struct tm SRT_EPOCH = {
	.tm_sec = 0,
	.tm_min = 32,
	.tm_hour = 13,
	.tm_mday = 16,
	.tm_mon = 6,
	.tm_year = 69,
	.tm_isdst = 0,
	.tm_gmtoff = 0
};
time_t srt_epoch = 0;

struct tm SYS_EPOCH = {
	.tm_sec = 0,
	.tm_min = 0,
	.tm_hour = 0,
	.tm_mday = 1,
	.tm_mon = 0,
	.tm_year = 70,
	.tm_isdst = 0,
	.tm_gmtoff = 0
};
time_t sys_epoch = 0;

struct tm PICO_EPOCH = {
	.tm_sec = 0,
	.tm_min = 0,
	.tm_hour = 0,
	.tm_mday = 1,
	.tm_mon = 0,
	.tm_year = 121,
	.tm_isdst = 0,
	.tm_gmtoff = 0
};
time_t pico_epoch = 0;

/*
 * Standard Reference Time:
 *    1   SI second   = 1 std second
 *    100 std seconds = 1 std minute
 *    100 std minutes = 1 std hour
 *    10  std hours   = 1 std day
 *    10  std days    = 1 std week
 *    10  std weeks   = 1 std month
 *    10  std months   = 1 std year
 */
struct std_tm {
	long stm_sec;
	long stm_min;
	long stm_hour;
	long stm_day;
	long stm_week;
	long stm_month;
	long stm_year;
};

char *std_tm_str(struct std_tm *stddate) {
	/* you must free() this later */
	int size = 30;
	char *asc = (char *)malloc(size);

	sprintf(asc, "%04d.%1d.%1d.%1d %01d:%02d:%02d",
		stddate->stm_year, stddate->stm_month, stddate->stm_week,
		stddate->stm_day, stddate->stm_hour, stddate->stm_min,
		stddate->stm_sec);
	return asc;
}

int main(int argc, char *argv[]) {
	int res;

	if (DO_DEBUG)
		printf("delta, v%s.\n", VERSION);

	srt_epoch = mktime(&SRT_EPOCH) - timezone;
	sys_epoch = mktime(&SYS_EPOCH) - timezone;
	pico_epoch = mktime(&PICO_EPOCH) - timezone;
	printf("timezone:   %11ld\n", (long)timezone);
	printf("srt_epoch:  %11ld  %s", (long)srt_epoch,
		asctime(gmtime(&srt_epoch)));
	printf("sys_epoch:  %11ld  %s", (long)sys_epoch,
		asctime(gmtime(&sys_epoch)));
	printf("pico_epoch: %11ld  %s", (long)pico_epoch,
		asctime(gmtime(&pico_epoch)));
	printf("srt secs from sys:    %11ld\n", (long)srt_epoch);
	printf("srt secs from pico:   %11ld\n",
		(long)pico_epoch - (long)srt_epoch);
	printf("srt secs from local:  %11ld\n",
		(long)time(NULL) - (long)srt_epoch);

	return 0;
}
