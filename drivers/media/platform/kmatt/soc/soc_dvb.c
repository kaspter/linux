
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


#define USE_SW_DEMUX 1


#define MAX_STREAM 2
#define SOC_FILTER_NUM	32


DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);





static LIST_HEAD(dvb_soc_devices);
static DEFINE_MUTEX(list_lock);



struct kmatt_info {
	char		*model_name;
	char		*dev_type;
	u32			ts_size;
	u32			bytes;

	irqreturn_t (*irq_handler)(int irq, void *dev_id);

	u8			power;
	u8			reset;

};


/* for each TS input */
struct kmatt_channel {
	struct kmatt_dvb *kdvb;			    /* kmatt dvb */
	int number							/* channel number */

	struct dvb_adapter adapter;			/* one adapter one channel*/
	struct dvb_frontend *fe;            /* one frontend one channel */

	struct soft_dvb_dmx  *soft_dmx;	    /* software demux */
	struct kmatt_dmx  *soc_dmx;		    /* hardware demux */

	struct dvb_net dvbnet;
	struct dmx_frontend hw_frontend;
	struct dmx_frontend mem_frontend;
};




struct kmatt_dvb {
	struct list_head list;		/* list of all registered devices */

#if 0
	struct kmatt_desc *sdesc;

	struct device *parent;		/* Camera host device */
	struct device *control;		/* E.g., the i2c client */
#endif

	struct platform_device *pdev;	/* platform device */

	/* dvb */
	struct kmatt_channel channel[MAX_STREAM];

	struct kmatt_info   *dvb_info;

	struct i2c_adapter	i2c_adap;

	unsigned int full_ts_users;
	unsigned int boardnr;
	int nr;
};









/**
 *
 *
 */

/* one adapter for one channel */
static int init_channel(struct kmatt_channel *chan)
{
	int ret;
	struct kmatt_dvb *kmatt = chan->kdvb;


	/* register dvb adapter for each channal */
	ret = dvb_register_adapter(&chan->adapter, DRIVER_NAME,
					THIS_MODULE, &kdvb->pdev->dev, adapter_nr);
	if (ret < 0)
		goto err;

	chan->dvb_adapter.priv	= kmatt;


#if USE_SW_DEMUX
	ret = soft_dvb_dmx_init(chan)
#else
	ret = kmatt_dmx_init(chan);
#endif
/********************************************************************/


#if 0
	/*Register descrambler device*/
	ret = dvb_register_device(&chan->adapter, &kmatt->dsc_dev,
                                   &dvbdev_dsc, kmatt, DVB_DEVICE_DSC);
#endif

/********************************************************************/

	ret = kmatt_frontend_init(kmatt, chan->number);
	if (ret < 0 || chan->fe == NULL) {
		goto err;
	}

	/* frontend init success */
	chan->fe->callback = dvb_soc_frontend_callback;

	/* register frontend for this adapter */
	if (dvb_register_frontend(&chan->adapter, chan->fe) < 0)
		goto err;

/*********************************************************************/

	return ret;

err:
	if (chan->fe) {
		dvb_frontend_detach(chan->fe);
		chan->fe = NULL;
	}
	release_channel(chan);
	return ret;
}


static int release_channel(struct kmatt_channel *chan)
{
	int ret;
	struct kmatt_dvb *kmatt = chan->kdvb;

    return 0;
}


static int kmatt_init_channels(struct kmatt_dvb *kmatt)
{
	int i, j;

	for (i = 0; i < MAX_STREAM; i++) {
		kmatt->channel[i].number = i;
		kmatt->channel[i].kdvb = kmatt;
		if (init_channel(&kdvb->channel[i]) < 0) {
			for (j = i - 1; j >= 0; j--)
				release_channel(&kmatt->channel[j]);
			return -1;
		}
	}
	return 0;
}











static irqreturn_t kmatt_irq_handler(int irq, void *dev_id)
{

}


static int kmatt_dvb_init(struct kmatt_dvb *kmatt)
{
	int err, ret = 0;

	struct platform_device *pdev = kmatt->pdev;

	struct kmatt_info *info	= kmatt->kmatt_info;

	err = request_irq(pdev->irq, info->irq_handler,
			  IRQF_SHARED, DRIVER_NAME, kdvb);
	if (err != 0) {
		printk("IRQ ERR\n");
		return -ENODEV;
	}

	platform_set_drvdata(pdev, kmatt);
	return ret;
}


static int kmatt_dvb_probe(struct platform_device *pdev)
{
	int ret;
	struct kmatt_dvb *kmatt;
	struct kmatt_info *info;

	kmatt = kzalloc(sizeof(struct kmatt_dvb), GFP_KERNEL);
	if (!kmatt)
		return -ENOMEM;

	kmatt->pdev = pdev;
	info = (struct kmatt_info *)pdev->dev.platform_data;
	if (!info) {
		kfree(kmatt);
		return -EINVAL;
	}

	info->irq_handler = &kmatt_irq_handle;
	kmatt->dvb_info = info;

	kmatt_dvb_init(kmatt);


	/* some thing else here */



	/* register DVB adapters and devices for channels */
	ret = kmatt_init_channels(kmatt);
	if (ret < 0)
		goto out;


	return 0;

out:
    kfree(kmatt);
	return ret;
}

static int kmatt_dvb_remove(struct platform_device *pdev)
{
	struct kmatt_dvb *kmatt = platform_get_drvdata(pdev);

    /** FIXME **/

	return 0;
}

static struct platform_driver kmatt_dvb_driver = {
	.probe		= kmatt_dvb_probe,
	.remove		= kmatt_dvb_remove,
	.driver		= {
		.name	= "kmatt_dvb",
		.owner	= THIS_MODULE,
	},
};

module_platform_driver(kmatt_dvb_driver);

MODULE_DESCRIPTION("Kmatt DVB Platform driver");
MODULE_AUTHOR("Kaspter Ju");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:kmatt_dvb");
