#ifndef DataContainer_h
#define DataContainer_h

#include "Pythia8/Pythia.h"
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include <memory>
#include <vector>


class DataContainer
{
 public:
  DataContainer() : tree_(0) {};
  ~DataContainer() {};

  void init(const std::string& lbl)
  {
    // Define output file name
    auto name = lbl.substr(0, lbl.rfind("."))+".root"; // change file name to ROOT
    if (name.rfind("/")!=std::string::npos) name = name.substr(name.rfind("/")+1);
    const auto outputFileName = "output/"+name;
    gSystem->mkdir("output"); // make output directory

    // Create output file
    file_.reset(TFile::Open(outputFileName.c_str(), "RECREATE"));
    if (!file_ || !file_->IsOpen() || file_->IsZombie()) throw std::runtime_error("[ERROR] Failed to create output file!");

    // Create ROOT tree
    tree_ = new TTree("GenParticles", "GenParticles");
    if (!tree_) throw std::runtime_error("[ERROR] Failed to create GenParticles tree!");

    // Generate dictionary
    generateDictionary(); 

    // Initialize branches
    for (const auto& l : {"pdgId", "status", "charge"}) {
      tree_->Branch(l, &parVInt_[l]);
    }
    for (const auto& l : {"pT", "eta", "phi", "mass", "vxProd", "vyProd", "vzProd", "vtProd", "vxDecay", "vyDecay", "vzDecay", "vtDecay", "decayLength"}) {
      tree_->Branch(l, &parVFloat_[l]);
    }
    for (const auto& l : {"dauIdx", "momIdx"}) {
      tree_->Branch(l, &parVVInt_[l]);
    }
  };

  int add(const Pythia8::Event& event, const int& pIdx)
  {
    // Use the last copy
    const auto parIndex = event[pIdx].iBotCopyId();

    // Check if the particle has already been added
    const auto& parIt = particleList_.find(parIndex);

    // If yes, return the particle index
    if (parIt!=particleList_.end()) return parIt->second;

    // If not, add the particle information
    const auto& particle = event[parIndex];
    parVInt_["pdgId"].push_back(particle.id());
    parVInt_["status"].push_back(particle.statusHepMC());
    parVInt_["charge"].push_back(particle.charge());
    parVFloat_["pT"].push_back(particle.pT());
    parVFloat_["eta"].push_back(particle.eta());
    parVFloat_["phi"].push_back(particle.phi());
    parVFloat_["mass"].push_back(particle.m());
    parVFloat_["vxProd"].push_back(particle.xProd());
    parVFloat_["vyProd"].push_back(particle.yProd());
    parVFloat_["vzProd"].push_back(particle.zProd());
    parVFloat_["vtProd"].push_back(particle.tProd());
    parVFloat_["vxDecay"].push_back(particle.xDec());
    parVFloat_["vyDecay"].push_back(particle.yDec());
    parVFloat_["vzDecay"].push_back(particle.zDec());
    parVFloat_["vtDecay"].push_back(particle.tDec());
    parVFloat_["decayLength"].push_back(particle.tau()*particle.pAbs()/particle.m()/1000.);

    // initialize daughter and mother information
    parVVInt_["dauIdx"].emplace_back();
    parVVInt_["momIdx"].emplace_back();

    // Add particle to map
    const int index = parVInt_["pdgId"].size() - 1;
    particleList_[parIndex] = index;

    // Add daughters
    for (const auto& dauIndex : particle.daughterList()) {
      const auto& daughter = event[dauIndex];
      const auto& dauIdx = add(event, dauIndex);
      parVVInt_["dauIdx"][index].push_back(dauIdx);
    }

    // Add mothers
    const auto& firstCopy = event[particle.iTopCopyId()];
    for (const auto& momIndex : firstCopy.motherList()) {
      const auto& mother = event[momIndex];
      if (mother.isGluon()) continue;
      const auto& momIdx = add(event, momIndex);
      parVVInt_["momIdx"][index].push_back(momIdx);
    }

    // Return the particle index
    return index;
  };

  void fill()
  {
    if (!tree_) throw std::runtime_error("[ERROR] Filling null tree!");

    // Fill event information to tree
    tree_->Fill();

    // Clear data containers
    particleList_.clear();
    for (auto& p : parVInt_  ) p.second.clear();
    for (auto& p : parVFloat_) p.second.clear();
    for (auto& p : parVVInt_ ) p.second.clear();
  };

  void write()
  {
    if (!tree_) throw std::runtime_error("[ERROR] Writing null tree!");
    if (!file_ || !file_->IsOpen() || file_->IsZombie()) throw std::runtime_error("[ERROR] Output file is not open!");
    tree_->Write(tree_->GetName());
  };


 private:
  void generateDictionary()
  {
    const std::string currentDirectory = getcwd(NULL, 0);
    const std::string targetDirectory = currentDirectory + "/dict";
    const std::string includePath = gInterpreter->GetIncludePath();
    if (includePath.rfind(targetDirectory)!=std::string::npos) return;
    gSystem->mkdir(targetDirectory.c_str());
    gSystem->ChangeDirectory(targetDirectory.c_str());
    gInterpreter->AddIncludePath(targetDirectory.c_str()); // Needed to find the new dictionaries
    gInterpreter->GenerateDictionary("vector<vector<int>>", "vector");
    gSystem->ChangeDirectory(currentDirectory.c_str());
  };

  TTree* tree_;
  std::unique_ptr<TFile> file_;
  std::map<int, int> particleList_;
  std::map<std::string, std::vector<int> > parVInt_;
  std::map<std::string, std::vector<float> > parVFloat_;
  std::map<std::string, std::vector<std::vector<int> > > parVVInt_;

};

#endif // ifndef DataContainer_h
