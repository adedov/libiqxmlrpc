#!/bin/sh
tailor --verbose --source-kind cvs --target-kind bzr \
--repository ../libiqxmlrpc-cvs \
--module libiqxmlrpc --revision INITIAL \
--target-repository . \
--target-module / libiqxmlrpc > libiqxmlrpc.tailor
