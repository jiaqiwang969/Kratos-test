#importing the Kratos Library
from Kratos import *
from KratosULFApplication import *
from KratosStructuralApplication import *
from KratosMeshingApplication import *
#import time

def AddVariables(model_part):
    model_part.AddNodalSolutionStepVariable(PRESSURE);
    model_part.AddNodalSolutionStepVariable(DISPLACEMENT);
    model_part.AddNodalSolutionStepVariable(VELOCITY);
    model_part.AddNodalSolutionStepVariable(ACCELERATION);
    model_part.AddNodalSolutionStepVariable(DENSITY);
    model_part.AddNodalSolutionStepVariable(VISCOSITY);
    model_part.AddNodalSolutionStepVariable(NODAL_AREA);
    model_part.AddNodalSolutionStepVariable(BODY_FORCE);
    model_part.AddNodalSolutionStepVariable(FORCE);
    model_part.AddNodalSolutionStepVariable(IS_FLUID);
    model_part.AddNodalSolutionStepVariable(IS_INTERFACE);
    model_part.AddNodalSolutionStepVariable(IS_STRUCTURE);
    model_part.AddNodalSolutionStepVariable(IS_LAGRANGIAN_INLET);
    model_part.AddNodalSolutionStepVariable(POSITIVE_FACE_PRESSURE);
    model_part.AddNodalSolutionStepVariable(IS_BOUNDARY);
    model_part.AddNodalSolutionStepVariable(IS_FREE_SURFACE);
    model_part.AddNodalSolutionStepVariable(BULK_MODULUS);
    model_part.AddNodalSolutionStepVariable(NODAL_H);
    model_part.AddNodalSolutionStepVariable(NORMAL);

def AddDofs(model_part):
    for node in model_part.Nodes:
        #adding dofs
        node.AddDof(DISPLACEMENT_X);
        node.AddDof(DISPLACEMENT_Y);
        node.AddDof(DISPLACEMENT_Z); 
        node.AddDof(IS_STRUCTURE);

