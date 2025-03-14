// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.

  Consult the COPYING file in the top-level source directory of this
  module for the precise wording of the license and the list of
  copyright holders.
*/
/*!
 * \file
 *
 * \copydoc Opm::UniformTabulated2DFunction
 */
#ifndef OPM_UNIFORM_TABULATED_2D_FUNCTION_HPP
#define OPM_UNIFORM_TABULATED_2D_FUNCTION_HPP

#include <opm/common/OpmLog/OpmLog.hpp>
#include <opm/common/Exceptions.hpp>

#include <opm/material/common/MathToolbox.hpp>
#include <opm/common/utility/gpuDecorators.hpp>

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

// forward declaration of the class so the function in the next namespace can be declared
template <class Scalar, class ContainerT = std::vector<Scalar>>
class UniformTabulated2DFunction;

// declaration of make_view in correct namespace so friend function can be declared in the class
namespace Opm::gpuistl
{
    template <class ViewType, class ScalarT, class ContainerType>
    UniformTabulated2DFunction<ScalarT, ViewType> make_view(UniformTabulated2DFunction<ScalarT, ContainerType>& params);
}

namespace Opm {

/*!
 * \brief Implements a scalar function that depends on two variables and which is sampled
 *        on an uniform X-Y grid.
 *
 * This class can be used when the sampling points are calculated at
 * run time.
 */
template <class Scalar, class ContainerT = std::vector<Scalar>>
class UniformTabulated2DFunction
{
public:
    OPM_HOST_DEVICE UniformTabulated2DFunction()
    { }


    // Intended for construction of UniformTabulated2DFunction<double, GPUBuffer>
    UniformTabulated2DFunction(Scalar minX, Scalar maxX, unsigned m,
                               Scalar minY, Scalar maxY, unsigned n,
                               const ContainerT& samples)
        : samples_(samples), m_(m), n_(n), xMin_(minX), yMin_(minY), xMax_(maxX), yMax_(maxY){
    }

     /*!
      * \brief Constructor where the tabulation parameters are already
      *        provided.
      */
    UniformTabulated2DFunction(Scalar minX, Scalar maxX, unsigned m,
                               Scalar minY, Scalar maxY, unsigned n)
    {
        resize(minX, maxX, m, minY, maxY, n);
    }

    UniformTabulated2DFunction(Scalar minX, Scalar maxX, unsigned m,
                               Scalar minY, Scalar maxY, unsigned n,
                               const std::vector<std::vector<Scalar>>& vals)
    {
        resize(minX, maxX, m, minY, maxY, n);

        for (unsigned i = 0; i < m; ++i)
            for (unsigned j = 0; j < n; ++j)
                this->setSamplePoint(i, j, vals[i][j]);
    }

    // Both CO2Tables and H2Tables have values of dimes [200][500]
    // suboptimal hardcoding for now but easier than templating this function etc
    UniformTabulated2DFunction(Scalar minX, Scalar maxX, unsigned m,
                               Scalar minY, Scalar maxY, unsigned n,
                               const double vals[200][500])
    {
        resize(minX, maxX, m, minY, maxY, n);

        for (unsigned i = 0; i < m; ++i)
            for (unsigned j = 0; j < n; ++j)
                this->setSamplePoint(i, j, vals[i][j]);
    }

    /*!
     * \brief Resize the tabulation to a new range.
     */
    void resize(Scalar minX, Scalar maxX, unsigned m,
                Scalar minY, Scalar maxY, unsigned n)
    {
        samples_.resize(m*n);

        m_ = m;
        n_ = n;

        xMin_ = minX;
        xMax_ = maxX;

        yMin_ = minY;
        yMax_ = maxY;
    }

    /*!
     * \brief Returns the minimum of the X coordinate of the sampling points.
     */
    OPM_HOST_DEVICE Scalar xMin() const
    { return xMin_; }

    /*!
     * \brief Returns the maximum of the X coordinate of the sampling points.
     */
    OPM_HOST_DEVICE Scalar xMax() const
    { return xMax_; }

    /*!
     * \brief Returns the minimum of the Y coordinate of the sampling points.
     */
    OPM_HOST_DEVICE Scalar yMin() const
    { return yMin_; }

    /*!
     * \brief Returns the maximum of the Y coordinate of the sampling points.
     */
    OPM_HOST_DEVICE Scalar yMax() const
    { return yMax_; }

    /*!
     * \brief Returns the number of sampling points in X direction.
     */
    OPM_HOST_DEVICE unsigned numX() const
    { return m_; }

    /*!
     * \brief Returns the number of sampling points in Y direction.
     */
    OPM_HOST_DEVICE unsigned numY() const
    { return n_; }

    /*!
     * \brief Returns the sampling points.
     */
    OPM_HOST_DEVICE const ContainerT& samples() const
    { return samples_; }


    /*!
     * \brief Return the position on the x-axis of the i-th interval.
     */
    OPM_HOST_DEVICE Scalar iToX(unsigned i) const
    {
        assert(i < numX());

        return xMin() + i*(xMax() - xMin())/(numX() - 1);
    }

    /*!
     * \brief Return the position on the y-axis of the j-th interval.
      */
    OPM_HOST_DEVICE Scalar jToY(unsigned j) const
    {
        assert(j < numY());

        return yMin() + j*(yMax() - yMin())/(numY() - 1);
    }

    /*!
     * \brief Return the interval index of a given position on the x-axis.
     *
     * This method returns a *floating point* number. The integer part
     * should be interpreted as interval, the decimal places are the
     * position of the x value between the i-th and the (i+1)-th
     * sample point.
      */
    template <class Evaluation>
    OPM_HOST_DEVICE Evaluation xToI(const Evaluation& x) const
    { return (x - xMin())/(xMax() - xMin())*(numX() - 1); }

