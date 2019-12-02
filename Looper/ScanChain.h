bool goodMuon(unsigned int muIdx) {
  if (!isTightMuonPOG(muIdx))           { return false; }
  if (muRelIso03EA(muIdx,1) > 0.4)      { return false; }
  return true;
}

bool lepsPassPOG(bool isElEvt, int &id1, int &id2) {
  // Are there at least 2 leptons?
  // If electrons, do they pass loose POG ID?
  // If muons, do they pass tight POG ID?
  int nLeps;
  if (isElEvt)
    nLeps = (els_p4()).size();
  else
    nLeps = (mus_p4()).size();
  int nGoodLeps = 0;
  for (int i=0; i<nLeps; i++) {
    if (isElEvt) {
      if (isLooseElectronPOGspring16_v1(i)) {
        nGoodLeps++;
        if (nGoodLeps == 1) id1 = i;
        else if (nGoodLeps == 2) id2 = i;
      }
    }
    else {
      if (goodMuon(i)) {
        nGoodLeps++;
        if (nGoodLeps == 1) id1 = i;
        else if (nGoodLeps == 2) id2 = i;
      }
    }
  }
  return (nGoodLeps == 2);
}

bool opposite_sign(bool isElEvt, int id1, int id2) {
  double charge_product;
  if (isElEvt)
    charge_product = els_charge().at(id1)*els_charge().at(id2);
  else
    charge_product = mus_charge().at(id1)*mus_charge().at(id2);
  return (charge_product < 0); 
}

double dilepMass(bool isElEvt, int id1, int id2, double &ZpT) {
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fZ;
  if (isElEvt)
    fZ = els_p4().at(id1)+els_p4().at(id2);
  else
    fZ = mus_p4().at(id1)+mus_p4().at(id2);
  ZpT = fZ.pt();
  return fZ.mass();
}

double dilepPhi(bool isElEvt, int id1, int id2) {
  if (isElEvt)
    return (els_p4().at(id1)+els_p4().at(id2)).phi();
  else
    return (mus_p4().at(id1)+mus_p4().at(id2)).phi();
}

int lepsEtaRegion(bool isElEvt, int id1, int id2) {
  if (isElEvt) {
    if (abs(els_p4().at(id1).eta()) < 1.3 && abs(els_p4().at(id2).eta()) < 1.3)
      return 0;
    else if (abs(els_p4().at(id1).eta()) > 1.3 && abs(els_p4().at(id2).eta()) > 1.3)
      return 1;
    else
      return 2;
  }
  else {
    if (abs(mus_p4().at(id1).eta()) < 1.3 && abs(mus_p4().at(id2).eta()) < 1.3)
      return 0;
    else if (abs(mus_p4().at(id1).eta()) > 1.3 && abs(mus_p4().at(id2).eta()) > 1.3)
      return 1;
    else
      return 2;
  }
}

bool lepsPassOther(bool isElEvt, int id1, int id2) {
  if (isElEvt) {
    if ((els_p4().at(id1)).Pt() >= 100 && (els_p4().at(id2)).Pt() >= 30
          && abs((els_p4().at(id1)).eta()) < 2.4 && abs((els_p4().at(id2)).eta()) < 2.4
          && (abs((els_p4().at(id1)).eta()) < 1.4 || abs((els_p4().at(id1)).eta()) > 1.6)
          && (abs((els_p4().at(id2)).eta()) < 1.4 || abs((els_p4().at(id2)).eta()) > 1.6)) { return true; }
    else { return false; }
  }
  else {
    if ((mus_p4().at(id1)).Pt() >= 100 && (mus_p4().at(id2)).Pt() >= 30
          && abs((mus_p4().at(id1)).eta()) < 2.4 && abs((mus_p4().at(id2)).eta()) < 2.4) { return true; }
    else { return false; }
  }
}

bool lepsPassOtherLenient(bool isElEvt, int id1, int id2) {
  double lead_pt = 25;
  double sublead_pt = 20;
  if (isElEvt) {
    if ((els_p4().at(id1)).Pt() >= lead_pt && (els_p4().at(id2)).Pt() >= sublead_pt
          && abs((els_p4().at(id1)).eta()) < 2.4 && abs((els_p4().at(id2)).eta()) < 2.4
          && (abs((els_p4().at(id1)).eta()) < 1.4 || abs((els_p4().at(id1)).eta()) > 1.6)
          && (abs((els_p4().at(id2)).eta()) < 1.4 || abs((els_p4().at(id2)).eta()) > 1.6)) { return true; }
    else { return false; }
  }
  else {
    if ((mus_p4().at(id1)).Pt() >= lead_pt && (mus_p4().at(id2)).Pt() >= sublead_pt
          && abs((mus_p4().at(id1)).eta()) < 2.4 && abs((mus_p4().at(id2)).eta()) < 2.4) { return true; }
    else { return false; }
  }
}

int sortEta(double eta) {
  if (eta >= 0 && eta < 1.3)            { return 0; }
  else if (eta >= 1.3 && eta < 1.6)     { return 1; }
  else if (eta >= 1.6 && eta < 2.5)     { return 2; }
  else if (eta >= 2.5 && eta < 3.0)     { return 3; }
  else if (eta >= 3.0)                  { return 4; }

}

double DeltaR(const LorentzVector p1, const LorentzVector p2){
  /*Returns the DeltaR between objects p1 and p2.*/
  double dphi = acos( cos( p1.phi() - p2.phi() ) );
  return sqrt( (p1.eta() - p2.eta())*(p1.eta() - p2.eta())+ dphi*dphi );
}

bool low_pt_ec_jet() {
  for (int i = 0; i < pfjets_p4().size(); i++) { 
    if (abs(pfjets_p4()[i].eta()) > 2.65 && abs(pfjets_p4()[i].eta()) < 3.139 && pfjets_p4()[i].pt() > 25 && pfjets_p4()[i].pt() < 50) {
    //cout << "Jet with eta: " << abs(pfjets_p4()[i].eta()) << " and pt: " << pfjets_p4()[i].pt() << endl;
    return true;
  }
  }
  return false;
}


