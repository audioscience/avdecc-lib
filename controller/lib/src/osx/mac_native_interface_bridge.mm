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
 * mac_native_interface_bridge.mm
 *
 * Mac Native Interface Bridge Implementation
 */

#include <map>
#include "jdksavdecc_adp.h"
#include "jdksavdecc_aecp.h"
#include "jdksavdecc_acmp.h"
#include "jdksavdecc_aem_descriptor.h"
#include "util.h"
#include "log_imp.h"
#include "enumeration.h"
#include "system_rx_queue.h"
#include "mac_native_interface_bridge.h"
#import <AudioVideoBridging/AudioVideoBridging.h>

#define AECPDU_COMMAND_SPECIFIC_DATA_OFFSET avdecc_lib::ETHER_HDR_SIZE + \
                                            JDKSAVDECC_AECPDU_COMMON_LEN + \
                                            sizeof(jdksavdecc_aecpdu_aem::command_type)

@interface MacAVBInterface : NSObject <AVB17221EntityDiscoveryDelegate, AVB17221AECPClient, AVB17221ACMPClient>

/* constructor */
-(id)init:(NSString *)interfaceName;
/* destructor */
-(void)deinit;

/* network packet to Mac Native msg */
-(int)send_Mac_msg:(uint8_t *)tx_frame_base len:(uint16_t)tx_frame_len;

/* network packet to Mac Native ACMP msg */
-(int)send_Mac_ACMP_msg:(uint8_t *)tx_frame_base len:(uint16_t)tx_frame_len;
/* Mac Native ACMP msg to network packet */
-(int)process_Mac_ACMP_msg:(AVB17221ACMPMessage *)rx_msg base:(uint8_t *)rx_frame_base;

/* network packet to Mac Native AECP msg */
-(int)send_Mac_AECP_msg:(uint8_t *)tx_frame_base len:(uint16_t)tx_frame_len;
/* Mac Native AECP msg to network packet */
-(int)process_Mac_AECP_msg:(AVB17221AECPAEMMessage *)msg base:(uint8_t *)frame_base;
-(int)Mac_AECP_msg_to_frame:(AVB17221AECPAEMMessage *)msg;

/* Mac Native ADP callback to network packet */
-(int)Mac_entity_to_ADP_frame:(AVB17221Entity *)Mac_entity;

@property (retain) AVBInterface * interface;

@end

// Bridge Interface object
MacAVBInterface * bridge = nil;

// Name, MAC address and Controller GUID of the selected Mac Native network interface
std::string netif_name;
uint64_t netif_mac_address;
uint64_t controller_entity_id;

/*
 * The Mac AVB17221EntityDiscoveryDelegate protocol provides
 * a 1722.1 End Station discovery state machine.
 *
 * This map stores discovered End Stations by
 * GUID and their connection status (True/False).
 */
std::map<uint64_t, bool> Mac_discovered_end_stations;

namespace avdecc_lib
{
bool mac_native_interface_bridge::is_avb_enabled(const char * netif_name)
{
    auto ns_netif_name = [NSString stringWithUTF8String:netif_name];

    // Check if the interface is AVB Capable
    if(![AVBInterface isAVBCapableInterfaceNamed:ns_netif_name])
        return false;

    // Check if Mac Native AVB is enabled on the Interface
    if(![AVBInterface isAVBEnabledOnInterfaceNamed:ns_netif_name])
        return false;

    return true;
}

void mac_native_interface_bridge::select(const char * name, uint64_t eui, uint64_t mac)
{
    netif_name = std::string(name);
    controller_entity_id = eui;
    netif_mac_address = mac;
}
   
void mac_native_interface_bridge::open()
{
    bridge = [[MacAVBInterface alloc]
              init:[NSString stringWithUTF8String:netif_name.c_str()]];
}

void mac_native_interface_bridge::close()
{
    if (!bridge)
        return;

    [bridge deinit];
    bridge = NULL;
}
    
int mac_native_interface_bridge::send_frame(uint8_t * tx_frame_base, uint16_t tx_frame_len)
{
    if (!bridge)
        return -1;

    return [(id) bridge send_Mac_msg:tx_frame_base len:tx_frame_len];
}
    
bool mac_native_interface_bridge::is_end_station_connected(uint64_t entity_id)
{
    if (!bridge)
        return false;

    auto entity_iter = Mac_discovered_end_stations.find(entity_id);
    if (entity_iter != Mac_discovered_end_stations.end() && entity_iter->second) // found && connected
        return true;
    
    return false;
}
}

