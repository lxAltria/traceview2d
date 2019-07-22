#include <QApplication>
#include "widget.h"
#include "advect.h"
#include "Data.h"
#include <string>
#include "opts2.h"

using namespace std;

int main(int argc, char **argv){


	// read inputs
	string ip_filename;
	
	using namespace opts;

	Options ops(argc, argv);
   

	// initialize 
	Data dat;
	double p[2];
	int nmax=100;
	double step = 0.1;
	size_t offset = 22;
	double lic_size = 150.0, rate=1;
	int lic_nmax = 50;


	ops >> PosOption(ip_filename) >> PosOption(offset) >> PosOption(p[0]) >> PosOption(p[1]) >> PosOption(nmax) >> PosOption(step)>> PosOption(lic_size)>> PosOption(lic_nmax)>> PosOption(rate);

	dat.read_nek5000_nc(ip_filename, offset);
	dat.trace_xy.push_back(p[0]); dat.trace_xy.push_back(p[1]);
	dat.px = p[0]; dat.py = p[1];
	dat.nmax = nmax;

	// advect
	for (int i=0; i<nmax; i++){

		double nx[2];
		if (advect(dat, p, nx, step)){
			p[0] = nx[0]; p[1] = nx[1];
			dat.trace_xy.push_back(p[0]); dat.trace_xy.push_back(p[1]);
		}else{
			break;
		}

	}


	// vis
	QApplication app(argc, argv);
    QGLFormat fmt = QGLFormat::defaultFormat();
    fmt.setSampleBuffers(true);
    fmt.setSamples(16); 
    QGLFormat::setDefaultFormat(fmt); 


    CGLWidget *widget = new CGLWidget();
	widget->d = &dat;

	
	widget->set_lic_size(lic_size, lic_nmax, step, rate);
	widget->show();
	app.exec();


	return 0;
}