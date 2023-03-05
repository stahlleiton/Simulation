#include "TCanvas.h"
#include "TLegend.h"
#include "TLegendEntry.h"

void Acceptance_Mass()
{

	TFile *input = new TFile("Pythia8_KsToPiMuMu_pp_14TeV.root", "read"); //el archivo .root es el Pyhia8 etc. 
	TTree *tree = (TTree*)input->Get("GenParticles"); //El tree en si se llama GenParticles
	
	std::vector<float> eta;  //vectores
	std::vector<std::vector<int>> momldx;  //matriz
	std::vector<int>pdgId;
	
	tree->SetBranchAddress("momldx", &momldx); //la info de esto es en un branch no se si asi se accesa o branch o esto es para leaves 
	tree->SetBranchAddress("eta", &eta);
	tree->SetBranchAddress("pdgId", &pdgId);
	
	int entries = (int)tree->GetEntries();
	int accepted = 0;
	int counter =0;
	double acceptance_factor;
	
	for (int i=0; i<entries; i++) {
		tree->GetEntry(i);
		for (int k=0; k<momldx.size(); k++) {
			auto mothers = momldx[k];
			for (int m=0; m<mothers.size(); m++) {
				auto index =mothers[m];
				auto idm = pdgId[index];  //id de la madre numero m
				if (idm == 310) { 
					counter = counter +1;
					auto part_eta = eta[k];
					if (part_eta>2 && part_eta<5) {
					accepted = accepted +1;
					std::cout<<accepted<<endl;
					}				
				}
			}
		}	
	}
	
	acceptance_factor = accepted/counter;
	std::cout<<acceptance_factor<<endl;
	
	input->Close();
}
