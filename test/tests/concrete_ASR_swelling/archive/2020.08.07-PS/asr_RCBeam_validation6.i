# Rebar:        ASTM A615 grade 60
# Concrete:     highly reactive concrete mix
# Cement:       high alkali type I/II with alkali content = 1.1%
# Agg:          half and half blend of low and high reactive 19 mm crushed coarse aggregate and highly reactive concrete sand
# Coarse Agg:   contained quartz and granite
# Fine Agg:     contained quartz, chert, and feldspar
# NaOH:         50% NaOH was added to the mixing water to boost equivalent alkali content to 1.25%
# w/c:          0.5
# fc'|28days:   33 MPa (measured) 31 MPa (designed)
# Observation:  prism expansion reached 0.61% after 1 year (tested as per ASTM C 1293)
# Environment:  cast at 10.6C and <80% RH
#               week long drying and wetting @ 5C-10C more than seasonal temperature (5C-40C), and RH (between 90%-100%)

[GlobalParams]
 displacements = 'disp_x disp_y disp_z'
 volumetric_locking_correction = true
[]

[Mesh]
 file = gold/mesh_WaldBeam.e
 # block 1      : concrete domain
 # block 2      : long rebar
 # block 3      : trans rebar
 # block 4      : headed rebar
 # nodeset 1    : Zone A
 # nodeset 2    : Zone B
 # nodeset 3    : Zone C
 # nodeset 4    : BC @(front, left,  bottom)  for (x, y, and z)
 # nodeset 5    : BC @(back,  right, bottom)  for (y and z)
 # nodeset 6    : BC @(front, right, bottom)  for (z)
 # nodeset 7    : Ends of headed reinforcement
[]

[Variables]
 [./T]
   order = FIRST
   family = LAGRANGE
   initial_condition = 10.6
 [../]
 [./rh]
   order = FIRST
   family = LAGRANGE
   initial_condition = 0.8
 [../]
[]

[AuxVariables]
 [./resid_x]
 [../]
 [./resid_y]
 [../]
 [./resid_z]
 [../]
 [./ASR_ex]
   order = CONSTANT
   family = MONOMIAL
   block = 1
 [../]

 [./ASR_vstrain]
   order = CONSTANT
   family = MONOMIAL
   block = 1
 [../]
 [./ASR_strain_xx]
   order = CONSTANT
   family = MONOMIAL
   block = 1
 [../]
 [./ASR_strain_yy]
   order = CONSTANT
   family = MONOMIAL
   block = 1
 [../]
 [./ASR_strain_zz]
   order = CONSTANT
   family = MONOMIAL
   block = 1
 [../]
 [./ASR_strain_xy]
   order = CONSTANT
   family = MONOMIAL
   block = 1
 [../]
 [./ASR_strain_yz]
   order = CONSTANT
   family = MONOMIAL
   block = 1
 [../]
 [./ASR_strain_zx]
   order = CONSTANT
   family = MONOMIAL
   block = 1
 [../]
 [./volumetric_strain]
   order = CONSTANT
   family = MONOMIAL
   block = 1
 [../]
 [./thermal_strain_xx]
   order = CONSTANT
   family = MONOMIAL
   block = 1
 [../]
 [./thermal_strain_yy]
   order = CONSTANT
   family = MONOMIAL
   block = 1
 [../]
 [./thermal_strain_zz]
   order = CONSTANT
   family = MONOMIAL
   block = 1
 [../]
 [./thermal_conductivity]
   order = CONSTANT
   family = Monomial
 [../]
 [./thermal_capacity]
   order = CONSTANT
   family = Monomial
 [../]
 [./moisture_capacity]
   order = CONSTANT
   family = Monomial
 [../]
 [./humidity_diffusivity]
   order = CONSTANT
   family = Monomial
 [../]
 [./water_content]
   order = CONSTANT
   family = Monomial
 [../]
 [./water_hydrated]
   order = CONSTANT
   family = Monomial
 [../]
 [damage_index]
   order = CONSTANT
   family = MONOMIAL
 []
 [./area_long]
   order = CONSTANT
   family = MONOMIAL
 [../]
 [./area_trans]
   order = CONSTANT
   family = MONOMIAL
 [../]
 [./area_headed]
   order = CONSTANT
   family = MONOMIAL
 [../]
 [./axial_stress]
   order = CONSTANT
   family = MONOMIAL
 [../]
 [./axial_strain]
   order = CONSTANT
   family = MONOMIAL
 [../]
