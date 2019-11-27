#include <stdint.h>
#include <stdbool.h>
#include "StdAfx.h"
#include "sdk/vsm.hpp"
//#include "chips/chips/z80.h"

#define InfoLog(__s__) sprintf_s(LogLineT, "%05d: ", LogLine++); myPopup->print(LogLineT); myPopup->print(__s__); myPopup->print("\n")

//	/*--- address lines ---*/
//#define Z80_A0  (1ULL<<0)
//#define Z80_A1  (1ULL<<1)
//#define Z80_A2  (1ULL<<2)
//#define Z80_A3  (1ULL<<3)
//#define Z80_A4  (1ULL<<4)
//#define Z80_A5  (1ULL<<5)
//#define Z80_A6  (1ULL<<6)
//#define Z80_A7  (1ULL<<7)
//#define Z80_A8  (1ULL<<8)
//#define Z80_A9  (1ULL<<9)
//#define Z80_A10 (1ULL<<10)
//#define Z80_A11 (1ULL<<11)
//#define Z80_A12 (1ULL<<12)
//#define Z80_A13 (1ULL<<13)
//#define Z80_A14 (1ULL<<14)
//#define Z80_A15 (1ULL<<15)
//
///*--- data lines ------*/
//#define Z80_D0  (1ULL<<16)
//#define Z80_D1  (1ULL<<17)
//#define Z80_D2  (1ULL<<18)
//#define Z80_D3  (1ULL<<19)
//#define Z80_D4  (1ULL<<20)
//#define Z80_D5  (1ULL<<21)
//#define Z80_D6  (1ULL<<22)
//#define Z80_D7  (1ULL<<23)
//
///*--- control pins ---*/
//
///* system control pins */
//#define  Z80_M1    (1ULL<<24)       /* machine cycle 1 */
//#define  Z80_MREQ  (1ULL<<25)       /* memory request */
//#define  Z80_IORQ  (1ULL<<26)       /* input/output request */
//#define  Z80_RD    (1ULL<<27)       /* read */
//#define  Z80_WR    (1ULL<<28)       /* write */
//#define  Z80_CTRL_MASK (Z80_M1|Z80_MREQ|Z80_IORQ|Z80_RD|Z80_WR)
//
///* CPU control pins */
//#define  Z80_HALT  (1ULL<<29)       /* halt state */
//#define  Z80_INT   (1ULL<<30)       /* interrupt request */
//#define  Z80_NMI   (1ULL<<31)       /* non-maskable interrupt */
//#define  Z80_BUSREQ (1ULL<<32)      /* bus request */
//#define  Z80_BUSACK (1ULL<<33)      /* bus acknowledge */
//
///* up to 7 wait states can be injected per machine cycle */
//#define Z80_WAIT0   (1ULL<<34)
//#define Z80_WAIT1   (1ULL<<35)
//#define Z80_WAIT2   (1ULL<<36)
//#define Z80_WAIT_SHIFT (34)
//#define Z80_WAIT_MASK (Z80_WAIT0|Z80_WAIT1|Z80_WAIT2)
//
///* interrupt-related 'virtual pins', these don't exist on the Z80 */
//#define Z80_IEIO    (1ULL<<37)      /* unified daisy chain 'Interrupt Enable In+Out' */
//#define Z80_RETI    (1ULL<<38)      /* cpu has decoded a RETI instruction */
//
///* bit mask for all CPU bus pins */
//#define Z80_PIN_MASK ((1ULL<<40)-1)
//
///*--- status indicator flags ---*/
//#define Z80_CF (1<<0)           /* carry */
//#define Z80_NF (1<<1)           /* add/subtract */
//#define Z80_VF (1<<2)           /* parity/overflow */
//#define Z80_PF Z80_VF
//#define Z80_XF (1<<3)           /* undocumented bit 3 */
//#define Z80_HF (1<<4)           /* half carry */
//#define Z80_YF (1<<5)           /* undocumented bit 5 */
//#define Z80_ZF (1<<6)           /* zero */
//#define Z80_SF (1<<7)           /* sign */
//
///* helper macro to start interrupt handling in tick callback */
//#define Z80_DAISYCHAIN_BEGIN(pins) if (pins&Z80_M1) { pins|=Z80_IEIO;
///* helper macro to end interrupt handling in tick callback */
//#define Z80_DAISYCHAIN_END(pins) pins&=~Z80_RETI; }
///* return a pin mask with control-pins, address and data bus */
//#define Z80_MAKE_PINS(ctrl, addr, data) ((ctrl)|(((data)<<16)&0xFF0000ULL)|((addr)&0xFFFFULL))
///* extract 16-bit address bus from 64-bit pins */
//#define Z80_GET_ADDR(p) ((uint16_t)(p&0xFFFFULL))
///* merge 16-bit address bus value into 64-bit pins */
//#define Z80_SET_ADDR(p,a) {p=((p&~0xFFFFULL)|((a)&0xFFFFULL));}
///* extract 8-bit data bus from 64-bit pins */
//#define Z80_GET_DATA(p) ((uint8_t)((p&0xFF0000ULL)>>16))
///* merge 8-bit data bus value into 64-bit pins */
//#define Z80_SET_DATA(p,d) {p=((p&~0xFF0000ULL)|(((d)<<16)&0xFF0000ULL));}
///* extract number of wait states from pin mask */
//#define Z80_GET_WAIT(p) ((p&Z80_WAIT_MASK)>>Z80_WAIT_SHIFT)
///* set up to 7 wait states in pin mask */
//#define Z80_SET_WAIT(p,w) {p=((p&~Z80_WAIT_MASK)|((((uint64_t)w)<<Z80_WAIT_SHIFT)&Z80_WAIT_MASK));}