@implementation MacAVBInterface

-(id)init:(NSString *)interfaceName
{
    self = [super init];
    if(self)
    {
        // set up Discovery
        self.interface = [[AVBEthernetInterface alloc]
                          initWithInterfaceName:interfaceName];
        self.interface.entityDiscovery.discoveryDelegate = self;
        [self.interface.entityDiscovery primeIterators];

        // set up the Handlers to receive commands and responses to our controller GUID
        if (![self.interface.aecp setCommandHandler:self forEntityID:controller_entity_id])
            return nil;
        if (![self.interface.aecp setResponseHandler:self forControllerEntityID:controller_entity_id])
            return nil;
    }

    return self;
}

-(void)deinit
{
    self.interface.entityDiscovery.discoveryDelegate = nil;
    [self.interface.aecp removeCommandHandlerForEntityID:controller_entity_id];
    [self.interface.aecp removeResponseHandlerForControllerEntityID:controller_entity_id];
    [self.interface release];
    [self release];
}

-(int)send_Mac_msg:(uint8_t *)tx_frame_base len:(uint16_t)tx_frame_len
{
    uint8_t subtype = jdksavdecc_common_control_header_get_subtype(tx_frame_base, avdecc_lib::ETHER_HDR_SIZE);
    if (subtype == JDKSAVDECC_SUBTYPE_AECP)
    {
        return [(id) self send_Mac_AECP_msg:tx_frame_base len:tx_frame_len];
    }
    else if (subtype == JDKSAVDECC_SUBTYPE_ACMP)
    {
        return [(id) self send_Mac_ACMP_msg:tx_frame_base len:tx_frame_len];
    }

    return 0;
}

