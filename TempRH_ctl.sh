#!/bin/sh

set -e

stop_i2clcd () {
    kill `ps ax | grep "python /usr/pi/bin/i2clcdb.py" | awk '{print $1}'` || :
}

# get to the information of templature and RH
/usr/pi/bin/dht11_get_temp_RH
RH=`cat /var/pi/log/dht.txt | awk '{print $3}'`
TEMP=`cat /var/pi/log/dht.txt | awk '{print $7}'`
cat /var/pi/log/dht.txt >> /var/pi/log/static_Temp_RH.txt

# display information to monitor
stop_i2clcd
python /usr/pi/bin/reset_i2clcdb.py
python /usr/pi/bin/i2clcdb.py ${TEMP} ${RH} &


