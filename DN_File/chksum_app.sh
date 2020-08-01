#!/bin/sh

PATH="${WDIR}:/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin"

WDIR="/app/app"
EXT="chksum"

DN_FILE="IRO_App_DN"
APP1_FILE="IRO_App_1"
APP2_FILE="IRO_App_2"

#####################
if [ -f /app/etc/bftpd.conf ]; then
	/etc/init.d/bftp stop
	cp -f /app/etc/bftpd.conf /etc/bftpd.conf
	if [ ! -d /app/unZip ]; then
		mkdir /app/unZip
	fi
	if [ ! -d /app/ftp ]; then
		mkdir /app/ftp
	fi
	/etc/init.d/bftp start
fi	
#####################
if [ ! -d /app/ftp/log ]; then
	mkdir /app/ftp/log
	if [ ! -d /app/ftp/log/alarm ]; then	
	mkdir /app/ftp/log/alarm
	fi
	
	if [ ! -d /app/ftp/log/evcmd ]; then	
	mkdir /app/ftp/log/evcmd
	fi
	
	if [ ! -d /app/ftp/log/evrst ]; then	
	mkdir /app/ftp/log/evrst
	fi	
fi
#####################
if [ ! -d /app/ftp/fw ]; then
	mkdir /app/ftp/fw
fi
#####################
if [ ! -d /app/ftp/zip ]; then
	mkdir /app/ftp/zip
fi
#####################
if [ ! -d /app/ftp/zip_old ]; then
	mkdir /app/ftp/zip_old
fi
#####################

if [ ! -d $WDIR ]; then
    echo "No working dir $WDIR ..."
    exit 1
fi

_CWD=`pwd`
cd $WDIR

_Check_Cheksum()
{
    local _FFF=$1
    local _RE=1

    if [ -f ${_FFF}.${EXT} ]; then
        FILE_CHKSUM=`cat ${_FFF}.${EXT} | awk '{ print $1 }'`
        CALC_CHKSUM=`sha1sum ${_FFF} | awk '{ print $1 }'`
    else
        FILE_CHKSUM="none"
        CALC_CHKSUM="none"
        echo "No checksum file ${_FFF}.${EXT}, and skip ..."
	fi

    if [ "$FILE_CHKSUM" = "$CALC_CHKSUM" ]; then
        _RE=0
    else
        echo "File $_FFF checksum is not match !"
    fi

    return $_RE
}

if [ -x ${DN_FILE} ]; then
    _Check_Cheksum ${DN_FILE}
    if [ $? != 0 ]; then
        cd $_CWD
        exit 1
    fi

    ${DN_FILE} test

    if [ $? = 0 ]; then
        echo "OK!"
        mv -f ${DN_FILE} ${APP1_FILE}
        if [ -f ${DN_FILE}.${EXT} ]; then
            mv -f ${DN_FILE}.${EXT} ${APP1_FILE}.${EXT}
        elif [ -f ${APP1_FILE}.${EXT} ]; then
            rm -f ${APP1_FILE}.${EXT}
        fi

        cp ${APP1_FILE} ${APP2_FILE}
        if [ -f ${APP1_FILE}.${EXT} ]; then
            cp ${APP1_FILE}.${EXT} ${APP2_FILE}.${EXT}
        elif [ -f ${APP2_FILE}.${EXT} ]; then
            rm -f ${APP2_FILE}.${EXT}
        fi
        sync
    else
        echo "Fail!"
        cd $_CWD
        exit 1
    fi
fi

INVALID=1

echo "/app/app/core" > /proc/sys/kernel/core_pattern
ulimit -c unlimited

_Check_Cheksum ${APP1_FILE}
if [ $? = 0 ]; then
    ${APP1_FILE} test
    INVALID=$?
fi

if [ "$INVALID" = "0" ]; then
    ${APP1_FILE} daemon
else
    _Check_Cheksum ${APP2_FILE}
    if [ $? = 0 ]; then
        ${APP2_FILE} daemon
    fi
fi

cd $_CWD

exit 0

# EOT