//#define DEBUGCALLS
/*--- callback function typedefs ---*/
//typedef uint64_t(*z80_tick_t)(int num_ticks, uint64_t pins, void* user_data, DsimModel model);
//typedef int (*z80_trap_t)(uint16_t pc, int ticks, uint64_t pins, void* trap_user_data);

using namespace std;

class DsimModel : public IDSIMMODEL
{
public:
	~DsimModel();
	INT isdigital (CHAR *pinname);
	VOID setup (IINSTANCE *inst, IDSIMCKT *dsim);
	VOID runctrl (RUNMODES mode);
	VOID actuate (REALTIME time, ACTIVESTATE newstate);
	BOOL indicate (REALTIME time, ACTIVEDATA *data);
	VOID clockstep(ABSTIME time, DSIMMODES mode);
	VOID simulate(ABSTIME time, DSIMMODES mode);
	VOID callback (ABSTIME time, EVENTID eventid);
	IDSIMPIN* pin_M1;
	IDSIMPIN* pin_MREQ, * pin_IORQ;
	IDSIMPIN* pin_RD, * pin_WR;
	IDSIMPIN* pin_RFSH;
	IDSIMPIN* pin_HALT;
	IDSIMPIN* pin_WAIT;
	IDSIMPIN* pin_INT, * pin_NMI;
	IDSIMPIN* pin_RESET;
	IDSIMPIN* pin_BUSRQ, * pin_BUSAK;
	IDSIMPIN* pin_CLK;
	IDSIMPIN* pin_A[16];
	IDSIMPIN* pin_D[8];
	VOID SetAddr(UINT16 val, ABSTIME time);
	VOID SetData(UINT8 val, ABSTIME time);
	UINT8 GetData(void);
	void HIZAddr(ABSTIME time);
	void HIZData(ABSTIME time);
	void ResetCPU(ABSTIME time);
	void Execute(void);

private:
	
	/*virtual uint64_t tick(int num_ticks, uint64_t pins, void* user_data, ABSTIME time);
	virtual int trap(uint16_t pc, int ticks, uint64_t pins, void* trap_user_data, ABSTIME time);*/

/* initialization attributes */
	//typedef struct {
	//	//z80_tick_t tick_cb;
	//	void* user_data;
	//} z80_desc_t;

