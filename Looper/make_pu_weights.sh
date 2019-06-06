#source setup.sh

root -l -b -q 'makeWeightHisto_noconf.C+("pileupReweightsMC_A.root", "../histograms/pileup/Zll_histograms_pileup_A.root", "../histograms/Zll_histograms_Drell-Yan.root", "hNVtx0", "hNVtx0", "pileupReweight")'
root -l -b -q 'makeWeightHisto_noconf.C+("pileupReweightsMC_B.root", "../histograms/Zll_histograms_B.root", "../histograms/Zll_histograms_Drell-Yan.root", "hNVtx0", "hNVtx0", "pileupReweight")'
root -l -b -q 'makeWeightHisto_noconf.C+("pileupReweightsMC_C.root", "../histograms/Zll_histograms_C.root", "../histograms/Zll_histograms_Drell-Yan.root", "hNVtx0", "hNVtx0", "pileupReweight")'
root -l -b -q 'makeWeightHisto_noconf.C+("pileupReweightsMC_D.root", "../histograms/Zll_histograms_D.root", "../histograms/Zll_histograms_Drell-Yan.root", "hNVtx0", "hNVtx0", "pileupReweight")'
