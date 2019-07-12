#ifndef _DATA_H
#define _DATA_H

#include <string>
#include <vector>

class Data{

	public:

	std::vector<float> u, v, w;
	size_t nvecs;
	size_t nu, nv;
	float res; // assuming same resolution along all axis
	float bounds[2][2];
	std::vector<float> trace_xy;

	void read_nek5000_nc(const std::string &filename, size_t offset);

};

#endif 