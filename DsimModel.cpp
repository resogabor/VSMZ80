#include "StdAfx.h"
#include "DsimModel.h"


//static z80_t cpu;
//static uint64_t pins;
//
const STATE pLOHI[] = { SLO,FLT,FLT,FLT,SLO,SHI };
const STATE dLOHI[] = { SLO,SHI,FLT,FLT };


void DsimModel::HIZAddr(ABSTIME time) {						// Sets the address bus to HIZ
	int i;

	for (i = 0; i < 16; i++) {
		pin_A[i]->SetFloat;
	}
}

void DsimModel::HIZData(ABSTIME time) {						// Sets the data bus to HIZ
	int i;

	for (i = 0; i < 8; i++) {
		pin_D[i]->SetFloat;
	}
}

void DsimModel::SetAddr(UINT16 val, ABSTIME time) {			// Sets an address onto the address bus
	int i, j;

	for (i = 0; i < 16; i++) {
		j = (val >> i) & 0x01;
		if (j) {
			pin_A[i]->SetHigh;
		} else {
			pin_A[i]->SetLow;
		}
	}
}

void DsimModel::SetData(UINT8 val, ABSTIME time) {			// Sets a value onto the data bus
	int i, j;

	for (i = 0; i < 8; i++) {
		j = (val >> i) & 0x01;
		if (j) {
			pin_D[i]->SetHigh;
		} else {
			pin_D[i]->SetLow;
		}
	}
}

UINT8 DsimModel::GetData(void) {							// Reads a value from the data bus
	int i;
	UINT8 val = 0;

	for (i = 0; i < 8; i++) {
		if (ishigh(pin_D[i]->istate()))
			val |= (1 << i);
	}
	return(val);
}

void DsimModel::SetCPUState(ABSTIME time) {
	uint64_t tempData;
	const uint16_t addr = Z80_GET_ADDR(pins);
// Input pins
	ishigh(pin_WAIT->istate()) ? pins = pins & ~Z80_WAIT : pins = pins | Z80_WAIT;
	ishigh(pin_INT->istate()) ? pins = pins & ~Z80_INT : pins = pins | Z80_INT;
	ishigh(pin_NMI->istate()) ? pins = pins & ~Z80_NMI : pins = pins | Z80_NMI;
	ishigh(pin_RESET->istate()) ? pins = pins & ~Z80_RES : pins = pins | Z80_RES;
// Output pins
	(pins & Z80_MREQ) ? pin_MREQ->SetLow : pin_MREQ->SetHigh;
	(pins & Z80_RD) ? pin_RD->SetLow : pin_RD->SetHigh;
	(pins & Z80_WR) ? pin_WR->SetLow : pin_WR->SetHigh;
	(pins & Z80_M1) ? pin_M1->SetLow : pin_M1->SetHigh;

	
	SetAddr(addr, time);
	if (pins & Z80_MREQ) {
		if (pins & Z80_RD) {
			tempData = GetData();
			Z80_SET_DATA(pins, tempData);
		}
		else if (pins & Z80_WR) {
			
			tempData = Z80_GET_DATA(pins);
			SetData(tempData, time);
			HIZData(time + 20000);
		}
	}
	else if (pins & Z80_IORQ) {
		if (pins & Z80_RD) {
			//Z80_SET_DATA(pins, (Z80_GET_ADDR(pins) & 0xFF) * 2);
		}
		else if (pins & Z80_WR) {
			//out_port = Z80_GET_ADDR(pins);
			//out_byte = Z80_GET_DATA(pins);
		}
	}

}

void DsimModel::ResetCPU(ABSTIME time) {					// Rests the CPU
	
}

void DsimModel::Execute(void) {								// Executes an instruction

}

INT DsimModel::isdigital(CHAR *pinname) {
	return TRUE;											// Indicates all the pins are digital
}

