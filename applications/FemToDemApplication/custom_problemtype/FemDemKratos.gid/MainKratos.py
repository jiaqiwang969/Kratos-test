import KratosMultiphysics
import KratosMultiphysics.FemToDemApplication
import KratosMultiphysics.FemToDemApplication.MainCouplingFemDem as MainCouplingFemDem

model = KratosMultiphysics.Model()
MainCouplingFemDem.MainCoupledFemDem_Solution(model).Run()
