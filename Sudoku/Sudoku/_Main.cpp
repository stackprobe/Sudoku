#include "all.h"

static void Main2(void)
{
	// Close @ termination()
	{
		Dc.EvStop = CreateEventA(NULL, FALSE, FALSE, "{073fc80e-1a30-41fd-a3b9-3fd7df1b4b8d}");
		Dc.EvBruteForceStarted = CreateEventA(NULL, FALSE, FALSE, EV_BRUTE_FORCE_STARTED);
	}

	if(argIs("/S"))
	{
#if 1
		SetEvent(Dc.EvStop);
#else
		for(int c = 20; ; c--)
		{
			SetEvent(Dc.EvStop);

			if(!c)
				break;

			Sleep(30);
		}
#endif
		return;
	}
	char *dataDir = ".";

	if(argIs("/debug-test-mode-1111"))
	{
		LOGPOS();
		dataDir = DEF_DATA_DIR;
	}
	if(argIs("/D")) // i/o Directory
	{
		dataDir = nextArg();
	}
	Dc.FrameCsvFile = combine(dataDir, DEF_FRAME_CSVFILE);
	Dc.SquareCsvFile = combine(dataDir, DEF_SQUARE_CSVFILE);
	Dc.GroupCsvFile = combine(dataDir, DEF_GROUP_CSVFILE);
	Dc.XInequalCsvFile = combine(dataDir, DEF_X_INEQUAL_CSVFILE);
	Dc.YInequalCsvFile = combine(dataDir, DEF_Y_INEQUAL_CSVFILE);
	Dc.XJointCsvFile = combine(dataDir, DEF_X_JOINT_CSVFILE);
	Dc.YJointCsvFile = combine(dataDir, DEF_Y_JOINT_CSVFILE);
	Dc.CondCsvFile = combine(dataDir, DEF_COND_CSVFILE);
	Dc.RelCsvFile = combine(dataDir, DEF_REL_CSVFILE);
	Dc.InputCsvFile = combine(dataDir, DEF_INPUT_CSVFILE);
	Dc.OutputCsvFile = combine(dataDir, DEF_OUTPUT_CSVFILE);
	Dc.ErrorCsvFile = combine(dataDir, DEF_ERROR_CSVFILE);
	Dc.LoadedDataCsvFile = combine(dataDir, DEF_LOADEDDATA_CSVFILE);

	if(argIs("/F")) // Frame csv file
	{
		Dc.FrameCsvFile = nextArg();
	}
	if(argIs("/S")) // Square csv file
	{
		Dc.SquareCsvFile = nextArg();
	}
	if(argIs("/G")) // Group csv file
	{
		Dc.GroupCsvFile = nextArg();
	}
	if(argIs("/XI")) // X-Inequal csv file
	{
		Dc.XInequalCsvFile = nextArg();
	}
	if(argIs("/YI")) // Y-Inequal csv file
	{
		Dc.YInequalCsvFile = nextArg();
	}
	if(argIs("/XJ")) // X-Joint csv file
	{
		Dc.XJointCsvFile = nextArg();
	}
	if(argIs("/YJ")) // Y-Joint csv file
	{
		Dc.YJointCsvFile = nextArg();
	}
	if(argIs("/C")) // Condition csv file
	{
		Dc.CondCsvFile = nextArg();
	}
	if(argIs("/R")) // Relation csv file
	{
		Dc.RelCsvFile = nextArg();
	}
	if(argIs("/I")) // Input csv file
	{
		Dc.InputCsvFile = nextArg();
	}
	if(argIs("/O")) // Output csv file
	{
		Dc.OutputCsvFile = nextArg();
	}
	if(argIs("/E")) // Error csv file
	{
		Dc.ErrorCsvFile = nextArg();
	}
	if(argIs("/L")) // Loaded-data csv file
	{
		Dc.LoadedDataCsvFile = nextArg();
	}
	if(argIs("/OL")) // force Output Loaded-data
	{
		Dc.ForceOutputLoadedData = 1;
	}
	errorCase_m(hasArgs(1), "不明なコマンド引数が指定された。");

	LoadData();

	if(Dc.ForceOutputLoadedData)
		WriteLoadedDataCsvFile();

	Analyze();
	MakeOutput();

	return;
}
int main(int argc, char **argv)
{
	Main2();
	termination(0);
}
