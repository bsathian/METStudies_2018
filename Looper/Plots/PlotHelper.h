#ifndef _PLOTHELPER_H_
#define _PLOTHELPER_H_

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>

// ROOT
#include "TString.h"
#include "TBenchmark.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTreeCache.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TLine.h"
#include "TPad.h"
#include "TLatex.h"
#include "TFrame.h"

using namespace std;

const vector<int> dataColors = {kBlack, kRed, kGreen, kBlue, kGray};

class Comparison 
{
  public:
    Comparison(TCanvas* c1, TH1D* hData, TH1D* hMC);
    Comparison(TCanvas* c1, vector<TH1D*> hData, TH1D* hMC);
    Comparison(TCanvas* c1, vector<TH1D*> hData, vector<TH1D*> hMC);
    Comparison(TCanvas* c1, TH1D* hData, vector<TH1D*> hMC);
    Comparison(TCanvas* c1, TH2D* hData, TH2D* hMC);
    Comparison(TCanvas* c1, TH1D* hData, vector<TH1D*> hMC, TH1D* hRatUnc);
    Comparison(TCanvas* c1, TH1D* hData, vector<TH1D*> hMC, vector<TH1D*> hRatUnc);
    //Comparison(TCanvas* c1, TH1D* hData, vector<TH1D*> hMC, TH1D* hDataUp, TH1D* hDataDown, vector<TH1D*> hMCUp, vector<TH1D*> hMCDown);
  
    ~Comparison();

    void plot(int idx);

    // Setters
    void set_filename(string filename) { mFilename = filename; }
    void set_x_bin_range(vector<int> xBinRange) { mCustomXRange = true; mXBinRange = xBinRange; }
    void set_y_bin_range(vector<int> yBinRange) { mCustomX2Range = true; mX2BinRange = yBinRange; }
    void set_y_lim_range(vector<double> yLimRange) { mCustomYRange = true; mYLimRange = yLimRange; }
    void set_rat_lim_range(vector<double> ratLimRange) { mCustomRatRange = true; mRatRange = ratLimRange; }
    void set_x_label(TString xLabel) {mXLabel = xLabel; }
    void set_y_label(TString yLabel) {mYLabel = yLabel; }
    void set_y_label(TString yLabel, double fontSize) { mYLabel = yLabel; mYLabelFontSize = fontSize; }
    void set_rat_label(TString ratLabel) {mRatLabel = ratLabel; }
    void set_info(vector<TString> vInfo) {mVInfo = vInfo;}
    void set_legend_labels(vector<TString> vLegendLabels) {mVLegendLabels = vLegendLabels;} 
    void set_scale(double scale) { mScale = scale; }
    void set_colors(int color1, int color2) { mColor1 = color1; mColor2 = color2; }
    void set_no_flow() { mFlow = false; }
    void set_no_log() { mLog = false; }
    void set_log_x() { mLogX = true; }
    void set_log_y() { mLogY = true; }
    void set_log_rat() { mLogRat = true; }
    void set_both_data() { mBothData = true; }
    void set_lumi(double lumi) {mLumi = lumi;}
    void set_no_lumi() {mLumi = -1;}
    void set_2DdrawOpt(TString drawOpt) {m2DDrawOpt = drawOpt;}
    void set_data_drawOpt(TString drawOpt) {mDataDrawOpt = drawOpt;}
    void set_multiple_comparisons() {mMultipleComparisons = true;}
    void set_legend_lower_right() {mLegendLowerRight = true;} 
    void set_rat_unc_label(vector<TString> label) {mRatUncLabel = label;} 

    void give_hlines(vector<double> vHLines) {mVHLines = vHLines;}
    void give_vlines(vector<double> vVLines) {mVVLines = vVLines;}
    void give_info(TString info) {mVInfo.push_back(info); }

  private:
    void default_options(TCanvas* c1);
    void set_main_pad(TPad* mainPad, bool log);
    void set_rat_pad(TPad* ratPad);
    void set_2d_pad(TPad* pad, bool log);

    void plot1D(int idx);
    void plot2D(int idx);

    void compute_limits(bool customXRange, bool customYRange);
    void compute_limits(bool customXRange, bool customYRange, bool customZRange);
    void compute_flow(vector<int> xBinRange);
    void set_histogram_options(int color1, int color2);
    void draw_main_histograms();
    void draw_2D_histograms(int idx);
    void annotate_plot();
    void make_rat_histogram(TH1D* hData, TH1D* hMC);
    void print(int idx);

    TCanvas* mCanvas;
    TPad* mMainPad;
    TPad* mRatPad;
    string mFilename;
    
    TH1D* mHData;
    vector<TH1D*> mVHData;
    TH1D* mHMC;
    vector<TH1D*> mVHMC;
    TH1D* mHRat;
    vector<TH1D*> mVHRat;
    TH1D* mHRatUnc;
    vector<TH1D*> mVHRatUnc;

