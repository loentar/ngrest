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
##case struct
Object\
##case template
##ifeq($(.name),list||vector)
Array\
##else
Object\
##endif
##case typedef
Any\
##default
Unknown\
##endswitch
