c=1
while [ $c -le 5 ]
do
    clear
    cat /proc/`ps -e |grep wvWare |awk '{print $1}'`/status |grep VmSize
    sleep 0.25
done


