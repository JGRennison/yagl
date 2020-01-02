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
#include "Action00Objects.h"
#include "StreamHelpers.h"
#include "Descriptors.h"


namespace {


constexpr const char* str_class_label          = "class_label";
constexpr const char* str_class_text_id        = "class_text_id";
constexpr const char* str_object_text_id       = "object_text_id";
constexpr const char* str_climate_availability = "climate_availability";
constexpr const char* str_size_x               = "size_x";
constexpr const char* str_size_y               = "size_y";
constexpr const char* str_cost_factor          = "cost_factor";
constexpr const char* str_introduction_date    = "introduction_date";
constexpr const char* str_end_of_life_date     = "end_of_life_date";
constexpr const char* str_object_flags         = "object_flags";
constexpr const char* str_animation_frames     = "animation_frames";
constexpr const char* str_animation_type       = "animation_type";
constexpr const char* str_animation_speed      = "animation_speed";
constexpr const char* str_animation_triggers   = "animation_triggers";
constexpr const char* str_removal_cost_factor  = "removal_cost_factor";
constexpr const char* str_callback_flags       = "callback_flags";
constexpr const char* str_building_height      = "building_height";
constexpr const char* str_number_of_views      = "number_of_views";
constexpr const char* str_number_on_creation   = "number_on_creation";


// Properties are only 8 bits. Pad to 16 bits to allow sub-properties to be 
// split out and not ambiguous for the parser. Not all features need this, but
// it's simpler to be consistent.
const std::map<std::string, uint16_t> g_indices =
{
    { str_class_label,          0x08'00 },
    { str_class_text_id,        0x09'00 },
    { str_object_text_id,       0x0A'00 },
    { str_climate_availability, 0x0B'00 },
    { str_size_x,               0x0C'00 },
    { str_size_y,               0x0C'01 },
    { str_cost_factor,          0x0D'00 },
    { str_introduction_date,    0x0E'00 },
    { str_end_of_life_date,     0x0F'00 },
    { str_object_flags,         0x10'00 },
    { str_animation_frames,     0x11'00 },
    { str_animation_type,       0x11'01 },
    { str_animation_speed,      0x12'00 },
    { str_animation_triggers,   0x13'00 },
    { str_removal_cost_factor,  0x14'00 },
    { str_callback_flags,       0x15'00 },
    { str_building_height,      0x16'00 },
    { str_number_of_views,      0x17'00 },
    { str_number_on_creation,   0x18'00 },
};


constexpr IntegerDescriptorT<uint32_t> desc_08  = { 0x08, str_class_label,          PropFormat::Hex };
constexpr IntegerDescriptorT<uint16_t> desc_09  = { 0x09, str_class_text_id,        PropFormat::Hex };
constexpr IntegerDescriptorT<uint16_t> desc_0A  = { 0x0A, str_object_text_id,       PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>  desc_0B  = { 0x0B, str_climate_availability, PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>  desc_0C0 = { 0x0C, str_size_x,               PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>  desc_0C1 = { 0x0C, str_size_y,               PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>  desc_0D  = { 0x0D, str_cost_factor,          PropFormat::Hex };
constexpr IntegerDescriptorT<uint32_t> desc_0E  = { 0x0E, str_introduction_date,    PropFormat::Hex };
constexpr IntegerDescriptorT<uint32_t> desc_0F  = { 0x0F, str_end_of_life_date,     PropFormat::Hex };
constexpr IntegerDescriptorT<uint16_t> desc_10  = { 0x10, str_object_flags,         PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>  desc_110 = { 0x11, str_animation_frames,     PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>  desc_111 = { 0x11, str_animation_type,       PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>  desc_12  = { 0x12, str_animation_speed,      PropFormat::Hex };
constexpr IntegerDescriptorT<uint16_t> desc_13  = { 0x13, str_animation_triggers,   PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>  desc_14  = { 0x14, str_removal_cost_factor,  PropFormat::Hex };
constexpr IntegerDescriptorT<uint16_t> desc_15  = { 0x15, str_callback_flags,       PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>  desc_16  = { 0x16, str_building_height,      PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>  desc_17  = { 0x17, str_number_of_views,      PropFormat::Hex };
constexpr IntegerDescriptorT<uint8_t>  desc_18  = { 0x18, str_number_on_creation,      PropFormat::Hex };
    

} // namespace {


bool Action00Objects::read_property(std::istream& is, uint8_t property)
{
    switch (property)
    {
        case 0x08: m_08_class_label            = read_uint32(is); break;
        case 0x09: m_09_class_text_id          = read_uint16(is); break;
        case 0x0A: m_0A_object_text_id         = read_uint16(is); break;
        case 0x0B: m_0B_climate_availability   = read_uint8(is); break;
        case 0x0C: m_0C_size_x                 = read_uint8(is);
                   m_0C_size_y                 = ((m_0C_size_x & 0xF0) >> 4);
                   m_0C_size_x                &= 0x0F; break;
        case 0x0D: m_0D_cost_factor            = read_uint8(is); break;
        case 0x0E: m_0E_introduction_date      = read_uint32(is); break;
        case 0x0F: m_0F_end_of_life_date       = read_uint32(is); break;
        case 0x10: m_10_object_flags           = read_uint16(is); break;
        case 0x11: m_11_animation_frames       = read_uint8(is);
                   m_11_animation_type         = read_uint8(is); break;
        case 0x12: m_12_animation_speed        = read_uint8(is); break;
        case 0x13: m_13_animation_triggers     = read_uint16(is); break;
        case 0x14: m_14_removal_cost_factor    = read_uint8(is); break;
        case 0x15: m_15_callback_flags         = read_uint16(is); break;
        case 0x16: m_16_building_height        = read_uint8(is); break;
        case 0x17: m_17_number_of_views        = read_uint8(is); break;
        case 0x18: m_18_number_on_creation     = read_uint8(is); break;
        default:   throw PROPERTY_ERROR("Unknown property", property);
    }

    return true;
}   


bool Action00Objects::write_property(std::ostream& os, uint8_t property) const
{
    switch (property)
    {
        case 0x08: write_uint32(os, m_08_class_label); break;
        case 0x09: write_uint16(os, m_09_class_text_id); break;
        case 0x0A: write_uint16(os, m_0A_object_text_id); break;
        case 0x0B: write_uint8(os, m_0B_climate_availability); break;
        case 0x0C: write_uint8(os, (m_0C_size_x & 0x0F) | ((m_0C_size_y & 0x0F) << 4)); break;
        case 0x0D: write_uint8(os, m_0D_cost_factor); break;
        case 0x0E: write_uint32(os, m_0E_introduction_date); break;
        case 0x0F: write_uint32(os, m_0F_end_of_life_date); break;
        case 0x10: write_uint16(os, m_10_object_flags); break;
        case 0x11: write_uint8(os, (m_11_animation_type == 0xFF) ? 0xFF : m_11_animation_frames);
                   write_uint8(os, m_11_animation_type); break;
        case 0x12: write_uint8(os, m_12_animation_speed); break;
        case 0x13: write_uint16(os, m_13_animation_triggers); break;
        case 0x14: write_uint8(os, m_14_removal_cost_factor); break;
        case 0x15: write_uint16(os, m_15_callback_flags); break;
        case 0x16: write_uint8(os, m_16_building_height); break;
        case 0x17: write_uint8(os, m_17_number_of_views); break;
        case 0x18: write_uint8(os, m_18_number_on_creation); break;
        default:   throw PROPERTY_ERROR("Unknown property", property);
    }

    return true;
}


bool Action00Objects::print_property(std::ostream& os, uint8_t property, uint16_t indent) const 
{
    switch (property)
    {
        case 0x08: desc_08.print(m_08_class_label, os, indent); break;
        case 0x09: desc_09.print(m_09_class_text_id, os, indent); break;
        case 0x0A: desc_0A.print(m_0A_object_text_id, os, indent); break;
        case 0x0B: desc_0B.print(m_0B_climate_availability, os, indent); break;
        case 0x0C: desc_0C0.print(m_0C_size_x, os, indent); 
                   desc_0C1.print(m_0C_size_y, os, indent); break;
        case 0x0D: desc_0D.print(m_0D_cost_factor, os, indent); break;
        case 0x0E: desc_0E.print(m_0E_introduction_date, os, indent); break;
        case 0x0F: desc_0F.print(m_0F_end_of_life_date, os, indent); break;
        case 0x10: desc_10.print(m_10_object_flags, os, indent); break;
        case 0x11: desc_110.print(m_11_animation_frames, os, indent); 
                   desc_111.print(m_11_animation_type, os, indent); break; 
        case 0x12: desc_12.print(m_12_animation_speed, os, indent); break;
        case 0x13: desc_13.print(m_13_animation_triggers, os, indent); break;
        case 0x14: desc_14.print(m_14_removal_cost_factor, os, indent); break;
        case 0x15: desc_15.print(m_15_callback_flags, os, indent); break;
        case 0x16: desc_16.print(m_16_building_height, os, indent); break;
        case 0x17: desc_17.print(m_17_number_of_views, os, indent); break;
        case 0x18: desc_18.print(m_18_number_on_creation, os, indent); break;
        default:   throw PROPERTY_ERROR("Unknown property", property);
    }

    return true;
}


bool Action00Objects::parse_property(TokenStream& is, const std::string& name, uint8_t& property)
{
    const auto& it = g_indices.find(name);
    if (it != g_indices.end())
    {
        uint16_t index = it->second;
        property = (index >> 8); // The property index is in the high byte.
        switch (index)
        {
            case 0x08'00: desc_08.parse(m_08_class_label, is); break;
            case 0x09'00: desc_09.parse(m_09_class_text_id, is); break;
            case 0x0A'00: desc_0A.parse(m_0A_object_text_id, is); break;
            case 0x0B'00: desc_0B.parse(m_0B_climate_availability, is); break;
            case 0x0C'00: desc_0C0.parse(m_0C_size_x, is); break;
            case 0x0C'01: desc_0C1.parse(m_0C_size_y, is); break;
            case 0x0D'00: desc_0D.parse(m_0D_cost_factor, is); break;
            case 0x0E'00: desc_0E.parse(m_0E_introduction_date, is); break;
            case 0x0F'00: desc_0F.parse(m_0F_end_of_life_date, is); break;
            case 0x10'00: desc_10.parse(m_10_object_flags, is); break;
            case 0x11'00: desc_110.parse(m_11_animation_frames, is); break;
            case 0x11'01: desc_111.parse(m_11_animation_type, is); break;
            case 0x12'00: desc_12.parse(m_12_animation_speed, is); break;
            case 0x13'00: desc_13.parse(m_13_animation_triggers, is); break;
            case 0x14'00: desc_14.parse(m_14_removal_cost_factor, is); break;
            case 0x15'00: desc_15.parse(m_15_callback_flags, is); break;
            case 0x16'00: desc_16.parse(m_16_building_height, is); break;
            case 0x17'00: desc_17.parse(m_17_number_of_views, is); break;
            case 0x18'00: desc_18.parse(m_18_number_on_creation, is); break;
            default:      throw PROPERTY_ERROR("Unknown property", property);
        }

        return true;
    }

    throw PROPERTY_ERROR("Unknown property", property);
}
