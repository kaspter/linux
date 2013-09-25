

#include "tsc_regs.h"



#define TSC_DMX_QUEUE_COUNT			32	/* HIGH_SPEED queue isn't really a queue */

#define TSC_DMX_QUEUE_VIDEO			0
#define TSC_DMX_QUEUE_AUDIO			1
#define TSC_DMX_QUEUE_TELETEXT		2
#define TSC_DMX_QUEUE_USER_START	3
#define TSC_DMX_QUEUE_USER_END		30
#define TSC_DMX_QUEUE_MESSAGE		31

#define TSC_DMX_QUEUE_HIGH_SPEED	32

#define TSC_DMX_SYSTEM_QUEUES		0xff	/* alias for video+audio+teletext queues */



struct tsc_state {
	void __iomem	*regs_base;
	struct clk		*clk_per;
	struct clk		*clk_ahb;
	int				 irq;
};


static inline void tsc_write(struct tsc_state *tsc, u32 data, u32 reg)
{
	printk("%s: data=0x%x, reg=0x%x\n", __func__, data, reg);
	writel(data, tsc->regs_base + reg);
}

static inline unsigned int tsc_read(struct tsc_state *tsc, u32 reg)
{
	u32 data;
	data = readl(tsc->regs_base + reg);
	printk("%s: data=0x%x, reg=0x%x\n", __func__, data, reg);
	return data;
}



int tsc_reset(tsc_state *tsc)
{
	tsc_write(tsc, TSC_RST, TSC_CTRL);
}


static irqreturn_t tsc_irq_handler(int irq, void *data)
{
	int pcr0, chan0;
	int pcr1, chan1;
	struct tsc_state *tsc = data;

	// get interrupt status
	pcr0 = tsc_read(tsc, TSC_XXX);
	chan0 = tsc_read(tsc, TSC_XXX);
	pcr1 = tsc_read(tsc, TSC_XXX);
	chan1 = tsc_read(tsc, TSC_XXX);

	// clear interrupt
	tsc_write(tsc, pcr0, TSC_CTRL);
	tsc_write(tsc, chan0, TSC_CTRL);
	tsc_write(tsc, pcr1, TSC_CTRL);
	tsc_write(tsc, chan1, TSC_CTRL);

	// do something here

	return IRQ_HANDLED;
}


int tsc_clk_init()
{

}

int tsc_init(struct tsc_state *tsc)
{
	int ret;
	int queue_nr;

	for () {

	}



	ret = request_irq(tsc->irq, tsc_irq_handler, 0, "soc_tsc", tsc);

}

#if TSC_STANDLONE

static int tsc_probe(struct platform_device *pdev)
{

}

static int tsc_remove(struct platform_device *pdev)
{

}

static struct platform_driver tsc_driver = {
	.probe		= tsc_probe,
	.remove		= tsc_remove,
	.driver		= {
		.name	= "soc_tsc",
		.owner	= THIS_MODULE,
	},
};

module_platform_driver(tsc_driver);
#endif
