/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2013 AudioScience Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * configuration_descriptor_imp.cpp
 *
 * CONFIGURATION descriptor implementation
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include "enumeration.h"
#include "log_imp.h"
#include "util.h"
#include "end_station_imp.h"
#include "descriptor_base_imp.h"
#include "configuration_descriptor_imp.h"

namespace avdecc_lib
{
    configuration_descriptor_imp::configuration_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj, frame, frame_len, pos)
    {
        ssize_t ret = jdksavdecc_descriptor_configuration_read(&config_desc, frame, pos, frame_len);

        if(ret < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, config_desc_read error", end_station_obj->entity_id());
            assert(ret >= 0);
        }

        desc_type_vec_init(frame, pos);
        desc_count_vec_init(frame, pos);
    }

    template <typename T>
    void delete_pointed_to(T* const ptr)
    {
        delete ptr;
    }

    configuration_descriptor_imp::~configuration_descriptor_imp()
    {
        std::map<uint16_t, DITEM>::iterator it;
        for (it = m_all_desc.begin(); it != m_all_desc.end(); ++it)
            std::for_each(it->second.begin(), it->second.end(), delete_pointed_to<descriptor_base_imp>);
        m_all_desc.clear();
    }

    size_t configuration_descriptor_imp::desc_count(uint16_t desc_type)
    {
        if (m_all_desc.find(desc_type) == m_all_desc.end())
            return 0;
        else
            return m_all_desc[desc_type].size();
    }
    descriptor_base_imp * configuration_descriptor_imp::lookup_desc_imp(uint16_t desc_type, size_t index)
    {
        if (desc_count(desc_type) <= index)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, lookup_desc(%s,%d) error",
                                      base_end_station_imp_ref->entity_id(),
                                      utility::aem_desc_value_to_name(desc_type),
                                      index);
            return NULL;
        }
        else
        {
            return m_all_desc[desc_type].at(index);
        }
    }
    
    descriptor_base * configuration_descriptor_imp::lookup_desc(uint16_t desc_type, size_t index)
    {
        descriptor_base_imp *imp = lookup_desc_imp(desc_type, index);
        return imp;
    }

    uint16_t STDCALL configuration_descriptor_imp::descriptor_type() const
    {
        assert(config_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_CONFIGURATION);
        return config_desc.descriptor_type;
    }

    uint16_t STDCALL configuration_descriptor_imp::descriptor_index() const
    {
        return config_desc.descriptor_index;
    }

    uint8_t * STDCALL configuration_descriptor_imp::object_name()
    {
        return config_desc.object_name.value;
    }

    uint16_t STDCALL configuration_descriptor_imp::localized_description()
    {
        return config_desc.localized_description;
    }

    uint16_t STDCALL configuration_descriptor_imp::descriptor_counts_count()
    {
        assert(config_desc.descriptor_counts_count <= 108);
        return config_desc.descriptor_counts_count;
    }

    uint16_t configuration_descriptor_imp::descriptor_counts_offset()
    {
        assert(config_desc.descriptor_counts_offset == 74);
        return config_desc.descriptor_counts_offset;
    }

    void configuration_descriptor_imp::desc_type_vec_init(const uint8_t *frame, size_t pos)
    {
        uint16_t offset = 0;

        for(uint32_t i = 0; i < descriptor_counts_count(); i++)
        {
            desc_type_vec.push_back(jdksavdecc_uint16_get(frame, descriptor_counts_offset() + pos + offset));
            offset += 0x4;
        }
    }

    void configuration_descriptor_imp::desc_count_vec_init(const uint8_t *frame, size_t pos)
    {
        uint16_t offset = 0x2;

        for(uint32_t i = 0; i < descriptor_counts_count(); i++)
        {
            desc_count_vec.push_back(jdksavdecc_uint16_get(frame, descriptor_counts_offset() + pos + offset));
            offset += 0x4;
        }
    }

    bool STDCALL configuration_descriptor_imp::are_desc_type_and_index_in_config(int desc_type, int desc_count_index)
    {
        int desc_index;

        for(uint32_t i = 0; i < desc_type_vec.size(); i++)
        {
            if(desc_type_vec.at(i) == desc_type)
            {
                desc_index = i;

                if(desc_count_index < desc_count_vec.at(desc_index))
                {
                    return true;
                }
            }
        }

        return false;
    }

    uint16_t STDCALL configuration_descriptor_imp::get_desc_type_from_config_by_index(int desc_index)
    {
        return desc_type_vec.at(desc_index);
    }

    uint16_t STDCALL configuration_descriptor_imp::get_desc_count_from_config_by_index(int desc_index)
    {
        return desc_count_vec.at(desc_index);
    }

    /*
    template <class T>
    static void add_or_replace_descriptor_and_sort(T *d, std::vector<T *> &desc_vec)
    {
        typename std::vector<T*>::iterator it;

        it = std::find_if(
                 desc_vec.begin(),
                 desc_vec.end(),
                 [d](T const* n)
        {
            return *d == *n;
        }
             );

        if (it != desc_vec.end())
        {
            // If the descriptor was found, delete the old and add the new in its place
            delete *it;
            it = desc_vec.erase(it);
            desc_vec.insert(it, d);
        }
        else
        {
            // Insert the descriptor in the list and sort by descriptor ID
            desc_vec.push_back(d);
            std::sort(
                desc_vec.begin(),
                desc_vec.end(),
                [](T const* a, T const* b)
            {
                return *a < *b;
            });
        }
    }
    */

    void configuration_descriptor_imp::update_desc_database(descriptor_base_imp *desc, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        uint16_t desc_type = desc->descriptor_type();
        uint16_t desc_index = desc->descriptor_index();

        if (m_all_desc[desc_type].size() <= desc_index)
            m_all_desc[desc_type].resize(desc_index + 1);
        if (m_all_desc[desc_type][desc_index])
        {
            // exists
            desc->replace_desc_frame(frame, pos, frame_len);
            delete desc;
        }
        else
        {
            // does not exist
            m_all_desc[desc_type][desc_index] = desc;
        }
    }

    void configuration_descriptor_imp::store_entity_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new entity_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_audio_unit_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new audio_unit_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_stream_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new stream_input_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_stream_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new stream_output_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_jack_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new jack_input_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_jack_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new jack_output_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_avb_interface_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new avb_interface_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_clock_source_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new clock_source_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_memory_object_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new memory_object_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_locale_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new locale_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_strings_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new strings_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_stream_port_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new stream_port_input_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_stream_port_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new stream_port_output_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_audio_cluster_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new audio_cluster_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_audio_map_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new audio_map_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_clock_domain_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new clock_domain_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_control_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new control_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_external_port_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new external_port_input_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }

    void configuration_descriptor_imp::store_external_port_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        update_desc_database(new external_port_output_descriptor_imp(end_station_obj, frame, pos, frame_len), frame, pos, frame_len);
    }
    
    size_t STDCALL configuration_descriptor_imp::entity_desc_count()
    {
        return desc_count(AEM_DESC_ENTITY);
    }
    
    size_t STDCALL configuration_descriptor_imp::audio_unit_desc_count()
    {
        return desc_count(AEM_DESC_AUDIO_UNIT);
    }

    size_t STDCALL configuration_descriptor_imp::stream_input_desc_count()
    {
        return desc_count(AEM_DESC_STREAM_INPUT);
    }
    
    size_t STDCALL configuration_descriptor_imp::stream_output_desc_count()
    {
        return desc_count(AEM_DESC_STREAM_OUTPUT);
    }

    size_t STDCALL configuration_descriptor_imp::jack_input_desc_count()
    {
        return desc_count(AEM_DESC_JACK_INPUT);
    }

    size_t STDCALL configuration_descriptor_imp::jack_output_desc_count()
    {
        return desc_count(AEM_DESC_JACK_OUTPUT);
    }

    size_t STDCALL configuration_descriptor_imp::avb_interface_desc_count()
    {
        return desc_count(AEM_DESC_AVB_INTERFACE);
    }

    size_t STDCALL configuration_descriptor_imp::clock_source_desc_count()
    {
        return desc_count(AEM_DESC_CLOCK_SOURCE);
    }

    size_t STDCALL configuration_descriptor_imp::memory_object_desc_count()
    {
        return desc_count(AEM_DESC_MEMORY_OBJECT);
    }

    size_t STDCALL configuration_descriptor_imp::locale_desc_count()
    {
        return desc_count(AEM_DESC_LOCALE);
    }

    size_t STDCALL configuration_descriptor_imp::strings_desc_count()
    {
        return desc_count(AEM_DESC_STRINGS);
    }

    size_t STDCALL configuration_descriptor_imp::stream_port_input_desc_count()
    {
        return desc_count(AEM_DESC_STREAM_PORT_INPUT);
    }

    size_t STDCALL configuration_descriptor_imp::stream_port_output_desc_count()
    {
        return desc_count(AEM_DESC_STREAM_PORT_OUTPUT);
    }

    size_t STDCALL configuration_descriptor_imp::audio_cluster_desc_count()
    {
        return desc_count(AEM_DESC_AUDIO_CLUSTER);
    }

    size_t STDCALL configuration_descriptor_imp::audio_map_desc_count()
    {
        return desc_count(AEM_DESC_AUDIO_MAP);
    }

    size_t STDCALL configuration_descriptor_imp::clock_domain_desc_count()
    {
        return desc_count(AEM_DESC_CLOCK_DOMAIN);
    }

    size_t STDCALL configuration_descriptor_imp::control_desc_count()
    {
        return desc_count(AEM_DESC_CONTROL);
    }

    size_t STDCALL configuration_descriptor_imp::external_port_input_desc_count()
    {
        return desc_count(AEM_DESC_EXTERNAL_PORT_INPUT);
    }

    size_t STDCALL configuration_descriptor_imp::external_port_output_desc_count()
    {
        return desc_count(AEM_DESC_EXTERNAL_PORT_OUTPUT);
    }

    entity_descriptor * STDCALL configuration_descriptor_imp::get_entity_descriptor_by_index(size_t entity_desc_index)
    {
        return dynamic_cast<entity_descriptor *>(lookup_desc(AEM_DESC_ENTITY, entity_desc_index));
    }
    
    audio_unit_descriptor * STDCALL configuration_descriptor_imp::get_audio_unit_desc_by_index(size_t audio_unit_desc_index)
    {
        return dynamic_cast<audio_unit_descriptor *>(lookup_desc(AEM_DESC_AUDIO_UNIT, audio_unit_desc_index));
    }

    stream_input_descriptor * STDCALL configuration_descriptor_imp::get_stream_input_desc_by_index(size_t stream_input_desc_index)
    {
        return dynamic_cast<stream_input_descriptor *>(lookup_desc(AEM_DESC_STREAM_INPUT, stream_input_desc_index));
    }

    stream_output_descriptor * STDCALL configuration_descriptor_imp::get_stream_output_desc_by_index(size_t stream_output_desc_index)
    {
        return dynamic_cast<stream_output_descriptor *>(lookup_desc(AEM_DESC_STREAM_OUTPUT, stream_output_desc_index));
    }

    jack_input_descriptor * STDCALL configuration_descriptor_imp::get_jack_input_desc_by_index(size_t jack_input_desc_index)
    {
        return dynamic_cast<jack_input_descriptor *>(lookup_desc(AEM_DESC_JACK_INPUT, jack_input_desc_index));
    }

    jack_output_descriptor * STDCALL configuration_descriptor_imp::get_jack_output_desc_by_index(size_t jack_output_desc_index)
    {
        return dynamic_cast<jack_output_descriptor *>(lookup_desc(AEM_DESC_JACK_OUTPUT, jack_output_desc_index));
    }

    avb_interface_descriptor * STDCALL configuration_descriptor_imp::get_avb_interface_desc_by_index(size_t avb_interface_desc_index)
    {
        return dynamic_cast<avb_interface_descriptor *>(lookup_desc(AEM_DESC_AVB_INTERFACE, avb_interface_desc_index));
    }

    clock_source_descriptor * STDCALL configuration_descriptor_imp::get_clock_source_desc_by_index(size_t clock_source_desc_index)
    {
        return dynamic_cast<clock_source_descriptor *>(lookup_desc(AEM_DESC_CLOCK_SOURCE, clock_source_desc_index));
    }

    memory_object_descriptor * STDCALL configuration_descriptor_imp::get_memory_object_desc_by_index(size_t memory_object_desc_index)
    {
        return dynamic_cast<memory_object_descriptor *>(lookup_desc(AEM_DESC_MEMORY_OBJECT, memory_object_desc_index));
    }

    locale_descriptor * STDCALL configuration_descriptor_imp::get_locale_desc_by_index(size_t locale_desc_index)
    {
        return dynamic_cast<locale_descriptor *>(lookup_desc(AEM_DESC_LOCALE, locale_desc_index));
    }

    strings_descriptor * STDCALL configuration_descriptor_imp::get_strings_desc_by_index(size_t strings_desc_index)
    {
        return dynamic_cast<strings_descriptor *>(lookup_desc(AEM_DESC_STRINGS, strings_desc_index));
    }

    int STDCALL configuration_descriptor_imp::get_strings_desc_string_by_reference(size_t reference, size_t &string_desc_index, size_t &string_index)
    {
        if (reference == 0xffff)
        {
            return -1;
        }

        string_desc_index = reference >> 3;
        string_index = reference & 0x7;

        return 0;
    }

    stream_port_input_descriptor * STDCALL configuration_descriptor_imp::get_stream_port_input_desc_by_index(size_t stream_port_input_desc_index)
    {
        return dynamic_cast<stream_port_input_descriptor *>(lookup_desc(AEM_DESC_STREAM_PORT_INPUT, stream_port_input_desc_index));
    }

    stream_port_output_descriptor * STDCALL configuration_descriptor_imp::get_stream_port_output_desc_by_index(size_t stream_port_output_desc_index)
    {
        return dynamic_cast<stream_port_output_descriptor *>(lookup_desc(AEM_DESC_STREAM_PORT_OUTPUT, stream_port_output_desc_index));
    }

    audio_cluster_descriptor * STDCALL configuration_descriptor_imp::get_audio_cluster_desc_by_index(size_t audio_cluster_desc_index)
    {
        return dynamic_cast<audio_cluster_descriptor *>(lookup_desc(AEM_DESC_AUDIO_CLUSTER, audio_cluster_desc_index));
    }

    audio_map_descriptor * STDCALL configuration_descriptor_imp::get_audio_map_desc_by_index(size_t audio_map_desc_index)
    {
        return dynamic_cast<audio_map_descriptor *>(lookup_desc(AEM_DESC_AUDIO_MAP, audio_map_desc_index));
    }

    clock_domain_descriptor * STDCALL configuration_descriptor_imp::get_clock_domain_desc_by_index(size_t clock_domain_desc_index)
    {
        return dynamic_cast<clock_domain_descriptor *>(lookup_desc(AEM_DESC_CLOCK_DOMAIN, clock_domain_desc_index));
    }

    control_descriptor * STDCALL configuration_descriptor_imp::get_control_desc_by_index(size_t control_desc_index)
    {
        return dynamic_cast<control_descriptor *>(lookup_desc(AEM_DESC_CONTROL, control_desc_index));
    }

    external_port_input_descriptor * STDCALL configuration_descriptor_imp::get_external_port_input_desc_by_index(size_t index)
    {
        return dynamic_cast<external_port_input_descriptor *>(lookup_desc(AEM_DESC_EXTERNAL_PORT_INPUT, index));
    }

    external_port_output_descriptor * STDCALL configuration_descriptor_imp::get_external_port_output_desc_by_index(size_t index)
    {
        return dynamic_cast<external_port_output_descriptor *>(lookup_desc(AEM_DESC_EXTERNAL_PORT_OUTPUT, index));
    }
}
