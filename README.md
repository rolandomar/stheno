Stheno 
======
Copyright 2012 Rolando Martins, CRACS & INESC-TEC, DCC/FCUP

This project includes software developed during the PhD of Rolando
Martins, supervised by Fernando da Silva and Luis Lopes, at CRACS &
INESC-TEC, DCC/FCUP, with the collaboration of Prof. Priya Narasimhan
and her research group at Carnegie Mellon University.

This software contains code derived from ACE(tm), which is copyrighted
by Douglas C. Schmidt and his research group at Washington University,
University of California, Irvine, and Vanderbilt University, Copyright
(c) 1993-2009, all rights reserved.

Operating system abstraction provided by the ACE framework, which is
open-source software copyrighted by Douglas C. Schmidt and his
research group at Washington University, University of California,
Irvine, and Vanderbilt University. The original software is available
from:
http://www.cs.wustl.edu/~schmidt/ACE.html

Configuration support is provided by the libconfig++, which is
open-source software, written and copyrighted by Mark Lindner. The
original software is available from:
http://www.hyperrealm.com/libconfig/

Unit tests support is provided by libunit++ which is open-source
software, written and copyrighted by Claus Draeb and David Hallas. The
original software is available from:
http://sourceforge.net/projects/unitpp/

Compiling:
==========

* $ git clone https://github.com/rolandomar/stheno.git
* $ cd stheno
* $ mkdir build && cd build
* $ cmake .. && make (or make -j#of_processors (e.g., make -j4))

Generating documentation:
=========================

The default configuration generates both latex and html documents.
* $ cd stheno
* $ doxygen doxygen.conf

