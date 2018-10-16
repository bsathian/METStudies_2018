#include "ScanChain.C"
#include <stdlib.h>

int main(int argc, char* argv[])
{
  cout << "Input files: " << argv[1] << endl;
  cout << "Output file: " << argv[2] << endl;
  cout << "Index: " << argv[3] << endl;
  cout << "Weight file: " << argv[4] << endl;
  cout << "Selection: " << argv[5] << endl;
  cout << "Scale1fb: " << argv[6] << endl;


  TChain *chain = new TChain("Events");
  
  TString input_files = argv[1];
  TObjArray *tx = input_files.Tokenize(",");
  for (int i = 0; i < tx->GetEntries(); i++) {
    TString fname = ((TObjString *)(tx->At(i)))->String();
    fname.ReplaceAll("'","");
    cout << "Adding file: " << fname << endl;
    chain->Add(fname);
  }

  TString output_file = argv[2];
  TString idx = argv[3];
  TString output_name = output_file + "_" + idx;

  TString weight_file = argv[4];
  bool puReweight = (weight_file != "none");

  int selection = atoi(argv[5]);
  double scale1fb = atof(argv[6]);

  vector<TString> vWeightFile = {"pileupReweightsMC_B.root", "pileupReweightsMC_C.root", "pileupReweightsMC_D,E.root", "pileupReweightsMC_F.root"};
  ScanChain(chain, output_name, vWeightFile, puReweight, selection, scale1fb); 
  return 0;
}