int nJets(bool isElEvt, int id1, int id2) {
  int n = 0;
  int nJetCands = pfjets_p4().size();
  for (int i=0; i<nJetCands; i++) {
    if ((pfjets_p4().at(i)).Pt() < 35)                  { continue; }
    if (abs((pfjets_p4().at(i)).eta()) > 3.0)           { continue; }
    LorentzVector pl1, pl2;
    if (isElEvt) {
      pl1 = els_p4().at(id1);
      pl2 = els_p4().at(id2);
    }
    else {
      pl1 = mus_p4().at(id1);
      pl2 = mus_p4().at(id2);
    }
    if (DeltaR(pfjets_p4().at(i),pl1) < 0.4)            { continue; }
    if (DeltaR(pfjets_p4().at(i),pl2) < 0.4)            { continue; }
    //if (!isLoosePFJet(i)) { continue; }
    n++;
  }
  return n;
}

double ZRemovedMET(ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fT1CMET, bool isElEvt, int id1, int id2, double &phiZ) {
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fZRemoved = fT1CMET;
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fLep1;
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fLep2;
  if (isElEvt) {
    fLep1 = els_p4().at(id1);
    fLep2 = els_p4().at(id2);
  }
  else {
    fLep1 = mus_p4().at(id1);
    fLep2 = mus_p4().at(id2);
  }
  fZRemoved += fLep1;
  fZRemoved += fLep2;
  phiZ = fZRemoved.phi();
  return fZRemoved.pt();
}

double sgn(double x) {
  return x > 0 ? 1 : -1;
}

double pPRel(const LorentzVector& pCand, const LorentzVector& pLep) {
  if (pLep.pt()<=0.) return 0.;
  LorentzVector jp4 = pLep;
  double dot = pCand.Vect().Dot( pLep.Vect() );
  return sgn(dot)*sqrt((dot*dot)/pLep.P2());
}

double boson_pT(bool isElEvt, int id1, int id2, ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fMet, double &u_para, double &u_perp, double &u_para_plus_qt) {
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fZ;
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fLep1;
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fLep2;
  if (isElEvt) {
    fLep1 = els_p4().at(id1);
    fLep2 = els_p4().at(id2);
  }
  else {
    fLep1 = mus_p4().at(id1);
    fLep2 = mus_p4().at(id2);
    //cout << "Mus pt: " << fLep1.pt() << endl;
    //cout << "Mus pt: " << fLep2.pt() << endl;
  }

  fZ += fLep1;
  fZ += fLep2;

  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fU = -fMet - fZ;
  //cout << fU.pt() << " " << fU.mag() << endl;


  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> qT = fZ;
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> uT = fU;
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> eT = fMet;
  qT.SetPz(0);
  uT.SetPz(0);
  eT.SetPz(0);

  u_para = pPRel(uT, qT);
  u_perp = sgn((qT.px()*uT.py()) - (uT.px()*qT.py()))*sqrt(pow(uT.pt(), 2) - pow(u_para, 2));
  u_para_plus_qt = qT.pt() + u_para; 

  //cout << "Boson pT: " << qT.pt() << " , u_parallel: " << u_para << " , u_perp: " << u_perp << endl;

  return qT.pt();
}

int find_index(vector<double> bins, double value) {
  for (int i = 0; i < bins.size() - 1; i++) {
    if (value >= bins[i] && value < bins[i+1])
      return i;
  }
  if (value > bins[bins.size() -1])
    return bins.size() - 1;
  else {
    cout << "Did not sort into a bin" << endl;
    return -1;
  }
}

double ZRemovedMETRaw(bool isElEvt, int id1, int id2, double &phiZ) {
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fZRemoved;
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fLep1;
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fLep2;
  for (int i = 0; i < cms3.pfcands_p4().size(); i++) {
    ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fourV = cms3.pfcands_p4().at(i);
    fZRemoved += fourV;
  }
  fZRemoved = -fZRemoved;  // want negative sum of pf candidates
  if (isElEvt) {
    fLep1 = els_p4().at(id1);
    fLep2 = els_p4().at(id2);
  }
  else {
    fLep1 = mus_p4().at(id1);
    fLep2 = mus_p4().at(id2);
  }
  fZRemoved += fLep1;
  fZRemoved += fLep2;
  phiZ = fZRemoved.phi();
  return fZRemoved.pt();
}

double mht(bool isElEvt, int id1, int id2) {
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fMHT;
  for (int i=0; i< pfjets_p4().size(); i++) {
    if ((pfjets_p4().at(i)).Pt() < 35)                  { continue; }
    if (abs((pfjets_p4().at(i)).eta()) > 2.4)           { continue; }
    LorentzVector pl1, pl2;
    if (isElEvt) {
      pl1 = els_p4().at(id1);
      pl2 = els_p4().at(id2);
    }
    else {
      pl1 = mus_p4().at(id1);
      pl2 = mus_p4().at(id2);
    }
    if (DeltaR(pfjets_p4().at(i),pl1) < 0.4)            { continue; }
    if (DeltaR(pfjets_p4().at(i),pl2) < 0.4)            { continue; }
    fMHT += cms3.pfjets_p4().at(i);
  }
  return fMHT.pt();
}

vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>>> correctedJets(TString currentFileName, string version, string version_mc,bool use_puppi=false) {

  std::vector<std::string> jetcorr_filenames_pfL1FastJetL2L3;
  std::string jetcorr_uncertainty_filename;


  FactorizedJetCorrector* jet_corrector(0);
  if(!(use_puppi))
  {
  if (currentFileName.Contains("2017B")) {
    jetcorr_filenames_pfL1FastJetL2L3.clear();
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017B_" + version + "_DATA_L1FastJet_AK4PFchs.txt"   );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017B_" + version + "_DATA_L2Relative_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017B_" + version + "_DATA_L3Absolute_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017B_" + version + "_DATA_L2L3Residual_AK4PFchs.txt");
    jetcorr_uncertainty_filename = "jetCorrections/Fall17_17Nov2017B_" + version + "_DATA_Uncertainty_AK4PFchs.txt";
  }else if (currentFileName.Contains("2017C")) {
    jetcorr_filenames_pfL1FastJetL2L3.clear();
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017C_" + version + "_DATA_L1FastJet_AK4PFchs.txt"   );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017C_" + version + "_DATA_L2Relative_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017C_" + version + "_DATA_L3Absolute_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017C_" + version + "_DATA_L2L3Residual_AK4PFchs.txt");
    jetcorr_uncertainty_filename = "jetCorrections/Fall17_17Nov2017C_" + version + "_DATA_Uncertainty_AK4PFchs.txt";
  }else if (currentFileName.Contains("2017D")) {
    jetcorr_filenames_pfL1FastJetL2L3.clear();
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + version + "_DATA_L1FastJet_AK4PFchs.txt"   );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + version + "_DATA_L2Relative_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + version + "_DATA_L3Absolute_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + version + "_DATA_L2L3Residual_AK4PFchs.txt");
    jetcorr_uncertainty_filename = "jetCorrections/Fall17_17Nov2017DE_" + version + "_DATA_Uncertainty_AK4PFchs.txt";
  }else if (currentFileName.Contains("2017E")) {
    jetcorr_filenames_pfL1FastJetL2L3.clear();
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + version + "_DATA_L1FastJet_AK4PFchs.txt"   );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + version + "_DATA_L2Relative_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + version + "_DATA_L3Absolute_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + version + "_DATA_L2L3Residual_AK4PFchs.txt");
    jetcorr_uncertainty_filename = "jetCorrections/Fall17_17Nov2017DE_" + version + "_DATA_Uncertainty_AK4PFchs.txt";
  }
  else if (currentFileName.Contains("2017F")) {
        jetcorr_filenames_pfL1FastJetL2L3.clear();
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017F_" + version + "_DATA_L1FastJet_AK4PFchs.txt"   );
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017F_" + version + "_DATA_L2Relative_AK4PFchs.txt"  );
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017F_" + version + "_DATA_L3Absolute_AK4PFchs.txt"  );
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017F_" + version + "_DATA_L2L3Residual_AK4PFchs.txt");
        jetcorr_uncertainty_filename = "jetCorrections/Fall17_17Nov2017F_" + version + "_DATA_Uncertainty_AK4PFchs.txt";
    }  
    else if (currentFileName.Contains("Fall17")) {
        jetcorr_filenames_pfL1FastJetL2L3.clear();
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017_" + version_mc + "_MC_L1FastJet_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017_" + version_mc + "_MC_L2Relative_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017_" + version_mc + "_MC_L3Absolute_AK4PFchs.txt");
        jetcorr_uncertainty_filename = "jetCorrections/Fall17_17Nov2017_" + version_mc + "_MC_Uncertainty_AK4PFchs.txt";
    }
    else if (currentFileName.Contains("Autumn18"))
    {
        jetcorr_filenames_pfL1FastJetL2L3.clear();
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_" + version_mc + "_MC_L1FastJet_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_" + version_mc + "_MC_L2Relative_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_" + version_mc + "_MC_L3Absolute_AK4PFchs.txt");
          jetcorr_uncertainty_filename = "jetCorrections/Autumn18_" + version_mc + "_MC_Uncertainty_AK4PFchs.txt";
 
    }
    else if(currentFileName.Contains("2018A"))
    {
        jetcorr_filenames_pfL1FastJetL2L3.clear();
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunA_" + version + "_DATA_L1FastJet_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunA_" + version + "_DATA_L2Relative_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunA_" + version + "_DATA_L3Absolute_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back("jetCorrections/Autumn18_RunA_" + version + "_DATA_L2L3Residual_AK4PFchs.txt");
        jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunA_" + version + "_DATA_Uncertainty_AK4PFchs.txt";
    }
    else if(currentFileName.Contains("2018B"))
    {
        jetcorr_filenames_pfL1FastJetL2L3.clear();
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunB_" + version + "_DATA_L1FastJet_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunB_" + version + "_DATA_L2Relative_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunB_" + version + "_DATA_L3Absolute_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back("jetCorrections/Autumn18_RunB_"+version+"_DATA_L2L3Residual_AK4PFchs.txt");
         jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunB_" + version + "_DATA_Uncertainty_AK4PFchs.txt";
     }
      else if(currentFileName.Contains("2018C"))
      {
          jetcorr_filenames_pfL1FastJetL2L3.clear();
          jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + version + "_DATA_L1FastJet_AK4PFchs.txt");
          jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + version + "_DATA_L2Relative_AK4PFchs.txt");
          jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + version + "_DATA_L3Absolute_AK4PFchs.txt");
          jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + version + "_DATA_L2L3Residual_AK4PFchs.txt"); 
          jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunC_" + version + "_DATA_Uncertainty_AK4PFchs.txt";
    }
    else if(currentFileName.Contains("2018D"))
    {
        jetcorr_filenames_pfL1FastJetL2L3.clear();
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + version + "_DATA_L1FastJet_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + version + "_DATA_L2Relative_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + version + "_DATA_L3Absolute_AK4PFchs.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + version + "_DATA_L2L3Residual_AK4PFchs.txt");
        jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunD_" + version + "_DATA_Uncertainty_AK4PFchs.txt";
    }
    else {
    cout << "Did not grab JECs" << endl; // should not happen
  }

  }
  else
  {
    if (currentFileName.Contains("Autumn18"))
    {
        jetcorr_filenames_pfL1FastJetL2L3.clear();
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_" + version_mc + "_MC_L1FastJet_AK4PFPuppi.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_" + version_mc + "_MC_L2Relative_AK4PFPuppi.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_" + version_mc + "_MC_L3Absolute_AK4PFPuppi.txt");
          jetcorr_uncertainty_filename = "jetCorrections/Autumn18_" + version_mc + "_MC_Uncertainty_AK4PFPuppi.txt";
 
    }
    else if(currentFileName.Contains("2018A"))
    {
        jetcorr_filenames_pfL1FastJetL2L3.clear();
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunA_" + version + "_DATA_L1FastJet_AK4PFPuppi.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunA_" + version + "_DATA_L2Relative_AK4PFPuppi.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunA_" + version + "_DATA_L3Absolute_AK4PFPuppi.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back("jetCorrections/Autumn18_RunA_" + version + "_DATA_L2L3Residual_AK4PFPuppi.txt");
        jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunA_" + version + "_DATA_Uncertainty_AK4PFPuppi.txt";
    }
    else if(currentFileName.Contains("2018B"))
    {
        jetcorr_filenames_pfL1FastJetL2L3.clear();
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunB_" + version + "_DATA_L1FastJet_AK4PFPuppi.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunB_" + version + "_DATA_L2Relative_AK4PFPuppi.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunB_" + version + "_DATA_L3Absolute_AK4PFPuppi.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back("jetCorrections/Autumn18_RunB_"+version+"_DATA_L2L3Residual_AK4PFPuppi.txt");
         jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunB_" + version + "_DATA_Uncertainty_AK4PFPuppi.txt";
     }
      else if(currentFileName.Contains("2018C"))
      {
          jetcorr_filenames_pfL1FastJetL2L3.clear();
          jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + version + "_DATA_L1FastJet_AK4PFPuppi.txt");
          jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + version + "_DATA_L2Relative_AK4PFPuppi.txt");
          jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + version + "_DATA_L3Absolute_AK4PFPuppi.txt");
          jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + version + "_DATA_L2L3Residual_AK4PFPuppi.txt"); 
          jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunC_" + version + "_DATA_Uncertainty_AK4PFPuppi.txt";
    }
    else if(currentFileName.Contains("2018D"))
    {
        jetcorr_filenames_pfL1FastJetL2L3.clear();
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + version + "_DATA_L1FastJet_AK4PFPuppi.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + version + "_DATA_L2Relative_AK4PFPuppi.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + version + "_DATA_L3Absolute_AK4PFPuppi.txt");
        jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + version + "_DATA_L2L3Residual_AK4PFPuppi.txt");
        jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunD_" + version + "_DATA_Uncertainty_AK4PFPuppi.txt";
        }
        else {
        cout << "Did not grab JECs" << endl; // should not happen
    } 
  }
  
  
  jet_corrector = makeJetCorrector(jetcorr_filenames_pfL1FastJetL2L3);

  vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>>> v = getCorrectedJets( jet_corrector );
  delete jet_corrector;
  return v;
}


ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> t1CMET(TString currentFileName, int use_cleaned_met, string JEC_version_data, string JEC_version_mc, int unc) {
  std::pair<float, float> pT1CMET;

  std::vector<std::string> jetcorr_filenames_pfL1FastJetL2L3;
  std::string jetcorr_uncertainty_filename;


  FactorizedJetCorrector* jet_corrector(0);
  JetCorrectionUncertainty jet_uncertainty;

   if (currentFileName.Contains("2017B")) {
    jetcorr_filenames_pfL1FastJetL2L3.clear();
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017B_" + JEC_version_data + "_DATA_L1FastJet_AK4PFchs.txt"   );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017B_" + JEC_version_data + "_DATA_L2Relative_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017B_" + JEC_version_data + "_DATA_L3Absolute_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017B_" + JEC_version_data + "_DATA_L2L3Residual_AK4PFchs.txt");
    jetcorr_uncertainty_filename = "jetCorrections/Fall17_17Nov2017B_" + JEC_version_data + "_DATA_Uncertainty_AK4PFchs.txt";
  }else if (currentFileName.Contains("2017C")) {
    jetcorr_filenames_pfL1FastJetL2L3.clear();
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017C_" + JEC_version_data + "_DATA_L1FastJet_AK4PFchs.txt"   );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017C_" + JEC_version_data + "_DATA_L2Relative_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017C_" + JEC_version_data + "_DATA_L3Absolute_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017C_" + JEC_version_data + "_DATA_L2L3Residual_AK4PFchs.txt");
    jetcorr_uncertainty_filename = "jetCorrections/Fall17_17Nov2017C_" + JEC_version_data + "_DATA_Uncertainty_AK4PFchs.txt";
  }else if (currentFileName.Contains("2017D")) {
    jetcorr_filenames_pfL1FastJetL2L3.clear();
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + JEC_version_data + "_DATA_L1FastJet_AK4PFchs.txt"   );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + JEC_version_data + "_DATA_L2Relative_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + JEC_version_data + "_DATA_L3Absolute_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + JEC_version_data + "_DATA_L2L3Residual_AK4PFchs.txt");
    jetcorr_uncertainty_filename = "jetCorrections/Fall17_17Nov2017DE_" + JEC_version_data + "_DATA_Uncertainty_AK4PFchs.txt";
  }else if (currentFileName.Contains("2017E")) {
    jetcorr_filenames_pfL1FastJetL2L3.clear();
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + JEC_version_data + "_DATA_L1FastJet_AK4PFchs.txt"   );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + JEC_version_data + "_DATA_L2Relative_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + JEC_version_data + "_DATA_L3Absolute_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017DE_" + JEC_version_data + "_DATA_L2L3Residual_AK4PFchs.txt");
    jetcorr_uncertainty_filename = "jetCorrections/Fall17_17Nov2017DE_" + JEC_version_data + "_DATA_Uncertainty_AK4PFchs.txt";
  }else if (currentFileName.Contains("2017F")) {
    jetcorr_filenames_pfL1FastJetL2L3.clear();
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017F_" + JEC_version_data + "_DATA_L1FastJet_AK4PFchs.txt"   );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017F_" + JEC_version_data + "_DATA_L2Relative_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017F_" + JEC_version_data + "_DATA_L3Absolute_AK4PFchs.txt"  );
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017F_" + JEC_version_data + "_DATA_L2L3Residual_AK4PFchs.txt");
    jetcorr_uncertainty_filename = "jetCorrections/Fall17_17Nov2017F_" + JEC_version_data + "_DATA_Uncertainty_AK4PFchs.txt";
  }  else if (currentFileName.Contains("Fall17")) {
    jetcorr_filenames_pfL1FastJetL2L3.clear();
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017_" + JEC_version_mc + "_MC_L1FastJet_AK4PFchs.txt");
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017_" + JEC_version_mc + "_MC_L2Relative_AK4PFchs.txt");
    jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Fall17_17Nov2017_" + JEC_version_mc + "_MC_L3Absolute_AK4PFchs.txt");
    jetcorr_uncertainty_filename = "jetCorrections/Fall17_17Nov2017_" + JEC_version_mc + "_MC_Uncertainty_AK4PFchs.txt";
   }
  else if (currentFileName.Contains("Autumn18"))
  {
      jetcorr_filenames_pfL1FastJetL2L3.clear();
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_" + JEC_version_mc + "_MC_L1FastJet_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_" + JEC_version_mc + "_MC_L2Relative_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_" + JEC_version_mc + "_MC_L3Absolute_AK4PFchs.txt");
      jetcorr_uncertainty_filename = "jetCorrections/Autumn18_" + JEC_version_mc + "_MC_Uncertainty_AK4PFchs.txt";
 
  }
  else if(currentFileName.Contains("2018A"))
  {
      jetcorr_filenames_pfL1FastJetL2L3.clear();
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunA_" + JEC_version_data + "_DATA_L1FastJet_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunA_" + JEC_version_data + "_DATA_L2Relative_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunA_" + JEC_version_data + "_DATA_L3Absolute_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back("jetCorrections/Autumn18_RunA_"+JEC_version_data+"_DATA_L2L3Residual_AK4PFchs.txt");
      jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunA_" + JEC_version_data + "_DATA_Uncertainty_AK4PFchs.txt";
  }
  else if(currentFileName.Contains("2018B"))
  {
      jetcorr_filenames_pfL1FastJetL2L3.clear();
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunB_" + JEC_version_data + "_DATA_L1FastJet_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunB_" + JEC_version_data + "_DATA_L2Relative_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunB_" + JEC_version_data + "_DATA_L3Absolute_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back("jetCorrections/Autumn18_RunB_"+JEC_version_data+"_DATA_L2L3Residual_AK4PFchs.txt");
      jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunB_" + JEC_version_data + "_DATA_Uncertainty_AK4PFchs.txt";
  }
  else if(currentFileName.Contains("2018C"))
  {
      jetcorr_filenames_pfL1FastJetL2L3.clear();
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + JEC_version_data + "_DATA_L1FastJet_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + JEC_version_data + "_DATA_L2Relative_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + JEC_version_data + "_DATA_L3Absolute_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back("jetCorrections/Autumn18_RunC_"+JEC_version_data+"_DATA_L2L3Residual_AK4PFchs.txt");
      jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunC_" + JEC_version_data + "_DATA_Uncertainty_AK4PFchs.txt";
  }
  else if(currentFileName.Contains("2018D"))
  {
      jetcorr_filenames_pfL1FastJetL2L3.clear();
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + JEC_version_data + "_DATA_L1FastJet_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + JEC_version_data + "_DATA_L2Relative_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + JEC_version_data + "_DATA_L3Absolute_AK4PFchs.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back("jetCorrections/Autumn18_RunD_"+JEC_version_data+"_DATA_L2L3Residual_AK4PFchs.txt");
      jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunD_" + JEC_version_data + "_DATA_Uncertainty_AK4PFchs.txt";
  }


  jet_corrector = makeJetCorrector(jetcorr_filenames_pfL1FastJetL2L3);
  jet_uncertainty.setParameters(jetcorr_uncertainty_filename);

  if (unc == 0)
    pT1CMET = getT1CHSMET_fromMINIAOD(jet_corrector, NULL, 0, 0, use_cleaned_met);
  else
    pT1CMET = getT1CHSMET_fromMINIAOD(jet_corrector, &jet_uncertainty, unc == 1, 0, use_cleaned_met);

  float metX = pT1CMET.first * cos(pT1CMET.second);
  float metY = pT1CMET.first * sin(pT1CMET.second);
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fT1CMET(metX, metY, 0, pT1CMET.first);
  delete jet_corrector;
  return fT1CMET;
}

ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> t1CPuppiMET(TString currentFileName, int use_cleaned_met, string JEC_version_data, string JEC_version_mc, int unc) {
  std::pair<float, float> pT1CMET;

  std::vector<std::string> jetcorr_filenames_pfL1FastJetL2L3;
  std::string jetcorr_uncertainty_filename;


  FactorizedJetCorrector* jet_corrector(0);
  JetCorrectionUncertainty jet_uncertainty;


 if (currentFileName.Contains("Autumn18"))
  {
      jetcorr_filenames_pfL1FastJetL2L3.clear();
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_" + JEC_version_mc + "_MC_L1FastJet_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_" + JEC_version_mc + "_MC_L2Relative_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_" + JEC_version_mc + "_MC_L3Absolute_AK4PFPuppi.txt");
      jetcorr_uncertainty_filename = "jetCorrections/Autumn18_" + JEC_version_mc + "_MC_Uncertainty_AK4PFPuppi.txt";
 
  }
  else if(currentFileName.Contains("2018A"))
  {
      jetcorr_filenames_pfL1FastJetL2L3.clear();
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunA_" + JEC_version_data + "_DATA_L1FastJet_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunA_" + JEC_version_data + "_DATA_L2Relative_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunA_" + JEC_version_data + "_DATA_L3Absolute_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back("jetCorrections/Autumn18_RunA_"+JEC_version_data+"_DATA_L2L3Residual_AK4PFPuppi.txt");
      jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunA_" + JEC_version_data + "_DATA_Uncertainty_AK4PFPuppi.txt";
  }
  else if(currentFileName.Contains("2018B"))
  {
      jetcorr_filenames_pfL1FastJetL2L3.clear();
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunB_" + JEC_version_data + "_DATA_L1FastJet_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunB_" + JEC_version_data + "_DATA_L2Relative_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunB_" + JEC_version_data + "_DATA_L3Absolute_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back("jetCorrections/Autumn18_RunB_"+JEC_version_data+"_DATA_L2L3Residual_AK4PFPuppi.txt");
      jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunB_" + JEC_version_data + "_DATA_Uncertainty_AK4PFPuppi.txt";
  }
  else if(currentFileName.Contains("2018C"))
  {
      jetcorr_filenames_pfL1FastJetL2L3.clear();
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + JEC_version_data + "_DATA_L1FastJet_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + JEC_version_data + "_DATA_L2Relative_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunC_" + JEC_version_data + "_DATA_L3Absolute_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back("jetCorrections/Autumn18_RunC_"+JEC_version_data+"_DATA_L2L3Residual_AK4PFPuppi.txt");
      jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunC_" + JEC_version_data + "_DATA_Uncertainty_AK4PFPuppi.txt";
  }
  else if(currentFileName.Contains("2018D"))
  {
      jetcorr_filenames_pfL1FastJetL2L3.clear();
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + JEC_version_data + "_DATA_L1FastJet_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + JEC_version_data + "_DATA_L2Relative_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/Autumn18_RunD_" + JEC_version_data + "_DATA_L3Absolute_AK4PFPuppi.txt");
      jetcorr_filenames_pfL1FastJetL2L3.push_back("jetCorrections/Autumn18_RunD_"+JEC_version_data+"_DATA_L2L3Residual_AK4PFPuppi.txt");
      jetcorr_uncertainty_filename = "jetCorrections/Autumn18_RunD_" + JEC_version_data + "_DATA_Uncertainty_AK4PFPuppi.txt";
  }


  jet_corrector = makeJetCorrector(jetcorr_filenames_pfL1FastJetL2L3);
  jet_uncertainty.setParameters(jetcorr_uncertainty_filename);

  if (unc == 0)
    pT1CMET = getT1PUPPIMET_fromMINIAOD(jet_corrector, NULL, 0);
  else
    pT1CMET = getT1PUPPIMET_fromMINIAOD(jet_corrector, &jet_uncertainty, unc == 1);

  float metX = pT1CMET.first * cos(pT1CMET.second);
  float metY = pT1CMET.first * sin(pT1CMET.second);
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> fT1CMET(metX, metY, 0, pT1CMET.first);
  delete jet_corrector;
  return fT1CMET;
}

