/*
 * Copyright (c) 2019 Ritsumeikan University, Shiga, Japan.
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

#ifndef LR_WPAN_MAC_PL_HEADERS_H
#define LR_WPAN_MAC_PL_HEADERS_H

#include "lr-wpan-fields.h"

#include <ns3/header.h>
#include <ns3/mac16-address.h>
#include <ns3/mac64-address.h>

namespace ns3
{

/**
 * \ingroup lr-wpan
 * Implements the header for the MAC payload beacon frame according to
 * the IEEE 802.15.4-2011 Std.
 */
class BeaconPayloadHeader : public Header
{
  public:
    BeaconPayloadHeader();
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    void Print(std::ostream& os) const override;
    /**
     * Set the superframe specification field to the beacon payload header.
     * \param sfrmField The superframe specification field
     */
    void SetSuperframeSpecField(SuperframeField sfrmField);
    /**
     * Set the superframe Guaranteed Time Slot (GTS) fields to the beacon payload header.
     * \param gtsFields The GTS fields.
     */
    void SetGtsFields(GtsFields gtsFields);
    /**
     * Set the superframe Pending Address fields to the beacon payload header.
     * \param pndAddrFields The Pending Address fields.
     */
    void SetPndAddrFields(PendingAddrFields pndAddrFields);
    /**
     * Get the superframe specification field from the beacon payload header.
     * \return The superframe specification field
     */
    SuperframeField GetSuperframeSpecField() const;
    /**
     * Get the Guaranteed Time Slots (GTS) fields from the beacon payload header.
     * \return The GTS fields.
     */
    GtsFields GetGtsFields() const;
    /**
     * Get the pending address fields from the beacon payload header.
     * \return The Pending Address fields.
     */
    PendingAddrFields GetPndAddrFields() const;

  private:
    /**
     * Superframe Specification Field
     */
    SuperframeField m_superframeField;
    /**
     * GTS Fields
     */
    GtsFields m_gtsFields;
    /**
     * Pending Address Fields
     */
    PendingAddrFields m_pndAddrFields;
};

/**
 * \ingroup lr-wpan
 * Implements the header for the MAC payload command frame according to
 * the IEEE 802.15.4e-2011 section 5.2.2.5.2 LL Beacon frame format
 */

class LLBeaconPayloadHeader : public Header
{
  // TODOs

  public:

    LLBeaconPayloadHeader();

    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    void Print(std::ostream& os) const override;

    void SetFlagsFields(FlagsField flagsFields);
    void SetLLPanCoordAddr(Mac8Address panCAddr);
    void SetConfigurationSeqNum(uint8_t configurationSeqNum);
    void SetBaseTimeSlotSize(uint8_t baseTimeSlotSize);
    void SetNumOfBaseTsInSuperframe(uint8_t numOfBaseTSinSuperframe);
    void SetgroupAckBmp(uint16_t groupAckBmp);

    FlagsField GetFlagsFields() const;
    Mac8Address GetLLPanCoordAddr() const;
    uint8_t GetConfigurationSeqNum() const;
    uint8_t GetBaseTimeSlotSize() const;
    uint8_t GetNumOfBaseTsInSuperframe() const;
    uint16_t GetgroupAckBmp() const;



  private:

    FlagsField m_flagsFields;          // The Flags field contains control information.
    Mac8Address m_LLPanCoordIdFields;  // 8-bit simple address (i.e., macSimpleAddress) of the LLDN PAN coordinator.
    uint8_t m_configurationSeqNum;     // Identify the PAN-C ID & Configuration
    uint8_t m_timeslotSize;            // Length of a base timeslot
    uint8_t m_numOfBaseTSinSuperframe; // corresponds to macLLDNnumTimeSlot (Uplink + Bidirectional TS time length) 
                                       //! Note : This field present only in Online mode
    uint16_t m_groupAckBmp;            // Only present in online mode 
                                       //! Note : This field present only in Online mode
    
};

class CommandPayloadHeader : public Header
{
  public:
    /**
     *  The MAC command frames.
     *  See IEEE 802.15.4-2011 & 802.15.4e , Table 5
     */
    enum MacCommand
    {
        ASSOCIATION_REQ = 0x01,      //!< Association request (RFD true: Tx)
        ASSOCIATION_RESP = 0x02,     //!< Association response (RFD true: Rx)
        DISASSOCIATION_NOTIF = 0x03, //!< Disassociation notification (RFD true: TX, Rx)
        DATA_REQ = 0x04,             //!< Data Request (RFD true: Tx)
        PANID_CONFLICT = 0x05,       //!< Pan ID conflict notification (RFD true: Tx)
        ORPHAN_NOTIF = 0x06,         //!< Orphan Notification (RFD true: Tx)
        BEACON_REQ = 0x07,           //!< Beacon Request (RFD true: none )
        COOR_REALIGN = 0x08,         //!< Coordinator Realignment (RFD true: Rx)
        GTS_REQ = 0x09,              //!< GTS Request (RFD true: none)

