#!/bin/sh

DN_COPY_VERSION="200713.02"

OLD_WORKING_DIR=$(pwd)
NEW_WORKING_DIR=$(dirname $0)

#######################################################################
#######################################################################

umount_p1_p2()
{
    umount /mnt/p1
    umount /mnt/p2
}

PRU_mount_p1_p2()
{
    local _RE=0

    if [ ! -d /mnt/p1 ]; then
        mkdir /mnt/p1
    fi
    if [ ! -d /mnt/p2 ]; then
        mkdir /mnt/p2
    fi

    ######################################################### mount p1

    fsck.fat -a /dev/mmcblk0p1 > /dev/null 2>&1
    _RE=$?
    if [ $_RE != 0 ]; then
        fsck.fat -a /dev/mmcblk0p1 > /dev/null 2>&1
        _RE=$?
    fi
    if [ $_RE != 0 ]; then
        return 1
    fi
    mount -t vfat /dev/mmcblk0p1 /mnt/p1 > /dev/null 2>&1
    if [ $? != 0 ]; then
        return 1
    fi

    ######################################################### mount p2

    fsck.ext3 -f -y /dev/mmcblk0p2 > /dev/null 2>&1
    _RE=$?
    if [ $_RE != 0 ]; then
        fsck.ext3 -f -y /dev/mmcblk0p2 > /dev/null 2>&1
        _RE=$?
    fi
    if [ $_RE != 0 ]; then
        return 1
    fi
    mount -t ext3 /dev/mmcblk0p2 /mnt/p2 > /dev/null 2>&1
    if [ $? != 0 ]; then
        umount /mnt/p1
        return 1
    fi

    return 0
}

#######################################################################

PRU_install_BOOT_BIN()
{
    if [ ! -f /mnt/p1/BOOT.BIN ]; then
        echo "[dn_copy] NEW STYLE FILE SYSTEM is not supported ..."
        return 2
    fi

	echo "[dn_copy] PRU, Install BOOT.BIN ..."

    if [ -f /mnt/p1/BOOT.BIN.old ]; then
        \rm -f /mnt/p1/BOOT.BIN.old
    fi
    cp -f /mnt/p1/BOOT.BIN /mnt/p1/BOOT.BIN.old
    cp -f BOOT.BIN /mnt/p1/BOOT.BIN

    sync

    if [ -f /mnt/p2/BOOT.BIN.old ]; then
        \rm -f /mnt/p2/BOOT.BIN.old
    fi
    cp -f /mnt/p2/BOOT.BIN /mnt/p2/BOOT.BIN.old
    cp -f BOOT.BIN /mnt/p2/BOOT.BIN

    sync

    return 0
}

PRU_install_image_ub()
{
    if [ ! -f /mnt/p2/image.ub ]; then
        return 1
    fi

    if [ ! -f /mnt/p2/BOOT.BIN ]; then
        echo "[dn_copy] NEW STYLE FILE SYSTEM is not supported"
        return 2
    fi

	echo "[dn_copy] PRU, Install image.ub ..."

    if [ -f /usr/app_dft/images/image.ub ]; then
        \rm -f /usr/app_dft/images/image.ub
    fi

    if [ -f /mnt/p2/image.ub.old ]; then
        \rm -f /mnt/p2/image.ub.old
    fi
    cp -f /mnt/p2/image.ub /mnt/p2/image.ub.old
    cp -f image.ub /mnt/p2/image.ub

    sync

    return 0
}

PRU_check_and_install_images()
{
    local _RE

    if [ -f BOOT.BIN -o -f image.ub ]; then
        PRU_mount_p1_p2
    else
        echo "[dn_copy] NO BOOT.BIN or image.ub ..."
        return 1
    fi

    if [ -f BOOT.BIN ]; then
        PRU_install_BOOT_BIN
        _RE=$?
        if [ $_RE != 0 ]; then
            umount_p1_p2
            return $_RE
        fi
    fi

    if [ -f image.ub ]; then
        PRU_install_image_ub
        _RE=$?
        if [ $_RE != 0 ]; then
            umount_p1_p2
            return $_RE
        fi
    fi

    umount_p1_p2

    return 0
}

#######################################################################
#######################################################################

