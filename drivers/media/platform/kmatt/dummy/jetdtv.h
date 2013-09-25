#ifndef _JETDTV_H
#define _JETDTV_H

#include <linux/time.h>
#include <linux/dvb/dmx.h>
#include <linux/dvb/frontend.h>
#include <linux/list.h>
#include <linux/mod_devicetable.h>
#include <linux/mutex.h>
#include <linux/spinlock_types.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/workqueue.h>

#include <demux.h>
#include <dmxdev.h>
#include <dvb_demux.h>
#include <dvb_frontend.h>
#include <dvb_net.h>
#include <dvbdev.h>




struct jetdtv {
	struct dvb_adapter adapter;			/* dvb adapter one adapter on channel*/
	struct dvb_frontend *fe;

	struct dvb_demux demux;
	struct dmxdev dmxdev;

	struct dvb_net dvbnet;
	struct dmx_frontend hw_frontend;
	struct dmx_frontend mem_frontend;
};

int jetdtv_adapter_init(struct jetdtv *dtv, const char *name);
void jetdtv_adapter_exit(struct jetdtv *dtv);

#endif
