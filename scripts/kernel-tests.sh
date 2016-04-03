#!/bin/sh

if [ -z $1 ]; then
	echo "You must provide a test name."
	exit -1
fi

TEST=$1
REPO="/home/Renault/Logiciels/kernel-tests"
CURRENT_VERSION=$(uname -r)

# Check if wanted test has been executed with current kernel version
for file in $REPO/logs/*; do
	VERSION_TESTED=$(cat $file | sed -n 3p | cut -d ' ' -f 2)
	TEST_EXECUTED=$(cat $file | sed -n 2p | cut -d ' ' -f 3)

	if [ "$VERSION_TESTED" == "$CURRENT_VERSION" -a "$TEST_EXECUTED" == "$TEST" ]; then
		echo "The current kernel was already tested with this test, abort."
		exit 0
	fi
done

cd $REPO
./runtests.sh -t $TEST
