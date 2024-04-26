
#include <iostream>
#include <limits>
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
#include "StPicoEvent/StPicoPhysicalHelix.h"
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
#include "StRoot/VtxShiftTool/VtxShiftTool.h"

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
	mtVtx = new VtxShiftTool();

	// mean dca tool setup
	mtDca->ReadParams();

	// centrality tool
	mtCent->EnableIndianMethod(true);
	mtCent->ReadParams();

	// Multiplicity and shift tool
	mtShift->Init();
	mtMult->ImportShiftTool(mtShift);

	// vertex shift tool
	mtVtx = new VtxShiftTool();

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
	mPicoDst = mPicoDstMaker->mPicoDst();
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

	if (fabs(vx) < 1.e-5 && 
		fabs(vy) < 1.e-5 &&
		fabs(vz) < 1.e-5) {
		return kStOK;
	}

	// wide vertex cut -> tight cut now
	if (fabs(vz) > 50) {
		return kStOK;
	}
	auto vr = mtVtx->GetShiftedVr(vx, vy);
	if (vr > 1) { // shifted vr < 1
		return kStOK;
	}

	mFemtoEvent->Clear();
	std::vector<StFemtoTrack> trkArray;

	Int_t nTracks = mPicoDst->numberOfTracks();
	Int_t cent = -1;
	Int_t centX = -1;

	// check trigger ID
	Int_t trgid = mtTrg->GetTriggerID(event);
	if (trgid < 0) { return kStOK; }

	// centrality
	mtMult->make(mPicoDst);
	Int_t refMult = mtMult->mRefMult;
	Int_t tofMult = mtMult->mTofMult;
	Int_t nTofMatch = mtMult->mNTofMatch;
	Int_t nTofBeta = mtMult->mNTofBeta;

	Int_t refMult3 = mtMult->mRefMult3;
	refMult3 = mtCent->GetIndianRefMult3Corr(
		refMult, refMult3, tofMult, nTofMatch, nTofBeta,
		vz, false
	);
	Int_t refMult3X = mtMult->mRefMult3X;
	refMult3X = mtCent->GetIndianRefMult3Corr(
		refMult, refMult3X, tofMult, nTofMatch, nTofBeta,
		vz, true
	);

	if (refMult3 < 0) { return kStOK; }
	if (refMult3X < 0) { return kStOK; }
	cent = mtCent->GetCentrality9(refMult3);
	centX = mtCent->GetCentrality9(refMult3X, true);
	if (cent < 0 && centX < 0) { return kStOK; } // only skip this event when both of them are invalid

	// check DCA
	if (!mtDca->Make(mPicoDst)) { return kStOK; }
	if (mtDca->IsBadMeanDcaZEvent(mPicoDst) || mtDca->IsBadMeanDcaXYEvent(mPicoDst)) {
		return kStOK;
	}

	// track loop
	const Float_t mField = event->bField();
	for (Int_t i = 0; i < nTracks; i++){
		StPicoTrack *mPicoTrack = mPicoDst->track(i);
		if (!mPicoTrack) { continue; }
		if (!mPicoTrack->isPrimary()){ continue; }
		TVector3 momentum = mPicoTrack->pMom();
		// Float_t dca = fabs(mPicoTrack->gDCA(vx, vy, vz));
		StPicoPhysicalHelix helix = mPicoTrack->helix(mField);
		Double_t dca = fabs(helix.geometricSignedDistance(pVtx));
		if (dca > 1.3){ continue; }
		Float_t nHitsFit = mPicoTrack->nHitsFit();
		Float_t nHitsMax = mPicoTrack->nHitsMax();
		Float_t nHitsDedx = mPicoTrack->nHitsDedx();

		if (nHitsDedx < 5) { continue; }
		if (nHitsFit < 14){ continue; }
		if ((nHitsFit / nHitsMax) < 0.52){ continue; }

		Float_t q = mPicoTrack->charge();
		Float_t pz = momentum.Z();
		Float_t pt = momentum.Perp();
		Float_t pcm = momentum.Mag();
		Float_t eta = momentum.PseudoRapidity();
		if (pt <= 0.3 || pt >= 2){ continue; }

		Float_t EP = sqrt(pcm * pcm + 0.938272 * 0.938272);
		Float_t YP = TMath::Log((EP + pz) / (EP - pz)) * 0.5;
		double nSigProton = mPicoTrack->nSigmaProton();
		nSigProton -= mtShift->GetShift(runId, pt, eta);
		if (fabs(nSigProton) > 2.6){ continue; }

		Float_t mass2 = -999;

		StFemtoTrack fTrack;

		// with bTOF
        Int_t tofId = mPicoTrack->bTofPidTraitsIndex();
        Int_t btofMatchFlag = 0;
        Double_t beta = -1.0;
        Double_t btofYLocal = -999.0;
        if (tofId >= 0) {
            StPicoBTofPidTraits* tofPid = mPicoDst->btofPidTraits(tofId);
            btofMatchFlag = tofPid->btofMatchFlag();
            if (tofPid) {
                beta = tofPid->btofBeta();
                btofYLocal = tofPid->btofYLocal();
                if (beta < 1e-4) { // recalculate time of flight
                    Double_t tof = tofPid->btof();
                    TVector3 btofHitPos = tofPid->btofHitPos();
                    const StThreeVectorF* btofHitsPosSt = new StThreeVectorF(
                        btofHitPos.X(), btofHitPos.Y(), btofHitPos.Z()
                    );
                    const StThreeVectorF* vtxPosSt = new StThreeVectorF(
                        vx, vy, vz
                    );
                    Double_t L = tofPathLength(vtxPosSt, btofHitsPosSt, helix.curvature());
                    beta = tof > 0 ? L / (tof * (C_C_LIGHT/1.e9)) : std::numeric_limits<Float_t>::quiet_NaN(); // note: quiet nan will never pass > N or < N
                }
            }
        }
        if (btofMatchFlag > 0 && beta > 0 && fabs(btofYLocal) < 1.8) {
            mass2 = pcm * pcm * (pow(1.0 / beta, 2) - 1);
        }

		fTrack.SetPt(pt*q);
		fTrack.SetY(YP);
		fTrack.SetP(pcm);
		fTrack.SetNHitsFit(nHitsFit);
		fTrack.SetDca(dca);
		fTrack.SetNSigmaProton(nSigProton);
		fTrack.SetMass2(mass2);

		trkArray.push_back(fTrack);
	}

	// Fill Event Varialbles

	mFemtoEvent->SetRefMult3(refMult3);
	mFemtoEvent->SetRefMult3X(refMult3X);
	mFemtoEvent->SetVz(vz);

	mFemtoEvent->SetStFemtoTrackArray(trkArray);

	fDstTree->Fill();

	return kStOK;
}
