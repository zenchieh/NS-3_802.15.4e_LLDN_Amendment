/*
 * Copyright (c) 2020 Ritsumeikan University, Shiga, Japan.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Author: Alberto Gallegos Ramonet <ramonet@fc.ritsumei.ac.jp>
 */

#include "lr-wpan-mac-pl-headers.h"

#include <ns3/address-utils.h>
#include <ns3/simulator.h>
#include <ns3/log.h>


namespace ns3
{

/***********************************************************
 *                Beacon MAC Payload
 ***********************************************************/

BeaconPayloadHeader::BeaconPayloadHeader()
{
}

NS_OBJECT_ENSURE_REGISTERED(BeaconPayloadHeader);

TypeId
BeaconPayloadHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::BeaconPayloadHeader")
                            .SetParent<Header>()
                            .SetGroupName("LrWpan")
                            .AddConstructor<BeaconPayloadHeader>();
    return tid;
}

TypeId
BeaconPayloadHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
BeaconPayloadHeader::GetSerializedSize() const
{
    uint32_t size = 0;
    size += m_superframeField.GetSerializedSize();
    size += m_gtsFields.GetSerializedSize();
    size += m_pndAddrFields.GetSerializedSize();

    return size;
}

void
BeaconPayloadHeader::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;
    i = m_superframeField.Serialize(i);
    i = m_gtsFields.Serialize(i);
    i = m_pndAddrFields.Serialize(i);
}

uint32_t
BeaconPayloadHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;
    i = m_superframeField.Deserialize(i);
    i = m_gtsFields.Deserialize(i);
    i = m_pndAddrFields.Deserialize(i);

    return i.GetDistanceFrom(start);
}

void
BeaconPayloadHeader::Print(std::ostream& os) const
{
    os << "| Superframe Spec Field | = " << m_superframeField
       << "| GTS Spec Field | = " << m_gtsFields.GetGtsSpecField()
       << "| Pending Spec Field| =" << m_pndAddrFields.GetPndAddrSpecField();
}

void
BeaconPayloadHeader::SetSuperframeSpecField(SuperframeField sf)
{
    m_superframeField = sf;
}

void
BeaconPayloadHeader::SetGtsFields(GtsFields gtsFields)
{
    m_gtsFields = gtsFields;
}

void
BeaconPayloadHeader::SetPndAddrFields(PendingAddrFields pndAddrFields)
{
    m_pndAddrFields = pndAddrFields;
}

SuperframeField
BeaconPayloadHeader::GetSuperframeSpecField() const
{
    return m_superframeField;
}

GtsFields
BeaconPayloadHeader::GetGtsFields() const
{
    return m_gtsFields;
}

PendingAddrFields
BeaconPayloadHeader::GetPndAddrFields() const
{
    return m_pndAddrFields;
}

/***********************************************************
 *                Command MAC Payload
 ***********************************************************/

CommandPayloadHeader::CommandPayloadHeader()
{
    SetCommandFrameType(CMD_RESERVED);
}

CommandPayloadHeader::CommandPayloadHeader(enum MacCommand macCmd)
{
    SetCommandFrameType(macCmd);
}

NS_OBJECT_ENSURE_REGISTERED(CommandPayloadHeader);

TypeId
CommandPayloadHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::CommandPayloadHeader")
                            .SetParent<Header>()
                            .SetGroupName("LrWpan")
                            .AddConstructor<CommandPayloadHeader>();
    return tid;
}

