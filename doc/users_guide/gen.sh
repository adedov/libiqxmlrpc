#!/bin/sh
deplate -f htmlslides \
	-t html-tabbar-right.html \
	-m html-deplate-button \
	--css tabbar-right --css deplate \
	-d html \
	index.txt
