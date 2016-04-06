#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Tools to calc bit timing for CAN bus.
 * Based on CAN Linux driver.
 */

#define CAN_CALC_MAX_ERROR 50

/*
 * CAN bit-timing parameters
 *
 * For futher information, please read chapter "8 BIT TIMING
 * REQUIREMENTS" of the "Bosch CAN Specification version 2.0"
 * at http://www.semiconductors.bosch.de/pdf/can2spec.pdf.
 */
struct can_bittiming {
	uint32_t bitrate;		/* Bit-rate in bits/second */
	uint32_t sample_point;	/* Sample point in one-tenth of a percent */
	uint32_t tq;		/* Time quanta (TQ) in nanoseconds */
	uint32_t prop_seg;		/* Propagation segment in TQs */
	uint32_t phase_seg1;	/* Phase buffer segment 1 in TQs */
	uint32_t phase_seg2;	/* Phase buffer segment 2 in TQs */
	uint32_t sjw;		/* Synchronisation jump width in TQs */
	uint32_t brp;		/* Bit-rate prescaler */
};

/*
 * CAN harware-dependent bit-timing constant
 *
 * Used for calculating and checking bit-timing parameters
 */
struct can_bittiming_const {
	char name[16];		/* Name of the CAN controller hardware */
	uint32_t tseg1_min;	/* Time segement 1 = prop_seg + phase_seg1 */
	uint32_t tseg1_max;
	uint32_t tseg2_min;	/* Time segement 2 = phase_seg2 */
	uint32_t tseg2_max;
	uint32_t sjw_max;		/* Synchronisation jump width */
	uint32_t brp_min;		/* Bit-rate prescaler */
	uint32_t brp_max;
	uint32_t brp_inc;
};

/*
 * Bit-timing calculation derived from:
 *
 * Code based on LinCAN sources and H8S2638 project
 * Copyright 2004-2006 Pavel Pisa - DCE FELK CVUT cz
 * Copyright 2005      Stanislav Marek
 * email: pisa@cmp.felk.cvut.cz
 *
 * Calculates proper bit-timing parameters for a specified bit-rate
 * and sample-point, which can then be used to set the bit-timing
 * registers of the CAN controller. You can find more information
 * in the header file linux/can/netlink.h.
 */
static int can_update_spt(const struct can_bittiming_const *btc,
			  int sampl_pt, int tseg, int *tseg1, int *tseg2)
{
	*tseg2 = tseg + 1 - (sampl_pt * (tseg + 1)) / 1000;
	if (*tseg2 < btc->tseg2_min)
		*tseg2 = btc->tseg2_min;
	if (*tseg2 > btc->tseg2_max)
		*tseg2 = btc->tseg2_max;
	*tseg1 = tseg - *tseg2;
	if (*tseg1 > btc->tseg1_max) {
		*tseg1 = btc->tseg1_max;
		*tseg2 = tseg - *tseg1;
	}
	return 1000 * (tseg + 1 - *tseg2) / (tseg + 1);
}

int can_calc_bittiming(struct can_bittiming_const *btc, struct can_bittiming *bt)
{
	long rate, best_rate = 0;
	long best_error = 1000000000, error = 0;
	int best_tseg = 0, best_brp = 0, brp = 0;
	int tsegall, tseg = 0, tseg1 = 0, tseg2 = 0;
	int spt_error = 1000, spt = 0, sampl_pt;
	uint64_t v64;
	int freq = 20000000;

	/* Use CIA recommended sample points */
	if (bt->sample_point) {
		sampl_pt = bt->sample_point;
	} else {
		if (bt->bitrate > 800000)
			sampl_pt = 750;
		else if (bt->bitrate > 500000)
			sampl_pt = 800;
		else
			sampl_pt = 875;
	}

	/* tseg even = round down, odd = round up */
	for (tseg = (btc->tseg1_max + btc->tseg2_max) * 2 + 1;
	     tseg >= (btc->tseg1_min + btc->tseg2_min) * 2; tseg--) {
		tsegall = 1 + tseg / 2;
		/* Compute all possible tseg choices (tseg=tseg1+tseg2) */
		brp = freq / (tsegall * bt->bitrate) + tseg % 2;
		/* chose brp step which is possible in system */
		brp = (brp / btc->brp_inc) * btc->brp_inc;
		if ((brp < btc->brp_min) || (brp > btc->brp_max))
			continue;
		rate = freq / (brp * tsegall);
		error = bt->bitrate - rate;
		/* tseg brp biterror */
		if (error < 0)
			error = -error;
		if (error > best_error)
			continue;
		best_error = error;
		if (error == 0) {
			spt = can_update_spt(btc, sampl_pt, tseg / 2,
					     &tseg1, &tseg2);
			error = sampl_pt - spt;
			if (error < 0)
				error = -error;
			if (error > spt_error)
				continue;
			spt_error = error;
		}
		best_tseg = tseg / 2;
		best_brp = brp;
		best_rate = rate;
		if (error == 0)
			break;
	}

	if (best_error) {
		/* Error in one-tenth of a percent */
		error = (best_error * 1000) / bt->bitrate;
		if (error > CAN_CALC_MAX_ERROR) {
			printf("bitrate error %ld.%ld%% too high\n", error / 10, error % 10);
			return -1;
		} else {
			printf("bitrate error %ld.%ld%%\n", error / 10, error % 10);
		}
	}

	/* real sample point */
	bt->sample_point = can_update_spt(btc, sampl_pt, best_tseg, &tseg1, &tseg2);

	v64 = (uint64_t)best_brp * 1000000000UL;
	v64 /= freq;
	bt->tq = (uint32_t)v64;
	bt->prop_seg = tseg1 / 2;
	bt->phase_seg1 = tseg1 - bt->prop_seg;
	bt->phase_seg2 = tseg2;
	bt->sjw = 1;
	bt->brp = best_brp;
	/* real bit-rate */
	bt->bitrate = freq / (bt->brp * (tseg1 + tseg2 + 1));

	return 0;
}

int main(void)
{
	struct can_bittiming_const btc = { "TI", 0, 8, 0, 8, 4, 1, 1024, 1 };
	struct can_bittiming bt = { 500000, 0, 0, 0, 0, 0, 0, 0 };
	int error;

	error = can_calc_bittiming(&btc, &bt);
	if (!error) {
		printf("Results :\n\tbitrate : %u\n\tsample_point : %u\n\ttq (ns) : %u\n\tprop_seg : %u\n\t"
		       "phase_seg1 : %u\n\tphase_seg2 : %u\n\tsjw : %u\n\tbrp : %u\n", bt.bitrate, bt.sample_point,
		       bt.tq, bt.prop_seg, bt.phase_seg1, bt.phase_seg2, bt.sjw, bt.brp);
	}

	return error;
}
