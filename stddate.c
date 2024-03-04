/*
 * stddate -- calculate std date, an experiment in "decimalized" time
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

const char *VERSION = "0.2.1";

/* 
 * Date and time Neil Armstrong stepped on the moon:
 * July 16, 1969 13:32:00 UTC (Universal Coordinated Time)
 *
 */
struct tm EPOCH = {
	.tm_sec = 0,
	.tm_min = 32,
	.tm_hour = 13,
	.tm_mday = 16,
	.tm_mon = 6,
	.tm_year = 69,
};
time_t std_epoch = 0;

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

int std_time(time_t when, struct std_tm *stddate) {
	int res = 0;
	long val = (long)when;

	if (DO_DEBUG) {
		printf("Elapsed since SRT epoch:\n");
		printf("    %ld std secs\n", (long)when - (long)std_epoch);
	}

	stddate->stm_sec = val % 100;
	val = (long)(val/100);
	stddate->stm_min = val % 100;
	val = (long)(val/100);
	stddate->stm_hour = val % 10;
	val = (long)(val/10);
	stddate->stm_day = val % 10;
	val = (long)(val/10);
	stddate->stm_week = val % 10;
	val = (long)(val/10);
	stddate->stm_month = val % 10;
	val = (long)(val/10);
	stddate->stm_year = val;

	if (DO_DEBUG) {
		long tmp;

		val = when;
		tmp = val % 100;
		val = (long)(val/100);
		printf("    %02d stm_sec   [%ld]\n", stddate->stm_sec, tmp);
		tmp = val % 100;
		val = (long)(val/100);
		printf("    %02d stm_min   [%ld]\n", stddate->stm_min, tmp);
		tmp = val % 10;
		val = (long)(val/10);
		printf("     %01d stm_hour  [%ld]\n", stddate->stm_hour, tmp);
		tmp = val % 10;
		val = (long)(val/10);
		printf("     %01d stm_day   [%ld]\n", stddate->stm_day, tmp);
		tmp = val % 10;
		val = (long)(val/10);
		printf("     %01d stm_week  [%ld]\n", stddate->stm_week, tmp);
		tmp = val % 10;
		val = (long)(val/10);
		printf("     %01d stm_month [%ld]\n", stddate->stm_month, tmp);
		tmp = val;
		printf("  %04d stm_year  [%ld]\n", stddate->stm_year, tmp);
	}

	return res;
}

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

void usage () {
	printf("stddate, v%s.\n", VERSION);
	printf("usage: stddate [-c [n]] [-h]\n");
	printf("where:\n");
	printf("   -c n => continuous display for n seconds.\n");
	printf("   -C   => non-stop continuous display.\n");
	printf("   -h   => display this help message.\n");
}

int main(int argc, char *argv[]) {
	int res, opt;
	time_t now, std_epoch;
	struct std_tm current;
	long delta, years, days, hours, minutes, seconds;
	int second, last_second = -1;

	bool do_continuous = false;
	int limit = 10;
	bool do_forever = false;

	char *optstring = "c:Ch";

	if (DO_DEBUG)
		printf("stddate, v%s.\n", VERSION);

	while ((opt = getopt(argc, argv, optstring)) != -1) {
		switch (opt) {
		case 'c':
			do_continuous = true;
			if (optarg)
				limit = atoi(optarg);
			break;
		case 'C':
			do_forever = true;
			break;
		case 'h':
			usage();
		default:
			exit(1);
		}
	}

	std_epoch = mktime(&EPOCH);
	if (do_continuous) {
		now = time(NULL);
		res = std_time(now - std_epoch, &current);
		if (res) {
			printf("? errno: %d\n", res);
			exit(res);
		}

		do {
			if (current.stm_sec != last_second) {
				printf("%s\r", std_tm_str(&current));
				fflush(stdout);
				limit--;
			}
			usleep(10);
			last_second = current.stm_sec;

			now = time(NULL);
			res = std_time(now - std_epoch, &current);
			if (res) {
				printf("? errno: %d\n", res);
				exit(res);
			}
		}
		while (limit);
		printf("\n");

	} else if (do_forever) {
		now = time(NULL);
		res = std_time(now - std_epoch, &current);
		if (res) {
			printf("? errno: %d\n", res);
			exit(res);
		}

		do {
			if (current.stm_sec != last_second) {
				printf("%s\r", std_tm_str(&current));
				fflush(stdout);
			}
			usleep(10);
			last_second = current.stm_sec;

			now = time(NULL);
			res = std_time(now - std_epoch, &current);
			if (res) {
				printf("? errno: %d\n", res);
				exit(res);
			}
		}
		while (true);
		printf("\n");

	} else {
		if (DO_DEBUG) {
			printf("SRT epoch (UTC):    %s", ctime(&std_epoch));
			printf("   seconds since Unix epoch: %ld\n", std_epoch);
			printf("   timezone seconds: %ld\n", timezone);
		}

		now = time(NULL);
		printf("Current time (UTC):            %s",
			asctime(gmtime(&now)));
		printf("Current time (local):          %s",
			asctime(localtime(&now)));

		res = std_time(now - std_epoch, &current);
		if (res) {
			printf("? errno: %d\n", res);
			exit(res);
		} else {
			if (DO_DEBUG) {
				printf("Standard Reference Time (SRT):\n");
				printf("    yyyy.m.w.d h:mm:ss => %s\n",
			       		std_tm_str(&current));
			} else {
				printf("Standard Reference Time (SRT): %s\n",
			       		std_tm_str(&current));
			}
		}
	}

	return 0;
}
