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

#include <opm/input/eclipse/Deck/Deck.hpp>
#include <opm/input/eclipse/Parser/ParserKeywords/B.hpp>
#include <opm/input/eclipse/EclipseState/Tables/Biofpara.hpp>

#include <iostream>

namespace Opm {

Biofpara::Biofpara(const Deck& deck) {
    using BIOFPARA = ParserKeywords::BIOFPARA;
    if (!deck.hasKeyword<BIOFPARA>())
        return;

    const auto& keyword = deck.get<BIOFPARA>().back();
    for (const auto& record : keyword) {
        const double biofilm_density = record.getItem<BIOFPARA::BIOFILM_DENSITY>().getSIDouble(0);
        const double max_growth_rate = record.getItem<BIOFPARA::MAX_GROWTH_RATE>().getSIDouble(0);
        const double half_velocity_coefficient = record.getItem<BIOFPARA::HALF_VELOCITY_COEFFICIENT>().getSIDouble(0);
        const double yield_coefficient = record.getItem<BIOFPARA::YIELD_COEFFICIENT>().getSIDouble(0);
        const double decay_coefficient = record.getItem<BIOFPARA::DECAY_COEFFICIENT>().getSIDouble(0);

        this->data.emplace_back(biofilm_density, max_growth_rate, half_velocity_coefficient, yield_coefficient, decay_coefficient);
    }
}

Biofpara::Biofpara(std::initializer_list<BiofparaRecord> records)
    : data{ records }
{}

bool Biofpara::empty() const {
    return this->data.empty();
}

std::size_t Biofpara::size() const {
    return this->data.size();
}

std::vector< BiofparaRecord >::const_iterator Biofpara::begin() const {
    return this->data.begin();
}

std::vector< BiofparaRecord >::const_iterator Biofpara::end() const {
    return this->data.end();
}

Biofpara Biofpara::serializationTestObject() {
    return Biofpara({{1.0, 2.0, 4.0, 8.0, 16.0}});
}

const BiofparaRecord& Biofpara::operator[](const std::size_t index) const {
    return this->data.at(index);
}

}