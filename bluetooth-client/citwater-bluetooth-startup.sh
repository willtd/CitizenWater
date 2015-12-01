#!/bin/sh

BLUETOOTH_CLIENT_CHANNEL=22


/usr/sbin/rfkill unblock bluetooth \
&& sleep 5 \
&& hciconfig hci0 up \
&& hciconfig hci0 piscan \
&& hciconfig hci0 sspmode 0 \
&& sdptool add --channel=$BLUETOOTH_CLIENT_CHANNEL SP \
&& /opt/citwater/simple-agent.py
