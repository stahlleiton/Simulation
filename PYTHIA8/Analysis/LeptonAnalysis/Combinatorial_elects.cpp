#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "Math/Vector4D.h"
#include <vector>


typedef ROOT::Math::PtEtaPhiMVector PtEtaPhiMVector;
PtEtaPhiMVector smearMomentum(const PtEtaPhiMVector& v, const float& pTRes=0.01, const float& etaRes=0, const float& phiRes=0);


void Combinatorial_elects()
{
	TFile *input = new TFile("Pythia8_KsTo4e_pp_14TeV.root", "read"); //el archivo .root es el Pyhia8 etc. 
    if (!input || !input->IsOpen() || input->IsZombie()) throw std::logic_error("[ERROR] Input file not found!");

    TTree *tree = (TTree*)input->Get("GenParticles"); //El tree en si se llama GenParticles
    if (!tree) throw std::logic_error("[ERROR] Input tree not found!");


    gInterpreter->GenerateDictionary("vector<vector<int>>", "vector");
    

    const double eleMass = 0.00051099895000; // GeV
    
    std::vector<float>* eta(0);  //vectores
    std::vector<std::vector<int>>* dauIdx(0);  //matriz
    std::vector<int>* pdgId(0);
    std::vector<float>* pT(0);
    std::vector<float>* phi(0);
    std::vector<int>* charge(0);
    std::vector<int>* status(0);
    

    //set up relevant branches to the TTree
    tree->SetBranchAddress("dauIdx", &dauIdx); 
    tree->SetBranchAddress("eta", &eta);
    tree->SetBranchAddress("pdgId", &pdgId);
    tree->SetBranchAddress("pT", &pT);
    tree->SetBranchAddress("phi", &phi);
    tree->SetBranchAddress("charge", &charge);
    tree->SetBranchAddress("status", &status);
    
    //define mass histogram
    TH1D hMass("hMass", "Four-electron mass", 100, 0.4, 0.6);
    
    //loop over events
    const auto entries = tree->GetEntries();
    for (Long64_t i=0; i<entries; i++){
   	    tree->GetEntry(i);

        //create needed vectors
        PtEtaPhiMVector total_elects;

        //select electrons
        std::vector<size_t> electron_list;
        electron_list.reserve(pT->size());
   	    for (size_t k=0; k<pT->size() ; k++){
            auto stat = (*status)[k];
   	        auto idm = std::abs((*pdgId)[k]);
            auto p_eta = (*eta)[k];
   	        if (stat==1 && idm==11 && (p_eta>2 && p_eta<5)){
                electron_list.push_back(k);
            }
   	    }
                  
        //permuate over all combinations of four electrons
        const auto& nEl = electron_list.size();
   	    for (size_t i1=0; i1<nEl; i1++){
            const auto& el1 = electron_list[i1];
            const auto vEl1 = smearMomentum({(*pT)[el1], (*eta)[el1], (*phi)[el1], eleMass});

            for (size_t i2=i1+1; i2<nEl; i2++){
                const auto& el2 = electron_list[i2];
                const auto vEl2 = smearMomentum({(*pT)[el2], (*eta)[el2], (*phi)[el2], eleMass});

                //require electron 1 and 2 to have opposite electric charge (+-)
                if ((*charge)[el1] == (*charge)[el2]) continue;

                for (size_t i3=i2+1; i3<nEl; i3++){
                    const auto& el3 = electron_list[i3];
                    const auto vEl3 = smearMomentum({(*pT)[el3], (*eta)[el3], (*phi)[el3], eleMass});

                    for (size_t i4=i3+1; i4<nEl; i4++){
                        const auto& el4 = electron_list[i4];
                        const auto vEl4 = smearMomentum({(*pT)[el4], (*eta)[el4], (*phi)[el4], eleMass});

                        //require electron 3 and 4 to have opposite electric charge (+-)
                        if ((*charge)[el3] == (*charge)[el4]) continue;

                        //derive four-electron mass
                        const auto mass = (vEl1 + vEl2 + vEl3 + vEl4).M();

                        //fill histogram
                        hMass.Fill(mass);
                    }
                }
            }
        }
    }

  
    TCanvas c("c", "", 1000, 1000);
    hMass.Draw();
    c.SaveAs("hMass.png");
    
    input->Close();
}


PtEtaPhiMVector smearMomentum(const PtEtaPhiMVector& v, const float& pTRes, const float& etaRes, const float& phiRes)
{
    //smear pT
    const auto pT = (pTRes>0 ? gRandom->Gaus(v.Pt(), pTRes*v.Pt()) : v.Pt());
    //smear eta
    const auto eta = (etaRes>0 ? gRandom->Gaus(v.Eta(), etaRes) : v.Eta());
    //smear phi
    const auto phi = (phiRes>0 ? gRandom->Gaus(v.Phi(), phiRes) : v.Phi());
    //return smeared momentum
    return PtEtaPhiMVector(pT, eta, phi, v.M());
}
