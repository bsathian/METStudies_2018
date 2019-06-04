#include "PlotHelper.h"

void make_comparison(TCanvas* c1, int histIdx, TFile* file, string output_name, TString hist_name1, TString hist_name2, TString x_label, double lumi, double scale, vector<TString> vInfo, int idx) {
  TH1D* h1 = (TH1D*)file->Get(hist_name1 + "0");
  TH1D* h2 = (TH1D*)file->Get(hist_name2 + "0");

  h1->Rebin(2);
  h2->Rebin(2);

  Comparison* c = new Comparison(c1, h1, h2);
  c->set_filename(output_name);
  c->set_both_data();
  c->set_legend_labels({"Official Recipe", "My Implementation"});
  c->set_rat_label("#frac{Official Recipe}{My Implementation}");
  for (int i = 0; i < vInfo.size(); i++)
    c->give_info(vInfo[i]);
  c->give_info("2017 Run F");
  c->set_x_label(x_label);
  c->set_y_label("Events");
  c->set_lumi(lumi);
  c->set_scale(scale);
  //c->set_x_bin_range({1,80});
  c->plot(idx);

  delete h1;
  delete h2;
  delete c;
}

void make_comparison(TCanvas* c1, int histIdx, TFile* file, string output_name, TString hist_name1, TString hist_name2, TFile* file2, TString hist_name3, TString x_label, double lumi, double scale, vector<TString> vInfo, int idx) {
  TH1D* h1 = (TH1D*)file->Get(hist_name1 + "0");
  TH1D* h2 = (TH1D*)file->Get(hist_name2 + "0");
  TH1D* h3 = (TH1D*)file2->Get(hist_name3 + "0");

  vector<TH1D*> vH = {h1, h2};

  Comparison* c = new Comparison(c1, vH, {h3});
  c->set_filename(output_name);
  c->set_both_data();
  c->set_legend_labels({"Official Recipe", "My Implementation", "Nominal MET"});
  c->set_rat_label("#frac{Modified MET}{Nominal MET}");
  for (int i = 0; i < vInfo.size(); i++)
    c->give_info(vInfo[i]);
  c->give_info("2017 Run F");
  c->set_x_label(x_label);
  c->set_y_label("Events");
  c->set_lumi(lumi);
  c->set_scale(scale);
  //c->set_x_bin_range({1,80});
  c->plot(idx);

  delete h1;
  delete h2;
  delete c;
}


void make_2D_comparison(TCanvas* c1, TFile* file1, TFile* file2, string output_name, TString hist_name, TString x_label, TString y_label, double lumi, double scale, vector<TString> vInfo, int idx) {
  TH2D* hData1 = (TH2D*)file1->Get(hist_name + "0");
  TH2D* hData2 = (TH2D*)file2->Get(hist_name + "0");
  Comparison* c = new Comparison(c1, hData1, hData2);
  c->set_filename(output_name);
  c->set_x_label(x_label);
  c->set_y_label(y_label);
  c->set_x_bin_range({3,50});
  c->set_lumi(lumi);
  c->set_scale(scale);
  for (int i = 0; i < vInfo.size(); i++)
    c->give_info(vInfo[i]);
  c->plot(idx);
  delete hData1;
  delete hData2;
  delete c;
}


int main(int argc, char* argv[])
{
  // Style options
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow,0);
  gStyle->SetPaintTextFormat(".2f");
  gStyle->SetTickLength(0.01);
  gStyle->SetErrorX(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetTitleAlign(33);
  gStyle->SetTitleX(.93);
  gStyle->SetTitleFontSize(0.03);

  int histIdx = 3; // run F
  TFile* file = new TFile("../histograms/Zll_histograms_F_withRecipeV11.root");
  TFile* file2 = new TFile("../histograms/Zll_histograms_F.root");
  //TFile* file = new TFile("/hadoop/cms/store/user/smay/MET/DoubleMuon_Run2017F-17Nov2017-v1_MINIAOD_CMS4_V09-04-13_949_allPfCands__MET_v12/Zll_histograms_132.root");
  TCanvas* c1 = new TCanvas("c1", "histos", 600, 800);
  
  double lumi = 13.5;
  string output_name = "validation.pdf";

  //make_comparison(c1, histIdx, file, output_name, "hpfMET_recipe", "hpfMET_myImplementation", "E_{T}^{miss} [GeV]", lumi, -1, {"Type-1 Corr. Met"}, 0);
  make_comparison(c1, histIdx, file, output_name, "hpfModifiedMET_recipe", "hpfModifiedMET_myImplementation", file2, "hT1CMETV11_std", "E_{T}^{miss} [GeV]", lumi, -1, {"Modified Type-1 Corr. Met"}, 1); 

  return 0;
}