TypeId
CommandPayloadHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
CommandPayloadHeader::GetSerializedSize() const
{
    uint32_t size = 1; // Command frame identifier
    // TODO: add missing serialize commands size when other commands are added.
    switch (m_cmdFrameId)
    {
    case ASSOCIATION_REQ:
        size += m_capabilityInfo.GetSerializedSize();
        break;
    case ASSOCIATION_RESP:
        size += 3; // (short address + Association Status)
        break;
    case DISASSOCIATION_NOTIF:
        break;
    case DATA_REQ:
        break;
    case PANID_CONFLICT:
        break;
    case ORPHAN_NOTIF:
        break;
    case BEACON_REQ:
        break;
    case COOR_REALIGN:
        break;
    case GTS_REQ:
        break;
    case CMD_RESERVED:
        break;
    // TODO : Add LLDN and DSME amendment
    case LL_DISCOVER_RESP:
        // TODO : need to pass Discovery parameters to get length
        NS_LOG_UNCOND("GetSerializedSize() case LL_CONFIGURATON_REQ");
        // TODO - need to calculate timeslot length
        /**
         * Discover response command
         * See 802.15.4e-2011 Section 5.3.10.1.4
         *      MHR         - Command Frame Identifier - //?Discovery parameters
         * Variable Octets  -            1             -   Variable Octets        
         *? Discovery parameters
         *  Full MAC address - Required timeslot duration - Uplink/bidirectional type indicator
         *          1        -              1             -                 1
         */
        size += 1 + 1 + 1;
        break;
    case LL_CONFIGURATION_STATUS:
        NS_LOG_UNCOND("GetSerializedSize() case LL_CONFIGURATION_STATUS");
        /**
         * Configuration status command
         * See 802.15.4e-2011 Section 5.3.10.2.5
         *      MHR         - Command Frame Identifier - //?Configuration Parameters 
         * Variable Octets  -            1             -   Variable Octets  
         * //! 可以考慮把Short Macaddress field 拿掉?
         *? Configuration Parameters
         *  Full MAC address - Short MAC address - Required timeslot duration - Uplink/bidirectional type indicator - Assigned timeslots
         *          1        -          2        -             1              -                  1                  -         1 
         */
        size += 1 + 2 + 1 + 1 + 1;
    case LL_CONFIGURATON_REQ:
        NS_LOG_UNCOND("GetSerializedSize() case LL_CONFIGURATON_REQ");
        /**
         * Configuration request command
         * See 802.15.4e-2011 Section 5.3.10.3.4
         *      MHR         - Command Frame Identifier - //?Configuration Parameters 
         * Variable Octets  -            1             -   Variable Octets        
         *? Configuration Parameters
         *  Full MAC address - Short MAC address - Transmission channel - Existence of management frames - Timeslot duration - Assigned timeslots
         *          1        -          2        -          1           -               1                -         1         -         1
         */
        size += 1 + 2 + 1 + 1 + 1 + 1;
        break;
    case LL_CTS_SHARED_GROUP:
        NS_LOG_UNCOND("GetSerializedSize() case LL_CTS_SHARED_GROUP");
        /**
         * Clear to send shared group command
         * See 802.15.4e-2011 Section 5.3.10.4.1
         *      MHR         - Command Frame Identifier - Network ID   // The Network ID field contains an identifier specific to the LLDN PAN coordinator.
         * Variable Octets  -            1             -     1      
         */
        size += 1;
        break;

    case LL_RTS:
        NS_LOG_UNCOND("GetSerializedSize() case LL_RTS");
        /**
         * Ready to send command
         * See 802.15.4e-2011 Section 5.3.10.5.1
         *      MHR         - Command Frame Identifier - Short Originator Address - Network ID  
         * Variable Octets  -            1             -             1            -      1
         *?  Short Originator Address = simple addr of the device who sending this RTS frame.
         *?  Network ID = should identical to the corresponding received CTS shared group frame.
         */
        size += 1 + 1;
        break;
    case LL_CTS:
        NS_LOG_UNCOND("GetSerializedSize() case LL_CTS");
        /**
         * Clear to send command
         * See 802.15.4e-2011 Section 5.3.10.6.1
         *      MHR         - Command Frame Identifier - Short Originator Address - Network ID  
         * Variable Octets  -            1             -             1            -      1
         *?  Short Originator Address = simple address of the device to which this CTS frame is directed.
         *?  Network ID = should identical to the corresponding received RTS frame.
         */
        size += 1 + 1;
        break;
    default:
        break;
    }
    return size;
}

