/**
 * julia interface helpers
 *
 * @author Tobias Weber <tobias.weber@tum.de>
 * @date 23-apr-2017
 * @license GPLv2 or GPLv3
 */

#ifndef __TL_JL_H__
#define __TL_JL_H__

#include <julia.h>
#include <tuple>
#include <limits>


namespace tl
{

// ----------------------------------------------------------------------------
/**
 * Julia data type traits
 */
template<typename T> struct jl_traits {};

template<> struct jl_traits<int64_t>
{
	using value_type = int64_t;

	static jl_datatype_t* get_type() { return jl_int64_type; }
	static value_type unbox(jl_value_t *pVal) { return jl_unbox_int64(pVal); }
	static jl_value_t* box(value_type val) { return jl_box_int64(val); }
};

template<> struct jl_traits<uint64_t>
{
	using value_type = uint64_t;

	static jl_datatype_t* get_type() { return jl_uint64_type; }
	static value_type unbox(jl_value_t *pVal) { return jl_unbox_uint64(pVal); }
	static jl_value_t* box(value_type val) { return jl_box_uint64(val); }
};

template<> struct jl_traits<int32_t>
{
	using value_type = int32_t;

	static jl_datatype_t* get_type() { return jl_int32_type; }
	static value_type unbox(jl_value_t *pVal) { return jl_unbox_int32(pVal); }
	static jl_value_t* box(value_type val) { return jl_box_int32(val); }
};

template<> struct jl_traits<uint32_t>
{
	using value_type = uint32_t;

	static jl_datatype_t* get_type() { return jl_uint32_type; }
	static value_type unbox(jl_value_t *pVal) { return jl_unbox_uint32(pVal); }
	static jl_value_t* box(value_type val) { return jl_box_uint32(val); }
};

template<> struct jl_traits<int16_t>
{
	using value_type = int16_t;

	static jl_datatype_t* get_type() { return jl_int16_type; }
	static value_type unbox(jl_value_t *pVal) { return jl_unbox_int16(pVal); }
	static jl_value_t* box(value_type val) { return jl_box_int16(val); }
};

template<> struct jl_traits<uint16_t>
{
	using value_type = uint16_t;

	static jl_datatype_t* get_type() { return jl_uint16_type; }
	static value_type unbox(jl_value_t *pVal) { return jl_unbox_uint16(pVal); }
	static jl_value_t* box(value_type val) { return jl_box_uint16(val); }
};

template<> struct jl_traits<int8_t>
{
	using value_type = int8_t;

	static jl_datatype_t* get_type() { return jl_int8_type; }
	static value_type unbox(jl_value_t *pVal) { return jl_unbox_int8(pVal); }
	static jl_value_t* box(value_type val) { return jl_box_int8(val); }
};

template<> struct jl_traits<uint8_t>
{
	using value_type = uint8_t;

	static jl_datatype_t* get_type() { return jl_uint8_type; }
	static value_type unbox(jl_value_t *pVal) { return jl_unbox_uint8(pVal); }
	static jl_value_t* box(value_type val) { return jl_box_uint8(val); }
};

template<> struct jl_traits<float>
{
	using value_type = float;

	static jl_datatype_t* get_type() { return jl_float32_type; }
	static value_type unbox(jl_value_t *pVal) { return jl_unbox_float32(pVal); }
	static jl_value_t* box(value_type val) { return jl_box_float32(val); }
};

template<> struct jl_traits<double>
{
	using value_type = double;

	static jl_datatype_t* get_type() { return jl_float64_type; }
	static value_type unbox(jl_value_t *pVal) { return jl_unbox_float64(pVal); }
	static jl_value_t* box(value_type val) { return jl_box_float64(val); }
};
// ----------------------------------------------------------------------------




// ----------------------------------------------------------------------------
/**
 * converts an stl container of containers into a julia 2d array
 */
template<template<class...> class t_cont/*=std::vector*/, class T/*=double*/>
jl_array_t* make_jl_2darr(const t_cont<t_cont<T>>& vecvec)
{
	// number of columns and rows
	std::size_t iCols = vecvec.size();
	std::size_t iRows = std::numeric_limits<std::size_t>::max();
	for(const auto& vec : vecvec)
		iRows = std::min(iRows, vec.size());
	if(!iCols) iRows = 0;

	jl_array_t *pArr = jl_alloc_array_2d(jl_apply_array_type(jl_traits<T>::get_type(), 2), iRows, iCols);
	T* pDat = reinterpret_cast<T*>(jl_array_data(pArr));

	std::size_t iCurCol = 0;
	for(const auto& vec : vecvec)
	{
		std::size_t iCurRow = 0;
		for(const auto& d : vec)
		{
			pDat[iCurCol*iRows + iCurRow] = d;
			++iCurRow;
		}

		++iCurCol;
	}

	return pArr;
}

/**
 * converts an stl container of strings into a julia array of strings
 */
template<template<class...> class t_cont/*=std::vector*/, class t_str/*=std::string*/>
jl_array_t* make_jl_str_arr(const t_cont<t_str>& vecStr)
{
	jl_array_t *pArr = jl_alloc_array_1d(jl_apply_array_type(jl_string_type, 1), vecStr.size());
	jl_value_t** pDat = reinterpret_cast<jl_value_t**>(jl_array_data(pArr));

	std::size_t iIdx = 0;
	for(const t_str& str : vecStr)
	{
		pDat[iIdx] = jl_cstr_to_string(str.c_str());
		++iIdx;
	}

	return pArr;
}

/**
 * converts a map of strings into two julia arrays of strings (key & value)
 */
template<template<class...> class t_cont/*=std::map*/, class t_str/*=std::string*/>
std::tuple<jl_array_t*, jl_array_t*> make_jl_strmap_arr(const t_cont<t_str, t_str>& map)
{
	jl_array_t *pArrKey = jl_alloc_array_1d(jl_apply_array_type(jl_string_type, 1), map.size());
	jl_array_t *pArrVal = jl_alloc_array_1d(jl_apply_array_type(jl_string_type, 1), map.size());

	jl_value_t** pDatKey = reinterpret_cast<jl_value_t**>(jl_array_data(pArrKey));
	jl_value_t** pDatVal = reinterpret_cast<jl_value_t**>(jl_array_data(pArrVal));

	std::size_t iIdx = 0;
	for(const auto& pair : map)
	{
		pDatKey[iIdx] = jl_cstr_to_string(pair.first.c_str());
		pDatVal[iIdx] = jl_cstr_to_string(pair.second.c_str());
		++iIdx;
	}

	return std::make_tuple(pArrKey, pArrVal);
}
// ----------------------------------------------------------------------------



}
#endif