    THStack* mStack;

    TH2D* mH2DData;
    TH2D* mH2DMC;
    TH2D* mH2DRat;
    TString m2DDrawOpt;
    TString mDataDrawOpt;

    bool m2D;
    bool mMultipleData;
    bool mMultipleComparisons;


    bool mCustomXRange;
    vector<int> mXBinRange;
    bool mCustomYRange;
    vector<double> mYLimRange;
    bool mCustomX2Range;
    vector<int> mX2BinRange;
    bool mCustomRatRange;
    vector<double> mRatRange;

    vector<double> mVHLines;
    vector<double> mVVLines;

    TString mXLabel;
    TString mYLabel;
    double mYLabelFontSize;
    TString mRatLabel;
    vector<TString> mVInfo;
    vector<TString> mVLegendLabels;
    bool mLegendLowerRight;

    double mScale;
    bool mScaled;    
    double mLumi;

    bool mLog;
    bool mLogX;
    bool mLogY;
    bool mLogRat;
    bool mFlow;
    bool mBothData;
    bool mRatio;

    bool mRatioUnc;

    int mColor1;
    int mColor2;
    vector<int> mColorData;

    vector<TString> mRatUncLabel;

    const double topSpace = 0.15;
    const double botSpace = 0.05;
    const double fs = 0.04;
};


inline
Comparison::~Comparison()
{
  if (!m2D) {
    for (int i=0; i<mVHData.size(); i++){
      delete mVHData[i];
      delete mVHRat[i];
    }
    delete mHMC;  
    for (int i = 0; i < mVHMC.size(); i++)
      delete mVHMC[i];
    for (int i = 0; i < mVHRatUnc.size(); i++)
      delete mVHRatUnc[i];
  }
  else {
    delete mH2DData;
    delete mH2DMC; 
  }
}

inline
Comparison::Comparison(TCanvas* c1, TH1D* hData, TH1D* hMC) 
{
  default_options(c1);
  mVHData.push_back((TH1D*)hData->Clone("mHData"));
  //mVHRat.push_back((TH1D*)hData->Clone("mHRat"));
  mHMC = (TH1D*)hMC->Clone("mHMC");
  mVHMC.push_back((TH1D*)hMC->Clone("mHMC0"));
}

inline
Comparison::Comparison(TCanvas* c1, vector<TH1D*> hData, TH1D* hMC)
{
  default_options(c1);
  int nDataHists = hData.size();
  for (int i=0; i<nDataHists; i++) {
    TString idx = Form("%d", i);
    mVHData.push_back((TH1D*)hData[i]->Clone("mHData"+idx));
    //mVHRat.push_back((TH1D*)hData[i]->Clone("mHRat"+idx));
  }

  mHMC = (TH1D*)hMC->Clone("mHMC");
}

inline
Comparison::Comparison(TCanvas* c1, vector<TH1D*> hData, vector<TH1D*> hMC)
{
  default_options(c1);

  int nDataHists = hData.size();
  for (int i=0; i<nDataHists; i++) {
    TString idx = Form("%d", i);
    mVHData.push_back((TH1D*)hData[i]->Clone("mHData"+idx));
    //mVHRat.push_back((TH1D*)hData[i]->Clone("mHRat"+idx));
  }


  int nMCHists = hMC.size();
  for (int i=0; i<nMCHists; i++) {
    TString idx = Form("%d", i);
    mVHMC.push_back((TH1D*)hMC[i]->Clone("mHMC"+idx));
    //mVHRat.push_back((TH1D*)hData[i]->Clone("mHRat"+idx));
  }

  mHMC = (TH1D*)hMC[0]->Clone("mHMC");
  //for (int i=1; i<nMCHists; i++)
  //  mHMC->Add(hMC[i]);
}

inline
Comparison::Comparison(TCanvas* c1, TH1D* hData, vector<TH1D*> hMC)
{
  default_options(c1);
  mVHData.push_back((TH1D*)hData->Clone("mHData"));

  int nMCHists = hMC.size();
  for (int i=0; i<nMCHists; i++) {
    TString idx = Form("%d", i);
    mVHMC.push_back((TH1D*)hMC[i]->Clone("mHMC"+idx));
    //mVHRat.push_back((TH1D*)hData[i]->Clone("mHRat"+idx));
  }

  mHMC = (TH1D*)hMC[0]->Clone("mHMC");
  for (int i=1; i<nMCHists; i++)
    mHMC->Add(hMC[i]);
}

