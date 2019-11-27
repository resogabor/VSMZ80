//#include "StdAfx.h"
#include "DsimModel.h"

#include <string.h>





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

void DsimModel::ResetCPU(ABSTIME time) {					// Rests the CPU
	int i;

#ifdef DEBUGCALLS
	InfoLog("Resetting CPU...");
#endif

	// zeroes all the flags
	cycle = 0;
	nextcycle = 0;
	state = 0;
	step = 0;
	IsHalted = 0;
	IsWaiting = 0;
	IsBusRQ = 0;
	IsInt = 0;
	IsNMI = 0;

	// zeroes all the registers
	for (i = 0; i < REGSIZE; i++)
		reg.ARRAY[i] = 0;

	// sets all output pins to high
	pin_M1->SetHigh;
	pin_MREQ->SetHigh;
	pin_IORQ->SetHigh;
	pin_RD->SetHigh;
	pin_WR->SetHigh;
	pin_RFSH->SetHigh;
	pin_HALT->SetHigh;
	pin_BUSAK->SetHigh;

	HIZAddr(time);
	HIZData(time);
}

void DsimModel::Execute(void) {								// Executes an instruction
#ifdef DEBUGCALLS
	sprintf_s(LogMessage, "    Executing 0x%02x step %d...", InstR, step);
	InfoLog(LogMessage);
#endif
	int done = 0; // Indicates done executing

	if (step) {
		if (OpPrefix1 == 0) {
			bool jr = false;
			switch (opCodePrefix.x)				
			{
				case 0:
					switch (opCodePrefix.z)
					{
						case 0:
							switch (opCodePrefix.y)
							{
								case 0:								// NOP
									done++;
									break;
								case 1:								// EX AF, AF'
									reg.A = reg.A_;
									reg.F = reg.F_;
									done++;
									break;
								case 2:								//DJNZ d
									break;
								case 3:								//JR d
									jr = true;
								//JR cc[y-4], d
								case 4:
									jr = (reg.F & 0b00000001) == 0b00000001; //JR F[NZ], d
								case 5:
									jr = (reg.F & 0b00000010) == 0b00000010; //JR F[Z], d
								case 6:
									jr = (reg.F & 0b00000100) == 0b00000100; //JR F[NC], d
								case 7:								
									jr = (reg.F & 0b00001000) == 0b00001000; //JR F[C], d
										if (jr) {
											switch (step++)
											{
											case 1:
												cycle = READ;
												Addr = reg.PC++;
												break;
											case 2:
												reg.PC = reg.PC + Data;
												done++;
												break;
											default:
												break;
											}
										}
									break;
								default:
									break;
							}
							break;
						case 1:
							switch (opCodePrefix.q)
							{
								case 0:								// LD dd, nn
									switch (step++)
									{
										case 1:
											cycle = READ;
											Addr = reg.PC++;
											break;
										case 2:
											if (opCodePrefix.p != 3) {
												reg.ARRAY[(opCodePrefix.p * 2) + 1] = Data;
											}
											else {
												reg.SPl = Data;
											}
											Addr = reg.PC++;
										case 3:
											if (opCodePrefix.p != 3) {
												reg.ARRAY[opCodePrefix.p * 2] = Data;
											}
											else {
												reg.SPh = Data;
												reg.SP = (reg.SPh << 8) | reg.SPl;
											}
											done++;
										default:
											break;
									}
									break;
								case 1:								// ADD HL, dd
									switch (step++) {
										case 1:
											if (opCodePrefix.p != 3) {
												reg.L = reg.L + reg.ARRAY[(opCodePrefix.p * 2) + 1];
												if ((reg.L & 0b100) == 0b100) {
													reg.H++;
													reg.L = reg.L << 1;
												}
												reg.H = reg.H + reg.ARRAY[opCodePrefix.p * 2]; 												
											}											
											else {
												tReg.temp1 = reg.SP;
											}
											tReg.WAIT_C = 6;
											cycle = WAIT;
											done++;
											break;
										default:
											break;
									}
									break;
								default:
									break;
							}
						case 2:
						default:
							break;
					}
					break;
				case 1:
					if (opCodePrefix.x != 6) {
						reg.ARRAY[opCodePrefix.y] = reg.ARRAY[opCodePrefix.z];		// LD r, s
						done++;
					}
					else {
						IsHalted = 1;
						done++;														// HALT
					}
					break;
				case 2:
					break;
				case 3:
					break;
				default:
					break;
			}
		}
		//switch()

//		switch (InstR) {
//		case 0x00:											// nop
//			done++;
//			break;
//		case 0x01:											//LD BC, nnnn
//			switch (step++)
//			case 1:
//
//				break;
//			{
//			default:
//				break;
//			}
//		case 0x21:											// LD HL, nnnn
//			switch (step++) {
//			case 1:
//				cycle = READ;
//				Addr = reg.PC++;
//				break;
//			case 2:
//				reg.L = Data;
//				Addr = reg.PC++;
//				break;
//			case 3:
//				reg.H = Data;
//#ifdef DEBUGCALLS
//				sprintf_s(LogMessage, "        HL=0x%04x", reg.HL);
//				InfoLog(LogMessage);
//#endif
//				done++;
//				break;
//			}
//			break;
//		case 0x2C:											// INC L
//			reg.L++;
//#ifdef DEBUGCALLS
//			sprintf_s(LogMessage, "        L=0x%02x", reg.L);
//			InfoLog(LogMessage);
//#endif
//			done++;
//			break;
//		case 0x3C:											// INC A
//			reg.A++;
//#ifdef DEBUGCALLS
//			sprintf_s(LogMessage, "        A=0x%02x", reg.A);
//			InfoLog(LogMessage);
//#endif
//			done++;
//			break;
//		case 0x3E:											// LD A, nn
//			switch (step++) {
//			case 1:
//				cycle = READ;
//				Addr = reg.PC;
//				break;
//			case 2:
//				reg.A = Data;
//				reg.PC++;
//#ifdef DEBUGCALLS
//				sprintf_s(LogMessage, "        A=0x%02x", reg.A);
//				InfoLog(LogMessage);
//#endif
//				done++;
//				break;
//			}
//			break;
//		case 0x77:											// LD (HL), A
//			switch (step++) {
//			case 1:
//				cycle = WRITE;
//				Addr = reg.HL;
//				Data = reg.A;
//				break;
//			case 2:
//#ifdef DEBUGCALLS
//				sprintf_s(LogMessage, "        0x%04x=0x%02x", reg.HL, reg.A);
//				InfoLog(LogMessage);
//#endif
//				done++;
//				break;
//			}
//			break;
//		case 0xc3:											// JP nnnn
//			switch (step++) {
//			case 1:
//				cycle = READ;
//				Addr = reg.reg16[REG_PC]++;
//				break;
//			case 2:
//				Addr = reg.reg16[REG_PC]++;
//				reg.Z = Data;
//				break;
//			case 3:
//				reg.W = Data;
//				reg.PC = reg.WZ;
//#ifdef DEBUGCALLS
//				sprintf_s(LogMessage, "        PC=0x%04x", reg.WZ);
//				InfoLog(LogMessage);
//#endif
//				done++;
//				break;
//			}
//			break;
//		default:											// Undefined = NOP
//			break;
//		}
//		if (done) {
//			cycle = FETCH;
//			step = 0;
//			OpPrefix1 = 0;
//			OpPrefix2 = 0;
//		}
	}
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

	CREATEPOPUPSTRUCT *msg = new CREATEPOPUPSTRUCT;
	msg->caption = "MyMessage";
	msg->type = PWT_USER;
	msg->flags = PWF_HIDEONANIMATE | PWF_SIZEABLE | PWF_VISIBLE;
	msg->height = 500;
	msg->width = 400;
	msg->id = 1234;

	myPopup = (IDEBUGPOPUP *)instance->createpopup(cps);
	myMessage = (IUSERPOPUP *)instance->createpopup(msg);

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

	// Connects function to handle Clock steps (instead of using "simulate")
	pin_CLK->sethandler(this, (PINHANDLERFN)&DsimModel::clockstep);

	

	//desc.tick_cb = tick_;
	ResetCPU(0);
}


