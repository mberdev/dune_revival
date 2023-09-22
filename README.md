# dune_revival
Some C++ and ScummVM-compatible code that can decode resource files from the 1992 videogame "Dune" aka "Dune 1" by Cryo

# details

**This is actually several projects in one :**
- There is some unfinished code for adding the games of videogames company "Cryo" to the ScummVM engine (however the code in this repo focuses specifically on "Dune", aka "Dune 1")
- There is an unfinished standalone C++ project to create a clone of Dune 1.
- Interestingly, one of the projects includes a small voxel rendering (in the style of videogames Outcast or Comanche) inspired by the ornithopter travelling sequence in Dune.

# highlight

Some of the c++ functions let you unpack the HSQ file format used by Dune.

# how to compile and run

To compile this code, you need to perform the following:
1) Get the latest ScummVM source from its trunk, i.e.
https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk
2) Apply the scummvm_add_cryo.patch
3) Copy the cryo folder inside the /trunk/engines folder
4) Compile ScummVM, which will include the cryo engine!

THIS REPO DOES NOT CONTAIN THE RESOURCE FILES FOR DUNE. YOU NEED TO PURCHASE THEM LEGALLY!

Compilation instructions for ScummVM can be found in its relevant wiki section:
http://wiki.scummvm.org/index.php/Compiling_ScummVM