inline
Comparison::Comparison(TCanvas* c1, TH1D* hData, vector<TH1D*> hMC, TH1D* hRatUnc)
{
  default_options(c1);
  mVHData.push_back((TH1D*)hData->Clone("mHData"));

  int nMCHists = hMC.size();
  for (int i=0; i<nMCHists; i++) {
    TString idx = Form("%d", i);
    mVHMC.push_back((TH1D*)hMC[i]->Clone("mHMC"+idx));
    //mVHRat.push_back((TH1D*)hData[i]->Clone("mHRat"+idx));
  }

  mHMC = (TH1D*)hMC[0]->Clone("mHMC");
  for (int i=1; i<nMCHists; i++)
    mHMC->Add(hMC[i]);

  mRatioUnc = true;
  //mHRatUnc = (TH1D*)hRatUnc->Clone("mHRatUnc");
  mVHRatUnc.push_back((TH1D*)hRatUnc->Clone("mHRatUnc"));
}

inline
Comparison::Comparison(TCanvas* c1, TH1D* hData, vector<TH1D*> hMC, vector<TH1D*> hRatUnc)
{
  default_options(c1);
  mVHData.push_back((TH1D*)hData->Clone("mHData"));

  int nMCHists = hMC.size();
  for (int i=0; i<nMCHists; i++) {
    TString idx = Form("%d", i);
    mVHMC.push_back((TH1D*)hMC[i]->Clone("mHMC"+idx));
    //mVHRat.push_back((TH1D*)hData[i]->Clone("mHRat"+idx));
  }

  mHMC = (TH1D*)hMC[0]->Clone("mHMC");
  for (int i=1; i<nMCHists; i++)
    mHMC->Add(hMC[i]);

  mRatioUnc = true;
  //mHRatUnc = (TH1D*)hRatUnc->Clone("mHRatUnc");
  for (int i = 0; i < hRatUnc.size(); i++) {
    TString idx = Form("%d", i);
    mVHRatUnc.push_back((TH1D*)hRatUnc[i]->Clone("mHRatUnc"+idx));
  } 
}


inline
Comparison::Comparison(TCanvas* c1, TH2D* hData, TH2D* hMC)
{
  default_options(c1);
  mH2DData = (TH2D*)hData->Clone("mH2DData");
  mH2DMC = (TH2D*)hMC->Clone("mH2DMC");
  m2D = true;

  mLogY = false;

  m2DDrawOpt = "COLZ";
}

inline
void Comparison::default_options(TCanvas* c1)
{
  mCanvas = c1;
  m2D = false;
  mFilename = "plots.pdf";

  mLog = true;
  mLogX = false;
  mLogRat = false;
  mFlow = true;
  mBothData = false;
  mCustomXRange = false;
  mCustomYRange = false;
  mCustomRatRange = false;
  mXBinRange.reserve(2);
  mYLimRange.reserve(2);
  mScale = 1;
  mScaled = false;
  mColor1 = kAzure+1;
  mColor2 = kBlack;

  mXLabel = "";
  mYLabel = "Events";
  mYLabelFontSize = 0.04;
  mRatLabel = "#frac{2017 Data}{2016 Data}";
  mRatioUnc = false;
  //mRatUncLabel = "Syst. Unc.";

  mDataDrawOpt = "E";
  mMultipleComparisons = false;
  mLegendLowerRight = false;
}

inline
void Comparison::plot(int idx)
{
  if (!m2D) plot1D(idx);
  else plot2D(idx);
}

inline
void Comparison::plot1D(int idx)
{
  set_main_pad(mMainPad, mLog); 
  compute_limits(mCustomXRange, mCustomYRange);
  compute_flow(mXBinRange);
  draw_main_histograms();
  set_histogram_options(mColor1, mColor2);
  annotate_plot();
  set_rat_pad(mRatPad);
  make_rat_histogram(mVHData[0], mHMC);
  print(idx);
}

/*
inline
void Comparison::plot1DMultipleData(int idx)
{
  set_main_pad(mMainPad, mLog);
  compute_limits(mCustomXRange, mCustomYRange);
  compute_flow(mXBinRange);
  draw_main_histograms();
  set_histogram_options(mColor1, mColor2);
  annotate_plot();
  set_rat_pad(mRatPad);
  make_rat_histogram(mHData, mHMC);
  print(idx);
}
*/

inline
void Comparison::plot2D(int idx)
{
  set_2d_pad(mMainPad, mLog);
  compute_limits(mCustomXRange, mCustomYRange, mCustomX2Range);
  draw_2D_histograms(idx);
  //annotate_2D_plot();
  //print_2D(idx);

}

inline
void Comparison::set_main_pad(TPad* mainPad, bool log)
{
  mCanvas->cd();
  mainPad = new TPad("p_main", "p_main", 0.0, 0.3, 1.0, 1.0);
  mainPad->SetBottomMargin(0.02);
  mainPad->SetRightMargin(0.07);
  mainPad->SetTopMargin(0.08);
  mainPad->SetLeftMargin(0.12);
  mainPad->Draw();
  mainPad->cd();
  if (log) mainPad->SetLogy();
  if (mLogX) mainPad->SetLogx();
}

