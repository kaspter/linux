

#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/slab.h>

#include "dvb_frontend.h"
#include "jetfe.h"


struct jetfe_state {
	struct dvb_frontend frontend;
};


static int jetfe_read_status(struct dvb_frontend* fe, fe_status_t* status)
{
	*status = FE_HAS_SIGNAL | FE_HAS_CARRIER | FE_HAS_VITERBI | FE_HAS_SYNC | FE_HAS_LOCK;

	return 0;
}

static int jetfe_read_ber(struct dvb_frontend* fe, u32* ber)
{
	struct jetfe_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;

	switch (c->delivery_system) {
	case SYS_DVBS:
	case SYS_DVBS2:
		 *ber = 0;
		 break;
	default:
		  return 1;
	}

	return 0;
}

static int jetfe_read_signal_strength(struct dvb_frontend* fe, u16* strength)
{
	*strength = 0;
	return 0;
}

static int jetfe_read_snr(struct dvb_frontend* fe, u16* snr)
{
	*snr = 0;
	return 0;
}

static int jetfe_read_ucblocks(struct dvb_frontend* fe, u32* ucblocks)
{
	*ucblocks = 0;
	return 0;
}

/*
 * Only needed if it actually reads something from the hardware
 */
static int jetfe_get_frontend(struct dvb_frontend *fe)
{
	return 0;
}

static int jetfe_set_frontend(struct dvb_frontend *fe)
{
	if (fe->ops.tuner_ops.set_params) {
		fe->ops.tuner_ops.set_params(fe);
		if (fe->ops.i2c_gate_ctrl)
			fe->ops.i2c_gate_ctrl(fe, 0);
	}

	return 0;
}

static int jetfe_sleep(struct dvb_frontend* fe)
{
	return 0;
}

static int jetfe_init(struct dvb_frontend* fe)
{
	return 0;
}

static int jetfe_set_tone(struct dvb_frontend* fe, fe_sec_tone_mode_t tone)
{
	return 0;
}

static int jetfe_set_voltage(struct dvb_frontend* fe, fe_sec_voltage_t voltage)
{
	return 0;
}

static void jetfe_release(struct dvb_frontend* fe)
{
	struct jetfe_state* state = fe->demodulator_priv;
	kfree(state);
}

static struct dvb_frontend_ops jetfe_ofdm_ops;

struct dvb_frontend* jetfe_ofdm_attach(void)
{
	struct jetfe_state* state = NULL;

	/* allocate memory for the internal state */
	state = kzalloc(sizeof(struct jetfe_state), GFP_KERNEL);
	if (!state)
		return NULL;

	/* create dvb_frontend */
	memcpy(&state->frontend.ops, &jetfe_ofdm_ops, sizeof(struct dvb_frontend_ops));
	state->frontend.demodulator_priv = state;
	return &state->frontend;
}

static struct dvb_frontend_ops jetfe_qpsk_ops;

struct dvb_frontend *jetfe_qpsk_attach(void)
{
	struct jetfe_state* state = NULL;

	/* allocate memory for the internal state */
	state = kzalloc(sizeof(struct jetfe_state), GFP_KERNEL);
	if (!state)
		return NULL;

	/* create dvb_frontend */
	memcpy(&state->frontend.ops, &jetfe_qpsk_ops, sizeof(struct dvb_frontend_ops));
	state->frontend.demodulator_priv = state;
	return &state->frontend;
}

static struct dvb_frontend_ops jetfe_qam_ops;

struct dvb_frontend *jetfe_qam_attach(void)
{
	struct jetfe_state* state = NULL;

	/* allocate memory for the internal state */
	state = kzalloc(sizeof(struct jetfe_state), GFP_KERNEL);
	if (!state)
		return NULL;

	/* create dvb_frontend */
	memcpy(&state->frontend.ops, &jetfe_qam_ops, sizeof(struct dvb_frontend_ops));
	state->frontend.demodulator_priv = state;
	return &state->frontend;
}

