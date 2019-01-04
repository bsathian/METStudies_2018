import time
import itertools
import json

from metis.Sample import DBSSample
from metis.CMSSWTask import CMSSWTask
from metis.StatsParser import StatsParser

import os, sys

tar_name = "lib_CMS4Plus_V10-02-01.tar.gz"
#os.system("mtarfile -b %s --xz" % tar_name) 

if __name__ == "__main__":

    pds = ["DoubleMuon", "EGamma"]
    proc_vers = [
	#("Run2018A", "v1"),
	#("Run2018A", "v2"),
	#("Run2018A", "v3"),
	#("Run2018B", "v1"),
	#("Run2018B", "v2"),
	#("Run2018C", "v1"),
        #("Run2018C", "v2"),
	#("Run2018C", "v3"),
	("Run2018D", "v1"),
        ("Run2018D", "v2"),
	]	

    proc_vers_2 = [
	("Run2018A", "v2"),
	("Run2018B", "v1"),
	("Run2018C", "v1")
	]

    dataset_names = ["/{0}/{1}-PromptReco-{2}/MINIAOD".format(x[0],x[1][0],x[1][1]) for x in itertools.product(pds,proc_vers)]
    dataset_names += ["/{0}/{1}-17Sep2018-{2}/MINIAOD".format(x[0],x[1][0],x[1][1]) for x in itertools.product(pds,proc_vers_2)]

    mc_samples = [
		"/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall18MiniAOD-102X_upgrade2018_realistic_v12_ext1-v1/MINIAODSIM", 
		#"/RelValZEE_13/CMSSW_10_2_5-PU25ns_102X_upgrade2018_realistic_v15_HS-v1/MINIAODSIM",
		"/RelValZEE_13/CMSSW_10_2_5-102X_upgrade2018_realistic_v15_Autumn18_HS-v1/MINIAODSIM"
		]

    cmsswver = "CMSSW_10_2_5"
    tarfile = tar_name

    for i in range(10000):

        total_summary = {}
        total_counts = {}
	for dsname in mc_samples:
            open_dataset = False
            task = CMSSWTask(
                    sample = DBSSample(dataset=dsname),
                    open_dataset = open_dataset,
                    flush = ((i+1)%48==0),
                    # flush = ((i)%48==0), 
                    events_per_output = 100e3,
                    output_name = "merged_ntuple.root",
                    tag = "CMS4_V10-02-01_allPfCands",
                    #tag = "CMS4_V00-00-06_allPfCands",
                    global_tag = "", # if global tag blank, one from DBS is used

                    #pset = "/home/users/sjmay/Commissioning2017/Commissioning2017_MET/BabyMaker/CMSSW_10_1_0/src/CMS3/NtupleMaker/test/main_pset.py",
                    pset = cmsswver + "/src/CMS3/NtupleMaker/test/main_pset.py",
                    pset_args = "data=False prompt=False relval=True" if "RelVal" in dsname else "data=False prompt=False",
                    cmssw_version = cmsswver,
                    publish_to_dis = False,
                    tarfile = tarfile,
                    is_data = False,
            )
            task.process()
            total_summary[dsname] = task.get_task_summary()

        for dsname in dataset_names:
	    if "DoubleMuon" in dsname and "Run2018D-PromptReco-v1" in dsname:
	      continue
            open_dataset = False
            task = CMSSWTask(
                    sample = DBSSample(dataset=dsname), 
                    open_dataset = open_dataset,
                    flush = ((i+1)%48==0), 
                    # flush = ((i)%48==0), 
                    events_per_output = 100e3,
                    output_name = "merged_ntuple.root",
		    tag = "CMS4_V10-02-01_allPfCands",
                    #tag = "CMS4_V00-00-06_allPfCands",
                    global_tag = "", # if global tag blank, one from DBS is used
                    
		    #pset = "/home/users/sjmay/Commissioning2017/Commissioning2017_MET/BabyMaker/CMSSW_10_1_0/src/CMS3/NtupleMaker/test/main_pset.py",
		    pset = cmsswver + "/src/CMS3/NtupleMaker/test/main_pset.py",
                    pset_args = "data=True prompt=True", 
                    cmssw_version = cmsswver,
		    publish_to_dis = False,
                    tarfile = tarfile,
                    is_data = True,
            )
            task.process()
            total_summary[dsname] = task.get_task_summary()

        #StatsParser(data=total_summary, webdir="~/public_html/dump/metis/").do()

        # time.sleep(1.*3600)
        time.sleep(10.*60)

