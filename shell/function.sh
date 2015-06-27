#! /bin/sh

true_or_false()
{
    if [ $1 = "a" ];
    then
        echo "a return 0"
        return 0
    else
        echo "not a return 0"
        return 1
    fi
}

if true_or_false "a"
then
    echo "input a, if true"
else
    echo "input a, if false"
fi
