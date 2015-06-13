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
        : ${T_INFORM:$_INFROME}
        case $T_INFORM in
            yes|Yes) break;;
            no|No) break;;
            *) echo "the choices are yes, no";;
        esac
    done

    while :
    do
        echo -n "Backup Type               [$_TYPE]:"
        read T_TYPE
        : ${T_TYPE:=$_TYPE}
        case $T_TYPE in
            Full|full) break;;
            Normal|normal)break;;
            Sybase|sybase)break;;
            *)echo "The choices are either ... full, normal, sybase"
                ;;
            esac
    done
    _DEVICE=$T_DEVICE; _INFROME=$T_INFORM; _TYPE=$T_TYPE;
}

show_settings()
{
    cat <<EOF
                  Default Settings Are ...
                  Tape Device To Be Used : $_DEVICE
                  Mail Admin When Done   : $_INFROME
                  Type Of Backup         : $_TYPE
                  Log file of backup     : $_LOGFILE
EOF
}

get_code()
{
    clear
    header
    _COUNTER=0
    echo " You Must enter the correct code to be able change
           default settings"
    while
    do
        _COUNTER=`expr $_COUNTER + 1`
        echo -n "Enter the code to change the settings: "
        read T_CODE
        if [ "$T_CODE" = "$_CODE" ] ; then
            return 0
        fi
    done
}
