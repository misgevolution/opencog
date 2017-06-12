#!/bin/sh

 for f in $(ls *.json -v); do
	 echo $(echo ""$f | sed 's/.json//')"------------------------";
	 ./extract_nodes_from_afjson.py $f >> nodes.txt; 
	 echo "Extracted from "$f;
 done

 echo "Done node extraction. All extracts have been dumped to nodes.txt"
