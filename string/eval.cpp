/**
 * minimalistic expression evaluator
 * @author Tobias Weber <tobias.weber@tum.de>
 * @date apr-2016
 * @license GPLv2 or GPLv3
 */

#include <boost/integer_fwd.hpp>
#include "eval.h"
#include "eval_impl.h"

namespace tl
{
	template std::pair<bool, double> eval_expr<std::string, double>(const std::string& str) noexcept;
	template std::pair<bool, float> eval_expr<std::string, float>(const std::string& str) noexcept;
	template std::pair<bool, int> eval_expr<std::string, int>(const std::string& str) noexcept;
}



/*
// test
// gcc -o eval string/eval.cpp log/log.cpp -std=c++11 -lstdc++ -lm
int main()
{
	while(1)
	{
		std::string strLine;
		std::cout << "> ";
		std::getline(std::cin, strLine);

		auto pairRes = tl::eval_expr<std::string, double>(strLine);
		if(!pairRes.first)
			tl::log_err("Could not parse expression.");
		std::cout << pairRes.second << std::endl;
	}
	return 0;
}
*/
