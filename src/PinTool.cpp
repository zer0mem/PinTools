// PinToolsBBT.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include "pin.H"

ofstream OutFile;

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static size_t icount;

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
							"o", "inscount.out", "specify output file name");

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
	// Write to a file since cout and cerr maybe closed by the application
	OutFile.setf(ios::showbase);
	OutFile << "Count " << icount << endl;
	OutFile.close();

	printf("number of ins : %x\n", icount);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
	cerr << "This tool counts the number of dynamic instructions executed" << endl;
	cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
	return -1;
}
// The running count of instructions is kept here
// make it static to help the compiler optimize docount


// This function is called before every block
VOID docount(UINT32 c) { icount += c; }
VOID Trace(TRACE trace, VOID *v)
{
	// Visit every basic block  in the trace
	for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
	{
		// Insert a call to docount before every bbl, passing the number of instructions
		BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)docount, IARG_UINT32, BBL_NumIns(bbl), IARG_END);
	}
}
/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char * argv[])
{
	// Initialize pin
	if (PIN_Init(argc, argv)) return Usage();

	icount = 0;
	OutFile.open(KnobOutputFile.Value().c_str());

/***********************************************************************************************/
/* http://software.intel.com/sites/landingpage/pintool/docs/58423/Pin/html/index.html#EXAMPLES */
/***********************************************************************************************/

	//INS_AddInstrumentFunction
	//TRACE_AddInstrumentFunction
	//RTN_AddInstrumentFunction

	// Register Fini to be called when the application exits
	PIN_AddFiniFunction(Fini, 0);

	// Start the program, never returns
	PIN_StartProgram();

	return 0;
}
