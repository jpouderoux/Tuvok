/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2011 Scientific Computing and Imaging Institute,
   University of Utah.


   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/
#ifndef TUVOK_AMIRA_CONVERTER_H
#define TUVOK_AMIRA_CONVERTER_H

#include "RAWConverter.h"


// Your standard ostream_iterator will essentially do "stream << *iter".  That
// doesn't work well for binary files, however, in which we need to use
// "stream.write(&*iter, sizeof(T))".  Hence this implements a binary
// ostream_iterator.
template<typename T> class binary_ostream_iterator :
public std::iterator<std::output_iterator_tag, void, void, void, void> {
public:
	binary_ostream_iterator(std::ostream& os) : stream(os) {}
	binary_ostream_iterator(const binary_ostream_iterator& boi) :
	stream(boi.stream) { }

	binary_ostream_iterator& operator=(const T& value) {
		this->stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
		return *this;
	}
	
	// TODO: This is clearly not what we want but I added this operator to get it to compile at all
	//       Tom please fix this.
	binary_ostream_iterator& operator=(const binary_ostream_iterator<T>& ) {
		return *this;
	}
	

	binary_ostream_iterator& operator*() { return *this; }
	binary_ostream_iterator& operator++() { return *this; }
	binary_ostream_iterator& operator++(int) { return *this; }
	std::output_iterator_tag iterator_category(const binary_ostream_iterator&) {
		return std::output_iterator_tag();
	}

private:
	std::ostream& stream;

private:
};



/// A converter for Amira files, we think.
class AmiraConverter : public RAWConverter {
public:
   AmiraConverter();
   virtual ~AmiraConverter() {}

   virtual bool CanRead(const std::string& fn,
                        const std::vector<int8_t>& start) const;

   virtual bool ConvertToRAW(const std::string& strSourceFilename,
                             const std::string& strTempDir,
                             bool bNoUserInteraction,
                             uint64_t& iHeaderSkip, uint64_t& iComponentSize,
                             uint64_t& iComponentCount, bool& bConvertEndianness,
                             bool& bSigned, bool& bIsFloat,
                             UINT64VECTOR3& vVolumeSize,
                             FLOATVECTOR3& vVolumeAspect,
                             std::string& strTitle,
                             UVFTables::ElementSemanticTable& eType,
                             std::string& strIntermediateFile,
                             bool& bDeleteIntermediateFile);

   virtual bool ConvertToNative(const std::string& strRawFilename,
                                const std::string& strTargetFilename,
                                uint64_t iHeaderSkip, uint64_t iComponentSize,
                                uint64_t iComponentCount, bool bSigned,
                                bool bFloatingPoint, UINT64VECTOR3 vVolumeSize,
                                FLOATVECTOR3 vVolumeAspect,
                                bool bNoUserInteraction,
                                const bool bQuantizeTo8Bit);

   virtual bool CanExportData() const { return false; }
   virtual bool CanImportData() const { return true; }

};
#endif // TUVOK_AMIRA_CONVERTER_H
