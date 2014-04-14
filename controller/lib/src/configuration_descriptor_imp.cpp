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
#include "end_station_imp.h"
#include "descriptor_base_imp.h"
#include "configuration_descriptor_imp.h"

namespace avdecc_lib
{
    configuration_descriptor_imp::configuration_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
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
        std::for_each(audio_unit_desc_vec.begin(), audio_unit_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(stream_input_desc_vec.begin(), stream_input_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(stream_output_desc_vec.begin(), stream_output_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(jack_input_desc_vec.begin(), jack_input_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(jack_output_desc_vec.begin(), jack_output_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(avb_interface_desc_vec.begin(), avb_interface_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(clock_source_desc_vec.begin(), clock_source_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(memory_object_desc_vec.begin(), memory_object_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(locale_desc_vec.begin(), locale_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(strings_desc_vec.begin(), strings_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(stream_port_input_desc_vec.begin(), stream_port_input_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(stream_port_output_desc_vec.begin(), stream_port_output_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(audio_cluster_desc_vec.begin(), audio_cluster_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(audio_map_desc_vec.begin(), audio_map_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(clock_domain_desc_vec.begin(), clock_domain_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(control_desc_vec.begin(), control_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(external_port_input_desc_vec.begin(), external_port_input_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
        std::for_each(external_port_output_desc_vec.begin(), external_port_output_desc_vec.end(), delete_pointed_to<descriptor_base_imp>);
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

    void configuration_descriptor_imp::store_audio_unit_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        audio_unit_descriptor_imp *d = new audio_unit_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, audio_unit_desc_vec);
    }

    void configuration_descriptor_imp::store_stream_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        stream_input_descriptor_imp *d = new stream_input_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, stream_input_desc_vec);
    }

    void configuration_descriptor_imp::store_stream_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        stream_output_descriptor_imp *d = new stream_output_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, stream_output_desc_vec);
    }

    void configuration_descriptor_imp::store_jack_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        jack_input_descriptor_imp *d = new jack_input_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, jack_input_desc_vec);
    }

    void configuration_descriptor_imp::store_jack_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        jack_output_descriptor_imp *d = new jack_output_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, jack_output_desc_vec);
    }

    void configuration_descriptor_imp::store_avb_interface_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        avb_interface_descriptor_imp *d = new avb_interface_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, avb_interface_desc_vec);
    }

    void configuration_descriptor_imp::store_clock_source_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        clock_source_descriptor_imp *d = new clock_source_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, clock_source_desc_vec);
    }

    void configuration_descriptor_imp::store_memory_object_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        memory_object_descriptor_imp *d = new memory_object_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, memory_object_desc_vec);
    }

    void configuration_descriptor_imp::store_locale_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        locale_descriptor_imp *d = new locale_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, locale_desc_vec);
    }

    void configuration_descriptor_imp::store_strings_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        strings_descriptor_imp *d = new strings_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, strings_desc_vec);
    }

    void configuration_descriptor_imp::store_stream_port_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        stream_port_input_desc_vec.push_back(new stream_port_input_descriptor_imp(end_station_obj, frame, pos, frame_len));
    }

    void configuration_descriptor_imp::store_stream_port_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        stream_port_output_desc_vec.push_back(new stream_port_output_descriptor_imp(end_station_obj, frame, pos, frame_len));
    }

    void configuration_descriptor_imp::store_audio_cluster_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        audio_cluster_descriptor_imp *d = new audio_cluster_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, audio_cluster_desc_vec);
    }

    void configuration_descriptor_imp::store_audio_map_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        audio_map_descriptor_imp *d = new audio_map_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, audio_map_desc_vec);
    }

    void configuration_descriptor_imp::store_clock_domain_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        clock_domain_descriptor_imp *d = new clock_domain_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, clock_domain_desc_vec);
    }

    void configuration_descriptor_imp::store_control_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        control_descriptor_imp *d = new control_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, control_desc_vec);
    }

    void configuration_descriptor_imp::store_external_port_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        external_port_input_descriptor_imp *d = new external_port_input_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, external_port_input_desc_vec);
    }

    void configuration_descriptor_imp::store_external_port_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
    {
        external_port_output_descriptor_imp *d = new external_port_output_descriptor_imp(end_station_obj, frame, pos, frame_len);
        add_or_replace_descriptor_and_sort(d, external_port_output_desc_vec);
    }

    size_t STDCALL configuration_descriptor_imp::audio_unit_desc_count()
    {
        return audio_unit_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::stream_input_desc_count()
    {
        return stream_input_desc_vec.size();
    }
    size_t STDCALL configuration_descriptor_imp::stream_output_desc_count()
    {
        return stream_output_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::jack_input_desc_count()
    {
        return jack_input_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::jack_output_desc_count()
    {
        return jack_output_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::avb_interface_desc_count()
    {
        return avb_interface_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::clock_source_desc_count()
    {
        return clock_source_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::memory_object_desc_count()
    {
        return memory_object_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::locale_desc_count()
    {
        return locale_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::strings_desc_count()
    {
        return strings_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::stream_port_input_desc_count()
    {
        return stream_port_input_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::stream_port_output_desc_count()
    {
        return stream_port_output_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::audio_cluster_desc_count()
    {
        return audio_cluster_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::audio_map_desc_count()
    {
        return audio_map_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::clock_domain_desc_count()
    {
        return clock_domain_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::control_desc_count()
    {
        return control_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::external_port_input_desc_count()
    {
        return external_port_input_desc_vec.size();
    }

    size_t STDCALL configuration_descriptor_imp::external_port_output_desc_count()
    {
        return external_port_output_desc_vec.size();
    }

    audio_unit_descriptor * STDCALL configuration_descriptor_imp::get_audio_unit_desc_by_index(size_t audio_unit_desc_index)
    {
        bool is_valid = (audio_unit_desc_index < audio_unit_desc_vec.size());

        if(is_valid)
        {
            return audio_unit_desc_vec.at(audio_unit_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_audio_unit_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    stream_input_descriptor * STDCALL configuration_descriptor_imp::get_stream_input_desc_by_index(size_t stream_input_desc_index)
    {
        bool is_valid = (stream_input_desc_index < stream_input_desc_vec.size());

        if(is_valid)
        {
            return stream_input_desc_vec.at(stream_input_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_stream_input_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    stream_output_descriptor * STDCALL configuration_descriptor_imp::get_stream_output_desc_by_index(size_t stream_output_desc_index)
    {
        bool is_valid = (stream_output_desc_index < stream_output_desc_vec.size());

        if(is_valid)
        {
            return stream_output_desc_vec.at(stream_output_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_stream_output_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    jack_input_descriptor * STDCALL configuration_descriptor_imp::get_jack_input_desc_by_index(size_t jack_input_desc_index)
    {
        bool is_valid = (jack_input_desc_index < jack_input_desc_vec.size());

        if(is_valid)
        {
            return jack_input_desc_vec.at(jack_input_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_jack_input_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    jack_output_descriptor * STDCALL configuration_descriptor_imp::get_jack_output_desc_by_index(size_t jack_output_desc_index)
    {
        bool is_valid = (jack_output_desc_index < jack_output_desc_vec.size());

        if(is_valid)
        {
            return jack_output_desc_vec.at(jack_output_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_jack_output_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    avb_interface_descriptor * STDCALL configuration_descriptor_imp::get_avb_interface_desc_by_index(size_t avb_interface_desc_index)
    {
        bool is_valid = (avb_interface_desc_index < avb_interface_desc_vec.size());

        if(is_valid)
        {
            return avb_interface_desc_vec.at(avb_interface_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_avb_interface_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    clock_source_descriptor * STDCALL configuration_descriptor_imp::get_clock_source_desc_by_index(size_t clock_source_desc_index)
    {
        bool is_valid = (clock_source_desc_index < clock_source_desc_vec.size());

        if(is_valid)
        {
            return clock_source_desc_vec.at(clock_source_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_clock_source_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    memory_object_descriptor * STDCALL configuration_descriptor_imp::get_memory_object_desc_by_index(size_t memory_object_desc_index)
    {
        bool is_valid = (memory_object_desc_index < memory_object_desc_vec.size());

        if(is_valid)
        {
            return memory_object_desc_vec.at(memory_object_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_memory_object_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    locale_descriptor * STDCALL configuration_descriptor_imp::get_locale_desc_by_index(size_t locale_desc_index)
    {
        bool is_valid = (locale_desc_index < locale_desc_vec.size());

        if(is_valid)
        {
            return locale_desc_vec.at(locale_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_locale_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    strings_descriptor * STDCALL configuration_descriptor_imp::get_strings_desc_by_index(size_t strings_desc_index)
    {
        bool is_valid = (strings_desc_index < strings_desc_vec.size());

        if(is_valid)
        {
            return strings_desc_vec.at(strings_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_strings_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    uint8_t * STDCALL configuration_descriptor_imp::get_strings_desc_string_by_reference(size_t reference)
    {
        if (reference == 0xffff)
        {
            return NULL;
        }
        strings_descriptor * desc = get_strings_desc_by_index(reference >> 3);

        if(desc)
        {
            return desc->get_string_by_index(reference & 0x3);
        }

        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR,
                                  "0x%llx, get_strings_desc_string_by_reference error, ref 0x%04x",
                                  base_end_station_imp_ref->entity_id(),
                                  (unsigned int)reference & 0xffff);
        return NULL;
    }

    stream_port_input_descriptor * STDCALL configuration_descriptor_imp::get_stream_port_input_desc_by_index(size_t stream_port_input_desc_index)
    {
        bool is_valid = (stream_port_input_desc_index < stream_port_input_desc_vec.size());

        if(is_valid)
        {
            return stream_port_input_desc_vec.at(stream_port_input_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_stream_port_input_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    stream_port_output_descriptor * STDCALL configuration_descriptor_imp::get_stream_port_output_desc_by_index(size_t stream_port_output_desc_index)
    {
        bool is_valid = (stream_port_output_desc_index < stream_port_output_desc_vec.size());

        if(is_valid)
        {
            return stream_port_output_desc_vec.at(stream_port_output_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_stream_port_output_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    audio_cluster_descriptor * STDCALL configuration_descriptor_imp::get_audio_cluster_desc_by_index(size_t audio_cluster_desc_index)
    {
        bool is_valid = (audio_cluster_desc_index < audio_cluster_desc_vec.size());

        if(is_valid)
        {
            return audio_cluster_desc_vec.at(audio_cluster_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_audio_cluster_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    audio_map_descriptor * STDCALL configuration_descriptor_imp::get_audio_map_desc_by_index(size_t audio_map_desc_index)
    {
        bool is_valid = (audio_map_desc_index < audio_map_desc_vec.size());

        if(is_valid)
        {
            return audio_map_desc_vec.at(audio_map_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_audio_map_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    clock_domain_descriptor * STDCALL configuration_descriptor_imp::get_clock_domain_desc_by_index(size_t clock_domain_desc_index)
    {
        bool is_valid = (clock_domain_desc_index < clock_domain_desc_vec.size());

        if(is_valid)
        {
            return clock_domain_desc_vec.at(clock_domain_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_clock_domain_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    control_descriptor * STDCALL configuration_descriptor_imp::get_control_desc_by_index(size_t control_desc_index)
    {
        bool is_valid = (control_desc_index < control_desc_vec.size());

        if(is_valid)
        {
            return control_desc_vec.at(control_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_control_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    external_port_input_descriptor * STDCALL configuration_descriptor_imp::get_external_port_input_desc_by_index(size_t index)
    {
        bool is_valid = (index < external_port_input_desc_vec.size());

        if (is_valid)
        {
            return external_port_input_desc_vec.at(index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_control_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }

    external_port_output_descriptor * STDCALL configuration_descriptor_imp::get_external_port_output_desc_by_index(size_t index)
    {
        bool is_valid = (index < external_port_output_desc_vec.size());

        if (is_valid)
        {
            return external_port_output_desc_vec.at(index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, get_control_desc_by_index error", base_end_station_imp_ref->entity_id());
        }

        return NULL;
    }
}
