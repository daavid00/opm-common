/*
  Copyright (C) 2024 by NORCE.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef BIOFPARA_HPP
#define BIOFPARA_HPP

#include <cstddef>
#include <vector>

namespace Opm {
class Deck;

struct BiofparaRecord {
    double biofilm_density;
    double max_growth_rate;
    double half_velocity_coefficient;
    double yield_coefficient;
    double decay_coefficient;

    BiofparaRecord() = default;
    BiofparaRecord(double dens, double grow, double half, double yiel, double deca) :
        biofilm_density(dens),
        max_growth_rate(grow),
        half_velocity_coefficient(half),
        yield_coefficient(yiel),
        decay_coefficient(deca)
    {};

    bool operator==(const BiofparaRecord& other) const {
        return this->biofilm_density == other.biofilm_density &&
               this->max_growth_rate == other.max_growth_rate &&
               this->half_velocity_coefficient == other.half_velocity_coefficient &&
               this->yield_coefficient == other.yield_coefficient &&
               this->decay_coefficient == other.decay_coefficient;
    }

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(biofilm_density);
        serializer(max_growth_rate);
        serializer(half_velocity_coefficient);
        serializer(yield_coefficient);
        serializer(decay_coefficient);
    }
};

class Biofpara {
public:
    Biofpara() = default;
    explicit Biofpara(const Deck& deck);
    explicit Biofpara(std::initializer_list<BiofparaRecord> records);
    static Biofpara serializationTestObject();
    std::size_t size() const;
    bool empty() const;
    std::vector< BiofparaRecord >::const_iterator begin() const;
    std::vector< BiofparaRecord >::const_iterator end() const;
    const BiofparaRecord& operator[](const std::size_t index) const;

    bool operator==(const Biofpara& other) const {
        return this->data == other.data;
    }

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(data);
    }

private:
    std::vector<BiofparaRecord> data;  

};
}

#endif
