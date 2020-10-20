#!/bin/bash
echo 192.168.2.2 > ip.txt
for ip in 192.168.2.{1..254}; do
(sudo ping -c 1 $ip | grep -n "64 bytes" | grep -v "192.168.2.2" | awk '{print $4}' | cut -d":" -f1 &) >> ip.txt
done
