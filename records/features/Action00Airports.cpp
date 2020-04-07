///////////////////////////////////////////////////////////////////////////////
// Copyright 2019 Alan Chambers (unicycle.bloke@gmail.com)
//
// This file is part of yagl.
//
// yagl is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// yagl is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with yagl. If not, see <https://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////////
#include "Action00Airports.h"
#include "Descriptors.h"
#include "StreamHelpers.h"


namespace {


constexpr const char* str_sprite_id               = "sprite_id";
constexpr const char* str_airport_override_id     = "airport_override_id";
constexpr const char* str_airport_layouts         = "airport_layouts";
constexpr const char* str_years_available         = "years_available";
constexpr const char* str_compatible_ttd_airport  = "compatible_ttd_airport";
constexpr const char* str_catchment_area          = "catchment_area";
constexpr const char* str_noise_level             = "noise_level";
constexpr const char* str_airport_name_id         = "airport_name_id";
constexpr const char* str_maintenance_cost_factor = "maintenance_cost_factor";


// Properties are only 8 bits. Pad to 16 bits to allow sub-properties to be 
// split out and not ambiguous for the parser. Not all features need this, but
// it's simpler to be consistent.
const std::map<std::string, uint16_t> g_indices =
{
    { str_airport_override_id,       0x08'00 },
    { str_airport_layouts,           0x0A'00 },
    { str_years_available,           0x0C'00 },
    { str_compatible_ttd_airport,    0x0D'00 },
    { str_catchment_area,            0x0E'00 },
    { str_noise_level,               0x0F'00 },
    { str_airport_name_id,           0x10'00 },
    { str_maintenance_cost_factor,   0x11'00 },
};


constexpr IntegerDescriptorT<uint8_t>   desc_08 = { 0x08, str_airport_override_id,        PropFormat::Hex };
constexpr AirportLayoutsDescriptor      desc_0A = { 0x0A, str_airport_layouts };
constexpr YearsAvailableDescriptor      desc_0C = { 0x0C, str_years_available,            PropFormat::Dec };
constexpr IntegerDescriptorT<uint8_t>   desc_0D = { 0x0D, str_compatible_ttd_airport,     PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>   desc_0E = { 0x0E, str_catchment_area,             PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>   desc_0F = { 0x0F, str_noise_level,                PropFormat::Hex };
constexpr IntegerDescriptorT<uint16_t>  desc_10 = { 0x10, str_airport_name_id,            PropFormat::Hex };
constexpr IntegerDescriptorT<uint16_t>  desc_11 = { 0x11, str_maintenance_cost_factor,    PropFormat::Hex };


} // namespace {


bool Action00Airports::read_property(std::istream& is, uint8_t property)
{
    switch (property)
    {
        case 0x08: m_08_airport_override_id     = read_uint8(is); break;
        case 0x0A: m_0A_airport_layouts.read(is, AirportType::Airport); break;
        case 0x0C: desc_0C.read(m_0C_years_available, is); break;
        case 0x0D: m_0D_compatible_ttd_airport  = read_uint8(is); break;
        case 0x0E: m_0E_catchment_area          = read_uint8(is); break;
        case 0x0F: m_0F_noise_level             = read_uint8(is); break;
        case 0x10: m_10_airport_name_id         = read_uint16(is); break;
        case 0x11: m_11_maintenance_cost_factor = read_uint16(is); break;
        default:   throw PROPERTY_ERROR("Unknown property", property);
    }

    return true;
}   


bool Action00Airports::write_property(std::ostream& os, uint8_t property) const
{
    switch (property)
    {
        case 0x08: write_uint8(os,  m_08_airport_override_id); break;
        case 0x0A: m_0A_airport_layouts.write(os, AirportType::Airport); break;
        case 0x0C: desc_0C.write(m_0C_years_available, os); break;
        case 0x0D: write_uint8(os,  m_0D_compatible_ttd_airport); break;
        case 0x0E: write_uint8(os,  m_0E_catchment_area); break;
        case 0x0F: write_uint8(os,  m_0F_noise_level); break;
        case 0x10: write_uint16(os, m_10_airport_name_id); break;
        case 0x11: write_uint16(os, m_11_maintenance_cost_factor); break;
        default:   throw PROPERTY_ERROR("Unknown property", property);
    }

    return true;
}


bool Action00Airports::print_property(std::ostream& os, uint8_t property, uint16_t indent) const 
{
    switch (property)
    {
        case 0x08: desc_08.print(m_08_airport_override_id, os, indent); break;
        case 0x0A: desc_0A.print(m_0A_airport_layouts, os, indent, AirportType::Airport); break;
        case 0x0C: desc_0C.print(m_0C_years_available, os, indent); break;
        case 0x0D: desc_0D.print(m_0D_compatible_ttd_airport, os, indent); break;
        case 0x0E: desc_0E.print(m_0E_catchment_area, os, indent); break;
        case 0x0F: desc_0F.print(m_0F_noise_level, os, indent); break;
        case 0x10: desc_10.print(m_10_airport_name_id, os, indent); break;
        case 0x11: desc_11.print(m_11_maintenance_cost_factor, os, indent); break;
        default:   throw PROPERTY_ERROR("Unknown property", property);
    }

    return true;
}


bool Action00Airports::parse_property(TokenStream& is, const std::string& name, uint8_t& property)
{
    const auto& it = g_indices.find(name);
    if (it != g_indices.end())
    {
        uint16_t index = it->second;
        property = (index >> 8); // The property index is in the high byte.
        switch (index)
        {
            case 0x08'00: desc_08.parse(m_08_airport_override_id, is); break;
            case 0x0A'00: desc_0A.parse(m_0A_airport_layouts, is, AirportType::Airport); break;
            case 0x0C'00: desc_0C.parse(m_0C_years_available, is); break;
            case 0x0D'00: desc_0D.parse(m_0D_compatible_ttd_airport, is); break;
            case 0x0E'00: desc_0E.parse(m_0E_catchment_area, is); break;
            case 0x0F'00: desc_0F.parse(m_0F_noise_level, is); break;
            case 0x10'00: desc_10.parse(m_10_airport_name_id, is); break;
            case 0x11'00: desc_11.parse(m_11_maintenance_cost_factor, is); break;
            default:      throw PROPERTY_ERROR("Unknown property", property);
        }

        return true;
    }

    throw PROPERTY_ERROR("Unknown property", property);
}
