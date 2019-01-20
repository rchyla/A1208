/* A1208 configuration registers */
#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#define CFG_ADDRESS		0xE88000	


#define CFG_STATUS		0x10
#define CFG_CONTROL		0x14
#define CFG_REV			0x18
#define CFG_SUBREV		0x1C

#define CFG_ID0_OFFSET	0x0
#define CFG_ID1_OFFSET	0x4
#define CFG_ID2_OFFSET	0x8
#define CFG_ID3_OFFSET	0xC

#define CFG_ID0	0xC0		/* ID sequence for A1208 */
#define CFG_ID1	0x50
#define CFG_ID2	0x30
#define CFG_ID3	0x30
#define CFG_ID_MASK	0xF0

#define CFG_JP_STAT			0x80	/* 1xxxxxxx - Status of a jumper */
#define CFG_CC_STAT			0x40	/* x1xxxxxx - Status of PCMCIA friendly mode */
#define CFG_PSLOWMEM_STAT 	0x20	/* xx1xxxxx - Satus of P-SLOWmem */
#define CFG_CARD_STAT		0x10	/* xxx1xxxx - Card status on/off*/

#define CFG_JP_INVERT	0x80	/* 1xxxxxxx - invert jumper */
#define CFG_EXTRAMEM	0x40	/* x1xxxxxx - enable extra memory at $A80000-$BEFFFF only in PCMCIA friendly mode */
#define CFG_PSLOWMEM	0x20	/* xx1xxxxx - enable P-SLOW memory, only in PCMCIA friendly mode */
#define CFG_CARD_EN		0x10	/* xxx1xxxx - enable card */

#define CFG_RX_MASK		0xF0

#define ADDMEM_0_BASE		0xA80000
#define ADDMEM_PRI		20


#endif /* _HARDWARE_H_ */