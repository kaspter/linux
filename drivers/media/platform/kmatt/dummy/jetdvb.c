#include <linux/kernel.h>
#include <linux/bitops.h>

#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>

#include "dmxdev.h"
#include "dvbdev.h"
#include "dvb_demux.h"
#include "dvb_frontend.h"
#include "dvb_net.h"

#include "jetdtv.h"
#include "jetfe.h"

#define DUMMY_FILTER_NUM	32

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);


static int dump_demux_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	printk("  pid: 0x%04X\n", dvbdmxfeed->pid);

	switch (dvbdmxfeed->type) {
	case DMX_TYPE_TS:
		 printk("  type: DMX_TYPE_TS\n");
		 break;
	case DMX_TYPE_SEC:
		 printk("  type: DMX_TYPE_SEC\n");
		 break;
	default:
		 printk("  type: unknown (%d)\n", dvbdmxfeed->type);
		 return -EINVAL;
	}

	printk("  ts_type:");

	if (dvbdmxfeed->ts_type & TS_PACKET)
		printk(" TS_PACKET");

	if (dvbdmxfeed->ts_type & TS_DECODER)
		printk(" TS_DECODER");

	if (dvbdmxfeed->ts_type & TS_PAYLOAD_ONLY)
		printk(" TS_PAYLOAD_ONLY");

	printk("\n");

	switch (dvbdmxfeed->pes_type) {
	case DMX_PES_VIDEO:
		printk("  pes_type: DMX_PES_VIDEO\n");
		break;
	case DMX_PES_AUDIO:
		printk("  pes_type: DMX_PES_AUDIO\n");
		break;
	case DMX_PES_TELETEXT:
		printk("  pes_type: DMX_PES_TELETEXT\n");
		break;
	case DMX_PES_PCR:
		printk("  pes_type: DMX_PES_PCR\n");
		break;
	case DMX_PES_OTHER:
		printk("  pes_type: DMX_PES_OTHER\n");
		break;
	default:
		printk("  pes_type: unknown (%d)\n", dvbdmxfeed->pes_type);
		return -EINVAL;
	}

	return 0;
}


static int dummy_start_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	dump_demux_feed(dvbdmxfeed);
	return 0;
}

static int dummy_stop_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	dump_demux_feed(dvbdmxfeed);
	return 0;
}




#if 0
int dmx_ts_card_init(struct dvb_demux *dvbdemux,
			    int (*start_feed)(struct dvb_demux_feed *),
			    int (*stop_feed)(struct dvb_demux_feed *),
			    void *priv)
{
	dvbdemux->priv = priv;

	dvbdemux->filternum = DUMMY_FILTER_NUM;
	dvbdemux->feednum = DUMMY_FILTER_NUM;
	dvbdemux->start_feed = start_feed;
	dvbdemux->stop_feed = stop_feed;
	dvbdemux->write_to_decoder = NULL;
	dvbdemux->dmx.capabilities = (DMX_TS_FILTERING |
				      DMX_SECTION_FILTERING | DMX_PES_FILTERING |
				      DMX_MEMORY_BASED_FILTERING | DMX_TS_DESCRAMBLING);
	return dvb_dmx_init(dvbdemux);
}


int dmxdev_ts_card_init(struct dmxdev *dmxdev,
			       struct dvb_demux *dvbdemux,
			       struct dmx_frontend *hw_frontend,
			       struct dmx_frontend *mem_frontend,
			       struct dvb_adapter *dvb_adapter)
{
	int ret;

	dmxdev->filternum = DUMMY_FILTER_NUM;
	dmxdev->demux = &dvbdemux->dmx;
	dmxdev->capabilities = 0;
	ret = dvb_dmxdev_init(dmxdev, dvb_adapter);
	if (ret < 0)
		return ret;

	hw_frontend->source = DMX_FRONTEND_0;
	ret = dvbdemux->dmx.add_frontend(&dvbdemux->dmx, hw_frontend);
	if (ret < 0)
		return ret;
	mem_frontend->source = DMX_MEMORY_FE;
	dvbdemux->dmx.add_frontend(&dvbdemux->dmx, mem_frontend);
	if (ret < 0)
		return ret;
	return dvbdemux->dmx.connect_frontend(&dvbdemux->dmx, hw_frontend);
}

#endif


