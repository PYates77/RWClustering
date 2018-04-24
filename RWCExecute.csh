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
set expt
set native
set x11
set OUTDIR
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
    echo "--outdir/--od    Specifies the directory to place all output files (default is just RWClustering/)"
    echo "------WARNING-----"
    echo "IF USING --gui, YOU MUST SPECIFY EITHER --native or --x11 to determine which method you are running the execution script"
    echo "--native    Specifies the user is not using an X11 server for the GUI" 
    echo "--x11    Specifies the user is using an X11 server for the GUI"
    echo "Experimental Optional Arguments:"
    echo "--exp:    (RW Only; PREFERRED) Use experimental cluster-subset based, non-overlap to avoid overlapping clusters (runtime increase)"
    echo "--exp2:    (RW Only; DEPRECATED) Use experimental alternate traversal, non-overlap to avoid overlapping clusters (runtime increase)"
    echo ""
    echo "EXAMPLE USAGE:"
    echo "    ./RWCExecute.csh example_lecture.blif --s 4 --gui --x11 --outdir demo"
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
		echo "--c/--inter_cluster_delay <value>    Specify inter cluster delay as <value>"
		echo "--i/--pi_delay <value>    Specify every primary input delay as <value>"
		echo "--o/--po_delay <value>    Specify every primary output delay as <value>"
		echo "--n/--node_delay <value>    Specify every non-IO gate delay as <value>"
		echo "--lawler    Use Lawler Labeling and Clustering instead of RW"
		echo "--no_sparse    Use full delay matrix instead of default sparse matrix"
		echo "--no_matrix    Use on-the-fly delay calculations instead of matrix (runtime increase; memory decrease)"
		echo "NOTE: --no_matrix overrides --no_sparse"
        echo "--gui    Use interactive GUI if the circuit is small enough"
        echo "--outdir/--od    Specifies the directory to place all output files (default is just RWClustering/)"
        echo "------WARNING-----"
        echo "IF USING --gui, YOU MUST SPECIFY EITHER --native or --x11 to determine which method you are running the execution script"
        echo "--native    Specifies the user is not using an X11 server for the GUI" 
        echo "--x11    Specifies the user is using an X11 server for the GUI"
        echo "Experimental Optional Arguments:"
        echo "--exp:    (RW Only; PREFERRED) Use experimental cluster-subset based, non-overlap to avoid overlapping clusters (runtime increase)"
        echo "--exp2:    (RW Only; DEPRECATED) Use experimental alternate traversal, non-overlap to avoid overlapping clusters (runtime increase)"
        echo ""
        echo "EXAMPLE USAGE:"
        echo "    ./RWCExecute.csh example_lecture.blif --s 4 --gui --x11 --outdir demo"
		exit
	endif
	if ( $argv[$i] == "--max_cluster_size" || $argv[$i] == "--s") then
		@ i++
		if ( $i > $#argv ) then
			echo "[ERROR] Did not specify max cluster size value"
			exit
		endif
		if ( $argv[$i] == "" ) then
			echo "[ERROR] Did not specify max cluster size value"
			exit
		endif
		@ MCS = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--inter_cluster_delay" || $argv[$i] == "--c") then
		@ i++
		if ( $i > $#argv ) then
			echo "[ERROR] Did not specify inter cluster delay value"
			exit
		endif
		if ( $argv[$i] == "" ) then
			echo "[ERROR] Did not specify inter cluster delay value"
			exit
		endif
		@ ICD = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--pi_delay" || $argv[$i] == "--i") then
		@ i++
		if ( $i > $#argv ) then
			echo "[ERROR] Did not specify primary input node delay value"
			exit
		endif
		if ( $argv[$i] == "" ) then
			echo "[ERROR] Did not specify primary input node delay value"
			exit
		endif
		@ PID = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--po_delay" || $argv[$i] == "--o") then
		@ i++
		if ( $i > $#argv ) then
			echo "[ERROR] Did not specify primary output node delay value"
			exit
		endif
		if ( $argv[$i] == "" ) then
			echo "[ERROR] Did not specify primary output node delay value"
			exit
		endif
		@ POD = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--node_delay" || $argv[$i] == "--n") then
		@ i++
		if ( $i > $#argv ) then
			echo "[ERROR] Did not specify node delay value"
			exit
		endif
		if ( $argv[$i] == "" ) then
			echo "[ERROR] Did not specify node delay value"
			exit
		endif
		@ ND = $argv[$i]
		@ i++
		continue
	endif
    if ( $argv[$i] == "--outdir" || $argv[$i] == "--od") then
		@ i++
		if ( $i > $#argv ) then
			echo "ERROR: Did not specify output directory"
			exit
		endif
		if ( $argv[$i] == "" ) then
			echo "ERROR: Did not specify output directory"
			exit
		endif
		set OUTDIR = $argv[$i]
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
    if ( $argv[$i] == "--exp2" ) then
		set expt = $argv[$i]
		@ i++
		continue
	endif
	if ( $argv[$i] == "--gui" ) then
		set gui = $argv[$i]
		@ i++
		continue
	endif
    if ( $argv[$i] == "--native" ) then
        set native = $argv[$i]
        @ i++
        continue
    endif
    if ( $argv[$i] == "--x11" ) then
        set x11 = $argv[$i]
        @ i++
        continue
    endif
	echo "UNSUPPORTED OPTION: $argv[$i]"
	exit	
end
if ( $gui != "" && $native == "" && $x11 == "" ) then
    echo "[ERROR] You must specify either --native or --x11 if you want to use the GUI"
    exit
endif
if ( $gui != "" && $native != "" && $x11 != "" ) then
    echo "[ERROR] You cannot specify both --native or -x11; Pick one"
    exit
endif
if ( ($exp != "" || $expt != "") && $lawler != "" ) then
    echo "[WARNING] Experimental methods do not support Lawler; Turning off Lawler..."
    set lawler = ""
endif
if ( $exp != "" && $expt != "" ) then
    echo "[ERROR] Cannot use both $exp and $expt; Try with only one of the experimental methods"
    exit
endif
if ( $gui != "" && $lawler != "" ) then
    echo "[WARNING] RWGUI does not support Lawler at this time; Turning off GUI..."
    set gui = ""
endif
if ( $gui != "" && $expt != "" ) then
    echo "[WARNING] RWGUI does not support non-L set experimental traversal; Turning off GUI..."
    set gui = ""
endif
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
if ( $exp != "" || $expt != "" ) then
	echo "EXPERIMENTAL NON-OVERLAP CLUSTER MODE: ENABLED WITH $exp $expt"
else
	echo "EXPERIMENTAL NON-OVERLAP CLUSTER MODE: DISABLED"
endif
if ( $gui != "" ) then
	echo "GUI MODE: ENABLED WITH $native $x11 SUPPORT"
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
if ( $OUTDIR != "" ) then
    echo "--------------------"
    echo "[RWCEXECUTE] SETTING UP OUTPUT DIRECTORY $OUTDIR"
    echo "--------------------"
    if ( -d $OUTDIR ) then
        echo "[WARNING] $OUTDIR already exists; Moving on..."
    else    
        mkdir $OUTDIR
        if ( $status != 0 ) then
            echo "[ERROR] $OUTDIR creation failed; Check your permissions to create folders"
        endif
        echo "$OUTDIR created successfully"
    endif
endif
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
if ( $OUTDIR != "" ) then
    echo "--------------------"
    echo "[RWCEXECUTE] MOVING OUTPUT FILES TO $OUTDIR"
    echo "--------------------"
    mv output_* $OUTDIR/
    if ( $status != 0 ) then
        echo "[ERROR] Move to $OUTDIR failed"
        exit
    endif
endif
if ( -r "Python/input_graph.dmp" ) then
    echo "--------------------"
    echo "[RWCEXECUTE] RUNNING INTERACTIVE PYTHON GUI"
    echo "--------------------"
    cd Python
    python RWGUI.py $native $x11
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