-(int)send_Mac_ACMP_msg:(uint8_t *)tx_frame_base len:(uint16_t)tx_frame_len
{
    // create response frame
    jdksavdecc_frame rx_frame;
    jdksavdecc_frame_init(&rx_frame);
    memcpy(rx_frame.payload, tx_frame_base, tx_frame_len);

    // read ACMP
    struct jdksavdecc_acmpdu acmpdu;
    memset(&acmpdu, 0, sizeof(acmpdu));
    jdksavdecc_acmpdu_read(&acmpdu,
                           tx_frame_base,
                           avdecc_lib::ETHER_HDR_SIZE,
                           tx_frame_len);
    
    // destination MAC
    auto * destination_MAC = [[AVBMACAddress alloc]
                              initWithBytes:(const unsigned char *)&acmpdu.stream_dest_mac.value];

    // create Mac ACMP Message
    auto acmp_msg = [[AVB17221ACMPMessage alloc] init];
    acmp_msg.messageType = static_cast<AVB17221ACMPMessageType>(acmpdu.header.message_type);
    acmp_msg.status = static_cast<AVB17221ACMPStatusCode>(acmpdu.header.status);
    acmp_msg.streamID = jdksavdecc_eui64_convert_to_uint64(&acmpdu.header.stream_id);
    acmp_msg.controllerEntityID = jdksavdecc_eui64_convert_to_uint64(&acmpdu.controller_entity_id);
    acmp_msg.talkerEntityID = jdksavdecc_eui64_convert_to_uint64(&acmpdu.talker_entity_id);
    acmp_msg.talkerUniqueID = acmpdu.talker_unique_id;
    acmp_msg.listenerEntityID = jdksavdecc_eui64_convert_to_uint64(&acmpdu.listener_entity_id);
    acmp_msg.listenerUniqueID = acmpdu.listener_unique_id;
    acmp_msg.destinationMAC = destination_MAC;
    acmp_msg.connectionCount = acmpdu.connection_count;
    acmp_msg.flags = static_cast<AVB17221ACMPFlags>(acmpdu.flags);
    acmp_msg.vlanID = acmpdu.stream_vlan_id;
    
    if(![self.interface.acmp sendACMPCommandMessage:acmp_msg completionHandler:^(NSError *error, AVB17221ACMPMessage *response){
        if(kIOReturnSuccess == (IOReturn)error.code)
        {
            // process the Mac ACMP response
            uint8_t * rx_frame_base = (uint8_t *)rx_frame.payload;
            [(id) self process_Mac_ACMP_msg:response base:rx_frame_base];
        }
        else
        {
            avdecc_lib::log_imp_ref->post_log_msg(
                                                  avdecc_lib::LOGGING_LEVEL_ERROR,
                                                  "(talker:0x%llx, listener:0x%llx, %s) Mac Native ACMP sendCommand returned error",
                                                  acmp_msg.talkerEntityID,
                                                  acmp_msg.listenerEntityID,
                                                  avdecc_lib::utility::acmp_cmd_value_to_name(acmp_msg.messageType));
        }
    }])
    {
        avdecc_lib::log_imp_ref->post_log_msg(
                                              avdecc_lib::LOGGING_LEVEL_ERROR,
                                              "(talker:0x%llx, listener:0x%llx, %s) Mac Native ACMP sendCommand error",
                                              acmp_msg.talkerEntityID,
                                              acmp_msg.listenerEntityID,
                                              avdecc_lib::utility::acmp_cmd_value_to_name(acmp_msg.messageType));
    }

    // cleanup
    [destination_MAC release];
    [acmp_msg release];
    
    return 0;
}

-(int)process_Mac_ACMP_msg:(AVB17221ACMPMessage *)rx_msg base:(uint8_t *)rx_frame_base
{
    // read ACMP
    struct jdksavdecc_acmpdu acmpdu;
    memset(&acmpdu, 0, sizeof(acmpdu));
    jdksavdecc_acmpdu_read(&acmpdu,
                           rx_frame_base,
                           avdecc_lib::ETHER_HDR_SIZE,
                           JDKSAVDECC_FRAME_MAX_PAYLOAD_SIZE);

    // write ACMP from Mac msg
    acmpdu.header.message_type = static_cast<uint32_t>(rx_msg.messageType);
    acmpdu.header.status = static_cast<uint32_t>(rx_msg.status);
    acmpdu.connection_count = rx_msg.connectionCount;
    acmpdu.flags = rx_msg.flags;
    acmpdu.stream_vlan_id = rx_msg.vlanID;
    jdksavdecc_eui64_init_from_uint64(&acmpdu.header.stream_id,
                                      rx_msg.streamID);
    jdksavdecc_eui64_init_from_uint64(&acmpdu.listener_entity_id,
                                      rx_msg.listenerEntityID);
    jdksavdecc_eui48_read(&acmpdu.stream_dest_mac,
                          [rx_msg.destinationMAC bytes],
                          0,
                          sizeof(acmpdu.stream_dest_mac));
    acmpdu.listener_unique_id = rx_msg.listenerUniqueID;
    jdksavdecc_eui64_init_from_uint64(&acmpdu.talker_entity_id,
                                      rx_msg.talkerEntityID);
    acmpdu.talker_unique_id = rx_msg.talkerUniqueID;

    // write ACMP
    jdksavdecc_acmpdu_write(&acmpdu,
                            rx_frame_base,
                            avdecc_lib::ETHER_HDR_SIZE,
                            JDKSAVDECC_FRAME_MAX_PAYLOAD_SIZE);

    // queue
    avdecc_lib::system_queue_rx(
                                (const uint8_t *)rx_frame_base,
                                JDKSAVDECC_FRAME_MAX_PAYLOAD_SIZE);
    return 0;
}