static int jetdtv_frontend_callback(void *dev, int component, int cmd, int arg)
{
	return 0;
}





int jetdtv_frontend_init(struct jetdtv *dtv)
{

	/* attach demod and tunner here */
//	dtv->fe = dvb_attach(jetfe_ofdm_attach);
	dtv->fe = jetfe_ofdm_attach();
	if (!dtv->fe)
		return -1;

//	dvb_attach(demo_tuner_attach, fe, &info->tuner_info, i2c);

	return 0;
}





int jetdtv_adapter_init(struct jetdtv *dtv, const char *name)
{
	int ret;

	/* register dvb adapter */
	ret = dvb_register_adapter(&dtv->adapter, name,THIS_MODULE, NULL, adapter_nr);
	if (ret < 0)
		goto err;

	dtv->adapter.priv	= dtv;

/********************************************************************/
	//kernel internal demux init
	dtv->demux.priv = dtv;

	dtv->demux.filternum = DUMMY_FILTER_NUM;
	dtv->demux.feednum = DUMMY_FILTER_NUM;
	dtv->demux.start_feed = dummy_start_feed;
	dtv->demux.stop_feed = dummy_stop_feed;
	dtv->demux.write_to_decoder = NULL;
	dtv->demux.dmx.capabilities = (DMX_TS_FILTERING |
				      DMX_SECTION_FILTERING | DMX_PES_FILTERING |
				      DMX_MEMORY_BASED_FILTERING | DMX_TS_DESCRAMBLING);
	ret = dvb_dmx_init(&dtv->demux);
	if (ret < 0)
		printk("error kernel demux init\n");

/********************************************************************/
	// kernel dmxdev interface
	dtv->dmxdev.filternum = DUMMY_FILTER_NUM;
	dtv->dmxdev.demux = &dtv->demux.dmx;
	dtv->dmxdev.capabilities = 0;
	ret = dvb_dmxdev_init(&dtv->dmxdev, &dtv->adapter);
	if (ret < 0)
		printk("error dmxdev init\n");


	dtv->hw_frontend.source = DMX_FRONTEND_0;
	ret = dtv->demux.dmx.add_frontend(&dtv->demux.dmx, &dtv->hw_frontend);
	if (ret < 0)
		printk("error add hw_frontend\n");

	dtv->mem_frontend.source = DMX_MEMORY_FE;
	ret = dtv->demux.dmx.add_frontend(&dtv->demux.dmx, &dtv->mem_frontend);
	if (ret < 0)
		printk("error add mem_frontend\n");
	ret = dtv->demux.dmx.connect_frontend(&dtv->demux.dmx, &dtv->hw_frontend);
	if (ret < 0)
		printk("error connect hw frontend\n");
/********************************************************************/

	// dvbnet init
	ret = dvb_net_init(&dtv->adapter, &dtv->dvbnet, &dtv->demux.dmx);
	if (ret < 0)
		printk("error dvbnet init\n");

/********************************************************************/

	ret = jetdtv_frontend_init(dtv);
	if (ret < 0 || dtv->fe == NULL) {
		printk("error jetdtv_frontend_init init\n");
		goto err;
	}

	/* frontend init success */
	dtv->fe->callback = jetdtv_frontend_callback;

	/* register frontend for this adapter */
	if (dvb_register_frontend(&dtv->adapter, dtv->fe) < 0)
		goto err;

/*********************************************************************/

	printk("jetdtv_adapter: init success!\n");
	return ret;

err:
	if (dtv->fe) {
		dvb_frontend_detach(dtv->fe);
		dtv->fe = NULL;
	}
	return ret;
}


void jetdtv_adapter_exit(struct jetdtv *dtv)
{

	if (dtv->fe) {
		dvb_frontend_detach(dtv->fe);
		dtv->fe = NULL;
	}
	dvb_net_release(&dtv->dvbnet);

	dtv->demux.dmx.close(&dtv->demux.dmx);
	dtv->demux.dmx.remove_frontend(&dtv->demux.dmx, &dtv->hw_frontend);
	dtv->demux.dmx.remove_frontend(&dtv->demux.dmx, &dtv->mem_frontend);
	dvb_dmxdev_release(&dtv->dmxdev);
	dvb_dmx_release(&dtv->demux);
	dvb_unregister_adapter(&dtv->adapter);
}
