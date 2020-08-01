#!/bin/sh

PATH="${WDIR}:/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin"

WDIR="/app/app"
EXT="chksum"

DN_FILE="IRO_App_DN"
WAIT_FILE="IRO_App_WAIT"

_CWD=`pwd`
cd $WDIR

_Compare_Cheksum()
{
    local _FFF=$1
    local _RE=1  # 1 is fail..

    if [ -f ${_FFF}.${EXT} ]; then
        FILE_CHKSUM=`cat ${_FFF}.${EXT} | awk '{ print $1 }'`
        CALC_CHKSUM=`sha1sum ${_FFF} | awk '{ print $1 }'`
    else
        FILE_CHKSUM="0"
        CALC_CHKSUM="1"
        echo "No checksum file ${_FFF}.${EXT}, and skip ..."
	  fi

    if [ "$FILE_CHKSUM" = "$CALC_CHKSUM" ]; then
        _RE=0
    else
        echo "File $_FFF checksum is not match !"
    fi

    return $_RE
}

if [ -x ${WAIT_FILE} ]; then
    _Compare_Cheksum ${WAIT_FILE}
    if [ $? != 0 ]; then       # fail
        cd $_CWD
        exit 1
    fi

    mv -f ${WAIT_FILE}        ${DN_FILE}
    mv -f ${WAIT_FILE}.${EXT} ${DN_FILE}.${EXT}
    
    sync  
fi

cd $_CWD

exit 0

# EOT