	/* Z80 CPU state */
	//typedef struct {
	//	/* tick callback */
	//	//z80_tick_t tick_cb;
	//	/* main register bank (BC, DE, HL, FA) */
	//	uint64_t bc_de_hl_fa;   /* B:63..56 C:55..48 D:47..40 E:39..32 H:31..24 L:23..16: F:15..8, A:7..0 */
	//	/* shadow register bank (BC', DE', HL', FA') */
	//	uint64_t bc_de_hl_fa_;
	//	/* IR,WZ,SP,PC */
	//	uint64_t wz_ix_iy_sp;
	//	/* control bits,IM,IY,IX */
	//	uint64_t im_ir_pc_bits;
	//	/* last pin state (only for debug inspection) */
	//	uint64_t pins;
	//	void* user_data;
	//	//z80_trap_t trap_cb;
	//	void* trap_user_data;
	//	int trap_id;
	//} z80_t;

	/* initialize a new z80 instance */
	//void z80_init(z80_t* cpu, const z80_desc_t* desc);
	///* reset an existing z80 instance */
	//void z80_reset(z80_t* cpu);
	///* set optional trap callback function */
	////void z80_trap_cb(z80_t* cpu, z80_trap_t trap_cb, void* trap_user_data);
	///* execute instructions for at least 'ticks', but at least one, return executed ticks */
	//uint32_t z80_exec(z80_t* cpu, uint32_t ticks, ABSTIME time);
	///* return false if z80_exec() returned in the middle of an extended intruction */
	//bool z80_opdone(z80_t* cpu);

	///* register access functions */
	//void z80_set_a(z80_t* cpu, uint8_t v);
	//void z80_set_f(z80_t* cpu, uint8_t v);
	//void z80_set_l(z80_t* cpu, uint8_t v);
	//void z80_set_h(z80_t* cpu, uint8_t v);
	//void z80_set_e(z80_t* cpu, uint8_t v);
	//void z80_set_d(z80_t* cpu, uint8_t v);
	//void z80_set_c(z80_t* cpu, uint8_t v);
	//void z80_set_b(z80_t* cpu, uint8_t v);
	//void z80_set_fa(z80_t* cpu, uint16_t v);
	//void z80_set_af(z80_t* cpi, uint16_t v);
	//void z80_set_hl(z80_t* cpu, uint16_t v);
	//void z80_set_de(z80_t* cpu, uint16_t v);
	//void z80_set_bc(z80_t* cpu, uint16_t v);
	//void z80_set_fa_(z80_t* cpu, uint16_t v);
	//void z80_set_af_(z80_t* cpi, uint16_t v);
	//void z80_set_hl_(z80_t* cpu, uint16_t v);
	//void z80_set_de_(z80_t* cpu, uint16_t v);
	//void z80_set_bc_(z80_t* cpu, uint16_t v);
	//void z80_set_pc(z80_t* cpu, uint16_t v);
	//void z80_set_wz(z80_t* cpu, uint16_t v);
	//void z80_set_sp(z80_t* cpu, uint16_t v);
	//void z80_set_ir(z80_t* cpu, uint16_t v);
	//void z80_set_i(z80_t* cpu, uint8_t v);
	//void z80_set_r(z80_t* cpu, uint8_t v);
	//void z80_set_ix(z80_t* cpu, uint16_t v);
	//void z80_set_iy(z80_t* cpu, uint16_t v);
	//void z80_set_im(z80_t* cpu, uint8_t v);
	//void z80_set_iff1(z80_t* cpu, bool b);
	//void z80_set_iff2(z80_t* cpu, bool b);
	//void z80_set_ei_pending(z80_t* cpu, bool b);
	//void z80_set_userData(z80_t* cpu, void* userData);

