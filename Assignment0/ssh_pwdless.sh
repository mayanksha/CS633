#!bin/bash

# Navigatge to HOME directory (if not already in)
cd ~
# Generate public/private RSA key
echo "StrictHostKeyChecking no" >> config
# Reset all hosts to connect password-less henceforth
rm -f known_hosts*
# Add all nodes in cluster to known_hosts
for i in `seq 1 120`
do
ssh csews$i uptime
done