-(int)send_Mac_AECP_msg:(uint8_t *)tx_frame_base len:(uint16_t)tx_frame_len
{
    // read AECP
    struct jdksavdecc_aecpdu_aem aecpdu;
    memset(&aecpdu, 0, sizeof(aecpdu));
    ssize_t ret = jdksavdecc_aecpdu_aem_read(&aecpdu,
                                             tx_frame_base,
                                             avdecc_lib::ETHER_HDR_SIZE,
                                             tx_frame_len);
    if (ret < 0)
        return -1;
    
    // create Mac AECP
    auto aecp_msg = [AVB17221AECPAEMMessage commandMessage];
    aecp_msg.messageType = static_cast<AVB17221AECPMessageType>(aecpdu.aecpdu_header.header.message_type);
    aecp_msg.targetEntityID = jdksavdecc_eui64_convert_to_uint64(&aecpdu.aecpdu_header.header.target_entity_id);
    aecp_msg.status = static_cast<AVB17221AECPStatusCode>(aecpdu.aecpdu_header.header.status);
    aecp_msg.controllerEntityID = jdksavdecc_eui64_convert_to_uint64(&aecpdu.aecpdu_header.controller_entity_id);
    aecp_msg.commandType = static_cast<AVB17221AEMCommandType>(aecpdu.command_type);
    
    // command specific data
    assert(tx_frame_len >= AECPDU_COMMAND_SPECIFIC_DATA_OFFSET);
    uint8_t * aecpdu_cmd_specific_base = tx_frame_base + AECPDU_COMMAND_SPECIFIC_DATA_OFFSET;
    aecp_msg.commandSpecificData = [NSData
                                    dataWithBytes:aecpdu_cmd_specific_base
                                    length:(tx_frame_len - AECPDU_COMMAND_SPECIFIC_DATA_OFFSET)];
    
    // destination MAC
    auto * destination_MAC = [[AVBMACAddress alloc]
                              initWithBytes:tx_frame_base];

    if (aecp_msg.messageType == AVB17221AECPMessageTypeAEMResponse)
    {
        NSError * error;
        [self.interface.aecp sendResponse:(AVB17221AECPMessage *)aecp_msg toMACAddress:destination_MAC error:(NSError **)&error];
    }
    else if (aecp_msg.messageType == AVB17221AECPMessageTypeAEMCommand)
    {
        // create response frame
        jdksavdecc_frame rx_frame;
        jdksavdecc_frame_init(&rx_frame);
        memcpy(rx_frame.payload, tx_frame_base, tx_frame_len);

        if (![self.interface.aecp sendCommand:aecp_msg toMACAddress:destination_MAC completionHandler:^(NSError *error, AVB17221AECPMessage *response)
        {
            if(kIOReturnSuccess == (IOReturn)error.code)
            {
                // swap destination and source MAC address
                uint8_t * rx_frame_base = (uint8_t *)rx_frame.payload;
                jdksavdecc_eui48 dest_address = jdksavdecc_eui48_get(rx_frame_base, 0);
                jdksavdecc_eui48 src_address = jdksavdecc_eui48_get(rx_frame_base, 6);
                jdksavdecc_eui48 temp_address = src_address;
                src_address = dest_address;
                dest_address = temp_address;
                jdksavdecc_eui48_set(dest_address, rx_frame_base, 0);
                jdksavdecc_eui48_set(src_address, rx_frame_base, 6);
                
                // process the Mac AECP response
                [(id) self process_Mac_AECP_msg:(AVB17221AECPAEMMessage *)response base:rx_frame_base];
            }
            else
            {
                avdecc_lib::log_imp_ref->post_log_msg(
                                                      avdecc_lib::LOGGING_LEVEL_ERROR,
                                                      "(0x%llx, %s) Mac Native AECP sendCommand returned error",
                                                      aecp_msg.targetEntityID,
                                                      avdecc_lib::utility::aem_cmd_value_to_name(aecp_msg.commandType));
            }
            
        }])
        {
            avdecc_lib::log_imp_ref->post_log_msg(
                                                  avdecc_lib::LOGGING_LEVEL_ERROR,
                                                  "(0x%llx, %s) Mac Native AECP sendCommand error",
                                                  aecp_msg.targetEntityID,
                                                  avdecc_lib::utility::aem_cmd_value_to_name(aecp_msg.commandType));
            return -1;
        }
    }

    // cleanup
    [destination_MAC release];
    [aecp_msg release];

    return 0;
}

