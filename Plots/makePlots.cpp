#include "PlotHelper.h"

const int nEtaRegions = 6;
const int nCandCats = 4;

const double met_bins[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 195, 210, 230, 255, 285, 335, 400};
const int n_met_bins = (sizeof(met_bins) / sizeof(met_bins[0])) - 1;

void make_plot(TCanvas* c1, int histIdx, vector<TFile*> vFiles, string output_name, TString hist_name, TString x_label, double lumi, double scale, vector<TString> vInfo, int idx, bool rebin = false, vector<double> rat_range = {}) {
  TH1D* hDataTemp = (TH1D*)vFiles[0]->Get(hist_name + "0");
  TH1D* hData;
  if (rebin)
    hData = (TH1D*)hDataTemp->Rebin(n_met_bins, hist_name + "0" + "rebin", met_bins);
  else
    hData = (TH1D*)hDataTemp->Clone(hist_name + "0" + "rebin");
  vector<TH1D*> hMC;
  if (histIdx == 4) {
    for (int i = 1; i < vFiles.size(); i++) {
      TH1D* hMCTemp;
      for (int h = 0; h < 4; h++) {
	TH1D* hMCTemp1;
        TH1D* hMCTemp2;
	hMCTemp1 = (TH1D*)vFiles[i]->Get(hist_name + to_string(h));
	if (rebin)
	  hMCTemp2 = (TH1D*)hMCTemp1->Rebin(n_met_bins, hist_name + to_string(h) + "rebin", met_bins);
	else
	  hMCTemp2 = (TH1D*)hMCTemp1->Clone(hist_name + to_string(h) + "rebin");
	double era_lumi;
	if (h == 0)
	  era_lumi = 4.8;
	else if (h == 1)
	  era_lumi = 9.76;
	else if (h ==2)
	  era_lumi = 13.74;
	else if (h == 3)
	  era_lumi = 13.5;
	hMCTemp2->Scale(era_lumi);
	if (h == 0) {
          hMCTemp = (TH1D*)hMCTemp2->Clone(hist_name + "clone");
	}
        else {
	  hMCTemp->Add(hMCTemp2);	
	}
      }
      hMC.push_back(hMCTemp); 
   } 
  }
  else {
    for (int i = 1; i < vFiles.size(); i++) {
      TH1D* hMCTemp1 = (TH1D*)vFiles[i]->Get(hist_name + to_string(histIdx)); 
      TH1D* hMCTemp2;
      if (rebin)
	hMCTemp2 = (TH1D*)hMCTemp1->Rebin(n_met_bins, hist_name + to_string(histIdx) + "rebin", met_bins);
      else
	hMCTemp2 = (TH1D*)hMCTemp1->Clone(hist_name + to_string(histIdx) + "rebin");
      hMC.push_back(hMCTemp2);
      delete hMCTemp1;
    }
  }
  TString name = output_name;
  cout << hData->GetMean() << endl;
  for (int i = 0; i < hMC.size(); i++)
    cout << hMC[i]->GetMean() << endl;
  Comparison* c = new Comparison(c1, hData, hMC);
  if (lumi == 4.8)
    c->give_info({"2017 Run B"});
  else if (lumi == 23.5)
    c->give_info({"2017 Runs C,D,E"});
  else if (lumi == 13.5) {
    if (name.Contains("v2"))
      c->give_info({"2017 Run F (09May2018 ReReco)"});
    else
      c->give_info({"2017 Run F"});
  }
  else if (lumi == 9.76)
    c->give_info({"2017 Run C"});
  else if (lumi == 13.74)
    c->give_info({"2017 Runs D,E"});
  else if (lumi == 41.5)
    c->give_info({"2017 All Runs"});
  c->set_filename(output_name);
  c->set_rat_label("#frac{Data}{MC}");
  //c->set_legend_labels({"2017 Data", "Drell-Yan", "DiBoson", "TriBoson", "Top"});
 
  if (name.Contains("v2") && !name.Contains("nominal"))
    c->set_legend_labels({"2018 Data", "Drell-Yan (12Apr2018)", "DiBoson", "Top"});
  else
    c->set_legend_labels({"2018 Data", "Drell-Yan", "DiBoson", "Top"});
  c->set_x_label(x_label);
  c->set_y_label("Events");
  c->set_data_drawOpt("E");
  c->set_lumi(lumi);
  c->set_scale(scale);
  if (rat_range.size() > 0) {
    c->set_rat_lim_range(rat_range);
    c->set_log_rat();
  }
  if (rebin)
    c->set_x_bin_range({1, n_met_bins});
  //if (hist_name.Contains("T1CMET"))
  //  c->set_x_bin_range({1,70});

  if (hist_name == "hT1CMET" || hist_name == "hT1CMET_tightID" || hist_name == "h_eta5_cand3MET") {
    cout << hist_name + to_string(histIdx) << endl;
    cout << hMC[0]->Integral(41,80) << endl;
  }
  for (int i = 0; i < vInfo.size(); i++)
    c->give_info(vInfo[i]);
  if(hist_name.Contains("UPerp"))
  {
      c->give_info(TString("Data Stdev=")+TString(Form("%.3f",hData->GetStdDev()))+TString(" GeV"));
  }
  c->plot(idx);
  delete hData;
  for (int i = 0; i < hMC.size(); i++)
    delete hMC[i];
  delete c;
} 

