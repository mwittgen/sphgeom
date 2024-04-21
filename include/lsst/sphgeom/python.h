/*
 * This file is part of sphgeom.
 *
 * Developed for the LSST Data Management System.
 * This product includes software developed by the LSST Project
 * (http://www.lsst.org).
 * See the COPYRIGHT file at the top-level directory of this distribution
 * for details of code ownership.
 *
 * This software is dual licensed under the GNU General Public License and also
 * under a 3-clause BSD license. Recipients may choose which of these licenses
 * to use; please see the files gpl-3.0.txt and/or bsd_license.txt,
 * respectively.  If you choose the GPL option then the following text applies
 * (but note that there is still no warranty even if you opt for BSD instead):
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PYTHON_LSST_SPHGEOM_SPHGEOM_H
#define PYTHON_LSST_SPHGEOM_SPHGEOM_H

#include <functional>
#include <vector>
#include <array>
#include <nanobind/ndarray.h>
#include <iostream>
#include <concepts>


namespace nanobind {
    template <std::size_t Index = 0, typename... Types, std::size_t N>
    constexpr void write_tuple(std::array<const void *, N> &arr, std::size_t index, std::tuple<Types...> &t) {
        if constexpr (Index < sizeof...(Types)) {
            using ptr_type = std::remove_reference<decltype(std::get<Index>(t))>::type *;
            std::get<Index>(t) = ((ptr_type) arr[Index])[index];
            write_tuple<Index + 1>(arr, index, t);
        }
    }

    using array = nanobind::ndarray<>;
    template <typename ReturnType, typename Class, size_t Np>
    array call(Class &obj, auto fargs, std::array<dlpack::dtype,Np>  const &dtypes, auto func, auto... args) {
        constexpr size_t N = sizeof...(args);
        std::array<size_t, N> ndim;
        std::array<size_t const *, N> shapes;
        std::array<int64_t const *, N> strides;
        std::array<dlpack::dtype, N> dtype;
        std::array<size_t, N> size;
        std::array<const void *, N> data;
        for (int i = 0; array const &a: {args...}) {
            ndim[i] = a.ndim();
            shapes[i] = (const size_t *) a.shape_ptr();
            strides[i] = a.stride_ptr();
            dtype[i] =  a.dtype();
            size[i] = a.size();
            data[i] = a.data();
            ++i;
        }
        if(!std::equal(ndim.cbegin()+1, ndim.cbegin(), ndim.cend())) {
            throw;
        }
        if(!std::equal(dtype.cbegin(), dtypes.cbegin() , dtypes.cend())) {
            throw;
        }
        auto  *d = new ReturnType [size[0]];
        nanobind::capsule owner(d, [](void *p) noexcept {
            delete[] (float *) p;
        });

        for(int i = 0; i<size[0]; ++i) {
          decltype(fargs) call_args{};
          write_tuple(data, i, call_args);
          d[i] = std::apply([&obj, func](auto... cargs) {return (obj.*func)(cargs...);}, call_args);
        }
        nanobind::ndarray<>result(d, ndim[0], shapes[0],
                                  owner, strides[0],
                                  nanobind::dtype<ReturnType>());
        return result;
    }

template<typename Class, typename ReturnType, typename... Args>
auto vectorize(ReturnType(Class::*func)(Args... args) const) {
    // Return a lambda capturing the ember function pointer
    constexpr size_t N  = sizeof...(Args);
    std::tuple<Args...> fargs;
    static_assert(std::conjunction_v<std::is_scalar<Args>...>, "All arguments must be scalar types");
    std::array<dlpack::dtype, N> dtypes {nanobind::dtype<Args>()...};
    // Not an ideal solution.
    // The function signature does not get deducted correctly
    // by nanobind::def when not returning a lambda function
    // with the desired function signature.
    if constexpr(N==1) return
        [func, fargs, dtypes](Class &obj, array a) -> array
        { return call<ReturnType>(obj, fargs, dtypes, func, a); };
    if constexpr(N==2) return
                    [func, fargs, dtypes](Class &obj,  array a, array b) -> array
                    { return call<ReturnType>(obj, fargs, dtypes, func, a, b); };
    if constexpr(N==3) return
                [func, fargs,dtypes](Class &obj,  array a, array b, array c) -> array
                { return call<ReturnType>(obj, fargs, dtypes, func, a, b, c); };
    if constexpr(N==4) return
                [func, fargs, dtypes](Class &obj, array a, array b, array c, array d) -> array
                { return call<ReturnType>(obj, fargs, dtypes, func, a, b ,c, d); };
    static_assert(true, "nanobind::vectorize called with more than 4 arguments");
    }
}
namespace lsst {
namespace sphgeom {

template <typename NanoBindClass>
void defineClass(NanoBindClass &cls);

}  // sphgeom
}  // lsst

#endif  // PYTHON_LSST_SPHGEOM_SPHGEOM_H
