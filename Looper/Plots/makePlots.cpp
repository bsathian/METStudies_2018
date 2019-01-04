#include "PlotHelper.h"

const int nEtaRegions = 6;
const int nCandCats = 4;

const double met_bins[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 195, 210, 230, 255, 285, 335, 400};
const int n_met_bins = (sizeof(met_bins) / sizeof(met_bins[0])) - 1;

vector<double> get_normalizations(vector<TFile*> vFiles, double lumi, int histIdx) {
  TString hist_name = "hNVtx";
  TH1D* hData = (TH1D*)vFiles[0]->Get(hist_name + "0");
  TH1D* hDY = (TH1D*)vFiles[1]->Get(hist_name + to_string(histIdx));
  TH1D* hDY_RelVal = (TH1D*)vFiles[2]->Get(hist_name + to_string(histIdx));

  vector<double> scales = {hData->Integral() / (hDY->Integral() * lumi), hData->Integral() / (hDY_RelVal->Integral() * lumi)};
  delete hData;
  delete hDY;
  delete hDY_RelVal;
  return scales;
}

void get_normalization(vector<TFile*> vFiles, double lumi, int histIdx) {
  TString hist_name = "hNVtx";
  TH1D* hData = (TH1D*)vFiles[0]->Get(hist_name + "0");
  TH1D* hDY = (TH1D*)vFiles[1]->Get(hist_name + to_string(histIdx));

  cout << "Total yield for data: " << hData->Integral() << endl;
  cout << "Total yield for MC: "   << hDY->Integral() * lumi << endl;
  cout << "Data/MC Ratio: "        << hData->Integral() / (hDY->Integral() * lumi) << endl;
  
  delete hData;
  delete hDY;
}

void make_plot(TCanvas* c1, int histIdx, vector<TFile*> vFiles, string output_name, TString hist_name, TString x_label, double lumi, vector<double> scale, vector<TString> vInfo, int idx) {
  TString name = output_name;
  TH1D* hData = (TH1D*)vFiles[0]->Get(hist_name + "0");
  TH1D* hDY = (TH1D*)vFiles[1]->Get(hist_name + to_string(histIdx));
  TH1D* hDY_RelVal = (TH1D*)vFiles[2]->Get(hist_name + to_string(histIdx));
  vector<TH1D*> hMC = {hDY, hDY_RelVal};
  for (int i = 0; i < hMC.size(); i++) {
    hMC[i]->Scale(lumi);
    hMC[i]->Scale(scale[i]);
  }
  Comparison* c = new Comparison(c1, hMC, hData);
  c->set_filename(output_name);
  c->set_rat_label("#frac{MC}{Data}");
  c->set_legend_labels({"Fall18 DY", "Autumn18 DY (RelVal)", "2018 Data"});
  c->set_x_label(x_label);
  c->set_y_label("Events");
  c->set_data_drawOpt("E");
  c->set_lumi(lumi);
  c->set_scale(1);
  if (histIdx == 0) c->give_info("Run 2018A");
  if (histIdx == 1) c->give_info("Run 2018B");
  if (histIdx == 2) c->give_info("Run 2018C");
  if (histIdx == 3) c->give_info("Run 2018D");
  if (hist_name.Contains("Phi")) {
    c->set_no_log();
    c->set_y_lim_range({0, hData->Integral() / 15});
  }
  c->set_multiple_comparisons();
  for (int i = 0; i < vInfo.size(); i++)
    c->give_info(vInfo[i]);
  c->plot(idx);
  delete hData;
  for (int i = 0; i < hMC.size(); i++)
    delete hMC[i];
  delete c;
}

