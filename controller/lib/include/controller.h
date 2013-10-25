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
 * controller.h
 *
 * Public Controller interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_CONTROLLER_H_
#define _AVDECC_CONTROLLER_LIB_CONTROLLER_H_

#include <stdint.h>
#include "build.h"

namespace avdecc_lib
{
        class end_station;
        class configuration_descriptor;

        class controller
        {
        public:
                /**
                 * Deallocate memory
                 */
                AVDECC_CONTROLLER_LIB32_API virtual void STDCALL destroy() = 0;

                /**
                 * Get the current build release version.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual const char * STDCALL get_version() = 0;

                /**
                 * Get the AVDECC Controller GUID of the AVDECC Entity sending the command.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_controller_guid() = 0;

                /**
                 * Get the number of End Stations connected.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_end_station_count() = 0;

                /**
                 * Get the corresponding End Station by index.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual end_station * STDCALL get_end_station_by_index(uint32_t end_station_index) = 0;

                /**
                 * Get the corresponding Configuration descriptor by index.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual configuration_descriptor * STDCALL get_config_by_index(uint32_t end_station_index, uint16_t entity_index, uint16_t config_index) = 0;

                /**
                 * Get the corresponding Configuration descriptor by GUID.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual configuration_descriptor * STDCALL get_config_by_guid(uint64_t end_station_guid, uint16_t entity_index, uint16_t config_index) = 0;

                /**
                 * Check if the command with the corresponding notification id is in the inflight list.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL is_inflight_cmd_with_notification_id(void *notification_id) = 0;

                /**
                 * Check for End Station connection, command packet, and response packet timeouts.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual void STDCALL time_tick_event() = 0;

                /**
                 * Lookup and process packet received.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual void STDCALL rx_packet_event(void *notification_id, bool &notification_id_flag, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len) = 0;

                /**
                 * Send queued packet to the AEM Controller State Machine.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual void STDCALL tx_packet_event(void *notification_id, uint32_t notification_flag, uint8_t *frame, uint16_t mem_buf_len) = 0;
        };

        /**
         * Create a public AVDECC Controller object with notification and logging callback functions used for accessing from outside the library.
         */
        extern "C" AVDECC_CONTROLLER_LIB32_API controller * STDCALL create_controller(net_interface *netif,
                                                                                      void (*notification_callback) (void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, void *),
                                                                                      void (*log_callback) (void *, int32_t, const char *, int32_t));
}

#endif