#!/usr/bin/env bash
TF_GATEWAYCODE="yucXzY65deV86pIh"
TF_GATEWAYIP="192.168.8.181"
TF_PSKFILE="psk.conf"

set_dim() {
  local val;
  if [ $2 -gt 254 ]; then
    val=254;
    echo "error input out of range: brightness 0-254"
  else
    val=$2;
  fi
  coap-client -m put -u "$TF_USERNAME" -k "$TF_PRESHARED_KEY" -e '{ "3311": [{ "5851": '$val' }] }' "coaps://$TF_GATEWAYIP:5684/15001/$1" 2>/dev/null
}

#logging in
if [ ! -f "$TF_PSKFILE" ];
then
  TF_USERNAME=$(pwgen 16 -n 1)
  TF_PRESHARED_KEY=$(coap-client -m post -u "Client_identity" -k "$TF_GATEWAYCODE" -e "{\"9090\":\"$TF_USERNAME\"}" "coaps://$TF_GATEWAYIP:5684/15011/9063" 2>/dev/null | jq -r '."9091"')

  echo '{ "identity": "'$TF_USERNAME'", "key": "'$TF_PRESHARED_KEY'" }' > $TF_PSKFILE;
else
  TF_USERNAME=$(cat "$TF_PSKFILE" | jq -r '.identity')
  TF_PRESHARED_KEY=$(cat "$TF_PSKFILE" | jq -r '.key')
fi

#DEVICES=$(coap-client -m get -u "$TF_USERNAME" -k "$TF_PRESHARED_KEY" "coaps://$TF_GATEWAYIP:5684/15001" 2>/dev/null | jq -c ".[]")
#for row in $DEVICES; do
#  echo $row
#done

while true
do
    mosquitto_sub -h mqtt.devlol.org -t "doebi/zigvader/in" | while read -r payload
    do
      #echo "Rx MQTT: ${payload * 254}"
      v=$(echo "$payload * 254" | bc)
      vv=$(/usr/bin/printf "%.0f\n" $v)
      set_dim 65540 $vv
    done
    sleep 10
done
