#!/usr/bin/env bash
TF_GATEWAYCODE="jvw5BaHbnbt5JCHH"
TF_GATEWAYIP="192.168.8.107"
TF_PSKFILE="psk.conf"

set_dim() {
  local val;
  if [ $2 -gt 254 ]; then
    val=254;
    echo "error input out of range: brightness 0-254"
  else
    val=$2;
  fi
  echo "dimming $1 to $2"
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

DEVICES=$(coap-client -m get -u "$TF_USERNAME" -k "$TF_PRESHARED_KEY" "coaps://$TF_GATEWAYIP:5684/15001" 2>/dev/null | jq -c ".[]")
for row in $DEVICES; do
  set_dim $row $1
done
