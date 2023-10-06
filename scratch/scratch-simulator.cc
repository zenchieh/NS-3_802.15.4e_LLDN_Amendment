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

    Ptr<LrWpanNetDevice> dev0_PAN_C = CreateObject<LrWpanNetDevice>();
    Ptr<LrWpanNetDevice> dev1_device = CreateObject<LrWpanNetDevice>();

    dev0_PAN_C->SetAddress(Mac8Address("1"));
    dev1_device->SetAddress(Mac8Address("2"));

    Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel>();
    Ptr<LogDistancePropagationLossModel> propModel =
        CreateObject<LogDistancePropagationLossModel>();
    Ptr<ConstantSpeedPropagationDelayModel> delayModel =
        CreateObject<ConstantSpeedPropagationDelayModel>();
    channel->AddPropagationLossModel(propModel);
    channel->SetPropagationDelayModel(delayModel);

    dev0_PAN_C->SetChannel(channel);
    dev1_device->SetChannel(channel);

    node_LLDN_PAN_C->AddDevice(dev0_PAN_C);
    node_LLDN_device->AddDevice(dev1_device);

    ///////////////// Mobility   ///////////////////////
    Ptr<ConstantPositionMobilityModel> sender0Mobility =
        CreateObject<ConstantPositionMobilityModel>();
    sender0Mobility->SetPosition(Vector(0, 0, 0));
    dev0_PAN_C->GetPhy()->SetMobility(sender0Mobility);
    Ptr<ConstantPositionMobilityModel> sender1Mobility =
        CreateObject<ConstantPositionMobilityModel>();

    sender1Mobility->SetPosition(Vector(0, 10, 0)); // 10 m distance
    dev1_device->GetPhy()->SetMobility(sender1Mobility);

    /////// MAC layer Callbacks hooks/////////////
    // Set LLDN association callback
    MlmeLLDNDiscoveryConfirmCallback discoveryConfirmCallBack;
    discoveryConfirmCallBack = MakeCallback(&DiscoveryConfirm);
    dev0_PAN_C->GetMac()-> SetMlmeLLDNDiscoveryConfirmCallback(discoveryConfirmCallBack);

    MlmeLLDNConfigurationConfirmCallback configurationConfirmCallBack;
    configurationConfirmCallBack = MakeCallback(&ConfigurationConfirm);
    dev0_PAN_C->GetMac()-> SetMlmeLLDNConfigurationConfirmCallback(configurationConfirmCallBack);

    MlmeLLDNOnlineIndicationCallback onlineIndicationCallBack;
    onlineIndicationCallBack = MakeCallback(&OnlineIndication);
    dev1_device->GetMac()-> SetMlmeLLDNOnlineIndicationCallback(onlineIndicationCallBack);


    // TODO set LLDN params

    //////////// Manual device association ////////////////////
    // Note: We manually associate the devices to a PAN coordinator
    //       because currently there is no automatic association behavior (bootstrap);
    //       The PAN COORDINATOR does not need to associate or set its
    //       PAN Id or its own coordinator id, these are set
    //       by the MLME-start.request primitive when used.

    dev1_device->GetMac()->SetPanId(5);
    dev1_device->GetMac()->SetAssociatedCoor(Mac16Address("00:01"));

    ///////////////////// Start transmitting beacons from coordinator ////////////////////////

    MlmeStartRequestParams params;
    params.m_panCoor = true;
    params.m_PanId = 5;
    params.m_bcnOrd = 14;
    params.m_sfrmOrd = 6;
    Simulator::ScheduleWithContext(1,
                                   Seconds(2.0),
                                   &LrWpanMac::MlmeStartRequest,
                                   dev0_PAN_C->GetMac(),
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

    // MCPS-DATA.request Beacon enabled Direct Transmission (dev1_device)
    // Frame transmission from End Device to Coordinator (Direct transmission)
    Simulator::ScheduleWithContext(1,
                                   Seconds(2.93),
                                   &LrWpanMac::McpsDataRequest,
                                   dev1_device->GetMac(),
                                   params2,
                                   p1);

    Simulator::Stop(Seconds(600));
    Simulator::Run();

    Simulator::Destroy();
    return 0;
}

