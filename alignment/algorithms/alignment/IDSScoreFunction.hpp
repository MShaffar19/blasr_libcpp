#ifndef _BLASR_IDS_SCORE_FUNCTION_HPP_
#define _BLASR_IDS_SCORE_FUNCTION_HPP_

#include <cmath>
#include <string>

#include <alignment/algorithms/alignment/BaseScoreFunction.hpp>
#include <alignment/algorithms/alignment/ScoreMatrices.hpp>
#include <alignment/utils/LogUtils.hpp>
#include <pbdata/FASTASequence.hpp>
#include <pbdata/FASTQSequence.hpp>

float SumAsValidPhred(float v1, float v2, float v3);

template <typename T_RefSequence, typename T_QuerySequence>
class IDSScoreFunction : public BaseScoreFunction
{
public:
    int scoreMatrix[5][5];

    IDSScoreFunction(int scoreMatrixP[5][5], int insertionP, int deletionP,
                     int globalInsertionPriorP, int globalDeletionPriorP)
        : BaseScoreFunction(insertionP, deletionP, globalInsertionPriorP, globalDeletionPriorP)
    {
        InitializeScoreMatrix(scoreMatrixP);
    }

    IDSScoreFunction()
    {
        substitutionPrior = 20;
        globalDeletionPrior = 13;
    }

    void InitializeScoreMatrix(int scoreMatrixP[5][5])
    {
        int i, j;
        for (i = 0; i < 5; i++) {
            for (j = 0; j < 5; j++) {
                scoreMatrix[i][j] = scoreMatrixP[i][j];
            }
        }
    }

    int Deletion(T_QuerySequence &seq, DNALength pos)
    {
        PB_UNUSED(seq);
        PB_UNUSED(pos);
        std::cout << "IDS. For now, deletion must be specialized with FASTQ or FASTA Sequences. "
                  << std::endl;
        std::exit(EXIT_FAILURE);
        return 0;
    }
    int Deletion(T_RefSequence &refSeq, DNALength refPos, T_QuerySequence &querySeq,
                 DNALength queryPos)
    {
        PB_UNUSED(refSeq);
        PB_UNUSED(refPos);
        PB_UNUSED(querySeq);
        PB_UNUSED(queryPos);
        std::cout << "IDS. For now, this function must be specialized with either FASTQ or FASTA "
                     "sequences"
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }
    int Match(T_RefSequence &ref, DNALength refPos, T_QuerySequence &query, DNALength queryPos)
    {
        PB_UNUSED(ref);
        PB_UNUSED(refPos);
        PB_UNUSED(query);
        PB_UNUSED(queryPos);
        std::cout << "IDS. For now, this function must be specialized with either FASTQ or FASTA "
                     "sequences"
                  << std::endl;
        return 0;
        std::exit(EXIT_FAILURE);
    }
    int Insertion(T_RefSequence &refSeq, DNALength refPos, T_QuerySequence &querySeq,
                  DNALength queryPos)
    {
        PB_UNUSED(refSeq);
        PB_UNUSED(refPos);
        PB_UNUSED(querySeq);
        PB_UNUSED(queryPos);
        std::cout << "IDS. For now, this function must be specialized with either FASTQ or FASTA "
                     "sequences"
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }
    int Insertion(T_QuerySequence &seq, DNALength pos)
    {
        PB_UNUSED(seq);
        PB_UNUSED(pos);
        std::cout << "IDS. For now, this function must be specialized with either FASTQ or FASTA "
                     "sequences"
                  << std::endl;
        return 0;
        std::exit(EXIT_FAILURE);
    }

    float NormalizedMatch(T_RefSequence &ref, DNALength refPos, T_QuerySequence &query,
                          DNALength queryPos);
    float NormalizedInsertion(T_RefSequence &ref, DNALength refPos, T_QuerySequence &query,
                              DNALength queryPos);
    float NormalizedDeletion(T_RefSequence &ref, DNALength refPos, T_QuerySequence &query,
                             DNALength queryPos);
};

/*
 * Define all specializations for a FASTA reference and FASTQSequence for the query, or FASTA sequence for query.
 */

template <>
int IDSScoreFunction<DNASequence, FASTQSequence>::Deletion(DNASequence &ref, DNALength refPos,
                                                           FASTQSequence &query,
                                                           DNALength queryPos);

template <>
int IDSScoreFunction<DNASequence, FASTQSequence>::Deletion(FASTQSequence &query,
                                                           DNALength queryPos);

template <>
int IDSScoreFunction<DNASequence, DNASequence>::Deletion(DNASequence &query, DNALength pos);

template <>
int IDSScoreFunction<DNASequence, FASTQSequence>::Insertion(DNASequence &refSeq, DNALength refPos,
                                                            FASTQSequence &query, DNALength pos);

template <>
int IDSScoreFunction<DNASequence, FASTQSequence>::Insertion(FASTQSequence &query, DNALength pos);

template <>
int IDSScoreFunction<DNASequence, FASTQSequence>::Match(DNASequence &ref, DNALength refPos,
                                                        FASTQSequence &query, DNALength queryPos);

template <>
float IDSScoreFunction<DNASequence, FASTQSequence>::NormalizedMatch(DNASequence &ref,
                                                                    DNALength refPos,
                                                                    FASTQSequence &query,
                                                                    DNALength queryPos);

template <>
float IDSScoreFunction<DNASequence, FASTQSequence>::NormalizedInsertion(DNASequence &ref,
                                                                        DNALength refPos,
                                                                        FASTQSequence &query,
                                                                        DNALength queryPos);

template <>
float IDSScoreFunction<DNASequence, FASTQSequence>::NormalizedDeletion(DNASequence &ref,
                                                                       DNALength refPos,
                                                                       FASTQSequence &query,
                                                                       DNALength queryPos);

#endif  // _BLASR_IDS_SCORE_FUNCTION_HPP_