    /*!
     * \brief Return the interval index of a given position on the y-axis.
     *
     * This method returns a *floating point* number. The integer part
     * should be interpreted as interval, the decimal places are the
     * position of the y value between the j-th and the (j+1)-th
     * sample point.
     */
    template <class Evaluation>
    OPM_HOST_DEVICE Evaluation yToJ(const Evaluation& y) const
    { return (y - yMin())/(yMax() - yMin())*(numY() - 1); }

    /*!
     * \brief Returns true iff a coordinate lies in the tabulated range
     */
    template <class Evaluation>
    OPM_HOST_DEVICE bool applies(const Evaluation& x, const Evaluation& y) const
    {
        return
            xMin() <= x && x <= xMax() &&
            yMin() <= y && y <= yMax();
    }

    /*!
     * \brief Evaluate the function at a given (x,y) position.
     *
     * \param x x-position
     * \param y y-position
     * \param extrapolate Whether to extrapolate for untabulated values. If false then
     * an exception might be thrown.
     */
    template <class Evaluation>
    OPM_HOST_DEVICE Evaluation eval(const Evaluation& x,
                    const Evaluation& y,
                    [[maybe_unused]] bool extrapolate) const
    {
#ifndef NDEBUG
#if !OPM_IS_INSIDE_DEVICE_FUNCTION
        if (!extrapolate && !applies(x,y)) {
            std::string msg = "Attempt to get tabulated value for ("
                +std::to_string(double(scalarValue(x)))+", "+std::to_string(double(scalarValue(y)))
                +") on a table of extent "
                +std::to_string(xMin())+" to "+std::to_string(xMax())+" times "
                +std::to_string(yMin())+" to "+std::to_string(yMax());

            throw NumericalProblem(msg);
        }
#endif
#endif

        Evaluation alpha = xToI(x);
        Evaluation beta = yToJ(y);

        unsigned i =
            static_cast<unsigned>(
                std::max(0, std::min(static_cast<int>(numX()) - 2,
                                     static_cast<int>(scalarValue(alpha)))));
        unsigned j =
            static_cast<unsigned>(
                std::max(0, std::min(static_cast<int>(numY()) - 2,
                                     static_cast<int>(scalarValue(beta)))));

        alpha -= i;
        beta -= j;

        // bi-linear interpolation
        const Evaluation& s1 = getSamplePoint(i, j)*(1.0 - alpha) + getSamplePoint(i + 1, j)*alpha;
        const Evaluation& s2 = getSamplePoint(i, j + 1)*(1.0 - alpha) + getSamplePoint(i + 1, j + 1)*alpha;
        return s1*(1.0 - beta) + s2*beta;
    }

    /*!
     * \brief Get the value of the sample point which is at the
     *         intersection of the \f$i\f$-th interval of the x-Axis
     *         and the \f$j\f$-th of the y-Axis.
     */
    OPM_HOST_DEVICE Scalar getSamplePoint(unsigned i, unsigned j) const
    {
        assert(i < m_);
        assert(j < n_);

        return samples_[j*m_ + i];
    }

    /*!
     * \brief Set the value of the sample point which is at the
     *        intersection of the \f$i\f$-th interval of the x-Axis
     *        and the \f$j\f$-th of the y-Axis.
     */
    void setSamplePoint(unsigned i, unsigned j, Scalar value)
    {
        assert(i < m_);
        assert(j < n_);

        samples_[j*m_ + i] = value;
    }

    OPM_HOST_DEVICE bool operator==(const UniformTabulated2DFunction<Scalar>& data) const
    {
        return samples_ == data.samples_ &&
               m_ == data.m_ &&
               n_ == data.n_ &&
               xMin_ == data.xMin_ &&
               xMax_ == data.xMax_ &&
               yMin_ == data.yMin_ &&
               yMax_ == data.yMax_;
    }

private:
    template <class ViewType, class ScalarT, class Container>
    friend UniformTabulated2DFunction<ScalarT, ViewType> gpuistl::make_view(UniformTabulated2DFunction<ScalarT, Container>&);

    // the vector which contains the values of the sample points
    // f(x_i, y_j). don't use this directly, use getSamplePoint(i,j)
    // instead!
    ContainerT samples_;

    // the number of sample points in x direction
    unsigned m_;

    // the number of sample points in y direction
    unsigned n_;

    // the range of the tabulation on the x axis
    Scalar xMin_;
    Scalar xMax_;

    // the range of the tabulation on the y axis
    Scalar yMin_;
    Scalar yMax_;
};

} // namespace Opm

namespace Opm::gpuistl{
    template<class GPUContainer, class ScalarT>
    UniformTabulated2DFunction<ScalarT, GPUContainer>
    copy_to_gpu(const UniformTabulated2DFunction<ScalarT>& tab){
        return UniformTabulated2DFunction<ScalarT, GPUContainer>(tab.xMin(), tab.xMax(), tab.numX(), tab.yMin(), tab.yMax(), tab.numY(), GPUContainer(tab.samples()));
    }

    template <class ViewType, class ScalarT, class ContainerType>
    UniformTabulated2DFunction<ScalarT, ViewType>
    make_view(UniformTabulated2DFunction<ScalarT, ContainerType>& tab) {

        ViewType newTab = make_view<typename ViewType::value_type>(tab.samples_);

        return UniformTabulated2DFunction<ScalarT, ViewType>(tab.xMin(), tab.xMax(), tab.numX(), tab.yMin(), tab.yMax(), tab.numY(), newTab);
    }
}

#endif
