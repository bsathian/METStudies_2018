export SCRAM_ARCH=slc6_amd64_gcc630
export CMSSW_VERSION=CMSSW_9_4_9
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/cmssw/$CMSSW_VERSION/src
eval `scramv1 runtime -sh`
cd -

pushd /home/users/sjmay/Commissioning2018/Commissioning2018_MET/BabyMaker/ProjectMetis
source setup.sh
popd
