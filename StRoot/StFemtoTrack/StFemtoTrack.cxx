
#include "TObject.h"
#include <iostream>
#include "StFemtoTrack.h"

ClassImp(StFemtoTrack)

//___________________________________________________
StFemtoTrack::StFemtoTrack() : TObject(),
	mPt(-999), mY(-999), mP(-999), mNHitsFit(-122),
	mNHitsDedx(-999), mDca(-999),
	mNSigmaProton(-122), mMass2(-999), mIsETofMass2(0.0) {
}

//___________________________________________________
StFemtoTrack::StFemtoTrack(const StFemtoTrack &track) : TObject() {

	mPt = track.mPt;
	mY = track.mY;
	mP = track.mP;

	mNHitsFit = track.mNHitsFit;
	mNHitsDedx = track.mNHitsDedx;
	mDca  = track.mDca;

	mNSigmaProton = track.mNSigmaProton;
	mMass2 = track.mMass2;
	mIsETofMass2 = track.mIsETofMass2;

}