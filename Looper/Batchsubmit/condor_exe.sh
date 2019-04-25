PACKAGE=package.tar.gz
OUTPUTDIR=$1
OUTPUTFILENAME=$2
INPUTFILENAMES=$3
INDEX=$4
CMSSWVER=$5
SCRAMARCH=$6
WEIGHTFILE=$7
SELECTION=$8
SCALE1FB=$9

echo "[wrapper] OUTPUTDIR       = " ${OUTPUTDIR}
echo "[wrapper] OUTPUTFILENAME  = " ${OUTPUTFILENAME}
echo "[wrapper] INPUTFILENAMES  = " ${INPUTFILENAMES}
echo "[wrapper] INDEX           = " ${INDEX}
echo "[wrapper] CMSSWVER           = " ${CMSSWVER}
echo "[wrapper] SCRAMARCH           = " ${SCRAMARCH}
echo "[wrapper] WEIGHTFILE           = " ${WEIGHTFILE}
echo "[wrapper] SELECTION           = " ${SELECTION}
echo "[wrapper] SCALE1FB           = " ${SCALE1FB}

echo "[wrapper] printing env"
printenv
echo 

echo "[wrapper] hostname  = " `hostname`
echo "[wrapper] date      = " `date`
echo "[wrapper] linux timestamp = " `date +%s`

######################
# Set up environment #
######################
export SCRAM_ARCH=slc6_amd64_gcc630   # or whatever scram_arch you need for your desired CMSSW release
source /cvmfs/cms.cern.ch/cmsset_default.sh
pushd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/cmssw/CMSSW_9_4_0/src
eval `scramv1 runtime -sh`
popd

# Untar
tar -xvf package.tar.gz

echo "[wrapper] running analyze now"
echo "[wrapper] ./analyze ${INPUTFILENAMES} ${OUTPUTFILENAME} ${INDEX} ${WEIGHTFILE} ${SELECTION} ${SCALE1FB}"
./analyze "${INPUTFILENAMES}" "${OUTPUTFILENAME}" "${INDEX}" "${WEIGHTFILE}" "${SELECTION}" "${SCALE1FB}" 

if [ "$?" != "0" ]; then
    echo "Removing output file because cmsRun crashed with exit code $?"
    rm *.root
fi

echo "[wrapper] gfal-copy output file"
gfal-copy -p -f -t 4200 --verbose file://`pwd`/${OUTPUTFILENAME}_${INDEX}.root gsiftp://gftp.t2.ucsd.edu/${OUTPUTDIR}/${OUTPUTFILENAME}_${INDEX}.root --checksum ADLER32
