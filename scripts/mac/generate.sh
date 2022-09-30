#!/bin/bash

function pause(){
   read -p "$*"
}

cd ../../bin
./premake5 --file=../premake5.lua xcode4
pause 'Press [Enter] key to continue...'