AAU_mount_p1_p2()
{
    local _RE=0

    if [ ! -d /mnt/p1 ]; then
        mkdir /mnt/p1
    fi
    if [ ! -d /mnt/p2 ]; then
        mkdir /mnt/p2
    fi

    ######################################################### mount p1

    fsck.fat -a /dev/mmcblk1p1 > /dev/null 2>&1
    _RE=$?
    if [ $_RE != 0 ]; then
        fsck.fat -a /dev/mmcblk1p1 > /dev/null 2>&1
        _RE=$?
    fi
    if [ $_RE != 0 ]; then
        return 1
    fi
    mount -t vfat /dev/mmcblk1p1 /mnt/p1 > /dev/null 2>&1
    if [ $? != 0 ]; then
        return 1
    fi

    ######################################################### mount p2

    fsck.ext3 -f -y /dev/mmcblk1p2 > /dev/null 2>&1
    _RE=$?
    if [ $_RE != 0 ]; then
        fsck.ext3 -f -y /dev/mmcblk1p2 > /dev/null 2>&1
        _RE=$?
    fi
    if [ $_RE != 0 ]; then
        return 1
    fi
    mount -t ext3 /dev/mmcblk1p2 /mnt/p2 > /dev/null 2>&1
    if [ $? != 0 ]; then
        umount /mnt/p1
        return 1
    fi

    return 0
}

#######################################################################

AAU_install_MLO_uboot_img()
{
    if [ ! -f /mnt/p1/MLO -o ! -f /mnt/p1/u-boot.img ]; then
        echo "[dn_copy] NEW STYLE FILE SYSTEM is not supported"
        return 2
    fi

	echo "[dn_copy] AAU, Install MLO & u-boot.img ..."

    if [ -f /mnt/p1/MLO.old ]; then
        \rm -f /mnt/p1/MLO.old
    fi
    cp -f /mnt/p1/MLO /mnt/p1/MLO.old
    cp -f MLO /mnt/p1/MLO

    if [ -f /mnt/p1/u-boot.img.old ]; then
        \rm -f /mnt/p1/u-boot.img.old
    fi
    cp -f /mnt/p1/u-boot.img /mnt/p1/u-boot.img.old
    cp -f u-boot.img /mnt/p1/u-boot.img

    sync

    if [ -f /mnt/p2/MLO.old ]; then
        \rm -f /mnt/p2/MLO.old
    fi
    cp -f /mnt/p2/MLO /mnt/p2/MLO.old
    cp -f MLO /mnt/p2/MLO

    if [ -f /mnt/p2/u-boot.img.old ]; then
        \rm -f /mnt/p2/u-boot.img.old
    fi
    cp -f /mnt/p2/u-boot.img /mnt/p2/u-boot.img.old
    cp -f u-boot.img /mnt/p2/u-boot.img

    sync

    return 0
}

AAU_install_image_ub()
{
    if [ ! -f /mnt/p2/image.ub ]; then
        return 1
    fi

    if [ -f /mnt/p2/MLO.chksum ]; then
        echo "[dn_copy] NEW STYLE FILE SYSTEM is not supported"
        return 2
    fi

	echo "[dn_copy] AAU, Install image.ub ..."

    if [ -f /usr/app_dft/images/image.ub ]; then
        \rm -f /usr/app_dft/images/image.ub
    fi

    if [ -f /mnt/p2/image.ub.old ]; then
        \rm -f /mnt/p2/image.ub.old
    fi
    cp -f /mnt/p2/image.ub /mnt/p2/image.ub.old
    cp -f image.ub /mnt/p2/image.ub

    sync

    return 0
}

AAU_check_and_install_images()
{
    local _RE

    if [ -f MLO -a -f u-boot.img ] || [ -f image.ub ]; then
        AAU_mount_p1_p2
    else
        echo "[dn_copy] NO MLO & u-boot.img or image.ub ..."
        return 1
    fi

    if [ -f MLO -a -f u-boot.img ]; then
        AAU_install_MLO_uboot_img
        _RE=$?
        if [ $_RE != 0 ]; then
            umount_p1_p2
            return $_RE
        fi
    fi

    if [ -f image.ub ]; then
        AAU_install_image_ub
        _RE=$?
        if [ $_RE != 0 ]; then
            umount_p1_p2
            return $_RE
        fi
    fi

    umount_p1_p2

    return 0
}

#######################################################################
#######################################################################
echo "--- run : $0"


if [ ! -f /Model ]; then
    echo "[dn_copy] invalid system ..."
    exit 1
fi

MODEL=$(cat /Model)

if [ "x$MODEL" != "xANTS_PRU_0" -a "x$MODEL" != "xANTS_AAU_0" ]; then
    echo "[dn_copy] not supported system model ($MODEL)  ..."
    exit 1
fi

cd $NEW_WORKING_DIR

if [ "x$MODEL" = "xANTS_PRU_0" ]; then
    PRU_check_and_install_images
else
    AAU_check_and_install_images
fi

RE=$?

cd $OLD_WORKING_DIR
echo "--- end : $0"


exit $RE

# EOF