void make_plot_rat_unc(TCanvas* c1, int histIdx, vector<TFile*> vFiles, string output_name, TString hist_name, TString hist_name_up, TString hist_name_down, TString x_label, double lumi, double scale, vector<TString> vInfo, int idx, bool rebin = false) {
  TH1D* hDataTemp = (TH1D*)vFiles[0]->Get(hist_name + "0");
  TH1D* hData;

  TH1D* hDataUpTemp = (TH1D*)vFiles[0]->Get(hist_name_up + "0");
  TH1D* hDataUp;

  TH1D* hDataDownTemp = (TH1D*)vFiles[0]->Get(hist_name_down + "0");
  TH1D* hDataDown;

  if (rebin) {
    hData = (TH1D*)hDataTemp->Rebin(n_met_bins, hist_name + "0" + "rebin", met_bins);
    hDataUp = (TH1D*)hDataUpTemp->Rebin(n_met_bins, hist_name_up + "0" + "rebin", met_bins);
    hDataDown = (TH1D*)hDataDownTemp->Rebin(n_met_bins, hist_name_down + "0" + "rebin", met_bins);
  }
  else {
    hData = (TH1D*)hDataTemp->Clone(hist_name + "0" + "rebin");
    hDataUp = (TH1D*)hDataUpTemp->Clone(hist_name_up + "0" + "rebin");
    hDataDown = (TH1D*)hDataDownTemp->Clone(hist_name_down + "0" + "rebin");
  }

  vector<TH1D*> hMC;
  vector<TH1D*> hMCUp;
  vector<TH1D*> hMCDown;
  if (histIdx == 4) {
    for (int i = 1; i < vFiles.size(); i++) {
      TH1D* hMCTemp;
      TH1D* hMCUpTemp;
      TH1D* hMCDownTemp;
      for (int h = 0; h < 4; h++) {
	TH1D* hMCTemp1 = (TH1D*)vFiles[i]->Get(hist_name + to_string(h));
	TH1D* hMCTemp2;

	TH1D* hMCUpTemp1 = (TH1D*)vFiles[i]->Get(hist_name_up + to_string(h));
	TH1D* hMCUpTemp2;

	TH1D* hMCDownTemp1 = (TH1D*)vFiles[i]->Get(hist_name_down + to_string(h));
	TH1D* hMCDownTemp2;
        if (rebin) {
	  hMCTemp2 = (TH1D*)hMCTemp1->Rebin(n_met_bins, hist_name + to_string(h) + "rebin", met_bins);
	  hMCUpTemp2 = (TH1D*)hMCUpTemp1->Rebin(n_met_bins, hist_name_up + to_string(h) + "rebin", met_bins);
	  hMCDownTemp2 = (TH1D*)hMCDownTemp1->Rebin(n_met_bins, hist_name_down + to_string(h) + "rebin", met_bins);
	}
	else {
	  hMCTemp2 = (TH1D*)hMCTemp1->Clone(hist_name + to_string(h) + "rebin");
	  hMCUpTemp2 = (TH1D*)hMCUpTemp1->Clone(hist_name_up + to_string(h) + "rebin");
	  hMCDownTemp2 = (TH1D*)hMCDownTemp1->Clone(hist_name_down + to_string(h) + "rebin");   
	}
        double era_lumi;
        if (h == 0)
          era_lumi = 4.8;
        else if (h == 1)
          era_lumi = 9.76;
        else if (h ==2)
          era_lumi = 13.74;
        else if (h == 3)
          era_lumi = 13.5;
        hMCTemp2->Scale(era_lumi);
        hMCUpTemp2->Scale(era_lumi);
        hMCDownTemp2->Scale(era_lumi);     
	if (h == 0) {
	  hMCTemp = (TH1D*)hMCTemp2->Clone(hist_name + to_string(h) + "clone");
	  hMCUpTemp = (TH1D*)hMCUpTemp2->Clone(hist_name_up + to_string(h) + "clone");
	  hMCDownTemp = (TH1D*)hMCDownTemp2->Clone(hist_name_down + to_string(h) + "clone");
        }
        else {
	  hMCTemp->Add(hMCTemp2);
	  hMCUpTemp->Add(hMCUpTemp2);
	  hMCDownTemp->Add(hMCDownTemp2);
	}
	delete hMCTemp1;
	delete hMCTemp2;
	delete hMCUpTemp1;
	delete hMCUpTemp2;
	delete hMCDownTemp1;
        delete hMCDownTemp2;
      }
      hMC.push_back(hMCTemp);
      hMCUp.push_back(hMCUpTemp);
      hMCDown.push_back(hMCDownTemp); 
    }
  }
  else {
    for (int i = 1; i < vFiles.size(); i++) {
      cout<<hist_name<<histIdx<<endl;
      TH1D* hMCTemp1 = (TH1D*)vFiles[i]->Get(hist_name + to_string(histIdx));
      TH1D* hMCTemp2;

      TH1D* hMCUpTemp1 = (TH1D*)vFiles[i]->Get(hist_name_up + to_string(histIdx));
      TH1D* hMCUpTemp2;

      TH1D* hMCDownTemp1 = (TH1D*)vFiles[i]->Get(hist_name_down + to_string(histIdx));
      TH1D* hMCDownTemp2;

      if (rebin) {
	hMCTemp2 = (TH1D*)hMCTemp1->Rebin(n_met_bins, hist_name + to_string(histIdx) + "rebin", met_bins);
	hMCUpTemp2 = (TH1D*)hMCUpTemp1->Rebin(n_met_bins, hist_name_up + to_string(histIdx) + "rebin", met_bins);
	hMCDownTemp2 = (TH1D*)hMCDownTemp1->Rebin(n_met_bins, hist_name_down + to_string(histIdx) + "rebin", met_bins);
      }
      else {
        cout<<hMCTemp1->GetName()<<hist_name<<histIdx<<endl;
	hMCTemp2 = (TH1D*)hMCTemp1->Clone(hist_name + to_string(histIdx) + "rebin");
	hMCUpTemp2 = (TH1D*)hMCUpTemp1->Clone(hist_name_up + to_string(histIdx) + "rebin");
	hMCDownTemp2 = (TH1D*)hMCDownTemp1->Clone(hist_name_down + to_string(histIdx) + "rebin");    
      }
      hMC.push_back(hMCTemp2);
      hMCUp.push_back(hMCUpTemp2);
      hMCDown.push_back(hMCDownTemp2);

      delete hMCTemp1;
      delete hMCUpTemp1;
      delete hMCDownTemp1;
      //hMC.push_back((TH1D*)vFiles[i]->Get(hist_name + to_string(histIdx)));
      //if (rebin)
      //  hMC[i-1]->Rebin(n_met_bins, "", met_bins);
    }
  }
  TH1D* hRatUnc = (TH1D*)hData->Clone("rat_unc");
  TH1D* hRat = (TH1D*)hData->Clone("rat");
  TH1D* hMCSum = (TH1D*)hMC[0]->Clone("mc_unc");
  TH1D* hMCSumUp = (TH1D*)hMCUp[0]->Clone("mc_unc_up");
  TH1D* hMCSumDown = (TH1D*)hMCDown[0]->Clone("mc_unc_down");
  for (int i = 1; i < hMC.size(); i++) {
    hMCSum->Add(hMC[i]);
    hMCSumUp->Add(hMCUp[i]);
    hMCSumDown->Add(hMCDown[i]);
  }
  hMCSum->Scale(scale);
  hMCSumUp->Scale(scale);
  hMCSumDown->Scale(scale);


  TH1D* hRatUp = (TH1D*)hData->Clone("rat_up");
  TH1D* hRatDown = (TH1D*)hData->Clone("rat_down");
  hRat->Divide(hMCSum);
  hRatUp->Divide(hMCSumUp);
  hRatDown->Divide(hMCSumDown);
  for (int i = 0; i < hRatUnc->GetNbinsX() + 1; i++) {
    hRatUnc->SetBinContent(i, 1);
    cout << hRatUp->GetBinContent(i) << " " << hRat->GetBinContent(i) << " " << hRatDown->GetBinContent(i) << endl;
    double diff_up = abs(hRatUp->GetBinContent(i) - hRat->GetBinContent(i));
    double diff_down = abs(hRatDown->GetBinContent(i) - hRat->GetBinContent(i));
    double larger_unc = diff_up > diff_down ? diff_up : diff_down; 
    hRatUnc->SetBinError(i, larger_unc); 
  }

  if (hist_name == "hT1CMET" || hist_name == "hT1CMET_tightID")
    cout << hMC[0]->Integral(41,80) << endl;
  //Comparison* c = new Comparison(c1, hData, hMC);
  Comparison* c = new Comparison(c1, hData, hMC, {hRatUnc});
  if (lumi == 14.02)
    c->give_info({"2018 Run A"});
  else if(lumi == 7.07)
      c->give_info({"2018 Run B"});
  else if (lumi == 6.90)
    c->give_info({"2018 Run C"});
  else if (lumi == 31.75)
      c->give_info({"2018 Run D"});
  c->set_filename(output_name);
  c->set_rat_label("Data/MC");
  //c->set_legend_labels({"2017 Data", "Drell-Yan", "DiBoson", "TriBoson", "Top"});
  c->set_legend_labels({"2018 Data", "Drell-Yan", "DiBoson", "Top"});
  c->set_x_label(x_label);
  c->set_y_label("Events");
  c->set_data_drawOpt("E");
  c->set_lumi(lumi);
  c->set_scale(scale);
  c->set_rat_unc_label({"JES Unc."});
  if (rebin)
    c->set_x_bin_range({1, n_met_bins});
  //if (hist_name.Contains("T1CMET"))
  //  c->set_x_bin_range({1,70});

  if (hist_name == "hT1CMET" || hist_name == "hT1CMET_tightID")
    cout << hMC[0]->Integral(41,80) << endl;
  for (int i = 0; i < vInfo.size(); i++)
    c->give_info(vInfo[i]);
  c->plot(idx);
  delete hData;
  for (int i = 0; i < hMC.size(); i++) {
    delete hMC[i];
    delete hMCUp[i];
    delete hMCDown[i];
  }

  delete hRatUnc;
  delete hRat;
  delete hMCSum;
  delete hMCSumUp;
  delete hMCSumDown;
  delete hRatUp;
  delete hRatDown;

  delete c;
}