inline
void Comparison::set_rat_pad(TPad* ratPad)
{
  mCanvas->cd();
  ratPad = new TPad( "p_main", "p_main", 0.0, 0.0, 1.0, 0.3);
  ratPad->SetBottomMargin(0.36);
  ratPad->SetRightMargin(0.07);
  ratPad->SetTopMargin(0.07);
  ratPad->SetLeftMargin(0.12);
  ratPad->Draw();
  ratPad->cd();
  ratPad->SetGridy();
  if (mLogX) ratPad->SetLogx();
  if (mLogRat) ratPad->SetLogy();
}

inline
void Comparison::set_2d_pad(TPad* pad, bool log)
{
  mCanvas->cd();
  pad = new TPad("p_main", "p_main", 0.0, 0.0, 1.0, 1.0);
  pad->SetBottomMargin(0.1);
  pad->SetRightMargin(0.15);
  pad->SetTopMargin(0.08);
  pad->SetLeftMargin(0.12);
  pad->Draw();
  pad->cd();
  if (log) mCanvas->SetLogz();
  if (mLogX) mCanvas->SetLogx();
  if (mLogY) mCanvas->SetLogy();
}

inline
void Comparison::compute_limits(bool customXRange, bool customYRange)
{
  if (!customXRange) { // calculate x limits
    int nBins = mVHData[0]->GetSize() - 2;
    mXBinRange[0] = 1;
    mXBinRange[1] = nBins;
    double area = mVHData[0]->Integral(0, nBins+1);
    for (int i=1; i<=nBins; i++) {
      double binContent = mVHData[0]->GetBinContent(i);
      if (binContent < (area/(pow(10,6))) )
        continue;
      else {
        mXBinRange[0] = i;
	break;
      } 
    } 
    for (int i=nBins; i>0; i--) {
      double binContent = mVHData[0]->GetBinContent(i);
      if (binContent < (area/(pow(10,6))) )
        continue;
      else {
        mXBinRange[1] = i;
        break;
      }
    }
  } 
  
  if (!customYRange) { // calculate y limits 
    double minContent(1.5*pow(10,308)), maxContent(0), avgContent(0);
    for (int j = 0; j < mVHData.size(); j++) {
      for (int i=mXBinRange[0]; i<=mXBinRange[1]; i++) {
	double binContent = mVHData[j]->GetBinContent(i);
	avgContent += binContent/float(mXBinRange[1] - mXBinRange[0]);
	if ((binContent < minContent) && binContent > 0)  minContent = binContent;
	if (binContent > maxContent) maxContent = binContent;
	//binContent = (mHMC->GetBinContent(i))*mScale;
	//if ((binContent < minContent) && binContent > 0)  minContent = binContent;
	//if (binContent > maxContent) maxContent = binContent;
      }
    }
    //if (minContent < avgContent/1000) minContent = avgContent/1000;
    double range;
    double tSpace = topSpace + (0.05*mVInfo.size());
    if (mLog) {
      range = log10(maxContent) - log10(minContent);
      mYLimRange[1] = pow(10, log10(maxContent) + tSpace*range);
      mYLimRange[0] = pow(10, log10(minContent) - botSpace*range);
    }
    else {
      range = maxContent - minContent;
      mYLimRange[1] = maxContent + tSpace*range;
      mYLimRange[0] = minContent - botSpace*range;
    }
    if (mYLimRange[0] < 0) mYLimRange[0] = 0;
    //cout << minContent << " " << maxContent << " " << range << endl;
  }

}

inline
void Comparison::compute_limits(bool customXRange, bool customYRange, bool customZRange) {  
  if (mCustomXRange) {
    mH2DData->GetXaxis()->SetRange(mXBinRange[0],mXBinRange[1]);
    mH2DMC->GetXaxis()->SetRange(mXBinRange[0],mXBinRange[1]);
  }
  if (mCustomYRange) {
    mH2DData->GetYaxis()->SetRange(mX2BinRange[0],mX2BinRange[1]);
    mH2DMC->GetYaxis()->SetRange(mX2BinRange[0],mX2BinRange[1]);
  } 

} 

