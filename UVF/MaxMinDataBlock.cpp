#define _NOMINMAX
#include <stdexcept>
#include "MaxMinDataBlock.h"

using namespace std;
using namespace UVFTables;

MaxMinDataBlock::MaxMinDataBlock(size_t iComponentCount) : 
  DataBlock(),
  m_GlobalMaxMin( std::numeric_limits<double>::max(), 
                 -std::numeric_limits<double>::max(), 
                  std::numeric_limits<double>::max(), 
                 -std::numeric_limits<double>::max())
{
  ulBlockSemantics = BS_MAXMIN_VALUES;
  strBlockID       = "Brick Max/Min Values";
  m_vfMaxMinData.clear();
  m_iComponentCount = iComponentCount;
}

MaxMinDataBlock::MaxMinDataBlock(const MaxMinDataBlock &other) :
  DataBlock(other),
  m_GlobalMaxMin(other.m_GlobalMaxMin),
  m_vfMaxMinData(other.m_vfMaxMinData),
  m_iComponentCount(other.m_iComponentCount)
{
}

MaxMinDataBlock& MaxMinDataBlock::operator=(const MaxMinDataBlock& other) {
  strBlockID = other.strBlockID;
  ulBlockSemantics = other.ulBlockSemantics;
  ulCompressionScheme = other.ulCompressionScheme;
  ulOffsetToNextDataBlock = other.ulOffsetToNextDataBlock;

  m_iComponentCount = other.m_iComponentCount;
  m_vfMaxMinData = other.m_vfMaxMinData;

  return *this;
}


MaxMinDataBlock::MaxMinDataBlock(LargeRAWFile* pStreamFile, UINT64 iOffset, bool bIsBigEndian) {
  GetHeaderFromFile(pStreamFile, iOffset, bIsBigEndian);
}

MaxMinDataBlock::~MaxMinDataBlock() 
{
}

DataBlock* MaxMinDataBlock::Clone() const {
  return new MaxMinDataBlock(*this);
}

UINT64 MaxMinDataBlock::GetHeaderFromFile(LargeRAWFile* pStreamFile, UINT64 iOffset, bool bIsBigEndian) {
  UINT64 iStart = iOffset + DataBlock::GetHeaderFromFile(pStreamFile, iOffset, bIsBigEndian);
  pStreamFile->SeekPos(iStart);

  UINT64 ulBrickCount;
  pStreamFile->ReadData(ulBrickCount, bIsBigEndian);
  { // Widen component count to 64 bits during the read.
    UINT64 component_count;
    pStreamFile->ReadData(component_count, bIsBigEndian);
    m_iComponentCount = static_cast<size_t>(component_count);
  }

  m_vfMaxMinData.resize(size_t(ulBrickCount));

  for(MaxMin::iterator i = m_vfMaxMinData.begin();
	  i != m_vfMaxMinData.end(); ++i) {
    (*i).resize(m_iComponentCount);
    for (size_t j = 0;j<m_iComponentCount;j++) {
      pStreamFile->ReadData((*i)[j].minScalar, bIsBigEndian);
      pStreamFile->ReadData((*i)[j].maxScalar, bIsBigEndian);
      pStreamFile->ReadData((*i)[j].minGradient, bIsBigEndian);
      pStreamFile->ReadData((*i)[j].maxGradient, bIsBigEndian);
  
      m_GlobalMaxMin.Merge((*i)[j]);
    }
  }

  return pStreamFile->GetPos() - iOffset;
}

UINT64 MaxMinDataBlock::CopyToFile(LargeRAWFile* pStreamFile, UINT64 iOffset, bool bIsBigEndian, bool bIsLastBlock) {
  CopyHeaderToFile(pStreamFile, iOffset, bIsBigEndian, bIsLastBlock);
  
  // for some strange reason throwing in the raw expression into WriteData causes random values to written into the file on windows
  UINT64 ulBrickCount = UINT64(m_vfMaxMinData.size());
  pStreamFile->WriteData(ulBrickCount, bIsBigEndian);
  { // Widen to 64bits during the write.
    UINT64 component_count = m_iComponentCount;
    pStreamFile->WriteData(component_count, bIsBigEndian);
  }

  for (MaxMin::const_iterator i = m_vfMaxMinData.begin();
	   i != m_vfMaxMinData.end(); ++i) {
    for (size_t j = 0;j<m_iComponentCount;j++) {
      pStreamFile->WriteData((*i)[j].minScalar, bIsBigEndian);
      pStreamFile->WriteData((*i)[j].maxScalar, bIsBigEndian);
      pStreamFile->WriteData((*i)[j].minGradient, bIsBigEndian);
      pStreamFile->WriteData((*i)[j].maxGradient, bIsBigEndian);
    }
  }

  return pStreamFile->GetPos() - iOffset;
}


UINT64 MaxMinDataBlock::GetOffsetToNextBlock() const {
  return DataBlock::GetOffsetToNextBlock() + ComputeDataSize();
}

UINT64 MaxMinDataBlock::ComputeDataSize() const {
  // We're writing 4 values per iteration in CopyToFile.  We used to use
  // sizeof(InternalMaxMinElement) to compute the size of the data, but that's
  // a bad idea because the compiler is free to pack the class however it
  // wants.  We'll write out 4 64bit values regardless of the packing of that
  // class.
  // Still, if you ever add a new element to that class, you of course need to
  // add a write for it in CopyToFile, but you also need to come here and
  // increment the size by 8.  Hopefully this assert will clue you in if you
  // forget to do that.
  assert(sizeof(InternalMaxMinElement) == 32 &&
         "assuming there are 4 values per element/component!");
  return sizeof(UINT64) +                                       // length of the vector
         sizeof(UINT64) +                                       // component count
         32 * m_vfMaxMinData.size() * m_iComponentCount;        // vector of data
}

const InternalMaxMinElement& MaxMinDataBlock::GetValue(size_t iIndex, size_t iComponent) {
  if(iIndex >= m_vfMaxMinData.size() ||
     iComponent >= m_vfMaxMinData[iIndex].size()) {
    throw std::length_error("MaxMinDataBlock: Invalid maxmin index.");
  }
  return m_vfMaxMinData[iIndex][iComponent];
}

void MaxMinDataBlock::StartNewValue() {
  vector<InternalMaxMinElement> elems;
  InternalMaxMinElement elem(std::numeric_limits<double>::max(), 
                            -std::numeric_limits<double>::max(), 
                             std::numeric_limits<double>::max(), 
                            -std::numeric_limits<double>::max());  
  for (UINT64 i = 0;i<m_iComponentCount;i++) elems.push_back(elem);  
  m_vfMaxMinData.push_back(elems);
}

void MaxMinDataBlock::MergeData(const std::vector<DOUBLEVECTOR4>& fMaxMinData)
{
  for (size_t i = 0;i<m_iComponentCount;i++) {
    MergeData(InternalMaxMinElement(fMaxMinData[i].x, fMaxMinData[i].y,
                                    fMaxMinData[i].z, fMaxMinData[i].w), i);
  }
}

void MaxMinDataBlock::MergeData(const InternalMaxMinElement& data, const size_t iComponent) {
  m_GlobalMaxMin.Merge(data);
  m_vfMaxMinData[m_vfMaxMinData.size()-1][iComponent].Merge(data);
}
