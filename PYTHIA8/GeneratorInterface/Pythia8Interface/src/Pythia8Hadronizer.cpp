#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/EvtGen.h"
#include "../include/DataContainer.h"
#include <sys/stat.h>


bool existFile(const std::string& file)
{
  struct stat buffer;
  return (stat (file.c_str(), &buffer) == 0);
};


int main(int argc, char *argv[])
{
  // Get input parameters
  if (argc<2 || argc>3) {
    std::cout << "[ERROR] Missing configuration file" << std::endl;
    std::cout << "Use this program as 'Pythia8Hadronizer CONFIG_FILE.cmd <CONFIG_FILE.dec>'" << std::endl;
    std::cout << "Use .dec card in case you want to use EvtGen!" << std::endl;
    return 1;
  }
  std::string configPythia, configEvtGen;
  for (int i=1; i<argc; i++) {
    const std::string& arg = argv[i];
    // Set configuration file
    if      (arg.rfind(".cmd")!=std::string::npos) configPythia = arg;
    else if (arg.rfind(".dec")!=std::string::npos) configEvtGen = arg;
    else throw std::runtime_error("[ERROR] Invalid configuration file "+arg);
    // Check if configuration file exists
    if (!existFile(arg)) throw std::runtime_error("[ERROR] Configuration file "+arg+" not found!");
  }

  // Check configuration files
  if (configPythia=="") throw std::runtime_error("[ERROR] Pythia configuration not defined!");
  if (!existFile("conf/LHCbDefault.cmd")) throw std::runtime_error("[ERROR] Default LHCb configuration file not found!");
  if (configEvtGen!="") {
    if (!existFile("conf/DECAY.DEC")) throw std::runtime_error("[ERROR] EvtGen decay file not found!");
    if (!existFile("conf/evt.pdl")) throw std::runtime_error("[ERROR] EvtGen particle data file not found!");
  }

  // Define generators
  Pythia8::Pythia pythia;
  std::unique_ptr<Pythia8::EvtGenDecays> evtgen;

  // Read configuration files
  pythia.readFile("conf/LHCbDefault.cmd");
  pythia.readFile(configPythia.c_str());

  // Extract settings to be used in the main program
  const auto nEvent = pythia.mode("Main:numberOfEvents");

  // Create data container
  DataContainer data;

  // Initialize
  pythia.init();
  data.init(configPythia);
  if (configEvtGen!="") {
    evtgen.reset(new Pythia8::EvtGenDecays(&pythia, "conf/DECAY.DEC", "conf/evt.pdl"));
    evtgen->readDecayFile(configEvtGen);
  }

  // Loop over generated events
  for (size_t iEvent = 0; iEvent < nEvent; iEvent++) {
    // Generate the event
    if (!pythia.next()) continue;

    // Perform the decays with EvtGen
    if (evtgen) evtgen->decay();
    
    // Loop over particles in the event
    const auto& event = pythia.event;
    for (size_t parIndex=0; parIndex<event.size(); parIndex++) {
      const auto& particle = event[parIndex];
      // Select final state particles
      if (!particle.isFinal()) continue;
      // Select hadrons or leptons
      if (!(particle.isLepton() || particle.isHadron())) continue;
      // Select particles within LHCb geometric acceptance
      if (!(particle.eta()>2 && particle.eta()<5)) continue;
      // Add particle to data container
      data.add(event, parIndex);
    }

    // Fill tree
    data.fill();
  }

  // Store tree
  data.write();

  return 0;
}
