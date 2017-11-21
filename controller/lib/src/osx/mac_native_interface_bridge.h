/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2017 AudioScience Inc.
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
 * mac_native_interface_bridge.h
 *
 * Mac Native Network Interface Bridge
 *
 * A bridge between the packet frames expected in
 * system_layer2_multithreaded_callback and the macOS
 * AudioVideoBridging APIs.
 *
 * It is necessary to use the macOS AVB APIs in order
 * to communicate with the Mac Virtual AVB Entity.
 *
 * Frames passed to net_interface_imp::send_frame()
 * are translated to their corresponding AECP and ACMP Mac Native layer messages.
 * Commands and Responses received from the Mac Native layer are converted
 * back to frames and queued in system_layer2_multithreaded_callback via
 * avdecc_lib::system_queue_rx().
 *
 * ADP is handled specially, since the Mac Native layer
 * provides its own Discovery State Machine.
 * See mac_native_interface_bridge::is_end_station_connected().
 */

namespace avdecc_lib
{
class mac_native_interface_bridge
{
public:
    ///
    /// Check whether Mac Native mode is supported and enabled on a given network interface.
    ///
    /// \param netif_name The interface BSD name.
    /// \return True if Mac Native is enabled and supported.
    ///
    static bool is_avb_enabled(const char * netif_name);

    ///
    /// Select a network interface for Mac Native 1722.1 capture.
    ///
    /// \param netif_name The interface BSD name.
    /// \param netif_eui The Extended Unique Identifier (EUI) of the selected network interface.
    /// \param netif_mac The MAC address of the selected network interface.
    ///
    static void select(const char * netif_name, uint64_t netif_eui, uint64_t netif_mac);

    ///
    /// Start capturing 1722.1 traffic on the selected network interface.
    ///
    static void open();

    ///
    /// Stop capturing 1722.1 traffic, remove Handlers, and clean up objects.
    ///
    static void close();
    
    ///
    /// Send a network packet to Mac Native layer.
    ///
    static int send_frame(uint8_t * tx_frame_base, uint16_t tx_frame_len);

    ///
    /// Since the Mac Native Interface provides its own
    /// End Station Discovery state machine, this function is used
    /// to check the connection status of a device.
    ///
    /// \param entity_id the entity ID of the End Station.
    ///
    static bool is_end_station_connected(uint64_t entity_id);
};
}
