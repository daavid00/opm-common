/*
  Copyright 2026 NORCE Research AS.

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

#include <opm/input/eclipse/Schedule/Well/WellParticleProperties.hpp>

#include <opm/input/eclipse/Deck/DeckRecord.hpp>
#include <opm/input/eclipse/Parser/ParserKeywords/W.hpp>

Opm::WellParticleProperties Opm::WellParticleProperties::serializationTestObject()
{
    Opm::WellParticleProperties result;
    result.m_particleConcentration = 1.0;

    return result;
}

void Opm::WellParticleProperties::handleWPARTICL(const DeckRecord& rec)
{
    this->m_particleConcentration = rec.getItem<ParserKeywords::WPARTICL::PARTICLE_CONCENTRATION>().getSIDouble(0);
}

bool Opm::WellParticleProperties::operator==(const WellParticleProperties& other) const
{
    if (m_particleConcentration == other.m_particleConcentration)
        return true;
    else
        return false;
}

bool Opm::WellParticleProperties::operator!=(const WellParticleProperties& other) const
{
    return !(*this == other);
}
