# Succinct PPMC algorithm (tree version)
PPMC compression algorithm using XBW transform and succinct tree as an underliying structure.

This branch contains version of algorithm that is not using de Bruijn graphs but rather trees to represent contexts. It can be used for comparison of both of these approaches. It uses much more space (as the tree grows much bigger than corresponding graph) and time (because of huge trees), but the resulting archive can be smaller.

This branch is not really well maintained and it just stands as a sole reminder that you should really explore what you should implement before implementing something else...
