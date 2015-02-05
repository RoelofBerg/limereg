limereg
=======

Lightweight Image Registration

The github project 'fimreg' (https://github.com/RoelofBerg/fimreg.git) contains an approach for fast image registration. It is meant as supplementary material for an article in a scientific journal and demands special and expensive hardware. This project - limereg - is a lightweight fork of fimreg where all code related to special hardware was removed. (It is no fork in a technical sense because I didn't see any benefit in having the hardware related stuff cluttering the version history).

It is a commandline based application that performs a 2D, rigid image registration on two greyscale images and outputs either the transformation parameters or the registered image.

Installation on Ubuntu Linux:

sudo add-apt-repository ppa:roelofberg/limereg

sudo apt-get update

sudo apt-get install limereg

Installation on other systems (including MS Windows):
Build from source as written in the file install.txt

More information: see readme.txt

Imprint: Responsible: Roelof Berg Web: http://www.berg-solutions.de E-Mail: rberg@berg-solutions.de Postal Address: Adalbert-Stifter-Str. 19, 23562 Luebeck, Germany Git: https://github.com/RoelofBerg/limereg.git