[]

[Modules/TensorMechanics/Master]
 [./concrete]
   block = 1
   strain = FINITE
   add_variables = true
   eigenstrain_names = 'asr_expansion thermal_expansion'
   generate_output = 'stress_xx stress_yy stress_zz stress_xy stress_yz stress_zx vonmises_stress hydrostatic_stress elastic_strain_xx elastic_strain_yy elastic_strain_zz strain_xx strain_yy strain_zz'
   save_in = 'resid_x resid_y resid_z'
 [../]
[]

[Modules/TensorMechanics/LineElementMaster]
 [./longitudinal_rebar_block]
   block = 2
   truss = true
   area = area_long
   displacements = 'disp_x disp_y disp_z'
   save_in = 'resid_x resid_y resid_z'
 [../]
 [./transverse_rebar_block]
   block = 3
   truss = true
   area = area_trans
   displacements = 'disp_x disp_y disp_z'
   save_in = 'resid_x resid_y resid_z'
 [../]
 [./headed_rebar_block]
   block = 4
   truss = true
   area = area_headed
   displacements = 'disp_x disp_y disp_z'
   save_in = 'resid_x resid_y resid_z'
 [../]
[]

[Constraints]
 [./rebar_long_x]
   type = EqualValueEmbeddedConstraint
   secondary = 2
   primary = 1
   penalty = 1e12
   variable = 'disp_x'
   primary_variable = 'disp_x'
   formulation = penalty
 [../]
 [./rebar_long_y]
   type = EqualValueEmbeddedConstraint
   secondary = 2
   primary = 1
   penalty = 1e12
   variable = 'disp_y'
   primary_variable = 'disp_y'
   formulation = penalty
 [../]
 [./rebar_long_z]
   type = EqualValueEmbeddedConstraint
   secondary = 2
   primary = 1
   penalty = 1e12
   variable = 'disp_z'
   primary_variable = 'disp_z'
   formulation = penalty
 [../]

 [./rebar_trans_x]
   type = EqualValueEmbeddedConstraint
   secondary = 3
   primary = 1
   penalty = 1e12
   variable = 'disp_x'
   primary_variable = 'disp_x'
   formulation = penalty
 [../]
 [./rebar_trans_y]
   type = EqualValueEmbeddedConstraint
   secondary = 3
   primary = 1
   penalty = 1e12
   variable = 'disp_y'
   primary_variable = 'disp_y'
   formulation = penalty
 [../]
 [./rebar_trans_z]
   type = EqualValueEmbeddedConstraint
   secondary = 3
   primary = 1
   penalty = 1e12
   variable = 'disp_z'
   primary_variable = 'disp_z'
   formulation = penalty
 [../]
[]

[Kernels]
 [./T_td]
   type     = ConcreteThermalTimeIntegration
   variable = T
   block = 1
 [../]
 [./T_diff]
   type     = ConcreteThermalConduction
   variable = T
   block = 1
 [../]

 [./T_conv]
   type     = ConcreteThermalConvection
   variable = T
   relative_humidity = rh
   block = 1
 [../]

 [./T_adsorption]
   type     = ConcreteLatentHeat
   variable = T
   H = rh
   block = 1
 [../]

 [./rh_td]
   type     = ConcreteMoistureTimeIntegration
   variable = rh
   block = 1
 [../]

 [./rh_diff]
   type     = ConcreteMoistureDiffusion
   variable = rh
   temperature = T
   block = 1
 [../]
 [./heat_dt]
   type = TimeDerivative
   variable = T
   block = '2 3 4'
 [../]
 [./heat_conduction]
   type = HeatConduction
   variable = T
   diffusion_coefficient = 53.0
   block = '2 3 4'
 [../]
[]

