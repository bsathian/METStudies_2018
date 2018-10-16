import time
import itertools
import json

from metis.Sample import DBSSample
from metis.CMSSWTask import CMSSWTask
from metis.StatsParser import StatsParser

if __name__ == "__main__":


    pds = ["DoubleMuon", "EGamma"]
    proc_vers = [
	("Run2018A", "v1"),
	("Run2018A", "v2"),
	("Run2018A", "v3"),
	("Run2018B", "v1"),
	("Run2018B", "v2"),
	("Run2018C", "v1"),
        ("Run2018C", "v2")
	]	

    dataset_names = ["/{0}/{1}-PromptReco-{2}/MINIAOD".format(x[0],x[1][0],x[1][1]) for x in itertools.product(pds,proc_vers)]

    cmsswver = "CMSSW_10_1_0"
    #tarfile = "/nfs-7/userdata/libCMS3/lib_CMS4_V00-00-06.tar.gz"
    tarfile = "/home/users/sjmay/Metis/ProjectMetis/scripts/lib_CMS4_V10-01-00.tar.gz"

    for i in range(10000):

        total_summary = {}
        total_counts = {}
        for dsname in dataset_names:


            open_dataset = False

            task = CMSSWTask(
                    sample = DBSSample(dataset=dsname),
                    open_dataset = open_dataset,
                    flush = ((i+1)%48==0), 
                    # flush = ((i)%48==0), 
                    events_per_output = 100e3,
                    output_name = "merged_ntuple.root",
		    tag = "CMS4_V10-01-00_allPfCands",
                    #tag = "CMS4_V00-00-06_allPfCands",
                    global_tag = "", # if global tag blank, one from DBS is used
                    #pset = "/home/users/sjmay/Commissioning2017/NTupling/CMSSW_9_4_0/src/CMS3/NtupleMaker/test/main_pset.py",
		    pset = "/home/users/sjmay/Commissioning2017/Commissioning2017_MET/BabyMaker/CMSSW_10_1_0/src/CMS3/NtupleMaker/test/main_pset.py",
                    pset_args = "data=True prompt=True",
                    cmssw_version = cmsswver,
		    publish_to_dis = False,
                    tarfile = tarfile,
                    is_data = True,
            )
            
            task.process()

            total_summary[dsname] = task.get_task_summary()

        StatsParser(data=total_summary, webdir="~/public_html/dump/metis1/").do()

        # time.sleep(1.*3600)
        time.sleep(10.*60)

