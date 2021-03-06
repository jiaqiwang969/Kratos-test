##################################################################
##################################################################
#setting the domain size for the problem to be solved
domain_size = 2

##################################################################
##################################################################
## ATTENTION: here the order is important

#including kratos path
kratos_libs_path = '../../../../libs' ##kratos_root/libs
kratos_applications_path = '../../../../applications' ##kratos_root/applications
import sys
sys.path.append(kratos_libs_path)
sys.path.append(kratos_applications_path)

print "before importing kratos"

#importing Kratos main library
from Kratos import *
print "Kratos library imported"
kernel = Kernel()   #defining kernel
print "kernel created"
#importing applications
import applications_interface
applications_interface.Import_ConvectionDiffusionApplication = True

applications_interface.ImportApplications(kernel, kratos_applications_path)

## from now on the order is not anymore crucial
##################################################################
##################################################################

from KratosConvectionDiffusionApplication import *

##x = raw_input("stopped to allow debug: set breakpoints and press enter to continue");

#defining a model part
model_part = ModelPart("FluidPart");  

#adding of Variables to Model Part
import nonlinear_convection_diffusion_solver
nonlinear_convection_diffusion_solver.AddVariables(model_part)

#reading a model
#reading a model
gid_mode = GiDPostMode.GiD_PostBinary
multifile = MultiFileFlag.MultipleFiles
deformed_mesh_flag = WriteDeformedMeshFlag.WriteUndeformed
write_conditions = WriteConditionsFlag.WriteElementsOnly
gid_io = GidIO("face_heat",gid_mode,multifile,deformed_mesh_flag, write_conditions)
gid_io.ReadModelPart(model_part)

mesh_name = 0.0
gid_io.InitializeMesh( mesh_name );
gid_io.WriteMesh((model_part).GetMesh());
gid_io.FinalizeMesh()
print model_part

#the buffer size should be set up here after the mesh is read for the first time
model_part.SetBufferSize(3)

#importing the solver files
nonlinear_convection_diffusion_solver.AddDofs(model_part)

    
#creating a fluid solver object
solver = nonlinear_convection_diffusion_solver.ConvectionDiffusionSolver(model_part,domain_size)
solver.time_order = 1
#pDiagPrecond = DiagonalPreconditioner()
#solver.linear_solver =  BICGSTABSolver(1e-3, 5000,pDiagPrecond)
solver.linear_solver = SkylineLUFactorizationSolver();
solver.echo_level = 0
solver.Initialize()

#assigning the fluid properties
conductivity = 0.25;
density = 900.0;
specific_heat = 2400.0;
for node in model_part.Nodes:
    node.SetSolutionStepValue(CONDUCTIVITY,0,conductivity);
    node.SetSolutionStepValue(DENSITY,0,density);
    node.SetSolutionStepValue(SPECIFIC_HEAT,0,specific_heat);

model_part.Properties[1][EMISSIVITY] = 1.0
model_part.Properties[1][AMBIENT_TEMPERATURE] = 25.0
model_part.Properties[1][CONVECTION_COEFFICIENT] = 8.0

   
#applying a temperature of 100
for node in model_part.Nodes:
    if(node.Y > 0.499):
         node.SetSolutionStepValue(FACE_HEAT_FLUX,0,10000.0);

 


#settings to be changed
nsteps = 10000
output_step = 50

Dt = 10.0

out = 1


for step in range(0,nsteps):
    time = Dt*step
    model_part.CloneTimeStep(time)

    print time
    #print model_part.ProcessInfo()[TIME]

    #solving the fluid problem
    if(step > 3):
        solver.Solve()

    #print the results
    if(out == output_step):
        gid_io.WriteNodalResults(TEMPERATURE,model_part.Nodes,time,0)
        gid_io.WriteNodalResults(VELOCITY,model_part.Nodes,time,0)
        gid_io.WriteNodalResults(TEMP_CONV_PROJ,model_part.Nodes,time,0)
        gid_io.WriteNodalResults(FACE_HEAT_FLUX,model_part.Nodes,time,0)
        out = 0
    out = out + 1

          
        