[AuxKernels]
 [./ASR_ex]
   type = MaterialRealAux
   variable = ASR_ex
   block = 1
   property = ASR_extent
   execute_on = 'timestep_end'
 [../]
 [./ASR_vstrain]
   type = MaterialRealAux
   block = 1
   variable = ASR_vstrain
   property = ASR_volumetric_strain
   execute_on = 'timestep_end'
 [../]

 [./ASR_strain_xx]
   type = RankTwoAux
   block = 1
   rank_two_tensor = asr_expansion
   variable = ASR_strain_xx
   index_i = 0
   index_j = 0
   execute_on = 'timestep_end'
 [../]
 [./ASR_strain_yy]
   type = RankTwoAux
   block = 1
   rank_two_tensor = asr_expansion
   variable = ASR_strain_yy
   index_i = 1
   index_j = 1
   execute_on = 'timestep_end'
 [../]
 [./ASR_strain_zz]
   type = RankTwoAux
   block = 1
   rank_two_tensor = asr_expansion
   variable = ASR_strain_zz
   index_i = 2
   index_j = 2
   execute_on = 'timestep_end'
 [../]

 [./ASR_strain_xy]
   type = RankTwoAux
   block = 1
   rank_two_tensor = asr_expansion
   variable = ASR_strain_xy
   index_i = 0
   index_j = 1
   execute_on = 'timestep_end'
 [../]

 [./ASR_strain_yz]
   type = RankTwoAux
   block = 1
   rank_two_tensor = asr_expansion
   variable = ASR_strain_yz
   index_i = 1
   index_j = 2
   execute_on = 'timestep_end'
 [../]

 [./ASR_strain_zx]
   type = RankTwoAux
   block = 1
   rank_two_tensor = asr_expansion
   variable = ASR_strain_zx
   index_i = 0
   index_j = 2
   execute_on = 'timestep_end'
 [../]
 [./thermal_strain_xx]
   type = RankTwoAux
   block = 1
   rank_two_tensor = thermal_expansion
   variable = thermal_strain_xx
   index_i = 0
   index_j = 0
   execute_on = 'timestep_end'
 [../]
 [./thermal_strain_yy]
   type = RankTwoAux
   block = 1
   rank_two_tensor = thermal_expansion
   variable = thermal_strain_yy
   index_i = 1
   index_j = 1
   execute_on = 'timestep_end'
 [../]
 [./thermal_strain_zz]
   type = RankTwoAux
   block = 1
   rank_two_tensor = thermal_expansion
   variable = thermal_strain_zz
   index_i = 2
   index_j = 2
   execute_on = 'timestep_end'
 [../]

 [./volumetric_strain]
   type = RankTwoScalarAux
   scalar_type = VolumetricStrain
   rank_two_tensor = total_strain
   variable = volumetric_strain
   block = 1
 [../]

 [./k]
   type = MaterialRealAux
   variable = thermal_conductivity
   property = thermal_conductivity
   execute_on = 'timestep_end'
   block = 1
 [../]
 [./capacity]
   type = MaterialRealAux
   variable = thermal_capacity
   property = thermal_capacity
   execute_on = 'timestep_end'
   block = 1
 [../]

 [./rh_capacity]
   type = MaterialRealAux
   variable = moisture_capacity
   property = moisture_capacity
   execute_on = 'timestep_end'
   block = 1
 [../]
 [./rh_duff]
   type = MaterialRealAux
   variable = humidity_diffusivity
   property = humidity_diffusivity
   execute_on = 'timestep_end'
   block = 1
 [../]
 [./wc_duff]
   type = MaterialRealAux
   variable = water_content
   property = moisture_content
   execute_on = 'timestep_end'
   block = 1
 [../]
 [./hydrw_duff]
   type = MaterialRealAux
   variable = water_hydrated
   property = hydrated_water
   execute_on = 'timestep_end'
   block = 1
 [../]

 [damage_index]
   type = MaterialRealAux
   block = 1
   variable = damage_index
   property = damage_index
   execute_on = timestep_end
 []
 [./area_long]
   type = ConstantAux
   block = 2
   variable = area_long
   value = 0.00100716
   execute_on = 'initial timestep_begin'
 [../]
 [./area_trans]
   type = ConstantAux
   block = 3
   variable = area_trans
   value = 0.00019793
   execute_on = 'initial timestep_begin'
 [../]
 [./area_headed]
   type = ConstantAux
   block = 4
   variable = area_headed
   value = 0.00064467
   execute_on = 'initial timestep_begin'
 [../]
 [./axial_stress]
   type = MaterialRealAux
   block = '2 3 4'
   variable = axial_stress
   property = axial_stress
 [../]
