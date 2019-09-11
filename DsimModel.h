#pragma once
#include "StdAfx.h"
#include "sdk/vsm.hpp"

#define InfoLog(__s__) sprintf_s(LogLineT, "%05d: ", LogLine++); myPopup->print(LogLineT); myPopup->print(__s__); myPopup->print("\n")

//#define DEBUGCALLS

#define SetHigh setstate(time, 1, SHI)
#define SetLow setstate(time, 1, SLO)
#define SetFloat setstate(time, 1, FLT)

enum CYCLES {
	FETCH = 0,
	READ = 1,
	WRITE = 2,
	IOREAD = 3,
	IOWRITE = 4
};

enum STATES {
	T1p = 0,
	T1n = 1,
	T2p = 2,
	T2n = 3,
	T3p = 4,
	T3n = 5,
	T4p = 6,
	T4n = 7
};

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
	void ResetCPU(ABSTIME time);
	void Execute(void);

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
	IUSERPOPUP *myMessage;


	// Global variables
	UINT8 cycle = 0;		// Current cycle of the state machine
	UINT8 nextcycle = 0;	// Next cycle of the state machine
	UINT8 state = 0;		// Current t-state
	UINT8 step = 0;			// Instruction execution step (0 = nothing to do)
	UINT8 IsHalted = 0;		// Indicates if the processor is halted
	UINT8 IsWaiting = 0;	// Indicates if the processor is waiting
	UINT8 IsBusRQ = 0;		// Indicates if the processor is on bus request
	UINT8 IsInt = 0;		// Indicates if the processor is interrupted
	UINT8 IsNMI = 0;		// Indicates if the processor is on non-maskable interrupt
	UINT8 OpPrefix = 0;		// Op code Prefix (CB, DD, ED, FD)

	int LogLine = 1;
	char LogLineT[10];
	char LogMessage[256];

	// Processor registers
#define REGSIZE 30

	typedef struct {
		union {
			struct {
				UINT8 ARRAY[REGSIZE];
			};
			/*struct {
				UINT8 reg8[18];
			};
			struct {
				UINT16 reg16[11];
			};
			struct {
				UINT16 reg16_2[4];
			};*/

			struct {
				UINT16 BC, DE, HL, SP, AF, IR, BC_, DE_, HL_, SP_, AF_, IY, IX, PC, WZ;
			};
			struct {// 0  1  2  3  4  5   6    7   8  9  10 11 12 13  14  15  16  17   18    19   20   21   22   23   24    25   26  27   28  29
				UINT8  B, C, D, E, H, L,SPh, SPl, A, F, I, R, B_, C_, D_, E_, H_, L_, SPh_, SPl_, A_, IYl, IYh, IXl, IXh, PCh, PC1, WZl, WZh, W;
			};
		};
	} tZ80REG;

	// Processor related variables
	UINT8 InstR = 0;		// Instruction Register
	tZ80REG reg;			// Registers
	UINT16 Addr;			// Memory address to read/write
	UINT8 Data;				// Data to/from the data bus
};
