import sys
def loadDatasets():
    dslist = open("metstudies_datasets.txt")
    datasets = {}
    currKey = None
    for line in dslist:
        if line[0] == "#": #Commenting out dataset names
            continue
        if line[0:2] == "ds":
            key = line.rstrip("\n")[3:]
            currKey = key
            if key not in datasets:
                datasets[key] = []
        else:
            if line.rstrip("\n") != "":
                datasets[currKey].append(line.rstrip("\n"))

    return datasets


def get(data = [],mc = [],year = []):

    allDatasets = loadDatasets()
    yearDatasets = {}

    if type(year) is int:
        print(year)
        year = [year]
    if type(year) is list:
        for y in year:
            for dsname,datasets in allDatasets.items():
                if str(y) in dsname:
                    yearDatasets[dsname] = datasets
    if year == []:
        print("No year")
        yearDatasets = allDatasets #No year specified

    if data and mc:
        print("WTF!")
        sys.exit(1)


    if type(data) is not list:
        data = [data]
    if type(mc) is not list:
        mc = [mc]

    finalDataset = []

    #Assumption : I don't fuck up grouping datasets and ensure MCs are with MCs and data with data
    if data[0] == "all":
        for dsname,datasets in yearDatasets.items():
            if "SIM" not in datasets[0]:
                finalDataset.append(datasets)
    elif mc[0] == "all":
        for dsname,datasets in yearDatasets.items():
            if "SIM" in datasets[0]:
                finalDataset.append(dataset)

    elif len(data) > 0:
        for dataEntry in data:
            searchString = dataEntry
            for dsname,datasets in yearDatasets.items():
                if searchString in dsname:
                    finalDataset.extend(datasets)

    elif len(mc) > 0:
        print("MC")
        for dataEntry in mc:
            searchString = dataEntry
            for dsname,datasets in yearDatasets.items():
                if searchString in dsname:
                    finalDataset.extend(datasets)

    else: #All datasets for that year
        for dsname,datasets in yearDatasets.items():
            finalDataset.exted(datasets)


    return finalDataset

if __name__ == "__main__":
    #print(get(mc = ["SingleTop","GammaJets","WGamma"],year = 2017))
    print(get(mc = "DoubleEG",year = 2017))
