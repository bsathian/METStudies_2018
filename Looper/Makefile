ROOTLIBS = $(shell root-config --libs) -lMinuit -lMLP -lXMLIO -lTMVA -lGenVector
#SHARED_LIBS = -LCORE -lCMS3_CORE

CC = g++
CFLAGS = -g -std=c++1y

#TARGET = analyze analyze_evt_weights analyzeTest
TARGET = analyze

all: $(TARGET)

analyze: ScanChain.C ScanChain.h analyze.cpp 
	$(CC) $(CFLAGS) -I${ROOTSYS}/include -o analyze analyze.cpp $(ROOTLIBS)

analyzeTest: ScanChainTest.C analyzeTest.cpp
	$(CC) $(CFLAGS) -I${ROOTSYS}/include -o analyzeTest analyzeTest.cpp $(ROOTLIBS)

analyze_evt_weights: ScanEvtWeights.C analyze_evt_weights.cpp
	$(CC) $(CFLAGS) -I${ROOTSYS}/include -o analyze_evt_weights analyze_evt_weights.cpp $(ROOTLIBS)