[]

[Functions]
 [./ramp_temp]
   type = PiecewiseLinear
   x = '2291660.465	2761759.674	3366507.473	4105903.863	4441647.196	4845007.538	5048736.71	5252173.167	5388870.757	5729004.805	5864824.253	6068553.424	6201153.014	6332874.461	6530749.346	6661885.365	6861223.821	7262827.878	7664724.649	7867868.391	8140092.715	8344114.601	8548721.916	8685419.506	8957058.402	9160202.145	9495067.334	9829639.81	10097180.7	10297690.02	10566694.49	10970054.83	11306090.87	11642126.92	12045779.98	12449140.32	12718144.79	13188536.71	13591311.62	13860316.09	14129905.99	14399495.88	14669378.49	14938968.39	15141526.7	15411702.02	15816818.65	16086993.98	16424786.31	16560605.76	16696717.92	16967478.67	17171207.84	17374937.01	17578666.18	17715363.78	17785029.79	17921141.95	17990222.53	18126627.4	18195707.99	18264788.57	18401193.44	18470274.02	18539061.89	18607849.76	18744254.63	18879488.65	19215524.7	19551560.75	19820272.5	20021659.96	20157772.12	20428825.59	20565815.89	20769252.35	20973566.95	21109679.11	21245498.56	21378683.58	21578022.03	21710036.19	21842635.78	21974942.66	22107249.54	22306587.99	22510609.88	22714631.76	22851329.35	22987441.52	23123846.39	23192926.97	23464565.87	23601263.46	23737668.34	23941397.51	24077216.96	24142492.25	24274213.7	24403593.43	24467990.58	24532680.45	24664401.89	24729677.19	24794952.48	24860227.78	25064835.09	25134793.82	25272076.84	25342328.28	25479904.01	25616601.6	25754177.33	25824428.77	25961419.08	26030792.37	26365364.85	26632613.03	26966307.36	27233555.54	27433772.14	27633696.03	27833619.91	28033836.51	28367530.84	28634193.6	28767378.61	29034919.51	29239234.11	29376224.41	29444426.85	29781633.76	30253781.97	30860871.48	31196029.38	31732282.03	32536953.72	33140237.94	33676783.3	34414423.41	34614932.72	35015658.64	35283492.24	35484001.56	35684218.16	36154902.8	36625294.72	36894006.47	37231213.38	37703654.3	38108185.5	38375433.68	38642389.15	38908466.47	39309777.81	39513214.27	39716650.72	39920965.32	40124694.5	40329009.1	40532738.27	40733247.58	40933464.18	41133388.07	41467667.83	41667884.43	41934839.89	42202380.79	42402890.1	42740682.44	43079352.91	43417437.96	43688491.43	44026283.76	44230012.93	44500773.68	44704210.14	44907353.88	45178114.64	45517370.54	45855455.59	46262035.79	46668323.27'
   y = '28.88	29.40	29.92	30.45	30.84	31.10	30.32	29.67	28.75	27.19	26.67	25.89	26.80	28.10	29.93	31.50	32.67	33.71	34.63	34.11	32.80	31.89	30.72	29.80	28.76	28.24	29.02	29.94	30.72	31.37	31.51	31.77	32.03	32.29	32.42	32.68	32.81	33.21	33.73	33.86	33.73	33.60	33.34	33.21	32.95	32.56	32.04	31.65	31.13	30.61	29.96	29.30	28.52	27.74	26.96	26.04	25.00	24.35	23.57	22.78	22.00	21.22	20.44	19.66	19.00	18.35	17.57	17.31	17.57	17.83	18.09	18.35	17.70	16.92	15.88	15.23	14.18	13.53	13.01	13.66	14.84	16.01	16.92	17.97	19.01	20.19	19.27	18.36	17.45	16.80	16.01	15.23	14.19	13.28	12.49	11.71	11.19	12.10	13.41	15.76	17.06	18.24	19.54	20.45	21.37	22.28	21.11	19.93	18.76	17.46	16.15	15.24	13.93	12.63	11.59	10.67	11.59	12.50	13.81	14.72	15.50	16.42	17.33	18.11	19.42	20.59	21.25	22.03	20.99	19.94	19.55	19.29	18.90	18.38	19.04	20.08	21.52	22.69	23.61	24.91	25.57	27.00	27.65	28.31	29.09	29.35	29.74	30.01	29.75	29.23	28.97	29.88	30.92	32.36	33.53	32.88	32.23	31.19	30.41	29.36	28.58	29.23	30.02	30.93	31.97	32.76	33.80	34.58	35.24	34.72	33.80	33.15	32.37	31.85	31.07	30.42	29.76	29.24	28.59	27.42	26.77	25.59	24.55'
 [../]
