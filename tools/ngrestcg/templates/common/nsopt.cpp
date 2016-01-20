##ifneq($(.ns),$($lastNs))
$($lastNsEnd)
##var lastNs $(.ns)
##var lastNsEnd $(.endCppNs)
$(.startCppNs)
##endif