void make_2Dplot_dataRat(TCanvas* c1, int histIdx, vector<TFile*> vFiles, string output_name, TString hist_name1, TString hist_name2, TString x_label, TString y_label, double lumi, double scale, vector<TString> vInfo, int idx, bool useMC = false) {
  TH2D* hData1;
  TH2D* hData2;
  if (!useMC) {
    hData1 = (TH2D*)vFiles[0]->Get(hist_name1 + "0");
    hData2 = (TH2D*)vFiles[0]->Get(hist_name2 + "0");
  }
  else {
    hData1 = (TH2D*)vFiles[1]->Get(hist_name1 + to_string(histIdx));
    hData2 = (TH2D*)vFiles[1]->Get(hist_name2 + to_string(histIdx));
  }


  Comparison* c = new Comparison(c1, hData1, hData2);
  c->set_filename(output_name);
  c->set_x_label(x_label);
  c->set_y_label(y_label);
  c->set_lumi(lumi);
  c->set_scale(scale);

  for (int i = 0; i < vInfo.size(); i++)
    c->give_info(vInfo[i]);
  c->set_rat_lim_range({0.86, 1.14});
  if (hist_name1 == "hJetEtaPhi_1Jet_qTGeq200V6_std" || hist_name1 == "hJetEtaPhi_1Jet_qTGeq200V27_std")
    c->set_rat_lim_range({0.8, 1.2});
  c->plot(idx);
  delete hData1;
  delete hData2;
  delete c;

}

