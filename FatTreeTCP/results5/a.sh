
echo " "
echo " "

echo " Running FatTree Traffic Matrix .... "
 
#echo $0 $1 ' > echo $0 $1 '
#args=("$@")
#declare -i a=${args[0]}
#declare -i b=${args[1]}


#echo " >>> Enter FatTreeSize: "
#read a
#echo " >>> Enter numShortFlows: "
#read b
#echo " "

#../../FatTree -u Cmdenv  -m  -n ../..:../../../inet/src -l ../../../inet/src/INET -f  ../omnetpp5.ini  -c General -r   "\$FatTreeSize==$(printf '%d' $1)  &&  \$numShortFlows==$(printf '%d' $2)"
#../../FatTree -u Cmdenv  -m  -n ../..:../../../inet/src -l ../../../inet/src/INET -f  ../centralSch.ini  -c General   -q runs


 
opp_run -u Cmdenv -m -n ../../..:../../../../examples   -l ../../../INET -f ../omnetpp5.ini -c General -r "\$FatTreeSize==$(printf '%d' $1)  &&  \$numShortFlows==$(printf '%d' $2)"