VOID DsimModel::runctrl(RUNMODES mode) {
	switch (mode)
	{
	case RM_BATCH:
		break;
	case RM_START:
		break;
	case RM_STOP:
		break;
	case RM_SUSPEND:
		sprintf_s(LogMessage, "Suspended...");
		InfoLog(LogMessage);
		//myMessage->callwindowproc(1, 23, 10);
		break;
	case RM_ANIMATE:
		break;
	case RM_STEPTIME:
		break;
	case RM_STEPOVER:
		sprintf_s(LogMessage, "Step over...");
		InfoLog(LogMessage);
		break;
	case RM_STEPINTO:
		sprintf_s(LogMessage, "STEP into...");
		InfoLog(LogMessage);
		break;
	case RM_STEPOUT:
		sprintf_s(LogMessage, "Step out...");
		InfoLog(LogMessage);

		break;
	case RM_STEPTO:
		sprintf_s(LogMessage, "Step to...");
		InfoLog(LogMessage);

		break;
	case RM_META:
		break;
	case RM_DUMP:
		break;
	default:
		break;
	}
	
	if (mode == RM_SUSPEND) {
	}
}

VOID DsimModel::actuate(REALTIME time, ACTIVESTATE newstate) {
	
}

BOOL DsimModel::indicate(REALTIME time, ACTIVEDATA *data) {
	return FALSE;
}

