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
 * Configuration descriptor implementation
 */

#include <vector>
#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "configuration_descriptor_imp.h"

namespace avdecc_lib
{
	configuration_descriptor_imp::configuration_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
	{
		desc_config_read_returned = jdksavdecc_descriptor_configuration_read(&config_desc, frame, pos, frame_len);

		if(desc_config_read_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "desc_config_read error");
			assert(desc_config_read_returned >= 0);
		}

		desc_type_vector_init(frame, pos);
		desc_count_vector_init(frame, pos);
	}

	configuration_descriptor_imp::~configuration_descriptor_imp()
	{
		for(uint32_t audio_unit_vec_index = 0; audio_unit_vec_index < audio_unit_desc_vec.size(); audio_unit_vec_index++)
		{
			delete audio_unit_desc_vec.at(audio_unit_vec_index);
		}

		for(uint32_t stream_input_vec_index = 0; stream_input_vec_index < stream_input_desc_vec.size(); stream_input_vec_index++)
		{
			delete stream_input_desc_vec.at(stream_input_vec_index);
		}

		for(uint32_t stream_output_vec_index = 0; stream_output_vec_index < stream_output_desc_vec.size(); stream_output_vec_index++)
		{
			delete stream_output_desc_vec.at(stream_output_vec_index);
		}

		for(uint32_t jack_input_vec_index = 0; jack_input_vec_index < jack_input_desc_vec.size(); jack_input_vec_index++)
		{
			delete jack_input_desc_vec.at(jack_input_vec_index);
		}

		for(uint32_t jack_output_vec_index = 0; jack_output_vec_index < jack_output_desc_vec.size(); jack_output_vec_index++)
		{
			delete jack_output_desc_vec.at(jack_output_vec_index);
		}

		for(uint32_t avb_if_vec_index = 0; avb_if_vec_index < avb_interface_desc_vec.size(); avb_if_vec_index++)
		{
			delete avb_interface_desc_vec.at(avb_if_vec_index);
		}

		for(uint32_t clk_src_vec_index = 0; clk_src_vec_index < clock_source_desc_vec.size(); clk_src_vec_index++)
		{
			delete clock_source_desc_vec.at(clk_src_vec_index);
		}

		for(uint32_t locale_vec_index = 0; locale_vec_index < locale_desc_vec.size(); locale_vec_index++)
		{
			delete locale_desc_vec.at(locale_vec_index);
		}

		for(uint32_t strings_vec_index = 0; strings_vec_index < strings_desc_vec.size(); strings_vec_index++)
		{
			delete strings_desc_vec.at(strings_vec_index);
		}

		for(uint32_t clk_domain_vec_index = 0; clk_domain_vec_index < clock_domain_desc_vec.size(); clk_domain_vec_index++)
		{
			delete clock_domain_desc_vec.at(clk_domain_vec_index);
		}
	}

	uint16_t STDCALL configuration_descriptor_imp::get_descriptor_type()
	{
		assert(config_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_CONFIGURATION);
		return config_desc.descriptor_type;
	}

	uint16_t STDCALL configuration_descriptor_imp::get_descriptor_index()
	{
		assert(config_desc.descriptor_index == 0);
		return config_desc.descriptor_index;
	}

	uint8_t * STDCALL configuration_descriptor_imp::get_object_name()
	{
		return config_desc.object_name.value;
	}

	uint16_t STDCALL configuration_descriptor_imp::get_localized_description()
	{
		return config_desc.localized_description;
	}

	uint16_t STDCALL configuration_descriptor_imp::get_descriptor_counts_count()
	{
		assert(config_desc.descriptor_counts_count <= 108);
		return config_desc.descriptor_counts_count;
	}

	uint16_t STDCALL configuration_descriptor_imp::get_descriptor_counts_offset()
	{
		assert(config_desc.descriptor_counts_offset == 74);
		return config_desc.descriptor_counts_offset;
	}

	void configuration_descriptor_imp::desc_type_vector_init(const uint8_t *frame, size_t pos)
	{
		uint16_t offset = 0x0;

		for(uint32_t index_i = 0; index_i < get_descriptor_counts_count(); index_i++)
		{
			desc_type_vector.push_back(jdksavdecc_uint16_get(frame, get_descriptor_counts_offset() + pos + offset));
			offset += 0x4;
		}
	}

	void configuration_descriptor_imp::desc_count_vector_init(const uint8_t *frame, size_t pos)
	{
		uint16_t offset = 0x2;

		for(uint32_t index_i = 0; index_i < get_descriptor_counts_count(); index_i++)
		{
			desc_count_vector.push_back(jdksavdecc_uint16_get(frame, get_descriptor_counts_offset() + pos + offset));
			offset += 0x4;
		}
	}

	bool STDCALL configuration_descriptor_imp::are_desc_type_and_index_in_config(int desc_type, int desc_count_index)
	{
		int desc_index;

		for(uint32_t index_i = 0; index_i < desc_type_vector.size(); index_i++)
		{
			if(desc_type_vector.at(index_i) == desc_type)
			{
				desc_index = index_i;

				if(desc_count_index < desc_count_vector.at(desc_index))
				{
					return true;
				}
			}
		}

		return false;
	}

	uint16_t STDCALL configuration_descriptor_imp::get_desc_type_from_config_by_index(int desc_index)
	{
		return desc_type_vector.at(desc_index);
	}

	uint16_t STDCALL configuration_descriptor_imp::get_desc_count_from_config_by_index(int desc_index)
	{
		return desc_count_vector.at(desc_index);
	}

	void configuration_descriptor_imp::store_audio_unit_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len)
	{
		audio_unit_desc_vec.push_back(new audio_unit_descriptor_imp(end_station_obj, frame, pos, frame_len));
	}

	void configuration_descriptor_imp::store_stream_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len)
	{
		stream_input_desc_vec.push_back(new stream_input_descriptor_imp(end_station_obj, frame, pos, frame_len));
	}

	void configuration_descriptor_imp::store_stream_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len)
	{
		stream_output_desc_vec.push_back(new stream_output_descriptor_imp(end_station_obj, frame, pos, frame_len));
	}

	void configuration_descriptor_imp::store_jack_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len)
	{
		jack_input_desc_vec.push_back(new jack_input_descriptor_imp(end_station_obj, frame, pos, frame_len));
	}

	void configuration_descriptor_imp::store_jack_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len)
	{
		jack_output_desc_vec.push_back(new jack_output_descriptor_imp(end_station_obj, frame, pos, frame_len));
	}

	void configuration_descriptor_imp::store_avb_interface_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len)
	{
		avb_interface_desc_vec.push_back(new avb_interface_descriptor_imp(end_station_obj, frame, pos, frame_len));
	}

	void configuration_descriptor_imp::store_clock_source_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len)
	{
		clock_source_desc_vec.push_back(new clock_source_descriptor_imp(end_station_obj, frame, pos, frame_len));
	}

	void configuration_descriptor_imp::store_locale_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len)
	{
		locale_desc_vec.push_back(new locale_descriptor_imp(end_station_obj, frame, pos, frame_len));
	}

	void configuration_descriptor_imp::store_strings_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len)
	{
		strings_desc_vec.push_back(new strings_descriptor_imp(end_station_obj, frame, pos, frame_len));
	}

	void configuration_descriptor_imp::store_audio_cluster_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len)
	{
		audio_cluster_desc_vec.push_back(new audio_cluster_descriptor_imp(end_station_obj, frame, pos, frame_len));
	}

	void configuration_descriptor_imp::store_audio_map_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len)
	{
		audio_map_desc_vec.push_back(new audio_map_descriptor_imp(end_station_obj, frame, pos, frame_len));
	}

	void configuration_descriptor_imp::store_clock_domain_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len)
	{
		clock_domain_desc_vec.push_back(new clock_domain_descriptor_imp(end_station_obj, frame, pos, frame_len));
	}

	uint32_t STDCALL configuration_descriptor_imp::get_audio_unit_desc_count()
	{
		return audio_unit_desc_vec.size();
	}

	uint32_t STDCALL configuration_descriptor_imp::get_stream_input_desc_count()
	{
		return stream_input_desc_vec.size();
	}
	uint32_t STDCALL configuration_descriptor_imp::get_stream_output_desc_count()
	{
		return stream_output_desc_vec.size();
	}

	uint32_t STDCALL configuration_descriptor_imp::get_jack_input_desc_count()
	{
		return jack_input_desc_vec.size();
	}

	uint32_t STDCALL configuration_descriptor_imp::get_jack_output_desc_count()
	{
		return jack_output_desc_vec.size();
	}

	uint32_t STDCALL configuration_descriptor_imp::get_avb_interface_desc_count()
	{
		return avb_interface_desc_vec.size();
	}

	uint32_t STDCALL configuration_descriptor_imp::get_clock_source_desc_count()
	{
		return clock_source_desc_vec.size();
	}

	uint32_t STDCALL configuration_descriptor_imp::get_locale_desc_count()
	{
		return locale_desc_vec.size();
	}

	uint32_t STDCALL configuration_descriptor_imp::get_strings_desc_count()
	{
		return strings_desc_vec.size();
	}

	uint32_t STDCALL configuration_descriptor_imp::get_audio_cluster_desc_count()
	{
		return audio_cluster_desc_vec.size();
	}

	uint32_t STDCALL configuration_descriptor_imp::get_audio_map_desc_count()
	{
		return audio_map_desc_vec.size();
	}

	uint32_t STDCALL configuration_descriptor_imp::get_clock_domain_desc_count()
	{
		return clock_domain_desc_vec.size();
	}

	audio_unit_descriptor * STDCALL configuration_descriptor_imp::get_audio_unit_desc_by_index(uint32_t audio_unit_desc_index)
	{
		bool is_valid = (audio_unit_desc_index < audio_unit_desc_vec.size());

		if(is_valid)
		{
			return audio_unit_desc_vec.at(audio_unit_desc_index);
		}
		else
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_audio_unit_desc_by_index error");
		}

		return NULL;
	}

	stream_input_descriptor * STDCALL configuration_descriptor_imp::get_stream_input_desc_by_index(uint32_t stream_input_desc_index)
	{
		bool is_valid = (stream_input_desc_index < stream_input_desc_vec.size());

		if(is_valid)
		{
			return stream_input_desc_vec.at(stream_input_desc_index);
		}
		else
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_stream_input_desc_by_index error");
		}

		return NULL;
	}

	stream_output_descriptor * STDCALL configuration_descriptor_imp::get_stream_output_desc_by_index(uint32_t stream_output_desc_index)
	{
		bool is_valid = (stream_output_desc_index < stream_output_desc_vec.size());

		if(is_valid)
		{
			return stream_output_desc_vec.at(stream_output_desc_index);
		}
		else
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_stream_output_desc_by_index error");
		}

		return NULL;
	}

	jack_input_descriptor * STDCALL configuration_descriptor_imp::get_jack_input_desc_by_index(uint32_t jack_input_desc_index)
	{
		bool is_valid = (jack_input_desc_index < jack_input_desc_vec.size());

		if(is_valid)
		{
			return jack_input_desc_vec.at(jack_input_desc_index);
		}
		else
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_jack_input_desc_by_index error");
		}

		return NULL;
	}

	jack_output_descriptor * STDCALL configuration_descriptor_imp::get_jack_output_desc_by_index(uint32_t jack_output_desc_index)
	{
		bool is_valid = (jack_output_desc_index < jack_output_desc_vec.size());

		if(is_valid)
		{
			return jack_output_desc_vec.at(jack_output_desc_index);
		}
		else
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_jack_output_desc_by_index error");
		}

		return NULL;
	}

	avb_interface_descriptor * STDCALL configuration_descriptor_imp::get_avb_interface_desc_by_index(uint32_t avb_interface_desc_index)
	{
		bool is_valid = (avb_interface_desc_index < avb_interface_desc_vec.size());

		if(is_valid)
		{
			return avb_interface_desc_vec.at(avb_interface_desc_index);
		}
		else
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_avb_interface_desc_by_index error");
		}

		return NULL;
	}

	clock_source_descriptor * STDCALL configuration_descriptor_imp::get_clock_source_desc_by_index(uint32_t clock_source_desc_index)
	{
		bool is_valid = (clock_source_desc_index < clock_source_desc_vec.size());

		if(is_valid)
		{
			return clock_source_desc_vec.at(clock_source_desc_index);
		}
		else
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_clock_source_desc_by_index error");
		}

		return NULL;
	}

	locale_descriptor * STDCALL configuration_descriptor_imp::get_locale_desc_by_index(uint32_t locale_desc_index)
	{
		bool is_valid = (locale_desc_index < locale_desc_vec.size());

		if(is_valid)
		{
			return locale_desc_vec.at(locale_desc_index);
		}
		else
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_locale_desc_by_index error");
		}

		return NULL;
	}

	strings_descriptor * STDCALL configuration_descriptor_imp::get_strings_desc_by_index(uint32_t strings_desc_index)
	{
		bool is_valid = (strings_desc_index < strings_desc_vec.size());

		if(is_valid)
		{
			return strings_desc_vec.at(strings_desc_index);
		}
		else
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_strings_desc_by_index error");
		}

		return NULL;
	}

	audio_cluster_descriptor * STDCALL configuration_descriptor_imp::get_audio_cluster_desc_by_index(uint32_t audio_cluster_desc_index)
	{
		bool is_valid = (audio_cluster_desc_index < audio_cluster_desc_vec.size());

		if(is_valid)
		{
			return audio_cluster_desc_vec.at(audio_cluster_desc_index);
		}
		else
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_audio_cluster_desc_by_index error");
		}

		return NULL;
	}

	audio_map_descriptor * STDCALL configuration_descriptor_imp::get_audio_map_desc_by_index(uint32_t audio_map_desc_index)
	{
		bool is_valid = (audio_map_desc_index < audio_map_desc_vec.size());

		if(is_valid)
		{
			return audio_map_desc_vec.at(audio_map_desc_index);
		}
		else
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_audio_map_desc_by_index error");
		}

		return NULL;
	}

	clock_domain_descriptor * STDCALL configuration_descriptor_imp::get_clock_domain_desc_by_index(uint32_t clock_domain_desc_index)
	{
		bool is_valid = (clock_domain_desc_index < clock_domain_desc_vec.size());

		if(is_valid)
		{
			return clock_domain_desc_vec.at(clock_domain_desc_index);
		}
		else
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_clock_domain_desc_by_index error");
		}

		return NULL;
	}
}
