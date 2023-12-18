
#include <iostream>
#include <cstdio>
#include <vector>
#include <utility>
#include "Riostream.h"

#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TString.h"
#include "TVector3.h"
#include "TF1.h"
#include "TH1D.h"

#include "StMaker.h"
#include "StPicoEvent/StPicoBTofPidTraits.h"
#include "StPicoEvent/StPicoETofPidTraits.h"
#include "StPicoEvent/StPicoTrack.h"
#include "StPicoEvent/StPicoEvent.h"
#include "StPicoEvent/StPicoDst.h"
#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StTriggerData.h"
#include "StTriggerIdCollection.h"
#include "StRunInfo.h"

#include "StBTofUtil/tofPathLength.hh"
#include "StarClassLibrary/StPhysicalHelixD.hh"
#include "StarClassLibrary/StLorentzVectorF.hh"
#include "phys_constants.h"

#include "StFemtoDstMaker.h"
#include "StRoot/StFemtoTrack/StFemtoTrack.h"
#include "StRoot/StFemtoEvent/StFemtoEvent.h"

#include "StRoot/CentCorrTool/CentCorrTool.h"
#include "StRoot/MeanDcaTool/MeanDcaTool.h"
#include "StRoot/TpcShiftTool/TpcShiftTool.h"
#include "StRoot/StCFMult/StCFMult.h"
#include "StRoot/TriggerTool/TriggerTool.h"
#include "StRoot/TofT0Correction/TofT0Correction.h"

ClassImp(StFemtoDstMaker)
StFemtoDstMaker::StFemtoDstMaker(char *name) : StMaker(name) {
}

StFemtoDstMaker::~StFemtoDstMaker() {
	delete mPicoDstMaker;
}

Int_t StFemtoDstMaker::Init(){
	TString filename = "";
	filename.Append(mFileIndex);
	filename.Append(".fDst.root");
	filename.Prepend(mOutDir);
	mOutfile = new TFile(filename, "recreate");

	mFemtoEvent = new StFemtoEvent();
	fDstTree = new TTree("fDst", "fDst");
	fDstTree->Branch("StFemtoEvent", mFemtoEvent);

	mtDca = new MeanDcaTool();
	mtCent = new CentCorrTool();
	mtMult = new StCFMult();
	mtShift = new TpcShiftTool();
	mtTrg = new TriggerTool();
	mtTofT0 = new TofT0Correction();

	// mean dca tool setup
	mtDca->SetUpperCurveParZ(-0.0531713, 1.71842, 0.465328);
	mtDca->SetLowerCurveParZ(0.0532244, -1.72135, 0.464709);
	mtDca->SetUpperCurveParXY(0.045491, 2.14648, 0.558145);
	mtDca->SetLowerCurveParXY(-0.102939, -2.14641, 0.54303);

	// centrality tool
	mtCent->SetDoMatchPileUp(true);
	mtCent->SetDoBetaPileUp(true);
	mtCent->SetDoLumi(false);
	mtCent->SetDoVz(true);
	mtCent->ReadParams();

	// CFMult tool
	mtShift->Init(
		"/star/u/yghuang/Work/DataAnalysis/BES2/14p6/yqa/ShiftFile/14GeV.shift.root",
		"ProtonShift1D", "ProtonShift2D"
	);
	mtMult->ImportShiftTool(mtShift);

	// eTOF t0 correction
	mtTofT0->SetT0(-0.19); // in case we want to use eTOF...

	return kStOK;
}

Int_t StFemtoDstMaker::Finish() {
	mOutfile->cd();
	fDstTree->Write();
	return kStOK;
}

