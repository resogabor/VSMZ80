#pragma once
#include "StdAfx.h"
#include "sdk/vsm.hpp"
#include "z80.h"

#define InfoLog(__s__) sprintf_s(LogLineT, "%05d: ", LogLine++); myPopup->print(LogLineT); myPopup->print(__s__); myPopup->print("\n")

//#define DEBUGCALLS

#define SetHigh drivestate(time, SHI)
#define SetLow drivestate(time, SLO)
#define SetFloat drivestate(time, FLT)


class DsimModel : public IDSIMMODEL
{
public:
	INT isdigital (CHAR *pinname);
	VOID setup (IINSTANCE *inst, IDSIMCKT *dsim);
	VOID runctrl (RUNMODES mode);
	VOID actuate (REALTIME time, ACTIVESTATE newstate);
	BOOL indicate (REALTIME time, ACTIVEDATA *data);
	VOID clockstep(ABSTIME time, DSIMMODES mode);
	VOID simulate(ABSTIME time, DSIMMODES mode);
	VOID callback (ABSTIME time, EVENTID eventid);
	
private:
	VOID SetAddr(UINT16 val, ABSTIME time);
	VOID SetData(UINT8 val, ABSTIME time);
	UINT8 GetData(void);
	void HIZAddr(ABSTIME time);
	void HIZData(ABSTIME time);
	void SetCPUState(ABSTIME time);

	IINSTANCE *inst;
	IDSIMCKT *ckt;
	IDSIMPIN *pin_M1;
	IDSIMPIN *pin_MREQ, *pin_IORQ;
	IDSIMPIN *pin_RD, *pin_WR;
	IDSIMPIN *pin_RFSH;
	IDSIMPIN *pin_HALT;
	IDSIMPIN *pin_WAIT;
	IDSIMPIN *pin_INT, *pin_NMI;
	IDSIMPIN *pin_RESET;
	IDSIMPIN *pin_BUSRQ, *pin_BUSAK;
	IDSIMPIN *pin_CLK;
	IDSIMPIN *pin_A[16];
	IDSIMPIN *pin_D[8];

	IDEBUGPOPUP *myPopup;

	// Global variables
	UINT8 cycle = 0;		// Current cycle of the state machine

	int LogLine = 1;
	char LogLineT[10];
	char LogMessage[256];

	z80_t cpu;
	uint64_t pins;
};