TH1D* create_histogram(TString name, int nBins, double x_low, double x_high) {
  TH1D* h = new TH1D(name, "", nBins, x_low, x_high);
  h->Sumw2();
  return h;
}

vector<TH1D*> create_histogram_vector(TString name, int nBins, double x_low, double x_high, int nHists) {
  vector<TH1D*> vHists;
  for (int i = 0; i < nHists; i++) {
    vHists.push_back(create_histogram(name+to_string(i), nBins, x_low, x_high));
  }
  return vHists;
}

TProfile* create_profile_histogram(TString name, int nBins, double x_low, double x_high)
{
    TProfile *h = new TProfile(name, "", nBins, x_low, x_high);
    h->Sumw2();
    return h;
}

vector<TProfile*> create_profile_histogram_vector(TString name, int nBins, double x_los, double x_high, int nHists)
{
    vector<TProfile*> vHists;
    for(int i=0;i<nHists;i++)
    {
        vHists.push_back(create_profile_histogram(name_to_string(i),nBins,x_low,x_high));
    }
    return vHists;
}


vector<TH2D*> create_2Dhistogram_vector(TString name, int nBinsX, double x_low, double x_high, int nBinsY, double y_low, double y_high, int nHists) {
  vector<TH2D*> vHists;
  for (int i = 0; i < nHists; i++) {
    vHists.push_back(new TH2D(name+to_string(i), "", nBinsX, x_low, x_high, nBinsY, y_low, y_high));
    vHists[i]->Sumw2();
  }
  return vHists;
}

