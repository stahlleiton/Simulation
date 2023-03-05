import ROOT

# Enable multi-threading
ROOT.ROOT.EnableImplicitMT()

# Create dataframe from genParticle tree
df = ROOT.RDataFrame("GenParticles", "../../GeneratorInterface/Pythia8Interface/output/Pythia8_MinimumBias_pp_14TeV.root")

# Define map of particles
pdgIdMap = {"K^{#pm}": 321, "K_{s}^{0}": 310, "K_{L}^{0}": 130, "#Sigma^{+}": 3222, "#Lambda": 3122}

# Make histogram for each particle
histM = {}
for key, value in pdgIdMap.items():
    # Select final state particles (status 1) within LHCb geometric acceptance
    sel = "status==1 && eta>2 && eta<5 && abs(pdgId)=="+str(value)
    histM[key] = df.Define("dist", "decayLength["+sel+"]").Histo1D((key, ";L [m];Arbitrary units", 33, 0, 12), "dist")

# Format histograms
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetTextFont(42)
COLOR = {"K^{#pm}": ROOT.kGreen+1, "K_{s}^{0}": ROOT.kBlue+1, "K_{L}^{0}": ROOT.kViolet-6, "#Sigma^{+}": ROOT.kRed+1, "#Lambda": ROOT.kCyan-3}
for key, hist in histM.items():
    # Normalize histogram
    hist.Scale(1./hist.Integral())
    # Edit style of histogram
    hist.SetLineColor(COLOR[key])
    hist.SetLineWidth(2)
    hist.GetXaxis().SetLabelSize(0.040)
    hist.GetXaxis().SetTitleOffset(0.78)
    hist.GetXaxis().SetTitleSize(0.055)
    hist.GetYaxis().SetLabelSize(0.040)
    hist.GetYaxis().SetTitleSize(0.055)
    hist.GetYaxis().SetTitleOffset(0.78)
    hist.GetYaxis().SetRangeUser(8E-5, 2)

# Draw histograms
c = ROOT.TCanvas("", "", 1000, 800)
next(iter(histM.values())).Draw("AXIS")
for hist in histM.values(): hist.Draw("HISTSAME")
leg = ROOT.TLegend(0.75, 0.63, 0.90, 0.88)
leg.SetBorderSize(0)
for key, hist in histM.items(): leg.AddEntry(hist.GetPtr(), key, "l").SetTextSize(0.040)
leg.Draw("SAME")
c.SetLogy()
c.SaveAs("dist_python.png")
