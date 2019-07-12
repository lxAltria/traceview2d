#include "Data.h"
#include <netcdf.h>
#include "def.h"



void Data::read_nek5000_nc(const std::string &filename, size_t offset){


        int ncid, ndims, nvars, ngatts, unlimited;
        int ret;
        NC_SAFE_CALL( nc_open(filename.c_str(), NC_NOWRITE, &ncid) );

        nu = 512; nv = 512;
        res = 1; // unit resolution
        bounds[0][0] = 0;
        bounds[0][1] = nu - 1;
        bounds[1][0] = 0;
        bounds[1][1] = nv - 1;


        nvecs = nu * nv;
        u.resize(nvecs);
        v.resize(nvecs);
        w.resize(nvecs);

        size_t start[3] = {offset,0,0};
        size_t count[3] = {1, nv, nu};

        NC_SAFE_CALL( nc_get_vara_float(ncid, 0, start, count, &u[0]) );
        NC_SAFE_CALL( nc_get_vara_float(ncid, 1, start, count, &v[0]) );
        NC_SAFE_CALL( nc_get_vara_float(ncid, 2, start, count, &w[0]) );

        NC_SAFE_CALL( nc_close(ncid));


}