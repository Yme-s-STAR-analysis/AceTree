# Ace Tree

Version: 4.5

Author: yghuang

## Users' Guide

What do you need to modify when dealing with a new data set:

1. replace `StRoot/CentCorrTool/CentParams.h` with the very data set

2. change the MB trigger numbers in `StRoot/TriggrtTool/TriggerTool.cxx`

3. fill in the correct value of mean dca xyz cut in `StRoot/MeanDcaTool/MeanDcaParams.h`

4. change TPC n sigma shift root file and run number mapping file in `StRoot/TpcShiftTool/MakeRunNumberHeader.py` and run that script, or you can directly replace the `RunNumber.h` with pre-generated one

5. in `StRoot/StFemtoDstMaker/StFemtoDstMaker.cxx`, change TOF t0 offset (if necessary)

## Patch Note

27.04.2024 by yghuang (v4.5):

> Vertext shift is now using VtxShiftTool, don't modify source code anymore
>
> Template xml file now only use at most 40 files per job

18.04.2024 by yghuang (v4.4):

> Updated bTOF beta and mass2 calculation
>
> And with latest `StCFMult` v2.3 (which is also updating bTOF beta and mass2)

07.04.2024 by yghuang (v4.3):

> Now using git submodule

05.04.2024 by yghuang (v4.2):

> Update: support modules:
>
>> TpcShiftTool: v2.1.1, support Ashish's shift format
>>
>> StCFMult: v2.2.1, support latest TpcShiftTool and new definitions of multiplicities
>>
>> MeanDcaCut: v2.1.1, load parameters from header file
>>
>> CentCorrTool: v6.1, using Indian method and parameters
>>
> And interfaces are changed accordingly
>
> Cuts are tighter for saving storage

29.01.2024 by yghuang (v4.1):

> New centrality definition package and RefMult3X in use.

18.12.2023 by yghuang (v4.0.1):

> Fix a bug: the mean dca cut was not applied

15.12.2023 by yghuang (v4.0):

> Updated Centrality tools and DCA tools.

2023 Oct. 15th by yghuang (3.2):

> Updated CentralityUtil.

2023 Oct. 2nd by yghuang (3.1.1):

> Using new TofT0Correction package.

2023 Sept. 29th by yghuang (3.1):

> With TofT0Correction.
>
> Better format.

2023 Aug. 11th by yghuang (3.0):

> With new StCFMult, TpcShiftUtil, CentralityUtil.

2022 Dec. 23rd by yghuang (2.0):

> Updates with newly found issues. 

2021 Nov. 3rd by yghuang (1.0):

> A realse version.
