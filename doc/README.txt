Nodule - ground.
CE(n_rf24l01) - J4(IO-Port#1), 1-st pin

To figure out how to connect n_rf24l01 board to odroid look at:
 - http://odroid.com/dokuwiki/doku.php?id=en:u3_ioport_spi
 - http://odroid.com/dokuwiki/doku.php?id=en:u3_hardware
 - http://wiki.iteadstudio.com/NRF24L01_Module
 - https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
 
 
1. Download and flash Ubuntu OS image to Odroid eMMC card:
    http://odroid.com/dokuwiki/doku.php?id=en:odroid-u3
  
2. Turn on hotspot on host machine:
    host@ > nm-connection-editor
    host@ > nmcli c
    host@ > sudo nmcli c up <saved_wifi_connection>
    
3. Promt Odroid via serial port:
    host@ > minicom
  
4. Connect to hotspot from Odroid:
    (odroid) root@ > ifconfig -a 
    (odroid) root@ > nmcli d wifi connect <wifi_ssid> password <password> iface <Odroid wifi iface> [name <some pretty name>]
  
5. Fix /etc/ssh/sshd_config file on Odroid: (optional)
    PermitRootLogin no
    PasswordAuthentication no
    
6. Restart sshd: (optional)
    (odroid) root@ > service ssh restart
    
6. Generate rsa pair on host and copy *.pub one to Odroid:
    host@ > ssh-keygen
    host@ > ssh-copy-id -i <name.pub> odroid@<Odroid wifi iface ip>

7. Promt Odroid via ssh:
    host@ > ssh odroid@<Odroid wifi iface ip>
    
    
    Tips:

1. Add kernel module under modprobe 'eyes':
   - create link to your module in /lib/modules/`uname -r` 
   (for odroid: /lib/modules/3.8.13.30/kernel/drivers/n_rf24l01/n_rf24l01.ko -> home/odroid/n_rf24l01/n_rf24l01.ko)
   - depmod -a

2. To use notebook as hotspot (in Ubuntu 14.04):
   - turn on wi-fi on host (notebook)
   - type sudo ap-hotspot start
   - turn on wi-fi on device (odroid, Galaxy A5)
   - in several seconds you will see new device discovering
   - use...
  
   If after `sudo ap-hotspot start`, you see "Another process is already running" type:
   - `sudo rm /tmp/hotspot.pid`
   
2.1. To use notebook as hotspot (Fedora 25)
   - sudo nm-connection-editor

3. To connect to wifi network from odroid board:
   
   - just edit /etc/network/interfaces and write:
      auto wlan2
      iface wlan2 inet dhcp 
      wpa-ssid {ssid}
      wpa-psk  {password}

   - after that write and close file and use command:
      sudo dhclient wlan0

   - replace {ssid} and {password} with your respective WiFi SSID and password.
  
4. To connect to odroid-u3 over tty:
   - sudo minicom -s -- (for setup)
   - sudo minicom    -- (after setup) 
   
5. To debug linux kernel:
   -kdmx (http://elinux.org/Kdmx)
   ( 
     change target console baundrate (e.g. change linux boot arguments),
     preferred value - 115200
     [host]> ./kdmx -n -d -p /dev/ttyUSB0 -b <target console baundrate>
     [host]> minicom -o -w -p /dev/pts/<slave pty for terminal emulator>  
     [target]> echo ttySAC1 > /sys/module/kgdboc/parameters/kgdboc 
     [target]> echo g > /proc/sysrq-trigger 
     [host]> gdb (target remote <slave pty for gdb> 
   )
     
6. To write fimeware to stm32-discovery: 
   - st-flash write v1 radio_car.bin 0x8000000
 
7. 
   Recreate (with previous wifi_ssid and password) the hotspot connection on host (Fedora) before any
   devices have attempts to connect.