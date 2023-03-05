1) After downloading the files of the repository unto a pertinent directory, open the setupSoftware file. Copy line #6 into your terminal and run it in orden to download the programs needed for the generator and the analysis. 
2) When the download is done check that the programs where actually installed. 
3) Now you can run setupSoftware.sh from the main directory; this could take some time so go read a book or something.
4) Check that all the program files are inside the Software directory. It should contain:EvtGen, HepMc3, LHAPDF, PHOTOS, Pythia8, root, and zlib. 
5) Now run "source env.sh" from the main directory. 
6) Navigate to Generator Interface directory and from there go to the Pythia8Interface directory. On this directory you will find several subdirectories and a make file. Type "make" in your terminal. This will compile all the files located in the src directory, and place the respective executables in the bin directory.  
7) To run the generator a card from the "cards" subdirectory has to be selected in order for the program to know which cmd file has the parameters for the particle events. For example, you could type into the terminal: "bin/Pythia8Hadronizer cards/Pythia8_MinimumBias_pp_14TeV.cmd". 
8) Now that the data has been generated, return to the main directory and run "source env.sh" again. 
9) In the Analysis directory are the programs which use the data produced by the generator in order to display the results you wish to analyze. For example, the DecayLength subdirectory has the plotDecayLength program which creates a histogram of the decay length of several particles. In order to run this from the DecayLength subdirectory you should type in the terminal: root -l -b -q plotDecayLength.cpp.
