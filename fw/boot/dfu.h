#ifndef DFU_H
#define DFU_H

#include <stdint.h>


enum dfu_request {
	DFU_DETACH,
	DFU_DNLOAD,
	DFU_UPLOAD,
	DFU_GETSTATUS,
	DFU_CLRSTATUS,
	DFU_GETSTATE,
	DFU_ABORT,
};


enum dfu_status {
	OK,
	errTARGET,
	errFILE,
	errWRITE,
	errERASE,
	errCHECK_ERASED,
	errPROG,
	errVERIFY,
	errADDRESS,
	errNOTDONE,
	errFIRMWARE,
	errVENDOR,
	errUSBR,
	errPOR,
	errUNKNOWN,
	errSTALLEDPKT,
};


enum dfu_state {
	appIDLE,
	appDETACH,
	dfuIDLE,
	dfuDNLOAD_SYNC,
	dfuDNBUSY,
	dfuDNLOAD_IDLE,
	dfuMANIFEST_SYNC,
	dfuMANIFEST,
	dfuMANIFEST_WAIT_RESET,
	dfuUPLOAD_IDLE,
	dfuERROR
};


#define DFU_DT_FUNCTIONAL	0x21	/* DFU FUNCTIONAL descriptor type */


#define	DFU_TO_DEV(req)		(0x21 | (req) << 8)
#define	DFU_FROM_DEV(req)	(0xa1 | (req) << 8)


struct dfu {
	uint8_t status;		/* bStatus */
	uint8_t toL, toM, toH;	/* bwPollTimeout */
	uint8_t state;		/* bState */
	uint8_t iString;
};


extern struct dfu dfu;


void dfu_init(void);

#endif /* !DFU_H */