-(int)process_Mac_AECP_msg:(AVB17221AECPAEMMessage *)msg base:(uint8_t *)frame_base
{
    // message type
    jdksavdecc_common_control_header_set_control_data(static_cast<uint8_t>(msg.messageType),
                                                      frame_base,
                                                      avdecc_lib::ETHER_HDR_SIZE);
    
    // status
    jdksavdecc_common_control_header_set_status(static_cast<uint8_t>(msg.status),
                                                frame_base,
                                                avdecc_lib::ETHER_HDR_SIZE);
    
    // command specific data
    uint8_t * cmd_specific_data_base = (uint8_t *)[msg.commandSpecificData bytes];
    int cmd_specific_data_length = [msg.commandSpecificData length];
    memcpy((frame_base + AECPDU_COMMAND_SPECIFIC_DATA_OFFSET),
           cmd_specific_data_base,
           cmd_specific_data_length);

    // queue
    avdecc_lib::system_queue_rx(
                                (const uint8_t *)frame_base,
                                AECPDU_COMMAND_SPECIFIC_DATA_OFFSET + cmd_specific_data_length);
    return 0;
}

-(int)Mac_entity_to_ADP_frame:(AVB17221Entity *)Mac_entity
{
    // create a frame
    jdksavdecc_frame cmd_frame;
    jdksavdecc_frame_init(&cmd_frame);

    // write ADP
    struct jdksavdecc_adpdu adpdu;
    memset(&adpdu, 0, sizeof(adpdu));
    adpdu.header.subtype = JDKSAVDECC_SUBTYPE_ADP;
    jdksavdecc_uint64_write(Mac_entity.entityID, &adpdu.header.entity_id, 0, sizeof(uint64_t));
    jdksavdecc_uint64_write(Mac_entity.entityModelID, &adpdu.entity_model_id, 0, sizeof(uint64_t));
    adpdu.entity_capabilities = Mac_entity.entityCapabilities;
    adpdu.talker_stream_sources = Mac_entity.talkerStreamSources;
    adpdu.talker_capabilities = Mac_entity.talkerCapabilities;
    adpdu.listener_stream_sinks = Mac_entity.listenerStreamSinks;
    adpdu.listener_capabilities = Mac_entity.listenerCapabilities;
    adpdu.controller_capabilities = Mac_entity.controllerCapabilities;
    adpdu.available_index = Mac_entity.availableIndex;
    jdksavdecc_uint64_write(Mac_entity.gPTPGrandmasterID, &adpdu.gptp_grandmaster_id, 0, sizeof(uint64_t));
    adpdu.gptp_domain_number = Mac_entity.gPTPDomainNumber;
    adpdu.identify_control_index = Mac_entity.identifyControlIndex;
    adpdu.interface_index = Mac_entity.interfaceIndex;
    jdksavdecc_uint64_write(Mac_entity.associationID, &adpdu.association_id, 0, sizeof(uint64_t));
    jdksavdecc_adpdu_write(&adpdu, &cmd_frame.payload, avdecc_lib::ETHER_HDR_SIZE, sizeof(cmd_frame.payload));
    
    // write source mac
    jdksavdecc_eui48 src_mac;
    jdksavdecc_eui48_read(&src_mac, (const char *)[[Mac_entity.macAddresses firstObject] bytes], 0, sizeof(jdksavdecc_eui48));
    jdksavdecc_eui48_set(src_mac, &cmd_frame.payload, 6);
    
    // write destination mac
    jdksavdecc_eui48 dest_mac;
    jdksavdecc_eui48_init_from_uint64(&dest_mac, netif_mac_address);
    jdksavdecc_eui48_set(
                         dest_mac,
                         (void *)cmd_frame.payload, 0);
    
    // Set up Handlers to receive commands and responses
    [self.interface.acmp setHandler:self forEntityID:Mac_entity.entityID];
    [self.interface.aecp setCommandHandler:self forEntityID:Mac_entity.entityID];

    // queue
    avdecc_lib::system_queue_rx((const uint8_t *)&cmd_frame.payload,
                                sizeof(cmd_frame.payload));
    
    Mac_discovered_end_stations[Mac_entity.entityID] = true;
    return 0;
}

