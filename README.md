limereg
=======

Lightweight Image Registration

Commandline based application and/or software development library, that performs a 2D, rigid image registration on two greyscale images and outputs either the transformation parameters or the registered image.

The parent project of 'limereg' is 'fimreg' (https://github.com/RoelofBerg/fimreg.git), which contains an approach for fast image registration on DSP coprocessors. Fimreg was meant as supplementary material for an article in a scientific journal and demands special and expensive hardware. This project - limereg - is a lightweight fork of fimreg where all code related to special hardware was removed. (It is no fork in a technical sense because I didn't see any benefit in having the hardware related stuff cluttering the version history).

Installation of the shell application on Ubuntu Linux:

sudo add-apt-repository ppa:roelofberg/limereg

sudo apt-get update

sudo apt-get install limereg

Installaton of the library version for software developers, or installation on other systems (MS Windows):
Build from source as written in the file install.txt

More information: see readme.txt

Imprint: Responsible: Roelof Berg Web: http://www.berg-solutions.de E-Mail: rberg@berg-solutions.de Postal Address: Adalbert-Stifter-Str. 19, 23562 Luebeck, Germany Git: https://github.com/RoelofBerg/limereg.git
