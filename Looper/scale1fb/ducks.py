import time
import itertools
import json

from metis.Sample import DBSSample
from metis.CMSSWTask import CMSSWTask
from metis.StatsParser import StatsParser

import os, sys

import ROOT as r

input_jsons = ["mc_samples.json"]
output_json = "n_events.json"

with open(output_json, "r") as f_in:
  mc_samples = json.load(f_in)

def get_negative_events(files):
  n_events_neg = 0
  n_events_pos = 0
  idx = 0
  for file in files:
    idx += 1
    print ("checking file %d / %d" % (idx, len(files)))
    sys.stdout.write("\033[F")
    f = r.TFile.Open("root://cmsxrootd.fnal.gov//" + file.get_name())
    if not f:
      print "bad file: %s" % file.get_name()
      return -(10**12), -(10**12)
    tree = f.Get("Events")
    evt_neg = tree.GetEntries("GenEventInfoProduct_generator__SIM.obj.weights_ < 0")
    evt_pos = tree.GetEntries("GenEventInfoProduct_generator__SIM.obj.weights_ > 0")
    n_events_neg += evt_neg
    n_events_pos += evt_pos
  return n_events_neg, n_events_pos

for input_json in input_jsons:
  with open(input_json, "r") as f_in:
    datasets = json.load(f_in)
    for key, dict in datasets["processes"].iteritems():
      if key == "data":
	continue
      for sample in dict["datasets"]:
        if sample in mc_samples.keys():
          print "%s already found in samples list\n" % sample
          continue
        else:
          print "Adding %s to list of mc samples\n" % sample
          mc_samples[sample] = { "n_events_tot" : -1, "n_events_neg" : -1 , "n_events_pos" : -1}

for key, dict in mc_samples.iteritems():
  if dict["n_events_neg"] + dict["n_events_pos"] == dict["n_events_tot"]:
    #print "%s has trustworthy n_events data, skipping\n\n" % key
    continue
  elif dict["n_events_tot"] > 0 and dict["n_events_pos"] > 0:
    print "%s has already had n_event info calculated, but n_neg + n_pos != n_total" % key
    print "Probably want to check it manually"
    continue
  print "Calculating n_events for %s\n\n" % key
  sample = DBSSample(dataset=key)
  files = sample.get_files()
  n_events = 0
  for file in files:
    n_events += file.get_nevents()
  dict["n_events_tot"] = n_events
  dict["n_events_neg"], dict["n_events_pos"] = get_negative_events(files)
  if dict["n_events_neg"] + dict["n_events_pos"] == dict["n_events_tot"]:
    print "%s has: \n Total events: %d \n Positive Events: %d \n Negative Events: %d \n" % (key, dict["n_events_tot"], dict["n_events_pos"], dict["n_events_neg"])
  else:
    print "Some issue calculating n_events for %s: \n Total events: %d \n Positive Events: %d \n Negative Events: %d \n" % (key, dict["n_events_tot"], dict["n_events_pos"], dict["n_events_neg"])
  with open("n_events.json", "w") as f_out:
    json.dump(mc_samples, f_out, indent=4, sort_keys=True)

#for key, dict in mc_samples.iteritems():
#  print "%s   has   %d events" % (key, dict["n_events_tot"])
