 _ _                               
| (_)_ __ ___   ___ _ __ ___  __ _ 
| | | '_ ` _ \ / _ \ '__/ _ \/ _` |
| | | | | | | |  __/ | |  __/ (_| |
|_|_|_| |_| |_|\___|_|  \___|\__, |
                             |___/ 

Lightweight IMagE REGistration

Open source and commandline based application and/or software development library, that performs a 2D, rigid
image registration on two greyscale images and outputs either the transformation parameters or the registered
image.

YOU DOWNLOADED A PACKAGING PROJECT

Limereg is based on the GNU build environment (autotools). This file here is NO part of the release distribution.
Instead it is the autotools environment for packaging source and binary distributions. You probably don't
want to package the project, and (if this is the case, if you only want to USE the library) you should better
use the official binary-packages or the source-distribution-tarball releases from:

  http://embedded-software-architecture.com/limereg.html

The file README not related to this packaging project, it is an input file for generating the
source-distributions-tarballs and its content - esp. its installation instructions - will only work when
a source-distribution-tarball from the link above is used.

BUILD AND INSTALLATION

If you're really sure you DO want to package, and you DON'T want to use a source-distribution-tarball from the
link above you can for example package a source-tarball as follows:

Prerequisites for packaging:
GNU compiler, autotools, boost-program-options, opencv, help2man and doxygen.
An apt-get commandline for all prerequisites can be found in the file .travis.yml (very last line).
The lib can be built lateron from the source-tarball just with the GNU compiler as prerequisite.
Doxygen is optional for the lib manpage. Boost, OpenCV and help2man are a prerequisite only for the
(optional) command-line utility.

To create a source tarball from this git repository use:
 ./generate-tarball-distro.sh

If you want to install binaries directly from this repo (better use a release source-tarball as mentioned above):
 ./autogen.sh
 ./configure CFLAGS="-Ofast" CXXFLAGS="-Ofast"
 make
 make check
 sudo make install

WHEN YOU MODIFY FILES

When changing some autotools specific files, checkin your changes to GIT (important !), then execute the script
autoclean.sh (which does a git cleanup that also purges any uncommited changes !), then autogen.sh. Also the
generate-tarball-distro.sh script internally calls ./autoclean.sh, thus also here checkin befor execution !

Don't forget to call autoscan from time to time and in case of warnings: kdiff3 configure.ac configure.scan 