void fill_histograms(vector<TH1D*> vHists, double value, vector<double> vWeights) {
  for (int i = 0; i < vHists.size(); i++) {
    vHists[i]->Fill(value, vWeights[i]);
  }
}

void fill_histograms2D(vector<TH2D*> vHists, double x_value, double y_value, vector<double> vWeights) {
  for (int i = 0; i < vHists.size(); i++) {
    vHists[i]->Fill(x_value, y_value, vWeights[i]);
  }
}


void fill_profile_histograms(vector<TProfile*> vHists,double x_value,double y_value, vector<double> vWeights)
{
    for(int i=0;i<vHists.size();i++)
    {
        vHists[i]->Fill(x_value,y_value,vWeights[i]);
    }
}

vector<vector<vector<TH1D*>>> create_met_histograms(int nEtaRegions, int nCandCats) {
  vector<vector<vector<TH1D*>>> vhMET;
  for (int i=0; i<nEtaRegions; i++) { 
    vector<vector<TH1D*>> vTemp2;
    for (int j=0; j<nCandCats; j++) {
      TString etaIdx = to_string(i);
      TString candIdx = to_string(j);
      TString base = "h_eta" + etaIdx + "_cand" + candIdx;
      TString name1 = base + "MET";
      TString name2 = base + "SumET";
      TString name3 = base + "METPhi";
      vector<TH1D*> vTemp1;
      vTemp1.push_back(new TH1D(name1,"",80,0,400));
      vTemp1.push_back(new TH1D(name2,"",200,0,2000));
      vTemp1.push_back(new TH1D(name3,"",40,-3.142,3.142));
      for (int k=0; k<vTemp1.size(); k++)
        vTemp1[k]->Sumw2();
      vTemp2.push_back(vTemp1);
    }
    vhMET.push_back(vTemp2);
  }
  return vhMET;
}