inline
void Comparison::compute_flow(vector<int> xBinRange)
{
  int nBins = mVHData[0]->GetSize() - 2;
  if (mFlow) {
    for (int i=0; i<mVHMC.size(); i++) {
      double overflowMC = mVHMC[i]->Integral(mXBinRange[1]+1, nBins+1);
      mVHMC[i]->AddBinContent(mXBinRange[1], overflowMC);

      double underflowMC = mVHMC[i]->Integral(0, mXBinRange[0]-1);
      mVHMC[i]->AddBinContent(mXBinRange[0], underflowMC);
    }
    double overflowMC = mHMC->Integral(mXBinRange[1]+1, nBins+1);
    mHMC->AddBinContent(mXBinRange[1], overflowMC);
    double underflowMC = mHMC->Integral(0, mXBinRange[0]-1);
    mHMC->AddBinContent(mXBinRange[0], underflowMC);
    for (int i=0; i<mVHData.size(); i++) {
      double overflowData = mVHData[i]->Integral(mXBinRange[1]+1, nBins+1);
      mVHData[i]->AddBinContent(mXBinRange[1], overflowData);

      double underflowData = mVHData[i]->Integral(0, mXBinRange[0]-1);
      mVHData[i]->AddBinContent(mXBinRange[0], underflowData);
    }
  }
  mHMC->GetXaxis()->SetRange(mXBinRange[0],mXBinRange[1]);
  for (int i=0; i<mVHMC.size(); i++)
    mVHMC[i]->GetXaxis()->SetRange(mXBinRange[0],mXBinRange[1]);

  for (int i=0; i<mVHData.size(); i++) 
    mVHData[i]->GetXaxis()->SetRange(mXBinRange[0],mXBinRange[1]);
}

inline
void Comparison::set_histogram_options(int color1, int color2)
{
  mHMC->SetFillColor(mColor1);
  mHMC->SetLineColor(mColor1);
  mHMC->SetMarkerColor(mColor1);
  if (mBothData) mHMC->SetMarkerStyle(20);
  mHMC->GetYaxis()->SetTitle(mYLabel);
  mHMC->GetYaxis()->SetTitleSize(mYLabelFontSize);
  mHMC->GetYaxis()->SetTitleOffset(1.2);
  if (!mLog) mHMC->GetYaxis()->SetTitleOffset(1.6);
  mHMC->GetXaxis()->SetLabelOffset(999);
  mHMC->GetXaxis()->SetLabelSize(0);

  for (int i=0; i<mVHData.size(); i++) {
    mVHData[i]->SetMarkerStyle(20);
    mVHData[i]->SetMarkerColor(dataColors[i]);
    mVHData[i]->SetLineColor(dataColors[i]); 
    mVHData[i]->SetLineWidth(2);
    mVHData[i]->GetXaxis()->SetLabelOffset(999);
    mVHData[i]->GetXaxis()->SetLabelSize(0);
    if (mMultipleComparisons) {
      if (i != 0) continue;
      mVHData[i]->GetYaxis()->SetTitle(mYLabel);
      mVHData[i]->GetYaxis()->SetTitleSize(mYLabelFontSize);
      mVHData[i]->GetYaxis()->SetTitleOffset(1.2);
    }
  }

  vector<int> vDefaultColors = {kRed - 7, kAzure+1, kCyan-7, kViolet -4, kOrange+1, kGreen-3, kTeal+3, kBlue-6};
  for (int i=0; i<mVHMC.size(); i++) {
    mVHMC[i]->SetFillColor(vDefaultColors[i]);
    mVHMC[i]->SetLineColor(vDefaultColors[i]);
    mVHMC[i]->SetMarkerColor(vDefaultColors[i]);
    if (mBothData) mVHMC[i]->SetMarkerStyle(20);
    mVHMC[i]->GetYaxis()->SetTitle(mYLabel);
    mVHMC[i]->GetYaxis()->SetTitleSize(mYLabelFontSize);
    mVHMC[i]->GetYaxis()->SetTitleOffset(1.2);
    if (!mLog) mVHMC[i]->GetYaxis()->SetTitleOffset(1.6);
    mVHMC[i]->GetXaxis()->SetLabelOffset(999);
    mVHMC[i]->GetXaxis()->SetLabelSize(0);
  }
  if (!mBothData) { 
    if (!mMultipleComparisons) {
      mStack->GetXaxis()->SetLabelOffset(999);
      mStack->GetXaxis()->SetLabelSize(0);
    }
  }
}

bool sortByValue(const std::pair<int,float>& pair1, const std::pair<int,float>& pair2 ) {
  return pair1.second < pair2.second;
}

