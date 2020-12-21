[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  block = 1
[]

[Mesh]
  file = concrete_block_mesh.e
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
  [../]
  [./ASR_vstrain]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ASR_strain_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ASR_strain_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ASR_strain_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ASR_strain_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ASR_strain_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ASR_strain_zx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./volumetric_strain]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./thermal_strain_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./thermal_strain_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./thermal_strain_zz]
    order = CONSTANT
    family = MONOMIAL
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
[]

[Modules/TensorMechanics/Master]
  [./concrete]
    strain = FINITE
    add_variables = true
    eigenstrain_names = 'asr_expansion thermal_expansion'
    generate_output = 'stress_xx stress_yy stress_zz stress_xy stress_yz stress_zx vonmises_stress hydrostatic_stress elastic_strain_xx elastic_strain_yy elastic_strain_zz strain_xx strain_yy strain_zz'
    save_in = 'resid_x resid_y resid_z'
  [../]
[]

[Kernels]
  [./T_td]
    type     = ConcreteThermalTimeIntegration
    variable = T
  [../]
  [./T_diff]
    type     = ConcreteThermalConduction
    variable = T
  [../]
  [./T_conv]
    type     = ConcreteThermalConvection
    variable = T
    relative_humidity = rh
  [../]
  [./T_adsorption]
    type     = ConcreteLatentHeat
    variable = T
    H = rh
  [../]
  [./rh_td]
    type     = ConcreteMoistureTimeIntegration
    variable = rh
  [../]
  [./rh_diff]
    type     = ConcreteMoistureDiffusion
    variable = rh
    temperature = T
  [../]
[]

[AuxKernels]
  [./ASR_ex]
    type = MaterialRealAux
    variable = ASR_ex
    property = ASR_extent
    execute_on = 'timestep_end'
  [../]
  [./ASR_vstrain]
    type = MaterialRealAux
    variable = ASR_vstrain
    property = ASR_volumetric_strain
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_xx]
    type = RankTwoAux
    rank_two_tensor = asr_expansion
    variable = ASR_strain_xx
    index_i = 0
    index_j = 0
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_yy]
    type = RankTwoAux
    rank_two_tensor = asr_expansion
    variable = ASR_strain_yy
    index_i = 1
    index_j = 1
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_zz]
    type = RankTwoAux
    rank_two_tensor = asr_expansion
    variable = ASR_strain_zz
    index_i = 2
    index_j = 2
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_xy]
    type = RankTwoAux
    rank_two_tensor = asr_expansion
    variable = ASR_strain_xy
    index_i = 0
    index_j = 1
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_yz]
    type = RankTwoAux
    rank_two_tensor = asr_expansion
    variable = ASR_strain_yz
    index_i = 1
    index_j = 2
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_zx]
    type = RankTwoAux
    rank_two_tensor = asr_expansion
    variable = ASR_strain_zx
    index_i = 0
    index_j = 2
    execute_on = 'timestep_end'
  [../]
  [./thermal_strain_xx]
    type = RankTwoAux
    rank_two_tensor = thermal_expansion
    variable = thermal_strain_xx
    index_i = 0
    index_j = 0
    execute_on = 'timestep_end'
  [../]
  [./thermal_strain_yy]
    type = RankTwoAux
    rank_two_tensor = thermal_expansion
    variable = thermal_strain_yy
    index_i = 1
    index_j = 1
    execute_on = 'timestep_end'
  [../]
  [./thermal_strain_zz]
    type = RankTwoAux
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
  [../]
  [./k]
    type = MaterialRealAux
    variable = thermal_conductivity
    property = thermal_conductivity
    execute_on = 'timestep_end'
  [../]
  [./capacity]
    type = MaterialRealAux
    variable = thermal_capacity
    property = thermal_capacity
    execute_on = 'timestep_end'
  [../]
  [./rh_capacity]
    type = MaterialRealAux
    variable = moisture_capacity
    property = moisture_capacity
    execute_on = 'timestep_end'
  [../]
  [./rh_duff]
    type = MaterialRealAux
    variable = humidity_diffusivity
    property = humidity_diffusivity
    execute_on = 'timestep_end'
  [../]
  [./wc_duff]
    type = MaterialRealAux
    variable = water_content
    property = moisture_content
    execute_on = 'timestep_end'
  [../]
  [./hydrw_duff]
    type = MaterialRealAux
    variable = water_hydrated
    property = hydrated_water
    execute_on = 'timestep_end'
  [../]
  [damage_index]
    type = MaterialRealAux
    variable = damage_index
    property = damage_index
    execute_on = timestep_end
  []
