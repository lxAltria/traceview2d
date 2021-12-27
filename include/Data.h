#ifndef _DATA_H
#define _DATA_H

#include <string>
#include <vector>

#include <fstream>
template<typename Type>
Type * readfile(const char * file, size_t& num){
  std::ifstream fin(file, std::ios::binary);
  if(!fin){
        printf(" Error, Couldn't find the file\n");
        return 0;
    }
    fin.seekg(0, std::ios::end);
    const size_t num_elements = fin.tellg() / sizeof(Type);
    fin.seekg(0, std::ios::beg);
    Type * data = (Type *) malloc(num_elements*sizeof(Type));
  fin.read(reinterpret_cast<char*>(&data[0]), num_elements*sizeof(Type));
  fin.close();
  num = num_elements;
  return data;
}

class Data{

	public:

	std::vector<float> u, v, w;
	size_t nvecs;
	size_t nu, nv;
	double res; // assuming same resolution along all axis
	double bounds[2][2];
	int nmax;

	// std::vector<double> trace_xy;
	// std::vector<double> itrace_xy; // interactive streamline

	// std::vector<std::vector<double>> itraces_xy; // to support multiple seed points

	double px, py; // x and y coordinates of seed point


	void read_nek5000_nc(const std::string &filename, size_t offset);

};

#endif 