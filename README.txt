# RWClustering
Performs and extends Rajaraman-Wong Algorithm and shows results in GUI form

To Run on Linux Machine:


(1) Copy entire dir into Linux machine


(2) cd RWClustering


(3) cmake .


(4) make


(5) ./rw

For running the GUI after the main C++ progrm generates an output file (let's call this file "graph.dmp")

(1) cp graph.dmp Python/

(2)cd Python/

(3)python RWGUI.py graph.dmp
