import sys, os
import time
import itertools
import numpy

import argparse

from metis.Sample import DirectorySample
from metis.CondorTask import CondorTask
from metis.StatsParser import StatsParser

exec_path = "condor_exe.sh"
tar_path = "package.tar.gz"
hadoop_path = "MET"

#Get 1fb weights from CORE file
def get_weights(dataset):
    fistSlash = dataset.find("RunII")
    secondSlash = dataset.find("MINIAOD")
    terminate = dataset.find("CMS4")

    miniAODString = "/".join(["",dataset[:firstSlash-1],dataset[firstSlash:secondSlash-1],dataset[secondSlash:terminate-1]])

    try:
        scalefile = open("scale1fbs.txt"."r")
    except:
        print("scale1fb.txt not found! Using scale1fb = 1")
        return 1


    scale1fb = None
    for i in scalefile:
        if miniAODString in i.split()[0]:
            scale1fb = float(i.split()[5])
            break
    return scale1fb

# Don't need to parse args anymore, take care of PU reweighting bookkeeping in analyze.cpp

parser = argparse.ArgumentParser()
#parser.add_argument("eras", help = "Which 2017 data eras to consider (B,C,D,E,F)", type=str)
#parser.add_argument("weightfile", type=str, help = "root file to use for pileup reweighting")
parser.add_argument("--runF_only", help = "Run only on data from Run F and no mc", action="store_true")
parser.add_argument("--reweight", help = "(Re)derive pileup weights", action="store_true")
parser.add_argument("--validation", help = "Run only on 2017F DoubleMuon 949 to validate MET recipe", action="store_true")
parser.add_argument("--validate_recipe", help = "Validate recipe in CMS4", action="store_true")
parser.add_argument("--test_run", help = "Just submit for 1 MC sample", action="store_true")
parser.add_argument("--data_only", help = "Just submit jobs for data", action="store_true")
args = parser.parse_args()

#job_tag = "MET_v1" + args.eras
job_tag = "MET_v28"

eras = "B,C,D,E,F,F_v2"
eras = eras.split(",")

if args.validation or args.runF_only:
  eras = ["F_v2"]

data = {
        "B":["/DoubleEG_Run2017B-31Mar2018-v1_MINIAOD_CMS4_V10-02-05","/DoubleMuon_Run2017B-31Mar2018-v1_MINIAOD_CMS4_V10-02-05"],
        "C":["/DoubleEG_Run2017C-31Mar2018-v1_MINIAOD_CMS4_V10-02-05","/DoubleMuon_Run2017C-31Mar2018-v1_MINIAOD_CMS4_V10-02-05"],
        "D":["/DoubleEG_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V10-02-05","/DoubleMuon_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V10-02-05"],
        "E":["/DoubleEG_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V10-02-05","/DoubleMuon_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V10-02-05"],
        "F":["/DoubleEG_Run2017F-31Mar2018-v1_MINIAOD_CMS4_V10-02-05","/DoubleMuon_Run2017F-09May2018-v1_MINIAOD_CMS4_V10-02-04"],
        "F_v2":["/DoubleEG_Run2017F-09May2018-v1_MINIAOD_CMS4_V10-02-04","/DoubleMuon_Run2017F-09May2018-v1_MINIAOD_CMS4_V10-02-04"]
        }



if args.validation:
  data = {"F" :  ["/DoubleMuon_Run2017F-09May2018-v1_MINIAOD_CMS4_V10-02-04"]}


mc = {"Drell-Yan":["/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V10-02-05"],
        "WW":["/WW_TuneCP5_13TeV-pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V10-02-05"],
        "WZ":["/WZ_TuneCP5_13TeV-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V10-02-05"],
        "ZZ":[""],
        }


if args.runF_only or args.data_only:
  mc = {}

dataBasePath = "/hadoop/cms/store/group/snt/run2_data2017"
mcBasePath = "/hadoop/cms/store/group/snt/run2_mc2017"

total_summary = {}
while True:
  all_jobs_done = True
  for key, info in mc.iteritems():
    if args.validation:
      continue
    for set in info[0]:
      #if args.weightfile == "none" and (not args.reweight or eras[0] != "MC") : # only if you've already made unweighted MC histograms for nvtx reweighting
      #  continue
      sample = DirectorySample(dataset = set, location = mcBasepath + set)

      weight1fb = get_weights(set)

      if args.validate_recipe:
        job_args = 'none 7 1'
        output_name = "Zll_histograms.root"
      elif not args.reweight:
        job_args = "%s %d %.12f" % ("Reweight", 1, weight1fb)
	#output_name = "Zll_histograms_" + args.eras + ".root"
        output_name = "Zll_histograms.root"
      else:
        job_args = "%s %d %.12f" % ("none", 2, weight1fb)
	output_name = "Zll_histograms_All.root"
      task = CondorTask(
              sample = sample,
              open_dataset = False,
              flush = True,
              files_per_output = info[4],
              output_name = output_name,
              tag = job_tag,
              cmssw_version = "CMSSW_9_2_1", # doesn't do anything
              arguments = job_args,
              executable = exec_path,
              tarfile = tar_path,
              condor_submit_params = {"sites" : "T2_US_UCSD"},
              special_dir = hadoop_path
              )
      task.process()
      if not task.complete():
        print("Job %s still running." % job_tag)
        all_jobs_done = False
      total_summary[set] = task.get_task_summary()
      if task.complete():
	print ""
	print "Job={} finished".format(job_tag)
	print ""

  for era in eras:
    if args.test_run:
      continue
    if era == "MC":
      continue
    for set in data[era]:
      if args.validation:
        if "DoubleEG" in set:
          continue
      #if args.weightfile != "none" and not args.reweight: # we already made histos for data
      #  continue
      sample = DirectorySample(dataset = set, location = dataBasepath + set)
      #print(sample.get_files())
      if args.validation:
        job_args = 'none 5 1'
      elif args.validate_recipe:
        job_args = 'none 7 1'
      elif not args.reweight:
        job_args = 'none 1 1'
      else:
        job_args = 'none 2 1'
      task = CondorTask(
	      sample = sample,
	      open_dataset = False,
	      flush = True,
	      files_per_output = 1,
	      output_name = "Zll_histograms.root",
	      tag = job_tag,
	      cmssw_version = "CMSSW_9_2_1", # doesn't do anything
	      arguments = job_args,
	      executable = exec_path,
	      tarfile = tar_path,
	      condor_submit_params = {"sites" : "T2_US_UCSD"},
	      special_dir = hadoop_path
	      )
      task.process()
      if not task.complete():
        print("Job %s still running." % job_tag)
        all_jobs_done = False
      total_summary[set] = task.get_task_summary()
      if task.complete():
	print ""
	print "Job={} finished".format(job_tag)
	print ""

  if all_jobs_done:
    print ""
    print "All jobs are finished."
    print ""
    break
  StatsParser(data=total_summary, webdir="~/public_html/dump/Zll_MetStudies/").do()
  #os.system("chmod -R 755 ~/public_html/dump/Zll_MetStudies")
  os.system("python ../delete_bad_files.py")
  print "Sleeping 300 seconds ..."
  time.sleep(300)