[]

[Functions]
  [./ramp_temp]
    type = PiecewiseLinear
    x = ' 0.0 2722893.36 3498624.13 4274354.9 5050085.66 5825816.43 6566286.71 7306756.99 8082487.76 8858218.53 9633949.3 10409680.1 11185410.8 11961141.6
          12736872.4 13512603.1 14288333.9 14923022.7 15557711.5 16333442.3 17109173.1 17708601.4 18308029.7 19083760.5 19859491.3 20635222 21234650.3 21834078.7
          22609809.4 23385540.2 24161271 24937001.7 25712732.5 26488463.3 27264194.1 28039924.8 28815655.6 29591386.4 30367117.1 31037066.4 31707015.7 32482746.5
          33258477.3 34034208 34668896.9 35303585.7 36079316.4 36855047.2 37630778 38312480.8'

    y = ' 10.6 32.9151617 31.7921103 33.1021308 35.8192291 36.7921373 36.7285394 40.745467 41.0148363 41.9877445 40.8207219 38.0853938 37.8710801 37.055827
          36.6216574 34.53124 33.1736756 30.544127 26.0594134 24.5113073 23.5348264 20.5026548 17.5926252 16.6747727 18.7029892 18.5619608 15.8253731 13.8362956
          15.1902873 15.6208842 17.6491007 18.3728389 20.8847384 20.3479693 22.0537305 23.0852669 23.8236621 25.0603974 26.0333055 27.3627238 29.2674317 29.302288
          29.0733172 30.2514242 32.2974778 35.5810061 38.0635915 36.7499983 38.0307047 37.1936084'
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
    # block = 1
  [../]
  [./logcreep]
    type = ConcreteLogarithmicCreepModel
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
    expansion_type = Anisotropic

    reference_temperature  = 35.0
    temperature_unit = Celsius
    max_volumetric_expansion = 2.2e-2

    characteristic_time = 18.9
    latency_time = 18.0
    characteristic_activation_energy = 5400.0
    latency_activation_energy = 9400.0
    stress_latency_factor = 1.0

    compressive_strength = 31.0e6
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
    temperature = T
    thermal_expansion_coeff = 8.0e-6
    stress_free_temperature = 10.6
    eigenstrain_name = thermal_expansion
  []

  [ASR_damage_concrete]
    type = ConcreteASRMicrocrackingDamage
    residual_youngs_modulus_fraction = 0.1
  []
  [./stress]
    type = ComputeMultipleInelasticStress
    inelastic_models = 'creep'
    damage_model = ASR_damage_concrete
  [../]
[]

[UserObjects]
  [./visco_update]
    type = LinearViscoelasticityManager
    viscoelastic_model = logcreep
  [../]
[]


[BCs]
  [./left]
    type = DirichletBC
    variable = disp_x
    boundary = '2000 2005'
    value = 0.0
  [../]
  [./bottom]
    type = DirichletBC
    variable = disp_y
    boundary = '2000 2001'
    value = 0.0
  [../]
  [./back]
    type = DirichletBC
    variable = disp_z
    boundary = '2000 2005'
    value = 0.0
  [../]
  [./T]
    type = FunctionDirichletBC
    variable = T
    boundary = '101 102 103 104 105 106'
    function = ramp_temp
  [../]
  [./rh]
    type = FunctionDirichletBC
    variable = rh
    boundary = '101 102 103 104 105 106'
    function = ramp_humidity
  [../]
[]