VOID DsimModel::setup(IINSTANCE *instance, IDSIMCKT *dsimckt) {

	int n;
	char s[8];

	inst = instance;
	ckt = dsimckt;

	

	CREATEPOPUPSTRUCT *cps = new CREATEPOPUPSTRUCT;
	cps->caption = "Z80 Simulator Debugger Log";			// WIN Header
	cps->flags = PWF_VISIBLE | PWF_SIZEABLE;				// Show + Size
	cps->type = PWT_DEBUG;									// WIN DEBUG
	cps->height = 500;
	cps->width = 400;
	cps->id = 123;

	myPopup = (IDEBUGPOPUP *)instance->createpopup(cps);
	
	InfoLog("Connecting control pins...");

	pin_M1 = inst->getdsimpin("$M1$", true);				// Connects M1 cycle pin
	pin_MREQ = inst->getdsimpin("$MREQ$", true);			// Connects memory request pin
	pin_IORQ = inst->getdsimpin("$IORQ$", true);			// Connects IO request pin
	pin_RD = inst->getdsimpin("$RD$", true);				// Connects memory read pin
	pin_WR = inst->getdsimpin("$WR$", true);				// Connects memory write pin
	pin_RFSH = inst->getdsimpin("$RFSH$", true);			// Connects memory refresh pin
	pin_HALT = inst->getdsimpin("$HALT$", true);			// Connects halt pin
	pin_WAIT = inst->getdsimpin("$WAIT$", true);			// Connects memory wait pin
	pin_INT = inst->getdsimpin("$INT$", true);				// Connects interrupt request pin
	pin_NMI = inst->getdsimpin("$NMI$", true);				// Connects non-maskable interrupt pin
	pin_RESET = inst->getdsimpin("$RESET$", true);			// Connects reset pin
	pin_BUSRQ = inst->getdsimpin("$BUSRQ$", true);			// Connects bus request pin
	pin_BUSAK = inst->getdsimpin("$BUSAK$", true);			// Connects bus acknowledge pin
	pin_CLK = inst->getdsimpin("CLK", true);				// Connects Clock pin
	
	InfoLog("Connecting data pins...");
	for (n = 0; n < 8; n++) {								// Connects Data pins
		s[0] = 'D';
		_itoa_s(n, &s[1], 7, 10);
		pin_D[n] = inst->getdsimpin(s, true);
	}

	InfoLog("Connecting address pins...");
	for (n = 0; n < 16; n++) {								// Connects Address pins
		s[0] = 'A';
		_itoa_s(n, &s[1], 7, 10);
		pin_A[n] = inst->getdsimpin(s, true);
	}

	pins = z80_init(&cpu);
	cpu.f = 0;
	cpu.af2 = 0xFF00;
	cpu.bc2 = 0xFFFF;
	cpu.de2 = 0xFFFF;
	cpu.hl2 = 0xFFFF;
	
	// Connects function to handle Clock steps (instead of using "simulate")
	pin_CLK->sethandler(this, (PINHANDLERFN)&DsimModel::clockstep);

	z80_reset(&cpu);
	HIZAddr(0);
	HIZData(0);
}

VOID DsimModel::runctrl(RUNMODES mode) {
}

VOID DsimModel::actuate(REALTIME time, ACTIVESTATE newstate) {
}

BOOL DsimModel::indicate(REALTIME time, ACTIVEDATA *data) {
	return FALSE;
}

VOID DsimModel::clockstep(ABSTIME time, DSIMMODES mode) {
	if (pin_CLK->isposedge()) {
		
		//pins = pins | (Z80_INT | Z80_WAIT | Z80_NMI | Z80_RES);
		SetCPUState(time);
		pins = z80_tick(&cpu, pins);
		SetCPUState(time);
		
		if (z80_opdone(&cpu)) {
			sprintf_s(LogMessage, "0x%X opcode done! cyle number is: %d", cpu.opcode, cycle);
			InfoLog(LogMessage);
			cycle = 1;
			
			//SetData(0x00, time);
		}
		else {
			cycle++;
		}
	}
}

VOID DsimModel::simulate(ABSTIME time, DSIMMODES mode) {
}

VOID DsimModel::callback(ABSTIME time, EVENTID eventid) {
}
