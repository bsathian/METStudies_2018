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
hadoop_path = "MET_2018"

parser = argparse.ArgumentParser()
parser.add_argument("--selection", help = "1: regular, 2: just fill nvtx hist for PU reweighting", type=str)
parser.add_argument("--test_run", help = "just submit for DoubleMuon and EGamma 2018 Run A", action="store_true")
parser.add_argument("--mc_only", help = "only run mc", action="store_true")
parser.add_argument("--no_tar", help = "don't remake tar file", action="store_true")
args = parser.parse_args()


# Make tarball
if not args.no_tar:
  os.system("XZ_OPT=-9 tar -Jc --exclude='.git' --exclude='*.tar*' -f package.tar.gz analyze *_JSON_snt.txt pileupReweight*.root CORE/")

job_tag = "MET_v2"
if args.selection == 2:
  job_tag += "pu"


eras = "A,B,C,D"
eras = eras.split(",")
if args.test_run:
  eras = ["A"]

data = {
	"A" : { "dirs" : ["EGamma_Run2018A-PromptReco-v1_MINIAOD_CMS4_V10-02-01_allPfCands", "EGamma_Run2018A-PromptReco-v2_MINIAOD_CMS4_V10-02-01_allPfCands", "EGamma_Run2018A-PromptReco-v3_MINIAOD_CMS4_V10-02-01_allPfCands", "DoubleMuon_Run2018A-PromptReco-v1_MINIAOD_CMS4_V10-02-01_allPfCands", "DoubleMuon_Run2018A-PromptReco-v2_MINIAOD_CMS4_V10-02-01_allPfCands", "DoubleMuon_Run2018A-PromptReco-v3_MINIAOD_CMS4_V10-02-01_allPfCands"], "globber" : "*2018A*MINIAOD_CMS4_V10-02-01_allPfCands"},
	"B" : { "dirs" :  ["EGamma_Run2018B-PromptReco-v1_MINIAOD_CMS4_V10-02-01_allPfCands", "EGamma_Run2018B-PromptReco-v2_MINIAOD_CMS4_V10-02-01_allPfCands", "DoubleMuon_Run2018B-PromptReco-v1_MINIAOD_CMS4_V10-02-01_allPfCands", "DoubleMuon_Run2018B-PromptReco-v2_MINIAOD_CMS4_V10-02-01_allPfCands"], "globber" : "*2018B*MINIAOD_CMS4_V10-02-01_allPfCands"},
	"C" : { "dirs" : ["EGamma_Run2018C-PromptReco-v1_MINIAOD_CMS4_V10-02-01_allPfCands", "EGamma_Run2018C-PromptReco-v2_MINIAOD_CMS4_V10-02-01_allPfCands", "EGamma_Run2018C-PromptReco-v3_MINIAOD_CMS4_V10-02-01_allPfCands", "DoubleMuon_Run2018C-PromptReco-v1_MINIAOD_CMS4_V10-02-01_allPfCands", "DoubleMuon_Run2018C-PromptReco-v2_MINIAOD_CMS4_V10-02-01_allPfCands", "DoubleMuon_Run2018C-PromptReco-v3_MINIAOD_CMS4_V10-02-01_allPfCands"], "globber" : "*2018C*MINIAOD_CMS4_V10-02-01_allPfCands"},
	"D" : { "dirs" : ["EGamma_Run2018D-PromptReco-v1_MINIAOD_CMS4_V10-02-01_allPfCands", "EGamma_Run2018D-PromptReco-v2_MINIAOD_CMS4_V10-02-01_allPfCands", "DoubleMuon_Run2018D-PromptReco-v2_MINIAOD_CMS4_V10-02-01_allPfCands"], "globber" : "*2018D*MINIAOD_CMS4_V10-02-01_allPfCands"},
}

mc = {
	"DY" : { "dirs" : ["DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall18MiniAOD-102X_upgrade2018_realistic_v12_ext1-v1_MINIAODSIM_CMS4_V10-02-01_allPfCands"], "globber" : "DYJetsToLL*MINIAODSIM_CMS4_V10-02-01_allPfCands", "xs" : 5765.4, "n_events" : 43814247, "frac_neg" : 0.0, "fpo" : 5}, 
}

def scale1fb(dict):
  return (dict["xs"] * 1000.) / (dict["n_events"] * (1 - (2*dict["frac_neg"])))

basepath = "/hadoop/cms/store/user/smay/ProjectMetis/"


all_jobs_done = False
total_summary = {}
while True:
  all_jobs_done = True
  for key, info in mc.iteritems():
    if args.test_run:
      continue
    sample = DirectorySample(dataset = key, location = basepath + info["globber"])
    job_args = '%s %s %.12f' % ("none" if args.selection == "2" else "Reweight", args.selection, scale1fb(info))
    output_name = "Zll_histograms.root"
    task = CondorTask(
              sample = sample,
              open_dataset = False,
              flush = True,
              files_per_output = info["fpo"] if args.selection == 1 else info["fpo"] * 3,
              output_name = output_name,
              tag = job_tag,
              cmssw_version = "CMSSW_9_4_9", # doesn't do anything
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
    total_summary[key] = task.get_task_summary()
    if task.complete():
      print ""
      print "Job={} finished".format(job_tag)
      print ""

  for key, info in data.iteritems():
    if args.mc_only:
      continue
    if key not in eras:
      continue
    sample = DirectorySample(dataset = "Data_Run2018" + key, location = basepath + info["globber"])
    job_args = '%s %s %.12f' % ("none", args.selection, 1.0)
    output_name = "Zll_histograms.root"
    task = CondorTask(
              sample = sample,
              open_dataset = False,
              flush = True,
              files_per_output = 10 if args.selection == 1 else 30,
              output_name = output_name,
              tag = job_tag,
              cmssw_version = "CMSSW_9_4_9", # doesn't do anything
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
    total_summary["Data_Run2018" + key] = task.get_task_summary()
    if task.complete():
      print ""
      print "Job={} finished".format(job_tag)
      print ""

  
  if all_jobs_done:
    print ""
    print "All jobs are finished."
    print ""
    break
  #StatsParser(data=total_summary, webdir="~/public_html/dump/Zll_MetStudies2018/").do()
  #os.system("chmod -R 755 ~/public_html/dump/Zll_MetStudies")
  print "Sleeping 300 seconds ..."
  time.sleep(300)
