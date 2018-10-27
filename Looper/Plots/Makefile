ROOTLIBS = $(shell root-config --libs) -lMinuit -lMLP -lXMLIO -lTMVA -lGenVector

CC = g++
CFLAGS = -g -std=c++1y

TARGET = makePlots 

all: $(TARGET)

makePlots: makePlots.cpp PlotHelper.h
	$(CC) $(CFLAGS) -I${ROOTSYS}/include -o makePlots makePlots.cpp $(ROOTLIBS)