static struct dvb_frontend_ops jetfe_ofdm_ops = {
	.delsys = { SYS_DVBT },
	.info = {
		.name			= "Dummy DVB-T",
		.frequency_min		= 0,
		.frequency_max		= 863250000,
		.frequency_stepsize	= 62500,
		.caps = FE_CAN_FEC_1_2 | FE_CAN_FEC_2_3 | FE_CAN_FEC_3_4 |
				FE_CAN_FEC_4_5 | FE_CAN_FEC_5_6 | FE_CAN_FEC_6_7 |
				FE_CAN_FEC_7_8 | FE_CAN_FEC_8_9 | FE_CAN_FEC_AUTO |
				FE_CAN_QAM_16 | FE_CAN_QAM_64 | FE_CAN_QAM_AUTO |
				FE_CAN_TRANSMISSION_MODE_AUTO |
				FE_CAN_GUARD_INTERVAL_AUTO |
				FE_CAN_HIERARCHY_AUTO,
	},

	.release = jetfe_release,

	.init = jetfe_init,
	.sleep = jetfe_sleep,

	.set_frontend = jetfe_set_frontend,
	.get_frontend = jetfe_get_frontend,

	.read_status = jetfe_read_status,
	.read_ber = jetfe_read_ber,
	.read_signal_strength = jetfe_read_signal_strength,
	.read_snr = jetfe_read_snr,
	.read_ucblocks = jetfe_read_ucblocks,
};

static struct dvb_frontend_ops jetfe_qam_ops = {
	.delsys = { SYS_DVBC_ANNEX_A },
	.info = {
		.name			= "Dummy DVB-C",
		.frequency_stepsize	= 62500,
		.frequency_min		= 51000000,
		.frequency_max		= 858000000,
		.symbol_rate_min	= (57840000/2)/64,     /* SACLK/64 == (XIN/2)/64 */
		.symbol_rate_max	= (57840000/2)/4,      /* SACLK/4 */
		.caps = FE_CAN_QAM_16 | FE_CAN_QAM_32 | FE_CAN_QAM_64 |
			FE_CAN_QAM_128 | FE_CAN_QAM_256 |
			FE_CAN_FEC_AUTO | FE_CAN_INVERSION_AUTO
	},

	.release = jetfe_release,

	.init = jetfe_init,
	.sleep = jetfe_sleep,

	.set_frontend = jetfe_set_frontend,
	.get_frontend = jetfe_get_frontend,

	.read_status = jetfe_read_status,
	.read_ber = jetfe_read_ber,
	.read_signal_strength = jetfe_read_signal_strength,
	.read_snr = jetfe_read_snr,
	.read_ucblocks = jetfe_read_ucblocks,
};

static struct dvb_frontend_ops jetfe_qpsk_ops = {
	.delsys = { SYS_DVBS, SYS_DVBS2 },
	.info = {
		.name			= "Dummy DVB-S",
		.frequency_min		= 950000,
		.frequency_max		= 2150000,
		.frequency_stepsize	= 250,           /* kHz for QPSK frontends */
		.frequency_tolerance	= 29500,
		.symbol_rate_min	= 1000000,
		.symbol_rate_max	= 45000000,
		.caps = FE_CAN_INVERSION_AUTO |
			FE_CAN_FEC_1_2 | FE_CAN_FEC_2_3 | FE_CAN_FEC_3_4 |
			FE_CAN_FEC_5_6 | FE_CAN_FEC_7_8 | FE_CAN_FEC_AUTO |
			FE_CAN_QPSK
	},

	.release = jetfe_release,

	.init = jetfe_init,
	.sleep = jetfe_sleep,

	.set_frontend = jetfe_set_frontend,
	.get_frontend = jetfe_get_frontend,

	.read_status = jetfe_read_status,
	.read_ber = jetfe_read_ber,
	.read_signal_strength = jetfe_read_signal_strength,
	.read_snr = jetfe_read_snr,
	.read_ucblocks = jetfe_read_ucblocks,

	.set_voltage = jetfe_set_voltage,
	.set_tone = jetfe_set_tone,
};

//MODULE_DESCRIPTION("DVB DUMMY Frontend");
//MODULE_AUTHOR("Anyone");
//MODULE_LICENSE("GPL");

//EXPORT_SYMBOL(jetfe_ofdm_attach);
//EXPORT_SYMBOL(jetfe_qam_attach);
//EXPORT_SYMBOL(jetfe_qpsk_attach);