-(int)Mac_AECP_msg_to_frame:(AVB17221AECPAEMMessage *)msg
{
    // create a frame
    jdksavdecc_frame cmd_frame;
    jdksavdecc_frame_init(&cmd_frame);
    uint8_t * tx_frame_base = cmd_frame.payload;
    
    // set u-field if unsolicited
    uint16_t command_type = static_cast<uint16_t>(msg.commandType);
    if (msg.isUnsolicited)
        command_type |= 1 << 15;
    
    // write AECP common
    jdksavdecc_common_control_header_set_subtype(JDKSAVDECC_SUBTYPE_AECP, tx_frame_base, avdecc_lib::ETHER_HDR_SIZE);
    jdksavdecc_aecpdu_aem_set_command_type(command_type, tx_frame_base, avdecc_lib::ETHER_HDR_SIZE);
    jdksavdecc_eui64 target_entity_id, controller_entity_id;
    jdksavdecc_eui64_init_from_uint64(&target_entity_id, msg.targetEntityID);
    jdksavdecc_eui64_init_from_uint64(&controller_entity_id, msg.controllerEntityID);
    jdksavdecc_aecpdu_aem_set_controller_entity_id(controller_entity_id, tx_frame_base, avdecc_lib::ETHER_HDR_SIZE);
    jdksavdecc_common_control_header_set_stream_id(target_entity_id, tx_frame_base, avdecc_lib::ETHER_HDR_SIZE);
    jdksavdecc_aecpdu_aem_set_sequence_id(msg.sequenceID, tx_frame_base, avdecc_lib::ETHER_HDR_SIZE);
    jdksavdecc_common_control_header_set_control_data(static_cast<uint8_t>(msg.messageType),
                                                      tx_frame_base,
                                                      avdecc_lib::ETHER_HDR_SIZE);
    
    // write destination MAC
    jdksavdecc_eui48 dest_mac;
    jdksavdecc_eui48_init_from_uint64(&dest_mac, netif_mac_address);
    jdksavdecc_eui48_set(dest_mac, tx_frame_base, 0);
    
    // write source MAC
    jdksavdecc_eui48_set(jdksavdecc_eui48_get([msg.sourceMAC bytes], 0),
                         tx_frame_base, 6);
    
    return [(id) self process_Mac_AECP_msg:msg base:tx_frame_base];
}

- (void)didAddLocalEntity:(AVB17221Entity *)newEntity on17221EntityDiscovery:(AVB17221EntityDiscovery *)entityDiscovery
{
    [(id) self Mac_entity_to_ADP_frame:newEntity];
}

- (void)didRemoveLocalEntity:(AVB17221Entity *)oldEntity on17221EntityDiscovery:(AVB17221EntityDiscovery *)entityDiscovery
{
    Mac_discovered_end_stations[oldEntity.entityID] = false;
}