class ULF_FSISolver:

    def __init__(self, fluid_model_part, structure_model_part, combined_model_part, box_corner1,box_corner2, domain_size, remeshing_flag, bulk_modulus, density):

        self.domain_size=domain_size;
        self.echo_level = 0
        self.counter = int(0)

        # TO REMESH OR NOT: 0 - no remeshing, 1 - remeshing
        self.remeshing_flag = remeshing_flag
        # K - the bulk modulus
        self.bulk_modulus = bulk_modulus
        self.density = density
        
        #saving the different model parts
        self.combined_model_part  = combined_model_part; #contains both structure and fluid
        self.fluid_model_part     = fluid_model_part; #contains only fluid elements
        self.structure_model_part = structure_model_part; #contains only structural elements

        #time integration scheme
        damp_factor = -0.3
        self.time_scheme = ResidualBasedPredictorCorrectorBossakScheme(damp_factor)

        #definition of the solvers
        #self.model_linear_solver =  SkylineLUFactorizationSolver()
        pDiagPrecond = DiagonalPreconditioner()
        self.model_linear_solver =  BICGSTABSolver(1e-8, 5000,pDiagPrecond)

        #definition of the convergence criteria
        self.conv_criteria = DisplacementCriteria(1e-6,1e-8)

        #self.pressure_calculate_process = PressureCalculateProcess(fluid_model_part,domain_size);
        self.ulf_apply_bc_process = UlfApplyBCProcess(fluid_model_part);
        self.ulf_time_step_dec_process = UlfTimeStepDecProcess(fluid_model_part);
        self.mark_fluid_process = MarkFluidProcess(fluid_model_part);
        self.mark_close_nodes_process = MarkCloseNodesProcess(fluid_model_part);
        self.mark_outer_nodes_process = MarkOuterNodesProcess(fluid_model_part);
        self.node_erase_process = NodeEraseProcess(fluid_model_part);

        #tools to save and merge the structural contributions
        self.save_structure_model_part_process = SaveStructureModelPartProcess();
        self.save_structure_conditions_process = SaveStructureConditionsProcess();
        self.merge_model_parts_process = MergeModelPartsProcess();

        ###temporary ... i need it to calculate the nodal area
        self.UlfUtils = UlfUtils()

        #self.save_structural_elements
        self.alpha_shape = 1.5;
        self.h_multiplier = 0.6

        ##saving the limits of the box (all the nodes external to this will be erased)
        self.box_corner1 = box_corner1
        self.box_corner2 = box_corner2
        
        if(domain_size == 2):
            #self.Mesher = TriGenModeler()
            self.Mesher = TriGenPFEMModeler()
            self.combined_neigh_finder = FindNodalNeighboursProcess(combined_model_part,9,18)
            self.fluid_neigh_finder = FindNodalNeighboursProcess(fluid_model_part,9,18)
        elif (domain_size == 3):
            #improved mesher
            self.Mesher = TetGenPfemModeler()
            #self.Mesher = TetGenModeler()
            self.combined_neigh_finder = FindNodalNeighboursProcess(combined_model_part,20,30)
            self.fluid_neigh_finder = FindNodalNeighboursProcess(fluid_model_part,20,30)

        print "after reading all the model contains:"
        print self.fluid_model_part

        #detect initial size distribution - note that initially the fluid model part contains
        #all the elements of both structure and fluid ... this is only true after reading the input
        (self.fluid_neigh_finder).Execute();
        Hfinder  = FindNodalHProcess(fluid_model_part);
        Hfinder.Execute();
    

       
    #######################################################################
    #delta time estimation based on the non-negativity of the jacobian
    def EstimateDeltaTime(self,max_dt,domain_size):
    	#return (self.UlfUtils).EstimateDeltaTime(min_dt,max_dt,self.combined_model_part)
        return (self.ulf_time_step_dec_process).EstimateDeltaTime(max_dt,domain_size)
    
    #######################################################################
    #this function is needed only in case there is a lagrangian nodes-inlet in the problem
    #three numbers - are veolicties in x y and z directions
    #def MoveInletNodes(self, model_part):
    #    (self.UlfUtils).MoveInletNodes(self.fluid_model_part, 0.1, 0.0, 0.0)
        
    
    #######################################################################
    def Initialize(self):

        #creating the solution strategy
        CalculateReactionFlag = False
        ReformDofSetAtEachStep = True
        MoveMeshFlag = True
        import ulf_strategy_python_inc
        self.solver = ulf_strategy_python_inc.ULFStrategyPythonInc(self.combined_model_part,self.fluid_model_part, self.time_scheme,self.model_linear_solver,self.conv_criteria,CalculateReactionFlag,ReformDofSetAtEachStep,MoveMeshFlag,self.domain_size, self.bulk_modulus, self.density)
        
        print "self.echo_level = " , self.echo_level
        (self.solver).SetEchoLevel(self.echo_level)
        print "finished initialization of the fluid strategy"
        
        #saving the structural elements
        (self.mark_fluid_process).Execute(); #we need this before saving the structrural elements

        #we specify domain size, to deal with problems involving membarnes in 3D in a specific way (see save_structure_model_part_process.h
        (self.save_structure_model_part_process).SaveStructure(self.fluid_model_part, self.structure_model_part, self.domain_size);
        (self.save_structure_conditions_process).SaveStructureConditions(self.fluid_model_part, self.structure_model_part, self.domain_size);

        #creating initially empty container for lagrangian inlet-nodes

        #lagrangian_inlet_model_part = ModelPart("LagrangianInletPart");
        #self.lagrangian_inlet_model_part = lagrangian_inlet_model_part;
        #saving the lagrangian inlet-nodes if they exist
        #(self.UlfUtils).SaveLagrangianInlet(self.fluid_model_part, self.lagrangian_inlet_model_part)
        
        #marking the fluid
        (self.fluid_neigh_finder).Execute();
        (self.combined_neigh_finder).Execute();
        
        (self.ulf_apply_bc_process).Execute();  
        (self.mark_fluid_process).Execute();
        #caluclating nodal area in order to calculate pressures
        (self.UlfUtils).CalculateNodalArea(self.fluid_model_part,self.domain_size);
        #remeshing before the first solution

        self.Remesh();
               

    ######################################################################
    def CheckForInvertedElements(self):
        volume = (self.UlfUtils).CalculateVolume(self.combined_model_part,self.domain_size)
        inverted_elements = False
        if(volume < 0.0):
            volume = - volume
            inverted_elements = True
        return [inverted_elements,volume]
                         
    #######################################################################
    def Solve(self):
        #next lines serve only in case of lagrangian inlet
        #for node in self.fluid_model_part.Nodes:
        #    if (node.GetSolutionStepValue(IS_LAGRANGIAN_INLET)!=1 and (node.GetSolutionStepValue(IS_STRUCTURE)!=1)):
        #        node.Free(DISPLACEMENT_X)
        #        node.Free(DISPLACEMENT_Y)
        #        node.Free(DISPLACEMENT_Z)

        print "solving the fluid problem"
        inverted_elements = (self.solver).Solve(self.domain_size,self.UlfUtils)
        print "succesful solution of the fluid "

        reduction_factor = 0.5
        max_reduction_steps = 5
        time_reduction_step = 0
        while(inverted_elements == True and time_reduction_step <= max_reduction_steps):
            print " *************************************************** "
            print "inverted element found ... reducing the time step"
            (self.UlfUtils).ReduceTimeStep(self.combined_model_part,reduction_factor);
            (self.UlfUtils).ReduceTimeStep(self.fluid_model_part,reduction_factor);
            (self.UlfUtils).ReduceTimeStep(self.structure_model_part,reduction_factor);
            print "reduction_step = ", time_reduction_step
            time_reduction_step = time_reduction_step + 1

            #copying vars from the old step
