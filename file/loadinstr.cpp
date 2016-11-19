/**
 * Loads instrument-specific data files
 * @author tweber
 * @date feb-2015
 * @license GPLv2 or GPLv3
 */

#include "loadinstr.h"
#include "loadinstr_impl.h"

namespace tl
{
	template FileInstrBase<double>* FileInstrBase<double>::LoadInstr(const char* pcFile);

	template class FilePsi<double>;
	template class FileFrm<double>;
	template class FileMacs<double>;
	template class FileTrisp<double>;
	template class FileRaw<double>;


	template FileInstrBase<float>* FileInstrBase<float>::LoadInstr(const char* pcFile);

	template class FilePsi<float>;
	template class FileFrm<float>;
	template class FileMacs<float>;
	template class FileTrisp<float>;
	template class FileRaw<float>;
}


/*
// test
// gcc -DNO_IOSTR -o 0 file/loadinstr.cpp helper/log.cpp -std=c++11 -lstdc++ -lm
int main()
{
	//tl::FileFrm dat;
	//tl::FileMacs dat;
	tl::FileTrisp dat;
	//if(!dat.Load("/home/tweber/tmp/tst.dat"))
	//if(!dat.Load("/home/tweber/Messdaten/MACS_2014/data/Escan_31896.ng0"))
	if(!dat.Load("/home/tweber/Messdaten/trisp-15/data/sc77087.log"))
	{
		tl::log_err("Cannot load data file.");
		return -1;
	}

	std::array<double,3> latt = dat.GetSampleLattice();
	std::cout << latt[0] << ", " << latt[1] << ", " << latt[2] << std::endl;

	std::cout << "kfix = " << dat.GetKFix() << std::endl;

	return 0;
}
*/