- (void)didRediscoverLocalEntity:(AVB17221Entity *)entity on17221EntityDiscovery:(AVB17221EntityDiscovery *)entityDiscovery
{
    [self didAddLocalEntity:entity on17221EntityDiscovery:entityDiscovery];
}

- (void)didUpdateLocalEntity:(AVB17221Entity *)entity changedProperties:(AVB17221EntityPropertyChanged)changedProperties on17221EntityDiscovery:(AVB17221EntityDiscovery *)entityDiscovery
{

}

- (void)didAddRemoteEntity:(AVB17221Entity *)newEntity on17221EntityDiscovery:(AVB17221EntityDiscovery *)entityDiscovery
{
    [(id) self Mac_entity_to_ADP_frame:newEntity];
}

- (void)didRemoveRemoteEntity:(AVB17221Entity *)oldEntity on17221EntityDiscovery:(AVB17221EntityDiscovery *)entityDiscovery
{
    Mac_discovered_end_stations[oldEntity.entityID] = false;
}

- (void)didRediscoverRemoteEntity:(AVB17221Entity *)entity on17221EntityDiscovery:(AVB17221EntityDiscovery *)entityDiscovery
{
    [(id) self Mac_entity_to_ADP_frame:entity];
}

- (void)didUpdateRemoteEntity:(AVB17221Entity *)entity changedProperties:(AVB17221EntityPropertyChanged)changedProperties on17221EntityDiscovery:(AVB17221EntityDiscovery *)entityDiscovery
{

}

- (BOOL)AECPDidReceiveCommand:(AVB17221AECPMessage *)message onInterface:(AVB17221AECPInterface *)anInterface
{
    switch ([message messageType])
    {
        case AVB17221AECPMessageTypeAEMCommand:
        {
            auto aecp_cmd = static_cast<AVB17221AECPAEMMessage *>(message);
            [(id) self Mac_AECP_msg_to_frame:aecp_cmd];
            break;
        }
        default:
            break;
    }
    return NO;
}

- (BOOL)AECPDidReceiveResponse:(AVB17221AECPMessage *)message onInterface:(AVB17221AECPInterface *)anInterface
{
    switch ([message messageType])
    {
        case AVB17221AECPMessageTypeAEMResponse:
        {
            auto aecp_resp = static_cast<AVB17221AECPAEMMessage *>(message);
            /*
             * We are only interested in unsolicited responses here.
             * Solicited responses are handled in the completion block
             * of send_Mac_AECP_msg()
             */
            if (aecp_resp.isUnsolicited)
                [(id) self Mac_AECP_msg_to_frame:aecp_resp];
            break;
        }
        default:
            break;
    }
    return NO;
}

- (BOOL)ACMPDidReceiveCommand:(AVB17221ACMPMessage *)message onInterface:(AVB17221ACMPInterface *)anInterface
{
    return NO;
}

- (BOOL)ACMPDidReceiveResponse:(AVB17221ACMPMessage *)message onInterface:(AVB17221ACMPInterface *)anInterface
{
    // create a frame
    jdksavdecc_frame cmd_frame;
    jdksavdecc_frame_init(&cmd_frame);

    // write ACMP common
    struct jdksavdecc_acmpdu_common_control_header acmpdu_common_ctrl_hdr;
    acmpdu_common_ctrl_hdr.subtype = JDKSAVDECC_SUBTYPE_ACMP;
    jdksavdecc_acmpdu_common_control_header_write(&acmpdu_common_ctrl_hdr,
                                                  cmd_frame.payload,
                                                  avdecc_lib::ETHER_HDR_SIZE,
                                                  sizeof(cmd_frame.payload));
    // write destination MAC
    jdksavdecc_eui48 dest_mac;
    jdksavdecc_eui48_init_from_uint64(&dest_mac, netif_mac_address);
    jdksavdecc_eui48_set(dest_mac, (void *)cmd_frame.payload, 0);
    
    // process Mac ACMP
    [(id) self process_Mac_ACMP_msg:message base:cmd_frame.payload];
    return NO;
}

@end
