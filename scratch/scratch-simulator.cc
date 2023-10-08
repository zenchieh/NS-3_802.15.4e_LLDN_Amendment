/*
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
 */


#include <ns3/constant-position-mobility-model.h>
#include <ns3/core-module.h>
#include <ns3/log.h>
#include <ns3/lr-wpan-module.h>
#include <ns3/packet.h>
#include <ns3/propagation-delay-model.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/simulator.h>
#include <ns3/single-model-spectrum-channel.h>

#include <iostream>

/* Define base timeslot calculation parameters */
/* Unit : symblos */
#define NUM_PHY_HDR               6  // (Uint : symblos) , (p) Number of octets of PHY header
#define NUM_MAC_OVERHEAD          3  // (Uint : symblos) , (m) Number of octets of MAC overhead (MHR + MFR) for LL-DATA frames
#define MAX_DATA_PAYLOAD         40  // (Uint : symblos) , (n) Maximum expected number of octets of data payload, rcv from beacon frame - Timeslot size field
#define NUM_PHY_HEADER_PER_OCTECT 2  // (Uint : symblos/octect) , (sp) Number of symbols per octet in PHY header
#define NUM_PSDU_PER_OCTECT       2  // (Uint : symblos/octect) , (sm) Number of symbols per octet in PSDU
#define SYMBOL_RATE           62500  // (Uint : symbols/s) , (v) Symbol rate
using namespace ns3;

// static void
// BeaconIndication(MlmeBeaconNotifyIndicationParams params, Ptr<Packet> p)
// {
//     NS_LOG_UNCOND(Simulator::Now().GetSeconds()
//                   << " secs | Received BEACON packet of size " << p->GetSize());
// }

// static void
// DataIndication(McpsDataIndicationParams params, Ptr<Packet> p)
// {
//     NS_LOG_UNCOND(Simulator::Now().GetSeconds()
//                   << " secs | Received DATA packet of size " << p->GetSize());
// }

// static void
// TransEndIndication(McpsDataConfirmParams params)
// {
//     // In the case of transmissions with the Ack flag activated, the transaction is only
//     // successful if the Ack was received.
//     if (params.m_status == LrWpanMcpsDataConfirmStatus::IEEE_802_15_4_SUCCESS)
//     {
//         NS_LOG_UNCOND(Simulator::Now().GetSeconds() << " secs | Transmission successfully sent");
//     }
// }

// static void
// DataIndicationCoordinator(McpsDataIndicationParams params, Ptr<Packet> p)
// {
//     NS_LOG_UNCOND(Simulator::Now().GetSeconds()
//                   << "s Coordinator Received DATA packet (size " << p->GetSize() << " bytes)");
// }

static void
DiscoveryConfirm(MlmeLLDNDiscoveryConfirmsParams params)
{

}

static void
ConfigurationConfirm(MlmeLLDNConfigurationConfirmParams params)
{

}

static void
OnlineIndication(MlmeLLDNOnlineIndicationParams params)
{
    NS_LOG_UNCOND(Simulator::Now().GetSeconds()
                  << " secs | Switch to Online State." << "\n");
}