	//uint8_t z80_a(z80_t* cpu);
	//uint8_t z80_f(z80_t* cpu);
	//uint8_t z80_l(z80_t* cpu);
	//uint8_t z80_h(z80_t* cpu);
	//uint8_t z80_e(z80_t* cpu);
	//uint8_t z80_d(z80_t* cpu);
	//uint8_t z80_c(z80_t* cpu);
	//uint8_t z80_b(z80_t* cpu);
	//uint16_t z80_fa(z80_t* cpu);
	//uint16_t z80_af(z80_t* cpu);
	//uint16_t z80_hl(z80_t* cpu);
	//uint16_t z80_de(z80_t* cpu);
	//uint16_t z80_bc(z80_t* cpu);
	//uint16_t z80_fa_(z80_t* cpu);
	//uint16_t z80_af_(z80_t* cpu);
	//uint16_t z80_hl_(z80_t* cpu);
	//uint16_t z80_de_(z80_t* cpu);
	//uint16_t z80_bc_(z80_t* cpu);
	//uint16_t z80_pc(z80_t* cpu);
	//uint16_t z80_wz(z80_t* cpu);
	//uint16_t z80_sp(z80_t* cpu);
	//uint16_t z80_ir(z80_t* cpu);
	//uint8_t z80_i(z80_t* cpu);
	//uint8_t z80_r(z80_t* cpu);
	//uint16_t z80_ix(z80_t* cpu);
	//uint16_t z80_iy(z80_t* cpu);
	//uint8_t z80_im(z80_t* cpu);
	//bool z80_iff1(z80_t* cpu);
	//bool z80_iff2(z80_t* cpu);
	//bool z80_ei_pending(z80_t* cpu);




#define SetHigh setstate(time, 1, SHI)
#define SetLow setstate(time, 1, SLO)
#define SetFloat setstate(time, 1, FLT)

	enum CYCLES {
		FETCH = 0,
		READ = 1,
		WRITE = 2,
		IOREAD = 3,
		IOWRITE = 4,
		WAIT = 5,
		HALT = 6,
		INT = 7,
		NMI = 8
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

	//uint64_t tick(int num, uint64_t pins, void* user_data);
	//virtual uint64_t tick_(int num, uint64_t pins, void* user_data);

	IINSTANCE *inst;
	IDSIMCKT *ckt;
	

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
	UINT8 OpPrefix1 = 0;		// Op code Prefix (CB, DD, ED, FD)
	UINT8 OpPrefix2 = 0;		// Op code Prefix (CB) (DDCB, FDCB)

	int LogLine = 1;
	char LogLineT[10];
	char LogMessage[256];

	// Processor registers
#define REGSIZE 20

	typedef struct {
		union {
			struct {
				UINT8 ARRAY[REGSIZE]; // 8 bit register array
			};
			struct {// 0  1  2  3  4  5   6   7  8   9  10  11  12  13   14   15  16 17 18 19   20   21   22   23   24    25   26  27   28  29
				UINT8  B, C, D, E, H, L, SPh, A, B_, C_, D_, E_, H_, L_, SPl, A_, I, R, F, F_;  //8 bit registers // 
			};			
		};
		
		struct {
			UINT16 IY, IX, PC, SP, tmp1, tmp2; // 16 bit registers
		};
		struct {
			BYTE IFF1, IFF2, IM; //special flags
		};
	} tZ80REG;

	typedef struct {
		struct {
			UINT8 TB, TC, TD, TE, TH, TL, TS, TA, TSPh, TPCh, tmph, tmpl; //temp 8 bit registers
		};
		struct {
			UINT16 temp1, temp2, temp3, temp4, WAIT_C; // temp 16 bit registers;
		};
	} tmpREG;

	typedef struct{	
		union{		
			struct {
				UINT16 ARRAY[4];
			};
			struct{
				UINT32 DDCB, FDCB;
			};
			struct{
				UINT16 DD, CB, FD, CB_;
			};		
		};
		struct {
			UINT8 x, y, z, p, q;
		};
	} opPrefix;

	
	// Processor related variables
	UINT8 InstR = 0;		// Instruction Register
	tZ80REG reg;			// Registers
	tmpREG tReg;			// temporary registers of opCode executes
	UINT16 Addr;			// Memory address to read/write
	UINT8 Data;				// Data to/from the data bus
	opPrefix opCodePrefix;			// opcode prefix


};