##            for node in (self.combined_model_part).Nodes:
##                pold = node.GetSolutionStepValue(PRESSURE,1);
##                dispold = node.GetSolutionStepValue(DISPLACEMENT,1);
##                velold = node.GetSolutionStepValue(VELOCITY,1);
##                accold = node.GetSolutionStepValue(ACCELERATION,1);
##
##                node.SetSolutionStepValue(PRESSURE,0,pold);
##                node.SetSolutionStepValue(DISPLACEMENT,0,dispold);
##                node.SetSolutionStepValue(VELOCITY,0,velold);
##                node.SetSolutionStepValue(ACCELERATION,0,accold);

            self.solver.MoveMesh()
            print "time step reduction completed"
            print " *************************************************** "

            (self.solver).Solve(self.domain_size,self.UlfUtils)
            [inverted_elements,vol] = self.CheckForInvertedElements()            

        if(inverted_elements == True):
            
            print "***********************************************************************"
            print "***********************************************************************"
            print "CRITICAL: ... element is still inverted after reducing the time step"
            print "***********************************************************************"
            print "***********************************************************************"
            factor = 2.0**5 #this is the original time step
            (self.UlfUtils).ReduceTimeStep(self.combined_model_part,factor);
            (self.UlfUtils).ReduceTimeStep(self.fluid_model_part,factor);
            (self.UlfUtils).ReduceTimeStep(self.structure_model_part,factor);
            
