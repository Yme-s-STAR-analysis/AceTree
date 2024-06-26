#include "StFemtoEvent.h"
#include "StRoot/StFemtoTrack/StFemtoTrack.h"


ClassImp(StFemtoEvent)

//____________________________________________________________
StFemtoEvent::StFemtoEvent(): TObject(),
	mRefMult3(-999), mRefMult3X(-999), mVz(-999) {
}

//____________________________________________________________
StFemtoEvent::StFemtoEvent(const StFemtoEvent &event): TObject() {

	mRefMult3 = event.mRefMult3;
	mRefMult3X = event.mRefMult3X;

	mVz = event.mVz;
	mFemtoTrackArray = event.mFemtoTrackArray;

}

//____________________________________________________________
void StFemtoEvent::ClearEvent() {
	mRefMult3 = -999;
	mRefMult3X= -999;

	mVz = -999;

	mFemtoTrackArray.clear();
}