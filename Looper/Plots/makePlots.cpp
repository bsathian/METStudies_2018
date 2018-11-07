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

  make_plot(c1, histIdx, vFiles, output_name, "hNVtx", "# Vertices", lumi, scaleMC, {""}, 2);

  get_normalization(vFiles, lumi, histIdx);

}
