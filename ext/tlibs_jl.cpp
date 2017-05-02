/**
 * julia module
 * gcc -I. -I.. -I/usr/include/julia -std=c++11 -shared -fPIC -o tlibs_jl.so ../log/log.cpp tlibs_jl.cpp -lstdc++ -lboost_system -lboost_iostreams
 *
 * @author Tobias Weber <tobias.weber@tum.de>
 * @date 23-apr-2017
 * @license GPLv2 or GPLv3
 */

#define TLIBS_INC_HDR_IMPLS
#include "version.h"
#include "ext/jl.h"
#include "log/log.h"
#include "file/loadinstr.h"


using t_real = double;


extern "C" void load_tlibs()
{
	tl::log_debug("Loaded tlibs version ", TLIBS_VERSION, ".");
}


extern "C" jl_array_t* load_instr(const char* pcFile)
{
	// [ column names, data, keys, values ]
	jl_array_t *pArr = jl_alloc_array_1d(jl_apply_array_type(jl_any_type, 1), 4);
	jl_array_t** pArrDat = reinterpret_cast<jl_array_t**>(jl_array_data(pArr));

	tl::FileInstrBase<t_real>* pInstr = tl::FileInstrBase<t_real>::LoadInstr(pcFile);
	if(!pInstr)
		tl::log_err("In ", __func__, ": Cannot load ", pcFile, ".");

	// data column names
	pArrDat[0] = tl::make_jl_str_arr(pInstr->GetColNames());

	// data matrix
	pArrDat[1] = tl::make_jl_2darr(pInstr->GetData());

	// scan property map
	std::tie(pArrDat[2], pArrDat[3]) = tl::make_jl_strmap_arr(pInstr->GetAllParams());

	return pArr;
}