void make_2Dplot(TCanvas* c1, int histIdx, vector<TFile*> vFiles, string output_name, TString hist_name, TString x_label, TString y_label, double lumi, double scale, vector<TString> vInfo, int idx) {
  TH2D* hData = (TH2D*)vFiles[0]->Get(hist_name + "0");
  vector<TH2D*> hMC;
  for (int i = 1; i < vFiles.size(); i++)
    hMC.push_back((TH2D*)vFiles[i]->Get(hist_name + to_string(histIdx)));
  TH2D* hMCSum;
  hMCSum = (TH2D*)hMC[0]->Clone("hMCSum");
  for (int i = 1; i < hMC.size(); i++)
    hMCSum->Add(hMC[i]);
  cout << hData->GetEntries() << endl;
  Comparison* c = new Comparison(c1, hData, hMCSum);
  c->set_filename(output_name);
  c->set_x_label(x_label);
  c->set_y_label(y_label);
  c->set_lumi(lumi);
  c->set_scale(scale);
  if (hist_name.Contains("Uncorr"))
    c->set_x_bin_range({3,50});


  //c->set_x_bin_range({1,40});
  //c->set_y_bin_range({1,40});

  for (int i = 0; i < vInfo.size(); i++)
    c->give_info(vInfo[i]);
  c->plot(idx);
  delete hData;
  for (int i = 0; i < hMC.size(); i++)
    delete hMC[i];
  delete hMCSum;
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
  c_response->plot((idx == 0 || idx == 3) ? 0 : 1);

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

void make_triple_plot(TCanvas* c1, int histIdx, vector<TFile*> vFiles, string output_name, TString hist_name1, TString hist_name2, TString hist_name3, TString label1, TString label2, TString label3, TString x_label, double lumi, double scale, vector<TString> vInfo, bool data, int idx) {
  TH1D* h1;
  TH1D* h2;
  TH1D* h3;
  if (data) {
    h1 = (TH1D*)vFiles[0]->Get(hist_name1 + "0");
    h2 = (TH1D*)vFiles[0]->Get(hist_name2 + "0");
    h3 = (TH1D*)vFiles[0]->Get(hist_name3 + "0");
  }
  else {
    h1 = (TH1D*)vFiles[1]->Get(hist_name1 + to_string(histIdx));
    h2 = (TH1D*)vFiles[1]->Get(hist_name2 + to_string(histIdx));
    h3 = (TH1D*)vFiles[1]->Get(hist_name3 + to_string(histIdx));
    for (int i = 2; i < vFiles.size(); i++) {
      h1->Add((TH1D*)vFiles[i]->Get(hist_name1 + to_string(histIdx)));
      h2->Add((TH1D*)vFiles[i]->Get(hist_name2 + to_string(histIdx)));
      h3->Add((TH1D*)vFiles[i]->Get(hist_name3 + to_string(histIdx)));
    }
  }
  vector<TH1D*> vH = {h1, h2};
  Comparison* c = new Comparison(c1, vH, {h3});
  if (lumi == 4.8)
    c->give_info({"2017 Run B"});
  else if (lumi == 23.5)
    c->give_info({"2017 Runs C,D,E"});
  else if (lumi == 13.5)
    c->give_info({"2017 Run F"});
  else if (lumi == 9.76)
    c->give_info({"2017 Run C"});
  else if (lumi == 13.74)
    c->give_info({"2017 Runs D,E"});
  c->set_filename(output_name);
  c->set_both_data();
  TString rat_label = "#frac{Modified MET}{Nominal MET}";
  //TString rat_label = "#frac{" + label1 + "}{" + label2 + "}";
  c->set_rat_label(rat_label);
  c->set_legend_labels({label1, label2, label3});
  if (data)
    c->give_info("Data vs. Data");
  else
    c->give_info("MC vs. MC");
  c->set_x_label(x_label);
  c->set_y_label("Events");
  c->set_lumi(lumi);
  c->set_scale(scale);
  c->set_x_bin_range({1,80});
  c->set_rat_lim_range({0.0, 2.0});
  c->plot(idx);

  delete h1;
  delete h2;
  delete h3;

  delete c;
}


void make_double_plot(TCanvas* c1, int histIdx, vector<TFile*> vFiles, string output_name, TString hist_name1, TString hist_name2, TString label1, TString label2, TString x_label, double lumi, double scale, vector<TString> vInfo, bool data, int idx) {
  TH1D* h1;
  TH1D* h2;
  if (data) {
    h1 = (TH1D*)vFiles[0]->Get(hist_name1 + "0");
    h2 = (TH1D*)vFiles[0]->Get(hist_name2 + "0");
  }  
  else {
    h1 = (TH1D*)vFiles[1]->Get(hist_name1 + to_string(histIdx)); 
    h2 = (TH1D*)vFiles[1]->Get(hist_name2 + to_string(histIdx));
    for (int i = 2; i < vFiles.size(); i++) {
      h1->Add((TH1D*)vFiles[i]->Get(hist_name1 + to_string(histIdx)));
      h2->Add((TH1D*)vFiles[i]->Get(hist_name2 + to_string(histIdx)));
    }
  }
  Comparison* c = new Comparison(c1, h1, h2);
  if (lumi == 4.8) 
    c->give_info({"2017 Run B"});
  else if (lumi == 23.5)
    c->give_info({"2017 Runs C,D,E"});
  else if (lumi == 13.5)
    c->give_info({"2017 Run F"}); 
  else if (lumi == 9.76)
    c->give_info({"2017 Run C"});
  else if (lumi == 13.74)
    c->give_info({"2017 Runs D,E"});
  c->set_filename(output_name);
  c->set_both_data();
  TString rat_label = "#frac{" + label1 + "}{" + label2 + "}";
  c->set_rat_label(rat_label);
  c->set_legend_labels({label1, label2});
  if (data)
    c->give_info("Data vs. Data");
  else
    c->give_info("MC vs. MC");

  for (int i = 0; i < vInfo.size(); i++)
    c->give_info(vInfo[i]);
  c->set_x_label(x_label);
  c->set_y_label("Events");
  c->set_lumi(lumi);
  c->set_scale(scale);
  //c->set_x_bin_range({1,80});
  c->set_rat_lim_range({0.8, 1.2});
  if (hist_name1 == "hJetPtV6_std")
    c->set_rat_lim_range({0.95, 1.05});
  c->plot(idx);

  delete h1;
  delete h2;

  delete c;
}

double get_scaleMC(vector<TFile*> vFiles, TString hist_name, int histIdx, double lumi) {
  TH1D* hData = (TH1D*)vFiles[0]->Get(hist_name + "0");
  TH1D* hMC;
  for (int i = 1; i < vFiles.size(); i++) {
    if (i == 1)
      hMC = (TH1D*)vFiles[i]->Get(hist_name + to_string(histIdx));
    else
      hMC->Add((TH1D*)vFiles[i]->Get(hist_name + to_string(histIdx)));
  }
  cout << "Data/MC: " << hData->Integral()/(hMC->Integral() * lumi) << endl;
  double scale = hData->Integral()/(hMC->Integral());
  delete hMC;
  delete hData;
  return scale; 
}

void make_met_plots(TCanvas* c1, int histIdx, vector<TFile*> vFiles, string output_name, TString name, double lumi, double scale, vector<TString> vInfo, int idx) {
  //make_plot(c1, histIdx, vFiles, output_name, "hT1CMET" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo, idx == 0 ? 0 : 1, true);
  make_plot_rat_unc(c1, histIdx, vFiles, output_name, "hT1CMET" + name, "hT1CMET_up" + name, "hT1CMET_down" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo, idx == 0 ? 0 : 1, true); 
  vector<TString> vInfo_up = vInfo;
  if (histIdx != 4) {
    vInfo_up.push_back("JEC Varied Up");
    make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_up" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo_up, 1, true);
    vector<TString> vInfo_down = vInfo;
    vInfo_down.push_back("JEC Varied Down");
    make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_down" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo_down, 1, true);
  }
  vector<TString> vInfo_ee = vInfo;
  vInfo_ee.push_back("Z #rightarrow ee Events");
  vector<TString> vInfo_mm = vInfo;
  vInfo_mm.push_back("Z #rightarrow #mu#mu Events");

  if (histIdx != 4) {
    make_plot_rat_unc(c1, histIdx, vFiles, output_name, "hT1CMET_EE" + name, "hT1CMET_EE_up" + name, "hT1CMET_EE_down" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo_ee, 1, true);
    make_plot_rat_unc(c1, histIdx, vFiles, output_name, "hT1CMET_MM" + name, "hT1CMET_MM_up" + name, "hT1CMET_MM_down" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo_mm, 1, true);
  }

/*  vector<TString> vInfo_0Jets = vInfo;
  vInfo_0Jets.push_back("0 Jets");
  //make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_0Jets" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo_0Jets, idx == 0 ? 0 : 1, true);
  make_plot_rat_unc(c1, histIdx, vFiles, output_name, "hT1CMET_0Jets" + name, "hT1CMET_0Jets_up" + name, "hT1CMET_0Jets_down" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo_0Jets, idx == 0 ? 0 : 1, true);
  vector<TString> vInfo_0Jets_up = vInfo_0Jets;
  if (histIdx != 4) {
    vInfo_0Jets_up.push_back("JEC Varied Up");
    //vInfo_0Jets_up.push_back("0 Jets");
    make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_0Jets_up" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo_0Jets_up, 1, true);
    vector<TString> vInfo_0Jets_down = vInfo_0Jets;
    vInfo_0Jets_down.push_back("JEC Varied Down");
    //vInfo_0Jets_down.push_back("0 Jets");
    make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_0Jets_down" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo_0Jets_down, 1, true); 
  }
  vector<TString> vInfo_1pJets = vInfo;
  vInfo_1pJets.push_back("#geq 1 Jets");
  make_plot_rat_unc(c1, histIdx, vFiles, output_name, "hT1CMET_1pJets" + name, "hT1CMET_1pJets_up" + name, "hT1CMET_1pJets_down" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo_1pJets, histIdx == 4 ? (idx == 2 ? 2 : 1) : idx == 0 ? 0 : 1, true);
  //make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_1pJets" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo_1pJets, idx == 0 ? 0 : 1, true);

  
  if (histIdx == 4) {
    make_plot(c1, histIdx, vFiles, output_name, "hJetPt" + name, "Jet p_{T} [GeV]", lumi, scale, vInfo, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_UPara_over_high_qT" + name, "u_{#parallel} [GeV]", lumi, scale, vInfo, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hLeadJetPt" + name, "Lead Jet p_{T} [GeV]", lumi, scale, vInfo, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hJetMultiplicity" + name, "Jet Multiplicity", lumi, scale, vInfo, 1);
  }*/
  

  /*
  make_plot_rat_unc(c1, histIdx, vFiles, output_name, "hJetEta_30GeV" + name, "hJetEta_30GeV" + name, "hJetEta_30GeV" + name, "Jet |#eta|", lumi, scale, {"Jet p_{T} #geq 30 GeV"}, 1, false);
  make_plot_rat_unc(c1, histIdx, vFiles, output_name, "hJetEta_50GeV" + name, "hJetEta_50GeV" + name, "hJetEta_50GeV" + name, "Jet |#eta|", lumi, scale, {"Jet p_{T} #geq 50 GeV"}, 1, false);
  make_plot_rat_unc(c1, histIdx, vFiles, output_name, "hJetEta_100GeV" + name, "hJetEta_100GeV" + name, "hJetEta_100GeV" + name, "Jet |#eta|", lumi, scale, {"Jet p_{T} #geq 100 GeV"}, 1, false);

  make_plot_rat_unc(c1, histIdx, vFiles, output_name, "hZMassEE" + name, "hZMassEE" + name, "hZMassEE" + name, "m_{ll} [GeV]", lumi, scale, {"Z #to ee"}, 1, false);
  make_plot_rat_unc(c1, histIdx, vFiles, output_name, "hZMassMuMu" + name, "hZMassMuMu" + name, "hZMassMuMu" + name, "m_{ll} [GeV]", lumi, scale, {"Z #to #mu#mu"}, 1, false);
  make_plot_rat_unc(c1, histIdx, vFiles, output_name, "hZMassEE_high_qT" + name, "hZMassEE_high_qT" + name, "hZMassEE_high_qT" + name, "m_{ll} [GeV]", lumi, scale, {"Z #to ee", "q_{T} #geq 300 GeV"}, 1, false);
  make_plot_rat_unc(c1, histIdx, vFiles, output_name, "hZMassMuMu_high_qT" + name, "hZMassMuMu_high_qT" + name, "hZMassMuMu_high_qT" + name, "m_{ll} [GeV]", lumi, scale, {"Z #to #mu#mu", "q_{T} #geq 300 GeV"}, histIdx == 4 ? (idx == 2 ? 2 : 1) : idx == 0 ? 0 : 1, false);
  */

//  vector<TString> vInfo_1pJets_up = vInfo_1pJets;
  if (histIdx != 4) {
  //  vInfo_1pJets_up.push_back("JEC Varied Up");
   // vInfo_1pJets_up.push_back("#geq 1 Jets");
    //make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_1pJets_up" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo_1pJets_up, 1, true);
    //vector<TString> vInfo_1pJets_down = vInfo_1pJets;
    //vInfo_1pJets_down.push_back("JEC Varied Down");
    //vInfo_1pJets_down.push_back("#geq 1 Jets");
    //make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_1pJets_down" + name, "E_{T}^{miss} [GeV]", lumi, scale, vInfo_1pJets_down, 1, true);

    make_plot(c1, histIdx, vFiles, output_name, "hJetPt" + name, "Jet p_{T} [GeV]", lumi, scale, vInfo, 1);

    make_plot(c1, histIdx, vFiles, output_name, "hZpT" + name, "q_{T} [GeV]", lumi, scale, vInfo, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_UPara" + name, "u_{#parallel} [GeV]", lumi, scale, vInfo, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_UPerp" + name, "u_{#perp}  [GeV]", lumi, scale, vInfo, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hRaw_UPara","Raw u_{#parallel} [GeV]",lumi,scale,{"No JECs"},1);
    make_plot(c1,histIdx,  vFiles,output_name,  "hRaw_UPerp","Raw u_{#perp}  [GeV]",lumi,scale,{"No JECs"},1);

    make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_UPara_over_high_qT" + name, "u_{#parallel} [GeV]", lumi, scale, vInfo, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hLeadJetPt" + name, "Lead Jet p_{T} [GeV]", lumi, scale, vInfo, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hJetMultiplicity" + name, "Jet Multiplicity", lumi, scale, vInfo, 1);

    //make_2Dplot(c1, histIdx, vFiles, output_name, "hJetEtaPhi" + name, "Jet #eta", "Jet #phi", lumi, -1, {}, 1);

    make_plot(c1, histIdx, vFiles, output_name, "hT1CMET_UParaPlusqT" + name, "u_{#parallel} + q_{T} [GeV]", lumi, scale, vInfo, 1);
  }
} 


int main(int argc, char* argv[])
{
  // Style options
  gStyle->SetOptStat(0);

//  gStyle->SetPalette(kRainBow,0);
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
  string output_name = "plots_" + eras_s + ".pdf";
  string output_res = "plots_res" + eras_s + ".pdf";

  int histIdx=0;
/*  if (eras == "A")  histIdx = 0;
  else if (eras == "B") 	histIdx = 1;
  else if (eras == "C")	histIdx = 2;
  else if (eras == "D")	histIdx = 3;
  else if (eras == "All" || eras == "All_old") 	histIdx = 4;*/
  cout<<histIdx<<endl;
  TString era_info;
  if (eras == "A")  era_info = "2018 Run A";
  if (eras == "B") 	era_info = "2018 Run B";
  if (eras == "C")      era_info = "2018 Run C";
  if (eras == "D,E")      era_info = "2018 Runs D";
  if (eras == "All" || eras == "All_old")	era_info = "2018 All Runs";

  TString eras_trimmed = eras.Contains("nominal") ? "Fv2" : eras;
  

  TFile* fData;
  if (eras == "All")
    fData = new TFile("../histograms/Zll_histograms_All.root");
  else if (eras == "All_old") {
    fData = new TFile("../histograms/Zll_histograms_All_v2.root");
  }
  else
    fData = new TFile("../histograms/Zll_histograms_" + eras_trimmed + ".root");
  TFile* fDY;
  fDY = new TFile("../histograms/Zll_histograms_Drell-Yan.root");
  TFile* fDiBoson = new TFile("../histograms/Zll_histograms_DiBoson.root");
//  TFile* fTriBoson = new TFile("../histograms/Zll_histograms_TriBoson.root");
  TFile* fTop = new TFile("../histograms/Zll_histograms_top.root");
  vector<TFile*> vFiles = {fData, fDY, fDiBoson,fTop};

  TCanvas* c1 = new TCanvas("c1", "histos", 600, 800);

  double scaleMC = lumi;

  if (eras == "All" || eras == "All_old") {
    make_met_plots(c1, histIdx, vFiles, output_name, "V8", lumi, 1, {"V8 JECs"}, 0);
    make_plot(c1, histIdx, vFiles, output_name, "hZMassEEV8", "m_{ll} [GeV]", lumi, 1, {"Z #rightarrow ee"}, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hZMassMuMuV8", "m_{ll} [GeV]", lumi, 1, {"Z #rightarrow #mu#mu"}, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hZMassEE_high_qTV8", "m_{ll} [GeV]", lumi, 1, {"Z #rightarrow ee", "q_{T} #geq 300 GeV"}, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hZMassMuMu_high_qTV8", "m_{ll} [GeV]", lumi, 1, {"Z #rightarrow #mu#mu", "q_{T} #geq 300 GeV"}, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hJetEta_30GeVV8", "Jet |#eta|", lumi, 1, {"V8 JECs", "Jet p_{T} #geq 30 GeV"}, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hJetEta_50GeVV8", "Jet |#eta|", lumi, 1, {"V8 JECs", "Jet p_{T} #geq 50 GeV"}, 1);
    make_plot(c1, histIdx, vFiles, output_name, "hJetEta_100GeVV8_std", "Jet |#eta|", lumi, 1, {"V8 JECs", "Jet p_{T} #geq 100 GeV"}, 2);
    //make_plot(c1, histIdx, vFiles, output_name, "hCCeta", "|#eta|", lumi, -1, {"Charged Candidates"}, 2);
    //make_met_plots(c1, histIdx, vFiles, output_name, "V32_v2C_50GeV", lumi, 1, {"V32 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 1);
    //make_resolution_plots(c1, histIdx, vFiles, output_name, {"V32_std", "V32_v2C_50GeV"}, lumi, 1, {era_info}, {"Nominal (V32)", "Modified (V32)"}, 2, true, false);
  }

  else {
    double scaleMC = get_scaleMC(vFiles, "hDilepMass", histIdx, lumi);

    make_met_plots(c1, histIdx, vFiles, output_name, "V8", lumi, scaleMC, {eras == "F" ? "09May2018 V8 JECs" : "V8 JECs"}, 0);
    make_plot(c1,histIdx,vFiles,output_name,"hRawMET","Raw MET",lumi,scaleMC,{"No JECs"},2,true);

 //   make_plot(c1,histIdx,vFiles,output_name,"hRaw_ResponseMM","Raw MET",lumi,1,{"Raw MEt in Z #rightarrow #mu#mu"},2);

//    make_plot(c1, histIdx, vFiles, output_name, "hJetEta_100GeVV8", "Jet |#eta|", lumi, 1, {"V8 JECs", "Jet p_{T} #geq 100 GeV"}, 2);
    //make_plot(c1, histIdx, vFiles, output_name, "hCCeta", "|#eta|", lumi, -1, {"Charged Candidates"}, 2);
    //make_met_plots(c1, histIdx, vFiles, output_name, "V32_v2C_50GeV", lumi, scaleMC, {eras == "F" ? "09May2018 V2 JECs" : "V32 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 1);
    //make_resolution_plots(c1, histIdx, vFiles, output_name, {"V32_std", "V32_v2C_50GeV"}, lumi, scaleMC, {era_info}, {eras == "F" ? "Nominal (09May2018 V2)" : "Nominal (V32)", eras == "F" ? "Modified (09May2018 V2)" : "Modified (V32)"}, 2);
  }

  // 14 Dec 2018
  /*
  if (eras == "All") {
    make_2Dplot_dataRat(c1, histIdx, vFiles, output_name, "hJetEtaPhiV6_std", "hJetEtaPhiV27_std", "Jet #eta", "Jet #phi", lumi, 1, {}, 0);
    make_2Dplot_dataRat(c1, histIdx, vFiles, output_name, "hJetEtaPhi_1Jet_qTGeq200V6_std", "hJetEtaPhi_1Jet_qTGeq200V27_std", "Jet #eta", "Jet #phi", lumi, 1, {}, 1);
    make_2Dplot_dataRat(c1, histIdx, vFiles, output_name, "hJetEtaPhi_1HighPtJet_qTGeq200V6_std", "hJetEtaPhi_1HighPtJet_qTGeq200V27_std", "Jet #eta", "Jet #phi", lumi, 1, {}, 1);

    make_2Dplot_dataRat(c1, 2, vFiles, output_name, "hJetEtaPhiV6_std", "hJetEtaPhiV27_std", "Jet #eta", "Jet #phi", lumi, 1, {}, 1, true);
    make_2Dplot_dataRat(c1, 2, vFiles, output_name, "hJetEtaPhi_1Jet_qTGeq200V6_std", "hJetEtaPhi_1Jet_qTGeq200V27_std", "Jet #eta", "Jet #phi", lumi, 1, {}, 1, true);
    make_2Dplot_dataRat(c1, 2, vFiles, output_name, "hJetEtaPhi_1HighPtJet_qTGeq200V6_std", "hJetEtaPhi_1HighPtJet_qTGeq200V27_std", "Jet #eta", "Jet #phi", lumi, 1, {}, 1, true);

    make_double_plot(c1, histIdx, vFiles, output_name, "hJetPtV6_std", "hJetPtV27_std", "V6 JECs", "V27 JECs", "Jet p_{T} [GeV]", lumi, 1, {}, true, 1);
    make_double_plot(c1, histIdx, vFiles, output_name, "hJetPt_BarrelV6_std", "hJetPt_BarrelV27_std", "V6 JECs", "V27 JECs", "Jet p_{T} [GeV]", lumi, 1, {"Jet |#eta| < 1.6"}, true, 1);
    make_double_plot(c1, histIdx, vFiles, output_name, "hJetPt_ECV6_std", "hJetPt_ECV27_std", "V6 JECs", "V27 JECs", "Jet p_{T} [GeV]", lumi, 1, {"1.6 < Jet |#eta| < 3.0"}, true, 1);
    make_double_plot(c1, histIdx, vFiles, output_name, "hJetPt_ForwardV6_std", "hJetPt_ForwardV27_std", "V6 JECs", "V27 JECs", "Jet p_{T} [GeV]", lumi, 1, {"Jet |#eta| > 3.0"}, true, 1);

    make_double_plot(c1, 2, vFiles, output_name, "hJetPtV6_std", "hJetPtV27_std", "V6 JECs", "V27 JECs", "Jet p_{T} [GeV]", lumi, 1, {}, false, 1);
    make_double_plot(c1, 2, vFiles, output_name, "hJetPt_BarrelV6_std", "hJetPt_BarrelV27_std", "V6 JECs", "V27 JECs", "Jet p_{T} [GeV]", lumi, 1, {"Jet |#eta| < 1.6"}, false, 1);
    make_double_plot(c1, 2, vFiles, output_name, "hJetPt_ECV6_std", "hJetPt_ECV27_std", "V6 JECs", "V27 JECs", "Jet p_{T} [GeV]", lumi, 1, {"1.6 < Jet |#eta| < 3.0"}, false, 1);
    make_double_plot(c1, 2, vFiles, output_name, "hJetPt_ForwardV6_std", "hJetPt_ForwardV27_std", "V6 JECs", "V27 JECs", "Jet p_{T} [GeV]", lumi, 1, {"Jet |#eta| > 3.0"}, false, 1);


    make_double_plot(c1, histIdx, vFiles, output_name, "hJetMultiplicityV6_std", "hJetMultiplicityV27_std", "V6 JECs", "V27 JECs", "Jet Multiplicity", lumi, 1, {}, true, 1);
    make_double_plot(c1, histIdx, vFiles, output_name, "hT1CMET_UParaV6_std", "hT1CMET_UParaV27_std", "V6 JECs", "V27 JECs", "u_{#parallel} [GeV]", lumi, 1, {}, true, 1);
    make_double_plot(c1, histIdx, vFiles, output_name, "hT1CMET_UPara_over_high_qTV6_std", "hT1CMET_UPara_over_high_qTV27_std", "V6 JECs", "V27 JECs", "-u_{#parallel}/q_{T}", lumi, 1, {"q_{T} > 300 GeV"}, true, 1);

     
    make_met_plots(c1, histIdx, vFiles, output_name, "V6_std", lumi, 1, {"V6 JECs"}, 1);
    make_met_plots(c1, histIdx, vFiles, output_name, "V6_v2C_50GeV", lumi, 1, {"V6 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 1);
    make_met_plots(c1, histIdx, vFiles, output_name, "V27_std", lumi, 1, {"V27 JECs"}, 1);
    make_met_plots(c1, histIdx, vFiles, output_name, "V27_v2C_50GeV", lumi, 1, {"V27 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 1);

    make_resolution_plots(c1, histIdx, vFiles, output_name, {"V6_std", "V6_v2C_50GeV", "V27_std", "V27_v2C_50GeV"}, lumi, 1, {era_info}, {"Nominal (V6)", "Modified (V6)", "Nominal (V27)", "Modified (V27)"}, 2, true, true);

    //make_met_plots(c1, histIdx, vFiles, output_name, "V11_std", lumi, 1, {"V11 JECs"}, 0);
    //make_met_plots(c1, histIdx, vFiles, output_name, "V11_v1", lumi, 1, {"V11 JECs", "Modified v1 Type-1 MET"}, 1);
    //make_met_plots(c1, histIdx, vFiles, output_name, "V11_v2C", lumi, 1, {"V11 JECs", "Modified v2 Type-1 MET"}, 1);
    //make_met_plots(c1, histIdx, vFiles, output_name, "V11_v2C_50GeV", lumi, 1, {"V11 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 2);
    //make_met_plots(c1, histIdx, vFiles, output_name, "V11_v2C_50GeV", lumi, 1, {"V11 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 0);
    //make_met_plots(c1, histIdx, vFiles, output_name, "V6_v2C_50GeV", lumi, 1, {"V6 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 1);
    //make_met_plots(c1, histIdx, vFiles, output_name, "V25_v2C_50GeV", lumi, 1, {"V25 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 1);
    //make_met_plots(c1, histIdx, vFiles, output_name, "V27_v2C_50GeV", lumi, 1, {"V27 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 2);
    return 0;
  }

  //double scaleMC = get_scaleMC(vFiles, "hDilepMass", histIdx, lumi);
  double scaleMC = lumi; 

  //make_met_plots(c1, histIdx, vFiles, output_name, "V8_std", lumi, scaleMC, {"V8 JECs"}, 0);
  //make_met_plots(c1, histIdx, vFiles, output_name, "V8_v1", lumi, scaleMC, {"V8 JECs", "Modified v1 Type-1 MET"}, 1);  
  //make_resolution_plots(c1, histIdx, vFiles, output_name, {"V8_std", "V8_v1"}, lumi, scaleMC, {era_info, "V8 JECs"}, {"Std. Type-1 MET", "Mod. v1 Type-1 MET"}, 1);


  make_met_plots(c1, histIdx, vFiles, output_name, "V6_std", lumi, scaleMC, {"V6 JECs"}, 0);
  make_met_plots(c1, histIdx, vFiles, output_name, "V6_v2C_50GeV", lumi, scaleMC, {"V6 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 1);
  make_met_plots(c1, histIdx, vFiles, output_name, "V27_std", lumi, scaleMC, {"V27 JECs"}, 1);
  make_met_plots(c1, histIdx, vFiles, output_name, "V27_v2C_50GeV", lumi, scaleMC, {"V27 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 1);
  make_resolution_plots(c1, histIdx, vFiles, output_name, {"V6_std", "V6_v2C_50GeV", "V27_std", "V27_v2C_50GeV"}, lumi, scaleMC, {era_info}, {"Nominal (V6)", "Modified (V6)", "Nominal (V27)", "Modified (V27)"}, 2);
  */
  // 14 Oct 2018
  /*
  if (eras == "F_V6") {
    make_resolution_plots(c1, histIdx, vFiles, output_name, {"V6_v2C_50GeV", "V24_v2C_50GeV", "V25_v2C_50GeV", "V26_v2C_50GeV"}, lumi, scaleMC, {era_info}, {"V6 JECs", "V24 JECs", "V25 JECs", "V26 JECs"}, 3, true);
    //make_double_plot(c1, histIdx, vFiles, output_name, "hT1CMETV6_v2C", "hT1CMETV11_v2C", "V6 JECs", "V11 JECs", "E_{T}^{miss} [GeV]", lumi, -1, {"MC vs. MC"}, false, 0);
    //make_double_plot(c1, histIdx, vFiles, output_name, "hT1CMETV6_v2C", "hT1CMETV11_v2C", "V6 JECs", "V11 JECs", "E_{T}^{miss} [GeV]", lumi, -1, {"Data vs. Data"}, true, 1);
    return 0;
  }


  make_met_plots(c1, histIdx, vFiles, output_name, "V11_v2C_50GeV", lumi, scaleMC, {"V11 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 0);
  make_met_plots(c1, histIdx, vFiles, output_name, "V6_v2C_50GeV", lumi, scaleMC, {"V6 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 1);
  //make_met_plots(c1, histIdx, vFiles, output_name, "V25_v2C_50GeV", lumi, scaleMC, {"V25 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 1);
  make_met_plots(c1, histIdx, vFiles, output_name, "V27_v2C_50GeV", lumi, scaleMC, {"V27 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 1);

  make_resolution_plots(c1, histIdx, vFiles, output_name, {"V6_v2C_50GeV", "V11_v2C_50GeV", "V27_v2C_50GeV"}, lumi, scaleMC, {era_info}, {"V6 JECs", "V11 JECs", "V27 JECs"}, 1);
  make_resolution_plots(c1, histIdx, vFiles, output_name, {"V6_v2C_50GeV", "V11_v2C_50GeV", "V27_v2C_50GeV"}, lumi, scaleMC, {era_info}, {"V6 JECs", "V11 JECs", "V27 JECs"}, 2, true);
  */
  // 2 Oct 2018

  /*
  make_met_plots(c1, histIdx, vFiles, output_name, "V11_std", lumi, scaleMC, {"V11 JECs"}, 0);
  make_met_plots(c1, histIdx, vFiles, output_name, "V11_v1", lumi, scaleMC, {"V11 JECs", "Modified v1 Type-1 MET"}, 1);
  make_met_plots(c1, histIdx, vFiles, output_name, "V11_v2C", lumi, scaleMC, {"V11 JECs", "Modified v2 Type-1 MET"}, 1);
  make_met_plots(c1, histIdx, vFiles, output_name, "V11_v2C_50GeV", lumi, scaleMC, {"V11 JECs", "Modified v2 Type-1 MET", "50 GeV Jet Threshold"}, 1);
  //make_met_plots(c1, histIdx, vFiles, output_name, "V11_v2D", lumi, scaleMC, {"V11D JECs", "Modified v2D Type-1 MET"}, 1);
 
  //make_resolution_plots(c1, histIdx, vFiles, output_name, {"V11_std", "V11_v1", "V11_v2C", "V11_v2D"}, lumi, scaleMC, {era_info, "V11 JECs"}, {"Std. Type-1 MET", "Mod. v1 Type-1 MET", "Mod. v2C Type-1 MET", "Mod. v2D Type-1 MET"}, 1);
  //make_resolution_plots(c1, histIdx, vFiles, output_name, {"V11_std", "V11_v1"}, lumi, scaleMC, {era_info, "V11 JECs"}, {"Std. Type-1 MET", "Mod. v1 Type-1 MET"}, 1);
  //make_resolution_plots(c1, histIdx, vFiles, output_name, {"V11_v2C", "V11_v2C_recipe"}, lumi, scaleMC, {era_info, "V11 JECs"}, {"My Implementation", "Official Recipe"}, 1);
  make_resolution_plots(c1, histIdx, vFiles, output_name, {"V11_std", "V11_v1", "V11_v2C", "V11_v2C_50GeV"}, lumi, scaleMC, {era_info, "V11 JECs"}, {"Nominal T1 E_{T}^{miss}", "Mod. v1 T1 E_{T}^{miss}", "Mod. v2 T1 E_{T}^{miss}", "v2 w/50 GeV Thresh."}, 1);

  //make_plot(c1, histIdx, vFiles, output_name, "hJet_Neutral_Emfrac_central", "Jet Neutral EM Fraction", lumi, -1, {"|#eta| <= 2.7"}, 1);
  //make_plot(c1, histIdx, vFiles, output_name, "hJet_Neutral_Hadfrac_central", "Jet Neutral Hadronic Fraction", lumi, -1, {"|#eta| <= 2.7"}, 1);
  //make_plot(c1, histIdx, vFiles, output_name, "hJet_Neutral_Emfrac_forward", "Jet Neutral EM Fraction", lumi, -1, {"2.7 < |#eta| <= 3.0"}, 1);
  //make_plot(c1, histIdx, vFiles, output_name, "hJet_Neutral_Hadfrac_forward", "Jet Neutral Hadronic Fraction", lumi, -1, {"2.7 < |#eta| <= 3.0"}, 1);

  //make_plot(c1, histIdx, vFiles, output_name, "hCCpT", "p_{T} [GeV]", lumi, -1, {"Charged Candidates"}, 1);
  //make_plot(c1, histIdx, vFiles, output_name, "hPhotonpT", "p_{T} [GeV]", lumi, -1, {"Photon Candidates"}, 1);
  //make_plot(c1, histIdx, vFiles, output_name, "hNeutpT", "p_{T} [GeV]", lumi, -1, {"Neutral Hadron Candidates"}, 1);
  //make_plot(c1, histIdx, vFiles, output_name, "hCCeta", "|#eta|", lumi, -1, {"Charged Candidates"}, 1);
  //make_plot(c1, histIdx, vFiles, output_name, "hPhotoneta", "|#eta|", lumi, -1, {"Photon Candidates"}, 1, false, {0.1, 33.3});
  //make_plot(c1, histIdx, vFiles, output_name, "hNeuteta", "|#eta|", lumi, -1, {"Neutral Hadron Candidates"}, 1);

  //make_plot_rat_unc(TCanvas* c1, int histIdx, vector<TFile*> vFiles, string output_name, TString hist_name, TString hist_name_up, TString hist_name_down, TString x_label, double lumi, double scale, vector<TString> vInfo, int idx, bool rebin = false)

  make_plot(c1, histIdx, vFiles, output_name, "hpfMETraw", "E_{T}^{miss} [GeV]", lumi, scaleMC, {"Nominal Raw E_{T}^{miss}"}, 1, true);
  make_plot(c1, histIdx, vFiles, output_name, "hpfModMETraw", "E_{T}^{miss} [GeV]", lumi, scaleMC, {"Mod. v2 Raw E_{T}^{miss}"}, 1, true);

  make_2Dplot(c1, histIdx, vFiles, output_name, "hT1CMET_Modv1_vs_Nominal", "Nominal T1 E_{T}^{miss} [GeV]", "Mod. v1 T1 E_{T}^{miss}", lumi, -1, {}, 1);
  make_2Dplot(c1, histIdx, vFiles, output_name, "hT1CMET_Modv2_vs_Nominal", "Nominal T1 E_{T}^{miss} [GeV]", "Mod. v2 T1 E_{T}^{miss}", lumi, -1, {}, 1);
  make_2Dplot(c1, histIdx, vFiles, output_name, "hRawCMET_Modv2_vs_Nominal", "Nominal Raw E_{T}^{miss} [GeV]", "Mod. v2 Raw E_{T}^{miss}", lumi, -1, {}, 1);

  // make_double_plot(TCanvas* c1, int histIdx, vector<TFile*> vFiles, string output_name, TString hist_name1, TString hist_name2, TString label1, TString label2, TString x_label, double lumi, double scale, vector<TString> vInfo, bool data, int idx) 
  make_double_plot(c1, histIdx, vFiles, output_name, "hpfMETraw", "hpfModMETraw", "Nominal Raw E_{T}^{miss}", "Mod. v2 Raw E_{T}^{miss}", "E_{T}^{miss} [GeV]", lumi, -1, {"Data vs. Data"}, true, 1);
  make_double_plot(c1, histIdx, vFiles, output_name, "hpfMETraw", "hpfModMETraw", "Nominal Raw E_{T}^{miss}", "Mod. v2 Raw E_{T}^{miss}", "E_{T}^{miss} [GeV]", lumi, -1, {"MC vs. MC"}, false, 1);
  
  make_triple_plot(c1, histIdx, vFiles, output_name, "hT1CMETV11_v1", "hT1CMETV11_v2C", "hT1CMETV11_std", "Mod. v1 E_{T}^{miss}", "Mod. v2 E_{T}^{miss}", "Nominal E_{T}^{miss}", "E_{T}^{miss} [GeV]", lumi, -1, {"Data vs. Data"}, true, 1);
  make_triple_plot(c1, histIdx, vFiles, output_name, "hT1CMETV11_v1", "hT1CMETV11_v2C", "hT1CMETV11_std", "Mod. v1 E_{T}^{miss}", "Mod. v2 E_{T}^{miss}", "Nominal E_{T}^{miss}", "E_{T}^{miss} [GeV]", lumi, -1, {"MC vs. MC"}, false, 1);

  make_double_plot(c1, histIdx, vFiles, output_name, "hT1CMETV11_v2C", "hT1CMETV11_v2C_recipe", "My Implementation", "Official Recipe", "E_{T}^{miss} [GeV]", lumi, -1, {"Data vs. Data"}, true, 1);
  make_double_plot(c1, histIdx, vFiles, output_name, "hT1CMETV11_v2C", "hT1CMETV11_v2C_recipe", "My Implementation", "Official Recipe", "E_{T}^{miss} [GeV]", lumi, -1, {"MC vs. MC"}, false, 1);

  make_plot(c1, histIdx, vFiles, output_name, "hpfMETraw_0jets", "E_{T}^{miss} [GeV]", lumi, scaleMC, {"Nominal Raw E_{T}^{miss}", "0 Jets"}, 1, true);
  make_plot(c1, histIdx, vFiles, output_name, "hpfModMETraw_0jets", "E_{T}^{miss} [GeV]", lumi, scaleMC, {"Mod. v2 Raw E_{T}^{miss}", "0 Jets"}, 1, true);
  make_plot(c1, histIdx, vFiles, output_name, "hpfMETraw_1pjets", "E_{T}^{miss} [GeV]", lumi, scaleMC, {"Nominal Raw E_{T}^{miss}", "#geq 1 Jets"}, 1, true);
  make_plot(c1, histIdx, vFiles, output_name, "hpfModMETraw_1pjets", "E_{T}^{miss} [GeV]", lumi, scaleMC, {"Mod. v2 Raw E_{T}^{miss}", "#geq 1 Jets"}, 1, true);

  make_triple_plot(c1, histIdx, vFiles, output_name, "hT1CMET_upV11_v2C", "hT1CMET_downV11_v2C", "hT1CMETV11_v2C", "JES Varied Up", "JES Varied Down", "Nominal", "E_{T}^{miss} [GeV]", lumi, -1, {"MC vs. MC"}, false, 1);
  make_triple_plot(c1, histIdx, vFiles, output_name, "hT1CMET_upV11_v2C", "hT1CMET_downV11_v2C", "hT1CMETV11_v2C", "JES Varied Up", "JES Varied Down", "Nominal", "E_{T}^{miss} [GeV]", lumi, -1, {"Data vs. Data"}, true, 2);

  make_resolution_plots(c1, histIdx, vFiles, output_res, {"V11_std", "V11_v2C_50GeV"}, lumi, scaleMC, {era_info, "V11 JECs"}, {"Nominal E_{T}^{miss}", "Mod. v2 E_{T}^{miss}"}, 3);

  //make_plot(c1, histIdx, vFiles, output_name, "h_eta5_cand3MET", "Raw E_{T}^{miss} [GeV]", lumi, scaleMC, {}, 1, true);
  //make_2Dplot(c1, histIdx, vFiles, output_name, "hJetUncorrPtEta", "(Uncorrected) Jet p_{T} [GeV]", "Jet #eta", lumi, -1, {}, 1);

  //make_2Dplot(c1, histIdx, vFiles, output_name, "hT1CMETModvT1CMETModCorr", "Modified E_{T}^{miss} (Uncorr. Thresh.) [GeV]", "Modified E_{T}^{miss} (Corr. Thresh.) [GeV]", lumi, -1, {}, 1);
  //make_met_plots(c1, histIdx, vFiles, output_name, "V11_v2C_recipe", lumi, scaleMC, {"V11 JECs", "Modified Type-1 MET", "Official MET Recipe"}, 2); 
  */
}
