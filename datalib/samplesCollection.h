/* handle collection of samples
 */

#ifndef DTCOLLECTION_H
#define DTCOLLECTION_H

#include <cstdint>

#include <vector>

template <class DT> class samplesCollection {
	DT min, max;

protected:
	std::vector<DT> samples;

public:
	DT &getMin( void ){ return this->min; };
	DT &getMax( void ){ return this->max; };
	DT &getFirst(void){ return this->samples.front(); };
	DT &getLast(void){ return this->samples.back(); };
	uint32_t getSampleCount(void){ return this->samples.size(); };

	std::vector<DT> &getSamples(void){ return this->samples; };
};

#endif
