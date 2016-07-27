#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DataCenter2");

int main (int argc, char *argv[])
{
	bool verbose = true;

	if (verbose)
	{
		LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
		LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
	}
 
 	
	//Construct the upper Network coragg1: core->aggregation1 coragg2:core->aggregation2
	//ptpNodes
	NodeContainer coragg1;
	coragg1.Create(2);

	NodeContainer coragg2;
	coragg2.Add(coragg1.Get(0));
	coragg2.Create(1);


	//Construct the middle Network aggrtoR1: aggregation1->toRswitch1,aggrtoR2:aggregation2->toRswitch2
	//Consider preorder
	NodeContainer aggrtoR1,aggrtoR2;
	aggrtoR1.Add(coragg1.Get(1));
	aggrtoR1.Create(2);
	aggrtoR2.Add(coragg2.Get(1));
	aggrtoR2.Create(2);

	//Construct the lower Network:csmaNodes 
	NodeContainer csmaNodes1,csmaNodes2,csmaNodes3,csmaNodes4;

	csmaNodes1.Add(aggrtoR1.Get(1));
	csmaNodes1.Create(2);
	csmaNodes2.Add(aggrtoR1.Get(2));
	csmaNodes2.Create(2);
	csmaNodes3.Add(aggrtoR2.Get(1));
	csmaNodes3.Create(2);
	csmaNodes4.Add(aggrtoR2.Get(2));
	csmaNodes4.Create(2);
	


	//Construct ptp topo  upper
	PointToPointHelper ptp1,ptp2;
	ptp1.SetDeviceAttribute ("DataRate", StringValue ("1.5Mbps"));
	ptp1.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (500)));
	ptp2.SetDeviceAttribute ("DataRate", StringValue ("1.5Mbps"));
	ptp2.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (500)));
	
	NetDeviceContainer devicePtp1,devicePtp2;
	devicePtp1=ptp1.Install (coragg1);
	devicePtp2=ptp2.Install (coragg2);

	//Construct csma topo
	CsmaHelper csma;
	csma.SetChannelAttribute ("DataRate", StringValue ("1Mbps"));
	csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (500)));
	
	//middle topo
	NetDeviceContainer  deviceAggToR1,deviceAggToR2;
	deviceAggToR1=csma.Install (aggrtoR1);
	deviceAggToR2=csma.Install (aggrtoR2);

	//lower topo
	NetDeviceContainer deviceCsmaNodes1,deviceCsmaNodes2,deviceCsmaNodes3,deviceCsmaNodes4;
	deviceCsmaNodes1=csma.Install (csmaNodes1);
	deviceCsmaNodes2=csma.Install (csmaNodes2);
	deviceCsmaNodes3=csma.Install (csmaNodes3);
	deviceCsmaNodes4=csma.Install (csmaNodes4);

	//Install protocol stack
	InternetStackHelper stack;
	stack.Install (coragg1);
	stack.Install (coragg2.Get(1));
	stack.Install (aggrtoR1.Get(1));
	stack.Install (aggrtoR1.Get(2));
	stack.Install (aggrtoR2.Get(1));
	stack.Install (aggrtoR2.Get(2));
	stack.Install (csmaNodes1.Get(1));
	stack.Install (csmaNodes1.Get(2));
	stack.Install (csmaNodes2.Get(1));
	stack.Install (csmaNodes2.Get(2));
	stack.Install (csmaNodes3.Get(1));
	stack.Install (csmaNodes3.Get(2));
	stack.Install (csmaNodes4.Get(1));
	stack.Install (csmaNodes4.Get(2));

	//Allocate addresses on the network
	Ipv4AddressHelper address;

	address.SetBase ("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesPtp1 = address.Assign (devicePtp1);

	address.SetBase ("192.168.2.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesPtp2 = address.Assign (devicePtp2);

	address.SetBase ("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesAggToR1 = address.Assign (deviceAggToR1);

	address.SetBase ("10.2.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesAggToR2 = address.Assign (deviceAggToR2);

	address.SetBase ("10.0.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesCsma1 = address.Assign (deviceCsmaNodes1);

	address.SetBase ("10.0.2.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesCsma2 = address.Assign (deviceCsmaNodes2);

	address.SetBase ("10.0.3.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesCsma3 = address.Assign (deviceCsmaNodes3);

	address.SetBase ("10.0.4.0", "255.255.255.0");
	Ipv4InterfaceContainer interfacesCsma4 = address.Assign (deviceCsmaNodes4);


	//Set up many to one traffic
	//Set up n1 as a sinkApp	
	PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma1.GetAddress(1),8080));
	ApplicationContainer sinkApp = packetSinkHelper.Install(csmaNodes1.Get(1));
	
	sinkApp.Start(Seconds(0.0));
	sinkApp.Stop(Seconds(80.0));
  
  	csma.EnablePcap("N1 recieved packets", deviceCsmaNodes1.Get(1),true);




	//set up n2-->n1
	OnOffHelper client1("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma1.GetAddress(1), 8080));
	client1.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));	
	client1.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client1.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
	client1.SetAttribute ("PacketSize", UintegerValue (2000));  

	ApplicationContainer clientApp1 = client1.Install (csmaNodes1.Get(2));
	clientApp1.Start(Seconds (1.0 ));
	clientApp1.Stop (Seconds (21.0));

    csma.EnablePcap ("Pattern2 n2 to n1 ", deviceCsmaNodes1.Get (2), true);

	//set up n3-->n1
	OnOffHelper client2("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma1.GetAddress(1), 8080));
	client2.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client2.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client2.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
	client2.SetAttribute ("PacketSize", UintegerValue (2000));
	ApplicationContainer clientApp2 = client2.Install (csmaNodes2.Get(1));
	clientApp2.Start(Seconds (1.0 ));
	clientApp2.Stop (Seconds (21.0));

    csma.EnablePcap ("Pattern2 n3 to n1", deviceCsmaNodes2.Get (1), true);


	//Set up n4-->n1
	OnOffHelper client3("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma1.GetAddress(1), 8080));
	client3.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client3.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client3.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
	client3.SetAttribute ("PacketSize", UintegerValue (2000));
	
	ApplicationContainer clientApp3 = client3.Install (csmaNodes2.Get(2));
	clientApp3.Start(Seconds (1.0 ));
	clientApp3.Stop (Seconds (21.0));

    csma.EnablePcap ("Pattern2 n4 to n1", deviceCsmaNodes2.Get (2), true);

	//Set up n5-->n1
	OnOffHelper client4("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma1.GetAddress(1), 8080));
	client4.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client4.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client4.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
	client4.SetAttribute ("PacketSize", UintegerValue (2000));
	
	ApplicationContainer clientApp4 = client4.Install (csmaNodes3.Get(1));
	clientApp4.Start(Seconds (1.0 ));
	clientApp4.Stop (Seconds (21.0));

    csma.EnablePcap ("Pattern2 n5 to n1", deviceCsmaNodes3.Get (1), true);


	//Set up n6-->n1
	OnOffHelper client5("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma1.GetAddress(1), 8080));
	client5.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client5.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client5.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
	client5.SetAttribute ("PacketSize", UintegerValue (2000));
	
	ApplicationContainer clientApp5 = client5.Install (csmaNodes3.Get(2));
	clientApp5.Start(Seconds (1.0 ));
	clientApp5.Stop (Seconds (21.0));

    csma.EnablePcap ("Pattern2 n6 to n1", deviceCsmaNodes3.Get (2), true);

	//Set up n7-->n1
	OnOffHelper client6("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma1.GetAddress(1), 8080));
	client6.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client6.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client6.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
	client6.SetAttribute ("PacketSize", UintegerValue (2000));
	
	ApplicationContainer clientApp6 = client6.Install (csmaNodes4.Get(1));
	clientApp6.Start(Seconds (1.0 ));
	clientApp6.Stop (Seconds (21.0));

    csma.EnablePcap ("Pattern2 n7 to n1", deviceCsmaNodes4.Get (1), true);

	//Set up n8-->n1
	OnOffHelper client7("ns3::TcpSocketFactory", InetSocketAddress(interfacesCsma1.GetAddress(1), 8080));
	client7.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client7.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client7.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
	client7.SetAttribute ("PacketSize", UintegerValue (2000));
	
	ApplicationContainer clientApp7 = client7.Install (csmaNodes4.Get(2));
	clientApp7.Start(Seconds (1.0 ));
	clientApp7.Stop (Seconds (21.0));

    csma.EnablePcap ("Pattern2 n8 to n1", deviceCsmaNodes4.Get (2), true);


	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
	
	
	Simulator::Run ();
	Simulator::Destroy ();
	return 0;
} 
