#!/bin/sh

LOG=regression_test.log
SERVER_LOG=regression_test_server.log
export BOOST_TEST_LOG_LEVEL=all

echo "+++ Libiqxmlrpc REGRESSION TEST +++" | tee $LOG $SERVER_LOG
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
  echo | tee -a ${SERVER_LOG}
  echo "+++ STARTING SERVER $* +++" | tee -a $LOG $SERVER_LOG
  ./$* >>${SERVER_LOG} 2>&1 &
  echo | tee -a $LOG
}

exec_test value-usage

# no-ssl, single thread
start_server server-test 3344 1
exec_test client-test --host=localhost --port=3344
exec_test client-stress-test --host=localhost --port=3344 \
  --client-threads=5 --calls-per-thread=2 --stop=yes

# no-ssl, 3 threads, stress only
start_server server-test 3344 3
exec_test client-stress-test --host=localhost --port=3344 \
  --client-threads=5 --calls-per-thread=2 --stop=yes

# ssl, single thread
start_server server-test 3344 1 1
exec_test client-test --host=localhost --port=3344 --use-ssl=yes
exec_test client-stress-test --host=localhost --port=3344 --use-ssl=yes \
  --client-threads=5 --calls-per-thread=2 --stop=yes

# ssl, 3 threads, stress only
start_server server-test 3344 3 1
exec_test client-stress-test --host=localhost --port=3344 --use-ssl=yes \
  --client-threads=5 --calls-per-thread=2 --stop=yes
