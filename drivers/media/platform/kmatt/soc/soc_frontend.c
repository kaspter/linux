









/* frontend callback cmd */
#define TUNER_CMD_RESET		0
#define TUNER_CMD_PWRON		1
#define TUNER_CMD_PWROFF	2

#define DEMOD_CMD_RESET		0
#define DEMOD_CMD_PWRON		1
#define DEMOD_CMD_PWROFF	2




struct kmatt_frontend {
	int						 id;
	struct dvb_frontend		*fe;
	struct platform_device	*pdev;
};


static tuner_callback(struct kmatt_dvb *kdvb, int cmd, int arg)
{
	struct kmatt_platform_info *p = kdvb->pdev->dev.platform_data;
}


static demod_callback(struct kmatt_dvb *kdvb,int cmd, int arg)
{

}


static int dvb_soc_frontend_callback(void *dev, int component, int cmd, int arg)
{
	struct kmatt_dvb *kdvb = dev;

	dev_dbg(&d->pdev, "%s: component=%d cmd=%d arg=%d\n",
			__func__, component, cmd, arg);

	switch (component) {
	case DVB_FRONTEND_COMPONENT_TUNER:
		return tuner_callback(kdvb, cmd, arg);
	case DVB_FRONTEND_COMPONENT_DEMOD:
		return demo_callback(kdvb, cmd, arg);
	default:
		break;
	}

	return 0;
}



int kmatt_frontend_init(struct kmatt_dvb *kdvb, int chan)
{
	int ret;
	struct kmatt_info *info = kdvb->dvb_info;
	struct kmatt_channel *chan = &kdvb->channel[chan];
	struct i2c_adapter *i2c	= &kdvb->i2c_adap;

	/**/
	/* reset low and delay*/
	/* pwron and delay */
	/* reset high */

	/* attach demod and tunner here */
	chan->fe = dvb_attach(demo_demod_attach, &info->demod_info, i2c);
	if (!chan->fe)
		return -1;

	dvb_attach(demo_tuner_attach, fe, &info->tuner_info, i2c);

	return 0;
}
