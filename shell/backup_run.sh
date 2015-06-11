#!/bin/sh
SOURCE=/appdva/bin/backup.defaults
check_source()
{
    if [ -r $SOURCE ]; then
        . $SOURCE
    else
        echo "`basename $0`: cannot localte defaults file"
        exit 1
    fi
}

header()
{
    USER=`whoami`
    MYDATE=`date +%A " "%e" of "%B-%Y`
    clear
    cat <<EOF
User: $USER                     $MYDATE
                NETWORK SYSTEM BACKUP
                ====================
EOF
}

change_settings()
{
    header
    echo "Valid Entries Are ..."
    echo "Tape Device: rmt0, rmt1, rmt3"
    echo "Mail Admin: yes, no"
    echo "Backup Type: full, Normal, sybase "
    while :
    do
        echo -n -c "\n\n Tape Device To Be Used for This BackUp [$_DEVICE]:"
        read T_DEVICE : ${T_DEVICE:=$_DEVICE}
        case $T_DEVICE in
            rmt0|rmt1|rmt3) break;;
            *) echo "The devices are either ... rmt0, rmt1, rmt3"
                ;;
            esac
    done

    while :
    do
        echo -n "Mail Admin When Done"
        read T_INFORM
        : {}
}
