standalone_ss316h: standalone_ss316h.C SS316ROMPrediction.C SS316ROMPrediction.h ROMBase.C ROMBase.h
	g++ -c ROMBase.C
	g++ -c SS316ROMPrediction.C
	g++ -c standalone_ss316h.C
	g++ -o standalone_ss316h ROMBase.o SS316ROMPrediction.o standalone_ss316h.o

clean:
	rm -f standalone_ss316h *.o
