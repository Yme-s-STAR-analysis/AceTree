/*
	My StFemtoEvent v2.0
	29.09.2023 by yghuang
*/

#ifndef StFemtoEvent_h
#define StFemtoEvent_h

// C++ headers
#include <vector>
#include <iostream>

// ROOT headers
#include "TObject.h"
#include "TClonesArray.h"
#include "StRoot/StFemtoTrack/StFemtoTrack.h"
#include "TVector3.h"


class StFemtoTrack;

//_________________
class StFemtoEvent : public TObject {

	public:
  		StFemtoEvent();
  		StFemtoEvent(const StFemtoEvent &event);
  		virtual ~StFemtoEvent(){}
  		void ClearEvent();

		// Getters
		Int_t GetRefMult3()			{		return mRefMult3;			}
		Int_t GetRefMult3X()		{		return mRefMult3X;			}
		Float_t GetVz()				{		return (Float_t) mVz;		}
		Float_t GetVr()				{		return (Float_t) mVr;		}
		Int_t GetRunId()			{		return mRunId;				}

		// Setters

		void SetRefMult3(Int_t val) {		mRefMult3 = val;			}
		void SetRefMult3X(Int_t val){		mRefMult3X= val;			}
		void SetVz(Float_t val)   	{ 		mVz  = val;					}
		void SetVr(Float_t val)   	{		mVr  = val;					}
		void SetRunId(Float_t val)  { 		mRunId  = val;				}
		
		Int_t GetEntries()			{	return mFemtoTrackArray.size();	}
		
		StFemtoTrack GetFemtoTrack(int i)	{	return (StFemtoTrack) mFemtoTrackArray[i];}
		void SetStFemtoTrackArray(std::vector< StFemtoTrack > val){mFemtoTrackArray = val;}


	private:

		Int_t   mRefMult3;
		Int_t   mRefMult3X;
		Float_t mVz;
		Float_t mVr;
		std::vector< StFemtoTrack>  mFemtoTrackArray;
		Int_t mRunId;

	ClassDef(StFemtoEvent,1)

};

#endif