int
main(int argc, char* argv[])
{
    LogComponentEnableAll(LOG_PREFIX_TIME);
    LogComponentEnableAll(LOG_PREFIX_FUNC);
    LogComponentEnable("LrWpanMac", LOG_LEVEL_INFO);
    LogComponentEnable("LrWpanCsmaCa", LOG_LEVEL_INFO);

    LrWpanHelper lrWpanHelper;

    // Create 2 nodes, and a NetDevice for each one
    Ptr<Node> node_LLDN_PAN_C = CreateObject<Node>();
    Ptr<Node> node_LLDN_device = CreateObject<Node>();

    Ptr<LrWpanNetDevice> dev1_PAN_C = CreateObject<LrWpanNetDevice>();
    Ptr<LrWpanNetDevice> dev2_device = CreateObject<LrWpanNetDevice>();

    dev1_PAN_C->SetAddress(Mac8Address("1"));
    dev2_device->SetAddress(Mac8Address("2"));

    Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel>();
    Ptr<LogDistancePropagationLossModel> propModel =
        CreateObject<LogDistancePropagationLossModel>();
    Ptr<ConstantSpeedPropagationDelayModel> delayModel =
        CreateObject<ConstantSpeedPropagationDelayModel>();
    channel->AddPropagationLossModel(propModel);
    channel->SetPropagationDelayModel(delayModel);

    dev1_PAN_C->SetChannel(channel);
    dev2_device->SetChannel(channel);

    node_LLDN_PAN_C->AddDevice(dev1_PAN_C);
    node_LLDN_device->AddDevice(dev2_device);

    ///////////////// Mobility   ///////////////////////
    Ptr<ConstantPositionMobilityModel> sender1Mobility =
        CreateObject<ConstantPositionMobilityModel>();
    sender1Mobility->SetPosition(Vector(0, 0, 0));
    dev1_PAN_C->GetPhy()->SetMobility(sender1Mobility);
    Ptr<ConstantPositionMobilityModel> sender2Mobility =
        CreateObject<ConstantPositionMobilityModel>();

    sender2Mobility->SetPosition(Vector(0, 10, 0)); // 10 m distance
    dev2_device->GetPhy()->SetMobility(sender2Mobility);

    /////// MAC layer Callbacks hooks/////////////
    // Set LLDN association callback
    MlmeLLDNDiscoveryConfirmCallback discoveryConfirmCallBack;
    discoveryConfirmCallBack = MakeCallback(&DiscoveryConfirm);
    dev1_PAN_C->GetMac()-> SetMlmeLLDNDiscoveryConfirmCallback(discoveryConfirmCallBack);

    MlmeLLDNConfigurationConfirmCallback configurationConfirmCallBack;
    configurationConfirmCallBack = MakeCallback(&ConfigurationConfirm);
    dev1_PAN_C->GetMac()-> SetMlmeLLDNConfigurationConfirmCallback(configurationConfirmCallBack);

    MlmeLLDNOnlineIndicationCallback onlineIndicationCallBack;
    onlineIndicationCallBack = MakeCallback(&OnlineIndication);
    dev2_device->GetMac()-> SetMlmeLLDNOnlineIndicationCallback(onlineIndicationCallBack);


    // TODO set LLDN params

    /***************************/
    /*     Discovery State     */
    /***************************/

    int dataPayloadSize = MAX_DATA_PAYLOAD;

    // Set PAN-C Discovery state parameters
    dev1_PAN_C->GetMac()->SetMacLLDNcoordinator(true);
    dev1_PAN_C->GetMac()->SetLLDNModeEnabled();     /* In discovery state must have two mgmt timeslot */
    dev1_PAN_C->GetMac()->SetMacLLDNmgmtTSEnabled();
    dev1_PAN_C->GetMac()->SetAssociatedCoor(Mac8Address("1"));
    dev1_PAN_C->GetMac()->SetMlmeLLDNTransmissionState(FlagsField::DISCOVERY_STATE);
    dev1_PAN_C->GetMac()->SetMlmeLLDNTransmissionDirection(FlagsField::DOWNLINK);
    dev1_PAN_C->GetMac()->SetMlmeLLDNTimeSlotPerMgmtTS(2); // For one Uplink mgmt timeslot and one Downlink mgmt timeslot
    dev1_PAN_C->GetMac()->SetMlmeLLDNTimeslotSize(MAX_DATA_PAYLOAD);
    
    // TODO: Set LLDN device Discovery state parameters
    dev2_device->GetMac()->SetLLDNModeEnabled();
    dev2_device->GetMac()->SetSimpleAddress(Mac8Address("2"));
    dev2_device->GetMac()->SetMlmeLLDNTransmissionState(FlagsField::DISCOVERY_STATE);

    ///////////////////// Start transmitting beacons from coordinator ////////////////////////


    // Simulator::ScheduleWithContext(1,
    //                                Seconds(2.0),
    //                                //TODO
    //                                &LrWpanMac::MlmeLLDiscoveryStart,
    //                                dev1_PAN_C->GetMac(),
    //                                params);

    MlmeStartRequestParams params;
    params.m_panCoor = true;
    params.m_PanId = 5;
    params.m_bcnOrd = 14;
    params.m_sfrmOrd = 6;
    Simulator::ScheduleWithContext(1,
                                   Seconds(2.0),
                                   &LrWpanMac::MlmeStartRequest,
                                   dev1_PAN_C->GetMac(),
                                   params);

    ///////////////////// Transmission of data Packets from end device //////////////////////

    Ptr<Packet> p1 = Create<Packet>(5);
    McpsDataRequestParams params2;
    params2.m_dstPanId = 5;
    params2.m_srcAddrMode = SHORT_ADDR;
    params2.m_dstAddrMode = SHORT_ADDR;
    params2.m_dstAddr = Mac16Address("00:01");
    params2.m_msduHandle = 0;
    // params2.m_txOptions = TX_OPTION_ACK;  // Enable direct transmission with Ack

    /////////////////////////////////////////////////////////////////////////////////////
    // Examples of time parameters for transmissions in the first incoming superframe. //
    /////////////////////////////////////////////////////////////////////////////////////

    // 2.981 sec      No time to finish CCA in CAP, the transmission at this time will cause
    //                the packet to be deferred to the next superframe.

    // 2.982 sec      No time to finish random backoff delay in CAP, the  transmission at this
    //                time will cause the packet to be deferred to the next superframe.

    // 2.93 sec       Enough time, the packet can be transmitted within the CAP of the first
    // superframe

    // MCPS-DATA.request Beacon enabled Direct Transmission (dev2_device)
    // Frame transmission from End Device to Coordinator (Direct transmission)
    Simulator::ScheduleWithContext(1,
                                   Seconds(2.93),
                                   &LrWpanMac::McpsDataRequest,
                                   dev2_device->GetMac(),
                                   params2,
                                   p1);

    Simulator::Stop(Seconds(600));
    Simulator::Run();

    Simulator::Destroy();
    return 0;
}

