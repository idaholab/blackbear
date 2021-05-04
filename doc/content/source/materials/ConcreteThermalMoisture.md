# ConcreteThermalMoisture

!syntax description /Materials/ConcreteThermalMoisture

## Description

This class computes the set of material coefficients and parameters needed for moisture diffusion and heat transfer in concrete. The equivalent moisture diffusion/heat transfer model [!cite](bazant1979pore,bazant1982finite,xi_moisture_1994_a,xi_moisture_1994_b) is implemented using a set of kernels that provide the individual terms in the system of partial differential equations. This class provides with a full set of constitutive models compiled by [!cite](saouma_structural_2014). The following sections provide detailed descriptions of the governing equations and associated constitutive laws for the coupled moisture diffusion and heat transfer model.

## Heat transfer model

### Governing equation

The governing partial differential equation for heat transfer in concrete is given by [!cite](bazant1982finite) and [!cite](saouma_structural_2014) as:

!equation id=thermal_governing
\rho C\frac{\partial{T}}{\partial{t}} =  \nabla \cdot (k\nabla T) - C_w \mathbf{J} \nabla T + C_a\frac{\partial{W}}{\partial{H}}\frac{\partial{H}}{\partial{t}} + Q

where:

$\rho$  = density in kg/m$^3$ \\
$C$   =  specific heat of concrete in J/kg$\degree$C\\
$T$   =  temperature in $\degree$C\\
$k$   =  thermal conductivity of concrete in W/m$\degree$C \\
$C_w$   =  mass density and isobaric (constant pressure) heat capacity of liquid water\\
$\mathbf{J}$  =  moisture flux, $-D_h\mathbf{\nabla H}$\\
$W$   =  water (moisture) content in g/g (for unit volume  of material, m$^3$)\\
$H$   =  pore relative humidity\\
$C_a$   =  heat absorption of free water in $J/kg$\\
$\frac{\partial{W}}{\partial{H}}$   =  moisture capacity in g/gm$^3$\\
$Q$   =  rate of heat per unit volume generated within the body W/m$^3$\\
$t$   =  time in $s$

The term on the left side of [!eqref](thermal_governing) represents time-dependent effects, and is provided by [ConcreteThermalTimeIntegration](ConcreteThermalTimeIntegration.md). The first term on the right side of [!eqref](thermal_governing) represents the thermal conduction, and is provided by [ConcreteThermalConduction](ConcreteThermalConduction.md). The second term represents the convective transport of heat due to fluid flow, and is provided by [ConcreteThermalConvection](ConcreteThermalConvection.md). The third term represents adsorption heat due to adsorption of free water molecules in pores onto pore walls, and is provided by [ConcreteLatentHeat](ConcreteLatentHeat.md). The last term is a volumetric heating from other sources that can be provided by general-purpose kernels provided by MOOSE.

The mass density and isobaric (constant pressure) heat capacity of liquid water $C_w$ is given by

!equation id=Cw
C_w = \rho_{water}C_{water}

where $\rho_{water}$ is mass density of liquid water in $kg/m^3$ which is given as [!cite](raznjevic1970tables)

!equation id=rho_w
\rho_{water} = 314.4 + 685.6\left[1-\left(\frac{T-273.15}{374.14}\right)^\frac{1}{0.55}\right]^{0.55}

and $C_{water}$ is the isobaric (constant pressure) heat capacity of liquid water in J/kg$\degree$C. The values of $C_{water}$
are tabulated in [!cite](yunus2011heat). The adsorption heat $C_a$ usually can be neglected according to [!cite](bazant1982finite). Thus, a small fraction of the concrete specific heat capacity $C$ value is simply assigned to $C_a$ (i.e., $C_a=0.001 \times C$).

### Thermal capacity

Following [!cite](saouma_structural_2014), four constitutive models are available for concrete thermal capacity  $\rho C$ (in MJ/m$^3\degree$C ):

1. A user-supplied constant thermal capacity;
2. The [!cite](asce_1992) model for normal-strength concrete;
3. [!cite](kodur2004predicting) model for high-strength concrete and
4. The [!cite](Eurocode) model for both normal- and high-strength concrete.  

Details of these models are provided below:

1. Constant

  In this model, the user provides a value of $\rho C$ that remains constant during the simulation.