inline
void Comparison::draw_main_histograms()
{
  mStack = new THStack("hStack", "");

  if (mScale == -1) {
    double eventsData = mVHData[0]->Integral();
    double eventsMC = mHMC->Integral();
    mScale = eventsData/eventsMC;
  }
  if (!mScaled) {
    mHMC->Scale(mScale);
    std::vector<std::pair<int, float> > hist_ordering;
    for (unsigned int i=0; i<mVHMC.size(); i++) {
      mVHMC[i]->Scale(mScale);
      hist_ordering.push_back(std::pair<int, float>(i, mVHMC[i]->Integral()));
    }
    // Sort MC histograms by area
    std::sort(hist_ordering.begin(), hist_ordering.end(), sortByValue);
    for (std::vector<std::pair<int, float> >::iterator it = hist_ordering.begin(); it != hist_ordering.end(); ++it)
      mStack->Add(mVHMC[it->first]);
    mScaled = true;
  }

  for (int i=0; i<mVHData.size(); i++) { 
    if (i == 0) {
     mVHData[i]->Draw(mDataDrawOpt);
     mVHData[i]->SetMinimum(mYLimRange[0]);
     mVHData[i]->SetMaximum(mYLimRange[1]);
    }
    else
     mVHData[i]->Draw("SAME, " + mDataDrawOpt);
  }
  if (!mMultipleComparisons) {
    if (!mBothData) mStack->Draw("SAME, HIST");
    else mHMC->Draw("SAME, E");
  }
  //mStack->GetXaxis()->SetRange(mXBinRange[0],mXBinRange[1]);
  //mStack->SetMinimum(mYLimRange[0]);
  //mStack->SetMinimum(mYLimRange[0]);
  //mHMC->SetMaximum(mYLimRange[1]);
  //mHMC->SetMaximum(mYLimRange[1]); 
  for (int i=0; i<mVHData.size(); i++)
    mVHData[i]->Draw("SAME, " + mDataDrawOpt);
  gPad->RedrawAxis();
}

inline
void Comparison::draw_2D_histograms(int idx)
{
  if (mScale == -1) {
    double eventsData = mH2DData->Integral();
    double eventsMC = mH2DMC->Integral();
    mScale = eventsData/eventsMC;
  }
  if (!mScaled) {
    mH2DMC->Scale(mScale);
    mScaled = true;
  }

  mCanvas->Clear();
  mH2DRat = (TH2D*)mH2DData->Clone("mH2DRat");

  mH2DData->Draw(m2DDrawOpt);


  mH2DData->SetMinimum(pow(10,-1));
  mH2DData->SetMaximum(pow(10,5));
  if (mCustomYRange) {
    mH2DData->SetMinimum(mYLimRange[0]);
    mH2DData->SetMaximum(mYLimRange[1]);
  }

  mH2DData->GetXaxis()->SetTitle(mXLabel);
  mH2DData->GetYaxis()->SetTitle(mYLabel);
  mH2DData->GetYaxis()->SetTitleOffset(1.3);


  TLatex* cms = new TLatex(0.11, 0.91, "CMS Preliminary");
  cms->SetTextSize(fs*0.75);
  cms->SetNDC(kTRUE);
  cms->Draw("SAME");

  TLatex* lumi;
  TString lumiText = Form("%.2f",mLumi);
  lumiText += " fb^{-1} (13 TeV)";
  lumi = new TLatex(0.70, 0.91, lumiText.Data());
  lumi->SetTextSize(fs*0.75);
  lumi->SetNDC(kTRUE);
  lumi->Draw("SAME");

  vector<TLatex*> vLabs;
  bool drawLabs = false;
  if (mVLegendLabels.size() >= 2) {
    drawLabs = true;
    vLabs.push_back(new TLatex(0.40, 0.91, mVLegendLabels[0]));
    vLabs.push_back(new TLatex(0.40, 0.91, mVLegendLabels[1]));
    vLabs.push_back(new TLatex(0.40, 0.91, mRatLabel));
  }
  for (int i=0; i<vLabs.size(); i++) {
    vLabs[i]->SetTextSize(fs*0.75);
    vLabs[i]->SetNDC(kTRUE);
  }

  vector<TLatex*> t(mVInfo.size());
  for(int i=0; i<mVInfo.size(); i++) {
    double j = i;
    j *= 0.05;
    t[i] = new TLatex(0.20, 0.85-j, mVInfo[i]);
    t[i]->SetTextSize(fs*0.75);
    t[i]->SetNDC(kTRUE);
    t[i]->Draw("SAME");
  }
  if (drawLabs) vLabs[0]->Draw("SAME");

  if (idx == 0)
    mCanvas->Print((mFilename+"(").c_str());
  else
    mCanvas->Print((mFilename).c_str());  
  mCanvas->Clear();

  mH2DMC->Draw(m2DDrawOpt);
  mH2DMC->GetXaxis()->SetTitle(mXLabel);
  mH2DMC->GetYaxis()->SetTitle(mYLabel);
  mH2DMC->GetYaxis()->SetTitleOffset(1.3);

  mH2DMC->SetMinimum(pow(10,-1));
  mH2DMC->SetMaximum(pow(10,5));
  if (mCustomYRange) {
    mH2DMC->SetMinimum(mYLimRange[0]);
    mH2DMC->SetMaximum(mYLimRange[1]);
  }

  cms->Draw("SAME");
  lumi->Draw("SAME");
  //vector<TLatex*> t(mVInfo.size());
  for(int i=0; i<mVInfo.size(); i++) {
    double j = i;
    j *= 0.05; 
    //t[i] = new TLatex(0.20, 0.85-j, mVInfo[i]);
    //t[i]->SetTextSize(fs);
    //t[i]->SetNDC(kTRUE);
    t[i]->Draw("SAME");
  }
  if (drawLabs) vLabs[1]->Draw("SAME");

  mCanvas->Print((mFilename).c_str());
  mCanvas->Clear();
  mCanvas->SetLogz(0); 
  //mH2DRat = (TH2D*)mH2DData->Clone("mH2DRat");
  mH2DRat->Divide(mH2DMC);
  mH2DRat->Draw(m2DDrawOpt);
  mH2DRat->SetMinimum(0);
  mH2DRat->SetMaximum(2);

  mH2DRat->GetXaxis()->SetTitle(mXLabel);
  mH2DRat->GetYaxis()->SetTitle(mYLabel);
  mH2DRat->GetYaxis()->SetTitleOffset(1.3);
  
  cms->Draw("SAME");
  lumi->Draw("SAME");
  //vector<TLatex*> t(mVInfo.size());
  for(int i=0; i<mVInfo.size(); i++) {
    double j = i;
    j *= 0.05;
    //t[i] = new TLatex(0.20, 0.85-j, mVInfo[i]);
    //t[i]->SetTextSize(fs);
    //t[i]->SetNDC(kTRUE);
    t[i]->Draw("SAME");
  }
  if (drawLabs) vLabs[2]->Draw("SAME");
  
  if (idx == 2)
    mCanvas->Print((mFilename+")").c_str());
  else
    mCanvas->Print((mFilename).c_str());
  mCanvas->Clear("D"); 
  mCanvas->SetLogx(0);
  mCanvas->SetLogy(0);
}

