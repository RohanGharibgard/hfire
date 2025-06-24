/*
 * Table of good seeds for pseudo-random number generators.
 * This is a port of SeedTable.h used in http://wwwinfo.cern.ch/asd/lhc++/clhep CLHEP 1.4.0 (C++).
 * CLHEP's implementation, in turn, is part of http://wwwinfo.cern.ch/asd/geant/geant4.html GEANT 4,
 * a C++ simulation toolkit for High Energy Physics.
 * Geant4, in turn,  took the table from the original FORTRAN77 implementation of the HEP CERN Library routine RECUSQ.
 * Each sequence has a period of 10**9 numbers.
 */
#ifndef	RandSeedTable_H
#define RandSeedTable_H

#include <stdlib.h>
#include <time.h>
#include <limits.h>

/*
 *********************************************************
 * DEFINES, ENUMS
 *********************************************************
 */

/* dimensions of Random Number Seed Table */
#define RAND_SEED_TABLE_SIZE_ROWS								(215)
#define RAND_SEED_TABLE_SIZE_COLS								(2)

/*
 *********************************************************
 * STRUCTS, TYPEDEFS
 *********************************************************
 */
 
/*
 *********************************************************
 * MACROS
 *********************************************************
 */
 
/*
 *********************************************************
 * PUBLIC FUNCTIONS
 *********************************************************
 */

int GetSeedRandRecordRandSeedTable();

int GetSeedFixedRecordRandSeedTable(int row, int col);

/*
 *********************************************************
 * NON PUBLIC FUNCTIONS
 *********************************************************
 */
  
#endif RandSeedTable_H		/* end of RandSeedTable.h */
