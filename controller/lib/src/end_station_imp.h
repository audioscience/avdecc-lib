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
 * end_station_imp.h
 *
 * End Station implementation class
 */

#pragma once
#include <list>

#include "entity_descriptor_imp.h"
#include "end_station.h"
#include "timer.h"

namespace avdecc_lib
{
    class adp;

	class background_read_request
	{
	public:
		background_read_request(uint16_t t, uint16_t I) :
            m_type(t), m_index(I) {};
		uint16_t m_type;
		uint16_t m_index;
        timer m_timer;
	};

    class end_station_imp : public virtual end_station
    {
    private:
        uint64_t end_station_entity_id; // The unique identifier of the AVDECC Entity the command is targeted to
        uint64_t end_station_mac; // The source MAC address of the End Station
        char end_station_connection_status; // The connection status of an End Station
        uint16_t current_entity_desc; // The ENTITY descriptor associated with the End Station
        uint16_t current_config_desc; // The CONFIGURATION descriptor associated with the ENTITY descriptor in the same End Station

        uint16_t selected_entity_index; // The controller-selected entity index
        uint16_t selected_config_index; // The controller-selected configuraition descriptor index

		std::list<background_read_request *> m_backbround_read_pending; // Store a list of background reads
        std::list<background_read_request *> m_backbround_read_inflight; // Store a list of background reads that are inflight

        adp *adp_ref; // ADP associated with the End Station
        std::vector<entity_descriptor_imp *> entity_desc_vec; // Store a list of ENTITY descriptor objects

        void queue_background_read_request(uint16_t desc_type, uint16_t desc_base_index, uint16_t count);  ///< Generate "count" read requests
        void background_read_deduce_next(configuration_descriptor *cd, uint16_t desc_type, void *frame, ssize_t pos); ///< Deduce what else needs to be read from the rx'd frame
        void background_read_update_inflight(uint16_t desc_type, void *frame, ssize_t read_desc_offset); ///< Remove rx'd frame from background read inflight list

        bool desc_index_from_frame(uint16_t desc_type, void *frame, ssize_t read_desc_offset, uint16_t &desc_index);

    public:
        end_station_imp(const uint8_t *frame, size_t frame_len);
        virtual ~end_station_imp();

        const char STDCALL get_connection_status() const;

        /**
         * Change the End Station connection status to connected.
         */
        void set_connected();

        /**
         * Change the End Station connection status to disconnected.
         */
        void set_disconnected();

        /**
         * Re-enumerate the endpoint by re-reading the descriptors
         */
        void end_station_reenumerate();

        uint64_t STDCALL entity_id();
        uint64_t STDCALL mac();
        adp * get_adp();
        size_t STDCALL entity_desc_count();
        entity_descriptor * STDCALL get_entity_desc_by_index(size_t entity_desc_index);
        int STDCALL send_read_desc_cmd(void *notification_id, uint16_t desc_type, uint16_t desc_index);
        int proc_read_desc_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int STDCALL send_entity_avail_cmd(void *notification_id);
        int proc_entity_avail_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);
        int proc_rcvd_aem_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status, uint16_t &operation_id, bool &is_operation_id_valid);
        int STDCALL send_aecp_address_access_cmd(void *notification_id,
                                        unsigned mode,
                                        unsigned length,
                                        uint64_t address,
                                        uint8_t memory_data[]);
        int STDCALL send_identify(void *notification_id, bool turn_on);
        int proc_set_control_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        void background_read_update_timeouts(void); ///< update timeout conditions
        void background_read_submit_pending(void); ///< Submit pending background reads

        /**
         * Process response received for the corresponding AECP Address Access command.
         */
        int proc_rcvd_aecp_aa_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int proc_rcvd_acmp_resp(uint32_t msg_type, void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        void STDCALL set_current_entity_index(uint16_t entity_index);
        uint16_t STDCALL get_current_entity_index() const;
        void STDCALL set_current_config_index(uint16_t entity_index);
        uint16_t STDCALL get_current_config_index() const;

    private:
        /**
         * Initialize End Station with Entity and Configuration descriptors information.
         */
        int end_station_init();

        /**
         * Initialize End Station by sending non blocking Read Descriptor commands to read
         * all the descriptors for the End Station.
         */
        int read_desc_init(uint16_t desc_type, uint16_t desc_index);

        /**
         * Send a READ_DESCRIPTOR command with or without a notification id based on the post_notification_msg flag
         * to read a descriptor from an AVDECC Entity.
         */
        int send_read_desc_cmd_with_flag(void *notification_id, uint32_t notification_flag, uint16_t desc_type, uint16_t desc_index);
    };
}


