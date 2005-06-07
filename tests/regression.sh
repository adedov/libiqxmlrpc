#!/bin/sh

LOG=regression_test.log
SERVER_LOG=regression_test_server.log
export BOOST_TEST_LOG_LEVEL=all

echo "+++ Libiqxmlrpc REGRESSION TEST +++" | tee $LOG
echo | tee -a $LOG

function exec_test
{
  echo "+++ RUNNING $1 +++" | tee -a $LOG
  . ./$* 2>&1 | tee -a $LOG
  echo "+++ END +++" | tee -a $LOG
  echo | tee -a $LOG
}

function start_server
{
  echo "+++ STARTING SERVER $* +++" | tee -a $LOG $SERVER_LOG
  ./$* >${SERVER_LOG} 2>&1 &
  echo | tee -a $LOG
}

start_server server-test 3344 1 
exec_test value-usage
exec_test client-test localhost 3344
exec_test stop-test-server localhost 3344
