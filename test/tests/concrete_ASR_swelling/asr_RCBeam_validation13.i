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
 file = gold/mesh_WaldBeam1.e
 construct_side_list_from_node_list = true
 # block 1      : zone A
 # block 2      : Zone B
 # block 3      : Zone C
 # block 4      : long rebar
 # block 5      : trans rebar
 # block 6      : headed rebar
 # nodeset 1    : Zone A
 # nodeset 2    : Zone B
 # nodeset 3    : Zone C
 # nodeset 4    : BC @(front, left,  bottom)  for (x, y, and z)
 # nodeset 5    : BC @(back,  right, bottom)  for (y and z)
 # nodeset 6    : BC @(front, right, bottom)  for (z)
 # nodeset 7    : Ends of headed reinforcement
 # nodeset 8    : surface xy in zone A
 # nodeset 9    : surface z in zone A
 # nodeset 10   : surface xy in zone B
 # nodeset 11   : surface z in zone B
 # nodeset 12   : surface xy in zone C
 # nodeset 13   : surface z in zone C
 # [./strain]
 #   type = ComputeIncrementalSmallStrain
 #   displacements = 'disp_x disp_y disp_z'
 # [../]
 #
 # for PointsSeparation
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
   block = '5 6 7'
 [../]
 [./ASR_vstrain]
   order = CONSTANT
   family = MONOMIAL
   block = '5 6 7'
 [../]
 [./ASR_strain_xx]
   order = CONSTANT
   family = MONOMIAL
   block = '5 6 7'
 [../]
 [./ASR_strain_yy]
   order = CONSTANT
   family = MONOMIAL
   block = '5 6 7'
 [../]
 [./ASR_strain_zz]
   order = CONSTANT
   family = MONOMIAL
   block = '5 6 7'
 [../]
 [./ASR_strain_xy]
   order = CONSTANT
   family = MONOMIAL
   block = '5 6 7'
 [../]
 [./ASR_strain_yz]
   order = CONSTANT
   family = MONOMIAL
   block = '5 6 7'
 [../]
 [./ASR_strain_zx]
   order = CONSTANT
   family = MONOMIAL
   block = '5 6 7'
 [../]
 [./volumetric_strain]
   order = CONSTANT
   family = MONOMIAL
   block = '5 6 7'
 [../]
 [./thermal_strain_xx]
   order = CONSTANT
   family = MONOMIAL
   block = '5 6 7'
 [../]
 [./thermal_strain_yy]
   order = CONSTANT
   family = MONOMIAL
   block = '5 6 7'
 [../]
 [./thermal_strain_zz]
   order = CONSTANT
   family = MONOMIAL
   block = '5 6 7'
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
   block = '5 6 7'
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
   block = '5 6 7'
 [../]
 [./T_diff]
   type     = ConcreteThermalConduction
   variable = T
   block = '5 6 7'
 [../]

 [./T_conv]
   type     = ConcreteThermalConvection
   variable = T
   relative_humidity = rh
   block = '5 6 7'
 [../]

 [./T_adsorption]
   type     = ConcreteLatentHeat
   variable = T
   H = rh
   block = '5 6 7'
 [../]

 [./rh_td]
   type     = ConcreteMoistureTimeIntegration
   variable = rh
   block = '5 6 7'
 [../]

 [./rh_diff]
   type     = ConcreteMoistureDiffusion
   variable = rh
   temperature = T
   block = '5 6 7'
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
   block = '5 6 7'
   property = ASR_extent
   execute_on = 'timestep_end'
 [../]
 [./ASR_vstrain]
   type = MaterialRealAux
   block = '5 6 7'
   variable = ASR_vstrain
   property = ASR_volumetric_strain
   execute_on = 'timestep_end'
 [../]

 [./ASR_strain_xx]
   type = RankTwoAux
   block = '5 6 7'
   rank_two_tensor = asr_expansion
   variable = ASR_strain_xx
   index_i = 0
   index_j = 0
   execute_on = 'timestep_end'
 [../]
 [./ASR_strain_yy]
   type = RankTwoAux
   block = '5 6 7'
   rank_two_tensor = asr_expansion
   variable = ASR_strain_yy
   index_i = 1
   index_j = 1
   execute_on = 'timestep_end'
 [../]
 [./ASR_strain_zz]
   type = RankTwoAux
   block = '5 6 7'
   rank_two_tensor = asr_expansion
   variable = ASR_strain_zz
   index_i = 2
   index_j = 2
   execute_on = 'timestep_end'
 [../]

 [./ASR_strain_xy]
   type = RankTwoAux
   block = '5 6 7'
   rank_two_tensor = asr_expansion
   variable = ASR_strain_xy
   index_i = 0
   index_j = 1
   execute_on = 'timestep_end'
 [../]

 [./ASR_strain_yz]
   type = RankTwoAux
   block = '5 6 7'
   rank_two_tensor = asr_expansion
   variable = ASR_strain_yz
   index_i = 1
   index_j = 2
   execute_on = 'timestep_end'
 [../]

 [./ASR_strain_zx]
   type = RankTwoAux
   block = '5 6 7'
   rank_two_tensor = asr_expansion
   variable = ASR_strain_zx
   index_i = 0
   index_j = 2
   execute_on = 'timestep_end'
 [../]
 [./thermal_strain_xx]
   type = RankTwoAux
   block = '5 6 7'
   rank_two_tensor = thermal_expansion
   variable = thermal_strain_xx
   index_i = 0
   index_j = 0
   execute_on = 'timestep_end'
 [../]
 [./thermal_strain_yy]
   type = RankTwoAux
   block = '5 6 7'
   rank_two_tensor = thermal_expansion
   variable = thermal_strain_yy
   index_i = 1
   index_j = 1
   execute_on = 'timestep_end'
 [../]
 [./thermal_strain_zz]
   type = RankTwoAux
   block = '5 6 7'
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
   block = '5 6 7'
 [../]

 [./k]
   type = MaterialRealAux
   variable = thermal_conductivity
   property = thermal_conductivity
   execute_on = 'timestep_end'
   block = '5 6 7'
 [../]
 [./capacity]
   type = MaterialRealAux
   variable = thermal_capacity
   property = thermal_capacity
   execute_on = 'timestep_end'
   block = '5 6 7'
 [../]

 [./rh_capacity]
   type = MaterialRealAux
   variable = moisture_capacity
   property = moisture_capacity
   execute_on = 'timestep_end'
   block = '5 6 7'
 [../]
 [./rh_duff]
   type = MaterialRealAux
   variable = humidity_diffusivity
   property = humidity_diffusivity
   execute_on = 'timestep_end'
   block = '5 6 7'
 [../]
 [./wc_duff]
   type = MaterialRealAux
   variable = water_content
   property = moisture_content
   execute_on = 'timestep_end'
   block = '5 6 7'
 [../]
 [./hydrw_duff]
   type = MaterialRealAux
   variable = water_hydrated
   property = hydrated_water
   execute_on = 'timestep_end'
   block = '5 6 7'
 [../]

 [damage_index]
   type = MaterialRealAux
   block = '5 6 7'
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
   x = '2291660.465	2761759.674	3366507.473	4105903.863	4441647.196	4845007.538	5048736.71	5252173.167	5388870.757	5729004.805	5864824.253	6068553.424	6201153.014	6332874.461	6530749.346	6661885.365	6861223.821	7262827.878	7664724.649	7867868.391	8140092.715	8344114.601	8548721.916	8685419.506	8957058.402	9160202.145	9495067.334	9829639.81	10097180.7	10297690.02	10566694.49	10970054.83	11306090.87	11642126.92	12045779.98	12449140.32	12718144.79	13188536.71	13591311.62	13860316.09	14129905.99	14399495.88	14669378.49	14938968.39	15141526.7	15411702.02	15816818.65	16086993.98  16424786.31	16560605.76	16696717.92	16967478.67	17171207.84	17374937.01	17578666.18	17715363.78	17785029.79	17921141.95	17990222.53	18126627.4	18195707.99	18264788.57	18401193.44	18470274.02	18539061.89	18607849.76	18744254.63	18879488.65	19215524.7	19551560.75	19820272.5	20021659.96	20157772.12	20428825.59	20565815.89	20769252.35	20973566.95	21109679.11	21245498.56	21378683.58	21578022.03	21710036.19	21842635.78	21974942.66	22107249.54	22306587.99	22510609.88	22714631.76	22851329.35	22987441.52	23123846.39	23192926.97	23464565.87	23601263.46	23737668.34	23941397.51	24077216.96	24142492.25	24274213.7	24403593.43	24467990.58	24532680.45	24664401.89	24729677.19	24794952.48	24860227.78	25064835.09	25134793.82	25272076.84	25342328.28	25479904.01	25616601.6	25754177.33	25824428.77	25961419.08	26030792.37	26365364.85	26632613.03	26966307.36	27233555.54	27433772.14	27633696.03	27833619.91	28033836.51	28367530.84	28634193.6	28767378.61	29034919.51	29239234.11	29376224.41	29444426.85	29781633.76	30253781.97	30860871.48	31196029.38	31732282.03	32536953.72	33140237.94	33676783.3	34414423.41	34614932.72	35015658.64	35283492.24	35484001.56	35684218.16	36154902.8	36625294.72	36894006.47	37231213.38	37703654.3	38108185.5	38375433.68	38642389.15	38908466.47	39309777.81	39513214.27	39716650.72	39920965.32	40124694.5	40329009.1	40532738.27	40733247.58	40933464.18	41133388.07	41467667.83	41667884.43	41934839.89	42202380.79	42402890.1	42740682.44	43079352.91	43417437.96	43688491.43	44026283.76	44230012.93	44500773.68	44704210.14	44907353.88	45178114.64	45517370.54	45855455.59	46262035.79	46668323.27'
   y = '28.88	29.40	29.92	30.45	30.84	31.10	30.32	29.67	28.75	27.19	26.67	25.89	26.80	28.10	29.93	31.50	32.67	33.71	34.63	34.11	32.80	31.89	30.72	29.80	28.76	28.24	29.02	29.94	30.72	31.37	31.51	31.77	32.03	32.29	32.42	32.68	32.81	33.21	33.73	33.86	33.73	33.60	33.34	33.21	32.95	32.56	32.04	31.65	31.13	30.61	29.96	29.30	28.52	27.74	26.96	26.04	25.00	24.35	23.57	22.78	22.00	21.22	20.44	19.66	19.00	18.35	17.57	17.31	17.57	17.83	18.09	18.35	17.70	16.92	15.88	15.23	14.18	13.53	13.01	13.66	14.84	16.01	16.92	17.97	19.01	20.19	19.27	18.36	17.45	16.80	16.01	15.23	14.19	13.28	12.49	11.71	11.19	12.10	13.41	15.76	17.06	18.24	19.54	20.45	21.37	22.28	21.11	19.93	18.76	17.46	16.15	15.24	13.93	12.63	11.59	10.67	11.59	12.50	13.81	14.72	15.50	16.42	17.33	18.11	19.42	20.59	21.25	22.03	20.99	19.94	19.55	19.29	18.90	18.38	19.04	20.08	21.52	22.69	23.61	24.91	25.57	27.00	27.65	28.31	29.09	29.35	29.74	30.01	29.75	29.23	28.97	29.88	30.92	32.36	33.53	32.88	32.23	31.19	30.41	29.36	28.58	29.23	30.02	30.93	31.97	32.76	33.80	34.58	35.24	34.72	33.80	33.15	32.37	31.85	31.07	30.42	29.76	29.24	28.59	27.42	26.77	25.59	24.55'
 [../]

 [./ramp_humidity]
   type = PiecewiseLinear
   x = ' 0.0 2552703.1 2981085.48 3185563.89 3198189.55 3200503.52 3251590.42 3278497.15 3603988.47 4299558.61 5001524.78 5154372.91 5269522.92 5373275.25 5561487.78 5825501.93 5831099.87 6000201.9
         6385613.81 6412574.34 6479141.58 6767796.91 6787129.39 6800502.03 6816524.98 6817373.77 6928125.1 7116508.04 7173227.41 7241893.37 7358202.17 7569863.27 8225348.79
         8765521.18 8838592.15 8996612.89 9709390.58 9782493.46 10503434.1 10506575.8 10552590.8 10572407.6 11139170.8 11245476.8 11811901.7 12262472.7 12408952.9 12476461.9
         12506462.9 12534876.4 12539870.3 12643471.9 13265824.4 13527525.9 14059475.9 14299769.1 14330173.7 14987693.3 15061005.1 15141685.8 15153266.5 15184184.1 15293030.8
         15330592.5 15369634.2 15408191.5 15646542.1 15648543.9 15695893.6 15728765.9 15750287.8 15761868.9 16336215.2 16371861.2 16380708.1 16381676.8 16384044.6 16410197.9
         16412565.7 16417516.5 16440634.8 16552965 16639066.5 16957803.6 16960171.3 16990845 17131154.6 17200367.7 17452180 17480188.6 17511029.7 17523131.7 17925593.5 18005605.2
         18115725.4 18535566 18574843.8 18576440.3 18588216.5 18732140.5 18998337.7 19030912.8 19096053.9 19108727 19117148.8 19140100.3 19815584.6 19866303.7 19925938 20067101.5
         20343960.9 20419676.4 20663612.7 20718717.7 20721291 20751328.7 20763544.3 20768312.2 20780172.7 20968915.7 21435961.3 21456177.2 21573631.2 21581024.4 21582239 22196937.2
         22237946.7 22249076.5 22421016.4 22811076.8 22867309.3 22988052.3 23021300.9 23043566.2 23085953.3 23317907.2 23513465.3 23515725.4 23553341 23682913 23941312.9 23945013.2
         23952403.6 24020495.6 24020759.7 24364726.7 24427684 24690107 24783381.9 24806989.8 24817924.7 24827528.1 24830237.2 24831087.5 24837240.2 24843895.1 24849997.5 24857904.5
         25354746.1 25449670 25565949.3 25589884 25697633.5 25970953.2 26065765.3 26079756.8 26081837.6 26084205.4 26104654.5 26104798 26137347.4 26303146.1 26317361.1 26355999.1
         26465778.6 27286299.1 27306631.6 27312833 27315600.5 27317968.3 27320336.1 27322596.3 27324081.5 28557938.3 28842180.4 28856710 28859077.8 28919779.4 28934596 29112682.6
         29115014.6 29141526.6 29143894.4 29369244.6 29381754.4 29437003.5 29519282.7 29917749.7 29919956.1 29922323.9 29964190.7 29968388.2 30022416.9 30027521.5 30091271.2
         30404815.2 30434843.1 30437456.9 30442834.4 30505267 30724054.5 30812021.6 30851144 31079815.2 31082183 31209084.1 31250843.3 31746233.8 31800251.7 31834423.3
         31844244.2 31846567.2 31849329.6 31851697.4 32047094 32432183 32444360.2 32800224 32868251.3 32874045.2 32955734.1 32958101.8 32966429.5 32975053.1 33014536.8
         33032396.7 33603414.8 33607737.8 33732997.5 33870170.6 34160536.4 34250339.5 34273248.7 34815150.1 34820262.4 34823679.5 34960580.9 35434220 35466902.7 35473073.3
         35489695.7 35566672.8 35583857.2 35586153.3 35624289.1 35962990.9 36001723.1 36121337 36504156.9 36560987.4 36627640.8 36644183 36717578.4 36764075.8 37279208
         37283734.3 37287138 37345824.3 37386248.9 37419236.6 37830511.2 37864665.9 37877619.5 38023692.3 38347787.6 38349455.8 38370846.7 38423572.3 '

   y = ' 0.8 0.89169302 0.89027882 0.82593274 0.82675632 0.87217886 0.70598167 0.76654505 0.64530995 0.70256115 0.73429153 0.64844646 0.80457322 0.73696759 0.57376336 0.44893942 0.51790949 0.4179535 0.50175632
         0.44647463 0.56077041 0.75598167 0.78476688 0.69656266 0.83573519 0.90021983 0.62992534 0.58696759 0.53133379 0.47640421 0.42147463 0.3678529 0.35438116 0.39109234
         0.25562956 0.31808155 0.28147463 0.31364012 0.4372694 0.49893942 0.35008379 0.38837604 0.29119294 0.33747975 0.40369294 0.59753097 0.52696759 0.23890421 0.47710843
         0.43364767 0.29119294 0.36091125 0.41971407 0.35470526 0.38077041 0.25791829 0.32869294 0.40787766 0.35422111 0.30133379 0.24809435 0.20034787 0.46654505 0.50246055
         0.56742534 0.62288308 0.6723197 0.71161548 0.75930159 0.80336598 0.91752013 0.85195753 0.74330562 0.60133379 0.49443238 0.79400984 0.84048872 0.65246055 0.69893942
         0.44541829 0.54851688 0.88309435 0.93661548 0.88062956 0.92710843 0.82781266 0.77640421 0.73221407 0.5649354 0.61372815 0.67358731 0.52147463 0.3554535 0.40633379
         0.46372815 0.52194411 0.59154505 0.38908026 0.44489012 0.63978449 0.72147463 0.89330562 0.7691859 0.66724928 0.83256618 0.93239012 0.63133379 0.5748197 0.69330562
         0.75217886 0.85612252 0.79929153 0.39724928 0.75749073 0.66225568 0.43569998 0.53520703 0.48851688 0.72133379 0.59192325 0.64154505 0.80457322 0.73615502 0.85044847
         0.67388912 0.53790656 0.57915069 0.62616947 0.68908026 0.74722916 0.44823519 0.573869 0.49004857 0.63485491 0.5316859 0.7010521 0.5066859 0.5510521 0.58802393 0.7466859
         0.88908026 0.79471407 0.93978449 0.8401869 0.64497015 0.73353449 0.68344646 0.87809435 0.53467886 0.36682674 0.58147463 0.64974425 0.82316477 0.41900984 0.77358731
         0.31971407 0.93048872 0.47781266 0.73823519 0.98831652 0.87966925 0.93503097 0.82898637 0.76992534 0.57358731 0.38062956 0.42147463 0.4679535 0.63555914 0.52147463
         0.7095028 0.98886358 0.83626336 0.89330562 0.94400984 0.56742534 0.61584083 0.73757121 0.79189717 0.83837604 0.88485491 0.92922111 0.67781266 0.97993571 0.83133379
         0.88274224 0.92922111 0.7179535 0.77499576 0.52992534 0.57569998 0.62851688 0.67499576 0.42248067 0.49268942 0.34974425 0.28147463 0.66830562 0.71161548 0.75809435
         0.87851688 0.82062956 0.61865773 0.91926135 0.56742534 0.4095028 0.29753097 0.34883882 0.45439717 0.51091125 0.59717886 0.6873197 0.753869 0.80034787 0.84682674
         0.941369 0.88432674 0.50837604 0.44647463 0.55612252 0.60862252 0.65422111 0.70844646 0.75492534 0.80288308 0.92710843 0.86553902 0.43612252 0.36865773 0.48239012
         0.68309435 0.72957322 0.54233731 0.59471407 0.63495552 0.7851366 0.92734318 0.85633379 0.97710843 0.79732976 0.71883379 0.64461346 0.59330562 0.94330562 0.80985491
         0.87693238 0.75862252 0.54800045 0.64400984 0.59813459 0.70175632 0.49823519 0.3679535 0.41302393 0.46020703 0.79612252 0.85501336 0.74805914 0.66408026 0.47034787
         0.51619294 0.42006618 0.57821507 0.38872815 0.91466712 0.76971407 0.83652745 0.72598167 0.67781266 0.62393942 0.42048872 0.44964365 0.50351688 0.56523721 0.61443238
         0.64717886 0.716369 0.79944244 '
 [../]
