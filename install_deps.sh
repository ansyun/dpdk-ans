#!/bin/sh -e

set -e

if test -e ans 
then
    echo "Start to generate librte_ans.a/librte_anssock.a for -mnative."
else
    echo "You must run this script in the top-level dpdk-ans directory"
    exit 1
fi

NATIVE_STR=$(gcc -c -Q -march=native --help=target | grep march | sed 's/[[:space:]]//g')
NATIVE_MATCH=${NATIVE_STR#*=}

echo "Your native march is $NATIVE_MATCH."

if [ $NATIVE_MATCH = "westmere" ] ; then

    cd $RTE_ANS/librte_ans/
    rm -f librte_ans.a
    ln -s librte_ans_westmere.a librte_ans.a

    cd $RTE_ANS/librte_anssock/
    rm -f librte_anssock.a
    ln -s librte_anssock_westmere.a librte_anssock.a
    
    echo "Generate librte_ans.a/librte_anssock.a for westmere successfully."
    
elif [ $NATIVE_MATCH = "sandybridge" ] ; then   

    cd $RTE_ANS/librte_ans/
    rm -f librte_ans.a
    ln -s librte_ans_sandybridge.a librte_ans.a

    cd $RTE_ANS/librte_anssock/
    rm -f librte_anssock.a
    ln -s librte_anssock_sandybridge.a librte_anssock.a

    echo "Generate librte_ans.a/librte_anssock.a for sandybridge successfully."

elif [ $NATIVE_MATCH = "ivybridge" ] ; then 

    cd $RTE_ANS/librte_ans/
    rm -f librte_ans.a
    ln -s librte_ans_ivybridge.a librte_ans.a

    cd $RTE_ANS/librte_anssock/
    rm -f librte_anssock.a
    ln -s librte_anssock_ivybridge.a librte_anssock.a
 
    echo "Generate librte_ans.a/librte_anssock.a for ivybridge successfully."

elif [ $NATIVE_MATCH = "haswell" ] ; then   

    cd $RTE_ANS/librte_ans/
    rm -f librte_ans.a
    ln -s librte_ans_haswell.a librte_ans.a

    cd $RTE_ANS/librte_anssock/
    rm -f librte_anssock.a
    ln -s librte_anssock_haswell.a librte_anssock.a

    echo "Generate librte_ans.a/librte_anssock.a for haswell successfully."

elif [ $NATIVE_MATCH = "broadwell" ] ; then   

    cd $RTE_ANS/librte_ans/
    rm -f librte_ans.a
    ln -s librte_ans_broadwell.a librte_ans.a

    cd $RTE_ANS/librte_anssock/
    rm -f librte_anssock.a
    ln -s librte_anssock_broadwell.a librte_anssock.a
  
    echo "Generate librte_ans.a/librte_anssock.a for broadwell successfully."

else 

    echo "Your native march is none of westmere/sandybridge/ivybridge/haswell/broadwell." 
    echo "You may generate librte_ans.a/librte_anssock.a based on your -mnative by manual,"
    echo "Or ask help from ANS develop team." 

fi     

exit 0
