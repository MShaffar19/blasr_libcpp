#ifndef _BLASR_SCORE_ANCHOR_IMPL_HPP_
#define _BLASR_SCORE_ANCHOR_IMPL_HPP_

template <typename TSequence, typename T_Tuple>
int GetTupleCount(TSequence &seq, DNALength startPos, TupleMetrics &tm,
                  TupleCountTable<TSequence, T_Tuple> &ct, int &count)
{

    T_Tuple tuple;
    if (tuple.FromStringLR(&seq.seq[startPos], tm)) {
        count = ct.countTable[TupleData(tuple)];
        return 1;
    } else {
        return 0;
    }
}

template <typename TSequence, typename T_Tuple>
int PMatch(TSequence &seq, DNALength startPos, DNALength length, TupleMetrics &tm,
           TupleCountTable<TSequence, T_Tuple> &ct, float &pMatch)
{
    int tupleCount;
    T_Tuple tuple, curTuple;
    //
    // Compute the probability of a match of length 'length'
    // in the genome using a k-th order	Markov model of the genome.
    // Other than that there is no spatial constraint on a match.
    // This means that if the length of seq is k, and that sequence
    // of length k exists in the genome, then the probability of a
    // match is 1.
    pMatch = 1;
    if (GetTupleCount(seq, startPos, tm, ct, tupleCount)) {
        if (tupleCount == 0) return 0;
        //
        // Compute the frequency of the following tuple, and compare this
        // to the frequencies of all 4 possible tuples that are next.
        //
        curTuple.FromStringLR(&seq.seq[startPos], tm);
        if (length < static_cast<DNALength>(tm.tupleSize)) {
            // the match is shorter than the tuples used to model the
            // genome sequence composition.  Don't try and compute a p-value
            // for it -- assume that you will always find a match of this
            // length.
            //
            pMatch = 0;
            return 1;
        }
        for (size_t i = 1; i < length - tm.tupleSize; i++) {
            //
            // now add on the log counts for the transitions.
            //
            if (tuple.FromStringLR(&seq.seq[i + startPos], tm) == 0) {
                return 0;
            }
            int nextTupleCount = 0;
            int rightMarCount = SumRightShiftMarginalTupleCounts(
                tm, ct, tuple, TwoBit[seq.GetNuc(startPos + i + tm.tupleSize - 1)], nextTupleCount);
            //
            // tuple counts are not defined for N's.
            //
            if (TwoBit[seq.GetNuc(startPos + i + tm.tupleSize)] > 3) {
                return 0;
            }

            if (nextTupleCount == 0) {
                //
                // There is no background distribution available for this
                // sequence context, therefore no way to evaluate p-value.
                //
                return 0;
            }
            pMatch += log((nextTupleCount / (1.0 * rightMarCount)));
            curTuple.tuple = tuple.tuple;
        }
        //
        // Done computing the probability of an extension.  Now compute the probability
        // of the match.  There are nMatches of the initial seed.  We assume that each has
        // an equal probability of matching.
        //
        return 1;
    } else {
        return 0;
    }
}

template <typename TSequence, typename T_Tuple>
int POneOrMoreMatches(TSequence &seq, DNALength startPos, DNALength length, TupleMetrics &tm,
                      TupleCountTable<TSequence, T_Tuple> &ct, float &pValue)
{
    float pMatch = 1;
    //
    // Compute the probability that the sequence matches ANY spot
    // in the reference for at least 'length' bases.
    //
    PMatch(seq, startPos, length, tm, ct, pMatch);
    pValue = pMatch;
    return 1;
}

template <typename TSequence, typename T_Tuple>
int SumRightShiftMarginalTupleCounts(TupleMetrics &tm, TupleCountTable<TSequence, T_Tuple> &ct,
                                     T_Tuple curTuple, int nextNuc, int &nextSeqCount)
{
    (void)(tm);
    int totalCount = 0;
    int rightMarCount = 0;
    long i;
    T_Tuple altMask;
    altMask.tuple = 3L;
    altMask.tuple = ~altMask.tuple;
    for (i = 0; i < 4; i++) {
        T_Tuple alt = curTuple;
        //		next.ShiftLeft(tm, 2);
        alt.tuple = alt.tuple & altMask.tuple;
        alt.tuple += i;
        //		next.Append(i,2L);
        rightMarCount = ct.countTable[TupleData(alt)];
        totalCount += rightMarCount;
        if (i == nextNuc) {
            nextSeqCount = rightMarCount;
        }
    }
    return totalCount;
}

template <typename TSequence, typename T_Tuple>
int ComputeTotalTupleCount(TupleMetrics &tm, TupleCountTable<TSequence, T_Tuple> &ct,
                           TSequence &seq, int start, int end)
{
    PB_UNUSED(start);
    if (end == -1) {
        end = seq.length;
    }
    int nTuples = end - tm.tupleSize + 1;
    if (nTuples == 0) {
        return 0;
    }
    int totalCount = 0;
    T_Tuple tuple;
    int i;
    for (i = 0; i < nTuples; i++) {
        tuple.FromStringLR(&seq.seq[i], tm);
        totalCount += ct.countTable[tuple.ToLongIndex()];
    }
    return totalCount;
}

template <typename TSequence, typename T_Tuple>
int ComputeAverageTupleCount(TupleMetrics &tm, TupleCountTable<TSequence, T_Tuple> &ct,
                             TSequence &seq)
{
    int nTuples = seq.length - tm.tupleSize + 1;
    if (nTuples == 0) {
        return 0;
    }
    int totalCount = ComputeTotalTupleCount(tm, ct, seq);
    return totalCount / nTuples;
}

inline int ComputeExpectedFirstWaitingTime(float lambda)
{
    // The first waiting time of a Poisson process is
    // exponentially distributed. The mean of an exponentially
    // distributed variable with parameter lambda is 1/lambda.
    return (1 / lambda);
}

#endif
