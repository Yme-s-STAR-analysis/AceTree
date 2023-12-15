/*
	fDST maker v2.0
	29.09.2023 by yghuang
*/

#ifndef ST_FEMTO_DSTMAKER_h
#define ST_FEMTO_DSTMAKER_h

// C++ headers
#include <vector>

// ROOT headers
#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "StPicoEvent/StPicoTrack.h"

#include "StFemtoTrack/StFemtoTrack.h"
#include "StFemtoEvent/StFemtoEvent.h"

#include "StRoot/StPicoDstMaker/StPicoDstMaker.h"
#include "TVector3.h"


class StFemtoEvent;
class StFemtoTrack;
class TH1D;
class TF1;
class StCFMult;
class MeanDcaTool;
class CentCorrTool;
class TpcShiftTool;
class TriggerTool;
class TofT0Correction;

//_________________
class StFemtoDstMaker : public StMaker {
	public:
		StFemtoDstMaker(char * name);
		virtual ~StFemtoDstMaker();

		Int_t Init();
		Int_t Finish();
		Int_t Make();
		void SetFileIndex(char *val) {mFileIndex=val;}
		void SetOutDir(char *val) {mOutDir=val;}

	private:
	
		StPicoDstMaker * mPicoDstMaker;

		char *mFileIndex;
		char *mOutDir;
		StFemtoEvent * mFemtoEvent;
		TTree * fDstTree;
		TFile * mOutfile;

		StCFMult* mtMult;
		MeanDcaTool* mtDca;
		CentCorrTool* mtCent;
		TpcShiftTool* mtShift;
		TriggerTool* mtTrg;
		TofT0Correction* mtTofT0;

		ClassDef(StFemtoDstMaker,1)
};


#endif
