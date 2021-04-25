# Temp Monitoring
Solar powered external esp8266 with dht11 recording outside temperature into hassio.

esp8266 runs every hour, takes temperature reading. sends it up to cloud MQTT. hassio is subscribed to same mqtt and "saves" it to influxdb.



