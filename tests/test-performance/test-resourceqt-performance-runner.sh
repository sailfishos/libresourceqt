#!/bin/sh

# Source and export D-Bus session info 
. /tmp/session_bus_address.user 

# D-Bus spamming is default option
nospam="0"
spamparam="-t10 -s50"

if [ $# -eq 0 ]
then
	echo "Usage: test-resourceqt-performance-runner.sh test_case_nr"
	exit 2
fi

case "$1" in
	0)
	export TEST_DESC="Testcase without dbus spamming"
	export TEST_NAME="no dbus load"
	nospam="1"
	collectparam="--create"
	;;
	1)
	export TEST_DESC="Testcase with 5 dbus spamming threads, message size 50 bytes"
	export TEST_NAME="small dbus load"
	spamparam="-t5 -s50"
	;;
	2)
	export TEST_DESC="Testcase with 10 dbus spamming threads, message size 50 bytes"
	export TEST_NAME="medium dbus load"
	spamparam="-t10 -s50"
	;;
	3)
	export TEST_DESC="Testcase with 20 dbus spamming threads, message size 50 bytes"
	export TEST_NAME="heavy dbus load"
	spamparam="-t20 -s50"
	;;
	4)
	export TEST_DESC="Testcase with 20 dbus spamming threads, message size 150 bytes"
	export TEST_NAME="very heavy dbus load"
	spamparam="-t20 -s150"
	collectparam="--finalize"
	;;
	*)
	echo "Usage: test-resourceqt-performance-runner.sh test_case_nr"
	exit 2
	;;
esac

# Run spam server
if [ "$nospam" != "1" ]
then
  /usr/lib/libresourceqt-performance-tests/dbus-spammer $spamparam &
  if [ $? -ne 0 ]
  then
    exit 3;
  fi
  sleep 5
fi

# Check the speed!
/usr/lib/libresourceqt-performance-tests/test-resourceqt-performance
status=$?

# Kill spammer
if [ "$nospam" != "1" ]
then
  killall dbus-spammer
fi

# Collect the results
/usr/lib/libresourceqt-performance-tests/test-resourceqt-performance-collector $collectparam

if [ "$status" -ne 0 ]
then
  echo "Test failed with value $status"
fi

# Return number of failed acquire/release cycles, nothing should be failing... 
exit $status
