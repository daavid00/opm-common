/*
  Copyright (C) 2024 NORCE

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
#ifndef BACTPARA_HPP
#define BACTPARA_HPP

#include <cstddef>
#include <vector>

namespace Opm {
class Deck;

struct BactparaRecord {
    double biofilm_density;
    double max_growth_rate;
    double half_velocity_coefficient;
    double yield_coefficient;
    double decay_coefficient;

    BactparaRecord() = default;
    BactparaRecord(double dens, double grow, double half, double yiel, double deca) :
        biofilm_density(dens),
        max_growth_rate(grow),
        half_velocity_coefficient(half),
        yield_coefficient(yiel),
        decay_coefficient(deca)
    {};

    bool operator==(const BactparaRecord& other) const {
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

class Bactpara {
public:
    Bactpara() = default;
    explicit Bactpara(const Deck& deck);
    explicit Bactpara(std::initializer_list<BactparaRecord> records);
    static Bactpara serializationTestObject();
    std::size_t size() const;
    bool empty() const;
    std::vector< BactparaRecord >::const_iterator begin() const;
    std::vector< BactparaRecord >::const_iterator end() const;
    const BactparaRecord& operator[](const std::size_t index) const;

    bool operator==(const Bactpara& other) const {
        return this->data == other.data;
    }

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(data);
    }

private:
    std::vector<BactparaRecord> data;  

};
}

#endif
