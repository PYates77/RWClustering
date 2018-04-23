#!/bin/csh -f
#FILE: RWCExecute.csh
#AUTHOR: Akshay Nagendra <akshaynag@gatech>
#DESCRIPTION: File to handle execution of the RWClustering application
#RUN ./RWCExecute.csh --help for example usages and details
set i = 2
set ICD = 3
set MCS = 8
set PID = 0
set POD = 1
set ND = 1
set lawler
set no_sparse
set no_matrix
set gui
set exp
if ($#argv == 0) then
	echo "Must specify input BLIF files to use"
    echo "Try: ./RWCExecute.csh --help for more information"
	exit
endif
set BLIFILE = $argv[1]
if ( !(-r $BLIFILE) && $argv[1] != "--help" ) then
	echo "[ERROR] $BLIFILE does not exist"
	exit
else if ( $argv[1] == "--help" ) then
    echo "FILE: RWCExecute.csh"
    echo "AUTHOR: Akshay Nagendra <akshaynag@gatech.edu>"
    echo "DESCRIPTION: Script to run RWClustering Application"
    echo "USAGE:"
    echo "./RWCExecute.csh <BLIFILE> [optional args]"
    echo "Optional Arguments:"
    echo "--s/--max_cluster_size <value>:    Specify max cluster size as <value>"
    echo "--c/--inter_cluster_delay <value>:    Specify inter cluster delay as <value>"
    echo "--i/--pi_delay <value>:    Specify every primary input delay as <value>"
    echo "--o/--po_delay <value>:    Specify every primary output delay as <value>"
    echo "--n/--node_delay <value>:    Specify every non-IO gate delay as <value>"
    echo "--lawler:    Use Lawler Labeling and Clustering instead of RW"
    echo "--no_sparse:    Use full delay matrix instead of default sparse matrix"
    echo "--no_matrix:    Use on-the-fly delay calculations instead of matrix (runtime increase; memory decrease)"
    echo "NOTE: --no_matrix overrides --no_sparse"
    echo "--gui:    Use interactive GUI if the circuit is small enough"
    echo "--exp:    Use experimental non-overlap to avoid overlapping clusters (runtime increase)"
    echo ""
    echo "EXAMPLE USAGE:"
    echo "    ./RWCExecute.csh example_lecture.blif --s 4 --gui"
    exit
endif
while ( $i <= $#argv )
	if ($argv[$i] == "--help") then
		echo "FILE: RWCExecute.csh"
		echo "AUTHOR: Akshay Nagendra <akshaynag@gatech.edu>"
		echo "DESCRIPTION: Script to run RWClustering Application"
		echo "USAGE:"
		echo "./RWCExecute.csh <BLIFILE> [optional args]"
		echo "Optional Arguments:"
		echo "--s/--max_cluster_size <value> \tSpecify max cluster size as <value>"
		echo "--c/--inter_cluster_delay <value> \tSpecify inter cluster delay as <value>"
		echo "--i/--pi_delay <value> \tSpecify every primary input delay as <value>"
		echo "--o/--po_delay <value> \tSpecify every primary output delay as <value>"
		echo "--n/--node_delay <value> \tSpecify every non-IO gate delay as <value>"
		echo "--lawler \tUse Lawler Labeling and Clustering instead of RW"
		echo "--no_sparse \tUse full delay matrix instead of default sparse matrix"
		echo "--no_matrix \tUse on-the-fly delay calculations instead of matrix (runtime increase; memory decrease)"
		echo "NOTE: --no_matrix overrides --no_sparse"
		echo "--gui \tUse interactive GUI if the circuit is small enough"
		echo "--exp \tUse experimental non-overlap to avoid overlapping clusters (runtime increase)"
        echo ""
        echo "EXAMPLE USAGE:"
        echo "    ./RWCExecute.csh example_lecture.blif --s 4 --gui"
		exit
	endif
	if ( $argv[$i] == "--max_cluster_size" || $argv[$i] == "--s") then
		@ i++
		if ( $i > $#argv ) then
			echo "ERROR: Did not MCS specify value"
			exit
		endif
		if ( $argv[$i] == "" ) then
			echo "ERROR: Did not MCS specify value"
			exit
		endif
		@ MCS = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--inter_cluster_delay" || $argv[$i] == "--c") then
		@ i++
		if ( $i > $#argv ) then
			echo "ERROR: Did not ICD specify value"
			exit
		endif
		if ( $argv[$i] == "" ) then
			echo "ERROR: Did not ICD specify value"
			exit
		endif
		@ ICD = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--pi_delay" || $argv[$i] == "--i") then
		@ i++
		if ( $i > $#argv ) then
			echo "ERROR: Did not PID specify value"
			exit
		endif
		if ( $argv[$i] == "" ) then
			echo "ERROR: Did not PID specify value"
			exit
		endif
		@ PID = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--po_delay" || $argv[$i] == "--o") then
		@ i++
		if ( $i > $#argv ) then
			echo "ERROR: Did not POD specify value"
			exit
		endif
		if ( $argv[$i] == "" ) then
			echo "ERROR: Did not POD specify value"
			exit
		endif
		@ POD = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--node_delay" || $argv[$i] == "--n") then
		@ i++
		if ( $i > $#argv ) then
			echo "ERROR: Did not ND specify value"
			exit
		endif
		if ( $argv[$i] == "" ) then
			echo "ERROR: Did not ND specify value"
			exit
		endif
		@ ND = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--lawler" ) then
		set lawler = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--no_sparse" ) then
		set no_sparse = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--no_matrix" ) then
		set no_sparse
		set no_matrix = $argv[$i]
		@ i++
		continue
	endif	
	if ( $argv[$i] == "--exp" ) then
		set exp = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--gui" ) then
		set gui = $argv[$i]
		@ i++
		continue
	endif
	echo "UNSUPPORTED OPTION: $argv[$i]"
	exit	
end
echo "--------------------"
echo "[RWCEXECUTE] RUN PARAMETERS"
echo "--------------------"
echo "BLIF FILE: $BLIFILE"
echo "MAX CLUSTER SIZE: $MCS"
echo "INTER CLUSTER DELAY: $ICD"
echo "PI DELAY: $PID"
echo "PO DELAY: $POD"
echo "NODE DELAY: $ND"
if ( $lawler != "" ) then
	echo "LAWLER MODE: ENABLED"
else
	echo "LAWLER MODE: DISABLED"
endif
if ( $no_sparse == "" && $no_matrix == "") then
	echo "SPARSE MATRIX MODE: ENABLED"
else
	echo "SPARSE MATRIX MODE: DISABLED"
endif
if ( $no_matrix == "") then
	echo "MATRIX MODE: ENABLED"
else
	echo "MATRIX MODE: DISABLED"
endif
if ($exp != "") then
	echo "EXPERIMENTAL NON-OVERLAP CLUSTER MODE: ENABLED"
else
	echo "EXPERIMENTAL NON-OVERLAP CLUSTER MODE: DISABLED"
endif
if ($gui != "") then
	echo "GUI MODE: ENABLED"
else
	echo "GUI MODE: DISABLED"
endif
#BEGIN RW CLUSTERING EXECUTION
#USER MUST HAVE CALLED CMAKE . AND MAKE
echo "--------------------"
echo "[RWCEXECUTE] CLEANING FILES"
echo "--------------------"
rm output_*
rm Python/input_graph.dmp
echo "--------------------"
echo "[RWEXECUTE] RUNNING RWCLUSTERING APPLICATION"
echo "--------------------"
./rw $BLIFILE --max_cluster_size $MCS -inter_cluster_delay $ICD --pi_delay $PID --po_delay $POD --node_delay $ND $lawler $no_sparse $no_matrix $exp $gui
if ( $status != 0 ) then
    echo "--------------------"
    echo "[RWCEXECUTE] EXECUTION STATUS: FAILURE"
    echo "--------------------"
    exit
else
    echo "--------------------"
    echo "[RWCEXECUTE] EXECUTION STATUS: PASS"
    echo "--------------------"
endif
if ( -r "Python/input_graph.dmp" ) then
    echo "--------------------"
    echo "[RWCEXECUTE] RUNNING INTERACTIVE PYTHON GUI"
    echo "--------------------"
    cd Python
    python RWGUI.py
    if ( $status != 0 ) then
        echo "PYTHON GUI FAILURE"
    endif
    exit
endif
if ($gui != "") then
    echo "--------------------"
    echo "[RWCEXECUTE] GUI ABORTED DUE TO SIZE"
    echo "--------------------"
    exit
endif