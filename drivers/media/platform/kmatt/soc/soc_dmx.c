


struct kmatt_dmx {
	struct dvb_demux demux;		/* first item */
	struct dmxdev dmxdev;

#if USE_SW_DEMUX
	int soft;
#else
	int hard;
	struct tsc_state *tsc;
#endif
};


/*********************************************************************
 * kernel hardware & software demux api
 *********************************************************************/






int soc_dmx_add_feed(struct kmatt_channel *chan, struct dvb_demux_feed *feed)
{

}


int dump_dvb_demux_feed(struct dvb_demux_feed *dvbdmxfeed)
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
	case DMX_TS_PES_VIDEO:
		printk("  pes_type: DMX_TS_PES_VIDEO\n");
		break;
	case DMX_TS_PES_AUDIO:
		printk("  pes_type: DMX_TS_PES_AUDIO\n");
		break;
	case DMX_TS_PES_TELETEXT:
		printk("  pes_type: DMX_TS_PES_TELETEXT\n");
		break;
	case DMX_TS_PES_PCR:
		printk("  pes_type: DMX_TS_PES_PCR\n");
		break;
	case DMX_TS_PES_OTHER:
		printk("  pes_type: DMX_TS_PES_OTHER\n");
		break;
	default:
		printk("  pes_type: unknown (%d)\n", dvbdmxfeed->pes_type);
		return -EINVAL;
	}
}

int soc_start_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	int ret = 0;
	struct kmatt_dmx *socdmx = (struct kmatt_dmx *)dvbdmxfeed->demux;
	struct kmatt_channel *chan = (struct kmatt_channel *)socdmx->demux.priv;

	struct dvb_demux *dvbdmx = socdmx->demux;

	printk("*** start_feed ***\n");

	dump_dvb_demux_feed(dvbdmxfeed);

	if (!dvbdmx->dmx.frontend)
		return -EINVAL;

	if (feed->pid > 0x1fff)
		return -EINVAL;

	ret = soc_dmx_add_feed(socdmx, dvbdmxfeed);

	return ret;
}

int soc_stop_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	struct kmatt_dmx *socdmx = (struct kmatt_dmx *)dvbdmxfeed->demux;
	struct kmatt_channel *chan = (struct kmatt_channel *)socdmx->demux.priv;

	struct dvb_demux *dvbdmx = socdmx->demux;

	if (--chan->users)
		return chan->users;

	if (!chan->dev->cmd_timeout_workaround)
		set_transfer(chan, 0);

	return 0;
}



int kmatt_dmx_ts_card_init(struct dvb_demux *dvbdemux,
			    int (*start_feed)(struct dvb_demux_feed *),
			    int (*stop_feed)(struct dvb_demux_feed *),
			    void *priv)
{
	dvbdemux->priv = priv;

	dvbdemux->filternum = SOC_FILTER_NUM;
	dvbdemux->feednum = SOC_FILTER_NUM;
	dvbdemux->start_feed = start_feed;
	dvbdemux->stop_feed = stop_feed;
	dvbdemux->write_to_decoder = NULL;
	dvbdemux->dmx.capabilities = (DMX_TS_FILTERING |
				      DMX_SECTION_FILTERING | DMX_PES_FILTERING |
				      DMX_MEMORY_BASED_FILTERING | DMX_TS_DESCRAMBLING);
	return dvb_dmx_init(dvbdemux);
}



int kmatt_dmxdev_ts_card_init(struct dmxdev *dmxdev,
			       struct dvb_demux *dvbdemux,
			       struct dmx_frontend *hw_frontend,
			       struct dmx_frontend *mem_frontend,
			       struct dvb_adapter *dvb_adapter)
{
	int ret;

	dmxdev->filternum = SOC_FILTER_NUM;
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




int kmatt_dmx_init(struct kmatt_channel *chan)
{
	struct kmatt_dmx *socdmx;
	socdmx = kzalloc(sizeof(struct kmatt_dmx), GFP_KERNEL);
	if (!socdmx)
		return -ENOMEM;

	ret = kmatt_dmx_ts_card_init(&socdmx->demux,
					  soc_start_feed, soc_stop_feed, chan);
	ret = kmatt_dmxdev_ts_card_init(&socdmx->dmxdev, &socdmx->demux,
					 &chan->hw_frontend,
					 &chan->mem_frontend, &chan->adapter);

	ret = kmatt_dmx_hw_init(socdmx);

	ret = dvb_net_init(&adapter, &chan->dvbnet, &socdmx->demux.dmx);
	return ret;
}


///////////////////////////////////////////////////////////////

int kmatt_dmx_hw_init(struct kmatt_dmx *soc_dmx)
{
	struct tsc_state *tsc;
	tsc = kzalloc(sizeof(struct tsc_state), GFP_KERNEL);
	if (!tsc)
		return -ENOMEM;

	/* IRQ */
	tsc->irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "failed to get irq resource\n");
		return -ENOENT;
	}
}