2. [!cite](asce_1992)

- Siliceous aggregate concrete

  !equation id=rho_C_sil_ASCE
  \rho C = \left\{\begin{array}{lll}
          0.005T + 1.7 & \text{for} & 20\degree \text{C} \le T \le 200 \degree\text{C} \\
          2.7 & \text{for} & 200\degree \text{C} \le T \le 400 \degree \text{C} \\
          0.013T - 2.5 & \text{for} & 400\degree \text{C} \le T \le 500 \degree \text{C} \\
          10.5 - 0.013T & \text{for} & 500\degree \text{C} \le T \le 600 \degree \text{C} \\
          2.7 & \text{for} & T > 600 \degree \text{C}
      \end{array}\right.

- Carbonate aggregate concrete

  !equation id=rho_C_carb_ASCE
  \rho C = \left\{\begin{array}{lll}
          2.566 & \text{for} & 20 \degree \text{C} \le T \le  400 \degree \text{C} \\
          0.1756T - 68.034 & \text{for} & 400 \degree \text{C} \le T \le  410 \degree \text{C} \\
          25.00671 - 0.05043T & \text{for} & 410 \degree \text{C} \le T \le  445 \degree \text{C} \\
          2.556 & \text{for} & 445 \degree \text{C} \le T \le  500 \degree \text{C} \\
          0.01603T - 5.44881 & \text{for} & 500 \degree \text{C} \le T \le  635 \degree \text{C} \\
          0.16635T - 100.90225 & \text{for} & 635 \degree \text{C} \le T \le 715 \degree \text{C} \\
          176.07343 - 0.22103T & \text{for} & 715 \degree \text{C} \le T \le  785 \degree \text{C} \\
          2.566 & \text{for} & T > 785 \degree \text{C}
      \end{array}\right.

  Note that $\rho C$ is in units of MJ/m$^3{\degree}$C in this particular thermal capacity model.

3. [!cite](kodur2004predicting)

- Siliceous aggregate concrete

!equation id=rho_C_sil_kodur
\rho C = \left\{\begin{array}{lll}
        0.005T + 1.7 & \text{for} & 20\degree \text{C} \le T \le 200 \degree \text{C} \\
        2.7 & \text{for} & 200\degree \text{C} \le T \le 400 \degree \text{C} \\
        0.013T - 2.5 & \text{for} & 400\degree \text{C} \le T \le 500 \degree \text{C} \\
        10.5 - 0.013T & \text{for} & 500° \text{C} \le T \le 600 \degree \text{C} \\
        2.7 & \text{for} & 600 \degree \text{C} \le T \le 635 \degree \text{C}
    \end{array}\right.

- Carbonate aggregate concrete

  !equation id=rho_C_carb_kodur
  \rho C = \left\{\begin{array}{lll}
          2.45 & \text{for} & 20 \degree \text{C} \le T \le  400 \degree \text{C} \\
          0.026T - 12.85 & \text{for} & 400 \degree \text{C} \le T \le  475 \degree \text{C} \\
          0.0143T - 6.295 & \text{for} & 475 \degree \text{C} \le T \le  650 \degree \text{C} \\
          0.1894T - 120.11 & \text{for} & 650 \degree \text{C} \le T \le  735 \degree \text{C} \\
          -0.263T + 212.4 & \text{for} & 735 \degree \text{C} \le T \le  800 \degree \text{C} \\
          2.0 & \text{for} & 800 \degree \text{C} \le T \le 1000 \degree \text{C}
      \end{array}\right.

  Note that $\rho C$ is in units of  MJ/m$^3{\degree}$C in this particular thermal capacity model.

4. [!cite](Eurocode)

!equation id=rho_euro
\rho = \left\{\begin{array}{lll}
      \rho_{ref} & \text{for} & 20\degree \text{C} \le T \le 115 \degree \text{C} \\
      \rho_{ref}\left(1 - \frac{0.02(T-115)}{85}\right) & \text{for} & 115\degree \text{C} \le T \le 200 \degree \text{C} \\
      \rho_{ref}\left(0.98 - \frac{0.03(T-200)}{200}\right) & \text{for} & 200\degree \text{C} \le T \le 400 \degree \text{C} \\
      \rho_{ref}\left(0.95 - \frac{0.07(T-400)}{800}\right) & \text{for} & 400\degree \text{C} \le T \le 1200 \degree \text{C} \\
      \end{array}\right.

where $\rho_{ref}$ is reference concrete density (in kg/m$^3$) at 20$\degree$C and the equation for specific heat of concrete (in J/kg$\degree$C) is given by

!equation id=C_euro
C = \left\{\begin{array}{lll}
        900 & \text{for} & 20 \degree \text{C} \le T \le  100 \degree \text{C} \\
        900 + (T-100) & \text{for} & 100 \degree \text{C} \le T \le  200 \degree \text{C} \\
        1000 + \left(\frac{T-200}{2}\right) & \text{for} & 200 \degree \text{C} \le T \le  400 \degree \text{C} \\
        1100 & \text{for} & 400 \degree \text{C} \le T \le 1200 \degree \text{C}
    \end{array}\right.



### Thermal conductivity

Also Following [!cite](saouma_structural_2014), four thermal conductivity models are available, all depending on the temperature and concrete texture,
including

1. A user-supplied constant thermal conductivity;
2. The [!cite](asce_1992) model for normal-strength concrete at high temperature;
3. [!cite](kodur2004predicting) model for high-strength concrete;
4. The [!cite](Eurocode) model for both normal- and high-strength concrete

Details of these models are provided below:

1. Constant

  In this model, the user provides a value of $k$ that remains constant during the simulation.

2. [!cite](asce_1992)

- Siliceous aggregate concrete

!equation id=k_sil_ASCE
k = \left\{\begin{array}{lll}
        -0.000625T + 1.5 & \text{for} & 20 \degree \text{C} \le T \le 800 \degree \text{C}\\
        1.0 & \text{for} & T>800 \degree \text{C}
    \end{array}\right.


- Carbonate aggregate concrete

!equation id=k_carb_ASCE
k = \left\{\begin{array}{lll}
        1.355 & \text{for} & 20 \degree \text{C} \le T \le 293 \degree \text{C}\\
        1.0 & \text{for} & T>293 \degree \text{C}
    \end{array}\right.

3. [!cite](kodur2004predicting)

- Siliceous aggregate concrete

!equation id=k_sil_kodur
k = \begin{array}{lll}
        0.85(2-0.0011T) & \text{for} & 20 \degree \text{C} \le T \le 1000 \degree \text{C}\\
    \end{array}

- Carbonate aggregate concrete

!equation id=k_carb_ASCE
k = \left\{\begin{array}{lll}
        0.85(2-0.0013T) & \text{for} & 20 \degree \text{C} \le T \le 300 \degree \text{C}\\
        0.85(2.21-0.002T) & \text{for} & T>300 \degree \text{C}
    \end{array}\right.

4. [!cite](Eurocode)

- Upper limit

!equation id=euro_ul
\begin{array}{lll}
k = 2-0.2451\left(\frac{T}{100}\right) + 0.0107\left(\frac{T}{100}\right)^2 & \text{for} & 20 \degree \text{C} \le T \le 1200 \degree \text{C}\\
\end{array}

- Lower limit

!equation id=euro_ll
\begin{array}{lll}
k = 1.36 - 0.136\left(\frac{T}{100}\right) + 0.0057\left(\frac{T}{100}\right)^2 & \text{for} & 20 \degree \text{C} \le T \le 1200 \degree \text{C}\\
\end{array}

These various heat transfer constitutive models can be conveniently chosen and specified from input file.


## Moisture diffusion

[!cite](saouma_structural_2014) compiled and provided a comprehensive set of constitutive models and parameters for moisture diffusion in concrete structures, which were also implemented here. Detailed descriptions of the governing equation and constitutive models for moisture diffusion are provided here.

### Governing equation

Following [!cite](saouma_structural_2014), the governing equation for moisture diffusion in concrete is formulated by using relative humidity $H$ as the primary variable:

!equation id=moisture_governing
\frac{\partial{W}}{\partial{H}} \frac{\partial{H}}{\partial{t}} = \nabla (D_h\nabla H) + \nabla (D_{ht}\nabla T) + \frac{\partial{W_d}}{\partial{t}}

where

$W$ = total water content (g/g) (for unit volume of concrete, cm$^3$) \\
$H$ = pore relative humidity, and $H=P_v$/$P_{vs}$ \\
$P_{vs}$ = saturate vapor pressure $= P_{atm}\exp\left(4871.3\frac{T-100}{373.15T}\right)$ [!cite](Bary2012) (where $T$ is the temperature in K))\\
$P_{atm}$ = standard atmospheric pressure $= 101.325 Pa$ \\
$D_h$ = moisture diffusivity (also referred as humidity diffusivity) (cm$^2$/day)\\
$D_{ht}$= coupled moisture diffusivity under the influence of a temperature gradient in cm$^2$/day \\
$W_d$= total mass of free evaporable water released into the pores\\
   by dehydration of the cement paste\\
$t$  = time $(day)$

The term on the left side of [!eqref](moisture_governing) represents time-dependent effects, and is provided by [ConcreteMoistureTimeIntegration](ConcreteMoistureTimeIntegration.md). The first term on the right side of [!eqref](moisture_governing) represents Fickian diffusion, and the second term represents Soret diffusion. These are both provided by [ConcreteMoistureDiffusion](ConcreteMoistureDiffusion.md). The third term on the right hand side of this equation represents a source due to dehydrated water, and is provided by [ConcreteMoistureDehydration](ConcreteMoistureDehydration.md).

Two important parameters, moisture capacity ${\partial W}/{\partial H}$ and moisture diffusivity $D_h$, both depend on the relative humidity, $H$. Thus the moisture diffusion governing equation is highly nonlinear. The following sections describes in detail the constitutive models for this two parameters.

### Moisture capacity

[!cite](xi_moisture_1994_a,xi_moisture_1994_b) developed a concrete moisture capacity model based on the Brunauer-Emmett-Teller (BET) adsorption isotherm theory, which was implemented here. The total water content $W$ in concrete at a constant temperature $T$ is referred as water adsorption isotherm, which was proposed by [!cite](xi_moisture_1994_a) as:

!equation id=adsorption_isotherm
W = \frac{CkV_mH}{(1-kH)[1+(C-1)kH]}

where

$C$ = $\exp\left(\frac{C_0}{T}\right), C_0 = 855$ \\
$H$ = relative humidity \\
$T$ = absolute temperature in $K$ \\
$W$ = quantity of vapor absorbed at pressure $p$ \\
      (g water/g cement) \\
$V_m$ = monolayer capacity: mass of adsorbate required to cover \\
       the adsorbent with a single molecular layer \\
$k$ = empirical constant

The monolayer capacity, $V_m$, is defined as the mass of adsorbate required to cover the surface of the adsorbent with a single molecular layer. To
evaluate $W$ at a given relative humidity value, $V_m$ and the empirical constant $k$ in the above equation need to be evaluated first. This is done separately
for cement and aggregate materials as follows:

- **Monolayer capacity, $V_m$**

  - **Cement Paste:**

    !equation id=Vm
    V_m = V_t(t)V_{wc}(w/c)V_{ct}(ct)V_T(T)

    where $t$ is the age of concrete material in $days$; $V_{ct}(ct)$ represents the effect of cement types on the adsorption isotherm and is given by [V_{ct}];
    $V_T(T) = 1$ at room temperature and remains constant during simulations, and

    !equation id=Vt_Vwc
    V_t(t) = \left\{\begin{array}{llll}
            0.068 - \frac{0.22}{t} & \text{for} & t > 5 & days \\
            V_t(5) & if & t \le 5 & days
             \end{array}\right.\\
    V_{wc}(w/c) = \left\{\begin{array}{llll}
            0.85 + 0.45\frac{w}{c} & \text{for} & 0.3<w/c < 0.7 &\\
            V_{wc}(0.3) & if & w/c \le 0.3 &
             \end{array}\right.

    $V_t(t)$ represents the effects of concrete age and $V_{wc}(w/c)$ represents the effect of water to cement ratio $w/c$ on the adsorption isotherm, respectively.

    !table id=V_{ct} caption=$V_{ct}$ for different types of concrete
    | Concrete Type | 1 | 2 | 3 | 4 |
    |---------------|---|---|---|---|
    | $V_{ct}$      |  0.9 | 1.0 | 0.85 | 0.6 |

  - **Aggregates:**

    The monolayer capacity $V_m$ of aggregates is determined by

    !equation id=Vm
    V_m = 0.00647V_{ag}

    where $V_{ag}$ depends on the pore structure of various aggregates as listed in [V_{ag}].

    !table id=V_{ag} caption=$V_{ag}$ of various pore structure of aggregate
    |Pore structure of aggregate | $V_{ag}$ |
    |---------------|---|---|---|---|
    | dense        |  0.05-0.1 |
    | porous       |  0.1-0.04 |

- **Empirical constant $k$**

  The empirical constant $k$ in Equation [!eqref](adsorption_isotherm) is related to the the number of layers of adsorbed water molecule, $n$, under saturated state. $n$ is
  determined separately for cement and aggregate materials.

  - **Cement Paste:**

    $n$ is expressed in terms similar to those of $V_m$:

    !equation id=n_def
    n = N_t(t)N_{wc}(w/c)N_{ct}(ct)N_T(T) \\
    N_t(t) = \left\{\begin{array}{llll}
            2.5 + \frac{1.5}{t_e} & \text{for} & t > 5 & days \\
            5.5 & if & t \le 5 & days
             \end{array}\right.\\
    N_{wc}(w/c) = \left\{\begin{array}{llll}
            0.33 + 2.2\frac{w}{c} & \text{for} & 0.3 < w/c < 0.7 & days \\
            N_{wc}(0.3) & if & w/c \le 0.3 &\\
            N_{wc}(0.7) & if & w/c \ge 0.7 &
             \end{array}\right.

    where $N_{ct}(ct)$ is given by [$N_{ct}$] and $N_T(T) = 1$ at room temperature and remains constant during the simulation.

    !table id=$N_{ct}$ caption=$N_{ct}$ for different types of concrete
    | Concrete Type | 1 | 2 | 3 | 4 |
    |---------------|---|---|---|---|
    | $N_{ct}$      |  1.1 | 1.0 | 1.15 | 1.5 |

  - **Aggregates:**

    For the aggregate, $n$ is expressed as:

    !equation id=n in Nag
    n = 4.603N_{ag} \\

    where $N_{ag}$ is defined in [N_{ag}].

    !table id=N_{ag} caption=$N_{ag}$ of various pore structure of aggregate
    |Pore structure of aggregate | $N_{ag}$ |
    |---------------|---|---|---|---|
    | dense        |  1.0-1.5 |
    | porous       |  1.7-2.0 |

  Once the number of adsorbed layers of molecule, $n$, is obtained, $k$ can be obtained by

  !equation id=k in C
  k = \frac{\left(1-\frac{1}{n}\right)C - 1}{C - 1}

Finally, once the monolayer capacity $V_m$ and empirical constant $k$ are obtained, then using [!eqref](adsorption_isotherm), the water content $W$ in cement and aggregate materials
can be obtained. The moisture capacities for cement paste or aggregate material can also be determined by taking derivatives
of both sides of [!eqref](adsorption_isotherm) with respect to relative humidity $H$ as

!equation id=moisture_capacity
\frac{\partial W}{\partial H}\mid_{cp; agg} = \frac{CkV_m + WK[1+(C-1)kH] - Wk(1-kH)(C-1)}{(1-kH)[1+(C-1)kH]}

The total moisture capacity of the concrete structure $\frac{\partial W}{\partial H}$ required by the moisture diffusion governing equation [!eqref](moisture_governing) is then simply
the weight-average value between cement and aggregate materials as:

!equation id=moisture_capacity
\frac{\partial W}{\partial H} = f_{agg}\left(\frac{\partial W}{\partial H}\right)_{agg}
    f_{cp}\left(\frac{\partial W}{\partial H}\right)_{cp}

where

$f_{agg}$ = weight percentage of the aggregate \\
$f_{cp}$ = weight percentage of the cement paste \\
$\left(\frac{\partial{W}}{\partial{H}}\right)_{agg}$ = moisture capacity of aggregate (g/g) \\
& & (for the unit volume of material, cm$^3$) \\
$\left(\frac{\partial{W}}{\partial{H}}\right)_{cp}$ = moisture capacity of cement paste (g/g) (for the unit volume of material, cm$^3$)

The total moisture capacity ${\partial W}/{\partial H}$ (with the units of g water/g material) is a function of water content $W$, temperature $T$
and relative humidity $H$, and strongly depends on the concrete texture.

### Moisture diffusivity

The moisture diffusivity of concrete $D_h$ is a complex function of temperature $T$, relative humidity $H$ and pore structure of concrete. Various diffusion mechanisms often
interact, such as molecular diffusion in large pores (usually 50nm - 10 microns and beyond) and microcracks, Knudson diffusion in mesopores (2.5nm - 50 nm) and
 micropores ($<$2.5nm) and surface diffusion along pore walls [!cite](saouma_structural_2014). Most existing moisture diffusivity models typically do not account for individual diffusion
mechanisms separately. Instead, they tend to reproduce the general combined trend.

Calculation of $D_h$ starts with the calculation of a reference moisture diffusivity $D_{h,0}$ at a given temperature $T$ and relative humidity $H$. Three reference moisture diffusivity $D_{h,0}$ models are implemented as:

1. [!cite](mensi1988sechage) (in units of $m^2/s$)

!equation id=Dh0_mensi
D_{h,0} = Ae^{BC}

where

$D_{h,0}$ = humidity diffusion coefficient of concrete (cm$^2$/day) \\
$A = 3.8 \times 10^{-13}$ \\
$B = 0.05$ \\
$C$ = free water content in L/m$^3$

$C$ is a function of relative humidity $H$ in concrete as given by

!equation id=C_mensi
C = HC_0

where $C_0$ is constant takes a value of 130 (in L/m$^3$).

2. [!cite](bazant1982finite) (in units of $m^2/s$)

!equation id=Dh0_bazant
D_{h,0} = D_1f_H

where $D_1 = 3.10 \times 10^{-10}m^2/s$ and

!equation id=fH
f_H = \left\{\begin{array}{lll}
        \alpha_D + \frac{1-\alpha_D}{1+\left(\frac{1-H}{1-0.75}\right)^n} & \text{for} & T \le 95\degree C\\
        1 & \text{for} & T > 95 \degree C
    \end{array}\right.

Also, $f_H = 1$ when $H > 1$ and $\alpha_D$ is given by

!equation id=D_bazant
\alpha_D = \frac{1}{1+\frac{19(95-T)}{70}}

where $T$ is in $\degree$C. $\alpha \in [0.037:1]$ from $T\in[0\degree C : 95\degree C]$ and $n\in[6:16]$.

It's obvious that all three reference moisture diffusivity models strongly depend on the value of humidity $H$, and indirectly on the temperature $T$. Once the value of
reference moisture diffusivity $D_{h,0}$ is obtained, the actual concrete moisture diffusivity $D_h$ required by the moisture diffusion governing equation, [!eqref](moisture_governing),
can then be calculated by

!equation id=Dh_coupled
D_h = \left\{\begin{array}{lll}
        D_{h, 0}f_1(T) & \text{for} & T \le 95 \degree C \\
        D_{h, 0}f_1(95\degree C)f_2(T) & \text{for} & T > 95 \degree C
    \end{array}\right.

where

!equation id=f1T
\begin{array}{lll}
    f_1(T) = \exp\left(\frac{Q}{R}\left(\frac{1}{T_{ref}}-\frac{1}{T}\right)\right) & \text{for} & T \le 95° C
\end{array}

in which $T$ is the absolute temperature ($K$), $Q$ is activation energy for water migration along the adsorption layers in the necks, and $R$ is gas constant
with $Q/R$=2700 K, and

!equation id=f2T
\begin{array}{lll}
    f_2(T) = \exp\left(\frac{T-95}{0.881 + 0.214(T-95)}\right) & \text{for} & T > 95 \degree C
\end{array}

### Coupled moisture diffusion by thermal gradient $D_{ht}$

It has been reported by Bazant et al. [!cite](bazant1982finite) that the additional moisture diffusion due to thermal gradients included in the moisture governing equation is negligible. Thus the value of $D_{ht}$ is set to 0.0 by default. This parameter can, however, be set to an arbitrary value if desired.

!syntax parameters /Materials/ConcreteThermalMoisture

!syntax inputs /Materials/ConcreteThermalMoisture

!syntax children /Materials/ConcreteThermalMoisture

!bibtex bibliography