void
CommandPayloadHeader::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;
    i.WriteU8(m_cmdFrameId);
    // TODO: add missing serialize commands when other commands are added.
    switch (m_cmdFrameId)
    {
    case ASSOCIATION_REQ:
        i = m_capabilityInfo.Serialize(i);
        break;
    case ASSOCIATION_RESP:
        WriteTo(i, m_shortAddr);
        i.WriteU8(m_assocStatus);
        break;
    case DISASSOCIATION_NOTIF:
        break;
    case DATA_REQ:
        break;
    case PANID_CONFLICT:
        break;
    case ORPHAN_NOTIF:
        break;
    case BEACON_REQ:
        break;
    case COOR_REALIGN:
        break;
    case GTS_REQ:
        break;
    case CMD_RESERVED:
        break;

    //!< LLDN MAC command frame serializer
    case LL_DISCOVER_RESP:

        NS_LOG_UNCOND("Serialize() case LL_CONFIGURATON_REQ");
        /**
         * Discover response command
         * See 802.15.4e-2011 Section 5.3.10.1.4
         *      MHR         - Command Frame Identifier - //?Discovery parameters
         * Variable Octets  -            1             -   Variable Octets        
         *? Discovery parameters
         *  Full MAC address - Required timeslot duration - Uplink/bidirectional type indicator
         *          1        -              1             -                 1
         */
        i.WriteU8(m_timeslotDuration);
        i.WriteU8(m_typeIndicator);
        break;
    case LL_CONFIGURATION_STATUS:
        NS_LOG_UNCOND("Serialize() case LL_CONFIGURATION_STATUS");
        /**
         * Configuration status command
         * See 802.15.4e-2011 Section 5.3.10.2.5
         *      MHR         - Command Frame Identifier - //?Configuration Parameters 
         * Variable Octets  -            1             -   Variable Octets  
         *? Configuration Parameters
         *  Full MAC address - Short MAC address - Required timeslot duration - Uplink/bidirectional type indicator - Assigned timeslots
         *          1        -          2        -             1              -                  1                  -         1 
         */
        WriteTo(i,m_simpleAddr);
        WriteTo(i,m_shortAddr);
        i.WriteU8(m_timeslotDuration);
        i.WriteU8(m_typeIndicator);
        i.WriteU8(m_assignedTimeSlot);
        break;
    case LL_CONFIGURATON_REQ:
        NS_LOG_UNCOND("Serialize() case LL_CONFIGURATON_REQ");
        /**
         * Configuration request command
         * See 802.15.4e-2011 Section 5.3.10.3.4
         *      MHR         - Command Frame Identifier - //?Configuration Parameters 
         * Variable Octets  -            1             -   Variable Octets        
         *? Configuration Parameters
         *  Full MAC address - Short MAC address - Transmission channel - Existence of management frames - Timeslot duration - Assigned timeslots
         *          1        -          2        -          1           -               1                -         1         -         1
         */
        WriteTo(i,m_simpleAddr);
        WriteTo(i,m_shortAddr);
        i.WriteU8(m_txChannel);
        i.WriteU8(m_enMgntFrame);
        i.WriteU8(m_timeslotDuration);
        i.WriteU8(m_assignedTimeSlot);
        break;
    case LL_CTS_SHARED_GROUP:
        NS_LOG_UNCOND("Serialize() case LL_CTS_SHARED_GROUP");
        /**
         * Clear to send shared group command
         * See 802.15.4e-2011 Section 5.3.10.4.1
         *      MHR         - Command Frame Identifier - Network ID   // The Network ID field contains an identifier specific to the LLDN PAN coordinator.
         * Variable Octets  -            1             -     1      
         */
        i.WriteU8(m_networkID);
        break;

    case LL_RTS:
        NS_LOG_UNCOND("Serialize() case LL_RTS");
        /**
         * Ready to send command
         * See 802.15.4e-2011 Section 5.3.10.5.1
         *      MHR         - Command Frame Identifier - Short Originator Address - Network ID  
         * Variable Octets  -            1             -             1            -      1
         *?  Short Originator Address = simple addr of the device who sending this RTS frame.
         *?  Network ID = should identical to the corresponding received CTS shared group frame.
         */
        WriteTo(i,m_shortOriginatorAddr);
        i.WriteU8(m_networkID);
        break;
    case LL_CTS:
        NS_LOG_UNCOND("Serialize() case LL_CTS");
        /**
         * Clear to send command
         * See 802.15.4e-2011 Section 5.3.10.6.1
         *      MHR         - Command Frame Identifier - Short Originator Address - Network ID  
         * Variable Octets  -            1             -             1            -      1
         *?  Short Originator Address = simple address of the device to which this CTS frame is directed.
         *?  Network ID = should identical to the corresponding received RTS frame.
         */
        WriteTo(i,m_shortOriginatorAddr);
        i.WriteU8(m_networkID);
        break;
    default:
        break;
    }
}

