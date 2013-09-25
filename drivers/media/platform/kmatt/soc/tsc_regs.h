#ifndef _REGS_TSC_H_
#define _REGS_TSC_H_



/* HW registers */
#define TSC_CTRL			0x00
#define     TSC_RST			(1 << 1)
#define     TSC_EN			(1 << 0)

#define TSC_STAR			0x04

#define TSC_PCTRL			0x10
#define     TSC_OUTP0CTRL		(1 << 16)
#define     TSCINP1CTRL			(1 << 1)
#define     TSCINP0CTRL			(1 << 0)

#define TSC_PPARR			0x14
//#define 	TSCOUTP0PAR			(((x) & 0xff) << 23)
//#define 	TSCINP1PAR			(((x) & 0xff) << 7)
//#define 	TSCINP0PAR			(((x) & 0xff) << 7)

#define TSF_INMUXRCTRL		0x20
#define     TSF1INMUXCTRL		()
#define     TSF0INMUXCTRL		()

#define TSF_OUTMUXCTRL		0x28
#define     TSOUTMUXCTRL		()

/* TSG registers */
#define TSG_CSR				0x00			/* Control and status register */
#define     TSGSTS				()			/* Status for TS generator */
#define     TSGLBUFMODE			(1 << 9)	/* loop buffer mode */
#define     TSGSYNCBYTECHKEN	(1 << 8)	/* sync byte check enable */
#define     TSGPAUSE			(1 << 2)	/* pause bit fot TS generator */
#define     TSGSTOP				(1 << 1)	/* stop bit fot TS generator */
#define     TSGSTART			(1 << 0)	/* start bit fot TS generator */

#define TSG_PPR				0x04			/* TSG packet parameter register */
#define     TSG_SYNC_BYTE_VAL		()		/* sync byte value */
#define     TSG_SYNC_BYTE_POS		(1 << 7)		/* sync byte position */
#define     TSG_PKT_SIZE(x)			(((x) & 0x03) << 0)	/* packet size */

#define TSG_INTESR			0x08			/* TSG interrupt enable and status register */
#define     TSG_END_INTEN			(1<<19)
#define     TSG_FULLFIN_INTEN		(1<<18)
#define     TSG_HALFFIN_INTEN		(1<<17)
#define     TSG_ERRSYNC_INTEN		(1<<16)




#define TSG_CCR				0x0c













#endif
