#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/tcp-l4-protocol.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DataCenter3");

int main (int argc, char *argv[])
{
	bool verbose = true;

	if (verbose)
	{
		LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
		LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
	}


	//Construct the upper Network core1 = coragg11.get(0) core2 = coragg21.get(0)
	//ptpNodes 
	NodeContainer coragg11,coragg12,coragg13,coragg14;
	coragg11.Create(2);
	coragg12.Add(coragg11.Get(0));
	coragg12.Create(1);
	coragg13.Add(coragg11.Get(0));
	coragg13.Create(1);
	coragg14.Add(coragg11.Get(0));
	coragg14.Create(1);

	NodeContainer coragg21,coragg22,coragg23,coragg24;
	coragg21.Create(1);
	coragg21.Add(coragg11.Get(1));
	coragg22.Add(coragg21.Get(0));
	coragg22.Add(coragg12.Get(1));
	coragg23.Add(coragg21.Get(0));
	coragg23.Add(coragg13.Get(1));
	coragg24.Add(coragg21.Get(0));
	coragg24.Add(coragg14.Get(1));

	//Construct the middle Network agg1 = coragg11.get(1)
	//Consider preorder
	NodeContainer aggrtoR11,aggrtoR12,aggrtoR21,aggrtoR22;
	aggrtoR11.Add(coragg11.Get(1));
	aggrtoR11.Create(1);
	aggrtoR12.Add(coragg11.Get(1));
	aggrtoR12.Create(1);
	aggrtoR21.Add(coragg12.Get(1));
	aggrtoR21.Add(aggrtoR11.Get(1));
	aggrtoR22.Add(coragg12.Get(1));
	aggrtoR22.Add(aggrtoR12.Get(1));

	NodeContainer aggrtoR33,aggrtoR34,aggrtoR43,aggrtoR44;
	aggrtoR33.Add(coragg13.Get(1));
	aggrtoR33.Create(1);
	aggrtoR34.Add(coragg13.Get(1));
	aggrtoR34.Create(1);
	aggrtoR43.Add(coragg14.Get(1));
	aggrtoR43.Add(aggrtoR33.Get(1));
	aggrtoR44.Add(coragg14.Get(1));
	aggrtoR44.Add(aggrtoR34.Get(1));

	//Construct the lower Network:csmaNodes 
	NodeContainer csmaNodes1,csmaNodes2,csmaNodes3,csmaNodes4;

	csmaNodes1.Add(aggrtoR11.Get(1));
	csmaNodes1.Create(2);
	csmaNodes2.Add(aggrtoR12.Get(1));
	csmaNodes2.Create(2);
	csmaNodes3.Add(aggrtoR33.Get(1));
	csmaNodes3.Create(2);
	csmaNodes4.Add(aggrtoR34.Get(1));
	csmaNodes4.Create(2);
	
	//Construct ptp topo  upper
	PointToPointHelper ptp;
	ptp.SetDeviceAttribute ("DataRate", StringValue ("1.5Mbps"));
	ptp.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (500)));
	
	NetDeviceContainer devicePtp11,devicePtp12,devicePtp13,devicePtp14,devicePtp21,devicePtp22,devicePtp23,devicePtp24;
	devicePtp11=ptp.Install (coragg11);
	devicePtp12=ptp.Install (coragg12);
	devicePtp13=ptp.Install (coragg13);
	devicePtp14=ptp.Install (coragg14);
	devicePtp21=ptp.Install (coragg21);
	devicePtp22=ptp.Install (coragg22);
	devicePtp23=ptp.Install (coragg23);
	devicePtp24=ptp.Install (coragg24);

	//Construct csma topo
	CsmaHelper csma;
	csma.SetChannelAttribute ("DataRate", StringValue ("1Mbps"));
	csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (500)));

	//middle topo
	NetDeviceContainer  deviceAggToR11,deviceAggToR12,deviceAggToR21,deviceAggToR22,deviceAggToR33,deviceAggToR34,deviceAggToR43,deviceAggToR44;
	deviceAggToR11=csma.Install (aggrtoR11);
	deviceAggToR12=csma.Install (aggrtoR12);
	deviceAggToR21=csma.Install (aggrtoR21);
	deviceAggToR22=csma.Install (aggrtoR22);
	deviceAggToR33=csma.Install (aggrtoR33);
	deviceAggToR34=csma.Install (aggrtoR34);
	deviceAggToR43=csma.Install (aggrtoR43);
	deviceAggToR44=csma.Install (aggrtoR44);

	//lower topo
	NetDeviceContainer deviceCsmaNodes1,deviceCsmaNodes2,deviceCsmaNodes3,deviceCsmaNodes4;
	deviceCsmaNodes1=csma.Install (csmaNodes1);
	deviceCsmaNodes2=csma.Install (csmaNodes2);
	deviceCsmaNodes3=csma.Install (csmaNodes3);
	deviceCsmaNodes4=csma.Install (csmaNodes4);

	//Install protocol stack
	InternetStackHelper stack;
	stack.Install (coragg11);
	stack.Install (coragg12.Get(1));
	stack.Install (coragg13.Get(1));
	stack.Install (coragg24);

	stack.Install (csmaNodes1);
	stack.Install (csmaNodes2);
	stack.Install (csmaNodes3);
	stack.Install (csmaNodes4);

	//Allocate addresses on the network
	Ipv4AddressHelper address;

	address.SetBase ("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesPtp11 = address.Assign (devicePtp11);

	address.SetBase ("192.168.2.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesPtp12 = address.Assign (devicePtp12);

	address.SetBase ("192.168.3.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesPtp13 = address.Assign (devicePtp13);

	address.SetBase ("192.168.4.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesPtp14 = address.Assign (devicePtp14);

	address.SetBase ("192.168.5.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesPtp21 = address.Assign (devicePtp21);

	address.SetBase ("192.168.6.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesPtp22 = address.Assign (devicePtp22);

	address.SetBase ("192.168.7.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesPtp23 = address.Assign (devicePtp23);

	address.SetBase ("192.168.8.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesPtp24 = address.Assign (devicePtp24);

	address.SetBase ("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesAggToR11 = address.Assign (deviceAggToR11);

	address.SetBase ("10.2.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesAggToR12 = address.Assign (deviceAggToR12);

	address.SetBase ("10.3.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesAggToR21 = address.Assign (deviceAggToR21);

	address.SetBase ("10.4.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesAggToR22 = address.Assign (deviceAggToR22);

	address.SetBase ("10.5.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesAggToR33 = address.Assign (deviceAggToR33);

	address.SetBase ("10.6.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesAggToR34 = address.Assign (deviceAggToR34);

	address.SetBase ("10.7.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesAggToR43 = address.Assign (deviceAggToR43);

	address.SetBase ("10.8.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesAggToR44 = address.Assign (deviceAggToR44);

	address.SetBase ("10.0.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesCsma1 = address.Assign (deviceCsmaNodes1);

	address.SetBase ("10.0.2.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesCsma2 = address.Assign (deviceCsmaNodes2);

	address.SetBase ("10.0.3.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesCsma3 = address.Assign (deviceCsmaNodes3);

	address.SetBase ("10.0.4.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesCsma4 = address.Assign (deviceCsmaNodes4);

	//like mydc1, set up traffic for n1 to n5, n6 to n2,n3 to n7 and n8 to n4
	//Set up traffic n1 -- n5	
	PacketSinkHelper packetSinkHelper1("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma3.GetAddress(1),8080));
	ApplicationContainer sinkApp1 = packetSinkHelper1.Install(csmaNodes3.Get(1));
	
	sinkApp1.Start(Seconds(0.0));
	sinkApp1.Stop(Seconds(20.0));
  
	OnOffHelper client1("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma3.GetAddress(1), 8080));
	client1.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));	
	client1.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client1.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
	client1.SetAttribute ("PacketSize", UintegerValue (2000));  

	ApplicationContainer clientApp1 = client1.Install (csmaNodes1.Get(1));
	clientApp1.Start(Seconds (1.0 ));
	clientApp1.Stop (Seconds (21.0));

        csma.EnablePcap ("Pattern3 n3 to n5", deviceCsmaNodes3.Get (1), true);

	//Set up traffic n6 -- n2
	PacketSinkHelper packetSinkHelper2("ns3::TcpSocketFactory",InetSocketAddress(interfacesCsma1.GetAddress(2),8081));		
	ApplicationContainer sinkApp2 = packetSinkHelper2.Install(csmaNodes1.Get(2));
	sinkApp2.Start(Seconds(0.0));
	sinkApp2.Stop(Seconds(20.0));
  
	OnOffHelper client2("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma1.GetAddress(2), 8081));
	client2.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client2.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client2.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
	client2.SetAttribute ("PacketSize", UintegerValue (2000));
	ApplicationContainer clientApp2 = client2.Install (csmaNodes3.Get(2));
	clientApp2.Start(Seconds (1.0 ));
	clientApp2.Stop (Seconds (21.0));

        csma.EnablePcap ("Pattern3 n6 to n2", deviceCsmaNodes1.Get (2), true);

	//Set up traffic n3 -- n7
	PacketSinkHelper packetSinkHelper3("ns3::TcpSocketFactory",InetSocketAddress(interfacesCsma4.GetAddress(1),8082));
	ApplicationContainer sinkApp3 = packetSinkHelper3.Install(csmaNodes4.Get(1));
	sinkApp3.Start(Seconds(0.0));
	sinkApp3.Stop(Seconds(20.0));
  
	OnOffHelper client3("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma4.GetAddress(1), 8082));
	client3.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client3.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client3.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
	client3.SetAttribute ("PacketSize", UintegerValue (2000));
	
	ApplicationContainer clientApp3 = client3.Install (csmaNodes2.Get(1));
	clientApp3.Start(Seconds (1.0 ));
	clientApp3.Stop (Seconds (21.0));

	csma.EnablePcap ("Pattern3 n3 to n7", deviceCsmaNodes4.Get (1), true);

	//Set up traffic n8 -- n4
	PacketSinkHelper packetSinkHelper4("ns3::TcpSocketFactory",InetSocketAddress(interfacesCsma2.GetAddress(2),8083));
	ApplicationContainer sinkApp4 = packetSinkHelper4.Install(csmaNodes4.Get(2));
	sinkApp4.Start(Seconds(0.0));
	sinkApp4.Stop(Seconds(20.0));
  
	OnOffHelper client4("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma2.GetAddress(2), 8083));
	client4.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client4.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client4.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
	client4.SetAttribute ("PacketSize", UintegerValue (2000));
	
	ApplicationContainer clientApp4 = client4.Install (csmaNodes4.Get(2));
	clientApp4.Start(Seconds (1.0 ));
	clientApp4.Stop (Seconds (21.0));

	csma.EnablePcap ("Pattern3 n8 to n4", deviceCsmaNodes2.Get (2), true);

	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	Simulator::Run ();
	Simulator::Destroy ();
	return 0;
} 
