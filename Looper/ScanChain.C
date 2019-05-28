
// ROOT
#include "TBenchmark.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTreeCache.h"
#include "TH2D.h"

// CMS3
#include "../../CORE/CMS3.cc"
#include "../../CORE/Base.cc"
#include "../../CORE/Config.cc"
#include "../../CORE/Tools/JetCorrector.h"
#include "../../CORE/TriggerSelections.cc"
#include "../../CORE/IsolationTools.cc"
#include "../../CORE/ElectronSelections.cc"
#include "../../CORE/MuonSelections.cc"
#include "../../CORE/Tools/JetCorrector.cc"
#include "../../CORE/Tools/goodrun.cc"
#include "../../CORE/MetSelections.cc"
#include "../../CORE/VertexSelections.cc"

//#include "ScanChain.h"
#include "MetHelper.h"

using namespace std;
using namespace tas;

const TString elT = "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v";
const TString muT1 = "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v";
const TString muT2 = "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v";
const TString muT3 = "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v";

const int nEtaRegions = 6;
const int nCandCats = 4;
const int nMETVars = 3;

int ScanChain(TChain* chain, TString output_name, vector<TString> vWeightFile, bool puReweight, int selection, double scale1fb, double target_lumi = 1.) {
  gconf.year = -1;
  // Benchmark
  TBenchmark *bmark = new TBenchmark();
  bmark->Start("benchmark");

  int nHists = puReweight ? vWeightFile.size() : 1; 
  cout << nHists << endl;
  if (puReweight) cout << "puReweight" << endl;
  // Loop over events to Analyze
  unsigned int nEventsTotal = 0;
  unsigned int nEventsChain = chain->GetEntries();
  TObjArray *listOfFiles = chain->GetListOfFiles();
  TIter fileIter(listOfFiles);
  TFile *currentFile = 0;

  TFile* f1 = new TFile(output_name+".root", "RECREATE");
  f1->cd();

  // Pf cand MET, Sum E_T, MET phi for each eta region and pf candidate category
  vector<vector<vector<vector<TH1D*>>>> vhMET = create_met_histograms_vector(nEtaRegions, nCandCats, nHists);

  // MET
  vector<TH1D*> hT1CMET = create_histogram_vector("hT1CMET", 80, 0, 400, nHists);
  vector<TH1D*> hMHT = create_histogram_vector("hMHT", 80, 0, 400, nHists);

  // Dilepton Mass
  vector<TH1D*> hDilepMass = create_histogram_vector("hDilepMass", 100, 0, 250, nHists);
  vector<TH1D*> hDilepMassEE = create_histogram_vector("hDilepMassEE", 100, 0, 250, nHists);
  vector<TH1D*> hDilepMassMuMu = create_histogram_vector("hDilepMassMuMu", 100, 0, 250, nHists);

  // Pf Cand pT
  vector<TH1D*> hCCpT = create_histogram_vector("hCCpT", 100, 0, 100, nHists); 
  vector<TH1D*> hPhotonpT = create_histogram_vector("hPhotonpT", 100, 0, 100, nHists); 
  vector<TH1D*> hNeutpT = create_histogram_vector("hNeutpT", 100, 0, 100, nHists); 
  vector<vector<TH1D*>> vhCandpT = {hCCpT, hPhotonpT, hNeutpT};
 
  // Pf Cand eta
  vector<TH1D*> hCCeta = create_histogram_vector("hCCeta", 50, 0, 5, nHists);
  vector<TH1D*> hPhotoneta = create_histogram_vector("hPhotoneta", 50, 0, 5, nHists);
  vector<TH1D*> hNeuteta = create_histogram_vector("hNeuteta", 50, 0, 5, nHists);
  vector<vector<TH1D*>> vhCandeta = {hCCeta, hPhotoneta, hNeuteta};

  // Candidate multiplicity
  vector<TH1D*> hNCands = create_histogram_vector("hNCands", 300, 0, 3000, nHists);
  vector<TH1D*> hNCCands = create_histogram_vector("hNCCands", 300, 0, 3000, nHists);
  vector<TH1D*> hNPCands = create_histogram_vector("hNPCands", 300, 0, 3000, nHists);
  vector<TH1D*> hNNCands = create_histogram_vector("hNNCands", 300, 0, 3000, nHists);

  // Jets
  vector<TH1D*> hNJets = create_histogram_vector("hNJets", 16, -0.5, 15.5, nHists);
  vector<TH1D*> hLeadJetPt = create_histogram_vector("hLeadJetPt", 100, 0, 500, nHists);
  vector<TH1D*> hLeadJetEta = create_histogram_vector("hLeadJetEta", 50, 0, 5, nHists);

  vector<TH1D*> hJetPt = create_histogram_vector("hJetPt", 100, 0, 500, nHists);
  vector<TH1D*> hJetEta = create_histogram_vector("hJetEta", 50, 0, 5, nHists);  

  vector<TH2D*> hJetEtaPhi = create_2Dhistogram_vector("hJetEtaPhi", 50, -5, 5, 50, -3.142, 3.142, nHists);
  vector<TH2D*> hJetPtEta = create_2Dhistogram_vector("hJetPtEta", 50, 0, 500, 50, -5, 5, nHists);
  vector<TH2D*> hJetPtPhi = create_2Dhistogram_vector("hJetPtPhi", 50, 0, 500, 50, -3.142, 3.142, nHists);
  vector<TH1D*> hJetUncorrPt = create_histogram_vector("hJetUncorrPt", 100, 0, 500, nHists);

  vector<TH1D*> hJet_Emfrac = create_histogram_vector("hJet_Emfrac", 100, 0, 1, nHists);
  vector<TH1D*> hJet_Neutral_Emfrac = create_histogram_vector("hJet_Neutral_Emfrac", 100, 0, 1, nHists);
  vector<TH1D*> hJet_Neutral_Hadfrac = create_histogram_vector("hJet_Neutral_Hadfrac", 100, 0, 1, nHists);
  vector<TH1D*> hJet_Charged_Hadfrac = create_histogram_vector("hJet_Charged_Hadfrac", 100, 0, 1, nHists);
  vector<TH1D*> hJet_Neutral_Emfrac_hf = create_histogram_vector("hJet_Neutral_Emfrac_hf", 100, 0, 1, nHists);
  vector<TH1D*> hJet_Neutral_Hadfrac_hf = create_histogram_vector("hJet_Neutral_Hadfrac_hf", 100, 0, 1, nHists);

  vector<TH1D*> hJet_Neutral_Emfrac_central = create_histogram_vector("hJet_Neutral_Emfrac_central", 100, 0, 1, nHists);
  vector<TH1D*> hJet_Neutral_Hadfrac_central = create_histogram_vector("hJet_Neutral_Hadfrac_central", 100, 0, 1, nHists);
  vector<TH1D*> hJet_Neutral_Emfrac_forward = create_histogram_vector("hJet_Neutral_Emfrac_forward", 100, 0, 1, nHists);
  vector<TH1D*> hJet_Neutral_Hadfrac_forward = create_histogram_vector("hJet_Neutral_Hadfrac_forward", 100, 0, 1, nHists);

  // Other
  vector<TH1D*> hVertexZ = create_histogram_vector("hVertexZ", 100, -10, 10, nHists);
  vector<TH1D*> hVertexZ_forwardPhoton = create_histogram_vector("hVertexZ_forwardPhoton", 100, -10, 10, nHists);
  
  MetHelper* mV8 = new MetHelper("V8", nHists, "V8", "V8", 0);
  mV8->create_raw_met_histograms();

  double vtxBins[] = {0,10,15,20,25,30,35,40,45,50,60,75,100};
  int nVtxBins = (sizeof(vtxBins)/sizeof(vtxBins[0]))-1;
  vector<TH1D*> hNVtx;
  vector<TH1D*> hNVtx_up, hNVtx_down;
  for (int i = 0; i < nHists; i++) { 
   TString name = "hNVtx"+to_string(i);
   hNVtx.push_back(new TH1D(name,"", nVtxBins, vtxBins));
   hNVtx[i]->Sumw2();
   hNVtx_up.push_back(new TH1D(name + "_up","", nVtxBins, vtxBins));
   hNVtx_up[i]->Sumw2();
   hNVtx_down.push_back(new TH1D(name + "_down","", nVtxBins, vtxBins));
   hNVtx_down[i]->Sumw2();
  }


  vector<TFile*> fWeights;
  vector<TH1D*> hWeights;
  vector<TFile*> fWeights_up;
  vector<TH1D*> hWeights_up;
  vector<TFile*> fWeights_down;
  vector<TH1D*> hWeights_down;
  if (puReweight) {
    for (int i = 0; i < nHists; i++) {
      fWeights.push_back(new TFile(vWeightFile[i], "READ"));
      hWeights.push_back((TH1D*)fWeights[i]->Get("pileupReweight"));
      //fWeights_down.push_back(new TFile(vWeightFile[i].ReplaceAll("MC", "MC_down"), "READ"));
      //hWeights_down.push_back((TH1D*)fWeights_down[i]->Get("pileupReweight_down"));
      //fWeights_up.push_back(new TFile(vWeightFile[i].ReplaceAll("MC", "MC_up"), "READ"));
      //hWeights_up.push_back((TH1D*)fWeights_up[i]->Get("pileupReweight_up"));
    }
  } 

  // File Loop
  while ( (currentFile = (TFile*)fileIter.Next()) ) {
    TString currentFileName = currentFile->GetTitle();
    TFile file(currentFileName);
    TTree *tree = (TTree*)file.Get("Events");
    TTreeCache::SetLearnEntries(10);
    tree->SetCacheSize(128*1024*1024);
    cms3.Init(tree);
    if (nEventsTotal >= nEventsChain) continue;
    unsigned int nEventsTree = tree->GetEntriesFast();

    const char* json_file;
    if (currentFileName.Contains("2016")) {
      json_file = "Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON_snt.txt";
      set_goodrun_file(json_file);
      gconf.year = 2016;
    }
    else if (currentFileName.Contains("2017")) {
      json_file = "Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_snt.txt"; 
      set_goodrun_file(json_file);
      gconf.year = 2017;
    }
    else if (currentFileName.Contains("2018")) {
      //json_file = "Cert_314472-322633_13TeV_PromptReco_Collisions18_JSON_snt.txt";
      json_file = "Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON_snt.txt";
      set_goodrun_file(json_file);
      gconf.year = 2018;
    } 

    for (unsigned int event = 0; event < nEventsTree; ++event) {
      if (nEventsTotal >= nEventsChain) continue;
      tree->LoadTree(event);
      cms3.GetEntry(event);
      ++nEventsTotal;
      CMS3::progress( nEventsTotal, nEventsChain );
  
      // Check golden json 
      if (evt_isRealData()) {
        if (!goodrun(evt_run(), evt_lumiBlock())) {  continue; }
      }

      // Check triggers, categorize as ee/uu
      bool isElEvt;
      int id1(-1), id2(-1);
      if (cms3.evt_isRealData()) {
        if (!(passHLTTriggerPattern(elT) ||  passHLTTriggerPattern(muT1) || passHLTTriggerPattern(muT2) || passHLTTriggerPattern(muT3))) continue;
        if (passHLTTriggerPattern(elT))                                          isElEvt = true;
        else if (passHLTTriggerPattern(muT1) || passHLTTriggerPattern(muT2) || passHLTTriggerPattern(muT3))      isElEvt = false;
      }

      else {
        if (lepsPassPOG(true, id1, id2))
          isElEvt = true;
        else if (lepsPassPOG(false, id1, id2))
          isElEvt = false;
        else continue;
      }

      if (selection == 3) {
	if (!isElEvt)				continue;
      }

      // Check filters
      if (firstGoodVertex() == -1)		continue;
      if (!filt_goodVertices())          	continue;
      if (!filt_globalSuperTightHalo2016())   	continue;
      if (!filt_hbheNoise())             	continue;
      if (!filt_hbheNoiseIso())          	continue; 
      if (!filt_ecalTP())			continue;
      if (evt_isRealData()) { if (!filt_eeBadSc()) continue; } // only for data
      if (!filt_BadPFMuonFilter())		continue;
      if (!filt_BadChargedCandidateFilter())	continue;
      if (!filt_ecalBadCalibFilter())		continue;

      // Check if leps pass POG IDs
      if (!lepsPassPOG(isElEvt, id1, id2)) 						continue;
      if (!opposite_sign(isElEvt, id1, id2))						continue;

      // Check pT/eta requirements
      if (!lepsPassOtherLenient(isElEvt, id1, id2))                                   continue;

      // Grab pu weights
      vector<double> weight(nHists, 1);
      vector<double> weight_up(nHists, 1);
      vector<double> weight_down(nHists, 1);
      int nvtx = evt_nvtxs();
      if (puReweight) {
        for (int i = 0; i < nHists; i++) {
	  weight[i] *= hWeights[i]->GetBinContent(hWeights[i]->FindBin(nvtx));
	  //weight_up[i] *= hWeights_up[i]->GetBinContent(hWeights_up[i]->FindBin(nvtx));
	  //weight_down[i] *= hWeights_down[i]->GetBinContent(hWeights_down[i]->FindBin(nvtx));
        }
      }
      // Weight further if MC
      if (!cms3.evt_isRealData()) {
	for (int i = 0; i < nHists; i++) {
          weight[i] *= scale1fb * target_lumi *sgn(genps_weight());
	  //weight_up[i] *= scale1fb * target_lumi *sgn(genps_weight());
	  //weight_down[i] *= scale1fb * target_lumi *sgn(genps_weight());
	}
      }

      // Fill dilep mass before cutting on it
      double ZpT = -1;
      double dMass = dilepMass(isElEvt, id1, id2, ZpT);
      fill_histograms(hDilepMass,dMass, weight);
      if (isElEvt)
	fill_histograms(hDilepMassEE,dMass, weight);
      else
	fill_histograms(hDilepMassMuMu,dMass, weight);


      if (dMass < 81 || dMass > 101)                                                  continue;

      cout << event << " / " << nEventsTree << endl;

      fill_histograms(hNVtx, nvtx, weight);
      if (selection == 2)	continue;
      

      // Done with selection, now fill histograms
      double lead_jet_eta = cms3.pfjets_p4().size() > 0 ? abs(cms3.pfjets_p4().at(0).eta()) : 999;
      double pu = cms3.evt_nvtxs();

      int nJet = nJets(isElEvt, id1, id2);

      vector<double> vId = {pu, lead_jet_eta};      

      mV8->fill_met_histograms(currentFileName, isElEvt, id1, id2, nJet, weight, vId); 
      mV8->fill_raw_met_histograms(isElEvt, id1, id2, nJet, weight);
      //ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fMET = t1CMET(currentFileName, 0, "V6", "V6", 0);
      //fill_histograms(hT1CMET, fMET.pt(), weight);


      int nCands = pfcands_p4().size();
      fill_histograms(hNCands,nCands, weight);

      vector<vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> >>> vFourVec(nEtaRegions, vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> >>(nCandCats));
      vector<vector<double>> vSumET(nEtaRegions, vector<double>(nCandCats, 0.0));

      int nCCands(0), nPCands(0), nNCands(0);

      for (int i = 0; i < cms3.pfjets_p4().size(); i++) {
	ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> jet_p4 = cms3.pfjets_p4().at(i);
	//ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> jet_p4 = cms3.pfjets_p4().at(i)*cms3.pfjets_undoJEC().at(i);	
	if (!(jet_p4.pt() > 0)) continue;
	if (i == 0) {
	  fill_histograms(hLeadJetPt, jet_p4.pt(), weight);
	  fill_histograms(hLeadJetEta, abs(cms3.pfjets_p4().at(0).eta()), weight); 
	}
	fill_histograms(hJetPt, jet_p4.pt(), weight);
	fill_histograms(hJetEta, abs(cms3.pfjets_p4().at(i).eta()), weight);
 
	float emfrac = (cms3.pfjets_chargedEmE().at(i) + cms3.pfjets_neutralEmE().at(i)) / jet_p4.E();
        float neutral_emfrac = cms3.pfjets_neutralEmE().at(i) / jet_p4.E();
        float neutral_hadfrac = cms3.pfjets_neutralHadronE().at(i) / jet_p4.E();
        float charged_hadfrac = cms3.pfjets_chargedHadronE().at(i) / jet_p4.E();
        float neutral_emfrac_hf = cms3.pfjets_hfEmE().at(i) / jet_p4.E();
        float neutral_hadfrac_hf = cms3.pfjets_hfHadronE().at(i) / jet_p4.E();

        if (abs(cms3.pfjets_p4().at(i).eta()) <= 3.0) {
          fill_histograms(hJet_Emfrac, emfrac, weight);
          fill_histograms(hJet_Neutral_Emfrac, neutral_emfrac, weight);
          fill_histograms(hJet_Neutral_Hadfrac, neutral_hadfrac, weight);
          fill_histograms(hJet_Charged_Hadfrac, charged_hadfrac, weight);
        }
        else {
          fill_histograms(hJet_Neutral_Emfrac_hf, neutral_emfrac_hf, weight);
          fill_histograms(hJet_Neutral_Hadfrac_hf, neutral_hadfrac_hf, weight);
        }

        if (abs(cms3.pfjets_p4().at(i).eta()) <= 2.7) {
          fill_histograms(hJet_Neutral_Emfrac_central, neutral_emfrac, weight);
          fill_histograms(hJet_Neutral_Hadfrac_central, neutral_hadfrac, weight);
        }
        else if (abs(cms3.pfjets_p4().at(i).eta()) <= 3.0) {
          fill_histograms(hJet_Neutral_Emfrac_forward, neutral_emfrac, weight);
          fill_histograms(hJet_Neutral_Hadfrac_forward, neutral_hadfrac, weight);
        }


      }

      fill_histograms(hVertexZ, cms3.vtxs_position().at(0).z(), weight);
      for (int i=0; i<nCands; i++) { // begin pf cand loop
	ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fourV = cms3.pfcands_p4().at(i);
        double pt = fourV.Pt();
        double phi = fourV.Phi();
        double etaS = fourV.eta();
        double eta = abs(etaS);
        int etaIdx = sortEta(eta);
        int candIdx = -1;
        int particleId = abs(pfcands_particleId().at(i));

	if (abs(pfcands_charge().at(i)) > 0)             { candIdx = 0; nCCands++; }        // charged candidate
        else if (particleId == 22 )                      { candIdx = 1; nPCands++; }        // photon candidate
        else                                             { candIdx = 2; nNCands++; }        // neutral hadron candidate

	if (candIdx == 1 && eta > 3.5) {
	  fill_histograms(hVertexZ_forwardPhoton, cms3.vtxs_position().at(cms3.pfcands_IdAssociatedPV().at(i)).z(), weight);
	}

	fill_histograms(vhCandpT[candIdx], pt, weight);
	fill_histograms(vhCandeta[candIdx], eta, weight);
	vFourVec[etaIdx][candIdx] += fourV;
	vFourVec[etaIdx][3] += fourV;
	vFourVec[5][3] += fourV;
	vSumET[etaIdx][candIdx] += pt;
	vSumET[etaIdx][3] += pt;
	vSumET[5][3] += pt;

   
      } // end pf cand loop
      fill_histograms(hNCCands,nCCands, weight);
      fill_histograms(hNPCands,nPCands, weight);
      fill_histograms(hNNCands,nNCands, weight);

      // Fill pf cand histos
      for (int i=0; i<nEtaRegions; i++) {
        for (int j=0; j<nCandCats; j++) {
          if (vSumET[i][j] == 0) { continue; }
	  fill_histograms(vhMET[i][j][0], vFourVec[i][j].Pt(), weight);
          fill_histograms(vhMET[i][j][1], vSumET[i][j], weight);
	  fill_histograms(vhMET[i][j][2], vFourVec[i][j].Phi(), weight);
        }
      }
    }
    delete tree;
    file.Close();
  }
  f1->cd();
  f1->Write();
  f1->Close();
  delete f1;

  if (nEventsChain != nEventsTotal) {
    cout << Form( "ERROR: number of events from files (%d) is not equal to total number of events (%d)", nEventsChain, nEventsTotal ) << endl;
  }

  bmark->Stop("benchmark");
  cout << endl;
  cout << nEventsTotal << " Events Processed" << endl;
  cout << "------------------------------" << endl;
  cout << "CPU  Time: " << Form( "%.01f", bmark->GetCpuTime("benchmark")  ) << endl;
  cout << "Real Time: " << Form( "%.01f", bmark->GetRealTime("benchmark") ) << endl;
  cout << endl;
  delete bmark;
  return 0;
}
