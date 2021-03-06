#ifndef _BLASR_HDF_ALN_INFO_GROUP_HPP_
#define _BLASR_HDF_ALN_INFO_GROUP_HPP_

#include <cinttypes>
#include <string>
#include <vector>

#include <hdf/HDF2DArray.hpp>
#include <hdf/HDFArray.hpp>
#include <hdf/HDFAtom.hpp>
#include <hdf/HDFGroup.hpp>
#include <pbdata/alignment/CmpAlignment.hpp>
#include <pbdata/saf/AlnInfo.hpp>

class HDFAlnInfoGroup
{
public:
    HDFGroup alnInfoGroup;
    HDF2DArray<unsigned int> alnIndexArray;
    HDFArray<float> startTime;
    static const int NCols = 22;
    HDFArray<unsigned int> numPasses;
    HDFAtom<std::vector<std::string> > columnNames;
    HDFAtom<int> frameRate;

    int Initialize(HDFGroup &rootGroup);

    int InitializeNumPasses();

    void InitializeDefaultColumnNames(std::vector<std::string> &defaultColumnNames);

    bool Create(HDFGroup &parent);

    ~HDFAlnInfoGroup();

    // Return size of /AlnInfo/AlnIndex in KB
    UInt GetAlnIndexSize();

    void Read(AlnInfo &alnInfo);

    int GetNAlignments();

    unsigned int WriteAlnIndex(std::vector<unsigned int> &aln);

    void ReadCmpAlignment(UInt alignmentIndex, CmpAlignment &cmpAlignment);
};

#endif
