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
	int nmax;

	std::vector<float> trace_xy;
	std::vector<double> itrace_xy; // interactive streamline

	std::vector<std::vector<double>> itraces_xy; // to support multiple seed points

	double px, py; // x and y coordinates of seed point


	void read_nek5000_nc(const std::string &filename, size_t offset);

};

#endif 