        /* LLDN Amendment */
        LL_DISCOVER_RESP            = 0x0d,        //!< LLDN Association Request (RFD true: Tx)
        LL_CONFIGURATION_STATUS     = 0x0e,        
        LL_CONFIGURATON_REQ         = 0x0f,        
        LL_CTS_SHARED_GROUP         = 0x10,        
        LL_RTS                      = 0x11,        
        LL_CTS                      = 0x12,  

        /* DSME Amendment */
        DSME_ASSOCIATION_REQ        = 0x13,        //!< DSME Association Request (RFD true: Tx)
        DSME_ASSOCIATION_RESP       = 0x14,        //!< DSME Association Response (RFD true: Rx)
        DSME_GTS_REQ                = 0x15,        //!< DSME GTS Request (RFD true: TX, Rx)
        DSME_GTS_REPLY              = 0x16,        //!< DSME GTS Reply (RFD true: TX, Rx)
        DSME_GTS_NOTIFY             = 0x17,        //!< DSME GTS Notify (RFD true: TX, Rx)
        DSME_INFO_REQ               = 0x18,        //!< DSME Information Request (RFD true: TX, Rx)
        DSME_INFO_REPLY             = 0x19,        //!< DSME Information Reply (RFD true: TX, Rx)
        DSME_BEACON_ALLOC_NOTIF     = 0x1a,        //!< DSME Beacon allocation notification (RFD true: none)
        DSME_BEACON_COLLISION_NOTIF = 0x1b,        //!< DSME Beacon collision notification (RFD true: Tx)
        DSME_LINK_STATUS_REPORT     = 0x1c,        //!< DSME Link status report (RFD true: TX, Rx)

        /* AMCA Amendment */
        AMCA_BEACON_REQ             = 0x1d,
        AMCA_HELLO                  = 0x1e,
        AMCA_CHENNEL_PROBE          = 0x1f,

        /* LE Amendment */
        LE_RIT_DATA_REQ             = 0x20,

        CMD_RESERVED                = 0xff          //!< Reserved
    };

    /**
     *  Association Status Field values.
     *  See IEEE 802.15.4-2011, Table 6
     */
    enum AssocStatus
    {
        SUCCESSFUL = 0x00,    //!< Association successful
        FULL_CAPACITY = 0x01, //!< PAN at capacity
        ACCESS_DENIED = 0x02  //!< PAN access denied
    };

    CommandPayloadHeader();
    /**
     * Constructor
     * \param macCmd the command type of this command header
     */
    CommandPayloadHeader(enum MacCommand macCmd);
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    void Print(std::ostream& os) const override;

    /**
     * Set the command frame type
     * \param macCmd the command frame type
     */
    void SetCommandFrameType(MacCommand macCmd);
    /**
     * Set the Capability Information Field to the command payload header (Association Request
     * Command).
     * \param cap The capability Information field
     */
    void SetCapabilityField(CapabilityField cap);
    /**
     * Set the Short Address Assigned by the coordinator (Association Response Command).
     * \param shortAddr The short address assigned by the coordinator
     */
    void SetShortAddr(Mac16Address shortAddr);
    /**
     * Set status resulting from the association attempt (Association Response Command).
     * \param status The status resulting from the association attempt
     */
    void SetAssociationStatus(AssocStatus status);
    /**
     * Get the Short address assigned by the coordinator (Association Response Command).
     * \return The Mac16Address assigned by the coordinator
     */

    // TODO : add LLDN GET & SET FUNCTIONS
    Mac16Address GetShortAddr() const;
    /**
     * Get the status resulting from an association request (Association Response Command).
     * \return The resulting status from an association request
     */
    AssocStatus GetAssociationStatus() const;
    /**
     * Get the command frame type ID
     * \return The command type ID from the command payload header
     */
    MacCommand GetCommandFrameType() const;
    /**
     * Get the Capability Information Field from the command payload header. (Association Request
     * Command)
     * \return The Capability Information Field
     */
    CapabilityField GetCapabilityField() const;

  private:
    MacCommand m_cmdFrameId; //!< The command Frame Identifier
    CapabilityField
        m_capabilityInfo;      //!< Capability Information Field (Association Request Command)
    Mac16Address m_shortAddr;  //!< Contains the short address assigned by the coordinator
                               //!< (Association Response Command) See IEEE 802.15.4-2011 5.3.2.2.
    AssocStatus m_assocStatus; //!< Association Status (Association Response Command)

    // For LLDN MAC Command frame payload variable

    // LLDN Discover response command 
    Mac8Address m_simpleAddr;
    uint16_t m_timeslotDuration; //!< Timeslot length , received by beacon
    uint8_t m_typeIndicator;     //!< Uplink/bidirectional type indicator

    // Configuration status command
    uint8_t m_assignedTimeSlot;     //!< Timeslot assigned for transmission
    // Configuration request command
    uint8_t m_txChannel;            //!< Transmission channel
    uint8_t m_enMgntFrame;          //!< Enable MgmtTS or not

    // Clear to send shared group command
    uint8_t m_networkID;
    // Ready to send command
    Mac8Address m_shortOriginatorAddr;

};

} // namespace ns3

#endif /* LR_WPAN_MAC_PL_HEADERS_H */
