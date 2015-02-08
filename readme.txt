limereg
=======

Lightweight Image Registration

Performs a highly efficient Image Registration. This is a supplementary software for our article in the
Springer Journal of Realtime Image Processing with the title: "Highly efficient image registration for
embedded systems using a distributed multicore DSP architecture" which is available for purchase at:
http://link.springer.com/article/10.1007/s11554-014-0457-3. There is also a preprint available free of charge
at http://embedded-software-architecture.com/Berg2014Highly_Preprint.pdf

If you want to understand what the application does this paper is the best starting point. Please be aware
that this software is a limited research prototype. If you need industrial quality please contact one of
the authors.

The original paper's software is called fimreg and was written for an Advantech DSPC-8681 PCIe expansion
card. This is a fork which does not need additional PCIe DSP hardware.
Fimreg: https://github.com/RoelofBerg/fimreg.git (Fast Image Registration, needs a DSPC-8681 PCIe card)

INSTALLATION: See install.txt

OPERATION:
- Start StartExampleRegistration.bat for an example registration.
- Execute limereg --help (on windows <limereg-install-folder>\x64\Release\limereg.exe --help) for more
  instructions how to perform your own image registrations. The images have to be 2D, square dimension
  (e.g. 1000x1000 pixels) and grayscale
