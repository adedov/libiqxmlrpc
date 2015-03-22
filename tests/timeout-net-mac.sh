sudo ipfw add pipe 1 ip from any to any 3345
sudo ipfw add pipe 2 ip from any 3345 to any
sudo ipfw pipe 1 config delay 1500ms bw 1Mbit/s plr 0.1
sudo ipfw pipe 2 config delay 1500ms bw 1Mbit/s plr 0.1
