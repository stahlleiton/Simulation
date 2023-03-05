! 1) Settings used in the main program.
Main:numberOfEvents = 1000         ! number of events to generate

! 2) Settings related to output in init(), next() and stat().
Init:showChangedParticleData = on
Init:showChangedSettings = on      ! list changed settings
Next:numberCount = 100             ! print message every n events
Next:numberShowInfo = 1            ! print event information n times
Next:numberShowProcess = 1         ! print process record n times
Init:showProcesses = on

! 3) Beam parameter settings. Values below agree with default ones.
Beams:eCM = 14000.                 ! CM energy of collision

! 4) Settings for the process generation.
SoftQCD:nonDiffractive = on
310:oneChannel = 1 1.0 0 11 -11 11 -11