[]

[Materials]
 [./concrete]
   type = PorousMediaBase
   block = '5 6 7'
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
   block = '5 6 7'
 [../]
 [./logcreep]
   type = ConcreteLogarithmicCreepModel
   block = '5 6 7'
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
   block = '5 6 7'
   expansion_type = Anisotropic

   reference_temperature  = 35.0                # parameter to play
   temperature_unit = Celsius
   max_volumetric_expansion = 1.125e-2            # parameter to play

   characteristic_time = 100                   # parameter to play
   latency_time = 50                           # parameter to play
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
   block = '5 6 7'
   temperature = T
   thermal_expansion_coeff = 8.0e-6
   stress_free_temperature = 10.6
   eigenstrain_name = thermal_expansion
 []

 [ASR_damage_concrete]
   type = ConcreteASRMicrocrackingDamage
   residual_youngs_modulus_fraction = 0.1
   block = '5 6 7'
   # outputs = Exodus
 []

 [./stress]
   type = ComputeMultipleInelasticStress
   block = '5 6 7'
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
   block = '5 6 7'
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
   type = FunctionDirichletBC
   variable = rh
   boundary = '1 2'
   function = ramp_humidity
 [../]
 [./rh_zone_C]
   type = DirichletBC
   variable = rh
   boundary = '3'
   value    = 0.95
 [../]
