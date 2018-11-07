import sys, os
import time
import itertools
import numpy
import glob

os.chdir("histograms")

import argparse
parser = argparse.ArgumentParser()
parser.add_argument("--eras", help = "Which 2018 data eras to consider", type=str)
parser.add_argument("--tag", help = "job tag", type=str)
parser.add_argument("--skip_data", action="store_true")
parser.add_argument("--skip_mc", action="store_true")
args = parser.parse_args()
eras = args.eras.split(",")

basepath = "/hadoop/cms/store/user/smay/MET_2018/"

data = {
	"A" : "Data_Run2018A",
	"B" : "Data_Run2018B",
	"C" : "Data_Run2018C",
	"D" : "Data_Run2018D",
}

mc = {
	"DY" : "DY",
	"DY_RelVal_Zee" : "DY_RelVal",
}

nPar = 8

# Make data files by era
if not args.skip_data:
  data_files = ""
  for key, era in data.iteritems():
    if key not in eras:
      continue  
    files = glob.glob(basepath + era + "_" + args.tag + "/*.root")
    if len(files) == 0:
      continue
    file_name = era
    data_files += file_name + ".root "
    print("addHistos %s %s %d %d" % (file_name, basepath + era + "_" + args.tag + "/Zll_histograms", len(files), nPar))
    os.system("addHistos %s %s %d %d" % (file_name, basepath + era + "_" + args.tag + "/Zll_histograms", len(files), nPar)) 

  # Make one big data histogram
  os.system("hadd -fk Data_%s.root %s" % ("All", data_files))

# Make MC files
if not args.skip_mc:
  for key, sample in mc.iteritems():
    files = glob.glob(basepath + sample + "_" + args.tag + "/*.root")
    if len(files) == 0:
      continue
    file_name = sample
    print("addHistos %s %s %d %d" % (file_name, basepath + sample + "_" + args.tag + "/Zll_histograms", len(files), nPar))
    os.system("addHistos %s %s %d %d" % (file_name, basepath + sample + "_" + args.tag + "/Zll_histograms", len(files), nPar))