uint32_t
CommandPayloadHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;
    m_cmdFrameId = static_cast<MacCommand>(i.ReadU8());
    // TODO: add missing deserialize commands when other commands are added.
    switch (m_cmdFrameId)
    {
    case ASSOCIATION_REQ:
        i = m_capabilityInfo.Deserialize(i);
        break;
    case ASSOCIATION_RESP:
        ReadFrom(i, m_shortAddr);
        m_assocStatus = static_cast<AssocStatus>(i.ReadU8());
        break;
    case DISASSOCIATION_NOTIF:
        break;
    case DATA_REQ:
        break;
    case PANID_CONFLICT:
        break;
    case ORPHAN_NOTIF:
        break;
    case BEACON_REQ:
        break;
    case COOR_REALIGN:
        break;
    case GTS_REQ:
        break;
    case CMD_RESERVED:
        break;

    //!< LLDN MAC command frame deserializer
    case LL_DISCOVER_RESP:

        NS_LOG_UNCOND("Deserialize() case LL_CONFIGURATON_REQ");
        /**
         * Discover response command
         * See 802.15.4e-2011 Section 5.3.10.1.4
         *      MHR         - Command Frame Identifier - //?Discovery parameters
         * Variable Octets  -            1             -   Variable Octets        
         *? Discovery parameters
         *  Full MAC address - Required timeslot duration - Uplink/bidirectional type indicator
         *          1        -              1             -                 1
         */

        m_timeslotDuration = i.ReadU8();
        m_typeIndicator = i.ReadU8();

        break;
    case LL_CONFIGURATION_STATUS:
        NS_LOG_UNCOND("Deserialize() case LL_CONFIGURATION_STATUS");
        /**
         * Configuration status command
         * See 802.15.4e-2011 Section 5.3.10.2.5
         *      MHR         - Command Frame Identifier - //?Configuration Parameters 
         * Variable Octets  -            1             -   Variable Octets  
         *? Configuration Parameters
         *  Full MAC address - Short MAC address - Required timeslot duration - Uplink/bidirectional type indicator - Assigned timeslots
         *          1        -          2        -             1              -                  1                  -         1 
         */
        ReadFrom(i,m_simpleAddr);
        ReadFrom(i,m_shortAddr);
        m_timeslotDuration = i.ReadU8();
        m_typeIndicator = i.ReadU8();
        m_assignedTimeSlot = i.ReadU8();
        break;
    case LL_CONFIGURATON_REQ:
        NS_LOG_UNCOND("Deserialize() case LL_CONFIGURATON_REQ");
        /**
         * Configuration request command
         * See 802.15.4e-2011 Section 5.3.10.3.4
         *      MHR         - Command Frame Identifier - //?Configuration Parameters 
         * Variable Octets  -            1             -   Variable Octets        
         *? Configuration Parameters
         *  Full MAC address - Short MAC address - Transmission channel - Existence of management frames - Timeslot duration - Assigned timeslots
         *          1        -          2        -          1           -               1                -         1         -         1
         */
        ReadFrom(i,m_simpleAddr);
        ReadFrom(i,m_shortAddr);
        m_txChannel = i.ReadU8();
        m_enMgntFrame = i.ReadU8();
        m_timeslotDuration = i.ReadU8();
        m_assignedTimeSlot = i.ReadU8();
        break;
    case LL_CTS_SHARED_GROUP:
        NS_LOG_UNCOND("Deserialize() case LL_CTS_SHARED_GROUP");
        /**
         * Clear to send shared group command
         * See 802.15.4e-2011 Section 5.3.10.4.1
         *      MHR         - Command Frame Identifier - Network ID   // The Network ID field contains an identifier specific to the LLDN PAN coordinator.
         * Variable Octets  -            1             -     1      
         */
        m_networkID =i.ReadU8();
        break;

    case LL_RTS:
        NS_LOG_UNCOND("Deserialize() case LL_RTS");
        /**
         * Ready to send command
         * See 802.15.4e-2011 Section 5.3.10.5.1
         *      MHR         - Command Frame Identifier - Short Originator Address - Network ID  
         * Variable Octets  -            1             -             1            -      1
         *?  Short Originator Address = simple addr of the device who sending this RTS frame.
         *?  Network ID = should identical to the corresponding received CTS shared group frame.
         */

        ReadFrom(i,m_shortOriginatorAddr);
        m_networkID = i.ReadU8();
        break;
    case LL_CTS:
        NS_LOG_UNCOND("Deserialize() case LL_CTS");
        /**
         * Clear to send command
         * See 802.15.4e-2011 Section 5.3.10.6.1
         *      MHR         - Command Frame Identifier - Short Originator Address - Network ID  
         * Variable Octets  -            1             -             1            -      1
         *?  Short Originator Address = simple address of the device to which this CTS frame is directed.
         *?  Network ID = should identical to the corresponding received RTS frame.
         */
        ReadFrom(i,m_shortOriginatorAddr);
        m_networkID = i.ReadU8();
        break;
    default:
        break;
    }

    return i.GetDistanceFrom(start);
}

