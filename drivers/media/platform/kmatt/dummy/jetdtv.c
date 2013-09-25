
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


static struct jetdtv *dtv;

static int __init jetdtv_init(void)
{

	printk("--------------- %s -----------\n", __func__);
	dtv = kzalloc(sizeof(struct jetdtv), GFP_KERNEL);
	if (!dtv)
		return -ENOMEM;

	jetdtv_adapter_init(dtv, "jetdtv");
}

static void __exit jetdtv_exit(void)
{
	printk("--------------- %s -----------\n", __func__);
	jetdtv_adapter_exit(dtv);
	kfree(dtv);
}



module_init(jetdtv_init);
module_exit(jetdtv_exit);

MODULE_AUTHOR("Kaspter Ju");
MODULE_LICENSE("GPL v2");
