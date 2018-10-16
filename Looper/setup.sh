export SCRAM_ARCH=slc6_amd64_gcc700   # or whatever scram_arch you need for your desired CMSSW release
export CMSSW_VERSION=CMSSW_10_2_5
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/cmssw/$CMSSW_VERSION/src
eval `scramv1 runtime -sh`
cd -

if [ ! -d CORE ]; then
  git clone "https://github.com/cmstas/CORE"
fi
