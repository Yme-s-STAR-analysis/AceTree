# Ace Tree Base User's Guide

## Standard Tree

Version: 4.1

Author: yghuang

Date: 29.12.2024

## Notification

What do you need to modify when dealing with a new data set:

1. replace `StRoot/CentCorrTool/CentParams.h` with the very data set

2. replace the trigger numbers in `StRoot/TriggrtTool/TriggerTool.h` 

3. there are some initialization in `StFemtoDstMaker::Init()`, including:

>> mean dca tool parameters
>>
>> n sigma shift tool root file path
>>
>> TOF t0 offset (if necessary)

## New features

> Modulized design, DCA cut, RefMult3 counting and centrality spliting, simplified usage!

## What is Ace Tree based fDst?

To use a highly efficient coding logic to speed up the fDst generating.

To prepare high-quality fDst for cumulants calculating.

Once generated, use it everywhere.

Easy to modify and deploy.

## Quick Start!

Firstly, prepare your target directory, like: `mkdir $SOME_PATH`.

Then, `cons` and `./prepare.sh $SOME_PATH` to deploy the submit directory.

After that, `cd getList` and change bad run list and file list in `conf.py`.

The `nLoc` argument depends on the location of run number in your file list.

For example, for `/star/dataXX/reco/prodXX/XX/PXXiX/20XX/1XX/RUNNUMBER/XX.picoDst.root`, `nLoc` should be `9`.

And then, using `python3 getList` to get the file list without bad runs.

Copy the path to the newly generate `file.list` and paste into `Csubmit.xml`.

Then, `star-submit Csubmit.xml` to submit your jobs.

At last, using `find $SOME_PATH -name "*root" > out.list` to get the file list of Ace Tree root files for further use.

## Advanced usage

You can modify codes in `StRoot`, like some correction or cutoff parameters.

Remember to `cons` every when you modified the codes.

If you want to test your code, prepare a `file.list` in advance, and `root -l -b -q makeFDst.C` after `cons`.

The `maxFilesPerProcess` argument in `Csubmit.xml` can be changed if you want.

If you are prety sure about your file list, (I mean all the files there are valid and accessible), `star-submit -u ie Csubmit.xml` is better than a simple `star-submit Csubmit.xml`.

## Change Log

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