[]

[Postprocessors]
 # [./ASR_strain]
 #   type = ElementAverageValue
 #   variable = ASR_vstrain
 #   block = '5 6 7'
 # [../]
 # [./ASR_strain_xx]
 #   type = ElementAverageValue
 #   variable = ASR_strain_xx
 #   block = '5 6 7'
 # [../]
 # [./ASR_strain_yy]
 #   type = ElementAverageValue
 #   variable = ASR_strain_yy
 #   block = '5 6 7'
 # [../]
 # [./ASR_strain_zz]
 #   type = ElementAverageValue
 #   variable = ASR_strain_zz
 #   block = '5 6 7'
 # [../]
 # [ASR_ext]
 #   type = ElementAverageValue
 #   variable = ASR_ex
 #   block = '5 6 7'
 # []
 # [./vonmises]
 #   type = ElementAverageValue
 #   variable = vonmises_stress
 #   block = '5 6 7'
 # [../]
 # [./vstrain_A]
 #   type = ElementAverageValue
 #   variable = volumetric_strain
 #   block = '5'
 # [../]
 # [./vstrain_B]
 #   type = ElementAverageValue
 #   variable = volumetric_strain
 #   block = '6'
 # [../]
 # [./vstrain_C]
 #   type = ElementAverageValue
 #   variable = volumetric_strain
 #   block = '7'
 # [../]
 # [./strain_xx]
 #   type = ElementAverageValue
 #   variable = strain_xx
 #   block = '5 6 7'
 # [../]
 # [./strain_yy]
 #   type = ElementAverageValue
 #   variable = strain_yy
 #   block = '5 6 7'
 # [../]
 # [./strain_zz]
 #   type = ElementAverageValue
 #   variable = strain_zz
 #   block = '5 6 7'
 # [../]
 # [./temp]
 #   type = ElementAverageValue
 #   variable = T
 #   block = '5 6 7'
 # [../]
 # [./humidity]
 #   type = ElementAverageValue
 #   variable = rh
 #   block = '5 6 7'
 # [../]
 # [./thermal_strain_xx]
 #   type = ElementAverageValue
 #   variable = thermal_strain_xx
 #   block = '5 6 7'
 # [../]
 # [./thermal_strain_yy]
 #   type = ElementAverageValue
 #   variable = thermal_strain_yy
 #   block = '5 6 7'
 # [../]
 # [./thermal_strain_zz]
 #   type = ElementAverageValue
 #   variable = thermal_strain_zz
 #   block = '5 6 7'
 # [../]

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

 # [./vstrain]
 #   type = ElementAverageValue
 #   variable = volumetric_strain
 #   block = '5 6 7'
 # [../]
 # [./B_vstrain]
 #   type = ElementAverageValue
 #   variable = volumetric_strain
 #   block = 6
 # [../]
 # [./C_vstrain]
 #   type = ElementAverageValue
 #   variable = volumetric_strain
 #   block = 7
 # [../]

 # [./Ax]
 #   type = SideAverageValue
 #   variable = disp_x
 #   boundary = 8
 # [../]
 # [./Ay]
 #   type = SideAverageValue
 #   variable = disp_y
 #   boundary = 8
 # [../]
 # [./Az]
 #   type = SideAverageValue
 #   variable = disp_z
 #   boundary = 9
 # [../]
 #
 # [./Bx]
 #   type = SideAverageValue
 #   variable = disp_x
 #   boundary = 10
 # [../]
 # [./By]
 #   type = SideAverageValue
 #   variable = disp_y
 #   boundary = 10
 # [../]
 # [./Bz]
 #   type = SideAverageValue
 #   variable = disp_z
 #   boundary = 11
 # [../]
 #
 # [./Cx]
 #   type = SideAverageValue
 #   variable = disp_x
 #   boundary = 12
 # [../]
 # [./Cy]
 #   type = SideAverageValue
 #   variable = disp_y
 #   boundary = 12
 # [../]
 # [./Cz]
 #   type = SideAverageValue
 #   variable = disp_z
 #   boundary = 13
 # [../]
[]

[Executioner]
 type       = Transient
 solve_type = 'PJFNK'
 line_search = none
 petsc_options = '-ksp_snes_ew'
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
