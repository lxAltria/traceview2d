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
	float p[2];
	int nmax=100;
	float step = 0.25;
	size_t offset = 22;


	ops >> PosOption(ip_filename) >> PosOption(offset) >> PosOption(p[0]) >> PosOption(p[1]) >> PosOption(nmax) >> PosOption(step);

	dat.read_nek5000_nc(ip_filename, offset);
	dat.trace_xy.push_back(p[0]); dat.trace_xy.push_back(p[1]);

	// advect
	for (int i=0; i<nmax; i++){

		float nx[2];
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
	widget->show();
	app.exec();


	return 0;
}