vector<vector<vector<TH1D*>>> create_met_histograms(int nEtaRegions, int nCandCats, TString idx) {
  vector<vector<vector<TH1D*>>> vhMET;
  for (int i=0; i<nEtaRegions; i++) {
    vector<vector<TH1D*>> vTemp2;
    for (int j=0; j<nCandCats; j++) {
      TString etaIdx = to_string(i);
      TString candIdx = to_string(j);
      TString base = "h_eta" + etaIdx + "_cand" + candIdx;
      TString name1 = base + "MET";
      TString name2 = base + "SumET";
      TString name3 = base + "METPhi";
      vector<TH1D*> vTemp1;
      vTemp1.push_back(new TH1D(name1+idx,"",80,0,400));
      vTemp1.push_back(new TH1D(name2+idx,"",200,0,2000));
      vTemp1.push_back(new TH1D(name3+idx,"",40,-3.142,3.142));
      for (int k=0; k<vTemp1.size(); k++)
        vTemp1[k]->Sumw2();
      vTemp2.push_back(vTemp1);
    }
    vhMET.push_back(vTemp2);
  }
  return vhMET;
}

vector<vector<vector<vector<TH1D*>>>> create_met_histograms_vector(int nEtaRegions, int nCandCats, int nHists) {
  vector<vector<vector<vector<TH1D*>>>> vhMET;
  for (int i=0; i<nEtaRegions; i++) {
    vector<vector<vector<TH1D*>>> vTemp2;
    for (int j=0; j<nCandCats; j++) {
      TString etaIdx = to_string(i);
      TString candIdx = to_string(j);
      TString base = "h_eta" + etaIdx + "_cand" + candIdx;
      TString name1 = base + "MET";
      TString name2 = base + "SumET";
      TString name3 = base + "METPhi";
      vector<vector<TH1D*>> vTemp1;
      vTemp1.push_back(create_histogram_vector(name1, 40, 0, 400, nHists));
      vTemp1.push_back(create_histogram_vector(name2, 200, 0, 2000, nHists));
      vTemp1.push_back(create_histogram_vector(name3, 40, -3.142, 3.142, nHists));
      vTemp2.push_back(vTemp1);
    }
    vhMET.push_back(vTemp2);
  }
  return vhMET;
}

