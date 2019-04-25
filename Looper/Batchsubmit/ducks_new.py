import sys, os
import time
import itertools
import numpy
import METStudies_datasets as METStudies
import argparse

from metis.Sample import DirectorySample,SNTSample
from metis.CondorTask import CondorTask
from metis.StatsParser import StatsParser

exec_path = "condor_exe.sh"
tar_path = "package.tar.gz"
hadoop_path = "METStudies"
job_tag = "METStudies"
year = 2017

def get_weights(dataset):
    try:
        scalefile = open("../../CORE/Tools/datasetinfo/scale1fbs.txt","r")
    except:
        print("scale1fb.txt not found! Using scale1fb = 1")
        return 1

    scale1fb = 1
    for i in scalefile:
        if dataset in i.split()[0]:
            scale1fb = float(i.split()[5])
            break
    return scale1fb

parser = argparse.ArgumentParser()
parser.add_argument("--runF_only", help = "Run only on data from Run F and no mc", action="store_true")
parser.add_argument("--reweight", help = "(Re)derive pileup weights", action="store_true")
parser.add_argument("--validation", help = "Run only on 2017F DoubleMuon 949 to validate MET recipe", action="store_true")
parser.add_argument("--validate_recipe", help = "Validate recipe in CMS4", action="store_true")
parser.add_argument("--test_run", help = "Just submit for 1 MC sample", action="store_true")
parser.add_argument("--data_only", help = "Just submit jobs for data", action="store_true")
args = parser.parse_args()

eras = ["eraB","eraC","eraD","eraE","eraF"]
datasetsToProcess = None
if args.validation:
    datasetsToProcess = METStudies.get(data = ["DoubleMuon_eraF"],year = year)
elif args.data_only:
    datasetsToProcess = METStudies.get(data = "all",year = year)
else:
    mcToProcess = METStudies.get(mc = "all",year = year)
    dataToProcess = METStudies.get(data = eras,year = year)


while True:
    all_jobs_done = True
    for dsname in mcsToProcess:
        if args.validation:
            break
        sample = SNTSample(dataset = dsname)
        weight1fb = get_weights(dsname)

        if args.validate_recipe:
            job_args = 'none 7 1'
            output_name = "Zll_histograms.root"
        elif not args.reweight:
            job_args = "%s %d %.12f" %("Reweight", 1, weight1fb)
            output_name = "Zll_histograms.root"

        else:
            job_args = "%s %d %.12f" % ("none", 2, weight1fb)
            output_name = "Zll_histograms_All.root"

        task = CondorTask(
                sample = sample
                open_dataset = False,
                flush = True,
                files_per_output = 1,
                output_name = output_name,
                tag = job_tag,
                executable = exec_path,
                tarfile = tar_path,
                condor_submit_params = {"sites":"T2_US_UCSD"},
                special_dir = hadoop_path,
                )
        task.process()
        if not task.complete():
            print("Job %s still running." % job_tag)
            all_jobs_done = False
        total_summary[dsname] = task.get_task_summary()
        if task.complete():
            print "Job={} finished".format(job_tag)

    for dsname in dataToProcess:
        if args.test_run:
            continue
        if args.validation:
            if "DoubleEG" in dsname:
                continue
        sample = SNTSample(dataset = dsname)

        if args.validation:
            job_args = 'none 5 1'
        elif args.validate.recipe:
            job_args = 'none 7 1'
        elif not args.reweight:
            job_args = 'none 1 1'
        else:
            job_args = 'none 2 1'

        task = CondorTask(
                sample = sample
                open_dataset = False,
                flush = True,
                files_per_output = 1,
                output_name = output_name,
                tag = job_tag,
                executable = exec_path,
                tarfile = tar_path,
                condor_submit_params = {"sites":"T2_US_UCSD"},
                special_dir = hadoop_path,
                )
        task.process()
        if not task.complete():
            print("Job %s still running." % job_tag)
            all_jobs_done = False
        total_summary[dsname] = task.get_task_summary()
        if task.complete():
            print ""
            print "Job={} finished".format(job_tag)

    if all_jobs_done:
        print "All jobs finished!"
        break
    StatsParser(data = total_summary, webdir = "~/public_html/dump/METStudies/").do()
    os.system("chmod -R 755 ~/public_html/dump/METStudies")
    print "Sleeping 300 seconds..."
    time.sleep(300)











