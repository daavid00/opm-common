/*
  Copyright 2024 NORCE

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
#include <opm/input/eclipse/Deck/Deck.hpp>
#include <opm/input/eclipse/Parser/ParserKeywords/B.hpp>
#include <opm/input/eclipse/EclipseState/Tables/Bactpara.hpp>

#include <iostream>

namespace Opm {

Bactpara::Bactpara(const Deck& deck) {
    using BACTPARA = ParserKeywords::BACTPARA;
    if (!deck.hasKeyword<BACTPARA>())
        return;
    
    const auto& keyword = deck.get<BACTPARA>().back();
    for (const auto& record : keyword) {
        const double biofilm_density = record.getItem<BACTPARA::BIOFILM_DENSITY>().getSIDouble(0);
        const double max_growth_rate = record.getItem<BACTPARA::MAX_GROWTH_RATE>().getSIDouble(0);
        const double half_velocity_coefficient = record.getItem<BACTPARA::HALF_VELOCITY_COEFFICIENT>().getSIDouble(0);
        const double yield_coefficient = record.getItem<BACTPARA::YIELD_COEFFICIENT>().getSIDouble(0);
        const double decay_coefficient = record.getItem<BACTPARA::DECAY_COEFFICIENT>().getSIDouble(0);
        
        this->data.emplace_back(biofilm_density, max_growth_rate, half_velocity_coefficient, yield_coefficient, decay_coefficient);
    }
}

Bactpara::Bactpara(std::initializer_list<BactparaRecord> records)
    : data{ records }
{}

bool Bactpara::empty() const {
    return this->data.empty();
}

std::size_t Bactpara::size() const {
    return this->data.size();
}

std::vector< BactparaRecord >::const_iterator Bactpara::begin() const {
    return this->data.begin();
}

std::vector< BactparaRecord >::const_iterator Bactpara::end() const {
    return this->data.end();
}

Bactpara Bactpara::serializationTestObject() {
    return Bactpara({{1.0, 2.0, 4.0, 8.0, 16.0}});
}


const BactparaRecord& Bactpara::operator[](const std::size_t index) const {
    return this->data.at(index);
}

}