[]

[Materials]
 [./concrete]
   type = PorousMediaBase
   block = 1
   # setup thermal property models and parameters
   # options available: CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004 KIM-2003
   thermal_conductivity_model =  KODUR-2004
   thermal_capacity_model     =  KODUR-2004
   aggregate_type = Siliceous               #options: Siliceous Carbonate

   ref_density_of_concrete = 2231.0         # in kg/m^3
   ref_specific_heat_of_concrete = 1100.0   # in J/(Kg.0C)
   ref_thermal_conductivity_of_concrete = 3 # in W/(m.0C)


   # setup moisture capacity and humidity diffusivity models
   aggregate_pore_type = dense              #options: dense porous
   aggregate_mass = 1877.0                  #mass of aggregate (kg) per m^3 of concrete
   cement_type = 1                          #options: 1 2 3 4
   cement_mass = 354.0                      #mass of cement (kg) per m^3 of concrete
   water_to_cement_ratio       = 0.5
   concrete_cure_time          = 28.0       #curing time in (days)

   # options available for humidity diffusivity:
   moisture_diffusivity_model = Bazant      #options: Bazant Xi Mensi
   D1 = 3.0e-8
   aggregate_vol_fraction = 0.7             #used in Xi's moisture diffusivity model

   coupled_moisture_diffusivity_factor = 1.0e-2  # factor for mositure diffusivity due to heat

   # coupled nonlinear variables
   relative_humidity = rh
   temperature = T
 [../]

 [./creep]
   type = LinearViscoelasticStressUpdate
   block = 1
 [../]
 [./logcreep]
   type = ConcreteLogarithmicCreepModel
   block = 1
   poissons_ratio = 0.22
   youngs_modulus = 37.3e9
   recoverable_youngs_modulus = 37.3e9
   recoverable_viscosity = 1
   long_term_viscosity = 1
   long_term_characteristic_time = 1
   humidity = rh
   temperature = T
   activation_temperature = 23.0
 [../]

 [ASR_expansion]
   type = ConcreteASREigenstrain
   block = 1
   expansion_type = Anisotropic

   reference_temperature  = 35.0                # parameter to play
   temperature_unit = Celsius
   max_volumetric_expansion = 1.5e-2            # parameter to play

   characteristic_time = 200                   # parameter to play
   latency_time = 200                          # parameter to play
   characteristic_activation_energy = 5400.0
   latency_activation_energy = 9400.0
   stress_latency_factor = 1.0

   compressive_strength = 33.0e6
   compressive_stress_exponent = 0.0
   expansion_stress_limit = 8.0e6

   tensile_strength = 3.2e6
   tensile_retention_factor = 1.0
   tensile_absorption_factor = 1.0

   ASR_dependent_tensile_strength = false
   residual_tensile_strength_fraction = 1.0

   temperature = T
   relative_humidity = rh
   rh_exponent = 1.0
   eigenstrain_name = asr_expansion
   absolute_tolerance = 1e-10
   output_iteration_info_on_error = true
 []

 [thermal_strain_concrete]
   type = ComputeThermalExpansionEigenstrain
   block = 1
   temperature = T
   thermal_expansion_coeff = 8.0e-6
   stress_free_temperature = 10.6
   eigenstrain_name = thermal_expansion
 []

 [ASR_damage_concrete]
   type = ConcreteASRMicrocrackingDamage
   residual_youngs_modulus_fraction = 0.1
   block = 1
   # outputs = Exodus
 []

 [./stress]
   type = ComputeMultipleInelasticStress
   block = 1
   inelastic_models = 'creep'
   damage_model = ASR_damage_concrete
   # outputs = Exodus
 [../]

 [truss]
   type = LinearElasticTruss
   block = '2 3 4'
   youngs_modulus = 2e11
   temperature = T
   thermal_expansion_coeff = 11.3e-6
   temperature_ref = 10.6
   # outputs = exodus
 []