##            for node in (self.combined_model_part).Nodes:
##                pold = node.GetSolutionStepValue(PRESSURE,1);
##                dispold = node.GetSolutionStepValue(DISPLACEMENT,1);
##                velold = node.GetSolutionStepValue(VELOCITY,1);
##                accold = node.GetSolutionStepValue(ACCELERATION,1);
##
##                node.SetSolutionStepValue(PRESSURE,0,pold);
##                node.SetSolutionStepValue(DISPLACEMENT,0,dispold);
##                node.SetSolutionStepValue(VELOCITY,0,velold);
##                node.SetSolutionStepValue(ACCELERATION,0,accold);

            self.solver.MoveMesh()

            print "advancing in time without doing anything..."
            (self.solver).PredictionStep(self.domain_size,self.UlfUtils)
            
                            
        #print "pressure contribution process" - to be executed using exclusively fluid elements
        #and neighbouring relationships
        (self.fluid_neigh_finder).Execute();
        #print "injecting lagrangian inlet nodes - if given"
        #(self.UlfUtils).InjectNodesAtInlet(self.fluid_model_part, self.lagrangian_inlet_model_part, 10.0, 0.0, 0.0, 0.06)
        #moving the nodes
        #(self.UlfUtils).MoveInletNodes(self.fluid_model_part, 1.0, 0.0, 0.0)
        self.Remesh();


   ######################################################################
   #  the parameter in this function is set to 1 IN CASE WE DO NOT WANT TO CREATE THE NEW MESH, BUT JUST
   #  the operations on model part
   #  This is done to make switching off/on of remeshing easier
    def Remesh(self):
        
        #erasing nodes close to the wall
        #print "MARKING NODES CLOSE TO WALL FOR ERASING"
        #if (1.0*self.alpha_shape>1.0):
        #    reduced_alpha=1.0*self.alpha_shape
        #else:
        #    reduced_alpha=self.alpha_shape
        #reduced_alpha=5.0;
        #(self.UlfUtils).MarkNodesCloseToWall(self.fluid_model_part,self.domain_size, reduced_alpha);

        
        ##erase all conditions and elements prior to remeshing
        if (self.remeshing_flag==1.0):
            ((self.combined_model_part).Elements).clear();
            ((self.combined_model_part).Conditions).clear();
            ((self.combined_model_part).Nodes).clear();
            ((self.fluid_model_part).Elements).clear();
            ((self.fluid_model_part).Conditions).clear();
            

        #and erase bad nodes
        (self.mark_close_nodes_process).MarkCloseNodes(self.h_multiplier);
        (self.mark_outer_nodes_process).MarkOuterNodes(self.box_corner1, self.box_corner2);

        (self.node_erase_process).Execute();

        
        if (self.remeshing_flag==1.0):
                        
            #remesh CHECK for 3D or 2D            
            if (self.domain_size == 2):                               
                (self.Mesher).ReGenerateUpdatedLagrangian2Dinc(self.fluid_model_part,self.alpha_shape)            

            elif (self.domain_size == 3):                
                #improved qaulity mesher
                (self.Mesher).ReGenerateMeshPfemUlf3Dinc(self.fluid_model_part,self.alpha_shape)
                #(self.Mesher).ReGenerateUpdatedLagrangian3Dinc(self.fluid_model_part,self.alpha_shape)

        (self.node_erase_process).Execute();
        ##calculating fluid neighbours before applying boundary conditions
        (self.fluid_neigh_finder).Execute();
        ##(self.UlfUtils).CalculateNodalArea(self.fluid_model_part,self.domain_size);

        #(self.node_erase_process).Execute();

        #print "marking fluid" and applying fluid boundary conditions
        (self.ulf_apply_bc_process).Execute();
        (self.mark_fluid_process).Execute();
        #(self.node_erase_process).Execute();
        
        #saving structural conditions - walls
        #(self.save_structure_conditions_process).SaveStructureConditions(self.fluid_model_part, self.structure_model_part, self.domain_size);
         #merging the structural elements back (they are saved in the Initialize)
        (self.merge_model_parts_process).MergeParts(self.fluid_model_part, self.structure_model_part, self.combined_model_part);
        
        #calculating the neighbours for the overall model
        (self.combined_neigh_finder).Execute();
        (self.UlfUtils).CalculateNodalArea(self.fluid_model_part,self.domain_size);
        
##        for elem in self.combined_model_part.Elements:
##            print elem
            
        print "end of remesh fucntion"

       
    ######################################################################
    def FindNeighbours(self):
        (self.neigh_finder).Execute();
        
