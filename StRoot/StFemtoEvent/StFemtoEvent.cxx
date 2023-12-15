#include "StFemtoEvent.h"
#include "StRoot/StFemtoTrack/StFemtoTrack.h"


ClassImp(StFemtoEvent)

//____________________________________________________________
StFemtoEvent::StFemtoEvent(): TObject(),
	mRefMult3(-999), mVz(-999), mVr(-999) {
}

//____________________________________________________________
StFemtoEvent::StFemtoEvent(const StFemtoEvent &event): TObject() {

  mRefMult3 = event.mRefMult3;

  mVz = event.mVz;
  mVr = event.mVr;
  mFemtoTrackArray = event.mFemtoTrackArray;

}

//____________________________________________________________
void StFemtoEvent::ClearEvent() {
	mRefMult3 = -999;

	mVz = -999;
	mVr = -999;

	mFemtoTrackArray.clear();
}