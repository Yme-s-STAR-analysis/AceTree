/*
	My StFemtoTrack v2.0
	29.09.2023 by yghuang
	Now will record IsETofMass2: 1.0 for eTOF, and 0.0 for bTOF
*/
#ifndef StFemtoTrack_h
#define StFemtoTrack_h

// C++ headers
#include <cmath>

// ROOT headers
#include "TObject.h"
#include "TVector3.h"

// StFemtoTrack

//_________________
class StFemtoTrack : public TObject {

	public:
  		StFemtoTrack();
  		StFemtoTrack(const StFemtoTrack &track);
  		virtual ~StFemtoTrack(){};

		//Getters
		Float_t GetPt() 			{ 	return (Float_t) mPt/1000.0; 			}
		Float_t GetY()				{	return (Float_t) mY/1000.0;				}
		Float_t GetP()				{	return (Float_t) mP/1000.0;				}
		Short_t GetNHitsFit()		{	return (Int_t) mNHitsFit;				}
		Short_t GetNHitsDedx()		{	return (Int_t) mNHitsDedx;				}
		Float_t GetDca()			{	return (Float_t) mDca/1000.0;			}
		Float_t GetNSigmaProton()	{	return ((Float_t)mNSigmaProton)/1000.0;	}
		Float_t GetMass2()			{	return (Float_t) mMass2/1000.0;			}
        Float_t IsETofMass2()		{	return mIsETofMass2;					}


		//Setters
		void SetPt(Float_t val)		{	mPt			= val*1000;					}
		void SetY(Float_t val)		{	mY 			= val*1000;					}
		void SetP(Float_t val)		{	mP 			= val*1000;					}
		void SetNHitsFit(Int_t val)	{ 	mNHitsFit 	= val;						}
		void SetNHitsDedx(Int_t val){ 	mNHitsDedx 	= val;						}
		void SetDca(Float_t val)	{ 	mDca 		= val*1000;					}
		void SetNSigmaProton(Float_t val){ mNSigmaProton	= val*1000; 		}
		void SetMass2(Float_t val)	{ 	mMass2 		= val*1000;					}
		void SetETof(Float_t val)	{	mIsETofMass2= val;						}

	protected:

		Float_t mPt;
		Float_t mY;
		Float_t mP;
		Int_t   mNHitsFit;
		Int_t   mNHitsDedx;
		Float_t mDca;
		Float_t mNSigmaProton;
		Float_t mMass2;
		Float_t mIsETofMass2;

  	ClassDef(StFemtoTrack,1)

};

#endif