void
CommandPayloadHeader::Print(std::ostream& os) const
{
    os << "| MAC Command Frame ID | = " << (uint32_t)m_cmdFrameId;
    switch (m_cmdFrameId)
    {
    case ASSOCIATION_REQ:
        os << "| Device Type FFD | = " << m_capabilityInfo.IsDeviceTypeFfd()
           << "| Alternative Power Source available | = " << m_capabilityInfo.IsPowSrcAvailable()
           << "| Receiver on when Idle | = " << m_capabilityInfo.IsReceiverOnWhenIdle()
           << "| Security capable | = " << m_capabilityInfo.IsSecurityCapability()
           << "| Allocate address on | = " << m_capabilityInfo.IsShortAddrAllocOn();
        break;
    case ASSOCIATION_RESP:
        os << "| Assigned Short Address | = " << m_shortAddr
           << "| Status Response | = " << m_assocStatus;
        break;
    case DISASSOCIATION_NOTIF:
        break;
    case DATA_REQ:
        break;
    case PANID_CONFLICT:
        break;
    case ORPHAN_NOTIF:
        break;
    case BEACON_REQ:
        break;
    case COOR_REALIGN:
        break;
    case GTS_REQ:
        break;
    case CMD_RESERVED:
        break;
    default:
        break;
    }
}

void
CommandPayloadHeader::SetCommandFrameType(MacCommand macCommand)
{
    m_cmdFrameId = macCommand;
}

void
CommandPayloadHeader::SetCapabilityField(CapabilityField cap)
{
    NS_ASSERT(m_cmdFrameId == ASSOCIATION_REQ);
    m_capabilityInfo = cap;
}

CommandPayloadHeader::MacCommand
CommandPayloadHeader::GetCommandFrameType() const
{
    switch (m_cmdFrameId)
    {
    case 0x01:
        return ASSOCIATION_REQ;
        break;
    case 0x02:
        return ASSOCIATION_RESP;
        break;
    case 0x03:
        return DISASSOCIATION_NOTIF;
        break;
    case 0x04:
        return DATA_REQ;
        break;
    case 0x05:
        return PANID_CONFLICT;
        break;
    case 0x06:
        return ORPHAN_NOTIF;
        break;
    case 0x07:
        return BEACON_REQ;
        break;
    case 0x08:
        return COOR_REALIGN;
        break;
    case 0x09:
        return GTS_REQ;
        break;
    case 0x0d:
        return LL_DISCOVER_RESP;
        break;
    case 0x0e:
        return LL_CONFIGURATION_STATUS;
        break;
    case 0x0f:
        return LL_CONFIGURATON_REQ;
        break;
    case 0x10:
        return LL_CTS_SHARED_GROUP;
        break;
    case 0x11:
        return LL_RTS;
        break;
    case 0x12:
        return LL_CTS;
        break;
    case 0x13:
        return DSME_ASSOCIATION_REQ;
        break;
    case 0x14:
        return DSME_ASSOCIATION_RESP;
        break;
    case 0x15:
        return DSME_GTS_REQ;
        break;
    case 0x16:
        return DSME_GTS_REPLY;
        break;
    case 0x17:
        return DSME_GTS_NOTIFY;
        break;
    case 0x18:
        return DSME_INFO_REQ;
        break;
    case 0x19:
        return DSME_INFO_REPLY;
        break;
    case 0x1a:
        return DSME_BEACON_ALLOC_NOTIF;
        break;
    case 0x1b:
        return DSME_BEACON_COLLISION_NOTIF;
        break;
    case 0x1c:
        return DSME_LINK_STATUS_REPORT;
        break;
    case 0x1d:
        return AMCA_BEACON_REQ;
        break;
    case 0x1e:
        return AMCA_HELLO;
        break;
    case 0x1f:
        return AMCA_CHENNEL_PROBE;
        break;
    case 0x20:
        return LE_RIT_DATA_REQ;
        break;
    default:
        return CMD_RESERVED;
    }
}

void
CommandPayloadHeader::SetShortAddr(Mac16Address shortAddr)
{
    NS_ASSERT(m_cmdFrameId == ASSOCIATION_RESP);
    m_shortAddr = shortAddr;
}

void
CommandPayloadHeader::SetAssociationStatus(AssocStatus status)
{
    NS_ASSERT(m_cmdFrameId == ASSOCIATION_RESP);
    m_assocStatus = status;
}

Mac16Address
CommandPayloadHeader::GetShortAddr() const
{
    NS_ASSERT(m_cmdFrameId == ASSOCIATION_RESP);
    return m_shortAddr;
}

CommandPayloadHeader::AssocStatus
CommandPayloadHeader::GetAssociationStatus() const
{
    NS_ASSERT(m_cmdFrameId == ASSOCIATION_RESP);
    return m_assocStatus;
}

CapabilityField
CommandPayloadHeader::GetCapabilityField() const
{
    NS_ASSERT(m_cmdFrameId == ASSOCIATION_REQ);
    return m_capabilityInfo;
}

} // namespace ns3
