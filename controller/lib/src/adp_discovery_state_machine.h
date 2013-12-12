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
 * adp_discovery_state_machine.h
 *
 * ADP Discovery State Machine class
 */

#pragma once

#include "timer.h"

namespace avdecc_lib
{
    class adp_discovery_state_machine
    {
    private:
        struct entities
        {
            uint64_t entity_id;
            timer inflight_timer;
        };

        std::vector<struct entities> entities_vec;

    public:
        adp_discovery_state_machine();

        ~adp_discovery_state_machine();

        /**
         * Initialize and fill Ethernet frame payload with Ethernet frame information for ADP messages.
         */
        int ether_frame_init(struct jdksavdecc_frame *cmd_frame);

        /**
         * Initialize and fill Ethernet frame payload with 1722 ADP Header information.
         */
        void common_hdr_init(struct jdksavdecc_frame *cmd_frame, uint64_t target_guid);   

        /**
         * Process the Discover state of the ADP Discovery State Machine.
         */
        int state_discover(uint64_t discover_id);

        /**
         * Process the Available state of the ADP Discovery State Machine.
         */
        int state_avail(const uint8_t *frame, size_t frame_len);

        /**
         * Process the Departing state of the ADP Discovery State Machine.
         */
        int state_departing();

        /**
         * Check timeout for the end stations.
         */
        bool tick(uint64_t &end_station_guid);

    private:
        /**
         * The perform discover event is used to trigger an AVDECC Entity discovery search to search
         * for all AVDECC Entities or to the Entity ID of an AVDECC Entity to search for.
         */
        int perform_discover(uint64_t entity_id);

        /**
         * Transmit an ENTITY_DISCOVER message.
         */
        int tx_discover(struct jdksavdecc_frame *cmd_frame);

        /**
         * Check if an AVDECC Entity is present in the entities variable.
         */
        bool have_entity(uint64_t entity_id, uint32_t *entity_index);

        /**
         * Update the AVDECC Entity record timeout information.
         */
        int update_entity_timeout(uint32_t entity_index, uint32_t timeout_ms);

        /**
         * Add a new Entity record to the entities variable.
         */
        int add_entity(struct entities new_entity);

        /**
         * Remove an Entity record form the entities variable.
         */
        int remove_entity(uint32_t entity_index);

        /**
         * Process the Timeout state of the ADP Discovery State Machine.
         */
        int state_timeout(uint32_t entity_index);
    };

    extern adp_discovery_state_machine *adp_discovery_state_machine_ref;
}