inline
void Comparison::annotate_plot()
{
  TLatex* cms = new TLatex(0.20, 0.93, "CMS Preliminary");
  cms->SetTextSize(fs);
  cms->SetNDC(kTRUE);
  cms->Draw("SAME");

  TLatex* lumi;
  if (mLumi != -1) {
    TString lumiText = Form("%.2f",mLumi);
    lumiText += " fb^{-1} (13 TeV)";
    lumi = new TLatex(0.66, 0.93, lumiText.Data());
    lumi->SetTextSize(fs);
    lumi->SetNDC(kTRUE);
    lumi->Draw("SAME");
  }  
  else {
    lumi = new TLatex(0.73, 0.93, "(13 TeV)");
    lumi->SetTextSize(fs);
    lumi->SetNDC(kTRUE);
    lumi->Draw("SAME");
  }

  vector<TLatex*> t(mVInfo.size());
  for(int i=0; i<mVInfo.size(); i++) {
    double j = i;
    j *= 0.05;
    t[i] = new TLatex(0.15, 0.85-j, mVInfo[i]);
    t[i]->SetTextSize(fs);
    t[i]->SetNDC(kTRUE);
    t[i]->Draw("SAME");
  }

  if (mVLegendLabels.size() > 0) {
    double j = mVHData.size()*0.05;
    TLegend* l1;
    if (!mLegendLowerRight)
      l1 = new TLegend(0.70, 0.75-j, 0.92, 0.89);
    else
      l1 = new TLegend(0.70, 0.06, 0.92, 0.23+j); 
    for (int i=0; i<mVHData.size(); i++)
      l1->AddEntry(mVHData[i], mVLegendLabels[i], "lep");
    int idxMC = mVHData.size();
    cout << "setting legend labels" << endl;
    for (int i=0; i<mVHMC.size(); i++) {
      cout << "inside loop" << endl;
      if (mMultipleComparisons) break;
      if (!mBothData) l1->AddEntry(mVHMC[i], mVLegendLabels[idxMC+i], "f");
      else l1->AddEntry(mHMC, mVLegendLabels[idxMC], "lep");
    }
    l1->SetBorderSize(0);
    l1->Draw("SAME");
  }

  vector<TLine*> vLH(mVHLines.size());
  for (int i=0; i<mVHLines.size(); i++) {
    //cout << mXBinRange[0] << " " << mXBinRange[1] << endl;
    //cout << mHMC->GetBinLowEdge(mXBinRange[0]) << " " << mHMC->GetBinLowEdge(mXBinRange[1]+1) << endl;
    vLH[i] = new TLine(mHMC->GetBinLowEdge(mXBinRange[0]), mVHLines[i], mHMC->GetBinLowEdge(mXBinRange[1]+1), mVHLines[i]);
    vLH[i]->SetLineWidth(1);
    vLH[i]->SetLineStyle(2);
    vLH[i]->Draw("SAME");
  }
  vector<TLine*> vLV(mVVLines.size());
  for (int i=0; i<mVVLines.size(); i++) {
    vLV[i] = new TLine(mVVLines[i], mYLimRange[0], mVVLines[i], mYLimRange[1]);
    vLV[i]->SetLineWidth(2);
    vLV[i]->SetLineStyle(2);
    vLV[i]->Draw("SAME");
  }

}