float sgn(float x) {
  if (x < 0)
    return -1;
  else if (x > 0)
    return 1;
  else
    return 0;

}

enum TheRunEra{y2016B,y2016C,y2016D,y2016E,y2016F,y2016G,y2016H,y2017B,y2017C,y2017D,y2017E,y2017F,y2018A,y2018B,y2018C,y2018D,y2016MC,y2017MC,y2018MC};

ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> METXYCorr(TString currentFileName,double uncormet,double uncormet_phi, int npv){

 ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>>  TheXYCorr_Met_MetPhi(uncormet,uncormet*cos(uncormet_phi),uncormet*sin(uncormet_phi),0);
  
  if(npv>100) npv=100;
  int runera =-1;
  bool usemetv2 =false;
  bool isMC = false;  

  
  if(currentFileName.Contains("2018A")) {isMC = false; runera= y2018A;}
  else if(currentFileName.Contains("2018B")) {isMC = false; runera = y2018B;}
  else if(currentFileName.Contains("2018C")) {isMC = false; runera = y2018C;}
  else if(currentFileName.Contains("2018D")) {isMC = false; runera = y2018D;}
  else if(currentFileName.Contains("Autumn18")){isMC = true; runera = y2018MC;}
  else {
    //Couldn't find data/MC era => no correction applied
    return TheXYCorr_Met_MetPhi;
  }

  
  double METxcorr(0.),METycorr(0.);

  if(!usemetv2){//Current recommendation for 2016 and 2018
    if(runera==y2016B) METxcorr = -(-0.0478335*npv -0.108032);
    if(runera==y2016B) METycorr = -(0.125148*npv +0.355672);
    if(runera==y2016C) METxcorr = -(-0.0916985*npv +0.393247);
    if(runera==y2016C) METycorr = -(0.151445*npv +0.114491);
    if(runera==y2016D) METxcorr = -(-0.0581169*npv +0.567316);
    if(runera==y2016D) METycorr = -(0.147549*npv +0.403088);
    if(runera==y2016E) METxcorr = -(-0.065622*npv +0.536856);
    if(runera==y2016E) METycorr = -(0.188532*npv +0.495346);
    if(runera==y2016F) METxcorr = -(-0.0313322*npv +0.39866);
    if(runera==y2016F) METycorr = -(0.16081*npv +0.960177);
    if(runera==y2016G) METxcorr = -(0.040803*npv -0.290384);
    if(runera==y2016G) METycorr = -(0.0961935*npv +0.666096);
    if(runera==y2016H) METxcorr = -(0.0330868*npv -0.209534);
    if(runera==y2016H) METycorr = -(0.141513*npv +0.816732);
    if(runera==y2017B) METxcorr = -(-0.259456*npv +1.95372);
    if(runera==y2017B) METycorr = -(0.353928*npv -2.46685);
    if(runera==y2017C) METxcorr = -(-0.232763*npv +1.08318);
    if(runera==y2017C) METycorr = -(0.257719*npv -1.1745);
    if(runera==y2017D) METxcorr = -(-0.238067*npv +1.80541);
    if(runera==y2017D) METycorr = -(0.235989*npv -1.44354);
    if(runera==y2017E) METxcorr = -(-0.212352*npv +1.851);
    if(runera==y2017E) METycorr = -(0.157759*npv -0.478139);
    if(runera==y2017F) METxcorr = -(-0.232733*npv +2.24134);
    if(runera==y2017F) METycorr = -(0.213341*npv +0.684588);
    if(runera==y2018A) METxcorr = -(0.362865*npv -1.94505);
    if(runera==y2018A) METycorr = -(0.0709085*npv -0.307365);
    if(runera==y2018B) METxcorr = -(0.492083*npv -2.93552);
    if(runera==y2018B) METycorr = -(0.17874*npv -0.786844);
    if(runera==y2018C) METxcorr = -(0.521349*npv -1.44544);
    if(runera==y2018C) METycorr = -(0.118956*npv -1.96434);
    if(runera==y2018D) METxcorr = -(0.531151*npv -1.37568);
    if(runera==y2018D) METycorr = -(0.0884639*npv -1.57089);
    if(runera==y2016MC) METxcorr = -(-0.195191*npv -0.170948);
    if(runera==y2016MC) METycorr = -(-0.0311891*npv +0.787627);
    if(runera==y2017MC) METxcorr = -(-0.217714*npv +0.493361);
    if(runera==y2017MC) METycorr = -(0.177058*npv -0.336648);
    if(runera==y2018MC) METxcorr = -(0.296713*npv -0.141506);
    if(runera==y2018MC) METycorr = -(0.115685*npv +0.0128193);
  }
  else {//these are the corrections for v2 MET recipe (currently recommended for 2017)
    if(runera==y2016B) METxcorr = -(-0.0374977*npv +0.00488262);
    if(runera==y2016B) METycorr = -(0.107373*npv +-0.00732239);
    if(runera==y2016C) METxcorr = -(-0.0832562*npv +0.550742);
    if(runera==y2016C) METycorr = -(0.142469*npv +-0.153718);
    if(runera==y2016D) METxcorr = -(-0.0400931*npv +0.753734);
    if(runera==y2016D) METycorr = -(0.127154*npv +0.0175228);
    if(runera==y2016E) METxcorr = -(-0.0409231*npv +0.755128);
    if(runera==y2016E) METycorr = -(0.168407*npv +0.126755);
    if(runera==y2016F) METxcorr = -(-0.0161259*npv +0.516919);
    if(runera==y2016F) METycorr = -(0.141176*npv +0.544062);
    if(runera==y2016G) METxcorr = -(0.0583851*npv +-0.0987447);
    if(runera==y2016G) METycorr = -(0.0641427*npv +0.319112);
    if(runera==y2016H) METxcorr = -(0.0706267*npv +-0.13118);
    if(runera==y2016H) METycorr = -(0.127481*npv +0.370786);
    if(runera==y2017B) METxcorr = -(-0.19563*npv +1.51859);
    if(runera==y2017B) METycorr = -(0.306987*npv +-1.84713);
    if(runera==y2017C) METxcorr = -(-0.161661*npv +0.589933);
    if(runera==y2017C) METycorr = -(0.233569*npv +-0.995546);
    if(runera==y2017D) METxcorr = -(-0.180911*npv +1.23553);
    if(runera==y2017D) METycorr = -(0.240155*npv +-1.27449);
    if(runera==y2017E) METxcorr = -(-0.149494*npv +0.901305);
    if(runera==y2017E) METycorr = -(0.178212*npv +-0.535537);
    if(runera==y2017F) METxcorr = -(-0.165154*npv +1.02018);
    if(runera==y2017F) METycorr = -(0.253794*npv +0.75776);
    if(runera==y2018A) METxcorr = -(0.362642*npv +-1.55094);
    if(runera==y2018A) METycorr = -(0.0737842*npv +-0.677209);
    if(runera==y2018B) METxcorr = -(0.485614*npv +-2.45706);
    if(runera==y2018B) METycorr = -(0.181619*npv +-1.00636);
    if(runera==y2018C) METxcorr = -(0.503638*npv +-1.01281);
    if(runera==y2018C) METycorr = -(0.147811*npv +-1.48941);
    if(runera==y2018D) METxcorr = -(0.520265*npv +-1.20322);
    if(runera==y2018D) METycorr = -(0.143919*npv +-0.979328);
    if(runera==y2016MC) METxcorr = -(-0.159469*npv +-0.407022);
    if(runera==y2016MC) METycorr = -(-0.0405812*npv +0.570415);
    if(runera==y2017MC) METxcorr = -(-0.182569*npv +0.276542);
    if(runera==y2017MC) METycorr = -(0.155652*npv +-0.417633);
    if(runera==y2018MC) METxcorr = -(0.299448*npv +-0.13866);
    if(runera==y2018MC) METycorr = -(0.118785*npv +0.0889588);
  }

  double CorrectedMET_x = uncormet *cos( uncormet_phi)+METxcorr;
  double CorrectedMET_y = uncormet *sin( uncormet_phi)+METycorr;

  double CorrectedMET = sqrt(CorrectedMET_x*CorrectedMET_x+CorrectedMET_y*CorrectedMET_y);
  double CorrectedMETPhi;
  if(CorrectedMET_x==0 && CorrectedMET_y>0) CorrectedMETPhi = TMath::Pi();
  else if(CorrectedMET_x==0 && CorrectedMET_y<0 )CorrectedMETPhi = -TMath::Pi();
  else if(CorrectedMET_x >0) CorrectedMETPhi = TMath::ATan(CorrectedMET_y/CorrectedMET_x);
  else if(CorrectedMET_x <0&& CorrectedMET_y>0) CorrectedMETPhi = TMath::ATan(CorrectedMET_y/CorrectedMET_x) + TMath::Pi();
  else if(CorrectedMET_x <0&& CorrectedMET_y<0) CorrectedMETPhi = TMath::ATan(CorrectedMET_y/CorrectedMET_x) - TMath::Pi();
  else CorrectedMETPhi =0;

  //TheXYCorr_Met_MetPhi.first= CorrectedMET;
  //TheXYCorr_Met_MetPhi.second= CorrectedMETPhi;
//  return TheXYCorr_Met_MetPhi;
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> XYCorrMET(CorrectedMET,CorrectedMET_x,CorrectedMET_y,0);
  return XYCorrMET;

}
