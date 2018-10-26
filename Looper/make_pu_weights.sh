source setup.sh

root -l -b -q 'makeWeightHisto_noconf.C+("pileupReweightsMC_A.root", "histograms/Data_2018RunA.root", "histograms/DY.root", "hNVtx0", "hNVtx0", "pileupReweight")'
root -l -b -q 'makeWeightHisto_noconf.C+("pileupReweightsMC_B.root", "histograms/Data_2018RunB.root", "histograms/DY.root", "hNVtx0", "hNVtx0", "pileupReweight")'
root -l -b -q 'makeWeightHisto_noconf.C+("pileupReweightsMC_C.root", "histograms/Data_2018RunC.root", "histograms/DY.root", "hNVtx0", "hNVtx0", "pileupReweight")'
root -l -b -q 'makeWeightHisto_noconf.C+("pileupReweightsMC_D.root", "histograms/Data_2018RunD.root", "histograms/DY.root", "hNVtx0", "hNVtx0", "pileupReweight")'
root -l -b -q 'makeWeightHisto_noconf.C+("pileupReweightsMC_All.root", "histograms/Data_All.root", "histograms/DY.root", "hNVtx0", "hNVtx0", "pileupReweight")'
