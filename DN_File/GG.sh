#!/bin/sh

HHH="192.168.1.150"

UUU="user"
PPP="user.peta"

GetFile()
{
  local _PATH=$1
  local _FILE=$2

  if [ ! -z $_PATH -a ! -z $_FILE ]; then
    echo "ftp://${HHH}/${_PATH}/${_FILE}"
    ftpget -u $UUU -p $PPP $HHH $_FILE ${_PATH}/${_FILE}
    sync
  else
    echo "usage: GG.sh remote_path file_name"
  fi
  chmod +x $_FILE
}

GetFile Share/folder test

# EOF
