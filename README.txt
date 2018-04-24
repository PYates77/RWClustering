RWClustering Application
Authors: Akshay Nagendra <akshaynag@gatech.edu>
         Paul Yates <paul.maxyat@gatech.edu>

Description: C++ implementation of the Rajaraman-Wong clustering algorithm designed to cluster gates in a complex input circuit netlist while minimizing the increase in critical delay through the circuit

Instructions for execution:
(1) Make sure you have unzipped version of the RWClustering directory
(2) Execute the following commands:
    >>> cmake .
    >>> make
    >>> chmod +x RWCExecute.csh
(3) Execute the RWCExecute.csh script (>>> ./RWCExecute.csh --help to see all the possible options the RWClustering application can be run with)
  (a) Demo execution (if using SSH and X11 forwarding to run on an UNIX machine): ./RWCExecute.csh example_lecture.blif --s 4 --c 3 --gui --x11
  (b) Demo execution (if running locally on an UNIX machine): ./RWCExecute.csh example_lecture.blif --s 4 --c 3 --gui --native
  
Further resources:
  Please consult the docs/ subdirectory for documentation about the project and a manual on how to use the interactive GUI (RWGUI.py)
