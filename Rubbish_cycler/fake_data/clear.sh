#!/usr/bin/sh
rm ./mu_root/* ./su_root/* -rf
mysql -uroot -pmysql -e"drop database su_test"
mysql -uroot -pmysql -e"create database su_test"
clear