VOID DsimModel::clockstep(ABSTIME time, DSIMMODES mode) {
	
	
		
		ckt->suspend(inst, "Clolc period!");
#ifdef DEBUGCALLS
		sprintf_s(LogMessage, "Cycle %d state %d...", cycle, state);
		InfoLog(LogMessage);
#endif
		if (IsHalted) { cycle = HALT; }
		switch (cycle) {
			/*----------------------------------------------*/
		case FETCH:											// Instruction fetch cycle
			switch (state) {
			case T1p:
#ifdef DEBUGCALLS
				InfoLog("  Fetch...");
				sprintf_s(LogMessage, "    Setting instruction address to 0x%04x...", reg.PC);
				InfoLog(LogMessage);
#endif
				pin_M1->SetLow;
				SetAddr(reg.PC, time);
				break;
			case T1n:
				pin_MREQ->SetLow;
				pin_RD->SetLow;
				break;
			case T2p:
				reg.PC++;
				break;
			case T2n:
				break;
			case T3p:
#ifdef DEBUGCALLS
				InfoLog("    Reading instruction...");
#endif
				InstR = GetData();
#ifdef DEBUGCALLS
				sprintf_s(LogMessage, "      -> 0x%02x...", InstR);
				InfoLog(LogMessage);
#endif
				pin_MREQ->SetHigh;
				pin_RD->SetHigh;
				pin_M1->SetHigh;
#ifdef DEBUGCALLS
				sprintf_s(LogMessage, "    Setting refresh address to 0x%04x...", reg.IR);
				InfoLog(LogMessage);
#endif
				//SetAddr(reg.IR, time + 20000);				// Puts the refresh address on the bus 20ns after RD goes up
				//reg.W = reg.R++;
				//reg.R = (reg.W & 0x80) | (reg.R & 0x7f);	// Increments only the 7 first bits of R (the 8th bit stays the same)
				//pin_RFSH->setstate(time + 22000, 1, SLO);	// And brings RFSH low 2ns after that
				break;
			case T3n:
				pin_MREQ->SetLow;
				break;
			case T4p:
				if (IsHalted) { InstR = 0; }
				break;
			case T4n:
				pin_MREQ->SetHigh;
				if (InstR == 0xCB || InstR == 0xDD || InstR == 0xED || InstR == 0xFD) {
					if (OpPrefix1==0 && OpPrefix2==0) {
						OpPrefix1 = InstR;
					}
					else {
						OpPrefix2 = InstR;
					}		
					cycle = FETCH;
					step = 0;
				}
				else {
					opCodePrefix.x = InstR >> 6;
					opCodePrefix.y = (InstR >> 3) & 7;
					opCodePrefix.z = InstR & 7;
					step = 1;									// Start execution of the fetched instruction
					Execute();
				}
				pin_RFSH->SetHigh;
				break;
			}
			
			
			state++;
			if (state > T4n)
				state = T1p;
			break;
			/*----------------------------------------------*/
		case READ:											// Memory read cycle
			switch (state) {
			case T1p:
#ifdef DEBUGCALLS
				InfoLog("  Read...");
				sprintf_s(LogMessage, "    Setting read memory address to 0x%04x...", Addr);
				InfoLog(LogMessage);
#endif
				SetAddr(Addr, time);
				break;
			case T1n:
				pin_MREQ->SetLow;
				pin_RD->SetLow;
				break;
			case T3n:
#ifdef DEBUGCALLS
				InfoLog("    Reading data...");
#endif
				Data = GetData();
#ifdef DEBUGCALLS
				sprintf_s(LogMessage, "      -> 0x%02x...", Data);
				InfoLog(LogMessage);
#endif
				pin_MREQ->SetHigh;
				pin_RD->SetHigh;
				Execute();
				break;
			}
			state++;
			if (state > T3n)
				state = T1p;
			break;
			/*----------------------------------------------*/
		case WRITE:											// Memory write cycle
			switch (state) {
			case T1p:
#ifdef DEBUGCALLS
				InfoLog("  Write...");
				sprintf_s(LogMessage, "    Setting write memory address to 0x%04x...", Addr);
				InfoLog(LogMessage);
#endif
				SetAddr(Addr, time);
				break;
			case T1n:
				pin_MREQ->SetLow;
#ifdef DEBUGCALLS
				sprintf_s(LogMessage, "    Setting data to 0x%02x...", Data);
				InfoLog(LogMessage);
#endif
				SetData(Data, time);
				break;
			case T2n:
				pin_WR->SetLow;
				break;
			case T3n:
				pin_MREQ->SetHigh;
				pin_WR->SetHigh;
				HIZData(time + 20000);						// Put the data bus in FLT 20ns after the WR pin goes up
				Execute();
				break;
			}
			state++;
			if (state > T3n)
				state = T1p;
			break;
		case IOREAD:
			switch (state)
			{
			case T1p:
				break;
			case T1n:
				break;
			default:
				break;
			}
			break;
		case IOWRITE:
			switch (state)
			{
			case T1p:
				break;
			case T1n:
				break;
			default:
				break;
			}
			break;
		case WAIT:
			if (tReg.WAIT_C > 0) {
				tReg.WAIT_C--;
			}
			else {
				Execute();
			}
			break;
		//case HALT:
		//	opCodePrefix.x = 0;
		//	opCodePrefix.y = 0;
		//	opCodePrefix.z = 0;
		//	pin_HALT
		//	step = 1;									// Start execution NOP instruction
		//	Execute();
		//	break;

		}		

}

VOID DsimModel::simulate(ABSTIME time, DSIMMODES mode) {
	
}

VOID DsimModel::callback(ABSTIME time, EVENTID eventid) {
	
		//z80_exec(&z80Cpu, 0, time);
	sprintf_s(LogMessage, "    Clock pin callback: %d", pin_CLK->istate());
	InfoLog(LogMessage);

}
DsimModel::~DsimModel(){}