inline
void Comparison::make_rat_histogram(TH1D* hData, TH1D* hMC)
{
  mVHRat.push_back((TH1D*)hData->Clone("mVHRat0"));
  mVHRat[0]->SetTitle("");
  mVHRat[0]->Divide(hMC);
  if (mCustomRatRange) {
    mVHRat[0]->GetYaxis()->SetRangeUser(mRatRange[0],mRatRange[1]);
  }
  else
    mVHRat[0]->GetYaxis()->SetRangeUser(0.0,2.0);
  mVHRat[0]->GetYaxis()->SetLabelSize(0.08);
  mVHRat[0]->GetXaxis()->SetLabelSize(0.08);
  mVHRat[0]->GetYaxis()->SetNdivisions(5);

  mVHRat[0]->GetYaxis()->SetTitle(mRatLabel);
  mVHRat[0]->GetYaxis()->SetTitleSize(0.08);
  mVHRat[0]->GetYaxis()->SetTitleOffset(0.65);
  mVHRat[0]->GetYaxis()->CenterTitle();

  mVHRat[0]->GetXaxis()->SetTitle(mXLabel);
  mVHRat[0]->GetXaxis()->SetTitleOffset(1.1);
  mVHRat[0]->GetXaxis()->SetTitleSize(0.12);

  mVHRat[0]->SetMarkerStyle(20);
  //mVHRat[0]->SetMarkerSize(1);
  mVHRat[0]->SetStats(0);
  mVHRat[0]->GetXaxis()->SetRange(mXBinRange[0], mXBinRange[1]);
  mVHRat[0]->SetMarkerColor(dataColors[0]);
  mVHRat[0]->SetLineColor(dataColors[0]);
  mVHRat[0]->SetFillColor(dataColors[0]);


  mVHRat[0]->Draw("e1");
  mVHRat[0]->Draw("e1 SAME");
  mVHRat[0]->GetXaxis()->SetLabelOffset();
  mVHRat[0]->GetXaxis()->SetLabelSize(0.07);


  vector<int> vRatUncColors = {kGray+0, kAzure+1, kRed-7, kGreen, kOrange};
  for (int i = 0; i < mVHRatUnc.size(); i++) {
    mVHRatUnc[i]->SetFillStyle(1001);
    mVHRatUnc[i]->SetFillColor(vRatUncColors[i]);
    mVHRatUnc[i]->SetMarkerColor(vRatUncColors[i]);
    mVHRatUnc[i]->SetLineColor(vRatUncColors[i]);
    mVHRatUnc[i]->Draw("E2, SAME");
  }

  mVHRat[0]->Draw("e1 SAME");

  for (int i=1; i<mVHData.size(); i++) {
    TString idx = Form("%d", i);
    mVHRat.push_back((TH1D*)mVHData[i]->Clone("mVHRat"+idx));
    if (!mMultipleComparisons)
      mVHRat[i]->Divide(hMC);
    else
      mVHRat[i]->Divide(mVHMC[i]);
    mVHRat[i]->SetMarkerStyle(20);
    mVHRat[i]->SetMarkerColor(dataColors[i]);
    mVHRat[i]->SetFillColor(dataColors[i]);
    mVHRat[i]->SetLineColor(dataColors[i]);
    mVHRat[i]->SetStats(0);
    mVHRat[i]->Draw("e1, same");
  }
  gPad->RedrawAxis();

  /*if (mRatioUnc && mRatUncLabel.size() > 0) {
    TLegend* legend = new TLegend(0.14, 0.38, 0.34, 0.48);
    legend->AddEntry(mHRatUnc, mRatUncLabel[0], "f");
    legend->SetBorderSize(0);
    legend->Draw("SAME");
  } */

  for (int i = 0; i < mRatUncLabel.size(); i++) {
    TLegend* legend = new TLegend(0.14 + (i*0.2), 0.38, 0.34 + (i*0.2), 0.48);
    legend->AddEntry(mVHRatUnc[i], mRatUncLabel[i], "f");
    legend->SetBorderSize(0);
    legend->Draw("SAME");
  }

}

inline
void Comparison::print(int idx)
{
  if (idx == 0)
    mCanvas->Print((mFilename+"(").c_str());
  else if (idx == 1)
    mCanvas->Print((mFilename).c_str());
  else if (idx == 2)
    mCanvas->Print((mFilename+")").c_str());
  mCanvas->Clear("D");
}

#endif // _PLOTHELPER_H_