[Postprocessors]
  [./nelem]
    type = NumElems
  [../]
  [./ndof]
    type = NumDOFs
  [../]
  [./ASR_strain]
    type = ElementAverageValue
    variable = ASR_vstrain
  [../]
  [./ASR_strain_xx]
    type = ElementAverageValue
    variable = ASR_strain_xx
  [../]
  [./ASR_strain_yy]
    type = ElementAverageValue
    variable = ASR_strain_yy
  [../]
  [./ASR_strain_zz]
    type = ElementAverageValue
    variable = ASR_strain_zz
  [../]
  [ASR_ext]
    type = ElementAverageValue
    variable = ASR_ex
  []
  [./vonmises]
    type = ElementAverageValue
    variable = vonmises_stress
  [../]
  [./vstrain]
    type = ElementAverageValue
    variable = volumetric_strain
  [../]
  [./strain_xx]
    type = ElementAverageValue
    variable = strain_xx
  [../]
  [./strain_yy]
    type = ElementAverageValue
    variable = strain_yy
  [../]
  [./strain_zz]
    type = ElementAverageValue
    variable = strain_zz
  [../]

  [./temp]
    type = ElementAverageValue
    variable = T
  [../]
  [./temp_bc]
    type = SideAverageValue
    variable = T
    boundary = 102
  [../]
  [./humidity]
    type = ElementAverageValue
    variable = rh
  [../]
  [./humidity_bc]
    type = SideAverageValue
    variable = rh
    boundary = 102
  [../]
  [./thermal_strain_xx]
    type = ElementAverageValue
    variable = thermal_strain_xx
  [../]
  [./thermal_strain_yy]
    type = ElementAverageValue
    variable = thermal_strain_yy
  [../]
  [./thermal_strain_zz]
    type = ElementAverageValue
    variable = thermal_strain_zz
  [../]
  [./disp_x_101]
    type = SideAverageValue
    variable = disp_x
    boundary = 101
  [../]
  [./disp_x_102]
    type = SideAverageValue
    variable = disp_x
    boundary = 102
  [../]
  [./disp_x_103]
    type = SideAverageValue
    variable = disp_x
    boundary = 103
  [../]
  [./disp_x_104]
    type = SideAverageValue
    variable = disp_x
    boundary = 104
  [../]
  [./disp_x_105]
    type = SideAverageValue
    variable = disp_x
    boundary = 105
  [../]
  [./disp_x_106]
    type = SideAverageValue
    variable = disp_x
    boundary = 106
  [../]
  [./disp_y_101]
    type = SideAverageValue
    variable = disp_y
    boundary = 101
  [../]
  [./disp_y_102]
    type = SideAverageValue
    variable = disp_y
    boundary = 102
  [../]
  [./disp_y_103]
    type = SideAverageValue
    variable = disp_y
    boundary = 103
  [../]
  [./disp_y_104]
    type = SideAverageValue
    variable = disp_y
    boundary = 104
  [../]
  [./disp_y_105]
    type = SideAverageValue
    variable = disp_y
    boundary = 105
  [../]
  [./disp_y_106]
    type = SideAverageValue
    variable = disp_y
    boundary = 106
  [../]
  [./disp_z_101]
    type = SideAverageValue
    variable = disp_z
    boundary = 101
  [../]
  [./disp_z_102]
    type = SideAverageValue
    variable = disp_z
    boundary = 102
  [../]
  [./disp_z_103]
    type = SideAverageValue
    variable = disp_z
    boundary = 103
  [../]
  [./disp_z_104]
    type = SideAverageValue
    variable = disp_z
    boundary = 104
  [../]
  [./disp_z_105]
    type = SideAverageValue
    variable = disp_z
    boundary = 105
  [../]
  [./disp_z_106]
    type = SideAverageValue
    variable = disp_z
    boundary = 106
  [../]
  [disp_x_p1_pos]
    type = PointValue
    variable = disp_x
    point = '0.24 -0.08 -0.08'
  [../]
  [disp_x_p1_neg]
    type = PointValue
    variable = disp_x
    point = '-0.24 -0.08 -0.08'
  [../]
  [disp_x_p2_pos]
    type = PointValue
    variable = disp_x
    point = '0.24 -0.08 0.08'
  [../]
  [disp_x_p2_neg]
    type = PointValue
    variable = disp_x
    point = '-0.24 -0.08 0.08'
  [../]
  [disp_x_p3_pos]
    type = PointValue
    variable = disp_x
    point = '0.24 0.08 -0.08'
  [../]
  [disp_x_p3_neg]
    type = PointValue
    variable = disp_x
    point = '-0.24 0.08 -0.08'
  [../]
  [disp_x_p4_pos]
    type = PointValue
    variable = disp_x
    point = '0.24 0.08 0.08'
  [../]
  [disp_x_p4_neg]
    type = PointValue
    variable = disp_x
    point = '-0.24 0.08 0.08'
  [../]
  [disp_x_p5_pos]
    type = PointValue
    variable = disp_x
    point = '0.24 0.08 -0.235'
  [../]
  [disp_x_p5_neg]
    type = PointValue
    variable = disp_x
    point = '-0.24 0.08 -0.235'
  [../]
  [disp_x_p6_pos]
    type = PointValue
    variable = disp_x
    point = '0.24 0.08 0.235'
  [../]
  [disp_x_p6_neg]
    type = PointValue
    variable = disp_x
    point = '-0.24 0.08 0.235'
  [../]

  [disp_y_p1_pos]
    type = PointValue
    variable = disp_y
    point = '-0.08 0.24 -0.08'
  [../]
  [disp_y_p1_neg]
    type = PointValue
    variable = disp_y
    point = '-0.08 -0.24 -0.08'
  [../]
  [disp_y_p2_pos]
    type = PointValue
    variable = disp_y
    point = '-0.08 0.24 0.08'
  [../]
  [disp_y_p2_neg]
    type = PointValue
    variable = disp_y
    point = '-0.08 -0.24 0.08'
  [../]
  [disp_y_p3_pos]
    type = PointValue
    variable = disp_y
    point = '0.08 0.24 -0.08'
  [../]
  [disp_y_p3_neg]
    type = PointValue
    variable = disp_y
    point = '0.08 -0.24 -0.08'
  [../]
  [disp_y_p4_pos]
    type = PointValue
    variable = disp_y
    point = '0.08 0.24 0.08'
  [../]
  [disp_y_p4_neg]
    type = PointValue
    variable = disp_y
    point = '0.08 -0.24 0.08'
  [../]
  [disp_y_p5_pos]
    type = PointValue
    variable = disp_y
    point = '0.08 0.24 -0.235'
  [../]
  [disp_y_p5_neg]
    type = PointValue
    variable = disp_y
    point = '0.08 -0.24 -0.235'
  [../]
  [disp_y_p6_pos]
    type = PointValue
    variable = disp_y
    point = '0.08 0.24 0.235'
  [../]
  [disp_y_p6_neg]
    type = PointValue
    variable = disp_y
    point = '0.08 -0.24 0.235'
  [../]
  [disp_y_p7_pos]
    type = PointValue
    variable = disp_y
    point = '-0.235 0.24 0.08'
  [../]
  [disp_y_p7_neg]
    type = PointValue
    variable = disp_y
    point = '-0.235 -0.24 0.08'
  [../]
  [disp_y_p8_pos]
    type = PointValue
    variable = disp_y
    point = '0.235 0.24 0.08'
  [../]
  [disp_y_p8_neg]
    type = PointValue
    variable = disp_y
    point = '0.235 -0.24 0.08'
  [../]

  [disp_z_p1_pos]
    type = PointValue
    variable = disp_z
    point = '-0.08 -0.08 0.24'
  [../]
  [disp_z_p1_neg]
    type = PointValue
    variable = disp_z
    point = '-0.08 -0.08 -0.24'
  [../]
  [disp_z_p2_pos]
    type = PointValue
    variable = disp_z
    point = '-0.08 0.08 0.24'
  [../]
  [disp_z_p2_neg]
    type = PointValue
    variable = disp_z
    point = '-0.08 0.08 -0.24'
  [../]
  [disp_z_p3_pos]
    type = PointValue
    variable = disp_z
    point = '0.08 -0.08 0.24'
  [../]
  [disp_z_p3_neg]
    type = PointValue
    variable = disp_z
    point = '0.08 -0.08 -0.24'
  [../]
  [disp_z_p4_pos]
    type = PointValue
    variable = disp_z
    point = '0.08 0.08 0.24'
  [../]
  [disp_z_p4_neg]
    type = PointValue
    variable = disp_z
    point = '0.08 0.08 -0.24'
  [../]
  [disp_z_p5_pos]
    type = PointValue
    variable = disp_z
    point = '0.235 0.08 0.24'
  [../]
  [disp_z_p5_neg]
    type = PointValue
    variable = disp_z
    point = '0.235 0.08 -0.24'
  [../]
  [disp_z_p6_pos]
    type = PointValue
    variable = disp_z
    point = '-0.235 0.08 0.24'
  [../]
  [disp_z_p6_neg]
    type = PointValue
    variable = disp_z
    point = '-0.235 0.08 -0.24'
  [../]
[]

[Executioner]
  type       = Transient
  solve_type = 'PJFNK'
  line_search = none
  petsc_options_iname = '-pc_type -ksp_gmres_restart'
  petsc_options_value = 'lu       101'
  start_time = 2419200 #28 days
  dt = 86400 #1 day in  sec
  automatic_scaling = true
  end_time = 38880000 #450 days
  l_max_its  = 50
  l_tol      = 1e-4
  nl_max_its = 10
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-10
[]

[Outputs]
  exodus         = true
  perf_graph     = true
  csv = true
  [./Console]
    type = Console
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
