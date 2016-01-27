##switch $(.type)
##case string||enum
String\
##case generic
##ifeq($(.name.!match/bool/),true)
Boolean\
##else
##ifeq($(.name),void)
Undefined\
##else
Number\
##endif
##endif
##case struct||typedef
Object\
##case template
Array\
##default
Unknown\
##endswitch
