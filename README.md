# Libraries
   We are providing all the libraries direcly so that one only has to link if they are workign on similar environments. But in a case linking given in "Methods to run a test case" fail, you might have to build the required libraries again according to instructions.
   ->gurobi1100
   ->z3 
   ->yices-2.6.4
   



# Configuring libary paths
    export PROJ_HOME="/home/poqer/Desktop" #this is just an example
    export GUROBI_HOME="${PROJ_HOME}/opt/gurobi1100/linux64"
    export GRB_LICENSE_FILE="${PROJ_HOME}/gurobi.lic"
    export PATH="${PATH}:${GUROBI_HOME}/bin"
    export PATH="${PATH}:/usr/bin/python3.8"
    export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${GUROBI_HOME}/lib"
    export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${PROJ_HOME}/z3/build"
    export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${PROJ_HOME}/yices-2.6.4/lib"
    export JAVA_HOME="/usr/lib/jvm/jdk-18"
    export JDK_HOME="/usr/lib/jvm/jdk-18"
    export PATH="${PATH}:${JAVA_HOME}/bin"
    export PATH="${PATH}:${PROJ_HOME}/DryadSynth/StarExec/bin"
    export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${PROJ_HOME}/DryadSynth/StarExec/lib"

# Method to run a test case 
    cd grid_solve
    cd handelman/cmake
    ## -> change the test case number
       make run
    ## -> if this succeeds, then you can just run the example using 
       ../exec/test<i> dump1 dump2 0 1
    ## -> else
       linkers tell you which library the problem originates from
       use the build instructions in this doc to build them and repeat the step


# Understanding the output
     cd $PROJ_HOME
     cd tests
     ## -> Gtest<i>.sl (smt equivalent of our problem)
     ## -> tc_<i>.txt (if else from of our code)
     python3 draw_graph.py <i>
     ## -> binary_tree_<i>.pdf (graphical output)

     cd $PROJ_HOME/grid_solve/handelman/tests/results_prog_sys
     ## ->results<i>-1.csv (includes all ouput boxes)
     python3 count_solutions.py <i> (displays the number of solutions in a test case)







# building yices-2.6.4
    navigate into the folder
    ./install_yices here

# building z3
    use the README.md in the folder

# building gurobi
    Gurobi needs a license file that you need to place in the $PROJ_HOME folder
    If this doesnt work, download and replace $PROJ_HOME/opt 



# building the project
   ->update the home dir path in the make file
   -> make build
    