Int_t StFemtoDstMaker::Make() {
	mPicoDstMaker = (StPicoDstMaker *)GetMaker("PicoDst");
	if (!mPicoDstMaker) {
		fputs("ERROR: StFemtoDstMaker::Init() - Can't get pointer to StPicoDstMaker!", stderr);
		return kStFATAL;
	}

	StPicoDst *mPicoDst = NULL;
	mPicoDst = mPicoDstMaker->picoDst();
	if (!mPicoDst) {
		fputs("ERROR: StFemtoDstMaker::Init() - Can't get pointer to StPicoDst!", stderr);
		return kStFATAL;
	}

	StPicoEvent *event = mPicoDst->event();

	if (!event) {
		cout << "StFemtoDstMaker::Make() No Event Found!" << endl;
		return kStOK;
	}

	Int_t runId = event->runId();

	// Vertex Cut
	TVector3 pVtx = event->primaryVertex();
	Float_t vx = event->primaryVertex().X();
	Float_t vy = event->primaryVertex().Y();
	Float_t vz = event->primaryVertex().Z();

	// wide vertex cut
	if (fabs(vz) > 50) {
		return kStOK;
	}
	Float_t vr = TMath::Sqrt(vx * vx + vy * vy);
	if (vr > 2) {
		return kStOK;
	}

	mFemtoEvent->Clear();
	std::vector<StFemtoTrack> trkArray;

	Int_t nTracks = mPicoDst->numberOfTracks();
	Int_t cent = -1;

	// check trigger ID
	Int_t trgid = mtTrg->GetTriggerID(event);
	if (trgid < 0) { return kStOK; }

	// centrality
	mtMult->make(mPicoDst);
	Int_t refMult = mtMult->mRefMult;
	Int_t nTofMatch = mtMult->mNTofMatch;
	Int_t nTofBeta = mtMult->mNTofBeta;

	Int_t refMult3 = mtMult->mRefMult3;
	refMult3 = mtCent->GetRefMult3Corr(
		refMult, refMult3, nTofMatch, nTofBeta,
		0, vz, trgid
	);
	if (refMult3 < 0) { return kStOK; }
	cent = mtCent->GetCentrality9(refMult3);
	if (cent <0 || cent >= 9) { return kStOK; }

	// check DCA
	if (!mtDca->Make(mPicoDst)) { return kStOK; }
	if (mtDca->IsBadMeanDcaZEvent(mPicoDst) || mtDca->IsBadMeanDcaXYEvent(mPicoDst)) {
		return kStOK;
	}

	// track loop
	for (Int_t i = 0; i < nTracks; i++){
		StPicoTrack *mPicoTrack = mPicoDst->track(i);
		if (!mPicoTrack){
			continue;
		}
		if (!mPicoTrack->isPrimary()){
			continue;
		}
		TVector3 momentum = mPicoTrack->pMom();
		Float_t dca = fabs(mPicoTrack->gDCA(vx, vy, vz));
		if (dca > 2){
			continue;
		}
		Float_t nHitsFit = mPicoTrack->nHitsFit();
		Float_t nHitsMax = mPicoTrack->nHitsMax();
		Float_t nHitsDedx = mPicoTrack->nHitsDedx();

		if (nHitsDedx < 5){
			continue;
		}
		if (nHitsFit < 10){
			continue;
		}
		if ((nHitsFit / nHitsMax) < 0.52){
			continue;
		}

		Float_t q = mPicoTrack->charge();
		Float_t pz = momentum.Z();
		Float_t pt = momentum.Perp();
		Float_t pcm = momentum.Mag();
		if (pt < 0.2 || pt > 2){
			continue;
		}

		Float_t EP = sqrt(pcm * pcm + 0.938272 * 0.938272);
		Float_t YP = TMath::Log((EP + pz) / (EP - pz)) * 0.5;
		double nSigProton = mPicoTrack->nSigmaProton();
        if (0.4 < pt && pt < 2.0 && fabs(YP) < 0.5) {
            nSigProton -= mtShift->GetShift(pt, YP);
        } else {
            nSigProton -= mtShift->GetShift(pcm);
        }
		if (fabs(nSigProton) >= 3){
			continue;
		}

		Float_t mass2 = -999;

		StFemtoTrack fTrack;

		// with bTOF
		bool bTofGood = true;
		if (mPicoTrack->isTofTrack()) {
			StPicoBTofPidTraits* bTofTraits = mPicoDst->btofPidTraits(mPicoTrack->bTofPidTraitsIndex());
			if (!bTofTraits) {
				bTofGood = false;
			}
			if (bTofTraits->btofMatchFlag() <= 0) {
				bTofGood = false;
			}
		} else {
			bTofGood = false;
		}
		if (bTofGood) {
			mtTofT0->ReadBTofTrack(mPicoDst, event, mPicoTrack);
			mass2 = mtTofT0->GetMass2(0.0, false); // bTOF has 0 t0
			fTrack.SetETof(0.0);
		}

		// with eTOF
		bool eTofGood = true;
		if (mPicoTrack->isETofTrack()) {
			StPicoETofPidTraits* eTofTraits = mPicoDst->etofPidTraits(mPicoTrack->eTofPidTraitsIndex());
			if (!eTofTraits) {
				eTofGood = false;
			}
			if (eTofTraits->matchFlag() <= 0) {
				eTofGood = false;
			}
		} else {
			eTofGood = false;
		}
		if (eTofGood) {
			mtTofT0->ReadETofTrack(mPicoDst, event, mPicoTrack);
			mass2 = mtTofT0->GetMass2(false);
			fTrack.SetETof(1.0);
		}

		fTrack.SetPt(pt*q);
		fTrack.SetY(YP);
		fTrack.SetP(pcm);
		fTrack.SetNHitsFit(nHitsFit);
		fTrack.SetNHitsDedx(nHitsDedx);
		fTrack.SetDca(dca);
		fTrack.SetNSigmaProton(nSigProton);
		fTrack.SetMass2(mass2);

		trkArray.push_back(fTrack);
	}

	// Fill Event Varialbles

	mFemtoEvent->SetRefMult3(refMult3);
	mFemtoEvent->SetVz(vz);
	mFemtoEvent->SetVr(vr);
	mFemtoEvent->SetRunId(runId);

	mFemtoEvent->SetStFemtoTrackArray(trkArray);

	fDstTree->Fill();

	return kStOK;
}
