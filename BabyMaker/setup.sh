#!/bin/bash

# Metis #
if [ ! -d ProjectMetis ]; then
  git clone "https://github.com/aminnj/ProjectMetis"
fi

cd ProjectMetis
source setup.sh
cd ..


###################
### NtupleMaker ###
###################

#USER INPUTS
CMS3Tag=102X
CMSSW_release=CMSSW_10_2_5
CMSSW_release_name=    #Leave this blank if you don't know what it is.  It's just a marker in case you have multiple identical directories.  Don't forget the underscore!
export SCRAM_ARCH=slc6_amd64_gcc700

if [ ! -d $CMSSW_release ]; then 
  #--Here there be dragons----
  export CMS_PATH=/cvmfs/cms.cern.ch
  source /cvmfs/cms.cern.ch/cmsset_default.sh
  scramv1 p -n ${CMSSW_release}${CMSSW_release_name} CMSSW $CMSSW_release
  cd ${CMSSW_release}${CMSSW_release_name}
  eval `scramv1 runtime -sh`

  # # SLOW -- default
  # git cms-init

  # FAST -- because there's a million tags to fetch nowadays
  curl -O "https://raw.githubusercontent.com/cms-sw/cms-git-tools/master/git-cms-init"
  chmod u+x git-cms-init
  sed -i 's/--tags//' git-cms-init
  sed -i 's/git fetch/git fetch --no-tags/' git-cms-init
  ./git-cms-init

  cd src

  git clone git@github.com:sam-may/NtupleMaker.git CMS3/NtupleMaker
  cd CMS3/NtupleMaker
  git checkout $CMS3Tag
  source setup/patchesToSource.sh
  cd $CMSSW_BASE/src
  scram b -j 25
  cd ..

  # see comment in patchesToSource.sh
  rm $CMSSW_BASE/lib/$SCRAM_ARCH/.poisonededmplugincache

  cd $CMSSW_BASE/src/CMS3/NtupleMaker/test/
else
  cd $CMSSW_release/src
  cmsenv
  cd ../..
fi
