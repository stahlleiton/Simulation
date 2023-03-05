#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLegendEntry.h"


void plotDecayLength()
{
  // Enable multi-threading
  ROOT::EnableImplicitMT();

  // Create dataframe from genParticle tree
  ROOT::RDataFrame df("GenParticles", "../../GeneratorInterface/Pythia8Interface/output/Pythia8_MinimumBias_pp_14TeV.root");

  // Define map of particles
  std::map<std::string, int> pdgIdMap({{"K^{#pm}", 321}, {"K_{s}^{0}", 310}, {"K_{L}^{0}", 130}, {"#Sigma^{+}", 3222}, {"#Lambda", 3122}});

  // Make histogram for each particle
  std::map<std::string, ROOT::RDF::RResultPtr<TH1D> > histM;
  for (const auto& p : pdgIdMap) {
    // Select final state particles (status 1) within LHCb geometric acceptance
    const std::string sel = Form("status==1 && eta>2 && eta<5 && abs(pdgId)==%d", p.second);
    histM[p.first] = df.Define("dist", "decayLength["+sel+"]").Histo1D({p.first.c_str(), ";L [m];Arbitrary units", 33, 0, 12}, "dist");
  }

  // Format histograms
  gStyle->SetOptStat(0);
  gStyle->SetTextFont(42);
  std::map<std::string, int> COLOR({{"K^{#pm}", kGreen+1}, {"K_{s}^{0}", kBlue+1}, {"K_{L}^{0}", kViolet-6}, {"#Sigma^{+}", kRed+1}, {"#Lambda", kCyan-3}});
  for (auto& h : histM) {
    auto& hist = *h.second;
    // Normalize histogram
    hist.Scale(1./hist.Integral());
    // Edit style of histogram
    hist.SetLineColor(COLOR[h.first]);
    hist.SetLineWidth(2);
    hist.GetXaxis()->SetLabelSize(0.040);
    hist.GetXaxis()->SetTitleOffset(0.78);
    hist.GetXaxis()->SetTitleSize(0.055);
    hist.GetYaxis()->SetLabelSize(0.040);
    hist.GetYaxis()->SetTitleSize(0.055);
    hist.GetYaxis()->SetTitleOffset(0.78);
    hist.GetYaxis()->SetRangeUser(8E-5, 2);
  }

  // Draw histograms
  TCanvas c("", "", 1000, 800);
  histM.begin()->second->Draw("AXIS");
  for (auto& h : histM) h.second->Draw("HISTSAME");
  TLegend leg(0.75, 0.63, 0.90, 0.88);
  leg.SetBorderSize(0);
  for (auto& h : histM) leg.AddEntry(h.second.GetPtr(), h.first.c_str(), "l")->SetTextSize(0.040);
  leg.Draw("SAME");
  c.SetLogy();
  c.SaveAs("dist.png");
};
