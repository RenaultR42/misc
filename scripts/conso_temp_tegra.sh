#!/bin/sh

# Script to monitor consumption information and temperature on Tegra K1 Jetson board.

# The TMP_FILE is a i2cdump result to display in one "i2c dump request" all needed values.
TMP_FILE="i2cdump.tmp"
I2C_BUS=4
I2C_ADDRESS=0x40

# Parse i2cdump result: select the line then the column
get_pmic_reg() {
	cat $TMP_FILE | grep ^$1 | cut -d ' ' -f $($2+2)
}

while true
do
	i2cdump -f -y $I2C_BUS $I2C_ADRESS > $TMP_FILE

	# Get some PMIC registers
	for i in $(seq 0x0 0xB)
	do
		LDO_VALUE=$(get_pmic_reg 1 $i)
		echo "LDO$i Voltage: 0x$LDO_VALUE"
	done

	RES_VOLT_RISE_VALUE=$(get_pmic_reg a 9)
	echo "Fuse 9 (ResVoltRise): 0x$RES_VOLT_RISE_VALUE"

	# Print all temperatures available (with associated names)
	for i in /sys/devices/virtual/thermal/thermal_zone*
	do
		TYPE=$(cat $i/type)
		TEMP=$(cat $i/temp)
		TEMP_FORMAT=$(awk "BEGIN {print $TEMP/1000}")
		echo "$TYPE: $TEMP_FORMAT°C"
	done

	rm $TMP_FILE
	sleep 1
done
