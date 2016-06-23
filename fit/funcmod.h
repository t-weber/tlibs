/*
 * abstract function model base classes
 * @author tweber
 * @date 2013
 * @license GPLv2 or GPLv3
 */

#ifndef __FUNC_MOD_H__
#define __FUNC_MOD_H__

#include <boost/numeric/ublas/vector.hpp>

namespace tl {

// parametric function
template<class t_vec = boost::numeric::ublas::vector<double>, class T = typename t_vec::value_type>
class FunctionModel_param
{
public:
	virtual ~FunctionModel_param() = default;

	// t = 0..1
	virtual t_vec operator()(T t) const = 0;
	virtual const char* GetModelName() const = 0;
};


// ----------------------------------------------------------------------------


// explicit function
template<class T = double> class FunctionModel
{
public:
	virtual ~FunctionModel() = default;

	virtual T operator()(T x) const = 0;
	virtual const char* GetModelName() const = 0;
};

// synonym
//template<class T=double> using FunctionModel_gen = class FunctionModel<T>;


// ----------------------------------------------------------------------------


// explicit function with multiple internal parameter sets
template<class T = double> class FunctionModel_multi : public FunctionModel<T>
{
public:
	virtual ~FunctionModel_multi() = default;

	virtual std::size_t GetParamSetCount() const = 0;
	virtual void SetParamSet(std::size_t iSet) = 0;
};

// synonym
//template<class T=double> using FunctionModel_multi_gen = class FunctionModel_multi<T>;


// ----------------------------------------------------------------------------


// interface for n dimensional function
template<class T = double> class FunctionModel_nd
{
protected:

public:
	virtual ~FunctionModel_nd() = default;

	virtual T operator()(const T* px) const = 0;
	virtual const char* GetModelName() const = 0;
};

// synonym
//template<class T=double> using FunctionModel_nd_gen = class FunctionModel_nd<T>;



}

#endif
