Dual-list
=========

A list like container where objects are stored in multiple lists simultaneously. The list pointers are stored in the same objects as the data to improve memory locality. Note, there is no benefit to using this to store pointers since one would lose any possible benefits of data locality.

