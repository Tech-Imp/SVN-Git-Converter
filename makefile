#Makefile for SVN-GIT automation
.SUFFIXES: .cpp
.cpp.o:
	g++ -c $(CXXFLAGS) $<

CXXFLAGS = -g -Wall -I../.. -static-libgcc -static-libstdc++  -std=gnu++11 

#object files- the meat and bones prior to creation
OBJ =  tools.o core.o syscheck.o hgconvert.o svnconvert.o

program: $(OBJ)
	g++ $(CXXFLAGS) -o convert-to-git $(OBJ)

clean:
	rm -f $(OBJ) program


#Dependencies to create objects
tools.o: 	tools.cpp tools.hpp
core.o: 	core.cpp syscheck.hpp
syscheck.o: syscheck.cpp syscheck.hpp
hgconvert.o: hgconvert.cpp hgconvert.hpp
svnconvert.o: svnconvert.cpp svnconvert.hpp
