#ifndef _BLASR_INTERVAL_ALIGNMENT_PRINTER_HPP_
#define _BLASR_INTERVAL_ALIGNMENT_PRINTER_HPP_

#include <alignment/datastructures/alignment/AlignmentCandidate.hpp>
#include <pbdata/FASTQSequence.hpp>

namespace IntervalOutput {

void Print(T_AlignmentCandidate &alignment, std::ostream &outFile);

// Print an alignment from a sam file in Interval (m 4) format.
void PrintFromSAM(AlignmentCandidate<> &alignment, std::ostream &outFile);

void PrintHeader(std::ostream &out);
}

#endif