[]

[UserObjects]
 [./visco_update]
   type = LinearViscoelasticityManager
   block = 1
   viscoelastic_model = logcreep
 [../]
[]

[BCs]
 [./x_disp]
   type = DirichletBC
   variable = disp_x
   boundary = 4
   value    = 0.0
 [../]
 [./y_disp]
   type = DirichletBC
   variable = disp_y
   boundary = '4 5'
   value    = 0.0
 [../]
 [./z_disp]
   type = DirichletBC
   variable = disp_z
   boundary = '4 5 6'
   value    = 0.0
 [../]
 [./T_zone_A_B_C]
   type = FunctionDirichletBC
   variable = T
   boundary = '1 2 3'
   function = ramp_temp
 [../]
 [./rh_zone_A_B]
   type = DirichletBC
   variable = rh
   boundary = '1 2'
   value    = 0.8
 [../]
 [./rh_zone_C]
   type = DirichletBC
   variable = rh
   boundary = '3'
   value    = 0.95
 [../]
[]

[Postprocessors]
 [./ASR_strain]
   type = ElementAverageValue
   variable = ASR_vstrain
   block = 1
 [../]
 [./ASR_strain_xx]
   type = ElementAverageValue
   variable = ASR_strain_xx
   block = 1
 [../]
 [./ASR_strain_yy]
   type = ElementAverageValue
   variable = ASR_strain_yy
   block = 1
 [../]
 [./ASR_strain_zz]
   type = ElementAverageValue
   variable = ASR_strain_zz
   block = 1
 [../]
 [ASR_ext]
   type = ElementAverageValue
   variable = ASR_ex
   block = 1
 []
 [./vonmises]
   type = ElementAverageValue
   variable = vonmises_stress
   block = 1
 [../]
 [./vstrain]
   type = ElementAverageValue
   variable = volumetric_strain
   block = 1
 [../]
 [./strain_xx]
   type = ElementAverageValue
   variable = strain_xx
   block = 1
 [../]
 [./strain_yy]
   type = ElementAverageValue
   variable = strain_yy
   block = 1
 [../]
 [./strain_zz]
   type = ElementAverageValue
   variable = strain_zz
   block = 1
 [../]
 [./temp]
   type = ElementAverageValue
   variable = T
   block = 1
 [../]
 [./humidity]
   type = ElementAverageValue
   variable = rh
   block = 1
 [../]
 [./thermal_strain_xx]
   type = ElementAverageValue
   variable = thermal_strain_xx
   block = 1
 [../]
 [./thermal_strain_yy]
   type = ElementAverageValue
   variable = thermal_strain_yy
   block = 1
 [../]
 [./thermal_strain_zz]
   type = ElementAverageValue
   variable = thermal_strain_zz
   block = 1
 [../]

 [./AZ1]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-0.0025 -0.455 -0.125'
  point2 = '-0.0025 -0.455 +0.105'
 [../]
 [./AZ2]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-0.0025 +0.455 -0.125'
  point2 = '-0.0025 +0.455 +0.105'
 [../]
 [./AX1]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+0.2225 -0.225 -0.255'
  point2 = '-0.2275 -0.225 -0.255'
 [../]
 [./AX2]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+0.2225 +0.225 -0.255'
  point2 = '-0.2275 +0.225 -0.255'
 [../]
 [./AX3]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+0.2225 -0.225 +0.255'
  point2 = '-0.2275 -0.225 +0.255'
 [../]
 [./AX4]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+0.2225 +0.225 +0.255'
  point2 = '-0.2275 +0.225 +0.255'
 [../]
 [./AY1]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-0.2275 +0.225 -0.255'
  point2 = '-0.2275 -0.225 -0.255'
 [../]
 [./AY2]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+0.2225 +0.225 -0.255'
  point2 = '+0.2225 -0.225 -0.255'
 [../]
 [./AY3]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-0.2275 +0.225 +0.255'
  point2 = '-0.2275 -0.225 +0.255'
 [../]
 [./AY4]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+0.2225 +0.225 +0.255'
  point2 = '+0.2225 -0.225 +0.255'
 [../]

 [./BZ1]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+2.5925 -0.455 -0.125'
  point2 = '+2.5925 -0.455 +0.105'
 [../]
 [./BZ2]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+2.5925 +0.455 -0.125'
  point2 = '+2.5925 +0.455 +0.105'
 [../]
 [./BX1]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+2.8175 -0.225 -0.255'
  point2 = '+2.3675 -0.225 -0.255'
 [../]
 [./BX2]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+2.8175 +0.225 -0.255'
  point2 = '+2.3675 +0.225 -0.255'
 [../]
 [./BX3]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+2.8175 -0.225 +0.255'
  point2 = '+2.3675 -0.225 +0.255'
 [../]
 [./BX4]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+2.8175 +0.225 +0.255'
  point2 = '+2.3675 +0.225 +0.255'
 [../]
 [./BY1]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+2.3675 +0.225 -0.255'
  point2 = '+2.3675 -0.225 -0.255'
 [../]
 [./BY2]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+2.8175 +0.225 -0.255'
  point2 = '+2.8175 -0.225 -0.255'
 [../]
 [./BY3]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+2.3675 +0.225 +0.255'
  point2 = '+2.3675 -0.225 +0.255'
 [../]
 [./BY4]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '+2.8175 +0.225 +0.255'
  point2 = '+2.8175 -0.225 +0.255'
 [../]

 [./CZ1]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-2.5925 -0.455 -0.125'
  point2 = '-2.5925 -0.455 +0.105'
 [../]
 [./CZ2]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-2.5925 +0.455 -0.125'
  point2 = '-2.5925 +0.455 +0.105'
 [../]
 [./CX1]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-2.3675 -0.225 -0.255'
  point2 = '-2.8175 -0.225 -0.255'
 [../]
 [./CX2]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-2.3675 +0.225 -0.255'
  point2 = '-2.8175 +0.225 -0.255'
 [../]
 [./CX3]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-2.3675 -0.225 +0.255'
  point2 = '-2.8175 -0.225 +0.255'
 [../]
 [./CX4]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-2.3675 +0.225 +0.255'
  point2 = '-2.8175 +0.225 +0.255'
 [../]
 [./CY1]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-2.3675 +0.225 -0.255'
  point2 = '-2.3675 -0.225 -0.255'
 [../]
 [./CY2]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-2.8175 +0.225 -0.255'
  point2 = '-2.8175 -0.225 -0.255'
 [../]
 [./CY3]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-2.3675 +0.225 +0.255'
  point2 = '-2.3675 -0.225 +0.255'
 [../]
 [./CY4]
  type = PointsSeparation
  variable1 = disp_x
  variable2 = disp_y
  variable3 = disp_z
  point1 = '-2.8175 +0.225 +0.255'
  point2 = '-2.8175 -0.225 +0.255'
 [../]
[]

#[Preconditioning]
#  [./SMP]
#    type = SMP
#    full = true
#  [../]
#[]

[Executioner]
 type       = Transient
 solve_type = 'PJFNK'
 line_search = none
 petsc_options = '-ksp_snes_ew'
 # petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
 # petsc_options_value = 'lu      superlu_dist'
 # petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart  -snes_ls  -pc_hypre_boomeramg_strong_threshold'
 # petsc_options_value = 'lu       boomeramg      201                 cubic     0.7'

 petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart -snes_ls -pc_hypre_boomeramg_strong_threshold'
 petsc_options_value = 'hypre boomeramg 201 cubic 0.7'
 start_time = 2419200 # 28 days
 dt = 86400 # 1 day
 automatic_scaling = true
 end_time = 47520000 # 550 days
 l_max_its  = 100
 l_tol      = 1e-4
 nl_max_its = 20
 nl_rel_tol = 1e-8
 nl_abs_tol = 1e-10
[]

[Outputs]
 perf_graph     = true
 csv = true
 [./Console]
   type = Console
 [../]
 [./Exo]
   type = Exodus
   elemental_as_nodal = true
 [../]
[]

[Debug]
 show_var_residual_norms = true
[]