void make_resolution_plots(TCanvas* c1, int histIdx, vector<TFile*> vFiles, string output_name, vector<TString> hist_names, double lumi, double scale, vector<TString> vInfo, vector<TString> vLabels, int idx, bool compare_to_data = false, bool do_jec_unc = false) {
  c1->cd();
  TPad* mainPad = new TPad("p_main", "p_main", 0.0, 0.3, 1.0, 1.0);
  mainPad->SetBottomMargin(0.02);
  mainPad->SetRightMargin(0.07);
  mainPad->SetTopMargin(0.08);
  mainPad->SetLeftMargin(0.12);
  mainPad->Draw();
  mainPad->cd();

  double resolution_bins[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 220, 240, 260, 280, 300, 325, 350, 375, 400, 450, 500};
  int n_resolution_bins = (sizeof(resolution_bins)/sizeof(resolution_bins[0]))-1;

  vector<TH1D*> vHData_RawResponse;
  vector<TH1D*> vHMC_RawResponse;
  vector<TH1D*> vHData_RawResponseEE;
  vector<TH1D*> vHMC_RawResponseEE;
  vector<TH1D*> vHData_RawResponseMM;
  vector<TH1D*> vHMC_RawResponseMM;

  vector<TH1D*> vHData_response;
  vector<TH1D*> vHData_responseUp;
  vector<TH1D*> vHData_responseDown; 
  vector<TH1D*> vHMC_response;
  vector<TH1D*> vHData_responseEE;
  vector<TH1D*> vHMC_responseEE;
  vector<TH1D*> vHData_responseMM;
  vector<TH1D*> vHMC_responseMM;

  vector<TH1D*> vHData_responseVeryLowPU;
  vector<TH1D*> vHMC_responseVeryLowPU;
  vector<TH1D*> vHData_responseLowPU;
  vector<TH1D*> vHMC_responseLowPU;
  vector<TH1D*> vHData_responseMedPU;
  vector<TH1D*> vHMC_responseMedPU;
  vector<TH1D*> vHData_responseHighPU;
  vector<TH1D*> vHMC_responseHighPU;

  vector<TH1D*> vHData_responseLeadJetLowEta;
  vector<TH1D*> vHMC_responseLeadJetLowEta;
  vector<TH1D*> vHData_responseLeadJetMedEta;
  vector<TH1D*> vHMC_responseLeadJetMedEta;
  vector<TH1D*> vHData_responseLeadJetHighEta;
  vector<TH1D*> vHMC_responseLeadJetHighEta;

  vector<TH1D*> vHData_responseLeadJetLowEta_30GeVJet;
  vector<TH1D*> vHMC_responseLeadJetLowEta_30GeVJet;
  vector<TH1D*> vHData_responseLeadJetMedEta_30GeVJet;
  vector<TH1D*> vHMC_responseLeadJetMedEta_30GeVJet;
  vector<TH1D*> vHData_responseLeadJetHighEta_30GeVJet;
  vector<TH1D*> vHMC_responseLeadJetHighEta_30GeVJet;

  vector<TH1D*> vHData_responseLeadJetLowEta_100GeVJet;
  vector<TH1D*> vHMC_responseLeadJetLowEta_100GeVJet;
  vector<TH1D*> vHData_responseLeadJetMedEta_100GeVJet;
  vector<TH1D*> vHMC_responseLeadJetMedEta_100GeVJet;
  vector<TH1D*> vHData_responseLeadJetHighEta_100GeVJet;
  vector<TH1D*> vHMC_responseLeadJetHighEta_100GeVJet; 

  vector<TH1D*> vHData_res_para;
  vector<TH1D*> vHMC_res_para;
  vector<TH1D*> vHData_res_perp;
  vector<TH1D*> vHMC_res_perp;

  for (int i = 0; i < hist_names.size(); i++) {
    vHData_RawResponse.push_back(new TH1D(("hData_RawResponse" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_RawResponse.push_back(new TH1D(("hMC_RawResponse" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_RawResponseEE.push_back(new TH1D(("hData_RawResponseEE" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_RawResponseEE.push_back(new TH1D(("hMC_RawResponseEE" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_RawResponseMM.push_back(new TH1D(("hData_RawResponseMM" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_RawResponseMM.push_back(new TH1D(("hMC_RawResponseMM" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));

    vHData_response.push_back(new TH1D(("hData_response" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_responseUp.push_back(new TH1D(("hData_response" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_responseDown.push_back(new TH1D(("hData_response" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));   
    vHMC_response.push_back(new TH1D(("hMC_response" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_responseEE.push_back(new TH1D(("hData_responseEE" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseEE.push_back(new TH1D(("hMC_responseEE" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_responseMM.push_back(new TH1D(("hData_responseMM" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseMM.push_back(new TH1D(("hMC_responseMM" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));

    vHData_responseVeryLowPU.push_back(new TH1D(("hData_responseVeryLowPU" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseVeryLowPU.push_back(new TH1D(("hMC_responseVeryLowPU" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_responseLowPU.push_back(new TH1D(("hData_responseLowPU" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseLowPU.push_back(new TH1D(("hMC_responseLowPU" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_responseMedPU.push_back(new TH1D(("hData_responseMedPU" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseMedPU.push_back(new TH1D(("hMC_responseMedPU" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_responseHighPU.push_back(new TH1D(("hData_responseHighPU" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseHighPU.push_back(new TH1D(("hMC_responseHighPU" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_responseLeadJetLowEta.push_back(new TH1D(("hData_responseLeadJetLowEta" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseLeadJetLowEta.push_back(new TH1D(("hMC_responseLeadJetLowEta" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_responseLeadJetMedEta.push_back(new TH1D(("hData_responseLeadJetMedEta" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseLeadJetMedEta.push_back(new TH1D(("hMC_responseLeadJetMedEta" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));

    vHData_responseLeadJetHighEta.push_back(new TH1D(("hData_responseLeadJetHighEta" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseLeadJetHighEta.push_back(new TH1D(("hMC_responseLeadJetHighEta" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));

    vHData_responseLeadJetLowEta_30GeVJet.push_back(new TH1D(("hData_responseLeadJetLowEta_30GeVJet" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseLeadJetLowEta_30GeVJet.push_back(new TH1D(("hMC_responseLeadJetLowEta_30GeVJet" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_responseLeadJetMedEta_30GeVJet.push_back(new TH1D(("hData_responseLeadJetMedEta_30GeVJet" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseLeadJetMedEta_30GeVJet.push_back(new TH1D(("hMC_responseLeadJetMedEta_30GeVJet" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));

    vHData_responseLeadJetHighEta_30GeVJet.push_back(new TH1D(("hData_responseLeadJetHighEta_30GeVJet" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseLeadJetHighEta_30GeVJet.push_back(new TH1D(("hMC_responseLeadJetHighEta_30GeVJet" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));

    vHData_responseLeadJetLowEta_100GeVJet.push_back(new TH1D(("hData_responseLeadJetLowEta_100GeVJet" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseLeadJetLowEta_100GeVJet.push_back(new TH1D(("hMC_responseLeadJetLowEta_100GeVJet" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_responseLeadJetMedEta_100GeVJet.push_back(new TH1D(("hData_responseLeadJetMedEta_100GeVJet" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseLeadJetMedEta_100GeVJet.push_back(new TH1D(("hMC_responseLeadJetMedEta_100GeVJet" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));

    vHData_responseLeadJetHighEta_100GeVJet.push_back(new TH1D(("hData_responseLeadJetHighEta_100GeVJet" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_responseLeadJetHighEta_100GeVJet.push_back(new TH1D(("hMC_responseLeadJetHighEta_100GeVJet" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
 

    vHData_res_para.push_back(new TH1D(("hData_res_para" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_res_para.push_back(new TH1D(("hMC_res_para" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHData_res_perp.push_back(new TH1D(("hData_res_perp" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
    vHMC_res_perp.push_back(new TH1D(("hMC_res_perp" + to_string(i)).data(), "", n_resolution_bins, resolution_bins));
  }  

  for (int i = 0; i < n_resolution_bins; i++) {
    for (int j = 0; j <  hist_names.size(); j++) {
      TString nameDataTempRaw = "hRaw_Response" + to_string(i+1) + "0";
      TString nameMCTempRaw = "hRaw_Response" + to_string(i+1) + to_string(histIdx);
      TString nameDataTempRawEE = "hRaw_ResponseEE" + to_string(i+1) + "0";
      TString nameMCTempRawEE = "hRaw_ResponseEE" + to_string(i+1) + to_string(histIdx);
      TString nameDataTempRawMM = "hRaw_ResponseMM" + to_string(i+1) + "0";
      TString nameMCTempRawMM = "hRaw_ResponseMM" + to_string(i+1) + to_string(histIdx);
      TH1D* hDataTempRaw = (TH1D*)vFiles[0]->Get(nameDataTempRaw);
      TH1D* hMCTempRaw = (TH1D*)vFiles[1]->Get(nameMCTempRaw);
      TH1D* hDataTempRawEE = (TH1D*)vFiles[0]->Get(nameDataTempRawEE);
      TH1D* hMCTempRawEE = (TH1D*)vFiles[1]->Get(nameMCTempRawEE);
      TH1D* hDataTempRawMM = (TH1D*)vFiles[0]->Get(nameDataTempRawMM);
      TH1D* hMCTempRawMM = (TH1D*)vFiles[1]->Get(nameMCTempRawMM);

      TH1D* hDataTemp = (TH1D*)vFiles[0]->Get("hT1CMET_Response" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hDataUpTemp = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseUp" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hDataDownTemp = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseDown" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTemp = (TH1D*)vFiles[1]->Get("hT1CMET_Response" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      TH1D* hDataTempEE = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseEE" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempEE = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseEE" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      TH1D* hDataTempMM = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseMM" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempMM = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseMM" + hist_names[j] + to_string(i+1) + to_string(histIdx));

      TH1D* hDataTempVeryLowPU = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseVeryLowPU" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempVeryLowPU = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseVeryLowPU" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      TH1D* hDataTempLowPU = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseLowPU" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempLowPU = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseLowPU" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      TH1D* hDataTempMedPU = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseMedPU" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempMedPU = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseMedPU" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      TH1D* hDataTempHighPU = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseHighPU" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempHighPU = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseHighPU" + hist_names[j] + to_string(i+1) + to_string(histIdx));

      TH1D* hDataTempLeadJetLowEta = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseLeadJetLowEta" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempLeadJetLowEta = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseLeadJetLowEta" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      TH1D* hDataTempLeadJetMedEta = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseLeadJetMedEta" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempLeadJetMedEta = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseLeadJetMedEta" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      TH1D* hDataTempLeadJetHighEta = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseLeadJetHighEta" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempLeadJetHighEta = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseLeadJetHighEta" + hist_names[j] + to_string(i+1) + to_string(histIdx));

      TH1D* hDataTempLeadJetLowEta_30GeVJet = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseLeadJetLowEta_30GeVJet" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempLeadJetLowEta_30GeVJet = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseLeadJetLowEta_30GeVJet" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      TH1D* hDataTempLeadJetMedEta_30GeVJet = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseLeadJetMedEta_30GeVJet" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempLeadJetMedEta_30GeVJet = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseLeadJetMedEta_30GeVJet" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      TH1D* hDataTempLeadJetHighEta_30GeVJet = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseLeadJetHighEta_30GeVJet" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempLeadJetHighEta_30GeVJet = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseLeadJetHighEta_30GeVJet" + hist_names[j] + to_string(i+1) + to_string(histIdx));

      TH1D* hDataTempLeadJetLowEta_100GeVJet = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseLeadJetLowEta_100GeVJet" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempLeadJetLowEta_100GeVJet = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseLeadJetLowEta_100GeVJet" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      TH1D* hDataTempLeadJetMedEta_100GeVJet = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseLeadJetMedEta_100GeVJet" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempLeadJetMedEta_100GeVJet = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseLeadJetMedEta_100GeVJet" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      TH1D* hDataTempLeadJetHighEta_100GeVJet = (TH1D*)vFiles[0]->Get("hT1CMET_ResponseLeadJetHighEta_100GeVJet" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempLeadJetHighEta_100GeVJet = (TH1D*)vFiles[1]->Get("hT1CMET_ResponseLeadJetHighEta_100GeVJet" + hist_names[j] + to_string(i+1) + to_string(histIdx)); 
      //}

      vHData_RawResponse[j]->SetBinContent(i+1, hDataTempRaw->GetMean());
      vHData_RawResponse[j]->SetBinError(i+1, hDataTempRaw->GetMeanError());
      if (!compare_to_data) {
        vHMC_RawResponse[j]->SetBinContent(i+1, hMCTempRaw->GetMean());
        vHMC_RawResponse[j]->SetBinError(i+1, hMCTempRaw->GetMeanError());
      }

      vHData_RawResponseEE[j]->SetBinContent(i+1, hDataTempRawEE->GetMean());
      vHData_RawResponseEE[j]->SetBinError(i+1, hDataTempRawEE->GetMeanError());
      if (!compare_to_data) {
        vHMC_RawResponseEE[j]->SetBinContent(i+1, hMCTempRawEE->GetMean());
        vHMC_RawResponseEE[j]->SetBinError(i+1, hMCTempRawEE->GetMeanError());
      }

      vHData_RawResponseMM[j]->SetBinContent(i+1, hDataTempRawMM->GetMean());
      vHData_RawResponseMM[j]->SetBinError(i+1, hDataTempRawMM->GetMeanError());
      if (!compare_to_data) {
        vHMC_RawResponseMM[j]->SetBinContent(i+1, hMCTempRawMM->GetMean());
        vHMC_RawResponseMM[j]->SetBinError(i+1, hMCTempRawMM->GetMeanError());
      }

      vHData_response[j]->SetBinContent(i+1, hDataTemp->GetMean());
      vHData_response[j]->SetBinError(i+1, hDataTemp->GetMeanError());
      if (!compare_to_data) {
        vHMC_response[j]->SetBinContent(i+1, hMCTemp->GetMean());
        vHMC_response[j]->SetBinError(i+1, hMCTemp->GetMeanError());
      }

      vHData_responseEE[j]->SetBinContent(i+1, hDataTempEE->GetMean());
      vHData_responseEE[j]->SetBinError(i+1, hDataTempEE->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseEE[j]->SetBinContent(i+1, hMCTempEE->GetMean());
        vHMC_responseEE[j]->SetBinError(i+1, hMCTempEE->GetMeanError());
      }

      vHData_responseMM[j]->SetBinContent(i+1, hDataTempMM->GetMean());
      vHData_responseMM[j]->SetBinError(i+1, hDataTempMM->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseMM[j]->SetBinContent(i+1, hMCTempMM->GetMean());
        vHMC_responseMM[j]->SetBinError(i+1, hMCTempMM->GetMeanError());
      }

      vHData_responseVeryLowPU[j]->SetBinContent(i+1, hDataTempVeryLowPU->GetMean());
      vHData_responseVeryLowPU[j]->SetBinError(i+1, hDataTempVeryLowPU->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseVeryLowPU[j]->SetBinContent(i+1, hMCTempVeryLowPU->GetMean());
        vHMC_responseVeryLowPU[j]->SetBinError(i+1, hMCTempVeryLowPU->GetMeanError());
      } 

      vHData_responseLowPU[j]->SetBinContent(i+1, hDataTempLowPU->GetMean());
      vHData_responseLowPU[j]->SetBinError(i+1, hDataTempLowPU->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseLowPU[j]->SetBinContent(i+1, hMCTempLowPU->GetMean());
        vHMC_responseLowPU[j]->SetBinError(i+1, hMCTempLowPU->GetMeanError());
      }

      vHData_responseMedPU[j]->SetBinContent(i+1, hDataTempMedPU->GetMean());
      vHData_responseMedPU[j]->SetBinError(i+1, hDataTempMedPU->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseMedPU[j]->SetBinContent(i+1, hMCTempMedPU->GetMean());
        vHMC_responseMedPU[j]->SetBinError(i+1, hMCTempMedPU->GetMeanError());
      }

      vHData_responseHighPU[j]->SetBinContent(i+1, hDataTempHighPU->GetMean());
      vHData_responseHighPU[j]->SetBinError(i+1, hDataTempHighPU->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseHighPU[j]->SetBinContent(i+1, hMCTempHighPU->GetMean());
        vHMC_responseHighPU[j]->SetBinError(i+1, hMCTempHighPU->GetMeanError());
      }

      vHData_responseLeadJetLowEta[j]->SetBinContent(i+1, hDataTempLeadJetLowEta->GetMean());
      vHData_responseLeadJetLowEta[j]->SetBinError(i+1, hDataTempLeadJetLowEta->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseLeadJetLowEta[j]->SetBinContent(i+1, hMCTempLeadJetLowEta->GetMean());
        vHMC_responseLeadJetLowEta[j]->SetBinError(i+1, hMCTempLeadJetLowEta->GetMeanError());
      }

      vHData_responseLeadJetMedEta[j]->SetBinContent(i+1, hDataTempLeadJetMedEta->GetMean());
      vHData_responseLeadJetMedEta[j]->SetBinError(i+1, hDataTempLeadJetMedEta->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseLeadJetMedEta[j]->SetBinContent(i+1, hMCTempLeadJetMedEta->GetMean());
        vHMC_responseLeadJetMedEta[j]->SetBinError(i+1, hMCTempLeadJetMedEta->GetMeanError());
      }

      vHData_responseLeadJetHighEta[j]->SetBinContent(i+1, hDataTempLeadJetHighEta->GetMean());
      vHData_responseLeadJetHighEta[j]->SetBinError(i+1, hDataTempLeadJetHighEta->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseLeadJetHighEta[j]->SetBinContent(i+1, hMCTempLeadJetHighEta->GetMean());
        vHMC_responseLeadJetHighEta[j]->SetBinError(i+1, hMCTempLeadJetHighEta->GetMeanError());
      }

      vHData_responseLeadJetLowEta_30GeVJet[j]->SetBinContent(i+1, hDataTempLeadJetLowEta_30GeVJet->GetMean());
      vHData_responseLeadJetLowEta_30GeVJet[j]->SetBinError(i+1, hDataTempLeadJetLowEta_30GeVJet->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseLeadJetLowEta_30GeVJet[j]->SetBinContent(i+1, hMCTempLeadJetLowEta_30GeVJet->GetMean());
        vHMC_responseLeadJetLowEta_30GeVJet[j]->SetBinError(i+1, hMCTempLeadJetLowEta_30GeVJet->GetMeanError());
      }

      vHData_responseLeadJetMedEta_30GeVJet[j]->SetBinContent(i+1, hDataTempLeadJetMedEta_30GeVJet->GetMean());
      vHData_responseLeadJetMedEta_30GeVJet[j]->SetBinError(i+1, hDataTempLeadJetMedEta_30GeVJet->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseLeadJetMedEta_30GeVJet[j]->SetBinContent(i+1, hMCTempLeadJetMedEta_30GeVJet->GetMean());
        vHMC_responseLeadJetMedEta_30GeVJet[j]->SetBinError(i+1, hMCTempLeadJetMedEta_30GeVJet->GetMeanError());
      }

      vHData_responseLeadJetHighEta_30GeVJet[j]->SetBinContent(i+1, hDataTempLeadJetHighEta_30GeVJet->GetMean());
      vHData_responseLeadJetHighEta_30GeVJet[j]->SetBinError(i+1, hDataTempLeadJetHighEta_30GeVJet->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseLeadJetHighEta_30GeVJet[j]->SetBinContent(i+1, hMCTempLeadJetHighEta_30GeVJet->GetMean());
        vHMC_responseLeadJetHighEta_30GeVJet[j]->SetBinError(i+1, hMCTempLeadJetHighEta_30GeVJet->GetMeanError());
      }

      vHData_responseLeadJetLowEta_100GeVJet[j]->SetBinContent(i+1, hDataTempLeadJetLowEta_100GeVJet->GetMean());
      vHData_responseLeadJetLowEta_100GeVJet[j]->SetBinError(i+1, hDataTempLeadJetLowEta_100GeVJet->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseLeadJetLowEta_100GeVJet[j]->SetBinContent(i+1, hMCTempLeadJetLowEta_100GeVJet->GetMean());
        vHMC_responseLeadJetLowEta_100GeVJet[j]->SetBinError(i+1, hMCTempLeadJetLowEta_100GeVJet->GetMeanError());
      }

      vHData_responseLeadJetMedEta_100GeVJet[j]->SetBinContent(i+1, hDataTempLeadJetMedEta_100GeVJet->GetMean());
      vHData_responseLeadJetMedEta_100GeVJet[j]->SetBinError(i+1, hDataTempLeadJetMedEta_100GeVJet->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseLeadJetMedEta_100GeVJet[j]->SetBinContent(i+1, hMCTempLeadJetMedEta_100GeVJet->GetMean());
        vHMC_responseLeadJetMedEta_100GeVJet[j]->SetBinError(i+1, hMCTempLeadJetMedEta_100GeVJet->GetMeanError());
      }

      vHData_responseLeadJetHighEta_100GeVJet[j]->SetBinContent(i+1, hDataTempLeadJetHighEta_100GeVJet->GetMean());
      vHData_responseLeadJetHighEta_100GeVJet[j]->SetBinError(i+1, hDataTempLeadJetHighEta_100GeVJet->GetMeanError());
      if (!compare_to_data) {
        vHMC_responseLeadJetHighEta_100GeVJet[j]->SetBinContent(i+1, hMCTempLeadJetHighEta_100GeVJet->GetMean());
        vHMC_responseLeadJetHighEta_100GeVJet[j]->SetBinError(i+1, hMCTempLeadJetHighEta_100GeVJet->GetMeanError());
      }


      delete hDataTempRaw;
      delete hMCTempRaw;
      delete hDataTempRawEE;
      delete hMCTempRawEE;
      delete hDataTempRawMM;
      delete hMCTempRawMM;
      delete hDataTemp;
      delete hMCTemp;
      delete hDataTempEE;
      delete hMCTempEE;
      delete hDataTempMM;
      delete hMCTempMM;
      delete hDataTempVeryLowPU;
      delete hMCTempVeryLowPU;
      delete hDataTempLowPU;
      delete hMCTempLowPU;
      delete hDataTempMedPU;
      delete hMCTempMedPU;
      delete hDataTempHighPU;
      delete hMCTempHighPU;
      delete hDataTempLeadJetLowEta;
      delete hMCTempLeadJetLowEta;
      delete hDataTempLeadJetMedEta;
      delete hMCTempLeadJetMedEta;
      delete hDataTempLeadJetHighEta;
      delete hMCTempLeadJetHighEta;
      delete hDataTempLeadJetLowEta_30GeVJet;
      delete hMCTempLeadJetLowEta_30GeVJet;
      delete hDataTempLeadJetMedEta_30GeVJet;
      delete hMCTempLeadJetMedEta_30GeVJet;
      delete hDataTempLeadJetHighEta_30GeVJet;
      delete hMCTempLeadJetHighEta_30GeVJet;
      delete hDataTempLeadJetLowEta_100GeVJet;
      delete hMCTempLeadJetLowEta_100GeVJet;
      delete hDataTempLeadJetMedEta_100GeVJet;
      delete hMCTempLeadJetMedEta_100GeVJet;
      delete hDataTempLeadJetHighEta_100GeVJet;
      delete hMCTempLeadJetHighEta_100GeVJet;
    }   
  }

  for (int i = 0; i < n_resolution_bins; i++) {
    for (int j = 0; j <  hist_names.size(); j++) {
      TH1D* hDataTempPara = (TH1D*)vFiles[0]->Get("hT1CMET_ResPara" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempPara = (TH1D*)vFiles[1]->Get("hT1CMET_ResPara" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      TH1D* hDataTempPerp = (TH1D*)vFiles[0]->Get("hT1CMET_ResPerp" + hist_names[j] + to_string(i+1) + "0");
      TH1D* hMCTempPerp = (TH1D*)vFiles[1]->Get("hT1CMET_ResPerp" + hist_names[j] + to_string(i+1) + to_string(histIdx));
      for (int k = 2; k < vFiles.size(); k++) {
	if (compare_to_data) continue;
        hMCTempPara->Add((TH1D*)vFiles[k]->Get("hT1CMET_ResPara" + hist_names[j] + to_string(i+1) + to_string(histIdx)));
	hMCTempPerp->Add((TH1D*)vFiles[k]->Get("hT1CMET_ResPerp" + hist_names[j] + to_string(i+1) + to_string(histIdx)));
      }

      vHData_res_para[j]->SetBinContent(i+1, hDataTempPara->GetStdDev() / vHData_response[j]->GetBinContent(i+1));
      vHData_res_perp[j]->SetBinContent(i+1, hDataTempPerp->GetStdDev() / vHData_response[j]->GetBinContent(i+1));
      if (!compare_to_data) {
	vHMC_res_para[j]->SetBinContent(i+1, hMCTempPara->GetStdDev() / vHMC_response[j]->GetBinContent(i+1));
	vHMC_res_perp[j]->SetBinContent(i+1, hMCTempPerp->GetStdDev() / vHMC_response[j]->GetBinContent(i+1));  
      }

      vHData_res_para[j]->SetBinError(i+1, hDataTempPara->GetStdDevError() / vHData_response[j]->GetBinContent(i+1));
      vHData_res_perp[j]->SetBinError(i+1, hDataTempPerp->GetStdDevError() / vHData_response[j]->GetBinContent(i+1));
      if (!compare_to_data) {
	vHMC_res_para[j]->SetBinError(i+1, hMCTempPara->GetStdDevError() / vHMC_response[j]->GetBinContent(i+1));
	vHMC_res_perp[j]->SetBinError(i+1, hMCTempPerp->GetStdDevError() / vHMC_response[j]->GetBinContent(i+1));
      } 

      delete hDataTempPara;
      delete hMCTempPara;
      delete hDataTempPerp;
      delete hMCTempPerp;
    }
  } 

  if (compare_to_data) {
    vHMC_RawResponse.clear();
    for (int i = 0; i < vHData_RawResponse.size(); i++) {
      TString h_name = "vHData_Response" + to_string(i);
      vHMC_RawResponse.push_back((TH1D*)vHData_RawResponse[0]->Clone(h_name));
    }
  }
  Comparison* c_RawResponse = new Comparison(c1, vHData_RawResponse, vHMC_RawResponse);
  c_RawResponse->set_filename(output_name);
  c_RawResponse->set_y_lim_range({0.75, 1.1});
  //c_RawResponse->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_RawResponse->give_hlines({1});
  //c_RawResponse->give_hlines({0.98,1,1.02});
  c_RawResponse->set_info(vInfo);
  c_RawResponse->set_lumi(lumi);
  c_RawResponse->set_multiple_comparisons();
  c_RawResponse->set_no_log();
  c_RawResponse->set_legend_labels(vLabels);
  c_RawResponse->set_scale(1);
  c_RawResponse->set_x_label("q_{T} [GeV]");
  c_RawResponse->set_y_label("-<u_{#parallel}>/q_{T}");
  c_RawResponse->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_RawResponse->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_RawResponse->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_RawResponse->set_rat_lim_range({0.95, 1.05});
  //c_RawResponse->set_legend_lower_right();
  c_RawResponse->give_info("Raw Response");
  c_RawResponse->plot((idx == 0 || idx == 3) ? 0 : 1);

  if (compare_to_data) {
    vHMC_RawResponseEE.clear();
    for (int i = 0; i < vHData_RawResponseEE.size(); i++) {
      TString h_name = "vHData_ResponseEE" + to_string(i);
      vHMC_RawResponseEE.push_back((TH1D*)vHData_RawResponseEE[0]->Clone(h_name));
    }
  }
  Comparison* c_RawResponseEE = new Comparison(c1, vHData_RawResponseEE, vHMC_RawResponseEE);
  c_RawResponseEE->set_filename(output_name);
  c_RawResponseEE->set_y_lim_range({0.75, 1.1});
  c_RawResponseEE->give_hlines({1});
  //c_RawResponseEE->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_RawResponseEE->set_info(vInfo);
  c_RawResponseEE->set_lumi(lumi);
  c_RawResponseEE->set_multiple_comparisons();
  c_RawResponseEE->set_no_log();
  c_RawResponseEE->set_legend_labels(vLabels);
  c_RawResponseEE->set_scale(1);
  c_RawResponseEE->set_x_label("q_{T} [GeV]");
  c_RawResponseEE->set_y_label("-<u_{#parallel}>/q_{T}");
  c_RawResponseEE->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_RawResponseEE->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_RawResponseEE->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_RawResponseEE->set_rat_lim_range({0.95, 1.05});
  c_RawResponseEE->give_info("Raw Response");
  c_RawResponseEE->give_info("Z #rightarrow ee Events");
  //c_RawResponseEE->set_legend_lower_right();
  c_RawResponseEE->plot(1);

  if (compare_to_data) {
    vHMC_RawResponseMM.clear();
    for (int i = 0; i < vHData_RawResponseMM.size(); i++) {
      TString h_name = "vHData_ResponseMM" + to_string(i);
      vHMC_RawResponseMM.push_back((TH1D*)vHData_RawResponseMM[0]->Clone(h_name));
    }
  }
  Comparison* c_RawResponseMM = new Comparison(c1, vHData_RawResponseMM, vHMC_RawResponseMM);
  c_RawResponseMM->set_filename(output_name);
  c_RawResponseMM->set_y_lim_range({0.75, 1.1});
  c_RawResponseMM->give_hlines({1});
  //c_RawResponseMM->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_RawResponseMM->set_info(vInfo);
  c_RawResponseMM->set_lumi(lumi);
  c_RawResponseMM->set_multiple_comparisons();
  c_RawResponseMM->set_no_log();
  c_RawResponseMM->set_legend_labels(vLabels);
  c_RawResponseMM->set_scale(1);
  c_RawResponseMM->set_x_label("q_{T} [GeV]");
  c_RawResponseMM->set_y_label("-<u_{#parallel}>/q_{T}");
  c_RawResponseMM->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_RawResponseMM->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_RawResponseMM->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_RawResponseMM->set_rat_lim_range({0.95, 1.05});
  c_RawResponseMM->give_info("Raw Response");
  c_RawResponseMM->give_info("Z #rightarrow #mu#mu Events");
  //c_RawResponseMM->set_legend_lower_right();
  c_RawResponseMM->plot(1);


  if (compare_to_data) {
    vHMC_response.clear();
    for (int i = 0; i < vHData_response.size(); i++) {
      TString h_name = "vHData_Response" + to_string(i);
      vHMC_response.push_back((TH1D*)vHData_response[0]->Clone(h_name));
    }
  }
  Comparison* c_response = new Comparison(c1, vHData_response, vHMC_response);
  c_response->set_filename(output_name);
  c_response->set_y_lim_range({0.85, 1.1});
  //c_response->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_response->give_hlines({1});
  //c_response->give_hlines({0.98,1,1.02});
  c_response->set_info(vInfo);
  c_response->set_lumi(lumi);
  c_response->set_multiple_comparisons();
  c_response->set_no_log();
  c_response->set_legend_labels(vLabels);
  c_response->set_scale(1);
  c_response->set_x_label("q_{T} [GeV]");
  c_response->set_y_label("-<u_{#parallel}>/q_{T}");
  c_response->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_response->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_response->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_response->set_rat_lim_range({0.95, 1.05});
  c_response->set_legend_lower_right();
  c_response->plot(1);

  if (compare_to_data) {
    vHMC_responseEE.clear();
    for (int i = 0; i < vHData_responseEE.size(); i++) {
      TString h_name = "vHData_ResponseEE" + to_string(i);
      vHMC_responseEE.push_back((TH1D*)vHData_responseEE[0]->Clone(h_name));
    }
  }
  Comparison* c_responseEE = new Comparison(c1, vHData_responseEE, vHMC_responseEE);
  c_responseEE->set_filename(output_name);
  c_responseEE->set_y_lim_range({0.85, 1.1});
  c_responseEE->give_hlines({1});
  //c_responseEE->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseEE->set_info(vInfo);
  c_responseEE->set_lumi(lumi);
  c_responseEE->set_multiple_comparisons();
  c_responseEE->set_no_log();
  c_responseEE->set_legend_labels(vLabels);
  c_responseEE->set_scale(1);
  c_responseEE->set_x_label("q_{T} [GeV]");
  c_responseEE->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseEE->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseEE->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseEE->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseEE->set_rat_lim_range({0.95, 1.05});
  c_responseEE->give_info("Z #rightarrow ee Events");
  c_responseEE->set_legend_lower_right();
  c_responseEE->plot(1);

  if (compare_to_data) {
    vHMC_responseMM.clear();
    for (int i = 0; i < vHData_responseMM.size(); i++) {
      TString h_name = "vHData_ResponseMM" + to_string(i);
      vHMC_responseMM.push_back((TH1D*)vHData_responseMM[0]->Clone(h_name));
    }
  }
  Comparison* c_responseMM = new Comparison(c1, vHData_responseMM, vHMC_responseMM);
  c_responseMM->set_filename(output_name);
  c_responseMM->set_y_lim_range({0.85, 1.1});
  c_responseMM->give_hlines({1});
  //c_responseMM->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseMM->set_info(vInfo);
  c_responseMM->set_lumi(lumi);
  c_responseMM->set_multiple_comparisons();
  c_responseMM->set_no_log();
  c_responseMM->set_legend_labels(vLabels);
  c_responseMM->set_scale(1);
  c_responseMM->set_x_label("q_{T} [GeV]");
  c_responseMM->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseMM->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseMM->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseMM->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseMM->set_rat_lim_range({0.95, 1.05});
  c_responseMM->give_info("Z #rightarrow #mu#mu Events");
  c_responseMM->set_legend_lower_right();
  c_responseMM->plot(1);

  if (compare_to_data) {
    vHMC_responseVeryLowPU.clear();
    for (int i = 0; i < vHData_responseVeryLowPU.size(); i++) {
      TString h_name = "vHData_ResponseVeryLowPU" + to_string(i);
      vHMC_responseVeryLowPU.push_back((TH1D*)vHData_responseVeryLowPU[0]->Clone(h_name));
    }
  }
  Comparison* c_responseVeryLowPU = new Comparison(c1, vHData_responseVeryLowPU, vHMC_responseVeryLowPU);
  c_responseVeryLowPU->set_filename(output_name);
  c_responseVeryLowPU->set_y_lim_range({0.85, 1.1});
  c_responseVeryLowPU->give_hlines({1});
  //c_responseVeryLowPU->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseVeryLowPU->set_info(vInfo);
  c_responseVeryLowPU->set_lumi(lumi);
  c_responseVeryLowPU->set_multiple_comparisons();
  c_responseVeryLowPU->set_no_log();
  c_responseVeryLowPU->set_legend_labels(vLabels);
  c_responseVeryLowPU->set_scale(1);
  c_responseVeryLowPU->set_x_label("q_{T} [GeV]");
  c_responseVeryLowPU->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseVeryLowPU->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseVeryLowPU->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseVeryLowPU->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseVeryLowPU->set_rat_lim_range({0.95, 1.05});
  c_responseVeryLowPU->give_info("N_{vtx} < 10");
  c_responseVeryLowPU->set_legend_lower_right();
  c_responseVeryLowPU->plot(1);

  if (compare_to_data) {
    vHMC_responseLowPU.clear();
    for (int i = 0; i < vHData_responseLowPU.size(); i++) {
      TString h_name = "vHData_ResponseLowPU" + to_string(i);
      vHMC_responseLowPU.push_back((TH1D*)vHData_responseLowPU[0]->Clone(h_name));
    }
  }
  Comparison* c_responseLowPU = new Comparison(c1, vHData_responseLowPU, vHMC_responseLowPU);
  c_responseLowPU->set_filename(output_name);
  c_responseLowPU->set_y_lim_range({0.85, 1.1});
  c_responseLowPU->give_hlines({1});
  //c_responseLowPU->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseLowPU->set_info(vInfo);
  c_responseLowPU->set_lumi(lumi);
  c_responseLowPU->set_multiple_comparisons();
  c_responseLowPU->set_no_log();
  c_responseLowPU->set_legend_labels(vLabels);
  c_responseLowPU->set_scale(1);
  c_responseLowPU->set_x_label("q_{T} [GeV]");
  c_responseLowPU->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseLowPU->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseLowPU->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseLowPU->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseLowPU->set_rat_lim_range({0.95, 1.05});
  c_responseLowPU->give_info("10 < N_{vtx} < 25");
  c_responseLowPU->set_legend_lower_right();
  c_responseLowPU->plot(1);

  if (compare_to_data) {
    vHMC_responseMedPU.clear();
    for (int i = 0; i < vHData_responseMedPU.size(); i++) {
      TString h_name = "vHData_ResponseMedPU" + to_string(i);
      vHMC_responseMedPU.push_back((TH1D*)vHData_responseMedPU[0]->Clone(h_name));
    }
  }
  Comparison* c_responseMedPU = new Comparison(c1, vHData_responseMedPU, vHMC_responseMedPU);
  c_responseMedPU->set_filename(output_name);
  c_responseMedPU->set_y_lim_range({0.85, 1.1});
  c_responseMedPU->give_hlines({1});
  //c_responseMedPU->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseMedPU->set_info(vInfo);
  c_responseMedPU->set_lumi(lumi);
  c_responseMedPU->set_multiple_comparisons();
  c_responseMedPU->set_no_log();
  c_responseMedPU->set_legend_labels(vLabels);
  c_responseMedPU->set_scale(1);
  c_responseMedPU->set_x_label("q_{T} [GeV]");
  c_responseMedPU->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseMedPU->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseMedPU->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseMedPU->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseMedPU->set_rat_lim_range({0.95, 1.05});
  c_responseMedPU->give_info("25 < N_{vtx} < 35");
  c_responseMedPU->set_legend_lower_right();
  c_responseMedPU->plot(1);

  if (compare_to_data) {
    vHMC_responseHighPU.clear();
    for (int i = 0; i < vHData_responseHighPU.size(); i++) {
      TString h_name = "vHData_ResponseHighPU" + to_string(i);
      vHMC_responseHighPU.push_back((TH1D*)vHData_responseHighPU[0]->Clone(h_name));
    }
  }
  Comparison* c_responseHighPU = new Comparison(c1, vHData_responseHighPU, vHMC_responseHighPU);
  c_responseHighPU->set_filename(output_name);
  c_responseHighPU->set_y_lim_range({0.85, 1.1});
  c_responseHighPU->give_hlines({1});
  //c_responseHighPU->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseHighPU->set_info(vInfo);
  c_responseHighPU->set_lumi(lumi);
  c_responseHighPU->set_multiple_comparisons();
  c_responseHighPU->set_no_log();
  c_responseHighPU->set_legend_labels(vLabels);
  c_responseHighPU->set_scale(1);
  c_responseHighPU->set_x_label("q_{T} [GeV]");
  c_responseHighPU->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseHighPU->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseHighPU->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseHighPU->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseHighPU->set_rat_lim_range({0.95, 1.05});
  c_responseHighPU->give_info("N_{vtx} > 35");
  c_responseHighPU->set_legend_lower_right();
  c_responseHighPU->plot(1);

  if (compare_to_data) {
    vHMC_responseLeadJetLowEta.clear();
    for (int i = 0; i < vHData_responseLeadJetLowEta.size(); i++) {
      TString h_name = "vHData_ResponseLeadJetLowEta" + to_string(i);
      vHMC_responseLeadJetLowEta.push_back((TH1D*)vHData_responseLeadJetLowEta[0]->Clone(h_name));
    }
  }
  Comparison* c_responseLeadJetLowEta = new Comparison(c1, vHData_responseLeadJetLowEta, vHMC_responseLeadJetLowEta);
  c_responseLeadJetLowEta->set_filename(output_name);
  c_responseLeadJetLowEta->set_y_lim_range({0.85, 1.1});
  c_responseLeadJetLowEta->give_hlines({1});
  //c_responseLeadJetLowEta->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseLeadJetLowEta->set_info(vInfo);
  c_responseLeadJetLowEta->set_lumi(lumi);
  c_responseLeadJetLowEta->set_multiple_comparisons();
  c_responseLeadJetLowEta->set_no_log();
  c_responseLeadJetLowEta->set_legend_labels(vLabels);
  c_responseLeadJetLowEta->set_scale(1);
  c_responseLeadJetLowEta->set_x_label("q_{T} [GeV]");
  c_responseLeadJetLowEta->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseLeadJetLowEta->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseLeadJetLowEta->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseLeadJetLowEta->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseLeadJetLowEta->set_rat_lim_range({0.95, 1.05});
  c_responseLeadJetLowEta->give_info("Lead jet |#eta| < 1.5");
  c_responseLeadJetLowEta->set_legend_lower_right();
  c_responseLeadJetLowEta->plot(1);

  if (compare_to_data) {
    vHMC_responseLeadJetMedEta.clear();
    for (int i = 0; i < vHData_responseLeadJetMedEta.size(); i++) {
      TString h_name = "vHData_ResponseLeadJetMedEta" + to_string(i);
      vHMC_responseLeadJetMedEta.push_back((TH1D*)vHData_responseLeadJetMedEta[0]->Clone(h_name));
    }
  }
  Comparison* c_responseLeadJetMedEta = new Comparison(c1, vHData_responseLeadJetMedEta, vHMC_responseLeadJetMedEta);
  c_responseLeadJetMedEta->set_filename(output_name);
  c_responseLeadJetMedEta->set_y_lim_range({0.85, 1.1});
  c_responseLeadJetMedEta->give_hlines({1});
  //c_responseLeadJetMedEta->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseLeadJetMedEta->set_info(vInfo);
  c_responseLeadJetMedEta->set_lumi(lumi);
  c_responseLeadJetMedEta->set_multiple_comparisons();
  c_responseLeadJetMedEta->set_no_log();
  c_responseLeadJetMedEta->set_legend_labels(vLabels);
  c_responseLeadJetMedEta->set_scale(1);
  c_responseLeadJetMedEta->set_x_label("q_{T} [GeV]");
  c_responseLeadJetMedEta->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseLeadJetMedEta->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseLeadJetMedEta->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseLeadJetMedEta->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseLeadJetMedEta->set_rat_lim_range({0.95, 1.05});
  c_responseLeadJetMedEta->give_info("Lead jet: 1.5 #leq |#eta| #leq 2.5");
  c_responseLeadJetMedEta->set_legend_lower_right();
  c_responseLeadJetMedEta->plot(1);


  if (compare_to_data) {
    vHMC_responseLeadJetHighEta.clear();
    for (int i = 0; i < vHData_responseLeadJetHighEta.size(); i++) {
      TString h_name = "vHData_ResponseLeadJetHighEta" + to_string(i);
      vHMC_responseLeadJetHighEta.push_back((TH1D*)vHData_responseLeadJetHighEta[0]->Clone(h_name));
    }
  }
  Comparison* c_responseLeadJetHighEta = new Comparison(c1, vHData_responseLeadJetHighEta, vHMC_responseLeadJetHighEta);
  c_responseLeadJetHighEta->set_filename(output_name);
  c_responseLeadJetHighEta->set_y_lim_range({0.65, 1.3});
  c_responseLeadJetHighEta->give_hlines({1});
  //c_responseLeadJetHighEta->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseLeadJetHighEta->set_info(vInfo);
  c_responseLeadJetHighEta->set_lumi(lumi);
  c_responseLeadJetHighEta->set_multiple_comparisons();
  c_responseLeadJetHighEta->set_no_log();
  c_responseLeadJetHighEta->set_legend_labels(vLabels);
  c_responseLeadJetHighEta->set_scale(1);
  c_responseLeadJetHighEta->set_x_label("q_{T} [GeV]");
  c_responseLeadJetHighEta->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseLeadJetHighEta->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseLeadJetHighEta->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseLeadJetHighEta->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseLeadJetHighEta->set_rat_lim_range({0.95, 1.05});
  c_responseLeadJetHighEta->give_info("Lead jet |#eta| #geq 2.5");
  c_responseLeadJetHighEta->set_legend_lower_right();
  c_responseLeadJetHighEta->plot(1);


  if (compare_to_data) {
    vHMC_responseLeadJetLowEta_30GeVJet.clear();
    for (int i = 0; i < vHData_responseLeadJetLowEta_30GeVJet.size(); i++) {
      TString h_name = "vHData_ResponseLeadJetLowEta_30GeVJet" + to_string(i);
      vHMC_responseLeadJetLowEta_30GeVJet.push_back((TH1D*)vHData_responseLeadJetLowEta_30GeVJet[0]->Clone(h_name));
    }
  }
  Comparison* c_responseLeadJetLowEta_30GeVJet = new Comparison(c1, vHData_responseLeadJetLowEta_30GeVJet, vHMC_responseLeadJetLowEta_30GeVJet);
  c_responseLeadJetLowEta_30GeVJet->set_filename(output_name);
  c_responseLeadJetLowEta_30GeVJet->set_y_lim_range({0.85, 1.1});
  c_responseLeadJetLowEta_30GeVJet->give_hlines({1});
  //c_responseLeadJetLowEta_30GeVJet->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseLeadJetLowEta_30GeVJet->set_info(vInfo);
  c_responseLeadJetLowEta_30GeVJet->set_lumi(lumi);
  c_responseLeadJetLowEta_30GeVJet->set_multiple_comparisons();
  c_responseLeadJetLowEta_30GeVJet->set_no_log();
  c_responseLeadJetLowEta_30GeVJet->set_legend_labels(vLabels);
  c_responseLeadJetLowEta_30GeVJet->set_scale(1);
  c_responseLeadJetLowEta_30GeVJet->set_x_label("q_{T} [GeV]");
  c_responseLeadJetLowEta_30GeVJet->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseLeadJetLowEta_30GeVJet->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseLeadJetLowEta_30GeVJet->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseLeadJetLowEta_30GeVJet->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseLeadJetLowEta_30GeVJet->set_rat_lim_range({0.95, 1.05});
  c_responseLeadJetLowEta_30GeVJet->give_info("Lead jet |#eta| < 1.5");
  c_responseLeadJetLowEta_30GeVJet->give_info("N_{jets} (p_{T} > 30 GeV) == 1");
  c_responseLeadJetLowEta_30GeVJet->set_legend_lower_right();
  c_responseLeadJetLowEta_30GeVJet->plot(1);

  if (compare_to_data) {
    vHMC_responseLeadJetMedEta_30GeVJet.clear();
    for (int i = 0; i < vHData_responseLeadJetMedEta_30GeVJet.size(); i++) {
      TString h_name = "vHData_ResponseLeadJetMedEta_30GeVJet" + to_string(i);
      vHMC_responseLeadJetMedEta_30GeVJet.push_back((TH1D*)vHData_responseLeadJetMedEta_30GeVJet[0]->Clone(h_name));
    }
  }
  Comparison* c_responseLeadJetMedEta_30GeVJet = new Comparison(c1, vHData_responseLeadJetMedEta_30GeVJet, vHMC_responseLeadJetMedEta_30GeVJet);
  c_responseLeadJetMedEta_30GeVJet->set_filename(output_name);
  c_responseLeadJetMedEta_30GeVJet->set_y_lim_range({0.85, 1.1});
  c_responseLeadJetMedEta_30GeVJet->give_hlines({1});
  //c_responseLeadJetMedEta_30GeVJet->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseLeadJetMedEta_30GeVJet->set_info(vInfo);
  c_responseLeadJetMedEta_30GeVJet->set_lumi(lumi);
  c_responseLeadJetMedEta_30GeVJet->set_multiple_comparisons();
  c_responseLeadJetMedEta_30GeVJet->set_no_log();
  c_responseLeadJetMedEta_30GeVJet->set_legend_labels(vLabels);
  c_responseLeadJetMedEta_30GeVJet->set_scale(1);
  c_responseLeadJetMedEta_30GeVJet->set_x_label("q_{T} [GeV]");
  c_responseLeadJetMedEta_30GeVJet->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseLeadJetMedEta_30GeVJet->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseLeadJetMedEta_30GeVJet->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseLeadJetMedEta_30GeVJet->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseLeadJetMedEta_30GeVJet->set_rat_lim_range({0.95, 1.05});
  c_responseLeadJetMedEta_30GeVJet->give_info("Lead jet: 1.5 #leq |#eta| #leq 2.5");
  c_responseLeadJetMedEta_30GeVJet->give_info("N_{jets} (p_{T} > 30 GeV) == 1");
  c_responseLeadJetMedEta_30GeVJet->set_legend_lower_right();
  c_responseLeadJetMedEta_30GeVJet->plot(1);


  if (compare_to_data) {
    vHMC_responseLeadJetHighEta_30GeVJet.clear();
    for (int i = 0; i < vHData_responseLeadJetHighEta_30GeVJet.size(); i++) {
      TString h_name = "vHData_ResponseLeadJetHighEta_30GeVJet" + to_string(i);
      vHMC_responseLeadJetHighEta_30GeVJet.push_back((TH1D*)vHData_responseLeadJetHighEta_30GeVJet[0]->Clone(h_name));
    }
  }
  Comparison* c_responseLeadJetHighEta_30GeVJet = new Comparison(c1, vHData_responseLeadJetHighEta_30GeVJet, vHMC_responseLeadJetHighEta_30GeVJet);
  c_responseLeadJetHighEta_30GeVJet->set_filename(output_name);
  c_responseLeadJetHighEta_30GeVJet->set_y_lim_range({0.65, 1.3});
  c_responseLeadJetHighEta_30GeVJet->give_hlines({1});
  //c_responseLeadJetHighEta_30GeVJet->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseLeadJetHighEta_30GeVJet->set_info(vInfo);
  c_responseLeadJetHighEta_30GeVJet->set_lumi(lumi);
  c_responseLeadJetHighEta_30GeVJet->set_multiple_comparisons();
  c_responseLeadJetHighEta_30GeVJet->set_no_log();
  c_responseLeadJetHighEta_30GeVJet->set_legend_labels(vLabels);
  c_responseLeadJetHighEta_30GeVJet->set_scale(1);
  c_responseLeadJetHighEta_30GeVJet->set_x_label("q_{T} [GeV]");
  c_responseLeadJetHighEta_30GeVJet->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseLeadJetHighEta_30GeVJet->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseLeadJetHighEta_30GeVJet->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseLeadJetHighEta_30GeVJet->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseLeadJetHighEta_30GeVJet->set_rat_lim_range({0.95, 1.05});
  c_responseLeadJetHighEta_30GeVJet->give_info("Lead jet |#eta| #geq 2.5");
  c_responseLeadJetHighEta_30GeVJet->give_info("N_{jets} (p_{T} > 30 GeV) == 1");
  c_responseLeadJetHighEta_30GeVJet->set_legend_lower_right();
  c_responseLeadJetHighEta_30GeVJet->plot(1);


  if (compare_to_data) {
    vHMC_responseLeadJetLowEta_100GeVJet.clear();
    for (int i = 0; i < vHData_responseLeadJetLowEta_100GeVJet.size(); i++) {
      TString h_name = "vHData_ResponseLeadJetLowEta_100GeVJet" + to_string(i);
      vHMC_responseLeadJetLowEta_100GeVJet.push_back((TH1D*)vHData_responseLeadJetLowEta_100GeVJet[0]->Clone(h_name));
    }
  }
  Comparison* c_responseLeadJetLowEta_100GeVJet = new Comparison(c1, vHData_responseLeadJetLowEta_100GeVJet, vHMC_responseLeadJetLowEta_100GeVJet);
  c_responseLeadJetLowEta_100GeVJet->set_filename(output_name);
  c_responseLeadJetLowEta_100GeVJet->set_y_lim_range({0.85, 1.1});
  c_responseLeadJetLowEta_100GeVJet->give_hlines({1});
  //c_responseLeadJetLowEta_100GeVJet->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseLeadJetLowEta_100GeVJet->set_info(vInfo);
  c_responseLeadJetLowEta_100GeVJet->set_lumi(lumi);
  c_responseLeadJetLowEta_100GeVJet->set_multiple_comparisons();
  c_responseLeadJetLowEta_100GeVJet->set_no_log();
  c_responseLeadJetLowEta_100GeVJet->set_legend_labels(vLabels);
  c_responseLeadJetLowEta_100GeVJet->set_scale(1);
  c_responseLeadJetLowEta_100GeVJet->set_x_label("q_{T} [GeV]");
  c_responseLeadJetLowEta_100GeVJet->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseLeadJetLowEta_100GeVJet->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseLeadJetLowEta_100GeVJet->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseLeadJetLowEta_100GeVJet->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseLeadJetLowEta_100GeVJet->set_rat_lim_range({0.95, 1.05});
  c_responseLeadJetLowEta_100GeVJet->give_info("Lead jet |#eta| < 1.5");
  c_responseLeadJetLowEta_100GeVJet->give_info("N_{jets} (p_{T} > 30 GeV) == 1");
  c_responseLeadJetLowEta_100GeVJet->give_info("Lead jet p_{T} > 100 GeV");
  c_responseLeadJetLowEta_100GeVJet->set_legend_lower_right();
  c_responseLeadJetLowEta_100GeVJet->plot(1);

  if (compare_to_data) {
    vHMC_responseLeadJetMedEta_100GeVJet.clear();
    for (int i = 0; i < vHData_responseLeadJetMedEta_100GeVJet.size(); i++) {
      TString h_name = "vHData_ResponseLeadJetMedEta_100GeVJet" + to_string(i);
      vHMC_responseLeadJetMedEta_100GeVJet.push_back((TH1D*)vHData_responseLeadJetMedEta_100GeVJet[0]->Clone(h_name));
    }
  }
  Comparison* c_responseLeadJetMedEta_100GeVJet = new Comparison(c1, vHData_responseLeadJetMedEta_100GeVJet, vHMC_responseLeadJetMedEta_100GeVJet);
  c_responseLeadJetMedEta_100GeVJet->set_filename(output_name);
  c_responseLeadJetMedEta_100GeVJet->set_y_lim_range({0.85, 1.1});
  c_responseLeadJetMedEta_100GeVJet->give_hlines({1});
  //c_responseLeadJetMedEta_100GeVJet->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseLeadJetMedEta_100GeVJet->set_info(vInfo);
  c_responseLeadJetMedEta_100GeVJet->set_lumi(lumi);
  c_responseLeadJetMedEta_100GeVJet->set_multiple_comparisons();
  c_responseLeadJetMedEta_100GeVJet->set_no_log();
  c_responseLeadJetMedEta_100GeVJet->set_legend_labels(vLabels);
  c_responseLeadJetMedEta_100GeVJet->set_scale(1);
  c_responseLeadJetMedEta_100GeVJet->set_x_label("q_{T} [GeV]");
  c_responseLeadJetMedEta_100GeVJet->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseLeadJetMedEta_100GeVJet->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseLeadJetMedEta_100GeVJet->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseLeadJetMedEta_100GeVJet->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseLeadJetMedEta_100GeVJet->set_rat_lim_range({0.95, 1.05});
  c_responseLeadJetMedEta_100GeVJet->give_info("Lead jet: 1.5 #leq |#eta| #leq 2.5");
  c_responseLeadJetMedEta_100GeVJet->give_info("N_{jets} (p_{T} > 30 GeV) == 1");
  c_responseLeadJetMedEta_100GeVJet->give_info("Lead jet p_{T} > 100 GeV");
  c_responseLeadJetMedEta_100GeVJet->set_legend_lower_right();
  c_responseLeadJetMedEta_100GeVJet->plot(1);


  if (compare_to_data) {
    vHMC_responseLeadJetHighEta_100GeVJet.clear();
    for (int i = 0; i < vHData_responseLeadJetHighEta_100GeVJet.size(); i++) {
      TString h_name = "vHData_ResponseLeadJetHighEta_100GeVJet" + to_string(i);
      vHMC_responseLeadJetHighEta_100GeVJet.push_back((TH1D*)vHData_responseLeadJetHighEta_100GeVJet[0]->Clone(h_name));
    }
  }
  Comparison* c_responseLeadJetHighEta_100GeVJet = new Comparison(c1, vHData_responseLeadJetHighEta_100GeVJet, vHMC_responseLeadJetHighEta_100GeVJet);
  c_responseLeadJetHighEta_100GeVJet->set_filename(output_name);
  c_responseLeadJetHighEta_100GeVJet->set_y_lim_range({0.65, 1.3});
  c_responseLeadJetHighEta_100GeVJet->give_hlines({1});
  //c_responseLeadJetHighEta_100GeVJet->give_hlines({0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08});
  c_responseLeadJetHighEta_100GeVJet->set_info(vInfo);
  c_responseLeadJetHighEta_100GeVJet->set_lumi(lumi);
  c_responseLeadJetHighEta_100GeVJet->set_multiple_comparisons();
  c_responseLeadJetHighEta_100GeVJet->set_no_log();
  c_responseLeadJetHighEta_100GeVJet->set_legend_labels(vLabels);
  c_responseLeadJetHighEta_100GeVJet->set_scale(1);
  c_responseLeadJetHighEta_100GeVJet->set_x_label("q_{T} [GeV]");
  c_responseLeadJetHighEta_100GeVJet->set_y_label("-<u_{#parallel}>/q_{T}");
  c_responseLeadJetHighEta_100GeVJet->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_responseLeadJetHighEta_100GeVJet->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_responseLeadJetHighEta_100GeVJet->set_rat_lim_range({0.9, 1.1});
  if (compare_to_data)
    c_responseLeadJetHighEta_100GeVJet->set_rat_lim_range({0.95, 1.05});
  c_responseLeadJetHighEta_100GeVJet->give_info("Lead jet |#eta| #geq 2.5");
  c_responseLeadJetHighEta_100GeVJet->give_info("N_{jets} (p_{T} > 30 GeV) == 1");
  c_responseLeadJetHighEta_100GeVJet->give_info("Lead jet p_{T} > 100 GeV");
  c_responseLeadJetHighEta_100GeVJet->set_legend_lower_right();
  c_responseLeadJetHighEta_100GeVJet->plot(1);


  if (compare_to_data) {
    vHMC_res_para.clear();
    for (int i = 0; i < vHData_res_para.size(); i++) {
      TString h_name = "vHData_res_para" + to_string(i);
      vHMC_res_para.push_back((TH1D*)vHData_res_para[0]->Clone(h_name));
    }
  }
  Comparison* c_res_para = new Comparison(c1, vHData_res_para, vHMC_res_para);
  c_res_para->set_filename(output_name);  
  c_res_para->set_x_bin_range({6, 23});
  c_res_para->set_y_lim_range({25, 60});
  c_res_para->set_no_flow();
  c_res_para->set_info(vInfo);
  c_res_para->set_lumi(lumi);
  c_res_para->set_multiple_comparisons();
  c_res_para->set_no_log();
  c_res_para->set_legend_labels(vLabels);
  c_res_para->set_scale(1);
  c_res_para->set_x_label("q_{T} [GeV]");
  c_res_para->set_y_label("#sigma(u_{#parallel}) [GeV]");
  c_res_para->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_res_para->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_res_para->give_info("Response corrected");
  c_res_para->set_rat_lim_range({0.75, 1.25});
  if (compare_to_data)
    c_res_para->set_rat_lim_range({0.95, 1.05});
  c_res_para->plot(1);

  if (compare_to_data) {
    vHMC_res_perp.clear();
    for (int i = 0; i < vHData_res_perp.size(); i++) {
      TString h_name = "vHData_res_perp" + to_string(i);
      vHMC_res_perp.push_back((TH1D*)vHData_res_perp[0]->Clone(h_name));
    }
  }
  Comparison* c_res_perp = new Comparison(c1, vHData_res_perp, vHMC_res_perp);
  c_res_perp->set_filename(output_name);
  c_res_perp->set_x_bin_range({6, 23});
  c_res_perp->set_y_lim_range({25, 60});
  c_res_perp->set_no_flow();
  c_res_perp->set_info(vInfo);
  c_res_perp->set_lumi(lumi);
  c_res_perp->set_multiple_comparisons();
  c_res_perp->set_no_log();
  c_res_perp->set_legend_labels(vLabels);
  c_res_perp->set_scale(1);
  c_res_perp->set_x_label("q_{T} [GeV]");
  c_res_perp->set_y_label("#sigma(u_{#perp}) [GeV]");
  c_res_perp->set_rat_label("#frac{Data}{MC}");
  if (compare_to_data)
    c_res_perp->set_rat_label("#frac{Data}{" + vLabels[0] + "}");
  c_res_perp->give_info("Response corrected");
  c_res_perp->set_rat_lim_range({0.75, 1.25});
  if (compare_to_data)
    c_res_perp->set_rat_lim_range({0.95, 1.05});
  c_res_perp->plot((idx == 2 || idx ==3) ? 2 : 1);

}


int main(int argc, char* argv[])
{
  // Style options
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow,0);
  gStyle->SetPaintTextFormat(".2f");
  gStyle->SetTickLength(0.01);
  //gStyle->SetErrorX(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetTitleAlign(33);
  gStyle->SetTitleX(.93);
  gStyle->SetTitleFontSize(0.03);


  TString eras = argv[1];
  string eras_s = argv[1];
  double lumi = atof(argv[2]);

  bool relval = argc >= 3;

  string output_name = "plots_2018" + eras_s + ".pdf";
  string output_res = "plots_res" + eras_s + ".pdf";

  int histIdx;
  if (eras == "A") 	histIdx = 0;
  if (eras == "B")	histIdx = 1;
  if (eras == "C")	histIdx = 2;
  if (eras == "D")	histIdx = 3;
  if (eras == "All")	histIdx = 4;

  TString era_info;
  if (eras == "A") 	era_info = "Run 2018A";
  if (eras == "B")      era_info = "Run 2018B";
  if (eras == "C")      era_info = "Run 2018C";
  if (eras == "D")      era_info = "Run 2018D";
  if (eras == "All")	era_info = "2018 All Runs";

  TFile* fData;
  if (eras == "All")
    fData = new TFile("../histograms/Data_All.root");
  else
    fData = new TFile("../histograms/Data_Run2018" + eras + ".root");
  TFile* fDY;
  //if (!relval)
  //  fDY = new TFile("../histograms/DY.root");
  //else
  fDY = new TFile("../histograms/DY.root");
  TFile* fDY_RelVal;
  fDY_RelVal = new TFile("../histograms/DY_RelVal.root");

  vector<TFile*> vFiles = {fData, fDY, fDY_RelVal};

  TCanvas* c1 = new TCanvas("c1", "histos", 600, 800);

  //double scaleMC = lumi; 
  vector<double> scaleMC = get_normalizations(vFiles, lumi, histIdx);

  make_plot(c1, histIdx, vFiles, output_name, "h_eta5_cand3MET", "E_{T}^{miss}", lumi, scaleMC, {"Raw E_{T}^{miss}"}, 0); 
  make_plot(c1, histIdx, vFiles, output_name, "hCCpT", "p_{T} [GeV]", lumi, scaleMC, {"Charged Candidates"}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hPhotonpT", "p_{T} [GeV]", lumi, scaleMC, {"Photon Candidates"}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hNeutpT", "p_{T} [GeV]", lumi, scaleMC, {"Neutral Hadron Candidates"}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hCCeta", "|#eta|", lumi, scaleMC, {"Charged Candidates"}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hPhotoneta", "|#eta|", lumi, scaleMC, {"Photon Candidates"}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hNeuteta", "|#eta|", lumi, scaleMC, {"Neutral Hadron Candidates"}, 1);

  vector<TString> vEta = {"0 < |#eta| < 1.3", "1.3 < |#eta| < 1.6", "1.6 < |#eta| < 2.5", "2.5 < |#eta| < 3.0", "|#eta| > 3.0", "All #eta"};
  vector<TString> vCands = {"Charged Candidates", "Photon Candidates", "Neutral Hadrons"};
  for (int i=0; i<nEtaRegions-1; i++) {
    for (int j=0; j<nCandCats-1; j++) {
      vector<TString> v1;
      v1.push_back(vCands[j]);
      v1.push_back(vEta[i]);
      make_plot(c1, histIdx, vFiles, output_name, "h_eta" + to_string(i) + "_cand" + to_string(j) + "MET", "E_{T}^{miss} [GeV]", lumi, scaleMC, v1, 1);
      make_plot(c1, histIdx, vFiles, output_name, "h_eta" + to_string(i) + "_cand" + to_string(j) + "SumET", "Sum E_{T} [GeV]", lumi, scaleMC, v1, 1);
      make_plot(c1, histIdx, vFiles, output_name, "h_eta" + to_string(i) + "_cand" + to_string(j) + "METPhi", "E_{T}^{miss} #phi", lumi, scaleMC, v1, 1); 
    }
  }

  make_plot(c1, histIdx, vFiles, output_name, "hJet_Emfrac", "Jet EM Fraction", lumi, scaleMC, {""}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hJet_Neutral_Emfrac", "Jet Neutral EM Fraction", lumi, scaleMC, {""}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hJet_Neutral_Hadfrac", "Jet Neutral Hadronic Fraction", lumi, scaleMC, {""}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hJet_Charged_Hadfrac", "Jet Charged Hadronic Fraction", lumi, scaleMC, {""}, 1);

  make_plot(c1, histIdx, vFiles, output_name, "hJet_Neutral_Emfrac_central", "Jet Neutral EM Fraction", lumi, scaleMC, {"|#eta| <= 2.7"}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hJet_Neutral_Hadfrac_central", "Jet Neutral Hadronic Fraction", lumi, scaleMC, {"|#eta| <= 2.7"}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hJet_Neutral_Emfrac_forward", "Jet Neutral EM Fraction", lumi, scaleMC, {"2.7 < |#eta| <= 3.0"}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hJet_Neutral_Hadfrac_forward", "Jet Neutral Hadronic Fraction", lumi, scaleMC, {"2.7 < |#eta| <= 3.0"}, 1);

  make_plot(c1, histIdx, vFiles, output_name, "hJet_Neutral_Emfrac_hf", "Jet HF Neutral EM Fraction", lumi, scaleMC, {"|#eta| #geq 3.0"}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hJet_Neutral_Hadfrac_hf", "Jet HF Neutral Hadronic Fraction", lumi, scaleMC, {"|#eta| #geq 3.0"}, 1);

  make_plot(c1, histIdx, vFiles, output_name, "hVertexZ", "0th Vertex Z", lumi, scaleMC, {}, 1);
  make_plot(c1, histIdx, vFiles, output_name, "hVertexZ_forwardPhoton", "0th Vertex Z", lumi, scaleMC, {"Events with:",  "pf #gamma# with |#eta| #geq 3.5"}, 1);

  make_plot(c1, histIdx, vFiles, output_name, "hT1CMET", "Type-1 Corr. E_{T}^{miss} [GeV]", lumi, scaleMC, {"2017F V32 JECs"}, 1);

  make_resolution_plots(c1, histIdx, vFiles, output_name, {"V32"}, lumi, 1, {era_info}, {"2017F V32 JECs"}, 1);

  make_plot(c1, histIdx, vFiles, output_name, "hNVtx", "# Vertices", lumi, scaleMC, {""}, 2);
  get_normalization(vFiles, lumi, histIdx);
}
