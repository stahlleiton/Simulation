// main03.cc is a part of the PYTHIA event generator.
// Copyright (C) 2020 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Keywords: basic usage; process selection; command file; python; matplotlib;
// main03.cc is a part of the PYTHIA event generator.
// Copyright (C) 2020 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Keywords: basic usage; process selection; command file; python; matplotlib;

// This is a simple test program.
// It illustrates how different processes can be selected and studied.
// All input is specified in the main03.cmnd file.
// Also illustrated output to be plotted by Python/Matplotlib/pyplot.

#include "Pythia8/Pythia.h"

using namespace Pythia8;

//==========================================================================

int main() {

  // Generator.
  Pythia pythia;

  // Shorthand for the event record in pythia.
  Event& event = pythia.event;

  // Read in commands from external file.
  pythia.readFile("main03.cmnd");

  // Extract settings to be used in the main program.
  int nEvent = pythia.mode("Main:numberOfEvents");
  int nAbort = pythia.mode("Main:timesAllowErrors");

  // Initialize.
  pythia.init();

  // Book histograms.
  //Histograms of the distance travelled by diferent particles
  
  /*map<int, Hist> hmap;
  for (auto i: {321, 3222, 3122, 130, 321});*/
  
  Hist KaonsS("Distance travelled by $Kaon_{s}$", 33, 0. , 50.);  //probar con bins de 100 o de 500
  Hist Sigmaplus("Distance travelled by $Sigma^{+}$", 33, 0., 50.);
  Hist Lambdazero("Distance travelled by $Lambda_{0}$", 33, 0., 50.);
  Hist KaonsL("Distance travelled by $Kaon^{0}_{L}$", 33, 0., 50.);
  Hist Kaonsplus("Distance travelled by $Kaon^+$", 33, 0., 50.);
   

  // Begin event loop.
  int iAbort = 0;
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {

    // Generate events. Quit if many failures.
    if (!pythia.next()) {
      if (++iAbort < nAbort) continue;
      cout << " Event generation aborted prematurely, owing to error!\n";
      break;
    }

   
    // Loop over final particles in the event.
    
    
   
    for (int i = 0; i < event.size(); ++i) if (event[i].isFinal()) {
    

	
	if (event[i].id() != 310 && event[i].id() != 3222 && event[i].id() != 3122 && event[i].id() != 130 && event[i].id() != 321) continue; //selecting pertinent particles
	
	//cout << "Paso 1, eta: " << event[i].eta() << endl;
	if (event[i].eta() <= 2 || event[i].eta() >= 5) continue;  //LHCb geometrical acceptance
	

	//cout << "Paso 2, prtid: " << event[i].id() << endl;
	
	//Goes through all the particles to calculate the distance travelled and fill the histograms
	
	// dist = event[i].tau()
	
	if (event[i].id() == 310) {  //Checks for K short
	
	double dist_ks = ((event[i].tau()*event[i].pAbs())/(event[i].m()))/1000;	
	KaonsS.fill(dist_ks);
	
	} else if (event[i].id() == 3222) { //checks for Sigma+
	
	double dist_s = ((event[i].tau()*event[i].pAbs())/(event[i].m()))/1000;
	Sigmaplus.fill(dist_s);
 
	} else if (event[i].id() == 3122) { //checks for Lambda0
	
	double dist_l0 = ((event[i].tau()*event[i].pAbs())/(event[i].m()))/1000;
	Lambdazero.fill(dist_l0); 
	
	} else if (event[i].id() == 130) { //checks for K0 long
	
	double dist_kl = ((event[i].tau()*event[i].pAbs())/(event[i].m()))/1000;
	KaonsL.fill(dist_kl);
	
	} else { //checks for K+
	
	double dist_kplus = ((event[i].tau()*event[i].pAbs())/(event[i].m()))/1000;
	Kaonsplus.fill(dist_kplus);
	
	}
	
	}
	
		
       // Analyze all particles.
     
  // End of event loop.
  }

	KaonsS.normalize();
	Sigmaplus.normalize();
	Lambdazero.normalize();
	KaonsL.normalize();
	Kaonsplus.normalize();
	
  // Final statistics. Normalize and output histograms.
  pythia.stat();
  
  cout << KaonsS << Sigmaplus << Lambdazero << KaonsL << Kaonsplus << endl;
   

  // Write Python code that can generate a PDF file with the spectra.
  // Assuming you have Python installed on your platform, do as follows.
  // After the program has run, type "python main03plot.py" (without the " ")
  // in a terminal window, and open "out03plot.pdf" in a PDF viewer.
  // Colours and other choices can be omitted, but are shown as illustration.
  
  HistPlot hpl("main_allpartsplot");   
  hpl.frame("out_allparts", "Histogram of distance travelled by particles", "Distance (m)", "");
  hpl.add(KaonsS, "h", "$K_s$");
  hpl.add(Sigmaplus, "h" , "$\\Sigma ^+$");
  hpl.add(Lambdazero, "h" , "$\\Lambda_0$");
  hpl.add(KaonsL, "h" , "$K_l$");
  hpl.add(Kaonsplus, "h", "$K^+$");
  hpl.plot(true);
  
  
   
  
  // Done.
  return 0;
}

